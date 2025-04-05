// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 Richtek Technology Corp.
 *
 * ChiYuan Huang <cy_huang@richtek.com>
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/util_macros.h>

#include <linux/iio/buffer.h>
#include <linux/iio/iio.h>
#include <linux/iio/driver.h>
#include <linux/iio/machine.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/triggered_buffer.h>

#define RTQ6056_REG_CONFIG	0x00
#define RTQ6056_REG_SHUNTVOLT	0x01
#define RTQ6056_REG_BUSVOLT	0x02
#define RTQ6056_REG_POWER	0x03
#define RTQ6056_REG_CURRENT	0x04
#define RTQ6056_REG_CALIBRATION	0x05
#define RTQ6056_REG_MASKENABLE	0x06
#define RTQ6056_REG_ALERTLIMIT	0x07
#define RTQ6056_REG_MANUFACTID	0xFE
#define RTQ6056_REG_DIEID	0xFF

#define RTQ6056_MAX_CHANNEL	4
#define RTQ6056_VENDOR_ID	0x1214
#define RTQ6056_DEFAULT_CONFIG	0x4127
#define RTQ6056_CONT_ALLON	7

#define RTQ6056_CONFIG_OPMODE_MASK	GENMASK(2, 0)
#define RTQ6056_CONFIG_OPMODE(x)	((x << 0) & RTQ6056_CONFIG_OPMODE_MASK)
#define RTQ6056_CONFIG_VSHUNTCT_MASK	GENMASK(5, 3)
#define RTQ6056_CONFIG_VSHUNTCT(x)	((x << 3) & \
					 RTQ6056_CONFIG_VSHUNTCT_MASK)
#define RTQ6056_CONFIG_VBUSCT_MASK	GENMASK(8, 6)
#define RTQ6056_CONFIG_VBUSCT(x)	((x << 6) & RTQ6056_CONFIG_VBUSCT_MASK)
#define RTQ6056_CONFIG_AVG_MASK		GENMASK(11, 9)
#define RTQ6056_CONFIG_AVG(x)		((x << 9) & RTQ6056_CONFIG_AVG_MASK)
#define RTQ6056_CONFIG_RESET_MASK	GENMASK(15, 15)
#define RTQ6056_CONFIG_RESET(x)		((x << 15) & RTQ6056_CONFIG_RESET_MASK)

struct rtq6056_priv {
	struct device *dev;
	struct regmap *regmap;
	u32 shunt_resistor_uohm;
	int vshuntct_us;
	int vbusct_us;
	int avg_sample;
};

static struct iio_map rtq6056_maps[] = {
	{
		.consumer_dev_name = "voltage0",
		.consumer_channel = "voltage0",
		.adc_channel_label = "Vshunt",
	}, {
		.consumer_dev_name = "voltage1",
		.consumer_channel = "voltage1",
		.adc_channel_label = "Vbus",
	}, {
		.consumer_dev_name = "power2",
		.consumer_channel = "power2",
		.adc_channel_label = "Power",
	}, {
		.consumer_dev_name = "current3",
		.consumer_channel = "current3",
		.adc_channel_label = "Current",
	}, { /* sentinel */ }
};

