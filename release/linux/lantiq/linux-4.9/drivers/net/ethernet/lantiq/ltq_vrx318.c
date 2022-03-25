/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011~2014 Lei Chuanhua <chuanhua.lei@lantiq.com>
 */
/*!
  \defgroup VRX318_INTERNAL Internal functions
  \ingroup VRX318
  \brief IFX PCIe EP internal driver functions
*/

/*!
  \defgroup VRX318_OS OS APIs
  \ingroup VRX318
  \brief IFX PCIe EP OS APIs for driver interface
*/

/*!
   \file ifxmips_pcie_ep_vrx318.c
   \ingroup VRX318
   \brief SmartPHY PCIe EP address mapping driver source file
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>

#include <lantiq.h>
#include <lantiq_pcie.h>
#include "ltq_vrx318.h"

#define ltq_vrx318_r32    ltq_r32
#define ltq_vrx318_w32    ltq_w32
#define MS(_v, _f)  (((_v) & (_f)) >> _f##_S)
#define SM(_v, _f)  (((_v) << _f##_S) & (_f))

/* #define LTQ_EP_DBG */

#define VRX318_DRV_VERSION "3.0.0 "
const char vrx318_driver_version[] = VRX318_DRV_VERSION;

static struct pcie_ep_info g_pcie_ep_info;
static const char ltq_pcie_driver_name[] = "ltq_pcie";
static DEFINE_SPINLOCK(pcie_ep_lock);

int ifx_pcie_ep_dev_num_get(int *dev_num)
{
	if ((g_pcie_ep_info.dev_num <= 0)
		|| (g_pcie_ep_info.dev_num > IFX_PCIE_EP_MAX_NUM)) {
		return -EIO;
	}

	*dev_num = g_pcie_ep_info.dev_num;
	return 0;
}
EXPORT_SYMBOL_GPL(ifx_pcie_ep_dev_num_get);

int ifx_pcie_ep_dev_info_req(int dev_idx, ifx_pcie_ep_int_module_t module,
			ifx_pcie_ep_dev_t *dev)
{
	int i;
	struct pcie_ep_dev_priv *ep;
	if ((dev_idx < 0) || (dev_idx >= IFX_PCIE_EP_MAX_NUM)) {
		dev_err(dev->dev, "%s invalid device index %d\n",
			__func__, dev_idx);
		return -EIO;
	}
	ep = &g_pcie_ep_info.pcie_ep[dev_idx];
	if (atomic_read(&ep->refcnt) >= PCIE_EP_MAX_REFCNT) {
		dev_err(dev->dev, "%s mismatch request/release module usage\n",
			__func__);
		return -EIO;
	}

	switch (module) {
	case IFX_PCIE_EP_INT_PPE:
		dev->irq = ep->irq_base;
		break;

	case IFX_PCIE_EP_INT_MEI:
		dev->irq = ep->irq_base + 1;
		/* XXX, Hardcode workaround for PCIe switch bonding for RC0 */
		if (dev->irq == 158)
			dev->irq = 30;
		else if (dev->irq == 166)
			dev->irq = 38;

		break;

	case IFX_PCIE_EP_INT_DYING_GASP:
	case IFX_PCIE_EP_INT_EDMA:
	case IFX_PCIE_EP_INT_FPI_BCU:
	case IFX_PCIE_EP_INT_ARC_LED0:
	case IFX_PCIE_EP_INT_ARC_LED1:
		dev->irq = ep->irq_base + 2;
		break;

	case IFX_PCIE_EP_INT_DMA:
		dev->irq = ep->irq_base + 3;
		if (dev->irq == 158)
			dev->irq = 30;
		else if (dev->irq == 166)
			dev->irq = 38;

		break;

	default:
		dev->irq = ep->irq_base;
		break;
	}
	dev->dev = ep->dev;
	dev->membase = ep->membase;
	dev->phy_membase = ep->phy_membase;
	dev->peer_num = ep->peer_num;
	for (i = 0; i < dev->peer_num; i++) {
		dev->peer_membase[i] = ep->peer_membase[i];
		dev->peer_phy_membase[i] = ep->peer_phy_membase[i];
	}
	atomic_inc(&ep->refcnt);
	return 0;
}
EXPORT_SYMBOL_GPL(ifx_pcie_ep_dev_info_req);

