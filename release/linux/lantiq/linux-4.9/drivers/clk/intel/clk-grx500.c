/*
 *  Copyright (C) 2016 Intel Corporation.
 *  Zhu YiXin <Yixin.zhu@intel.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  The full GNU General Public License is included in this distribution in
 *  the file called "COPYING".
 */

#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/spinlock.h>
#include <dt-bindings/clock/intel,grx500-clk.h>
#include "clk_api.h"

static DEFINE_SPINLOCK(pll0a_lock);
static DEFINE_SPINLOCK(pll0b_lock);

/* clock speeds */
#define CLK_16M		16000000
#define CLK_33M		33333333
#define CLK_50M		50000000
#define CLK_100M	100000000
#define CLK_133M	133333333
#define CLK_150M	150000000
#define CLK_166M	166666666
#define CLK_200M	200000000
#define CLK_250M	250000000
#define CLK_300M	300000000
#define CLK_333M	333333333
#define CLK_400M	400000000
#define CLK_500M	500000000
#define CLK_600M	600000000
#define CLK_666M	666666666
#define CLK_720M	720000000
#define CLK_800M	800000000
#define CLK_1000M	1000000000
#define CLK_1200M	1200000000
#define CLK_2000M	2000000000UL
#define CLK_2400M	2400000000UL

/* clock out speeds */
#define CLK_8M_192K	8192000
#define CLK_25M		25000000
#define CLK_40M		40000000

#define VOICE_CLK_VAL	0x2

static const struct gate_dummy_clk_data grx500_clk_gate_i2c_data __initconst = {
	0
};

static void __init grx500_clk_gate_i2c_setup(struct device_node *node)
{
	gate_dummy_clk_setup(node, &grx500_clk_gate_i2c_data);
}

CLK_OF_DECLARE(grx500_gatei2cclk, INTEL_GRX500_DT_GATE_I2C_CLK,
	       grx500_clk_gate_i2c_setup);

static const struct fixed_rate_clk_data grx500_clk_voice_data __initconst = {
	.shift = VOICE_CLK_SHIFT,
	.width = VOICE_CLK_WIDTH,
	.fixed_rate = CLK_8M_192K,
	.setval = VOICE_CLK_VAL,
};

static void __init grx500_clk_voice_setup(struct device_node *node)
{
	fixed_rate_clk_setup(node, &grx500_clk_voice_data);
}

CLK_OF_DECLARE(grx500_voiceclk, INTEL_GRX500_DT_VOICE_CLK,
	       grx500_clk_voice_setup);

static const struct gate_clk_data grx500_clk_gate2_data __initconst = {
	.mask = GATE2_CLK_MASK,
	.def_onoff = BIT(GATE_PCIE0_CTRL) | BIT(GATE_PCIE1_CTRL)
		| BIT(GATE_PCIE2_CTRL),
	.reg_size = 32,
	.flags = CLK_INIT_DEF_CFG_REQ,
};

static void __init grx500_clk_gate2_setup(struct device_node *node)
{
	gate_clk_setup(node, &grx500_clk_gate2_data);
}

CLK_OF_DECLARE(grx500_gate2clk, INTEL_GRX500_DT_GATE2_CLK,
	       grx500_clk_gate2_setup);

static const struct gate_clk_data grx500_clk_gate1_data __initconst = {
	.mask = GATE1_CLK_MASK,
	.def_onoff = BIT(GATE_CBM) | BIT(GATE_EBU) | BIT(GATE_UART)
		| BIT(GATE_PAE) | BIT(GATE_GSWIP)
		| BIT(GATE_USB0) | BIT(GATE_USB1),
	.reg_size = 32,
	.flags = CLK_INIT_DEF_CFG_REQ,
};

static void __init grx500_clk_gate1_setup(struct device_node *node)
{
	gate_clk_setup(node, &grx500_clk_gate1_data);
}

CLK_OF_DECLARE(grx500_gate1clk, INTEL_GRX500_DT_GATE1_CLK,
	       grx500_clk_gate1_setup);

static const struct gate_clk_data grx500_clk_gate0_data __initconst = {
	.mask = GATE0_CLK_MASK,
	.def_onoff = BIT(GATE_XBAR0) | BIT(GATE_XBAR1) | BIT(GATE_XBAR2)
		| BIT(GATE_XBAR3) | BIT(GATE_XBAR6) | BIT(GATE_XBAR7),
	.reg_size = 32,
	.flags = CLK_INIT_DEF_CFG_REQ,
};

static void __init grx500_clk_gate0_setup(struct device_node *node)
{
	gate_clk_setup(node, &grx500_clk_gate0_data);
}

CLK_OF_DECLARE(grx500_gate0clk, INTEL_GRX500_DT_GATE0_CLK,
	       grx500_clk_gate0_setup);

static const struct mux_clk_data grx500_clk_cpu_data __initconst = {
	.shift = CPU_CLK_SHIFT,
	.width = CPU_CLK_WIDTH,
	.lock = &pll0a_lock,
	.flags = CLK_SET_RATE_PARENT,
};

