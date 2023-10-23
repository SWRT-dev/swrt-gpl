// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright (C) 2018 Intel Corporation.
 *  Zhu YiXin <Yixin.zhu@intel.com>
 */
#include <linux/clk-provider.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <dt-bindings/clock/intel,prx300-clk.h>

#include "clk-cgu-pll.h"
#include "clk-cgu.h"

#define PLL_DIV_WIDTH		4
#define PLL_DDIV_WIDTH		3

/* Register definition */
#define PLL0A_CFG0		0x4
#define PLL0B_CFG0		0x24
#define PLL1_CFG0		0x44
#define PLL2_CFG0		0x64
#define LJPLL3_CFG0		0x84
#define LJPLL4_CFG0		0xA4
#define LJPLL5_CFG0		0xC4
#define CGU_GATE0		0x114
#define CGU_GATE1		0x120
#define CGU_GATE2		0x130
#define CGU_IF_CLK		0x140
#define DPLL_CFG5		0x214

#define PLL_DIV(x)		((x) + 0x4)
#define PLL_SSC(x)		((x) + 0x10)

static const struct clk_div_table pll_div[] = {
	{1,	2},
	{2,	3},
	{3,	4},
	{4,	5},
	{5,	6},
	{6,	8},
	{7,	10},
	{8,	12},
	{9,	16},
	{10,	20},
	{11,	24},
	{12,	32},
	{13,	40},
	{14,	48},
	{15,	64},
	{0,	0}
};

enum prx300_plls {
	PLL0A, PLL0B, PLL1, PLL2, LJPLL3, LJPLL4, LJPLL5
};

PNAME(pll_p)	= { "osc" };
PNAME(emmc_p)	= { "emmc4", "noc4" };
PNAME(sdio_p)	= { "sdio3", "sdio2" };

static struct intel_clk_provider *early_ctx;
static const struct intel_clk_early_data prx300_early_clks[] __initconst = {
	{ PLL0A_CFG0, 0, 4, "earlycpu", pll_p, 0,
	  0, 0, PLL_DIV_WIDTH,
	  0, 0, PLL_DIV_WIDTH,
	  PLL_PFM_PRX300, TYPE_ROPLL, pll_div
	},
};

static const struct intel_pll_rate_table pll1_clk_vco[] __initconst = {
	{40000000, 1572864, 39, 1,  5395553,  1},
	{40000000, 160000, 16, 4,  0,  1},
	{ 0 },
};

static const struct intel_pll_rate_table ljpll3_clk_vco[] __initconst = {
	{40000000, 10000000, 250, 1,  0,  1},
	{40000000, 5000000, 125, 1,  0,  1},
	{ 0 },
};

static const struct intel_pll_rate_table ljpll4_clk_vco[] __initconst = {
	{40000000, 5000000, 125, 1,  0,  1},
	{40000000, 640000, 32, 2,  0,  1},
	{ 0 },
};

static const struct intel_pll_rate_table ljpll5_clk_vco[] __initconst = {
	{40000000, 5000000, 125, 1,  0,  1},
	{ 0 },
};

static const struct intel_pll_clk_data prx300_pll_clks[] __initconst = {
	[PLL0A] = INTEL_PLL(PRX300_CLK_PLL0A, PLL_PFM_PRX300, "pll0a", pll_p,
			    CLK_IGNORE_UNUSED, PLL0A_CFG0, NULL, TYPE_ROPLL),
	[PLL0B] = INTEL_PLL(PRX300_CLK_PLL0B, PLL_PFM_PRX300, "pll0b", pll_p,
			    CLK_IGNORE_UNUSED, PLL0B_CFG0, NULL, TYPE_ROPLL),
	[PLL1] = INTEL_PLL(PRX300_CLK_PLL1, PLL_PFM_PRX300, "pll1", pll_p,
			   CLK_IGNORE_UNUSED, PLL1_CFG0, pll1_clk_vco,
			   TYPE_ROPLL),
	[PLL2] = INTEL_PLL(PRX300_CLK_PLL2, PLL_PFM_PRX300, "pll2", pll_p,
			   CLK_IGNORE_UNUSED, PLL2_CFG0, NULL, TYPE_ROPLL),
	[LJPLL3] = INTEL_PLL(PRX300_CLK_LJPLL3, PLL_PFM_PRX300, "ljpll3", pll_p,
			     CLK_IGNORE_UNUSED, LJPLL3_CFG0, ljpll3_clk_vco,
			     TYPE_LJPLL),
	[LJPLL4] = INTEL_PLL(PRX300_CLK_LJPLL4, PLL_PFM_PRX300, "ljpll4", pll_p,
			     CLK_IGNORE_UNUSED, LJPLL4_CFG0, ljpll4_clk_vco,
			     TYPE_LJPLL),
	[LJPLL5] = INTEL_PLL(PRX300_CLK_LJPLL5, PLL_PFM_PRX300, "ljpll5", pll_p,
			     CLK_IGNORE_UNUSED, LJPLL5_CFG0, ljpll5_clk_vco,
			     TYPE_LJPLL),
};

