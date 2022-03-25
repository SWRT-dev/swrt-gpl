/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011~2013 Lei Chuanhua <chuanhua.lei@lantiq.com>
 */
 /*!
 \file ltq_vrx318.h
 \ingroup VRX318
 \brief header file for SmartPHY PCIe EP address mapping driver internal
  definition
*/
#ifndef LTQ_VRX318_H
#define LTQ_VRX318_H

#include <linux/types.h>
#include <linux/pci.h>

#include <lantiq_pcie.h>

#define IFX_PCIE_EP_MAX_NUM		(IFX_PCIE_EP_MAX_PEER + 1)

/* Maximum 4, if PCIe switch attached, 2 is used. 2 is also default one */
#ifdef CONFIG_LANITQ_VRX318_PCIE_SWITCH_DSL_BONDING
#define PCIE_EP_DEFAULT_MSI_VECTOR	2
#else
#define PCIE_EP_DEFAULT_MSI_VECTOR	4
#endif /*  CONFIG_LANITQ_VRX318_PCIE_SWITCH_DSL_BONDING */

#define PCIE_EP_MAX_REFCNT		IFX_PCIE_EP_INT_MODULE_MAX

/* iATU specific register offset definition */

/* View Point Register */
#define PCIE_PL_IATU_VIEWPORT		0x900

#define PCIE_PL_IATU_REGION_IDX		0x0000000F
#define PCIE_PL_IATU_REGION_IDX_S	0

/* Inbound and outbound has seperate regions,
 * each one has 8 regions from index 0
 */
enum {
	PCIE_PL_IATU_REGION0 = 0,
	PCIE_PL_IATU_REGION1,
	PCIE_PL_IATU_REGION2,
	PCIE_PL_IATU_REGION3,
	PCIE_PL_IATU_REGION4,
	PCIE_PL_IATU_REGION5,
	PCIE_PL_IATU_REGION6,
	PCIE_PL_IATU_REGION7,
};
#define PCIE_PL_IATU_REGION_INBOUND	0x80000000

/* Region control registe for all kinds of types */
#define PCIE_PL_IATU_REGION_CTRL1	0x904

#define PCIE_PL_IATU_REGION_CTRL2	0x908
#define PCIE_PL_IATU_REGION_BAR		0x00000700
#define PCIE_PL_IATU_REGION_BAR_S	8

enum {
	PCIE_PL_IATU_REGION_BAR0 = 0,
	PCIE_PL_IATU_REGION_BAR1,
	PCIE_PL_IATU_REGION_BAR2,
	PCIE_PL_IATU_REGION_BAR3,
	PCIE_PL_IATU_REGION_BAR4,
	PCIE_PL_IATU_REGION_BAR5,
};
#define PCIE_PL_IATU_REGION_MATCH_EN	0x40000000
#define PCIE_PL_IATU_REGION_EN		0x80000000

#define PCIE_PL_IATU_REGION_LOWER_BASE_ADDR	0x90C
#define PCIE_PL_IATU_REGION_UPPER_BASE_ADDR	0x910
#define PCIE_PL_IATU_REGION_LIMIT		0x914
#define PCIE_PL_IATU_REGION_LOWER_TARGET_ADDR	0x918
#define PCIE_PL_IATU_REGION_UPPER_TARGET_ADDR	0x91C

/* Target & Base address definition for Inbound/Outbound */

/* Inbound address translation for iATU0 */
#define PCIE_EP_INBOUND_INTERNAL_BASE		0x1E000000
#define PCIE_EP_OUTBOUND_INTERNAL_BASE		0x20000000
#define PCIE_EP_OUTBOUND_MEMSIZE		0x40000000

/* EMI control stuff */
/* 36MHz clockout */
#define PCIE_EP_IF_CLK				0x00003024
#define PCIE_EP_IF_CLK_NO_36MHZ_CLKOUT		0x00000400

/* GPIO 1 Alternate1 Set/Clear */
#define PCIE_EP_P0_ALTSEL1			0x00102B20
#define PCIE_EP_P0_ALTSEL1_PIN1_SET		0x00000002

#ifdef CONFIG_SOC_GRX500
/* PCie 2.0  VRX318 hacking from SoC RC stuff */
#define PCIE_FMT_ADDR_IN_LBASE_0	0xA0
#define PCIE_FMT_ADDR_IN_UBASE_0	0xA4
#define PCIE_FMT_ADDR_OUT_LBASE_0	0xA8
#define PCIE_FMT_ADDR_EN_0		0xAC

