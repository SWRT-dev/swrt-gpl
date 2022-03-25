/*******************************************************************************
 *
 *  Copyright (C) 2016 Intel Corporation.
 *
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
 *
 ******************************************************************************/

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <linux/of.h>

#include <asm/time.h>

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
	unsigned long cpuclk;
	struct device_node *np;
	struct clk *clk;

	of_clk_init(NULL);

	np = of_get_cpu_node(0, NULL);
	if (!np) {
		pr_err("Failed to get CPU node\n");
		return;
	}

	clk = of_clk_get_by_name(np, "cpu");
	if (IS_ERR(clk)) {
		pr_err("Failed to get CPU clock: %ld\n", PTR_ERR(clk));
		return;
	}

	cpuclk = clk_get_rate(clk);
	mips_hpt_frequency = cpuclk / get_counter_resolution();
	clk_put(clk);

	write_c0_compare(read_c0_count());
	pr_info("CPU Clock: %ldHz  mips_hpt_frequency %dHz\n",
		cpuclk, mips_hpt_frequency);
	clocksource_probe();
}
