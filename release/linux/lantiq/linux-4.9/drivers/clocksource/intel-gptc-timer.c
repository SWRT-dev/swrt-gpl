/*
 *  Copyright(C) 2017 Intel Corporation.
 *  Lei Chuanhua <chuanhua.lei@intel.com>
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  The full GNU General Public License is included in this distribution in
 *  the file called "COPYING".
 */
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/irq.h>
#include <linux/sched_clock.h>
#include <linux/cpu.h>
#include <linux/debugfs.h>
#include <linux/platform_device.h>
#include <linux/module.h>

#include <asm/time.h>

#include <clocksource/intel-gptc-timer.h>
#ifdef CONFIG_LTQ_VMB
#include <asm/ltq_vmb.h>
#include <linux/irqchip/mips-gic.h>
#endif

#define GPTC_CLC	0x00
#define CLC_DIS		BIT(0)
#define CLC_SUSPEND	BIT(4)
#define CLC_RMC		BIT(8)
#define CLC_SMC		BIT(16)

#define GPTC_ID		0x08
#define ID_VER		0x1Fu
#define ID_VER_S	0
#define ID_CFG		0xE0u
#define ID_CFG_S	5
#define ID_ID		0xFF00u
#define ID_ID_S		8

#define GPTC_CON(X)	(0x10 + (X) * 0x20)
#define CON_EN_STAT	BIT(0) /* RO only */
#define CON_COUNT_UP	BIT(1) /* Down up or down */
#define CON_CNT		BIT(2)
#define CON_ONESHOT	BIT(3) /* Stop or continue when overflowing */
#define CON_EXT		BIT(4) /* 32 or 16 bit */
#define CON_EDGE_RISE	BIT(6)
#define CON_EDGE_FALL	BIT(7)
#define CON_EDGE_ANY	(CON_EDGE_RISE | CON_EDGE_FALL)
#define CON_SYNC	BIT(8) /* Signal sync to module clock or not */

#define GPTC_RUN(X)	(0x18 + (X) * 0x20)
#define RUN_EN		BIT(0)
#define RUN_STOP	BIT(1)
#define RUN_RELOAD	BIT(2)

#define GPTC_RLD(X)	(0x20 + (X) * 0x20)
#define GPTC_CNT(X)	(0x28 + (X) * 0x20)

#define GPTC_IRNEN	0xf4
#define GPTC_IRNICR	0xf8
#define GPTC_IRNCR	0xfc

#define TIMER_PER_GPTC	3

#define GPTC_MAX	4

enum gptc_cnt_dir {
	GPTC_COUNT_DOWN = 0,
	GPTC_COUNT_UP,
};

enum gptc_timer_type {
	TIMER_TYPE_CLK_SRC,
	TIMER_TYPE_CLK_EVT,
	TIMER_TYPE_WDT,
	TIMER_TYPE_HEARTBEAT,
	TIMER_TYPE_HT_YIELD,
	TIMER_TYPE_MAX,
};

/* Hardwre GPTC struct */
struct gptc {
	u32 id;
	struct device_node *np;
	void __iomem *base;
	unsigned long phy_base;
	struct clk *freqclk;
	struct clk *gateclk;
	u32 fpifreq;
	spinlock_t lock; /* Shared register access */
	struct list_head parent; /* Timers belonging to itsef */
	struct list_head next; /* Link to next GPTC */
	struct kref ref;
};

struct gptc_timer {
	struct gptc *gptc; /* Point back to parent */
	void __iomem *base;
	unsigned long phy_base;
	u32 gptcid;
	u32 tid; /* 0, 1, 2 only */
	u32 cpuid;
	u32 irq;
	u32 type;
	u32 frequency;
	unsigned long cycles;
	enum gptc_cnt_dir dir;
	bool used;
	bool irq_registered;
	u32 yield_pin;
	void (*call_back)(void *);
	void *call_back_param;
	struct list_head child; /* Node in parent list */
	struct list_head clksrc; /* Node in clock source list */
	struct list_head clkevt; /* Node in clock event list */
	struct list_head wdt; /* Node in watchdog timer */
	struct list_head heartbeat; /* Heartbeat */
	struct list_head ht_yield; /* Heartbeat yield */
};

struct gptc_clocksource {
	struct gptc_timer  *timer;
	struct clocksource cs;
};

struct gptc_clockevent {
	struct gptc_timer *timer;
	u32 ticks_per_jiffy;
	struct clock_event_device ce;
	char name[16];
};

static LIST_HEAD(gptc_list);
static LIST_HEAD(gptc_clksrc_list);
static LIST_HEAD(gptc_clkevt_list);
static LIST_HEAD(gptc_wdt_list);
static LIST_HEAD(gptc_heartbeat_list);
static LIST_HEAD(gptc_ht_yield_list);

unsigned long gptc_phy_base;
static bool gptc_clksrc_init;

static DEFINE_PER_CPU(struct gptc_clockevent, gptc_event_device);

static inline struct gptc_clockevent *to_gptc_clockevent(
	struct clock_event_device *evt)
{
	return container_of(evt, struct gptc_clockevent, ce);
}

