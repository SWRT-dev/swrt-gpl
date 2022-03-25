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

#define GATE_HW_REG_STAT(reg)	(reg)
#define GATE_HW_REG_EN(reg)	((reg) + 0x4)
#define GATE_HW_REG_DIS(reg)	((reg) + 0x8)
#define MAX_DDIV_REG	8
#define MAX_DIVIDER_VAL 64

#define to_intel_clk_mux(_hw) container_of(_hw, struct intel_clk_mux, hw)
#define to_intel_clk_divider(_hw) \
		container_of(_hw, struct intel_clk_divider, hw)
#define to_intel_clk_gate(_hw) container_of(_hw, struct intel_clk_gate, hw)
#define to_intel_clk_ddiv(_hw) container_of(_hw, struct intel_clk_ddiv, hw)
#define struct_size(p, member, n)  (sizeof(*(p)) +\
				   (sizeof(*(p)->member) * (n)))

void intel_set_clk_val(struct regmap *map, u32 reg, u8 shift,
		       u8 width, u32 set_val)
{
	u32 mask = GENMASK(width + shift - 1, shift);

	regmap_update_bits(map, reg, mask, set_val << shift);
}

u32 intel_get_clk_val(struct regmap *map, u32 reg, u8 shift,
		      u8 width)
{
	u32 val;

	if (regmap_read(map, reg, &val)) {
		WARN_ONCE(1, "Failed to read clk reg: 0x%x\n", reg);
		return 0;
	}
	val >>= shift;
	val &= BIT(width) - 1;

	return val;
}

void intel_clk_add_lookup(struct intel_clk_provider *ctx,
			  struct clk_hw *hw, unsigned int id)
{
	pr_debug("Add clk: %s, id: %u\n", clk_hw_get_name(hw), id);
	if (ctx->clk_data.hws)
		ctx->clk_data.hws[id] = hw;
}

static struct clk_hw
*intel_clk_register_fixed(struct intel_clk_provider *ctx,
			  const struct intel_clk_branch *list)
{
	if (list->div_flags & CLOCK_FLAG_VAL_INIT)
		intel_set_clk_val(ctx->map, list->div_off, list->div_shift,
				  list->div_width, list->div_val);

	return clk_hw_register_fixed_rate(NULL, list->name,
					  list->parent_names[0],
					  list->flags, list->mux_flags);
}

static u8 intel_clk_mux_get_parent(struct clk_hw *hw)
{
/*	struct intel_clk_mux *mux = to_intel_clk_mux(hw);
 *	u32 val;
 *
 *	val = intel_get_clk_val(mux->map, mux->reg,mux->shift, mux->width);
 *	return clk_mux_val_to_index(hw, NULL, mux->flags, val);
 */
	return 0;
}

static int intel_clk_mux_set_parent(struct clk_hw *hw, u8 index)
{
/*	struct intel_clk_mux *mux = to_intel_clk_mux(hw);
 *	u32 val;
 *
 *	val = clk_mux_index_to_val(NULL, mux->flags, index);
 *	intel_set_clk_val(mux->map, mux->reg, mux->shift, mux->width, val);
 */
	return 0;
}

static int intel_clk_mux_determine_rate(struct clk_hw *hw,
					struct clk_rate_request *req)
{
	return __clk_mux_determine_rate(hw, req);
}

const static struct clk_ops intel_clk_mux_ops = {
	.get_parent = intel_clk_mux_get_parent,
	.set_parent = intel_clk_mux_set_parent,
	.determine_rate = intel_clk_mux_determine_rate,
};