int ifx_pcie_ep_dev_info_release(int dev_idx)
{
	struct pcie_ep_dev_priv *ep;

	if ((dev_idx < 0) || (dev_idx >= IFX_PCIE_EP_MAX_NUM)) {
		pr_err("%s invalid device index %d\n",
			__func__, dev_idx);
		return -EIO;
	}
	ep = &g_pcie_ep_info.pcie_ep[dev_idx];
	if (atomic_read(&ep->refcnt) <= 0) {
		pr_err("%s mismatch request/release module usage\n",
			__func__);
		return -EIO;
	}
	atomic_dec(&ep->refcnt);
	return 0;
}
EXPORT_SYMBOL_GPL(ifx_pcie_ep_dev_info_release);

static DEFINE_PCI_DEVICE_TABLE(ltq_pcie_id_table) = {
	{0x1bef, 0x0020, PCI_ANY_ID, PCI_ANY_ID}, /* SmartPHY */
	{0x1bef, 0x0100, PCI_ANY_ID, PCI_ANY_ID}, /* GRX500 mapped VRX318 */
	{0},
};

/**
 * \fn  static int pci_msi_max_vector_get(struct pci_dev *dev, int *nvec)
 *
 * \brief  This function tries to get device's MSI interrupt vector number
 *
 * \param  dev      Device to operate
 * \param  nvec     Numer of interrupts to get
 * \return 0        on success
 * \return -EINVAL  The device doesn't support MSI capability
 *
 * \ingroup IFX_PCIE_EP_VRX318_INTERNAL
 */
static int pci_msi_max_vector_get(struct pci_dev *dev, int *nvec)
{
	int pos, maxvec;
	u16 msgctl;

	pos = pci_find_capability(dev, PCI_CAP_ID_MSI);
	if (!pos)
		return -EINVAL;

	pci_read_config_word(dev, pos + PCI_MSI_FLAGS, &msgctl);
	maxvec = 1 << ((msgctl & PCI_MSI_FLAGS_QMASK) >> 1);
	*nvec = maxvec;
	return 0;
}

/**
 * \fn  static int pci_msi_max_vector_set(struct pci_dev *dev, int nvec)
 *
 * \brief  This function tries to set device's MSI interrupt vector number
 *
 * \param  dev      Device to operate
 * \param  nvec     Numer of interrupts to set
 * \return 0        on success
 * \return -EINVAL  The device doesn't support MSI capability or invalid
 *                  interrupt number
 *
 * \remark smartPHY only supports 1 or 2 or 4 interrupts depend on other
 *         module usage
 * \ingroup IFX_PCIE_EP_VRX318_INTERNAL
 */
static int pci_msi_max_vector_set(struct pci_dev *dev, int nvec)
{
	int pos;
	u16 msgctl;

	if ((nvec != 1) && (nvec != 2) && (nvec != 4))
		return -EINVAL;

	pos = pci_find_capability(dev, PCI_CAP_ID_MSI);
	if (!pos)
		return -EINVAL;

	pci_read_config_word(dev, pos + PCI_MSI_FLAGS, &msgctl);
	msgctl &= ~PCI_MSI_FLAGS_QSIZE;
	msgctl |= (nvec >> 1) << 4;
	pci_write_config_word(dev, pos + PCI_MSI_FLAGS, msgctl);
	pci_read_config_word(dev, pos + PCI_MSI_FLAGS, &msgctl);
	/* ICU control if necessary */
	return 0;
}

