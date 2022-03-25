/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#ifndef PLL2_H
#define PLL2_H

#include <asm/arch/leopard.h>

/* TOPRGU Register */
#define TOPRGU_STRAP_PAR 		(RGU_BASE + 0x60)

/* APMIXEDSYS Register */
#define AP_PLL_CON0             (APMIXED_BASE + 0x00)
#define AP_PLL_CON1             (APMIXED_BASE + 0x04)
#define AP_PLL_CON2             (APMIXED_BASE + 0x08)
#define AP_PLL_CON3             (APMIXED_BASE + 0x0C)
#define AP_PLL_CON4             (APMIXED_BASE + 0x10)
#define AP_PLL_CON5             (APMIXED_BASE + 0x14)
#define AP_PLL_CON6             (APMIXED_BASE + 0x18)
#define AP_PLL_CON7             (APMIXED_BASE + 0x1C)
#define CLKSQ_STB_CON0          (APMIXED_BASE + 0x20)
#define PLL_PWR_CON0            (APMIXED_BASE + 0x24)
#define PLL_PWR_CON1            (APMIXED_BASE + 0x28)
#define PLL_ISO_CON0            (APMIXED_BASE + 0x2C)
#define PLL_ISO_CON1            (APMIXED_BASE + 0x30)
#define PLL_STB_CON0            (APMIXED_BASE + 0x34)
#define DIV_STB_CON0            (APMIXED_BASE + 0x38)
#define PLL_CHG_CON0            (APMIXED_BASE + 0x3C)
#define PLL_TEST_CON0           (APMIXED_BASE + 0x40)
#define PLL_TEST_CON1           (APMIXED_BASE + 0x44)
#define PLL_TEST_CON2           (APMIXED_BASE + 0x48)

#define ARMPLL_CON0             (APMIXED_BASE + 0x200)
#define ARMPLL_CON1             (APMIXED_BASE + 0x204)
#define ARMPLL_CON2             (APMIXED_BASE + 0x208)
#define ARMPLL_PWR_CON0         (APMIXED_BASE + 0x20C)

#define MAINPLL_CON0            (APMIXED_BASE + 0x210)
#define MAINPLL_CON1            (APMIXED_BASE + 0x214)
#define MAINPLL_PWR_CON0        (APMIXED_BASE + 0x21C)

#define UNIVPLL_CON0            (APMIXED_BASE + 0x220)
#define UNIVPLL_CON1            (APMIXED_BASE + 0x224)
#define UNIVPLL_PWR_CON0        (APMIXED_BASE + 0x22C)

#define ETH1PLL_CON0            (APMIXED_BASE + 0x300)
#define ETH1PLL_CON1            (APMIXED_BASE + 0x304)
#define ETH1PLL_CON2            (APMIXED_BASE + 0x308)
#define ETH1PLL_CON3            (APMIXED_BASE + 0x30C)
#define ETH1PLL_PWR_CON0        (APMIXED_BASE + 0x310)

#define ETH2PLL_CON0            (APMIXED_BASE + 0x314)
#define ETH2PLL_CON1            (APMIXED_BASE + 0x318)
#define ETH2PLL_CON2            (APMIXED_BASE + 0x31C)
#define ETH2PLL_PWR_CON0        (APMIXED_BASE + 0x320)

#define APLL1_CON0              (APMIXED_BASE + 0x324)
#define APLL1_CON1              (APMIXED_BASE + 0x328)
#define APLL1_CON2              (APMIXED_BASE + 0x32C)
#define APLL1_PWR_CON0          (APMIXED_BASE + 0x330)

#define APLL2_CON0              (APMIXED_BASE + 0x334)
#define APLL2_CON1              (APMIXED_BASE + 0x338)
#define APLL2_CON2              (APMIXED_BASE + 0x33C)
#define APLL2_PWR_CON0          (APMIXED_BASE + 0x340)

#define SGMIPLL_CON0            (APMIXED_BASE + 0x358)
#define SGMIPLL_CON1            (APMIXED_BASE + 0x35C)
#define SGMIPLL_CON2            (APMIXED_BASE + 0x360)
#define SGMIPLL_CON3            (APMIXED_BASE + 0x364)
#define SGMIPLL_PWR_CON0        (APMIXED_BASE + 0x368)

/* TOPCKGEN Register */
#define CLK_MODE                (CKSYS_BASE + 0x000)
#define TST_SEL_0               (CKSYS_BASE + 0x020)
#define TST_SEL_1               (CKSYS_BASE + 0x024)
#define CLK_CFG_0               (CKSYS_BASE + 0x040)
#define CLK_CFG_1               (CKSYS_BASE + 0x050)
#define CLK_CFG_2               (CKSYS_BASE + 0x060)
#define CLK_CFG_3               (CKSYS_BASE + 0x070)
#define CLK_CFG_4               (CKSYS_BASE + 0x080)
#define CLK_CFG_5               (CKSYS_BASE + 0x090)
#define CLK_CFG_6               (CKSYS_BASE + 0x0A0)
#define CLK_CFG_7               (CKSYS_BASE + 0x0B0)
#define CLK_CFG_8               (CKSYS_BASE + 0x0C0)
#define CLK_CFG_9               (CKSYS_BASE + 0x104)
#define CLK_CFG_10              (CKSYS_BASE + 0x108)
#define CLK_CFG_11              (CKSYS_BASE + 0x10C)
#define CLK_AUDDIV_0            (CKSYS_BASE + 0x120)
#define CLK_AUDDIV_1            (CKSYS_BASE + 0x124)
#define CLK_AUDDIV_2            (CKSYS_BASE + 0x128)
#define CLK_SCP_CFG_0           (CKSYS_BASE + 0x200)
#define CLK_SCP_CFG_1           (CKSYS_BASE + 0x204)
#define CLK_MISC_CFG_0          (CKSYS_BASE + 0x210)
#define CLK_MISC_CFG_1          (CKSYS_BASE + 0x214)
#define CLK_MISC_CFG_2          (CKSYS_BASE + 0x218)
#define CLK26CALI_0             (CKSYS_BASE + 0x220)
#define CLK26CALI_1             (CKSYS_BASE + 0x224)
#define CLK26CALI_2             (CKSYS_BASE + 0x228)
#define CKSTA_REG               (CKSYS_BASE + 0x22C)
#define MBIST_CFG_0             (CKSYS_BASE + 0x308)
#define MBIST_CFG_1             (CKSYS_BASE + 0x30C)

