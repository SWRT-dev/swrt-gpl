/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _GSWSS_API_H
#define _GSWSS_API_H

#include <xgmac_common.h>
#ifdef __KERNEL__
#include <net/switch_api/adap_ops.h>
#else
#include <adap_ops.h>
#endif

#if defined(PC_UTILITY) && PC_UTILITY
#define GSWIP_SS_TOP_BASE		0x40000
#else
#define GSWIP_SS_TOP_BASE		0xb8440000
#endif
#define GSWIP_CFG			0x0000
#define MACSEC_EN			0x0008
#define GSWIPSS_IER0			0x0010
#define GSWIPSS_ISR0			0x0014
#define GSWIPSS_IER1			0x0018
#define GSWIPSS_ISR1			0x001C
#define NCO_LSB				0x0040
#define NCO_MSB				0x0044
#define CFG0_1588			0x0050
#define CFG1_1588			0x0054
#define NCO1_LSB			0x0060
#define NCO1_MSB			0x0064
#define NCO2_LSB			0x0068
#define NCO2_MSB			0x006C
#define NCO3_LSB			0x0070
#define NCO3_MSB			0x0074
#define NCO4_LSB			0x0078
#define NCO4_MSB			0x007C

#define GSWIP_CFG_CORE_SE_POS		15
#define GSWIP_CFG_CORE_SE_WIDTH		1
#define GSWIP_CFG_CLK_MD_POS		2
#define GSWIP_CFG_CLK_MD_WIDTH		2
#define GSWIP_CFG_SS_HWRES_POS		1
#define GSWIP_CFG_SS_HWRES_WIDTH	1

#define MACSEC_EN_SEL_POS		0
#define MACSEC_EN_SEL_WIDTH		4
#define MACSEC_EN_RES_POS		15
#define MACSEC_EN_RES_WIDTH		1

#define GSWIPSS_IER0_XGMAC2_POS		2
#define GSWIPSS_IER0_XGMAC2_WIDTH	1
#define GSWIPSS_IER0_XGMAC3_POS		3
#define GSWIPSS_IER0_XGMAC3_WIDTH	1
#define GSWIPSS_IER0_XGMAC4_POS		4
#define GSWIPSS_IER0_XGMAC4_WIDTH	1
#define GSWIPSS_IER0_XGMAC5_POS		5
#define GSWIPSS_IER0_XGMAC5_WIDTH	1
#define GSWIPSS_IER0_XGMAC6_POS		6
#define GSWIPSS_IER0_XGMAC6_WIDTH	1
#define GSWIPSS_IER0_XGMAC7_POS		7
#define GSWIPSS_IER0_XGMAC7_WIDTH	1
#define GSWIPSS_IER0_XGMAC8_POS		8
#define GSWIPSS_IER0_XGMAC8_WIDTH	1
#define GSWIPSS_IER0_XGMAC9_POS		9
#define GSWIPSS_IER0_XGMAC9_WIDTH	1
#define GSWIPSS_IER0_XGMAC10_POS	10
#define GSWIPSS_IER0_XGMAC10_WIDTH	1
#define GSWIPSS_IER0_XGMAC11_POS	11
#define GSWIPSS_IER0_XGMAC11_WIDTH	1


#define GSWIPSS_ISR0_XGMAC2_POS		2
#define GSWIPSS_ISR0_XGMAC2_WIDTH	1
#define GSWIPSS_ISR0_XGMAC3_POS		3
#define GSWIPSS_ISR0_XGMAC3_WIDTH	1
#define GSWIPSS_ISR0_XGMAC4_POS		4
#define GSWIPSS_ISR0_XGMAC4_WIDTH	1
#define GSWIPSS_ISR0_XGMAC5_POS		5
#define GSWIPSS_ISR0_XGMAC5_WIDTH	1
#define GSWIPSS_ISR0_XGMAC6_POS		6
#define GSWIPSS_ISR0_XGMAC6_WIDTH	1
#define GSWIPSS_ISR0_XGMAC7_POS		7
#define GSWIPSS_ISR0_XGMAC7_WIDTH	1
#define GSWIPSS_ISR0_XGMAC8_POS		8
#define GSWIPSS_ISR0_XGMAC8_WIDTH	1
#define GSWIPSS_ISR0_XGMAC9_POS		9
#define GSWIPSS_ISR0_XGMAC9_WIDTH	1
#define GSWIPSS_ISR0_XGMAC10_POS	10
#define GSWIPSS_ISR0_XGMAC10_WIDTH	1
#define GSWIPSS_ISR0_XGMAC11_POS	11
#define GSWIPSS_ISR0_XGMAC11_WIDTH	1



