/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017-2019 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/firmware.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>
#include "pp_qos_kernel.h"
#include "pp_qos_common.h"
#include "pp_qos_utils.h"
#include "pp_qos_fw.h"
#include "lantiq.h"

/*
 * TODO - this obstruct multi instances but helps debug
 * at this phase
 */
struct device *cur_dev;

#define PPV4_QOS_CMD_BUF_SIZE	(0x1000)
#define PPV4_QOS_CMD_BUF_OFFSET	(0x4000U)

void stop_run(void)
{
	struct pp_qos_drv_data *pdata;

	if (cur_dev) {
		dev_err(cur_dev, "QoS Assertion\n");
		pdata = dev_get_drvdata(cur_dev);
		qos_dbg_tree_show(pdata->qdev, NULL);
		QOS_BITS_SET(pdata->qdev->flags, PP_QOS_FLAGS_ASSERT);
	}
}

void resume_run(void)
{
	struct pp_qos_drv_data *pdata;

	if (cur_dev) {
		pdata = dev_get_drvdata(cur_dev);
		QOS_BITS_CLEAR(pdata->qdev->flags, PP_QOS_FLAGS_ASSERT);
	}
}

void wake_uc(void *data)
{
	struct pp_qos_drv_data *pdata;

	pdata = (struct pp_qos_drv_data *)data;
	iowrite32(4, pdata->wakeuc);
}

int load_firmware(struct pp_qos_dev *qdev, const char *name)
{
	int err;
	const struct firmware *firmware;
	struct ppv4_qos_fw fw;
	struct pp_qos_drv_data *pdata;
	struct device *dev;
	struct platform_device *pdev;

	pdev = (struct platform_device *)(qdev->pdev);
	dev = &pdev->dev;
	pdata = platform_get_drvdata(pdev);
	err = request_firmware(&firmware, name, dev);
	if (err < 0) {
		dev_err(dev, "Failed loading firmware ret is %d\n", err);
		return err;
	}

	fw.size = firmware->size;
	fw.data = firmware->data;
	err = do_load_firmware(qdev, &fw, pdata);

	release_firmware(firmware);
	return err;
}

static const struct of_device_id pp_qos_match[] = {
	{ .compatible = "intel,prx300-ppv4qos" },
	{},
};
MODULE_DEVICE_TABLE(of, pp_qos_match);

static struct resource *get_resource(
		struct platform_device *pdev,
		const char *name,
		unsigned int type,
		size_t size)
{
	struct resource *r;
	size_t sz;
	struct device *dev;

	dev = &pdev->dev;

	r = platform_get_resource_byname(pdev, type, name);
	if (!r) {
		dev_err(dev, "Could not get %s resource\n", name);
		return NULL;
	}

	sz = resource_size(r);
	if (size && sz != size) {
		dev_err(dev, "Illegal size %zu for %s resource expected %zu\n",
				sz, name, size);
		return NULL;
	}

	return r;
}

static void print_resource(
		struct device *dev,
		const char *name,
		struct resource *r)
{
	dev_dbg(dev, "%s memory resource: start(0x%08zX), size(%zu)\n",
			name,
			(size_t)(uintptr_t)r->start,
			(size_t)(uintptr_t)resource_size(r));
}

static void *map_mem_resource(
		struct platform_device *pdev,
		const char *name,
		unsigned int type,
		unsigned int size)
{
	struct resource *r;
	void *addr;
	struct device *dev;
	int err;

	dev = &pdev->dev;

	r = get_resource(pdev, name, type, size);
	if (!r)
		return NULL;

	print_resource(dev, name, r);
	addr = devm_memremap(dev, r->start, resource_size(r), MEMREMAP_WT);
	if (IS_ERR_OR_NULL(addr)) {
		err = (int) PTR_ERR(addr);
		dev_err(dev,
			"devm_memremap on resource %s failed with %d\n",
			name,
			err);
		return NULL;
	}
	dev_dbg(dev, "%s memory mapped to %p\n", name, addr);

	return addr;
}

static void __iomem *map_reg_resource(
		struct platform_device *pdev,
		const char *name,
		unsigned int type,
		unsigned int size)
{
	struct resource *r;
	void __iomem *addr;

	r = get_resource(pdev, name, type, size);
	if (!r)
		return NULL;

	print_resource(&pdev->dev, name, r);

	addr = devm_ioremap_resource(&pdev->dev, r);
	if (!addr)
		dev_err(&pdev->dev, "Map of resource %s failed\n", name);
	else
		dev_dbg(&pdev->dev, "%s register mapped to %p\n", name, addr);

	return addr;
}

