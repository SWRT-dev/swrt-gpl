// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright (C) 2018 Intel Corporation.
 *  Zhu YiXin <Yixin.zhu@intel.com>
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/device.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#include "clk-cgu-pll.h"
#include "clk-cgu.h"

#define to_intel_clk_pll(_hw)   container_of(_hw, struct intel_clk_pll, hw)
#define to_intel_clk_early(_hw) container_of(_hw, struct intel_clk_early, hw)

/*
 * Calculate formula:
 * rate = (prate * mult + (prate * frac) / frac_div) / div
 */
static unsigned long
intel_pll_calc_rate(unsigned long prate, unsigned int mult,
		    unsigned int div, unsigned int frac,
		    unsigned int frac_div)
{
	u64 crate, frate, rate64;

	rate64 = (u64)prate;
	crate = rate64 * mult;

	if (frac) {
		frate = rate64 * frac;
		do_div(frate, frac_div);
		crate += frate;
	}
	do_div(crate, div);

	return (unsigned long)crate;
}

static unsigned long
grx500_early_recalc_rate(struct clk_hw *hw, unsigned long prate)
{
	struct intel_clk_early *early = to_intel_clk_early(hw);
	unsigned int i, mux, mult, div, frac, val;
	unsigned int reg, shift, width;

	if (!early->mux) {
		pr_err("%s: early clk must have mux!\n", __func__);
		return 0;
	}

	if (!early->table) {
		pr_err("%s: early clk must have div table!\n", __func__);
		return 0;
	}

	mux = intel_get_clk_val(early->map, early->reg,
				early->shift, early->width);
	if (!mux) {
		reg	= early->mreg0;
		shift	= early->mshift0;
		width	= early->mwidth0;
	} else {
		reg	= early->mreg1;
		shift	= early->mshift1;
		width	= early->mwidth1;
	}

	mult = intel_get_clk_val(early->map, reg, 2, 7);
	frac = intel_get_clk_val(early->map, reg, 9, 21);
	val = intel_get_clk_val(early->map, reg + 0x4, shift, width);
	div = 0;

	for (i = 0; early->table[i].div; i++)
		if (early->table[i].val == val) {
			div = early->table[i].div;
			break;
		}

	if (!div) {
		pr_err("%s: clk val %u is invalid for div table!\n",
		       __func__, val);
		return 0;
	}

	return intel_pll_calc_rate(prate, mult, div, frac, BIT(20));
}

static const struct clk_ops grx500_clk_early_ops = {
	.recalc_rate = grx500_early_recalc_rate,
};

static unsigned long
prx300_early_recalc_rate(struct clk_hw *hw, unsigned long prate)
{
	struct intel_clk_early *early = to_intel_clk_early(hw);
	unsigned int i, div1 = 0, mult, div, frac, val;

	mult = intel_get_clk_val(early->map, early->reg + 0x08, 0, 12);
	frac = intel_get_clk_val(early->map, early->reg, 2, 24);
	div = intel_get_clk_val(early->map, early->reg + 0x08, 18, 6);
	val = intel_get_clk_val(early->map, early->reg + 0x04,
				early->shift, early->width);

	for (i = 0; early->table[i].div; i++)
		if (early->table[i].val == val) {
			div1 = early->table[i].div;
			break;
		}

	if (!div1) {
		pr_err("%s: clk val %u is invalid for div table!\n",
		       __func__, val);
		return 0;
	}

	div = div * div1;

	return intel_pll_calc_rate(prate, mult, div, frac, BIT(24));
}

static const struct clk_ops prx300_clk_early_ops = {
	.recalc_rate = prx300_early_recalc_rate,
};

int intel_clk_register_early(struct intel_clk_provider *ctx,
			     const struct intel_clk_early_data *list,
			     unsigned int nr_clk)
{
	struct clk_hw *hw;
	struct clk_init_data init;
	struct intel_clk_early *early;
	int i, ret;

	for (i = 0; i < nr_clk; i++, list++) {
		early = kzalloc(sizeof(*early), GFP_KERNEL);
		if (!early)
			return -ENOMEM;

		memset(&init, 0, sizeof(init));
		init.name = list->name;
		if (list->platform == PLL_PFM_GRX500) {
			init.ops = &grx500_clk_early_ops;
		} else if (list->platform == PLL_PFM_PRX300) {
			init.ops = &prx300_clk_early_ops;
		} else {
			pr_err("%s: platform not support: %u\n",
			       __func__, list->platform);
			return -EINVAL;
		}

		init.flags = CLK_IS_BASIC |  CLK_GET_RATE_NOCACHE;
		init.parent_names = list->parent_names;
		init.num_parents = 1;

		early->map	= ctx->map;
		early->reg	= list->reg;
		early->shift	= list->shift;
		early->width	= list->width;
		early->mux	= list->mux;
		early->type	= list->type;
		early->table	= list->table;
		early->hw.init	= &init;

		hw = &early->hw;
		ret = clk_hw_register(NULL, hw);
		if (ret) {
			kfree(early);
			pr_err("early clk register fail: %s\n", list->name);
			return ret;
		}

		intel_clk_add_lookup(ctx, hw, i);
	}
	return 0;
}

