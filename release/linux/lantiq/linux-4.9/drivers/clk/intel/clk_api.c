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
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "clk_api.h"

#define to_gate_clk(_hw) container_of(_hw, struct gate_clk, hw)
#define to_clk_gate_dummy(_hw) container_of(_hw, struct gate_dummy_clk, hw)
#define to_clk_gate_single(_hw) container_of(_hw, struct gate_single_clk, hw)
#define to_div_clk(_hw) container_of(_hw, struct div_clk, hw)
#define to_mux_clk(_hw) container_of(_hw, struct mux_clk, hw)
#define to_clk_fixed_factor_frac(_hw) \
	container_of(_hw, struct clk_fixed_factor_frac, hw)
#define to_ljpll_div_clk(_hw) container_of(_hw, struct ljpll_div_clk, hw)
#define to_fixed_hw_input_clk(_hw) \
	container_of(_hw, struct clk_fixed_hw_input, hw)
#define to_ff_clk(_hw) container_of(_hw, struct clk_factor_frac, hw)

#define to_mask(width) (BIT(width) - 1)

#define PLL_EN_SHIFT		0
#define PLL_EN_WIDTH		1
#define PLL_LOCK_SHIFT		1
#define PLL_LOCK_WIDTH		1
#define PLL_FRAC_SHIFT		2
#define PLL_FRAC_WIDTH		26
#define PLL_LD_SHIFT		31
#define PLL_LD_WIDTH		1
#define PLL_MULT_SHIFT		0
#define PLL_MULT_WIDTH		12
#define PLL_DIV_SHIFT		18
#define PLL_DIV_WIDTH		6
#define FRAC_DIV		BIT(24)

struct regmap *clkregmap;

static inline void set_clk_val(struct regmap *map, u32 reg, u8 shift,
			       unsigned long mask, u32 set_val,
			       spinlock_t *lock)
{
	unsigned long flags = 0;
	u32 val;

	if (lock)
		spin_lock_irqsave(lock, flags);
	else
		__acquire(lock);

	regmap_read(map, reg, &val);
	val &= ~(mask << shift);
	val |= (set_val & mask) << shift;
	regmap_write(map, reg, val);

	if (lock)
		spin_unlock_irqrestore(lock, flags);
	else
		__release(lock);
}

static inline u32 get_clk_val(struct regmap *map, u32 reg, u8 shift,
			      unsigned long mask)
{
	u32 val;

	regmap_read(map, reg, &val);
	val >>= shift;
	val &= mask;

	return val;
}

/* Goto Grandpa node and get the regmap address */
static struct regmap *regmap_from_node(struct device_node *np)
{
	struct regmap *map;

	for ( ; np; ) {
		np = of_get_parent(np);
		if (!np)
			return ERR_PTR(-EINVAL);

		map = syscon_node_to_regmap(np);
		if (!IS_ERR(map))
			return map;
	}

	return ERR_PTR(-EINVAL);
}

static int of_get_clk_reg(struct device_node *np, u32 *reg)
{
	int ret;

	ret = of_property_read_u32(np, "reg", reg);
	if (ret)
		pr_err("%s no reg definition\n", np->name);

	return ret;
}

#define GATE_STAT_REG(reg)	(reg)
#define GATE_EN_REG(reg)	((reg) + 0x4)
#define GATE_DIS_REG(reg)	((reg) + 0x8)

static inline int get_gate(struct regmap *map, u32 reg, u8 shift)
{
	unsigned int val;

	regmap_read(map, GATE_STAT_REG(reg), &val);

	return !!(val & BIT(shift));
}

static inline void set_gate(struct regmap *map, u32 reg, u8 shift, int enable)
{
	if (enable)
		regmap_write(map, GATE_EN_REG(reg), BIT(shift));
	else
		regmap_write(map, GATE_DIS_REG(reg), BIT(shift));
}

void fixed_rate_clk_setup(struct device_node *node,
			  const struct fixed_rate_clk_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	unsigned long rate;
	struct regmap *regmap;
	u32 reg;

	if (!data)
		return;

	if (of_property_read_string(node, "clock-output-names", &clk_name))
		return;

	if (of_property_read_u32(node, "clock-frequency", (u32 *)&rate))
		rate = data->fixed_rate;
	if (!rate) {
		pr_err("clk(%s): Could not get fixed rate\n", clk_name);
		return;
	}

	regmap = regmap_from_node(node);
	if (IS_ERR(regmap))
		return;

	/* Register the fixed rate clock */
	clk = clk_register_fixed_rate(NULL, clk_name, NULL, 0, rate);
	if (IS_ERR(clk))
		goto __clk_reg_fail;

	/* Clock init */
	of_get_clk_reg(node, &reg);
	set_clk_val(regmap, reg, data->shift, to_mask(data->width),
		    data->setval, data->lock);

	/* Register clock provider */
	of_clk_add_provider(node, of_clk_src_simple_get, clk);
	return;

__clk_reg_fail:
	regmap_exit(regmap);
}

static int gate_clk_enable(struct clk_hw *hw)
{
	struct gate_clk *clk;

	clk = to_gate_clk(hw);
	if ((clk->flags & GATE_CLK_TYPE) == GATE_CLK)
		set_gate(clk->map, clk->reg, clk->bit_idx, 1);
	else
		set_clk_val(clk->map, clk->reg, clk->bit_idx, 1, 1, NULL);

	return 0;
}

static void gate_clk_disable(struct clk_hw *hw)
{
	struct gate_clk *clk;

	clk = to_gate_clk(hw);

	if ((clk->flags & GATE_CLK_TYPE) == GATE_CLK)
		set_gate(clk->map, clk->reg, clk->bit_idx, 0);
	else
		set_clk_val(clk->map, clk->reg, clk->bit_idx, 1, 0, NULL);
}

static int gate_clk_is_enabled(struct clk_hw *hw)
{
	struct gate_clk *clk;

	clk = to_gate_clk(hw);
	return get_gate(clk->map, clk->reg, clk->bit_idx);
}

static const struct clk_ops gate_clk_ops = {
	.enable = gate_clk_enable,
	.disable = gate_clk_disable,
	.is_enabled = gate_clk_is_enabled,
};

