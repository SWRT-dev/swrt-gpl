/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 *  Copyright (C) 2014 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 *  Copyright (C) 2017 Intel Corporation.
 */

#ifndef _LTQ_SOC_H
#define _LTQ_SOC_H

#include <lantiq.h>

/* GRX500 family chipid */
#define SOC_ID_GRX5838		0x020
#define SOC_ID_GRX5828		0x021
#define SOC_ID_GRX5628		0x022
#define SOC_ID_GRX5821		0x023
#define SOC_ID_GRX5831		0x024
#define SOC_ID_GRX58312		0x025
#define SOC_ID_GRX3506		0x026
#define SOC_ID_GRX3508		0x028
#define SOC_ID_IRX200		0x02F

/* PRX300 family chipid */
#define SOC_ID_FMX_FIBERPHY	0x002 /* HGU */
#define SOC_ID_FMX_SFU		0x003
#define SOC_ID_FMX_SFP		0x004
#define SOC_ID_FMX_DPU		0x005

/* SoC Types */
#define SOC_TYPE_GRX500		0x09
#define SOC_TYPE_PRX300		0x0a

/* SoC Name */
#define SOC_GRX500		"GRX500"
#define SOC_GRX350		"GRX350"
#define SOC_PRX300		"PRX300"

/* SoC compatible */
#define COMP_GRX500		"lantiq,grx500"
#define COMP_PRX300		"intel,prx300"
#define COMP_PRX300_BOOTCORE	"intel,prx300-bootcore"

static inline int ltq_is_grx500(void)
{
	return (ltq_get_soc_type() == SOC_TYPE_GRX500);
}

static inline int ltq_is_prx300(void)
{
	return (ltq_get_soc_type() == SOC_TYPE_PRX300);
}

/* BOOT_SEL - find what boot media we have */
#define BS_EXT_ROM		0x0
#define BS_FLASH		0x1
#define BS_MII0			0x2
#define BS_PCI			0x3
#define BS_UART1		0x4
#define BS_SPI			0x5
#define BS_NAND			0x6
#define BS_RMII0		0x7

/*
 * during early_printk no ioremap is possible
 * lets use KSEG1 instead
 */
#define LTQ_ASC0_BASE_ADDR	0x16600000
#define LTQ_EARLY_ASC		KSEG1ADDR(LTQ_ASC0_BASE_ADDR)

/* register access macros for EBU and CGU */
#define ltq_ebu_w32(x, y)	ltq_w32((x), ltq_ebu_membase + (y))
#define ltq_ebu_r32(x)		ltq_r32(ltq_ebu_membase + (x))
#define ltq_ebu_w32_mask(x, y, z) \
	ltq_w32_mask(x, y, ltq_ebu_membase + (z))

	/* helpers used to access the cgu and xbar register */
#define ltq_cgu_w32(x, y)	ltq_w32((x), ltq_cgu_membase + (y))
#define ltq_cgu_r32(x)		ltq_r32(ltq_cgu_membase + (x))
extern __iomem void *ltq_cgu_membase;

/* EBU - external bus unit */
#define LTQ_EBU_BUSCON0		0x0060
#define LTQ_EBU_PCC_CON		0x0090
#define LTQ_EBU_PCC_IEN		0x00A4
#define LTQ_EBU_PCC_ISTAT	0x00A0
#define LTQ_EBU_BUSCON1		0x0064
#define LTQ_EBU_ADDRSEL1	0x0024
#define EBU_WRDIS		0x80000000

/* WDT */
#define LTQ_RST_CAUSE_WDTRST	0x20

/* MPS - multi processor unit (voice) */
#define LTQ_MPS_BASE_ADDR	(KSEG1 + 0x1F107000)
#define LTQ_MPS_CHIPID		((u32 __iomem *)(LTQ_MPS_BASE_ADDR + 0x0344))
#define LTQ_FUSE_ID_CFG		((u32 __iomem *)(LTQ_MPS_BASE_ADDR + 0x0350))

/* allow the ethernet driver to load a flash mapped mac addr */
const u8 *ltq_get_eth_mac(void);

#endif /* _LTQ_SOC_H */
