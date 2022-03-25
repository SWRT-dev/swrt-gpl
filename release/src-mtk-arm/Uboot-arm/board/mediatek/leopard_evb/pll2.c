/*
 * (C) Copyright 2018 MediaTek.Inc
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/leopard.h>
#include <asm/arch/spm_mtcmos.h>

#include "pll2.h"
#include <asm/arch/timer.h>
#include <asm/arch/spl.h>

#define DEBUG_FQMTR     0
#define SET_CPU_MAX_FREQ 1
#define aor(v, a, o) (((v) & (a)) | (o))

#define spm_write(addr, val)  DRV_WriteReg32(addr, val)
#define SPM_PWR_STATUS              (SPM_BASE + 0x060c)
#define SPM_PWR_STATUS_2ND          (SPM_BASE + 0x0610)

#if DEBUG_FQMTR
static unsigned int mt_get_abist_ck_freq(unsigned int ID)
{
    int output = 0;
    int i =0;
    unsigned int temp, clk26cali_0, clk_cfg_9, clk_misc_cfg_1, pll_test_con0;

	if (ID == 21) {
		pll_test_con0 = DRV_Reg32(PLL_TEST_CON0);
		DRV_WriteReg32(PLL_TEST_CON0, 0xFF00012F);
	}

    clk26cali_0 = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0 | 0x80);

    clk_misc_cfg_1 = DRV_Reg32(CLK_MISC_CFG_1);
    DRV_WriteReg32(CLK_MISC_CFG_1, 0xFFFF0300);

    clk_cfg_9 = DRV_Reg32(CLK_CFG_9);
    DRV_WriteReg32(CLK_CFG_9, (ID << 8));

    temp = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, temp | 0x1);

    /*
     * wait frequency meter finish
     */
    while (DRV_Reg32(CLK26CALI_0) & 0x1)
    {
        mdelay(10);
        i++;
        if(i > 10)
        	break;
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;
	output = (temp * 40000) / 1024;
    DRV_WriteReg32(CLK_CFG_9, clk_cfg_9);
    DRV_WriteReg32(CLK_MISC_CFG_1, clk_misc_cfg_1);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
	if (ID == 21) {
		DRV_WriteReg32(PLL_TEST_CON0, pll_test_con0);
	}

    if(i>10)
        return 0;
    else
        return output;
}
#endif

#if DEBUG_FQMTR
static unsigned int mt_get_cpu_freq(void)
{
    int output = 0;
    output = mt_get_abist_ck_freq(21);

	return output;
}
#endif

static unsigned int mt_get_ckgen_ck_freq(unsigned int ID)
{
    int output = 0;
    int i =0;
    unsigned int temp, clk26cali_0, clk_cfg_9, clk_misc_cfg_1;

    clk26cali_0 = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0 | 0x80);

    clk_misc_cfg_1 = DRV_Reg32(CLK_MISC_CFG_1);
    DRV_WriteReg32(CLK_MISC_CFG_1, 0x00FFFFFF);

    clk_cfg_9 = DRV_Reg32(CLK_CFG_9);
    DRV_WriteReg32(CLK_CFG_9, (ID << 16));

    temp = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, temp | 0x10);

    /*
     * wait frequency meter finish
     */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        mdelay(10);
        i++;
        if(i > 10)
        	break;
    }

    temp = DRV_Reg32(CLK26CALI_2) & 0xFFFF;
	output = (temp * 40000) / 1024;
    DRV_WriteReg32(CLK_CFG_9, clk_cfg_9);
    DRV_WriteReg32(CLK_MISC_CFG_1, clk_misc_cfg_1);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);

    if(i>10)
        return 0;
    else
        return output;
}

unsigned int mt_get_mem_freq(unsigned char mux)
{
    int output = 0;
	output = mt_get_ckgen_ck_freq(2);
    return output;
}

