/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011~2013 Lei Chuanhua <chuanhua.lei@lantiq.com>
 */

/** \defgroup IFX_PCIE_EP_VRX318 PCIE EP Functions Reference
 *  This chapter describes the entire interfaces to the PCIE EP interface.
 */
#ifndef LANTIQ_PCIE_H
#define LANTIQ_PCIE_H
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/device.h>

/* @{ */

/*! \def IFX_PCIE_EP_MAX_PEER
 *  \brief how many EP partners existed. In most cases, this number should be
 *  one for bonding application For the future extension, it could be bigger
 *  value. For example, multiple bonding
 */
#define IFX_PCIE_EP_MAX_PEER     1

/** Structure used to specify interrupt source so that EP can assign unique
 *  interruot to it
 */
typedef enum ifx_pcie_ep_int_module {
	IFX_PCIE_EP_INT_PPE, /*!< PPE2HOST_INT 0/1 */
	IFX_PCIE_EP_INT_MEI, /*!< DSL MEI_IRQ */
	IFX_PCIE_EP_INT_DYING_GASP, /*!< DSL Dying_Gasp */
	IFX_PCIE_EP_INT_EDMA, /*!< PCIe eDMA */
	IFX_PCIE_EP_INT_FPI_BCU, /*!< FPI BUC */
	IFX_PCIE_EP_INT_ARC_LED0, /*!< ARC LED0 */
	IFX_PCIE_EP_INT_ARC_LED1, /*!< ARC LED1 */
	IFX_PCIE_EP_INT_DMA, /*!< Central DMA */
	IFX_PCIE_EP_INT_MODULE_MAX,
} ifx_pcie_ep_int_module_t;

/** Structure used to extract attached EP detailed information
 *  for PPE/DSL_MEI driver/Bonding
 */
typedef struct pcie_ep_dev {
	struct device *dev;
	u32 irq;          /*!< MSI interrupt number for this device */
	/*!< The EP inbound memory base address derived from BAR0, SoC
	 *   virtual address for PPE/DSL_MEI driver
	 */
	u8 __iomem *membase;
	u32 phy_membase;  /*!< The EP inbound memory base address derived
			   * from BAR0, physical address for PPE FW
			   */
	u32 peer_num;    /*!< Bonding peer number available */
	/*!< The bonding peer EP inbound memory base address derived from
	 *   its BAR0, SoC virtual address for PPE/DSL_MEI driver
	 */
	u8 __iomem *peer_membase[IFX_PCIE_EP_MAX_PEER];
	/*!< The bonding peer EP inbound memory base address derived from
	 *   its BAR0, physical address for PPE FW
	 */
	u32 peer_phy_membase[IFX_PCIE_EP_MAX_PEER];
} ifx_pcie_ep_dev_t;

/**
 * This function returns the total number of EPs attached. Normally,
 * the number should be one <standard smartPHY EP> or two <smartPHY
 * off-chip bonding cases>. Extended case is also considered

 * \param[in/out]  dev_num   Pointer to detected EP numbers in total.
 * \return         -EIO      Invalid total EP number which means this
 *			     module is not initialized properly
 * \return         0         Successfully return the detected EP numbers
 */
int ifx_pcie_ep_dev_num_get(int *dev_num);

/**
 * This function returns detailed EP device information for PPE/DSL/Bonding
 * partner by its logical index obtained
 * by \ref ifx_pcie_ep_dev_num_get and its interrupt module number
 * \ref ifx_pcie_ep_int_module_t
 *
 * \param[in]      dev_idx   Logical device index referred to the related
 *			     device
 * \param[in]      module    EP interrupt module user<PPE/MEI/eDMA/CDMA>
 * \param[in/out]  dev       Pointer to returned detail device structure
 *			     \ref ifx_pcie_ep_dev_t
 * \return         -EIO      Invalid logical device index or too many modules
 *			     referred to this module
 * \return         0         Successfully return required device information
 *
 * \remarks This function normally will be called to trace the detailed device
 *	    information after calling \ref ifx_pcie_ep_dev_num_get
 */
int ifx_pcie_ep_dev_info_req(int dev_idx, ifx_pcie_ep_int_module_t module,
			     ifx_pcie_ep_dev_t *dev);

/**
 * This function releases the usage of this module by PPE/DSL
 *
 * \param[in]  dev_idx   Logical device index referred to the related device
 * \return     -EIO      Invalid logical device index or release too many
 *			 times to refer to this module
 *  \return     0         Successfully release the usage of this module
 *
 * \remarks This function should be called once their reference is over.
 *	    The reference usage must matches \ref ifx_pcie_ep_dev_info_req
 */
int ifx_pcie_ep_dev_info_release(int dev_idx);

/* @} */
#endif /* LANTIQ_PCIE_H */
