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

#ifndef __INTEL_GRX500_CLK_H
#define __INTEL_GRX500_CLK_H

/* Intel GRX500 CGU device tree "compatible" strings */
#define INTEL_GRX500_DT_PLL0A_CLK	"lantiq, grx500-pll0a-output-clk"
#define INTEL_GRX500_DT_PLL0B_CLK	"lantiq, grx500-pll0b-output-clk"
#define INTEL_GRX500_DT_PCIE_CLK	"lantiq, grx500-pcie-clk"
#define INTEL_GRX500_DT_CPU_CLK		"lantiq, grx500-cpu-clk"
#define INTEL_GRX500_DT_GATE0_CLK	"lantiq, grx500-gate0-clk"
#define INTEL_GRX500_DT_GATE1_CLK	"lantiq, grx500-gate1-clk"
#define INTEL_GRX500_DT_GATE2_CLK	"lantiq, grx500-gate2-clk"
#define INTEL_GRX500_DT_VOICE_CLK	"lantiq, grx500-voice-clk"
#define INTEL_GRX500_DT_GATE_I2C_CLK	"lantiq, grx500-gate-dummy-clk"

/* clocks under pll0a-clk */
#define CBM_CLK			0
#define NGI_CLK			1
#define SSX4_CLK		2
#define CPU0_CLK		3

/* clocks under pll0b-clk */
#define PAE_CLK			0
#define GSWIP_CLK		1
#define DDR_CLK			2
#define CPU1_CLK		3

/* clocks under lcpll-clk */
#define GRX500_PCIE_CLK		0

/* clocks under gate0-clk */
#define GATE_XBAR0_CLK		0
#define GATE_XBAR1_CLK		1
#define GATE_XBAR2_CLK		2
#define GATE_XBAR3_CLK		3
#define GATE_XBAR6_CLK		4
#define GATE_XBAR7_CLK		5

/* clocks under gate1-clk */
#define GATE_V_CODEC_CLK	0
#define GATE_DMA0_CLK		1
#define GATE_USB0_CLK		2
#define GATE_SPI1_CLK		3
#define GATE_SPI0_CLK		4
#define GATE_CBM_CLK		5
#define GATE_EBU_CLK		6
#define GATE_SSO_CLK		7
#define GATE_GPTC0_CLK		8
#define GATE_GPTC1_CLK		9
#define GATE_GPTC2_CLK		10
#define GATE_URT_CLK		11
#define GATE_EIP97_CLK		12
#define GATE_EIP123_CLK		13
#define GATE_TOE_CLK		14
#define GATE_MPE_CLK		15
#define GATE_TDM_CLK		16
#define GATE_PAE_CLK		17
#define GATE_USB1_CLK		18
#define GATE_GSWIP_CLK		19

/* clocks under gate2-clk */
#define GATE_PCIE0_CLK		0
#define GATE_PCIE1_CLK		1
#define GATE_PCIE2_CLK		2

/* Gate clock bits */
#define GATE_XBAR0			0
#define GATE_XBAR1			1
#define GATE_XBAR2			2
#define GATE_XBAR3			3
#define GATE_XBAR6			6
#define GATE_XBAR7			7

#define GATE_V_CODEC			2
#define GATE_DMA0			5
#define GATE_USB0			6
#define GATE_SPI1			7
#define GATE_SPI0			8
#define GATE_CBM			9
#define GATE_EBU			10
#define GATE_SSO			11
#define GATE_GPTC0			12
#define GATE_GPTC1			13
#define GATE_GPTC2			14
#define GATE_UART			17
#define GATE_EIP97			20
#define GATE_EIP123			21
#define GATE_TOE			22
#define GATE_MPE			23
#define GATE_TDM			25
#define GATE_PAE			26
#define GATE_USB1			27
#define GATE_GSWIP			28

#define GATE_PCIE0_CTRL			1
#define GATE_PCIE1_CTRL			17
#define GATE_PCIE2_CTRL			25

/* clock shift and width */
#define CBM_CLK_SHIFT			0
#define CBM_CLK_WIDTH			4
#define NGI_CLK_SHIFT			4
#define NGI_CLK_WIDTH			4
#define SSX4_CLK_SHIFT			8
#define SSX4_CLK_WIDTH			4
#define CPU0_CLK_SHIFT			12
#define CPU0_CLK_WIDTH			4

#define PAE_CLK_SHIFT			0
#define PAE_CLK_WIDTH			4
#define GSWIP_CLK_SHIFT			4
#define GSWIP_CLK_WIDTH			4
#define DDR_CLK_SHIFT			8
#define DDR_CLK_WIDTH			4
#define CPU1_CLK_SHIFT			12
#define CPU1_CLK_WIDTH			4

#define PCIE_CLK_SHIFT			12
#define PCIE_CLK_WIDTH			2

#define CPU_CLK_SHIFT			29
#define CPU_CLK_WIDTH			1

#define VOICE_CLK_SHIFT			14
#define VOICE_CLK_WIDTH			2

#define GATE0_CLK_MASK			0xCF
#define GATE1_CLK_MASK			0x1EF27FE4
#define GATE2_CLK_MASK			0x2020002
#endif /* __INTEL_GRX500_CLK_H */
