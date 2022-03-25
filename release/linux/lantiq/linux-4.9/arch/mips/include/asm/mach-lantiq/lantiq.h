/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <john@phrozen.org>
 */
#ifndef _LANTIQ_H__
#define _LANTIQ_H__

#include <linux/irq.h>
#include <linux/device.h>
#include <linux/clk.h>

/* generic reg access functions */
#define ltq_r32(reg)		__raw_readl(reg)
#define ltq_w32(val, reg)	__raw_writel(val, reg)

#define ltq_w32_mask(clear, set, reg)	\
	ltq_w32((ltq_r32(reg) & ~(clear)) | (set), reg)

#define ltq_r16(reg)		__raw_readw(reg)
#define ltq_w16(val, reg)	__raw_writew(val, reg)

#define ltq_r8(reg)		__raw_readb(reg)
#define ltq_w8(val, reg)	__raw_writeb(val, reg)

/* spinlock all ebu i/o */
extern spinlock_t ebu_lock;

/* some irq helpers */
void ltq_disable_irq(struct irq_data *data);
void ltq_mask_and_ack_irq(struct irq_data *data);
void ltq_enable_irq(struct irq_data *data);
int ltq_eiu_get_irq(int exin);

/* clock handling */
int clk_activate(struct clk *clk);
void clk_deactivate(struct clk *clk);
struct clk *clk_get_cpu(void);
struct clk *clk_get_ddr(void);
struct clk *clk_get_fpi(void);
struct clk *clk_get_io(void);
struct clk *clk_get_ppe(void);
struct clk *clk_get_xbar(void);

/* find out what bootsource we have */
unsigned char ltq_boot_select(void);
/* find out what caused the last cpu reset */
int ltq_reset_cause(void);
void ltq_reset_once(unsigned int module, ulong u);
void ltq_hw_reset(unsigned int module);
void ltq_rst_init(void);
unsigned int ltq_get_cpu_id(void);
unsigned int ltq_get_soc_type(void);
unsigned int ltq_get_soc_rev(void);

#define IOPORT_RESOURCE_START	0x10000000
#define IOPORT_RESOURCE_END	0xffffffff
#define IOMEM_RESOURCE_START	0x10000000
#define IOMEM_RESOURCE_END	0xffffffff

#endif
