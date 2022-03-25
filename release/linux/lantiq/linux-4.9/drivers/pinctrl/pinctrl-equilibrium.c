/**
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2017 Zhu Yixin <yixin.zhu@intel.com>
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>

#include <dt-bindings/pinctrl/intel,equilibrium.h>
#include "pinctrl-equilibrium.h"

#define PIN_NAME_FMT	"io-%d"
#define PIN_NAME_LEN	10

static const struct pin_config pin_cfg_type[] = {
	{"intel,pullup",		PINCONF_TYPE_PULL_UP},
	{"intel,pulldown",		PINCONF_TYPE_PULL_DOWN},
	{"intel,drive-current",		PINCONF_TYPE_DRIVE_CURRENT},
	{"intel,slew-rate",		PINCONF_TYPE_SLEW_RATE},
	{"intel,open-drain",		PINCONF_TYPE_OPEN_DRAIN},
	{"intel,output",		PINCONF_TYPE_OUTPUT},
};

static inline void eqbr_set_val(void __iomem *addr, u32 offset,
				u32 mask, u32 set, spinlock_t *lock)
{
	u32 val;
	unsigned long flags;

	if (lock)
		spin_lock_irqsave(lock, flags);
	val = readl(addr);
	val &= ~(mask << offset);
	val |= (set & mask) << offset;
	writel(val, addr);

	if (lock)
		spin_unlock_irqrestore(lock, flags);
}

static int eqbr_irq_map(struct irq_domain *d,
			unsigned int virq, irq_hw_number_t hw)
{
	struct intel_gpio_desc *desc = d->host_data;

	irq_set_chip_data(virq, desc);
	irq_set_chip_and_handler(virq, desc->ic, handle_level_irq);
	return 0;
}

static const struct irq_domain_ops gc_irqdomain_ops = {
	.map = eqbr_irq_map,
	.xlate = irq_domain_xlate_twocell,
};

static void intel_eqbr_gpio_set_direction(struct gpio_chip *gc,
					  unsigned int offset, unsigned int dir)
{
	struct intel_gpio_desc *desc = gpiochip_get_data(gc);

	if (dir == GPIO_DIR_IN) {
		/* Set Dir Clear Register */
		writel(BIT(offset), desc->membase + GPIO_DIRCLR);
	} else {
		/* Set Dir Set Register */
		writel(BIT(offset), desc->membase + GPIO_DIRSET);
	}
}

static int intel_eqbr_gpio_get_direction(struct gpio_chip *gc,
					 unsigned int offset)
{
	struct intel_gpio_desc *desc = gpiochip_get_data(gc);
	u32 val;

	val = readl(desc->membase + GPIO_DIR) & BIT(offset);
	return !!val;
}

static int intel_eqbr_gpio_get_value(struct gpio_chip *gc,
				     unsigned int offset, unsigned int dir)
{
	struct intel_gpio_desc *desc = gpiochip_get_data(gc);
	int val;

	if (dir == GPIO_DIR_IN) {
		/* read GPIO_IN register */
		val = readl(desc->membase + GPIO_IN);
	} else {
		/* read GPIO_OUT register */
		val = readl(desc->membase + GPIO_OUT);
	}

	val &= BIT(offset);
	return !!val;
}

static void intel_eqbr_gpio_set_value(struct gpio_chip *gc,
				      unsigned int offset, int value)
{
	struct intel_gpio_desc *desc = gpiochip_get_data(gc);

	if (value) {
		/* Set outset register */
		writel(BIT(offset), desc->membase + GPIO_OUTSET);
	} else {
		/* Set outclr register */
		writel(BIT(offset), desc->membase + GPIO_OUTCLR);
	}
}

/* get direction 0 is out, 1 is in */
static int intel_eqbr_gpio_get_dir(struct gpio_chip *gc, unsigned int offset)
{
	struct intel_gpio_desc *desc = gpiochip_get_data(gc);

	return !(readl(desc->membase + GPIO_DIR) & BIT(offset));
}

static int intel_eqbr_gpio_dir_input(struct gpio_chip *gc, unsigned int offset)
{
	intel_eqbr_gpio_set_direction(gc, offset, GPIO_DIR_IN);
	return 0;
}

static int intel_eqbr_gpio_dir_output(struct gpio_chip *gc, unsigned int offset,
				      int value)
{
	intel_eqbr_gpio_set_value(gc, offset, value);
	intel_eqbr_gpio_set_direction(gc, offset, GPIO_DIR_OUT);

	return 0;
}

static void intel_eqbr_gpio_set(struct gpio_chip *gc,
				unsigned int offset, int value)
{
	intel_eqbr_gpio_set_value(gc, offset, value);
}

static int intel_eqbr_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	int dir;

	dir = intel_eqbr_gpio_get_direction(gc, offset);
	return intel_eqbr_gpio_get_value(gc, offset, dir);
}

static int intel_eqbr_gpio_to_irq(struct gpio_chip *gc, unsigned int offset)
{
	struct intel_gpio_desc *desc = gpiochip_get_data(gc);
	unsigned int virq;

	if (!desc->irq_domain)
		return -ENODEV;

	virq = irq_find_mapping(desc->irq_domain, offset);
	if (virq)
		return virq;
	else
		return irq_create_mapping(desc->irq_domain, offset);
}

static int gpiochip_setup(struct device *dev, struct intel_gpio_desc *desc)
{
	struct gpio_chip *gc;
	int ret;

	gc			= &desc->chip;
	gc->owner		= THIS_MODULE;
	gc->request		= gpiochip_generic_request;
	gc->free		= gpiochip_generic_free;
	gc->get_direction	= intel_eqbr_gpio_get_dir;
	gc->direction_input	= intel_eqbr_gpio_dir_input;
	gc->direction_output	= intel_eqbr_gpio_dir_output;
	gc->get			= intel_eqbr_gpio_get;
	gc->set			= intel_eqbr_gpio_set;
	gc->base		= -1; /* desc->bank->pin_base; */
	gc->ngpio		= desc->bank->nr_pins;
	gc->label		= desc->name;
	gc->to_irq		= intel_eqbr_gpio_to_irq;
	gc->of_node		= desc->node;
	gc->parent		= dev;

	ret = devm_gpiochip_add_data(dev, gc, desc);
	if (ret) {
		dev_err(dev, "failed to register gpiochip: %s, err: %d\n",
			gc->label, ret);
		return ret;
	}

	return 0;
}

static void eqbr_gpio_disable_irq(struct irq_data *d)
{
	unsigned int offset = irqd_to_hwirq(d);
	struct intel_gpio_desc *desc = irq_data_get_irq_chip_data(d);

	writel(BIT(offset), desc->membase + GPIO_IRNENCLR);
}