static inline struct gptc_timer *clkevt_to_gptc_timer(
	struct clock_event_device *evt)
{
	struct gptc_clockevent *gptce = container_of(evt,
						struct gptc_clockevent, ce);
	return gptce->timer;
}

static inline struct gptc_timer *clksrc_to_gptc_timer(struct clocksource *src)
{
	struct gptc_clocksource *gptcs = container_of(src,
					struct gptc_clocksource, cs);
	return gptcs->timer;
}

static inline u32 gptc_readl(struct gptc_timer *timer, u32 offs)
{
	return ioread32(timer->base + offs);
}

static inline void gptc_writel(struct gptc_timer *timer, unsigned long val,
			       u32 offs)
{
	iowrite32(val, timer->base + offs);
}

static u32 gptc_read_counter(struct gptc_timer *timer)
{
	return gptc_readl(timer,  GPTC_CNT(timer->tid));
}

static inline void gptc_stop_counter(struct gptc_timer *timer)
{
	gptc_writel(timer, RUN_STOP, GPTC_RUN(timer->tid));
}

static inline void gptc_reload_counter(struct gptc_timer *timer,
				       unsigned long cycles)
{
	gptc_writel(timer, cycles, GPTC_RLD(timer->tid));
}

static inline void gptc_reset_counter(struct gptc_timer *timer)
{
	gptc_reload_counter(timer, 0);
}

static inline void gptc_start_counter(struct gptc_timer *timer)
{
	gptc_writel(timer, RUN_EN, GPTC_RUN(timer->tid));
}

static inline void gptc_reload_and_run(struct gptc_timer *timer)
{
	gptc_writel(timer, RUN_EN | RUN_RELOAD, GPTC_RUN(timer->tid));
}

static inline void gptc_clc_enable(void __iomem *base)
{
	iowrite32(CLC_SUSPEND | CLC_RMC, base + GPTC_CLC);
}

static inline void gptc_irq_mask_all(void __iomem *base)
{
	iowrite32(0x00, base + GPTC_IRNEN);
}

static inline void gptc_irq_clear_all(void __iomem *base)
{
	iowrite32(0xff, base + GPTC_IRNCR);
}

static inline void gptc_irq_mask(struct gptc_timer *timer)
{
	u32 reg;
	unsigned long flags;
	struct gptc *gptc = timer->gptc;

	spin_lock_irqsave(&gptc->lock, flags);
	reg = gptc_readl(timer, GPTC_IRNEN);
	reg &= ~BIT(timer->tid * 2);
	gptc_writel(timer, reg, GPTC_IRNEN);
	spin_unlock_irqrestore(&gptc->lock, flags);
}

static inline void gptc_irq_unmask(struct gptc_timer *timer)
{
	u32 reg;
	unsigned long flags;
	struct gptc *gptc = timer->gptc;

	spin_lock_irqsave(&gptc->lock, flags);
	reg = gptc_readl(timer, GPTC_IRNEN);
	reg |= BIT(timer->tid * 2);
	gptc_writel(timer, reg, GPTC_IRNEN);
	spin_unlock_irqrestore(&gptc->lock, flags);
}

static inline void gptc_irq_ack(struct gptc_timer *timer)
{
	gptc_writel(timer, BIT(timer->tid * 2), GPTC_IRNCR);
}

static inline int gptc_irq_read(struct gptc_timer *timer)
{
	u32 reg;

	reg = gptc_readl(timer, GPTC_IRNCR);
	if (reg & (BIT(timer->tid * 2)))
		return 1;
	else
		return 0;
}

static void gptc_enable_32bit_timer(struct gptc_timer *timer)
{
	u32 reg;

	reg = gptc_readl(timer, GPTC_CON(timer->tid));
	reg |= CON_EXT;
	gptc_writel(timer, reg, GPTC_CON(timer->tid));
}

static void gptc_count_dir(struct gptc_timer *timer)
{
	u32 reg;

	reg = gptc_readl(timer, GPTC_CON(timer->tid));
	if (timer->dir == GPTC_COUNT_UP)
		reg |= CON_COUNT_UP;
	else
		reg &= ~CON_COUNT_UP;
	gptc_writel(timer, reg, GPTC_CON(timer->tid));
}

static void gptc_mode_setup(struct gptc_timer *timer, bool oneshot)
{
	u32 reg;

	reg = gptc_readl(timer, GPTC_CON(timer->tid));
	if (oneshot)
		reg |= CON_ONESHOT;
	else
		reg &= ~CON_ONESHOT;
	gptc_writel(timer, reg, GPTC_CON(timer->tid));
}

static irqreturn_t gptc_timer_interrupt(int irq, void *data)
{
	struct gptc_clockevent *gptce = data;
	struct gptc_timer *timer = gptce->timer;

	gptc_irq_mask(timer);
	gptc_irq_ack(timer);

	gptce->ce.event_handler(&gptce->ce);
	gptc_irq_unmask(timer);
	return IRQ_HANDLED;
}

