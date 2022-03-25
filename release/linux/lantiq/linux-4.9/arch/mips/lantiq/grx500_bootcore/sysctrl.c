/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011-2012 John Crispin <blogic@openwrt.org>
 *  Copyright (C) 2014 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 */
#include <linux/ioport.h>
#include <linux/export.h>
#include <linux/clkdev.h>
#include <linux/spinlock.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#include <lantiq_soc.h>

#include "clk.h"
#include "prom.h"

/* register access macros for EBU and CGU */
#define ltq_ebu_w32(x, y)	ltq_w32((x), ltq_ebu_membase + (y))
#define ltq_ebu_r32(x)		ltq_r32(ltq_ebu_membase + (x))
#define ltq_ebu_w32_mask(x, y, z) \
		ltq_w32_mask(x, y, ltq_ebu_membase + (z))

/* First register set */
#define PMU_CLK_SR1	0x120 /* status */
#define PMU_CLK_CR1_A	0x124 /* Enable */
#define PMU_CLK_CR1_B	0x128 /* Disable */
/* Second register set */
#define PMU_CLK_SR2	0x130 /* status */
#define PMU_CLK_CR2_A	0x134 /* Enable */
#define PMU_CLK_CR2_B	0x138 /* Disable */

/* clock gates that we can en/disable */
/* Register 0 /module 0 */
#define PMU_VCODEC	BIT(2)
#define PMU_DMA		BIT(5)
#define PMU_USB0	BIT(6)
#define PMU_SPI1	BIT(7)
#define PMU_SPI0	BIT(8)
#define PMU_CBM		BIT(9)
#define PMU_EBU		BIT(10)
#define PMU_LEDC	BIT(11)
#define PMU_GPTC0	BIT(12)
#define PMU_GPTC1	BIT(13)
#define PMU_GPTC2	BIT(14)
#define PMU_ASC0	BIT(17) /* ASC1 enabled by default */
#define PMU_EIP97	BIT(20)
#define PMU_EIP123	BIT(21)
#define PMU_TOE		BIT(22)
#define PMU_MPE		BIT(23)
#define PMU_TDM		BIT(25)
#define PMU_SWITCH_R	BIT(26)
#define PMU_USB1	BIT(27)
#define PMU_SWITCH_L	BIT(28)

#define PMU_MODULE0_ALL	0x1EF27FE4

/* Register 1 /module 1*/
#define PMU_PCIE_CTRL0	BIT(1)
#define PMU_PCIE_CTRL1	BIT(17)
#define PMU_PCIE_CTRL2	BIT(25)

#define PMU_MODULE1_ALL	0x02020002

#define PMU_REG_MODULE_0	0
#define PMU_REG_MODULE_1	1

#define pmu_w32(x, y)	ltq_w32((x), pmu_membase + (y))
#define pmu_r32(x)	ltq_r32(pmu_membase + (x))

static void __iomem *pmu_membase;
void __iomem *ltq_ebu_membase;
#if 0
/* Status */
static u32 pmu_clk_sr[] = {
	PMU_CLK_SR1,
	PMU_CLK_SR2,
};

/* Enable */
static u32 pmu_clk_cr_a[] = {
	PMU_CLK_CR1_A,
	PMU_CLK_CR2_A,
};

/* Disable */
static u32 pmu_clk_cr_b[] = {
	PMU_CLK_CR1_B,
	PMU_CLK_CR2_B,
};

/* power control register */
#define PWDCR_EN_XRX(x)  (pmu_clk_cr_a[(x)])
#define PWDCR_DIS_XRX(x)  (pmu_clk_cr_b[(x)])
/* power status register */
#define PWDSR_XRX(x)  (pmu_clk_sr[(x)])

