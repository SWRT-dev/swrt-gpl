
/***********************************************************************************
File:				PhyRxTdIfRiscPage0TdRegs.h
Module:				phyRxTdIfRiscPage0Td
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_TD_IF_RISC_PAGE_0_TD_REGS_H_
#define _PHY_RX_TD_IF_RISC_PAGE_0_TD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS                             MEMORY_MAP_UNIT_50_BASE_ADDRESS
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF00                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10000)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF01                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10004)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF02                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10008)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF03                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1000C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF04                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10010)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF05                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10014)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF06                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10018)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF07                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1001C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF08                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10020)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF09                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10024)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1B                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1006C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1C                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10070)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1D                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10074)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1E                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10078)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1007C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CLEAR_ERROR_INT          (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10080)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF21                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10084)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF22                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10088)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF23                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1008C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF24                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10090)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF25                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10094)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF26                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10098)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF27                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1009C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF28                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100A0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF29                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100A4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2A                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100A8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2B                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100AC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2C                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100B0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2D                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100B4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2E                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100B8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100BC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF30                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100C0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF31                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100C4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF32                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100C8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF33                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100CC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF34                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100D0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3B                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3C                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100F0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3D                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100F4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3E                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x100F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_AFE_DYN_CTRL                         (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10104)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_AFE_PD_CTRL                          (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10114)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_AFE_AUX_CTRL                         (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1011C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4A                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10128)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4C                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10130)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4D                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10134)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CCA_INT                  (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10138)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1013C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF50                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10140)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF51                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10144)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF52                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10148)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF53                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1014C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF54                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10150)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF55                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10154)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF56                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10158)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF5F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1017C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF67                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1019C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6A                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101A8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6B                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101AC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6C                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101B0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6D                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101B4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6E                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101B8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101BC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF70                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101C0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF71                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101C4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF72                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101C8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF73                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101CC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF74                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101D0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF75                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101D4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF76                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101D8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF77                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101DC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF78                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101E0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF79                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101E4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7A                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101E8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CLEAR_TIMERS_INT         (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7C                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101F0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7D                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101F4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7E                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x101FC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF80                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10200)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF81                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10204)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_SANITY_DELTA_F_VALID     (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10208)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF84                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10210)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF85                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10214)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF86                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10218)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF87                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1021C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF88                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10220)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_FREE_RUNNING_CNT_HIGH    (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10224)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF8A                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10228)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF8B                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1022C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF8F                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1023C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF90                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10240)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF91                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10244)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_SW_RESET_GENERATE        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10248)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF93                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1024C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF94                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10250)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF95                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10254)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF96                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10258)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF97                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1025C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF98                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10260)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF99                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10264)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF9A                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10268)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF9B                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1026C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFB5                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x102D4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFB6                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x102D8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFB7                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x102DC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TIMER0                   (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10320)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFC9                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10324)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TIMER1                   (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10328)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFCB                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1032C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_LOW_ONLY16     (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10330)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_MID_ONLY16     (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10334)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_HIGH_ONLY16    (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10338)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_FOUR_ONLY16    (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1033C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD0                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10340)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD1                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10344)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD2                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10348)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD9                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10364)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFDA                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10368)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFDD                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10374)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFDE                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10378)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFF0                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103C0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFF1                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103C4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFF2                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103C8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFB                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFC                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103F0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFD                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103F4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFE                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFF                        (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x103FC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF100                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10400)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF101                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10404)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF102                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10408)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF103                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1040C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CSM_CLEAR_ERROR_INT      (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10410)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF10D                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10434)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF10E                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10438)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF120                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10480)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF121                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10484)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF122                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10488)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF123                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1048C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF124                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10490)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF125                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10494)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF126                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10498)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF127                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1049C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF128                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104A0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_FCSI_ACCESS              (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104A4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12A                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104A8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12B                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104AC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12C                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104B0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12D                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104B4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12E                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104B8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12F                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104BC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF130                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104C0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF131                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104C4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF132                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104C8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF133                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104CC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF134                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104D0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF135                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104D4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF136                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104D8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF137                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104DC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF138                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104E0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF139                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104E4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CO_CH_COUNTER            (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104E8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF13B                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF13E                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF13F                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x104FC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF140                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10500)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF141                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10504)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF142                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10508)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF143                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1050C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_LOW            (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10510)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF145                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10514)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_MID            (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10518)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF147                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1051C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_HIGH           (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10520)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF149                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10524)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_FOUR           (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10528)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14B                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1052C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14C                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10530)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14D                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10534)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14E                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10538)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14F                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1053C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF150                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10540)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF151                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10544)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF152                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10548)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF153                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1054C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF154                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10550)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF155                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10554)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF156                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x10558)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF157                       (PHY_RX_TD_IF_RISC_PAGE_0_TD_BASE_ADDRESS + 0x1055C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF00 0x10000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpiLow_riscGpoLow:16;	//  IF Semaphore reg00
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf00_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF01 0x10004 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpiHigh_riscGpoHigh:16;	//  IF Semaphore reg01
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf01_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF02 0x10008 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpioOenLow:16;	//  IF Semaphore reg02
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf02_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF03 0x1000C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpioOenHigh:16;	//  IF Semaphore reg03
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf03_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF04 0x10010 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg04:16;	// GenRisc operational mode
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf04_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF05 0x10014 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg05:16;	//  IF Semaphore reg05
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf05_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF06 0x10018 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg06:16;	//  IF Semaphore reg06
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf06_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF07 0x1001C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg07:16;	//  IF Semaphore reg07
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf07_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF08 0x10020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg08:16;	//  IF Semaphore reg08
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf08_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF09 0x10024 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg09:16;	//  IF Semaphore reg09
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf09_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1B 0x1006C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdEnableOverride:1;	// td enable override
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf1B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1C 0x10070 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscMaskInt:11;	// Gen risc Mask Interrupt
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf1C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1D 0x10074 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscClearInt0:1;	// Gen risc Clear Int
		uint32 genRiscClearInt1:1;	// Gen risc Clear Int
		uint32 genRiscClearInt2:1;	// Gen risc Clear Int
		uint32 genRiscClearInt3:1;	// Gen risc Clear Int
		uint32 genRiscClearInt4:1;	// Gen risc Clear Int
		uint32 genRiscClearInt5:1;	// Gen risc Clear Int
		uint32 genRiscClearInt6:1;	// Gen risc Clear Int
		uint32 genRiscClearInt7:1;	// Gen risc Clear Int
		uint32 genRiscClearInt8:1;	// Gen risc Clear Int
		uint32 genRiscClearInt9:1;	// Gen risc Clear Int
		uint32 genRiscClearInt10:1;	// Gen risc Clear Int
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf1D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1E 0x10078 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscNoiseCauseInt0:1;	// Gen risc Cause Int
		uint32 genRiscTxEnStartCauseInt1:1;	// Gen risc Cause Int
		uint32 genRiscPreTxCauseInt2:1;	// Gen risc Cause Int
		uint32 genRiscRxLatchFallingRxRdyCauseInt3:1;	// Gen risc Cause Int
		uint32 genRiscErrorCauseInt4:1;	// Gen risc Cause Int
		uint32 genRiscFdLastSymbolCauseInt5:1;	// Gen risc Cause Int
		uint32 genRiscHostCauseInt6:1;	// Gen risc Cause Int
		uint32 genRiscHostCauseInt7:1;	// Gen risc Cause Int
		uint32 genRiscHostCauseInt8:1;	// Gen risc Cause Int
		uint32 genRiscHostCauseInt9:1;	// Gen risc Cause Int
		uint32 genRiscHostCauseInt10:1;	// Gen risc Cause Int
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf1E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF1F 0x1007C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableErrorInt:16;	// Gen Risc Enable Error Int
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf1F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CLEAR_ERROR_INT 0x10080 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearErrorInt:16;	// Gen Risc Clear Enable Error Int
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfClearErrorInt_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF21 0x10084 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyFallDelayIf:13;	// rx_rdy_fall_delay_if
		uint32 reserved0:2;
		uint32 rxRdyFallDelayIfEn:1;	// rx_rdy_fall_delay_if_en
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf21_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF22 0x10088 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntLowRt:16;	// Free Running Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf22_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF23 0x1008C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ofdmEnableBit:1;	// ofdm enable bit for csm
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf23_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF24 0x10090 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmAntennaEnable:4;	// csm_antenna_enable
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf24_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF25 0x10094 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 antennaDeletion:4;	// antenna_deletion
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf25_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF26 0x10098 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 symbolPramReadyStm:2;	// symbol_pram_ready_stm
		uint32 reserved0:2;
		uint32 deltaFStm:2;	// delta_f_stm
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf26_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF27 0x1009C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 antenna0ResetEnable:1;	// antenna0_reset_enable
		uint32 antenna1ResetEnable:1;	// antenna1_reset_enable
		uint32 antenna2ResetEnable:1;	// antenna2_reset_enable
		uint32 antenna3ResetEnable:1;	// antenna3_reset_enable
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf27_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF28 0x100A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopSignalThIf:13;	// stop_signal_th_if
		uint32 reserved0:2;
		uint32 stopSignalThIfEn:1;	// stop_signal_th_if_en
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf28_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF29 0x100A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscInt6:1;	// Rx TD Risc int 6
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf29_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2A 0x100A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:12;
		uint32 tdCsmState:3;	// td_csm_state
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf2A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2B 0x100AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscAirtimeUs:16;	// gen_risc_airtime_us
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf2B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2C 0x100B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRisc4UsBoundaryOffset:12;	// gen_risc_4us_boundary_offset
		uint32 ccaCntSrcCtrl:4;	// cca_cnt_src_ctrl (0 - gen risc, 1- hw)
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf2C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2D 0x100B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htShortInd:1;	// ht_short_ind
		uint32 ccaCs:1;	// cca_cs
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf2D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2E 0x100B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetType:1;	// packet_type
		uint32 reserved0:2;
		uint32 isHtSw:1;	// is_ht
		uint32 reserved1:3;
		uint32 isHtSel:1;	// select HW ht of sw ht
		uint32 genriscRdyEnable:1;	// select HW ht of sw ht
		uint32 reserved2:23;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf2E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF2F 0x100BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRemovalShifter:4;	// DC removal shifter
		uint32 dcRemovalShifterGearShift:4;	// DC removal shifter for gear shift
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf2F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF30 0x100C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscAirtimeUsMax:1;	// gen_risc_airtime_us_max
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf30_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF31 0x100C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscAirtimeUsOverride:1;	// gen_risc_airtime_us_max
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf31_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF32 0x100C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaBusyGenRisc:1;	// cca_busy_gen_risc
		uint32 ccaModeCtrl:1;	// cca_mode_ctrl
		uint32 reserved0:2;
		uint32 ccaBusyGenRiscSec:1;	// cca_busy_gen_risc_sec
		uint32 ccaModeCtrlSec:1;	// cca_mode_ctrl_sec
		uint32 reserved1:2;
		uint32 ccaBusyGenRiscSec40:1;	// cca_busy_gen_risc_sec_40
		uint32 ccaModeCtrlSec40:1;	// cca_mode_ctrl_sec_40
		uint32 reserved2:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf32_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF33 0x100CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cca20P:1;	// cca20p
		uint32 cca20S:1;	// cca20s
		uint32 cca40S:1;	// cca40s
		uint32 reserved0:2;
		uint32 cca20SPifs:1;	// cca20s_pifs
		uint32 cca40SPifs:1;	// cca40s_pifs
		uint32 reserved1:25;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf33_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF34 0x100D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errorCauseReg:16;	// Rx TD Risc Error Cause Reg
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf34_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3B 0x100EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decimator0RoundHb:3;	// Rx TD Risc Decimator antenna 0 Round for HB , 0- round bit 4 , 1- round bit 5 , .. , 7-round bit 11
		uint32 reserved0:1;
		uint32 decimator1RoundHb:3;	// Rx TD Risc Decimator antenna 1 Round for HB , 0- round bit 4 , 1- round bit 5 , .. , 7-round bit 11
		uint32 reserved1:1;
		uint32 decimator2RoundHb:3;	// Rx TD Risc Decimator antenna 2 Round for HB , 0- round bit 4 , 1- round bit 5 , .. , 7-round bit 11
		uint32 reserved2:1;
		uint32 decimator3RoundHb:3;	// Rx TD Risc Decimator antenna 3 Round for HB , 0- round bit 4 , 1- round bit 5 , .. , 7-round bit 11
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf3B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3C 0x100F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decimator0Round:3;	// Rx TD Risc Decimator antenna 0 Round
		uint32 decimator0MulBy3:1;	// Rx TD Risc Decimator antenna 0 mul by3 
		uint32 decimator1Round:3;	// Rx TD Risc Decimator antenna 1 Round
		uint32 decimator1MulBy3:1;	// Rx TD Risc Decimator antenna 1 mul by3 
		uint32 decimator2Round:3;	// Rx TD Risc Decimator antenna 2 Round
		uint32 decimator2MulBy3:1;	// Rx TD Risc Decimator antenna 2 mul by3 
		uint32 decimator3Round:3;	// Rx TD Risc Decimator antenna 3 Round
		uint32 decimator3MulBy3:1;	// Rx TD Risc Decimator antenna 3 mul by3 
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf3C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3D 0x100F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscTestBusEn:4;	// gen_risc_test_bus_en
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf3D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF3E 0x100F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bs0RoundInit:3;	// bs0_round_init
		uint32 bs0MulBy3:1;	// bs0_mul_by_3
		uint32 bs1RoundInit:3;	// bs1_round_init
		uint32 bs1MulBy3:1;	// bs1_mul_by_3
		uint32 bs2RoundInit:3;	// bs2_round_init
		uint32 bs2MulBy3:1;	// bs2_mul_by_3
		uint32 bs3RoundInit:3;	// bs3_round_init
		uint32 bs3MulBy3:1;	// bs3_mul_by_3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf3E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_AFE_DYN_CTRL 0x10104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx0Stby:1;	// Rx0 Standby 
		uint32 rx1Stby:1;	// Rx1 Standby
		uint32 rx2Stby:1;	// Rx2 Standby 
		uint32 rx3Stby:1;	// Rx2 Standby 
		uint32 tx0Stby:1;	// Tx0 Standby
		uint32 tx1Stby:1;	// Tx1 Standby 
		uint32 tx2Stby:1;	// Tx2 Standby
		uint32 tx3Stby:1;	// Tx3 Standby
		uint32 reserved0:4;
		uint32 aux0AdcStby:1;	// Aux0 Standby
		uint32 aux1AdcStby:1;	// Aux1 Standby
		uint32 aux2AdcStby:1;	// Aux2 Standby
		uint32 aux3AdcStby:1;	// Aux3 Standby
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdAfeDynCtrl_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_AFE_PD_CTRL 0x10114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx0Pd:1;	// Rx-0 Power Down
		uint32 rx1Pd:1;	// Rx-1 Power Down
		uint32 rx2Pd:1;	// Rx-2 Power Down
		uint32 rx3Pd:1;	// Rx-3 Power Down
		uint32 tx0Pd:1;	// Tx-0 Power Down
		uint32 tx1Pd:1;	// Tx-1 Power Down
		uint32 tx2Pd:1;	// Tx-2 Power Down
		uint32 tx3Pd:1;	// Tx-3 Power Down
		uint32 reserved0:4;
		uint32 auxAdc0Pd:1;	// Auxiliary ADC 0 Power Down 
		uint32 auxAdc1Pd:1;	// Auxiliary ADC 1 Power Down Mode PD-Active High
		uint32 auxAdc2Pd:1;	// Auxiliary ADC 2 Power Down Mode PD-Active High
		uint32 auxAdc3Pd:1;	// Auxiliary ADC 3 Power Down Mode PD-Active High
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdAfePdCtrl_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_AFE_AUX_CTRL 0x1011C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aux0ChEn:1;	// Aux ADC channel enables
		uint32 aux1ChEn:1;	// Aux ADC channel enables
		uint32 aux2ChEn:1;	// Aux ADC channel enables
		uint32 aux3ChEn:1;	// Aux ADC channel enables
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdAfeAuxCtrl_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4A 0x10128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testFifoFull:1;	// test_fifo_full
		uint32 reserved0:3;
		uint32 tbFifoError:5;	// tb fifo error
		uint32 reserved1:23;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf4A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4C 0x10130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorBypassEn:1;	// detector_bypass_en
		uint32 detectorBypassResetRxtdEn:1;	// detector_bypass_reset_rxtd_en
		uint32 reserved0:2;
		uint32 detectorBypassCntValue:12;	// detector_bypass_cnt_value
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf4C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4D 0x10134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableCcaInt:2;	// enable_cca_int
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf4D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CCA_INT 0x10138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearCcaInt:2;	// clear_cca_int
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfCcaInt_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF4F 0x1013C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaCauseReg:2;	// cca_cause_reg
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf4F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF50 0x10140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstRiscAn0:7;	// Noise_est_risc_an0
		uint32 reserved0:1;
		uint32 noiseEstRiscAn1:7;	// Noise_est_risc_an1
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf50_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF51 0x10144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstRiscAn2:7;	// Noise_est_risc_an2
		uint32 reserved0:1;
		uint32 noiseEstRiscAn3:7;	// Noise_est_risc_an3
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf51_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF52 0x10148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi0Data:7;	// Rx TD Noise Estimation gi0 Data
		uint32 noiseEstGi0DataValid:1;	// Rx TD Noise Estimation gi0 Data Valid
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf52_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF53 0x1014C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi1Data:7;	// Rx TD Noise Estimation gi1 Data
		uint32 noiseEstGi1DataValid:1;	// Rx TD Noise Estimation gi1 Data Valid
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf53_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF54 0x10150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi2Data:7;	// Rx TD Noise Estimation gi2 Data
		uint32 noiseEstGi2DataValid:1;	// Rx TD Noise Estimation gi2 Data Valid
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf54_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF55 0x10154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi3Data:7;	// Rx TD Noise Estimation gi3 Data
		uint32 noiseEstGi3DataValid:1;	// Rx TD Noise Estimation gi3 Data Valid
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf55_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF56 0x10158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstRiscAllValid:1;	// Noise_est_risc_all_valid
		uint32 noiseEstRiscSelect:1;	// Noise_est_risc_select
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf56_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF5F 0x1017C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detShortenedMa:7;	// Rx TD  Shortened moving average value
		uint32 reserved0:1;
		uint32 waitCycles:8;	// Rx TD  wait cycles 
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf5F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF67 0x1019C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wdCounterEn:1;	// Watch dog counter enble
		uint32 reserved0:3;
		uint32 clearWdCounter:1;	// Clear Watch dog counter enble
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf67_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6A 0x101A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 detNoiseEstValidEn:1;	// det_noise_est_valid_en
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf6A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6B 0x101AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetRawDetection:4;	// Sanity Detector Raw Detection
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf6B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6C 0x101B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deltaFDataReg:15;	// Sanity Delta F Data
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf6C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6D 0x101B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetCoarseRdyAn0:1;	// Sanity Detector Coarse Ready An0
		uint32 sanityDetCoarseRdyAn1:1;	// Sanity Detector Coarse Ready An1
		uint32 sanityDetCoarseRdyAn2:1;	// Sanity Detector Coarse Ready An2
		uint32 sanityDetCoarseRdyAn3:1;	// Sanity Detector Coarse Ready An3
		uint32 sanityDetCommonCoarseErr:1;	// Sanity_det_common_coarse_err
		uint32 sanityDetCommonFineErr:1;	// Sanity_det_common_fine_err
		uint32 reserved0:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf6D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6E 0x101B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityHostAntennaEn:4;	// sanity Host Antenna Enable
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf6E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF6F 0x101BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn0CoarseCnt:16;	// Sanity An 0 Coarse Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf6F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF70 0x101C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn1CoarseCnt:16;	// Sanity An 1 Coarse Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf70_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF71 0x101C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn2CoarseCnt:16;	// Sanity An 2 Coarse Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf71_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF72 0x101C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn3CoarseCnt:16;	// Sanity An 3 Coarse Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf72_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF73 0x101CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn0FineCnt:16;	// Sanity An 0 Fine Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf73_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF74 0x101D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn1FineCnt:16;	// Sanity An 1 Fine Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf74_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF75 0x101D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn2FineCnt:16;	// Sanity An 2 Fine Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf75_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF76 0x101D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn3FineCnt:16;	// Sanity An 3 Fine Counter
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf76_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF77 0x101DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityClkPerStrobe:5;	// Sanity Clock Per Strobe
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf77_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF78 0x101E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cntReset:1;	// Sanity reset the Sanity Counters
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf78_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF79 0x101E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonCoarseRdyCnt:16;	// Sanity Detector Coarse Max Abs Sc An0
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf79_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7A 0x101E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableTimersInt:2;	// enable_timers_int
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf7A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CLEAR_TIMERS_INT 0x101EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearTimersInt:2;	// clear_timers_int
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfClearTimersInt_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7C 0x101F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaTimersReg:2;	// cca_timers_reg
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf7C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7D 0x101F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonFineRdyCnt:16;	// common_fine_rdy_cnt
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf7D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7E 0x101F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxtdCsmNcoFreqData:15;	// Sanity Delta f value at the end of the packet
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf7E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF7F 0x101FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetFineRdyAn0:1;	// Sanity Detector Fine Ready An0
		uint32 sanityDetFineRdyAn1:1;	// Sanity Detector Fine Ready An1
		uint32 sanityDetFineRdyAn2:1;	// Sanity Detector Fine Ready An2
		uint32 sanityDetFineRdyAn3:1;	// Sanity Detector Fine Ready An3
		uint32 sanityDetCommonFineRdy:1;	// sanity_det_common_fine_rdy
		uint32 sanityDetCommonCoarseRdy:1;	// sanity_det_common_coarse_rdy
		uint32 reserved0:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf7F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF80 0x10200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetDetectorBw:2;	// Sanity Detector Top Is Full CB
		uint32 reserved0:1;
		uint32 sanityDetImdDetect:1;	// Sanity Detector Top Signal Detect
		uint32 sanityDetSignalDetect:1;	// Sanity Detector Top Signal Detect
		uint32 reserved1:3;
		uint32 sanityDetDetectorMask:4;	// Sanity Detector Top Is Full CB
		uint32 reserved2:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf80_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF81 0x10204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tbBytesInFifo:15;	// bytes in test fifo0
		uint32 loggerTdNextSeqAlowed:1;	// logger_td_next_seq_alowed
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf81_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_SANITY_DELTA_F_VALID 0x10208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDeltaFValid:1;	// sanity Delta F Data Valid
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfSanityDeltaFValid_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF84 0x10210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIntMask:1;	// phy_mac_int_mask
		uint32 txPacketIntMask:1;	// tx_packet_int_mask
		uint32 wdIntMask:1;	// wd_int_mask
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf84_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF85 0x10214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearPhyMacInt:1;	// clear_phy_mac_int
		uint32 clearTxPacketInt:1;	// clear_tx_l_length_int
		uint32 clearWdInt:1;	// clear_wd_int
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf85_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF86 0x10218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIntCauseReg:1;	// phy_mac_int_cause_reg
		uint32 txPacketIntCauseReg:1;	// tx_packet_int_cause_reg
		uint32 wdIntCauseReg:1;	// wd_int_cause_reg
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf86_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF87 0x1021C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacInt:1;	// phy_mac_int
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf87_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF88 0x10220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntLow:16;	// free_running_cnt_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf88_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_FREE_RUNNING_CNT_HIGH 0x10224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntHigh:16;	// free_running_cnt_high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfFreeRunningCntHigh_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF8A 0x10228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntThLow:16;	// free_running_cnt_th_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf8A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF8B 0x1022C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntThHigh:16;	// free_running_cnt_th_high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf8B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF8F 0x1023C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg:16;	// Rx TD Sub blocks SW Reset
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf8F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF90 0x10240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockEn:16;	// Rx TD Sub blocks Enable
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf90_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF91 0x10244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask:16;	// Rx TD Gsm Sw Reset.
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf91_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_SW_RESET_GENERATE 0x10248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate:16;	// sw_reset_generate
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfSwResetGenerate_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF93 0x1024C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn0:12;	// grisc_dc_removal_data_i_an0
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf93_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF94 0x10250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn0:12;	// grisc_dc_removal_data_q_an0
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf94_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF95 0x10254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn1:12;	// grisc_dc_removal_data_i_an1
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf95_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF96 0x10258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn1:12;	// grisc_dc_removal_data_q_an1
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf96_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF97 0x1025C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn2:12;	// grisc_dc_removal_data_i_an2
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf97_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF98 0x10260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn2:12;	// grisc_dc_removal_data_q_an2
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf98_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF99 0x10264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn3:12;	// grisc_dc_removal_data_i_an3
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf99_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF9A 0x10268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn3:12;	// grisc_dc_removal_data_q_an3
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf9A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF9B 0x1026C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalValid:1;	// grisc_dc_removal_valid
		uint32 reserved0:3;
		uint32 griscDcShiftValueOverride:1;	// grisc_dc_shift_value_override
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf9B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFB5 0x102D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr20UsbSec:8;	// cca_pwr_20_usb_sec
		uint32 ccaPwr20LsbSec:8;	// cca_pwr_20_lsb_sec
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfb5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFB6 0x102D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr20Usb:8;	// cca_pwr_20_usb
		uint32 ccaPwr20Lsb:8;	// cca_pwr_20_lsb
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfb6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFB7 0x102DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr40:8;	// cca_pwr_40
		uint32 ccaPwr40Sec:8;	// cca_pwr_40_sec
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfb7_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TIMER0 0x10320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0Th_timer0:16;	// timer0
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTimer0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFC9 0x10324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0StbTh:4;	// timer0_stb_th
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfc9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TIMER1 0x10328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1Th_timer1:16;	// timer1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTimer1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFCB 0x1032C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1StbTh:4;	// timer1_stb_th
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfcb_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_LOW_ONLY16 0x10330 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLowOnly16:16;	// Rx TD Risc Test Bus 16 Low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugLowOnly16_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_MID_ONLY16 0x10334 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugMidOnly16:16;	// Rx TD Risc Test Bus 16 mid
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugMidOnly16_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_HIGH_ONLY16 0x10338 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugHighOnly16:16;	// Rx TD Risc Test Bus 16 high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugHighOnly16_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_FOUR_ONLY16 0x1033C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugFourOnly16:16;	// Rx TD Risc Test Bus 16 four
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugFourOnly16_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD0 0x10340 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwrMeterAn0:8;	// cca_pwr_meter_an0
		uint32 ccaPwrMeterAn1:8;	// cca_pwr_meter_an1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfd0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD1 0x10344 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwrMeterAn2:8;	// cca_pwr_meter_an2
		uint32 ccaPwrMeterAn3:8;	// cca_pwr_meter_an3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfd1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD2 0x10348 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr80:8;	// cca_pwr_80
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfd2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFD9 0x10364 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bOverrideRxAntEn:1;	// b_override_rx_ant_en
		uint32 reserved0:3;
		uint32 bOverrideRxAntSel:2;	// b_override_rx_ant_sel
		uint32 reserved1:2;
		uint32 bOverrideTxAntEn:1;	// b_override_tx_ant_en
		uint32 reserved2:3;
		uint32 bOverrideTxAntSel:2;	// b_override_tx_ant_sel
		uint32 reserved3:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfd9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFDA 0x10368 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bInterfaceRxAnt:2;	// b_interface_rx_ant
		uint32 reserved0:2;
		uint32 bInterfaceTxAnt:2;	// b_interface_tx_ant
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfda_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFDD 0x10374 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigRiscIn:16;	// spare_config_risc_in
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfdd_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFDE 0x10378 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigRiscOut:16;	// spare_config_risc_out
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfde_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFF0 0x103C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 accumulatorResetValueI:12;	// accumulator_reset_value_I
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIff0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFF1 0x103C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 accumulatorResetValueQ:12;	// accumulator_reset_value_Q
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIff1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFF2 0x103C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 resetValuesIq:1;	// reset_values_IQ
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIff2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFB 0x103EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGprLow:16;	// gpr low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIffb_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFC 0x103F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGprHigh:16;	// gpr high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIffc_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFD 0x103F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGpo:16;	// gpo high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIffd_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFE 0x103F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGpiSyncLow:16;	// hyp_rxtd_gpi_sync_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIffe_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IFFF 0x103FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGpiSyncHigh:16;	// hyp_rxtd_gpi_sync_high
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfff_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF100 0x10400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bs080MulBy3:1;	// bs0_80_mul_by_3
		uint32 bs180MulBy3:1;	// bs1_80_mul_by_3
		uint32 bs280MulBy3:1;	// bs2_80_mul_by_3
		uint32 bs380MulBy3:1;	// bs3_80_mul_by_3
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf100_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF101 0x10404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bs080RoundInit:3;	// bs0_80_round_init
		uint32 reserved0:1;
		uint32 bs180RoundInit:3;	// bs1_80_round_init
		uint32 reserved1:1;
		uint32 bs280RoundInit:3;	// bs2_80_round_init
		uint32 reserved2:1;
		uint32 bs380RoundInit:3;	// bs3_80_round_init
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf101_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF102 0x10408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmErrorCauseReg:5;	// Rx TD Risc Error Cause Reg
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf102_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF103 0x1040C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmEnableErrorInt:5;	// Gen Risc Enable Error Int
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf103_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CSM_CLEAR_ERROR_INT 0x10410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmClearErrorInt:5;	// Gen Risc Clear Enable Error Int
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfCsmClearErrorInt_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF10D 0x10434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortCalcGo:1;	// short_calc_go
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf10D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF10E 0x10438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityRiscAntennaEn:4;	// Sanity Risc Antenna Enable
		uint32 sanityRiscAntennaEnValid:1;	// Sanity Risc Antenna Enable Valid
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf10E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF120 0x10480 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearPifsMem:1;	// clear_pifs_mem
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf120_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF121 0x10484 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaClearPrim20Genrisc:1;	// cca_clear_prim20_genrisc
		uint32 ccaClearPrim20Enable:1;	// cca_clear_prim20_en
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf121_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF122 0x10488 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr20Max:8;	// cca_pwr_20_max
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf122_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF123 0x1048C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timingDiffTh:7;	// timing_diff_th 
		uint32 reserved0:1;
		uint32 useVerifiedDet:1;	// use_verified_det 
		uint32 reserved1:3;
		uint32 quickFirstDetection:1;	// quick_first_detection
		uint32 reserved2:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf123_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF124 0x10490 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decimatorOobTap:4;	// decimator_oob_tap
		uint32 decimatorOobTapHb:4;	// decimator_oob_tap_hb
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf124_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF125 0x10494 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decimatorRiscBankEn:1;	// decimator_risc_bank_en
		uint32 reserved0:3;
		uint32 decimatorRiscBank:2;	// decimator_risc_bank
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf125_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF126 0x10498 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiClkWrDivrExtRf:5;	// fcsi_clk_wr_divr_ext_rf
		uint32 reserved0:3;
		uint32 fcsiClkRdDivrExtRf:5;	// fcsi_clk_rd_divr_ext_rf
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf126_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF127 0x1049C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiClkWrDivrAfe:5;	// fcsi_clk_wr_divr_afe
		uint32 reserved0:3;
		uint32 fcsiClkRdDivrAfe:5;	// fcsi_clk_rd_divr_afe
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf127_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF128 0x104A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRfMode:1;	// internal_rf_mode
		uint32 externalRfMode:1;	// internal_rf_mode
		uint32 reserved0:2;
		uint32 fcsiModeMs:1;	// fcsi_mode_ms
		uint32 fcsiModeRfExt:1;	// fcsi_mode_rf_ext
		uint32 fcsiModeRfInt:1;	// fcsi_mode_rf_int
		uint32 reserved1:1;
		uint32 fcsiAdditionalCycles:4;	// fcsi_additional_cycles
		uint32 reserved2:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf128_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_FCSI_ACCESS 0x104A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrAddrA:7;	// fcsi_gr_addr_a
		uint32 reserved0:4;
		uint32 fcsiGrDataAToAllAddress:1;	// fcsi_gr_data_a_to_all_address
		uint32 fcsiGrAddrAToAllAddress:1;	// fcsi_gr_addr_a_to_all_address
		uint32 fcsiGrMsAccess:1;	// fcsi_gr_ms_access
		uint32 fcsiGrRfAccess:1;	// fcsi_gr_rf_access
		uint32 fcsiGrRdWr:1;	// fcsi_gr_rd_wr
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfFcsiAccess_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12A 0x104A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrAddrB:7;	// fcsi_gr_addr_b
		uint32 reserved0:1;
		uint32 fcsiGrAddrC:7;	// fcsi_gr_addr_c
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf12A_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12B 0x104AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrAddrD:7;	// fcsi_gr_addr_d
		uint32 reserved0:1;
		uint32 fcsiGrAddrE:7;	// fcsi_gr_addr_e
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf12B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12C 0x104B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrWrDataA:16;	// fcsi_gr_wr_data_a
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf12C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12D 0x104B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrWrDataB:16;	// fcsi_gr_wr_data_b
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf12D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12E 0x104B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrWrDataC:16;	// fcsi_gr_wr_data_c
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf12E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF12F 0x104BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrWrDataD:16;	// fcsi_gr_wr_data_d
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf12F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF130 0x104C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrWrDataE:16;	// fcsi_gr_wr_data_e
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf130_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF131 0x104C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataA:16;	// fcsi_rd_data_a
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf131_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF132 0x104C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataB:16;	// fcsi_rd_data_b
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf132_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF133 0x104CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataC:16;	// fcsi_rd_data_c
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf133_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF134 0x104D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataD:16;	// fcsi_rd_data_d
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf134_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF135 0x104D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdDataE:16;	// fcsi_rd_data_e
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf135_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF136 0x104D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiBusy:1;	// fcsi_busy
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf136_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF137 0x104DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiMsResetAntennasN:1;	// fcsi_ms_reset_antennas_n
		uint32 fcsiMsResetCentralN:1;	// fcsi_ms_reset_central_n
		uint32 reserved0:2;
		uint32 fcsiRfResetN:1;	// fcsi_rf_reset_n
		uint32 fcsiRfIntResetN:1;	// fcsi_rf_int_reset_n
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf137_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF138 0x104E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypFcsiStateMachine:3;	// hyp_fcsi_state_machine
		uint32 hypFcsiCounter:5;	// hyp_fcsi_counter
		uint32 hypFcsiCounterLimit:5;	// hyp_fcsi_counter_limit
		uint32 hypFcsiClockActive:1;	// hyp_fcsi_clock_active
		uint32 hypFcsiClockEnable:1;	// hyp_fcsi_clock_enable
		uint32 hypFcsiActive:1;	// hyp_fcsi_active
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf138_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF139 0x104E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiForceDefaultWlan2MsA:1;	// fcsi_force_default_wlan2ms_a
		uint32 fcsiForceDefaultWlan2MsB:1;	// fcsi_force_default_wlan2ms_b
		uint32 fcsiForceDefaultWlan2MsC:1;	// fcsi_force_default_wlan2ms_c
		uint32 fcsiForceDefaultWlan2MsD:1;	// fcsi_force_default_wlan2ms_d
		uint32 fcsiForceDefaultWlan2MsCommon:1;	// fcsi_force_default_wlan2ms_common
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf139_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_CO_CH_COUNTER 0x104E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 coChannelCounter:16;	// co_channel_detected
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfCoChCounter_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF13B 0x104EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lltfFalseDetection:1;	// lltf_false_detection
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf13B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF13E 0x104F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi0Data:11;	// rssi_0_data
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf13E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF13F 0x104FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi1Data:11;	// rssi_1_data
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf13F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF140 0x10500 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi2Data:11;	// rssi_2_data
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf140_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF141 0x10504 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi3Data:11;	// rssi_3_data
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf141_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF142 0x10508 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 olaStateAn0:3;	// ola_state_an0: , 000 - IDLE , 001 - FIRST WINDOW , 011 - REGULAR WINDOW , 100 - FIRST LAST WINDOW , 010 - LAST WINDOW , 101 - End OF WINDOWS
		uint32 reserved0:1;
		uint32 olaStateAn1:3;	// ola_state_an1: , 000 - IDLE , 001 - FIRST WINDOW , 011 - REGULAR WINDOW , 100 - FIRST LAST WINDOW , 010 - LAST WINDOW , 101 - End OF WINDOWS
		uint32 reserved1:1;
		uint32 olaStateAn2:3;	// ola_state_an2: , 000 - IDLE , 001 - FIRST WINDOW , 011 - REGULAR WINDOW , 100 - FIRST LAST WINDOW , 010 - LAST WINDOW , 101 - End OF WINDOWS
		uint32 reserved2:1;
		uint32 olaStateAn3:3;	// ola_state_an3: , 000 - IDLE , 001 - FIRST WINDOW , 011 - REGULAR WINDOW , 100 - FIRST LAST WINDOW , 010 - LAST WINDOW , 101 - End OF WINDOWS
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf142_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF143 0x1050C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16Low:16;	// Rx TD Risc Test Bus 16 Low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf143_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_LOW 0x10510 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLow:8;	// Rx TD Risc Test Bus low 
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugLow_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF145 0x10514 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16Mid:16;	// Rx TD Risc Test Bus 16 Mid
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf145_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_MID 0x10518 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugMid:8;	// Rx TD Risc Test Bus Mid
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugMid_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF147 0x1051C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16High:16;	// Rx TD Risc Test Bus 16 High
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf147_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_HIGH 0x10520 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugHigh:8;	// Rx TD Risc Test Bus High
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugHigh_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF149 0x10524 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16Four:16;	// Rx TD Risc Test Bus 16 four
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf149_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF_TEST_PLUG_FOUR 0x10528 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugFour:8;	// Rx TD Risc Test Bus four
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIfTestPlugFour_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14B 0x1052C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn0LocalFineCntFromCoarseRdy:12;	// det_an0_local_fine_cnt_from_coarse
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf14B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14C 0x10530 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn1LocalFineCntFromCoarseRdy:12;	// det_an0_local_fine_cnt_from_coarse
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf14C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14D 0x10534 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn2LocalFineCntFromCoarseRdy:12;	// det_an0_local_fine_cnt_from_coarse
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf14D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14E 0x10538 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn3LocalFineCntFromCoarseRdy:12;	// det_an0_local_fine_cnt_from_coarse
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf14E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF14F 0x1053C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCommonFineCntFromCoarseRdy:12;	// det_an0_local_fine_cnt_from_coarse
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf14F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF150 0x10540 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerMode:1;	// logger_mode
		uint32 testBusFifoLowThr:15;	// test_bus_fifo_low_thr
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf150_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF151 0x10544 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 testBusFifoHighThr:15;	// test_bus_fifo_high_thr
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf151_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF152 0x10548 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusDumpAllDataStrb:1;	// test_bus_dump_all_data_strb
		uint32 testBusOneShotMode:1;	// test_bus_one_shot_mode
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf152_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF153 0x1054C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxEn:1;	// pac_phy_tx_en
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf153_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF154 0x10550 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerRiscBytesInFifo:8;	// logger_risc_bytes_in_fifo
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf154_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF155 0x10554 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxtdMuNdp:1;	// rxtd_mu_ndp
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf155_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF156 0x10558 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcCancellationOnHtStf:1;	// dc_cancellation_on_ht_stf
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf156_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TD_PHY_RXTD_IF157 0x1055C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muHostAntennaEnMaster:4;	// mu_host_antenna_en_master
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0TdPhyRxtdIf157_u;



#endif // _PHY_RX_TD_IF_RISC_PAGE_0_TD_REGS_H_