static int gptc_clkevt_next_event(unsigned long cycles,
				  struct clock_event_device *evt)
{
	struct gptc_timer *timer = clkevt_to_gptc_timer(evt);

	WARN_ON(cycles == 0);
	gptc_stop_counter(timer);
	gptc_mode_setup(timer, true);
	gptc_reload_counter(timer, cycles);
	gptc_reload_and_run(timer);
	return 0;
}

static int gptc_clkevt_shutdown(struct clock_event_device *evt)
{
	struct gptc_timer *timer = clkevt_to_gptc_timer(evt);

	gptc_stop_counter(timer);
	gptc_reset_counter(timer);
	return 0;
}

static int gptc_clkevt_periodic(struct clock_event_device *evt)
{
	struct gptc_clockevent *gptce = to_gptc_clockevent(evt);
	struct gptc_timer *timer = gptce->timer;

	gptc_stop_counter(timer);
	gptc_mode_setup(timer, false);
	gptc_reload_counter(timer, gptce->ticks_per_jiffy);
	gptc_start_counter(timer);
	return 0;
}

static int gptc_clkevt_resume(struct clock_event_device *evt)
{
	struct gptc_timer *timer = clkevt_to_gptc_timer(evt);

	gptc_start_counter(timer);
	return 0;
}

static cycle_t gptc_hpt_read(struct clocksource *cs)
{
	struct gptc_timer *timer = clksrc_to_gptc_timer(cs);

	return (cycle_t)gptc_read_counter(timer);
}

static void gptc_global_init(struct gptc *gptc)
{
	gptc_clc_enable(gptc->base);
	gptc_irq_mask_all(gptc->base);
	gptc_irq_clear_all(gptc->base);
}

static void gptc_per_timer_init(struct gptc_timer *timer)
{
	gptc_count_dir(timer);
	gptc_enable_32bit_timer(timer);
	if (timer->type == TIMER_TYPE_HEARTBEAT)
		gptc_reload_counter(timer, timer->cycles);
	else
		gptc_reset_counter(timer);
	if (timer->type == TIMER_TYPE_CLK_SRC ||
	    timer->type == TIMER_TYPE_HEARTBEAT)
		gptc_reload_and_run(timer);
}

static const char *const timer_type_to_str(u32 type)
{
	switch (type) {
	case TIMER_TYPE_CLK_SRC:
		return "src";

	case TIMER_TYPE_CLK_EVT:
		return "event";

	case TIMER_TYPE_WDT:
		return "wdt";

	case TIMER_TYPE_HEARTBEAT:
		return "heartbeat";

	case TIMER_TYPE_HT_YIELD:
		return "ht_yield";

	default:
		return "none";
	}
}

static void gptc_of_config_print(struct gptc *gptc)
{
	int i = 0;
	struct gptc_timer *timer;

	pr_debug("GPTC%d timer list info\n", gptc->id);
	list_for_each_entry(timer, &gptc->parent, child) {
		pr_debug("timer%d base %p gptcid %d freq %d tid %d cpuid %d irq %d clk %s\n",
			 i, timer->base, timer->gptcid, timer->frequency,
			 timer->tid, timer->cpuid, timer->irq,
			 timer_type_to_str(timer->type));
		i++;
	}
}

static int gptc_clock_init(struct gptc *gptc)
{
	struct device_node *np = gptc->np;

	gptc->gateclk = of_clk_get_by_name(np, "gptc");
	if (IS_ERR(gptc->gateclk)) {
		pr_err("Failed to get gptc gate clk: %ld\n",
		       PTR_ERR(gptc->gateclk));
		return PTR_ERR(gptc->gateclk);
	}

	gptc->freqclk = of_clk_get_by_name(np, "freq");
	if (IS_ERR(gptc->freqclk)) {
		pr_err("Failed to get gptc frequency clk: %ld\n",
		       PTR_ERR(gptc->freqclk));
		return PTR_ERR(gptc->freqclk);
	}
	return 0;
}

static void gptc_clock_deinit(struct gptc *gptc)
{
	if (gptc->gateclk && !IS_ERR(gptc->gateclk))
		clk_put(gptc->gateclk);

	if (gptc->freqclk && !IS_ERR(gptc->freqclk))
		clk_put(gptc->freqclk);

	gptc->gateclk = NULL;
	gptc->freqclk = NULL;
}

static int gptc_clock_enable(struct gptc *gptc)
{
	int ret;

	if (IS_ERR_OR_NULL(gptc->gateclk) ||
	    IS_ERR_OR_NULL(gptc->freqclk)) {
		pr_err("%s clock(s) is/are not initialized\n", __func__);
		ret = -EIO;
		goto out;
	}
	ret = clk_prepare_enable(gptc->gateclk);
	if (ret) {
		pr_err("%s failed to enable gate clk: %d\n", __func__, ret);
		goto out;
	}

	ret = clk_prepare_enable(gptc->freqclk);
	if (ret) {
		pr_err("%s failed to enable fpi clk: %d\n", __func__, ret);
		goto err_gateclk_disable;
	}
	gptc->fpifreq = clk_get_rate(gptc->freqclk);

	return 0;
err_gateclk_disable:
	clk_disable_unprepare(gptc->gateclk);
out:
	return ret;
}