static struct clk
*gate_clk_register(struct device *dev, const char *name,
		   const char *parent_name,
		   unsigned long flags,
		   struct regmap *map, unsigned int reg,
		   u8 bit_idx, const struct gate_clk_data *data)
{
	struct gate_clk *gate;
	struct clk *clk;
	struct clk_init_data init;

	/* Allocate the gate */
	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &gate_clk_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);

	/* Struct gate_clk assignments */
	gate->map = map;
	gate->reg = reg;
	gate->bit_idx = bit_idx;
	gate->flags = data->flags;
	gate->hw.init = &init;

	clkregmap = map;
	clk = clk_register(dev, &gate->hw);
	if (IS_ERR(clk)) {
		pr_err("0x%4x %d %s %d %s\n", (u32)reg, init.num_parents,
		       parent_name, bit_idx, name);
		kfree(gate);
	}

	return clk;
}

void gate_clk_setup(struct device_node *node, const struct gate_clk_data *data)
{
	struct clk_onecell_data *clk_data;
	const char *clk_parent;
	const char *clk_name = node->name;
	struct regmap *regmap;
	unsigned int reg;
	int i, j, num;
	unsigned int val;

	if (!data || !data->reg_size) {
		pr_err("%s: register bit size cannot be 0!\n", __func__);
		return;
	}

	regmap = regmap_from_node(node);
	if (IS_ERR(regmap))
		return;

	of_get_clk_reg(node, &reg);
	clk_parent = of_clk_get_parent_name(node, 0);

	/* Size probe and memory allocation */
	num = find_last_bit(&data->mask, data->reg_size);
	clk_data = kmalloc(sizeof(*clk_data), GFP_KERNEL);
	if (!clk_data)
		return;

	clk_data->clks = kcalloc(num + 1, sizeof(struct clk *), GFP_KERNEL);
	if (!clk_data->clks)
		goto __clkarr_alloc_fail;

	i = 0;
	j = 0;
	for_each_set_bit(i, &data->mask, data->reg_size) {
		of_property_read_string_index(node, "clock-output-names",
					      j, &clk_name);

		clk_data->clks[j] = gate_clk_register(NULL, clk_name,
					clk_parent, 0, regmap, reg, i, data);
		WARN_ON(IS_ERR(clk_data->clks[j]));

		j++;
	}

	/* Adjust to the real max */
	clk_data->clk_num = num + 1;

	/* Initial gate default setting */
	if (data->flags & CLK_INIT_DEF_CFG_REQ) {
		val = (unsigned int)data->def_onoff;
		if (val)
			set_clk_val(regmap, GATE_EN_REG(reg), val, 0, -1, NULL);
		val = (((unsigned int)(~data->def_onoff)) & data->mask);
		if (val)
			set_clk_val(regmap, GATE_DIS_REG(reg),
				    val, 0, -1, NULL);
	}

	/* Register to clock provider */
	of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	return;

__clkarr_alloc_fail:
	kfree(clk_data);
}

static unsigned long get_div_tbl_rate(const struct div_tbl *table,
				      unsigned int val, unsigned int table_sz)
{
	unsigned int i;

	for (i = 0; i < table_sz; i++)
		if (table[i].div == val)
			return table[i].freq;

	pr_err("DIV table search failed for div: %u\n", val);

	return 0;
}

static int
get_round_rate_idx(const struct div_tbl *table,
		   unsigned long rate, unsigned int table_sz)
{
	int i;
	unsigned long rate_min, rate_max;

	WARN_ON(!table_sz);
	rate_min = rate_max = 0;
	for (i = 0; i < table_sz; i++) {
		if (table[i].freq == rate)
			return i;

		if (rate < table[i].freq) {
			if (!rate_min)
				return i;
			rate_max = table[i].freq;
			if ((rate_max - rate) >= (rate - rate_min))
				return i - 1;
			else
				return i;
		}
		rate_min = table[i].freq;
	}

	return i - 1;
}

static unsigned int get_div_by_rate(const struct div_tbl *table,
				    unsigned long rate, unsigned int mask,
				    unsigned int table_sz)
{
	int i;

	i = get_round_rate_idx(table, rate, table_sz);

	if (rate > table[table_sz - 1].freq) {
		pr_warn("request rate: %lu bigger than max rate: %lu\n",
			rate, table[i - 1].freq);
	}

	return table[i].div;
}

static unsigned long div_recalc_rate(struct div_clk *div,
				     unsigned long parent_rate,
				     unsigned int val)
{
	return divider_recalc_rate(&div->hw, parent_rate,
			val, div->div_table, div->flags);
}

static unsigned long div_clk_recalc_rate(struct clk_hw *hw,
					 unsigned long parent_rate)
{
	struct div_clk *div = to_div_clk(hw);
	unsigned int val;
	unsigned long rate;

	val = get_clk_val(div->map, div->reg, div->shift, to_mask(div->width));

	if (div->table)
		rate = get_div_tbl_rate(div->table, val, div->tbl_sz);
	else
		rate = div_recalc_rate(div, parent_rate, val);

	return rate;
}

static long div_clk_round_rate(struct clk_hw *hw,
			       unsigned long rate, unsigned long *prate)
{
	struct div_clk *div = to_div_clk(hw);
	int i;

	if (div->table) {
		i = get_round_rate_idx(div->table, rate, div->tbl_sz);
		return div->table[i].freq;
	}
	return divider_round_rate(hw, rate, prate,
				  div->div_table, div->width, div->flags);
}

static int div_set_rate(struct div_clk *div, unsigned long rate,
			unsigned long parent_rate)
{
	unsigned int val;

	val = divider_get_val(rate, parent_rate, div->div_table,
			      div->width, div->flags);
	set_clk_val(div->map, div->reg, div->shift,
		    to_mask(div->width), val, div->lock);
	return 0;
}

static int div_clk_set_rate(struct clk_hw *hw, unsigned long rate,
			    unsigned long parent_rate)
{
	struct div_clk *div = to_div_clk(hw);
	unsigned int val;

	if (div->table) {
		val =  get_div_by_rate(div->table, rate,
				       to_mask(div->width), div->tbl_sz);
		if (val >= to_mask(div->width))
			return -EINVAL;
		set_clk_val(div->map, div->reg, div->shift,
			    to_mask(div->width), val, div->lock);
		return 0;
	}
	return div_set_rate(div, rate, parent_rate);
}