/* enable a clock gate */
static int pmu_enable(struct clk *clk)
{
	int retry = 1000000;

	if (!clk->always_on) {
		pr_debug("%s module %d position 0x%08x always on\n",
			__func__, clk->module, clk->bits);
		return 0;
	}

	if (atomic_read(&clk->refcount) == 0) {
		pmu_w32(clk->bits, PWDCR_EN_XRX(clk->module));
		do {} while (--retry
			&& (!(pmu_r32(PWDSR_XRX(clk->module)) & clk->bits)));
		
		if (!retry)
			panic("Activating PMU module failed!\n");
		pr_debug("%s module %d position 0x%08x enabled\n",
			__func__, clk->module, clk->bits);

	} else {
		pr_debug("%s module %d position 0x%08x already enabled\n",
			__func__, clk->module, clk->bits);
	}
	atomic_inc(&clk->refcount);
	return 0;
}

/* disable a clock gate */
static void pmu_disable(struct clk *clk)
{
	int retry = 1000000;

	if (!clk->always_on) {
		pr_debug("%s module %d position 0x%08x always on\n",
			__func__, clk->module, clk->bits);
		return;
	}

	/* Consider disabling the module when powering on the system */
	if (atomic_read(&clk->refcount) > 0)
		atomic_dec(&clk->refcount);

	if (atomic_read(&clk->refcount) == 0) {
		pmu_w32(clk->bits, PWDCR_DIS_XRX(clk->module));
		do {} while (--retry
			&& (pmu_r32(PWDSR_XRX(clk->module)) & clk->bits));

		if (!retry)
			panic("activating PMU module failed!\n");
		pr_debug("%s module %d position 0x%08x disabled\n",
			__func__, clk->module, clk->bits);
	} else {
		pr_debug("%s module %d position 0x%08x still used\n",
			__func__, clk->module, clk->bits);
	}
}