static void gptc_clock_disable(struct gptc *gptc)
{
	if (!IS_ERR_OR_NULL(gptc->gateclk))
		clk_disable_unprepare(gptc->gateclk);

	if (!IS_ERR_OR_NULL(gptc->freqclk))
		clk_disable_unprepare(gptc->freqclk);
}

static void __gptc_release(struct kref *ref)
{
	struct gptc *gptc = container_of(ref, struct gptc, ref);

	gptc_clock_disable(gptc);
	list_del(&gptc->next);
	kfree(gptc);
}

static int gptc_get(struct gptc *gptc)
{
	if (!gptc)
		return 0;
	kref_get(&gptc->ref);
	return 1;
}

static void gptc_put(struct gptc *gptc)
{
	if (!gptc)
		return;

	kref_put(&gptc->ref, __gptc_release);
}

static void gptc_free_timer(struct gptc_timer *timer)
{
	struct gptc *gptc = timer->gptc;
	unsigned long flags;

	spin_lock_irqsave(&gptc->lock, flags);

	switch (timer->type) {
	case TIMER_TYPE_CLK_SRC:
		list_del(&timer->clksrc);
		break;
	case TIMER_TYPE_HEARTBEAT:
		list_del(&timer->heartbeat);
		break;
	case TIMER_TYPE_CLK_EVT:
		list_del(&timer->clkevt);
		break;
	case TIMER_TYPE_WDT:
		list_del(&timer->wdt);
		break;
	default:
		break;
	}

	spin_unlock_irqrestore(&gptc->lock, flags);
	kfree(timer);
}

static int gptc_of_parse_timer(struct gptc *gptc)
{
	u32 type;
	struct of_phandle_args clkspec;
	int index, ret, nr_timers;
	struct gptc_timer *timer, *timer_list[GPTC_MAX];
	u32 tid;
	u32 cpuid;
	struct device_node *np = gptc->np;

	nr_timers = of_count_phandle_with_args(np, "intel,clk", "#gptc-cells");
	if ((nr_timers <= 0) || (nr_timers >= GPTC_MAX)) {
		pr_err("gptc%d: invalid value of phandler property at %s\n",
		       gptc->id, np->full_name);
		return -ENODEV;
	}

	pr_debug("%s nr_timers %d available\n", __func__, nr_timers);
	for (index = 0; index < nr_timers; index++) {
		ret = of_parse_phandle_with_args(np, "intel,clk", "#gptc-cells",
						 index, &clkspec);
		if (ret < 0) {
			pr_err("gptc%d:gptc-cells invalid phandle\n", gptc->id);
			goto err;
		}
		pr_debug("%s args_count %d arg[0] %d arg[1] %d arg[2] %d\n",
			 __func__, clkspec.args_count, clkspec.args[0],
			 clkspec.args[1], clkspec.args[2]);

		type = clkspec.args[0];
		tid = clkspec.args[1];
		cpuid = clkspec.args[2];

		/* Ignore CPU id check */
		if (type > TIMER_TYPE_MAX || tid > (TIMER_PER_GPTC - 1)) {
			pr_err("%s invalid clk type %d or timer id %d\n",
			       __func__, type, tid);
			ret = -EINVAL;
			goto err;
		}

		timer = kzalloc(sizeof(*timer), GFP_KERNEL);
		if (!timer) {
			ret = -ENOMEM;
			goto err;
		}

		timer_list[index] = timer;

		INIT_LIST_HEAD(&timer->child);
		timer->gptc = gptc;
		timer->base = gptc->base;
		timer->phy_base = gptc->phy_base;
		timer->gptcid = gptc->id;
		timer->tid = tid;
		timer->type = type;
		timer->frequency = gptc->fpifreq;
		timer->used = false;
		timer->irq_registered = false;
		timer->call_back = NULL;
		list_add_tail(&timer->child, &gptc->parent);
		switch (type) {
		case TIMER_TYPE_CLK_SRC:
			INIT_LIST_HEAD(&timer->clksrc);
			timer->irq = 0;
			timer->dir = GPTC_COUNT_UP;
			timer->cpuid = 0;
			list_add_tail(&timer->clksrc, &gptc_clksrc_list);
			break;
		case TIMER_TYPE_HEARTBEAT:
			INIT_LIST_HEAD(&timer->heartbeat);
			timer->irq = 0;
			timer->dir = GPTC_COUNT_DOWN;
			timer->cpuid = 0;
			timer->cycles = ((clkspec.args_count > 3)
					 ? clkspec.args[3] : 1);
			list_add_tail(&timer->heartbeat, &gptc_heartbeat_list);
			break;
		case TIMER_TYPE_CLK_EVT:
		case TIMER_TYPE_WDT:
			timer->irq = irq_of_parse_and_map(np, timer->tid);
			WARN_ON(timer->irq <= 0);
			timer->dir = GPTC_COUNT_DOWN;
			timer->cpuid = cpuid;
			if (type == TIMER_TYPE_CLK_EVT) {
				INIT_LIST_HEAD(&timer->clkevt);
				list_add_tail(&timer->clkevt,
					      &gptc_clkevt_list);
			} else {
				INIT_LIST_HEAD(&timer->wdt);
				list_add_tail(&timer->wdt, &gptc_wdt_list);
			}
			break;
		case TIMER_TYPE_HT_YIELD:
			timer->irq = irq_of_parse_and_map(np, timer->tid);
			timer->dir = GPTC_COUNT_DOWN;
			timer->cpuid = 0;
			list_add_tail(&timer->ht_yield, &gptc_ht_yield_list);
			break;
		default:
			break;
		}
	}
	return 0;

err:
	while (--index >= 0)
		gptc_free_timer(timer_list[index]);

	return ret;
}