#if DEBUG_FQMTR
static unsigned int mt_get_bus_freq(void)
{
    int output = 0;
    int i=0;
    unsigned int temp, clk26cali_0, clk_cfg_9, clk_misc_cfg_1;

    clk26cali_0 = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0 | 0x80);

    clk_misc_cfg_1 = DRV_Reg32(CLK_MISC_CFG_1);
    DRV_WriteReg32(CLK_MISC_CFG_1, 0x00FFFFFF);

    clk_cfg_9 = DRV_Reg32(CLK_CFG_9);
    DRV_WriteReg32(CLK_CFG_9, (54 << 16));

    temp = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, temp | 0x10);

    /*
     * wait frequency meter finish
     */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        mdelay(10);
        i++;
        if(i > 10)
            break;
    }

    temp = DRV_Reg32(CLK26CALI_2) & 0xFFFF;
	output = (temp * 40000) * 4 / 1024;
    DRV_WriteReg32(CLK_CFG_9, clk_cfg_9);
    DRV_WriteReg32(CLK_MISC_CFG_1, clk_misc_cfg_1);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);

    if(i>10)
        return 0;
    else
        return output;
}

#endif

#if DEBUG_FQMTR
const char *abist_array[] =
{
    [0]  = "Power down (no clock)",
	[1]  = "clkph_MCK_o",
    [2]  = "AD_MEMPLL2_CKOUT0_PRE_ISO",
    [3]  = "AD_MAIN_H546M_CK",
    [4]  = "AD_MAIN_H364M_CK",
    [5]  = "AD_MAIN_H218P4M_CK",
    [6]  = "AD_MAIN_H156M_CK",
    [7]  = "AD_UNIV_624M_CK",
    [8]  = "AD_UNIV_416M_CK",
    [9]  = "AD_UNIV_249P6M_CK",
    [10] = "AD_UNIV_178P3M_CK",
    [11] = "f_funivpll_d80_ck",
    [12] = "AD_UNIV_48M_CK",
    [13] = "AD_SGMIIPLL_CK",
    [14] = "AD_AUD1PLL_CK",
    [15] = "AD_AUD2PLL_CK ",
    [16] = "AD_ARMPLL_TOP_TST_CK",
    [17] = "AD_USB_48M_CK",
    [18] = "AD_MAINPLL_CORE_CK",
    [19] = "AD_TRGPLL_CK ",
    [20] = "AD_MEM_26M_CK",
    [21] = "AD_PLLGP_TST_CK",
    [22] = "AD_ETH1PLL_CK",
    [23] = "AD_ETH2PLL_CK",
    [24] = "AD_UNIVPLL_CK",
    [25] = "AD_MEM2MIPI_26M_CK",
    [26] = "AD_MEMPLL_MONCLK",
    [27] = "AD_MEMPLL2_MONCLK",
    [28] = "AD_MEMPLL3_MONCLK",
    [29] = "AD_MEMPLL4_MONCLK",
    [30] = "AD_MEMPLL_REFCLK_BUF",
    [31] = "AD_MEMPLL_FBCLK_BUF",
    [32] = "AD_MEMPLL2_REFCLK_BUF",
    [33] = "AD_MEMPLL2_FBCLK_BUF",
    [34] = "AD_MEMPLL3_REFCLK_BUF",
    [35] = "AD_MEMPLL3_FBCLK_BUF",
    [36] = "AD_MEMPLL4_REFCLK_BUF",
    [37] = "AD_MEMPLL4_FBCLK_BUF",
    [38] = "AD_MEMPLL_TSTDIV2_CK",
    [39] = "XTAL",
    [40] = "trng_freq_debug_in0",
    [41] = "trng_freq_debug_in1",
};

