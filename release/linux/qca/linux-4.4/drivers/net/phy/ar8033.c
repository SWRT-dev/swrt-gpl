/*
 * Driver for QCA ar8033 PHY
 *
 * Author: Heiko Schocher <hs@denx.de>
 *
 * Copyright (c) 2011 DENX Software Engineering GmbH
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/phy.h>

#define PHY_ID_AR8033		0x004DD074

#define MII_AR8033_PHYSTATUS	0x11		/* PHY specific status register */
#define MII_AR8033_IMASK	0x12		/* interrupt enable register */
#define MII_AR8033_IMASK_INIT	0x0c00
#define MII_AR8033_ISTATUS	0x13		/* interrupt status register */

MODULE_DESCRIPTION("QCA AR8033 PHY driver");
MODULE_AUTHOR("Heiko Schocher <hs@denx.de>");
MODULE_LICENSE("GPL");

static int ar8033_ack_interrupt(struct phy_device *phydev)
{
	int err, phy_status;

	err = phy_read(phydev, MII_AR8033_ISTATUS);
	phy_status = phy_read(phydev, MII_AR8033_PHYSTATUS);
	if (err < 0)
		return err;
	if (phy_status < 0)
		return err;

	return 0;
}

static int ar8033_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		err = phy_write(phydev, MII_AR8033_IMASK, MII_AR8033_IMASK_INIT);
	} else {
		err = phy_write(phydev, MII_AR8033_IMASK, 0);
	}

	return err;
}

static int ar8033_did_interrupt(struct phy_device *phydev)
{
      int imask;

      imask = phy_read(phydev, MII_AR8033_ISTATUS);

      if (imask & MII_AR8033_IMASK_INIT) {
              printk(KERN_ERR "### %d,%x ", phydev->addr, imask);
              return 1;
      }

      printk(KERN_ERR "### %d,0 ", phydev->addr);
      return 0;
}

static int ar8033_config_init(struct phy_device *phydev)
{
	struct mii_bus *bus = phydev->bus;
	int addr = phydev->addr;

	phydev->supported |= SUPPORTED_1000baseT_Full;
	phydev->advertising |= ADVERTISED_1000baseT_Full;

	mutex_lock(&bus->mdio_lock);
	/* start autoneg, ref. to qca_ar8327_hw_init() */
	bus->write(bus, addr, MII_ADVERTISE, ADVERTISE_ALL | ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM);
	bus->write(bus, addr, MII_CTRL1000, ADVERTISE_1000FULL);
	bus->write(bus, addr, MII_BMCR, BMCR_RESET);
	bus->write(bus, addr, MII_BMCR, BMCR_ANENABLE | bus->read(bus, addr, MII_BMCR));

	mutex_unlock(&bus->mdio_lock);

	return 0;
}

static int ar8033_probe(struct phy_device *phydev)
{
	return 0;
}

static struct phy_driver ar8033_driver = {
	.name		= "QCA AR8033",
	.phy_id		= 0x004d0000, //PHY_ID_AR8033,
	.phy_id_mask	= 0xffff0000,
	.probe		= ar8033_probe,
	.config_init	= ar8033_config_init,
	.config_aneg	= genphy_config_aneg,
	.read_status	= genphy_read_status,
	.ack_interrupt	= ar8033_ack_interrupt,
	.config_intr	= ar8033_config_intr,
	.did_interrupt	= ar8033_did_interrupt,
	.features	= PHY_BASIC_FEATURES,
	.flags		= PHY_HAS_INTERRUPT,
	.driver		= { .owner = THIS_MODULE,},
};

static int __init ar8033_init(void)
{
	int ret;

	ret = phy_driver_register(&ar8033_driver);
	if (ret)
		return ret;

	return 0;
}

static void __exit ar8033_exit(void)
{
	phy_driver_unregister(&ar8033_driver);
}

module_init(ar8033_init);
module_exit(ar8033_exit);

static struct mdio_device_id __maybe_unused ar8033_tbl[] = {
	{ PHY_ID_AR8033, 0xffff0000 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, ar8033_tbl);
