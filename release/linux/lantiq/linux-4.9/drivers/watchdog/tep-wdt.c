// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright (C) 2020 Intel Corporation.
 *  Ayyppan Rathinam <ayyappanx.rathinam@intel.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include "watchdog_core.h"
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <asm/irq.h>
#include <asm/setup.h>
#include <linux/clk.h>
#include <grx500_bootcore_interrupt.h>

/* RCU Defines */
#define RST_REQ			0x10
#define RCU_TEP_WDT_RST		0x0050
#define WDT_SCPU_RST_EN		0x100
#define RCU_GPHY_RESET		0x20000000

/* GPTC Defines */
#define GPTC2_TIMER2_IN_INDEX	1
#define GPTC2_TIMER2_OUT_INDEX	9
#define GPTC_R32(b, r)		readl((b) + (r))
#define GPTC_W32(b, r, v)	writel(v, (b) + (r))
#define GPTC_IRNEN		0x00F4
#define GPTC_IRNCR		0x00FC
#define GPTC_CLC		0x00
#define GPTC_RUN(X)		(0x18 + (X) * 0x20)
#define GPTC_RLD(X)		(0x20 + (X) * 0x20)
#define RUN_EN			BIT(0)
#define RUN_STOP		BIT(1)
#define RUN_RELOAD		BIT(2)
#define CLC_SUSPEND		BIT(4)
#define CLC_RMC			BIT(8)
#define GPTC_CON(X)		(0x10 + (X) * 0x20)
#define CON_ONESHOT		BIT(3) /* Stop or continue when overflowing */
#define CON_EXT			BIT(4) /* 32 or 16 bit */

/* CGU register */
#define CGU_CLKGCR1_A		0x124
#define  CGU_CLKGCR1_GPTC2	BIT(14)

enum gptc2 {
	TIMER_2 = 1,
	TIMER_3 = 2
};

struct tep_wdt {
	unsigned long clk;
	struct device *dev;
	struct regmap *rcu;
	struct regmap *cgu;
	void __iomem *gptc;
	struct watchdog_device *wdd;
};

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0644);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started\n"
	" (default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

static irqreturn_t tep_wdt_irq_handler(int irqno, void *param)
{
	struct tep_wdt *priv = param;
	u32 val;

	dev_warn(priv->dev, "%s: !!! WDT_Prewarming_Fm_GPT2_TIMER_2 !!!\n",
		 __func__);

	/* Added explicit GPHY reset to resolve known HW issue */
	regmap_read(priv->rcu, RST_REQ, &val);
	val |= RCU_GPHY_RESET;
	regmap_write(priv->rcu, RST_REQ, val);

	/* IRQ clear */
	GPTC_W32(priv->gptc, GPTC_IRNCR, BIT(TIMER_2 * 2));
	return IRQ_HANDLED;
}

static int tep_wdt_start(struct watchdog_device *wdd)
{
	struct tep_wdt *priv = watchdog_get_drvdata(wdd);
	u32 idx, tm = wdd->timeout;
	u64 tm_out;

	tm_out = ((u64)(priv->clk) * (u64)tm);
	if ( tm_out <= (u64)U32_MAX)
		tm = (u32)tm_out;
	else
		tm = U32_MAX;

	for (idx = TIMER_2; idx <= TIMER_3; idx++) {
		GPTC_W32(priv->gptc, GPTC_CON(idx), CON_ONESHOT | CON_EXT);
		GPTC_W32(priv->gptc, GPTC_RLD(idx), ((idx == TIMER_2) ? (tm / 2) : tm));
		GPTC_W32(priv->gptc, GPTC_RUN(idx), RUN_EN | RUN_RELOAD);
	}
	return 0;
}

static int tep_wdt_stop(struct watchdog_device *wdd)
{
	u32 idx;
	struct tep_wdt *priv = watchdog_get_drvdata(wdd);

	for (idx = TIMER_2; idx <= TIMER_3; idx++) {
		GPTC_W32(priv->gptc, GPTC_RUN(idx), RUN_STOP);
		GPTC_W32(priv->gptc, GPTC_RUN(idx), RUN_STOP);
	}
	return 0;
}

static int tep_wdt_ping(struct watchdog_device *wdd)
{
	tep_wdt_stop(wdd);
	tep_wdt_start(wdd);
	return 0;
}

static int tep_wdt_set_timeout(struct watchdog_device *wdd, u32 timeout)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	wdd->timeout = timeout;
	return 0;
}

static struct irqaction gptc2_timer2_irqaction = {
	.handler = tep_wdt_irq_handler,
	.name = "tep_gptc2_timer2_interrupt",
};

static const struct watchdog_info wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.identity = "TEP Watchdog",
};

static const struct watchdog_ops wdt_ops = {
	.start = tep_wdt_start,
	.stop = tep_wdt_stop,
	.ping = tep_wdt_ping,
	.set_timeout = tep_wdt_set_timeout,
};

static const struct of_device_id tep_wdt_match[] = {
	{
		.compatible = "intel,prx300-tep-wdt",
	},
	{ /* sentinel */ },
};