const char *ckgen_array[] =
{
    [0]  = "Power down (no clock)",
    [1]  = "hf_faxi_ck",
    [2]  = "hf_fmem_ck",
    [3]  = "hf_fddrphycfg_ck",
    [4]  = "hf_feth_ck",
    [5]  = "f_fpwm_ck",
    [6]  = "f_fsgmii_ref_0_ck",
    [7]  = "hf_fspinfi_infra_bclk_ck",
    [8]  = "hf_fflash_ck",
    [9]  = "f_fuart_ck",
    [10] = "hf_fspi0_ck",
    [11] = "hf_fspi1_ck",
    [12] = "hf_fmsdc50_0_ck",
    [13] = "hf_fmsdc30_0_ck",
    [14] = "hf_fmsdc30_1_ck",
    [15] = "hf_fap2wbmcu_ck",
    [16] = "hf_fap2wbhif_ck",
    [17] = "hf_audio_ck",
    [18] = "hf_faud_intbus_ck",
    [19] = "hf_fpmicspi_ck",
    [20] = "hf_fscp_ck",
    [21] = "hf_fatb_ck",
    [22] = "hf_fhif_ck",
    [23] = "f_fsata_ck",
    [24] = "hf_fusb20_ck",
    [25] = "f_faud1_ck",
    [26] = "f_faud2_ck",
    [27] = "hf_firrx_ck",
    [28] = "hf_firtx_ck",
    [29] = "hf_fsata_mcu_ck",
    [30] = "hf_fpcie0_mcu_ck",
    [31] = "hf_fpcie1_mcu_ck",
    [32] = "hf_fssusb_mcu_ck",
    [33] = "hf_fcrypto_ck",
    [34] = "f_fsgmii_ref_1_ck",
    [35] = "hf_f10m_ck",
    [36] = "f_faud26m_ck",
    [37] = "f_fsata_ref_ck",
    [38] = "f_fpcie_ref_ck",
    [39] = "f_fssusb_ref_ck",
    [40] = "f_funivpll3_d16_ck",
    [41] = "f_fauxadc_ck",
    [42] = "f_f75k_ck",
    [43] = "f_fefuse_ck",
    [44] = "f_fapmixed_ck",
    [45] = "f_frtc_apmixed_ck",
    [46] = "f_fmsdc_ext_ck",
    [47] = "hf_fpmicspi_ck_scan",
    [48] = "hf_fspi0_pad_ck",
    [49] = "hf_fspi1_pad_ck",
    [50] = "hf_fddrphycfg_ck_scan",
    [51] = "f_fddrphyperi_ck_scan",
    [52] = "f_frtc_fddrphyperi_ck",
    [53] = "hf_faxi_ck_scan",
    [54] = "hd_qaxidcm_ck",
    [55] = "f_fckbus_ck_scan",
    [56] = "f_fckrtc_ck_scan",
    [57] = "f_fclkmux_ck",
    [58] = "f_fxtal_ck",
    [59] = "f_fxtal_ck_cg",
    [60] = "f_fxtal_ck_scan",
    [61] = "f_frtc_ck",
    [62] = "f_frtc_ck_scan",
    [63] = "hf_fpcie1_mcu_ck_scan",
};
#endif

