/*
 * Copyright (c) 2018, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */
#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/pm_opp.h>
#include <linux/cpufreq.h>
#include <lantiq_soc.h>

#define VERSIONS_COUNT 1

/* OPP supported_hw definition for XRX500/PRX300 Family SoC.
   This enables us to select only a subset of OPPs from the
   larger OPP table, based on what version of the hardware we are running on.
   The representation is 1 bit per version, max 32 bit.
   This opp_table->supported_hw will be bitwise mask with OPP-DT paramter
   opp-supported-hw. */
#define SOC_HW_ID_GRX5838	0x1
#define SOC_HW_ID_GRX5828	0x2
#define SOC_HW_ID_GRX5628	0x4
#define SOC_HW_ID_GRX5821	0x8
#define SOC_HW_ID_GRX5831	0x10
#define SOC_HW_ID_GRX58312	0x20
#define SOC_HW_ID_GRX3506	0x40
#define SOC_HW_ID_GRX3508	0x80
#define SOC_HW_ID_IRX200	0x100
#define SOC_HW_ID_FMX_FIBERPHY	0x1000
#define SOC_HW_ID_FMX_SFU	0x2000
#define SOC_HW_ID_FMX_SFP	0x4000
#define SOC_HW_ID_FMX_DPU	0x8000

static const struct of_device_id machines[] = {
	{ .compatible = "lantiq,xrx500", },
	{ .compatible = "intel,prx300", },
	{ }
};

extern unsigned long loops_per_jiffy;
static int xrx500_cpufreq_transition_notifier(struct notifier_block *nb,
					unsigned long val, void *data);

/* MIPS cpu_data[] structure has no per_cpu jiffiy variable like arm.
   Therefore we just have one global loops_per_jiffy. The adjust_jiffies()
   function in the core cpufreq.c module is active only for none SMP systems.
   xrx500 has CONFIG_SMP set, therefore we need this implementation here.
   CPUFREQ_TRANSITION_NOTIFIER is called for every CPU running Linux.
   That means we have to consider that loops_per_jiffy is changed only once
   during one frequency transition.
*/
static int one_trans;
static struct notifier_block xrx500_cpufreq_transition_notifier_block = {
	.notifier_call = xrx500_cpufreq_transition_notifier
};

static int xrx500_cpufreq_transition_notifier(struct notifier_block *nb,
					unsigned long val, void *data)
{
	struct cpufreq_freqs *freq = data;

	if (val == CPUFREQ_PRECHANGE)
		one_trans = 1;

	if (val == CPUFREQ_POSTCHANGE) {
		if (one_trans) {
			pr_debug("cpu frequency change: %d kHz\n", freq->new);
			loops_per_jiffy = cpufreq_scale(loops_per_jiffy,
							freq->old,
							freq->new);
			one_trans = 0;
		}
		/* adjust udelay for each cpu */
		cpu_data[freq->cpu].udelay_val = loops_per_jiffy;

	}
	return NOTIFY_OK;
}

static unsigned int xrx500_pnum2version(struct device *dev, unsigned int id)
{
	switch (id) {
	case SOC_ID_GRX5838:
		return SOC_HW_ID_GRX5838;
	case SOC_ID_GRX5828:
		return SOC_HW_ID_GRX5828;
	case SOC_ID_GRX5628:
		return SOC_HW_ID_GRX5628;
	case SOC_ID_GRX5821:
		return SOC_HW_ID_GRX5821;
	case SOC_ID_GRX5831:
		return SOC_HW_ID_GRX5831;
	case SOC_ID_GRX58312:
		return SOC_HW_ID_GRX58312;
	case SOC_ID_GRX3506:
		return SOC_HW_ID_GRX3506;
	case SOC_ID_GRX3508:
		return SOC_HW_ID_GRX3508;
	case SOC_ID_IRX200:
		return SOC_HW_ID_IRX200;
	case SOC_ID_FMX_FIBERPHY:
		return SOC_HW_ID_FMX_FIBERPHY;
	case SOC_ID_FMX_SFU:
		return SOC_HW_ID_FMX_SFU;
	case SOC_ID_FMX_SFP:
		return SOC_HW_ID_FMX_SFP;
	case SOC_ID_FMX_DPU:
		return SOC_HW_ID_FMX_DPU;
	default:
		return -EINVAL;
	}
}


int xrx500_opp_set_supported_hw(struct device *cpu_dev)
{
	unsigned int version, id;
	int ret;

	id = ltq_get_cpu_id();

	version = xrx500_pnum2version(cpu_dev, id);
	if (version < 0) {
		dev_err(cpu_dev, "unknown chip id (0x%x)\n", id);
		return ret;
	}

	ret = dev_pm_opp_set_supported_hw(cpu_dev, &version, VERSIONS_COUNT);
	if (ret) {
		dev_err(cpu_dev, "Failed to set supported hardware\n");
		return ret;
	}

	return ret;
}

static int xrx500_cpufreq_driver_init(void)
{
	struct platform_device_info devinfo = { .name = "cpufreq-dt", };
	struct device_node *np = of_find_node_by_path("/");
	struct device *cpu_dev;
	const struct of_device_id *match;
	int ret;

	if (!np)
		return -ENODEV;

	match = of_match_node(machines, np);
	of_node_put(np);
	if (!match)
		return -ENODEV;

	cpu_dev = get_cpu_device(0);
	if (!cpu_dev) {
		pr_err("failed to get cpu device\n");
		return -ENODEV;
	}

	ret = xrx500_opp_set_supported_hw(cpu_dev);
	if (ret)
		return ret;

	if (cpufreq_register_notifier(
				&xrx500_cpufreq_transition_notifier_block,
				CPUFREQ_TRANSITION_NOTIFIER)) {
		pr_err("Fail in registering xrx500-cpufreq to CPUFreq\n");
		return -ENODEV;
	}

	/* Instantiate cpufreq-dt */
	platform_device_register_full(&devinfo);

	return ret;

}
module_init(xrx500_cpufreq_driver_init);

MODULE_DESCRIPTION("XRX500/PRX300 Family SoC CPUFreq driver");
MODULE_AUTHOR("Waldemar Rymarkiewicz <waldemarx.rymarkiewicz@intel.com>");
MODULE_LICENSE("GPL v2");