#ifdef LTQ_EP_DBG
static void ltq_pcie_iatu_dump(struct pci_dev *pdev, int outbound)
{
	u32 val;

	switch (outbound) {
	case 0:
		/* Inbound iATU0 */
		pci_read_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, &val);
		val &= ~PCIE_PL_IATU_REGION_IDX;
		/* Inbound, region0 */
		val |= PCIE_PL_IATU_REGION_INBOUND |
			SM(PCIE_PL_IATU_REGION0, PCIE_PL_IATU_REGION_IDX);
		pci_write_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, val);
		break;
	case 1:
	default:
		pci_read_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, &val);
		val &= ~PCIE_PL_IATU_REGION_IDX;
		/* Outbound, region1 */
		val &= ~PCIE_PL_IATU_REGION_INBOUND;
		val |= SM(PCIE_PL_IATU_REGION0, PCIE_PL_IATU_REGION_IDX);
		pci_write_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, val);
		break;
	}
	pr_info("iATU %s\n", outbound ? "Outbound" : "Inbound");
	pci_read_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, &val);
	pr_info("PCIE_PL_IATU_VIEWPORT: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_LOWER_BASE_ADDR, &val);
	pr_info("PCIE_PL_IATU_REGION_LOWER_BASE_ADDR: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_UPPER_BASE_ADDR, &val);
	pr_info("PCIE_PL_IATU_REGION_UPPER_BASE_ADDR: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_LIMIT, &val);
	pr_info("PCIE_PL_IATU_REGION_LIMIT: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_LOWER_TARGET_ADDR,
			&val);
	pr_info("PCIE_PL_IATU_REGION_LOWER_TARGET_ADDR: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_UPPER_TARGET_ADDR,
			&val);
	pr_info("PCIE_PL_IATU_REGION_UPPER_TARGET_ADDR: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_CTRL1, &val);
	pr_info("PCIE_PL_IATU_REGION_CTRL1: %08x\n", val);
	pci_read_config_dword(pdev, PCIE_PL_IATU_REGION_CTRL2, &val);
	pr_info("PCIE_PL_IATU_REGION_CTRL2: %08x\n", val);
}
#endif /* LTQ_EP_DBG */

/**
 * \fn  static void  ltq_pcie_iatu_setup(struct pci_dev *pdev)
 *
 * \brief  This function configures smartPHY PCIe EP inbound address
 *         translation iATU0 and Outbound address translation <iATU1>.
 *         Detailed information, please refer to related design
 *         documentation.
 *
 * \param  dev      PCI device to configure
 * \return none
 *
 * \ingroup IFX_PCIE_EP_VRX318_INTERNAL
 */
static void ltq_pcie_iatu_setup(struct pci_dev *pdev)
{
	u32 val;

	/* Inbound iATU0 */
	pci_read_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, &val);
	val &= ~PCIE_PL_IATU_REGION_IDX;
	/* Inbound, region0 */
	val |= PCIE_PL_IATU_REGION_INBOUND | SM(PCIE_PL_IATU_REGION0,
		PCIE_PL_IATU_REGION_IDX);
	pci_write_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, val);

	/* BAR match used, there is no need to configure
	 * base and limit register
	 */
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_LOWER_TARGET_ADDR,
		(u32) PCIE_EP_INBOUND_INTERNAL_BASE);

	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_UPPER_TARGET_ADDR, 0);

	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_CTRL1, 0);
	/* Inbound BAR match, BAR0 used only */
	val = PCIE_PL_IATU_REGION_MATCH_EN | PCIE_PL_IATU_REGION_EN |
		SM(PCIE_PL_IATU_REGION_BAR0, PCIE_PL_IATU_REGION_BAR);
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_CTRL2, val);

	/* Outbound iATU1 */
	pci_read_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, &val);
	val &= ~PCIE_PL_IATU_REGION_IDX;
	/* Outbound, region0 */
	val &= ~PCIE_PL_IATU_REGION_INBOUND;
	val |= SM(PCIE_PL_IATU_REGION0, PCIE_PL_IATU_REGION_IDX);
	pci_write_config_dword(pdev, PCIE_PL_IATU_VIEWPORT, val);

	/* 32 bit only, base address  */
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_LOWER_BASE_ADDR,
		(u32) PCIE_EP_OUTBOUND_INTERNAL_BASE);
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_UPPER_BASE_ADDR,
		0);

	/* Region limit from phymem to phymem + memsize -1 */
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_LIMIT,
		(u32) PCIE_EP_OUTBOUND_INTERNAL_BASE +
		PCIE_EP_OUTBOUND_MEMSIZE - 1);

	/* Mapped to 0x00000000 ~ 0x3FFFFFFF, total 2GB */
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_LOWER_TARGET_ADDR, 0);

	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_UPPER_TARGET_ADDR, 0);

	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_CTRL1, 0);
	/* Outbound, address match */
	val = PCIE_PL_IATU_REGION_EN;
	pci_write_config_dword(pdev, PCIE_PL_IATU_REGION_CTRL2, val);