static struct clk_hw
*intel_clk_register_mux(struct intel_clk_provider *ctx,
			const struct intel_clk_branch *list)
{
	struct clk_init_data init;
	struct clk_hw *hw;
	struct intel_clk_mux *mux;
	struct device *dev = ctx->dev;
	u32 reg = list->mux_off;
	u8 shift = list->mux_shift;
	u8 width = list->mux_width;
	unsigned long cflags = list->mux_flags;
	int ret;

	mux = devm_kzalloc(dev, sizeof(*mux), GFP_KERNEL);
	if (!mux)
		return ERR_PTR(-ENOMEM);

	init.name = list->name;
	init.ops = &intel_clk_mux_ops;
	init.flags = list->flags | CLK_IS_BASIC;
	init.parent_names = list->parent_names;
	init.num_parents = list->num_parents;

	mux->map = ctx->map;
	mux->reg = reg;
	mux->shift = shift;
	mux->width = width;
	mux->flags = cflags;
	mux->dev = dev;
	mux->hw.init = &init;

	hw = &mux->hw;
	ret = clk_hw_register(dev, hw);
	if (ret)
		return ERR_PTR(ret);

	if (cflags & CLOCK_FLAG_VAL_INIT)
		intel_set_clk_val(ctx->map, reg, shift, width, list->mux_val);

	return hw;
}

static unsigned long
intel_clk_divider_recalc_rate(struct clk_hw *hw,
			      unsigned long parent_rate)
{
	struct intel_clk_divider *divider = to_intel_clk_divider(hw);
	unsigned int val;
	unsigned long div;

	val = intel_get_clk_val(divider->map, divider->reg,
				divider->shift, divider->width);

	div = divider_recalc_rate(hw, parent_rate, val, divider->table,
				  divider->flags);
	return div;
}

static long
intel_clk_divider_round_rate(struct clk_hw *hw, unsigned long rate,
			     unsigned long *prate)
{
	struct intel_clk_divider *divider = to_intel_clk_divider(hw);

	return divider_round_rate(hw, rate, prate, divider->table,
				  divider->width, divider->flags);
}

static int
intel_clk_divider_set_rate(struct clk_hw *hw, unsigned long rate,
			   unsigned long prate)
{
	struct intel_clk_divider *divider = to_intel_clk_divider(hw);
	int value;

	value = divider_get_val(rate, prate, divider->table,
				divider->width, divider->flags);
	if (value < 0)
		return value;

	intel_set_clk_val(divider->map, divider->reg,
			  divider->shift, divider->width, value);

	return 0;
}

static int intel_clk_divider_is_enabled(struct clk_hw *hw)
{
	struct intel_clk_divider *div = to_intel_clk_divider(hw);

	return intel_get_clk_val(div->map, div->reg,
				div->shift_gate, div->width_gate);
}

static int intel_clk_divider_enable(struct clk_hw *hw)
{
	struct intel_clk_divider *div = to_intel_clk_divider(hw);

	intel_set_clk_val(div->map, div->reg,
			  div->shift_gate, div->width_gate, 1);
	return 0;
}

static void intel_clk_divider_disable(struct clk_hw *hw)
{
	struct intel_clk_divider *div = to_intel_clk_divider(hw);

	intel_set_clk_val(div->map, div->reg,
			  div->shift_gate, div->width_gate, 0);
}

const static struct clk_ops intel_clk_divider_ops = {
	.recalc_rate = intel_clk_divider_recalc_rate,
	.round_rate = intel_clk_divider_round_rate,
	.set_rate = intel_clk_divider_set_rate,
	.is_enabled = intel_clk_divider_is_enabled,
	.enable = intel_clk_divider_enable,
	.disable = intel_clk_divider_disable,
};

