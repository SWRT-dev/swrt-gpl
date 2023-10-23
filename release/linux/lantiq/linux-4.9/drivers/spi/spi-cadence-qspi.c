/*
 * Driver for Cadence QSPI Controller
 *
 * Copyright Altera Corporation (C) 2012-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/clk.h>
#include "spi-cadence-qspi.h"
#include "spi-cadence-qspi-apb.h"

#define CADENCE_QSPI_NAME			"cadence-qspi"

unsigned int cadence_qspi_init_timeout(const unsigned long timeout_in_ms)
{
	return jiffies + msecs_to_jiffies(timeout_in_ms);
}
unsigned int cadence_qspi_check_timeout(const unsigned long timeout)
{
	return time_before(jiffies, timeout);
}
static irqreturn_t cadence_qspi_irq_handler(int this_irq, void *dev)
{
	struct struct_cqspi *cadence_qspi = dev;
	u32 irq_status;

	/* Read interrupt status
	 * We need to ignore those that comes with irq_status 0.
	 */
	irq_status = CQSPI_READ_IRQ_STATUS(cadence_qspi->iobase);
	if (!irq_status)
		return IRQ_HANDLED;

	cadence_qspi->irq_status = irq_status;

	/* Clear interrupt */
	CQSPI_CLEAR_IRQ(cadence_qspi->iobase, irq_status);
	wake_up(&cadence_qspi->waitqueue);
	return IRQ_HANDLED;
}
static void cadence_qspi_work(struct work_struct *work)
{
	struct struct_cqspi *cadence_qspi
		= container_of(work, struct struct_cqspi, work);
	unsigned long flags;

	pr_debug("%s\n", __func__);
	spin_lock_irqsave(&cadence_qspi->lock, flags);
	while ((!list_empty(&cadence_qspi->msg_queue)) &&
		cadence_qspi->running) {
		struct spi_message *spi_msg;
		struct spi_device *spi;
		struct spi_transfer *spi_xfer;
		struct spi_transfer *xfer[CQSPI_MAX_TRANS];
		int status = 0;
		int n_trans = 0;
		int skip_xfer = 0;
		spi_msg = container_of(cadence_qspi->msg_queue.next,
			struct spi_message, queue);
		list_del_init(&spi_msg->queue);
		spin_unlock_irqrestore(&cadence_qspi->lock, flags);
		spi = spi_msg->spi;
		list_for_each_entry(spi_xfer, &spi_msg->transfers,
				transfer_list) {
			if (n_trans >= CQSPI_MAX_TRANS) {
				pr_debug("[%s]n_trans=%d CQSPI_MAX_TRANS=%d\n",
					__func__, n_trans, CQSPI_MAX_TRANS);
				dev_err(&spi->dev, "ERROR: Number of SPI "
					"transfer is more than %d.\n",
					CQSPI_MAX_TRANS);
				/* Skip process the queue if number of
				 * transaction is greater than max 2. */
				skip_xfer = 1;
				break;
			}
			xfer[n_trans++] = spi_xfer;
		}
		pr_debug("[%s]n_trans=%d CQSPI_MAX_TRANS=%d\n",
			__func__, n_trans, CQSPI_MAX_TRANS);
		if (!skip_xfer) {
			status = cadence_qspi_apb_process_queue(cadence_qspi,
						spi, n_trans, xfer);
			if (!status) {
				spi_msg->actual_length += xfer[0]->len;
				if (n_trans > 1)
					spi_msg->actual_length += xfer[1]->len;
			}
			spi_msg->status = status;
			spi_msg->complete(spi_msg->context);
		}
		spin_lock_irqsave(&cadence_qspi->lock, flags);
	}
	spin_unlock_irqrestore(&cadence_qspi->lock, flags);
}
static int cadence_qspi_transfer(struct spi_device *spi,
	struct spi_message *msg)
{
	struct struct_cqspi *cadence_qspi =
		spi_master_get_devdata(spi->master);
	struct spi_transfer *spi_xfer;
	struct platform_device *pdev = cadence_qspi->pdev;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	unsigned long flags;

