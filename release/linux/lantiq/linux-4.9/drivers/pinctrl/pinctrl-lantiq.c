/*
 *  linux/drivers/pinctrl/pinctrl-lantiq.c
 *  based on linux/drivers/pinctrl/pinctrl-pxa3xx.c
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  publishhed by the Free Software Foundation.
 *
 *  Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>

#include "pinctrl-lantiq.h"

#ifdef CONFIG_PINCTRL_SYSFS

static struct ltq_pinctrl_sysfs pctrl_sysfs;
static DEFINE_MUTEX(sysfs_lock);
static ssize_t export_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t len);
static ssize_t unexport_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t len);
static struct class_attribute pinctrl_class_attrs[] = {
	__ATTR(export, 0200, NULL, export_store),
	__ATTR(unexport, 0200, NULL, unexport_store),
	__ATTR_NULL,
};
static struct class pinctrl_class = {
	.name =         "pinctrl",
	.owner =        THIS_MODULE,
	.class_attrs =  pinctrl_class_attrs,
};
static const struct ltq_cfg_param sysfs_cfg_params[] = {
	{"pull",			LTQ_PINCONF_PARAM_PULL},
	{"current_control",	LTQ_PINCONF_PARAM_DRIVE_CURRENT},
	{"slew_rate",		LTQ_PINCONF_PARAM_SLEW_RATE},
	{"open_drain",		LTQ_PINCONF_PARAM_OPEN_DRAIN},
};
static int get_conf_param(const char *conf, enum ltq_pinconf_param *param)
{
	int i;
	for (i = 0;i < sizeof(sysfs_cfg_params)/sizeof(struct ltq_cfg_param); i++) {
		if(!strcmp(sysfs_cfg_params[i].property, conf)) {
			*param = sysfs_cfg_params[i].param;
			return 0;
		}
	}
	return -1;
}

static ssize_t pad_ctrl_avail_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ltq_pin_sys_desc *desc = dev_get_drvdata(dev);
	struct ltq_pinmux_info *info;
	ssize_t			status;
	u32 pin, value;

	if(!pctrl_sysfs.sysfs_ops->pin_mux_avail_get)
		return -EPERM;
	info = platform_get_drvdata(pctrl_sysfs.pinctrl_platform_dev);
	pin = desc - pctrl_sysfs.pin_desc_array;
	mutex_lock(&sysfs_lock);
	if (!test_bit((FLAG_EXPORT), &(desc->flags)))
		status = -EIO;
	else {
		if ((!pctrl_sysfs.sysfs_ops->pin_mux_avail_get(info->pctrl, pin, &value))) {
			if(value)
				status = sprintf(buf, "%s\n", "AV");
			else
				status = sprintf(buf, "%s\n", "NAV");
		} else {
			status = -ENODEV;
		}

	}
	mutex_unlock(&sysfs_lock);
	return status;
}
static const DEVICE_ATTR(padctrl_availability, 0644,
		pad_ctrl_avail_show, NULL);

static ssize_t pin_conf_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct ltq_pin_sys_desc *desc = dev_get_drvdata(dev);
	unsigned long config;
	struct ltq_pinmux_info *info;
	ssize_t	status = 0;
	u32 pin;
	enum ltq_pinconf_param param;
	long value ;

	if(!pctrl_sysfs.sysfs_ops->pin_config_set)
		return -EPERM;
	info = platform_get_drvdata(pctrl_sysfs.pinctrl_platform_dev);
	pin = desc - pctrl_sysfs.pin_desc_array;

	mutex_lock(&sysfs_lock);
	if (!test_bit(FLAG_EXPORT, &desc->flags))
		status = -EIO;

	if (get_conf_param(attr->attr.name, &param))
		status = -EIO;
	if (status) {
		mutex_unlock(&sysfs_lock);
		return status ? : size;
	}
	switch (param) {
		case LTQ_PINCONF_PARAM_PULL:
			if (sysfs_streq(buf, "DIS"))
				value = 0;
			else if (sysfs_streq(buf, "EN"))
				value = 1;
			else
				status = -EINVAL;
			break;
		case LTQ_PINCONF_PARAM_SLEW_RATE:
			status = kstrtol(buf, 0, &value);
			break;
		case LTQ_PINCONF_PARAM_OPEN_DRAIN:
			if (sysfs_streq(buf, "NOP"))
				value = 0;
			else if (sysfs_streq(buf, "EN"))
				value = 1;
			else
				status = -EINVAL;
			break;
		case LTQ_PINCONF_PARAM_DRIVE_CURRENT:
			status = kstrtol(buf, 0, &value);
			break;
		case LTQ_PINCONF_PARAM_OUTPUT:
		default:
			status =  -EIO;
			break;
	}
	if (status == 0) {
		config = LTQ_PINCONF_PACK(param, value);
		if (pctrl_sysfs.sysfs_ops->pin_config_set(info->pctrl, pin, &config, 1))
			status = -ENODEV;
	}

	mutex_unlock(&sysfs_lock);
	return status ? : size;
}
static ssize_t pin_conf_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ltq_pin_sys_desc *desc = dev_get_drvdata(dev);
	struct ltq_pinmux_info *info;
	ssize_t status = 0;
	unsigned long config;
	enum ltq_pinconf_param param;
	u32 pin;
	int arg;

	if(!pctrl_sysfs.sysfs_ops->pin_config_get)
		return -EPERM;

	info = platform_get_drvdata(pctrl_sysfs.pinctrl_platform_dev);
	pin = desc - pctrl_sysfs.pin_desc_array;

	mutex_lock(&sysfs_lock);
	if (!test_bit((FLAG_EXPORT), &(desc->flags)))
		status = -EIO;
	if (get_conf_param(attr->attr.name, &param))
		status = -EIO;
	if (status) {
		mutex_unlock(&sysfs_lock);
		return status;
	}
	config = LTQ_PINCONF_PACK(param, 0);
	if (!pctrl_sysfs.sysfs_ops->pin_config_get(info->pctrl, pin, &config)) {
		arg = LTQ_PINCONF_UNPACK_ARG(config);
		switch (param) {
		case LTQ_PINCONF_PARAM_PULL:
			if(arg == 1)
				status = sprintf(buf, "%s\n", "EN");
			if(arg == 2)
				status = sprintf(buf, "%s\n", "DIS");
			if(arg == 0)
				status = sprintf(buf, "%d\n", arg);
			break;
		case LTQ_PINCONF_PARAM_SLEW_RATE:
			status = sprintf(buf, "%d\n", arg);
			break;
		case LTQ_PINCONF_PARAM_OPEN_DRAIN:
			if(arg)
				status = sprintf(buf, "%s\n", "EN");
			else
				status = sprintf(buf, "%s\n", "NOP");
			break;
		case LTQ_PINCONF_PARAM_DRIVE_CURRENT:
			status = sprintf(buf, "%d\n", arg);
			break;
		case LTQ_PINCONF_PARAM_OUTPUT:
		default:
			status =  -EIO;
			break;
		}
	} else {
		status = -ENODEV;
	}

	mutex_unlock(&sysfs_lock);
	return status;
}
static const DEVICE_ATTR(pull, 0644,
		pin_conf_show, pin_conf_store);
static const DEVICE_ATTR(slew_rate, 0644,
		pin_conf_show, pin_conf_store);
static const DEVICE_ATTR(open_drain, 0644,
		pin_conf_show, pin_conf_store);
static const DEVICE_ATTR(current_control, 0644,
		pin_conf_show, pin_conf_store);

static ssize_t pinmux_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ltq_pin_sys_desc *desc = dev_get_drvdata(dev);
	struct ltq_pinmux_info *info;
	ssize_t status = 0;
	u32 pin;
	u32 mux;

	if(!pctrl_sysfs.sysfs_ops->pin_mux_get)
		return -EPERM;
	info = platform_get_drvdata(pctrl_sysfs.pinctrl_platform_dev);
	pin = desc - pctrl_sysfs.pin_desc_array;
	mutex_lock(&sysfs_lock);
	if (!test_bit((FLAG_EXPORT), &(desc->flags))) {
		status = -EIO;
	} else {
		if (!pctrl_sysfs.sysfs_ops->pin_mux_get(info->pctrl, pin, &mux))
			status = sprintf(buf, "%d\n", mux);
		else
			status = -ENODEV;
	}
	mutex_unlock(&sysfs_lock);
	return status;
}
static ssize_t pinmux_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct ltq_pin_sys_desc *desc = dev_get_drvdata(dev);
	struct ltq_pinmux_info *info;
	ssize_t	status = 0;
	u32 pin;
	long value;

	if(!pctrl_sysfs.sysfs_ops->pin_mux_set)
		return -EPERM;
	info = platform_get_drvdata(pctrl_sysfs.pinctrl_platform_dev);
	pin = desc - pctrl_sysfs.pin_desc_array;
	mutex_lock(&sysfs_lock);
	if (!test_bit(FLAG_EXPORT, &desc->flags)) {
		status = -EIO;
	} else {
		status = kstrtol(buf, 0, &value);
		if (status == 0) {
			if (pctrl_sysfs.sysfs_ops->pin_mux_set(info->pctrl, pin, value))
				status = -ENODEV;
		}
	}
	mutex_unlock(&sysfs_lock);
	return status ? : size;
}
static const DEVICE_ATTR(pinmux, 0644,
		pinmux_show, pinmux_store);
static const struct attribute *pin_attrs[] = {
	&dev_attr_pull.attr,
	&dev_attr_open_drain.attr,
	&dev_attr_pinmux.attr,
	&dev_attr_slew_rate.attr,
	&dev_attr_current_control.attr,
	&dev_attr_padctrl_availability.attr,
	NULL,
};
static const struct attribute_group pin_attr_group = {
	.attrs = (struct attribute **) pin_attrs,
};
static int pin_export(unsigned int pin)
{
	int		status;
	struct device	*dev;
	if (!pinctrl_class.p)
		return 0;
	mutex_lock(&sysfs_lock);
	if (pin >= pctrl_sysfs.total_pins) {
		pr_err("%s: invalid pin\n", __func__);
		status = -ENODEV;
		goto fail_unlock;
	}
	if (test_bit((FLAG_EXPORT), &pctrl_sysfs.pin_desc_array[pin].flags)) {
		pr_err("Pin %d already exported\r\n", pin);
		status = -EPERM;
		goto fail_unlock;
	}
	dev = device_create(&pinctrl_class,
	&pctrl_sysfs.pinctrl_platform_dev->dev,
	MKDEV(0, 0),
	&pctrl_sysfs.pin_desc_array[pin],
	"pin_%d", pin);
	if (IS_ERR(dev)) {
		status = PTR_ERR(dev);
		goto fail_unlock;
	}
	set_bit((FLAG_EXPORT), &pctrl_sysfs.pin_desc_array[pin].flags);
	status = sysfs_create_group(&dev->kobj, &pin_attr_group);
	if (status)
		goto fail_unregister_device;
	mutex_unlock(&sysfs_lock);
	return 0;
fail_unregister_device:
	device_unregister(dev);
fail_unlock:
	mutex_unlock(&sysfs_lock);
	return status;
}
static int match_export(struct device *dev, const void *data)
{
	return dev_get_drvdata(dev) == data;
}
static int pin_unexport(unsigned int pin)
{
	int			status = 0;
	struct device		*dev = NULL;
	if (pin >= pctrl_sysfs.total_pins) {
		pr_warn("%s: invalid pin\n", __func__);
		status = -ENODEV;
		goto fail_unlock;
	}
	mutex_lock(&sysfs_lock);
	if (!(test_bit((FLAG_EXPORT), &pctrl_sysfs.pin_desc_array[pin].flags))) {
		status = -ENODEV;
		goto fail_unlock;
	}
	dev = class_find_device(&pinctrl_class,
	NULL, &pctrl_sysfs.pin_desc_array[pin], match_export);
	if (dev) {
		clear_bit((FLAG_EXPORT), &pctrl_sysfs.pin_desc_array[pin].flags);
	} else {
		status = -ENODEV;
		goto fail_unlock;
	}
	if (dev) {
		device_unregister(dev);
		put_device(dev);
	}
	mutex_unlock(&sysfs_lock);
	return 0;
fail_unlock:
	mutex_unlock(&sysfs_lock);
	pr_debug("%s:  status %d\n", __func__, status);
	return status;
}
static ssize_t export_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t len)
{
	long			pin;
	int			status;
	status = kstrtol(buf, 0, &pin);
	if (status < 0)
		goto done;
	status = pin_export((unsigned int)pin);
	if (status < 0) {
		if (status == -EPROBE_DEFER)
			status = -ENODEV;
		goto done;
	}
done:
	if (status)
		pr_debug("%s: status %d\n", __func__, status);
	return status ? : len;
}
static ssize_t unexport_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t len)
{
	long	pin;
	int			status;
	status = kstrtol(buf, 0, &pin);
	if (status < 0)
		goto done;
	status = pin_unexport(pin);
done:
	if (status)
		pr_debug("%s: status %d\n", __func__, status);
	return status ? : len;
}
int pinctrl_sysfs_init(struct platform_device *pdev, int total_pins, struct ltq_pinctrl_sysfs_ops *ops)
{
	int	status;
	pctrl_sysfs.total_pins = total_pins;
	pctrl_sysfs.pinctrl_platform_dev = pdev;
	pctrl_sysfs.pin_desc_array = kzalloc(total_pins * sizeof(struct ltq_pin_sys_desc), GFP_KERNEL);
	pctrl_sysfs.sysfs_ops = ops;
	status = class_register(&pinctrl_class);
	return status;
}
#endif

static int ltq_get_group_count(struct pinctrl_dev *pctrldev)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);
	return info->num_grps;
}

static const char *ltq_get_group_name(struct pinctrl_dev *pctrldev,
					 unsigned selector)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);
	if (selector >= info->num_grps)
		return NULL;
	return info->grps[selector].name;
}

static int ltq_get_group_pins(struct pinctrl_dev *pctrldev,
				 unsigned selector,
				 const unsigned **pins,
				 unsigned *num_pins)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);
	if (selector >= info->num_grps)
		return -EINVAL;
	*pins = info->grps[selector].pins;
	*num_pins = info->grps[selector].npins;
	return 0;
}

static void ltq_pinctrl_dt_free_map(struct pinctrl_dev *pctldev,
				    struct pinctrl_map *map, unsigned num_maps)
{
	int i;

	for (i = 0; i < num_maps; i++)
		if (map[i].type == PIN_MAP_TYPE_CONFIGS_PIN ||
		    map[i].type == PIN_MAP_TYPE_CONFIGS_GROUP)
			kfree(map[i].data.configs.configs);
	kfree(map);
}

static void ltq_pinctrl_pin_dbg_show(struct pinctrl_dev *pctldev,
					struct seq_file *s,
					unsigned offset)
{
	seq_printf(s, " %s", dev_name(pctldev->dev));
}

static void ltq_pinctrl_dt_subnode_to_map(struct pinctrl_dev *pctldev,
				struct device_node *np,
				struct pinctrl_map **map)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctldev);
	struct property *pins = of_find_property(np, "lantiq,pins", NULL);
	struct property *groups = of_find_property(np, "lantiq,groups", NULL);
	unsigned long configs[3];
	unsigned num_configs = 0;
	struct property *prop;
	const char *group, *pin;
	const char *function;
	int ret, i;

	if (!pins && !groups) {
		dev_err(pctldev->dev, "%s defines neither pins nor groups\n",
			np->name);
		return;
	}

	if (pins && groups) {
		dev_err(pctldev->dev, "%s defines both pins and groups\n",
			np->name);
		return;
	}

	ret = of_property_read_string(np, "lantiq,function", &function);
	if (groups && !ret) {
		of_property_for_each_string(np, "lantiq,groups", prop, group) {
			(*map)->type = PIN_MAP_TYPE_MUX_GROUP;
			(*map)->name = function;
			(*map)->data.mux.group = group;
			(*map)->data.mux.function = function;
			(*map)++;
		}
	}

	for (i = 0; i < info->num_params; i++) {
		u32 val;
		int ret = of_property_read_u32(np,
				info->params[i].property, &val);
		if (!ret)
			configs[num_configs++] =
				LTQ_PINCONF_PACK(info->params[i].param,
				val);
	}

	if (!num_configs)
		return;

	of_property_for_each_string(np, "lantiq,pins", prop, pin) {
		(*map)->data.configs.configs = kmemdup(configs,
					num_configs * sizeof(unsigned long),
					GFP_KERNEL);
		(*map)->type = PIN_MAP_TYPE_CONFIGS_PIN;
		(*map)->name = pin;
		(*map)->data.configs.group_or_pin = pin;
		(*map)->data.configs.num_configs = num_configs;
		(*map)++;
	}
	of_property_for_each_string(np, "lantiq,groups", prop, group) {
		(*map)->data.configs.configs = kmemdup(configs,
					num_configs * sizeof(unsigned long),
					GFP_KERNEL);
		(*map)->type = PIN_MAP_TYPE_CONFIGS_GROUP;
		(*map)->name = group;
		(*map)->data.configs.group_or_pin = group;
		(*map)->data.configs.num_configs = num_configs;
		(*map)++;
	}
}

static int ltq_pinctrl_dt_subnode_size(struct device_node *np)
{
	int ret;

	ret = of_property_count_strings(np, "lantiq,groups");
	if (ret < 0)
		ret = of_property_count_strings(np, "lantiq,pins");
	return ret;
}

static int ltq_pinctrl_dt_node_to_map(struct pinctrl_dev *pctldev,
				      struct device_node *np_config,
				      struct pinctrl_map **map,
				      unsigned *num_maps)
{
	struct pinctrl_map *tmp;
	struct device_node *np;
	int max_maps = 0;

	for_each_child_of_node(np_config, np)
		max_maps += ltq_pinctrl_dt_subnode_size(np);
	*map = kzalloc(max_maps * sizeof(struct pinctrl_map) * 2, GFP_KERNEL);
	if (!*map)
		return -ENOMEM;
	tmp = *map;

	for_each_child_of_node(np_config, np)
		ltq_pinctrl_dt_subnode_to_map(pctldev, np, &tmp);
	*num_maps = ((int)(tmp - *map));

	return 0;
}

static const struct pinctrl_ops ltq_pctrl_ops = {
	.get_groups_count	= ltq_get_group_count,
	.get_group_name		= ltq_get_group_name,
	.get_group_pins		= ltq_get_group_pins,
	.pin_dbg_show		= ltq_pinctrl_pin_dbg_show,
	.dt_node_to_map		= ltq_pinctrl_dt_node_to_map,
	.dt_free_map		= ltq_pinctrl_dt_free_map,
};

static int ltq_pmx_func_count(struct pinctrl_dev *pctrldev)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);

	return info->num_funcs;
}

static const char *ltq_pmx_func_name(struct pinctrl_dev *pctrldev,
					 unsigned selector)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);

	if (selector >= info->num_funcs)
		return NULL;

	return info->funcs[selector].name;
}

static int ltq_pmx_get_groups(struct pinctrl_dev *pctrldev,
				unsigned func,
				const char * const **groups,
				unsigned * const num_groups)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);

	*groups = info->funcs[func].groups;
	*num_groups = info->funcs[func].num_groups;

	return 0;
}

/* Return function number. If failure, return negative value. */
static int match_mux(const struct ltq_mfp_pin *mfp, unsigned mux)
{
	int i;
	for (i = 0; i < LTQ_MAX_MUX; i++) {
		if (mfp->func[i] == mux)
			break;
	}
	if (i >= LTQ_MAX_MUX)
		return -EINVAL;
	return i;
}