#ifdef LTQ_EP_DBG
	ltq_pcie_iatu_dump(pdev, 0); /* Inbound */
	ltq_pcie_iatu_dump(pdev, 1); /* Outbound */
#endif /* LTQ_EP_DBG */
}

#ifdef CONFIG_SOC_GRX500
static void ltq_pcie_rc_fmt_cfg(struct pcie_ep_dev_priv *ep)
{
	int i;
	struct ltq_sys_data *soc_data = ep->soc_data;

	for (i = 0; i < ep->peer_num; i++) {
		ltq_vrx318_w32(PCIE_FMT_IN_LOW_ADDR_0 + PCIE_FMT_SIZE * i,
			soc_data->app_base + PCIE_FMT_ADDR_IN_LBASE_0
			+ PCIE_FMT_REG_OFF * i);
		ltq_vrx318_w32(PCIE_FMT_IN_HIGH_ADDR_0 + PCIE_FMT_SIZE * i,
			soc_data->app_base + PCIE_FMT_ADDR_IN_UBASE_0
			+ PCIE_FMT_REG_OFF * i);
		ltq_vrx318_w32(ep->peer_phy_membase[i],
			soc_data->app_base + PCIE_FMT_ADDR_OUT_LBASE_0
			+ PCIE_FMT_REG_OFF * i);
		/* Back up the original RC physical address */
		ep->org_peer_phy_membase[i] = ep->peer_phy_membase[i];
		ep->org_peer_membase[i] = ep->peer_membase[i];
		/* Fill new mapped address */
		ep->peer_phy_membase[i] =
			PCIE_FMT_IN_LOW_ADDR_0 + PCIE_FMT_SIZE * i;
		ltq_vrx318_w32(0x00000001,
			soc_data->app_base + PCIE_FMT_ADDR_EN_0
			+ PCIE_FMT_REG_OFF * i);
#ifdef LTQ_EP_DBG
		dev_info(ep->dev, "low addr %p value 0x%08x\n",
			soc_data->app_base + PCIE_FMT_ADDR_IN_LBASE_0
			+ PCIE_FMT_REG_OFF * i,
			ltq_vrx318_r32(soc_data->app_base
			+ PCIE_FMT_ADDR_IN_LBASE_0
			+ PCIE_FMT_REG_OFF * i));
		dev_info(ep->dev, "upper addr %p value 0x%08x\n",
			soc_data->app_base + PCIE_FMT_ADDR_IN_UBASE_0
			+ PCIE_FMT_REG_OFF * i,
			ltq_vrx318_r32(soc_data->app_base
				+ PCIE_FMT_ADDR_IN_UBASE_0
				+ PCIE_FMT_REG_OFF * i));
		dev_info(ep->dev, "target addr %p value 0x%08x\n",
			soc_data->app_base + PCIE_FMT_ADDR_OUT_LBASE_0
				+ PCIE_FMT_REG_OFF * i,
			ltq_vrx318_r32(soc_data->app_base
				+ PCIE_FMT_ADDR_OUT_LBASE_0
				+ PCIE_FMT_REG_OFF * i));
		dev_info(ep->dev, "status addr %p value 0x%08x\n",
			soc_data->app_base + PCIE_FMT_ADDR_EN_0
				+ PCIE_FMT_REG_OFF * i,
			ltq_vrx318_r32(soc_data->app_base
				+ PCIE_FMT_ADDR_EN_0
				+ PCIE_FMT_REG_OFF * i));
		dev_info(ep->dev, "Map from 0x%08x--> 0x%08x\n",
			 ep->peer_phy_membase[i], ep->org_peer_phy_membase[i]);
#endif /* LTQ_EP_DBG */
	}
}
#endif /* CONFIG_SOC_GRX500 */

