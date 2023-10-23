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
#include <linux/icc/drv_icc.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>

#define VERSIONS_COUNT 1
#define MAX_SCALING_FREQ 8
/* CGU Register: PLL0A_CFG1 */
#define CGU_PLL0A_CFG2	0xC
/* bitfield FBDIV / REFDIV */
#define FBDIV		0xFFF
#define REFDIV		0xFC0000

/* OPP supported_hw definition for XRX500/PRX300 Family SoC.
 * This enables us to select only a subset of OPPs from the
 * larger OPP table, based on what version of the hardware we are running on.
 * The representation is 1 bit per version, max 32 bit.
 * This opp_table->supported_hw will be bitwise mask with OPP-DT parameter
 * opp-supported-hw.
 */
#define SOC_HW_ID_GRX5838	0x1
#define SOC_HW_ID_GRX5828	0x2
#define SOC_HW_ID_GRX5628	0x4
#define SOC_HW_ID_GRX5821	0x8
#define SOC_HW_ID_GRX5831	0x10
#define SOC_HW_ID_GRX58312	0x20
#define SOC_HW_ID_GRX3506	0x40
#define SOC_HW_ID_GRX3508	0x80
#define SOC_HW_ID_IRX200	0x100
#define SOC_HW_ID_PRX_FIBERPHY	0x1000
#define SOC_HW_ID_PRX_SFU	0x2000
#define SOC_HW_ID_PRX_SFP	0x4000
#define SOC_HW_ID_PRX_DPU	0x8000
#define PLF_GRX500		0xAB
#define PLF_PRX			0xAC

static const struct of_device_id machines[] = {
	{ .compatible = "lantiq,xrx500", },
	{ .compatible = "intel,prx300", },
	{ }
};

extern unsigned long loops_per_jiffy;
static int xrx500_cpufreq_transition_notifier(struct notifier_block *nb,
					      unsigned long val, void *data);