const struct clk_ops clk_div_ops = {
	.recalc_rate = div_clk_recalc_rate,
	.round_rate = div_clk_round_rate,
	.set_rate = div_clk_set_rate,
};

static struct clk *div_clk_register(struct device *dev, const char *name,
				    const char *parent_name,
				    unsigned long flags,
				    struct regmap *map, unsigned int reg,
				    const struct div_clk_data *data)
{
	struct div_clk *div;
	struct clk *clk;
	struct clk_init_data init;

	/* Allocate the divider clock*/
	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &clk_div_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);

	/* Struct clk_divider assignments */
	div->map = map;
	div->reg = reg;
	div->shift = data->shift;
	div->width = data->width;
	div->flags = data->flags;
	div->div_table = data->div_table;
	div->lock = data->lock;
	div->hw.init = &init;
	div->table = data->table;
	div->tbl_sz = data->tbl_sz;

	clkregmap = map;
	/* Register the clock */
	clk = clk_register(dev, &div->hw);
	if (IS_ERR(clk))
		kfree(div);

	return clk;
}

void div_clk_setup(struct device_node *node, const struct div_clk_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *clk_parent;
	struct regmap *map;
	unsigned int reg;

	if (!data)
		return;

	map = regmap_from_node(node);
	if (IS_ERR(map))
		return;

	of_get_clk_reg(node, &reg);

	if (of_property_read_string(node, "clock-output-names", &clk_name))
		goto __clk_reg_fail;
	clk_parent = of_clk_get_parent_name(node, 0);

	clk = div_clk_register(NULL, clk_name, clk_parent, 0, map, reg, data);
	if (IS_ERR(clk))
		goto __clk_reg_fail;

	of_clk_add_provider(node, of_clk_src_simple_get, clk);
	return;

__clk_reg_fail:
	regmap_exit(map);
}

void cluster_div_clk_setup(struct device_node *node,
			   const struct div_clk_data *data, u32 num)
{
	struct clk_onecell_data *clk_data;
	const char *clk_name;
	const char *clk_parent;
	struct regmap *regmap;
	unsigned int reg;
	int i;

	if (!data || !num) {
		pr_err("%s: invalid array or array size!\n", __func__);
		return;
	}

	regmap = regmap_from_node(node);
	if (IS_ERR(regmap))
		return;

	of_get_clk_reg(node, &reg);

	clk_data = kmalloc(sizeof(*clk_data), GFP_KERNEL);
	if (!clk_data)
		goto __clkdata_alloc_fail;
	clk_data->clks = kcalloc(num, sizeof(struct clk *), GFP_KERNEL);
	if (!clk_data->clks)
		goto __clkarr_alloc_fail;

	clk_parent = of_clk_get_parent_name(node, 0);

	for (i = 0; i < num; i++) {
		of_property_read_string_index(node, "clock-output-names",
					      i, &clk_name);
		clk_data->clks[i] = div_clk_register(NULL, clk_name, clk_parent,
			0, regmap, reg, &data[i]);
		WARN_ON(IS_ERR(clk_data->clks[i]));
	}
	clk_data->clk_num = num + 1;

	of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	return;

__clkarr_alloc_fail:
	kfree(clk_data);
__clkdata_alloc_fail:
	regmap_exit(regmap);
}

static unsigned int mux_parent_from_table(const u32 *table,
					  unsigned int val, unsigned int num)
{
	unsigned int i;

	for (i = 0; i < num; i++)
		if (table[i] == val)
			return i;

	return -EINVAL;
}

static u8 mux_clk_get_parent(struct clk_hw *hw)
{
	struct mux_clk *mux = to_mux_clk(hw);
	int num_parents = clk_hw_get_num_parents(hw);
	unsigned int val;

	val = get_clk_val(mux->map, mux->reg, mux->shift, to_mask(mux->width));
	if (mux->table)
		return mux_parent_from_table(mux->table, val, num_parents);

	if (val && (mux->flags & CLK_MUX_INDEX_BIT))
		val = ffs(val) - 1;
	if (val && (mux->flags & CLK_MUX_INDEX_ONE))
		val -= 1;
	if (val >= num_parents)
		return -EINVAL;

	return val;
}

static int mux_clk_set_parent(struct clk_hw *hw, u8 index)
{
	struct mux_clk *mux = to_mux_clk(hw);

	if (mux->table) {
		index = mux->table[index];
	} else {
		if (mux->flags & CLK_MUX_INDEX_BIT)
			index = BIT(index);
		if (mux->flags & CLK_MUX_INDEX_ONE)
			index += 1;
	}

	set_clk_val(mux->map, mux->reg, mux->shift,
		    to_mask(mux->width), index, mux->lock);

	return 0;
}

static int mux_clk_mux_determine_rate(struct clk_hw *hw,
			     struct clk_rate_request *req)
{
	int ret;

	ret = __clk_mux_determine_rate(hw, req);

	return ret;
}

const struct clk_ops mux_clk_ops = {
	.get_parent = mux_clk_get_parent,
	.set_parent = mux_clk_set_parent,
	.determine_rate = mux_clk_mux_determine_rate,
};

static struct clk *mux_clk_register(struct device *dev, const char *name,
				    const char * const *parent_names,
				    u8 num_parents, unsigned long flags,
				    struct regmap *map, unsigned int reg,
				    const struct mux_clk_data *data)
{
	struct mux_clk *mux;
	struct clk_init_data init;
	struct clk *clk;

	/* allocate mux clk */
	mux = kzalloc(sizeof(*mux), GFP_KERNEL);
	if (!mux)
		return ERR_PTR(-ENOMEM);

	/* struct init assignments */
	init.name = name;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = parent_names;
	init.num_parents = num_parents;
	init.ops = &mux_clk_ops;

	/* struct mux_clk assignments */
	mux->map = map;
	mux->reg = reg;
	mux->shift = data->shift;
	mux->width = data->width;
	mux->flags = data->clk_flags;
	mux->table = data->table;
	mux->lock = data->lock;
	mux->hw.init = &init;

	clkregmap = map;
	clk = clk_register(dev, &mux->hw);
	if (IS_ERR(clk))
		kfree(mux);

	return clk;
}

