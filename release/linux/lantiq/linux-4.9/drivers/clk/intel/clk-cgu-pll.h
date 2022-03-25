/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright(c) 2018 Intel Corporation.
 *  Zhu YiXin <Yixin.zhu@intel.com>
 */

#ifndef __INTEL_CLK_PLL_H
#define __INTEL_CLK_PLL_H

enum pll_platform {
	PLL_PFM_GRX500,
	PLL_PFM_PRX300,
	PLL_PFM_LGM,
};

enum pll_type {
	TYPE_ROPLL,
	TYPE_LJPLL,
};

struct intel_pll_rate_table {
	unsigned long	prate;
	unsigned long	rate;
	unsigned int	mult;
	unsigned int	div;
	unsigned int	frac;
	unsigned int	pll_ena;
};

struct intel_clk_pll {
	struct clk_hw	hw;
	struct device	*dev;
	struct regmap	*map;
	unsigned int	reg;
	unsigned long	flags;
	enum pll_type	type;
	unsigned int	table_sz;
	const struct intel_pll_rate_table *rate_table;
};

#endif /* __INTEL_CLK_PLL_H */
