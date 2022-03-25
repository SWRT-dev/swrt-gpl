/*
 * Lantiq GRX550 SoC Thermal driver
 *
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cpu_cooling.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/thermal.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>

#include  "thermal_core.h"

struct ltq_thermal_sensor;

struct ltq_thermal_sensor_ops {
	void (*init)(struct ltq_thermal_sensor *s);
	int (*get_temp)(struct ltq_thermal_sensor *s);
};

struct ltq_thermal_sensor {
	int id;

	struct thermal_zone_device *tzd;

	const struct ltq_thermal_sensor_ops *ops;

	int temp;
	int last_temp;
	int emul_temp;

	void *drvdata;
};

struct ltq_thermal {
	struct regmap	*chiptop;

	struct device *dev;

	struct ltq_thermal_sensor *sensors;
	int count;
};

#define CTRL_REG  0x100
#define DATA_REG  0x104

/* Temp sensor control register */

#define SOC_MASK		0x1
#define SOC_START		1
#define SOC_NC			0

#define AVG_SEL_MASK		0x6

#define TSOVH_INT_EN_MASK	0x8
#define TSOVH_INT_EN		0x8
#define TSOVH_INT_DIS		0

#define CH_SEL_MASK		0x700
#define CH_SEL_OFFSET		0x8

#define TS_EN_MASK		0x800
#define TS_EN_WORKING		0
#define TS_EN_SHUNT		0x800

#define INT_LVL			0xFFFF0000

/* Temp sensor data (read out) register */
#define	TS_CODE		0x00000FFF
#define	TS_DV		0x80000000

#define TS_REG_OFF			0x10U
#define INT_LVL_POS			16
#define PRX300_TS_CODE		0x3FFu
#define VSAMPLE_MASK		0x4
#define VSAMPLE_V			0x4
#define VSMPLE_NV			0x0
#define CTRL_REG_ID(__X__)	(CTRL_REG + ((__X__) * TS_REG_OFF))
#define DATA_REG_ID(__X__)	(DATA_REG + ((__X__) * TS_REG_OFF))
#define TIMEOUT				1000
#define ETEMPINVAL			-255

void ltq_prx300_init(struct ltq_thermal_sensor *sensor)
{
	struct ltq_thermal *priv = sensor->drvdata;

	regmap_write(priv->chiptop, CTRL_REG_ID(sensor->id), SOC_MASK);
}

static int ltq_prx300_get_temp(struct ltq_thermal_sensor *sensor)
{
	struct ltq_thermal *priv = sensor->drvdata;
	u32 val;
	int ts_val = ETEMPINVAL;
	int a0 = -49;
	int v1 = 10;
	int v2 = 1000;

	if (!priv)
		return ts_val;

	/* Select a channel */
	regmap_update_bits(priv->chiptop, CTRL_REG_ID(sensor->id),
			   SOC_MASK | VSAMPLE_MASK, SOC_NC | VSMPLE_NV);

	/* Converted data availability */
	if (regmap_read_poll_timeout(priv->chiptop, DATA_REG_ID(sensor->id),
				     val, (val & TS_DV) == 0, 0, TIMEOUT)) {
		dev_err(priv->dev, "Conversion data not available\n");
		return ts_val;
	}

	/* Starts the data conversion */
	regmap_update_bits(priv->chiptop, CTRL_REG_ID(sensor->id),
			   SOC_MASK, SOC_START);

	/* TS code availability */
	if (regmap_read_poll_timeout(priv->chiptop, DATA_REG_ID(sensor->id),
				     val, (val & TS_DV) > 0, 0, TIMEOUT)) {
		dev_dbg(priv->dev, "ts code read failed\n");
		return ts_val;
	}

	/* Read ts code */
	val = 0;
	regmap_read(priv->chiptop, DATA_REG_ID(sensor->id), &val);
	val &= PRX300_TS_CODE;

	/* Temperature interpolation */
	ts_val = (int)(a0 * 1000000 + (185858 * (val - v1)) +
		  (-92 * (val - v1) * (val - v2)) + 1);

	return (ts_val / 1000); /* returns °mC */
}

