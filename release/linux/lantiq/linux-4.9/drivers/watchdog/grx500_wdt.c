/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 *                          (C) Copyright 2016~2017 Intel Corporation

	For licensing information, see the file 'LICENSE' in the root folder of
	this software module.

******************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/reset.h>
#include "../../arch/mips/lantiq/clk.h"
#include <linux/irqchip/mips-gic.h>
#include <lantiq.h>
/* #include <cgu.h> */
#include "watchdog_core.h"
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

/* WDT MACROs */
#define WD_RESET	(1 << 7)
#define WD_INTR		(1 << 6)
#define WD_NWAIT	(1 << 5)

/* Second Countdown mode */
#define WD_TYPE_SCD	(1 << 1)
#define WD_START	(0x1)

/* RCU MACROs */
static struct regmap *ltq_rcu_base;
#define RCU_IAP_WDT_RST_EN				0x0050
#define RCU_IAP_WDT_RST_STAT			0x0014
#define	RCU_WDTx_RESET					0xf
/* Default is 300 seconds > MAX_TIMEOUT so probe reads from DT file timeout */
#define TIMER_MARGIN	300

static unsigned int timeout = TIMER_MARGIN; /* in seconds */
module_param(timeout, uint, 0644);
MODULE_PARM_DESC(timeout, "Watchdog timeout in seconds\n"
	" (default=" __MODULE_STRING(TIMER_MARGIN) ")");

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0644);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started\n"
	" (default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");


struct grx500_clk_core {
	const char		*name;
	const struct clk_ops	*ops;
	struct clk_hw		*hw;
	struct module		*owner;
	struct clk_core		*parent;
	const char		**parent_names;
	struct clk_core		**parents;
	u8			num_parents;
	u8			new_parent_index;
	unsigned long		rate;
	unsigned long		req_rate;
	unsigned long		new_rate;
	struct clk_core		*new_parent;
	struct clk_core		*new_child;
	unsigned long		flags;
	bool			orphan;
	unsigned int		enable_count;
	unsigned int		prepare_count;
	unsigned long		min_rate;
	unsigned long		max_rate;
	unsigned long		accuracy;
	int			phase;
	struct hlist_head	children;
	struct hlist_node	child_node;
	struct hlist_head	clks;
	unsigned int		notifier_count;
#ifdef CONFIG_DEBUG_FS
	struct dentry		*dentry;
	struct hlist_node	debug_node;
#endif
	struct kref		ref;
};

struct grx500_clk_struct {
	struct grx500_clk_core	*core;
	const char *dev_id;
	const char *con_id;
	unsigned long min_rate;
	unsigned long max_rate;
	struct hlist_node clks_node;
};

struct grx500_wdt_struct {
	struct clk		*clk;
	unsigned long		rate;
	unsigned int		cpu;
	struct notifier_block	restart_handler;
	struct watchdog_device	wdd;

	/* Some platform (e.g. prx300) requires explicit resets for
	 * individual modules.
	 */
	int reset_count;
	struct reset_control **resets;
};

#define to_grx500_wdt(wdd)	container_of(wdd, struct grx500_wdt_struct, wdd)

DEFINE_PER_CPU(struct watchdog_device, grx500wdt);
static unsigned long cpu_clk;

#define MS(_v, _f, _p)  (((_v) & (_f)) >> _p)

static inline void Enable_WDT_intr(void *wdtirq)
{
	pr_debug("[%s]:[%d] irq= %d cpu = %d\n", __func__, __LINE__,
		(*(u32 *)wdtirq), smp_processor_id());

	enable_percpu_irq((*(u32 *)wdtirq), 0);
}

static int grx500wdt_start(struct watchdog_device *wdt_dev)
{
	uint32_t config0;
	unsigned long flags;

	pr_debug("[%s]:[%d] wdt_dev=0x%p id=%d cpu = %d\n", __func__, __LINE__,
		wdt_dev, wdt_dev->id, smp_processor_id());
	if (wdt_dev->id == smp_processor_id()) {
		config0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0));
		rmb();
		gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0),
			(config0 | WD_START));
		wmb();
	} else {
		local_irq_save(flags);
		gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_OTHER_ADDR), wdt_dev->id);
		wmb();
		config0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0));
		rmb();
		gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0),
			(config0 | WD_START));
		wmb();
		local_irq_restore(flags);
	}

	return 0;
}