static void eqbr_gpio_enable_irq(struct irq_data *d)
{
	unsigned int offset = irqd_to_hwirq(d);
	struct intel_gpio_desc *desc = irq_data_get_irq_chip_data(d);

	writel(BIT(offset), desc->membase + GPIO_IRNRNSET);
}

static void eqbr_gpio_ack_irq(struct irq_data *d)
{
	unsigned int offset = irqd_to_hwirq(d);
	struct intel_gpio_desc *desc = irq_data_get_irq_chip_data(d);

	writel(BIT(offset), desc->membase + GPIO_IRNCR);
}

static void eqbr_gpio_mask_ack_irq(struct irq_data *d)
{
	eqbr_gpio_disable_irq(d);
	eqbr_gpio_ack_irq(d);
}

static inline void eqbr_cfg_bit(void __iomem *addr,
				unsigned int offset, unsigned int set)
{
	u32 val;

	if (!set) { /* Bit Clear */
		val = readl(addr) & (~BIT(offset));
		writel(val, addr);
	} else {
		val = readl(addr) | BIT(offset);
		writel(val, addr);
	}
}

static int eqbr_irq_type_cfg(struct gpio_irq_type *type,
			     struct intel_gpio_desc *desc,
			     unsigned int offset)
{
	unsigned long flags;

	spin_lock_irqsave(&desc->lock, flags);
	eqbr_cfg_bit(desc->membase + GPIO_IRNCFG, offset, type->trig_type);
	eqbr_cfg_bit(desc->membase + GPIO_EXINTCR1, offset, type->edge_type);
	eqbr_cfg_bit(desc->membase + GPIO_EXINTCR0, offset, type->logic_type);
	spin_unlock_irqrestore(&desc->lock, flags);

	return 0;
}

static int eqbr_gpio_set_irq_type(struct irq_data *d, unsigned int type)
{
	unsigned int offset = irqd_to_hwirq(d);
	struct intel_gpio_desc *desc = irq_data_get_irq_chip_data(d);
	struct gpio_irq_type it;

	memset(&it, 0, sizeof(it));

	if ((type & IRQ_TYPE_SENSE_MASK) == IRQ_TYPE_NONE)
		return 0;

	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		it.trig_type = GPIO_EDGE_TRIG;
		it.edge_type = GPIO_SINGLE_EDGE;
		it.logic_type = GPIO_POSITIVE_TRIG;
		break;

	case IRQ_TYPE_EDGE_FALLING:
		it.trig_type = GPIO_EDGE_TRIG;
		it.edge_type = GPIO_SINGLE_EDGE;
		it.logic_type = GPIO_NEGATIVE_TRIG;
		break;

	case IRQ_TYPE_EDGE_BOTH:
		it.trig_type = GPIO_EDGE_TRIG;
		it.edge_type = GPIO_BOTH_EDGE;
		it.logic_type = GPIO_POSITIVE_TRIG;
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		it.trig_type = GPIO_LEVEL_TRIG;
		it.edge_type = GPIO_SINGLE_EDGE;
		it.logic_type = GPIO_POSITIVE_TRIG;
		break;

	case IRQ_TYPE_LEVEL_LOW:
		it.trig_type = GPIO_LEVEL_TRIG;
		it.edge_type = GPIO_SINGLE_EDGE;
		it.logic_type = GPIO_NEGATIVE_TRIG;
		break;

	default:
		return -EINVAL;
	}

	eqbr_irq_type_cfg(&it, desc, offset);
	if (it.trig_type == GPIO_EDGE_TRIG)
		/* edge trigger */
		irq_set_handler_locked(d, handle_edge_irq);
	else
		/* Level trigger */
		irq_set_handler_locked(d, handle_level_irq);

	return 0;
}

static int eqbr_gpio_irq_req_res(struct irq_data *d)
{
	struct intel_gpio_desc *desc = irq_data_get_irq_chip_data(d);
	unsigned int offset;
	int ret;

	offset = irqd_to_hwirq(d);

	/* gpio must be set as input */
	intel_eqbr_gpio_dir_input(&desc->chip, offset);
	ret = gpiochip_lock_as_irq(&desc->chip, offset);
	if (ret) {
		pr_err("%s: Failed to lock gpio %u as irq!\n",
		       desc->name, offset);
		return ret;
	}
	eqbr_gpio_enable_irq(d);

	return 0;
}

static void eqbr_gpio_irq_rel_res(struct irq_data *d)
{
	struct intel_gpio_desc *desc = irq_data_get_irq_chip_data(d);
	unsigned int offset = irqd_to_hwirq(d);

	eqbr_gpio_disable_irq(d);
	gpiochip_unlock_as_irq(&desc->chip, offset);
}

static struct irq_chip eqbr_irq_chip = {
	.name = "gpio_irq",
	.irq_mask = eqbr_gpio_disable_irq,
	.irq_unmask = eqbr_gpio_enable_irq,
	.irq_ack = eqbr_gpio_ack_irq,
	.irq_mask_ack = eqbr_gpio_mask_ack_irq,
	.irq_set_type = eqbr_gpio_set_irq_type,
	.irq_request_resources = eqbr_gpio_irq_req_res,
	.irq_release_resources = eqbr_gpio_irq_rel_res,
};

static void eqbr_irq_handler(struct irq_desc *desc)
{
	struct intel_gpio_desc *gc;
	struct irq_chip *ic;
	u32 pins, offset;
	unsigned int virq;

	gc = irq_desc_get_handler_data(desc);
	ic = irq_desc_get_chip(desc);

	chained_irq_enter(ic, desc);
	pins = readl(gc->membase + GPIO_IRNCR);

	for_each_set_bit(offset, (unsigned long *)&pins, gc->bank->nr_pins) {
		virq = irq_linear_revmap(gc->irq_domain, offset);
		if (!virq)
			pr_err("%s:gc[%s]:pin:%d irq not registered!\n",
			       __func__, gc->name, offset);
		else
			generic_handle_irq(virq);
	}
	chained_irq_exit(ic, desc);
}

static int irqchip_setup(struct device *dev, struct intel_gpio_desc *desc)
{
	struct device_node *np = desc->node;

	if (!of_find_property(np, "interrupt-controller", NULL)) {
		dev_info(dev, "gc %s: doesn't act as interrupt controller!\n",
			 desc->name);
		return 0;
	}

	desc->irq_domain = irq_domain_add_linear(desc->node,
						 desc->bank->nr_pins,
						 &gc_irqdomain_ops, desc);
	if (!desc->irq_domain) {
		dev_err(dev, "%s: failed to create gpio irq domain!\n",
			desc->name);
		return -ENODEV;
	}
	irq_set_chained_handler_and_data(desc->virq, eqbr_irq_handler, desc);
	desc->ic = &eqbr_irq_chip;

	return 0;
}