static int gptc_of_init(struct device_node *np)
{
	int ret;
	u32 gptcid;
	struct resource res;
	void __iomem *base;
	struct gptc *gptc;

	/* Which GPTC is being handled */
	gptcid = of_alias_get_id(np, "timer");
	if (gptcid >= (GPTC_MAX - 1))
		return -EINVAL;

	ret = of_address_to_resource(np, 0, &res);
	if (WARN_ON(ret))
		return ret;

	base = of_iomap(np, 0);
	if (!base) {
		pr_err("Can't map GPTC base address\n");
		return -ENXIO;
	}
	gptc = kzalloc(sizeof(*gptc), GFP_KERNEL);
	if (!gptc)
		goto err_iomap;

	INIT_LIST_HEAD(&gptc->parent);
	INIT_LIST_HEAD(&gptc->next);
	spin_lock_init(&gptc->lock);
	kref_init(&gptc->ref);
	gptc->np = np;
	gptc->id = gptcid;
	gptc->base = base;
	gptc->phy_base = res.start;

	ret = gptc_clock_init(gptc);
	if (ret)
		goto err_clk_init;

	ret = gptc_clock_enable(gptc);
	if (ret)
		goto err_clk_en;

	ret = gptc_of_parse_timer(gptc);
	if (ret)
		goto err_parse_fail;

	list_add_tail(&gptc->next, &gptc_list);

	/* GPTC level initialization */
	gptc_global_init(gptc);
	gptc_of_config_print(gptc);
	return 0;
err_parse_fail:
	gptc_clock_disable(gptc);
err_clk_en:
	gptc_clock_deinit(gptc);
err_clk_init:
	kfree(gptc);
err_iomap:
	iounmap(base);
	return ret;
}

static struct gptc_clocksource gptc_clksrc = {
	.cs = {
		.name = "gptc",
		.read = gptc_hpt_read,
		.mask = CLOCKSOURCE_MASK(32),
		.flags = CLOCK_SOURCE_IS_CONTINUOUS
			| CLOCK_SOURCE_SUSPEND_NONSTOP,
		.rating = 250,
	},
};

cycle_t gptc_read_count(void)
{
	return gptc_hpt_read(&gptc_clksrc.cs);
}

#ifdef CONFIG_GPTC_SCHED_CLOCK
static unsigned long sched_clock_mult __read_mostly;
static cycle_t cycle_last, cycle_offset;
static DEFINE_SPINLOCK(gptc_shed_lock);

unsigned long long notrace sched_clock(void)
{
	cycle_t cycle;
	unsigned long flags;

	if (!gptc_clksrc_init)
		return (jiffies_64 - INITIAL_JIFFIES) * (1000000000 / HZ);

	spin_lock_irqsave(&gptc_shed_lock, flags);
	cycle = gptc_read_count();
	cycle &= gptc_clksrc.cs.mask;
	/* Counter wrapped */
	if (unlikely(cycle_last > cycle))
		cycle_offset += BIT_ULL(32);

	cycle_last = cycle;
	cycle += cycle_offset;
	spin_unlock_irqrestore(&gptc_shed_lock, flags);

	return cycle * sched_clock_mult;
}
#endif /* CONFIG_GPTC_SCHED_CLOCK */

#ifndef CONFIG_X86
static u64 __maybe_unused notrace gptc_read_sched_clock(void)
{
	return (u64)gptc_read_count();
}
#endif /* CONFIG_X86 */

static int gptc_clocksource_init(void)
{
	int ret;
	struct gptc_timer *timer;

	list_for_each_entry(timer, &gptc_clksrc_list, clksrc) {
		if (!timer->used) {
			/* Only one clock source from GPTC allowed */
			if (gptc_clksrc_init)
				return -EEXIST;
			/* Record for VDSO */
			gptc_phy_base = timer->phy_base;
			gptc_clksrc.timer = timer;
			/*
			 * Calculate a somewhat reasonable rating value
			 * in 10MHz
			 */
			gptc_clksrc.cs.rating =
				250 + timer->frequency / 10000000;
			gptc_per_timer_init(timer);
			ret = clocksource_register_hz(&gptc_clksrc.cs,
						      timer->frequency);
		#ifndef CONFIG_X86
			sched_clock_register(gptc_read_sched_clock,
					     32, timer->frequency);
		#endif /* CONFIG_X86 */

		#ifdef CONFIG_GPTC_SCHED_CLOCK
			sched_clock_mult = NSEC_PER_SEC / timer->frequency;
		#endif /* CONFIG_GPTC_SCHED_CLOCK */
			timer->used = true;
			gptc_clksrc_init = true;
			gptc_get(timer->gptc);
			pr_debug("gptc %d timer %d clk src register @cpu%d\n",
				 timer->gptcid, timer->tid, timer->cpuid);
			return 0;
		}
	}
	return -EINVAL;
}