static int grx500wdt_stop(struct watchdog_device *wdt_dev)
{
	uint32_t config0;
	unsigned long flags;

	pr_debug("[%s]:[%d] wdt_dev=0x%p id=%d cpu = %d\n", __func__, __LINE__,
		wdt_dev, wdt_dev->id, smp_processor_id());
	if (wdt_dev->id == smp_processor_id()) {
		config0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0));
		rmb();
		gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0),
			(config0 & ~WD_START));
		wmb();
	} else {
		local_irq_save(flags);
		gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_OTHER_ADDR), wdt_dev->id);
		wmb();
		config0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0));
		rmb();
		gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0),
			(config0 & ~WD_START));
		wmb();
		local_irq_restore(flags);
	}

	return 0;
}

static int grx500wdt_set_timeout(struct watchdog_device *wdt_dev,
				unsigned int new_timeout)
{
	struct watchdog_device *grx500_wdt;
	unsigned long flags;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	/* grx500_wdt = &per_cpu(grx500wdt, smp_processor_id()); */
	grx500_wdt = wdt_dev;

	grx500_wdt->timeout = new_timeout;
	pr_debug("%s: timeout = %d, cpu = %d, id = %d wdt_dev=0x%p\n", __func__,
		new_timeout, smp_processor_id(), wdt_dev->id, wdt_dev);

	grx500wdt_stop(grx500_wdt);

	if (wdt_dev->id == smp_processor_id()) {
		if (((u64)cpu_clk * (u64)(grx500_wdt->timeout)) <= (u64)U32_MAX) {
			gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_INITIAL0),
				(cpu_clk * grx500_wdt->timeout));
		} else {
			gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_INITIAL0),
				(U32_MAX));
		}
		wmb();
	} else {
		local_irq_save(flags);
		gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_OTHER_ADDR), wdt_dev->id);
		wmb();
		if (((u64)cpu_clk * (u64)(grx500_wdt->timeout)) <= (u64)U32_MAX) {
			gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_INITIAL0),
				(cpu_clk * grx500_wdt->timeout));
		} else {
			gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_INITIAL0),
				(U32_MAX));
		}
		wmb();
		local_irq_restore(flags);
	}

	grx500wdt_start(grx500_wdt);

	return 0;
}

static uint32_t grx500wdt_get_timeleft(struct watchdog_device *wdt_dev)
{
	u32 count0, initial0, config0, rst_en, map0;
	unsigned long flags;

	if (wdt_dev->id == smp_processor_id()) {
		initial0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_INITIAL0));
		rmb();
		config0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0));
		rmb();
		count0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_COUNT0));
		rmb();
		map0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_MAP));
		rmb();
	} else {
		local_irq_save(flags);
		gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_OTHER_ADDR), wdt_dev->id);
		wmb();
		initial0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_INITIAL0));
		rmb();
		config0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0));
		rmb();
		count0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_COUNT0));
		rmb();
		map0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_MAP));
		rmb();
		local_irq_restore(flags);
	}

	regmap_read(ltq_rcu_base, RCU_IAP_WDT_RST_EN, &rst_en);
	pr_info("%s cpu=%d id=%d count0=%x config0=%x map0=0x%x left=%lu\n"
		"timeout=%u status=%lu rst_en=0x%x\n", __func__,
		smp_processor_id(), wdt_dev->id, count0, config0, map0,
		count0/cpu_clk, wdt_dev->timeout, wdt_dev->status, rst_en);

	return count0 / cpu_clk;
}

static int grx500wdt_ping(struct watchdog_device *wdt_dev)
{
	struct watchdog_device *grx500_wdt;

	pr_debug("[%s]:[%d] wdt_dev->id=%d cpu = %d wdt_dev=0x%p\n", __func__, __LINE__,
		wdt_dev->id, smp_processor_id(), wdt_dev);

	/* grx500_wdt = &per_cpu(grx500wdt, smp_processor_id()); */
	grx500_wdt = wdt_dev;
	/* grx500wdt_get_timeleft(grx500_wdt); */

	/* grx500wdt_stop(grx500_wdt); */
	/* grx500wdt_get_timeleft(grx500_wdt); */
	grx500wdt_start(grx500_wdt);
	/* grx500wdt_get_timeleft(grx500_wdt); */

	return 0;
}