void mux_clk_setup(struct device_node *node, const struct mux_clk_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char **parents;
	unsigned int num_parents;
	struct regmap *map;
	unsigned int reg;

	if (!data)
		return;

	map = regmap_from_node(node);
	if (IS_ERR(map))
		return;

	of_get_clk_reg(node, &reg);

	of_property_read_string(node, "clock-output-names", &clk_name);
	clkregmap = map;
	if (data->mux_flags & CLK_INIT_DEF_CFG_REQ) {
		set_clk_val(map, reg, data->shift, to_mask(data->width),
			    data->def_val, data->lock);
	}

	num_parents = of_clk_get_parent_count(node);
	if (num_parents) {
		parents = kmalloc_array(num_parents,
					sizeof(char *), GFP_KERNEL);
		if (!parents)
			goto __clk_reg_fail;
		of_clk_parent_fill(node, parents, num_parents);
	} else {
		pr_err("%s: mux clk no parent!\n", __func__);
		goto __clk_reg_fail;
	}

	clk = mux_clk_register(NULL, clk_name, parents, num_parents,
			       data->flags, map, reg, data);
	if (IS_ERR(clk))
		goto __mux_clk_fail;

	of_clk_add_provider(node, of_clk_src_simple_get, clk);
	return;

__mux_clk_fail:
	kfree(parents);
__clk_reg_fail:
	regmap_exit(map);
}

static int gate_clk_dummy_enable(struct clk_hw *hw)
{
	struct gate_dummy_clk *clk;

	clk = to_clk_gate_dummy(hw);
	clk->clk_status = 1;

	return 0;
}

static void gate_clk_dummy_disable(struct clk_hw *hw)
{
	struct gate_dummy_clk *clk;

	clk = to_clk_gate_dummy(hw);
	clk->clk_status = 0;
}

static int gate_clk_dummy_is_enabled(struct clk_hw *hw)
{
	struct gate_dummy_clk *clk;

	clk = to_clk_gate_dummy(hw);
	return clk->clk_status;
}

static const struct clk_ops clk_gate_dummy_ops = {
	.enable = gate_clk_dummy_enable,
	.disable = gate_clk_dummy_disable,
	.is_enabled = gate_clk_dummy_is_enabled,
};

static struct clk *clk_register_gate_dummy(struct device *dev, const char *name,
					   const char *parent_name,
					   unsigned long flags,
					 const struct gate_dummy_clk_data *data)
{
	struct gate_dummy_clk *gate_clk;
	struct clk *clk;
	struct clk_init_data init;

	/* Allocate the gate_dummy clock*/
	gate_clk = kzalloc(sizeof(*gate_clk), GFP_KERNEL);
	if (!gate_clk)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &clk_gate_dummy_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);
	gate_clk->hw.init = &init;

	/* Struct gate_clk assignments */
	if (data->flags & CLK_INIT_DEF_CFG_REQ)
		gate_clk->clk_status = data->def_val & 0x1;

	/* Register the clock */
	clk = clk_register(dev, &gate_clk->hw);
	if (IS_ERR(clk))
		kfree(gate_clk);

	return clk;
}

void gate_dummy_clk_setup(struct device_node *node,
			  const struct gate_dummy_clk_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;

	if (!data)
		return;

	if (of_property_read_string(node, "clock-output-names", &clk_name))
		return;

	clk = clk_register_gate_dummy(NULL, clk_name, NULL, 0, data);
	if (IS_ERR(clk)) {
		pr_err("%s: dummy gate clock register fail!\n", __func__);
		return;
	}

	of_clk_add_provider(node, of_clk_src_simple_get, clk);
}

static void pll_get_param(struct regmap *map, u32 reg,
			  u32 *mult, u32 *div, u32 *frac)
{
	if (frac)
		*frac = get_clk_val(map, reg,
				PLL_FRAC_SHIFT, to_mask(PLL_FRAC_WIDTH));
	if (mult)
		*mult = get_clk_val(map, reg + 0x8,
				PLL_MULT_SHIFT, to_mask(PLL_MULT_WIDTH));
	if (div)
		*div = get_clk_val(map, reg + 0x8,
				PLL_DIV_SHIFT, to_mask(PLL_DIV_WIDTH));
}

static void pll_clk_setup(struct device_node *node)
{
	struct regmap *regmap;
	struct clk *clk;
	const char *clk_name = node->name;
	const char *parent_name;
	u32 reg, div, mult, ex_div;

	regmap = regmap_from_node(node);
	if (IS_ERR(regmap))
		return;

	of_property_read_string(node, "clock-output-names", &clk_name);
	if (of_get_clk_reg(node, &reg)) {
		pr_err("%s:PLL clk <%s> need register address!\n",
		       __func__, clk_name);
		return;
	}
	pll_get_param(regmap, reg, &mult, &div, NULL);
	if (!of_property_read_u32(node, "exdiv", &ex_div))
		div = div * ex_div;
	parent_name = of_clk_get_parent_name(node, 0);

	clk = clk_register_fixed_factor(NULL, clk_name, parent_name,
					0, mult, div);

	if (IS_ERR(clk))
		return;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk))
		clk_unregister(clk);
}

static unsigned long clk_factor_frac_recalc_rate(struct clk_hw *hw,
						 unsigned long parent_rate)
{
	struct clk_fixed_factor_frac *fix = to_clk_fixed_factor_frac(hw);
	unsigned long long rate;
	unsigned int mult, div, frac, fdiv;

	mult = fix->mult;
	div = fix->div;
	frac = fix->frac;
	fdiv = fix->frac_div;

	rate = (unsigned long long)mult * fdiv;
	rate = parent_rate * (rate + frac);
	do_div(rate, fdiv);
	do_div(rate, div);

	return (unsigned long)rate;
}

/**
 * Original rate formula:
 * Fvco_clk = Fosc_clk * (mult + frac / frac_div) / div
 * Transformed formula:
 * Fvco_clk = Fosc_clk * (mult * frac_div + frac) / div / frac_div
 *
 * Original parent rate Formula:
 * Fosc_clk = Fvco_clk * div / (mult + frac / frac_div)
 * Transformed formula:
 * Fosc_clk = Fvco_clk * div * frac_div / (mult + frac)
 */