/* dont assume .mfp is linearly mapped. find the mfp with the correct .pin */
static int match_mfp(const struct ltq_pinmux_info *info, int pin)
{
	int i;
	for (i = 0; i < info->num_mfp; i++) {
		if (info->mfp[i].pin == pin)
			return i;
	}
	return -1;
}

/* check whether current pin configuration is valid. Negative for failure */
static int match_group_mux(const struct ltq_pin_group *grp,
			   const struct ltq_pinmux_info *info,
			   unsigned mux)
{
	int i, pin, ret = 0;
	for (i = 0; i < grp->npins; i++) {
		pin = match_mfp(info, grp->pins[i]);
		if (pin < 0) {
			dev_err(info->dev, "could not find mfp for pin %d\n",
				grp->pins[i]);
			return -EINVAL;
		}
		ret = match_mux(&info->mfp[pin], mux);
		if (ret < 0) {
			dev_err(info->dev, "Can't find mux %d on pin%d\n",
				mux, pin);
			break;
		}
	}
	return ret;
}

static int ltq_pmx_set(struct pinctrl_dev *pctrldev,
		       unsigned func,
		       unsigned group)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);
	const struct ltq_pin_group *pin_grp = &info->grps[group];
	int i, pin, pin_func, ret;

	if (!pin_grp->npins ||
		(match_group_mux(pin_grp, info, pin_grp->mux) < 0)) {
		dev_err(info->dev, "Failed to set the pin group: %s\n",
			info->grps[group].name);
		return -EINVAL;
	}
	for (i = 0; i < pin_grp->npins; i++) {
		pin = match_mfp(info, pin_grp->pins[i]);
		if (pin < 0) {
			dev_err(info->dev, "could not find mfp for pin %d\n",
				pin_grp->pins[i]);
			return -EINVAL;
		}
		pin_func = match_mux(&info->mfp[pin], pin_grp->mux);
		ret = info->apply_mux(pctrldev, pin, pin_func);
		if (ret) {
			dev_err(info->dev,
				"failed to apply mux %d for pin %d\n",
				pin_func, pin);
			return ret;
		}
	}
	return 0;
}