static const struct iio_chan_spec rtq6056_channels[RTQ6056_MAX_CHANNEL + 1] = {
	{
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.address = RTQ6056_REG_SHUNTVOLT,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SCALE) |
				      BIT(IIO_CHAN_INFO_SAMP_FREQ),
		.info_mask_separate_available = BIT(IIO_CHAN_INFO_SAMP_FREQ),
		.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.info_mask_shared_by_all_available = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.scan_index = 0,
		.scan_type = {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_CPU,
		},
		.datasheet_name = "voltage0",
	},
	{
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 1,
		.address = RTQ6056_REG_BUSVOLT,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SCALE) |
				      BIT(IIO_CHAN_INFO_SAMP_FREQ),
		.info_mask_separate_available = BIT(IIO_CHAN_INFO_SAMP_FREQ),
		.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.info_mask_shared_by_all_available = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.scan_index = 1,
		.scan_type = {
			.sign = 'u',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_CPU,
		},
		.datasheet_name = "voltage1",
	},
	{
		.type = IIO_POWER,
		.indexed = 1,
		.channel = 2,
		.address = RTQ6056_REG_POWER,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SCALE) |
				      BIT(IIO_CHAN_INFO_SAMP_FREQ),
		.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.info_mask_shared_by_all_available = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.scan_index = 2,
		.scan_type = {
			.sign = 'u',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_CPU,
		},
		.datasheet_name = "power2",
	},
	{
		.type = IIO_CURRENT,
		.indexed = 1,
		.channel = 3,
		.address = RTQ6056_REG_CURRENT,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SAMP_FREQ),
		.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.info_mask_shared_by_all_available = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO),
		.scan_index = 3,
		.scan_type = {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_CPU,
		},
		.datasheet_name = "current3",
	},
	IIO_CHAN_SOFT_TIMESTAMP(RTQ6056_MAX_CHANNEL),
};

static int rtq6056_adc_read_channel(struct rtq6056_priv *priv,
				    struct iio_chan_spec const *ch,
				    int *val)
{
	struct device *dev = priv->dev;
	unsigned int addr = ch->address;
	unsigned int regval;
	int ret;

	pm_runtime_get_sync(dev);
	ret = regmap_read(priv->regmap, addr, &regval);
	pm_runtime_mark_last_busy(dev);
	pm_runtime_put(dev);
	if (ret)
		return ret;

	/* Power and VBUS is unsigned 16-bit, others are signed 16-bit */
	if (addr == RTQ6056_REG_BUSVOLT || addr == RTQ6056_REG_POWER)
		*val = regval;
	else
		*val = sign_extend32(regval, 16);

	return IIO_VAL_INT;
}

static int rtq6056_adc_read_scale(struct iio_chan_spec const *ch, int *val,
				  int *val2)
{
	switch (ch->address) {
	case RTQ6056_REG_SHUNTVOLT:
		/* VSHUNT lsb  2.5uV */
		*val = 2500;
		*val2 = 1000000;
		return IIO_VAL_FRACTIONAL;
	case RTQ6056_REG_BUSVOLT:
		/* VBUS lsb 1.25mV */
		*val = 1250;
		*val2 = 1000;
		return IIO_VAL_FRACTIONAL;
	case RTQ6056_REG_POWER:
		/* Power lsb 25mW */
		*val = 25;
		return IIO_VAL_INT;
	default:
		return -EINVAL;
	}
}

/*
 * Sample frequency for channel VSHUNT and VBUS. The indices correspond
 * with the bit value expected by the chip. And it can be found at
 * https://www.richtek.com/assets/product_file/RTQ6056/DSQ6056-00.pdf
 */
static const int rtq6056_samp_freq_list[] = {
	7194, 4926, 3717, 1904, 964, 485, 243, 122,
};

static int rtq6056_adc_set_samp_freq(struct rtq6056_priv *priv,
				     struct iio_chan_spec const *ch, int val)
{
	unsigned int selector;
	int *ct, ret;

	if (val > 7194 || val < 122)
		return -EINVAL;

	if (ch->address == RTQ6056_REG_SHUNTVOLT) {
		ct = &priv->vshuntct_us;
	} else if (ch->address == RTQ6056_REG_BUSVOLT) {
		ct = &priv->vbusct_us;
	} else
		return -EINVAL;

	selector = find_closest_descending(val, rtq6056_samp_freq_list,
					   ARRAY_SIZE(rtq6056_samp_freq_list));

	if (ch->address == RTQ6056_REG_SHUNTVOLT) {
		ret = regmap_update_bits(priv->regmap, RTQ6056_REG_CONFIG,
					 RTQ6056_CONFIG_VSHUNTCT_MASK,
					 RTQ6056_CONFIG_VSHUNTCT(selector));
	} else {
		ret = regmap_update_bits(priv->regmap, RTQ6056_REG_CONFIG,
					 RTQ6056_CONFIG_VBUSCT_MASK,
					 RTQ6056_CONFIG_VBUSCT(selector));
	}
	if (ret)
		return ret;