static int tep_watchdog_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct tep_wdt *priv;
	struct resource res;
	struct clk *clk;
	int ret;

	/* TEP core validation */
	if (current_cpu_type() != CPU_4KEC) {
		dev_err(dev, "%s: Invalid cpu_type\n", __func__);
		return -EINVAL;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->wdd = devm_kzalloc(&pdev->dev, sizeof(*priv->wdd), GFP_KERNEL);
	if (!priv->wdd)
		return -ENOMEM;

	/* Get the Clock frequency */
	clk = clk_get_sys("cpu", "cpu");
	if (IS_ERR(clk))
		return PTR_ERR(clk);

	priv->clk = clk_get_rate(clk);
	priv->dev = &pdev->dev;

	priv->rcu = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
		"intel,wdt-rcu");
	if (IS_ERR(priv->rcu)) {
		dev_err(&pdev->dev, "Failed to find intel,wdt-rcu\n");
		return PTR_ERR(priv->rcu);
	}

	priv->cgu = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
		"intel,wdt-cgu");
	if (IS_ERR(priv->cgu)) {
		dev_err(&pdev->dev, "Failed to find intel,wdt-cgu\n");
		return PTR_ERR(priv->cgu);
	}

	if (of_address_to_resource(dev->of_node, 0, &res)) {
		dev_err(&pdev->dev, "Failed to find gptc base\n");
		return -ENXIO;
	}

	priv->gptc = ioremap(res.start, resource_size(&res));
	if (IS_ERR(priv->gptc)) {
		dev_err(&pdev->dev, "gptc base ioremap failed\n");
		return PTR_ERR(priv->gptc);
	}

	/* config vector interrupt for TIMER_2 */
	gptc2_timer2_irqaction.dev_id = priv;
	grx500_bootcore_register_static_irq(GRX500_BOOTCORE_GPTC2_TIMER2_IN_INDEX,
					    GRX500_BOOTCORE_GPTC2_TIMER2_OUT_INDEX,
					    &gptc2_timer2_irqaction,
					    grx500_bootcore_gptc2_timer2_irq);

	priv->wdd->info = &wdt_info;
	priv->wdd->ops = &wdt_ops;
	priv->wdd->min_timeout = 1;
	priv->wdd->max_timeout = ((U32_MAX) / priv->clk);
	priv->wdd->timeout = priv->wdd->max_timeout * priv->clk;
	priv->wdd->min_hw_heartbeat_ms = priv->wdd->max_timeout * 1000;
	priv->wdd->max_hw_heartbeat_ms = priv->wdd->max_timeout * 1000;
	priv->wdd->parent = &pdev->dev;

	watchdog_init_timeout(priv->wdd, priv->wdd->timeout, &pdev->dev);
	watchdog_set_nowayout(priv->wdd, nowayout);
	ret = watchdog_register_device(priv->wdd);
	if (ret) {
		dev_err(dev, "%s: WDT Register Failed %d\n", __func__, ret);
		return ret;
	}

	watchdog_set_drvdata(priv->wdd, priv);
	platform_set_drvdata(pdev, priv);

	/* Enable clk gate for GPTC2 */
	regmap_write(priv->cgu, CGU_CLKGCR1_A, CGU_CLKGCR1_GPTC2);

	/* Enable WDT reset to RCU for VPEx */
	regmap_write(priv->rcu, RCU_TEP_WDT_RST, WDT_SCPU_RST_EN);

	/* Enable GPTC2 */
	GPTC_W32(priv->gptc, GPTC_CLC, CLC_SUSPEND | CLC_RMC);

	/* Enable GPTC2 Timer2 interrupt as prewarn timer interrupt */
	GPTC_W32(priv->gptc, GPTC_IRNEN,
		 ((~BIT(TIMER_2 * 2)) & GPTC_R32(priv->gptc, GPTC_IRNEN)));
	GPTC_W32(priv->gptc, GPTC_IRNCR,
		 ((BIT(TIMER_2 * 2)) | GPTC_R32(priv->gptc, GPTC_IRNCR)));
	GPTC_W32(priv->gptc, GPTC_IRNEN,
		 ((BIT(TIMER_2 * 2)) | GPTC_R32(priv->gptc, GPTC_IRNEN)));

	/* Enable GPTC2 Timer3 interrupt as watchdog reset interrupt */
	GPTC_W32(priv->gptc, GPTC_IRNEN,
		 ((~BIT(TIMER_3 * 2)) & GPTC_R32(priv->gptc, GPTC_IRNEN)));
	GPTC_W32(priv->gptc, GPTC_IRNCR,
		 ((BIT(TIMER_3 * 2)) | GPTC_R32(priv->gptc, GPTC_IRNCR)));
	GPTC_W32(priv->gptc, GPTC_IRNEN,
		 ((BIT(TIMER_3 * 2)) | GPTC_R32(priv->gptc, GPTC_IRNEN)));

	return 0;
}

static int tep_watchdog_remove(struct platform_device *pdev)
{
	struct tep_wdt *priv = platform_get_drvdata(pdev);

	tep_wdt_stop(priv->wdd);
	regmap_write(priv->rcu, RCU_TEP_WDT_RST, ~(WDT_SCPU_RST_EN));
	return 0;
}

static struct platform_driver tep_watchdog_driver = {
	.probe = tep_watchdog_probe,
	.remove = tep_watchdog_remove,
	.driver = {
		.name = "tep-watchdog",
		.of_match_table = tep_wdt_match,
	}
};

module_platform_driver(tep_watchdog_driver);

MODULE_DESCRIPTION("Intel PRX300 TEP Watchdog driver");
MODULE_AUTHOR("Ayyppan Rathinam <ayyappanx.rathinam@intel.com>");
MODULE_LICENSE("GPL v2");

