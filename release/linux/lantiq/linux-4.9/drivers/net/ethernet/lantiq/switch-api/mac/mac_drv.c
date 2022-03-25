/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/adap_ops.h>
#include <gswss_api.h>
#include <xgmac_common.h>
#include <xgmac.h>
#include <lmac_api.h>
#include <linux/clk.h>

static void __iomem *base[2];

static irqreturn_t mac_isr(int irq, void *dev_id)
{
	u32 devid = 0;
	struct adap_ops *adap_ops = gsw_get_adap_ops(devid);
	struct mac_ops *mac_ops;
	struct mac_prv_data *pdata;
	int i = 0, mac_int_sts = 0, lmac_int_sts = 0;
	u32 max_mac = gsw_get_mac_subifcnt(devid);
	int task_sched = 0;

	/* Handle all the MAC Interrupts */
	for (i = 0; i < max_mac; i++) {
		task_sched = 0;

		mac_int_sts = gswss_get_int_stat(adap_ops, XGMAC);

		mac_ops = gsw_get_mac_ops(devid, (i + MAC_2));

		if (!mac_ops)
			return IRQ_HANDLED;

		pdata = GET_MAC_PDATA(mac_ops);

		/* Check XGMAC i Interrupts */
		if (mac_int_sts & (1 << (GSWIPSS_IER0_XGMAC2_POS + i))) {
			/* Disable the XGMAC 2/3/4 Interrupt First */
			gswss_set_interrupt(adap_ops, XGMAC,
					    pdata->mac_idx, 0);

			task_sched = 1;
		}

		lmac_int_sts = lmac_get_int(mac_ops);

		/* Check LMAC i Interrupts */
		if (lmac_int_sts & (1 << (LMAC_ISR_MAC2_POS + i))) {
			/* Disable the LMAC 2/3/4 Interrupt First */
			lmac_set_int(mac_ops, 0);

			task_sched = 1;
		}

		/* Schedule tasklet for this MAC */
		if (task_sched)
			tasklet_schedule(&pdata->mac_tasklet);
	}

	return IRQ_HANDLED;
}

static void mac_irq_tasklet(unsigned long data)
{
	int evnt = 0, j = 0;
	void *param;
	struct mac_prv_data *pdata = (struct mac_prv_data *)data;
	struct mac_ops *ops = &pdata->ops;

	/* Get all events available for this idx XGMAC and LMAC events */
	evnt = ops->get_int_sts(ops);

	if (evnt) {
		/* Check all events for this MAC */
		for (j = 0; j < MAX_IRQ_EVNT; j++) {
			if (evnt & (1 << j)) {
				/* Clear the MAC Interrupt status */
				ops->clr_int_sts(ops, j);

				/* If callback is registered, return callback */
				if (pdata->irq_hdl[j].cb) {
					param = pdata->irq_hdl[j].param;
					/* Call the Upper level callback */
					pdata->irq_hdl[j].cb(param);
				}
			}
		}
	}
	/* Enable Corresponding XGMAC and LMAC Interrupts back */
	ops->mac_int_en(ops);
}

static int mac_irq_init(struct mac_prv_data *pdata)
{
	int ret = 0;

	pdata->irq_hdl = kzalloc(sizeof(*pdata->irq_hdl) * (MAX_IRQ_EVNT - 1),
				 GFP_KERNEL);

	if (!pdata->irq_hdl) {
		mac_printf("IRQ handler alloc error\n");
		return -1;
	}

	if (pdata->mac_idx == MAC_2) {
		ret = request_irq(pdata->irq_num, mac_isr, 0, "gsw_mac", NULL);

		if (ret) {
			mac_printf("request_irq Error for MAC - %d", ret);
			return ret;
		}
	}

	tasklet_init(&pdata->mac_tasklet,
		     mac_irq_tasklet,
		     (unsigned long)pdata);

	return ret;
}