static long clk_factor_frac_round_rate(struct clk_hw *hw, unsigned long rate,
				       unsigned long *prate)
{
	struct clk_fixed_factor_frac *fix = to_clk_fixed_factor_frac(hw);
	struct clk_hw *parent_hw;
	unsigned int mult, div, frac, fdiv;
	unsigned long long frac_calc;

	mult = fix->mult;
	div = fix->div;
	frac = fix->frac;
	fdiv = fix->frac_div;

	if (clk_hw_get_flags(hw) & CLK_SET_RATE_PARENT) {
		unsigned long best_parent;

		frac_calc = rate * div * fdiv;
		do_div(frac_calc, (mult + frac));
		best_parent = frac_calc;
		parent_hw = clk_hw_get_parent(hw);
		if (parent_hw)
			*prate = clk_hw_round_rate(parent_hw, best_parent);
		else
			pr_err("%s: clk no parent!\n", __func__);
	}

	frac_calc = (unsigned long long)mult * fdiv;
	frac_calc = *prate * (frac_calc + frac);
	do_div(frac_calc, fdiv);
	do_div(frac_calc, div);
	rate = (unsigned long)frac_calc;

	return rate;
}

static int clk_factor_frac_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	/*
	 * We must report success but we can do so unconditionally because
	 * clk_factor_round_rate returns values that ensure this call is a
	 * nop.
	 */

	return 0;
}

const struct clk_ops clk_fixed_factor_frac_ops = {
	.round_rate = clk_factor_frac_round_rate,
	.set_rate = clk_factor_frac_set_rate,
	.recalc_rate = clk_factor_frac_recalc_rate,
};

static struct clk_hw
*clk_hw_register_fixed_factor_frac(struct device *dev,
				   const char *name, const char *parent_name,
				   unsigned long flags, unsigned int mult,
				   unsigned int div, unsigned int frac,
				   unsigned int frac_div)
{
	struct clk_fixed_factor_frac *fix;
	struct clk_init_data init;
	struct clk_hw *hw;
	int ret;

	fix = kmalloc(sizeof(*fix), GFP_KERNEL);
	if (!fix)
		return ERR_PTR(-ENOMEM);

	/* struct clk_fixed_factor assignments */
	fix->mult = mult;
	fix->div = div;
	fix->frac = frac;
	fix->frac_div = frac_div;
	fix->hw.init = &init;

	init.name = name;
	init.ops = &clk_fixed_factor_frac_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = &parent_name;
	init.num_parents = 1;

	hw = &fix->hw;
	ret = clk_hw_register(dev, hw);
	if (ret) {
		kfree(fix);
		hw = ERR_PTR(ret);
	}

	return hw;
}

static struct clk
*fixed_factor_frac_clk_register(struct device *dev,
				const char *name, const char *parent_name,
				unsigned long flags, unsigned int mult,
				unsigned int div, unsigned int frac,
				unsigned int frac_div)
{
	struct clk_hw *hw;

	hw = clk_hw_register_fixed_factor_frac(dev, name, parent_name,
					       flags, mult, div,
					       frac, frac_div);
	if (IS_ERR(hw))
		return ERR_CAST(hw);
	return hw->clk;
}

static void pllfrac_clk_setup(struct device_node *node)
{
	struct regmap *regmap;
	struct clk *clk;
	const char *clk_name = node->name;
	const char *parent_name;
	u32 reg, div, mult, frac, ex_div;

	regmap = regmap_from_node(node);
	if (IS_ERR(regmap))
		return;

	of_property_read_string(node, "clock-output-names", &clk_name);
	if (of_get_clk_reg(node, &reg)) {
		pr_err("%s:PLL clk <%s> need register address!\n",
		       __func__, clk_name);
		return;
	}

	pll_get_param(regmap, reg, &mult, &div, &frac);
	if (!of_property_read_u32(node, "exdiv", &ex_div))
		div = div * ex_div;

	parent_name = of_clk_get_parent_name(node, 0);

	clk = fixed_factor_frac_clk_register(NULL, clk_name, parent_name, 0,
					     mult, div, frac, FRAC_DIV);

	if (IS_ERR(clk))
		goto __clk_reg_fail;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		clk_unregister(clk);
		goto __clk_reg_fail;
	}

	return;

__clk_reg_fail:
	regmap_exit(regmap);
}

static int
pll_clk_set(struct regmap *map, u32 reg,
	    const struct factor_frac_tbl *ff_tbl,
	    spinlock_t *lock)
{
	unsigned int mult, div, frac;
	const int loop_cnt = 100;
	int i;

	mult = ff_tbl->mult;
	div = ff_tbl->div;
	frac = ff_tbl->frac;

	clkregmap = map;
	set_clk_val(map, reg, PLL_FRAC_SHIFT,
		    to_mask(PLL_FRAC_WIDTH), frac, lock);
	set_clk_val(map, reg + 0x8, PLL_MULT_SHIFT,
		    to_mask(PLL_MULT_WIDTH), mult, lock);
	set_clk_val(map, reg + 0x8, PLL_DIV_SHIFT,
		    to_mask(PLL_DIV_WIDTH), div, lock);

	set_clk_val(map, reg, PLL_LD_SHIFT, to_mask(PLL_LD_WIDTH), 1, lock);

	/* PLL enable */
	if (!get_clk_val(map, reg, PLL_EN_SHIFT, to_mask(PLL_EN_WIDTH))) {
		set_clk_val(map, reg, PLL_EN_SHIFT,
			    to_mask(PLL_EN_WIDTH), 1, lock);
		udelay(1);
	}

	for (i = 0; i < loop_cnt; i++) {
		if (!get_clk_val(map, reg, PLL_LOCK_SHIFT,
				 to_mask(PLL_LOCK_WIDTH)))
			udelay(1);
		else
			return 0;
	}

	pr_err("PLL failed to get lock!\n");
	return -ENODEV;
}

