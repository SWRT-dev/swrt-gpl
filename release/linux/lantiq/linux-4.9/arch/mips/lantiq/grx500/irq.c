/*
 *  Copyright (C) 2016 Intel Corporation.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 */
#include <linux/init.h>
#include <linux/irqchip.h>
#include <linux/of_irq.h>
#include <linux/irqchip/mips-gic.h>

#include <asm/traps.h>
#include <asm/irq_cpu.h>
#include <asm/mips-cm.h>

void __init arch_init_irq(void)
{
	struct device_node *intc_node;

	pr_info("EIC is %s\n", cpu_has_veic ? "on" : "off");
	pr_info("VINT is %s\n", cpu_has_vint ? "on" : "off");

	intc_node = of_find_compatible_node(NULL, NULL,
					    "mti,cpu-interrupt-controller");
	if (!cpu_has_veic && !intc_node)
		mips_cpu_irq_init();

	irqchip_init();
}

/* NB: The following twos are needed since they are different
 * from the default behavior
 */
int get_c0_perfcount_int(void)
{
	return gic_get_c0_perfcount_int();
}
EXPORT_SYMBOL_GPL(get_c0_perfcount_int);

unsigned int get_c0_compare_int(void)
{
	return gic_get_c0_compare_int();
}