static irqreturn_t grx500wdt_irq(int irqno, void *param)
{
	struct watchdog_device *grx500_wdt;
	struct grx500_wdt_struct *priv;
	int i;
	static int flag;

	grx500_wdt = &per_cpu(grx500wdt, smp_processor_id());
	priv = watchdog_get_drvdata(grx500_wdt);

	/* Certain platform (e.g. prx300) does not reset gphy RCU during
	 * global reset. We then explicitly add reset here for workaround.
	 */
	for (i = 0; i < priv->reset_count; i++) {
		if (!reset_control_status(priv->resets[i]))
			reset_control_assert(priv->resets[i]);
	}

	/* enable this for dump data */
/*	grx500wdt_start(grx500_wdt);*/
	WARN_ONCE(1, " IRQ %d triggered as WDT%d Timer Overflow on CPU %d!\n",
		irqno, grx500_wdt->id, smp_processor_id());

	if (flag == 0) {
		show_state();
		flag = 1;
	}
	return IRQ_HANDLED;
}

struct irqaction grx500wdt_irqaction = {
	.handler = grx500wdt_irq,
	.flags = IRQF_PERCPU | IRQF_NO_SUSPEND,
	.name = "watchdog",
};

static const struct watchdog_info grx500wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.identity = "Hardware Watchdog for GRX500",
};

static const struct watchdog_ops grx500wdt_ops = {
	.owner = THIS_MODULE,
	.start = grx500wdt_start,
	.stop = grx500wdt_stop,
	.ping = grx500wdt_ping,
	.set_timeout = grx500wdt_set_timeout,
	.get_timeleft = grx500wdt_get_timeleft,
};