static unsigned long
clk_ff_recalc_rate(struct clk_hw *hw,
		   unsigned long parent_rate)
{
	struct clk_factor_frac *ff = to_ff_clk(hw);
	unsigned long long rate;
	unsigned int mult, div, frac, fdiv;

	pll_get_param(ff->map, ff->reg, &mult, &div, &frac);
	fdiv = ff->frac_div;
	div = div * ff->exdiv;

	if (!frac) {
		rate =  (unsigned long long)parent_rate * mult;
		do_div(rate, div);
	} else {
		rate = (unsigned long long)mult * fdiv;
		rate = parent_rate * (rate + frac);
		do_div(rate, fdiv);
		do_div(rate, div);
	}

	return (unsigned long)rate;
}

/**
 * Original rate formula:
 * Fvco_clk = Fosc_clk * (mult + frac / frac_div) / div
 * Transformed formula:
 * Fvco_clk = Fosc_clk * (mult * frac_div + frac) / div / frac_div
 * in case (frac == 0):
 * Fvco_clk = Fosc_clk * mult / div
 *
 * Original parent rate Formula:
 * Fosc_clk = Fvco_clk * div / (mult + frac / frac_div)
 * Transformed formula:
 * Fosc_clk = Fvco_clk * div * frac_div / (mult + frac)
 * in case (frac == 0):
 * Fosc_clk = Fvco_clk * div / mult
 */
static long clk_ff_round_rate(struct clk_hw *hw, unsigned long rate,
			      unsigned long *prate)
{
	struct clk_factor_frac *ff = to_ff_clk(hw);
	struct clk_hw *parent_hw;
	unsigned int mult, div, frac, fdiv, i;
	unsigned long long frac_calc;
	unsigned long best_parent;
	unsigned long long cal_rate;

	for (i = 0; i < ff->tblsz; i++) {
		cal_rate = ff->tbl[i].rate;
		do_div(cal_rate, ff->exdiv);

		if (*prate == ff->tbl[i].parent_rate
		    && rate == (unsigned long)cal_rate) {
			return rate;
		}
	}

	pll_get_param(ff->map, ff->reg, &mult, &div, &frac);
	fdiv = ff->frac_div;
	div = div * ff->exdiv;

	if (clk_hw_get_flags(hw) & CLK_SET_RATE_PARENT) {
		if (!frac) {
			frac_calc = rate * div;
			do_div(frac_calc, mult);
		} else {
			frac_calc = rate * div * fdiv;
			do_div(frac_calc, (mult + frac));
		}

		best_parent = frac_calc;
		parent_hw = clk_hw_get_parent(hw);
		if (parent_hw)
			*prate = clk_hw_round_rate(parent_hw, best_parent);
		else
			pr_err("%s: clk no parent!\n", __func__);
	}

	if (!frac) {
		frac_calc = *prate * mult;
		do_div(frac_calc, div);
	} else {
		frac_calc = (unsigned long long)mult * fdiv;
		frac_calc = *prate * (frac_calc + frac);
		do_div(frac_calc, fdiv);
		do_div(frac_calc, div);
	}
	rate = (unsigned long)frac_calc;

	return rate;
}

static int clk_ff_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	struct clk_factor_frac *ff = to_ff_clk(hw);
	int i;
	int idx = -1;
	unsigned long long cal_rate;

	/* No table means not allowed to set rate */
	if (!ff->tblsz)
		return 0;

	for (i = 0; i < ff->tblsz; i++) {
		cal_rate = ff->tbl[i].rate;
		do_div(cal_rate, ff->exdiv);

		if (parent_rate == ff->tbl[i].parent_rate
		    && rate == (unsigned long)cal_rate) {
			idx = i;
			break;
		}
	}

	if (idx < 0)
		return -EINVAL;

	pll_clk_set(ff->map, ff->reg, &ff->tbl[idx], ff->lock);

	return 0;
}

const struct clk_ops clk_ff_ops = {
	.round_rate = clk_ff_round_rate,
	.set_rate = clk_ff_set_rate,
	.recalc_rate = clk_ff_recalc_rate,
};

static struct clk_hw
*clk_hw_register_ff(struct device *dev,
		    const char *name, const char *parent_name,
		    const struct clk_factor_frac_data *data,
		    unsigned int exdiv, struct regmap *regmap,
		    u32 reg)
{
	struct clk_factor_frac *ff;
	struct clk_init_data init;
	struct clk_hw *hw;
	int ret;

	ff = kmalloc(sizeof(*ff), GFP_KERNEL);
	if (!ff)
		return ERR_PTR(-ENOMEM);

	/* struct clk_fixed_factor assignments */
	ff->tbl = data->ff_tbl;
	ff->tblsz = (unsigned int)data->ff_tblsz;
	ff->frac_div = data->frac_div;
	ff->exdiv = exdiv;
	ff->map = regmap;
	ff->reg = reg;
	ff->lock = data->lock;
	ff->hw.init = &init;

	init.name = name;
	init.ops = &clk_ff_ops;
	init.flags = data->clk_flags | CLK_IS_BASIC;
	init.parent_names = &parent_name;
	init.num_parents = 1;

	hw = &ff->hw;
	ret = clk_hw_register(dev, hw);
	if (ret) {
		kfree(ff);
		hw = ERR_PTR(ret);
	}

	return hw;
}

static struct clk
*ff_clk_register(struct device *dev,
		 const char *name, const char *parent_name,
		 const struct clk_factor_frac_data *data,
		 unsigned int exdiv, struct regmap *regmap,
		 u32 reg)
{
	struct clk_hw *hw;

	hw = clk_hw_register_ff(dev, name, parent_name,
				data, exdiv, regmap, reg);
	if (IS_ERR(hw))
		return ERR_CAST(hw);
	return hw->clk;
}

void
pll_ff_clk_setup(struct device_node *node,
		 const struct clk_factor_frac_data *data)
{
	struct regmap *regmap;
	struct clk *clk;
	const char *clk_name = node->name;
	const char *parent_name;
	u32 reg, ex_div;

	regmap = regmap_from_node(node);
	if (IS_ERR(regmap))
		return;

	of_property_read_string(node, "clock-output-names", &clk_name);
	if (of_get_clk_reg(node, &reg)) {
		pr_err("%s:PLL clk <%s> need register address!\n",
		       __func__, clk_name);
		return;
	}

	if (data->flags & CLK_INIT_DEF_CFG_REQ) {
		pll_clk_set(regmap, reg,
			    &data->ff_tbl[data->def_idx],
			    data->lock);
	}

	if (of_property_read_u32(node, "exdiv", &ex_div))
		ex_div = 1;

	parent_name = of_clk_get_parent_name(node, 0);

#if 0
	if (!of_property_read_u32(node, "clk-ssc", &ssc)) {
		if (!ssc)
			return;
		np = of_find_node_by_phandle(ssc);
		ssc_setup(regmap, np);
	}
#endif

	clk = ff_clk_register(NULL, clk_name, parent_name,
			      data, ex_div, regmap, reg);

	if (IS_ERR(clk))
		goto __clk_reg_fail;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		clk_unregister(clk);
		goto __clk_reg_fail;
	}

	return;

