/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 * Copyright (C) 2013 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 */

#ifndef _LTQ_CLK_H__
#define _LTQ_CLK_H__

#include <linux/clkdev.h>

/* clock speeds */
#define CLOCK_16M	16000000
#define CLOCK_33M	33333333
#define CLOCK_50M	50000000
#define CLOCK_60M	60000000
#define CLOCK_62_5M	62500000
#define CLOCK_83M	83333333
#define CLOCK_83_5M	83500000
#define CLOCK_98_304M	98304000
#define CLOCK_100M	100000000
#define CLOCK_111M	111111111
#define CLOCK_125M	125000000
#define CLOCK_133M	133333333
#define CLOCK_150M	150000000
#define CLOCK_166M	166666666
#define CLOCK_167M	166666667
#define CLOCK_196_608M	196608000
#define CLOCK_200M	200000000
#define CLOCK_222M	222000000
#define CLOCK_240M	240000000
#define CLOCK_250M	250000000
#define CLOCK_266M	266666666
#define CLOCK_288M	288888888
#define CLOCK_300M	300000000
#define CLOCK_333M	333333333
#define CLOCK_360M	360000000
#define CLOCK_393M	393215332
#define CLOCK_400M	400000000
#define CLOCK_432M	432000000
#define CLOCK_450M	450000000
#define CLOCK_500M	500000000
#define CLOCK_600M	600000000
#define CLOCK_666M	666666666
#define CLOCK_720M	720000000
#define CLOCK_800M	800000000
#define CLOCK_1000M	1000000000
#define CLOCK_2000M	2000000000UL
#define CLOCK_2400M	2400000000UL

/* clock out speeds */
#define CLOCK_32_768K	32768
#define CLOCK_1_536M	1536000
#define CLOCK_2_5M	2500000
#define CLOCK_12M	12000000
#define CLOCK_24M	24000000
#define CLOCK_25M	25000000
#define CLOCK_30M	30000000
#define CLOCK_40M	40000000
#define CLOCK_48M	48000000
#define CLOCK_50M	50000000
#define CLOCK_60M	60000000

/* clock control register for legacy */
#define CGU_IFCCR	0x0018
#define CGU_IFCCR_VR9	0x0024
/* system clock register for legacy */
#define CGU_SYS		0x0010
/* pci control register */
#define CGU_PCICR	0x0034
#define CGU_PCICR_VR9	0x0038
/* ephy configuration register */
#define CGU_EPHY	0x10

/* Legacy PMU register for ar9, ase, danube */
/* power control register */
#define PMU_PWDCR	0x1C
/* power status register */
#define PMU_PWDSR	0x20
/* power control register */
#define PMU_PWDCR1	0x24
/* power status register */
#define PMU_PWDSR1	0x28
/* power control register */
#define PWDCR(x) ((x) ? (PMU_PWDCR1) : (PMU_PWDCR))
/* power status register */
#define PWDSR(x) ((x) ? (PMU_PWDSR1) : (PMU_PWDSR))


/* PMU register for ar10 and grx390 */

/* First register set */
#define PMU_CLK_SR	0x20 /* status */
#define PMU_CLK_CR_A	0x24 /* Enable */
#define PMU_CLK_CR_B	0x28 /* Disable */
/* Second register set */
#define PMU_CLK_SR1	0x30 /* status */
#define PMU_CLK_CR1_A	0x34 /* Enable */
#define PMU_CLK_CR1_B	0x38 /* Disable */
/* Third register set */
#define PMU_ANA_SR	0x40 /* status */
#define PMU_ANA_CR_A	0x44 /* Enable */
#define PMU_ANA_CR_B	0x48 /* Disable */

#define PWDCR_EN_XRX(x)		(pmu_clk_cr_a[(x)])
#define PWDCR_DIS_XRX(x)	(pmu_clk_cr_b[(x)])
#define PWDSR_XRX(x)		(pmu_clk_sr[(x)])

/* clock gates that we can en/disable */
#define PMU_USB0_P	BIT(0)
#define PMU_ASE_SDIO	BIT(2) /* ASE special */
#define PMU_PCI		BIT(4)
#define PMU_DMA		BIT(5)
#define PMU_USB0	BIT(6)
#define PMU_ASC0	BIT(7)
#define PMU_EPHY	BIT(7)	/* ase */
#define PMU_USIF	BIT(7) /* from vr9 until grx390 */
#define PMU_SPI		BIT(8)
#define PMU_DFE		BIT(9)
#define PMU_EBU		BIT(10)
#define PMU_STP		BIT(11)
#define PMU_GPT		BIT(12)
#define PMU_AHBS	BIT(13) /* vr9 */
#define PMU_FPI		BIT(14)
#define PMU_AHBM	BIT(15)
#define PMU_SDIO	BIT(16) /* danube, ar9, vr9 */
#define PMU_ASC1	BIT(17)
#define PMU_PPE_QSB	BIT(18)
#define PMU_PPE_SLL01	BIT(19)
#define PMU_DEU		BIT(20)
#define PMU_PPE_TC	BIT(21)
#define PMU_PPE_EMA	BIT(22)
#define PMU_PPE_DPLUM	BIT(23)
#define PMU_PPE_DP	BIT(23)
#define PMU_PPE_DPLUS	BIT(24)
#define PMU_USB1_P	BIT(26)
#define PMU_USB1	BIT(27)
#define PMU_SWITCH	BIT(28)
#define PMU_PPE_TOP	BIT(29)
#define PMU_GPHY	BIT(30)
#define PMU_PCIE_CLK	BIT(31)