static int gpiolib_reg(struct intel_pinctrl_drv_data *drvdata)
{
	struct device_node *np;
	struct intel_gpio_desc *desc;
	struct device *dev;
	int i, ret;
	char name[32];
	struct resource res;

	dev = drvdata->dev;
	for (i = 0; i < drvdata->nr_gpio_descs; i++) {
		desc = drvdata->gpio_desc + i;
		np = desc->node;
		sprintf(name, "gpiochip%d", i);
		desc->name = devm_kmemdup(dev, name,
					  strlen(name) + 1, GFP_KERNEL);
		if (!desc->name)
			return -ENOMEM;
		if (of_address_to_resource(np, 0, &res)) {
			dev_err(dev, "Failed to get GPIO register addrss\n");
			return -ENXIO;
		}
		desc->membase = devm_ioremap_resource(dev, &res);
		if (IS_ERR(desc->membase)) {
			dev_err(dev, "ioremap fail\n");
			return PTR_ERR(desc->membase);
		}
		dev_info(dev, "gpiochip membase: 0x%x\n", (u32)desc->membase);
		desc->virq = irq_of_parse_and_map(np, 0);
		if (!desc->virq) {
			dev_err(dev, "%s: failed to parse and map irq\n",
				name);
			return -ENXIO;
		}
		spin_lock_init(&desc->lock);
		writel(0x0, desc->membase + GPIO_DIR);

		ret = gpiochip_setup(dev, desc);
		if (ret)
			return ret;
		ret = irqchip_setup(dev, desc);
		if (ret)
			return ret;
	}

	return 0;
}

static int eqbr_get_groups_count(struct pinctrl_dev *pctldev)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	return pctl->nr_grps;
}

static const char *eqbr_get_group_name(struct pinctrl_dev *pctldev,
				       unsigned int selector)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	return pctl->pin_grps[selector].name;
}

static int eqbr_get_group_pins(struct pinctrl_dev *pctldev,
			       unsigned int selector,
			       const unsigned int **pins,
			       unsigned int *num_pins)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	*pins = pctl->pin_grps[selector].pins;
	*num_pins = pctl->pin_grps[selector].nr_pins;

	return 0;
}

static int parse_mux_info(struct device_node *np)
{
	int ret;
	const char *str;

	ret = of_property_read_string(np, "intel,function", &str);
	if (ret)
		return -ENODEV;
	ret = of_property_read_string(np, "intel,groups", &str);
	if (ret)
		return -ENODEV;

	return ret;
}

static int add_config(struct intel_pinctrl_drv_data *drvdata,
		      unsigned long **confs, unsigned int *nr_conf,
		      unsigned long pinconf)
{
	unsigned long *configs;
	struct device *dev = drvdata->dev;
	unsigned int num_conf = *nr_conf + 1;

	if (!(*nr_conf)) {
		configs = devm_kcalloc(dev, 1, sizeof(pinconf), GFP_KERNEL);
		if (!configs)
			return -ENOMEM;
	} else {
		configs = devm_kmemdup(dev, *confs,
				       num_conf * sizeof(pinconf), GFP_KERNEL);
		if (!configs)
			return -ENOMEM;
		devm_kfree(dev, *confs);
	}

	configs[num_conf - 1] = pinconf;
	*confs = configs;
	*nr_conf = num_conf;

	return 0;
}

static void eqbr_add_map_mux(struct device_node *np, struct pinctrl_map **map,
			     int *index)
{
	int idx = *index;
	const char *function, *group;

	of_property_read_string(np, "intel,function", &function);
	of_property_read_string(np, "intel,groups", &group);

	(*map)[idx].type = PIN_MAP_TYPE_MUX_GROUP;
	(*map)[idx].data.mux.group = group;
	(*map)[idx].data.mux.function = function;
	*index = idx + 1;
}

static void eqbr_add_map_configs(struct device_node *np,
				 struct pinctrl_map **map, int *index,
				 unsigned long *configs, unsigned int nr_config)
{
	int idx = *index;
	const char *group;

	of_property_read_string(np, "intel,groups", &group);
	(*map)[idx].type = PIN_MAP_TYPE_CONFIGS_GROUP;
	(*map)[idx].data.configs.group_or_pin = group;
	(*map)[idx].data.configs.configs = configs;
	(*map)[idx].data.configs.num_configs = nr_config;
	*index = idx + 1;
}

static int eqbr_dt_node_to_map(struct pinctrl_dev *pctldev,
			       struct device_node *np,
			       struct pinctrl_map **map, unsigned int *num_maps)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int map_cnt, nr_config;
	unsigned long pin_conf, *configs = NULL;
	int i, ret;
	unsigned int val;
	bool func = false;

	*map = NULL;
	*num_maps = map_cnt = nr_config = 0;

	ret = parse_mux_info(np);
	if (!ret) {
		map_cnt++;
		func = true;
	}

	for (i = 0; i < ARRAY_SIZE(pin_cfg_type); i++) {
		ret = of_property_read_u32(np, pin_cfg_type[i].property, &val);
		if (!ret) {
			pin_conf = PINCONF_PACK(pin_cfg_type[i].type, val);
			ret = add_config(pctl, &configs, &nr_config, pin_conf);
			if (ret)
				return ret;
		}
	}

	/**
	 * Create pinctrl_map for each groups, per group per entry.
	 * Create pinctrl_map for pin config, per group per entry.
	 */
	if (nr_config)
		map_cnt++;

	*map = devm_kcalloc(pctl->dev, map_cnt, sizeof(**map), GFP_KERNEL);
	if (!*map)
		return -ENOMEM;

	i = 0;
	if (func)
		eqbr_add_map_mux(np, map, &i);
	if (nr_config)
		eqbr_add_map_configs(np, map, &i, configs, nr_config);

	*num_maps = map_cnt;

	return 0;
}

static inline struct intel_pin_bank
*find_pinbank_via_pin(struct intel_pinctrl_drv_data *pctl, unsigned int pin)
{
	int i;
	struct intel_pin_bank *bank;

	for (i = 0; i < pctl->nr_banks; i++) {
		bank = &pctl->pin_banks[i];
		if ((pin >= bank->pin_base) &&
		    ((pin - bank->pin_base) < bank->nr_pins))
			return bank;
	}

	return NULL;
}