/**
 * \fn  static int ltq_pcie_probe(struct pci_dev *pdev,
 *	const struct pci_device_id *id)
 *
 * \brief This function initializes an adapter identified by
 *	  a pci_dev structure. The OS initialization, configuring
 *	  of the adapter private structure.
 *
 * \param  pdev     PCI device information struct
 * \param  id       entry in ltq_pcie_id_table
 * \return 0        OK
 * \return -ENODEV  Failed to initialize an adapter identified by pci_dev
 *
 * \ingroup IFX_PCIE_EP_VRX318_OS
 */
static int
ltq_pcie_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int i;
	int j;
	int k;
	struct device *dev;
	unsigned long phymem;
	void __iomem *mem;
	size_t memsize;
	static int cards_found;
	int nvec, err;
	struct pcie_ep_adapter *adapter;
	struct pcie_ep_dev_priv *priv;
	struct pci_bus *bus;
	int peer_num;
	u32 reg;

	bus = pdev->bus;
	dev = &pdev->dev;
	err = pci_enable_device(pdev);
	if (err)
		return err;

	/* XXX 32-bit addressing only */
	if (pci_set_dma_mask(pdev, DMA_BIT_MASK(32))) {
		pr_err("%s: 32-bit DMA not available\n",
			__func__);
		goto bad;
	}

	pci_set_master(pdev);

	/* Physical address */
	phymem = pci_resource_start(pdev, 0); /* BAR zero */
	memsize = pci_resource_len(pdev, 0);
	if (!request_mem_region(phymem, memsize, ltq_pcie_driver_name)) {
		dev_err(dev, "%s: cannot reserve PCI memory region\n",
			__func__);
		goto bad;
	}

	/* Virtual address */
	mem = ioremap_nocache(phymem, memsize);
	if (!mem) {
		dev_err(dev, "%s: cannot remap PCI memory region\n",
			__func__);
		goto bad1;
	}

	adapter = kmalloc(sizeof(struct pcie_ep_adapter), GFP_KERNEL);
	if (adapter == NULL)
		goto err_mem;

	pci_set_drvdata(pdev, adapter);

	err = pci_msi_max_vector_get(pdev, &nvec);
	if (err) {
		dev_err(dev, "%s: device doesn't support MSI, error code: %d",
			__func__, err);
		goto err_msi;
	}

	/* Overwrite maximum vector number according to
	 * the specific requirement
	 */
	if (nvec > PCIE_EP_DEFAULT_MSI_VECTOR)
		nvec = PCIE_EP_DEFAULT_MSI_VECTOR;

	pci_msi_max_vector_set(pdev, nvec);

	err = pci_enable_msi_exact(pdev, nvec);
	if (err) {
		dev_err(dev, "%s: Failed to enable MSI interrupts for the device error code: %d\n",
			__func__, err);
		goto err_msi;
	}

	/* Enough information to configure address translation */
	ltq_pcie_iatu_setup(pdev);

	adapter->pdev = pdev;
	adapter->device_id = pdev->device;
	adapter->mem = mem;
	adapter->phy_mem = phymem;
	adapter->memsize = memsize;
	adapter->msi_nvec = nvec;
	adapter->irq_base = pdev->irq;
	adapter->card_num = cards_found++; /* Logical index */
	/* EMI control stuff */
	reg = ltq_vrx318_r32(adapter->mem + PCIE_EP_IF_CLK);
	reg |= PCIE_EP_IF_CLK_NO_36MHZ_CLKOUT;
	ltq_vrx318_w32(reg, adapter->mem + PCIE_EP_IF_CLK);

	reg = ltq_vrx318_r32(adapter->mem + PCIE_EP_P0_ALTSEL1);
	reg &= ~PCIE_EP_P0_ALTSEL1_PIN1_SET;
	ltq_vrx318_w32(reg, adapter->mem + PCIE_EP_P0_ALTSEL1);
	priv = &g_pcie_ep_info.pcie_ep[adapter->card_num];
	spin_lock(&pcie_ep_lock);
	g_pcie_ep_info.dev_num = cards_found; /* Existing cards */
	atomic_set(&priv->refcnt, 0);
	priv->dev = &pdev->dev;