#define GSWIPSS_IER1_LINK2_POS		2
#define GSWIPSS_IER1_LINK2_WIDTH	1
#define GSWIPSS_IER1_LINK3_POS		3
#define GSWIPSS_IER1_LINK3_WIDTH	1
#define GSWIPSS_IER1_LINK4_POS		4
#define GSWIPSS_IER1_LINK4_WIDTH	1
#define GSWIPSS_IER1_LINK5_POS		5
#define GSWIPSS_IER1_LINK5_WIDTH	1
#define GSWIPSS_IER1_LINK6_POS		6
#define GSWIPSS_IER1_LINK6_WIDTH	1
#define GSWIPSS_IER1_LINK7_POS		7
#define GSWIPSS_IER1_LINK7_WIDTH	1
#define GSWIPSS_IER1_LINK8_POS		8
#define GSWIPSS_IER1_LINK8_WIDTH	1
#define GSWIPSS_IER1_LINK9_POS		9
#define GSWIPSS_IER1_LINK9_WIDTH	1
#define GSWIPSS_IER1_LINK10_POS		10
#define GSWIPSS_IER1_LINK10_WIDTH	1
#define GSWIPSS_IER1_LINK11_POS		11
#define GSWIPSS_IER1_LINK11_WIDTH	1


#define GSWIPSS_ISR1_LINK2_POS		2
#define GSWIPSS_ISR1_LINK2_WIDTH	1
#define GSWIPSS_ISR1_LINK3_POS		3
#define GSWIPSS_ISR1_LINK3_WIDTH	1
#define GSWIPSS_ISR1_LINK4_POS		4
#define GSWIPSS_ISR1_LINK4_WIDTH	1
#define GSWIPSS_ISR1_LINK5_POS		5
#define GSWIPSS_ISR1_LINK5_WIDTH	1
#define GSWIPSS_ISR1_LINK6_POS		6
#define GSWIPSS_ISR1_LINK6_WIDTH	1
#define GSWIPSS_ISR1_LINK7_POS		7
#define GSWIPSS_ISR1_LINK7_WIDTH	1
#define GSWIPSS_ISR1_LINK8_POS		8
#define GSWIPSS_ISR1_LINK8_WIDTH	1
#define GSWIPSS_ISR1_LINK9_POS		9
#define GSWIPSS_ISR1_LINK9_WIDTH	1
#define GSWIPSS_ISR1_LINK10_POS		10
#define GSWIPSS_ISR1_LINK10_WIDTH	1
#define GSWIPSS_ISR1_LINK11_POS		11
#define GSWIPSS_ISR1_LINK11_WIDTH	1


#define CFG0_1588_REFTIME_POS		0
#define CFG0_1588_REFTIME_WIDTH		3
#define CFG0_1588_DIGTIME_POS		4
#define CFG0_1588_DIGTIME_WIDTH		3
#define CFG0_1588_BINTIME_POS		8
#define CFG0_1588_BINTIME_WIDTH		3
#define CFG0_1588_PPSSEL_POS		12
#define CFG0_1588_PPSSEL_WIDTH		3

#define CFG1_1588_SWTRIG_POS		7
#define CFG1_1588_SWTRIG_WIDTH		1
#define CFG1_1588_TRIG0SEL_POS		8
#define CFG1_1588_TRIG0SEL_WIDTH	4
#define CFG1_1588_TRIG1SEL_POS		12
#define CFG1_1588_TRIG1SEL_WIDTH	4


#define FREEZE				0
#define UNFREEZE			1

#define ETHERNET_FCS_SIZE		4
#define SPTAG_SIZE			8
#define TIMESTAMP80_SIZE		10

enum {
	XGMAC = 0,
	LMAC,
	ADAP,
	MACSEC,
	SWSS,
	LINK
};

