// SPDX-License-Identifier: GPL-2.0
/*
 * MDIO I2C bridge
 *
 * Copyright (C) 2015 Russell King
 */
#ifndef MDIO_I2C_H
#define MDIO_I2C_H

struct device;
struct i2c_adapter;
struct mii_bus;

enum mdio_i2c_proto {
	MDIO_I2C_NONE,
	MDIO_I2C_MARVELL_C22,
	MDIO_I2C_C45,
	MDIO_I2C_ROLLBALL,
};

static inline const char *mdio_i2c_proto_type(int type)
{
	switch (type) {
	case MDIO_I2C_NONE:
		return "MDIO_I2C_NONE";
	case MDIO_I2C_MARVELL_C22:
		return "MDIO_I2C_MARVELL_C22";
	case MDIO_I2C_C45:
		return "MDIO_I2C_C45";
	case MDIO_I2C_ROLLBALL:
		return "MDIO_I2C_ROLLBALL";
	default:
		return "UNKNOWN";
	}
}

struct mii_bus *mdio_i2c_alloc(struct device *parent, struct i2c_adapter *i2c,
			       enum mdio_i2c_proto protocol);

#endif