	pr_debug("%s\n", __func__);
	list_for_each_entry(spi_xfer, &msg->transfers, transfer_list) {
		if (spi_xfer->speed_hz > (pdata->master_ref_clk_hz / 2)) {
			dev_err(&spi->dev, "speed_hz%d greater than "
				"maximum %dHz\n",
				spi_xfer->speed_hz,
				(pdata->master_ref_clk_hz / 2));
			msg->status = -EINVAL;
			return -EINVAL;
		}
	}
	spin_lock_irqsave(&cadence_qspi->lock, flags);
	if (!cadence_qspi->running) {
		spin_unlock_irqrestore(&cadence_qspi->lock, flags);
		return -ESHUTDOWN;
	}
	msg->status = -EINPROGRESS;
	msg->actual_length = 0;
	list_add_tail(&msg->queue, &cadence_qspi->msg_queue);
	queue_work(cadence_qspi->workqueue, &cadence_qspi->work);
	spin_unlock_irqrestore(&cadence_qspi->lock, flags);
	return 0;
}
static int cadence_qspi_setup(struct spi_device *spi)
{
	pr_debug("%s\n", __func__);
	if (spi->chip_select > spi->master->num_chipselect) {
		dev_err(&spi->dev, "%d chip select is out of range\n",
			spi->chip_select);
		return -EINVAL;
	}
	pr_debug("cadence_qspi : bits per word %d, chip select %d, "
		"speed %d KHz\n", spi->bits_per_word, spi->chip_select,
		spi->max_speed_hz);
	return 0;
}
static int cadence_qspi_start_queue(struct struct_cqspi *cadence_qspi)
{
	unsigned long flags;

	pr_debug("%s\n", __func__);
	spin_lock_irqsave(&cadence_qspi->lock, flags);
	if (cadence_qspi->running) {
		spin_unlock_irqrestore(&cadence_qspi->lock, flags);
		return -EBUSY;
	}
	if (!cadence_qspi_apb_is_controller_ready(cadence_qspi->iobase)) {
		spin_unlock_irqrestore(&cadence_qspi->lock, flags);
		return -EBUSY;
	}
	cadence_qspi->running = true;
	spin_unlock_irqrestore(&cadence_qspi->lock, flags);
	queue_work(cadence_qspi->workqueue, &cadence_qspi->work);
	return 0;
}

#ifdef CONFIG_PM
static int cadence_qspi_stop_queue(struct struct_cqspi *cadence_qspi)
{
	unsigned long flags;
	unsigned limit = 500;
	int status = 0;

	spin_lock_irqsave(&cadence_qspi->lock, flags);
	cadence_qspi->running = false;
	/* We will wait until controller process all the queue and ensure the
	 * controller is not busy. */
	while ((!list_empty(&cadence_qspi->msg_queue) ||
		!cadence_qspi_apb_is_controller_ready(cadence_qspi->iobase))
		&& limit--) {
		spin_unlock_irqrestore(&cadence_qspi->lock, flags);
		msleep(20);
		spin_lock_irqsave(&cadence_qspi->lock, flags);
	}
	if (!list_empty(&cadence_qspi->msg_queue) ||
		!cadence_qspi_apb_is_controller_ready(cadence_qspi->iobase))
		status = -EBUSY;
	spin_unlock_irqrestore(&cadence_qspi->lock, flags);
	return status;
}
#endif