/* MIPS cpu_data[] structure has no per_cpu jiffiy variable like arm.
 * Therefore we just have one global loops_per_jiffy. The adjust_jiffies()
 * function in the core cpufreq.c module is active only for none SMP systems.
 * xrx500 has CONFIG_SMP set, therefore we need this implementation here.
 * CPUFREQ_TRANSITION_NOTIFIER is called for every CPU running Linux.
 * That means we have to consider that loops_per_jiffy is changed only once
 * during one frequency transition.
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

static int xrx500_pnum2version(struct device *dev, unsigned int id,
			       unsigned int *plf)
{
	switch (id) {
	case SOC_ID_GRX5838:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX5838;
	case SOC_ID_GRX5828:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX5828;
	case SOC_ID_GRX5628:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX5628;
	case SOC_ID_GRX5821:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX5821;
	case SOC_ID_GRX5831:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX5831;
	case SOC_ID_GRX58312:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX58312;
	case SOC_ID_GRX3506:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX3506;
	case SOC_ID_GRX3508:
		*plf = PLF_GRX500;
		return SOC_HW_ID_GRX3508;
	case SOC_ID_IRX200:
		*plf = PLF_GRX500;
		return SOC_HW_ID_IRX200;
	case SOC_ID_FMX_FIBERPHY:
		*plf = PLF_PRX;
		return SOC_HW_ID_PRX_FIBERPHY;
	case SOC_ID_FMX_SFU:
		*plf = PLF_PRX;
		return SOC_HW_ID_PRX_SFU;
	case SOC_ID_FMX_SFP:
		*plf = PLF_PRX;
		return SOC_HW_ID_PRX_SFP;
	case SOC_ID_FMX_DPU:
		*plf = PLF_PRX;
		return SOC_HW_ID_PRX_DPU;
	default:
		return -EINVAL;
	}
}

int xrx500_opp_set_supported_hw(struct device *cpu_dev)
{
	unsigned int id, platform;
	int ret = 0, version;

	id = ltq_get_cpu_id();

	version = xrx500_pnum2version(cpu_dev, id, &platform);
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
	int ret, i;
	struct cpufreq_policy *policy;
	struct cpufreq_frequency_table *pos;
	struct cpufreq_frequency_table *table;
	struct regmap *cgu_pllcfg;
	unsigned int val, f_vco_clk;
	unsigned int div, platform;
	icc_msg_t w;

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

	(void)xrx500_pnum2version(cpu_dev, ltq_get_cpu_id(), &platform);
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

	if (platform == PLF_PRX && IS_ENABLED(CONFIG_LTQ_ICC)) {
		/* we need to check what is the current f_vco_clock for pll0a
		 *f_vco_clk = f_osc_clk * FBDIV / REFDIV
		 *f_osc_clk = 40MHz
		 */
		/* get CGU regmap */
		cgu_pllcfg = syscon_regmap_lookup_by_phandle(cpu_dev->of_node,
							"intel,cgu-syscon");
		if (IS_ERR(cgu_pllcfg)) {
			dev_err(cpu_dev, "No phandle for intel,cgu-syscon\n");
			return PTR_ERR(cgu_pllcfg);
		}

		regmap_read(cgu_pllcfg, CGU_PLL0A_CFG2, &val);
		f_vco_clk = 40000 * (val & FBDIV) / ((val & REFDIV) >> 18);

		/* expose scaling frequencies to TEP for policy check
		 * max scaling frequencies supported are MAX_SCALING_FREQ.
		 * We expose directly the valid clk divider value used in
		 * CGU Register: PLL0A_CFG1:PLL_CLK0. Every frequency value is
		 * mapped to a 4-bit divider value.
		 * write.param: bit(3:0)   -> freq1
		 * write.param: bit(7:4)   -> freq2
		 * write.param: bit(11:8)  -> freq3
		 * write.param: bit(15:12) -> freq4
		 * write.param: bit(19:16) -> freq5
		 * write.param: bit(23:20) -> freq6
		 * write.param: bit(27:24) -> freq7
		 * write.param: bit(31:28) -> freq8
		 */
		policy = cpufreq_cpu_get(0);
		if (!policy) {
			pr_err("failed to get policy\n");
			return -ENOMEM;
		}

		pos = policy->freq_table;
		table = policy->freq_table;
		if (!table) {
			cpufreq_cpu_put(policy);
			return -ENODEV;
		}
		i = 0;
		memset(&w, 0, sizeof(icc_msg_t));
		cpufreq_for_each_valid_entry(pos, table) {
			if (i < MAX_SCALING_FREQ) {
				div = (f_vco_clk / pos->frequency);
				switch (div) {
				case 2:
					w.param[0]  |= (1 << (i * 4));
					break;
				case 3:
					w.param[0]  |= (2 << (i * 4));
					break;
				case 4:
					w.param[0]  |= (3 << (i * 4));
					break;
				case 5:
					w.param[0]  |= (4 << (i * 4));
					break;
				case 6:
					w.param[0]  |= (5 << (i * 4));
					break;
				case 8:
					w.param[0]  |= (6 << (i * 4));
					break;
				case 10:
					w.param[0] |= (7 << (i * 4));
					break;
				case 12:
					w.param[0] |= (8 << (i * 4));
					break;
				case 16:
					w.param[0] |= (9 << (i * 4));
					break;
				case 20:
					w.param[0] |= (10 << (i * 4));
					break;
				case 24:
					w.param[0] |= (11 << (i * 4));
					break;
				case 32:
					w.param[0] |= (12 << (i * 4));
					break;
				case 40:
					w.param[0] |= (13 << (i * 4));
					break;
				case 48:
					w.param[0] |= (14 << (i * 4));
					break;
				case 64:
					w.param[0] |= (15 << (i * 4));
					break;
				default:
					break;
				}
				i++;
			}
		}
		/* expose scaling frequencies to TEP-RegMap for policy check */
		w.src_client_id = REGMAP_CGU;
		w.dst_client_id = REGMAP_CGU;
		w.msg_id = REGMAP_CF_MSGID;
		ret = icc_write(REGMAP_CGU, &w);

		cpufreq_cpu_put(policy);
	}
	return ret;
}
module_init(xrx500_cpufreq_driver_init);

MODULE_DESCRIPTION("XRX500/PRX300 Family SoC CPUFreq driver");
MODULE_AUTHOR("Waldemar Rymarkiewicz <waldemarx.rymarkiewicz@intel.com>");
MODULE_LICENSE("GPL v2");
