/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright(c) 2018 Intel Corporation.
 *  Zhu YiXin <Yixin.zhu@intel.com>
 */

#ifndef __INTEL_CLK_H
#define __INTEL_CLK_H

#define PNAME(x) static const char *const x[] __initconst

struct intel_clk_mux {
	struct clk_hw	hw;
	struct device	*dev;
	struct regmap	*map;
	unsigned int	reg;
	u8		shift;
	u8		width;
	unsigned long	flags;
};

struct intel_clk_divider {
	struct clk_hw	hw;
	struct device	*dev;
	struct regmap	*map;
	unsigned int	reg;
	u8		shift;
	u8		width;
	u8		shift_gate;
	u8		width_gate;
	unsigned long	flags;
	const struct clk_div_table	*table;
};

struct intel_clk_early {
	struct clk_hw	hw;
	struct regmap	*map;
	unsigned int	reg;
	u8	shift;
	u8	width;
	u8	mux;
	unsigned int	mreg0;
	u8	mshift0;
	u8	mwidth0;
	unsigned int	mreg1;
	u8	mshift1;
	u8	mwidth1;
	const struct clk_div_table *table;
	enum pll_type	type;
};

struct intel_clk_early_data {
	unsigned int	reg;
	u8	shift;
	u8	width;
	const char	*name;
	const char	*const *parent_names;
	u8	mux;
	unsigned int	mreg0;
	u8	mshift0;
	u8	mwidth0;
	unsigned int	mreg1;
	u8	mshift1;
	u8	mwidth1;
	enum pll_platform	platform;
	enum pll_type	type;
	const struct clk_div_table	*table;
};

struct intel_clk_ddiv {
	struct clk_hw	hw;
	struct device	*dev;
	struct regmap	*map;
	unsigned int	reg;
	u8		shift0;
	u8		width0;
	u8		shift1;
	u8		width1;
	u8		shift2;
	u8		width2;
	u8		shift_gate;
	u8		width_gate;
	unsigned int	mult;
	unsigned int	div;
	unsigned long	flags;
};

struct intel_clk_gate {
	struct clk_hw	hw;
	struct device	*dev;
	struct regmap	*map;
	unsigned int	reg;
	u8		shift;
	unsigned long	flags;
};

enum intel_clk_type {
	intel_clk_fixed,
	intel_clk_mux,
	intel_clk_divider,
	intel_clk_fixed_factor,
	intel_clk_gate,
};

/**
 * struct intel_clk_provider
 * @map: regmap type base address for register.
 * @np: device node
 * @dev: device
 * @clk_data: array of hw clocks and clk number.
 */
struct intel_clk_provider {
	struct regmap			*map;
	struct device_node		*np;
	struct device			*dev;
	struct clk_hw_onecell_data	clk_data;
};

/**
 * struct intel_pll_clk_data
 * @id: plaform specific id of the clock.
 * @name: name of this pll clock.
 * @parent_names: name of the parent clock.
 * @num_parents: number of parents.
 * @flags: optional flags for basic clock.
 * @platform: platform defines pll register format.
 * @type: platform type of pll.
 * @reg: offset of the register.
 * @rate_table: table of pll clock rate.
 */
struct intel_pll_clk_data {
	unsigned int		id;
	const char		*name;
	const char		*const *parent_names;
	u8			num_parents;
	unsigned long		flags;
	enum pll_platform	platform;
	enum pll_type		type;
	int			reg;
	const struct intel_pll_rate_table *rate_table;
};

#define INTEL_PLL(_id, _pfm, _name, _pnames, _flags,	\
		  _reg, _rtable, _type)			\
	{						\
		.id		= _id,			\
		.platform	= _pfm,			\
		.name		= _name,		\
		.parent_names	= _pnames,		\
		.num_parents	= ARRAY_SIZE(_pnames),	\
		.flags		= _flags,		\
		.reg		= _reg,			\
		.rate_table	= _rtable,		\
		.type		= _type,		\
	}

struct intel_clk_ddiv_data {
	unsigned int			id;
	const char			*name;
	const char			*const *parent_name;
	u8				flags;
	unsigned long			div_flags;
	unsigned int			reg;
	u8				shift0;
	u8				width0;
	u8				shift1;
	u8				width1;
	u8				shift_gate;
	u8				width_gate;
	u8				ex_shift;
	u8				ex_width;
};

#define INTEL_DDIV(_id, _name, _pname, _flags, _reg,		\
		   _shft0, _wdth0, _shft1, _wdth1,		\
		   _shft_gate, _wdth_gate, _xshft, _df)		\
	{							\
		.id		= _id,				\
		.name		= _name,			\
		.parent_name	= (const char *[]) { _pname },	\
		.flags		= _flags,			\
		.reg		= _reg,				\
		.shift0		= _shft0,			\
		.width0		= _wdth0,			\
		.shift1		= _shft1,			\
		.width1		= _wdth1,			\
		.shift_gate	= _shft_gate,			\
		.width_gate	= _wdth_gate,			\
		.ex_shift	= _xshft,			\
		.ex_width	= 1,				\
		.div_flags	= _df,				\
	}