static const struct intel_clk_branch prx300_branch_clks[] __initconst = {
	/* Divider clocks */
	INTEL_DIV(PRX300_CLK_CPU, "cpu", "pll0a", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL0A_CFG0), 0, PLL_DIV_WIDTH, 24, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_QSPI, "qspi", "pll0a", 0, PLL_DIV(PLL0A_CFG0),
		  12, PLL_DIV_WIDTH, 27, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_SW, "sw", "pll0b", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL0B_CFG0), 0, PLL_DIV_WIDTH, 24, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_SSX4, "ssx4", "pll0b", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL0B_CFG0), 4, PLL_DIV_WIDTH, 25, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_NGI, "ngi", "pll0b", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL0B_CFG0), 8, PLL_DIV_WIDTH, 26, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_DSP, "dsp", "pll1", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL1_CFG0), 0, PLL_DIV_WIDTH, 24, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_IF, "voice_if_clk", "pll1", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL1_CFG0), 4, PLL_DIV_WIDTH, 25, 1, 0, 0, pll_div),
	INTEL_DIV(PRX300_CLK_DDR, "ddr", "pll2", CLK_IGNORE_UNUSED,
		  PLL_DIV(PLL2_CFG0), 0, PLL_DIV_WIDTH, 26, 1, 0, 0, pll_div),

	/* Fixed Factor */
	INTEL_FIXED_FACTOR(PRX300_CLK_PONDEF, "pondef", "dd_pool",
			   CLK_SET_RATE_PARENT, 0, 0, 0, 0, 0, 1, 2),

	/* Gate0 clocks */
	INTEL_GATE(PRX300_GCLK_XBAR0, "g_xbar0", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE0, 0, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_XBAR1, "g_xbar1", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE0, 1, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_XBAR7, "g_xbar7", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE0, 7, GATE_CLK_HW, 0),

	/* Gate1 clocks */
	INTEL_GATE(PRX300_GCLK_V_CODEC, "g_code", NULL, 0, CGU_GATE1,
		   2, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_DMA0, "g_dma0", NULL, 0, CGU_GATE1,
		   3, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_I2C0, "g_i2c0", NULL, 0, CGU_GATE1,
		   4, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_I2C1, "g_i2c1", NULL, 0, CGU_GATE1,
		   5, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_I2C2, "g_i2c2", NULL, 0, CGU_GATE1,
		   6, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_SPI1, "g_spi1", NULL, 0, CGU_GATE1,
		   7, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_SPI0, "g_spi0", NULL, 0, CGU_GATE1,
		   8, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_QSPI, "g_qspi", NULL, 0, CGU_GATE1,
		   9, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_CQEM, "g_cqem", NULL, 0, CGU_GATE1,
		   10, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_SSO, "g_sso", NULL, 0, CGU_GATE1,
		   11, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_GPTC0, "g_gptc0", NULL, 0, CGU_GATE1,
		   12, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_GPTC1, "g_gptc1", NULL, 0, CGU_GATE1,
		   13, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_GPTC2, "g_gptc2", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE1, 14, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_URT0, "g_uart0", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE1, 17, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_URT1, "g_uart1", NULL, 0, CGU_GATE1,
		   18, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_SECPT, "g_secpt", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE1, 21, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_SCPU, "g_scpu", NULL, CLK_IGNORE_UNUSED,
		   CGU_GATE1, 22, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_MPE, "g_mpe", NULL, 0, CGU_GATE1,
		   23, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_TDM, "g_tdm", NULL, 0, CGU_GATE1,
		   25, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_PP, "g_pp", NULL, 0, CGU_GATE1,
		   26, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_DMA3, "g_dma3", NULL, 0, CGU_GATE1,
		   27, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_SWITCH, "g_switch", NULL, 0, CGU_GATE1,
		   28, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_PON, "g_pon", "pondef", 0, CGU_GATE1,
		   29, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_AON, "g_aon", "pon_phy",
		   CLK_SET_RATE_PARENT, CGU_GATE1,
		   30, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_DDR, "g_ddr", NULL, CLK_IGNORE_UNUSED, CGU_GATE1,
		   31, GATE_CLK_HW, 0),

	/* Gate2 clock */
	INTEL_GATE(PRX300_GCLK_PCIE_CTRL0, "pcie0", "pcie", CLK_SET_RATE_PARENT,
		   CGU_GATE2, 1, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_MSI0, "g_msi0", NULL, 0, CGU_GATE2,
		   2, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_PD0, "g_pd0", NULL, 0, CGU_GATE2,
		   7, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_PCIE_CTRL1, "pcie1", "pcie", CLK_SET_RATE_PARENT,
		   CGU_GATE2, 17, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_MSI1, "g_msi1", NULL, 0, CGU_GATE2,
		   18, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_PD1, "g_pd1", NULL, 0, CGU_GATE2,
		   23, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_ASPI, "g_aspi", NULL, 0, CGU_GATE2,
		   28, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_ADMA, "g_adma", NULL, 0, CGU_GATE2,
		   29, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_AHIF, "g_ashif", NULL, 0, CGU_GATE2,
		   30, GATE_CLK_HW, 0),
	INTEL_GATE(PRX300_GCLK_ASL, "g_asl", NULL, 0, CGU_GATE2,
		   31, GATE_CLK_HW, 0),

	/* Gate3 clock */
	INTEL_GATE(PRX300_GCLK_SWREF, "g_swref", "g_aon",
		   CLK_SET_RATE_PARENT, CGU_IF_CLK,
		   4, GATE_CLK_SW, 0),
	INTEL_GATE(PRX300_GCLK_CBPHY0, "cbphy0", NULL, 0, CGU_IF_CLK,
		   24, GATE_CLK_SW, 0),
	INTEL_GATE(PRX300_GCLK_CBPHY1, "cbphy1", NULL, 0, CGU_IF_CLK,
		   25, GATE_CLK_SW, 0),

	/* Miscellaneous clocks */
	INTEL_FIXED(PRX300_CLK_SLIC, "slic", NULL, 0, CGU_IF_CLK,
		    14, 2, CLOCK_FLAG_VAL_INIT, 8192000, 2),
};