	*ct = 1000000 / rtq6056_samp_freq_list[selector];

	return 0;
}

/*
 * Available averaging rate for rtq6056. The indices correspond with the bit
 * value expected by the chip. And it can be found at
 * https://www.richtek.com/assets/product_file/RTQ6056/DSQ6056-00.pdf
 */
static const int rtq6056_avg_sample_list[] = {
	1, 4, 16, 64, 128, 256, 512, 1024,
};

static int rtq6056_adc_set_average(struct rtq6056_priv *priv, int val)
{
	unsigned int selector;
	int ret;

	if (val > 1024 || val < 1)
		return -EINVAL;

	selector = find_closest(val, rtq6056_avg_sample_list,
				ARRAY_SIZE(rtq6056_avg_sample_list));

	ret = regmap_update_bits(priv->regmap, RTQ6056_REG_CONFIG,
				 RTQ6056_CONFIG_AVG_MASK,
				 RTQ6056_CONFIG_AVG(selector));

	if (ret)
		return ret;

	priv->avg_sample = rtq6056_avg_sample_list[selector];

	return 0;
}

static int rtq6056_adc_get_sample_freq(struct rtq6056_priv *priv,
				       struct iio_chan_spec const *ch, int *val)
{
	int sample_time;

	if (ch->address == RTQ6056_REG_SHUNTVOLT)
		sample_time = priv->vshuntct_us;
	else if (ch->address == RTQ6056_REG_BUSVOLT)
		sample_time = priv->vbusct_us;
	else {
		sample_time = priv->vshuntct_us + priv->vbusct_us;
		sample_time *= priv->avg_sample;
	}

	*val = 1000000 / sample_time;

	return IIO_VAL_INT;
}

static int rtq6056_adc_read_raw(struct iio_dev *indio_dev,
				struct iio_chan_spec const *chan, int *val,
				int *val2, long mask)
{
	struct rtq6056_priv *priv = iio_priv(indio_dev);

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		return rtq6056_adc_read_channel(priv, chan, val);
	case IIO_CHAN_INFO_SCALE:
		return rtq6056_adc_read_scale(chan, val, val2);
	case IIO_CHAN_INFO_OVERSAMPLING_RATIO:
		*val = priv->avg_sample;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SAMP_FREQ:
		return rtq6056_adc_get_sample_freq(priv, chan, val);
	default:
		return -EINVAL;
	}
}

static int rtq6056_adc_read_avail(struct iio_dev *indio_dev,
				  struct iio_chan_spec const *chan,
				  const int **vals, int *type, int *length,
				  long mask)
{
	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		*vals = rtq6056_samp_freq_list;
		*type = IIO_VAL_INT;
		*length = ARRAY_SIZE(rtq6056_samp_freq_list);
		return IIO_AVAIL_LIST;
	case IIO_CHAN_INFO_OVERSAMPLING_RATIO:
		*vals = rtq6056_avg_sample_list;
		*type = IIO_VAL_INT;
		*length = ARRAY_SIZE(rtq6056_avg_sample_list);
		return IIO_AVAIL_LIST;
	default:
		return -EINVAL;
	}
}

static int rtq6056_adc_write_raw(struct iio_dev *indio_dev,
				 struct iio_chan_spec const *chan, int val,
				 int val2, long mask)
{
	struct rtq6056_priv *priv = iio_priv(indio_dev);
	int ret;

	ret = iio_device_claim_direct_mode(indio_dev);
	if (ret)
		return ret;

	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		ret = rtq6056_adc_set_samp_freq(priv, chan, val);
		break;
	case IIO_CHAN_INFO_OVERSAMPLING_RATIO:
		ret = rtq6056_adc_set_average(priv, val);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	iio_device_release_direct_mode(indio_dev);

	return ret;
}