static struct clk_hw
*intel_clk_register_divider(struct intel_clk_provider *ctx,
			    const struct intel_clk_branch *list)
{
	struct clk_init_data init;
	struct clk_hw *hw;
	struct intel_clk_divider *div;
	struct device *dev = ctx->dev;
	unsigned long cflags = list->div_flags;
	u32 reg = list->div_off;
	u8 shift = list->div_shift;
	u8 width = list->div_width;
	u8 shift_gate = list->div_shift_gate;
	u8 width_gate = list->div_width_gate;
	int ret;

	div = devm_kzalloc(dev, sizeof(*div), GFP_KERNEL);
	if (!div)
		return ERR_PTR(-ENOMEM);

	init.name = list->name;
	init.ops = &intel_clk_divider_ops;
	init.flags = list->flags | CLK_IS_BASIC;
	init.parent_names = &list->parent_names[0];
	init.num_parents = 1;

	div->map	= ctx->map;
	div->reg	= reg;
	div->shift	= shift;
	div->width	= width;
	div->shift_gate	= shift_gate;
	div->width_gate	= width_gate;
	div->flags	= cflags;
	div->table	= list->div_table;
	div->dev	= dev;
	div->hw.init	= &init;

	hw = &div->hw;
	ret = clk_hw_register(dev, hw);

	if (ret) {
		dev_err(dev, "%s: register clk: %s failed!\n",
			__func__, list->name);
		return ERR_PTR(ret);
	}

	if (cflags & CLOCK_FLAG_VAL_INIT)
		intel_set_clk_val(ctx->map, reg, shift, width, list->div_val);

	return hw;
}

static struct clk_hw
*intel_clk_register_fixed_factor(struct intel_clk_provider *ctx,
				 const struct intel_clk_branch *list)
{
	struct clk_hw *hw;

	hw = clk_hw_register_fixed_factor(ctx->dev, list->name,
					  list->parent_names[0], list->flags,
					  list->mult, list->div);
	if (IS_ERR(hw))
		return ERR_CAST(hw);

	if (list->div_flags & CLOCK_FLAG_VAL_INIT)
		intel_set_clk_val(ctx->map, list->div_off, list->div_shift,
				  list->div_width, list->div_val);

	return hw;
}

static int
intel_clk_gate_enable(struct clk_hw *hw)
{
	struct intel_clk_gate *gate = to_intel_clk_gate(hw);
	unsigned int reg;

	if (gate->flags & GATE_CLK_HW) {
		reg = GATE_HW_REG_EN(gate->reg);
	} else if (gate->flags & GATE_CLK_SW) {
		reg = gate->reg;
	} else {
		pr_err("%s: gate clk: %s: flag 0x%lx not supported!\n",
		       __func__, clk_hw_get_name(hw), gate->flags);
		return 0;
	}

	intel_set_clk_val(gate->map, reg, gate->shift, 1, 1);

	return 0;
}

static void
intel_clk_gate_disable(struct clk_hw *hw)
{
	struct intel_clk_gate *gate = to_intel_clk_gate(hw);
	unsigned int reg;
	unsigned int set;

	if (gate->flags & GATE_CLK_HW) {
		reg = GATE_HW_REG_DIS(gate->reg);
		set = 1;
	} else if (gate->flags & GATE_CLK_SW) {
		reg = gate->reg;
		set = 0;
	} else {
		dev_err(gate->dev,
			"%s: gate clk: %s: flag 0x%lx not supported!\n",
			__func__, clk_hw_get_name(hw), gate->flags);
		return;
	}

	intel_set_clk_val(gate->map, reg, gate->shift, 1, set);
}

static int
intel_clk_gate_is_enabled(struct clk_hw *hw)
{
	struct intel_clk_gate *gate = to_intel_clk_gate(hw);
	unsigned int reg;

	if (gate->flags & GATE_CLK_HW) {
		reg = GATE_HW_REG_STAT(gate->reg);
	} else if (gate->flags & GATE_CLK_SW) {
		reg = gate->reg;
	} else {
		dev_err(gate->dev,
			"%s: gate clk: %s: flag 0x%lx not supported!\n",
			__func__, clk_hw_get_name(hw), gate->flags);
		return 0;
	}

	return intel_get_clk_val(gate->map, reg, gate->shift, 1);
}

const static struct clk_ops intel_clk_gate_ops = {
	.enable = intel_clk_gate_enable,
	.disable = intel_clk_gate_disable,
	.is_enabled = intel_clk_gate_is_enabled,
};