static void eqbr_pin_dbg_show(struct pinctrl_dev *pctldev, struct seq_file *s,
			      unsigned int offset)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);
	struct intel_pin_bank *bank;
	const struct intel_pin_group *group;
	unsigned int pin = offset;
	int i, j;

	bank = find_pinbank_via_pin(pctl, pin);
	if (!bank) {
		dev_err(pctl->dev, "Couldn't find pin bank for pin %u\n", pin);
		return;
	}
	offset = pin - bank->pin_base;

	seq_printf(s, "pin mux: %u\n", readl(bank->membase + (offset << 2)));

	for (i = 0; i < pctl->nr_grps; i++) {
		group = &pctl->pin_grps[i];
		for (j = 0; j < group->nr_pins; j++) {
			if (pin == group->pins[j]) {
				seq_printf(s, "group name: %s, mux: %u\n",
					   group->name, group->pmx[j]);
				break;
			}
		}
	}
}

static void eqbr_dt_free_map(struct pinctrl_dev *pctldev,
			     struct pinctrl_map *map, unsigned int num_maps)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);
	int i;

	for (i = 0; i < num_maps; i++)
		if (map[i].type == PIN_MAP_TYPE_CONFIGS_GROUP)
			devm_kfree(pctl->dev, map[i].data.configs.configs);
	devm_kfree(pctl->dev, map);
}

static const struct pinctrl_ops eqbr_pctl_ops = {
	.get_groups_count	= eqbr_get_groups_count,
	.get_group_name		= eqbr_get_group_name,
	.get_group_pins		= eqbr_get_group_pins,
	.pin_dbg_show		= eqbr_pin_dbg_show,
	.dt_node_to_map		= eqbr_dt_node_to_map,
	.dt_free_map		= eqbr_dt_free_map,
};

static int eqbr_pinmux_get_func_count(struct pinctrl_dev *pctldev)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	return pctl->nr_funcs;
}

static const char *eqbr_pinmux_get_fname(struct pinctrl_dev *pctldev,
					 unsigned int selector)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	return pctl->pmx_funcs[selector].name;
}

static int eqbr_pinmux_get_groups(struct pinctrl_dev *pctldev,
				  unsigned int selector,
				  const char * const **groups,
				  unsigned int *num_groups)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	*groups = pctl->pmx_funcs[selector].groups;
	*num_groups = pctl->pmx_funcs[selector].nr_groups;
	return 0;
}

static int eqbr_set_pin_mux(struct intel_pinctrl_drv_data *pctl,
			    unsigned int pmx, unsigned int pin)
{
	void __iomem *mem;
	struct intel_pin_bank *bank;
	unsigned int offset;

	bank = find_pinbank_via_pin(pctl, pin);
	if (!bank) {
		dev_err(pctl->dev, "Couldn't find pin bank for pin %u\n", pin);
		return -ENODEV;
	}
	mem = bank->membase;
	offset = pin - bank->pin_base;

	if (!(bank->aval_pinmap & BIT(offset))) {
		dev_err(pctl->dev,
			"PIN: %u is not valid, pinbase: %u, bitmap: %u\n",
			pin, bank->pin_base, bank->aval_pinmap);
		return -ENODEV;
	}

	writel(pmx, mem + (offset << 2));
	return 0;
}

static int eqbr_pinmux_set_mux(struct pinctrl_dev *pctldev,
			       unsigned int selector, unsigned int group)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);
	const struct intel_pin_group *pin_group = NULL;
	int i;

	pin_group = &pctl->pin_grps[group];

	dev_dbg(pctl->dev, "set mux group name: %s, pin num: %u\n",
		pin_group->name, pin_group->nr_pins);

	for (i = 0; i < pin_group->nr_pins; i++)
		eqbr_set_pin_mux(pctl, pin_group->pmx[i], pin_group->pins[i]);

	return 0;
}

static int eqbr_pinmux_gpio_request(struct pinctrl_dev *pctldev,
				    struct pinctrl_gpio_range *range,
				    unsigned int pin)
{
	struct intel_pinctrl_drv_data *pctl = pinctrl_dev_get_drvdata(pctldev);

	return eqbr_set_pin_mux(pctl, EQBR_MUX_GPIO, pin);
}

static const struct pinmux_ops eqbr_pinmux_ops = {
	.get_functions_count	= eqbr_pinmux_get_func_count,
	.get_function_name	= eqbr_pinmux_get_fname,
	.get_function_groups	= eqbr_pinmux_get_groups,
	.set_mux		= eqbr_pinmux_set_mux,
	.gpio_request_enable	= eqbr_pinmux_gpio_request,
};

static void set_drv_cur(void __iomem *mem, unsigned int offset,
			unsigned int set, spinlock_t *lock)
{
	unsigned int idx = offset; /* 16 pin per register*/
	unsigned int reg;

	/* do_div(idx, DRV_CUR_PINS); */
	idx = idx / DRV_CUR_PINS;
	offset %= DRV_CUR_PINS;
	reg = REG_DRCC(idx);
	eqbr_set_val(mem + REG_DRCC(idx), offset * 2,
		     0x3, set, lock);
}

static int get_drv_cur(void __iomem *mem, unsigned int offset)
{
	unsigned int idx = offset; /* 0-15, 16-31 per register*/
	unsigned int val;

	/* do_div(idx, DRV_CUR_PINS); */
	idx = idx / DRV_CUR_PINS;
	val = readl(mem + REG_DRCC(idx));
	offset %= DRV_CUR_PINS;
	val = PARSE_DRV_CURRENT(val, offset);

	return val;
}

static struct intel_gpio_desc
*get_gpio_desc_via_bank(struct intel_pinctrl_drv_data *pctl,
			struct intel_pin_bank *bank)
{
	int i;

	for (i = 0; i < pctl->nr_gpio_descs; i++) {
		if (pctl->gpio_desc[i].bank == bank)
			return &pctl->gpio_desc[i];
	}

	return NULL;
}