static int grx500wdt_probe(struct platform_device *pdev)
{
	/* struct resource *wdt_res; */
	struct clk *clk;
	struct grx500_wdt_struct *priv;
	int ret, cpu, irq, resetcause;
	int i;
	unsigned long flags;

	/*
	 * I/O memory need not be taken from Device Tree as WDT is part of
	 * GIC and can be accessed through GIC_[READ/WRITE] and VPE_LOCAL APIs
	 */
	ltq_rcu_base = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
		"lantiq,wdt-rcu");

	irq = platform_get_irq(pdev, 0);

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->reset_count = of_count_phandle_with_args(pdev->dev.of_node,
						       "resets",
						       "#reset-cells");
	if (priv->reset_count) {
		priv->resets =
			devm_kzalloc(&pdev->dev,
				     priv->reset_count * sizeof(*priv->resets),
				     GFP_KERNEL);
		if (!priv->resets)
			return -ENOMEM;
	}

	for (i = 0; i < priv->reset_count; i++) {
		priv->resets[i] =
			devm_reset_control_get_shared_by_index(&pdev->dev, i);
		if (IS_ERR(priv->resets[i])) {
			dev_err(&pdev->dev, "fail to get reset control %d\n",
				i);
			return PTR_ERR(priv->resets[i]);
		}
	}

	/* set up per-cpu IRQ */
	setup_percpu_irq(irq, &grx500wdt_irqaction);

	/* Get the Clock frequency */
	clk = devm_clk_get(&pdev->dev, "freq");
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "Failed to get CPU clock: %ld\n",
			PTR_ERR(clk));
		ret = -1;
		goto exit;
	}

	cpu_clk = clk_get_rate(clk);
	dev_info(&pdev->dev, "[%s]:[%d] cpu_clk=%lu\n",
		__func__, __LINE__, cpu_clk);
	/* cpu_clk = grx500_clk_get_rate((struct clk *)ltq_grx500_wdt->clk); */

	for_each_possible_cpu(cpu) {
		struct watchdog_device *grx500_wdt;

		grx500_wdt = &per_cpu(grx500wdt, cpu);

		grx500_wdt->id = cpu;
		grx500_wdt->info = &grx500wdt_info;
		grx500_wdt->ops = &grx500wdt_ops;
		watchdog_set_drvdata(grx500_wdt, priv);
	}

	regmap_read(ltq_rcu_base, RCU_IAP_WDT_RST_STAT, &resetcause);
	dev_info(&pdev->dev, "[%s]WDT reset is Bit31, RCU_IAP_WDT_RST_STAT=0x%x\n",
		__func__, resetcause);
	if (resetcause & 0x80000000)
		dev_info(&pdev->dev, "[%s]WDT reset.\n", __func__);
	else
		dev_info(&pdev->dev, "[%s]Not WDT reset.\n", __func__);

	for_each_online_cpu(cpu) {
		uint32_t config0;
		struct watchdog_device *grx500_wdt;

		grx500_wdt = &per_cpu(grx500wdt, cpu);

		/* dev_info(&pdev->dev, "cpu = %d cpu_clock (ltq_grx500_cpu_hz)
		 *  = %ld cpu_clk = %ld \n",cpu, ltq_grx500_cpu_hz(), cpu_clk);
		 */

		grx500_wdt->min_timeout = 1;
		/*grx500_wdt->max_timeout = 14;*/
		grx500_wdt->max_timeout = ((U32_MAX) / cpu_clk);
		grx500_wdt->max_hw_heartbeat_ms = grx500_wdt->max_timeout *1000;
		grx500_wdt->min_hw_heartbeat_ms = grx500_wdt->min_timeout *1000;

		timeout = 3;
		watchdog_init_timeout(grx500_wdt, timeout, &pdev->dev);
		watchdog_set_nowayout(grx500_wdt, nowayout);

		if (cpu == smp_processor_id()) {
			gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0), 0x0);
			wmb();
			config0 = gic_read_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0));
			rmb(); /* Reset CONFIG0 to 0x0 */
			gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_CONFIG0),
				(config0 | WD_TYPE_SCD | WD_NWAIT));
			wmb();
			gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_WD_INITIAL0),
				(U32_MAX));
			wmb();
		} else {
			local_irq_save(flags);
			gic_write_reg(GIC_REG(VPE_LOCAL, GIC_VPE_OTHER_ADDR), cpu);
			wmb();
			gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0), 0x0);
			wmb();
			config0 = gic_read_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0));
			rmb(); /* Reset CONFIG0 to 0x0 */
			gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_CONFIG0),
				(config0 | WD_TYPE_SCD | WD_NWAIT));
			wmb();
			gic_write_reg(GIC_REG(VPE_OTHER, GIC_VPE_WD_INITIAL0),
				(U32_MAX));
			wmb();
			local_irq_restore(flags);
		}

		/* Map to PIN is configured by GIC */

		/* Use enable_percpu_irq() for each Online CPU. */

		preempt_disable();
		if (cpu != smp_processor_id())
			smp_call_function_single(cpu,
				(smp_call_func_t)Enable_WDT_intr,
				(void *)&irq, 1);
		else
			Enable_WDT_intr((void *)&irq);

		preempt_enable();

		dev_info(&pdev->dev, "[%s]:[%d] grx500_wdt = %p\n",
			__func__, __LINE__, grx500_wdt);

		platform_set_drvdata(pdev, grx500_wdt);
		ret = watchdog_register_device(grx500_wdt);
		if (ret) {
			dev_info(&pdev->dev, "[%s]:[%d] ret = %d\n",
				__func__, __LINE__, ret);
			goto exit;
		}
	}

	/* Enable WDT reset to RCU for VPEx */
	regmap_write(ltq_rcu_base, RCU_IAP_WDT_RST_EN, RCU_WDTx_RESET);
	wmb();

	dev_info(&pdev->dev, "H/w Watchdog Timer: (max %ld) (nowayout= %d)\n",
		(U32_MAX / cpu_clk), nowayout);

	return 0;

exit:
	return ret;
}

static int __exit grx500wdt_remove(struct platform_device *dev)
{
	int cpu;

	for_each_online_cpu(cpu) {
		struct watchdog_device *grx500_wdt;

		grx500_wdt = &per_cpu(grx500wdt, cpu);
		grx500_wdt->id = cpu;
		dev_info(&dev->dev, "cpu = %d\n", cpu);
		watchdog_unregister_device(grx500_wdt);
	}
	return 0;
}

static void grx500wdt_shutdown(struct platform_device *dev)
{
	struct watchdog_device *grx500_wdt;

	/* grx500_wdt = &per_cpu(grx500wdt, smp_processor_id()); */
	grx500_wdt = platform_get_drvdata(dev);

	grx500wdt_stop(grx500_wdt);
}

static const struct of_device_id grx500wdt_match[] = {
	{ .compatible = "lantiq,grx500wdt" },
	{},
};

static struct platform_driver grx500wdt_driver = {
	.probe = grx500wdt_probe,
	.remove = __exit_p(grx500wdt_remove),
	.shutdown = grx500wdt_shutdown,
	.driver = {
		.name = "grx500wdt",
		.owner = THIS_MODULE,
		.of_match_table = grx500wdt_match,
	},
};

module_platform_driver(grx500wdt_driver);

MODULE_DESCRIPTION("GRX500 Watchdog Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:grx500wdt");