void  ltq_grx500_init(struct ltq_thermal_sensor *sensor)
{
	struct ltq_thermal *priv = sensor->drvdata;

	dev_dbg(priv->dev, "%s\n", __func__);

	/* Stop data conversion, disable overheat IRQ, power down sensor */
	regmap_update_bits(priv->chiptop, CTRL_REG,
			   SOC_MASK | TSOVH_INT_EN_MASK | TS_EN_MASK,
			   SOC_NC | TSOVH_INT_DIS | TS_EN_SHUNT);
	usleep_range(20, 50);  /* wait 1 TS_clk cycles to ensure TS is down */

	/* Power on the sensor */
	regmap_update_bits(priv->chiptop, CTRL_REG, TS_EN_MASK,
			   TS_EN_WORKING);
	usleep_range(160, 300);  /* required by TS spec */

	/* Start data conversion */
	regmap_update_bits(priv->chiptop, CTRL_REG, SOC_MASK, SOC_START);
	usleep_range(100, 200);  /* 5 TS_clk cycles before we put TS down */

	/* Stop data conversion and power down the sensor*/
	regmap_update_bits(priv->chiptop, CTRL_REG, SOC_MASK | TS_EN_MASK,
			   SOC_NC | TS_EN_SHUNT);
}

int ltq_grx500_get_temp(struct ltq_thermal_sensor *sensor)
{
	u32 reg;
	int T, v;
	int a0 = -40;
	int v1 = 3800;
	int v2 = 3421;
	int ret;
	struct ltq_thermal *priv = sensor->drvdata;

	if (!priv)
		return -EINVAL;

	/* Select a channel */
	regmap_update_bits(priv->chiptop, CTRL_REG, CH_SEL_MASK,
			   sensor->id << CH_SEL_OFFSET);

	/* Enable the temp sensor */
	regmap_update_bits(priv->chiptop, CTRL_REG, SOC_MASK | TS_EN_MASK,
			   SOC_START | TS_EN_WORKING);

	/* Wait for and read out the measurement */
	ret  = regmap_read_poll_timeout(priv->chiptop, DATA_REG, reg,
					(reg & TS_DV) > 0, 50, 2000);

	/* Disable the temp sensor */
	regmap_update_bits(priv->chiptop, CTRL_REG, SOC_MASK | TS_EN_MASK,
			   SOC_NC | TS_EN_SHUNT);

	if (ret)
		return -EIO;  /* temp read timeouted */

	v = reg & TS_CODE;

	/* Temperature interpolation */
	T = (int)(a0 * 1000000 + (-435356 * (v - v1)) +
		  (-360 * (v - v1) * (v - v2)) + 1);

	return T / 1000;  /* returns °mC */
}

/* Temperature sensor specific data */
static struct ltq_thermal_sensor_ops ltq_grx500_ops = {
	.init		= ltq_grx500_init,
	.get_temp	= ltq_grx500_get_temp,
};

static struct ltq_thermal_sensor_ops ltq_prx300_ops = {
	.init		 = ltq_prx300_init,
	.get_temp = ltq_prx300_get_temp,
};

static int ltq_thermal_get_temp(void *data, int *temp)
{
	struct ltq_thermal_sensor *sensor = data;

	if (!sensor)
		return -EINVAL;

	if (!sensor->emul_temp)
		*temp = sensor->ops->get_temp(sensor);
	else
		*temp = sensor->emul_temp;

	sensor->last_temp = sensor->temp;
	sensor->temp = *temp;

	return 0;
}

static int ltq_thermal_get_trend(void *data, int trip,
				 enum thermal_trend *trend)
{
	struct ltq_thermal_sensor *sensor = data;
	int trip_temp, trip_hyst, temp, last_temp;
	const struct thermal_trip *trips;

	if (!sensor || !sensor->tzd)
		return -EINVAL;

	trips = of_thermal_get_trip_points(sensor->tzd);
	if (!trips)
		return -EINVAL;

	if (!of_thermal_is_trip_valid(sensor->tzd, trip))
		return -EINVAL;