static int eqbr_pinconf_get(struct pinctrl_dev *pctldev, unsigned int pin,
			    unsigned long *config)
{
	struct intel_pinctrl_drv_data *pctl;
	struct intel_pin_bank *bank;
	unsigned int offset;
	void __iomem *mem;
	enum pincfg_type type = PINCONF_UNPACK_TYPE(*config);
	struct intel_gpio_desc *gpio;
	u32 val;

	pctl = pinctrl_dev_get_drvdata(pctldev);
	bank = find_pinbank_via_pin(pctl, pin);
	if (!bank) {
		dev_err(pctl->dev, "Couldn't find pin bank for pin %u\n", pin);
		return -ENODEV;
	}
	mem = bank->membase;
	offset = pin - bank->pin_base;

	if (!(bank->aval_pinmap & BIT(offset))) {
		dev_err(pctl->dev,
			"PIN: %u is not valid, pinbase: %u, bitmap: %u\n",
			pin, bank->pin_base, bank->aval_pinmap);
		return -ENODEV;
	}

	switch (type) {
	case PINCONF_TYPE_PULL_UP:
		val = !!(readl(mem + REG_PUEN) & BIT(offset));
		break;
	case PINCONF_TYPE_PULL_DOWN:
		val = !!(readl(mem + REG_PDEN) & BIT(offset));
		break;
	case PINCONF_TYPE_OPEN_DRAIN:
		val = !!(readl(mem + REG_OD) & BIT(offset));
		break;
	case PINCONF_TYPE_DRIVE_CURRENT:
		val = get_drv_cur(mem, offset);
		break;
	case PINCONF_TYPE_SLEW_RATE:
		val = !!(readl(mem + REG_SRC) & BIT(offset));
		break;
	case PINCONF_TYPE_OUTPUT:
		gpio = get_gpio_desc_via_bank(pctl, bank);
		if (!gpio) {
			dev_err(pctl->dev, "Failed to find gpio via bank pinbase: %u, pin: %u\n",
				bank->pin_base, pin);
			return -ENODEV;
		}
		val = !!(readl(gpio->membase + GPIO_DIR) & BIT(offset));
		break;
	default:
		dev_err(pctl->dev, "PINCONF type error: %u\n", type);
		return -ENODEV;
	}

	*config = PINCONF_PACK(type, val);

	return 0;
}

static int eqbr_pinconf_set(struct pinctrl_dev *pctldev, unsigned int pin,
			    unsigned long *configs, unsigned int num_configs)
{
	struct intel_pinctrl_drv_data *pctl;
	enum pincfg_type type;
	unsigned int val, offset;
	struct intel_pin_bank *bank;
	struct intel_gpio_desc *gpio;
	void __iomem *mem;
	int i;

	pctl = pinctrl_dev_get_drvdata(pctldev);
	for (i = 0; i < num_configs; i++) {
		type = PINCONF_UNPACK_TYPE(configs[i]);
		val = PINCONF_UNPACK_SET(configs[i]);

		bank = find_pinbank_via_pin(pctl, pin);
		if (!bank) {
			dev_err(pctl->dev,
				"Couldn't find pin bank for pin %u\n", pin);
			return -ENODEV;
		}
		mem = bank->membase;
		offset = pin - bank->pin_base;

		switch (type) {
		case PINCONF_TYPE_PULL_UP:
			eqbr_set_val(mem + REG_PUEN, offset,
				     1, val, &pctl->lock);
			break;
		case PINCONF_TYPE_PULL_DOWN:
			eqbr_set_val(mem + REG_PDEN, offset,
				     1, val, &pctl->lock);
			break;
		case PINCONF_TYPE_OPEN_DRAIN:
			eqbr_set_val(mem + REG_OD, offset,
				     1, val, &pctl->lock);
			break;
		case PINCONF_TYPE_DRIVE_CURRENT:
			set_drv_cur(mem, offset, val, &pctl->lock);
			break;
		case PINCONF_TYPE_SLEW_RATE:
			eqbr_set_val(mem + REG_SRC, offset,
				     1, val, &pctl->lock);
			break;
		case PINCONF_TYPE_OUTPUT:
			gpio = get_gpio_desc_via_bank(pctl, bank);
			if (!gpio) {
				dev_err(pctl->dev, "Failed to find gpio via bank pinbase: %u, pin: %u\n",
					bank->pin_base, pin);
				return -ENODEV;
			}
			intel_eqbr_gpio_dir_output(&gpio->chip, offset, 0);
			break;
		default:
			dev_err(pctl->dev, "PINCONF type error: %u\n", type);
			return -ENODEV;
		}
	}

	return 0;
}

static int eqbr_pinconf_group_get(struct pinctrl_dev *pctldev,
				  unsigned int group, unsigned long *config)
{
	struct intel_pinctrl_drv_data *pctl;
	const unsigned int *pins;

	pctl = pinctrl_dev_get_drvdata(pctldev);
	pins = pctl->pin_grps[group].pins;
	eqbr_pinconf_get(pctldev, pins[0], config);
	return 0;
}

static int eqbr_pinconf_group_set(struct pinctrl_dev *pctldev,
				  unsigned int group, unsigned long *configs,
				  unsigned int num_configs)
{
	struct intel_pinctrl_drv_data *pctl;
	const unsigned int *pins;
	unsigned int cnt;
	int i;

	pctl = pinctrl_dev_get_drvdata(pctldev);
	pins = pctl->pin_grps[group].pins;
	cnt = pctl->pin_grps[group].nr_pins;

	for (i = 0; i < cnt; i++)
		eqbr_pinconf_set(pctldev, pins[i], configs, num_configs);

	return 0;
}

static void eqbr_pinconf_dbg_show(struct pinctrl_dev *pctldev,
				  struct seq_file *s, unsigned int offset)
{
	struct intel_pinctrl_drv_data *pctl;
	struct intel_pin_bank *bank;
	struct intel_gpio_desc *gpio;
	void __iomem *mem;
	unsigned int pin, val;

	pin = offset;
	pctl = pinctrl_dev_get_drvdata(pctldev);
	bank = find_pinbank_via_pin(pctl, pin);
	if (!bank) {
		dev_err(pctl->dev, "Couldn't find pin bank for pin %u\n", pin);
		return;
	}
	mem = bank->membase;
	offset = pin - bank->pin_base;

	val = !!(readl(mem + REG_PUEN) & BIT(offset));
	seq_printf(s, "PULL UP: %u\n", val);
	val = !!(readl(mem + REG_PDEN) & BIT(offset));
	seq_printf(s, "PULL DOWN: %u\n", val);
	val = !!(readl(mem + REG_OD) & BIT(offset));
	seq_printf(s, "OPEN DRAIN: %u\n", val);
	val = get_drv_cur(mem, offset);
	seq_printf(s, "DRIVE CURRENT: %u\n", val);
	val = !!(readl(mem + REG_SRC) & BIT(offset));
	seq_printf(s, "SLEW RATE: %u\n", val);
	gpio = get_gpio_desc_via_bank(pctl, bank);
	val = intel_eqbr_gpio_get_dir(&gpio->chip, offset);
	seq_printf(s, "OUTPUT: %u\n", !val);
}

static const struct pinconf_ops eqbr_pinconf_ops = {
	.pin_config_get		= eqbr_pinconf_get,
	.pin_config_set		= eqbr_pinconf_set,
	.pin_config_group_get	= eqbr_pinconf_group_get,
	.pin_config_group_set	= eqbr_pinconf_group_set,
	.pin_config_dbg_show	= eqbr_pinconf_dbg_show,
};

