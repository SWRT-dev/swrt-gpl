/*
 * tps65273.c - Voltage and current regulation for the TPS65273
 *
 * Copyright (C) 2015 Lantiq
 * Copyright (C) 2017 Intel
 *
 * Ho Nghia Duc <nghia.duc.ho@intel.com>
 * Waldemar Rymarkiewicz <waldeamrx.rymarkiewicz@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/slab.h>

/* Registers */
#define TPS65273_REG_VOUT1_SEL	0x00
#define TPS65273_REG_VOUT2_SEL	0x01
#define TPS65273_REG_VOUT1_COM	0x02
#define TPS65273_REG_VOUT2_COM	0x03
#define TPS65273_REG_SYS_STATUS	0x04

/* Bits */
#define TPS65273_VOUT_SEL_GO_BIT	BIT(7)
#define TPS65273_VOUT_SEL_GO_OFFSET	0x7
#define TPS65273_VOUT_COM_ENABLE_BIT	BIT(0)
#define TPS65273_VOUT_COM_OFFSET	0x0

#define TPS65273_VOUT_SEL_MASK		0x7F
#define TPS65273_VOUT_COM_ENABLE_MASK	0x1

#define TPS65273_REG_SYS_STATUS_PGOOD1		BIT(0)
#define TPS65273_REG_SYS_STATUS_PGOOD2		BIT(1)
#define TPS65273_REG_SYS_STATUS_TEMP_WARN	BIT(2)

#define TPS65273_NUM_REGULATOR	2
#define TPS65273_MIN_VOLTAGE	680000
#define TPS65273_MAX_VOLTAGE	1950000
#define TPS65273_STEP_VOLTAGE	10000

#define TPS65273_BUCK1_ID 1
#define TPS65273_BUCK2_ID 2

#define tps65273_reg_desc(num) {			\
	.name		= "BUCK"#num,			\
	.of_match	= NULL,				\
	.regulators_node = of_match_ptr("regulators"),	\
	.id		= TPS65273_BUCK##num##_ID,	\
	.ops		= &tps65273_ops,		\
	.type		= REGULATOR_VOLTAGE,		\
	.owner		= THIS_MODULE,			\
	.n_voltages	= ARRAY_SIZE(vout_vsel_table),	\
	.volt_table	= vout_vsel_table,		\
	.vsel_reg	= TPS65273_REG_VOUT##num##_SEL,	\
	.vsel_mask	= TPS65273_VOUT_SEL_MASK,	\
	.enable_reg	= TPS65273_REG_VOUT##num##_COM,	\
	.enable_mask	= TPS65273_VOUT_COM_ENABLE_MASK,\
	.enable_is_inverted = true,			\
	.apply_reg	= TPS65273_REG_VOUT##num##_SEL,	\
	.apply_bit	= TPS65273_VOUT_SEL_GO_BIT,	\
	.uV_step	= TPS65273_STEP_VOLTAGE		\
}

struct tps65273_regulator_init_data {
	struct device_node *of_node;
	struct regulator_init_data *init_data;
};

struct tps65273_platform_data {
	u32 slew_rate[TPS65273_NUM_REGULATOR];
	u32 psm_mode[TPS65273_NUM_REGULATOR];
	u32 vout_init_sel[TPS65273_NUM_REGULATOR];
	struct tps65273_regulator_init_data rdata[TPS65273_NUM_REGULATOR];
};

struct tps_driver_data {
	struct regulator_desc *desc;
};

struct tps65273 {
	struct tps65273_platform_data *pdata;
	struct tps_driver_data *driver_data;

	struct regmap *regmap;
	struct regulator_dev *rdev[TPS65273_NUM_REGULATOR];
};

static struct regulator_ops tps65273_ops = {
	.is_enabled	 = regulator_is_enabled_regmap,
	.enable		 = regulator_enable_regmap,
	.disable	 = regulator_disable_regmap,
	.list_voltage	 = regulator_list_voltage_table,
	.map_voltage	 = regulator_map_voltage_ascend,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
};

static const struct regmap_config tps65273_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8
};