static int cadence_qspi_of_get_pdata(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device_node *nc;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	struct cqspi_flash_pdata *f_pdata;
	unsigned int cs;
	unsigned int prop;

	/* this function works with device tree information */
	if (of_property_read_u32(np, "lantiq,qspi-phyaddr", &prop)) {
		dev_err(&pdev->dev,
			"couldn't determine lantiq,qspi-phyaddr\n");
		return -ENXIO;
	}
	pdata->qspi_ahb_phy = prop;

	if (of_property_read_u32(np, "lantiq,qspi-physize", &prop)) {
		dev_info(&pdev->dev,
			 "missing lantiq,qspi-physize; default to 128 MB\n");
		prop = 128;
	}
	pdata->qspi_ahb_size = prop * 1024 * 1024; /* translate to MBytes */

	/* Some arch (e.g. twinhill) do not work with default mask 0xFFFFF */
	if (of_property_read_u32(np, "lantiq,qspi-phymask", &prop))
		prop = CQSPI_INDIRECTTRIGGER_ADDR_MASK;

	pdata->qspi_ahb_mask = prop;

	if (of_property_read_u32(np, "bus-num", &prop)) {
		dev_err(&pdev->dev, "couldn't determine bus-num\n");
		return -ENXIO;
	}
	pdata->bus_num = prop;
	if (of_property_read_u32(np, "num-chipselect", &prop)) {
		dev_err(&pdev->dev, "couldn't determine num-chipselect\n");
		return -ENXIO;
	}
	pdata->num_chipselect = prop;
	if (of_property_read_u32(np, "fifo-depth", &prop)) {
		dev_err(&pdev->dev, "couldn't determine fifo-depth\n");
		return -ENXIO;
	}
	pdata->fifo_depth = prop;

	if (of_property_read_u32(np, "ext-decoder", &prop)) {
		dev_err(&pdev->dev, "couldn't determine ext-decoder\n");
		return -ENXIO;
	}
	pdata->ext_decoder = prop;
	pdata->enable_dma = of_property_read_bool(np, "dmas");
	dev_info(&pdev->dev, "DMA %senabled\n",
		pdata->enable_dma ? "" : "NOT ");

	/* Get flash devices platform data */
	for_each_child_of_node(np, nc) {
		if (!of_device_is_available(nc))
			continue;

		if (of_property_read_u32(nc, "reg", &cs)) {
			dev_err(&pdev->dev, "couldn't determine reg\n");
			return -ENXIO;
		}
		f_pdata = &(pdata->f_pdata[cs]);

		/* use compatible string to determine flash type */
		if (of_device_is_compatible(nc, "lantiq,spinand"))
			f_pdata->flash_type = QSPI_FLASH_TYPE_NAND;
		else
			f_pdata->flash_type = QSPI_FLASH_TYPE_NOR;

		pr_debug("[%s]cs=%d flash-type=%d\n", __func__, cs,
			 f_pdata->flash_type);

		if (of_property_read_u32(nc, "spi-rx-bus-width", &prop)) {
			dev_err(&pdev->dev, "couldn't determine read quad\n");
			return -ENXIO;
		}
		pr_debug("[%s]cs=%d read quad=%d\n", __func__, cs, prop);
		if (prop == 4)
			f_pdata->rd_quad = QUAD_QIO;
		else if (prop == 2)
			f_pdata->rd_quad = QUAD_DIO;
		else
			f_pdata->rd_quad = QUAD_SIO;

		if (of_property_read_u32(nc, "spi-tx-bus-width", &prop)) {
			dev_err(&pdev->dev, "couldn't determine write quad\n");
			return -ENXIO;
		}
		pr_debug("[%s]cs=%d write quad=%d\n", __func__, cs, prop);
		if (prop == 4)
			f_pdata->wr_quad = QUAD_QIO;
		else if (prop == 2)
			f_pdata->wr_quad = QUAD_DIO;
		else
			f_pdata->wr_quad = QUAD_SIO;

		if (of_property_read_u32(nc, "page-size", &prop)) {
			dev_err(&pdev->dev, "couldn't determine page-size\n");
			return -ENXIO;
		}
		f_pdata->page_size = prop;
		if (of_property_read_u32(nc, "block-size", &prop)) {
			dev_err(&pdev->dev, "couldn't determine block-size\n");
			return -ENXIO;
		}
		f_pdata->block_size = prop;

		if (of_property_read_u32(nc, "read-delay", &prop)) {
			dev_err(&pdev->dev, "couldn't determine read-delay\n");
			return -ENXIO;
		}
		f_pdata->read_delay = prop;
		if (of_property_read_u32(nc, "tshsl-ns", &prop)) {
			dev_err(&pdev->dev, "couldn't determine tshsl-ns\n");
			return -ENXIO;
		}
		f_pdata->tshsl_ns = prop;
		if (of_property_read_u32(nc, "tsd2d-ns", &prop)) {
			dev_err(&pdev->dev, "couldn't determine tsd2d-ns\n");
			return -ENXIO;
		}
		f_pdata->tsd2d_ns = prop;
		if (of_property_read_u32(nc, "tchsh-ns", &prop)) {
			dev_err(&pdev->dev, "couldn't determine tchsh-ns\n");
			return -ENXIO;
		}
		f_pdata->tchsh_ns = prop;
		if (of_property_read_u32(nc, "tslch-ns", &prop)) {
			dev_err(&pdev->dev, "couldn't determine tslch-ns\n");
			return -ENXIO;
		}
		f_pdata->tslch_ns = prop;
	}
	return 0;
}