static int create_pin_func(struct device *dev, struct intel_pmx_func **funcs,
			   unsigned int nr_funcs, const char *fn_name,
			   const char *grp_name)
{
	unsigned int cnt = nr_funcs + 1;
	const char **grps;
	struct intel_pmx_func *function;

	if (!nr_funcs) {
		function = devm_kcalloc(dev, cnt,
					sizeof(*function), GFP_KERNEL);
		if (!function)
			return -ENOMEM;
	} else {
		function = devm_kmemdup(dev, *funcs,
					cnt * sizeof(*function), GFP_KERNEL);
		if (!function)
			return -ENOMEM;
		devm_kfree(dev, *funcs);
	}

	function[nr_funcs].name = fn_name;
	grps = devm_kzalloc(dev, sizeof(*grps), GFP_KERNEL);
	if (!grps)
		return -ENOMEM;
	*grps = grp_name;
	function[nr_funcs].groups = grps;
	function[nr_funcs].nr_groups = 1;
	*funcs = function;

	return 0;
}

static int add_group_to_func(struct device *dev, struct intel_pmx_func *funcs,
			     unsigned int nr_funcs, unsigned int idx,
			     const char *grp_name)
{
	unsigned int nr_grps = funcs[idx].nr_groups + 1;
	const char **grps;

	grps = devm_kmemdup(dev, funcs[idx].groups,
			    nr_grps * sizeof(*grps), GFP_KERNEL);
	if (!grps)
		return -ENOMEM;
	devm_kfree(dev, funcs[idx].groups);
	grps[nr_grps - 1] = grp_name;
	funcs[idx].groups = grps;
	funcs[idx].nr_groups = nr_grps;

	return 0;
}

static int is_func_exist(struct intel_pmx_func *funcs, const char *name,
			 unsigned int nr_funcs, unsigned int *idx)
{
	int i;

	if (!funcs || !nr_funcs)
		return 0;

	for (i = 0; i < nr_funcs; i++) {
		if (strcmp(funcs[i].name, name) == 0) {
			*idx = i;
			return 1;
		}
	}

	return 0;
}

static void dump_pinctrl_group_func(struct intel_pinctrl_drv_data *drvdata)
{
	struct device *dev = drvdata->dev;
	const struct intel_pin_group *group;
	const struct intel_pmx_func *func;
	int i, j;

	dev_info(dev, "Total %u groups, %u functions\n",
		 drvdata->nr_grps, drvdata->nr_funcs);

	for (i = 0; i < drvdata->nr_grps; i++) {
		group = &drvdata->pin_grps[i];

		dev_dbg(dev, "group name: %s, pin num: %u\n",
			group->name, group->nr_pins);
		for (j = 0; j < group->nr_pins; j++)
			dev_dbg(dev, "pin[%d]: %u pmx: %u\n",
				j, group->pins[j], group->pmx[j]);
	}

	for (i = 0; i < drvdata->nr_funcs; i++) {
		func = &drvdata->pmx_funcs[i];

		dev_dbg(dev, "function name: %s, group num: %u\n",
			func->name, func->nr_groups);
		for (j = 0; j < func->nr_groups; j++)
			dev_dbg(dev, "group[%d]: %s\n", j, func->groups[j]);
	}
}

static int pinctrl_setup_from_dt(struct device *dev,
				 struct intel_pinctrl_drv_data *drvdata)
{
	struct device_node *node = dev->of_node;
	struct device_node *np;
	struct property *prop;
	unsigned int nr_grps = 0;
	unsigned int nr_funcs = 0;
	struct intel_pin_group *grps = NULL;
	struct intel_pmx_func *funcs = NULL;
	int i, j, nr_pins, nr_pmx, ret;
	unsigned int *pins, pin_id, *pmx, pmx_id, fid;
	const char *fn_name;

	/* Count group number, DON'T support nested child device tree node */
	for_each_child_of_node(node, np) {
		if (of_find_property(np, EQBR_PINCTRL_GROUP, NULL))
			nr_grps++;
	}

	if (!nr_grps) {
		dev_err(dev, "No pin groups found in device tree!\n");
		return -EINVAL;
	}

	grps = devm_kcalloc(dev, nr_grps, sizeof(*grps), GFP_KERNEL);
	if (!grps)
		return -ENOMEM;

	i = 0;
	for_each_child_of_node(node, np) {
		prop = of_find_property(np, EQBR_PINCTRL_GROUP, NULL);
		if (!prop)
			continue;
		/* setup groups */
		nr_pins = of_property_count_u32_elems(np,
						      EQBR_PINCTRL_PINS);
		if (nr_pins <= 0) {
			dev_err(dev, "No pins in the group: %s\n",
				prop->name);
			return -EINVAL;
		}
		grps[i].name = prop->value;
		pins = devm_kcalloc(dev, nr_pins, sizeof(*pins),
				    GFP_KERNEL);
		if (!pins)
			return -ENOMEM;
		for (j = 0; j < nr_pins; j++) {
			if (of_property_read_u32_index(np, EQBR_PINCTRL_PINS,
						       j, &pin_id)) {
				dev_err(dev, "Group %s: Read intel pins id failed\n",
					grps[i].name);
				return -EINVAL;
			}
			if (pin_id >= drvdata->pctl_desc.npins) {
				dev_err(dev, "Group %s: Invalid pin ID, idx: %d, pin %u\n",
					grps[i].name, j, pin_id);
				return -EINVAL;
			}
			pins[j] = pin_id;
		}
		grps[i].pins = pins;
		grps[i].nr_pins = nr_pins;

		/* setup functions */
		if (of_property_read_string(np, EQBR_PINCTRL_FUNCTION,
					    &fn_name)) {
			/* some groups may not have function, it's OK */
			dev_dbg(dev, "Group %s: not function binded!\n",
				grps[i].name);
			continue;
		}

		nr_pmx = of_property_count_u32_elems(np,
						     EQBR_PINCTRL_MUX);
		if (nr_pmx <= 0) {
			dev_err(dev, "No mux in the group: %s\n",
				prop->name);
			return -EINVAL;
		}
		dev_dbg(dev, "Group %s: nr_pmx %d nr_pins %d.\n",
			grps[i].name, nr_pmx, nr_pins);
		if (nr_pmx != 1 && nr_pmx != nr_pins) {
			dev_err(dev, "Group %s: DT Number of MUX value (%d) not match to number of Pins (%d)!\n",
				grps[i].name, nr_pmx, nr_pins);
			return -EINVAL;
		}

		pmx = devm_kcalloc(dev, nr_pins, sizeof(*pmx),
				   GFP_KERNEL);
		if (!pmx)
			return -ENOMEM;

		if (nr_pmx == 1) {
			if (of_property_read_u32(np,
						 EQBR_PINCTRL_MUX, &pmx_id)) {
				dev_err(dev, "Group %s: Read intel mux failed\n",
					grps[i].name);
				return -EINVAL;
			}
			for (j = 0; j < nr_pins; j++)
				pmx[j] = pmx_id;
		} else { /* nr_pmx == nr_pins */
			for (j = 0; j < nr_pins; j++) {
				if (of_property_read_u32_index(np,
							       EQBR_PINCTRL_MUX,
							       j, &pmx_id)) {
					dev_err(dev, "Group %s: Read intel mux failed\n",
						grps[i].name);
					return -EINVAL;
				}
				pmx[j] = pmx_id;
			}
		}

		grps[i].pmx = pmx;

		if (is_func_exist(funcs, fn_name, nr_funcs, &fid)) {
			ret = add_group_to_func(dev, funcs, nr_funcs,
						fid, prop->value);
			if (ret)
				return ret;
		} else {
			ret = create_pin_func(dev, &funcs, nr_funcs,
					      fn_name, prop->value);
			if (ret)
				return ret;
			nr_funcs++;
		}
		i++;
	}