#ifdef CONFIG_SOC_GRX500
	priv->soc_data = pdev->sysdata;
#endif /* CONFIG_SOC_GRX500 */
	priv->card_idx = adapter->card_num;
	priv->membase = adapter->mem;
	priv->phy_membase = adapter->phy_mem;
	priv->memsize = adapter->memsize;
	priv->irq_base = adapter->irq_base;
	priv->irq_num = adapter->msi_nvec;

	/* More cards supported, exchange address information
	 * For example, suppose three cards dected.
	 * 0, <1, 2>
	 * 1, <0, 2>
	 * 2, <0, 1>
	 * For four cards detected
	 * 0, <1, 2, 3>
	 * 1, <0, 2, 3>
	 * 2, <0, 1, 3>
	 * 3, <0, 1, 2>
	 * and etc
	 */
	if (cards_found > 1) {
		peer_num = cards_found - 1;
		for (i = 0; i < cards_found; i++) {
			struct pcie_ep_dev_priv *ep =
				&g_pcie_ep_info.pcie_ep[i];
			j = 0;
			k = 0;
			ep->peer_num = peer_num;
			do {
				if (j == i) {
					j++;
					continue;
				}
				ep->peer_membase[k] =
					g_pcie_ep_info.pcie_ep[j].membase;
				ep->peer_phy_membase[k] =
					g_pcie_ep_info.pcie_ep[j].phy_membase;
				ep->peer_memsize[k] =
					g_pcie_ep_info.pcie_ep[j].memsize;
				k++;
				j++;
			} while ((k < peer_num) && (j < cards_found));
		}
	}
	spin_unlock(&pcie_ep_lock);
#ifdef CONFIG_SOC_GRX500
	for (i = 0; i < cards_found; i++) {
		priv = &g_pcie_ep_info.pcie_ep[i];
		ltq_pcie_rc_fmt_cfg(priv);
	}
#endif /* CONFIG_SOC_GRX500 */

#ifdef LTQ_EP_DBG
	dev_info(dev, "Total cards found %d\n", cards_found);
	/* Dump detailed debug information */
	for (i = 0; i < cards_found; i++) {
		priv = &g_pcie_ep_info.pcie_ep[i];
		dev_info(dev, "card %d attached\n", priv->card_idx);
		dev_info(dev, "irq base %d irq numbers %d\n",
			priv->irq_base, priv->irq_num);
		dev_info(dev, "its own phy membase  0x%08x virtual membase 0x%p size 0x%08x\n",
			priv->phy_membase, priv->membase, priv->memsize);
		if (cards_found > 1) {
			for (j = 0; j < priv->peer_num; j++)
				dev_info(dev, "its peer phy membase 0x%08x virtual membase 0x%p size 0x%08x\n",
			#ifdef CONFIG_SOC_GRX500
				priv->org_peer_phy_membase[j],
			#else
				priv->peer_phy_membase[j],
			#endif
				priv->peer_membase[j], priv->peer_memsize[j]);
		}
	}