void mt_pll_post_init(void)
{
#if DEBUG_FQMTR
    unsigned int temp;

	printf("mt_pll_post_init: mt_get_cpu_freq = %dKhz\n", mt_get_cpu_freq()*2);
	//printf("mt_pll_post_init: mt_get_mem_freq = %dKhz\n", mt_get_mem_freq(2));
	printf("mt_pll_post_init: mt_get_bus_freq = %dKhz\n", mt_get_bus_freq()*4);

    for(temp = 1; temp <= 41; temp++)
        printf("%s: %d\n", abist_array[temp], mt_get_abist_ck_freq(temp));

    for(temp = 1; temp <= 63; temp++)
        printf("%s: %d\n", ckgen_array[temp], mt_get_ckgen_ck_freq(temp));

    printf("%s: AP_PLL_CON3        = 0x%x\n", __func__, DRV_Reg32(AP_PLL_CON3));
    printf("%s: AP_PLL_CON4        = 0x%x\n", __func__, DRV_Reg32(AP_PLL_CON4));
    printf("%s: AP_PLL_CON6        = 0x%x\n", __func__, DRV_Reg32(AP_PLL_CON6));
    printf("%s: CLKSQ_STB_CON0     = 0x%x\n", __func__, DRV_Reg32(CLKSQ_STB_CON0));
    printf("%s: PLL_ISO_CON0       = 0x%x\n", __func__, DRV_Reg32(PLL_ISO_CON0));
    printf("%s: ARMPLL_CON0        = 0x%x\n", __func__, DRV_Reg32(ARMPLL_CON0));
    printf("%s: ARMPLL_CON1        = 0x%x\n", __func__, DRV_Reg32(ARMPLL_CON1));
    printf("%s: ARMPLL_PWR_CON0    = 0x%x\n", __func__, DRV_Reg32(ARMPLL_PWR_CON0));
    printf("%s: MAINPLL_CON0       = 0x%x\n", __func__, DRV_Reg32(MAINPLL_CON0));
    printf("%s: MAINPLL_CON1       = 0x%x\n", __func__, DRV_Reg32(MAINPLL_CON1));
    printf("%s: MAINPLL_PWR_CON0   = 0x%x\n", __func__, DRV_Reg32(MAINPLL_PWR_CON0));
    printf("%s: UNIVPLL_CON0       = 0x%x\n", __func__, DRV_Reg32(UNIVPLL_CON0));
    printf("%s: UNIVPLL_CON1       = 0x%x\n", __func__, DRV_Reg32(UNIVPLL_CON1));
    printf("%s: UNIVPLL_PWR_CON0   = 0x%x\n", __func__, DRV_Reg32(UNIVPLL_PWR_CON0));
    printf("%s: ETH1PLL_CON0       = 0x%x\n", __func__, DRV_Reg32(ETH1PLL_CON0));
    printf("%s: ETH1PLL_CON1       = 0x%x\n", __func__, DRV_Reg32(ETH1PLL_CON1));
    printf("%s: ETH1PLL_PWR_CON0   = 0x%x\n", __func__, DRV_Reg32(ETH1PLL_PWR_CON0));
    printf("%s: ETH2PLL_CON0       = 0x%x\n", __func__, DRV_Reg32(ETH2PLL_CON0));
    printf("%s: ETH2PLL_CON1       = 0x%x\n", __func__, DRV_Reg32(ETH2PLL_CON1));
    printf("%s: ETH2PLL_PWR_CON0   = 0x%x\n", __func__, DRV_Reg32(ETH2PLL_PWR_CON0));
    printf("%s: SGMIPLL_CON0       = 0x%x\n", __func__, DRV_Reg32(SGMIPLL_CON0));
    printf("%s: SGMIPLL_CON1       = 0x%x\n", __func__, DRV_Reg32(SGMIPLL_CON1));
    printf("%s: SGMIPLL_PWR_CON0   = 0x%x\n", __func__, DRV_Reg32(SGMIPLL_PWR_CON0));

    printf("%s: SPM_PWR_STATUS     = 0x%x, \n", __func__, DRV_Reg32(SPM_PWR_STATUS));
    printf("%s: SPM_PWR_STATUS_2ND = 0x%x, \n", __func__, DRV_Reg32(SPM_PWR_STATUS_2ND));
    printf("%s: INFRA_PDN_CLR0     = 0x%x, \n", __func__, DRV_Reg32(INFRA_PDN_CLR0));
    printf("%s: PERI_PDN_CLR0      = 0x%x, \n", __func__, DRV_Reg32(PERI_PDN_CLR0));
    printf("%s: PERI_PDN_CLR1      = 0x%x, \n", __func__, DRV_Reg32(PERI_PDN_CLR1));
#endif

}