static const unsigned int vout_vsel_table[] = {
	680000, 690000, 700000, 710000, 720000, 730000, 740000, 750000,
	760000, 770000, 780000, 790000, 800000, 810000, 820000, 830000,
	840000, 850000, 860000, 870000, 880000, 890000, 900000, 910000,
	920000, 930000, 940000, 950000, 960000, 970000, 980000, 990000,
	1000000, 1010000, 1020000, 1030000, 1040000, 1050000, 1060000, 1070000,
	1080000, 1090000, 1100000, 1110000, 1120000, 1130000, 1140000, 1150000,
	1160000, 1170000, 1180000, 1190000, 1200000, 1210000, 1220000, 1230000,
	1240000, 1250000, 1260000, 1270000, 1280000, 1290000, 1300000, 1310000,
	1320000, 1330000, 1340000, 1350000, 1360000, 1370000, 1380000, 1390000,
	1400000, 1410000, 1420000, 1430000, 1440000, 1450000, 1460000, 1470000,
	1480000, 1490000, 1500000, 1510000, 1520000, 1530000, 1540000, 1550000,
	1560000, 1570000, 1580000, 1590000, 1600000, 1610000, 1620000, 1630000,
	1640000, 1650000, 1660000, 1670000, 1680000, 1690000, 1700000, 1710000,
	1720000, 1730000, 1740000, 1750000, 1760000, 1770000, 1780000, 1790000,
	1800000, 1810000, 1820000, 1830000, 1840000, 1850000, 1860000, 1870000,
	1880000, 1890000, 1900000, 1910000, 1920000, 1930000, 1940000, 1950000,
};

static struct regulator_desc tps65273_regulators[] = {
	tps65273_reg_desc(1),
	tps65273_reg_desc(2)
};

static struct tps65273_platform_data *tps65273_parse_dt(struct device *dev)
{
	struct tps65273_platform_data *pd;
	struct device_node *np = dev->of_node;
	int i, ret;
	struct device_node *regulators;
	struct of_regulator_match *rmatch = NULL;
	unsigned int rnum = ARRAY_SIZE(tps65273_regulators);

	pd = devm_kzalloc(dev, sizeof(*pd), GFP_KERNEL);
	if (!pd) {
		dev_err(dev, "Failed to allocate platform data\n");
		return NULL;
	}

	regulators = of_find_node_by_name(np, "regulators");
	if (!regulators) {
		dev_err(dev, "regulator node not found\n");
		return ERR_PTR(-ENODEV);
	}

	rmatch = devm_kzalloc(dev, sizeof(*rmatch) * rnum, GFP_KERNEL);
	if (!rmatch)
		return ERR_PTR(-ENOMEM);

	for (i = 0; i < rnum; i++)
		rmatch[i].name = tps65273_regulators[i].name;

	ret = of_regulator_match(dev, regulators, rmatch, rnum);
	if (ret < 0) {
		dev_err(dev, "Error parsing regulator init data: %d\n", ret);
		return ERR_PTR(ret);
	}

	for (i = 0; i < rnum; i++) {
		struct regulator_init_data *init_data;
		struct device_node *of_node;

		init_data = rmatch[i].init_data;
		of_node = rmatch[i].of_node;

		if (!init_data || !of_node)
			continue;

		pd->rdata[i].init_data = init_data;
		pd->rdata[i].of_node = of_node;

		if (of_property_read_u32(of_node, "vout-slew-rate",
					 &pd->slew_rate[i])) {
			dev_warn(dev, "Slew rate not specified\n");
			pd->slew_rate[i] = 0;
		}

		if (of_property_read_u32(of_node, "vout-psm-mode",
					 &pd->psm_mode[i])) {
			dev_warn(dev, "PSM mode not specified\n");
			pd->psm_mode[i] = 0;
		}

		if (of_property_read_u32(of_node, "vout-init-selector",
					 &pd->vout_init_sel[i])) {
			dev_warn(dev, "VOUT init  not specified\n");
			pd->vout_init_sel[i] = 0;
		}

		init_data->constraints.valid_modes_mask = REGULATOR_MODE_NORMAL;
		init_data->constraints.apply_uV = 0;
	}

	return pd;
}

static struct tps_driver_data tps65273_drv_data = {
	.desc =  tps65273_regulators
};

