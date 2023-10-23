/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <gsw_init.h>

/**
  * gsw_smdio_read - Convenience function for reading a register
  * from an MDIO on a given PHY.
  * @bus - points to the mii_bus structure
  * @phyadr - the phy address to write
  * @phyreg - the phy register offset from read
  * NOTE: MUST NOT be called from interrupt context
  */

int gsw_mdio_read(struct mii_bus *bus, int phyadr, int phyreg)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops = platform_get_drvdata(bus->priv);
	if (!ops) {
		dev_err(&bus->dev, "%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = phyadr;
	mmd_data.nAddressReg = phyreg;
	ops->gsw_common_ops.MDIO_DataRead(ops, &mmd_data);
	return (mmd_data.nData);
}


/**
  * gsw_smdio_write - Convenience function for writing to a register
  * from an MDIO on a given PHY.
  * @bus - points to the mii_bus structure
  * @phyadr - the phy address to write
  * @phyreg - the phy register offset to write
  * @phydata: value to write to @phyreg
  * NOTE: MUST NOT be called from interrupt context
  */

int gsw_mdio_write(struct mii_bus *bus, int phyadr, int phyreg,
		   u16 phydata)
{
	GSW_MDIO_data_t mmd_data;
	struct core_ops *ops = platform_get_drvdata(bus->priv);
	if (!ops) {
		dev_err(&bus->dev, "%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	memset((void *)&mmd_data, 0x00, sizeof(mmd_data));
	mmd_data.nAddressDev = phyadr;
	mmd_data.nAddressReg = phyreg;
	mmd_data.nData = phydata;
	ops->gsw_common_ops.MDIO_DataWrite(ops, &mmd_data);
	return 0;
}

/**
  * gsw_smdio_alloc - allocate a mii_bus structure
  * @pdev: Device to allocate mii_bus for.
  * Description: called by a GSWIP core driver to allocate an mii_bus
  * structure to fill in.
  * If an mii_bus allocated with this function needs
  *	to be freed separately,
  * mdiobus_free() must be used.
  * RETURNS:
  * Pointer to allocated mii_bus on success, NULL on failure.
  */

int gsw_mdio_alloc(struct platform_device *pdev)
{
	int err = 0;
	struct device_node *mdio;
	struct core_ops *ops = platform_get_drvdata(pdev);
	ethsw_api_dev_t *pdata = container_of(ops, ethsw_api_dev_t, ops);
	struct mii_bus *new_bus = NULL;

	mdio = of_get_child_by_name(pdev->dev.of_node, "gsw_mdio");
	if (!mdio) {
		dev_dbg(&pdev->dev, "GSWIP %d: gsw_mdio node not found\n",
			pdev->dev.parent->id);
		return -ENOLINK;
	}

	new_bus = devm_mdiobus_alloc(&pdev->dev);
	if (!new_bus) {
		dev_dbg(pdata->dev, "GSWIP %d: Unable to allocate mdio bus\n",
			pdev->dev.parent->id);
		return -ENOMEM;
	}

	new_bus->name = "gswitch_mdio";
	new_bus->read = gsw_mdio_read;
	new_bus->write = gsw_mdio_write;
	new_bus->reset = NULL;
	snprintf(new_bus->id, MII_BUS_ID_SIZE, "%x", pdev->dev.parent->id);
	new_bus->priv = pdev;
	new_bus->parent = pdata->dev;

	new_bus->phy_mask = 0xFFFFFFFF;
	err = of_mdiobus_register(new_bus, mdio);
	if (err != 0) {
		dev_err(pdata->dev, "%s: Cannot register as MDIO bus\n",
			new_bus->name);
		mdiobus_free(new_bus);
		return err;
	}
	pdata->master_mii_bus = new_bus;
	return err;
}

/**
  * gsw_smdio_free - mii_bus device release
  * @pdev: Device this mii_bus belongs to
  * Free mii_bus allocated with mdiobus_alloc
  */

void gsw_mdio_free(void *pdev)
{
	struct core_ops *ops = platform_get_drvdata(pdev);
	ethsw_api_dev_t *pdata = container_of(ops, ethsw_api_dev_t, ops);

	mdiobus_unregister(pdata->master_mii_bus);
	pdata->master_mii_bus->priv = NULL;
	pdata->master_mii_bus = NULL;

}