static struct clock_event_device gptc_per_timer_clockevent = {
	.name = "gptc_clockevent",
	.features = CLOCK_EVT_FEAT_ONESHOT | CLOCK_EVT_FEAT_PERIODIC,
	.rating = 250,
	.set_state_shutdown = gptc_clkevt_shutdown,
	.set_state_periodic = gptc_clkevt_periodic,
	.set_state_oneshot = gptc_clkevt_shutdown,
	.set_next_event = gptc_clkevt_next_event,
	.tick_resume = gptc_clkevt_resume,
	.irq = 0,
};

static int gptc_clockevent_cpu_init(unsigned int cpu,
				    struct gptc_clockevent *cd)
{
	struct gptc_timer *timer;
	struct clock_event_device *ce;

	if (!cd)
		return -EINVAL;

	ce = &cd->ce;

	list_for_each_entry(timer, &gptc_clkevt_list, clkevt) {
		if (!timer->used && (timer->cpuid == cpu)) {
			memcpy(ce, &gptc_per_timer_clockevent, sizeof(*ce));
			ce->irq = timer->irq;
			ce->cpumask = cpumask_of(cpu);
			ce->name = cd->name;
			gptc_per_timer_init(timer);
			clockevents_config_and_register(ce, timer->frequency,
							0x64, 0x7FFFFFFF);
			gptc_irq_unmask(timer);
			gptc_get(timer->gptc);
			timer->used = true;
			pr_debug("gptc %d timer %d clk evt register @cpu%d\n",
				 timer->gptcid, timer->tid, timer->cpuid);
			return 0;
		}
	}
	return -EINVAL;
}

static int gptc_clockevent_cpu_exit(struct gptc_clockevent *cd)
{
	struct clock_event_device *levt;
	struct gptc_timer *timer;
	struct gptc *gptc;

	if (!cd)
		return -EINVAL;

	levt = &cd->ce;
	timer = cd->timer;

	if (!timer)
		return -EINVAL;

	if (levt->irq) {
		free_irq(levt->irq, cd);
			levt->irq = 0;
	}

	gptc_irq_mask(timer);
	list_del(&timer->clkevt);
	gptc = timer->gptc;
	kfree(timer);
	gptc_put(gptc);
	return 0;
}

static void gptc_clkevt_irq_init(void)
{
	int ret;
	int cpu;
	struct gptc_timer *timer;
	struct gptc_clockevent *gptc_evt;

	for_each_possible_cpu(cpu) {
		gptc_evt = per_cpu_ptr(&gptc_event_device, cpu);
		list_for_each_entry(timer, &gptc_clkevt_list, clkevt) {
			if (!timer->irq_registered && (timer->cpuid == cpu)) {
				gptc_evt->timer = timer;
				gptc_evt->ticks_per_jiffy =
					DIV_ROUND_UP(timer->frequency, HZ);
				snprintf(gptc_evt->name, sizeof(gptc_evt->name),
					 "gptc_event%d", cpu);
				ret = request_irq(timer->irq,
						  gptc_timer_interrupt,
						  IRQF_TIMER | IRQF_NOBALANCING,
						  gptc_evt->name, gptc_evt);
				if (ret) {
					pr_err("gptc irq %d register failed @cpu%d\n",
					       timer->irq, cpu);
					break;
				}
				irq_set_affinity(timer->irq, cpumask_of(cpu));
				timer->irq_registered = true;
				pr_debug("gptc %d timer %d irq %d register @cpu%d\n",
					 timer->gptcid, timer->tid, timer->irq,
					 timer->cpuid);
			}
		}
	}
}

static int gptc_starting_cpu(unsigned int cpu)
{
	gptc_clockevent_cpu_init(cpu, this_cpu_ptr(&gptc_event_device));
	return 0;
}

static int gptc_dying_cpu(unsigned int cpu)
{
	gptc_clockevent_cpu_exit(this_cpu_ptr(&gptc_event_device));
	return 0;
}

static void gptc_clkevent_init(void)
{
	gptc_clkevt_irq_init();
	cpuhp_setup_state(CPUHP_AP_INTEL_GPTC_TIMER_STARTING,
			  "AP_INTEL_GPTC_TIMER_STARTING", gptc_starting_cpu,
			  gptc_dying_cpu);
}

static int __init gptc_timer_init(struct device_node *np)
{
	gptc_of_init(np);

	gptc_clocksource_init();

	/* Register immediately the clock event on BSP */
	gptc_clkevent_init();
#ifdef CONFIG_X86
	global_clock_event = &gptc_per_timer_clockevent;
#endif
	return 0;
}