static int cadence_qspi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct struct_cqspi *cadence_qspi;
	struct resource *res;
	struct resource *res_ahb;
	struct cqspi_platform_data *pdata;
	int status;

	pr_debug("%s %s %s\n", __func__,
		pdev->name, pdev->id_entry->name);
	master = spi_alloc_master(&pdev->dev, sizeof(*cadence_qspi));
	if (master == NULL) {
		dev_err(&pdev->dev, "spi_alloc_master failed\n");
		return -ENOMEM;
	}
	master->mode_bits = SPI_CS_HIGH | SPI_CPOL | SPI_CPHA | SPI_TX_QUAD |
			    SPI_RX_QUAD | SPI_TX_DUAL | SPI_RX_DUAL;
	master->setup = cadence_qspi_setup;
	master->transfer = cadence_qspi_transfer;
	master->dev.of_node = pdev->dev.of_node;
	cadence_qspi = spi_master_get_devdata(master);
	cadence_qspi->pdev = pdev;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	cadence_qspi->iobase = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(cadence_qspi->iobase)) {
		dev_err(&pdev->dev, "devm_ioremap_resource res 0 failed\n");
		status = PTR_ERR(cadence_qspi->iobase);
		goto err_ioremap;
	}

	pdata = devm_kzalloc(&pdev->dev, sizeof(struct cqspi_platform_data),
			     GFP_KERNEL);
	if (!pdata) {
		status = -ENOMEM;
		goto err_ioremap;
	}
	pdev->dev.platform_data = pdata;
	cadence_qspi->clk = devm_clk_get(&pdev->dev, "qspi");
	if (IS_ERR(cadence_qspi->clk)) {
		dev_err(&pdev->dev, "cannot get qspi clk\n");
		status = PTR_ERR(cadence_qspi->clk);
		goto err_ioremap;
	}
	cadence_qspi->fpi_clk = devm_clk_get(&pdev->dev, "freq");
	if (IS_ERR(cadence_qspi->fpi_clk)) {
		dev_err(&pdev->dev, "cannot get qspi fpi_clk\n");
		status = PTR_ERR(cadence_qspi->fpi_clk);
		goto err_ioremap;
	}
	pdata->master_ref_clk_hz = clk_get_rate(cadence_qspi->fpi_clk);
	status = clk_prepare_enable(cadence_qspi->clk);
	if (status < 0) {
		dev_err(&pdev->dev,
			"failed to enable qspi clock: %d\n", status);
		goto err_ioremap;
	}

	status = clk_prepare_enable(cadence_qspi->fpi_clk);
	if (status < 0) {
		dev_err(&pdev->dev,
			"failed to enable fpi clock: %d\n", status);
		goto err_clk;
	}

	status = cadence_qspi_of_get_pdata(pdev);
	if (status) {
		dev_err(&pdev->dev, "Get platform data failed.\n");
		goto err_of;
	}

	cadence_qspi->res = res;

	/* request and remap ahb */
	res_ahb = devm_request_mem_region(&pdev->dev, pdata->qspi_ahb_phy,
					 pdata->qspi_ahb_size, "ahb");
	if (!res_ahb) {
		dev_err(&pdev->dev, "failed to request memory region\n");
		status = -EADDRNOTAVAIL;
		goto err_ahbremap;
	}

	cadence_qspi->qspi_ahb_virt = devm_ioremap(&pdev->dev,
						   pdata->qspi_ahb_phy,
						   pdata->qspi_ahb_size);
	if (!cadence_qspi->qspi_ahb_virt) {
		dev_err(&pdev->dev, "failed to remap memory region\n");
		status = -EADDRNOTAVAIL;
		goto err_ahbremap;
	}

	cadence_qspi->reset = devm_reset_control_get(&pdev->dev, "qspi");
	if (IS_ERR(cadence_qspi->reset)) {
		dev_err(&pdev->dev, "qspi get reset fail.\n");
		status = -EINVAL;
		goto err_ahbremap;
	}

	cadence_qspi->workqueue =
		create_singlethread_workqueue(dev_name(master->dev.parent));
	if (!cadence_qspi->workqueue) {
		dev_err(&pdev->dev, "create_workqueue failed\n");
		status = -ENOMEM;
		goto err_wq;
	}
	cadence_qspi->running = false;
	INIT_WORK(&cadence_qspi->work, cadence_qspi_work);
	spin_lock_init(&cadence_qspi->lock);
	INIT_LIST_HEAD(&cadence_qspi->msg_queue);
	init_waitqueue_head(&cadence_qspi->waitqueue);
	status = cadence_qspi_start_queue(cadence_qspi);
	if (status) {
		dev_err(&pdev->dev, "problem starting queue.\n");
		goto err_start_q;
	}
	cadence_qspi->irq = platform_get_irq(pdev, 0);
	if (cadence_qspi->irq < 0) {
		dev_err(&pdev->dev, "platform_get_irq failed\n");
		status = -ENXIO;
		goto err_irq;
	}
	status = request_irq(cadence_qspi->irq, cadence_qspi_irq_handler,
		0, pdev->name, cadence_qspi);
	if (status) {
		dev_err(&pdev->dev, "request_irq failed\n");
		goto err_irq;
	}
	master->bus_num = pdata->bus_num;
	master->num_chipselect = pdata->num_chipselect;
	platform_set_drvdata(pdev, master);
	cadence_qspi_apb_controller_init(cadence_qspi);
	cadence_qspi->current_cs = 0;
	status = spi_register_master(master);
	if (status) {
		dev_err(&pdev->dev, "spi_register_master=%d failed\n", status);
		goto err_of;
	}
	dev_info(&pdev->dev, "Cadence QSPI controller driver\n");
	return 0;