#endif /* LTQ_EP_DBG */
	return 0;
err_msi:
	kfree(adapter);
err_mem:
	iounmap(mem);
bad1:
	release_mem_region(phymem, memsize);
bad:
	pci_disable_device(pdev);
	return -ENODEV;
}

/**
 * \fn  static void ltq_pcie_remove(struct pci_dev *pdev)
 *
 * \brief  This function is called by the PCI subsystem to alert the driver
 *         that it should release a PCI device because the driver is going
 *         to be removed from memory.
 *
 * \param  pdev     PCI device information struct
 * \return none
 *
 * \ingroup IFX_PCIE_EP_VRX318_OS
 */
static void ltq_pcie_remove(struct pci_dev *pdev)
{

	struct device *dev = &pdev->dev;
	struct pcie_ep_adapter *adapter =
		(struct pcie_ep_adapter *)pci_get_drvdata(pdev);
	struct pcie_ep_dev_priv *priv =
		&g_pcie_ep_info.pcie_ep[adapter->card_num];
	if (atomic_read(&priv->refcnt) != 0)
		dev_err(dev, "%s still being used, can't remove\n", __func__);

	iounmap(adapter->mem);
	release_mem_region(adapter->phy_mem, adapter->memsize);
	pci_disable_msi(pdev);
	pci_disable_device(pdev);
	kfree(adapter);
	adapter = NULL;
}

MODULE_DEVICE_TABLE(pci, ltq_pcie_id_table);

static struct pci_driver ltq_pcie_driver = {
	.name = (char *) ltq_pcie_driver_name,
	.id_table = ltq_pcie_id_table,
	.probe = ltq_pcie_probe,
	.remove = ltq_pcie_remove,
	/* PM not supported */
	/* AER is controlled by RC */
};

/**
 * \fn  static int __init init_ltq_pcie(void)
 *
 * \brief  This function registered PCIe EP device driver with OS PCI subsystem
 *         and initializes PCIe EP address mapping driver.
 *
 * \return 0 on success
 * \return -ENODEV No related PCIe EP found
 *
 * \ingroup IFX_PCIE_EP_VRX318_OS
 */
static int __init init_ltq_pcie(void)
{
	memset(&g_pcie_ep_info, 0, sizeof(struct pcie_ep_info));

	if (pci_register_driver(&ltq_pcie_driver) < 0) {
		pr_err("%s: No devices found, driver not installed.\n",
			__func__);
		return -ENODEV;
	}
	pr_info("Lantiq VRX318 Version %s", vrx318_driver_version);
	return 0;
}

module_init(init_ltq_pcie);

/**
 * \fn  static void __exit exit_ltq_pcie(void)
 *
 * \brief  This function unregister PCIe EP device driver with OS PCI subsystem
 *
 * \return none
 *
 * \ingroup IFX_PCIE_EP_VRX318_OS
 */
static void __exit exit_ltq_pcie(void)
{
	pci_unregister_driver(&ltq_pcie_driver);

	pr_info("%s: %s driver unloaded\n", __func__,
		ltq_pcie_driver_name);
}

module_exit(exit_ltq_pcie);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chuanhua.Lei@lantiq.com");
MODULE_SUPPORTED_DEVICE("Lantiq SmartPHY PCIe EP");
MODULE_DESCRIPTION("Lantiq SmartPHY PCIe EP address mapping driver");