static struct clk_hw
*intel_clk_register_gate(struct intel_clk_provider *ctx,
			 const struct intel_clk_branch *list)
{
	struct clk_init_data init;
	struct clk_hw *hw;
	struct intel_clk_gate *gate;
	struct device *dev = ctx->dev;
	u32 reg = list->gate_off;
	u8 shift = list->gate_shift;
	unsigned long cflags = list->gate_flags;
	const char *pname = list->parent_names[0];
	int ret;

	gate = devm_kzalloc(dev, sizeof(*gate), GFP_KERNEL);
	if (!gate)
		return ERR_PTR(-ENOMEM);

	init.name = list->name;
	init.ops = &intel_clk_gate_ops;
	init.flags = list->flags | CLK_IS_BASIC;
	init.parent_names = pname ? &pname : NULL;
	init.num_parents = pname ? 1 : 0;

	gate->map	= ctx->map;
	gate->reg	= reg;
	gate->shift	= shift;
	gate->flags	= cflags;
	gate->dev	= dev;
	gate->hw.init	= &init;

	hw = &gate->hw;
	ret = clk_hw_register(dev, hw);
	if (ret)
		return ERR_PTR(ret);

	if (cflags & CLOCK_FLAG_VAL_INIT)
		intel_set_clk_val(ctx->map, reg, shift, 1, list->gate_val);

	return hw;
}

void intel_clk_register_branches(struct intel_clk_provider *ctx,
				 const struct intel_clk_branch *list,
				 unsigned int nr_clk)
{
	struct clk_hw *hw;
	unsigned int idx;

	for (idx = 0; idx < nr_clk; idx++, list++) {
		switch (list->type) {
		case intel_clk_fixed:
			hw = intel_clk_register_fixed(ctx, list);
			break;
		case intel_clk_mux:
			hw = intel_clk_register_mux(ctx, list);
			break;
		case intel_clk_divider:
			hw = intel_clk_register_divider(ctx, list);
			break;
		case intel_clk_fixed_factor:
			hw = intel_clk_register_fixed_factor(ctx, list);
			break;
		case intel_clk_gate:
			hw = intel_clk_register_gate(ctx, list);
			break;
		default:
			dev_err(ctx->dev, "%s: type: %u not supported!\n",
				__func__, list->type);
			return;
		}

		if (IS_ERR(hw)) {
			dev_err(ctx->dev,
				"%s: register clk: %s, type: %u failed!\n",
				__func__, list->name, list->type);
			return;
		}
		intel_clk_add_lookup(ctx, hw, list->id);
	}
}

static unsigned long
intel_clk_ddiv_recalc_rate(struct clk_hw *hw,
			   unsigned long parent_rate)
{
	struct intel_clk_ddiv *ddiv = to_intel_clk_ddiv(hw);
	unsigned int div0, div1, exdiv;
	u64 prate;

	div0 = intel_get_clk_val(ddiv->map, ddiv->reg,
				 ddiv->shift0, ddiv->width0) + 1;
	div1 = intel_get_clk_val(ddiv->map, ddiv->reg,
				 ddiv->shift1, ddiv->width1) + 1;
	exdiv = intel_get_clk_val(ddiv->map, ddiv->reg,
				  ddiv->shift2, ddiv->width2);

	prate = (u64)parent_rate;
	do_div(prate, div0);
	do_div(prate, div1);

	if (exdiv) {
		do_div(prate, ddiv->div);
		prate *= ddiv->mult;
	}

	return (unsigned long)prate;
}

static int intel_clk_ddiv_is_enabled(struct clk_hw *hw)
{
	struct intel_clk_ddiv *ddiv = to_intel_clk_ddiv(hw);

	return intel_get_clk_val(ddiv->map, ddiv->reg,
				 ddiv->shift_gate, ddiv->width_gate);
}

static int intel_clk_ddiv_enable(struct clk_hw *hw)
{
	struct intel_clk_ddiv *ddiv = to_intel_clk_ddiv(hw);

	intel_set_clk_val(ddiv->map, ddiv->reg,
			  ddiv->shift_gate, ddiv->width_gate, 1);
	return 0;
}