static int rtq6056_set_shunt_resistor(struct rtq6056_priv *priv,
				      int resistor_uohm)
{
	unsigned int calib_val;
	int ret;

	if (resistor_uohm <= 0) {
		dev_err(priv->dev, "Invalid resistor [%d]\n", resistor_uohm);
		return -EINVAL;
	}

	/* calibration = 5120000 / (Rshunt (uOhm) * current lsb (1mA)) */
	calib_val = 5120000 / resistor_uohm;
	ret = regmap_write(priv->regmap, RTQ6056_REG_CALIBRATION, calib_val);
	if (ret)
		return ret;

	priv->shunt_resistor_uohm = resistor_uohm;

	return 0;
}

static ssize_t shunt_resistor_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct rtq6056_priv *priv = iio_priv(dev_to_iio_dev(dev));
	int vals[2] = { priv->shunt_resistor_uohm, 1000000 };

	return iio_format_value(buf, IIO_VAL_FRACTIONAL, 1, vals);
}

static ssize_t shunt_resistor_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t len)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct rtq6056_priv *priv = iio_priv(indio_dev);
	int val, val_fract, ret;

	ret = iio_device_claim_direct_mode(indio_dev);
	if (ret)
		return ret;

	ret = iio_str_to_fixpoint(buf, 100000, &val, &val_fract);
	if (ret)
		goto out_store;

	ret = rtq6056_set_shunt_resistor(priv, val * 1000000 + val_fract);

out_store:
	iio_device_release_direct_mode(indio_dev);

	return ret ?: len;
}

static IIO_DEVICE_ATTR_RW(shunt_resistor, 0);

static struct attribute *rtq6056_attributes[] = {
	&iio_dev_attr_shunt_resistor.dev_attr.attr,
	NULL
};

static const struct attribute_group rtq6056_attribute_group = {
	.attrs = rtq6056_attributes,
};

static const struct iio_info rtq6056_info = {
	.attrs = &rtq6056_attribute_group,
	.read_raw = rtq6056_adc_read_raw,
	.read_avail = rtq6056_adc_read_avail,
	.write_raw = rtq6056_adc_write_raw,
};

static irqreturn_t rtq6056_buffer_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct rtq6056_priv *priv = iio_priv(indio_dev);
	struct device *dev = priv->dev;
	struct {
		u16 vals[RTQ6056_MAX_CHANNEL];
		s64 timestamp __aligned(8);
	} data;
	unsigned int raw;
	int i = 0, bit, ret;

	memset(&data, 0, sizeof(data));

	pm_runtime_get_sync(dev);

	for_each_set_bit(bit, indio_dev->active_scan_mask, indio_dev->masklength) {
		unsigned int addr = rtq6056_channels[bit].address;

		ret = regmap_read(priv->regmap, addr, &raw);
		if (ret)
			goto out;

		data.vals[i++] = raw;
	}

	iio_push_to_buffers_with_timestamp(indio_dev, &data, iio_get_time_ns(indio_dev));

out:
	pm_runtime_mark_last_busy(dev);
	pm_runtime_put(dev);

	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

static void rtq6056_enter_shutdown_state(void *dev)
{
	struct rtq6056_priv *priv = dev_get_drvdata(dev);

	/* Enter shutdown state */
	regmap_update_bits(priv->regmap, RTQ6056_REG_CONFIG,
			   RTQ6056_CONFIG_OPMODE_MASK,
			   RTQ6056_CONFIG_OPMODE(0));
}

static bool rtq6056_is_readable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case RTQ6056_REG_CONFIG ... RTQ6056_REG_ALERTLIMIT:
	case RTQ6056_REG_MANUFACTID ... RTQ6056_REG_DIEID:
		return true;
	default:
		return false;
	}
}

static bool rtq6056_is_writeable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case RTQ6056_REG_CONFIG:
	case RTQ6056_REG_CALIBRATION ... RTQ6056_REG_ALERTLIMIT:
		return true;
	default:
		return false;
	}
}

static const struct regmap_config rtq6056_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.val_format_endian = REGMAP_ENDIAN_BIG,
	.max_register = RTQ6056_REG_DIEID,
	.readable_reg = rtq6056_is_readable_reg,
	.writeable_reg = rtq6056_is_writeable_reg,
};