static int tps65273_probe(struct i2c_client *client,
			  const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct regulator_config config = { };
	struct tps65273_platform_data *pdata = client->dev.platform_data;
	struct tps65273 *tps;
	int i;
	int ret = 0;

	dev_dbg(&client->dev, "TPS65273 probing ...\n");

	if (client->dev.of_node)
		pdata = tps65273_parse_dt(&client->dev);

	if (!pdata) {
		dev_err(&client->dev, "Require a platform data\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE))
		return -EIO;

	tps = devm_kzalloc(&client->dev, sizeof(*tps), GFP_KERNEL);
	if (!tps)
		return -ENOMEM;

	tps->pdata = pdata;
	tps->driver_data = (struct tps_driver_data *)id->driver_data;

	tps->regmap = devm_regmap_init_i2c(client, &tps65273_regmap_config);
	if (IS_ERR(tps->regmap)) {
		ret = PTR_ERR(tps->regmap);
		dev_err(&client->dev, "Failed to allocate register map: %d\n",
			ret);
		return ret;
	}

	config.dev = &client->dev;
	config.driver_data = tps;
	config.regmap = tps->regmap;

	for (i = 0; i < TPS65273_NUM_REGULATOR; i++) {
		if (!tps->pdata->rdata[i].init_data)
			continue;

		config.init_data = pdata->rdata[i].init_data;
		config.of_node = pdata->rdata[i].of_node;

		tps->rdev[i] = devm_regulator_register(&client->dev,
					&tps->driver_data->desc[i], &config);
		if (IS_ERR(tps->rdev[i])) {
			ret = PTR_ERR(tps->rdev[i]);
			dev_err(&client->dev, "regulator init failed (%d)\n",
				ret);

			return ret;
		}

/*
		tps65273_set_slew(tps, i, pdata->slew_rate[i]);
		tps65273_set_psm(tps, i, pdata->psm_mode[i]);
*/
		ret = regulator_set_voltage_sel_regmap(tps->rdev[i],
						       pdata->vout_init_sel[i]);
		if (ret < 0) {
			dev_err(&client->dev,
				"failed to set regulator init selector (%d)\n",
				ret);
			return ret;
		}
	}

	i2c_set_clientdata(client, tps);

	dev_dbg(&client->dev, "TPS65273 probed succesfully\n");

	return 0;
}

static int tps65273_remove(struct i2c_client *client)
{
	struct tps65273 *tps = i2c_get_clientdata(client);
	int i;

	for (i = 0; i < TPS65273_NUM_REGULATOR; i++) {
		struct regulator_dev *rdev = tps->rdev[i];
		regulator_unregister(rdev);
	}

	return 0;
}

static void tps65273_shutdown(struct i2c_client *client)
{
	struct tps65273 *tps = i2c_get_clientdata(client);
	int i, ret = 0;

	dev_dbg(&client->dev, "function %s is called\n", __func__);

	for (i = 0; i < TPS65273_NUM_REGULATOR; i++) {
		struct regulator_dev *rdev = tps->rdev[i];
		ret = regmap_update_bits(tps->regmap,
					TPS65273_REG_VOUT1_SEL + i,
					TPS65273_VOUT_SEL_GO_BIT, 0);
		if (ret < 0) {
			dev_err(&client->dev,
				"shutdown regulator failed: (%d)\n", ret);
			return ret;
		}
	}
}

static struct of_device_id tps65273_of_match[] = {
	{ .compatible	 = "ti,tps65273" },
	{},
};
MODULE_DEVICE_TABLE(of, tps65273_dt_match);

static const struct i2c_device_id tps65273_id[] = {
	{
		.name = "tps65273",
		.driver_data = (unsigned long) &tps65273_drv_data
	},
	{ },
};
MODULE_DEVICE_TABLE(i2c, tps65273_id);

static struct i2c_driver tps65273_i2c_driver = {
	.driver	= {
		.name	= "tps65273",
		.of_match_table	= of_match_ptr(tps65273_of_match),
	},
	.probe		= tps65273_probe,
	.shutdown	= tps65273_shutdown,
	.remove		= tps65273_remove,
	.id_table	= tps65273_id,
};

module_i2c_driver(tps65273_i2c_driver);

MODULE_DESCRIPTION("TPS65273 voltage regulator driver");
MODULE_AUTHOR("Ho Nghia Duc <nghia.duc.ho@intel.com>");
MODULE_AUTHOR("Waldemar Rymarkiewicz <waldemarx.rymarkiewicz@intel.com>");
MODULE_LICENSE("GPL v2");