/* manage the clock gates via PMU */
static void clkdev_add_pmu(const char *dev, const char *con,
		bool used, unsigned int module, unsigned int bits)
{
	struct clk *clk = kzalloc(sizeof(struct clk), GFP_KERNEL);

	clk->cl.dev_id = dev;
	clk->cl.con_id = con;
	clk->cl.clk = clk;
	clk->always_on = used;
	clk->enable = pmu_enable;
	clk->disable = pmu_disable;
	clk->module = module;
	clk->bits = bits;
	atomic_set(&clk->refcount, 0);
	/* Disable it during the initialitin. Module should enable when used */
	pmu_disable(clk);
	clkdev_add(&clk->cl);
}
#endif
/* bring up all register ranges that we need for basic system control */
void __init ltq_soc_init(void)
{
	struct resource res_pmu, res_ebu;
	struct device_node *np_pmu =
		of_find_compatible_node(NULL, NULL, "lantiq,pmu-grx500");
	struct device_node *np_ebu =
		of_find_compatible_node(NULL, NULL, "lantiq,ebu-grx500");
	struct clk *clk = NULL;

	/* check if all the core register ranges are available */
	if (!np_pmu || !np_ebu)
#ifdef CONFIG_SOC_PRX300_BOOTCORE
		/* no ebu or pmu setting on prx300 */
		goto set_clk;
#else /* CONFIG_SOC_PRX300_BOOTCORE */
		panic("Failed to load core nodes from devicetree");
#endif /* CONFIG_SOC_PRX300_BOOTCORE */

	if (of_address_to_resource(np_pmu, 0, &res_pmu) ||
		of_address_to_resource(np_ebu, 0, &res_ebu))
		panic("Failed to get core resources");

	if ((request_mem_region(res_pmu.start, resource_size(&res_pmu),
		res_pmu.name) < 0) ||
		(request_mem_region(res_ebu.start, resource_size(&res_ebu),
				res_ebu.name) < 0))
		pr_err("Failed to request core reources");
	pmu_membase = ioremap_nocache(res_pmu.start, resource_size(&res_pmu));
	ltq_ebu_membase = ioremap_nocache(res_ebu.start,
		resource_size(&res_ebu));
	if (!pmu_membase || !ltq_ebu_membase)
		panic("Failed to remap core resources");

	ltq_ebu_w32(ltq_ebu_r32(LTQ_EBU_BUSCON0) & ~EBU_WRDIS,
			LTQ_EBU_BUSCON0);

#ifdef CONFIG_SOC_PRX300_BOOTCORE
	set_clk:
#endif /* CONFIG_SOC_PRX300_BOOTCORE */

		clk = kzalloc(sizeof(struct clk), GFP_KERNEL);
		if (clk) {
			clk->cl.dev_id = "cpu";
			clk->cl.con_id = "cpu";
			clk->cl.clk = clk;
			clk->rate = ltq_grx500_cpu_hz();
			clk->set_rate = ltq_grx500_set_cpu_hz;
			clk->get_rate = ltq_grx500_cpu_hz;
			clkdev_add(&clk->cl);
		}
#if 0
	/* add our generic grx500 clocks */
	clkdev_add_pmu(NULL, "vcodec", 1, PMU_REG_MODULE_0, PMU_VCODEC);
	clkdev_add_pmu("16e00000.dma", NULL, 1, PMU_REG_MODULE_0, PMU_DMA);
	clkdev_add_pmu("1a300000.usb", NULL, 1, PMU_REG_MODULE_0, PMU_USB0);
	clkdev_add_pmu("16900000.spi", NULL, 1, PMU_REG_MODULE_0, PMU_SPI1);
	clkdev_add_pmu("16800000.spi", NULL, 1, PMU_REG_MODULE_0, PMU_SPI0);
	clkdev_add_pmu("1e500000.cbm", NULL, 1, PMU_REG_MODULE_0, PMU_CBM);
	clkdev_add_pmu("16f00000.ebu", NULL, 0, PMU_REG_MODULE_0, PMU_EBU);

	clkdev_add_pmu("16d00000.stp", NULL, 1, PMU_REG_MODULE_0, PMU_LEDC);
	clkdev_add_pmu("16b00000.gptu", NULL, 1, PMU_REG_MODULE_0, PMU_GPTC0);
	clkdev_add_pmu("16300000.gptu", NULL, 1, PMU_REG_MODULE_0, PMU_GPTC1);
	clkdev_add_pmu("16400000.gptu", NULL, 1, PMU_REG_MODULE_0, PMU_GPTC2);
	clkdev_add_pmu("16700000.serial", NULL, 1, PMU_REG_MODULE_0, PMU_ASC0);
	clkdev_add_pmu("1e100000.eip97", NULL, 1, PMU_REG_MODULE_0, PMU_EIP97);
	clkdev_add_pmu("1e000000.eip123", NULL, 1, PMU_REG_MODULE_0,
		PMU_EIP123);
	clkdev_add_pmu("a2000000.toe", NULL, 1, PMU_REG_MODULE_0, PMU_TOE);
	clkdev_add_pmu("a0000000.mpe", NULL, 1, PMU_REG_MODULE_0, PMU_MPE);
	clkdev_add_pmu("16500000.tdm", NULL, 1, PMU_REG_MODULE_0, PMU_TDM);
	clkdev_add_pmu("1a000000.switch", NULL, 1, PMU_REG_MODULE_0,
		PMU_SWITCH_R);
	clkdev_add_pmu("1a600000.usb", NULL, 1, PMU_REG_MODULE_0, PMU_USB1);
	clkdev_add_pmu("1c000000.switch", NULL, 1, PMU_REG_MODULE_0,
		PMU_SWITCH_L);

	clkdev_add_pmu("18900000.pcie", NULL, 1, PMU_REG_MODULE_1,
		PMU_PCIE_CTRL0);
	clkdev_add_pmu("18400000.pcie", NULL, 1, PMU_REG_MODULE_1,
		PMU_PCIE_CTRL1);
	clkdev_add_pmu("18e00000.pcie", NULL, 1, PMU_REG_MODULE_1,
		PMU_PCIE_CTRL2);
#endif
}