static int ltq_pmx_gpio_request_enable(struct pinctrl_dev *pctrldev,
				struct pinctrl_gpio_range *range,
				unsigned pin)
{
	struct ltq_pinmux_info *info = pinctrl_dev_get_drvdata(pctrldev);
	int mfp = match_mfp(info, pin);
	int pin_func;

	if (mfp < 0) {
		dev_err(info->dev, "could not find mfp for pin %d\n", pin);
		return -EINVAL;
	}

	pin_func = match_mux(&info->mfp[mfp], 0);
	if (pin_func < 0) {
		dev_err(info->dev, "No GPIO function on pin%d\n", mfp);
		return -EINVAL;
	}

	return info->apply_mux(pctrldev, mfp, pin_func);
}

static const struct pinmux_ops ltq_pmx_ops = {
	.get_functions_count	= ltq_pmx_func_count,
	.get_function_name	= ltq_pmx_func_name,
	.get_function_groups	= ltq_pmx_get_groups,
	.set_mux		= ltq_pmx_set,
	.gpio_request_enable	= ltq_pmx_gpio_request_enable,
	.strict = true,
};

/*
 * allow different socs to register with the generic part of the lanti
 * pinctrl code
 */
int ltq_pinctrl_register(struct platform_device *pdev,
				struct ltq_pinmux_info *info)
{
	struct pinctrl_desc *desc;

	if (!info)
		return -EINVAL;
	desc = info->desc;
	desc->pctlops = &ltq_pctrl_ops;
	desc->pmxops = &ltq_pmx_ops;
	info->dev = &pdev->dev;

	info->pctrl = devm_pinctrl_register(&pdev->dev, desc, info);
	if (IS_ERR(info->pctrl)) {
		dev_err(&pdev->dev, "failed to register LTQ pinmux driver\n");
		return PTR_ERR(info->pctrl);
	}
	platform_set_drvdata(pdev, info);
	return 0;
}