static void intel_clk_ddiv_disable(struct clk_hw *hw)
{
	struct intel_clk_ddiv *ddiv = to_intel_clk_ddiv(hw);

	intel_set_clk_val(ddiv->map, ddiv->reg,
			  ddiv->shift_gate, ddiv->width_gate, 0);
}

static int
intel_clk_get_ddiv_val(u32 div, u32 *ddiv1, u32 *ddiv2)
{
	u32 idx, temp;

	*ddiv1 = 1;
	*ddiv2 = 1;

	if (div > MAX_DIVIDER_VAL) {
		pr_info("Crossed max DDIV %u\n", div);
		div = MAX_DIVIDER_VAL;
	}

	if (div > 1) {
		for (idx = 2; idx <= MAX_DDIV_REG; idx++) {
			temp = DIV_ROUND_UP_ULL((u64)div, idx);
			if ((div % idx == 0) && (temp <= MAX_DDIV_REG))
				break;
		}

		if (idx > 8) {
			pr_err("Invalid ddiv %u\n", div);
			return -EINVAL;
		}

		*ddiv1 = temp;
		*ddiv2 = idx;
	}

	return 0;
}

static long
intel_clk_ddiv_round_rate(struct clk_hw *hw, unsigned long rate,
			  unsigned long *prate)
{
	struct intel_clk_ddiv *ddiv = to_intel_clk_ddiv(hw);
	u32 div, ddiv1, ddiv2;
	u64 rate64 = rate;

	div = DIV_ROUND_CLOSEST_ULL((u64)*prate, rate);

	/* if predivide bit is enabled, modify div by factor of 2.5 */
	if (intel_get_clk_val(ddiv->map, ddiv->reg, ddiv->shift2, 1)) {
		div = div * 2;
		div = DIV_ROUND_CLOSEST_ULL((u64)div, 5);
	}

	if (div <= 0)
		return *prate;

	if (intel_clk_get_ddiv_val(div, &ddiv1, &ddiv2) != 0) {
		if (intel_clk_get_ddiv_val(div + 1, &ddiv1, &ddiv2) != 0)
			return -EINVAL;
	}

	rate64 = *prate;
	do_div(rate64, ddiv1);
	do_div(rate64, ddiv2);

	/* if predivide bit is enabled, modify rounded rate by factor of 2.5 */
	if (intel_get_clk_val(ddiv->map, ddiv->reg, ddiv->shift2, 1)) {
		rate64 = rate64 * 2;
		rate64 = DIV_ROUND_CLOSEST_ULL(rate64, 5);
	}

	return (unsigned long)rate64;
}

static int
intel_clk_ddiv_set_rate(struct clk_hw *hw, unsigned long rate,
			unsigned long prate)
{
	struct intel_clk_ddiv *ddiv = to_intel_clk_ddiv(hw);
	u32 div, ddiv1, ddiv2;

	div = DIV_ROUND_CLOSEST_ULL((u64)prate, rate);

	if (intel_get_clk_val(ddiv->map, ddiv->reg, ddiv->shift2, 1)) {
		div = DIV_ROUND_CLOSEST_ULL((u64)div, 5);
		div = div * 2;
	}

	if (div <= 0)
		return -EINVAL;

	if (intel_clk_get_ddiv_val(div, &ddiv1, &ddiv2))
		return -EINVAL;

	intel_set_clk_val(ddiv->map, ddiv->reg,
			  ddiv->shift0, ddiv->width0, ddiv1 - 1);

	intel_set_clk_val(ddiv->map, ddiv->reg,
			  ddiv->shift1, ddiv->width1, ddiv2 - 1);
	return 0;
}