static int pp_qos_get_resources(
		struct platform_device *pdev,
		struct qos_dev_init_info *info)
{
	int err;
	struct pp_qos_drv_data *pdata;
	struct device *dev;
	void __iomem  *regaddr;
	void *memaddr;

	dev = &pdev->dev;
	pdata = platform_get_drvdata(pdev);

	/* Cmd buffer and dccm */
	memaddr = map_mem_resource(pdev, "dccm", IORESOURCE_MEM, 0);
	if (!memaddr)
		return -ENODEV;
	pdata->dccm = memaddr;
	info->fwcom.cmdbuf = memaddr + PPV4_QOS_CMD_BUF_OFFSET;
	info->fwcom.cmdbuf_sz = PPV4_QOS_CMD_BUF_SIZE;

	memaddr = map_mem_resource(pdev, "ivt", IORESOURCE_MEM, 0);
	if (!memaddr)
		return -ENODEV;
	pdata->ivt = memaddr;

	/* Mbx from uc */
	regaddr = map_reg_resource(pdev, "mbx-from-uc", IORESOURCE_MEM, 24);
	if (!regaddr)
		return -ENODEV;
	info->fwcom.mbx_from_uc = regaddr;

	/* Mbx to uc */
	regaddr = map_reg_resource(pdev, "mbx-to-uc", IORESOURCE_MEM, 24);
	if (!regaddr)
		return -ENODEV;
	info->fwcom.mbx_to_uc = regaddr;

	regaddr = map_reg_resource(pdev, "wake-uc", IORESOURCE_MEM, 4);
	if (!regaddr)
		return -ENODEV;
	pdata->wakeuc = regaddr;

	err = platform_get_irq(pdev, 0);
	if (err < 0) {
		dev_err(dev,
			"Could not retrieve irqline from platform err is %d\n",
			err);
		return err;
	}
	info->fwcom.irqline = err;
	dev_dbg(dev, "irq is %d\n", err);

	return 0;
}

int allocate_ddr_for_qm(struct pp_qos_dev *qdev)
{
	void *addr;
	dma_addr_t dma;
	struct device *dev;
	unsigned int size;

	dev = &((struct platform_device *)qdev->pdev)->dev;
	size = qdev->hwconf.wred_total_avail_resources * PPV4_QOS_DESC_SIZE;

	addr = dmam_alloc_coherent(dev, size, &dma, GFP_KERNEL);
	if (addr == NULL) {
		dev_err(dev,
			"Could not allocate %u bytes for queue manager\n",
			size);
		return -ENOMEM;
	}

	qdev->hwconf.qm_ddr_start_virt = addr;
	qdev->hwconf.qm_ddr_start = (unsigned int)(dma);
	qdev->hwconf.qm_num_pages =
		qdev->hwconf.wred_total_avail_resources / PPV4_QOS_DESC_IN_PAGE;

	return 0;
}

void free_ddr_for_qm(struct pp_qos_dev *qdev)
{
	struct device *dev;
	unsigned int size;

	dev = &((struct platform_device *)qdev->pdev)->dev;
	size = qdev->hwconf.wred_total_avail_resources * PPV4_QOS_DESC_SIZE;

	dma_free_coherent(dev, size, qdev->hwconf.qm_ddr_start_virt,
			  qdev->hwconf.qm_ddr_start);
}

static int pp_qos_config_from_of_node(
		struct platform_device *pdev,
		struct ppv4_qos_platform_data *pdata,
		struct pp_qos_drv_data *pdrvdata)
{
	int err;
	uint32_t val;
	struct device_node *node;
	struct device *dev;
	void *addr;
	dma_addr_t dma;
	struct property *prop;
	const __be32 *p;

	dev = &pdev->dev;
	node = pdev->dev.of_node;
	dev_dbg(&pdev->dev, "Using device tree info to init platform data\n");
	err = of_alias_get_id(node, "qos");
	if (err < 0) {
		dev_err(dev, "failed to get alias id, errno %d\n", err);
		return err;
	}
	pdata->id = err;

	err = of_property_read_u32(node, "intel,qos-max-port", &val);
	if (err) {
		dev_err(dev,
			"Could not get max port from DT, error is %d\n",
			err);
		return -ENODEV;
	}
	pdata->max_port = val;

	err = of_property_read_u32(node, "intel,wred-prioritize", &val);
	if (err) {
		dev_err(dev,
			"Could not get wred pop prioritize from DT, error is %d\n",
			err);
		return -ENODEV;
	}
	pdata->wred_prioritize_pop = val;

	err = of_property_read_u32(node, "intel,clock-frequency-mhz", &val);
	if (err) {
		dev_err(dev,
			"Could not get qos clock from DT, error is %d\n",
			err);
		return -ENODEV;
	}
	pdata->qos_clock = val;

