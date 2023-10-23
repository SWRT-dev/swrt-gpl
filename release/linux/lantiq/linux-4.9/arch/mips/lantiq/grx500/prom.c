/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2014 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 *  Copyright (C) 2016 Intel Corporation.
 */
#include <linux/init.h>
#include <linux/export.h>
#include <linux/of_platform.h>
#include <linux/of_fdt.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <asm/mips-cm.h>
#include <asm/mips-cpc.h>
#include <asm/smp-ops.h>
#include <asm/dma-coherence.h>
#include <asm/prom.h>

#include <lantiq_soc.h>

#include "prom.h"

#define PART_SHIFT		12
#define PART_MASK		0x0FFFFFFF
#define REV_SHIFT		28
#define REV_MASK		0x70000000

/* access to the ebu needs to be locked between different drivers */
DEFINE_SPINLOCK(ebu_lock);
EXPORT_SYMBOL_GPL(ebu_lock);
static __initdata const void *dtb;

/*
 * this struct is filled by the soc specific detection code and holds
 * information about the specific soc type, revision and name
 */
static struct ltq_soc_info soc_info;

unsigned int ltq_get_cpu_id(void)
{
	return soc_info.partnum;
}
EXPORT_SYMBOL(ltq_get_cpu_id);

unsigned int ltq_get_soc_type(void)
{
	return soc_info.type;
}
EXPORT_SYMBOL(ltq_get_soc_type);

unsigned int ltq_get_soc_rev(void)
{
	return soc_info.rev;
}
EXPORT_SYMBOL(ltq_get_soc_rev);

const char *get_system_type(void)
{
	return soc_info.sys_type;
}

void prom_free_prom_memory(void)
{
}

static void __init prom_init_cmdline(void)
{
	int i;
	int argc;
	char **argv;

	/*
	 * If u-boot pass parameters, it is ok, however, if without u-boot
	 * JTAG or other tool has to reset all register value before it goes
	 * emulation most likely belongs to this category
	 */
	if (fw_arg0 == 0 || fw_arg1 == 0)
		return;

	argc = fw_arg0;
	argv = (char **)KSEG1ADDR(fw_arg1);

	arcs_cmdline[0] = '\0';

	for (i = 0; i < argc; i++) {
		char *p = (char *)KSEG1ADDR(argv[i]);

		if (argv[i] && *p) {
			strlcat(arcs_cmdline, p, sizeof(arcs_cmdline));
			strlcat(arcs_cmdline, " ", sizeof(arcs_cmdline));
		}
	}
}

static int __init plat_enable_iocoherency(void)
{
	int supported = 0;

	if (mips_cm_numiocu() != 0) {
		/* Nothing special needs to be done to enable coherency */
		pr_info("Coherence Manager IOCU detected\n");
		/* Second IOCU for MPE or other master access register */
		write_gcr_reg0_base(0xa0000000);
		write_gcr_reg0_mask(0xf8000000 | CM_GCR_REGn_MASK_CMTGT_IOCU1);
		supported = 1;
	}

	/* hw_coherentio = supported; */

	return supported;
}

static void __init plat_setup_iocoherency(void)
{
#ifdef CONFIG_DMA_NONCOHERENT
	/*
	 * Kernel has been configured with software coherency
	 * but we might choose to turn it off and use hardware
	 * coherency instead.
	 */
	if (plat_enable_iocoherency()) {
		if (coherentio == IO_COHERENCE_DISABLED)
			pr_info("Hardware DMA cache coherency disabled\n");
		else
			pr_info("Hardware DMA cache coherency enabled\n");
	} else {
		if (coherentio == IO_COHERENCE_ENABLED)
			pr_info("Hardware DMA cache coherency unsupported, but enabled from command line!\n");
		else
			pr_info("Software DMA cache coherency enabled\n");
	}
#else
	if (!plat_enable_iocoherency())
		panic("Hardware DMA cache coherency not supported!");
#endif
}

static void free_init_pages_eva_xrx500(void *begin, void *end)
{
	free_init_pages("unused kernel", __pa_symbol((unsigned long *)begin),
			__pa_symbol((unsigned long *)end));
}

void __init *plat_get_fdt(void)
{
	/*
	 * Load the builtin devicetree. This causes the chosen node to be
	 * parsed resulting in our memory appearing
	 */
	if (fw_passed_dtb) { /* used by CONFIG_MIPS_APPENDED_RAW_DTB as well */
		if (fw_arg0 == -2) {/* uboot pass dtb to kernel */
			dtb = (void *)LEGACY_TO_VADDR(fw_arg1);
		} else /* CONFIG_MIPS_APPENDED_RAW_DTB case */
			dtb = (void *)fw_passed_dtb;
	} else if (__dtb_start != __dtb_end) {
		dtb = (void *)__dtb_start;
	} else {
		panic("no dtb found");
	}

	return (void *)dtb;
}