CLOCKSOURCE_OF_DECLARE(lantiq_gptc_timer, "lantiq,gptc", gptc_timer_init);
CLOCKSOURCE_OF_DECLARE(intel_gptc_timer, "intel,gptc", gptc_timer_init);

static int heartbeat_init(void)
{
	struct gptc_timer *timer;

	list_for_each_entry(timer, &gptc_heartbeat_list, heartbeat) {
		if (!timer->used) {
			gptc_per_timer_init(timer);
			timer->used = true;
			gptc_get(timer->gptc);
			pr_debug("gptc %d timer %d heartbeat register @cpu%d\n",
				 timer->gptcid, timer->tid, timer->cpuid);
			return 0;
		}
	}
	return -EINVAL;
}

static int __init gptc_heartbeat_init(void)
{
	return heartbeat_init();
}
arch_initcall(gptc_heartbeat_init);

#if (IS_ENABLED(CONFIG_LTQ_VMB) && IS_ENABLED(CONFIG_LTQ_UMT_SW_MODE))
#define GPTC_TC_THREAD_STACK_SIZE 4096
#define GPTC_TC_THREAD_STACK_RESERVED_SIZE (32 + sizeof(struct pt_regs))

static u8 tc_thread_stack[GPTC_TC_THREAD_STACK_SIZE] __aligned(16);
static u8 tc_thread_gp[GPTC_TC_THREAD_STACK_SIZE] __aligned(16);

static void tc_thread(u32 arg0, u32 arg1)
{
	u32 mask, yqmask;
	struct gptc_timer *timer = (struct gptc_timer *)arg0;

	/* init the yq mask */
	mask = (1 << timer->yield_pin);
	yqmask = read_c0_yqmask();
	yqmask |= mask;
	write_c0_yqmask(yqmask);
	ehb();

	while (1) {
		mips_mt_yield(mask);

		if (gptc_irq_read(timer)) {
			gptc_irq_ack(timer);
			gic_clear_edge(timer->irq);

			gptc_stop_counter(timer);
			/* Do the call back stuff */
			if (timer->call_back)
				timer->call_back(timer->call_back_param);
			gptc_mode_setup(timer, true);
			gptc_reload_and_run(timer);
		}
	}
}

static irqreturn_t tc_dummy_isr(int irq, void *dev_id)
{
	return IRQ_HANDLED;
}

int gptc_ht_yield_init(struct gptc_ht_yield *param, void *call_back,
		       void *call_back_param)
{
	struct gptc_timer *timer;
	int tc_num, cpu;
	unsigned long cycles, interval;
	struct TC_launch_t tc_launch;
	int ret;

	if (!param || !call_back ||
	    param->interval == 0) {
		pr_err("Bad parameter.\n");
		return -EINVAL;
	}

	list_for_each_entry(timer, &gptc_ht_yield_list, ht_yield) {
		if (!timer->used) {
			cpu = smp_processor_id();
			gptc_per_timer_init(timer);
			timer->used = true;
			gptc_get(timer->gptc);
			timer->call_back = call_back;
			timer->call_back_param = call_back_param;
			timer->yield_pin = param->yield_pin;
			pr_debug("gptc %d timer %d ht_yield register @cpu%d\n",
				 timer->gptcid, timer->tid, timer->cpuid);

			/* enable this irq and mark this as gptc_yield_irq */
			ret = request_irq(timer->irq, tc_dummy_isr, 0,
					  "gptc_yield_irq", NULL);
			if (ret) {
				pr_err("%s: failed to request gptu irq - %d",
				       __func__, timer->irq);
				return -ENOENT;
			}

			gptc_irq_unmask(timer);
			ret = gic_yield_setup(cpu, param->yield_pin,
					      timer->irq);
			if (ret) {
				pr_err("Yield Setup Fail:\n");
				return -ENOENT;
			}

			pr_debug("Yield setup passed, gptc_irq = %d\n",
				 timer->irq);

			tc_num = vmb_tc_alloc(cpu);
			if (tc_num < 0) {
				pr_err("%s: failed to request vmb tc - %d",
				       __func__, tc_num);
				return -ENOENT;
			}
			memset(&tc_launch, 0, sizeof(tc_launch));
			tc_launch.tc_num = tc_num;
			tc_launch.start_addr = (u32)&tc_thread;
			tc_launch.sp = (u32)&tc_thread_stack +
					GPTC_TC_THREAD_STACK_SIZE -
					GPTC_TC_THREAD_STACK_RESERVED_SIZE;
			tc_launch.sp &= ~0xF;  /*padding to 16 bytes*/
			tc_launch.gp = (u32)&tc_thread_gp;
			tc_launch.priv_info = (u32)timer;
			ret = vmb_run_tc(cpu, &tc_launch);
			if (ret) {
				pr_err("Failed to run the vmb TC.\n");
				return -ENOENT;
			}

			interval = param->interval;
			/* interval is the unit of microsecond */
			cycles = interval * (timer->frequency / 1000000);
			gptc_mode_setup(timer, true);
			gptc_reload_counter(timer, cycles);
			gptc_reload_and_run(timer);

			return 0;
		}
		pr_err("only one ht_yield timer can be supported!\n");
		return -EINVAL;
	}
	return -EINVAL;
}
#else
int gptc_ht_yield_init(struct gptc_ht_yield *param, void *call_back,
		       void *call_back_param)
{
	return -EINVAL;
}
#endif
EXPORT_SYMBOL(gptc_ht_yield_init);