	if (of_property_count_u32_elems(node, "intel,fw-sec-data-stack") != 3) {
		dev_err(dev, "intel,fw-sec-data-stack num fields must be 3\n");
		return -ENODEV;
	}

	prop = of_find_property(node, "intel,fw-sec-data-stack", NULL);
	p = of_prop_next_u32(prop, NULL,
			     &pdrvdata->fw_sec_data_stack.is_in_dccm);
	p = of_prop_next_u32(prop, p, &pdrvdata->fw_sec_data_stack.dccm_offset);
	p = of_prop_next_u32(prop, p, &pdrvdata->fw_sec_data_stack.max_size);

	/* dccm is not available for A Step */
	if (ltq_get_soc_rev() == 0)
		pdrvdata->fw_sec_data_stack.is_in_dccm = 0;

	addr = dmam_alloc_coherent(
			dev,
			PPV4_QOS_LOGGER_BUF_SIZE + PPV4_QOS_STAT_SIZE,
			&dma,
			GFP_KERNEL);
	if (addr == NULL) {
		dev_err(dev, "Could not allocate %u bytes for logger buffer\n",
			PPV4_QOS_LOGGER_BUF_SIZE);
		return -ENOMEM;
	}
	pdata->fw_logger_start = (unsigned int)(dma);
	pdata->fw_stat = pdata->fw_logger_start + PPV4_QOS_LOGGER_BUF_SIZE;
	pdrvdata->dbg.fw_logger_addr = addr;

	dev_dbg(dev, "Dma allocated %u bytes for fw logger, bus address is 0x%08X, virtual addr is %p\n",
		 PPV4_QOS_LOGGER_BUF_SIZE,
		 pdata->fw_logger_start,
		 pdrvdata->dbg.fw_logger_addr);

	pdata->max_allowed_sz = PAGE_ALIGN(NUM_OF_QUEUES * sizeof(u32));
	pdata->max_allowed_ddr_virt = dmam_alloc_coherent(
			dev,
			pdata->max_allowed_sz,
			&pdata->max_allowed_ddr_phys,
			GFP_KERNEL | __GFP_ZERO);

	if (pdata->max_allowed_ddr_virt == NULL) {
		dev_err(dev, "Couldn't alloc %u bytes for max allowed buffer\n",
			pdata->max_allowed_sz);
		return -ENOMEM;
	}

	dev_dbg(dev, "Dma allocated %u bytes for max allowed buffer, bus address is 0x%08X, virtual addr is %p\n",
		 pdata->max_allowed_sz,
		 pdata->max_allowed_ddr_phys,
		 pdata->max_allowed_ddr_virt);

	pdata->bwl_sz = PAGE_ALIGN(NUM_OF_NODES * sizeof(u32));
	pdata->bwl_ddr_virt = dmam_alloc_coherent(
			dev,
			pdata->bwl_sz,
			&pdata->bwl_ddr_phys,
			GFP_KERNEL | __GFP_ZERO);

	if (pdata->bwl_ddr_virt == NULL) {
		dev_err(dev, "Couldn't alloc %u bytes for bwl buffer\n",
			pdata->bwl_sz);
		return -ENOMEM;
	}

	dev_dbg(dev, "Dma allocated %u bytes for bwl buffer, bus address is 0x%08X, virtual addr is %p\n",
		pdata->bwl_sz,
		pdata->bwl_ddr_phys,
		pdata->bwl_ddr_virt);

	pdata->sbwl_sz = PAGE_ALIGN(NUM_OF_NODES * sizeof(u32));
	pdata->sbwl_ddr_virt = dmam_alloc_coherent(
			dev,
			pdata->sbwl_sz,
			&pdata->sbwl_ddr_phys,
			GFP_KERNEL | __GFP_ZERO);

	if (pdata->sbwl_ddr_virt == NULL) {
		dev_err(dev, "Couldn't alloc %u bytes for sbwl buffer\n",
			pdata->sbwl_sz);
		return -ENOMEM;
	}

	dev_dbg(dev, "Dma allocated %u bytes for sbwl buffer, bus address is 0x%08X, virtual addr is %p\n",
		pdata->sbwl_sz,
		pdata->sbwl_ddr_phys,
		pdata->sbwl_ddr_virt);

	return 0;
}

