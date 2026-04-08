
/***********************************************************************************
File:				PhyRxTdIfRegs.h
Module:				PhyRxTdIf
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_TD_IF_REGS_H_
#define _PHY_RX_TD_IF_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_TD_IF_BASE_ADDRESS                             MEMORY_MAP_UNIT_50_BASE_ADDRESS
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF00                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1000)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF01                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1004)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF02                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1008)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF03                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x100C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF04                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1010)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF05                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1014)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF06                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1018)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF07                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x101C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF08                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1020)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF09                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1024)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF1B                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x106C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF1C                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1070)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF1D                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1074)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF1E                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1078)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF1F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x107C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_CLEAR_ERROR_INT          (PHY_RX_TD_IF_BASE_ADDRESS + 0x1080)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF21                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1084)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF22                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1088)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF23                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x108C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF24                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1090)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF25                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1094)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF26                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1098)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF28                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10A0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF29                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10A4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF2A                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10A8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF2B                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10AC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF2C                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10B0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF2D                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10B4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF2E                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10B8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF2F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10BC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF30                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10C0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF31                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10C4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF32                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10C8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF33                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10CC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF34                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10D0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF3D                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x10F4)
#define	REG_PHY_RX_TD_IF_AFE_DYN_CTRL                         (PHY_RX_TD_IF_BASE_ADDRESS + 0x1104)
#define	REG_PHY_RX_TD_IF_AFE_AUX_CTRL                         (PHY_RX_TD_IF_BASE_ADDRESS + 0x111C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF4A                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1128)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF4C                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1130)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF4D                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1134)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_CCA_INT                  (PHY_RX_TD_IF_BASE_ADDRESS + 0x1138)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF4F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x113C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF50                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1140)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF51                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1144)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF52                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1148)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF53                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x114C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF54                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1150)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF55                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1154)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF56                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1158)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF5F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x117C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF67                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x119C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF6A                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11A8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF6B                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11AC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF6D                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11B4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF6E                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11B8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF6F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11BC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF70                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11C0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF71                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11C4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF72                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11C8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF73                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11CC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF74                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11D0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF75                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11D4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF76                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11D8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF77                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11DC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF78                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11E0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF79                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11E4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF7A                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11E8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_CLEAR_TIMERS_INT         (PHY_RX_TD_IF_BASE_ADDRESS + 0x11EC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF7C                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11F0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF7D                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11F4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF7E                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11F8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF7F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x11FC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF80                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1200)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_SANITY_DELTA_F_VALID     (PHY_RX_TD_IF_BASE_ADDRESS + 0x1208)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF84                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1210)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF85                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1214)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF86                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1218)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF87                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x121C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF88                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1220)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_FREE_RUNNING_CNT_HIGH    (PHY_RX_TD_IF_BASE_ADDRESS + 0x1224)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF8A                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1228)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF8B                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x122C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF8F                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x123C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF90                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1240)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF91                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1244)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_SW_RESET_GENERATE        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1248)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF93                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x124C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF94                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1250)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF95                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1254)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF96                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1258)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF97                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x125C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF98                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1260)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF99                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1264)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF9A                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1268)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF9B                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x126C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFB5                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x12D4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFB6                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x12D8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFB7                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x12DC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TIMER0                   (PHY_RX_TD_IF_BASE_ADDRESS + 0x1320)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFC9                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1324)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TIMER1                   (PHY_RX_TD_IF_BASE_ADDRESS + 0x1328)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFCB                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x132C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_LOW_ONLY16     (PHY_RX_TD_IF_BASE_ADDRESS + 0x1330)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_MID_ONLY16     (PHY_RX_TD_IF_BASE_ADDRESS + 0x1334)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_HIGH_ONLY16    (PHY_RX_TD_IF_BASE_ADDRESS + 0x1338)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_FOUR_ONLY16    (PHY_RX_TD_IF_BASE_ADDRESS + 0x133C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFD0                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1340)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFD1                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1344)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFD2                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1348)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFD9                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1364)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFDA                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1368)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFDD                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1374)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFDE                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x1378)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFF0                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13C0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFF1                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13C4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFF2                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13C8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFFB                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13EC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFFC                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13F0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFFD                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13F4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFFE                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13F8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IFFF                        (PHY_RX_TD_IF_BASE_ADDRESS + 0x13FC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF102                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1408)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF103                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x140C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_CSM_CLEAR_ERROR_INT      (PHY_RX_TD_IF_BASE_ADDRESS + 0x1410)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF10D                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1434)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF10E                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1438)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF120                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1480)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF121                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1484)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF122                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1488)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF123                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x148C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF124                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1490)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF125                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1494)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF126                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1498)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF127                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x149C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF128                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14A0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS              (PHY_RX_TD_IF_BASE_ADDRESS + 0x14A4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF12C                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14B0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF131                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14C4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF136                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14D8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF137                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14DC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF138                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14E0)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF139                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14E4)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_CO_CH_COUNTER            (PHY_RX_TD_IF_BASE_ADDRESS + 0x14E8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF13B                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14EC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF13E                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14F8)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF13F                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x14FC)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF140                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1500)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF141                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1504)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF143                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x150C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_LOW            (PHY_RX_TD_IF_BASE_ADDRESS + 0x1510)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF145                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1514)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_MID            (PHY_RX_TD_IF_BASE_ADDRESS + 0x1518)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF147                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x151C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_HIGH           (PHY_RX_TD_IF_BASE_ADDRESS + 0x1520)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF149                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1524)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_FOUR           (PHY_RX_TD_IF_BASE_ADDRESS + 0x1528)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF14B                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x152C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF14C                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1530)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF14D                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1534)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF14E                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1538)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF14F                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x153C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF150                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1540)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF151                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1544)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF152                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1548)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF153                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x154C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF154                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1550)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF155                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1554)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF156                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1558)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF157                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x155C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF158                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1560)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF159                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1564)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF15A                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1568)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF15B                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x156C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF15C                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1570)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF15D                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1574)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF15F                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x157C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF160                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1580)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF161                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1584)
#define	REG_PHY_RX_TD_IF_FCSI_BUSY_ANT_162                    (PHY_RX_TD_IF_BASE_ADDRESS + 0x1588)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF163                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x158C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF164                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1590)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF165                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1594)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF166                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x1598)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF167                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x159C)
#define	REG_PHY_RX_TD_IF_PHY_RXTD_IF168                       (PHY_RX_TD_IF_BASE_ADDRESS + 0x15A0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_IF_PHY_RXTD_IF00 0x1000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpiLow_riscGpoLow : 16; // IF Semaphore reg00, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf00_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF01 0x1004 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpiHigh_riscGpoHigh : 16; // IF Semaphore reg01, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf01_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF02 0x1008 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpioOenLow : 16; // IF Semaphore reg02, reset value: 0xffff, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf02_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF03 0x100C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscGpioOenHigh : 16; // IF Semaphore reg03, reset value: 0xffff, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf03_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF04 0x1010 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg04 : 16; //GenRisc operational mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf04_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF05 0x1014 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg05 : 16; // IF Semaphore reg05, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf05_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF06 0x1018 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg06 : 16; // IF Semaphore reg06, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf06_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF07 0x101C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg07 : 16; // IF Semaphore reg07, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf07_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF08 0x1020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg08 : 16; // IF Semaphore reg08, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf08_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF09 0x1024 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 semaphoreReg09 : 16; // IF Semaphore reg09, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf09_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF1B 0x106C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdEnableOverride : 1; //td enable override, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf1B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF1C 0x1070 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscMaskInt : 11; //Gen risc Mask Interrupt, reset value: 0x0, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf1C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF1D 0x1074 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscClearInt0 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt1 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt2 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt3 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt4 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt5 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt6 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt7 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt8 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt9 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 genRiscClearInt10 : 1; //Gen risc Clear Int, reset value: 0x0, access type: WO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf1D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF1E 0x1078 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscNoiseCauseInt0 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscTxEnStartCauseInt1 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscPreTxCauseInt2 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscRxLatchFallingRxRdyCauseInt3 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscErrorCauseInt4 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscFdLastSymbolCauseInt5 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscHostCauseInt6 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscHostCauseInt7 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscHostCauseInt8 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscHostCauseInt9 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 genRiscHostCauseInt10 : 1; //Gen risc Cause Int, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf1E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF1F 0x107C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableErrorInt : 16; //Gen Risc Enable Error Int, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf1F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_CLEAR_ERROR_INT 0x1080 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearErrorInt : 16; //Gen Risc Clear Enable Error Int, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfClearErrorInt_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF21 0x1084 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxRdyFallDelayIf : 13; //rx_rdy_fall_delay_if, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 rxRdyFallDelayIfEn : 1; //rx_rdy_fall_delay_if_en, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf21_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF22 0x1088 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntLowRt : 16; //Free Running Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf22_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF23 0x108C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ofdmEnableBit : 1; //ofdm enable bit for csm, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf23_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF24 0x1090 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmAntennaEnable : 4; //csm_antenna_enable, reset value: 0xF, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf24_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF25 0x1094 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 antennaDeletion : 4; //antenna_deletion, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf25_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF26 0x1098 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 symbolPramReadyStm : 2; //symbol_pram_ready_stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 deltaFStm : 2; //delta_f_stm, reset value: 0x0, access type: RO
		uint32 reserved1 : 26;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf26_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF28 0x10A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopSignalThIf : 13; //stop_signal_th_if, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 stopSignalThIfEn : 1; //stop_signal_th_if_en, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf28_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF29 0x10A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscInt6 : 1; //Rx TD Risc int 6, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf29_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF2A 0x10A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 12;
		uint32 tdCsmState : 3; //td_csm_state, reset value: 0x0, access type: RO
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf2A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF2B 0x10AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscAirtimeUs : 16; //gen_risc_airtime_us, reset value: 0xfff, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf2B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF2C 0x10B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRisc4UsBoundaryOffset : 12; //gen_risc_4us_boundary_offset, reset value: 0x7ff, access type: RW
		uint32 ccaCntSrcCtrl : 4; //cca_cnt_src_ctrl (0 - gen risc, 1- hw), reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf2C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF2D 0x10B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htShortInd : 1; //ht_short_ind, reset value: 0x0, access type: RO
		uint32 ccaCs : 1; //cca_cs, reset value: 0x0, access type: RO
		uint32 heShortInd : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf2D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF2E 0x10B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetType : 1; //packet_type, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 isHtSw : 1; //is_ht, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 isHtSel : 1; //select HW ht of sw ht, reset value: 0x0, access type: RW
		uint32 genriscRdyEnable : 1; //select HW ht of sw ht, reset value: 0x0, access type: RW
		uint32 reserved2 : 23;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf2E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF2F 0x10BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcRemovalShifter : 4; //DC removal shifter, reset value: 0x0, access type: RW
		uint32 dcRemovalShifterGearShift : 4; //DC removal shifter for gear shift, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf2F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF30 0x10C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscAirtimeUsMax : 1; //gen_risc_airtime_us_max, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf30_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF31 0x10C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscAirtimeUsOverride : 1; //gen_risc_airtime_us_max, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf31_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF32 0x10C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaBusyGenRisc : 1; //cca_busy_gen_risc, reset value: 0x0, access type: RW
		uint32 ccaModeCtrl : 1; //cca_mode_ctrl, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 ccaBusyGenRiscSec : 1; //cca_busy_gen_risc_sec, reset value: 0x0, access type: RW
		uint32 ccaModeCtrlSec : 1; //cca_mode_ctrl_sec, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 ccaBusyGenRiscSec40 : 1; //cca_busy_gen_risc_sec_40, reset value: 0x0, access type: RW
		uint32 ccaModeCtrlSec40 : 1; //cca_mode_ctrl_sec_40, reset value: 0x0, access type: RW
		uint32 reserved2 : 22;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf32_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF33 0x10CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cca20P : 1; //cca20p, reset value: 0x1, access type: RO
		uint32 cca20S : 1; //cca20s, reset value: 0x1, access type: RO
		uint32 cca40S : 1; //cca40s, reset value: 0x1, access type: RO
		uint32 reserved0 : 2;
		uint32 cca20SPifs : 1; //cca20s_pifs, reset value: 0x1, access type: RO
		uint32 cca40SPifs : 1; //cca40s_pifs, reset value: 0x1, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf33_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF34 0x10D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errorCauseReg : 16; //Rx TD Risc Error Cause Reg, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf34_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF3D 0x10F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscTestBusEn : 4; //gen_risc_test_bus_en, reset value: 0x7, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf3D_u;

/*REG_PHY_RX_TD_IF_AFE_DYN_CTRL 0x1104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx0On : 1; //Rx0 Standby , reset value: 0x0, access type: RW
		uint32 rx1On : 1; //Rx1 Standby, reset value: 0x0, access type: RW
		uint32 rx2On : 1; //Rx2 Standby , reset value: 0x0, access type: RW
		uint32 rx3On : 1; //Rx2 Standby , reset value: 0x0, access type: RW
		uint32 tx0On : 1; //Tx0 Standby, reset value: 0x0, access type: RW
		uint32 tx1On : 1; //Tx1 Standby , reset value: 0x0, access type: RW
		uint32 tx2On : 1; //Tx2 Standby, reset value: 0x0, access type: RW
		uint32 tx3On : 1; //Tx3 Standby, reset value: 0x0, access type: RW
		uint32 aux0AdcOn : 1; //Aux0 Standby, reset value: 0x0, access type: RW
		uint32 aux1AdcOn : 1; //Aux1 Standby, reset value: 0x0, access type: RW
		uint32 aux2AdcOn : 1; //Aux2 Standby, reset value: 0x0, access type: RW
		uint32 aux3AdcOn : 1; //Aux3 Standby, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfAfeDynCtrl_u;

/*REG_PHY_RX_TD_IF_AFE_AUX_CTRL 0x111C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aux0ChEn : 1; //Aux ADC channel enables, reset value: 0x0, access type: RW
		uint32 aux1ChEn : 1; //Aux ADC channel enables, reset value: 0x0, access type: RW
		uint32 aux2ChEn : 1; //Aux ADC channel enables, reset value: 0x0, access type: RW
		uint32 aux3ChEn : 1; //Aux ADC channel enables, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfAfeAuxCtrl_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF4A 0x1128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testFifoFull : 1; //test_fifo_full, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 tbFifoError : 5; //tb fifo error, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf4A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF4C 0x1130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorBypassEn : 1; //detector_bypass_en, reset value: 0x0, access type: RW
		uint32 detectorBypassResetRxtdEn : 1; //detector_bypass_reset_rxtd_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 detectorBypassCntValue : 12; //detector_bypass_cnt_value, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf4C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF4D 0x1134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableCcaInt : 3; //enable_cca_int, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf4D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_CCA_INT 0x1138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearCcaInt : 3; //clear_cca_int, reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfCcaInt_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF4F 0x113C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaCauseReg : 3; //cca_cause_reg, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf4F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF50 0x1140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstRiscAn0 : 7; //Noise_est_risc_an0, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 noiseEstRiscAn1 : 7; //Noise_est_risc_an1, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf50_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF51 0x1144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstRiscAn2 : 7; //Noise_est_risc_an2, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 noiseEstRiscAn3 : 7; //Noise_est_risc_an3, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf51_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF52 0x1148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi0Data : 7; //Rx TD Noise Estimation gi0 Data, reset value: 0x0, access type: RO
		uint32 noiseEstGi0DataValid : 1; //Rx TD Noise Estimation gi0 Data Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf52_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF53 0x114C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi1Data : 7; //Rx TD Noise Estimation gi1 Data, reset value: 0x0, access type: RO
		uint32 noiseEstGi1DataValid : 1; //Rx TD Noise Estimation gi1 Data Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf53_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF54 0x1150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi2Data : 7; //Rx TD Noise Estimation gi2 Data, reset value: 0x0, access type: RO
		uint32 noiseEstGi2DataValid : 1; //Rx TD Noise Estimation gi2 Data Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf54_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF55 0x1154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstGi3Data : 7; //Rx TD Noise Estimation gi3 Data, reset value: 0x0, access type: RO
		uint32 noiseEstGi3DataValid : 1; //Rx TD Noise Estimation gi3 Data Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf55_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF56 0x1158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noiseEstRiscAllValid : 1; //Noise_est_risc_all_valid, reset value: 0x0, access type: RW
		uint32 noiseEstRiscSelect : 1; //Noise_est_risc_select, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf56_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF5F 0x117C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detShortenedMa : 7; //Rx TD  Shortened moving average value, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 waitCycles : 8; //Rx TD  wait cycles , reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf5F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF67 0x119C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wdCounterEn : 1; //Watch dog counter enble, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 clearWdCounter : 1; //Clear Watch dog counter enble, reset value: 0x0, access type: WO
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf67_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF6A 0x11A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detNoiseEstValidEn : 1; //det_noise_est_valid_en, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf6A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF6B 0x11AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetRawDetection : 8; //Sanity Detector Raw Detection, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf6B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF6D 0x11B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetCoarseRdyAn0 : 1; //Sanity Detector Coarse Ready An0, reset value: 0x0, access type: RO
		uint32 sanityDetCoarseRdyAn1 : 1; //Sanity Detector Coarse Ready An1, reset value: 0x0, access type: RO
		uint32 sanityDetCoarseRdyAn2 : 1; //Sanity Detector Coarse Ready An2, reset value: 0x0, access type: RO
		uint32 sanityDetCoarseRdyAn3 : 1; //Sanity Detector Coarse Ready An3, reset value: 0x0, access type: RO
		uint32 sanityDetCommonCoarseErr : 1; //Sanity_det_common_coarse_err, reset value: 0x0, access type: RO
		uint32 sanityDetCommonFineErr : 1; //Sanity_det_common_fine_err, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf6D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF6E 0x11B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityHostAntennaEn : 4; //sanity Host Antenna Enable, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf6E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF6F 0x11BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn0CoarseCnt : 16; //Sanity An 0 Coarse Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf6F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF70 0x11C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn1CoarseCnt : 16; //Sanity An 1 Coarse Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf70_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF71 0x11C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn2CoarseCnt : 16; //Sanity An 2 Coarse Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf71_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF72 0x11C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn3CoarseCnt : 16; //Sanity An 3 Coarse Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf72_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF73 0x11CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn0FineCnt : 16; //Sanity An 0 Fine Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf73_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF74 0x11D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn1FineCnt : 16; //Sanity An 1 Fine Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf74_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF75 0x11D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn2FineCnt : 16; //Sanity An 2 Fine Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf75_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF76 0x11D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityAn3FineCnt : 16; //Sanity An 3 Fine Counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf76_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF77 0x11DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityClkPerStrobe : 5; //Sanity Clock Per Strobe, reset value: 0xf, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf77_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF78 0x11E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cntReset : 1; //Sanity reset the Sanity Counters, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf78_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF79 0x11E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonCoarseRdyCnt : 16; //Sanity Detector Coarse Max Abs Sc An0, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf79_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF7A 0x11E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableTimersInt : 2; //enable_timers_int, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf7A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_CLEAR_TIMERS_INT 0x11EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearTimersInt : 2; //clear_timers_int, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfClearTimersInt_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF7C 0x11F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaTimersReg : 2; //cca_timers_reg, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf7C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF7D 0x11F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonFineRdyCnt : 16; //common_fine_rdy_cnt, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf7D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF7E 0x11F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxtdCsmNcoFreqData : 15; //Sanity Delta f value at the end of the packet, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf7E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF7F 0x11FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetFineRdyAn0 : 1; //Sanity Detector Fine Ready An0, reset value: 0x0, access type: RO
		uint32 sanityDetFineRdyAn1 : 1; //Sanity Detector Fine Ready An1, reset value: 0x0, access type: RO
		uint32 sanityDetFineRdyAn2 : 1; //Sanity Detector Fine Ready An2, reset value: 0x0, access type: RO
		uint32 sanityDetFineRdyAn3 : 1; //Sanity Detector Fine Ready An3, reset value: 0x0, access type: RO
		uint32 sanityDetCommonFineRdy : 1; //sanity_det_common_fine_rdy, reset value: 0x0, access type: RO
		uint32 sanityDetCommonCoarseRdy : 1; //sanity_det_common_coarse_rdy, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf7F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF80 0x1200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDetDetectorBw : 2; //Sanity Detector Top Is Full CB, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 sanityDetImdDetect : 1; //Sanity Detector Top Signal Detect, reset value: 0x0, access type: RO
		uint32 sanityDetSignalDetect : 1; //Sanity Detector Top Signal Detect, reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 sanityDetDetectorMask : 8; //Sanity Detector Top Is Full CB, reset value: 0x0, access type: RO
		uint32 reserved2 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf80_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_SANITY_DELTA_F_VALID 0x1208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityDeltaFValid : 1; //sanity Delta F Data Valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfSanityDeltaFValid_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF84 0x1210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIntMask : 1; //phy_mac_int_mask, reset value: 0x0, access type: RW
		uint32 txPacketIntMask : 1; //tx_packet_int_mask, reset value: 0x0, access type: RW
		uint32 wdIntMask : 1; //wd_int_mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf84_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF85 0x1214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearPhyMacInt : 1; //clear_phy_mac_int, reset value: 0x0, access type: WO
		uint32 clearTxPacketInt : 1; //clear_tx_l_length_int, reset value: 0x0, access type: WO
		uint32 clearWdInt : 1; //clear_wd_int, reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf85_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF86 0x1218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIntCauseReg : 1; //phy_mac_int_cause_reg, reset value: 0x0, access type: RO
		uint32 txPacketIntCauseReg : 1; //tx_packet_int_cause_reg, reset value: 0x0, access type: RO
		uint32 wdIntCauseReg : 1; //wd_int_cause_reg, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf86_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF87 0x121C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacInt : 1; //phy_mac_int, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf87_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF88 0x1220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntLow : 16; //free_running_cnt_low, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf88_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_FREE_RUNNING_CNT_HIGH 0x1224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntHigh : 16; //free_running_cnt_high, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfFreeRunningCntHigh_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF8A 0x1228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntThLow : 16; //free_running_cnt_th_low, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf8A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF8B 0x122C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRunningCntThHigh : 16; //free_running_cnt_th_high, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf8B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF8F 0x123C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg : 16; //Rx TD Sub blocks SW Reset, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf8F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF90 0x1240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blockEn : 16; //Rx TD Sub blocks Enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf90_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF91 0x1244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask : 16; //Rx TD Gsm Sw Reset., reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf91_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_SW_RESET_GENERATE 0x1248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate : 16; //sw_reset_generate, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfSwResetGenerate_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF93 0x124C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn0 : 13; //grisc_dc_removal_data_i_an0, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf93_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF94 0x1250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn0 : 13; //grisc_dc_removal_data_q_an0, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf94_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF95 0x1254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn1 : 13; //grisc_dc_removal_data_i_an1, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf95_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF96 0x1258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn1 : 13; //grisc_dc_removal_data_q_an1, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf96_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF97 0x125C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn2 : 13; //grisc_dc_removal_data_i_an2, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf97_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF98 0x1260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn2 : 13; //grisc_dc_removal_data_q_an2, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf98_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF99 0x1264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataIAn3 : 13; //grisc_dc_removal_data_i_an3, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf99_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF9A 0x1268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalDataQAn3 : 13; //grisc_dc_removal_data_q_an3, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf9A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF9B 0x126C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscDcRemovalValid : 1; //grisc_dc_removal_valid, reset value: 0x0, access type: WO
		uint32 reserved0 : 3;
		uint32 griscDcShiftValueOverride : 1; //grisc_dc_shift_value_override, reset value: 0x0, access type: WO
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf9B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFB5 0x12D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr200 : 8; //cca_pwr_20_0, reset value: 0x0, access type: RO
		uint32 ccaPwr201 : 8; //cca_pwr_20_1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfb5_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFB6 0x12D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr202 : 8; //cca_pwr_20_2, reset value: 0x0, access type: RO
		uint32 ccaPwr203 : 8; //cca_pwr_20_3, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfb6_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFB7 0x12DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr204 : 8; //cca_pwr_20_4, reset value: 0x0, access type: RO
		uint32 ccaPwr205 : 8; //cca_pwr_20_5, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfb7_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TIMER0 0x1320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0Th_timer0 : 16; //timer0, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTimer0_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFC9 0x1324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer0StbTh : 4; //timer0_stb_th, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfc9_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TIMER1 0x1328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1Th_timer1 : 16; //timer1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTimer1_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFCB 0x132C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timer1StbTh : 4; //timer1_stb_th, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfcb_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_LOW_ONLY16 0x1330 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLowOnly16 : 16; //Rx TD Risc Test Bus 16 Low, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugLowOnly16_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_MID_ONLY16 0x1334 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugMidOnly16 : 16; //Rx TD Risc Test Bus 16 mid, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugMidOnly16_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_HIGH_ONLY16 0x1338 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugHighOnly16 : 16; //Rx TD Risc Test Bus 16 high, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugHighOnly16_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_FOUR_ONLY16 0x133C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugFourOnly16 : 16; //Rx TD Risc Test Bus 16 four, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugFourOnly16_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFD0 0x1340 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr206 : 8; //cca_pwr_20_6, reset value: 0x0, access type: RO
		uint32 ccaPwr207 : 8; //cca_pwr_20_7, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfd0_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFD1 0x1344 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr400 : 8; //cca_pwr_40_0, reset value: 0x0, access type: RO
		uint32 ccaPwr401 : 8; //cca_pwr_40_1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfd1_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFD2 0x1348 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr402 : 8; //cca_pwr_40_2, reset value: 0x0, access type: RO
		uint32 ccaPwr403 : 8; //cca_pwr_40_3, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfd2_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFD9 0x1364 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bOverrideRxAntEn : 1; //b_override_rx_ant_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 bOverrideRxAntSel : 2; //b_override_rx_ant_sel, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 bOverrideTxAntEn : 1; //b_override_tx_ant_en, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 bOverrideTxAntSel : 2; //b_override_tx_ant_sel, reset value: 0x0, access type: RW
		uint32 reserved3 : 18;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfd9_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFDA 0x1368 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bInterfaceRxAnt : 2; //b_interface_rx_ant, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 bInterfaceTxAnt : 2; //b_interface_tx_ant, reset value: 0x0, access type: RO
		uint32 reserved1 : 26;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfda_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFDD 0x1374 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigRiscIn : 16; //spare_config_risc_in, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfdd_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFDE 0x1378 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigRiscOut : 16; //spare_config_risc_out, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfde_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFF0 0x13C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 accumulatorResetValueI : 12; //accumulator_reset_value_I, reset value: 0x1, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIff0_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFF1 0x13C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 accumulatorResetValueQ : 12; //accumulator_reset_value_Q, reset value: 0x1, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIff1_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFF2 0x13C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 resetValuesIq : 1; //reset_values_IQ, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIff2_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFFB 0x13EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGprLow : 16; //gpr low, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIffb_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFFC 0x13F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGprHigh : 16; //gpr high, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIffc_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFFD 0x13F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGpo : 16; //gpo high, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIffd_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFFE 0x13F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGpiSyncLow : 16; //hyp_rxtd_gpi_sync_low, reset value: 0xa008, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIffe_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IFFF 0x13FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxtdGpiSyncHigh : 16; //hyp_rxtd_gpi_sync_high, reset value: 0xa008, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfff_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF102 0x1408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmErrorCauseReg : 5; //Rx TD Risc Error Cause Reg, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf102_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF103 0x140C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmEnableErrorInt : 5; //Gen Risc Enable Error Int, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf103_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_CSM_CLEAR_ERROR_INT 0x1410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csmClearErrorInt : 5; //Gen Risc Clear Enable Error Int, reset value: 0x0, access type: WO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfCsmClearErrorInt_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF10D 0x1434 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shortCalcGo : 1; //short_calc_go, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf10D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF10E 0x1438 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sanityRiscAntennaEn : 4; //Sanity Risc Antenna Enable, reset value: 0x0, access type: RW
		uint32 sanityRiscAntennaEnValid : 1; //Sanity Risc Antenna Enable Valid, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf10E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF120 0x1480 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearPifsMem : 1; //clear_pifs_mem, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf120_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF121 0x1484 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaClearPrim20Genrisc : 1; //cca_clear_prim20_genrisc, reset value: 0x0, access type: RW
		uint32 ccaClearPrim20Enable : 1; //cca_clear_prim20_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf121_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF122 0x1488 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr20Max : 8; //cca_pwr_20_max, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf122_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF123 0x148C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timingDiffTh : 7; //timing_diff_th , reset value: 0x5, access type: RW
		uint32 reserved0 : 1;
		uint32 useVerifiedDet : 1; //use_verified_det , reset value: 0x1, access type: RW
		uint32 reserved1 : 3;
		uint32 quickFirstDetection : 1; //quick_first_detection, reset value: 0x1, access type: RW
		uint32 reserved2 : 19;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf123_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF124 0x1490 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decimatorOobTap : 4; //decimator_oob_tap, reset value: 0x9, access type: RW
		uint32 decimatorOobTapHb : 4; //decimator_oob_tap_hb, reset value: 0xb, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf124_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF125 0x1494 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 decimatorRiscBankEn : 1; //decimator_risc_bank_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 decimatorRiscBank : 2; //decimator_risc_bank, reset value: 0x0, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf125_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF126 0x1498 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiClkWrDivrExtRf : 6; //fcsi_clk_wr_divr_ext_rf, reset value: 0x4, access type: RW
		uint32 reserved0 : 2;
		uint32 fcsiClkRdDivrExtRf : 6; //fcsi_clk_rd_divr_ext_rf, reset value: 0xf, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf126_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF127 0x149C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiClkWrDivrAfe : 6; //fcsi_clk_wr_divr_afe, reset value: 0x4, access type: RW
		uint32 reserved0 : 2;
		uint32 fcsiClkRdDivrAfe : 6; //fcsi_clk_rd_divr_afe, reset value: 0xf, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf127_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF128 0x14A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 externalRfMode : 1; //internal_rf_mode, reset value: 0x1, access type: RW
		uint32 reserved1 : 2;
		uint32 fcsiModeMs : 1; //fcsi_mode_ms, reset value: 0x1, access type: RW
		uint32 fcsiModeRfExt : 1; //fcsi_mode_rf_ext, reset value: 0x1, access type: RW
		uint32 reserved2 : 2;
		uint32 fcsiAdditionalCycles : 4; //fcsi_additional_cycles, reset value: 0x0, access type: RW
		uint32 reserved3 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf128_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS 0x14A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrAddr : 7; //fcsi_gr_addr, reset value: 0x0, access type: RW
		uint32 fcsiGrMask : 5; //fcsi_gr_mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 fcsiGrMsAccess : 1; //fcsi_gr_ms_access, reset value: 0x0, access type: RW
		uint32 fcsiGrRfAccess : 1; //fcsi_gr_rf_access, reset value: 0x0, access type: RW
		uint32 fcsiGrRdWr : 1; //fcsi_gr_rd_wr, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfFcsiAccess_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF12C 0x14B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiGrWrData : 16; //fcsi_gr_wr_data, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf12C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF131 0x14C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiRdData : 16; //fcsi_rd_data, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf131_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF136 0x14D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiBusy : 1; //fcsi_busy, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf136_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF137 0x14DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 fcsiMsResetCentralN : 1; //fcsi_ms_reset_central_n, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 fcsiRfResetN : 1; //fcsi_rf_reset_n, reset value: 0x0, access type: RW
		uint32 reserved2 : 27;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf137_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF138 0x14E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypFcsiStateMachine : 3; //hyp_fcsi_state_machine, reset value: 0x0, access type: RO
		uint32 hypFcsiCounter : 5; //hyp_fcsi_counter, reset value: 0x0, access type: RO
		uint32 hypFcsiCounterLimit : 5; //hyp_fcsi_counter_limit, reset value: 0x0, access type: RO
		uint32 hypFcsiClockActive : 1; //hyp_fcsi_clock_active, reset value: 0x0, access type: RO
		uint32 hypFcsiClockEnable : 1; //hyp_fcsi_clock_enable, reset value: 0x0, access type: RO
		uint32 hypFcsiActive : 1; //hyp_fcsi_active, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf138_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF139 0x14E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiForceDefaultWlan2MsCommon : 1; //fcsi_force_default_wlan2ms_common, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf139_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_CO_CH_COUNTER 0x14E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 coChannelCounter : 16; //co_channel_detected, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfCoChCounter_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF13B 0x14EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lltfFalseDetection : 1; //lltf_false_detection, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf13B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF13E 0x14F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi0Data : 11; //rssi_0_data, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf13E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF13F 0x14FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi1Data : 11; //rssi_1_data, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf13F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF140 0x1500 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi2Data : 11; //rssi_2_data, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf140_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF141 0x1504 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rssi3Data : 11; //rssi_3_data, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf141_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF143 0x150C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16Low : 16; //Rx TD Risc Test Bus 16 Low, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf143_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_LOW 0x1510 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLow : 8; //Rx TD Risc Test Bus low , reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugLow_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF145 0x1514 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16Mid : 16; //Rx TD Risc Test Bus 16 Mid, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf145_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_MID 0x1518 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugMid : 8; //Rx TD Risc Test Bus Mid, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugMid_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF147 0x151C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16High : 16; //Rx TD Risc Test Bus 16 High, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf147_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_HIGH 0x1520 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugHigh : 8; //Rx TD Risc Test Bus High, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugHigh_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF149 0x1524 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlug16Four : 16; //Rx TD Risc Test Bus 16 four, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf149_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF_TEST_PLUG_FOUR 0x1528 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugFour : 8; //Rx TD Risc Test Bus four, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIfTestPlugFour_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF14B 0x152C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn0LocalFineCntFromCoarseRdy : 12; //det_an0_local_fine_cnt_from_coarse, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf14B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF14C 0x1530 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn1LocalFineCntFromCoarseRdy : 12; //det_an0_local_fine_cnt_from_coarse, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf14C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF14D 0x1534 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn2LocalFineCntFromCoarseRdy : 12; //det_an0_local_fine_cnt_from_coarse, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf14D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF14E 0x1538 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detAn3LocalFineCntFromCoarseRdy : 12; //det_an0_local_fine_cnt_from_coarse, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf14E_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF14F 0x153C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detCommonFineCntFromCoarseRdy : 12; //det_an0_local_fine_cnt_from_coarse, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf14F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF150 0x1540 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerMode : 1; //logger_mode, reset value: 0x0, access type: RW
		uint32 testBusFifoLowThr : 15; //test_bus_fifo_low_thr, reset value: 0x500, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf150_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF151 0x1544 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 testBusFifoHighThr : 15; //test_bus_fifo_high_thr, reset value: 0x200, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf151_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF152 0x1548 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusDumpAllDataStrb : 1; //test_bus_dump_all_data_strb, reset value: 0x0, access type: WO
		uint32 testBusOneShotMode : 1; //test_bus_one_shot_mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf152_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF153 0x154C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxEn : 1; //pac_phy_tx_en, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf153_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF154 0x1550 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerRiscBytesInFifo : 8; //logger_risc_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf154_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF155 0x1554 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxtdMuNdp : 1; //rxtd_mu_ndp, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf155_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF156 0x1558 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dcCancellationOnHtStf : 1; //dc_cancellation_on_ht_stf, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf156_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF157 0x155C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muHostAntennaEnMaster : 4; //mu_host_antenna_en_master, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf157_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF158 0x1560 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPacRxPhyActive : 1; //phy pac rx phy active, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf158_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF159 0x1564 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscCdbWrCause : 16; //grisc_cdb_wr_cause, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf159_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF15A 0x1568 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscCdbInt : 1; //grisc_cdb_int, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf15A_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF15B 0x156C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscCdbRdCause : 16; //grisc_cdb_rd_cause, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf15B_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF15C 0x1570 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscCdbIntType : 1; //grisc_cdb_int_type, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf15C_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF15D 0x1574 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscCdbPending : 1; //grisc_cdb_rd_cause, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf15D_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF15F 0x157C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorBypassCntDet : 16; //detector_bypass_cnt_det, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf15F_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF160 0x1580 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorBypassCntCoarse : 16; //detector_bypass_cnt_coarse, reset value: 0x500, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf160_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF161 0x1584 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorBypassCntFine : 16; //detector_bypass_cnt_fine, reset value: 0xA00, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf161_u;

/*REG_PHY_RX_TD_IF_FCSI_BUSY_ANT_162 0x1588 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiBusyAnt : 4; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxTdIfFcsiBusyAnt162_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF163 0x158C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 waitCyclesPrimary : 8; //Rx TD  wait cycles priamry, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf163_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF164 0x1590 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macToPhyMinRssi : 8; //mac_to_phy_min_rssi, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf164_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF165 0x1594 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr800 : 8; //cca_pwr_80_0, reset value: 0x0, access type: RO
		uint32 ccaPwr801 : 8; //cca_pwr_80_1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf165_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF166 0x1598 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccaPwr160 : 8; //cca_pwr_160, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf166_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF167 0x159C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cca80SPifs : 1; //cca80s_pifs, reset value: 0x1, access type: RO
		uint32 cca20PPwr : 1; //cca20p_pwr, reset value: 0x1, access type: RO
		uint32 cca20SPwrPifs : 1; //cca20s_pwr_pifs, reset value: 0x1, access type: RO
		uint32 cca40SPwrPifs : 1; //cca40s_pwr_pifs, reset value: 0x1, access type: RO
		uint32 cca20SbPifs : 8; //cca20sb_pifs, reset value: 0x1, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf167_u;

/*REG_PHY_RX_TD_IF_PHY_RXTD_IF168 0x15A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tdGearShiftSymNum : 10; //The symbol number for changing the mu (Start counting from first DATA symbol), reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxTdIfPhyRxtdIf168_u;



#endif // _PHY_RX_TD_IF_REGS_H_
