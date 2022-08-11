/* Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "pci.h"
#include "bus.h"
#include "debug.h"
#include <linux/of_address.h>

static enum cnss_dev_bus_type cnss_get_dev_bus_type(struct device *dev)
{
	if (!dev)
		return CNSS_BUS_NONE;

	if (!dev->bus)
		return CNSS_BUS_NONE;

	if (memcmp(dev->bus->name, "pci", 3) == 0)
		return CNSS_BUS_PCI;
	else if (memcmp(dev->bus->name, "platform", 8) == 0)
		return CNSS_BUS_AHB;
	else
		return CNSS_BUS_NONE;
}

static enum cnss_dev_bus_type cnss_get_bus_type(unsigned long device_id)
{
	switch (device_id) {
	case QCA6174_DEVICE_ID:
	case QCA6290_DEVICE_ID:
		return CNSS_BUS_PCI;
	case QCA8074_DEVICE_ID:
	case QCA8074V2_DEVICE_ID:
	case QCA6018_DEVICE_ID:
		return CNSS_BUS_AHB;
	default:
		pr_err("Unknown device_id: 0x%lx\n", device_id);
		return CNSS_BUS_NONE;
	}
}

int cnss_bus_alloc_qdss_mem(struct cnss_plat_data *plat_priv)
{
	int i;
	int bus_type;
	struct device_node *dev_node = NULL;
	struct resource q6_etr;
	int ret;

	if (!plat_priv)
		return -ENODEV;

	bus_type = cnss_get_bus_type(plat_priv->device_id);

	switch (bus_type) {
	case CNSS_BUS_PCI:
		return cnss_pci_alloc_qdss_mem(plat_priv->bus_priv);
	case CNSS_BUS_AHB:
		dev_node = of_find_node_by_name(NULL, "q6_etr_dump");
		if (!dev_node) {
			cnss_pr_err("No q6_etr_dump available in dts");
			return -ENOMEM;
		}

		ret = of_address_to_resource(dev_node, 0, &q6_etr);
		if (ret) {
			cnss_pr_err("Failed to get resource for q6_etr_dump");
			return -EINVAL;
		}

		for (i = 0; i < plat_priv->qdss_mem_seg_len; i++) {
			plat_priv->qdss_mem[i].va = NULL;
			plat_priv->qdss_mem[i].pa = q6_etr.start;
			plat_priv->qdss_mem[i].size = resource_size(&q6_etr);
			plat_priv->qdss_mem[i].type = QMI_WLFW_MEM_QDSS_V01;
			cnss_pr_dbg("QDSS mem addr 0x%x, size 0x%x",
				    plat_priv->qdss_mem[i].pa,
				    plat_priv->qdss_mem[i].size);
		}

		return 0;
	default:
		cnss_pr_err("Unsupported bus type:%d\n", bus_type);
		return -EINVAL;
	}
}

void cnss_bus_free_qdss_mem(struct cnss_plat_data *plat_priv)
{
	int bus_type;

	if (!plat_priv)
		return;

	bus_type = cnss_get_bus_type(plat_priv->device_id);

	switch (bus_type) {
	case CNSS_BUS_PCI:
		cnss_pci_free_qdss_mem(plat_priv->bus_priv);
		return;
	case CNSS_BUS_AHB:
		return;
	default:
		cnss_pr_err("Unsupported bus type %d\n", bus_type);
		return;
	}
}

void *cnss_bus_dev_to_bus_priv(struct device *dev)
{
	if (!dev)
		return NULL;

	switch (cnss_get_dev_bus_type(dev)) {
	case CNSS_BUS_PCI:
		return cnss_get_pci_priv(to_pci_dev(dev));
	case CNSS_BUS_AHB:
		return NULL;
	default:
		return NULL;
	}
}

struct cnss_plat_data *cnss_bus_dev_to_plat_priv(struct device *dev)
{
	void *bus_priv;
	struct pci_dev *pdev;

	if (!dev)
		return cnss_get_plat_priv(NULL);

	bus_priv = cnss_bus_dev_to_bus_priv(dev);
	if (cnss_get_dev_bus_type(dev) == CNSS_BUS_PCI) {
		pdev = to_pci_dev(dev);
		if (pdev->device != QCA6290_DEVICE_ID)
			return NULL;
	}

	switch (cnss_get_dev_bus_type(dev)) {
	case CNSS_BUS_PCI:
		if (bus_priv)
			return cnss_pci_priv_to_plat_priv(bus_priv);
		else
			return NULL;
	case CNSS_BUS_AHB:
		return cnss_get_plat_priv(to_platform_device(dev));
	default:
		return NULL;
	}
}