#define PMU1_PCIE_PHY	BIT(0)	/* vr9-specific,moved in ar10/grx390 */
#define PMU1_PCIE_CTL	BIT(1)
#define PMU1_PCIE_PDI	BIT(4)
#define PMU1_PCIE_MSI	BIT(5)
#define PMU1_CKE	BIT(6)
#define PMU1_PCIE1_CTL	BIT(17)
#define PMU1_PCIE1_PDI	BIT(20)
#define PMU1_PCIE1_MSI	BIT(21)
#define PMU1_PCIE2_CTL	BIT(25)
#define PMU1_PCIE2_PDI	BIT(26)
#define PMU1_PCIE2_MSI	BIT(27)

#define PMU_ANALOG_USB0_P	BIT(0)
#define PMU_ANALOG_USB1_P	BIT(1)
#define PMU_ANALOG_PCIE0_P	BIT(8)
#define PMU_ANALOG_PCIE1_P	BIT(9)
#define PMU_ANALOG_PCIE2_P	BIT(10)
#define PMU_ANALOG_DSL_AFE	BIT(16)
#define PMU_ANALOG_DCDC_2V5	BIT(17)
#define PMU_ANALOG_DCDC_1VX	BIT(18)
#define PMU_ANALOG_DCDC_1V0	BIT(19)

#define pmu_w32(x, y)	ltq_w32((x), pmu_membase + (y))
#define pmu_r32(x)	ltq_r32(pmu_membase + (x))

#define XBAR_ALWAYS_LAST	0x430
#define XBAR_FPI_BURST_EN	BIT(1)
#define XBAR_AHB_BURST_EN	BIT(2)

#define xbar_w32(x, y)	ltq_w32((x), ltq_xbar_membase + (y))
#define xbar_r32(x)	ltq_r32(ltq_xbar_membase + (x))

enum {
	STATIC_CPU_CLK = 1,
	STATIC_FPI_CLK,
	STATIC_IO_CLK,
	STATIC_PPE_CLK,
	STATIC_NO_PARENT = 0xff,
};

struct vol_sg {/*voltage speed-grade*/
	unsigned int slow[4];
	unsigned int typ[4];
	unsigned int fast[4];
};

struct clk_rates {
	unsigned int cpu_freq;
	unsigned int ddr_freq;
	unsigned int cpu_clkm_sel;
	unsigned int pll_clk1;
	unsigned int pll_clk2;
	unsigned int pll_clk4;
	struct vol_sg core_vol350;
	struct vol_sg core_vol550;
};

enum spd_class_t {
	FAST0 = 0,
	FAST1,
	FAST2,
	FAST3,
	MEDIUM0,
	MEDIUM1,
	MEDIUM2,
	MEDIUM3,
	SLOW0,
	SLOW1,
	SLOW2,
	SLOW3,
	SLOW_DEFAULT,
	UNDEF
};

struct clk {
	struct clk_lookup cl;
	unsigned long rate;
	unsigned long *rates;
	unsigned int module;
	unsigned int bits;
	unsigned long (*get_rate) (void);
	int (*enable) (struct clk *clk);
	void (*disable) (struct clk *clk);
	int (*activate) (struct clk *clk);
	void (*deactivate) (struct clk *clk);
	void (*reboot) (struct clk *clk);
};

extern void clkdev_add_static(unsigned long cpu, unsigned long fpi,
				unsigned long io, unsigned long ppe);

extern unsigned long ltq_danube_cpu_hz(void);
extern unsigned long ltq_danube_fpi_hz(void);
extern unsigned long ltq_danube_pp32_hz(void);

extern unsigned long ltq_ar9_cpu_hz(void);
extern unsigned long ltq_ar9_fpi_hz(void);

extern unsigned long ltq_vr9_cpu_hz(void);
extern unsigned long ltq_vr9_fpi_hz(void);
extern unsigned long ltq_vr9_pp32_hz(void);

extern unsigned long ltq_ar10_cpu_hz(void);
extern unsigned long ltq_ar10_fpi_hz(void);
extern unsigned long ltq_ar10_pp32_hz(void);

extern unsigned long ltq_grx500_cpu_hz(void);
extern unsigned long ltq_grx500_fpi_hz(void);
extern unsigned long ltq_grx500_pp32_hz(void);

extern unsigned long ltq_grx500_cpu_hz(void);
extern int ltq_grx500_set_cpu_hz(unsigned long cpu_freq);
extern unsigned long ltq_grx500_cpu_vol(unsigned long *rate);

extern unsigned long ltq_grx500_fpi_hz(void);
extern int ltq_grx500_set_fpi_hz(unsigned long fpi_freq);

extern unsigned long ltq_grx500_cbm_hz(void);
extern int ltq_grx500_set_cbm_hz(unsigned long cbm_freq);

extern unsigned long ltq_grx500_ngi_hz(void);
extern int ltq_grx500_set_ngi_hz(unsigned long ngi_freq);

extern unsigned long ltq_grx500_ddr_hz(void);

extern unsigned long *ltq_get_avail_scaling_rates(int sel);
extern unsigned long ltq_grx500_cpu_vol(unsigned long *rate);
extern int ltq_grx500_get_speed_grade(void);
extern void ltq_grx500_set_speed_grade(int spg);
#endif