static int intel_pll_wait_for_lock(struct intel_clk_pll *pll)
{
	const int max_loop_cnt = 100;
	int i;

	for (i = 0; i < max_loop_cnt; i++) {
		if (!intel_get_clk_val(pll->map, pll->reg, 0, 1))
			udelay(1);
		else
			return 0;
	}

	return -EIO;
}

static void
grx500_pll_get_params(struct intel_clk_pll *pll, unsigned int *mult,
		      unsigned int *frac)
{
	*mult = intel_get_clk_val(pll->map, pll->reg, 2, 7);
	*frac = intel_get_clk_val(pll->map, pll->reg, 9, 21);
}

static unsigned long
grx500_pll_recalc_rate(struct clk_hw *hw, unsigned long prate)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);
	unsigned int mult, frac;

	grx500_pll_get_params(pll, &mult, &frac);

	return intel_pll_calc_rate(prate, mult, 1, frac, BIT(21));
}

static int grx500_pll_is_enabled(struct clk_hw *hw)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);

	return intel_get_clk_val(pll->map, pll->reg, 1, 1);
}

static const struct clk_ops intel_grx500_pll_ops = {
	.recalc_rate	= grx500_pll_recalc_rate,
	.is_enabled	= grx500_pll_is_enabled,
};

static void
prx300_pll_get_params(struct intel_clk_pll *pll, unsigned int *mult,
		      unsigned int *div, unsigned int *frac)
{
	*mult = intel_get_clk_val(pll->map, pll->reg + 0x8, 0, 12);
	*div = intel_get_clk_val(pll->map, pll->reg + 0x8, 18, 6);
	*frac = intel_get_clk_val(pll->map, pll->reg, 2, 24);
}

static int
prx300_pll_set_params(struct intel_clk_pll *pll, unsigned int mult,
		      unsigned int div, unsigned int frac)
{
	intel_set_clk_val(pll->map, pll->reg + 0x8, 28, 1, 1);
	intel_set_clk_val(pll->map, pll->reg + 0x8, 0, 12, mult);
	intel_set_clk_val(pll->map, pll->reg + 0x8, 18, 6, div);
	intel_set_clk_val(pll->map, pll->reg, 2, 24, frac);
	intel_set_clk_val(pll->map, pll->reg, 28, 1, !!frac);

	if (pll->type == TYPE_LJPLL) {
		intel_set_clk_val(pll->map, pll->reg, 31, 1, 1);
		udelay(1);
		intel_set_clk_val(pll->map, pll->reg, 31, 1, 0);
	}

	return intel_pll_wait_for_lock(pll);
}

static unsigned long
prx300_pll_recalc_rate(struct clk_hw *hw, unsigned long prate)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);
	unsigned int div, mult, frac;

	prx300_pll_get_params(pll, &mult, &div, &frac);
	if (pll->type == TYPE_LJPLL)
		div *= 4;

	return intel_pll_calc_rate(prate, mult, div, frac, BIT(24));
}

static int prx300_pll_is_enabled(struct clk_hw *hw)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);

	return intel_get_clk_val(pll->map, pll->reg, 0, 1);
}

static int prx300_pll_enable(struct clk_hw *hw)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);

	intel_set_clk_val(pll->map, pll->reg, 0, 1, 1);
	return intel_pll_wait_for_lock(pll);
}

static void __maybe_unused prx300_pll_disable(struct clk_hw *hw)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);

	intel_set_clk_val(pll->map, pll->reg, 0, 1, 0);
}

static long
prx300_pll_round_rate(struct clk_hw *hw, unsigned long rate,
		      unsigned long *prate)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);
	int i;
	unsigned long cand_rate = 0;
	const struct intel_pll_rate_table *tbl;

	if (!pll->rate_table)
		return prx300_pll_recalc_rate(hw, *prate);

	tbl = pll->rate_table;
	for (i = 0; i < pll->table_sz; i++) {
		if (tbl[i].prate != *prate)
			continue;
		if (!cand_rate ||
		    abs(tbl[i].rate - rate) < abs(cand_rate - rate))
			cand_rate = tbl[i].rate;
	}

	return cand_rate;
}