	drvdata->pin_grps = grps;
	drvdata->nr_grps = nr_grps;
	drvdata->pmx_funcs = funcs;
	drvdata->nr_funcs = nr_funcs;

	dump_pinctrl_group_func(drvdata);

	return 0;
}

static int pinctrl_reg(struct intel_pinctrl_drv_data *drvdata)
{
	struct pinctrl_desc *pctl_desc;
	struct pinctrl_pin_desc *pdesc;
	struct device *dev;
	unsigned int nr_pins;
	char *pin_names;
	int i, ret;

	dev = drvdata->dev;
	pctl_desc = &drvdata->pctl_desc;
	pctl_desc->name = "intel-pinctrl";
	pctl_desc->owner = THIS_MODULE;
	pctl_desc->pctlops = &eqbr_pctl_ops;
	pctl_desc->pmxops = &eqbr_pinmux_ops;
	pctl_desc->confops = &eqbr_pinconf_ops;
	spin_lock_init(&drvdata->lock);

	for (i = 0, nr_pins = 0; i < drvdata->nr_banks; i++)
		nr_pins += drvdata->pin_banks[i].nr_pins;

	pdesc = devm_kcalloc(dev, nr_pins, sizeof(*pdesc), GFP_KERNEL);
	if (!pdesc)
		return -ENOMEM;
	pin_names = devm_kcalloc(dev, nr_pins, PIN_NAME_LEN, GFP_KERNEL);
	if (!pin_names)
		return -ENOMEM;

	for (i = 0; i < nr_pins; i++) {
		sprintf(pin_names, PIN_NAME_FMT, i);
		pdesc[i].number = i;
		pdesc[i].name = pin_names;
		pin_names += PIN_NAME_LEN;
	}
	pctl_desc->pins = pdesc;
	pctl_desc->npins = nr_pins;
	dev_dbg(dev, "pinctrl total pin number: %u\n", nr_pins);

	ret = pinctrl_setup_from_dt(dev, drvdata);
	if (ret)
		return ret;

	drvdata->pctl_dev = devm_pinctrl_register(dev, pctl_desc, drvdata);
	if (IS_ERR(drvdata->pctl_dev)) {
		dev_err(dev, "Register pinctrl failed!\n");
		return PTR_ERR(drvdata->pctl_dev);
	}

	return 0;
}

static int pinbank_init(struct device_node *np,
			struct intel_pinctrl_drv_data *drvdata,
			struct intel_pin_bank *bank, unsigned int id)
{
	unsigned int offset;
	struct device *dev = drvdata->dev;

	if (of_property_read_u32(np, EQBR_PINBANK_REG, &offset)) {
		dev_err(dev, "pin bank doesn't have address offset!\n");
		return -EFAULT;
	}
	bank->membase = drvdata->membase + offset;
	if (of_property_read_u32(np, EQBR_PINBANK_PINBASE, &bank->pin_base)) {
		dev_err(dev, "pin bank doesn't have pin base ID!\n");
		return -EFAULT;
	}
	if (of_property_read_u32(np, EQBR_PINBANK_PINNUM, &bank->nr_pins)) {
		dev_err(dev, "pin bank doesn't have pin number!\n");
		return -EFAULT;
	}
	bank->aval_pinmap = readl(bank->membase + REG_AVAIL);
	bank->id = id;

	dev_info(dev, "pinbank id: %d, reg: 0x%x, pinbase: %u, pin number: %u, pinmap: 0x%x\n",
		 id, (unsigned int)bank->membase, bank->pin_base,
		 bank->nr_pins, bank->aval_pinmap);

	return 0;
}

static int pinbank_probe(struct intel_pinctrl_drv_data *drvdata)
{
	struct device_node *node, *np_bank, *np_pad;
	struct intel_pin_bank *banks;
	struct intel_gpio_desc *gpio_desc;
	struct property *prop;
	struct device *dev;
	const char *status;
	phandle phandle;
	int nr_bank, nr_gpio;
	int i, len;

	dev = drvdata->dev;
	node = dev->of_node;
	nr_bank = 0;
	nr_gpio = 0;

	/* Count pin bank number and gpio controller number */
	np_pad = of_find_node_by_name(node, "pad");
	if (!np_pad) {
		dev_err(dev, "Cannot find pad node in device tree!\n");
		return -ENODEV;
	}

	for_each_child_of_node(np_pad, np_bank) {
		if (of_find_property(np_bank, EQBR_PINBANK_PROPERTY, NULL)) {
			status = of_get_property(np_bank, "status", &len);
			if (status && len > 0 &&
			    strcmp(status, "disabled") == 0)
				continue;
			nr_bank++;
			if (of_find_property(np_bank,
					     EQBR_PINBANK_GPIO_CTRL, NULL))
				nr_gpio++;
		}
	}

	if (!nr_bank) {
		dev_err(dev, "No pin bank in devic tree!\n");
		return -ENODEV;
	}

	if (!nr_gpio)
		dev_info(dev, "None of the pin banks support gpio!\n");

	banks = devm_kcalloc(dev, nr_bank, sizeof(*banks), GFP_KERNEL);
	if (!banks)
		return -ENOMEM;

	if (nr_gpio) {
		gpio_desc = devm_kcalloc(dev, nr_gpio, sizeof(*gpio_desc),
					 GFP_KERNEL);
		if (!gpio_desc)
			return -ENOMEM;
	} else {
		gpio_desc = NULL;
	}

	dev_dbg(dev, "%s: found %d pinbank and %d gpio controller!\n",
		__func__, nr_bank, nr_gpio);

	/* Initialize Pin bank */
	i = 0;
	for_each_child_of_node(np_pad, np_bank) {
		if (of_find_property(np_bank, EQBR_PINBANK_PROPERTY, NULL)) {
			status = of_get_property(np_bank, "status", &len);
			if (status && len > 0 &&
			    strcmp(status, "disabled") == 0)
				continue;

			pinbank_init(np_bank, drvdata, banks + i, i);
			prop = of_find_property(np_bank,
						EQBR_PINBANK_GPIO_CTRL, NULL);
			if (prop) {
				phandle = be32_to_cpup(prop->value);
				gpio_desc[i].node
					= of_find_node_by_phandle(phandle);
				if (!gpio_desc[i].node) {
					dev_err(dev, "Cannot get gpio controller handler!\n");
					return -EFAULT;
				}
				gpio_desc[i].bank = banks + i;
			}
			i++;
		}
	}

	drvdata->pin_banks = banks;
	drvdata->nr_banks = nr_bank;
	drvdata->gpio_desc = gpio_desc;
	drvdata->nr_gpio_descs = nr_gpio;

	return 0;
}

