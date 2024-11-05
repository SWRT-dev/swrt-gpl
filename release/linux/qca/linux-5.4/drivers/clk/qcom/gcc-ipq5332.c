// SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause)
/*
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk-provider.h>
#include <linux/regmap.h>

#include <linux/reset-controller.h>
#include <dt-bindings/clock/qcom,gcc-ipq5332.h>
#include <dt-bindings/reset/qcom,gcc-ipq5332.h>

#include "common.h"
#include "clk-regmap.h"
#include "clk-pll.h"
#include "clk-rcg.h"
#include "clk-branch.h"
#include "clk-alpha-pll.h"
#include "clk-regmap-divider.h"
#include "clk-regmap-mux.h"
#include "reset.h"

enum {
	P_PCIE3X2_PIPE,
	P_PCIE3X1_0_PIPE,
	P_PCIE3X1_1_PIPE,
	P_USB3PHY_0_PIPE,
	P_CORE_BI_PLL_TEST_SE,
	P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC,
	P_GPLL0_OUT_AUX,
	P_GPLL0_OUT_MAIN,
	P_GPLL2_OUT_AUX,
	P_GPLL2_OUT_MAIN,
	P_GPLL4_OUT_AUX,
	P_GPLL4_OUT_MAIN,
	P_SLEEP_CLK,
	P_XO,
};

static struct clk_alpha_pll gpll0_main = {
	.offset = 0x20000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_GCC_PLL_TYPE_STROMER_PLUS],
	.clkr = {
		.enable_reg = 0xb000,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gpll0_main",
			.parent_data = &(const struct clk_parent_data){
				.fw_name = "xo",
			},
			.num_parents = 1,
			.ops = &clk_alpha_pll_stromer_plus_ops,
			.flags = CLK_IS_CRITICAL,
		},
	},
};

static struct clk_fixed_factor gpll0_div2 = {
	.mult = 1,
	.div = 2,
	.hw.init = &(struct clk_init_data){
		.name = "gpll0_div2",
		.parent_hws = (const struct clk_hw *[]){
				&gpll0_main.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static struct clk_alpha_pll_postdiv gpll0 = {
	.offset = 0x20000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_GCC_PLL_TYPE_STROMER_PLUS],
	.width = 4,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll0",
		.parent_hws = (const struct clk_hw *[]){
				&gpll0_main.clkr.hw },
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ro_ops,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static struct clk_alpha_pll gpll2_main = {
	.offset = 0x21000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_GCC_PLL_TYPE_STROMER_PLUS],
	.clkr = {
		.enable_reg = 0xb000,
		.enable_mask = BIT(1),
		.hw.init = &(const struct clk_init_data){
			.name = "gpll2",
			.parent_data = &(const struct clk_parent_data){
				.fw_name = "xo",
			},
			.num_parents = 1,
			.ops = &clk_alpha_pll_stromer_plus_ops,
		},
	},
};

static struct clk_alpha_pll_postdiv gpll2 = {
	.offset = 0x21000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_GCC_PLL_TYPE_STROMER_PLUS],
	.width = 4,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll2_main",
		.parent_hws = (const struct clk_hw *[]){
				&gpll2_main.clkr.hw },
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ro_ops,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static struct clk_alpha_pll gpll4_main = {
	.offset = 0x22000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_GCC_PLL_TYPE_STROMER_PLUS],
	.clkr = {
		.enable_reg = 0xb000,
		.enable_mask = BIT(2),
		.hw.init = &(const struct clk_init_data){
			.name = "gpll4_main",
			.parent_data = &(const struct clk_parent_data){
				.fw_name = "xo",
			},
			.num_parents = 1,
			.ops = &clk_alpha_pll_stromer_plus_ops,
		},
	},
};

static struct clk_alpha_pll_postdiv gpll4 = {
	.offset = 0x22000,
	.regs = clk_alpha_pll_regs[CLK_ALPHA_GCC_PLL_TYPE_STROMER_PLUS],
	.width = 4,
	.clkr.hw.init = &(struct clk_init_data){
		.name = "gpll4",
		.parent_hws = (const struct clk_hw *[]){
				&gpll4_main.clkr.hw },
		.num_parents = 1,
		.ops = &clk_alpha_pll_postdiv_ro_ops,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static const struct parent_map gcc_parent_map_0[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_0[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_1[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_1[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_2[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL4_OUT_MAIN, 2 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_2[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_3[] = {
	{ P_XO, 0 },
};

static const struct clk_parent_data gcc_parent_data_3[] = {
	{ .fw_name = "xo" },
};

static const struct parent_map gcc_parent_map_4[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_SLEEP_CLK, 6 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_4[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "sleep_clk" },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_5[] = {
	{ P_XO, 0 },
	{ P_GPLL4_OUT_MAIN, 1 },
	{ P_GPLL0_OUT_AUX, 2 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_5[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_6[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL2_OUT_AUX, 2 },
	{ P_GPLL4_OUT_AUX, 3 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_GPLL0_OUT_AUX, 5 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_6[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll2.clkr.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .hw = &gpll0.clkr.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_7[] = {
	{ P_XO, 0 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_7[] = {
	{ .fw_name = "xo" },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_8[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL0_OUT_AUX, 2 },
	{ P_SLEEP_CLK, 6 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_8[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0.clkr.hw },
	{ .fw_name = "sleep_clk" },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_9[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL2_OUT_AUX, 2 },
	{ P_GPLL4_OUT_AUX, 3 },
	{ P_SLEEP_CLK, 6 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_9[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll2.clkr.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .fw_name = "sleep_clk" },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_10[] = {
	{ P_XO, 0 },
	{ P_GPLL4_OUT_MAIN, 1 },
	{ P_GPLL0_OUT_AUX, 2 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_10[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_11[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL2_OUT_AUX, 2 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_11[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll2.clkr.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_12[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL2_OUT_MAIN, 2 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_12[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll2.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_13[] = {
	{ P_SLEEP_CLK, 6 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_13[] = {
	{ .fw_name = "sleep_clk" },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_14[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_MAIN, 1 },
	{ P_GPLL4_OUT_MAIN, 2 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 3 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_14[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_15[] = {
	{ P_XO, 0 },
	{ P_GPLL0_OUT_AUX, 2 },
	{ P_SLEEP_CLK, 6 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_15[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll0.clkr.hw },
	{ .fw_name = "sleep_clk" },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct parent_map gcc_parent_map_16[] = {
	{ P_XO, 0 },
	{ P_GPLL4_OUT_AUX, 1 },
	{ P_GPLL0_OUT_MAIN, 3 },
	{ P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 4 },
	{ P_CORE_BI_PLL_TEST_SE, 7 },
};

static const struct clk_parent_data gcc_parent_data_16[] = {
	{ .fw_name = "xo" },
	{ .hw = &gpll4.clkr.hw },
	{ .hw = &gpll0.clkr.hw },
	{ .hw = &gpll0_div2.hw },
	{ .fw_name = "core_bi_pll_test_se" },
};

static const struct freq_tbl ftbl_gcc_adss_pwm_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	F(100000000, P_GPLL0_OUT_MAIN, 8, 0, 0),
	{ }
};


static const struct clk_parent_data gcc_pcie3x2_phy_pipe_clk_xo[] = {
	{ .fw_name = "pcie3x2_phy_pipe_clk" },
	{ .fw_name = "xo" },
};

static const struct parent_map gcc_pcie3x2_phy_pipe_clk_xo_map[] = {
	{ P_PCIE3X2_PIPE, 0 },
	{ P_XO, 2 },
};


static const struct clk_parent_data gcc_pcie3x1_0_phy_pipe_clk_xo[] = {
	{ .fw_name = "pcie3x1_0_phy_pipe_clk" },
	{ .fw_name = "xo" },
};

static const struct parent_map gcc_pcie3x1_0_phy_pipe_clk_xo_map[] = {
	{ P_PCIE3X1_0_PIPE, 0 },
	{ P_XO, 2 },
};


static const struct clk_parent_data gcc_pcie3x1_1_phy_pipe_clk_xo[] = {
	{ .fw_name = "pcie3x1_1_phy_pipe_clk" },
	{ .fw_name = "xo" },
};

static const struct parent_map gcc_pcie3x1_1_phy_pipe_clk_xo_map[] = {
	{ P_PCIE3X1_1_PIPE, 0 },
	{ P_XO, 2 },
};


static const struct clk_parent_data gcc_usb3phy_0_cc_pipe_clk_xo[] = {
	{ .fw_name = "usb3phy_0_cc_pipe_clk" },
	{ .fw_name = "xo" },
};

static const struct parent_map gcc_usb3phy_0_cc_pipe_clk_xo_map[] = {
	{ P_USB3PHY_0_PIPE, 0 },
	{ P_XO, 2 },
};

static struct clk_rcg2 gcc_adss_pwm_clk_src = {
	.cmd_rcgr = 0x1c004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_1,
	.freq_tbl = ftbl_gcc_adss_pwm_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_adss_pwm_clk_src",
		.parent_data = gcc_parent_data_1,
		.num_parents = ARRAY_SIZE(gcc_parent_data_1),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_apss_ahb_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	F(50000000, P_GPLL0_OUT_MAIN, 16, 0, 0),
	F(100000000, P_GPLL0_OUT_MAIN, 8, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_apss_ahb_clk_src = {
	.cmd_rcgr = 0x2400c,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_apss_ahb_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_apss_ahb_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_apss_axi_clk_src[] = {
	F(480000000, P_GPLL4_OUT_AUX, 2.5, 0, 0),
	F(533333333, P_GPLL0_OUT_MAIN, 1.5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_apss_axi_clk_src = {
	.cmd_rcgr = 0x24004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_6,
	.freq_tbl = ftbl_gcc_apss_axi_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_apss_axi_clk_src",
		.parent_data = gcc_parent_data_6,
		.num_parents = ARRAY_SIZE(gcc_parent_data_6),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_blsp1_qup1_spi_apps_clk_src[] = {
	F(960000, P_XO, 1, 1, 25),
	F(4800000, P_XO, 5, 0, 0),
	F(9600000, P_XO, 2.5, 0, 0),
	F(16000000, P_GPLL0_OUT_MAIN, 10, 1, 5),
	F(24000000, P_XO, 1, 0, 0),
	F(25000000, P_GPLL0_OUT_MAIN, 16, 1, 2),
	F(50000000, P_GPLL0_OUT_MAIN, 16, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_blsp1_qup1_spi_apps_clk_src = {
	.cmd_rcgr = 0x2004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_blsp1_qup1_spi_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_blsp1_qup1_spi_apps_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_blsp1_qup2_spi_apps_clk_src = {
	.cmd_rcgr = 0x3004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_blsp1_qup1_spi_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_blsp1_qup2_spi_apps_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_blsp1_qup3_spi_apps_clk_src = {
	.cmd_rcgr = 0x4004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_blsp1_qup1_spi_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_blsp1_qup3_spi_apps_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_blsp1_uart1_apps_clk_src[] = {
	F(3686400, P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 1, 144, 15625),
	F(7372800, P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 1, 288, 15625),
	F(14745600, P_GCC_GPLL0_OUT_MAIN_DIV_CLK_SRC, 1, 576, 15625),
	F(24000000, P_XO, 1, 0, 0),
	F(25000000, P_GPLL0_OUT_MAIN, 16, 1, 2),
	F(32000000, P_GPLL0_OUT_MAIN, 1, 1, 25),
	F(40000000, P_GPLL0_OUT_MAIN, 1, 1, 20),
	F(46400000, P_GPLL0_OUT_MAIN, 1, 29, 500),
	F(48000000, P_GPLL0_OUT_MAIN, 1, 3, 50),
	F(51200000, P_GPLL0_OUT_MAIN, 1, 8, 125),
	F(56000000, P_GPLL0_OUT_MAIN, 1, 7, 100),
	F(58982400, P_GPLL0_OUT_MAIN, 1, 1152, 15625),
	F(60000000, P_GPLL0_OUT_MAIN, 1, 3, 40),
	F(64000000, P_GPLL0_OUT_MAIN, 12.5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_blsp1_uart1_apps_clk_src = {
	.cmd_rcgr = 0x202c,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_blsp1_uart1_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_blsp1_uart1_apps_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_blsp1_uart2_apps_clk_src = {
	.cmd_rcgr = 0x302c,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_blsp1_uart1_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_blsp1_uart2_apps_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_blsp1_uart3_apps_clk_src = {
	.cmd_rcgr = 0x402c,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_blsp1_uart1_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_blsp1_uart3_apps_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_gp1_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN, 4, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_gp1_clk_src = {
	.cmd_rcgr = 0x8004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_4,
	.freq_tbl = ftbl_gcc_gp1_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_gp1_clk_src",
		.parent_data = gcc_parent_data_4,
		.num_parents = ARRAY_SIZE(gcc_parent_data_4),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_gp2_clk_src = {
	.cmd_rcgr = 0x9004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_4,
	.freq_tbl = ftbl_gcc_gp1_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_gp2_clk_src",
		.parent_data = gcc_parent_data_4,
		.num_parents = ARRAY_SIZE(gcc_parent_data_4),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_lpass_sway_clk_src[] = {
	F(133333333, P_GPLL0_OUT_MAIN, 6, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_lpass_sway_clk_src = {
	.cmd_rcgr = 0x27004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_1,
	.freq_tbl = ftbl_gcc_lpass_sway_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_lpass_sway_clk_src",
		.parent_data = gcc_parent_data_1,
		.num_parents = ARRAY_SIZE(gcc_parent_data_1),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_nss_ts_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_nss_ts_clk_src = {
	.cmd_rcgr = 0x17088,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_7,
	.freq_tbl = ftbl_gcc_nss_ts_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_nss_ts_clk_src",
		.parent_data = gcc_parent_data_7,
		.num_parents = ARRAY_SIZE(gcc_parent_data_7),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_pcie3x1_0_axi_clk_src[] = {
	F(240000000, P_GPLL4_OUT_MAIN, 5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_pcie3x1_0_axi_clk_src = {
	.cmd_rcgr = 0x29018,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_2,
	.freq_tbl = ftbl_gcc_pcie3x1_0_axi_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x1_0_axi_clk_src",
		.parent_data = gcc_parent_data_2,
		.num_parents = ARRAY_SIZE(gcc_parent_data_2),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_pcie3x1_0_rchg_clk_src = {
	.cmd_rcgr = 0x2907c,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_adss_pwm_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x1_0_rchg_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_branch gcc_pcie3x1_0_rchg_clk = {
	.halt_reg = 0x2907c,
	.clkr = {
		.enable_reg = 0x2907c,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gcc_pcie3x1_0_rchg_clk",
			.parent_hws = (const struct clk_hw *[]){
					&gcc_pcie3x1_0_rchg_clk_src.clkr.hw },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_rcg2 gcc_pcie3x1_1_axi_clk_src = {
	.cmd_rcgr = 0x2a004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_2,
	.freq_tbl = ftbl_gcc_pcie3x1_0_axi_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x1_1_axi_clk_src",
		.parent_data = gcc_parent_data_2,
		.num_parents = ARRAY_SIZE(gcc_parent_data_2),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_pcie3x1_1_rchg_clk_src = {
	.cmd_rcgr = 0x2a078,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_adss_pwm_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x1_1_rchg_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_branch gcc_pcie3x1_1_rchg_clk = {
	.halt_reg = 0x2a078,
	.clkr = {
		.enable_reg = 0x2a078,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gcc_pcie3x1_1_rchg_clk",
			.parent_hws = (const struct clk_hw *[]){
					&gcc_pcie3x1_1_rchg_clk_src.clkr.hw },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static const struct freq_tbl ftbl_gcc_pcie3x2_axi_m_clk_src[] = {
	F(266666667, P_GPLL4_OUT_MAIN, 4.5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_pcie3x2_axi_m_clk_src = {
	.cmd_rcgr = 0x28018,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_2,
	.freq_tbl = ftbl_gcc_pcie3x2_axi_m_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x2_axi_m_clk_src",
		.parent_data = gcc_parent_data_2,
		.num_parents = ARRAY_SIZE(gcc_parent_data_2),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_pcie3x2_axi_s_clk_src = {
	.cmd_rcgr = 0x28084,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_2,
	.freq_tbl = ftbl_gcc_pcie3x1_0_axi_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x2_axi_s_clk_src",
		.parent_data = gcc_parent_data_2,
		.num_parents = ARRAY_SIZE(gcc_parent_data_2),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_pcie3x2_rchg_clk_src = {
	.cmd_rcgr = 0x28078,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_adss_pwm_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie3x2_rchg_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_branch gcc_pcie3x2_rchg_clk = {
	.halt_reg = 0x28078,
	.clkr = {
		.enable_reg = 0x28078,
		.enable_mask = BIT(1),
		.hw.init = &(struct clk_init_data){
			.name = "gcc_pcie3x2_rchg_clk",
			.parent_hws = (const struct clk_hw *[]){
					&gcc_pcie3x2_rchg_clk_src.clkr.hw },
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static const struct freq_tbl ftbl_gcc_pcie_aux_clk_src[] = {
	F(2000000, P_XO, 12, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_pcie_aux_clk_src = {
	.cmd_rcgr = 0x28004,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_parent_map_8,
	.freq_tbl = ftbl_gcc_pcie_aux_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcie_aux_clk_src",
		.parent_data = gcc_parent_data_8,
		.num_parents = ARRAY_SIZE(gcc_parent_data_8),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_regmap_mux pcie3x2_pipe_clk_src = {
	.reg = 0x28064,
	.shift = 8,
	.width = 2,
	.parent_map = gcc_pcie3x2_phy_pipe_clk_xo_map,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "pcie3x2_phy_pipe_clk_src",
			.parent_data = gcc_pcie3x2_phy_pipe_clk_xo,
			.num_parents = 2,
			.ops = &clk_regmap_mux_closest_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_regmap_mux pcie3x1_0_pipe_clk_src = {
	.reg = 0x29064,
	.shift = 8,
	.width = 2,
	.parent_map = gcc_pcie3x1_0_phy_pipe_clk_xo_map,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "pcie3x1_0_phy_pipe_clk_src",
			.parent_data = gcc_pcie3x1_0_phy_pipe_clk_xo,
			.num_parents = 2,
			.ops = &clk_regmap_mux_closest_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_regmap_mux pcie3x1_1_pipe_clk_src = {
	.reg = 0x2A064,
	.shift = 8,
	.width = 2,
	.parent_map = gcc_pcie3x1_1_phy_pipe_clk_xo_map,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "pcie3x1_1_phy_pipe_clk_src",
			.parent_data = gcc_pcie3x1_1_phy_pipe_clk_xo,
			.num_parents = 2,
			.ops = &clk_regmap_mux_closest_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static const struct freq_tbl ftbl_gcc_pcnoc_bfdcd_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	F(50000000, P_GPLL0_OUT_MAIN, 16, 0, 0),
	F(100000000, P_GPLL0_OUT_MAIN, 8, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_pcnoc_bfdcd_clk_src = {
	.cmd_rcgr = 0x31004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_pcnoc_bfdcd_clk_src,
	.flags = CLK_RCG2_HW_CONTROLLED,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_pcnoc_bfdcd_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
		.flags = CLK_IS_CRITICAL,
	},
};

static struct clk_rcg2 gcc_q6_axim_clk_src = {
	.cmd_rcgr = 0x25004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_9,
	.freq_tbl = ftbl_gcc_apss_axi_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_q6_axim_clk_src",
		.parent_data = gcc_parent_data_9,
		.num_parents = ARRAY_SIZE(gcc_parent_data_9),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_qdss_at_clk_src[] = {
	F(240000000, P_GPLL4_OUT_MAIN, 5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_qdss_at_clk_src = {
	.cmd_rcgr = 0x2d004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_5,
	.freq_tbl = ftbl_gcc_qdss_at_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_qdss_at_clk_src",
		.parent_data = gcc_parent_data_5,
		.num_parents = ARRAY_SIZE(gcc_parent_data_5),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_qdss_stm_clk_src = {
	.cmd_rcgr = 0x2d00c,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_gp1_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_qdss_stm_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_qdss_traceclkin_clk_src[] = {
	F(240000000, P_GPLL4_OUT_MAIN, 4, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_qdss_traceclkin_clk_src = {
	.cmd_rcgr = 0x2d014,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_10,
	.freq_tbl = ftbl_gcc_qdss_traceclkin_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_qdss_traceclkin_clk_src",
		.parent_data = gcc_parent_data_10,
		.num_parents = ARRAY_SIZE(gcc_parent_data_10),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_qdss_tsctr_clk_src[] = {
	F(600000000, P_GPLL4_OUT_MAIN, 2, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_qdss_tsctr_clk_src = {
	.cmd_rcgr = 0x2d01c,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_5,
	.freq_tbl = ftbl_gcc_qdss_tsctr_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_qdss_tsctr_clk_src",
		.parent_data = gcc_parent_data_5,
		.num_parents = ARRAY_SIZE(gcc_parent_data_5),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_fixed_factor gcc_qdss_tsctr_div2_clk_src = {
	.mult = 1,
	.div = 2,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_qdss_tsctr_div2_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_qdss_tsctr_clk_src.clkr.hw },
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_fixed_factor gcc_qdss_tsctr_div3_clk_src = {
	.mult = 1,
	.div = 3,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_qdss_tsctr_div3_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_qdss_tsctr_clk_src.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_fixed_factor gcc_qdss_tsctr_div4_clk_src = {
	.mult = 1,
	.div = 4,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_qdss_tsctr_div4_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_qdss_tsctr_clk_src.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_fixed_factor gcc_qdss_tsctr_div8_clk_src = {
	.mult = 1,
	.div = 8,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_qdss_tsctr_div8_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_qdss_tsctr_clk_src.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_fixed_factor gcc_qdss_tsctr_div16_clk_src = {
	.mult = 1,
	.div = 16,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_qdss_tsctr_div16_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_qdss_tsctr_clk_src.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
	},
};

static const struct freq_tbl ftbl_gcc_qpic_io_macro_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	F(100000000, P_GPLL0_OUT_MAIN, 8, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN, 4, 0, 0),
	F(320000000, P_GPLL0_OUT_MAIN, 2.5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_qpic_io_macro_clk_src = {
	.cmd_rcgr = 0x32004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_11,
	.freq_tbl = ftbl_gcc_qpic_io_macro_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_qpic_io_macro_clk_src",
		.parent_data = gcc_parent_data_11,
		.num_parents = ARRAY_SIZE(gcc_parent_data_11),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_sdcc1_apps_clk_src[] = {
	F(143713, P_XO, 1, 1, 167),
	F(400000, P_XO, 1, 1, 60),
	F(24000000, P_XO, 1, 0, 0),
	F(48000000, P_GPLL2_OUT_MAIN, 12, 1, 2),
	F(96000000, P_GPLL2_OUT_MAIN, 12, 0, 0),
	F(177777778, P_GPLL0_OUT_MAIN, 4.5, 0, 0),
	F(192000000, P_GPLL2_OUT_MAIN, 6, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN, 4, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_sdcc1_apps_clk_src = {
	.cmd_rcgr = 0x33004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_12,
	.freq_tbl = ftbl_gcc_sdcc1_apps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_sdcc1_apps_clk_src",
		.parent_data = gcc_parent_data_12,
		.num_parents = ARRAY_SIZE(gcc_parent_data_12),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_sleep_clk_src[] = {
	F(32000, P_SLEEP_CLK, 1, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_sleep_clk_src = {
	.cmd_rcgr = 0x3400c,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_13,
	.freq_tbl = ftbl_gcc_sleep_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_sleep_clk_src",
		.parent_data = gcc_parent_data_13,
		.num_parents = ARRAY_SIZE(gcc_parent_data_13),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_system_noc_bfdcd_clk_src[] = {
	F(24000000, P_XO, 1, 0, 0),
	F(133333333, P_GPLL0_OUT_MAIN, 6, 0, 0),
	F(200000000, P_GPLL0_OUT_MAIN, 4, 0, 0),
	F(266666667, P_GPLL4_OUT_MAIN, 4.5, 0, 0),
	{ }
};

static struct clk_rcg2 gcc_system_noc_bfdcd_clk_src = {
	.cmd_rcgr = 0x2e004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_14,
	.freq_tbl = ftbl_gcc_system_noc_bfdcd_clk_src,
	.flags = CLK_RCG2_HW_CONTROLLED,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_system_noc_bfdcd_clk_src",
		.parent_data = gcc_parent_data_14,
		.num_parents = ARRAY_SIZE(gcc_parent_data_14),
		.ops = &clk_rcg2_ops,
		.flags = CLK_IS_CRITICAL,
	},
};

static struct clk_fixed_factor gcc_system_noc_bfdcd_div2_clk_src = {
	.mult = 1,
	.div = 2,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_system_noc_bfdcd_div2_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static struct clk_rcg2 gcc_uniphy_sys_clk_src = {
	.cmd_rcgr = 0x16004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_3,
	.freq_tbl = ftbl_gcc_nss_ts_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_uniphy_sys_clk_src",
		.parent_data = gcc_parent_data_3,
		.num_parents = ARRAY_SIZE(gcc_parent_data_3),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_usb0_aux_clk_src = {
	.cmd_rcgr = 0x2c018,
	.mnd_width = 16,
	.hid_width = 5,
	.parent_map = gcc_parent_map_15,
	.freq_tbl = ftbl_gcc_pcie_aux_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_usb0_aux_clk_src",
		.parent_data = gcc_parent_data_15,
		.num_parents = ARRAY_SIZE(gcc_parent_data_15),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_usb0_lfps_clk_src[] = {
	F(25000000, P_GPLL0_OUT_MAIN, 16, 1, 2),
	{ }
};

static struct clk_rcg2 gcc_usb0_lfps_clk_src = {
	.cmd_rcgr = 0x2c07c,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_1,
	.freq_tbl = ftbl_gcc_usb0_lfps_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_usb0_lfps_clk_src",
		.parent_data = gcc_parent_data_1,
		.num_parents = ARRAY_SIZE(gcc_parent_data_1),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_usb0_master_clk_src = {
	.cmd_rcgr = 0x2c004,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_0,
	.freq_tbl = ftbl_gcc_gp1_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_usb0_master_clk_src",
		.parent_data = gcc_parent_data_0,
		.num_parents = ARRAY_SIZE(gcc_parent_data_0),
		.ops = &clk_rcg2_ops,
	},
};

static const struct freq_tbl ftbl_gcc_usb0_mock_utmi_clk_src[] = {
	F(60000000, P_GPLL4_OUT_AUX, 10, 1, 2),
	{ }
};

static struct clk_rcg2 gcc_usb0_mock_utmi_clk_src = {
	.cmd_rcgr = 0x2c02c,
	.mnd_width = 8,
	.hid_width = 5,
	.parent_map = gcc_parent_map_16,
	.freq_tbl = ftbl_gcc_usb0_mock_utmi_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_usb0_mock_utmi_clk_src",
		.parent_data = gcc_parent_data_16,
		.num_parents = ARRAY_SIZE(gcc_parent_data_16),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_regmap_mux usb0_pipe_clk_src = {
	.reg = 0x2C074,
	.shift = 8,
	.width = 2,
	.parent_map = gcc_usb3phy_0_cc_pipe_clk_xo_map,
	.clkr = {
		.hw.init = &(struct clk_init_data){
			.name = "usb0phy_0_cc_pipe_clk_src",
			.parent_data = gcc_usb3phy_0_cc_pipe_clk_xo,
			.num_parents = 2,
			.ops = &clk_regmap_mux_closest_ops,
			.flags = CLK_SET_RATE_PARENT,
		},
	},
};

static struct clk_rcg2 gcc_wcss_ahb_clk_src = {
	.cmd_rcgr = 0x25030,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_1,
	.freq_tbl = ftbl_gcc_lpass_sway_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_wcss_ahb_clk_src",
		.parent_data = gcc_parent_data_1,
		.num_parents = ARRAY_SIZE(gcc_parent_data_1),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_rcg2 gcc_xo_clk_src = {
	.cmd_rcgr = 0x34004,
	.mnd_width = 0,
	.hid_width = 5,
	.parent_map = gcc_parent_map_3,
	.freq_tbl = ftbl_gcc_nss_ts_clk_src,
	.clkr.hw.init = &(const struct clk_init_data){
		.name = "gcc_xo_clk_src",
		.parent_data = gcc_parent_data_3,
		.num_parents = ARRAY_SIZE(gcc_parent_data_3),
		.ops = &clk_rcg2_ops,
	},
};

static struct clk_fixed_factor gcc_xo_div4_clk_src = {
	.mult = 1,
	.div = 4,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_xo_div4_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_xo_clk_src.clkr.hw },
		.num_parents = 1,
		.ops = &clk_fixed_factor_ops,
		.flags = CLK_SET_RATE_PARENT,
	},
};

static struct clk_regmap_div gcc_qdss_dap_div_clk_src = {
	.reg = 0x2d028,
	.shift = 0,
	.width = 4,
	.clkr.hw.init = &(const struct clk_init_data) {
		.name = "gcc_qdss_dap_div_clk_src",
		.parent_hws = (const struct clk_hw*[]){
			&gcc_qdss_tsctr_clk_src.clkr.hw,
		},
		.num_parents = 1,
		.ops = &clk_regmap_div_ro_ops,
	},
};

static struct clk_regmap_div gcc_usb0_mock_utmi_div_clk_src = {
	.reg = 0x2c040,
	.shift = 0,
	.width = 2,
	.clkr.hw.init = &(const struct clk_init_data) {
		.name = "gcc_usb0_mock_utmi_div_clk_src",
		.parent_hws = (const struct clk_hw*[]){
			&gcc_usb0_mock_utmi_clk_src.clkr.hw,
		},
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_regmap_div_ro_ops,
	},
};

static struct clk_branch gcc_adss_pwm_clk = {
	.halt_reg = 0x1c00c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1c00c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_adss_pwm_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_adss_pwm_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_ahb_clk = {
	.halt_reg = 0x34024,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x34024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_apss_ahb_clk = {
	.halt_reg = 0x24018,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_apss_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_apss_ahb_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_apss_axi_clk = {
	.halt_reg = 0x2401c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(1),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_apss_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_apss_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_ahb_clk = {
	.halt_reg = 0x1008,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(4),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_qup1_i2c_apps_clk = {
	.halt_reg = 0x2024,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_qup1_i2c_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_qup1_spi_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_qup1_spi_apps_clk = {
	.halt_reg = 0x2020,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_qup1_spi_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_qup1_spi_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_qup2_i2c_apps_clk = {
	.halt_reg = 0x3024,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_qup2_i2c_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_qup2_spi_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_qup2_spi_apps_clk = {
	.halt_reg = 0x3020,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_qup2_spi_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_qup2_spi_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_qup3_i2c_apps_clk = {
	.halt_reg = 0x4024,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x4024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_qup3_i2c_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_qup3_spi_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_qup3_spi_apps_clk = {
	.halt_reg = 0x4020,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x4020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_qup3_spi_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_qup3_spi_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_sleep_clk = {
	.halt_reg = 0x1010,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(5),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_sleep_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_sleep_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_uart1_apps_clk = {
	.halt_reg = 0x2040,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2040,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_uart1_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_uart1_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_uart2_apps_clk = {
	.halt_reg = 0x3040,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3040,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_uart2_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_uart2_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_blsp1_uart3_apps_clk = {
	.halt_reg = 0x4054,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x4054,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_blsp1_uart3_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_blsp1_uart3_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_ce_ahb_clk = {
	.halt_reg = 0x25074,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25074,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_ce_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_div2_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_ce_axi_clk = {
	.halt_reg = 0x25068,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25068,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_ce_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_ce_pcnoc_ahb_clk = {
	.halt_reg = 0x25070,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25070,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_ce_pcnoc_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_cmn_12gpll_ahb_clk = {
	.halt_reg = 0x3a004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3a004,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_cmn_12gpll_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_cmn_12gpll_apu_clk = {
	.halt_reg = 0x3a00c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3a00c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_cmn_12gpll_apu_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_cmn_12gpll_sys_clk = {
	.halt_reg = 0x3a008,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3a008,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_cmn_12gpll_sys_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_uniphy_sys_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_dcc_clk = {
	.halt_reg = 0x35004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x35004,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_dcc_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_dcc_xo_clk = {
	.halt_reg = 0x3500c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3500c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_dcc_xo_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_xo_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_gp1_clk = {
	.halt_reg = 0x8018,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x8018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_gp1_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_gp1_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_gp2_clk = {
	.halt_reg = 0x9018,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x9018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_gp2_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_gp2_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_lpass_core_axim_clk = {
	.halt_reg = 0x27018,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x27018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_lpass_core_axim_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_lpass_sway_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_lpass_sway_clk = {
	.halt_reg = 0x27014,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x27014,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_lpass_sway_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_lpass_sway_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mdio_ahb_clk = {
	.halt_reg = 0x12004,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x12004,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mdio_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mdio_slave_ahb_clk = {
	.halt_reg = 0x1200c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1200c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mdio_slave_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mem_noc_q6_axi_clk = {
	.halt_reg = 0x19010,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x19010,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mem_noc_q6_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_q6_axim_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mem_noc_snoc_axi_clk = {
	.halt_reg = 0x19018,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x19018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mem_noc_snoc_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mem_noc_ts_clk = {
	.halt_reg = 0x19028,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x19028,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mem_noc_ts_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div8_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nss_ts_clk = {
	.halt_reg = 0x17018,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x17018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nss_ts_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_nss_ts_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nsscc_clk = {
	.halt_reg = 0x17034,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17034,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nsscc_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nsscfg_clk = {
	.halt_reg = 0x1702c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1702c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nsscfg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_atb_clk = {
	.halt_reg = 0x17014,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17014,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_atb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_nsscc_clk = {
	.halt_reg = 0x17030,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17030,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_nsscc_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_qosgen_ref_clk = {
	.halt_reg = 0x1701c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1701c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_qosgen_ref_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_xo_div4_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_snoc_1_clk = {
	.halt_reg = 0x1707c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1707c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_snoc_1_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_snoc_clk = {
	.halt_reg = 0x17028,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17028,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_snoc_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_timeout_ref_clk = {
	.halt_reg = 0x17020,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_timeout_ref_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_xo_div4_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_xo_dcd_clk = {
	.halt_reg = 0x17074,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17074,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_xo_dcd_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_xo_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_0_ahb_clk = {
	.halt_reg = 0x29030,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x29030,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_0_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_0_aux_clk = {
	.halt_reg = 0x29070,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x29070,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_0_aux_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie_aux_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_0_axi_m_clk = {
	.halt_reg = 0x29038,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x29038,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_0_axi_m_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_0_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_0_axi_s_bridge_clk = {
	.halt_reg = 0x29048,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x29048,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_0_axi_s_bridge_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_0_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_0_axi_s_clk = {
	.halt_reg = 0x29040,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x29040,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_0_axi_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_0_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_0_pipe_clk = {
	.halt_reg = 0x29068,
	.halt_check = BRANCH_HALT_DELAY,
	.clkr = {
		.enable_reg = 0x29068,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_0_pipe_clk",
			.parent_names = (const char *[]){
				"pcie3x1_0_pipe_clk_src"
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_1_ahb_clk = {
	.halt_reg = 0x2a00c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2a00c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_1_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_1_aux_clk = {
	.halt_reg = 0x2a070,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2a070,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_1_aux_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie_aux_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_1_axi_m_clk = {
	.halt_reg = 0x2a014,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2a014,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_1_axi_m_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_1_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_1_axi_s_bridge_clk = {
	.halt_reg = 0x2a024,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2a024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_1_axi_s_bridge_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_1_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_1_axi_s_clk = {
	.halt_reg = 0x2a01c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2a01c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_1_axi_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_1_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_1_pipe_clk = {
	.halt_reg = 0x2a068,
	.halt_check = BRANCH_HALT_DELAY,
	.clkr = {
		.enable_reg = 0x2a068,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_1_pipe_clk",
			.parent_names = (const char *[]){
				"pcie3x1_1_pipe_clk_src"
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x1_phy_ahb_clk = {
	.halt_reg = 0x29078,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x29078,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x1_phy_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_ahb_clk = {
	.halt_reg = 0x28030,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x28030,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_aux_clk = {
	.halt_reg = 0x28070,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x28070,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_aux_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie_aux_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_axi_m_clk = {
	.halt_reg = 0x28038,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x28038,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_axi_m_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x2_axi_m_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_axi_s_bridge_clk = {
	.halt_reg = 0x28048,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x28048,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_axi_s_bridge_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x2_axi_s_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_axi_s_clk = {
	.halt_reg = 0x28040,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x28040,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_axi_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x2_axi_s_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_phy_ahb_clk = {
	.halt_reg = 0x28080,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x28080,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_phy_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcie3x2_pipe_clk = {
	.halt_reg = 0x28068,
	.halt_check = BRANCH_HALT_DELAY,
	.clkr = {
		.enable_reg = 0x28068,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcie3x2_pipe_clk",
			.parent_names = (const char *[]){
				"pcie3x2_pipe_clk_src"
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcnoc_at_clk = {
	.halt_reg = 0x31024,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x31024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcnoc_at_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcnoc_dcc_clk = {
	.halt_reg = 0x31080,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x31080,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcnoc_dcc_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcnoc_lpass_clk = {
	.halt_reg = 0x31020,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x31020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcnoc_lpass_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_lpass_sway_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_pcnoc_ts_clk = {
	.halt_reg = 0x3102c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x3102c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_pcnoc_ts_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div8_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_prng_ahb_clk = {
	.halt_reg = 0x13024,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(10),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_prng_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6_ahb_clk = {
	.halt_reg = 0x25014,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x25014,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_wcss_ahb_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6_ahb_s_clk = {
	.halt_reg = 0x25018,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x25018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6_ahb_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_wcss_ahb_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6_axim_clk = {
	.halt_reg = 0x2500c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2500c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6_axim_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_q6_axim_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6_axis_clk = {
	.halt_reg = 0x25010,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x25010,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6_axis_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6_tsctr_1to2_clk = {
	.halt_reg = 0x25020,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x25020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6_tsctr_1to2_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div2_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6ss_atbm_clk = {
	.halt_reg = 0x2501c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2501c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6ss_atbm_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6ss_pclkdbg_clk = {
	.halt_reg = 0x25024,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x25024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6ss_pclkdbg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_dap_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_q6ss_trig_clk = {
	.halt_reg = 0x250a0,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x250a0,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_q6ss_trig_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_dap_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_apb2jtag_clk = {
	.halt_reg = 0x2d060,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d060,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_apb2jtag_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_dap_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_at_clk = {
	.halt_reg = 0x2d038,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d038,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_at_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_cfg_ahb_clk = {
	.halt_reg = 0x2d06c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d06c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_cfg_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_dap_ahb_clk = {
	.halt_reg = 0x2d068,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d068,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_dap_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_dap_clk = {
	.halt_reg = 0x2d05c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(2),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_dap_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_dap_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_etr_usb_clk = {
	.halt_reg = 0x2d064,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d064,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_etr_usb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_fixed_factor gcc_eud_at_div_clk_src = {
	.mult = 1,
	.div = 6,
	.hw.init = &(struct clk_init_data){
		.name = "gcc_eud_at_div_clk_src",
		.parent_hws = (const struct clk_hw *[]){
				&gcc_qdss_at_clk_src.clkr.hw },
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_fixed_factor_ops,
	},
};

static struct clk_branch gcc_qdss_eud_at_clk = {
	.halt_reg = 0x2d070,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d070,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_eud_at_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_eud_at_div_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_stm_clk = {
	.halt_reg = 0x2d040,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d040,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_stm_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_stm_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_traceclkin_clk = {
	.halt_reg = 0x2d044,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d044,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_traceclkin_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_traceclkin_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_ts_clk = {
	.halt_reg = 0x2d078,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d078,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_ts_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_nss_ts_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_tsctr_div16_clk = {
	.halt_reg = 0x2d058,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d058,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_tsctr_div16_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div16_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_tsctr_div2_clk = {
	.halt_reg = 0x2d048,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d048,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_tsctr_div2_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div2_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_tsctr_div3_clk = {
	.halt_reg = 0x2d04c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d04c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_tsctr_div3_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div3_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_tsctr_div4_clk = {
	.halt_reg = 0x2d050,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d050,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_tsctr_div4_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div4_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qdss_tsctr_div8_clk = {
	.halt_reg = 0x2d054,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2d054,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qdss_tsctr_div8_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div8_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qpic_ahb_clk = {
	.halt_reg = 0x32010,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x32010,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qpic_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qpic_clk = {
	.halt_reg = 0x32014,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x32014,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qpic_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qpic_io_macro_clk = {
	.halt_reg = 0x3200c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3200c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qpic_io_macro_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qpic_io_macro_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_qpic_sleep_clk = {
	.halt_reg = 0x3201c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3201c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_qpic_sleep_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_sleep_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_sdcc1_ahb_clk = {
	.halt_reg = 0x33034,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x33034,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_sdcc1_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_sdcc1_apps_clk = {
	.halt_reg = 0x3302c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3302c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_sdcc1_apps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_sdcc1_apps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_lpass_cfg_clk = {
	.halt_reg = 0x2e028,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e028,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_lpass_cfg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_lpass_sway_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_nssnoc_1_clk = {
	.halt_reg = 0x17090,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17090,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_nssnoc_1_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_nssnoc_clk = {
	.halt_reg = 0x17084,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17084,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_nssnoc_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcie3_1lane_1_m_clk = {
	.halt_reg = 0x2e050,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e050,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcie3_1lane_1_m_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_1_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcie3_1lane_1_s_clk = {
	.halt_reg = 0x2e0ac,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e0ac,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcie3_1lane_1_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_1_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcie3_1lane_m_clk = {
	.halt_reg = 0x2e080,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e080,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcie3_1lane_m_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_0_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcie3_1lane_s_clk = {
	.halt_reg = 0x2e04c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e04c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcie3_1lane_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x1_0_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcie3_2lane_m_clk = {
	.halt_reg = 0x2e07c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e07c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcie3_2lane_m_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x2_axi_m_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcie3_2lane_s_clk = {
	.halt_reg = 0x2e048,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e048,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcie3_2lane_s_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcie3x2_axi_s_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_pcnoc_ahb_clk = {
	.halt_reg = 0x2e03c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2e03c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_pcnoc_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_ts_clk = {
	.halt_reg = 0x2e068,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2e068,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_ts_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div2_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_snoc_usb_clk = {
	.halt_reg = 0x2e058,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2e058,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_snoc_usb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_usb0_master_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_sys_noc_at_clk = {
	.halt_reg = 0x2e038,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2e038,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_sys_noc_at_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_sys_noc_axi_clk = {
	.halt_reg = 0x2e01c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2e01c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_sys_noc_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_sys_noc_qdss_stm_axi_clk = {
	.halt_reg = 0x2e034,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2e034,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_sys_noc_qdss_stm_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_stm_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_sys_noc_wcss_ahb_clk = {
	.halt_reg = 0x2e030,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e030,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_sys_noc_wcss_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_wcss_ahb_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_uniphy0_ahb_clk = {
	.halt_reg = 0x16010,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x16010,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_uniphy0_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_uniphy0_sys_clk = {
	.halt_reg = 0x1600c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1600c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_uniphy0_sys_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_uniphy_sys_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_uniphy1_ahb_clk = {
	.halt_reg = 0x1601c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1601c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_uniphy1_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_uniphy1_sys_clk = {
	.halt_reg = 0x16018,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x16018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_uniphy1_sys_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_uniphy_sys_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_aux_clk = {
	.halt_reg = 0x2c050,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2c050,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_aux_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_usb0_aux_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_eud_at_clk = {
	.halt_reg = 0x30004,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x30004,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_eud_at_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_eud_at_div_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_lfps_clk = {
	.halt_reg = 0x2c090,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2c090,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_lfps_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_usb0_lfps_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_master_clk = {
	.halt_reg = 0x2c048,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2c048,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_master_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_usb0_master_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_mock_utmi_clk = {
	.halt_reg = 0x2c054,
	.clkr = {
		.enable_reg = 0x2c054,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_mock_utmi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_usb0_mock_utmi_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_phy_cfg_ahb_clk = {
	.halt_reg = 0x2c05c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2c05c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_phy_cfg_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_pipe_clk = {
	.halt_reg = 0x2c078,
	.halt_check = BRANCH_HALT_DELAY,
	.clkr = {
		.enable_reg = 0x2c078,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_pipe_clk",
			.parent_names = (const char *[]){
				"usb0_pipe_clk_src"
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_usb0_sleep_clk = {
	.halt_reg = 0x2c058,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0x2c058,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_usb0_sleep_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_sleep_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_axim_clk = {
	.halt_reg = 0x2505c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2505c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_axim_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_axis_clk = {
	.halt_reg = 0x25060,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25060,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_axis_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_dbg_ifc_apb_bdg_clk = {
	.halt_reg = 0x25048,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25048,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_dbg_ifc_apb_bdg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_dap_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_dbg_ifc_apb_clk = {
	.halt_reg = 0x25038,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25038,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_dbg_ifc_apb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_dap_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_dbg_ifc_atb_bdg_clk = {
	.halt_reg = 0x2504c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2504c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_dbg_ifc_atb_bdg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_dbg_ifc_atb_clk = {
	.halt_reg = 0x2503c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2503c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_dbg_ifc_atb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_at_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_dbg_ifc_nts_bdg_clk = {
	.halt_reg = 0x25050,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25050,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_dbg_ifc_nts_bdg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div2_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_dbg_ifc_nts_clk = {
	.halt_reg = 0x25040,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25040,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_dbg_ifc_nts_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_qdss_tsctr_div2_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_ecahb_clk = {
	.halt_reg = 0x25058,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x25058,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_ecahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_wcss_ahb_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_mst_async_bdg_clk = {
	.halt_reg = 0x2e0b0,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e0b0,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_mst_async_bdg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_wcss_slv_async_bdg_clk = {
	.halt_reg = 0x2e0b4,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x2e0b4,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_wcss_slv_async_bdg_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_system_noc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_xo_clk = {
	.halt_reg = 0x34018,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x34018,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_xo_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_xo_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_xo_div4_clk = {
	.halt_reg = 0x3401c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x3401c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_xo_div4_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_xo_div4_clk_src.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IS_CRITICAL,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_im_sleep_clk = {
	.halt_reg = 0x34020,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x34020,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_im_sleep_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_sleep_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_nssnoc_pcnoc_1_clk = {
	.halt_reg = 0x17080,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x17080,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_nssnoc_pcnoc_1_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mem_noc_ahb_clk = {
	.halt_reg = 0x1900c,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x1900c,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mem_noc_ahb_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_pcnoc_bfdcd_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_branch gcc_mem_noc_apss_axi_clk = {
	.halt_reg = 0x1901c,
	.halt_check = BRANCH_HALT_VOTED,
	.clkr = {
		.enable_reg = 0xb004,
		.enable_mask = BIT(6),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mem_noc_apss_axi_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_apss_axi_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static struct clk_regmap_div gcc_snoc_qosgen_extref_div_clk_src = {
	.reg = 0x2e010,
	.shift = 0,
	.width = 2,
	.clkr.hw.init = &(const struct clk_init_data) {
		.name = "gcc_snoc_qosgen_extref_div_clk_src",
		.parent_hws = (const struct clk_hw*[]){
			&gcc_xo_clk_src.clkr.hw,
		},
		.num_parents = 1,
		.flags = CLK_SET_RATE_PARENT,
		.ops = &clk_regmap_div_ro_ops,
	},
};

static struct clk_branch gcc_mem_noc_qosgen_extref_clk = {
	.halt_reg = 0x19024,
	.halt_check = BRANCH_HALT,
	.clkr = {
		.enable_reg = 0x19024,
		.enable_mask = BIT(0),
		.hw.init = &(const struct clk_init_data){
			.name = "gcc_mem_noc_qosgen_extref_clk",
			.parent_hws = (const struct clk_hw*[]){
				&gcc_snoc_qosgen_extref_div_clk_src.clkr.hw,
			},
			.num_parents = 1,
			.flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED,
			.ops = &clk_branch2_ops,
		},
	},
};

static int clk_dummy_is_enabled(struct clk_hw *hw)
{
	return 1;
};

static int clk_dummy_enable(struct clk_hw *hw)
{
	return 0;
};

static void clk_dummy_disable(struct clk_hw *hw)
{
	return;
};

static u8 clk_dummy_get_parent(struct clk_hw *hw)
{
	return 0;
};

static int clk_dummy_set_parent(struct clk_hw *hw, u8 index)
{
	return 0;
};

static int clk_dummy_set_rate(struct clk_hw *hw, unsigned long rate,
			      unsigned long parent_rate)
{
	return 0;
};

static int clk_dummy_determine_rate(struct clk_hw *hw,
				struct clk_rate_request *req)
{
	return 0;
};

static unsigned long clk_dummy_recalc_rate(struct clk_hw *hw,
					   unsigned long parent_rate)
{
	return parent_rate;
};

static const struct clk_ops clk_dummy_ops = {
	.is_enabled = clk_dummy_is_enabled,
	.enable = clk_dummy_enable,
	.disable = clk_dummy_disable,
	.get_parent = clk_dummy_get_parent,
	.set_parent = clk_dummy_set_parent,
	.set_rate = clk_dummy_set_rate,
	.recalc_rate = clk_dummy_recalc_rate,
	.determine_rate = clk_dummy_determine_rate,
};

#define DEFINE_DUMMY_CLK(clk_name)				\
(&(struct clk_regmap) {						\
	.hw.init = &(struct clk_init_data){			\
		.name = #clk_name,				\
		.parent_names = (const char *[]){ "xo"},	\
		.num_parents = 1,				\
		.ops = &clk_dummy_ops,				\
	},							\
})

static struct clk_regmap *gcc_ipq5332_dummy_clocks[] = {
	[GCC_ADSS_PWM_CLK] = DEFINE_DUMMY_CLK(gcc_adss_pwm_clk),
	[GCC_ADSS_PWM_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_adss_pwm_clk_src),
	[GCC_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_ahb_clk),
	[GCC_APSS_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_apss_ahb_clk),
	[GCC_APSS_AHB_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_apss_ahb_clk_src),
	[GCC_APSS_AXI_CLK] = DEFINE_DUMMY_CLK(gcc_apss_axi_clk),
	[GCC_APSS_AXI_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_apss_axi_clk_src),
	[GCC_BLSP1_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_ahb_clk),
	[GCC_BLSP1_QUP1_I2C_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_qup1_i2c_apps_clk),
	[GCC_BLSP1_QUP1_SPI_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_qup1_spi_apps_clk),
	[GCC_BLSP1_QUP1_SPI_APPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_blsp1_qup1_spi_apps_clk_src),
	[GCC_BLSP1_QUP2_I2C_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_qup2_i2c_apps_clk),
	[GCC_BLSP1_QUP2_SPI_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_qup2_spi_apps_clk),
	[GCC_BLSP1_QUP2_SPI_APPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_blsp1_qup2_spi_apps_clk_src),
	[GCC_BLSP1_QUP3_I2C_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_qup3_i2c_apps_clk),
	[GCC_BLSP1_QUP3_SPI_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_qup3_spi_apps_clk),
	[GCC_BLSP1_QUP3_SPI_APPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_blsp1_qup3_spi_apps_clk_src),
	[GCC_BLSP1_SLEEP_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_sleep_clk),
	[GCC_BLSP1_UART1_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_uart1_apps_clk),
	[GCC_BLSP1_UART1_APPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_blsp1_uart1_apps_clk_src),
	[GCC_BLSP1_UART2_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_uart2_apps_clk),
	[GCC_BLSP1_UART2_APPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_blsp1_uart2_apps_clk_src),
	[GCC_BLSP1_UART3_APPS_CLK] = DEFINE_DUMMY_CLK(gcc_blsp1_uart3_apps_clk),
	[GCC_BLSP1_UART3_APPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_blsp1_uart3_apps_clk_src),
	[GCC_CE_AHB_CLK] = &gcc_ce_ahb_clk.clkr,
	[GCC_CE_AXI_CLK] = DEFINE_DUMMY_CLK(gcc_ce_axi_clk),
	[GCC_CE_PCNOC_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_ce_pcnoc_ahb_clk),
	[GCC_CMN_12GPLL_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_cmn_12gpll_ahb_clk),
	[GCC_CMN_12GPLL_APU_CLK] = DEFINE_DUMMY_CLK(gcc_cmn_12gpll_apu_clk),
	[GCC_CMN_12GPLL_SYS_CLK] = DEFINE_DUMMY_CLK(gcc_cmn_12gpll_sys_clk),
	[GCC_DCC_CLK] = DEFINE_DUMMY_CLK(gcc_dcc_clk),
	[GCC_DCC_XO_CLK] = DEFINE_DUMMY_CLK(gcc_dcc_xo_clk),
	[GCC_GP1_CLK] = DEFINE_DUMMY_CLK(gcc_gp1_clk),
	[GCC_GP1_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_gp1_clk_src),
	[GCC_GP2_CLK] = DEFINE_DUMMY_CLK(gcc_gp2_clk),
	[GCC_GP2_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_gp2_clk_src),
	[GCC_LPASS_CORE_AXIM_CLK] = DEFINE_DUMMY_CLK(gcc_lpass_core_axim_clk),
	[GCC_LPASS_SWAY_CLK] = DEFINE_DUMMY_CLK(gcc_lpass_sway_clk),
	[GCC_LPASS_SWAY_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_lpass_sway_clk_src),
	[GCC_MDIO_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_mdio_ahb_clk),
	[GCC_MDIO_SLAVE_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_mdio_slave_ahb_clk),
	[GCC_MEM_NOC_Q6_AXI_CLK] = &gcc_mem_noc_q6_axi_clk.clkr,
	[GCC_MEM_NOC_SNOC_AXI_CLK] = DEFINE_DUMMY_CLK(gcc_mem_noc_snoc_axi_clk),
	[GCC_MEM_NOC_TS_CLK] = DEFINE_DUMMY_CLK(gcc_mem_noc_ts_clk),
	[GCC_NSS_TS_CLK] = DEFINE_DUMMY_CLK(gcc_nss_ts_clk),
	[GCC_NSS_TS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_nss_ts_clk_src),
	[GCC_NSSCC_CLK] = DEFINE_DUMMY_CLK(gcc_nsscc_clk),
	[GCC_NSSCFG_CLK] = DEFINE_DUMMY_CLK(gcc_nsscfg_clk),
	[GCC_NSSNOC_ATB_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_atb_clk),
	[GCC_NSSNOC_NSSCC_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_nsscc_clk),
	[GCC_NSSNOC_QOSGEN_REF_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_qosgen_ref_clk),
	[GCC_NSSNOC_SNOC_1_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_snoc_1_clk),
	[GCC_NSSNOC_SNOC_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_snoc_clk),
	[GCC_NSSNOC_TIMEOUT_REF_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_timeout_ref_clk),
	[GCC_NSSNOC_XO_DCD_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_xo_dcd_clk),
	[GCC_PCIE3X1_0_AHB_CLK] = &gcc_pcie3x1_0_ahb_clk.clkr,
	[GCC_PCIE3X1_0_AUX_CLK] = &gcc_pcie3x1_0_aux_clk.clkr,
	[GCC_PCIE3X1_0_AXI_CLK_SRC] = &gcc_pcie3x1_0_axi_clk_src.clkr,
	[GCC_PCIE3X1_0_AXI_M_CLK] = &gcc_pcie3x1_0_axi_m_clk.clkr,
	[GCC_PCIE3X1_0_AXI_S_BRIDGE_CLK] = &gcc_pcie3x1_0_axi_s_bridge_clk.clkr,
	[GCC_PCIE3X1_0_AXI_S_CLK] = &gcc_pcie3x1_0_axi_s_clk.clkr,
	[GCC_PCIE3X1_0_PIPE_CLK] = &gcc_pcie3x1_0_pipe_clk.clkr,
	[GCC_PCIE3X1_0_RCHG_CLK] = &gcc_pcie3x1_0_rchg_clk.clkr,
	[GCC_PCIE3X1_0_RCHG_CLK_SRC] = &gcc_pcie3x1_0_rchg_clk_src.clkr,
	[GCC_PCIE3X1_1_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_ahb_clk),
	[GCC_PCIE3X1_1_AUX_CLK] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_aux_clk),
	[GCC_PCIE3X1_1_AXI_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_axi_clk_src),
	[GCC_PCIE3X1_1_AXI_M_CLK] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_axi_m_clk),
	[GCC_PCIE3X1_1_AXI_S_BRIDGE_CLK] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_axi_s_bridge_clk),
	[GCC_PCIE3X1_1_AXI_S_CLK] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_axi_s_clk),
	[GCC_PCIE3X1_1_PIPE_CLK] = DEFINE_DUMMY_CLK(gcc_pcie3x1_1_pipe_clk),
	[GCC_PCIE3X1_1_RCHG_CLK] = &gcc_pcie3x1_1_rchg_clk.clkr,
	[GCC_PCIE3X1_1_RCHG_CLK_SRC] = &gcc_pcie3x1_1_rchg_clk_src.clkr,
	[GCC_PCIE3X1_PHY_AHB_CLK] = &gcc_pcie3x1_phy_ahb_clk.clkr,
	[GCC_PCIE3X2_AHB_CLK] = &gcc_pcie3x2_ahb_clk.clkr,
	[GCC_PCIE3X2_AUX_CLK] = &gcc_pcie3x2_aux_clk.clkr,
	[GCC_PCIE3X2_AXI_M_CLK] = &gcc_pcie3x2_axi_m_clk.clkr,
	[GCC_PCIE3X2_AXI_M_CLK_SRC] = &gcc_pcie3x2_axi_m_clk_src.clkr,
	[GCC_PCIE3X2_AXI_S_BRIDGE_CLK] = &gcc_pcie3x2_axi_s_bridge_clk.clkr,
	[GCC_PCIE3X2_AXI_S_CLK] = &gcc_pcie3x2_axi_s_clk.clkr,
	[GCC_PCIE3X2_AXI_S_CLK_SRC] = &gcc_pcie3x2_axi_s_clk_src.clkr,
	[GCC_PCIE3X2_PHY_AHB_CLK] = &gcc_pcie3x2_phy_ahb_clk.clkr,
	[GCC_PCIE3X2_PIPE_CLK] = &gcc_pcie3x2_pipe_clk.clkr,
	[GCC_PCIE3X2_RCHG_CLK] = &gcc_pcie3x2_rchg_clk.clkr,
	[GCC_PCIE3X2_RCHG_CLK_SRC] = &gcc_pcie3x2_rchg_clk_src.clkr,
	[GCC_PCIE_AUX_CLK_SRC] = &gcc_pcie_aux_clk_src.clkr,
	[GCC_PCNOC_AT_CLK] = DEFINE_DUMMY_CLK(gcc_pcnoc_at_clk),
	[GCC_PCNOC_BFDCD_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_pcnoc_bfdcd_clk_src),
	[GCC_PCNOC_DCC_CLK] = DEFINE_DUMMY_CLK(gcc_pcnoc_dcc_clk),
	[GCC_PCNOC_LPASS_CLK] = DEFINE_DUMMY_CLK(gcc_pcnoc_lpass_clk),
	[GCC_PCNOC_TS_CLK] = DEFINE_DUMMY_CLK(gcc_pcnoc_ts_clk),
	[GCC_PRNG_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_prng_ahb_clk),
	[GCC_Q6_AHB_CLK] = &gcc_q6_ahb_clk.clkr,
	[GCC_Q6_AHB_S_CLK] = &gcc_q6_ahb_s_clk.clkr,
	[GCC_Q6_AXIM_CLK] = &gcc_q6_axim_clk.clkr,
	[GCC_Q6_AXIM_CLK_SRC] = &gcc_q6_axim_clk_src.clkr,
	[GCC_Q6_AXIS_CLK] = &gcc_q6_axis_clk.clkr,
	[GCC_Q6_TSCTR_1TO2_CLK] = &gcc_q6_tsctr_1to2_clk.clkr,
	[GCC_Q6SS_ATBM_CLK] = &gcc_q6ss_atbm_clk.clkr,
	[GCC_Q6SS_PCLKDBG_CLK] = &gcc_q6ss_pclkdbg_clk.clkr,
	[GCC_Q6SS_TRIG_CLK] = &gcc_q6ss_trig_clk.clkr,
	[GCC_QDSS_APB2JTAG_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_apb2jtag_clk),
	[GCC_QDSS_AT_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_at_clk),
	[GCC_QDSS_AT_CLK_SRC] = &gcc_qdss_at_clk_src.clkr,
	[GCC_QDSS_CFG_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_cfg_ahb_clk),
	[GCC_QDSS_DAP_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_dap_ahb_clk),
	[GCC_QDSS_DAP_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_dap_clk),
	[GCC_QDSS_DAP_DIV_CLK_SRC] = &gcc_qdss_dap_div_clk_src.clkr,
	[GCC_QDSS_ETR_USB_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_etr_usb_clk),
	[GCC_QDSS_EUD_AT_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_eud_at_clk),
	[GCC_QDSS_STM_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_stm_clk),
	[GCC_QDSS_STM_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_qdss_stm_clk_src),
	[GCC_QDSS_TRACECLKIN_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_traceclkin_clk),
	[GCC_QDSS_TRACECLKIN_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_qdss_traceclkin_clk_src),
	[GCC_QDSS_TS_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_ts_clk),
	[GCC_QDSS_TSCTR_CLK_SRC] = &gcc_qdss_tsctr_clk_src.clkr,
	[GCC_QDSS_TSCTR_DIV16_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_tsctr_div16_clk),
	[GCC_QDSS_TSCTR_DIV2_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_tsctr_div2_clk),
	[GCC_QDSS_TSCTR_DIV3_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_tsctr_div3_clk),
	[GCC_QDSS_TSCTR_DIV4_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_tsctr_div4_clk),
	[GCC_QDSS_TSCTR_DIV8_CLK] = DEFINE_DUMMY_CLK(gcc_qdss_tsctr_div8_clk),
	[GCC_QPIC_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_qpic_ahb_clk),
	[GCC_QPIC_CLK] = DEFINE_DUMMY_CLK(gcc_qpic_clk),
	[GCC_QPIC_IO_MACRO_CLK] = DEFINE_DUMMY_CLK(gcc_qpic_io_macro_clk),
	[GCC_QPIC_IO_MACRO_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_qpic_io_macro_clk_src),
	[GCC_QPIC_SLEEP_CLK] = DEFINE_DUMMY_CLK(gcc_qpic_sleep_clk),
	[GCC_SDCC1_AHB_CLK] = &gcc_sdcc1_ahb_clk.clkr,
	[GCC_SDCC1_APPS_CLK] = &gcc_sdcc1_apps_clk.clkr,
	[GCC_SDCC1_APPS_CLK_SRC] = &gcc_sdcc1_apps_clk_src.clkr,
	[GCC_SLEEP_CLK_SRC] = &gcc_sleep_clk_src.clkr,
	[GCC_SNOC_LPASS_CFG_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_lpass_cfg_clk),
	[GCC_SNOC_NSSNOC_1_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_nssnoc_1_clk),
	[GCC_SNOC_NSSNOC_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_nssnoc_clk),
	[GCC_SNOC_PCIE3_1LANE_1_M_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcie3_1lane_1_m_clk),
	[GCC_SNOC_PCIE3_1LANE_1_S_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcie3_1lane_1_s_clk),
	[GCC_SNOC_PCIE3_1LANE_M_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcie3_1lane_m_clk),
	[GCC_SNOC_PCIE3_1LANE_S_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcie3_1lane_s_clk),
	[GCC_SNOC_PCIE3_2LANE_M_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcie3_2lane_m_clk),
	[GCC_SNOC_PCIE3_2LANE_S_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcie3_2lane_s_clk),
	[GCC_SNOC_PCNOC_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_pcnoc_ahb_clk),
	[GCC_SNOC_TS_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_ts_clk),
	[GCC_SNOC_USB_CLK] = DEFINE_DUMMY_CLK(gcc_snoc_usb_clk),
	[GCC_SYS_NOC_AT_CLK] = DEFINE_DUMMY_CLK(gcc_sys_noc_at_clk),
	[GCC_SYS_NOC_AXI_CLK] = DEFINE_DUMMY_CLK(gcc_sys_noc_axi_clk),
	[GCC_SYS_NOC_QDSS_STM_AXI_CLK] = DEFINE_DUMMY_CLK(gcc_sys_noc_qdss_stm_axi_clk),
	[GCC_SYS_NOC_WCSS_AHB_CLK] = &gcc_sys_noc_wcss_ahb_clk.clkr,
	[GCC_SYSTEM_NOC_BFDCD_CLK_SRC] = &gcc_system_noc_bfdcd_clk_src.clkr,
	[GCC_UNIPHY0_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_uniphy0_ahb_clk),
	[GCC_UNIPHY0_SYS_CLK] = DEFINE_DUMMY_CLK(gcc_uniphy0_sys_clk),
	[GCC_UNIPHY1_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_uniphy1_ahb_clk),
	[GCC_UNIPHY1_SYS_CLK] = DEFINE_DUMMY_CLK(gcc_uniphy1_sys_clk),
	[GCC_UNIPHY_SYS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_uniphy_sys_clk_src),
	[GCC_USB0_AUX_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_aux_clk),
	[GCC_USB0_AUX_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_usb0_aux_clk_src),
	[GCC_USB0_EUD_AT_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_eud_at_clk),
	[GCC_USB0_LFPS_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_lfps_clk),
	[GCC_USB0_LFPS_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_usb0_lfps_clk_src),
	[GCC_USB0_MASTER_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_master_clk),
	[GCC_USB0_MASTER_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_usb0_master_clk_src),
	[GCC_USB0_MOCK_UTMI_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_mock_utmi_clk),
	[GCC_USB0_MOCK_UTMI_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_usb0_mock_utmi_clk_src),
	[GCC_USB0_MOCK_UTMI_DIV_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_usb0_mock_utmi_div_clk_src),
	[GCC_USB0_PHY_CFG_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_phy_cfg_ahb_clk),
	[GCC_USB0_PIPE_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_pipe_clk),
	[GCC_USB0_SLEEP_CLK] = DEFINE_DUMMY_CLK(gcc_usb0_sleep_clk),
	[GCC_WCSS_AHB_CLK_SRC] = &gcc_wcss_ahb_clk_src.clkr,
	[GCC_WCSS_AXIM_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_axim_clk),
	[GCC_WCSS_AXIS_CLK] = &gcc_wcss_axis_clk.clkr,
	[GCC_WCSS_DBG_IFC_APB_BDG_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_dbg_ifc_apb_bdg_clk),
	[GCC_WCSS_DBG_IFC_APB_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_dbg_ifc_apb_clk),
	[GCC_WCSS_DBG_IFC_ATB_BDG_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_dbg_ifc_atb_bdg_clk),
	[GCC_WCSS_DBG_IFC_ATB_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_dbg_ifc_atb_clk),
	[GCC_WCSS_DBG_IFC_NTS_BDG_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_dbg_ifc_nts_bdg_clk),
	[GCC_WCSS_DBG_IFC_NTS_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_dbg_ifc_nts_clk),
	[GCC_WCSS_ECAHB_CLK] = &gcc_wcss_ecahb_clk.clkr,
	[GCC_WCSS_MST_ASYNC_BDG_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_mst_async_bdg_clk),
	[GCC_WCSS_SLV_ASYNC_BDG_CLK] = DEFINE_DUMMY_CLK(gcc_wcss_slv_async_bdg_clk),
	[GCC_XO_CLK] = &gcc_xo_clk.clkr,
	[GCC_XO_CLK_SRC] = &gcc_xo_clk_src.clkr,
	[GCC_XO_DIV4_CLK] = &gcc_xo_div4_clk.clkr,
	[GCC_IM_SLEEP_CLK] = DEFINE_DUMMY_CLK(gcc_im_sleep_clk),
	[GCC_NSSNOC_PCNOC_1_CLK] = DEFINE_DUMMY_CLK(gcc_nssnoc_pcnoc_1_clk),
	[GCC_MEM_NOC_AHB_CLK] = DEFINE_DUMMY_CLK(gcc_mem_noc_ahb_clk),
	[GCC_MEM_NOC_APSS_AXI_CLK] = DEFINE_DUMMY_CLK(gcc_mem_noc_apss_axi_clk),
	[GCC_SNOC_QOSGEN_EXTREF_DIV_CLK_SRC] = DEFINE_DUMMY_CLK(gcc_snoc_qosgen_extref_div_clk_src),
	[GCC_MEM_NOC_QOSGEN_EXTREF_CLK] = DEFINE_DUMMY_CLK(gcc_mem_noc_qosgen_extref_clk),
	[GCC_PCIE3X2_PIPE_CLK_SRC] = DEFINE_DUMMY_CLK(pcie3x2_pipe_clk_src),
	[GCC_PCIE3X1_0_PIPE_CLK_SRC] = DEFINE_DUMMY_CLK(pcie3x1_0_pipe_clk_src),
	[GCC_PCIE3X1_1_PIPE_CLK_SRC] = DEFINE_DUMMY_CLK(pcie3x1_1_pipe_clk_src),
	[GCC_USB0_PIPE_CLK_SRC] = DEFINE_DUMMY_CLK(usb0_pipe_clk_src),
	[GPLL0_MAIN] = &gpll0_main.clkr,
	[GPLL0] = &gpll0.clkr,
	[GPLL2_MAIN] = &gpll2_main.clkr,
	[GPLL2] = &gpll2.clkr,
	[GPLL4_MAIN] = &gpll4_main.clkr,
	[GPLL4] = &gpll4.clkr,
};

static struct clk_regmap *gcc_ipq5332_clocks[] = {
	[GCC_ADSS_PWM_CLK] = &gcc_adss_pwm_clk.clkr,
	[GCC_ADSS_PWM_CLK_SRC] = &gcc_adss_pwm_clk_src.clkr,
	[GCC_AHB_CLK] = &gcc_ahb_clk.clkr,
	[GCC_APSS_AHB_CLK] = &gcc_apss_ahb_clk.clkr,
	[GCC_APSS_AHB_CLK_SRC] = &gcc_apss_ahb_clk_src.clkr,
	[GCC_APSS_AXI_CLK] = &gcc_apss_axi_clk.clkr,
	[GCC_APSS_AXI_CLK_SRC] = &gcc_apss_axi_clk_src.clkr,
	[GCC_BLSP1_AHB_CLK] = &gcc_blsp1_ahb_clk.clkr,
	[GCC_BLSP1_QUP1_I2C_APPS_CLK] = &gcc_blsp1_qup1_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP1_SPI_APPS_CLK] = &gcc_blsp1_qup1_spi_apps_clk.clkr,
	[GCC_BLSP1_QUP1_SPI_APPS_CLK_SRC] = &gcc_blsp1_qup1_spi_apps_clk_src.clkr,
	[GCC_BLSP1_QUP2_I2C_APPS_CLK] = &gcc_blsp1_qup2_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP2_SPI_APPS_CLK] = &gcc_blsp1_qup2_spi_apps_clk.clkr,
	[GCC_BLSP1_QUP2_SPI_APPS_CLK_SRC] = &gcc_blsp1_qup2_spi_apps_clk_src.clkr,
	[GCC_BLSP1_QUP3_I2C_APPS_CLK] = &gcc_blsp1_qup3_i2c_apps_clk.clkr,
	[GCC_BLSP1_QUP3_SPI_APPS_CLK] = &gcc_blsp1_qup3_spi_apps_clk.clkr,
	[GCC_BLSP1_QUP3_SPI_APPS_CLK_SRC] = &gcc_blsp1_qup3_spi_apps_clk_src.clkr,
	[GCC_BLSP1_SLEEP_CLK] = &gcc_blsp1_sleep_clk.clkr,
	[GCC_BLSP1_UART1_APPS_CLK] = &gcc_blsp1_uart1_apps_clk.clkr,
	[GCC_BLSP1_UART1_APPS_CLK_SRC] = &gcc_blsp1_uart1_apps_clk_src.clkr,
	[GCC_BLSP1_UART2_APPS_CLK] = &gcc_blsp1_uart2_apps_clk.clkr,
	[GCC_BLSP1_UART2_APPS_CLK_SRC] = &gcc_blsp1_uart2_apps_clk_src.clkr,
	[GCC_BLSP1_UART3_APPS_CLK] = &gcc_blsp1_uart3_apps_clk.clkr,
	[GCC_BLSP1_UART3_APPS_CLK_SRC] = &gcc_blsp1_uart3_apps_clk_src.clkr,
	[GCC_CE_AHB_CLK] = &gcc_ce_ahb_clk.clkr,
	[GCC_CE_AXI_CLK] = &gcc_ce_axi_clk.clkr,
	[GCC_CE_PCNOC_AHB_CLK] = &gcc_ce_pcnoc_ahb_clk.clkr,
	[GCC_CMN_12GPLL_AHB_CLK] = &gcc_cmn_12gpll_ahb_clk.clkr,
	[GCC_CMN_12GPLL_APU_CLK] = &gcc_cmn_12gpll_apu_clk.clkr,
	[GCC_CMN_12GPLL_SYS_CLK] = &gcc_cmn_12gpll_sys_clk.clkr,
	[GCC_DCC_CLK] = &gcc_dcc_clk.clkr,
	[GCC_DCC_XO_CLK] = &gcc_dcc_xo_clk.clkr,
	[GCC_GP1_CLK] = &gcc_gp1_clk.clkr,
	[GCC_GP1_CLK_SRC] = &gcc_gp1_clk_src.clkr,
	[GCC_GP2_CLK] = &gcc_gp2_clk.clkr,
	[GCC_GP2_CLK_SRC] = &gcc_gp2_clk_src.clkr,
	[GCC_LPASS_CORE_AXIM_CLK] = &gcc_lpass_core_axim_clk.clkr,
	[GCC_LPASS_SWAY_CLK] = &gcc_lpass_sway_clk.clkr,
	[GCC_LPASS_SWAY_CLK_SRC] = &gcc_lpass_sway_clk_src.clkr,
	[GCC_MDIO_AHB_CLK] = &gcc_mdio_ahb_clk.clkr,
	[GCC_MDIO_SLAVE_AHB_CLK] = &gcc_mdio_slave_ahb_clk.clkr,
	[GCC_MEM_NOC_Q6_AXI_CLK] = &gcc_mem_noc_q6_axi_clk.clkr,
	[GCC_MEM_NOC_SNOC_AXI_CLK] = &gcc_mem_noc_snoc_axi_clk.clkr,
	[GCC_MEM_NOC_TS_CLK] = &gcc_mem_noc_ts_clk.clkr,
	[GCC_NSS_TS_CLK] = &gcc_nss_ts_clk.clkr,
	[GCC_NSS_TS_CLK_SRC] = &gcc_nss_ts_clk_src.clkr,
	[GCC_NSSCC_CLK] = &gcc_nsscc_clk.clkr,
	[GCC_NSSCFG_CLK] = &gcc_nsscfg_clk.clkr,
	[GCC_NSSNOC_ATB_CLK] = &gcc_nssnoc_atb_clk.clkr,
	[GCC_NSSNOC_NSSCC_CLK] = &gcc_nssnoc_nsscc_clk.clkr,
	[GCC_NSSNOC_QOSGEN_REF_CLK] = &gcc_nssnoc_qosgen_ref_clk.clkr,
	[GCC_NSSNOC_SNOC_1_CLK] = &gcc_nssnoc_snoc_1_clk.clkr,
	[GCC_NSSNOC_SNOC_CLK] = &gcc_nssnoc_snoc_clk.clkr,
	[GCC_NSSNOC_TIMEOUT_REF_CLK] = &gcc_nssnoc_timeout_ref_clk.clkr,
	[GCC_NSSNOC_XO_DCD_CLK] = &gcc_nssnoc_xo_dcd_clk.clkr,
	[GCC_PCIE3X1_0_AHB_CLK] = &gcc_pcie3x1_0_ahb_clk.clkr,
	[GCC_PCIE3X1_0_AUX_CLK] = &gcc_pcie3x1_0_aux_clk.clkr,
	[GCC_PCIE3X1_0_AXI_CLK_SRC] = &gcc_pcie3x1_0_axi_clk_src.clkr,
	[GCC_PCIE3X1_0_AXI_M_CLK] = &gcc_pcie3x1_0_axi_m_clk.clkr,
	[GCC_PCIE3X1_0_AXI_S_BRIDGE_CLK] = &gcc_pcie3x1_0_axi_s_bridge_clk.clkr,
	[GCC_PCIE3X1_0_AXI_S_CLK] = &gcc_pcie3x1_0_axi_s_clk.clkr,
	[GCC_PCIE3X1_0_PIPE_CLK] = &gcc_pcie3x1_0_pipe_clk.clkr,
	[GCC_PCIE3X1_0_RCHG_CLK] = &gcc_pcie3x1_0_rchg_clk.clkr,
	[GCC_PCIE3X1_0_RCHG_CLK_SRC] = &gcc_pcie3x1_0_rchg_clk_src.clkr,
	[GCC_PCIE3X1_1_AHB_CLK] = &gcc_pcie3x1_1_ahb_clk.clkr,
	[GCC_PCIE3X1_1_AUX_CLK] = &gcc_pcie3x1_1_aux_clk.clkr,
	[GCC_PCIE3X1_1_AXI_CLK_SRC] = &gcc_pcie3x1_1_axi_clk_src.clkr,
	[GCC_PCIE3X1_1_AXI_M_CLK] = &gcc_pcie3x1_1_axi_m_clk.clkr,
	[GCC_PCIE3X1_1_AXI_S_BRIDGE_CLK] = &gcc_pcie3x1_1_axi_s_bridge_clk.clkr,
	[GCC_PCIE3X1_1_AXI_S_CLK] = &gcc_pcie3x1_1_axi_s_clk.clkr,
	[GCC_PCIE3X1_1_PIPE_CLK] = &gcc_pcie3x1_1_pipe_clk.clkr,
	[GCC_PCIE3X1_1_RCHG_CLK] = &gcc_pcie3x1_1_rchg_clk.clkr,
	[GCC_PCIE3X1_1_RCHG_CLK_SRC] = &gcc_pcie3x1_1_rchg_clk_src.clkr,
	[GCC_PCIE3X1_PHY_AHB_CLK] = &gcc_pcie3x1_phy_ahb_clk.clkr,
	[GCC_PCIE3X2_AHB_CLK] = &gcc_pcie3x2_ahb_clk.clkr,
	[GCC_PCIE3X2_AUX_CLK] = &gcc_pcie3x2_aux_clk.clkr,
	[GCC_PCIE3X2_AXI_M_CLK] = &gcc_pcie3x2_axi_m_clk.clkr,
	[GCC_PCIE3X2_AXI_M_CLK_SRC] = &gcc_pcie3x2_axi_m_clk_src.clkr,
	[GCC_PCIE3X2_AXI_S_BRIDGE_CLK] = &gcc_pcie3x2_axi_s_bridge_clk.clkr,
	[GCC_PCIE3X2_AXI_S_CLK] = &gcc_pcie3x2_axi_s_clk.clkr,
	[GCC_PCIE3X2_AXI_S_CLK_SRC] = &gcc_pcie3x2_axi_s_clk_src.clkr,
	[GCC_PCIE3X2_PHY_AHB_CLK] = &gcc_pcie3x2_phy_ahb_clk.clkr,
	[GCC_PCIE3X2_PIPE_CLK] = &gcc_pcie3x2_pipe_clk.clkr,
	[GCC_PCIE3X2_RCHG_CLK] = &gcc_pcie3x2_rchg_clk.clkr,
	[GCC_PCIE3X2_RCHG_CLK_SRC] = &gcc_pcie3x2_rchg_clk_src.clkr,
	[GCC_PCIE_AUX_CLK_SRC] = &gcc_pcie_aux_clk_src.clkr,
	[GCC_PCNOC_AT_CLK] = &gcc_pcnoc_at_clk.clkr,
	[GCC_PCNOC_BFDCD_CLK_SRC] = &gcc_pcnoc_bfdcd_clk_src.clkr,
	[GCC_PCNOC_DCC_CLK] = &gcc_pcnoc_dcc_clk.clkr,
	[GCC_PCNOC_LPASS_CLK] = &gcc_pcnoc_lpass_clk.clkr,
	[GCC_PCNOC_TS_CLK] = &gcc_pcnoc_ts_clk.clkr,
	[GCC_PRNG_AHB_CLK] = &gcc_prng_ahb_clk.clkr,
	[GCC_Q6_AHB_CLK] = &gcc_q6_ahb_clk.clkr,
	[GCC_Q6_AHB_S_CLK] = &gcc_q6_ahb_s_clk.clkr,
	[GCC_Q6_AXIM_CLK] = &gcc_q6_axim_clk.clkr,
	[GCC_Q6_AXIM_CLK_SRC] = &gcc_q6_axim_clk_src.clkr,
	[GCC_Q6_AXIS_CLK] = &gcc_q6_axis_clk.clkr,
	[GCC_Q6_TSCTR_1TO2_CLK] = &gcc_q6_tsctr_1to2_clk.clkr,
	[GCC_Q6SS_ATBM_CLK] = &gcc_q6ss_atbm_clk.clkr,
	[GCC_Q6SS_PCLKDBG_CLK] = &gcc_q6ss_pclkdbg_clk.clkr,
	[GCC_Q6SS_TRIG_CLK] = &gcc_q6ss_trig_clk.clkr,
	[GCC_QDSS_APB2JTAG_CLK] = &gcc_qdss_apb2jtag_clk.clkr,
	[GCC_QDSS_AT_CLK] = &gcc_qdss_at_clk.clkr,
	[GCC_QDSS_AT_CLK_SRC] = &gcc_qdss_at_clk_src.clkr,
	[GCC_QDSS_CFG_AHB_CLK] = &gcc_qdss_cfg_ahb_clk.clkr,
	[GCC_QDSS_DAP_AHB_CLK] = &gcc_qdss_dap_ahb_clk.clkr,
	[GCC_QDSS_DAP_CLK] = &gcc_qdss_dap_clk.clkr,
	[GCC_QDSS_DAP_DIV_CLK_SRC] = &gcc_qdss_dap_div_clk_src.clkr,
	[GCC_QDSS_ETR_USB_CLK] = &gcc_qdss_etr_usb_clk.clkr,
	[GCC_QDSS_EUD_AT_CLK] = &gcc_qdss_eud_at_clk.clkr,
	[GCC_QDSS_STM_CLK] = &gcc_qdss_stm_clk.clkr,
	[GCC_QDSS_STM_CLK_SRC] = &gcc_qdss_stm_clk_src.clkr,
	[GCC_QDSS_TRACECLKIN_CLK] = &gcc_qdss_traceclkin_clk.clkr,
	[GCC_QDSS_TRACECLKIN_CLK_SRC] = &gcc_qdss_traceclkin_clk_src.clkr,
	[GCC_QDSS_TS_CLK] = &gcc_qdss_ts_clk.clkr,
	[GCC_QDSS_TSCTR_CLK_SRC] = &gcc_qdss_tsctr_clk_src.clkr,
	[GCC_QDSS_TSCTR_DIV16_CLK] = &gcc_qdss_tsctr_div16_clk.clkr,
	[GCC_QDSS_TSCTR_DIV2_CLK] = &gcc_qdss_tsctr_div2_clk.clkr,
	[GCC_QDSS_TSCTR_DIV3_CLK] = &gcc_qdss_tsctr_div3_clk.clkr,
	[GCC_QDSS_TSCTR_DIV4_CLK] = &gcc_qdss_tsctr_div4_clk.clkr,
	[GCC_QDSS_TSCTR_DIV8_CLK] = &gcc_qdss_tsctr_div8_clk.clkr,
	[GCC_QPIC_AHB_CLK] = &gcc_qpic_ahb_clk.clkr,
	[GCC_QPIC_CLK] = &gcc_qpic_clk.clkr,
	[GCC_QPIC_IO_MACRO_CLK] = &gcc_qpic_io_macro_clk.clkr,
	[GCC_QPIC_IO_MACRO_CLK_SRC] = &gcc_qpic_io_macro_clk_src.clkr,
	[GCC_QPIC_SLEEP_CLK] = &gcc_qpic_sleep_clk.clkr,
	[GCC_SDCC1_AHB_CLK] = &gcc_sdcc1_ahb_clk.clkr,
	[GCC_SDCC1_APPS_CLK] = &gcc_sdcc1_apps_clk.clkr,
	[GCC_SDCC1_APPS_CLK_SRC] = &gcc_sdcc1_apps_clk_src.clkr,
	[GCC_SLEEP_CLK_SRC] = &gcc_sleep_clk_src.clkr,
	[GCC_SNOC_LPASS_CFG_CLK] = &gcc_snoc_lpass_cfg_clk.clkr,
	[GCC_SNOC_NSSNOC_1_CLK] = &gcc_snoc_nssnoc_1_clk.clkr,
	[GCC_SNOC_NSSNOC_CLK] = &gcc_snoc_nssnoc_clk.clkr,
	[GCC_SNOC_PCIE3_1LANE_1_M_CLK] = &gcc_snoc_pcie3_1lane_1_m_clk.clkr,
	[GCC_SNOC_PCIE3_1LANE_1_S_CLK] = &gcc_snoc_pcie3_1lane_1_s_clk.clkr,
	[GCC_SNOC_PCIE3_1LANE_M_CLK] = &gcc_snoc_pcie3_1lane_m_clk.clkr,
	[GCC_SNOC_PCIE3_1LANE_S_CLK] = &gcc_snoc_pcie3_1lane_s_clk.clkr,
	[GCC_SNOC_PCIE3_2LANE_M_CLK] = &gcc_snoc_pcie3_2lane_m_clk.clkr,
	[GCC_SNOC_PCIE3_2LANE_S_CLK] = &gcc_snoc_pcie3_2lane_s_clk.clkr,
	[GCC_SNOC_PCNOC_AHB_CLK] = &gcc_snoc_pcnoc_ahb_clk.clkr,
	[GCC_SNOC_TS_CLK] = &gcc_snoc_ts_clk.clkr,
	[GCC_SNOC_USB_CLK] = &gcc_snoc_usb_clk.clkr,
	[GCC_SYS_NOC_AT_CLK] = &gcc_sys_noc_at_clk.clkr,
	[GCC_SYS_NOC_AXI_CLK] = &gcc_sys_noc_axi_clk.clkr,
	[GCC_SYS_NOC_QDSS_STM_AXI_CLK] = &gcc_sys_noc_qdss_stm_axi_clk.clkr,
	[GCC_SYS_NOC_WCSS_AHB_CLK] = &gcc_sys_noc_wcss_ahb_clk.clkr,
	[GCC_SYSTEM_NOC_BFDCD_CLK_SRC] = &gcc_system_noc_bfdcd_clk_src.clkr,
	[GCC_UNIPHY0_AHB_CLK] = &gcc_uniphy0_ahb_clk.clkr,
	[GCC_UNIPHY0_SYS_CLK] = &gcc_uniphy0_sys_clk.clkr,
	[GCC_UNIPHY1_AHB_CLK] = &gcc_uniphy1_ahb_clk.clkr,
	[GCC_UNIPHY1_SYS_CLK] = &gcc_uniphy1_sys_clk.clkr,
	[GCC_UNIPHY_SYS_CLK_SRC] = &gcc_uniphy_sys_clk_src.clkr,
	[GCC_USB0_AUX_CLK] = &gcc_usb0_aux_clk.clkr,
	[GCC_USB0_AUX_CLK_SRC] = &gcc_usb0_aux_clk_src.clkr,
	[GCC_USB0_EUD_AT_CLK] = &gcc_usb0_eud_at_clk.clkr,
	[GCC_USB0_LFPS_CLK] = &gcc_usb0_lfps_clk.clkr,
	[GCC_USB0_LFPS_CLK_SRC] = &gcc_usb0_lfps_clk_src.clkr,
	[GCC_USB0_MASTER_CLK] = &gcc_usb0_master_clk.clkr,
	[GCC_USB0_MASTER_CLK_SRC] = &gcc_usb0_master_clk_src.clkr,
	[GCC_USB0_MOCK_UTMI_CLK] = &gcc_usb0_mock_utmi_clk.clkr,
	[GCC_USB0_MOCK_UTMI_CLK_SRC] = &gcc_usb0_mock_utmi_clk_src.clkr,
	[GCC_USB0_MOCK_UTMI_DIV_CLK_SRC] = &gcc_usb0_mock_utmi_div_clk_src.clkr,
	[GCC_USB0_PHY_CFG_AHB_CLK] = &gcc_usb0_phy_cfg_ahb_clk.clkr,
	[GCC_USB0_PIPE_CLK] = &gcc_usb0_pipe_clk.clkr,
	[GCC_USB0_SLEEP_CLK] = &gcc_usb0_sleep_clk.clkr,
	[GCC_WCSS_AHB_CLK_SRC] = &gcc_wcss_ahb_clk_src.clkr,
	[GCC_WCSS_AXIM_CLK] = &gcc_wcss_axim_clk.clkr,
	[GCC_WCSS_AXIS_CLK] = &gcc_wcss_axis_clk.clkr,
	[GCC_WCSS_DBG_IFC_APB_BDG_CLK] = &gcc_wcss_dbg_ifc_apb_bdg_clk.clkr,
	[GCC_WCSS_DBG_IFC_APB_CLK] = &gcc_wcss_dbg_ifc_apb_clk.clkr,
	[GCC_WCSS_DBG_IFC_ATB_BDG_CLK] = &gcc_wcss_dbg_ifc_atb_bdg_clk.clkr,
	[GCC_WCSS_DBG_IFC_ATB_CLK] = &gcc_wcss_dbg_ifc_atb_clk.clkr,
	[GCC_WCSS_DBG_IFC_NTS_BDG_CLK] = &gcc_wcss_dbg_ifc_nts_bdg_clk.clkr,
	[GCC_WCSS_DBG_IFC_NTS_CLK] = &gcc_wcss_dbg_ifc_nts_clk.clkr,
	[GCC_WCSS_ECAHB_CLK] = &gcc_wcss_ecahb_clk.clkr,
	[GCC_WCSS_MST_ASYNC_BDG_CLK] = &gcc_wcss_mst_async_bdg_clk.clkr,
	[GCC_WCSS_SLV_ASYNC_BDG_CLK] = &gcc_wcss_slv_async_bdg_clk.clkr,
	[GCC_XO_CLK] = &gcc_xo_clk.clkr,
	[GCC_XO_CLK_SRC] = &gcc_xo_clk_src.clkr,
	[GCC_XO_DIV4_CLK] = &gcc_xo_div4_clk.clkr,
	[GCC_IM_SLEEP_CLK] = &gcc_im_sleep_clk.clkr,
	[GCC_NSSNOC_PCNOC_1_CLK] = &gcc_nssnoc_pcnoc_1_clk.clkr,
	[GCC_MEM_NOC_AHB_CLK] = &gcc_mem_noc_ahb_clk.clkr,
	[GCC_MEM_NOC_APSS_AXI_CLK] = &gcc_mem_noc_apss_axi_clk.clkr,
	[GCC_SNOC_QOSGEN_EXTREF_DIV_CLK_SRC] = &gcc_snoc_qosgen_extref_div_clk_src.clkr,
	[GCC_MEM_NOC_QOSGEN_EXTREF_CLK] = &gcc_mem_noc_qosgen_extref_clk.clkr,
	[GCC_PCIE3X2_PIPE_CLK_SRC] = &pcie3x2_pipe_clk_src.clkr,
	[GCC_PCIE3X1_0_PIPE_CLK_SRC] = &pcie3x1_0_pipe_clk_src.clkr,
	[GCC_PCIE3X1_1_PIPE_CLK_SRC] = &pcie3x1_1_pipe_clk_src.clkr,
	[GCC_USB0_PIPE_CLK_SRC] = &usb0_pipe_clk_src.clkr,
	[GPLL0_MAIN] = &gpll0_main.clkr,
	[GPLL0] = &gpll0.clkr,
	[GPLL2_MAIN] = &gpll2_main.clkr,
	[GPLL2] = &gpll2.clkr,
	[GPLL4_MAIN] = &gpll4_main.clkr,
	[GPLL4] = &gpll4.clkr,
};

static const struct qcom_reset_map gcc_ipq5332_resets[] = {
	[GCC_ADSS_BCR] = { 0x1c000 },
	[GCC_ADSS_PWM_CLK_ARES] = { 0x1c00c, 2 },
	[GCC_AHB_CLK_ARES] = { 0x34024, 2 },
	[GCC_APC0_VOLTAGE_DROOP_DETECTOR_BCR] = { 0x38000 },
	[GCC_APC0_VOLTAGE_DROOP_DETECTOR_GPLL0_CLK_ARES] = { 0x3800c, 2 },
	[GCC_APSS_AHB_CLK_ARES] = { 0x24018, 2 },
	[GCC_APSS_AXI_CLK_ARES] = { 0x2401c, 2 },
	[GCC_BLSP1_AHB_CLK_ARES] = { 0x1008, 2 },
	[GCC_BLSP1_BCR] = { 0x1000 },
	[GCC_BLSP1_QUP1_BCR] = { 0x2000 },
	[GCC_BLSP1_QUP1_I2C_APPS_CLK_ARES] = { 0x2024, 2 },
	[GCC_BLSP1_QUP1_SPI_APPS_CLK_ARES] = { 0x2020, 2 },
	[GCC_BLSP1_QUP2_BCR] = { 0x3000 },
	[GCC_BLSP1_QUP2_I2C_APPS_CLK_ARES] = { 0x3024, 2 },
	[GCC_BLSP1_QUP2_SPI_APPS_CLK_ARES] = { 0x3020, 2 },
	[GCC_BLSP1_QUP3_BCR] = { 0x4000 },
	[GCC_BLSP1_QUP3_I2C_APPS_CLK_ARES] = { 0x4024, 2 },
	[GCC_BLSP1_QUP3_SPI_APPS_CLK_ARES] = { 0x4020, 2 },
	[GCC_BLSP1_SLEEP_CLK_ARES] = { 0x1010, 2 },
	[GCC_BLSP1_UART1_APPS_CLK_ARES] = { 0x2040, 2 },
	[GCC_BLSP1_UART1_BCR] = { 0x2028 },
	[GCC_BLSP1_UART2_APPS_CLK_ARES] = { 0x3040, 2 },
	[GCC_BLSP1_UART2_BCR] = { 0x3028 },
	[GCC_BLSP1_UART3_APPS_CLK_ARES] = { 0x4054, 2 },
	[GCC_BLSP1_UART3_BCR] = { 0x4028 },
	[GCC_CE_BCR] = { 0x18008 },
	[GCC_CMN_BLK_BCR] = { 0x3a000 },
	[GCC_CMN_LDO0_BCR] = { 0x1d000 },
	[GCC_CMN_LDO1_BCR] = { 0x1d008 },
	[GCC_DCC_BCR] = { 0x35000 },
	[GCC_GP1_CLK_ARES] = { 0x8018, 2 },
	[GCC_GP2_CLK_ARES] = { 0x9018, 2 },
	[GCC_LPASS_BCR] = { 0x27000 },
	[GCC_LPASS_CORE_AXIM_CLK_ARES] = { 0x27018, 2 },
	[GCC_LPASS_SWAY_CLK_ARES] = { 0x27014, 2 },
	[GCC_MDIOM_BCR] = { 0x12000 },
	[GCC_MDIOS_BCR] = { 0x12008 },
	[GCC_NSS_BCR] = { 0x17000 },
	[GCC_NSS_TS_CLK_ARES] = { 0x17018, 2 },
	[GCC_NSSCC_CLK_ARES] = { 0x17034, 2 },
	[GCC_NSSCFG_CLK_ARES] = { 0x1702c, 2 },
	[GCC_NSSNOC_ATB_CLK_ARES] = { 0x17014, 2 },
	[GCC_NSSNOC_NSSCC_CLK_ARES] = { 0x17030, 2 },
	[GCC_NSSNOC_QOSGEN_REF_CLK_ARES] = { 0x1701c, 2 },
	[GCC_NSSNOC_SNOC_1_CLK_ARES] = { 0x1707c, 2 },
	[GCC_NSSNOC_SNOC_CLK_ARES] = { 0x17028, 2 },
	[GCC_NSSNOC_TIMEOUT_REF_CLK_ARES] = { 0x17020, 2 },
	[GCC_NSSNOC_XO_DCD_CLK_ARES] = { 0x17074, 2 },
	[GCC_PCIE3X1_0_AHB_CLK_ARES] = { 0x29030, 2 },
	[GCC_PCIE3X1_0_AUX_CLK_ARES] = { 0x29070, 2 },
	[GCC_PCIE3X1_0_AXI_M_CLK_ARES] = { 0x29038, 2 },
	[GCC_PCIE3X1_0_AXI_S_BRIDGE_CLK_ARES] = { 0x29048, 2 },
	[GCC_PCIE3X1_0_AXI_S_CLK_ARES] = { 0x29040, 2 },
	[GCC_PCIE3X1_0_BCR] = { 0x29000 },
	[GCC_PCIE3X1_0_LINK_DOWN_BCR] = { 0x29054 },
	[GCC_PCIE3X1_0_PHY_BCR] = { 0x29060 },
	[GCC_PCIE3X1_0_PHY_PHY_BCR] = { 0x2905c },
	[GCC_PCIE3X1_1_AHB_CLK_ARES] = { 0x2a00c, 2 },
	[GCC_PCIE3X1_1_AUX_CLK_ARES] = { 0x2a070, 2 },
	[GCC_PCIE3X1_1_AXI_M_CLK_ARES] = { 0x2a014, 2 },
	[GCC_PCIE3X1_1_AXI_S_BRIDGE_CLK_ARES] = { 0x2a024, 2 },
	[GCC_PCIE3X1_1_AXI_S_CLK_ARES] = { 0x2a01c, 2 },
	[GCC_PCIE3X1_1_BCR] = { 0x2a000 },
	[GCC_PCIE3X1_1_LINK_DOWN_BCR] = { 0x2a028 },
	[GCC_PCIE3X1_1_PHY_BCR] = { 0x2a030 },
	[GCC_PCIE3X1_1_PHY_PHY_BCR] = { 0x2a02c },
	[GCC_PCIE3X1_PHY_AHB_CLK_ARES] = { 0x29078, 2 },
	[GCC_PCIE3X2_AHB_CLK_ARES] = { 0x28030, 2 },
	[GCC_PCIE3X2_AUX_CLK_ARES] = { 0x28070, 2 },
	[GCC_PCIE3X2_AXI_M_CLK_ARES] = { 0x28038, 2 },
	[GCC_PCIE3X2_AXI_S_BRIDGE_CLK_ARES] = { 0x28048, 2 },
	[GCC_PCIE3X2_AXI_S_CLK_ARES] = { 0x28040, 2 },
	[GCC_PCIE3X2_BCR] = { 0x28000 },
	[GCC_PCIE3X2_LINK_DOWN_BCR] = { 0x28054 },
	[GCC_PCIE3X2_PHY_AHB_CLK_ARES] = { 0x28080, 2 },
	[GCC_PCIE3X2_PHY_BCR] = { 0x28060 },
	[GCC_PCIE3X2PHY_PHY_BCR] = { 0x2805c },
	[GCC_PCNOC_BCR] = { 0x31000 },
	[GCC_PCNOC_LPASS_CLK_ARES] = { 0x31020, 2 },
	[GCC_PRNG_AHB_CLK_ARES] = { 0x13024, 2 },
	[GCC_PRNG_BCR] = { 0x13020 },
	[GCC_Q6_AHB_CLK_ARES] = { 0x25014, 2 },
	[GCC_Q6_AHB_S_CLK_ARES] = { 0x25018, 2 },
	[GCC_Q6_AXIM_CLK_ARES] = { 0x2500c, 2 },
	[GCC_Q6_AXIS_CLK_ARES] = { 0x25010, 2 },
	[GCC_Q6_TSCTR_1TO2_CLK_ARES] = { 0x25020, 2 },
	[GCC_Q6SS_ATBM_CLK_ARES] = { 0x2501c, 2 },
	[GCC_Q6SS_PCLKDBG_CLK_ARES] = { 0x25024, 2 },
	[GCC_Q6SS_TRIG_CLK_ARES] = { 0x250a0, 2 },
	[GCC_QDSS_APB2JTAG_CLK_ARES] = { 0x2d060, 2 },
	[GCC_QDSS_AT_CLK_ARES] = { 0x2d038, 2 },
	[GCC_QDSS_BCR] = { 0x2d000 },
	[GCC_QDSS_CFG_AHB_CLK_ARES] = { 0x2d06c, 2 },
	[GCC_QDSS_DAP_AHB_CLK_ARES] = { 0x2d068, 2 },
	[GCC_QDSS_DAP_CLK_ARES] = { 0x2d05c, 2 },
	[GCC_QDSS_ETR_USB_CLK_ARES] = { 0x2d064, 2 },
	[GCC_QDSS_EUD_AT_CLK_ARES] = { 0x2d070, 2 },
	[GCC_QDSS_STM_CLK_ARES] = { 0x2d040, 2 },
	[GCC_QDSS_TRACECLKIN_CLK_ARES] = { 0x2d044, 2 },
	[GCC_QDSS_TS_CLK_ARES] = { 0x2d078, 2 },
	[GCC_QDSS_TSCTR_DIV16_CLK_ARES] = { 0x2d058, 2 },
	[GCC_QDSS_TSCTR_DIV2_CLK_ARES] = { 0x2d048, 2 },
	[GCC_QDSS_TSCTR_DIV3_CLK_ARES] = { 0x2d04c, 2 },
	[GCC_QDSS_TSCTR_DIV4_CLK_ARES] = { 0x2d050, 2 },
	[GCC_QDSS_TSCTR_DIV8_CLK_ARES] = { 0x2d054, 2 },
	[GCC_QPIC_AHB_CLK_ARES] = { 0x32010, 2 },
	[GCC_QPIC_CLK_ARES] = { 0x32014, 2 },
	[GCC_QPIC_BCR] = { 0x32000 },
	[GCC_QPIC_IO_MACRO_CLK_ARES] = { 0x3200c, 2 },
	[GCC_QPIC_SLEEP_CLK_ARES] = { 0x3201c, 2 },
	[GCC_QUSB2_0_PHY_BCR] = { 0x2c068 },
	[GCC_SDCC1_AHB_CLK_ARES] = { 0x33034, 2 },
	[GCC_SDCC1_APPS_CLK_ARES] = { 0x3302c, 2 },
	[GCC_SDCC_BCR] = { 0x33000 },
	[GCC_SNOC_BCR] = { 0x2e000 },
	[GCC_SNOC_LPASS_CFG_CLK_ARES] = { 0x2e028, 2 },
	[GCC_SNOC_NSSNOC_1_CLK_ARES] = { 0x17090, 2 },
	[GCC_SNOC_NSSNOC_CLK_ARES] = { 0x17084, 2 },
	[GCC_SYS_NOC_QDSS_STM_AXI_CLK_ARES] = { 0x2e034, 2 },
	[GCC_SYS_NOC_WCSS_AHB_CLK_ARES] = { 0x2e030, 2 },
	[GCC_UNIPHY0_AHB_CLK_ARES] = { 0x16010, 2 },
	[GCC_UNIPHY0_BCR] = { 0x16000 },
	[GCC_UNIPHY0_SYS_CLK_ARES] = { 0x1600c, 2 },
	[GCC_UNIPHY1_AHB_CLK_ARES] = { 0x1601c, 2 },
	[GCC_UNIPHY1_BCR] = { 0x16014 },
	[GCC_UNIPHY1_SYS_CLK_ARES] = { 0x16018, 2 },
	[GCC_USB0_AUX_CLK_ARES] = { 0x2c050, 2 },
	[GCC_USB0_EUD_AT_CLK_ARES] = { 0x30004, 2 },
	[GCC_USB0_LFPS_CLK_ARES] = { 0x2c090, 2 },
	[GCC_USB0_MASTER_CLK_ARES] = { 0x2c048, 2 },
	[GCC_USB0_MOCK_UTMI_CLK_ARES] = { 0x2c054, 2 },
	[GCC_USB0_PHY_BCR] = { 0x2c06c },
	[GCC_USB0_PHY_CFG_AHB_CLK_ARES] = { 0x2c05c, 2 },
	[GCC_USB0_SLEEP_CLK_ARES] = { 0x2c058, 2 },
	[GCC_USB3PHY_0_PHY_BCR] = { 0x2c070 },
	[GCC_USB_BCR] = { 0x2c000 },
	[GCC_WCSS_AXIM_CLK_ARES] = { 0x2505c, 2 },
	[GCC_WCSS_AXIS_CLK_ARES] = { 0x25060, 2 },
	[GCC_WCSS_BCR] = { 0x18004 },
	[GCC_WCSS_DBG_IFC_APB_BDG_CLK_ARES] = { 0x25048, 2 },
	[GCC_WCSS_DBG_IFC_APB_CLK_ARES] = { 0x25038, 2 },
	[GCC_WCSS_DBG_IFC_ATB_BDG_CLK_ARES] = { 0x2504c, 2 },
	[GCC_WCSS_DBG_IFC_ATB_CLK_ARES] = { 0x2503c, 2 },
	[GCC_WCSS_DBG_IFC_NTS_BDG_CLK_ARES] = { 0x25050, 2 },
	[GCC_WCSS_DBG_IFC_NTS_CLK_ARES] = { 0x25040, 2 },
	[GCC_WCSS_ECAHB_CLK_ARES] = { 0x25058, 2 },
	[GCC_WCSS_MST_ASYNC_BDG_CLK_ARES] = { 0x2e0b0, 2 },
	[GCC_WCSS_Q6_BCR] = { 0x18000 },
	[GCC_WCSS_SLV_ASYNC_BDG_CLK_ARES] = { 0x2e0b4, 2 },
	[GCC_XO_CLK_ARES] = { 0x34018, 2 },
	[GCC_XO_DIV4_CLK_ARES] = { 0x3401c, 2 },
	[GCC_Q6SS_DBG_ARES] = { 0x25094 },
	[GCC_WCSS_DBG_BDG_ARES] = { 0x25098, 0 },
	[GCC_WCSS_DBG_ARES] = { 0x25098, 1 },
	[GCC_WCSS_AXI_S_ARES] = { 0x25098, 2 },
	[GCC_WCSS_AXI_M_ARES] = { 0x25098, 3 },
	[GCC_WCSSAON_ARES] = { 0x2509C },
	[GCC_PCIE3X2_PIPE_ARES] = { 0x28058, 0 },
	[GCC_PCIE3X2_CORE_STICKY_ARES] = { 0x28058, 1 },
	[GCC_PCIE3X2_AXI_S_STICKY_ARES] = { 0x28058, 2 },
	[GCC_PCIE3X2_AXI_M_STICKY_ARES] = { 0x28058, 3 },
	[GCC_PCIE3X1_0_PIPE_ARES] = { 0x29058, 0 },
	[GCC_PCIE3X1_0_CORE_STICKY_ARES] = { 0x29058, 1 },
	[GCC_PCIE3X1_0_AXI_S_STICKY_ARES] = { 0x29058, 2 },
	[GCC_PCIE3X1_0_AXI_M_STICKY_ARES] = { 0x29058, 3 },
	[GCC_PCIE3X1_1_PIPE_ARES] = { 0x2a058, 0 },
	[GCC_PCIE3X1_1_CORE_STICKY_ARES] = { 0x2a058, 1 },
	[GCC_PCIE3X1_1_AXI_S_STICKY_ARES] = { 0x2a058, 2 },
	[GCC_PCIE3X1_1_AXI_M_STICKY_ARES] = { 0x2a058, 3 },
	[GCC_IM_SLEEP_CLK_ARES] = { 0x34020, 2 },
	[GCC_NSSNOC_PCNOC_1_CLK_ARES] = { 0x17080, 2 },
	[GCC_UNIPHY0_XPCS_ARES] = { 0x16050 },
	[GCC_UNIPHY1_XPCS_ARES] = { 0x16060 },
};

static const struct regmap_config gcc_ipq5332_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.max_register = 0x3f024,
	.fast_io = true,
};

static struct clk_hw *gcc_ipq5332_hws[] = {
	&gpll0_div2.hw,
	&gcc_xo_div4_clk_src.hw,
	&gcc_system_noc_bfdcd_div2_clk_src.hw,
	&gcc_qdss_tsctr_div2_clk_src.hw,
	&gcc_qdss_tsctr_div3_clk_src.hw,
	&gcc_qdss_tsctr_div4_clk_src.hw,
	&gcc_qdss_tsctr_div8_clk_src.hw,
	&gcc_qdss_tsctr_div16_clk_src.hw,
	&gcc_eud_at_div_clk_src.hw,
};

static const struct qcom_cc_desc gcc_ipq5332_desc = {
	.config = &gcc_ipq5332_regmap_config,
	.clks = gcc_ipq5332_clocks,
	.num_clks = ARRAY_SIZE(gcc_ipq5332_clocks),
	.resets = gcc_ipq5332_resets,
	.num_resets = ARRAY_SIZE(gcc_ipq5332_resets),
	.clk_hws = gcc_ipq5332_hws,
	.num_clk_hws = ARRAY_SIZE(gcc_ipq5332_hws),
};

static const struct qcom_cc_desc gcc_ipq5332_dummy_desc = {
	.config = &gcc_ipq5332_regmap_config,
	.clks = gcc_ipq5332_dummy_clocks,
	.num_clks = ARRAY_SIZE(gcc_ipq5332_dummy_clocks),
	.resets = gcc_ipq5332_resets,
	.num_resets = ARRAY_SIZE(gcc_ipq5332_resets),
	.clk_hws = gcc_ipq5332_hws,
	.num_clk_hws = ARRAY_SIZE(gcc_ipq5332_hws),
};


static const struct of_device_id gcc_ipq5332_match_table[] = {
	{ .compatible = "qcom,gcc-ipq5332" },
	{ }
};
MODULE_DEVICE_TABLE(of, gcc_ipq5332_match_table);

static int gcc_ipq5332_probe(struct platform_device *pdev)
{
	struct qcom_cc_desc desc = gcc_ipq5332_desc;
	struct device_node *np = (&pdev->dev)->of_node;
	struct regmap *regmap;
	int ret;

	if (of_property_read_bool(np, "gcc-use-dummy"))
		desc = gcc_ipq5332_dummy_desc;

	regmap = qcom_cc_map(pdev, &desc);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	ret = qcom_cc_really_probe(pdev, &desc, regmap);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register GCC clocks\n");
		return ret;
	}

	dev_info(&pdev->dev, "Registered GCC clocks\n");

	return ret;
}

static struct platform_driver gcc_ipq5332_driver = {
	.probe = gcc_ipq5332_probe,
	.driver = {
		.name = "gcc-ipq5332",
		.of_match_table = gcc_ipq5332_match_table,
	},
};

static int __init gcc_ipq5332_init(void)
{
	return platform_driver_register(&gcc_ipq5332_driver);
}
core_initcall(gcc_ipq5332_init);

static void __exit gcc_ipq5332_exit(void)
{
	platform_driver_unregister(&gcc_ipq5332_driver);
}
module_exit(gcc_ipq5332_exit);

MODULE_DESCRIPTION("Qualcomm Technologies, Inc. GCC IPQ5332 Driver");
MODULE_LICENSE("Dual BSD/GPLv2");
MODULE_ALIAS("platform:gcc-ipq5332");