static const struct intel_clk_ddiv_data prx300_ddiv_clks[] __initconst = {
	INTEL_DDIV(PRX300_CLK_GPHY, "gphy", "ljpll3", 0,
		   PLL_DIV(LJPLL3_CFG0), 0, PLL_DDIV_WIDTH,
		   3, PLL_DDIV_WIDTH, 24, 1, 29, 0),
	INTEL_DDIV(PRX300_CLK_CBPHY, "cbphy", "ljpll3", 0,
		   PLL_DIV(LJPLL3_CFG0), 6, PLL_DDIV_WIDTH,
		   9, PLL_DDIV_WIDTH, 25, 1, 28, 0),
	INTEL_DDIV(PRX300_CLK_POOL, "dd_pool", "ljpll3", 0,
		   PLL_DIV(LJPLL3_CFG0), 12, PLL_DDIV_WIDTH,
		   15, PLL_DDIV_WIDTH, 26, 1, 28, 0),
	INTEL_DDIV(PRX300_CLK_PTP, "dd_ptp", "ljpll3", 0,
		   PLL_DIV(LJPLL3_CFG0), 18, PLL_DDIV_WIDTH,
		   21, PLL_DDIV_WIDTH, 27, 1, 28, 0),
	INTEL_DDIV(PRX300_CLK_PCIE, "pcie", "ljpll4", 0,
		   PLL_DIV(LJPLL4_CFG0), 0, PLL_DDIV_WIDTH,
		   3, PLL_DDIV_WIDTH, 24, 1, 29, 0),
	INTEL_DDIV(PRX300_CLK_PONPHY, "pon_phy", "ljpll5", 0,
		   PLL_DIV(LJPLL5_CFG0), 0, PLL_DDIV_WIDTH,
		   3, PLL_DDIV_WIDTH, 24, 1, 29, 0),
	INTEL_DDIV(PRX300_CLK_PONIP, "pon_ip", "ljpll5", 0,
		   PLL_DIV(LJPLL5_CFG0), 6, PLL_DDIV_WIDTH,
		   9, PLL_DDIV_WIDTH, 25, 1, 28, 0),
};

