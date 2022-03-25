/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2014 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 *  Copyright (C) 2017 Intel Corporation.
 */
#ifndef __ASM_MACH_LANTIQ_IOREMAP_H
#define __ASM_MACH_LANTIQ_IOREMAP_H

#include <linux/types.h>

static inline phys_addr_t fixup_bigphys_addr(phys_addr_t phys_addr,
					     phys_addr_t size)
{
	return phys_addr;
}

#ifdef CONFIG_SOC_GRX500

/* TOP IO Space definition for SSX7 components /PCIe/ToE/Memcpy
 * physical 0xa0000000 --> virtual 0xe0000000
 */
#define GRX500_TOP_IOREMAP_BASE			0xA0000000
#define GRX500_TOP_IOREMAP_SIZE			0x20000000
#define GRX500_TOP_IOREMAP_PHYS_VIRT_OFFSET	0x40000000

static inline void __iomem *plat_ioremap(phys_addr_t offset, unsigned long size,
					 unsigned long flags)
{
	if ((offset >= GRX500_TOP_IOREMAP_BASE) &&
	    offset < (GRX500_TOP_IOREMAP_BASE + GRX500_TOP_IOREMAP_SIZE))
		return (void __iomem *)(unsigned long)
			(offset + GRX500_TOP_IOREMAP_PHYS_VIRT_OFFSET);
	return NULL;
}

static inline int plat_iounmap(const volatile void __iomem *addr)
{
	return (unsigned long)addr >= (unsigned long)
		(GRX500_TOP_IOREMAP_BASE + GRX500_TOP_IOREMAP_PHYS_VIRT_OFFSET);
}
#else
static inline void __iomem *plat_ioremap(phys_addr_t offset, unsigned long size,
					 unsigned long flags)
{
	return NULL;
}

static inline int plat_iounmap(const volatile void __iomem *addr)
{
	return 0;
}
#endif /* CONFIG_SOC_GRX500 */
#endif /* __ASM_MACH_LANTIQ_IOREMAP_H */