static void __init grx500_clk_cpu_setup(struct device_node *node)
{
	mux_clk_setup(node, &grx500_clk_cpu_data);
}

CLK_OF_DECLARE(grx500_cpuclk, INTEL_GRX500_DT_CPU_CLK,
	       grx500_clk_cpu_setup);

static const struct div_tbl pcie_rgl_tbl[] = {
	{3,	CLK_100M},
};

static const struct div_clk_data grx500_clk_pcie_data __initconst = {
	.shift = PCIE_CLK_SHIFT,
	.width = PCIE_CLK_WIDTH,
	.table = pcie_rgl_tbl,
	.tbl_sz = ARRAY_SIZE(pcie_rgl_tbl),
	.lock = &pll0b_lock,
};

static void __init grx500_clk_pcie_setup(struct device_node *node)
{
	div_clk_setup(node, &grx500_clk_pcie_data);
}

CLK_OF_DECLARE(grx500_pcieclk, INTEL_GRX500_DT_PCIE_CLK,
	       grx500_clk_pcie_setup);

static const struct div_tbl cpu1_rgl_tbl[] = {
	{8,	CLK_166M},
	{5,	CLK_333M},
	{2,	CLK_666M},
	{1,	CLK_1000M},
};

static const struct div_tbl ddr_rgl_tbl[] = {
	{8,	CLK_333M},
	{5,	CLK_666M},
	{4,	CLK_800M},
};

static const struct div_tbl gswip_rgl_tbl[] = {
	{5,	CLK_333M},
};

static const struct div_tbl pae_rgl_tbl[] = {
	{6,	CLK_250M},
};

static const struct div_clk_data grx500_clk_pll0b[] __initconst = {
	{
		.shift = PAE_CLK_SHIFT,
		.width = PAE_CLK_WIDTH,
		.table = pae_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(pae_rgl_tbl),
		.lock = &pll0b_lock,
	},
	{
		.shift = GSWIP_CLK_SHIFT,
		.width = GSWIP_CLK_WIDTH,
		.table = gswip_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(gswip_rgl_tbl),
		.lock = &pll0b_lock,
	},
	{
		.shift = DDR_CLK_SHIFT,
		.width = DDR_CLK_WIDTH,
		.table = ddr_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(ddr_rgl_tbl),
		.lock = &pll0b_lock,
	},
	{
		.shift = CPU1_CLK_SHIFT,
		.width = CPU1_CLK_WIDTH,
		.table = cpu1_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(cpu1_rgl_tbl),
		.lock = &pll0b_lock,
	},
};

static void __init grx500_clk_pll0b_setup(struct device_node *node)
{
	cluster_div_clk_setup(node, grx500_clk_pll0b,
			      ARRAY_SIZE(grx500_clk_pll0b));
}

CLK_OF_DECLARE(grx500_pll0bclk, INTEL_GRX500_DT_PLL0B_CLK,
	       grx500_clk_pll0b_setup);

static const struct div_tbl cpu0_rgl_tbl[] = {
	{9,	CLK_150M},
	{6,	CLK_300M},
	{3,	CLK_600M},
	{2,	CLK_800M},
	{1,	CLK_1200M},
};

static const struct div_tbl ssx4_rgl_tbl[] = {
	{8,	CLK_200M},
};

static const struct div_tbl ngi_rgl_tbl[] = {
	{14,	CLK_50M},
	{9,	CLK_150M},
	{6,	CLK_300M},
	{5,	CLK_400M},
};

static const struct div_tbl cbm_rgl_tbl[] = {
	{14,	CLK_50M},
	{9,	CLK_150M},
	{8,	CLK_200M},
	{5,	CLK_400M},
	{3,	CLK_600M},
	{2,	CLK_800M},
};

static const struct div_clk_data grx500_clk_pll0a[] __initconst = {
	{
		.shift = CBM_CLK_SHIFT,
		.width = CBM_CLK_WIDTH,
		.table = cbm_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(cbm_rgl_tbl),
		.lock = &pll0a_lock,
	},
	{
		.shift = NGI_CLK_SHIFT,
		.width = NGI_CLK_WIDTH,
		.table = ngi_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(ngi_rgl_tbl),
		.lock = &pll0a_lock,
	},
	{
		.shift = SSX4_CLK_SHIFT,
		.width = SSX4_CLK_WIDTH,
		.table = ssx4_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(ssx4_rgl_tbl),
		.lock = &pll0a_lock,
	},
	{
		.shift = CPU0_CLK_SHIFT,
		.width = CPU0_CLK_WIDTH,
		.table = cpu0_rgl_tbl,
		.tbl_sz = ARRAY_SIZE(cpu0_rgl_tbl),
		.lock = &pll0a_lock,
	},
};

static void __init grx500_clk_pll0a_setup(struct device_node *node)
{
	cluster_div_clk_setup(node, grx500_clk_pll0a,
			      ARRAY_SIZE(grx500_clk_pll0a));
}

CLK_OF_DECLARE(grx500_pll0aclk, INTEL_GRX500_DT_PLL0A_CLK,
	       grx500_clk_pll0a_setup);