__clk_reg_fail:
	regmap_exit(regmap);
}

static long clk_fixed_input_round_rate(struct clk_hw *hw, unsigned long rate,
				       unsigned long *prate)
{
	struct clk_fixed_hw_input *fix;

	fix = to_fixed_hw_input_clk(hw);
	fix->clk_rate = rate;
	return fix->clk_rate;
}

/* input rate will be the clock rate */
static int clk_fixed_input_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	struct clk_fixed_hw_input *fix;

	fix = to_fixed_hw_input_clk(hw);
	fix->clk_rate = rate;

	return 0;
}

static unsigned long
clk_fixed_input_recalc_rate(struct clk_hw *hw,
			    unsigned long parent_rate)
{

	struct clk_fixed_hw_input *fix;

	fix = to_fixed_hw_input_clk(hw);
	return fix->clk_rate;
}

const struct clk_ops clk_fixed_input_ops = {
	.round_rate = clk_fixed_input_round_rate,
	.set_rate = clk_fixed_input_set_rate,
	.recalc_rate = clk_fixed_input_recalc_rate,
};

static struct clk_hw
*clk_hw_register_fixed_hw_input(struct device *dev,
				const char *name, unsigned long rate,
				unsigned long flags)
{
	struct clk_fixed_hw_input *fix;
	struct clk_init_data init;
	struct clk_hw *hw;
	int ret;

	fix = kmalloc(sizeof(*fix), GFP_KERNEL);
	if (!fix)
		return ERR_PTR(-ENOMEM);

	/* struct clk_fixed_factor assignments */
	fix->clk_rate = rate;
	fix->hw.init = &init;

	init.name = name;
	init.ops = &clk_fixed_input_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = NULL;
	init.num_parents = 0;

	hw = &fix->hw;
	ret = clk_hw_register(dev, hw);
	if (ret) {
		kfree(fix);
		hw = ERR_PTR(ret);
	}

	return hw;
}

static struct clk
*fixed_hw_input_clk_register(struct device *dev,
			     const char *name, unsigned long rate,
			     unsigned long flags)
{
	struct clk_hw *hw;

	hw = clk_hw_register_fixed_hw_input(dev, name, rate, flags);

	if (IS_ERR(hw))
		return ERR_CAST(hw);

	return hw->clk;
}

static void hw_input_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	unsigned int rate;

	of_property_read_string(node, "clock-output-names", &clk_name);

	if (of_property_read_u32(node, "clock-rate", &rate))
		rate = 0;

	clk = fixed_hw_input_clk_register(NULL, clk_name,
					  (unsigned long)rate, 0);

	if (IS_ERR(clk))
		goto __clk_reg_fail;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		clk_unregister(clk);
		goto __clk_reg_fail;
	}

	return;

__clk_reg_fail:
	return;
}

static struct clk_hw
*hw_div_register(struct device *dev, const char *name,
		 const char *parent_name, unsigned long flags,
		 struct regmap *map, unsigned int reg, u8 shift,
		 u8 width, unsigned long clk_flags,
		 const struct clk_div_table *table, spinlock_t *lock)
{
	struct div_clk *div;
	struct clk *clk;
	struct clk_init_data init;
	struct clk_hw *hw;

	/* Allocate the divider clock*/
	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &clk_div_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);

	/* Struct clk_divider assignments */
	div->map = map;
	div->reg = reg;
	div->shift = shift;
	div->width = width;
	div->flags = clk_flags;
	div->div_table = table;
	div->lock = lock;
	div->hw.init = &init;

	/* Register the clock */
	hw = &div->hw;
	clk = clk_register(dev, hw);
	if (IS_ERR(clk)) {
		kfree(div);
		hw = ERR_PTR((long)clk);
	}

	return hw;
}

static struct clk *clk_div_register(struct device *dev, const char *name,
				    const char *parent_name,
				    unsigned long flags, struct regmap *map,
				    unsigned int reg,
				    const struct clk_div_data *data)
{
	struct clk_hw *hw;

	hw = hw_div_register(dev, name, parent_name, flags,
			     map, reg, data->shift, data->width, 0,
			     data->table, data->lock);
	if (IS_ERR(hw))
		return ERR_CAST(hw);
	return hw->clk;
}

void clk_div_setup(struct device_node *node, const struct clk_div_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *clk_parent;
	struct regmap *map;
	unsigned int reg;

	if (!data)
		return;

	map = regmap_from_node(node);
	if (IS_ERR(map))
		return;

	of_get_clk_reg(node, &reg);
	of_property_read_string(node, "clock-output-names", &clk_name);
	clk_parent = of_clk_get_parent_name(node, 0);

	clk = clk_div_register(NULL, clk_name, clk_parent,
			       data->flags, map, reg, data);
	if (IS_ERR(clk))
		goto __clk_reg_fail;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		clk_unregister(clk);
		goto __clk_reg_fail;
	}

	return;

__clk_reg_fail:
	regmap_exit(map);
}

static unsigned long
ljpll_div_clk_recalc_rate(struct clk_hw *hw,
			  unsigned long parent_rate)
{
	struct ljpll_div_clk *div = to_ljpll_div_clk(hw);
	unsigned int val;
	unsigned long rate = 0;
	unsigned long long frate;
	int i;

	for (i = 0; i < div->mdiv_tblsz; i++) {
		val = get_clk_val(div->map, div->reg,
				  div->mdiv_tbl[i].shift,
				  to_mask(div->mdiv_tbl[i].width));
		rate = divider_recalc_rate(&div->hw, parent_rate,
					   val, div->div_table, div->flags);
		parent_rate = rate;
	}

	if (div->opt_factor) {
		val = get_clk_val(div->map, div->reg,
				  div->opt_factor->shift,
				  to_mask(div->opt_factor->width));
		if (val) {
			frate = parent_rate * div->opt_factor->mult;
			do_div(frate, div->opt_factor->div);
			rate = (unsigned long)frate;
		}
	}

	if (!rate)
		pr_err("clk: %s not correctly initialized\n",
		       hw->init->name);
	return rate;
}