#define PCIE_FMT_ADDR_IN_LBASE_1	0xB0
#define PCIE_FMT_ADDR_IN_UBASE_1	0xB4
#define PCIE_FMT_ADDR_OUT_LBASE_1	0xB8
#define PCIE_FMT_ADDR_EN_1		0xBC

#define PCIE_FMT_ADDR_IN_LBASE_2	0xC0
#define PCIE_FMT_ADDR_IN_UBASE_2	0xC4
#define PCIE_FMT_ADDR_OUT_LBASE_2	0xC8
#define PCIE_FMT_ADDR_EN_2		0xCC
#define PCIE_FMT_REG_OFF		0x10

#define PCIE_FMT_ADDR_EN		0x1

#define PCIE_FMT_IN_LOW_ADDR_0		0x9E800000
#define PCIE_FMT_IN_HIGH_ADDR_0		0x9EFFFFFF

#define PCIE_FMT_IN_LOW_ADDR_1		0x9F000000
#define PCIE_FMT_IN_HIGH_ADDR_1		0x9F7FFFFF

#define PCIE_FMT_IN_LOW_ADDR_2		0x9F800000
#define PCIE_FMT_IN_HIGH_ADDR_2		0x9FFFFFFF
#define PCIE_FMT_SIZE			0x00800000
#endif /* CONFIG_SOC_GRX500 */

#ifdef CONFIG_SOC_GRX500
struct ltq_sys_data {
	/*!< PCI controller information used as system specific information */
	struct pci_controller pcic;
	void __iomem	*app_base;
};
#endif /* CONFIG_SOC_GRX500 */

/* Structure used to extract attached EP detailed information for
 * PPE/DSL_MEI driver/Bonding
 */
struct pcie_ep_dev_priv {
	struct device *dev;
	u32 card_idx; /*!< EP logical index, the first found one will be 0
			regardless of RC physical index
			*/
	u32 irq_base; /*!< The first MSI interrupt number */
	u32 irq_num; /*!< How many MSI interrupt supported */
	u8 __iomem *membase;  /*!< The EP inbound memory base address
				derived from BAR0, SoC virtual address
				for PPE/DSL_MEI driver
				*/
	u32 phy_membase; /*!< The EP inbound memory base address
				derived from BAR0, physical address for
				PPE FW
				*/
	size_t memsize; /*!< The EP inbound memory window size */
	u32 peer_num;  /*!< Bonding peer number available */
	/*!< The bonding peer EP inbound memory base address derived from
	 * its BAR0, SoC virtual address for PPE/DSL_MEI driver
	 */

	u8 __iomem *peer_membase[IFX_PCIE_EP_MAX_PEER];

	/*!< The bonding peer EP inbound memory base address derived from
	 * its BAR0, physical address for PPE FW
	 */
	u32 peer_phy_membase[IFX_PCIE_EP_MAX_PEER];

	/*!< The bonding peer inbound memory window size */
	size_t peer_memsize[IFX_PCIE_EP_MAX_PEER];
	atomic_t refcnt; /*!< The EP mapping driver referenced times
				by other modules
				*/
#ifdef CONFIG_SOC_GRX500
	struct ltq_sys_data *soc_data;
	u8 __iomem *org_peer_membase[IFX_PCIE_EP_MAX_PEER];
	u32 org_peer_phy_membase[IFX_PCIE_EP_MAX_PEER];
#endif /* CONFIG_SOC_GRX500 */
};

struct pcie_ep_info {
	int dev_num;
	struct pcie_ep_dev_priv pcie_ep[IFX_PCIE_EP_MAX_NUM];
};

/* Card specific private data structure */
struct pcie_ep_adapter {
	/* OS defined structs */
	struct pci_dev *pdev;
	unsigned long phy_mem; /* Physical address */
	u8 __iomem *mem; /* Virtual address */
	size_t memsize;
	u32 card_num; /* EP card index */
	u32 rc_phy_idx; /* Attached which RC */

	/* PCI config space info */
	u16 device_id;
	u16 irq_base; /* irq base for multiple MSI */
	u32 msi_nvec; /* MSI vector number supported */
};

#endif /* LTQ_VRX318_H */