/* INFRASYS Register */
#define TOP_CKMUXSEL            (INFRACFG_AO_BASE + 0x00)
#define TOP_CKDIV1              (INFRACFG_AO_BASE + 0x08)
#define TOP_DCMCTL              (INFRACFG_AO_BASE + 0x10)

#define INFRA_PDN_SET0          (INFRACFG_AO_BASE + 0x0040)
#define INFRA_PDN_CLR0          (INFRACFG_AO_BASE + 0x0044)
#define INFRA_PDN_STA0          (INFRACFG_AO_BASE + 0x0048)

/* 0x10000050	INFRA_GLOBALCON_DCMCTL
 * 0	0	faxi_dcm_enable
 * 1	1	fmem_dcm_enable
 * 8	8	axi_clock_gated_en
 * 9	9	l2c_sram_infra_dcm_en
 */
#define	INFRA_GLOBALCON_DCMCTL  (INFRACFG_AO_BASE + 0x050)
#define INFRA_GLOBALCON_DCMCTL_MASK     (0x00000303)
#define INFRA_GLOBALCON_DCMCTL_ON       (0x00000303)

/** 0x10000054	INFRA_GLOBALCON_DCMDBC
 * 6	0	dcm_dbc_cnt (default 7'h7F)
 * 8	8	faxi_dcm_dbc_enable
 * 22	16	dcm_dbc_cnt_fmem (default 7'h7F)
 * 24	24	dcm_dbc_enable_fmem
 **/
#define INFRA_GLOBALCON_DCMDBC  (INFRACFG_AO_BASE + 0x054)
#define INFRA_GLOBALCON_DCMDBC_MASK  ((0x7f<<0) | (1<<8) | (0x7f<<16) | (1<<24))
#define INFRA_GLOBALCON_DCMDBC_ON      ((0<<0) | (1<<8) | (0<<16) | (1<<24))

/** 0x10000058	INFRA_GLOBALCON_DCMFSEL
 * 2	0	dcm_qtr_fsel ("1xx: 1/4, 01x: 1/8, 001: 1/16, 000: 1/32")
 * 11	8	dcm_half_fsel ("1xxx:1/2, 01xx: 1/4, 001x: 1/8,
 *			0001: 1/16, 0000: 1/32")
 * 20	16	dcm_full_fsel ("1xxxx:1/1, 01xxx:1/2, 001xx: 1/4,
 *			0001x: 1/8, 00001: 1/16, 00000: 1/32")
 * 28	24	dcm_full_fsel_fmem ("1xxxx:1/1, 01xxx:1/2, 001xx: 1/4,
 *			0001x: 1/8, 00001: 1/16, 00000: 1/32")
 **/
#define INFRA_GLOBALCON_DCMFSEL (INFRACFG_AO_BASE + 0x058)
#define INFRA_GLOBALCON_DCMFSEL_MASK ((0x7<<0) | (0xf<<8) | (0x1f<<16) | (0x1f<<24))
#define INFRA_GLOBALCON_DCMFSEL_ON ((0<<0) | (0<<8) | (0x10<<16) | (0x10<<24))

#define TOPAXI_PROT_EN          (INFRACFG_AO_BASE + 0x0220)
#define TOPAXI_PROT_STA1        (INFRACFG_AO_BASE + 0x0228)

/* PERI Register */
#define PERI_PDN_SET0           (PERICFG_BASE + 0x0008)
#define PERI_PDN_SET1           (PERICFG_BASE + 0x000C)
#define PERI_PDN_CLR0           (PERICFG_BASE + 0x0010)
#define PERI_PDN_CLR1           (PERICFG_BASE + 0x0014)
#define PERI_PDN_STA0           (PERICFG_BASE + 0x0018)
#define PERI_PDN_STA1           (PERICFG_BASE + 0x001C)

/* WBSYS Mapping Register */
#define EFUSE_RES_REG        (EFUSE_BASE+0x082C)
#define WB2AP_STATUS        (INFRACFG_AO_BASE + 0x384)
#define WB2AP_PLL_RDY        0x00000080

enum {
    PLL_MODE_1  = 1,
    PLL_MODE_2  = 2,
    PLL_MODE_3  = 3,
};

typedef unsigned int        U32;
typedef volatile unsigned int   *UINT32P;

/* for MTCMOS */
#define STA_POWER_DOWN  0
#define STA_POWER_ON    1

/* for SPM regsiter */
#define SPM_POWERON_CONFIG_SET      (SPM_BASE + 0x0000)
#define SPM_PROJECT_CODE            0xb16

#endif