int gptc_ht_yield_interval(u32 interval)
{
	unsigned long cycles;
	struct gptc_timer *timer;

	if (interval == 0) {
		pr_err("Bad parameter.\n");
		return -EINVAL;
	}
	list_for_each_entry(timer, &gptc_ht_yield_list, ht_yield) {
		if (timer->used) {
			cycles = interval * (timer->frequency / 1000000);
			gptc_stop_counter(timer);
			gptc_reload_counter(timer, cycles);
			gptc_reload_and_run(timer);
		}
	}
	return 0;
}
EXPORT_SYMBOL(gptc_ht_yield_interval);

static void *gptc_seq_start(struct seq_file *s, loff_t *pos)
{
	if (list_empty(&gptc_list))
		return NULL;

	return seq_list_start(&gptc_list, *pos);
}

static void *gptc_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	return seq_list_next(v, &gptc_list, pos);
}

static void gptc_seq_stop(struct seq_file *s, void *v)
{
}

static int gptc_seq_show(struct seq_file *s, void *v)
{
	int i = 0;
	struct gptc *gptc;
	struct gptc_timer *timer;

	gptc = list_entry(v, struct gptc, next);

	seq_printf(s, "GPTC%d base %p phy %lx freq %d\n",
		   gptc->id, gptc->base, gptc->phy_base, gptc->fpifreq);
	seq_printf(s, "CLC %08x ID %08x IRNEN %08x IRNICR %08x IRNCR %08x\n",
		   ioread32(gptc->base + GPTC_CLC),
		   ioread32(gptc->base + GPTC_ID),
		   ioread32(gptc->base + GPTC_IRNEN),
		   ioread32(gptc->base + GPTC_IRNICR),
		   ioread32(gptc->base + GPTC_IRNCR));

	list_for_each_entry(timer, &gptc->parent, child) {
		seq_printf(s, "\ttimer%d base %p freq %d tid %d cpuid %d irq %d clk %s %s\n",
			   i, timer->base, timer->frequency, timer->tid,
			   timer->cpuid, timer->irq,
			   timer_type_to_str(timer->type),
			   timer->used ? "used" : "unused");
		seq_printf(s, "\tCON %08x RUN %08x RLD %08x CNT %08x\n",
			   gptc_readl(timer, GPTC_CON(i)),
			   gptc_readl(timer, GPTC_RUN(i)),
			   gptc_readl(timer, GPTC_RLD(i)),
			   gptc_readl(timer, GPTC_CNT(i)));
		i++;
	}
	seq_putc(s, '\n');
	return 0;
}

static const struct seq_operations gptc_seq_ops = {
	.start = gptc_seq_start,
	.next = gptc_seq_next,
	.stop = gptc_seq_stop,
	.show = gptc_seq_show,
};

static int gptc_open(struct inode *inode, struct file *file)
{
	int err;

	err = seq_open(file, &gptc_seq_ops);
	if (err)
		return err;
	return 0;
}

static const struct file_operations gptc_ops = {
	.owner = THIS_MODULE,
	.open = gptc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

#if IS_ENABLED(CONFIG_DEBUG_FS)
static int gptc_debugfs_init(void)
{
	struct dentry *file;
	struct dentry *debugfs;

	debugfs = debugfs_create_dir("gptc", NULL);
	if (!debugfs)
		return -ENOMEM;

	file = debugfs_create_file("status", 0400, debugfs, NULL, &gptc_ops);
	if (!file)
		goto remove;

	return 0;
remove:
	debugfs_remove_recursive(debugfs);
	debugfs = NULL;
	return -ENOMEM;
}
late_initcall(gptc_debugfs_init);
#endif

static const struct of_device_id gptc_match[] = {
	{
		.compatible = "intel,prx300-gptc",
	},
	{}
};

static int gptc_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;

	gptc_of_init(np);

#ifdef CONFIG_GPTC_SCHED_CLOCK
	gptc_clocksource_init();
#endif /* CONFIG_GPTC_SCHED_CLOCK */

	/* Register immediately the clock event on BSP */
	gptc_clkevent_init();

	heartbeat_init();
	return 0;
}

/**
 * GPTC might not be used as clock source in PRX300.
 * GPTC driver for prx300 has same function scopes as other soc,
 * but it uses platform device instead of clock source init API
 * to better support scenarios that it is not used as clock source.
 */
static struct platform_driver gptc_drv = {
	.probe = gptc_probe,
	.driver = {
		.name = "gptc",
		.of_match_table = gptc_match,
	}
};

builtin_platform_driver(gptc_drv);