	temp = sensor->temp;
	last_temp = sensor->last_temp;

	trip_temp = trips[trip].temperature;
	trip_hyst = trips[trip].hysteresis;

	if (temp > last_temp)
		*trend = THERMAL_TREND_RAISING;
	else if (temp < last_temp && temp <= (trip_temp - trip_hyst))
		*trend = THERMAL_TREND_DROPPING;
/*
	else if (temp < last_temp && temp > (trip_temp - trip_hyst))
		*trend = THERMAL_TREND_DROPPING;
	else if (temp < last_temp && temp <= (trip_temp - trip_hyst))
		*trend = THERMAL_TREND_DROP_FULL;
*/		
	else
		*trend = THERMAL_TREND_STABLE;

	return 0;
}

static int ltq_thermal_set_emul_temp(void *data, int temp)
{
	struct ltq_thermal_sensor *sensor = data;

	if (!sensor || !sensor->tzd)
		return -EINVAL;

	sensor->emul_temp = temp;

	return 0;
}

static struct thermal_zone_of_device_ops ops = {
	.get_temp = ltq_thermal_get_temp,
	.get_trend = ltq_thermal_get_trend,
	.set_emul_temp = ltq_thermal_set_emul_temp,
};

static const struct of_device_id ltq_thermal_match[] = {
	{
		.compatible = "lantiq,ts-xrx500",
		.data       = &ltq_grx500_ops,
	},
	{
		.compatible = "intel,ts-prx300",
		.data       = &ltq_prx300_ops,
	},
	{ /* sentinel */ },
};

static int ltq_thermal_probe(struct platform_device *pdev)
{
	struct ltq_thermal *priv;
	struct device_node *node = pdev->dev.of_node;
	const struct ltq_thermal_sensor_ops *sens_ops =
		of_device_get_match_data(&pdev->dev);
	int i;

	dev_dbg(&pdev->dev, "%s\n", __func__);

	priv = devm_kzalloc(&pdev->dev, sizeof(struct ltq_thermal), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = &pdev->dev;
	platform_set_drvdata(pdev, priv);

	priv->chiptop = syscon_regmap_lookup_by_phandle(node, "intel,syscon");
	if (IS_ERR(priv->chiptop)) {
		dev_err(&pdev->dev, "Failed to find intel,syscon regmap\n");
		return PTR_ERR(priv->chiptop);
	}

	device_property_read_u32(&pdev->dev, "#thermal-sensor-cells",
				 &priv->count);
	priv->count++;

	priv->sensors = devm_kzalloc(&pdev->dev, priv->count *
				     sizeof(struct ltq_thermal_sensor),
				     GFP_KERNEL);
	if (!priv->sensors)
		return -ENOMEM;

	for (i = 0; i < priv->count; i++) {
		struct ltq_thermal_sensor *sensor = priv->sensors + i;

		sensor->id = i;
		sensor->drvdata = priv;
		sensor->ops = sens_ops;

		/* Init sensor */
		sensor->ops->init(sensor);

		/* Register sensor */
		sensor->tzd = devm_thermal_zone_of_sensor_register(&pdev->dev,
								   i,
								   sensor,
								   &ops);
		if (IS_ERR(sensor->tzd)) {
			dev_err(&pdev->dev,
				"Failed to register tzd for sensor id %d\n", i);
			return PTR_ERR(sensor->tzd);
		}
	}

	dev_dbg(&pdev->dev, "%s: tzd registered\n", __func__);

	return 0;
}

static int ltq_thermal_remove(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "%s:\n", __func__);
	return 0;
}

static struct platform_driver ltq_thermal_driver = {
	.probe = ltq_thermal_probe,
	.remove = ltq_thermal_remove,
	.driver = {
		.name = "ltq_thermal",
		.of_match_table = ltq_thermal_match,
	}
};

module_platform_driver(ltq_thermal_driver);

MODULE_DESCRIPTION("Lantiq SoC Thermal Driver");
MODULE_AUTHOR("Waldemar Rymarkiewicz <waldemarx.rymarkiewicz@intel.com>");
MODULE_LICENSE("GPL v2");