static int
prx300_pll_set_rate(struct clk_hw *hw, unsigned long rate,
		    unsigned long prate)
{
	struct intel_clk_pll *pll = to_intel_clk_pll(hw);
	struct device *dev = pll->dev;
	const struct intel_pll_rate_table *tbl;
	unsigned int mult, div, frac;
	int i;

	if (!pll->rate_table) {
		dev_err(dev, "%s: set clk failed: %s rate: %lu, prate: %lu\n",
			__func__, clk_hw_get_name(hw), rate, prate);
		return -ENODEV;
	}
	tbl = pll->rate_table;
	for (i = 0; i < pll->table_sz; i++) {
		if (tbl[i].prate == prate && tbl[i].rate == rate)
			break;
	}
	if (i >= pll->table_sz) {
		dev_err(dev, "%s: No valid clk: %s, rate: %lu, prate: %lu\n",
			__func__, clk_hw_get_name(hw), rate, prate);
		return -EINVAL;
	}

	mult	= tbl[i].mult;
	div	= tbl[i].div;
	frac	= tbl[i].frac;

	return prx300_pll_set_params(pll, mult, div, frac);
}

static const struct clk_ops intel_prx300_pll_ops = {
	.recalc_rate	= prx300_pll_recalc_rate,
	.is_enabled	= prx300_pll_is_enabled,
	.enable		= prx300_pll_enable,
	.round_rate	= prx300_pll_round_rate,
	.set_rate	= prx300_pll_set_rate,
};

static struct clk_hw
*intel_clk_register_pll(struct intel_clk_provider *ctx,
			const struct intel_pll_clk_data *list)
{
	struct clk_init_data init;
	struct intel_clk_pll *pll;
	struct device *dev = ctx->dev;
	struct clk_hw *hw;
	const struct intel_pll_rate_table *table = list->rate_table;
	int ret, i;

	if (list->platform == PLL_PFM_GRX500) {
		init.ops = &intel_grx500_pll_ops;
	} else if (list->platform == PLL_PFM_PRX300) {
		init.ops = &intel_prx300_pll_ops;
	} else {
		dev_err(dev, "%s: pll platform %d not supported!\n",
			__func__, list->platform);
		return ERR_PTR(-EINVAL);
	}
	init.name = list->name;
	init.flags = list->flags | CLK_IS_BASIC;
	init.parent_names = list->parent_names;
	init.num_parents = list->num_parents;

	pll = devm_kzalloc(dev, sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);

	pll->map	= ctx->map;
	pll->dev	= ctx->dev;
	pll->reg	= list->reg;
	pll->flags	= list->flags;
	pll->type	= list->type;
	pll->hw.init	= &init;

	if (table) {
		for (i = 0; table[i].rate != 0; i++)
			;
		pll->table_sz = i;
		pll->rate_table = devm_kmemdup(dev, table, i * sizeof(table[0]),
					       GFP_KERNEL);
		if (!pll->rate_table)
			return ERR_PTR(-ENOMEM);
	}
	hw = &pll->hw;
	ret = clk_hw_register(dev, hw);
	if (ret)
		return ERR_PTR(ret);

	return hw;
}

void intel_clk_register_plls(struct intel_clk_provider *ctx,
			     const struct intel_pll_clk_data *list,
			     unsigned int nr_clk)
{
	struct clk_hw *hw;
	int i;

	for (i = 0; i < nr_clk; i++, list++) {
		hw = intel_clk_register_pll(ctx, list);
		if (IS_ERR(hw)) {
			dev_err(ctx->dev, "%s: failed to register pll: %s\n",
				__func__, list->name);
			continue;
		}

		intel_clk_add_lookup(ctx, hw, list->id);
	}
}

void intel_clk_plls_parse_vco_config(struct intel_clk_provider *ctx,
				     const struct intel_pll_clk_data *list,
				     u32 nr_clk)
{
	const struct intel_pll_clk_data *plls;
	struct device *dev = ctx->dev;
	struct of_phandle_args r;
	struct clk *clk;
	u32 clk_id, clk_rate, clk_en;
	int cnt, idx, ret;

	for (cnt = 0;; cnt++) {
		/* arg0: Clk ID, arg1: Clk rate, arg2: Clk Enable */
		ret = of_parse_phandle_with_fixed_args(dev->of_node,
						       "intel,pll-clks",
						       3, cnt, &r);
		if (ret)
			break;

		clk_id   = r.args[0];
		clk_rate = r.args[1];
		clk_en   = r.args[2];

		for (idx = 0, plls = list; idx < nr_clk; idx++, plls++) {
			if (plls->id == clk_id)
				break;
		}
		if (plls->id != clk_id) {
			dev_warn(dev, "PLL%u: not supported!\n", clk_id);
			continue;
		}

		clk = __clk_lookup(plls->name);
		if (clk_en) {
			clk_prepare_enable(clk);
			if (clk_set_rate(clk, clk_rate)) {
				dev_warn(dev, "PLL %s not support rate: %u\n",
					 plls->name, clk_rate);
				clk_disable_unprepare(clk);
			}
		} else {
			/* PLL HW default is enabled.
			 * Linux clock default is disabled.
			 * Disable PLL clk requires to enable first in logic
			 */
			clk_prepare_enable(clk);
			clk_disable_unprepare(clk);
		}
	}
}