/* Debug API print pinmux pins initialized before pinctrl driver */
static void dbg_print_inited_pin(struct intel_pinctrl_drv_data *drvdata)
{
	int i;
	u32 mux, pin;
	struct device *dev = drvdata->dev;
	struct intel_pin_bank *bank;

	dev_dbg(dev, "Report used pins before pinctrl driver!\n");
	for (i = 0; i < drvdata->nr_banks; i++) {
		bank = &drvdata->pin_banks[i];
		for (pin = 0; pin < bank->nr_pins; pin++) {
			if (!(BIT(pin) & bank->aval_pinmap))
				continue;
			mux = readl(bank->membase + pin * 4);
			if (mux != EQBR_MUX_GPIO)
				dev_dbg(dev, "PIN %u has been set to %u\n",
					bank->pin_base + pin, mux);
		}
	}
}

static void pinctrl_reseve_pins(struct intel_pinctrl_drv_data *drvdata)
{
	struct intel_pin_bank *bank;
	const struct intel_pin_group *group = NULL;
	unsigned int pin, offset;
	int i;

	for (i = 0; i < drvdata->nr_grps; i++) {
		if (strcmp(drvdata->pin_grps[i].name,
			   RESERVE_PINGROUP_NAME) == 0) {
			group = &drvdata->pin_grps[i];
			break;
		}
	}

	if (!group)
		return;

	for (i = 0; i < group->nr_pins; i++) {
		pin = group->pins[i];
		bank = find_pinbank_via_pin(drvdata, pin);
		if (!bank) {
			dev_err(drvdata->dev,
				"Failed find pinbank for reserved pin: %u\n",
				pin);
			continue;
		}
		offset = pin - bank->pin_base;
		bank->aval_pinmap &= ~(BIT(offset));
	}
}

/**
 * reset all pins to DEFAULT state, including below registers
 * PINMUX set to GPIO
 * DIR set to INPUT
 * Clear PULLUP/PULLDOWN/SLEW RATE/DRIVE CURRENT/OPEN DRAIN
 */
static void pinctrl_pin_reset(struct intel_pinctrl_drv_data *drvdata)
{
	int i;
	unsigned int pin;
	unsigned long flags;
	struct intel_gpio_desc *gdesc;

	dbg_print_inited_pin(drvdata);
	pinctrl_reseve_pins(drvdata);

	for (i = 0; i < drvdata->nr_gpio_descs; i++) {
		gdesc = &drvdata->gpio_desc[i];
		for (pin = 0; pin < gdesc->bank->nr_pins; pin++) {
			if (!(BIT(pin) & gdesc->bank->aval_pinmap))
				continue;
			eqbr_set_pin_mux(drvdata, EQBR_MUX_GPIO,
					 pin + gdesc->bank->pin_base);
			spin_lock_irqsave(&drvdata->lock, flags);
			eqbr_set_val(gdesc->bank->membase + REG_PUEN,
				     pin, 1, 0, NULL);
			eqbr_set_val(gdesc->bank->membase + REG_PDEN,
				     pin, 1, 0, NULL);
			eqbr_set_val(gdesc->bank->membase + REG_SRC,
				     pin, 1, 0, NULL);
			eqbr_set_val(gdesc->bank->membase + REG_OD,
				     pin, 1, 0, NULL);
			set_drv_cur(gdesc->bank->membase, pin, 0, NULL);
			spin_unlock_irqrestore(&drvdata->lock, flags);
		}
	}

	for (i = 0; i < drvdata->nr_banks; i++) {
		dev_dbg(drvdata->dev,
			"bank: %d, pullup: %u, pulldown: %u, slew rate: %u\n",
			i, readl(drvdata->pin_banks[i].membase + REG_PUEN),
			readl(drvdata->pin_banks[i].membase + REG_PDEN),
			readl(drvdata->pin_banks[i].membase + REG_SRC));
		dev_dbg(drvdata->dev,
			"bank: %d, DRCC0: %u, DRCC1: %u, open drain: %u\n",
			i, readl(drvdata->pin_banks[i].membase + REG_DCC0),
			readl(drvdata->pin_banks[i].membase + REG_DCC1),
			readl(drvdata->pin_banks[i].membase + REG_OD));
	}
}

static int intel_pinctrl_probe(struct platform_device *pdev)
{
	struct intel_pinctrl_drv_data *drvdata;
	struct device *dev = &pdev->dev;
	struct resource *res;
	int ret;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->dev = dev;
	platform_set_drvdata(pdev, drvdata);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	drvdata->membase = devm_ioremap_resource(dev, res);
	if (IS_ERR(drvdata->membase))
		return PTR_ERR(drvdata->membase);

	ret = pinbank_probe(drvdata);
	if (ret)
		return ret;

	ret = pinctrl_reg(drvdata);
	if (ret)
		return ret;

	ret = gpiolib_reg(drvdata);
	if (ret)
		return ret;

	pinctrl_pin_reset(drvdata);

	return 0;
}

static const struct of_device_id intel_pinctrl_dt_match[] = {
	{ .compatible = "intel,pinctrl-xrx500" },
	{},
};

static struct platform_driver intel_pinctrl_driver = {
	.probe		= intel_pinctrl_probe,
	.driver = {
		.name	= "intel-pinctrl",
		.of_match_table = intel_pinctrl_dt_match,
	},
};

static int __init intel_pinctrl_drv_register(void)
{
	return platform_driver_register(&intel_pinctrl_driver);
}
postcore_initcall(intel_pinctrl_drv_register);

MODULE_AUTHOR("Zhu Yixin <yixin.zhu@intel.com>");
MODULE_DESCRIPTION("intel pinctrl driver");
