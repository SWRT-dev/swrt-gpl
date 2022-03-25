/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 * Copyright (C) 2010 Thomas Langer <thomas.langer@lantiq.com>
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */
#include <linux/io.h>
#include <linux/export.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/err.h>
#include <linux/list.h>
#include <asm/time.h>
#include <asm/div64.h>
#include <asm/setup.h>
#include <lantiq_soc.h>
#include "clk.h"
#include "prom.h"
#include <grx500_bootcore_time.h>

#ifdef CONFIG_USE_EMULATOR
#define EMULATOR_TEP_CPU_SPEED    GRX500_BOOTCORE_CPU_CLK
#endif /*CONFIG_USE_EMULATOR*/

static int mips_cpu_timer_irq;

//extern void ltq_soc_init(void);

int ltq_grx500_set_cpu_hz(unsigned long cpu_freq)
{
#ifdef CONFIG_LTQ_CPU_FREQ
        return ltq_set_cpu_hz(CLK_AR10, avail_rates_xrx390,
                                freq_transition_xrx300);

#else
        printk("[%s]:[%d] \n", __FUNCTION__, __LINE__);
        return -1;
#endif /* CONFIG_LTQ_CPU_FREQ */
}

unsigned long ltq_grx500_cpu_hz(void)
{
#ifdef CONFIG_USE_EMULATOR
        return EMULATOR_TEP_CPU_SPEED;
#else
        unsigned int clksys = CLOCK_300M;

#ifdef CONFIG_SOC_PRX300_BOOTCORE
		clksys = CLOCK_500M;
#endif
        return clksys;
#endif
}

struct clk *clk_get_cpu(void)
{
	return clk_get_sys("cpu", "cpu");
}
EXPORT_SYMBOL_GPL(clk_get_cpu);

static inline int clk_good(struct clk *clk)
{
	return clk && !IS_ERR(clk);
}

unsigned long clk_get_rate(struct clk *clk)
{
	if (unlikely(!clk_good(clk)))
		return 0;

#ifndef CONFIG_CPU_FREQ
	if (clk->rate != 0)
		return clk->rate;
#endif

	if (clk->get_rate != NULL) {
		clk->rate = clk->get_rate();
		return clk->rate;
	}
#if 0
	if (clk->parent) {
		pr_info("derive real clock speed for %s from clock %s ",
			clk->cl.dev_id,
			cpu_clk_generic[clk->parent - 1].cl.dev_id);
		pr_info("with scale data 0x%08X\n", clk->scale_data);
		return cpu_clk_generic[clk->parent - 1].rate;
	}
#endif
	return 0;
}
EXPORT_SYMBOL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	int i = 0;

	if (unlikely(!clk_good(clk)))
		return 0;

	if (clk->rates && *clk->rates) {
		unsigned long *r = clk->rates;

		while (*r && (*r != rate)) {
			r++;
			i++;
		}
		if (!*r) {
			pr_err("clk %s.%s: trying to set invalid rate %ld\n",
				clk->cl.dev_id, clk->cl.con_id, rate);
			return -1;
		}
	}

	if (clk->set_rate != NULL) {
		if (clk->set_rate(rate) == 0) {
			clk->rate = rate;
			return 0;
		}
	}

	pr_err("trying to set invalid clk %s.%s: rate %ld.\n",
			clk->cl.dev_id, clk->cl.con_id, rate);
	pr_err("clk_set_rate not supported for this clk.\n");
	return -EINVAL;
}
EXPORT_SYMBOL(clk_set_rate);

int clk_enable(struct clk *clk)
{
	if (unlikely(!clk_good(clk)))
		return -1;

	if (clk->enable)
		return clk->enable(clk);

	return -1;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
	if (unlikely(!clk_good(clk)))
		return;

	if (clk->disable)
		clk->disable(clk);
}
EXPORT_SYMBOL(clk_disable);

int clk_activate(struct clk *clk)
{
	if (unlikely(!clk_good(clk)))
		return -1;

	if (clk->activate)
		return clk->activate(clk);

	return -1;
}
EXPORT_SYMBOL(clk_activate);

void clk_deactivate(struct clk *clk)
{
	if (unlikely(!clk_good(clk)))
		return;

	if (clk->deactivate)
		clk->deactivate(clk);
}
EXPORT_SYMBOL(clk_deactivate);

struct clk *of_clk_get_from_provider(struct of_phandle_args *clkspec)
{
	return NULL;
}

static void mips_timer_dispatch(void)
{
        do_IRQ(mips_cpu_timer_irq);
}

#if 1 //WW:
unsigned int /*__init*/ get_c0_compare_int(void)
#else
unsigned int __cpuinit get_c0_compare_int(void)
#endif
{
	if (cpu_has_veic) {
		set_vi_handler(31, mips_timer_dispatch);
                mips_cpu_timer_irq = 31;
	}

        return mips_cpu_timer_irq;
}

static inline u32 get_counter_resolution(void)
{
	u32 res;

	__asm__ __volatile__(
		".set	push\n"
		".set	mips32r2\n"
		"rdhwr	%0, $3\n"
		".set pop\n"
		: "=&r" (res)
		: /* no input */
		: "memory");

	return res;
}

void __init plat_time_init(void)
{
	struct clk *clk;

	ltq_soc_init();

	clk = clk_get_sys("cpu", "cpu");
	if (clk == NULL) {
		pr_err("CPU clock structure not found\n");
		return;
	}
	mips_hpt_frequency = clk_get_rate(clk) / get_counter_resolution();
	write_c0_compare(read_c0_count());
#ifdef CONFIG_USE_EMULATOR
//        mips_hpt_frequency = 16000000;
//		mips_hpt_frequency = 240000000;
        pr_info("CPU Clock: %ldKHz\n", clk_get_rate(clk) / 1000);
#else
        pr_info("CPU Clock: %ldMHz\n", clk_get_rate(clk) / 1000000);
#endif
	clk_put(clk);
}