static int mac_probe(struct platform_device *pdev)
{
	int i = 0;
	struct resource *res[2];
	struct gswss *gswdev = dev_get_drvdata(pdev->dev.parent);
	struct adap_prv_data *adap_pdata = GET_ADAP_PDATA(gswdev->adap_ops);
	struct mac_prv_data *pdata =
		GET_MAC_PDATA(platform_get_drvdata(pdev));
	struct device *dev = &pdev->dev;
	int linksts = 0, duplex = 0, speed = 0;
	char *load, *ls, *dp;
	int ret;

	gswdev->mac_dev[pdev->id] = pdev;

	/* Initialize MAC Spin lock */
	spin_lock_init(&pdata->mac_lock);

	/* Initialize Spin lock for Indirect read/write */
	spin_lock_init(&pdata->rw_lock);

	/* load the memory ranges */
	for (i = 0; i < pdev->num_resources; i++) {
		res[i] = platform_get_resource(pdev, IORESOURCE_MEM, i);

		if (!res[i]) {
			pr_info("failed to get resources %d\n", i);
			return -1;
		}

		if (pdata->mac_idx == MAC_2)
			base[i] = devm_ioremap_resource(&pdev->dev, res[i]);
	}

	/* Update Adaption layer pointer and Lmac base address per Mac private
	 * data.This is needed since all MAC api's have Private data as argument
	 */
	pdata->ss_addr_base = base[0];
	adap_pdata->ss_addr_base = base[0];
	pdata->lmac_addr_base = base[1];

	pdata->max_mac = gsw_get_mac_subifcnt(0);

	if (device_property_present(dev, "board_type")) {
		pdata->haps = 1;
	} else {
		pdata->haps = 0;
	}

	ret = of_property_read_u32(dev->of_node, "clock-frequency",
				   &pdata->ptp_clk);
	if (ret < 0) {
		pdata->ptp_clk = 500000000;
		dev_info(dev, "using default %u Hz PTP clock frequency\n",
			 pdata->ptp_clk);
	}

	pdata->ker_ptp_clk = devm_clk_get(dev, "ptp_clk");
	if (IS_ERR(pdata->ker_ptp_clk)) {
		dev_err(dev, "Failed to get MAC %d ptp clock!\n",
			pdata->mac_idx);
		return PTR_ERR(pdata->ker_ptp_clk);
	}

	pdata->dev = dev;

	/* Init function fointers */
	mac_init_fn_ptrs(&pdata->ops);

	/* Disable unused MAC */
	if (pdata->mac_en == MAC_DIS) {
		mac_reset(&pdata->ops, RESET_OFF);
		return 0;
	}

	ret = clk_set_rate(pdata->ker_ptp_clk, pdata->ptp_clk);
	if (ret) {
		dev_err(dev, "Failed to ptp clock to %i Hz: err: %i\n",
			pdata->ptp_clk, ret);
		return ret;
	}

	ret = clk_get_rate(pdata->ker_ptp_clk);
	if (ret != pdata->ptp_clk) {
		dev_err(dev, "Expected PTP clock rate unsupported, will use %i Hz instead\n",
			ret);
		pdata->ptp_clk = ret;
	}

	ret = clk_prepare_enable(pdata->ker_ptp_clk);
	if (ret < 0) {
		dev_err(dev, "Failed to enable MAC %i ptp clock\n",
			pdata->mac_idx);
		return ret;
	}

	/* Request IRQ for MAC */
	mac_irq_init(pdata);

	pdata->ops.init(&pdata->ops);

	linksts = pdata->ops.get_link_sts(&pdata->ops);
	duplex = pdata->ops.get_duplex(&pdata->ops);
	speed = pdata->ops.get_speed(&pdata->ops);

	if (speed == GSW_PORT_SPEED_10)
		load = "10 Mbps";
	else if (speed == GSW_PORT_SPEED_100)
		load = "100 Mbps";
	else if (speed == GSW_PORT_SPEED_1000)
		load = "1 Gbps";
	else if (speed == GSW_PORT_SPEED_100000)
		load = "10 Gbps";
	else if (speed == GSW_PORT_SPEED_25000)
		load = "2.5 Gbps";
	else
		load = "AUTO";

	if (linksts == GSW_PORT_LINK_UP)
		ls = "UP";
	else if (linksts == GSW_PORT_LINK_DOWN)
		ls = "DOWN";
	else
		ls = "AUTO";

	if (duplex == GSW_DUPLEX_FULL)
		dp = "Full";
	else if (duplex == GSW_DUPLEX_HALF)
		dp = "Half";
	else
		dp = "AUTO";

	pr_debug("XGMAC Init %d: Speed: %s Link: %s Duplex: %s\n",
		 pdata->mac_idx, load, ls, dp);

	return 0;
}

static int mac_remove(struct platform_device *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(platform_get_drvdata(pdev));

	clk_disable_unprepare(pdata->ker_ptp_clk);

	return 0;
}

static const struct of_device_id gsw_mac_match[] = {
	{ .compatible = "intel,gsw_mac" },
	{},
};
MODULE_DEVICE_TABLE(of, gsw_mac_match);

static struct platform_driver gsw_mac_driver = {
	.probe = mac_probe,
	.remove = mac_remove,
	.driver = {
		.name = MAC_DEV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = gsw_mac_match,
	},
};

module_platform_driver(gsw_mac_driver);

MODULE_AUTHOR("Joby Thampan");
MODULE_DESCRIPTION("Intel GSW_MAC Device driver");
MODULE_LICENSE("GPL");