static int rtq6056_probe(struct i2c_client *i2c)
{
	struct iio_dev *indio_dev;
	struct rtq6056_priv *priv;
	struct device *dev = &i2c->dev;
	struct regmap *regmap;
	unsigned int vendor_id, shunt_resistor_uohm;
	int ret;

	if (!i2c_check_functionality(i2c->adapter, I2C_FUNC_SMBUS_WORD_DATA))
		return -EOPNOTSUPP;

	indio_dev = devm_iio_device_alloc(dev, sizeof(*priv));
	if (!indio_dev)
		return -ENOMEM;

	priv = iio_priv(indio_dev);
	priv->dev = dev;
	priv->vshuntct_us = priv->vbusct_us = 1037;
	priv->avg_sample = 1;
	i2c_set_clientdata(i2c, priv);

	regmap = devm_regmap_init_i2c(i2c, &rtq6056_regmap_config);
	if (IS_ERR(regmap)) {
		dev_err(dev, "Failed to init regmap\n");
		return PTR_ERR(regmap);
	}

	priv->regmap = regmap;

	ret = regmap_read(regmap, RTQ6056_REG_MANUFACTID, &vendor_id);
	if (ret) {
		dev_err(dev, "Failed to get manufacturer info: %d\n", ret);
		return ret;
	}

	if (vendor_id != RTQ6056_VENDOR_ID) {
		dev_err(dev, "Invalid vendor id 0x%04x\n", vendor_id);
		return -ENODEV;
	}

	/*
	 * By default, configure average sample as 1, bus and shunt conversion
	 * time as 1037 microsecond, and operating mode to all on.
	 */
	ret = regmap_write(regmap, RTQ6056_REG_CONFIG, RTQ6056_DEFAULT_CONFIG);
	if (ret) {
		dev_err(dev, "Failed to enable continuous sensing: %d\n", ret);
		return ret;
	}

	ret = devm_add_action_or_reset(dev, rtq6056_enter_shutdown_state, dev);
	if (ret)
		return ret;

	/* By default, use 2000 micro-Ohm resistor */
	shunt_resistor_uohm = 2000;
	device_property_read_u32(dev, "shunt-resistor-micro-ohms",
				 &shunt_resistor_uohm);

	ret = rtq6056_set_shunt_resistor(priv, shunt_resistor_uohm);
	if (ret) {
		dev_err(dev, "Failed to init shunt resistor: %d\n", ret);
		goto err;
	}

	indio_dev->name = "rtq6056";
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = rtq6056_channels;
	indio_dev->num_channels = ARRAY_SIZE(rtq6056_channels);
	indio_dev->info = &rtq6056_info;

	ret = iio_map_array_register(indio_dev, rtq6056_maps);
	if (ret) {
		dev_err(dev, "Failed to register iio map: %d\n", ret);
		goto err;
	}

	ret = iio_triggered_buffer_setup(indio_dev, NULL,
					 &rtq6056_buffer_trigger_handler, NULL);

	if (ret) {
		dev_err(dev, "Failed to allocate iio trigger buffer: %d\n",
			ret);
		goto err;
	}

	ret = devm_iio_device_register(dev, indio_dev);
	if (ret) {
		dev_err(dev, "Failed to register iio device: %d\n", ret);
		goto err;
	}

	return 0;

err:
	return ret;
}

static const struct of_device_id rtq6056_device_match[] = {
	{ .compatible = "richtek,rtq6056" },
	{}
};
MODULE_DEVICE_TABLE(of, rtq6056_device_match);

static struct i2c_driver rtq6056_driver = {
	.driver = {
		.name = "rtq6056",
		.of_match_table = rtq6056_device_match,
	},
	.probe_new = rtq6056_probe,
};
module_i2c_driver(rtq6056_driver);

MODULE_AUTHOR("ChiYuan Huang <cy_huang@richtek.com>");
MODULE_DESCRIPTION("Richtek RTQ6056 Driver");
MODULE_LICENSE("GPL v2");
