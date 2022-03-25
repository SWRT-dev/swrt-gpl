/*
 *
 *  Copyright(c) 2016 Intel Corporation.
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
#ifndef __INTEL_CLK_API_H
#define __INTEL_CLK_API_H

#include <linux/of.h>

struct div_tbl {
	unsigned int div;
	unsigned long freq;
};

struct div_clk_data {
	u8 shift;
	u8 width;
	const struct div_tbl *table;
	const unsigned int tbl_sz;
	const struct clk_div_table *div_table;
	spinlock_t *lock; /* exclusive register access */
	unsigned long flags;
};

struct mux_clk_data {
	u8 shift;
	u8 width;
	const u32 *table;
	spinlock_t *lock; /* exclusive register access */
	u32 def_val;
	unsigned long mux_flags;
	unsigned long flags;
	unsigned long clk_flags;
};

struct gate_clk_data {
	unsigned long mask;
	unsigned long def_onoff;
	u8 reg_size;
	unsigned long flags;
};

struct gate_dummy_clk_data {
	unsigned int def_val;
	unsigned long flags;
};

struct fixed_rate_clk_data {
	u8 shift;
	u8 width;
	unsigned long fixed_rate;
	unsigned int setval;
	spinlock_t *lock; /* exclusive register access */
};

struct gate_dummy_clk {
	struct clk_hw hw;
	unsigned int clk_status;
};

struct div_clk {
	struct clk_hw hw;
	struct regmap *map;
	unsigned int reg;
	u8 shift;
	u8 width;
	unsigned int flags;
	const struct clk_div_table *div_table;
	spinlock_t *lock; /* exclusive register access */
	const struct div_tbl *table;
	unsigned int tbl_sz;
};

struct mdiv_table {
	u8 shift;
	u8 width;
	u8 def_val;
};

struct factor_clk_data {
	u8 shift;
	u8 width;
	u32 mult;
	u32 div;
	u8 def_val;
};

struct ljpll_div_clk {
	struct clk_hw hw;
	struct regmap *map;
	unsigned int reg;
	const struct mdiv_table *mdiv_tbl;
	u8 mdiv_tblsz;
	const struct factor_clk_data *opt_factor; /* optional factor clk */
	unsigned int flags;
	const struct clk_div_table *div_table;
	spinlock_t *lock; /* exclusive register access */
};

struct gate_clk {
	struct clk_hw hw;
	struct regmap *map;
	unsigned int reg;
	u8 bit_idx;
	unsigned int flags;
	spinlock_t *lock; /* exclusive register access */
};

struct gate_single_clk_data {
	unsigned long mask;
	unsigned long def_onoff;
	u8 reg_size;
	unsigned long flags;
};

struct gate_single_clk {
	struct clk_hw hw;
	struct regmap *map;
	unsigned int reg;
	u8 bit_idx;
	unsigned int flags;
	spinlock_t *lock; /* exclusive register access */
	unsigned int clk_status;
};

struct mux_clk {
	struct clk_hw hw;
	struct regmap *map;
	unsigned int reg;
	const u32 *table;
	u8 shift;
	u8 width;
	unsigned int flags;
	spinlock_t *lock; /* exclusive register access */
};

/* Applicable to normal PLL */
struct clk_div_data {
	u8 shift;
	u8 width;
	const struct clk_div_table *table;
	spinlock_t *lock; /* exclusive register access */
	unsigned long flags;
};

/* multiple divider, Applicable to LJPLL */
struct clk_ljpll_div_data {
	const struct mdiv_table *mdiv_tbl;
	const u8 mdiv_tblsz;
	const struct factor_clk_data *opt_factor;
	const struct clk_div_table *table;
	spinlock_t *lock; /* exclusive register access */
	unsigned long flags;
	unsigned long clk_flags;
};

struct clk_fixed_factor_data {
	u8 shift;
	u8 width;
	unsigned int set_val;
	spinlock_t *lock; /* exclusive register access */
	unsigned long flags;
};

struct factor_frac_tbl {
	unsigned long		parent_rate;
	unsigned long long	rate;
	unsigned int		mult;
	unsigned int		div;
	unsigned int		frac;
};

struct clk_factor_frac_data {
	const struct factor_frac_tbl *ff_tbl;
	const unsigned int ff_tblsz;
	spinlock_t *lock; /* exclusive register access */
	unsigned int def_idx;
	unsigned int frac_div;
	unsigned long flags;
	unsigned long clk_flags;
};

struct clk_factor_frac {
	struct clk_hw hw;
	struct regmap *map;
	u32 reg;
	const struct factor_frac_tbl *tbl;
	unsigned int tblsz;
	unsigned int idx;
	unsigned int frac_div;
	unsigned int exdiv;
	spinlock_t *lock; /* exclusive register access */
};

/**
 * struct clk_fixed_factor_frac - fixed multiplier/divider/fraction clock
 *
 * @hw:		handle between common and hardware-specific interfaces
 * @mult:	multiplier(N)
 * @div:	divider(M)
 * @frac:	fraction(K)
 * @frac_div:	fraction divider(D)
 *
 * Clock with a fixed multiplier, divider and fraction.
 * The output frequency formula is clk = parent clk * (N+K/D)/M.
 * Implements .recalc_rate, .set_rate and .round_rate
 */

struct clk_fixed_factor_frac {
	struct clk_hw	hw;
	unsigned int	mult;
	unsigned int	div;
	unsigned int	frac;
	unsigned int	frac_div;
};

/**
 * struct clk_fixed_hw_input
 *
 * @hw:		handle between common and hardware-specific interfaces
 * @clk_rate:	clock rate, can be updated by user
 */

struct clk_fixed_hw_input {
	struct clk_hw	hw;
	unsigned long	clk_rate;
};

#define INTEL_FIXED_FACTOR_PLLCLK	"intel,fixed-factor-pllclk"
#define INTEL_FIXED_FACTOR_FRAC_PLLCLK	"intel,fixed-factor-frac-pllclk"
#define INTEL_FIXED_HW_INPUT_CLK	"intel,fixed-hw-input-clk"

#define CLK_INIT_DEF_CFG_REQ		BIT(0)

enum {
	GATE_CLK = 0,
	V_GATE_CLK = BIT(0),
	GATE_CLK_TYPE = 0x1,
};

void gate_clk_setup(struct device_node *np, const struct gate_clk_data *data);
void mux_clk_setup(struct device_node *np, const struct mux_clk_data *data);
void fixed_rate_clk_setup(struct device_node *np,
			  const struct fixed_rate_clk_data *data);
void div_clk_setup(struct device_node *np,
		   const struct div_clk_data *data);
void gate_dummy_clk_setup(struct device_node *np,
			  const struct gate_dummy_clk_data *data);
void cluster_div_clk_setup(struct device_node *np,
			   const struct div_clk_data *data, u32 num);
void clk_div_setup(struct device_node *node,
		   const struct clk_div_data *data);
void clk_ljpll_div_setup(struct device_node *node,
			 const struct clk_ljpll_div_data *data);
void clk_fixed_factor_setup(struct device_node *node,
			    const struct clk_fixed_factor_data *data);

void pll_ff_clk_setup(struct device_node *node,
		      const struct clk_factor_frac_data *data);
#endif /* __INTEL_CLK_API_H */