struct intel_clk_branch {
	unsigned int			id;
	enum intel_clk_type		type;
	const char			*name;
	const char			*const *parent_names;
	u8				num_parents;
	unsigned long			flags;
	unsigned int			mux_off;
	u8				mux_shift;
	u8				mux_width;
	unsigned long			mux_flags;
	unsigned int			mux_val;
	unsigned int			div_off;
	u8				div_shift;
	u8				div_width;
	u8				div_shift_gate;
	u8				div_width_gate;
	unsigned long			div_flags;
	unsigned int			div_val;
	const struct clk_div_table	*div_table;
	unsigned int			gate_off;
	u8				gate_shift;
	unsigned long			gate_flags;
	unsigned int			gate_val;
	unsigned int			mult;
	unsigned int			div;
};

/* clock flags definition */
#define CLOCK_FLAG_VAL_INIT	BIT(16)
#define GATE_CLK_HW		BIT(17)
#define GATE_CLK_SW		BIT(18)

#define INTEL_MUX(_id, _name, _pname, _f, _reg,			\
		  _shift, _width, _cf, _v)			\
	{							\
		.id		= _id,				\
		.type		= intel_clk_mux,		\
		.name		= _name,			\
		.parent_names	= _pname,			\
		.num_parents	= ARRAY_SIZE(_pname),		\
		.flags		= _f,				\
		.mux_off	= _reg,				\
		.mux_shift	= _shift,			\
		.mux_width	= _width,			\
		.mux_flags	= _cf,				\
		.mux_val	= _v,				\
	}

#define INTEL_DIV(_id, _name, _pname, _f, _reg,			\
	_shift, _width, _shift_gate, _width_gate, _cf, _v, _dtable)	\
	{							\
		.id		= _id,				\
		.type		= intel_clk_divider,		\
		.name		= _name,			\
		.parent_names	= (const char *[]) { _pname },	\
		.num_parents	= 1,				\
		.flags		= _f,				\
		.div_off	= _reg,				\
		.div_shift	= _shift,			\
		.div_width	= _width,			\
		.div_shift_gate = _shift_gate,			\
		.div_width_gate = _width_gate,			\
		.div_flags	= _cf,				\
		.div_val	= _v,				\
		.div_table	= _dtable,			\
	}

#define INTEL_GATE(_id, _name, _pname, _f, _reg,		\
		   _shift, _cf, _v)				\
	{							\
		.id		= _id,				\
		.type		= intel_clk_gate,		\
		.name		= _name,			\
		.parent_names	= (const char *[]) { _pname },	\
		.num_parents	= !_pname ? 0 : 1,		\
		.flags		= _f,				\
		.gate_off	= _reg,				\
		.gate_shift	= _shift,			\
		.gate_flags	= _cf,				\
		.gate_val	= _v,				\
	}

#define INTEL_FIXED(_id, _name, _pname, _f, _reg,		\
		    _shift, _width, _cf, _freq, _v)		\
	{							\
		.id		= _id,				\
		.type		= intel_clk_fixed,		\
		.name		= _name,			\
		.parent_names	= (const char *[]) { _pname },	\
		.num_parents	= !_pname ? 0 : 1,		\
		.flags		= _f,				\
		.div_off	= _reg,				\
		.div_shift	= _shift,			\
		.div_width	= _width,			\
		.div_flags	= _cf,				\
		.div_val	= _v,				\
		.mux_flags	= _freq,			\
	}

#define INTEL_FIXED_FACTOR(_id, _name, _pname, _f, _reg,	\
			   _shift, _width, _cf, _v, _m, _d)	\
	{							\
		.id		= _id,				\
		.type		= intel_clk_fixed_factor,	\
		.name		= _name,			\
		.parent_names	= (const char *[]) { _pname },	\
		.num_parents	= 1,				\
		.flags		= _f,				\
		.div_off	= _reg,				\
		.div_shift	= _shift,			\
		.div_width	= _width,			\
		.div_flags	= _cf,				\
		.div_val	= _v,				\
		.mult		= _m,				\
		.div		= _d,				\
	}

void intel_set_clk_val(struct regmap *map, u32 reg, u8 shift,
		       u8 width, u32 set_val);
u32 intel_get_clk_val(struct regmap *map, u32 reg, u8 shift, u8 width);
void intel_clk_add_lookup(struct intel_clk_provider *ctx,
			  struct clk_hw *hw, unsigned int id);
void __init intel_clk_of_add_provider(struct device_node *np,
				      struct intel_clk_provider *ctx);
struct intel_clk_provider *
intel_clk_init(struct regmap *map, unsigned int nr_clks);
void intel_clk_register_branches(struct intel_clk_provider *ctx,
				 const struct intel_clk_branch *list,
				 unsigned int nr_clk);
void intel_clk_register_plls(struct intel_clk_provider *ctx,
			     const struct intel_pll_clk_data *list,
			     unsigned int nr_clk);
void intel_clk_plls_parse_vco_config(struct intel_clk_provider *ctx,
				     const struct intel_pll_clk_data *list,
				     u32 nr_clk);
int intel_clk_register_ddiv(struct intel_clk_provider *ctx,
			    const struct intel_clk_ddiv_data *list,
			    unsigned int nr_clk);
void intel_clk_ssc_probe(struct intel_clk_provider *ctx);
int intel_clk_register_early(struct intel_clk_provider *ctx,
			     const struct intel_clk_early_data *list,
			     unsigned int nr_clk);

#endif /* __INTEL_CLK_H */