static void __init prx300_clk_init(struct device_node *np)
{
	struct regmap *map;

	map = syscon_node_to_regmap(np);
	if (IS_ERR(map))
		return;

	early_ctx = intel_clk_init(map, ARRAY_SIZE(prx300_early_clks));

	if (IS_ERR(early_ctx)) {
		pr_err("%s: early clock alloc ctx failed!\n", __func__);
		regmap_exit(map);
		return;
	}

	if (intel_clk_register_early(early_ctx, prx300_early_clks,
				     ARRAY_SIZE(prx300_early_clks))) {
		kfree(early_ctx);
		return;
	}
	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, &early_ctx->clk_data);
}

CLK_OF_DECLARE_DRIVER(intel_prx300_cgu, "intel,cgu-prx300",
		      prx300_clk_init);

static int __init intel_prx300_cgu_probe(struct platform_device *pdev)
{
	struct intel_clk_provider *ctx;
	struct regmap *map;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;

	if (!np)
		return -ENODEV;

	map = syscon_node_to_regmap(np);
	if (IS_ERR(map))
		return PTR_ERR(map);

	ctx = intel_clk_init(map, PRX300_CLK_NR_CLKS);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ctx->np = np;
	ctx->dev = dev;
	platform_set_drvdata(pdev, ctx);

	intel_clk_register_plls(ctx, prx300_pll_clks,
				ARRAY_SIZE(prx300_pll_clks));
	/* PON FW enables the sync (DPLL_CFG5 register configs),
	 * After soft reboot ETH link between SFP and Paragon-X cannot be
	 * established anymore only power-cycle of the SFP helps.
	 * CGU driver needs to reset DPLL_CFG5 register and re-init the LJPLL3
	 */
	intel_set_clk_val(map, DPLL_CFG5, 0, 31, 0);
	intel_clk_plls_parse_vco_config(ctx, prx300_pll_clks,
					ARRAY_SIZE(prx300_pll_clks));
	intel_clk_register_branches(ctx, prx300_branch_clks,
				    ARRAY_SIZE(prx300_branch_clks));
	intel_clk_register_ddiv(ctx, prx300_ddiv_clks,
				ARRAY_SIZE(prx300_ddiv_clks));
	of_clk_add_hw_provider(np, of_clk_hw_onecell_get, &ctx->clk_data);

	intel_clk_ssc_probe(ctx);

	return 0;
}

static const struct of_device_id of_intel_prx300_cgu_match[] = {
	{ .compatible = "intel,cgu-prx300" },
	{}
};

static struct platform_driver intel_prx300_cgu_driver __refdata = {
	.probe  = intel_prx300_cgu_probe,
	.driver = {
		.name = "cgu-prx300",
		.of_match_table = of_match_ptr(of_intel_prx300_cgu_match),
	},
};

static int __init intel_prx300_cgu_init(void)
{
	return platform_driver_register(&intel_prx300_cgu_driver);
}
arch_initcall(intel_prx300_cgu_init);