void clkmux_26M(void)
{
#if DEBUG_FQMTR
    DRV_WriteReg32(CLK_CFG_0, 0x00000000);
    DRV_WriteReg32(CLK_CFG_1, 0x00000000);
    DRV_WriteReg32(CLK_CFG_2, 0x00000000);
    DRV_WriteReg32(CLK_CFG_3, 0x00000000);
    DRV_WriteReg32(CLK_CFG_4, 0x00000000);
    DRV_WriteReg32(CLK_CFG_5, 0x00000000);
    DRV_WriteReg32(CLK_CFG_6, 0x00000000);
    DRV_WriteReg32(CLK_CFG_7, 0x00000000);
    DRV_WriteReg32(CLK_CFG_8, 0x00000000);
#endif

}


void mt_pll_init(void)
{
    unsigned int temp;

	/*
	 * reduce CLKSQ disable time
	 */
    DRV_WriteReg32(CLKSQ_STB_CON0, 0x98940501);

	/*
	 * extend PWR/ISO control timing to 1us
	 */
    DRV_WriteReg32(PLL_ISO_CON0, 0x00080008);

	DRV_WriteReg32(UNIVPLL_CON0, DRV_Reg32(UNIVPLL_CON0) | 0x08000000);

    /*
     * xPLL PWR ON
     */
    temp = DRV_Reg32(ARMPLL_PWR_CON0);
    DRV_WriteReg32(ARMPLL_PWR_CON0, temp | 0x1);

    temp = DRV_Reg32(MAINPLL_PWR_CON0);
    DRV_WriteReg32(MAINPLL_PWR_CON0, temp | 0x1);

    temp = DRV_Reg32(UNIVPLL_PWR_CON0);
    DRV_WriteReg32(UNIVPLL_PWR_CON0, temp | 0x1);

    temp = DRV_Reg32(ETH1PLL_PWR_CON0);
    DRV_WriteReg32(ETH1PLL_PWR_CON0, temp | 0x1);

    temp = DRV_Reg32(ETH2PLL_PWR_CON0);
    DRV_WriteReg32(ETH2PLL_PWR_CON0, temp | 0x1);

    temp = DRV_Reg32(SGMIPLL_PWR_CON0);
    DRV_WriteReg32(SGMIPLL_PWR_CON0, temp | 0x1);

    gpt_busy_wait_us(1);

    /*
     * xPLL ISO Disable
     */
    temp = DRV_Reg32(ARMPLL_PWR_CON0);
    DRV_WriteReg32(ARMPLL_PWR_CON0, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(MAINPLL_PWR_CON0);
    DRV_WriteReg32(MAINPLL_PWR_CON0, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(UNIVPLL_PWR_CON0);
    DRV_WriteReg32(UNIVPLL_PWR_CON0, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(ETH1PLL_PWR_CON0);
    DRV_WriteReg32(ETH1PLL_PWR_CON0, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(ETH2PLL_PWR_CON0);
    DRV_WriteReg32(ETH2PLL_PWR_CON0, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(SGMIPLL_PWR_CON0);
    DRV_WriteReg32(SGMIPLL_PWR_CON0, temp & 0xFFFFFFFD);

    /*
     * xPLL Frequency Set
     */
#if defined(MT7626_ASIC_BOARD)
    DRV_WriteReg32(ARMPLL_CON1, 0x811E0000); /* MAX : 1200M */
#else
    DRV_WriteReg32(ARMPLL_CON1, 0x811F4000); /* MAX : 1250M */
#endif
    DRV_WriteReg32(MAINPLL_CON1, 0x811C0000); /* 1120M */
    DRV_WriteReg32(UNIVPLL_CON1, 0x801E0000); /* 1200M */
    DRV_WriteReg32(ETH1PLL_CON1, 0x80190000); /* 500M */
    DRV_WriteReg32(ETH2PLL_CON1, 0x80118000); /* 700M */
    DRV_WriteReg32(SGMIPLL_CON1, 0x80104000); /* 650M */


	/*
     * xPLL Frequency Enable
     */
    temp = DRV_Reg32(ARMPLL_CON0);
    DRV_WriteReg32(ARMPLL_CON0, temp | 0x1);

    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x1);

    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x1);

    temp = DRV_Reg32(ETH1PLL_CON0);
    DRV_WriteReg32(ETH1PLL_CON0, 0x1105);

    temp = DRV_Reg32(ETH2PLL_CON0);
    DRV_WriteReg32(ETH2PLL_CON0, 0x103);

    temp = DRV_Reg32(SGMIPLL_CON0);
    DRV_WriteReg32(SGMIPLL_CON0, 0x1103);

    gpt_busy_wait_us(20);

    /*
     * xPLL DIV RSTB
     */
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x01000000);

    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x01000000);

    /*
     * INFRA CLKMUX
     */

    /*
    *Enable INFRA Bus Divider
     */
    temp = DRV_Reg32(TOP_DCMCTL);
    DRV_WriteReg32(TOP_DCMCTL, temp | 0x1);

    /*
     * Enable Infra DCM
     */
    DRV_WriteReg32(INFRA_GLOBALCON_DCMDBC,
              aor(DRV_Reg32(INFRA_GLOBALCON_DCMDBC),
		  ~INFRA_GLOBALCON_DCMDBC_MASK, INFRA_GLOBALCON_DCMDBC_ON));
    DRV_WriteReg32(INFRA_GLOBALCON_DCMFSEL,
              aor(DRV_Reg32(INFRA_GLOBALCON_DCMFSEL),
		  ~INFRA_GLOBALCON_DCMFSEL_MASK, INFRA_GLOBALCON_DCMFSEL_ON));
    DRV_WriteReg32(INFRA_GLOBALCON_DCMCTL,
              aor(DRV_Reg32(INFRA_GLOBALCON_DCMCTL),
		  ~INFRA_GLOBALCON_DCMCTL_MASK, INFRA_GLOBALCON_DCMCTL_ON));

	/* if > 900MHz, need to change CPU:CCI clock ratio to 1:2 (from 1:1),
	 * that is, set 0x10200640[4:0]=0x12
	 */
	writel((readl(ACLKEN_DIV) & ~0x1F) | 0x12, ACLKEN_DIV);
	gpt_busy_wait_us(10);
	/*
	 * switch MCU to ARMCA7PLL
	 */
	writel((readl(MCU_BUS_MUX) & ~0x600) | 0x200, MCU_BUS_MUX);

    /*
     * TOP CLKMUX
     */
    DRV_WriteReg32(CLK_CFG_0, 0x02010001);
    DRV_WriteReg32(CLK_CFG_1, 0x00080101);
    DRV_WriteReg32(CLK_CFG_2, 0x01010100);
    DRV_WriteReg32(CLK_CFG_3, 0x03030702);
    DRV_WriteReg32(CLK_CFG_4, 0x01060102);
    DRV_WriteReg32(CLK_CFG_5, 0x01010201);
    DRV_WriteReg32(CLK_CFG_6, 0x01010000);
    DRV_WriteReg32(CLK_CFG_7, 0x01010101);
    DRV_WriteReg32(CLK_CFG_8, 0x00010101);

    /* enable scpsys clock off control */
    DRV_WriteReg32(CLK_SCP_CFG_0, 0x3FF);
    /* enable scpsys clock off control */
    DRV_WriteReg32(CLK_SCP_CFG_1, 0x11);

    /*
     * for MTCMOS
     */
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));
    spm_mtcmos_ctrl_eth(STA_POWER_ON); /* if hang here, can comment out it */
    spm_mtcmos_ctrl_hif0(STA_POWER_ON); /* if hang here, can comment out it */
    spm_mtcmos_ctrl_hif1(STA_POWER_ON); /* if hang here, can comment out it */

    /*
     * for CG
     */
    DRV_WriteReg32(INFRA_PDN_CLR0, 0xFFFFFFFF);
    DRV_WriteReg32(PERI_PDN_CLR0, 0xFFFFFFFF);
    DRV_WriteReg32(PERI_PDN_CLR1, 0x7);

}