err_of:
	free_irq(cadence_qspi->irq, cadence_qspi);
err_start_q:
err_irq:
	destroy_workqueue(cadence_qspi->workqueue);
err_wq:
err_ahbremap:
	clk_disable_unprepare(cadence_qspi->fpi_clk);
err_clk:
	clk_disable_unprepare(cadence_qspi->clk);
err_ioremap:
	spi_master_put(master);
	dev_err(&pdev->dev, "Cadence QSPI controller probe failed\n");
	return status;
}

static int cadence_qspi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct struct_cqspi *cadence_qspi = spi_master_get_devdata(master);
	unsigned long flags;

	flush_workqueue(cadence_qspi->workqueue);
	spi_unregister_master(master);
	destroy_workqueue(cadence_qspi->workqueue);

	WARN(!list_empty(&cadence_qspi->msg_queue), "Cadence QSPI queue not empty at shutdown");

	spin_lock_irqsave(&cadence_qspi->lock, flags);
	cadence_qspi_apb_controller_disable(cadence_qspi->iobase);

	clk_disable_unprepare(cadence_qspi->fpi_clk);
	clk_disable_unprepare(cadence_qspi->clk);

	platform_set_drvdata(pdev, NULL);
	free_irq(cadence_qspi->irq, cadence_qspi);
	iounmap(cadence_qspi->iobase);
	iounmap(cadence_qspi->qspi_ahb_virt);
	release_mem_region(cadence_qspi->res->start,
		resource_size(cadence_qspi->res));
	kfree(pdev->dev.platform_data);
	spi_master_put(master);
	spin_unlock_irqrestore(&cadence_qspi->lock, flags);
	return 0;
}
#ifdef CONFIG_PM
static int cadence_qspi_suspend(struct device *dev)
{
	struct spi_master	*master = dev_get_drvdata(dev);
	struct struct_cqspi *cadence_qspi = spi_master_get_devdata(master);
	int status = 0;

	/* Stop the queue */
	status = cadence_qspi_stop_queue(cadence_qspi);
	if (status != 0)
		return status;
	/* Disable the controller to conserve the power */
	cadence_qspi_apb_controller_disable(cadence_qspi->iobase);
	return 0;
}
static int cadence_qspi_resume(struct device *dev)
{
	struct spi_master	*master = dev_get_drvdata(dev);
	struct struct_cqspi *cadence_qspi = spi_master_get_devdata(master);
	int status = 0;
	cadence_qspi_apb_controller_enable(cadence_qspi->iobase);
	/* Start the queue running */
	status = cadence_qspi_start_queue(cadence_qspi);
	if (status != 0) {
		cadence_qspi_apb_controller_disable(cadence_qspi->iobase);
		dev_err(dev, "problem starting queue (%d)\n", status);
		return status;
	}
	return 0;
}
static const struct dev_pm_ops cadence_qspi__dev_pm_ops = {
	.suspend	= cadence_qspi_suspend,
	.resume		= cadence_qspi_resume,
};
#define	CADENCE_QSPI_DEV_PM_OPS	(&cadence_qspi__dev_pm_ops)
#else
#define	CADENCE_QSPI_DEV_PM_OPS	NULL
#endif
#ifdef CONFIG_OF
static struct of_device_id cadence_qspi_of_match[] = {
	{ .compatible = "cadence,qspi",},
	{ /* end of table */}
};
MODULE_DEVICE_TABLE(of, cadence_qspi_of_match);
#else
#define cadence_qspi_of_match NULL
#endif /* CONFIG_OF */
static struct platform_driver cadence_qspi_platform_driver = {
	.probe		= cadence_qspi_probe,
	.remove		= cadence_qspi_remove,
	.driver = {
		.name	= CADENCE_QSPI_NAME,
		.owner	= THIS_MODULE,
		.pm	= CADENCE_QSPI_DEV_PM_OPS,
		.of_match_table = cadence_qspi_of_match,
	},
};
static int __init cadence_qspi_init(void)
{
	return platform_driver_register(&cadence_qspi_platform_driver);
}
static void __exit cadence_qspi_exit(void)
{
	platform_driver_unregister(&cadence_qspi_platform_driver);
}
module_init(cadence_qspi_init);
module_exit(cadence_qspi_exit);
MODULE_AUTHOR("Ley Foon Tan <lftan@altera.com>");
MODULE_DESCRIPTION("Cadence QSPI Controller Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" CADENCE_QSPI_NAME);