enum {
	XGMAC2 = 0,
	XGMAC3,
	XGMAC4,
};

struct adap_prv_data;

struct adap_prv_data {
	u32 flags;
#ifdef __KERNEL__
	/* Adaption layer private data */
	void __iomem *ss_addr_base;
	spinlock_t adap_lock;
#else
	/* Adaption layer private data */
	u32 ss_addr_base;
#endif
	u32 core_en_cnt;
	struct adap_ops ops;
	/* Mac Cli */
	GSW_MAC_Cli_t *mac_cli;
};

static inline struct adap_prv_data *GET_ADAP_PDATA(void *pdev)
{
	struct adap_prv_data *pdata;

	struct adap_ops *adap_ops = (struct adap_ops *)pdev;
	pdata = container_of(adap_ops, struct adap_prv_data, ops);

	return pdata;
}

static inline u32 GSWSS_RGRD(struct adap_prv_data *pdata, u32 reg)
{
	u32 reg_val;
#if defined(PC_UTILITY) || defined(CHIPTEST)
	u32 reg_addr = pdata->ss_addr_base + reg;
#else
	void __iomem *reg_addr = (void __iomem *)pdata->ss_addr_base + reg;
#endif

#if defined(CHIPTEST) && CHIPTEST
	reg_val = REG32(reg_addr);
#endif
#if defined(PC_UTILITY) && PC_UTILITY
	pcuart_reg_rd(reg_addr, &reg_val);
#endif
#ifdef __KERNEL__
	reg_val = mac_r32(reg_addr);
#endif
	return reg_val;
}

static inline void GSWSS_RGWR(struct adap_prv_data *pdata, u32 reg, u32 val)
{
#if defined(PC_UTILITY) || defined(CHIPTEST)
	u32 reg_addr = pdata->ss_addr_base + reg;
#else
	void __iomem *reg_addr = (void __iomem *)pdata->ss_addr_base + reg;
#endif

#if defined(CHIPTEST) && CHIPTEST
	REG32(reg_addr) = val;
#endif
#if defined(PC_UTILITY) && PC_UTILITY
	pcuart_reg_wr(reg_addr, val);
#endif
#ifdef __KERNEL__
	mac_w32(val, reg_addr);
#endif
}

extern struct adap_prv_data adap_priv_data;

int gswss_mac_reset(void *pdev, u32 mod, u32 idx, u32 reset);
int gswss_set_interrupt(void *pdev, u32 mod, u32 idx, u32 enable);
int gswss_macsec_reset(void *pdev, u32 reset);
int gswss_set_nco(void *pdev, u32 val, u32 nco_idx);
int gswss_set_corese(void *pdev, u32 enable);
int gswss_switch_ss_reset(void *pdev);
int gswss_set_clkmode(void *pdev, u32 clk_mode);
int gswss_cfg0_1588(void *pdev, u32 ref_time, u32 dig_time, u32 bin_time,
		    u32 pps_sel);
int gswss_cfg1_1588(void *pdev, u32 trig0_sel, u32 trig1_sel, u32 sw_trig);

int gswss_set_macsec_to_mac(void *pdev, u32 mac_idx, u32 enable);

int gswss_get_int_stat(void *pdev, u32 mod);

int gswss_get_int_en_sts(void *pdev);
void gswss_test_all_reg(void *pdev);
void gswss_check_reg(void *pdev, u32 reg, char *name, int idx,
		     u16 set_val, u16 clr_val);
int gswss_get_nco(void *pdev, u32 nco_idx);
int gswss_get_cfg0_1588(void *pdev, u32 *ref_time, u32 *dig_time,
			u32 *bin_time, u32 *pps_sel);
int gswss_get_clkmode(void *pdev);
int gswss_get_macsec_to_mac(void *pdev);
int gswss_dbg_macsec_to_mac(void *pdev);
int gswss_get_corese(void *pdev);
int gswss_get_switch_ss_reset(void *pdev);
int gswss_get_macsec_reset(void *pdev);
int gswss_get_cfg1_1588(void *pdev, u32 *trig1_sel, u32 *trig0_sel, u32 *sw_trig);

void gswss_init_fn_ptrs(struct adap_ops *adap_ops);

#endif