void __init plat_mem_setup(void)
{
	bool eva = IS_ENABLED(CONFIG_EVA);

	if (eva) {
	/* EVA has already been configured in mach-lantiq/kernel-entry-init.h */
#ifdef CONFIG_LTQ_EVA_2GB
		pr_info("Enhanced Virtual Addressing (EVA 2GB) activated\n");
#elif defined(CONFIG_LTQ_EVA_1GB)
		pr_info("Enhanced Virtual Addressing (EVA 1GB) activated\n");
#else
		pr_info("Enhanced Virtual Addressing (EVA Legacy 512MB) activated\n");
#endif /* CONFIG_LTQ_EVA_2GB */
	} else
		pr_info("EVA disabled, Lantiq legacy activiated\n");

	ioport_resource.start = IOPORT_RESOURCE_START;
	ioport_resource.end = ~0UL; /* No limit */
	iomem_resource.start = IOMEM_RESOURCE_START;
	iomem_resource.end = ~0UL; /* No limit */

	set_io_port_base((unsigned long)KSEG1);

	if (dtb)
		__dt_setup_arch((void *)dtb);

	plat_setup_iocoherency();
	free_init_pages_eva = eva ? free_init_pages_eva_xrx500 : NULL;
}

void __init device_tree_init(void)
{
	unflatten_and_copy_device_tree();
}

#define CPC_BASE_ADDR		0x12310000

phys_addr_t mips_cpc_default_phys_base(void)
{
	return CPC_BASE_ADDR;
}

static void __init ltq_soc_detect(struct ltq_soc_info *i)
{
	i->partnum = (ltq_r32(LTQ_MPS_CHIPID) & PART_MASK) >> PART_SHIFT;
	i->rev = (ltq_r32(LTQ_MPS_CHIPID) & REV_MASK) >> REV_SHIFT;

	sprintf(i->rev_type, "1.%d", i->rev);
	switch (i->partnum) {
	case SOC_ID_GRX5838:
	case SOC_ID_GRX5828:
	case SOC_ID_GRX5628:
	case SOC_ID_GRX5821:
	case SOC_ID_GRX5831:
	case SOC_ID_GRX58312:
		i->name = SOC_GRX500;
		i->type = SOC_TYPE_GRX500;
		i->compatible = COMP_GRX500;
		break;

	case SOC_ID_GRX3506:
	case SOC_ID_GRX3508:
		i->name = SOC_GRX350;
		i->type = SOC_TYPE_GRX500;
		i->compatible = COMP_GRX500;
		break;

	case SOC_ID_FMX_SFP:
	case SOC_ID_FMX_SFU:
	case SOC_ID_FMX_DPU:
		i->name = SOC_PRX300;
		i->type = SOC_TYPE_PRX300;
		i->compatible = COMP_PRX300;
		break;
	default:
		unreachable();
		break;
	}
}

void __init prom_init(void)
{
	/* call the soc specific detetcion code and get it to fill soc_info */
	ltq_soc_detect(&soc_info);

	snprintf(soc_info.sys_type, LTQ_SYS_TYPE_LEN - 1, "%s rev %s",
		 soc_info.name, soc_info.rev_type);
	soc_info.sys_type[LTQ_SYS_TYPE_LEN - 1] = '\0';
	pr_info("SoC: %s\n", get_system_type());

	prom_init_cmdline();
	plat_get_fdt();
	BUG_ON(!dtb);

	mips_cpc_probe();

	if (!register_cps_smp_ops())
		return;

	if (!register_cmp_smp_ops())
		return;

	if (!register_vsmp_smp_ops())
		return;
}

static int __init plat_publish_devices(void)
{
	if (!of_have_populated_dt())
		return 0;

	return of_platform_default_populate(NULL, NULL, NULL);
}
arch_initcall(plat_publish_devices);

#define CHIPTOP_IFMUX_CFG	0x120
#define SPI_DEBUG_EN		BIT(18)

/* Disable WLAN SPI debug which existed in pinctrl before */
static int __init chiptop_default_setup(void)
{
	unsigned int val = 0;
	struct regmap *chiptop_base;
	struct device_node *np;

	np = of_find_compatible_node(NULL, NULL, "lantiq,grx500");
	if (np) {
		chiptop_base =
		    syscon_regmap_lookup_by_compatible("lantiq,chiptop-grx500");
		if (IS_ERR(chiptop_base)) {
			pr_err("%s: failed to find chiptop regmap!\n",
			       __func__);
			return -EINVAL;
		}

		regmap_read(chiptop_base, CHIPTOP_IFMUX_CFG, &val);
		val &= ~SPI_DEBUG_EN;
		regmap_write(chiptop_base, CHIPTOP_IFMUX_CFG, val);
	}

	return 0;
}
postcore_initcall(chiptop_default_setup);