static long
ljpll_div_clk_round_rate(struct clk_hw *hw,
			 unsigned long rate, unsigned long *prate)
{
	if (clk_hw_get_flags(hw) & CLK_SET_RATE_PARENT) {
		unsigned long best_parent;
		struct clk_hw *parent_hw;

		best_parent = rate;
		parent_hw = clk_hw_get_parent(hw);
		if (parent_hw && strcmp(clk_hw_get_name(hw), "pll5_clk") == 0) {
			struct clk_factor_frac *ff = to_ff_clk(parent_hw);
			int i;

			for (i = 0; i < ff->tblsz; i++) {
				if (ff->tbl[i].parent_rate == rate)
					*prate = rate * ff->tbl[i].div;
			}
		}
	}

	return rate;
}

static int ljpll_div_clk_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	return 0;
}

const struct clk_ops clk_ljpll_div_ops = {
	.recalc_rate = ljpll_div_clk_recalc_rate,
	.round_rate = ljpll_div_clk_round_rate,
	.set_rate = ljpll_div_clk_set_rate,
};

static struct clk_hw
*hw_ljpll_div_register(struct device *dev, const char *name,
		       const char *parent_name, unsigned long flags,
		       struct regmap *map, unsigned int reg,
		       const struct clk_ljpll_div_data *data)
{
	struct ljpll_div_clk *div;
	struct clk *clk;
	struct clk_init_data init;
	struct clk_hw *hw;

	/* Allocate the divider clock*/
	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &clk_ljpll_div_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);

	/* Struct clk_divider assignments */
	div->map = map;
	div->reg = reg;
	div->mdiv_tbl = data->mdiv_tbl;
	div->mdiv_tblsz = data->mdiv_tblsz;
	div->opt_factor = data->opt_factor;
	div->flags = data->clk_flags;
	div->div_table = data->table;
	div->lock = data->lock;
	div->hw.init = &init;

	/* Register the clock */
	hw = &div->hw;
	clk = clk_register(dev, hw);
	if (IS_ERR(clk)) {
		kfree(div);
		hw = ERR_PTR((long)clk);
	}

	return hw;
}

static struct clk
*clk_ljpll_div_register(struct device *dev, const char *name,
			const char *parent_name, unsigned long flags,
			struct regmap *map, unsigned int reg,
			const struct clk_ljpll_div_data *data)
{
	struct clk_hw *hw;

	hw = hw_ljpll_div_register(dev, name, parent_name, flags,
				   map, reg, data);
	if (IS_ERR(hw))
		return ERR_CAST(hw);
	return hw->clk;
}

void clk_ljpll_div_setup(struct device_node *node,
			 const struct clk_ljpll_div_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *clk_parent;
	struct regmap *map;
	unsigned int reg;
	int i;

	if (!data)
		return;

	map = regmap_from_node(node);
	if (IS_ERR(map))
		return;

	of_get_clk_reg(node, &reg);

	clkregmap = map;
	of_property_read_string(node, "clock-output-names", &clk_name);
	if (data->flags & CLK_INIT_DEF_CFG_REQ) {
		for (i = 0; i < data->mdiv_tblsz; i++)
			set_clk_val(map, reg, data->mdiv_tbl[i].shift,
				    to_mask(data->mdiv_tbl[i].width),
				    data->mdiv_tbl[i].def_val, data->lock);
		if (data->opt_factor) {
			set_clk_val(map, reg, data->opt_factor->shift,
				    to_mask(data->opt_factor->width),
				    data->opt_factor->def_val, data->lock);
		}
	}

	clk_parent = of_clk_get_parent_name(node, 0);

	clk = clk_ljpll_div_register(NULL, clk_name, clk_parent,
				     data->clk_flags, map, reg, data);
	if (IS_ERR(clk))
		goto __clk_reg_fail;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		clk_unregister(clk);
		goto __clk_reg_fail;
	}

	return;

__clk_reg_fail:
	regmap_exit(map);
}

void clk_fixed_factor_setup(struct device_node *node,
			    const struct clk_fixed_factor_data *data)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *clk_parent;
	struct regmap *map;
	unsigned int reg;
	unsigned int mult, div;

	if (!data)
		return;

	map = regmap_from_node(node);
	if (IS_ERR(map))
		return;

	of_get_clk_reg(node, &reg);
	of_property_read_string(node, "clock-output-names", &clk_name);
	clk_parent = of_clk_get_parent_name(node, 0);

	if (of_property_read_u32(node, "clock-div", &div)) {
		pr_err("%s: intel fixed factor clock missing div property\n",
		       __func__);
		goto __clk_reg_fail;
	}

	if (of_property_read_u32(node, "clock-mult", &mult)) {
		pr_err("%s: intel fixed factor clock missing mult property\n",
		       __func__);
		goto __clk_reg_fail;
	}

	clk = clk_register_fixed_factor(NULL, clk_name, clk_parent,
					data->flags, mult, div);
	if (IS_ERR(clk))
		goto __clk_reg_fail;

	if (of_clk_add_provider(node, of_clk_src_simple_get, clk)) {
		clk_unregister(clk);
		goto __clk_reg_fail;
	}

	clkregmap = map;
	/* set default value */
	set_clk_val(map, reg, data->shift, to_mask(data->width),
		    data->set_val, data->lock);

	return;

__clk_reg_fail:
	regmap_exit(map);
}

CLK_OF_DECLARE(pllfracclk, INTEL_FIXED_FACTOR_FRAC_PLLCLK, pllfrac_clk_setup);
CLK_OF_DECLARE(pllclk, INTEL_FIXED_FACTOR_PLLCLK, pll_clk_setup);
CLK_OF_DECLARE(hwiptclk, INTEL_FIXED_HW_INPUT_CLK, hw_input_clk_setup);