const static struct clk_ops intel_clk_ddiv_ops = {
	.recalc_rate	= intel_clk_ddiv_recalc_rate,
	.is_enabled	= intel_clk_ddiv_is_enabled,
	.enable		= intel_clk_ddiv_enable,
	.disable	= intel_clk_ddiv_disable,
	.set_rate = intel_clk_ddiv_set_rate,
	.round_rate = intel_clk_ddiv_round_rate,
};

int intel_clk_register_ddiv(struct intel_clk_provider *ctx,
			    const struct intel_clk_ddiv_data *list,
			    unsigned int nr_clk)
{
	unsigned int idx;
	struct clk_init_data init;
	struct intel_clk_ddiv *ddiv;
	struct device *dev = ctx->dev;
	struct clk_hw *hw;
	int ret;

	for (idx = 0; idx < nr_clk; idx++, list++) {
		ddiv = devm_kzalloc(dev, sizeof(*ddiv), GFP_KERNEL);
		if (!ddiv)
			return -ENOMEM;

		memset(&init, 0, sizeof(init));
		init.name = list->name;
		init.ops = &intel_clk_ddiv_ops;
		init.flags = list->flags | CLK_IS_BASIC;
		init.parent_names = list->parent_name;
		init.num_parents = 1;

		ddiv->map		= ctx->map;
		ddiv->reg		= list->reg;
		ddiv->shift0		= list->shift0;
		ddiv->width0		= list->width0;
		ddiv->shift1		= list->shift1;
		ddiv->width1		= list->width1;
		ddiv->shift_gate	= list->shift_gate;
		ddiv->width_gate	= list->width_gate;
		ddiv->shift2		= list->ex_shift;
		ddiv->width2		= list->ex_width;
		ddiv->flags		= list->div_flags;
		ddiv->mult		= 2;
		ddiv->div		= 5;
		ddiv->dev		= dev;
		ddiv->hw.init		= &init;

		hw = &ddiv->hw;
		ret = clk_hw_register(dev, hw);
		if (ret) {
			dev_err(dev, "%s: register clk: %s failed!\n",
				__func__, list->name);
			return ret;
		}

		intel_clk_add_lookup(ctx, hw, list->id);
	}

	return 0;
}

struct intel_clk_provider * __init
intel_clk_init(struct regmap *map, unsigned int nr_clks)
{
	struct intel_clk_provider *ctx;

	ctx = kzalloc(struct_size(ctx, clk_data.hws, nr_clks), GFP_KERNEL);

	if (!ctx)
		return ERR_PTR(-ENOMEM);

	ctx->map = map;
	ctx->clk_data.num = nr_clks;

	return ctx;
}

static void
intel_clk_set_ssc(struct intel_clk_provider *ctx,
		  u32 reg, u32 spread, u32 dir, u32 div, u32 en)
{
	intel_set_clk_val(ctx->map, reg, 2, 5, spread);
	intel_set_clk_val(ctx->map, reg, 12, 1, dir);
	intel_set_clk_val(ctx->map, reg, 8, 4, div);
	intel_set_clk_val(ctx->map, reg, 0, 1, en);
}

void intel_clk_ssc_probe(struct intel_clk_provider *ctx)
{
	struct device_node *nc, *np_ssc;
	u32 reg, en, spread, dir, div;

	np_ssc = of_find_node_by_name(NULL, "cgussc");
	if (!np_ssc)
		return;

	for_each_child_of_node(np_ssc, nc) {
		if (of_property_read_u32(nc, "reg", &reg))
			continue;
		if (of_property_read_u32(nc, "intel,ssc-enable", &en))
			continue;
		if (of_property_read_u32(nc, "intel,ssc-divval", &div))
			continue;
		if (!div) {
			dev_err(ctx->dev, "%s: %s divval is Zero!\n",
				__func__, nc->name);
			continue;
		}

		if (of_property_read_u32(nc, "intel,ssc-spread", &spread))
			spread = 0;
		if (of_property_read_u32(nc, "intel,ssc-dir", &dir))
			dir = 0;

		intel_clk_set_ssc(ctx, reg, spread, dir, div, en);
	}

	of_node_put(np_ssc);
}