static int pp_qos_config_from_platform_data(
		struct platform_device *pdev,
		struct ppv4_qos_platform_data *pdata,
		struct pp_qos_drv_data *pdrvdata
		)
{
	struct ppv4_qos_platform_data *psrc;
	void *memaddr;

	dev_dbg(&pdev->dev, "Using platform info to init platform data\n");
	psrc = (struct ppv4_qos_platform_data *)dev_get_platdata(&pdev->dev);
	if (!psrc) {
		dev_err(&pdev->dev, "Device contain no platform data\n");
		return -ENODEV;
	}
	pdata->id = psrc->id;
	pdata->max_port = psrc->max_port;
	pdata->wred_prioritize_pop = psrc->wred_prioritize_pop;

	/* DDR */
	pdrvdata->ddr_phy_start = 0;
	memaddr = map_mem_resource(pdev, "ddr", IORESOURCE_MEM, 0);
	if (!memaddr)
		return -ENODEV;
	pdrvdata->ddr = memaddr;

	pdata->qm_ddr_start = psrc->qm_ddr_start;
	pdata->qm_ddr_start_virt = psrc->qm_ddr_start_virt;
	pdata->qm_num_pages = psrc->qm_num_pages;
	pdata->fw_logger_start = psrc->fw_logger_start;
	pdata->fw_stat = pdata->fw_logger_start + PPV4_QOS_LOGGER_BUF_SIZE;
	pdrvdata->dbg.fw_logger_addr = pdrvdata->ddr + pdata->fw_logger_start;
	return 0;
}

static int pp_qos_probe(struct platform_device *pdev)
{
	const struct device_node *of_node = pdev->dev.of_node;
	struct qos_dev_init_info init_info = {};
	const struct of_device_id *match;
	struct pp_qos_dev *qdev;
	int err;
	struct pp_qos_drv_data *pdata;
	struct device *dev;

	dev = &pdev->dev;
	dev_dbg(dev, "Probing...\n");

	/* there is no devm_vmalloc so using dev_kzalloc */
	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	platform_set_drvdata(pdev, pdata);

	match = of_match_device(of_match_ptr(pp_qos_match), dev);
	if (match && of_node) {
		err = pp_qos_config_from_of_node(
				pdev,
				&init_info.pl_data,
				pdata);
		if (err)
			return err;
	} else if (dev_get_platdata(dev)) {
		err = pp_qos_config_from_platform_data(
				pdev,
				&init_info.pl_data,
				pdata);
		if (err)
			return err;
	} else {
		dev_err(dev, "Could not get qos configuration\n");
		return -ENODEV;
	}

	dev_dbg(dev, "id(%d), max_port(%u), pop_prioritize(%u), clock(%u)\n",
			init_info.pl_data.id,
			init_info.pl_data.max_port,
			init_info.pl_data.wred_prioritize_pop,
			init_info.pl_data.qos_clock);

	pdata->id = init_info.pl_data.id;
	err = pp_qos_get_resources(pdev, &init_info);
	if (err)
		return err;

	qdev = create_qos_dev_desc(&init_info);
	if (!qdev)
		return -ENODEV;
	qdev->pdev = pdev;
	pdata->qdev = qdev;
	qdev->stat = pdata->dbg.fw_logger_addr + PPV4_QOS_LOGGER_BUF_SIZE;

	err = qos_dbg_dev_init(pdev);
	if (err)
		goto fail;

	dev_dbg(dev, "Probe completed\n");
	cur_dev = dev;
	return 0;

fail:
	_qos_clean(qdev);
	return err;
}

static int pp_qos_remove(struct platform_device *pdev)
{
	struct pp_qos_drv_data *pdata;

	dev_err(&pdev->dev, "Removing...\n");
	pdata = platform_get_drvdata(pdev);
	remove_qos_instance(pdata->id);
	qos_dbg_dev_clean(pdev);
	/* TODO !!! How to cleanup? */
	return 0;
}

static struct platform_driver pp_qos_driver = {
	.probe = pp_qos_probe,
	.remove = pp_qos_remove,
	.driver = {
		.name = "pp-qos",
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = pp_qos_match,
#endif
	},
};

static int __init pp_qos_mod_init(void)
{
	int rc;

	qos_module_init();
	qos_dbg_module_init();
	rc = platform_driver_register(&pp_qos_driver);
	PPV4_QOS_MANUAL_ADD(rc);

	return rc;
}

static void __exit pp_qos_mod_exit(void)
{
	PPV4_QOS_MANUAL_REMOVE();
	platform_driver_unregister(&pp_qos_driver);

	qos_dbg_module_clean();
}

//device_initcall(pp_qos_mod_init);
// TODO ask Thomas
arch_initcall(pp_qos_mod_init);
module_exit(pp_qos_mod_exit);

MODULE_VERSION(PPV4_QOS_DRV_VER);
MODULE_AUTHOR("Intel CHD");
MODULE_DESCRIPTION("PPv4 QoS driver");
MODULE_LICENSE("GPL v2");
MODULE_FIRMWARE(FIRMWARE_FILE);
