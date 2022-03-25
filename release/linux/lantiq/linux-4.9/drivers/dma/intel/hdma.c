/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2014 ~ 2015 Lei Chuanhua <chuanhua.lei@lantiq.com>
 *  Copyright (C) 2016 ~ 2017 Intel Corporation.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/export.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/bitmap.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/slab.h>

#include "hdma.h"

#define MS(_v, _f)  (((_v) & (_f)) >> _f##_S)
#define SM(_v, _f)  (((_v) << _f##_S) & (_f))

#define DMA_CLC			0x0000

#define DMA_ID			0x0008
#define DMA_ID_REV		0xFFu
#define DMA_ID_REV_S		0
#define DMA_ID_ID		0xFF00u
#define DMA_ID_ID_S		8
#define DMA_ID_PRTNR		0xF0000u
#define DMA_ID_PRTNR_S		16
#define DMA_ID_CHNR		0x7F00000u
#define DMA_ID_CHNR_S		20

#define DMA_CTRL		0x0010
#define DMA_CTRL_RST		BIT(0)
#define DMA_CTRL_DSRAM_PATH	BIT(1)
#define DMA_CTRL_CH_FL		BIT(6)
#define DMA_CTRL_DS_FOD		BIT(7)
#define DMA_CTRL_DRB		BIT(8)
#define DMA_CTRL_ENBE		BIT(9)
#define DMA_CTRL_PRELOAD_INT_S	10
#define DMA_CTRL_PRELOAD_INT	0x0C00u
#define DMA_CTRL_PRELOAD_EN	BIT(12)
#define DMA_CTRL_MBRST_CNT_S	16
#define DMA_CTRL_MBRST_CNT	0x3FF0000u
#define DMA_CTRL_MBRSTARB	BIT(30)
#define DMA_CTRL_PKTARB		BIT(31)

#define DMA_CPOLL		0x0014
#define DMA_CPOLL_CNT_S		4
#define DMA_CPOLL_CNT		0xFFF0u
#define DMA_CPOLL_EN		BIT(31)

#define DMA_CGBL		0x0030
#define DMA_CGBL_GBL_S		0
#define DMA_CGBL_GBL		0xFFFFu

#define DMA_CS			0x0018
#define DMA_CS_MASK		0x3Fu

#define DMA_CCTRL		0x001C
#define DMA_CCTRL_ON		BIT(0)
#define DMA_CCTRL_RST		BIT(1)
#define DMA_CCTRL_CH_POLL_EN	BIT(2) /* DMA V3 */
#define DMA_CCTRL_DIR_TX	BIT(8)
#define DMA_CCTRL_CLASS_S	9
#define DMA_CCTRL_CLASS		0xE00u
#define DMA_CCTRL_PRTNR_S	12
#define DMA_CCTRL_PRTNR		0xF000u
#define DMA_CCTRL_TXWGT_S	16
#define DMA_CCTRL_TXWGT		0x30000u
#define DMA_CCTRL_CLASSH_S	18
#define DMA_CCTRL_CLASSH	0xC0000u
#define DMA_CCTRL_TXWGT2	BIT(20) /* DMA V3 */
#define DMA_CCTRL_PDEN		BIT(23)
#define DMA_CCTRL_P2PCPY	BIT(24)
#define DMA_CCTRL_LBEN		BIT(25)
#define DMA_CCTRL_LBCHNR_S	26
#define DMA_CCTRL_LBCHNR	0xFC000000u
#define DMA_MAX_CLASS		31 /* 5 bits */

#define DMA_CDBA		0x0020

#define DMA_CDLEN		0x0024
#define DMA_CDLEN_CDLEN_S	0
#define DMA_CDLEN_CDLEN		0xFFFu

#define DMA_CIS			0x0028
#define DMA_CIE			0x002C

#define DMA_CI_EOP		BIT(1)
#define DMA_CI_DUR		BIT(2)
#define DMA_CI_DESCPT		BIT(3)
#define DMA_CI_CHOFF		BIT(4)
#define DMA_CI_RDERR		BIT(5)
#define DMA_CI_ALL	(DMA_CI_EOP | DMA_CI_DUR | DMA_CI_DESCPT\
			| DMA_CI_CHOFF | DMA_CI_RDERR)

#define DMA_CI_DEFAULT (DMA_CI_EOP | DMA_CI_DESCPT)

#define DMA_CDPTNR		0x0034
#define DMA_PS			0x0040
#define DMA_PS_PS_S		0
#define DMA_PS_PS		0xFu

#define DMA_PCTRL		0x0044
#define DMA_PCTRL_RXBL16	BIT(0)
#define DMA_PCTRL_TXBL16	BIT(1)
#define DMA_PCTRL_RXBL_S	2
#define DMA_PCTRL_RXBL		0xCu
#define DMA_PCTRL_TXBL_S	4
#define DMA_PCTRL_TXBL		0x30u
#define DMA_PCTRL_PDEN		BIT(6)
#define DMA_PCTRL_RXBL32	BIT(7)
#define DMA_PCTRL_PDEN_S	6
#define DMA_PCTRL_RXENDI	0x300u
#define DMA_PCTRL_RXENDI_S	8
#define DMA_PCTRL_TXENDI	0xC00u
#define DMA_PCTRL_TXENDI_S	10
#define DMA_PCTRL_TXWGT		0x7000u
#define DMA_PCTRL_TXWGT_S	12
#define DMA_PCTRL_TXBL32	BIT(15)
#define DMA_PCTRL_MEM_FLUSH	BIT(16)
#define DMA_DBG_P2D_CLS		0x1E0000u
#define DMA_DBG_P2D_CLS_S	17
#define DMA_DBG_P2D_ACK		BIT(21)
#define DMA_DBG_D2P_CLS		0x3C00000u
#define DMA_DBG_D2P_CLS_S	22
#define DMA_DBG_D2P_ACK		BIT(26)
#define DMA_DBG_P2D_REQ		BIT(27)
#define DMA_DBG_D2P_REQ		BIT(28)
#define DMA_DBG_P2D_RME		BIT(29)
#define DMA_DBG_D2P_XME		BIT(30)
#define DMA_DBG_P2D_JMB		BIT(31)

#define DMA_CPDCNT	0x0080

#define DMA_IRNEN	0x00F4
#define DMA_IRNCR	0x00F8
#define DMA_IRNICR	0x00FC
#define DMA_IRNEN1	0x00E8
#define DMA_IRNCR1	0x00EC
#define DMA_IRNICR1	0x00F0

/* DMA V3.0 new registers */
#define DMA_C_DP_TICK		0x100

#define DMA_C_DP_TICK_TIKNARB_S		0
#define DMA_C_DP_TICK_TIKNARB		0xFFFFu
#define DMA_C_DP_TICK_TIKARB_S		16
#define DMA_C_DP_TICK_TIKARB		0xFFFF0000u

#define DMA_C_HDRM		0x110

#define DMA_C_HDRM_HDR_LEN_S	0
#define DMA_C_HDRM_HDR_LEN	0xFFu
#define DMA_C_HDRM_EN		BIT(31)

#define DMA_C_BOFF		0x120

#define DMA_C_BOFF_BOF_LEN_S	0
#define DMA_C_BOFF_BOF_LEN	0xFFu
#define DMA_C_BOFF_EN		BIT(31)

#define DMA_C_INTCO		0x140
#define DMA_C_INTCO_COAL_LEN_S	0
#define DMA_C_INTCO_COAL_LEN	0xFFu
#define DMA_C_INTCO_TO_LEN	0xFF00u
#define DMA_C_INTCO_TO_LEN_S	8
#define DMA_C_INTCO_EN		BIT(31)
#define DMA_C_INTCO_TO_EN	BIT(30)
#define DMA_C_INTCO_TO_ST	BIT(29)
#define DMA_C_INTCO_TO		0x1FFF0000
#define DMA_C_INTCO_TO_S	16

#define DMA_C_INTCO_PEND	0x0000FF00
#define DMA_C_INTCO_PEND_S	8

#define DMA_C_SWPOLL		0x150
#define DMA_C_SWPOLL_MUX	BIT(30)
#define DMA_C_SWPOLL_EN		BIT(31)

#define DMA_ORRC		0x190
#define DMA_ORRC_ORRCNT_S	4

#define DMA_ORRC_ORRCNT		0x1F0u
#define DMA_ORRC_EN		BIT(31)

#define DMA_LOG_CH		0x194
#define DMA_LOG_CH_NR_S		0
#define DMA_LOG_CH_NR		0x3Fu

#define DMA_C_ENDIAN		0x200

#define DMA_C_END_DATAENDI_S	0
#define DMA_C_END_DATAENDI	0x3u
#define DMA_C_END_DE_EN		BIT(7)

#define DMA_C_END_DESENDI_S	8
#define DMA_C_END_DESENDI	0x300u
#define DMA_C_END_DES_EN	BIT(16)

/* Turn a irq_data into a dma controller */
#define itoc(i)		((struct dma_ctrl *)irq_get_chip_data((i)->irq))
#define dma_id_to_controller(id)	(&ltq_dma_controller[(id)])

static const char *const dma_name[] = {
	"dma0",
	"dma1tx",
	"dma1rx",
	"dma2tx",
	"dma2rx",
	"dma3",
	"dma4",
};

static const char *const dma0_port[] = {
	"SPI0", "SPI1", "HSNAN", "MCPY",
};

static const char *const arb_type_array[] = {
	"Single Burst",
	"Multi Burst",
	"Packet",
};

static struct dma_ctrl ltq_dma_controller[DMAMAX];
static int dma_chan_data_buf_free(struct dmax_chan *pch);
static void dma_chan_desc_free(struct dmax_chan *pch);

static char const *dma_get_name_by_cid(int cid)
{
	return dma_name[cid];
}

/* Handy dma register accessor */
static inline unsigned int ltq_dma_r32(struct dma_ctrl *pctrl, u32 offset)
{
	return ioread32(pctrl->membase + offset);
}

static inline void ltq_dma_w32(struct dma_ctrl *pctrl, u32 value, u32 offset)
{
	iowrite32(value, pctrl->membase + offset);
}

static inline void ltq_dma_w32_mask(struct dma_ctrl *pctrl, u32 clr, u32 set,
				    u32 offset)
{
	ltq_dma_w32(pctrl, (ltq_dma_r32(pctrl, offset) & ~clr) | set,  offset);
}

struct dma_ctrl *dma_cid_get_controller(int cid)
{
	return dma_id_to_controller(cid);
}

static struct dma_ctrl *dma_port_get_controller(struct dma_port *port)
{
	if (!port)
		return NULL;
	return port->controller;
}

static struct dma_ctrl *dma_chan_get_controller(struct dmax_chan *pch)
{
	if (!pch)
		return NULL;
	return pch->controller;
}

struct dma_port *dma_cid_pid_get_port(int cid, int pid)
{
	struct dma_ctrl *pctrl = dma_id_to_controller(cid);

	if (cid > DMA0 && pid > 0)
		return NULL;
	if ((cid == DMA0) && (pid >= (pctrl->port_nrs - 1)))
		return NULL;
	return &pctrl->ports[pid];
}

static struct dma_port *dma_ctrl_pid_get_port(struct dma_ctrl *pctrl, int pid)
{
	if (!pctrl)
		return NULL;
	return &pctrl->ports[pid];
}

static struct dma_port *dma_chan_get_port(struct dmax_chan *pch)
{
	if (!pch)
		return NULL;
	return pch->port;
}

static struct dmax_chan *dma_cid_pid_nid_get_chan(int cid, int pid, int nid)
{
	int i;
	struct dma_ctrl *pctrl = dma_id_to_controller(cid);
	struct dma_port *pport = dma_ctrl_pid_get_port(pctrl, pid);

	if (cid > DMA0 && pid > 0)
		return NULL;
	if ((cid == DMA0) && (pid >= (pctrl->port_nrs - 1)))
		return NULL;
	/* Due to DMA0 channel, search is necessary */
	for (i = 0; i < pport->chan_nrs; i++) {
		if (pport->chans[i].nr == nid)
			return &pport->chans[i];
	}
	return NULL;
}

struct dmax_chan *dma_ctrl_pid_nid_get_chan(struct dma_ctrl *pctrl, int pid,
					    int nid)
{
	int i;
	struct dma_port *pport = dma_ctrl_pid_get_port(pctrl, pid);

	if (WARN_ON(!pport))
		return ERR_PTR(-EINVAL);
	for (i = 0; i < pport->chan_nrs; i++) {
		if (pport->chans[i].nr == nid)
			return &pport->chans[i];
	}
	return NULL;
}

struct dmax_chan *dma_port_nid_get_chan(struct dma_port *port, int nid)
{
	int i;

	for (i = 0; i < port->chan_nrs; i++) {
		if (port->chans[i].nr == nid)
			return &port->chans[i];
	}
	return NULL;
}

static int dma_set_port_controller_data(struct dma_port *port,
					struct dma_ctrl *pctrl)
{
	port->controller = pctrl;
	return 0;
}

static int dma_set_chan_controller_data(struct dmax_chan *pch,
					struct dma_ctrl *pctrl)
{
	pch->controller = pctrl;
	return 0;
}

static int dma_set_chan_port_data(struct dmax_chan *pch, struct dma_port *port)
{
	pch->port = port;
	return 0;
}

static inline bool dma_is_64bit(struct dma_ctrl *ctrl)
{
	return (ctrl->flags & DMA_CTL_64BIT) ? true : false;
}

static inline bool dma_chan_tx(struct dmax_chan *pch)
{
	return (pch->flags & DMA_TX_CH) ? true : false;
}

static inline bool dma_chan_rx(struct dmax_chan *pch)
{
	return (pch->flags & DMA_RX_CH) ? true : false;
}

static inline bool dma_chan_in_use(struct dmax_chan *pch)
{
	return (pch->flags & CHAN_IN_USE) ? true : false;
}

static inline bool dma_chan_desc_alloc_by_device(struct dmax_chan *pch)
{
	return (pch->flags & DEVICE_ALLOC_DESC) ? true : false;
}

static inline bool dma_chan_controlled_by_device(struct dmax_chan *pch)
{
	return (pch->flags & DEVICE_CTRL_CHAN) ? true : false;
}

static inline bool dma_chan_is_hw_desc(struct dmax_chan *pch)
{
	return (pch->flags & DMA_HW_DESC) ? true : false;
}

static void dma_ctrl_reset(struct dma_ctrl *pctrl)
{
	unsigned long flags;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_RST, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

/* DMA controller related configuration */
static void dma_ctrl_pkt_arb_cfg(struct dma_ctrl *pctrl, int enable)
{
	if (enable) {
		ltq_dma_w32_mask(pctrl, DMA_CTRL_MBRSTARB, 0, DMA_CTRL);
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_PKTARB, DMA_CTRL);
	} else {
		ltq_dma_w32_mask(pctrl, DMA_CTRL_PKTARB, 0, DMA_CTRL);
	}
}

static void dma_ctrl_arb_cfg(struct dma_ctrl *pctrl)
{
	unsigned long flags;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	switch (pctrl->arb_type) {
	case DMA_ARB_BURST:
		ltq_dma_w32_mask(pctrl, DMA_CTRL_PKTARB, 0, DMA_CTRL);
		ltq_dma_w32_mask(pctrl, DMA_CTRL_MBRSTARB, 0, DMA_CTRL);
		break;
	case DMA_ARB_MUL_BURST:
		ltq_dma_w32_mask(pctrl, DMA_CTRL_PKTARB, 0, DMA_CTRL);
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_MBRSTARB, DMA_CTRL);
		ltq_dma_w32_mask(pctrl, DMA_CTRL_MBRSTARB,
				 SM(DMA_ARB_MUL_BURST_DEFAULT,
				    DMA_CTRL_MBRST_CNT), DMA_CTRL);
		break;
	case DMA_ARB_PKT:
		dma_ctrl_pkt_arb_cfg(pctrl, 1);
		break;
	}
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_sram_desc_cfg(struct dma_ctrl *pctrl, int enable)
{
	unsigned long flags;

	if ((pctrl->cid != DMA3) && (pctrl->cid != DMA4) &&
	    (pctrl->cid != DMA1TX))
		return;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_DSRAM_PATH, DMA_CTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CTRL_DSRAM_PATH, 0, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_chan_flow_ctl_cfg(struct dma_ctrl *pctrl, int enable)
{
	unsigned long flags;

	if ((pctrl->cid != DMA1TX) && (pctrl->cid != DMA2TX))
		return;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_CH_FL, DMA_CTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CTRL_CH_FL, 0, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_global_polling_enable(struct dma_ctrl *pctrl)
{
	u32 reg = 0;
	unsigned long flags;

	reg |= DMA_CPOLL_EN;
	reg |= (u32)(SM(pctrl->pollcnt, DMA_CPOLL_CNT));
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32_mask(pctrl, DMA_CPOLL_CNT, reg, DMA_CPOLL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_desc_fetch_on_demand_cfg(struct dma_ctrl *pctrl,
					      int enable)
{
	unsigned long flags;

	if ((pctrl->cid == DMA0) || (pctrl->cid == DMA3) ||
	    (pctrl->cid == DMA4))
		return;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_DS_FOD, DMA_CTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CTRL_DS_FOD, 0, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_desc_read_back_cfg(struct dma_ctrl *pctrl, int enable)
{
	unsigned long flags;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_DRB, DMA_CTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CTRL_DRB, 0, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_byte_enable_cfg(struct dma_ctrl *pctrl, int enable)
{
	unsigned long flags;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CTRL_ENBE, DMA_CTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CTRL_ENBE, 0, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ctrl_labcnt_cfg(struct dma_ctrl *pctrl)
{
	u32 reg = 0;
	unsigned long flags;

	if (pctrl->cid != DMA1TX)
		return;

	if (pctrl->labcnt <= 0)
		return;
	reg |= DMA_CTRL_PRELOAD_EN;
	reg |= (u32)(SM(pctrl->labcnt, DMA_CTRL_PRELOAD_INT));
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32_mask(pctrl, DMA_CTRL_PRELOAD_INT, reg, DMA_CTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

/*
 * orr_cnt >= 16, it will be 16
 * 4 <= orr_cnt < 16, it ill be orr_cnt
 * orr_cnt < 4, it will be 3. Minimum 3 orr supported
 */
static void dma_ctrl_orrc_cfg(struct dma_ctrl *pctrl)
{
	u32 val = 0;

	if (pctrl->ver <= DMA_VER_22)
		return;

	/* Only valid for DMA TX and memory copy DMA */
	if (pctrl->cid == DMA1RX || pctrl->cid == DMA2RX)
		return;

	if (pctrl->orrc <= 0 || pctrl->orrc > DMA_ORRC_MAX_CNT) {
		ltq_dma_w32_mask(pctrl, DMA_ORRC_EN | DMA_ORRC_ORRCNT, 0,
				 DMA_ORRC);
	} else {
		val = DMA_ORRC_EN | SM(pctrl->orrc, DMA_ORRC_ORRCNT);
		ltq_dma_w32(pctrl, val, DMA_ORRC);
	}
}

static int dma_ctrl_cfg(struct dma_ctrl *pctrl)
{
	int enable;

	if ((pctrl->flags & DMA_FLCTL))
		enable = 1;
	else
		enable = 0;
	dma_ctrl_chan_flow_ctl_cfg(pctrl, enable);

	if ((pctrl->flags & DMA_DESC_IN_SRAM))
		enable = 1;
	else
		enable = 0;
	dma_ctrl_sram_desc_cfg(pctrl, enable);

	dma_ctrl_arb_cfg(pctrl);
	if ((pctrl->flags & DMA_DRB))
		enable = 1;
	else
		enable = 0;
	dma_ctrl_desc_read_back_cfg(pctrl, enable);

	if ((pctrl->flags & DMA_EN_BYTE_EN))
		enable = 1;
	else
		enable = 0;
	dma_ctrl_byte_enable_cfg(pctrl, enable);
	dma_ctrl_labcnt_cfg(pctrl);
	dma_ctrl_orrc_cfg(pctrl);
	dma_ctrl_global_polling_enable(pctrl);
	dev_dbg(pctrl->dev, "%s Controller 0x%08x configuration done\n",
		pctrl->name, ltq_dma_r32(pctrl, DMA_CTRL));
	return 0;
}

static int dma_chan_cctrl_cfg(struct dmax_chan *pch, u32 val)
{
	u32 reg;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);
	unsigned long flags;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	reg = ltq_dma_r32(pctrl, DMA_CCTRL);

	/* Read from hardware */
	if ((reg & DMA_CCTRL_DIR_TX))
		pch->flags |= DMA_TX_CH;
	else
		pch->flags |= DMA_RX_CH;

	/* Keep the class value unchanged */
	reg &= (DMA_CCTRL_CLASS | DMA_CCTRL_CLASSH);
	val |= reg;
	ltq_dma_w32(pctrl, val, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	return 0;
}

static u32 dma_chan_get_class(struct dmax_chan *pch)
{
	u32 reg;
	unsigned long flags;
	u32 class_val = 0;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	reg = ltq_dma_r32(pctrl, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	/* Keep the class value unchanged */
	/* Lower three bits */
	class_val = MS(reg, DMA_CCTRL_CLASS);
	class_val |= MS(reg, DMA_CCTRL_CLASSH) << 3;
	return class_val;
}

static int dma_chan_set_class(struct dmax_chan *pch, u32 val)
{
	unsigned long flags;
	u32 class_val;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	if (val > DMA_MAX_CLASS)
		return -EINVAL;
	/* 3 bits low */ 	
	class_val = SM((val & 0x7), DMA_CCTRL_CLASS);
	/* 2 bits high */
	class_val |= SM(((val >> 3) & 0x3), DMA_CCTRL_CLASSH);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32_mask(pctrl, DMA_CCTRL_CLASS | DMA_CCTRL_CLASSH,
		class_val, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	return 0;
}

static struct dmax_chan *dma_chan_l2p(u32 chan)
{
	int cid = _DMA_CONTROLLER(chan);
	int pid = _DMA_PORT(chan);
	int nid = _DMA_CHANNEL(chan);
	struct dmax_chan *pch = dma_cid_pid_nid_get_chan(cid, pid, nid);
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (!pch) {
		dev_err(pctrl->dev, "%s l2p failed from %d.%d.%d\n",
			pctrl->name, cid, pid, nid);
		return NULL;
	}
	if (WARN_ON(!dma_chan_in_use(pch)))
		return ERR_PTR(-EBUSY);
	return pch;
}

static dma_addr_t dma_chan_get_desc_phys_base(struct dmax_chan *pch)
{
	return pch->desc_phys;
}

dma_addr_t ltq_dma_chan_get_desc_phys_base(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	return dma_chan_get_desc_phys_base(pch);
}
EXPORT_SYMBOL(ltq_dma_chan_get_desc_phys_base);

static u32 dma_chan_get_desc_vir_base(struct dmax_chan *ch)
{
	return ch->desc_base;
}

u32 ltq_dma_chan_get_desc_vir_base(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	return dma_chan_get_desc_vir_base(pch);
}
EXPORT_SYMBOL(ltq_dma_chan_get_desc_vir_base);

/* DMA channel related configuration */
static void dma_chan_on(struct dmax_chan *pch)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32_mask(pctrl, 0, DMA_CCTRL_ON, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	pch->onoff = 1;
}

int ltq_dma_chan_on(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	/* If descriptors not configured, not allow to turn on channel */
	if (WARN_ON(!pch || !pch->desc_configured))
		return -EINVAL;
	dma_chan_on(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_on);

static void dma_chan_off(struct dmax_chan *pch)
{
	u32 reg;
	int i = 10000;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32_mask(pctrl, DMA_CCTRL_ON, 0, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);

	/* Wait for channel off to complete */
	while (((reg = ltq_dma_r32(pctrl, DMA_CCTRL)) & DMA_CCTRL_ON) && i--)
		;
	if (i == 0)
		dev_err(pctrl->dev, "%s chan %d off failed\n",
			pctrl->name, pch->nr);
	pch->onoff = 0;
}

int ltq_dma_chan_off(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_off(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_off);

static void dma_chan_desc_hw_cfg(struct dmax_chan *pch, dma_addr_t desc_base,
				 int desc_num)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32(pctrl, desc_base, DMA_CDBA);
	ltq_dma_w32(pctrl, desc_num, DMA_CDLEN);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	pch->desc_configured = true;
}

/*
 * Descriptor base address and data pointer must be physical address when
 * writen to the register.
 * This API will be used by CBM which configure hardware descriptor.
 */
static void dma_chan_desc_cfg(struct dmax_chan *pch, dma_addr_t desc_base,
			      int desc_num)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (desc_num == 0 || desc_num > DMA_MAX_DESC_NUM) {
		dev_err(pctrl->dev,
			"%s must allocate descriptor first\n", pch->device_id);
		return;
	}

	if (((pctrl->ver <= DMA_VER_22) && desc_num > DMA_MAX_DESC_NUM) ||
	    ((pctrl->ver >= DMA_VER_30) && desc_num > DMAV3_MAX_DESC_NUM)) {
		dev_err(pctrl->dev,
			"%s descriptor number out of range %d\n",
			pch->device_id, desc_num);
		return;
	}

	mutex_lock(&pch->ch_lock);
	pch->flags |= DMA_HW_DESC;
	pch->desc_len = desc_num;
	pch->desc_phys = desc_base;
	mutex_unlock(&pch->ch_lock);
	dma_chan_desc_hw_cfg(pch, desc_base, desc_num);
}

int ltq_dma_chan_desc_cfg(u32 chan, dma_addr_t desc_base, int desc_num)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_desc_cfg(pch, desc_base, desc_num);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_desc_cfg);

static void dma_chan_2dws_desc_reset(struct dmax_chan *pch)
{
	int i;

	WARN_ON(!pch->desc_len);
	WARN_ON(dma_chan_rx(pch) && dma_chan_tx(pch));

	if (dma_chan_tx(pch)) {
		struct dma_tx_desc_2dw *tx_desc_p;

		for (i = 0; i < pch->desc_len; i++) {
			tx_desc_p =
				(struct dma_tx_desc_2dw *)pch->desc_base + i;
			tx_desc_p->status.field.own = CPU_OWN;
			/*
			 * Ensure owner bit change to take effect before
			 * other change
			 */
			wmb();
		}
	}

	if (dma_chan_rx(pch)) {
		struct dma_rx_desc_2dw *rx_desc_p;

		for (i = 0; i < pch->desc_len; i++) {
			rx_desc_p =
				(struct dma_rx_desc_2dw *)pch->desc_base + i;
			rx_desc_p->status.field.c = 1;
			rx_desc_p->status.field.own = CPU_OWN;
			/*
			 * Ensure owner bit change to take effect before
			 * other change
			 */
			wmb();
		}
	}
}

static void dma_chan_4dws_desc_reset(struct dmax_chan *pch)
{
	int i;

	WARN_ON(!pch->desc_len);
	WARN_ON(dma_chan_rx(pch) && dma_chan_tx(pch));

	if (dma_chan_tx(pch)) {
		struct dma_tx_desc *tx_desc_p;

		for (i = 0; i < pch->desc_len; i++) {
			tx_desc_p = (struct dma_tx_desc *)pch->desc_base + i;
			tx_desc_p->status.field.own = CPU_OWN;
			/*
			 * Ensure owner bit change to take effect before
			 * other change
			 */
			wmb();
		}
	}

	if (dma_chan_rx(pch)) {
		struct dma_rx_desc *rx_desc_p;

		for (i = 0; i < pch->desc_len; i++) {
			rx_desc_p = (struct dma_rx_desc *)pch->desc_base + i;
			rx_desc_p->status.field.c = 1;
			rx_desc_p->status.field.own = CPU_OWN;
			/*
			 * Ensure owner bit change to take effect before
			 * other change
			 */
			wmb();
		}
	}
}

static void dma_chan_desc_reset(struct dmax_chan *pch)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_chan_is_hw_desc(pch))
		return;
	if (dma_is_64bit(pctrl))
		dma_chan_4dws_desc_reset(pch);
	else
		dma_chan_2dws_desc_reset(pch);
}

static void dma_chan_reset(struct dmax_chan *pch)
{
	int i = 10000;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	dma_chan_off(pch);
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32_mask(pctrl, 0, DMA_CCTRL_RST, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	while ((ltq_dma_r32(pctrl, DMA_CCTRL) & DMA_CCTRL_RST) && i--)
		;
	if (i == 0)
		dev_err(pctrl->dev, "%s chan %d reset failed\n",
			pctrl->name, pch->nr);
	pch->rst = 1;
	/* Pointer to the back clear starting point */
	dma_chan_desc_hw_cfg(pch, pch->desc_phys, pch->desc_len);
	pch->curr_desc = 0;
	pch->prev_desc = 0;
	/* Descriptor reset */
	dma_chan_desc_reset(pch);
}

int ltq_dma_chan_reset(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_reset(pch);
	pch->desc_configured = false;
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_reset);

static void dma_chan_irq_enable(struct dmax_chan *pch)
{
	u32 val = DMA_CI_EOP;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	/* TX only enable EOP interrupt */
	if (dma_chan_rx(pch))
		val |= DMA_CI_DESCPT;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32(pctrl, val, DMA_CIE);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_irq_enable(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_irq_enable(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_irq_enable);

static void dma_chan_irq_disable(struct dmax_chan *pch)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32(pctrl, 0, DMA_CIE);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_irq_disable(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_irq_disable(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_irq_disable);

static void dma_chan_open(struct dmax_chan *pch)
{
	/* chann on, then enable irq */
	dma_chan_on(pch);
	if (dma_chan_rx(pch))
		dma_chan_irq_enable(pch);
}

int ltq_dma_chan_open(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_open(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_open);

static void dma_chan_close(struct dmax_chan *pch)
{
	dma_chan_off(pch);
	dma_chan_irq_disable(pch);
}

int ltq_dma_chan_close(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_close(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_close);

static void dma_chan_pkt_drop_cfg(struct dmax_chan *pch, int enable)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if ((dma_chan_tx(pch)))
		return;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CCTRL_PDEN, DMA_CCTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CCTRL_PDEN, 0, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_pkt_drop_cfg(u32 chan, int enable)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_pkt_drop_cfg(pch, enable);
	mutex_lock(&pch->ch_lock);
	pch->pden = enable;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_pkt_drop_cfg);

static void dma_chan_txwgt_cfg(struct dmax_chan *pch, int txwgt)
{
	u32 lwgt;
	u32 hwgt;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (txwgt < DMA_CHAN_TXWGT0 || txwgt >= DMA_CHAN_TXWGTMAX) {
		dev_err(pctrl->dev,
			"Invalid channel tx weight %d <%d~%d>\n",
			txwgt, DMA_CHAN_TXWGT0, DMA_CHAN_TXWGTMAX);
		return;
	}

	if ((pctrl->ver <= DMA_VER_22) && (txwgt > DMA_V22_TXWGTMAX)) {
		dev_err(pctrl->dev,
			"Invalid channel tx weight %d for DMA V22\n", txwgt);
		return;
	}

	lwgt = txwgt & 0x3; /* Get lower two bits */
	hwgt = txwgt & 0x4; /* Get higer one bit */

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32_mask(pctrl, DMA_CCTRL_TXWGT, SM(lwgt, DMA_CCTRL_TXWGT),
			 DMA_CCTRL);
	if (hwgt)
		ltq_dma_w32_mask(pctrl, 0, DMA_CCTRL_TXWGT2, DMA_CCTRL);

	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_txwgt_cfg(u32 chan, int txwgt)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_txwgt_cfg(pch, txwgt);
	mutex_lock(&pch->ch_lock);
	pch->txwgt = txwgt;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_txwgt_cfg);

int ltq_dma_chan_pktsize_cfg(u32 chan, size_t pktsize)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	if (pktsize > DMA_MAX_PKT_SIZE)
		return -EINVAL;
	mutex_lock(&pch->ch_lock);
	pch->pkt_size = pktsize;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_pktsize_cfg);

static void dma_chan_polling_cfg(struct dmax_chan *pch, u32 nonarb_cnt,
				 u32 arb_cnt)
{
	u32 val = 0;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);

	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	if (nonarb_cnt == 0 || arb_cnt == 0) {
		ltq_dma_w32(pctrl, 0, DMA_C_DP_TICK);
		/* Ensure counter ready, then enable it */
		wmb();
		ltq_dma_w32_mask(pctrl, DMA_CCTRL_CH_POLL_EN, 0, DMA_CCTRL);
	} else {
		val = SM(nonarb_cnt, DMA_C_DP_TICK_TIKNARB)
			| SM(arb_cnt, DMA_C_DP_TICK_TIKARB);
		ltq_dma_w32(pctrl, val, DMA_C_DP_TICK);
		/* Ensure counter ready, then enable it */
		wmb();
		ltq_dma_w32_mask(pctrl, 0, DMA_CCTRL_CH_POLL_EN, DMA_CCTRL);
	}
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_polling_cfg(u32 chan, u32 nonarb_cnt, u32 arb_cnt)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	if (arb_cnt > nonarb_cnt)
		return -EINVAL;

	dma_chan_polling_cfg(pch, nonarb_cnt, arb_cnt);

	mutex_lock(&pch->ch_lock);
	pch->nonarb_cnt = nonarb_cnt;
	pch->arb_cnt = arb_cnt;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_polling_cfg);

static void dma_chan_hdrm_cfg(struct dmax_chan *pch, u32 hdr_len)
{
	u32 val = 0;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (WARN_ON(dma_chan_rx(pch)))
		return;

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	if (hdr_len <= 0 || hdr_len > DMA_CHAN_HDRM_MAX) {
		ltq_dma_w32_mask(pctrl, DMA_C_HDRM_EN | DMA_C_HDRM_HDR_LEN,
				 0, DMA_C_HDRM);
	} else {
		val = SM(hdr_len, DMA_C_HDRM_HDR_LEN) | DMA_C_HDRM_EN;
		ltq_dma_w32(pctrl, val, DMA_C_HDRM);
	}
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_hdrm_cfg(u32 chan, u32 hdr_len)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_hdrm_cfg(pch, hdr_len);

	mutex_lock(&pch->ch_lock);
	pch->hdrm_len = hdr_len;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_hdrm_cfg);

static void dma_chan_byte_offset_cfg(struct dmax_chan *pch, u32 boff_len)
{
	u32 val = 0;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	if (boff_len <= 0 || boff_len > DMA_CHAN_BOFF_MAX) {
		ltq_dma_w32_mask(pctrl, DMA_C_BOFF_EN | DMA_C_BOFF_BOF_LEN,
				 0, DMA_C_BOFF);
	} else {
		val = SM(boff_len, DMA_C_BOFF_BOF_LEN) | DMA_C_BOFF_EN;
		ltq_dma_w32(pctrl, val, DMA_C_BOFF);
	}
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_byte_offset_cfg(u32 chan, u32 boff_len)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_byte_offset_cfg(pch, boff_len);

	mutex_lock(&pch->ch_lock);
	pch->boff_len = boff_len;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_byte_offset_cfg);

/*
 * Interrupt coalesce is working like EOP so EOP interrupt must be enabled
 * TO interrupt has to be cleard and even pending interrupts have to be
 * cleared. Refer to low level test
 */
static void dma_chan_int_coalesc_cfg(struct dmax_chan *pch, u32 coal_len,
				     u32 timeout)
{
	u32 val = 0;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	if (coal_len <= 0 || coal_len > DMA_CHAN_COAL_MAX) {
		ltq_dma_w32_mask(pctrl, DMA_C_INTCO_EN |
				 DMA_C_INTCO_COAL_LEN | DMA_C_INTCO_TO_EN,
				 0, DMA_C_INTCO);
	} else {
		val = SM(coal_len, DMA_C_INTCO_COAL_LEN) |
			 SM(timeout, DMA_C_INTCO_TO) |
			 DMA_C_INTCO_TO_EN | DMA_C_INTCO_EN;
		ltq_dma_w32(pctrl, val, DMA_C_INTCO);
	}
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_int_coalesc_cfg(u32 chan, u32 coal_len, u32 timeout)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_int_coalesc_cfg(pch, coal_len, timeout);

	mutex_lock(&pch->ch_lock);
	pch->coal_len = coal_len;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_int_coalesc_cfg);

static void dma_chan_sw_poll_cfg(struct dmax_chan *pch, int enable)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);

	ltq_dma_w32(pctrl, pch->nr, DMA_CS);

	/* Always enable for software polling, bit 31 contro on/off */
	ltq_dma_w32_mask(pctrl, 0, DMA_C_SWPOLL_MUX, DMA_C_SWPOLL);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_C_SWPOLL_EN, DMA_C_SWPOLL);
	else
		ltq_dma_w32_mask(pctrl, DMA_C_SWPOLL_EN, 0, DMA_C_SWPOLL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_sw_poll_cfg(u32 chan, int enable)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_sw_poll_cfg(pch, enable);

	mutex_lock(&pch->ch_lock);
	pch->sw_poll = enable;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_sw_poll_cfg);

static void dma_chan_data_endian_cfg(struct dmax_chan *pch, u32 endian_type)
{
	u32 val;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	val = DMA_C_END_DE_EN | SM(endian_type, DMA_C_END_DATAENDI);
	ltq_dma_w32_mask(pctrl, 0, val, DMA_C_ENDIAN);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_data_endian_cfg(u32 chan, u32 endian_type)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_data_endian_cfg(pch, endian_type);

	mutex_lock(&pch->ch_lock);
	pch->data_endian = endian_type;
	pch->data_endian_en = true;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_data_endian_cfg);

static void dma_chan_data_endian_disable(struct dmax_chan *pch)
{
	u32 val;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	val = DMA_C_END_DE_EN | DMA_C_END_DATAENDI;
	ltq_dma_w32_mask(pctrl, val, 0, DMA_C_ENDIAN);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_data_endian_disable(u32 chan)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_data_endian_disable(pch);

	mutex_lock(&pch->ch_lock);
	pch->data_endian_en = false;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_data_endian_disable);

static void dma_chan_desc_endian_cfg(struct dmax_chan *pch, u32 endian_type)
{
	u32 val;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	val = DMA_C_END_DES_EN | SM(endian_type, DMA_C_END_DESENDI);
	ltq_dma_w32_mask(pctrl, 0, val, DMA_C_ENDIAN);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_desc_endian_cfg(u32 chan, u32 endian_type)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_desc_endian_cfg(pch, endian_type);

	mutex_lock(&pch->ch_lock);
	pch->desc_endian = endian_type;
	pch->desc_endian_en = true;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_desc_endian_cfg);

static void dma_chan_desc_endian_disable(struct dmax_chan *pch)
{
	u32 val;
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	val = DMA_C_END_DES_EN | DMA_C_END_DESENDI;
	ltq_dma_w32_mask(pctrl, val, 0, DMA_C_ENDIAN);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

int ltq_dma_chan_desc_endian_disable(u32 chan)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	if (pctrl->ver <= DMA_VER_22)
		return -EPERM;

	dma_chan_desc_endian_disable(pch);

	mutex_lock(&pch->ch_lock);
	pch->desc_endian_en = false;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_desc_endian_disable);

/*
 * There is no user case for DMA ver 2.2 and new version
 * There is no DDR or descriptor involved at all
 * Only RX channel needs to be configure. However,
 * TX channel number has to be known before
 */
static void dma_chan_fast_path_cfg(struct dmax_chan *rx_ch,
				   struct dmax_chan *tx_ch, int enable)
{
	unsigned long flags;
	struct dma_ctrl *pctrl_rx = dma_chan_get_controller(rx_ch);
	struct dma_ctrl *pctrl_tx = dma_chan_get_controller(tx_ch);

	if (pctrl_rx != pctrl_tx)
		return;
	/* RX->TX, RX needs to configure TX channel in LPBKNR fields */
	spin_lock_irqsave(&pctrl_rx->ctrl_lock, flags);
	ltq_dma_w32(pctrl_rx, rx_ch->nr, DMA_CS);
	if (enable)
		ltq_dma_w32_mask(pctrl_rx, 0, DMA_CCTRL_LBEN |
			((tx_ch->nr & 0x3F) << DMA_CCTRL_LBCHNR_S), DMA_CCTRL);
	else
		ltq_dma_w32_mask(pctrl_rx, DMA_CCTRL_LBEN |
			((tx_ch->nr & 0x3F) << DMA_CCTRL_LBCHNR_S),
			0, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl_rx->ctrl_lock, flags);
	/* Update house keeping work info */
	rx_ch->lpbk_en = 1;
	rx_ch->lpbk_ch_nr = tx_ch->nr;
}

int ltq_dma_chan_fast_path_cfg(u32 tx_chan, u32 rx_chan, int enable)
{
	struct dmax_chan *rxch = dma_chan_l2p(rx_chan);
	struct dmax_chan *txch = dma_chan_l2p(tx_chan);

	if (WARN_ON((!rxch) || (!txch)))
		return -EINVAL;
	dma_chan_fast_path_cfg(rxch, txch, enable);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_fast_path_cfg);

static void dma_chan_p2p_cfg(struct dmax_chan *pch, int enable)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_chan_rx(pch)) {
		dev_err(pctrl->dev,
			"RX channel has no need to configure P2P bit\n");
		return;
	}
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	if (enable)
		ltq_dma_w32_mask(pctrl, 0, DMA_CCTRL_P2PCPY, DMA_CCTRL);
	else
		ltq_dma_w32_mask(pctrl, DMA_CCTRL_P2PCPY, 0, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_chan_global_buf_len_cfg(struct dmax_chan *pch, int data_len)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_chan_rx(pch)) {
		dev_err(pctrl->dev,
			"RX channel has no need to configure P2P bit\n");
		return;
	}
	if (data_len < 0 || data_len > DMA_MAX_PKT_SIZE)
		return;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32(pctrl, data_len, DMA_CGBL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static int dma_chan_desc_alloc(struct dmax_chan *pch, u32 desc_num)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (desc_num == 0 || desc_num > DMA_MAX_DESC_NUM) {
		dev_err(pctrl->dev,
			"%s must allocat descriptor first or out of range %d\n",
			pch->device_id, desc_num);
		return -EINVAL;
	}

	if (pch->desc_len == desc_num) {
		dev_err(pctrl->dev, "%s chan %d allocated already\n",
			pch->device_id, pch->nr);
		return -EINVAL;
	}

	pch->desc_len = desc_num;

	pch->desc_base = (u32)dmam_alloc_coherent(pctrl->dev,
				pch->desc_len * pctrl->desc_size,
				&pch->desc_phys, GFP_DMA);
	if (pch->desc_base == 0)
		return -ENOMEM;
	dev_dbg(pctrl->dev,
		"%s chan %d desc_base 0x%08x desc_phys 0x%08x\n",
		dma_get_name_by_cid(pctrl->cid), pch->nr, pch->desc_base,
		(u32)pch->desc_phys);
	memset((void *)pch->desc_base, 0, pch->desc_len * pctrl->desc_size);

	dma_chan_desc_hw_cfg(pch, pch->desc_phys, pch->desc_len);

	/* Save opt address pointer */
	pch->opt = (void **)devm_kzalloc(pctrl->dev,
		pch->desc_len * sizeof(void *), GFP_KERNEL);
	if (!pch->opt)
		return -ENOMEM;
	return 0;
}

int ltq_dma_chan_desc_alloc(u32 chan, u32 desc_num)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	return dma_chan_desc_alloc(pch, desc_num);
}
EXPORT_SYMBOL(ltq_dma_chan_desc_alloc);

static void dma_chan_desc_free(struct dmax_chan *pch)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (WARN_ON(dma_chan_is_hw_desc(pch)))
		return;

	if (WARN_ON(!pch->desc_base))
		return;
	dmam_free_coherent(pctrl->dev, pch->desc_len * pctrl->desc_size,
			   (void *)pch->desc_base, pch->desc_phys);
	pch->desc_base = 0;
	pch->desc_len = 0;
	pch->curr_desc = 0;
	pch->prev_desc = 0;
	if (!pch->opt)
		devm_kfree(pctrl->dev, pch->opt);
	pch->opt = NULL;
}

int ltq_dma_chan_desc_free(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_close(pch);
	dma_chan_desc_free(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_desc_free);

static int dma_rx_chan_data_buf_4dws_alloc(struct dmax_chan *pch)
{
	int i;
	int byte_offset;
	char *buffer;
	struct dma_rx_desc *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	for (i = 0; i < pch->desc_len; i++) {
		rx_desc_p = (struct dma_rx_desc *)pch->desc_base + i;
		buffer = pch->alloc(pch->pkt_size, &byte_offset,
				    &pch->opt[i]);
		if (!buffer) {
			dev_err(pctrl->dev, "No enough memory for %s\n",
				pch->device_id);
			return -ENOMEM;
		}
		rx_desc_p->data_pointer = dma_map_single(pctrl->dev, buffer,
			pch->pkt_size, DMA_FROM_DEVICE);
		if (dma_mapping_error(pctrl->dev, rx_desc_p->data_pointer)) {
			dev_err(pctrl->dev,
				"%s DMA map failed\n", __func__);
			pch->free(buffer, (void *)pch->opt[i]);
			buffer = NULL;
			break;
		}
		rx_desc_p->status.all = 0;
		rx_desc_p->status.field.sop = 1;
		rx_desc_p->status.field.eop = 1;
		rx_desc_p->status.field.c = 0;
		rx_desc_p->status.field.byte_offset = byte_offset;
		rx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		rx_desc_p->status.field.own = DMA_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	}
	return 0;
}

static int dma_tx_chan_data_buf_4dws_alloc(struct dmax_chan *pch)
{
	int i;
	int byte_offset;
	char *buffer;
	struct dma_tx_desc *tx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (pch->p2pcpy) {
		dev_err(pctrl->dev,
			"P2P TX channel use buffer from RX channel\n");
		return -EINVAL;
	}

	for (i = 0; i < pch->desc_len; i++) {
		tx_desc_p = (struct dma_tx_desc *)pch->desc_base + i;
		buffer = pch->alloc(pch->pkt_size, &byte_offset,
				    &pch->opt[i]);
		if (!buffer) {
			dev_err(pctrl->dev, "No enough memory for %s\n",
				pch->device_id);
			return -ENOMEM;
		}
		tx_desc_p->data_pointer = dma_map_single(pctrl->dev, buffer,
			pch->pkt_size, DMA_TO_DEVICE);
		if (dma_mapping_error(pctrl->dev, tx_desc_p->data_pointer)) {
			dev_err(pctrl->dev,
				"%s DMA map failed\n", __func__);
			pch->free(buffer, (void *)pch->opt[i]);
			buffer = NULL;
			break;
		}
		tx_desc_p->status.all = 0;
		tx_desc_p->status.field.sop = 1;
		tx_desc_p->status.field.eop = 1;
		tx_desc_p->status.field.c = 0;
		tx_desc_p->status.field.byte_offset = byte_offset;
		tx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		tx_desc_p->status.field.own = CPU_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	}
	return 0;
}

static int dma_chan_data_buf_4dws_alloc(struct dmax_chan *pch)
{
	int ret = 0;

	if (WARN_ON(!pch->desc_len))
		return -EINVAL;
	if (WARN_ON(dma_chan_rx(pch) && dma_chan_tx(pch)))
		return -EINVAL;
	if (dma_chan_rx(pch)) {
		ret = dma_rx_chan_data_buf_4dws_alloc(pch);
		if (ret != 0)
			goto done;
	}

	if (dma_chan_tx(pch)) {
		ret = dma_tx_chan_data_buf_4dws_alloc(pch);
		if (ret != 0)
			goto done;
	}
done:
	return ret;
}

static int dma_rx_chan_data_buf_2dws_alloc(struct dmax_chan *pch)
{
	int i;
	int byte_offset;
	char *buffer;
	struct dma_rx_desc_2dw *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	for (i = 0; i < pch->desc_len; i++) {
		rx_desc_p = (struct dma_rx_desc_2dw *)pch->desc_base + i;
		buffer = pch->alloc(pch->pkt_size, &byte_offset,
				    &pch->opt[i]);
		if (!buffer) {
			dev_err(pctrl->dev, "No enough memory for %s\n",
				pch->device_id);
			return -ENOMEM;
		}
		rx_desc_p->data_pointer = dma_map_single(pctrl->dev, buffer,
			pch->pkt_size, DMA_FROM_DEVICE);
		if (dma_mapping_error(pctrl->dev, rx_desc_p->data_pointer)) {
			dev_err(pctrl->dev,
				"%s DMA map failed\n", __func__);
			pch->free(buffer, (void *)pch->opt[i]);
			buffer = NULL;
			break;
		}
		rx_desc_p->status.all = 0;
		rx_desc_p->status.field.sop = 1;
		rx_desc_p->status.field.eop = 1;
		rx_desc_p->status.field.c = 0;
		rx_desc_p->status.field.byte_offset = byte_offset;
		rx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		rx_desc_p->status.field.own = DMA_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	}
	return 0;
}

static int dma_tx_chan_data_buf_2dws_alloc(struct dmax_chan *pch)
{
	int i;
	int byte_offset;
	char *buffer;
	struct dma_tx_desc_2dw *tx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (pch->p2pcpy) {
		dev_err(pctrl->dev,
			"P2P TX channel use buffer from RX channel\n");
		return -EINVAL;
	}
	for (i = 0; i < pch->desc_len; i++) {
		tx_desc_p = (struct dma_tx_desc_2dw *)pch->desc_base + i;
		buffer = pch->alloc(pch->pkt_size, &byte_offset,
				    &pch->opt[i]);
		if (!buffer) {
			dev_err(pctrl->dev,
				"No enough memory for %s\n", pch->device_id);
			return -ENOMEM;
		}
		tx_desc_p->data_pointer = dma_map_single(pctrl->dev, buffer,
			pch->pkt_size, DMA_TO_DEVICE);
		if (dma_mapping_error(pctrl->dev, tx_desc_p->data_pointer)) {
			dev_err(pctrl->dev,
				"%s DMA map failed\n", __func__);
			pch->free(buffer, (void *)pch->opt[i]);
			buffer = NULL;
			break;
		}
		tx_desc_p->status.all = 0;
		tx_desc_p->status.field.sop = 1;
		tx_desc_p->status.field.eop = 1;
		tx_desc_p->status.field.c = 0;
		tx_desc_p->status.field.byte_offset = byte_offset;
		tx_desc_p->status.field.data_len = pch->pkt_size;
		/* CPU own descripor for TX initialization */
		/* Ensure data field ready before ownership change */
		wmb();
		tx_desc_p->status.field.own = CPU_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	}
	return 0;
}

static int dma_chan_data_buf_2dws_alloc(struct dmax_chan *pch)
{
	int ret = 0;

	if (WARN_ON(!pch->desc_len))
		return -EINVAL;
	if (WARN_ON(dma_chan_rx(pch) && dma_chan_tx(pch)))
		return -EINVAL;

	if (dma_chan_rx(pch)) {
		ret = dma_rx_chan_data_buf_2dws_alloc(pch);
		if (ret != 0)
			goto done;
	}

	if (dma_chan_tx(pch)) {
		ret = dma_tx_chan_data_buf_2dws_alloc(pch);
		if (ret != 0)
			goto done;
	}
done:
	return ret;
}

static int dma_chan_data_buf_alloc(struct dmax_chan *pch)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_is_64bit(pctrl))
		return dma_chan_data_buf_4dws_alloc(pch);
	else
		return dma_chan_data_buf_2dws_alloc(pch);
}

int ltq_dma_chan_data_buf_alloc(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_data_buf_alloc(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_data_buf_alloc);

static int dma_rx_chan_data_buf_4dws_free(struct dmax_chan *pch)
{
	int i;
	struct dma_rx_desc *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	for (i = 0; i < pch->desc_len; i++) {
		rx_desc_p = (struct dma_rx_desc *)pch->desc_base + i;
		if (((rx_desc_p->status.field.own == CPU_OWN) &&
		     rx_desc_p->status.field.c) ||
		     ((rx_desc_p->status.field.own == DMA_OWN) &&
		     rx_desc_p->status.field.data_len > 0)) {
			dma_unmap_single(pctrl->dev, rx_desc_p->data_pointer,
					 pch->pkt_size, DMA_FROM_DEVICE);
			pch->free(__va(rx_desc_p->data_pointer),
				  pch->opt[i]);
		}
	}
	return 0;
}

static int dma_tx_chan_data_buf_4dws_free(struct dmax_chan *pch)
{
	int i;
	struct dma_tx_desc *tx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (pch->p2pcpy) {
		dev_err(pctrl->dev,
			"P2P TX channel use buffer from RX channel\n");
		return -EINVAL;
	}
	for (i = 0; i < pch->desc_len; i++) {
		tx_desc_p = (struct dma_tx_desc *)pch->desc_base + i;
		if (((tx_desc_p->status.field.own == CPU_OWN) &&
		     tx_desc_p->status.field.c) ||
		     ((tx_desc_p->status.field.own == DMA_OWN) &&
		     tx_desc_p->status.field.data_len > 0)) {
			dma_unmap_single(pctrl->dev, tx_desc_p->data_pointer,
					 pch->pkt_size, DMA_TO_DEVICE);
			pch->free(__va(tx_desc_p->data_pointer),
					(void *)pch->opt[i]);
		}
	}
	return 0;
}

static int dma_chan_data_buf_4dws_free(struct dmax_chan *pch)
{
	if (WARN_ON(!pch->desc_len))
		return -EINVAL;
	if (WARN_ON(dma_chan_rx(pch) && dma_chan_tx(pch)))
		return -EINVAL;

	if (dma_chan_rx(pch))
		dma_rx_chan_data_buf_4dws_free(pch);

	if (dma_chan_tx(pch))
		dma_tx_chan_data_buf_4dws_free(pch);
	return 0;
}

static int dma_rx_chan_data_buf_2dws_free(struct dmax_chan *pch)
{
	int i;
	struct dma_rx_desc_2dw *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	for (i = 0; i < pch->desc_len; i++) {
		rx_desc_p = (struct dma_rx_desc_2dw *)pch->desc_base + i;
		if (((rx_desc_p->status.field.own == CPU_OWN) &&
		     rx_desc_p->status.field.c) ||
		     ((rx_desc_p->status.field.own == DMA_OWN) &&
		     rx_desc_p->status.field.data_len > 0)) {
			dma_unmap_single(pctrl->dev, rx_desc_p->data_pointer,
					 pch->pkt_size, DMA_FROM_DEVICE);
			pch->free(__va(rx_desc_p->data_pointer),
					(void *)pch->opt[i]);
		}
	}
	return 0;
}

static int dma_tx_chan_data_buf_2dws_free(struct dmax_chan *pch)
{
	int i;
	struct dma_tx_desc_2dw *tx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (pch->p2pcpy) {
		dev_err(pctrl->dev,
			"P2P TX channel use buffer from RX channel\n");
		return -EINVAL;
	}

	for (i = 0; i < pch->desc_len; i++) {
		tx_desc_p = (struct dma_tx_desc_2dw *)pch->desc_base + i;
		if (((tx_desc_p->status.field.own == CPU_OWN) &&
		     tx_desc_p->status.field.c) ||
		     ((tx_desc_p->status.field.own == DMA_OWN) &&
		     tx_desc_p->status.field.data_len > 0)) {
			dma_unmap_single(pctrl->dev, tx_desc_p->data_pointer,
					 pch->pkt_size, DMA_TO_DEVICE);
			pch->free(__va(tx_desc_p->data_pointer),
					(void *)pch->opt[i]);
		}
	}
	return 0;
}

static int dma_chan_data_buf_2dws_free(struct dmax_chan *pch)
{
	if (WARN_ON(!pch->desc_len))
		return -EINVAL;
	if (WARN_ON(dma_chan_rx(pch) && dma_chan_tx(pch)))
		return -EINVAL;

	if (dma_chan_rx(pch))
		dma_rx_chan_data_buf_2dws_free(pch);

	if (dma_chan_tx(pch))
		dma_tx_chan_data_buf_2dws_free(pch);
	return 0;
}

static int dma_chan_data_buf_free(struct dmax_chan *pch)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (WARN_ON(dma_chan_is_hw_desc(pch)))
		return -EINVAL;
	if (dma_is_64bit(pctrl))
		return dma_chan_data_buf_4dws_free(pch);
	else
		return dma_chan_data_buf_2dws_free(pch);
}

int ltq_dma_chan_data_buf_free(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	dma_chan_data_buf_free(pch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_data_buf_free);

int ltq_dma_chan_buf_alloc_callback_cfg(u32 chan, buffer_alloc_t alloc)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!alloc || !pch))
		return -EINVAL;

	mutex_lock(&pch->ch_lock);
	pch->alloc = alloc;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_buf_alloc_callback_cfg);

int ltq_dma_chan_buf_free_callback_cfg(u32 chan, buffer_free_t free)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!free || !pch))
		return -EINVAL;

	mutex_lock(&pch->ch_lock);
	pch->free = free;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_buf_free_callback_cfg);

int ltq_dma_chan_irq_callback_cfg(u32 chan, irq_handler_t handler, void *data)
{
	int ret;
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!handler || !pch))
		return -EINVAL;

	pctrl = dma_chan_get_controller(pch);
	devm_free_irq(pctrl->dev, pch->irq, (void *)pch);
	ret = devm_request_irq(pctrl->dev, pch->irq, handler, 0,
			       pch->device_id, data);
	if (ret)
		return ret;
	mutex_lock(&pch->ch_lock);
	pch->flags |= DEVICE_CTRL_CHAN;
	pch->data = data;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_irq_callback_cfg);

int ltq_dma_chan_pseudo_irq_handler_callback_cfg(u32 chan,
						 intr_handler_t handler,
						 void *priv)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!handler || !pch))
		return -EINVAL;

	mutex_lock(&pch->ch_lock);
	pch->intr_handler = handler;
	pch->priv = priv;
	mutex_unlock(&pch->ch_lock);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_pseudo_irq_handler_callback_cfg);

static int dma_port_cfg(struct dma_port *port)
{
	u32 reg = 0;
	unsigned long flags;
	struct dma_ctrl *pctrl;
	int cid;

	if (!port)
		return -EINVAL;
	pctrl = dma_port_get_controller(port);
	cid = pctrl->cid;
	reg |= (port->flush_memcpy ? DMA_PCTRL_MEM_FLUSH : 0);
	reg |= (port->txwgt << DMA_PCTRL_TXWGT_S);
	reg |= (port->txendi << DMA_PCTRL_TXENDI_S);
	reg |= (port->rxendi << DMA_PCTRL_RXENDI_S);
	reg |= (port->pkt_drop << DMA_PCTRL_PDEN_S);

	if (port->txbl == DMA_BURSTL_32DW)
		reg |= DMA_PCTRL_TXBL32;
	else if (port->txbl == DMA_BURSTL_16DW)
		reg |= DMA_PCTRL_TXBL16;
	else
		reg |= (port->txbl << DMA_PCTRL_TXBL_S);

	if (port->rxbl == DMA_BURSTL_32DW)
		reg |= DMA_PCTRL_RXBL32;
	else if (port->rxbl == DMA_BURSTL_16DW)
		reg |= DMA_PCTRL_RXBL16;
	else
		reg |= (port->rxbl << DMA_PCTRL_RXBL_S);
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, port->pid, DMA_PS);
	ltq_dma_w32(pctrl, reg, DMA_PCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	spin_lock_init(&port->port_lock);
	dev_dbg(pctrl->dev, "%s Port %d %s Control 0x%08x configuration done\n",
		dma_name[cid], port->pid, port->name,
		ltq_dma_r32(pctrl, DMA_PCTRL));
	return 0;
}

static char *common_buffer_alloc(int len, int *byte_offset,  void **opt)
{
	char *buffer = kmalloc(len, GFP_ATOMIC);

	*byte_offset = 0;
	return buffer;
}

static int common_buffer_free(char *dataptr, void *opt)
{
	kfree(dataptr);
	return 0;
}

static int dma_chan_cfg(struct dmax_chan *pch)
{
	u32 reg = 0;
	int cid;
	int pid;
	struct dma_ctrl *pctrl;
	struct dma_port *pport;

	if (WARN_ON(!pch))
		return -EINVAL;
	pctrl = dma_chan_get_controller(pch);
	pport = dma_chan_get_port(pch);

	if (unlikely(!pport))
		return -EINVAL;

	cid = pctrl->cid;
	pid = pport->pid;
	reg |= (pch->lpbk_ch_nr << DMA_CCTRL_LBCHNR_S);
	reg |= (pch->lpbk_en ? DMA_CCTRL_LBEN : 0);
	reg |= (pch->p2pcpy ? DMA_CCTRL_P2PCPY : 0);
	reg |= (pch->pden ? DMA_CCTRL_PDEN : 0);

	reg |= (pch->txwgt << DMA_CCTRL_TXWGT_S);

	reg |= (pch->onoff ? DMA_CCTRL_ON : 0);
	reg |= (pch->rst ? DMA_CCTRL_RST : 0);
	dma_chan_cctrl_cfg(pch, reg);
	dma_chan_set_class(pch, pch->nr);
	/* Clear all interrupts and disabled it */
	ltq_dma_w32(pctrl, 0, DMA_CIE);
	ltq_dma_w32(pctrl, DMA_CI_ALL, DMA_CIS);

	/* Disable related interrupts */
	if (pch->nr < 32) {
		ltq_dma_w32_mask(pctrl, (1 << (pch->nr & 0x1f)), 0, DMA_IRNEN);
		ltq_dma_w32_mask(pctrl, 0, (1 << (pch->nr & 0x1f)), DMA_IRNCR);
	} else {
		ltq_dma_w32_mask(pctrl, (1 << (pch->nr & 0x1f)), 0, DMA_IRNEN1);
		ltq_dma_w32_mask(pctrl, 0, (1 << (pch->nr & 0x1f)), DMA_IRNCR1);
	}
	pch->alloc = &common_buffer_alloc;
	pch->free = &common_buffer_free;
	mutex_init(&pch->ch_lock);
	spin_lock_init(&pch->irq_lock);
	/* Some Kernel API can't be used anymore due to new EVA */
	if (!dma_chan_desc_alloc_by_device(pch)) {
		dma_chan_desc_alloc(pch, DMA_DEFAULT_DESC_LEN);
		dma_chan_data_buf_alloc(pch);
	}
	/* Descriptor related stuff in separate APIs */
	dev_dbg(pctrl->dev, "%s port %d chan %d dir %s class %d done\n",
		dma_name[cid], pid, pch->nr, dma_chan_tx(pch) ? "TX" : "RX",
		dma_chan_get_class(pch));
	return 0;
}

int ltq_request_dma(u32 chan, const char *device_id)
{
	int cid = _DMA_CONTROLLER(chan);
	int pid = _DMA_PORT(chan);
	int nid = _DMA_CHANNEL(chan);
	struct dmax_chan *pch;

	pr_debug("%s %s port %d chan %d\n", __func__,
		 dma_get_name_by_cid(cid), pid, nid);
	pch = dma_cid_pid_nid_get_chan(cid, pid, nid);
	if (!pch)
		return -ENODEV;
	if (!device_id)
		pr_warn("%s no device id given\n", __func__);
	mutex_lock(&pch->ch_lock);
	if (dma_chan_in_use(pch)) {
		mutex_unlock(&pch->ch_lock);
		pr_err("%s chan %d in use\n", dma_get_name_by_cid(cid), nid);
		return -EBUSY;
	}

	pch->flags |= CHAN_IN_USE;
	pch->lnr = chan;

	mutex_unlock(&pch->ch_lock);
	pr_debug("%s chan %d is allocated for %s\n",
		 dma_get_name_by_cid(cid), nid, device_id);
	pch->device_id = device_id;
	return 0;
}
EXPORT_SYMBOL(ltq_request_dma);

int ltq_free_dma(u32 chan)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	mutex_lock(&pch->ch_lock);
	pch->flags &= ~CHAN_IN_USE;
	pch->lnr = 0;
	mutex_unlock(&pch->ch_lock);
	pch->device_id = NULL;
	return 0;
}
EXPORT_SYMBOL(ltq_free_dma);

/*
 * P2P
 * 1. RX and TX channel has the same descriptor list so that they can
 * link together
 * 2. TX channel has to enable P2P
 * 3. Global buffer length has to be configured on TX channel DMA instance
 * 4. P2P can be extended from intraDMA in leagcy SoC to interDMA.
 */
static void dma_p2p_cfg(struct dmax_chan *rch, struct dmax_chan *tch)
{
	/*
	 * Assume RX and TX has been configured using
	 * DMA descriptor and data buffer functions.
	 * TX has to be released its old buffer for default cases.
	 */
	if (WARN_ON(rch->desc_len == 0))
		return;
	mutex_lock(&tch->ch_lock);
	tch->desc_len = rch->desc_len;
	tch->desc_base = rch->desc_base;
	tch->desc_phys = rch->desc_phys;
	tch->pkt_size = rch->pkt_size;
	tch->p2pcpy = 1;
	tch->global_buffer_len = tch->pkt_size;
	mutex_unlock(&tch->ch_lock);
	dma_chan_desc_hw_cfg(tch, tch->desc_phys, tch->desc_len);
	dma_chan_p2p_cfg(tch, 1);
	dma_chan_global_buf_len_cfg(tch, tch->global_buffer_len);
}

int ltq_dma_p2p_cfg(u32 rx_chan, u32 tx_chan)
{
	struct dmax_chan *rxch = dma_chan_l2p(rx_chan);
	struct dmax_chan *txch = dma_chan_l2p(tx_chan);

	if (WARN_ON((!rxch) || (!txch)))
		return -EINVAL;
	dma_p2p_cfg(rxch, txch);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_p2p_cfg);

static int dma_chan_4dws_read(struct dmax_chan *pch, char **dataptr, void **opt)
{
	unsigned long sys_flag;
	unsigned char *buf;
	void *p = NULL;
	int len, byte_offset = 0;
	struct dma_rx_desc *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pch->irq_lock, sys_flag);
	/* Get the rx data first */
	rx_desc_p = (struct dma_rx_desc *)pch->desc_base + pch->curr_desc;
	if (!((rx_desc_p->status.field.own == CPU_OWN) &&
	      rx_desc_p->status.field.c)) {
		spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
		return -EBUSY;
	}

	dma_unmap_single(pctrl->dev, rx_desc_p->data_pointer,
			 pch->pkt_size, DMA_FROM_DEVICE);

	buf = (u8 *)__va(rx_desc_p->data_pointer);
	*(u32 *)dataptr = (u32)buf;
	len = rx_desc_p->status.field.data_len;
	if (opt)
		*(int *)opt = (int)pch->opt[pch->curr_desc];
	/* Replace with a new allocated buffer */
	buf = pch->alloc(pch->pkt_size, &byte_offset, &p);
	if (buf) {
		pch->opt[pch->curr_desc] = p;
		rx_desc_p->data_pointer = dma_map_single(pctrl->dev, buf,
			pch->pkt_size, DMA_FROM_DEVICE);
		if (dma_mapping_error(pctrl->dev, rx_desc_p->data_pointer)) {
			dev_err(pctrl->dev, "%s DMA map failed\n", __func__);
			pch->free(buf, p);
			spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
			buf = NULL;
			return -ENOMEM;
		}
		rx_desc_p->status.field.byte_offset = byte_offset;
		rx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		rx_desc_p->status.field.own = DMA_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	} else {
	/*
	 * It will handle client driver using the dma_device_desc_setup
	 * function. So, handle with care.
	 */
		*(u32 *)dataptr = 0;
		if (opt)
			*(int *)opt = 0;
		len = 0;
		rx_desc_p->status.field.byte_offset = byte_offset;
		rx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		rx_desc_p->status.field.own = DMA_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	}

	/*
	 * Increase descriptor index and process wrap around
	 * Handle ith care when use the dma_device_desc_setup function
	 */
	pch->curr_desc++;
	if (pch->curr_desc == pch->desc_len)
		pch->curr_desc = 0;
	spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
	return len;
}

static int dma_chan_2dws_read(struct dmax_chan *pch, char **dataptr, void **opt)
{
	unsigned long sys_flag;
	unsigned char *buf;
	void *p = NULL;
	int len, byte_offset = 0;
	struct dma_rx_desc_2dw *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pch->irq_lock, sys_flag);
	/* Get the rx data first */
	rx_desc_p = (struct dma_rx_desc_2dw *)pch->desc_base + pch->curr_desc;
	if (!((rx_desc_p->status.field.own == CPU_OWN) &&
	      rx_desc_p->status.field.c)) {
		spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
		return -EBUSY;
	}

	dma_unmap_single(pctrl->dev, rx_desc_p->data_pointer,
			 pch->pkt_size, DMA_FROM_DEVICE);
	buf = (u8 *)__va(rx_desc_p->data_pointer);
	*(u32 *)dataptr = (u32)buf;
	len = rx_desc_p->status.field.data_len;
	if (opt)
		*(int *)opt = (int)pch->opt[pch->curr_desc];
	/* Replace with a new allocated buffer */
	buf = pch->alloc(pch->pkt_size, &byte_offset, &p);
	if (buf) {
		pch->opt[pch->curr_desc] = p;
		rx_desc_p->data_pointer = dma_map_single(pctrl->dev, buf,
			pch->pkt_size, DMA_FROM_DEVICE);
		if (dma_mapping_error(pctrl->dev, rx_desc_p->data_pointer)) {
			dev_err(pctrl->dev, "%s DMA map failed\n", __func__);
			pch->free(buf, p);
			spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
			buf = NULL;
			return -ENOMEM;
		}
		rx_desc_p->status.field.byte_offset = byte_offset;
		rx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		rx_desc_p->status.field.own = DMA_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	} else {
	/*
	 * It will handle client driver using the dma_device_desc_setup
	 * function. So, handle with care.
	 */
		*(u32 *)dataptr = 0;
		if (opt)
			*(int *)opt = 0;
		len = 0;
		rx_desc_p->status.field.byte_offset = byte_offset;
		rx_desc_p->status.field.data_len = pch->pkt_size;
		/* Ensure data field ready before ownership change */
		wmb();
		rx_desc_p->status.field.own = DMA_OWN;
		/* Ensure ownership changed before moving to next step */
		wmb();
	}

	/*
	 * Increase descriptor index and process wrap around
	 * Handle ith care when use the dma_device_desc_setup function
	 */
	pch->curr_desc++;
	if (pch->curr_desc == pch->desc_len)
		pch->curr_desc = 0;
	spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
	return len;
}

static int dma_chan_read(struct dmax_chan *pch, char **dataptr, void **opt)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_is_64bit(pctrl))
		return dma_chan_4dws_read(pch, dataptr, opt);
	else
		return dma_chan_2dws_read(pch, dataptr, opt);
}

int ltq_dma_chan_read(u32 chan, char **dataptr, void **opt)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	if (WARN_ON(!dma_chan_rx(pch)))
		return -EIO;
	return dma_chan_read(pch, dataptr, opt);
}
EXPORT_SYMBOL(ltq_dma_chan_read);

static int dma_chan_4dws_write(struct dmax_chan *pch, char *dataptr, int len,
			       int sop, int eop, void *opt)
{
	unsigned long sys_flag, flags;
	u32 cctrls;
	int byte_offset;
	struct dma_tx_desc *tx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pch->irq_lock, sys_flag);

	/*
	 * Set the previous descriptor pointer to verify data is sendout or not
	 * If its sendout then clear the buffer based on the client driver
	 * buffer free callaback function
	 */
	tx_desc_p = (struct dma_tx_desc *)pch->desc_base + pch->prev_desc;
	while ((tx_desc_p->status.field.own == CPU_OWN) &&
	       tx_desc_p->status.field.c) {
		dma_unmap_single(pctrl->dev, tx_desc_p->data_pointer,
				 pch->pkt_size, DMA_TO_DEVICE);
		pch->free((char *)__va(tx_desc_p->data_pointer),
			  pch->opt[pch->prev_desc]);
		memset(tx_desc_p, 0, sizeof(*tx_desc_p));
		pch->prev_desc = (pch->prev_desc + 1) % (pch->desc_len);
		tx_desc_p = (struct dma_tx_desc *)pch->desc_base
			+ pch->prev_desc;
	}
	/* Set the current descriptor pointer */
	tx_desc_p = (struct dma_tx_desc *)pch->desc_base + pch->curr_desc;

	/*
	 * Check whether this descriptor is available CPU and DMA excute
	 * tasks in its own envionment. DMA will change ownership and
	 * complete bit. Check the descriptors are available or not to
	 * process the packet.
	 */
	if (tx_desc_p->status.field.own == DMA_OWN) {
		/* This descriptor has not been released */
		pch->intr_handler(pch->lnr, pch->priv, TX_BUF_FULL_INT);
		spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
		return 0;
	}
	pch->opt[pch->curr_desc] = opt;
	byte_offset = ((u32)dataptr) & pctrl->burst_mask;

	tx_desc_p->data_pointer = dma_map_single(pctrl->dev, dataptr,
		len, DMA_TO_DEVICE) - byte_offset;
	if (dma_mapping_error(pctrl->dev, tx_desc_p->data_pointer)) {
		spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
		dev_err(pctrl->dev, "%s DMA map failed\n", __func__);
		return -ENOMEM;
	}
	tx_desc_p->status.field.byte_offset = byte_offset;
	tx_desc_p->status.field.data_len = len;
	tx_desc_p->status.field.sop = sop;
	tx_desc_p->status.field.eop = eop;
	/* Ensure data field ready before ownership change */
	wmb();
	tx_desc_p->status.field.own = DMA_OWN;
	/* Ensure ownership changed before moving to next step */
	wmb();
	pch->curr_desc++;
	if (pch->curr_desc == pch->desc_len)
		pch->curr_desc = 0;
	/* Check if the next descriptor is available */
	tx_desc_p = (struct dma_tx_desc *)pch->desc_base + pch->curr_desc;
	if (tx_desc_p->status.field.own == DMA_OWN || pch->desc_len == 1)
		pch->intr_handler(pch->lnr, pch->priv, TX_BUF_FULL_INT);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	cctrls = ltq_dma_r32(pctrl, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);

	/* If not open this channel, open it */
	if (!(cctrls & DMA_CCTRL_ON))
		dma_chan_open(pch);
	spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
	return len;
}

static int dma_chan_2dws_write(struct dmax_chan *pch, char *dataptr, int len,
			       int sop, int eop, void *opt)
{
	unsigned long sys_flag, flags;
	u32 cctrls;
	int byte_offset;
	struct dma_tx_desc_2dw *tx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	spin_lock_irqsave(&pch->irq_lock, sys_flag);

	/*
	 * Set the previous descriptor pointer to verify data is sendout or not
	 * If its sendout then clear the buffer based on the client driver
	 * buffer free callaback function
	 */
	tx_desc_p = (struct dma_tx_desc_2dw *)pch->desc_base + pch->prev_desc;
	while ((tx_desc_p->status.field.own == CPU_OWN) &&
	       tx_desc_p->status.field.c) {
		dma_unmap_single(pctrl->dev, tx_desc_p->data_pointer,
				 pch->pkt_size, DMA_TO_DEVICE);
		pch->free((char *)__va(tx_desc_p->data_pointer),
			  pch->opt[pch->prev_desc]);
		memset(tx_desc_p, 0, sizeof(*tx_desc_p));
		pch->prev_desc = (pch->prev_desc + 1) % (pch->desc_len);
		tx_desc_p = (struct dma_tx_desc_2dw *)pch->desc_base
			+ pch->prev_desc;
	}
	/* Set the current descriptor pointer */
	tx_desc_p = (struct dma_tx_desc_2dw *)pch->desc_base + pch->curr_desc;

	/*
	 * Check whether this descriptor is available CPU and DMA excute
	 * tasks in its own envionment. DMA will change ownership and
	 * complete bit. Check the descriptors are available or not to
	 * process the packet.
	 */
	if (tx_desc_p->status.field.own == DMA_OWN) {
		/* This descriptor has not been released */
		pch->intr_handler(pch->lnr, pch->priv, TX_BUF_FULL_INT);
		spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
		return 0;
	}
	pch->opt[pch->curr_desc] = opt;
	byte_offset = ((u32)dataptr) & pctrl->burst_mask;
	tx_desc_p->data_pointer = dma_map_single(pctrl->dev, dataptr,
		len, DMA_TO_DEVICE) - byte_offset;
	if (dma_mapping_error(pctrl->dev, tx_desc_p->data_pointer)) {
		spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
		dev_err(pctrl->dev, "%s DMA map failed\n", __func__);
		return -ENOMEM;
	}
	tx_desc_p->status.field.byte_offset = byte_offset;
	tx_desc_p->status.field.data_len = len;
	tx_desc_p->status.field.sop = sop;
	tx_desc_p->status.field.eop = eop;
	/* Ensure data field ready before ownership change */
	wmb();
	tx_desc_p->status.field.own = DMA_OWN;
	/* Ensure ownership changed before moving to next step */
	wmb();
	pch->curr_desc++;
	if (pch->curr_desc == pch->desc_len)
		pch->curr_desc = 0;
	/* Check if the next descriptor is available */
	tx_desc_p = (struct dma_tx_desc_2dw *)pch->desc_base + pch->curr_desc;
	if ((tx_desc_p->status.field.own == DMA_OWN || pch->desc_len == 1))
		pch->intr_handler(pch->lnr, pch->priv, TX_BUF_FULL_INT);

	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	cctrls = ltq_dma_r32(pctrl, DMA_CCTRL);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);

	/* If not open this channel, open it */
	if (!(cctrls & DMA_CCTRL_ON))
		dma_chan_open(pch);
	spin_unlock_irqrestore(&pch->irq_lock, sys_flag);
	return len;
}

static int dma_chan_write(struct dmax_chan *pch, char *dataptr, int len,
			  int sop, int eop, void *opt)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_is_64bit(pctrl))
		return dma_chan_4dws_write(pch, dataptr, len, sop, eop, opt);
	else
		return dma_chan_2dws_write(pch, dataptr, len, sop, eop, opt);
}

int ltq_dma_chan_write(u32 chan, char *dataptr, int len, int sop, int eop,
		       void *opt)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	if (WARN_ON(!dma_chan_tx(pch)))
		return -EIO;
	return dma_chan_write(pch, dataptr, len, sop, eop, opt);
}
EXPORT_SYMBOL(ltq_dma_chan_write);

static int dma_chan_4dws_desc_setup(struct dmax_chan *pch, char *buf,
				    size_t len)
{
	unsigned long flags;
	int byte_offset = 0;
	struct dma_rx_desc *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	rx_desc_p = (struct dma_rx_desc *)pch->desc_base + pch->curr_desc;
	if (rx_desc_p->status.field.own == DMA_OWN) {
		dev_err(pctrl->dev, "Already setup the descriptor\n");
		return -EBUSY;
	}
	pch->opt[pch->curr_desc] = NULL;
	byte_offset = ((u32)buf) & pctrl->burst_mask;
	rx_desc_p->data_pointer = dma_map_single(pctrl->dev, buf,
		len, DMA_FROM_DEVICE);
	if (dma_mapping_error(pctrl->dev, rx_desc_p->data_pointer)) {
		dev_err(pctrl->dev, "%s DMA map failed\n", __func__);
		return -ENOMEM;
	}
	spin_lock_irqsave(&pch->irq_lock, flags);
	rx_desc_p->status.field.byte_offset = byte_offset;
	rx_desc_p->status.field.data_len = len;
	/* Ensure data field ready before ownership change */
	wmb();
	rx_desc_p->status.field.own = DMA_OWN;
	/* Ensure ownership changed before moving to next step */
	wmb();

	pch->curr_desc++;
	if (pch->curr_desc == pch->desc_len)
		pch->curr_desc = 0;
	spin_unlock_irqrestore(&pch->irq_lock, flags);
	return 0;
}

static int dma_chan_2dws_desc_setup(struct dmax_chan *pch, char *buf,
				    size_t len)
{
	unsigned long flags;
	int byte_offset = 0;
	struct dma_rx_desc_2dw *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	rx_desc_p = (struct dma_rx_desc_2dw *)pch->desc_base + pch->curr_desc;
	if (rx_desc_p->status.field.own == DMA_OWN) {
		dev_err(pctrl->dev,
			"%s (Already setup the descriptor)\n", __func__);
		return -EBUSY;
	}
	pch->opt[pch->curr_desc] = NULL;
	byte_offset = ((u32)buf) & pctrl->burst_mask;
	rx_desc_p->data_pointer = dma_map_single(pctrl->dev, buf,
		len, DMA_FROM_DEVICE);
	if (dma_mapping_error(pctrl->dev, rx_desc_p->data_pointer)) {
		dev_err(pctrl->dev, "%s DMA map failed\n", __func__);
		return -ENOMEM;
	}
	spin_lock_irqsave(&pch->irq_lock, flags);
	rx_desc_p->status.field.byte_offset = byte_offset;
	rx_desc_p->status.field.data_len = len;
	/* Ensure data field ready before ownership change */
	wmb();
	rx_desc_p->status.field.own = DMA_OWN;
	/* Ensure ownership changed before moving to next step */
	wmb();

	pch->curr_desc++;
	if (pch->curr_desc == pch->desc_len)
		pch->curr_desc = 0;
	spin_unlock_irqrestore(&pch->irq_lock, flags);
	return 0;
}

static int dma_chan_sync_desc_setup(struct dmax_chan *pch, char *buf,
				    size_t len)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_is_64bit(pctrl))
		return dma_chan_4dws_desc_setup(pch, buf, len);
	else
		return dma_chan_2dws_desc_setup(pch, buf, len);
}

int ltq_dma_chan_sync_desc_setup(u32 chan, char *buf, size_t len)
{
	struct dmax_chan *pch = dma_chan_l2p(chan);

	if (WARN_ON(!pch))
		return -EINVAL;
	if (WARN_ON(!dma_chan_rx(pch)))
		return -EIO;
	dma_chan_sync_desc_setup(pch, buf, len);
	return 0;
}
EXPORT_SYMBOL(ltq_dma_chan_sync_desc_setup);

static void dma_rx_chan_4dws_int_handler(struct dmax_chan *pch)
{
	unsigned long flags;
	struct dma_rx_desc *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	/* Handle command complete interrupt, todo 2dws descriptor */
	rx_desc_p = (struct dma_rx_desc *)pch->desc_base + pch->curr_desc;
	if ((rx_desc_p->status.field.own == CPU_OWN) &&
	    rx_desc_p->status.field.c) {
		/* Everything is correct, then we inform the upper layer */
		if (pch->intr_handler)
			pch->intr_handler(pch->lnr, pch->priv, RCV_INT);
		/*
		 * Clear interrupt status bits once we sendout the pseudo
		 * interrupt to client driver
		 */
		spin_lock_irqsave(&pctrl->ctrl_lock, flags);
		clear_bit(pch->nr, pctrl->dma_int_status);
		ltq_dma_w32(pctrl, pch->nr, DMA_CS);
		ltq_dma_w32_mask(pctrl, 0, DMA_CI_DEFAULT, DMA_CIE);
		spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	}
}

static void dma_rx_chan_2dws_int_handler(struct dmax_chan *pch)
{
	unsigned long flags;
	struct dma_rx_desc_2dw *rx_desc_p;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	/* Handle command complete interrupt, todo 2dws descriptor */
	rx_desc_p = (struct dma_rx_desc_2dw *)pch->desc_base + pch->curr_desc;
	if ((rx_desc_p->status.field.own == CPU_OWN) &&
	    rx_desc_p->status.field.c) {
		/* Everything is correct, then we inform the upper layer */
		if (pch->intr_handler)
			pch->intr_handler(pch->lnr, pch->priv, RCV_INT);
		/*
		 * Clear interrupt status bits once we sendout the pseudo
		 * interrupt to client driver
		 */
		spin_lock_irqsave(&pctrl->ctrl_lock, flags);
		clear_bit(pch->nr, pctrl->dma_int_status);
		ltq_dma_w32(pctrl, pch->nr, DMA_CS);
		ltq_dma_w32_mask(pctrl, 0, DMA_CI_DEFAULT, DMA_CIE);
		spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
	}
}

static void dma_rx_chan_int_handler(struct dmax_chan *pch)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_is_64bit(pctrl))
		dma_rx_chan_4dws_int_handler(pch);
	else
		dma_rx_chan_2dws_int_handler(pch);
}

static void dma_tx_chan_4dws_int_handler(struct dmax_chan *pch)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	struct dma_tx_desc *tx_desc_p =
		(struct dma_tx_desc *)pch->desc_base + pch->curr_desc;

	/*
	 * DMA Descriptor update by Hardware is not sync with DMA interrupt
	 * (EOP/Complete). To ensure descriptor is available before sending
	 * pseudo interrupt to the client drivers.
	 */
	if (tx_desc_p->status.field.own == CPU_OWN) {
		if ((pch->cis & DMA_CI_EOP) && pch->intr_handler) {
			pch->intr_handler(pch->lnr, pch->priv,
				TRANSMIT_CPT_INT);
			spin_lock_irqsave(&pctrl->ctrl_lock, flags);
			clear_bit(pch->nr, pctrl->dma_int_status);
			pch->cis = 0;
			/* Enable this specific interrupt after handling */
			ltq_dma_w32(pctrl, pch->nr, DMA_CS);
			ltq_dma_w32_mask(pctrl, 0, DMA_CI_EOP, DMA_CIE);
			spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
		}
	}
}

static void dma_tx_chan_2dws_int_handler(struct dmax_chan *pch)
{
	unsigned long flags;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);
	struct dma_tx_desc_2dw *tx_desc_p =
		(struct dma_tx_desc_2dw *)pch->desc_base + pch->curr_desc;

	/*
	 * DMA Descriptor update by Hardware is not sync with DMA interrupt
	 * (EOP/Complete). To ensure descriptor is available before sending
	 * pseudo interrupt to the client drivers.
	 */
	if (tx_desc_p->status.field.own == CPU_OWN) {
		if ((pch->cis & DMA_CI_EOP) && pch->intr_handler) {
			pch->intr_handler(pch->lnr, pch->priv,
				TRANSMIT_CPT_INT);
			spin_lock_irqsave(&pctrl->ctrl_lock, flags);
			clear_bit(pch->nr, pctrl->dma_int_status);
			pch->cis = 0;
			ltq_dma_w32(pctrl, pch->nr, DMA_CS);
			ltq_dma_w32_mask(pctrl, 0, DMA_CI_EOP, DMA_CIE);
			spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
		}
	}
}

static void dma_tx_chan_int_handler(struct dmax_chan *pch)
{
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (dma_is_64bit(pctrl))
		dma_tx_chan_4dws_int_handler(pch);
	else
		dma_tx_chan_2dws_int_handler(pch);
}

/* Trigger when taklet schedule calls */
static void do_dma_tasklet(unsigned long dev_id)
{
	int ch_nr;
	struct dma_port *pport;
	struct dmax_chan *pch;
	struct dma_ctrl *pctrl = (struct dma_ctrl *)dev_id;
	int budget = pctrl->budget;

	while (!bitmap_empty(pctrl->dma_int_status, pctrl->chans)) {
		ch_nr = find_first_bit(pctrl->dma_int_status, pctrl->chans);
		if (budget-- < 0) {
			tasklet_schedule(&pctrl->dma_tasklet);
			return;
		}
		if (!dma_is_64bit(pctrl)) { /* DMA0 */
			if (ch_nr <= 5) { /* 0 ~ 5 */
				int pid = ch_nr / 2;

				pport = &pctrl->ports[pid];
				pch = &pport->chans[ch_nr & 0x1];
				goto handler;
			} else { /* 12 ~ 15 */
				int pid = DMA0_MEMCOPY;

				pport = &pctrl->ports[pid];
				pch = &pport->chans[ch_nr & 0x3];
				goto handler;
			}
		} else {
			pport = &pctrl->ports[0];
			pch = &pport->chans[ch_nr];
			/* Todo, Channel WFQ */
			/* will go to handler */
		}
handler:
		if (dma_chan_rx(pch))
			dma_rx_chan_int_handler(pch);
		if (dma_chan_tx(pch))
			dma_tx_chan_int_handler(pch);
	}
	/*
	 * Sanity check, check if there is new packet coming during this small
	 * gap However, at the same time, the following may cause interrupts is
	 * coming again on the same channel, because of rescheduling.
	 */
	atomic_set(&pctrl->dma_in_process, 0);
	if (!bitmap_empty(pctrl->dma_int_status, pctrl->chans)) {
		atomic_set(&pctrl->dma_in_process, 1);
		tasklet_schedule(&pctrl->dma_tasklet);
	}
}

static inline int dma_irq_to_chan_nr(struct dma_ctrl *pctrl, int irq)
{
	return irq - pctrl->irq_base;
}

static irqreturn_t dma_chan_interrupt(int irq, void *dev_id)
{
	unsigned long flags;
	struct dmax_chan *pch = (struct dmax_chan *)dev_id;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	/* Disable this channel interrput */
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	ltq_dma_w32_mask(pctrl, DMA_CI_ALL, 0, DMA_CIE);
	/* Record and clear it to prevent dummy interrupts for level irq */
	pch->cis = ltq_dma_r32(pctrl, DMA_CIS);
	ltq_dma_w32(pctrl, DMA_CI_ALL, DMA_CIS);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);

	/* Record this channel interrupt for tasklet */
	set_bit(dma_irq_to_chan_nr(pctrl, irq), pctrl->dma_int_status);

	/* if not in process, then invoke the tasklet */
	if (!atomic_read(&pctrl->dma_in_process)) {
		atomic_set(&pctrl->dma_in_process, 1);
		tasklet_schedule(&pctrl->dma_tasklet);
	}
	return IRQ_HANDLED;
}

static int dma_ctrl_init(struct dma_ctrl *pctrl)
{
	u32 i, j;
	int enable, ret;
	struct dma_port *pport = NULL;
	struct dmax_chan *pch = NULL;

	dev_dbg(pctrl->dev, "struct dma_ctrl size %d port %d chan %d\n",
		sizeof(*pctrl), sizeof(*pport), sizeof(*pch));
	spin_lock_init(&pctrl->ctrl_lock);
	bitmap_zero(pctrl->dma_int_status, MAX_DMA_CHAN_PER_PORT);
	atomic_set(&pctrl->dma_in_process, 0);
	dma_ctrl_reset(pctrl);
	dma_ctrl_cfg(pctrl);

	for (i = 0; i < pctrl->port_nrs; i++) {
		pport = &pctrl->ports[i];
		dma_set_port_controller_data(pport, pctrl);
		dma_port_cfg(pport);

		for (j = 0; j < pport->chan_nrs; j++) {
			pch = &pport->chans[j];
			dma_set_chan_controller_data(pch, pctrl);
			dma_set_chan_port_data(pch, pport);
			dma_chan_cfg(pch);
			pch->irq = pctrl->irq_base + pch->nr;
			/* Interrupt handler registered */
			ret = devm_request_irq(pctrl->dev, pch->irq,
					       dma_chan_interrupt,
				0, dma_get_name_by_cid(pctrl->cid),
				(void *)pch);
			if (ret) {
				dev_err(pctrl->dev, "Failed to register irq %d on chan %d %s\n",
					pch->irq, pch->nr, pctrl->name);
				return -ENODEV;
			}
		}
	}

	if ((pctrl->flags & DMA_FTOD))
		enable = 1;
	else
		enable = 0;
	dma_ctrl_desc_fetch_on_demand_cfg(pctrl, enable);

	return 0;
}

static void dma_disable_irq(struct irq_data *d)
{
	unsigned long flags;
	unsigned int reg_off;
	struct dma_ctrl *pctrl = itoc(d);
	unsigned int cn;

	if (unlikely(!pctrl))
		return;

	cn = d->irq - pctrl->irq_base;
	reg_off = (cn > 32) ? DMA_IRNEN1 : DMA_IRNEN;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32_mask(pctrl, BIT((cn & 0x1f)), 0, reg_off);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_enable_irq(struct irq_data *d)
{
	unsigned long flags;
	unsigned int reg_off;
	struct dma_ctrl *pctrl = itoc(d);
	unsigned int cn;

	if (unlikely(!pctrl))
		return;

	cn = d->irq - pctrl->irq_base;
	reg_off = (cn > 32) ? DMA_IRNEN1 : DMA_IRNEN;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32_mask(pctrl, 0, BIT((cn & 0x1f)), reg_off);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_ack_irq(struct irq_data *d)
{
	unsigned int reg_off;
	struct dma_ctrl *pctrl = itoc(d);
	unsigned int cn;

	if (unlikely(!pctrl))
		return;

	cn = d->irq - pctrl->irq_base;
	reg_off = (cn > 32) ? DMA_IRNCR1 : DMA_IRNCR;
	ltq_dma_w32(pctrl, BIT((cn & 0x1f)), reg_off);
}

static void dma_mask_and_ack_irq(struct irq_data *d)
{
	unsigned long flags;
	unsigned int reg_off;
	struct dma_ctrl *pctrl = itoc(d);
	unsigned int cn;

	if (unlikely(!pctrl))
		return;

	cn = d->irq - pctrl->irq_base;
	reg_off = (cn > 32) ? DMA_IRNEN1 : DMA_IRNEN;
	spin_lock_irqsave(&pctrl->ctrl_lock, flags);
	ltq_dma_w32_mask(pctrl, BIT((cn & 0x1f)), 0, reg_off);
	reg_off = (cn > 32) ? DMA_IRNCR1 : DMA_IRNCR;
	ltq_dma_w32(pctrl, BIT((cn & 0x1f)), reg_off);
	spin_unlock_irqrestore(&pctrl->ctrl_lock, flags);
}

static void dma_irq_handler(struct irq_desc *desc)
{
	int offset;
	unsigned long irncr;
	unsigned long irncr1;
	struct dma_ctrl *pctrl = irq_desc_get_handler_data(desc);

	if (unlikely(!pctrl))
		return;
	irncr = ltq_dma_r32(pctrl, DMA_IRNCR);
	if (pctrl->chans > 32)
		irncr1 = ltq_dma_r32(pctrl, DMA_IRNCR1);

	if (pctrl->chans <= 32) {
		for_each_set_bit(offset, &irncr, pctrl->chans)
			generic_handle_irq(pctrl->irq_base + offset);
	} else {
		for_each_set_bit(offset, &irncr, 32)
			generic_handle_irq(pctrl->irq_base + offset);
		for_each_set_bit(offset, &irncr1, pctrl->chans - 32)
			generic_handle_irq(pctrl->irq_base + 32 + offset);
	}
}

static struct irq_chip dma_irq_chip = {
	.name = "dma_irq",
	.irq_mask = dma_disable_irq,
	.irq_unmask = dma_enable_irq,
	.irq_ack = dma_ack_irq,
	.irq_mask_ack = dma_mask_and_ack_irq,
};

static int dma_irq_map(struct irq_domain *d, unsigned int irq,
		       irq_hw_number_t hw)
{
	struct dma_ctrl *pctrl = d->host_data;

	irq_set_chip_and_handler_name(irq, &dma_irq_chip,
				      handle_level_irq, "mux");
	irq_set_chip_data(irq, pctrl);
	dev_dbg(pctrl->dev, "%s irq %d --> hw %ld\n", __func__, irq, hw);
	return 0;
}

static const struct irq_domain_ops dma_irq_domain_ops = {
	.xlate = irq_domain_xlate_onetwocell,
	.map = dma_irq_map,
};

static struct irqaction dma_cascade = {
	.handler = no_action,
	.flags = 0,
	.name = "dma_cascade",
};

static int dma_irq_chip_init(struct dma_ctrl *pctrl)
{
	struct resource irqres;
	struct device_node *node = pctrl->dev->of_node;

	if (of_irq_to_resource_table(node, &irqres, 1) == 1) {
		pctrl->irq_base = irq_alloc_descs(-1, 0, pctrl->chans, 0);
		if (pctrl->irq_base < 0) {
			dev_err(pctrl->dev, "Can't allocate IRQ numbers\n");
			return -ENODEV;
		}
		pctrl->chained_irq = irqres.start;
		irq_domain_add_legacy(node, pctrl->chans, pctrl->irq_base, 0,
				      &dma_irq_domain_ops, pctrl);
		setup_irq(irqres.start, &dma_cascade);
		irq_set_handler_data(irqres.start, pctrl);
		irq_set_chained_handler(irqres.start, dma_irq_handler);
		/*
		 * NB DMA3 TOE and Memcpy doesn't use controller irq but it re
		 * directs to their individual module. channel level interrupt
		 * has to be enabled. Only the top level door disabled
		 * However, GIC also has to patch enable or disable low level
		 * API
		 */
		if (pctrl->cid == DMA3)
			disable_irq(irqres.start);
	}
	return 0;
}

#ifdef CONFIG_DEBUG_FS
static void *dma_chan_reg_seq_start(struct seq_file *s, loff_t *pos)
{
	struct dmax_chan *pch;
	struct dma_ctrl *pctrl = s->private;

	if (*pos >= pctrl->ports[0].chan_nrs)
		return NULL;
	pch = &pctrl->ports[0].chans[*pos];

	return pch;
}

static void *dma_chan_reg_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct dmax_chan *pch;
	struct dma_ctrl *pctrl = s->private;

	if (++*pos >= pctrl->ports[0].chan_nrs)
		return NULL;
	pch = &pctrl->ports[0].chans[*pos];
	return pch;
}

static void dma_chan_reg_seq_stop(struct seq_file *s, void *v)
{
}

static int dma_chan_reg_seq_show(struct seq_file *s, void *v)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = (struct dmax_chan *)v;

	pctrl = dma_chan_get_controller(pch);
	if (!pctrl || !pch)
		return -ENODEV;
	if (!dma_chan_in_use(pch))
		return 0; /* No need to show unnecessary info */
	seq_printf(s, "----------%s chan %d---------------\n",
		   pctrl->name, pch->nr);
	ltq_dma_w32(pctrl, pch->nr, DMA_CS);
	seq_printf(s, "DMA_CCTRL=  %08x\n", ltq_dma_r32(pctrl, DMA_CCTRL));
	seq_printf(s, "DMA_CDBA=   %08x\n", ltq_dma_r32(pctrl, DMA_CDBA));
	seq_printf(s, "DMA_CIE=    %08x\n", ltq_dma_r32(pctrl, DMA_CIE));
	seq_printf(s, "DMA_CIS=    %08x\n", ltq_dma_r32(pctrl, DMA_CIS));
	seq_printf(s, "DMA_CDLEN=  %08x\n", ltq_dma_r32(pctrl, DMA_CDLEN));
	seq_printf(s, "DMA_CGBL=   %08x\n", ltq_dma_r32(pctrl, DMA_CGBL));
	seq_printf(s, "DMA_CDPTNR= %08x\n", ltq_dma_r32(pctrl, DMA_CDPTNR));
	seq_printf(s, "DMA_CPDCNT= %08x\n", ltq_dma_r32(pctrl, DMA_CPDCNT));

	if (pctrl->ver > DMA_VER_22) {
		seq_printf(s, "DMA_C_DP_TICK=  %08x\n",
			   ltq_dma_r32(pctrl, DMA_C_DP_TICK));
		seq_printf(s, "DMA_C_HDRM=     %08x\n",
			   ltq_dma_r32(pctrl, DMA_C_HDRM));
		seq_printf(s, "DMA_C_BOFF=     %08x\n",
			   ltq_dma_r32(pctrl, DMA_C_BOFF));
		seq_printf(s, "DMA_C_INTCO=    %08x\n",
			   ltq_dma_r32(pctrl, DMA_C_INTCO));
		seq_printf(s, "DMA_C_SWPOLL=  %08x\n",
			   ltq_dma_r32(pctrl, DMA_C_SWPOLL));
		seq_printf(s, "DMA_C_ENDIAN=   %08x\n",
			   ltq_dma_r32(pctrl, DMA_C_ENDIAN));
	}
	return 0;
}

static const struct seq_operations dma_chan_reg_seq_ops = {
	.start = dma_chan_reg_seq_start,
	.next = dma_chan_reg_seq_next,
	.stop = dma_chan_reg_seq_stop,
	.show = dma_chan_reg_seq_show,
};

static int dma_chan_reg_seq_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	int ret = seq_open(file, &dma_chan_reg_seq_ops);

	if (!ret) {
		s = file->private_data;
		s->private = inode->i_private;
	}
	return ret;
}

static const struct file_operations dma_chan_reg_fops = {
	.open = dma_chan_reg_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static void *dma_chan_desc_seq_start(struct seq_file *s, loff_t *pos)
{
	struct dmax_chan *pch;
	struct dma_ctrl *pctrl = s->private;

	if (*pos >= pctrl->ports[0].chan_nrs)
		return NULL;
	pch = &pctrl->ports[0].chans[*pos];
	return pch;
}

static void *dma_chan_desc_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct dma_ctrl *pctrl = s->private;
	struct dmax_chan *pch;

	if (++*pos >= pctrl->ports[0].chan_nrs)
		return NULL;
	pch = &pctrl->ports[0].chans[*pos];

	return pch;
}

static void dma_chan_desc_seq_stop(struct seq_file *s, void *v)
{
}

static int dma_tx_desc_show(struct seq_file *s, struct dmax_chan *pch)
{
	int i;
	struct dma_tx_desc *tx_desc;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (!pctrl)
		return -ENODEV;
	if (pch->desc_len == 0)
		return -ENODEV;
	if (dma_chan_is_hw_desc(pch))
		return -ENODEV;
	seq_printf(s, "channel %d %s Tx descriptor list:\n",
		   pch->nr, pctrl->name);
	seq_puts(s, "No  address   dw0       dw1       status    ");
	seq_puts(s, "data pointer\n");
	seq_puts(s, "    Own, Complete, SoP, EoP,  Dic,   Pdu type");
	seq_puts(s, "  Offset  Qid  mpoa_pt, mpoa_mode\n");
	seq_puts(s, "    Session id, Tcp_err, Nat, Dec, Enc, Mpe2");
	seq_puts(s, " Mpe1, Color\n");
	seq_puts(s, "    Dest port, Class, TunnelID, FlowID, Eth_type,");
	seq_puts(s, " Dest_If\n");
	seq_puts(s, "------------------------------------------------------\n");

	for (i = 0; i < pch->desc_len; i++) {
		tx_desc = (struct dma_tx_desc *)pch->desc_base + i;
		seq_printf(s, "%03d ", i);
		seq_printf(s, "%p  %08x  ", tx_desc, tx_desc->dw0.all);
		seq_printf(s, "%08x  ", tx_desc->dw1.all);
		seq_printf(s, "%08x  ", tx_desc->status.all);
		seq_printf(s, "%08x\n", (u32)tx_desc->data_pointer);
		if (tx_desc->status.field.own)
			seq_puts(s, "    DMA  ");
		else
			seq_puts(s, "CPU  ");
		if (tx_desc->status.field.c)
			seq_puts(s, "CPT  ");
		else
			seq_puts(s, "Progress  ");
		if (tx_desc->status.field.sop)
			seq_puts(s, "SoP  ");
		else
			seq_puts(s, "No SoP  ");
		if (tx_desc->status.field.eop)
			seq_puts(s, "EoP   ");
		else
			seq_puts(s, "No EoP  ");
		if (tx_desc->status.field.dic)
			seq_puts(s, " To Drop  ");
		else
			seq_puts(s, " No drop  ");
		if (tx_desc->status.field.pdu)
			seq_puts(s, " OAM or AAL  ");
		else
			seq_puts(s, " Normal  ");
		seq_printf(s, "%02x    ", tx_desc->status.field.byte_offset);
		seq_printf(s, "%d    ", tx_desc->status.field.qid);
		if (tx_desc->status.field.mpoa_pt)
			seq_puts(s, "MPOA pt  ");
		else
			seq_puts(s, "No MPOA pt  ");
		if (tx_desc->status.field.mpoa_mode)
			seq_puts(s, "mpoa mode  ");
		else
			seq_puts(s, "No mpoa  ");
		seq_putc(s, '\n');
		seq_printf(s, "    %04x       ", tx_desc->dw1.field.session_id);
		if (tx_desc->dw1.field.tcp_err)
			seq_puts(s, " Error   ");
		else
			seq_puts(s, " N       ");
		if (tx_desc->dw1.field.nat)
			seq_puts(s, "Done   ");
		else
			seq_puts(s, "N      ");
		if (tx_desc->dw1.field.dec)
			seq_puts(s, "En    ");
		else
			seq_puts(s, "Dis   ");
		if (tx_desc->dw1.field.enc)
			seq_puts(s, "En    ");
		else
			seq_puts(s, "Dis   ");
		if (tx_desc->dw1.field.mpe2)
			seq_puts(s, "Y    ");
		else
			seq_puts(s, "N    ");
		if (tx_desc->dw1.field.mpe1)
			seq_puts(s, "Y    ");
		else
			seq_puts(s, "N    ");
		seq_printf(s, "%d  ", tx_desc->dw1.field.color);
		seq_putc(s, '\n');
		seq_printf(s, "    %d           ", tx_desc->dw1.field.ep);
		seq_printf(s, "%d       ", tx_desc->dw1.field.cla);

		seq_printf(s, "%d     ", tx_desc->dw0.field.tunnel_id);
		seq_printf(s, "%2x     ", tx_desc->dw0.field.flow_id);
		switch (tx_desc->dw0.field.eth_type) {
		case 0:
			seq_puts(s, "Eth V2   ");
			break;
		case 2:
			seq_puts(s, "IPX      ");
			break;
		case 3:
			seq_puts(s, "EAPOL    ");
			break;
		}
		seq_printf(s, "%4x   ", tx_desc->dw0.field.dest_id);
		if (pch->curr_desc == i)
			seq_puts(s, "<- CURR");
		if (pch->prev_desc == i)
			seq_puts(s, "<- PREV");
		seq_puts(s, "\n\n");
	}
	return 0;
}

static int dma_rx_desc_show(struct seq_file *s, struct dmax_chan *pch)
{
	int i;
	struct dma_rx_desc *rx_desc;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (!pctrl)
		return -ENODEV;
	if (pch->desc_len == 0)
		return -ENODEV;
	if (dma_chan_is_hw_desc(pch))
		return -ENODEV;

	seq_printf(s, "channel %d %s Rx descriptor list:\n",
		   pch->nr, pctrl->name);

	seq_puts(s, "No  address   dw0       dw1       status    ");
	seq_puts(s, "data pointer\n");
	seq_puts(s, "    Own, Complete, SoP, EoP,  Dic,   Pdu type");
	seq_puts(s, "  Offset  Qid  mpoa_pt, mpoa_mode\n");
	seq_puts(s, "    Session id, Tcp_err, Nat, Dec, Enc, Mpe2");
	seq_puts(s, " Mpe1, Color\n");
	seq_puts(s, "    Dest port, Class, TunnelID, FlowID, Eth_type,");
	seq_puts(s, " Dest_If\n");
	seq_puts(s, "------------------------------------------------------\n");

	for (i = 0; i < pch->desc_len; i++) {
		rx_desc = (struct dma_rx_desc *)pch->desc_base + i;
		seq_printf(s, "%03d ", i);
		seq_printf(s, "%p  %08x  ", rx_desc, rx_desc->dw0.all);
		seq_printf(s, "%08x  ", rx_desc->dw1.all);
		seq_printf(s, "%08x  ", rx_desc->status.all);
		seq_printf(s, "%08x\n", (u32)rx_desc->data_pointer);
		if (rx_desc->status.field.own)
			seq_puts(s, "    DMA  ");
		else
			seq_puts(s, "CPU  ");
		if (rx_desc->status.field.c)
			seq_puts(s, "CPT  ");
		else
			seq_puts(s, "Progress  ");
		if (rx_desc->status.field.sop)
			seq_puts(s, "SoP  ");
		else
			seq_puts(s, "No SoP  ");
		if (rx_desc->status.field.eop)
			seq_puts(s, "EoP   ");
		else
			seq_puts(s, "No EoP  ");
		if (rx_desc->status.field.dic)
			seq_puts(s, " To Drop  ");
		else
			seq_puts(s, " No drop  ");
		if (rx_desc->status.field.pdu)
			seq_puts(s, " OAM or AAL  ");
		else
			seq_puts(s, " Normal  ");
		seq_printf(s, "%02x    ", rx_desc->status.field.byte_offset);
		seq_printf(s, "%d    ", rx_desc->status.field.qid);
		if (rx_desc->status.field.mpoa_pt)
			seq_puts(s, "MPOA pt  ");
		else
			seq_puts(s, "No MPOA pt  ");
		if (rx_desc->status.field.mpoa_mode)
			seq_puts(s, "mpoa mode  ");
		else
			seq_puts(s, "No mpoa  ");
		seq_putc(s, '\n');
		seq_printf(s, "    %04x       ", rx_desc->dw1.field.session_id);
		if (rx_desc->dw1.field.tcp_err)
			seq_puts(s, " Error   ");
		else
			seq_puts(s, " N       ");
		if (rx_desc->dw1.field.nat)
			seq_puts(s, "Done   ");
		else
			seq_puts(s, "N      ");
		if (rx_desc->dw1.field.dec)
			seq_puts(s, "En    ");
		else
			seq_puts(s, "Dis   ");
		if (rx_desc->dw1.field.enc)
			seq_puts(s, "En    ");
		else
			seq_puts(s, "Dis   ");
		if (rx_desc->dw1.field.mpe2)
			seq_puts(s, "Y    ");
		else
			seq_puts(s, "N    ");
		if (rx_desc->dw1.field.mpe1)
			seq_puts(s, "Y    ");
		else
			seq_puts(s, "N    ");
		seq_printf(s, "%d  ", rx_desc->dw1.field.color);
		seq_putc(s, '\n');
		seq_printf(s, "    %d           ", rx_desc->dw1.field.ep);
		seq_printf(s, "%d       ", rx_desc->dw1.field.cla);

		seq_printf(s, "%d     ", rx_desc->dw0.field.tunnel_id);
		seq_printf(s, "%2x     ", rx_desc->dw0.field.flow_id);
		switch (rx_desc->dw0.field.eth_type) {
		case 0:
			seq_puts(s, "Eth V2   ");
			break;
		case 2:
			seq_puts(s, "IPX      ");
			break;
		case 3:
			seq_puts(s, "EAPOL    ");
			break;
		}
		seq_printf(s, "%4x   ", rx_desc->dw0.field.dest_id);
		if (pch->curr_desc == i)
			seq_puts(s, "<- CURR");
		if (pch->prev_desc == i)
			seq_puts(s, "<- PREV");
		seq_puts(s, "\n\n");
	}
	return 0;
}

static int dma_tx_2dws_desc_show(struct seq_file *s, struct dmax_chan *pch)
{
	int i;
	struct dma_tx_desc_2dw *tx_desc;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (!pctrl)
		return -ENODEV;
	if (pch->desc_len == 0)
		return -ENODEV;
	seq_printf(s, "channel %d %s Tx descriptor list:\n",
		   pch->nr, pctrl->name);
	seq_puts(s, "No  address   status     data pointer\n");
	seq_puts(s, "    Own, Complete, SoP, EoP,   Offset\n");

	seq_puts(s, "------------------------------------------------------\n");
	for (i = 0; i < pch->desc_len; i++) {
		tx_desc = (struct dma_tx_desc_2dw *)pch->desc_base + i;
		seq_printf(s, "%03d ", i);
		seq_printf(s, "%p  %08x  ", tx_desc, tx_desc->status.all);
		seq_printf(s, "%08x\n", (u32)tx_desc->data_pointer);
		if (tx_desc->status.field.own)
			seq_puts(s, "    DMA  ");
		else
			seq_puts(s, "    CPU  ");
		if (tx_desc->status.field.c)
			seq_puts(s, "CPT       ");
		else
			seq_puts(s, "Progress  ");
		if (tx_desc->status.field.sop)
			seq_puts(s, "SoP  ");
		else
			seq_puts(s, "No SoP  ");
		if (tx_desc->status.field.eop)
			seq_puts(s, "EoP   ");
		else
			seq_puts(s, "No EoP  ");
		seq_printf(s, "%02x    ", tx_desc->status.field.byte_offset);

		if (pch->curr_desc == i)
			seq_puts(s, "<- CURR");
		if (pch->prev_desc == i)
			seq_puts(s, "<- PREV");
		seq_puts(s, "\n\n");
	}
	return 0;
}

static int dma_rx_2dws_desc_show(struct seq_file *s, struct dmax_chan *pch)
{
	int i;
	struct dma_rx_desc_2dw *rx_desc;
	struct dma_ctrl *pctrl = dma_chan_get_controller(pch);

	if (!pctrl)
		return -ENODEV;
	if (pch->desc_len == 0)
		return -ENODEV;

	seq_printf(s, "channel %d %s Rx descriptor list:\n",
		   pch->nr, pctrl->name);

	seq_puts(s, "No  address   status    data pointer\n");
	seq_puts(s, "    Own, Complete, SoP, EoP,  Dic,  Offset\n");
	seq_puts(s, "------------------------------------------------------\n");

	for (i = 0; i < pch->desc_len; i++) {
		rx_desc = (struct dma_rx_desc_2dw *)pch->desc_base + i;
		seq_printf(s, "%03d ", i);
		seq_printf(s, "%p  %08x  ", rx_desc, rx_desc->status.all);
		seq_printf(s, "%08x\n", (u32)rx_desc->data_pointer);
		if (rx_desc->status.field.own)
			seq_puts(s, "    DMA  ");
		else
			seq_puts(s, "    CPU  ");
		if (rx_desc->status.field.c)
			seq_puts(s, "CPT       ");
		else
			seq_puts(s, "Progress  ");
		if (rx_desc->status.field.sop)
			seq_puts(s, "SoP  ");
		else
			seq_puts(s, "No SoP  ");
		if (rx_desc->status.field.eop)
			seq_puts(s, "EoP   ");
		else
			seq_puts(s, "No EoP  ");
		seq_printf(s, "%02x    ", rx_desc->status.field.byte_offset);
		if (pch->curr_desc == i)
			seq_puts(s, "<- CURR");
		if (pch->prev_desc == i)
			seq_puts(s, "<- PREV");
		seq_puts(s, "\n\n");
	}
	return 0;
}

static int dma_chan_desc_seq_show(struct seq_file *s, void *v)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = (struct dmax_chan *)v;

	if (!pch)
		return -ENODEV;

	pctrl = dma_chan_get_controller(pch);
	if (dma_is_64bit(pctrl)) {
		if (dma_chan_rx(pch))
			dma_rx_desc_show(s, pch);
		else
			dma_tx_desc_show(s, pch);
	} else {
		if (dma_chan_rx(pch))
			dma_rx_2dws_desc_show(s, pch);
		else
			dma_tx_2dws_desc_show(s, pch);
	}
	return 0;
}

static const struct seq_operations dma_chan_desc_seq_ops = {
	.start = dma_chan_desc_seq_start,
	.next = dma_chan_desc_seq_next,
	.stop = dma_chan_desc_seq_stop,
	.show = dma_chan_desc_seq_show,
};

static int dma_chan_desc_seq_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	int ret = seq_open(file, &dma_chan_desc_seq_ops);

	if (!ret) {
		s = file->private_data;
		s->private = inode->i_private;
	}
	return ret;
}

static const struct file_operations dma_chan_desc_fops = {
	.open = dma_chan_desc_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

/*
 * DMA ver 2 and above, each DMA has only one port
 * Port means controller so put controller info together
 */
static int dma_ctrl_port_read(struct seq_file *s, void *v)
{
	int i;
	u32 reg;
	struct dma_ctrl *pctrl = s->private;

	seq_puts(s, "\nGeneral DMA Registers\n");
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "CLC=        %08x\n", ltq_dma_r32(pctrl, DMA_CLC));
	seq_printf(s, "ID=         %08x\n", ltq_dma_r32(pctrl, DMA_ID));
	seq_printf(s, "CTRL=       %08x\n", ltq_dma_r32(pctrl, DMA_CTRL));
	seq_printf(s, "DMA_CPOLL=  %08x\n", ltq_dma_r32(pctrl, DMA_CPOLL));
	seq_printf(s, "DMA_CS=     %08x\n", ltq_dma_r32(pctrl, DMA_CS));
	seq_printf(s, "DMA_PS=     %08x\n", ltq_dma_r32(pctrl, DMA_PS));
	seq_printf(s, "DMA_IRNEN=  %08x\n", ltq_dma_r32(pctrl, DMA_IRNEN));
	seq_printf(s, "DMA_IRNCR=  %08x\n", ltq_dma_r32(pctrl, DMA_IRNCR));
	seq_printf(s, "DMA_IRNICR= %08x\n", ltq_dma_r32(pctrl, DMA_IRNICR));

	if (pctrl->chans > 32) {
		seq_printf(s, "DMA_IRNEN1=  %08x\n",
			   ltq_dma_r32(pctrl, DMA_IRNEN1));
		seq_printf(s, "DMA_IRNCR1=  %08x\n",
			   ltq_dma_r32(pctrl, DMA_IRNCR1));
		seq_printf(s, "DMA_IRNICR1= %08x\n",
			   ltq_dma_r32(pctrl, DMA_IRNICR1));
	}

	if (pctrl->ver > DMA_VER_22) {
		seq_printf(s, "DMA_ORRC=    %08x\n",
			   ltq_dma_r32(pctrl, DMA_ORRC));
		seq_printf(s, "DMA_LOG_CH=  %08x\n",
			   ltq_dma_r32(pctrl, DMA_LOG_CH));
	}

	seq_puts(s, "\nDMA Port Registers\n");
	seq_puts(s, "-----------------------------------------\n");
	for (i = 0; i < pctrl->port_nrs; i++) {
		ltq_dma_w32(pctrl, i, DMA_PS);
		reg = ltq_dma_r32(pctrl, DMA_PCTRL);
		seq_printf(s, "Port %d DMA_PCTRL= %08x\n", i, reg);
		if (pctrl->ver >= DMA_VER_22 && ((pctrl->cid == DMA1TX) ||
		    (pctrl->cid == DMA1RX) || (pctrl->cid == DMA2TX) ||
		    (pctrl->cid == DMA2RX))) {
			seq_printf(s, "P2D Class %d\n",
				   MS(reg, DMA_DBG_P2D_CLS));
			seq_printf(s, "P2D Ack %s\n", (reg & DMA_DBG_P2D_ACK) ?
				   "Yes" : "No");
			seq_printf(s, "D2P Class %d\n",
				   MS(reg, DMA_DBG_D2P_CLS));
			seq_printf(s, "D2P Ack %s\n", (reg & DMA_DBG_D2P_ACK) ?
				   "Yes" : "No");
			seq_printf(s, "P2D Req %s\n", (reg & DMA_DBG_P2D_REQ) ?
				   "Yes" : "No");
			seq_printf(s, "D2P Req %s\n", (reg & DMA_DBG_D2P_REQ) ?
				   "Yes" : "No");
			seq_printf(s, "P2D RME %s\n", (reg & DMA_DBG_P2D_RME) ?
				   "Yes" : "No");
			seq_printf(s, "D2P XME %s\n", (reg & DMA_DBG_D2P_XME) ?
				   "Yes" : "No");
			seq_printf(s, "P2D JMB %s\n", (reg & DMA_DBG_P2D_JMB) ?
				   "Yes" : "No");
		}
	}
	return 0;
}

static int dma_ctrl_port_read_open(struct inode *inode, struct file *file)
{
	return single_open(file, dma_ctrl_port_read, inode->i_private);
}

static const struct file_operations dma_ctrl_port_fops = {
	.open           = dma_ctrl_port_read_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static int dma_ctrl_cfg_read(struct seq_file *s, void *v)
{
	struct dma_ctrl *pctrl = s->private;

	seq_printf(s, "\n%s controller configuration\n", pctrl->name);
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "Controller id            %d\n", pctrl->cid);
	seq_printf(s, "Membase                  %p\n", pctrl->membase);
	seq_printf(s, "Chained irq <GIC>        %d\n", pctrl->chained_irq);
	seq_printf(s, "Virtual irq base         %d\n", pctrl->irq_base);
	seq_printf(s, "Number of ports          %d\n", pctrl->port_nrs);
	seq_printf(s, "Number of channels       %d\n", pctrl->chans);
	seq_printf(s, "Global polling counter   %d\n", pctrl->pollcnt);
	seq_printf(s, "Look ahead buff counter  %d\n", pctrl->labcnt);
	seq_printf(s, "Outstanding read counter %d\n", pctrl->orrc);

	seq_printf(s, "Arbtration type          %s\n",
		   arb_type_array[pctrl->arb_type]);
	seq_printf(s, "Descriptor Size          %d byte\n", pctrl->desc_size);
	seq_printf(s, "DMA burst mask           %d <0 ~ %d> byte\n",
		   pctrl->burst_mask, pctrl->burst_mask);

	seq_printf(s, "DMA                      %s\n",
		   (pctrl->flags & DMA_CTL_64BIT) ? "64bit" : "32bit");

	seq_printf(s, "DMA channel flow control %s\n",
		   (pctrl->flags & DMA_FLCTL) ? "enable" : "disable");

	seq_printf(s, "DMA desc fetch on demand %s\n",
		   (pctrl->flags & DMA_FTOD) ? "enable" : "disable");

	seq_printf(s, "DMA descriptor in        %s\n",
		   (pctrl->flags & DMA_DESC_IN_SRAM) ? "SRAM" : "DDR");

	seq_printf(s, "DMA descriptor read back %s\n",
		   (pctrl->flags & DMA_DRB) ? "enable" : "disable");

	seq_printf(s, "DMA Byte enable function %s\n",
		   (pctrl->flags & DMA_EN_BYTE_EN) ? "enable" : "disable");
	return 0;
}

static int dma_ctrl_cfg_read_open(struct inode *inode, struct file *file)
{
	return single_open(file, dma_ctrl_cfg_read, inode->i_private);
}

static const struct file_operations dma_ctrl_cfg_fops = {
	.open           = dma_ctrl_cfg_read_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static void *dma_port_cfg_seq_start(struct seq_file *s, loff_t *pos)
{
	struct dma_port *pport;
	struct dma_ctrl *pctrl = s->private;

	if (*pos >= pctrl->port_nrs)
		return NULL;
	pport = &pctrl->ports[*pos];
	return pport;
}

static void *dma_port_cfg_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct dma_port *pport;
	struct dma_ctrl *pctrl = s->private;

	if (++*pos >= pctrl->port_nrs)
		return NULL;
	pport = &pctrl->ports[*pos];
	return pport;
}

static void dma_port_cfg_seq_stop(struct seq_file *s, void *v)
{
}

static int burst_cfg_burst_len(int burst)
{
	if (burst < DMA_BURSTL_8DW)
		return burst * 2;
	else if (burst == DMA_BURSTL_8DW)
		return 8;
	else if (burst == DMA_BURSTL_16DW)
		return 16;
	else if (burst == DMA_BURSTL_32DW)
		return 32;
	else
		return 0;
}

static int dma_port_cfg_seq_show(struct seq_file *s, void *v)
{
	struct dma_ctrl *pctrl;
	struct dma_port *pport = (struct dma_port *)v;

	pctrl = dma_port_get_controller(pport);
	if (!pctrl)
		return -ENODEV;
	seq_printf(s, "---------%s port %s-----\n", pctrl->name, pport->name);
	seq_printf(s, "Port id              %d\n", pport->pid);
	seq_printf(s, "Number of Channel    %d\n", pport->chan_nrs);
	seq_printf(s, "TX endian            %d\n", pport->txendi);
	seq_printf(s, "RX endian            %d\n", pport->rxendi);
	seq_printf(s, "TX Burst length      %d dw\n",
		   burst_cfg_burst_len(pport->txbl));
	seq_printf(s, "RX Burst length      %d dw\n",
		   burst_cfg_burst_len(pport->rxbl));
	seq_printf(s, "TX weight            %d\n", pport->txwgt);
	seq_printf(s, "Pkt drop             %s\n",
		   pport->pkt_drop ? "Enable" : "Disable");
	seq_printf(s, "Memory Flush         %s\n",
		   pport->flush_memcpy ? "Enable" : "Disable");
	return 0;
}

static const struct seq_operations dma_port_cfg_seq_ops = {
	.start = dma_port_cfg_seq_start,
	.next = dma_port_cfg_seq_next,
	.stop = dma_port_cfg_seq_stop,
	.show = dma_port_cfg_seq_show,
};

static int dma_port_cfg_seq_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	int ret = seq_open(file, &dma_port_cfg_seq_ops);

	if (!ret) {
		s = file->private_data;
		s->private = inode->i_private;
	}
	return ret;
}

static const struct file_operations dma_port_cfg_fops = {
	.open = dma_port_cfg_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static void *dma_chan_cfg_seq_start(struct seq_file *s, loff_t *pos)
{
	struct dmax_chan *pch;
	struct dma_ctrl *pctrl = s->private;

	if (*pos >= pctrl->ports[0].chan_nrs)
		return NULL;
	pch = &pctrl->ports[0].chans[*pos];
	return pch;
}

static void *dma_chan_cfg_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct dmax_chan *pch;
	struct dma_ctrl *pctrl = s->private;

	if (++*pos >= pctrl->ports[0].chan_nrs)
		return NULL;
	pch = &pctrl->ports[0].chans[*pos];
	return pch;
}

static void dma_chan_cfg_seq_stop(struct seq_file *s, void *v)
{
}

static int dma_chan_cfg_seq_show(struct seq_file *s, void *v)
{
	struct dma_ctrl *pctrl;
	struct dmax_chan *pch = (struct dmax_chan *)v;

	pctrl = dma_chan_get_controller(pch);
	if (!pctrl)
		return -ENODEV;
	if (!dma_chan_in_use(pch))
		return 0; /* No need to show unnecessary info */
	seq_printf(s, "-----------------%s-------------------\n", pctrl->name);
	seq_printf(s, "Channel number                %d\n", pch->nr);
	seq_printf(s, "Global channel number     %d.%d.%d\n",
		   _DMA_CONTROLLER(pch->lnr), _DMA_PORT(pch->lnr),
		   _DMA_CHANNEL(pch->lnr));
	seq_printf(s, "Channel direction             %s\n",
		   dma_chan_tx(pch) ? "TX" : "RX");
	seq_printf(s, "Device id <client>            %s\n", pch->device_id);
	seq_printf(s, "Channel Reset                 %d\n", pch->rst);
	seq_printf(s, "Channel OnOff                 %s\n",
		   pch->onoff ? "On" : "Off");
	seq_printf(s, "Channel virtual irq           %d\n", pch->irq);
	seq_printf(s, "Channel packet drop           %s\n",
		   pch->pden ? "Enable" : "Disable");
	seq_printf(s, "Packet size                   %d\n", pch->pkt_size);
	seq_printf(s, "Desc type                     %s\n",
		   dma_chan_is_hw_desc(pch) ? "hw" : "sw");
	seq_printf(s, "Desc base                     0x%08x\n", pch->desc_base);
	seq_printf(s, "Desc phys base                0x%08x\n", pch->desc_phys);
	seq_printf(s, "Desc len                      %d\n", pch->desc_len);
	seq_printf(s, "Current desc                  %d\n", pch->curr_desc);
	seq_printf(s, "Prev desc                     %d\n", pch->prev_desc);
	seq_printf(s, "Channel tx weight             %d\n", pch->txwgt);
	if (pctrl->ver > DMA_VER_22) {
		seq_printf(s, "Non arb polling counter       %d\n",
			   pch->nonarb_cnt);
		seq_printf(s, "Arbitration polling counter   %d\n",
			   pch->arb_cnt);
		seq_printf(s, "Header only transfer size     %d\n",
			   pch->hdrm_len);
		seq_printf(s, "Byte offset length            %d\n",
			   pch->boff_len);
		seq_printf(s, "Interrupt coalesce number     %d\n",
			   pch->coal_len);
		seq_printf(s, "Software polling              %s\n",
			   pch->sw_poll ? "Enable" : "Disable");
		seq_printf(s, "Data endian                   %s\n",
			   pch->data_endian_en ? "Enable" : "Disable");
		seq_printf(s, "Data endian type              %d\n",
			   pch->data_endian);
		seq_printf(s, "Desc endian                   %s\n",
			   pch->desc_endian_en ? "Enable" : "Disable");
		seq_printf(s, "Desc endian type              %d\n",
			   pch->desc_endian);
	}
	if (dma_chan_tx(pch)) {
		seq_printf(s, "P2P                           %s\n",
			   pch->p2pcpy ? "Enabled" : "Disabled");
		seq_printf(s, "Global buffer length          %d\n",
			   pch->global_buffer_len);
	}

	if (dma_chan_rx(pch)) {
		seq_printf(s, "Fast Loopback                 %s\n",
			   pch->lpbk_en ? "Enabled" : "Disabled");
		seq_printf(s, "Global buffer length          %d\n",
			   pch->lpbk_ch_nr);
	}
	seq_printf(s, "Channel is                    %s\n",
		   dma_chan_in_use(pch) ? "In use" : "not used");
	seq_printf(s, "Descriptor allocated by       %s\n",
		   dma_chan_desc_alloc_by_device(pch) ?
		   "Device" : "DMA controller");

	seq_printf(s, "Interrupt handled by          %s\n",
		   dma_chan_controlled_by_device(pch) ?
		   "Device" : "DMA controller");

	return 0;
}

static const struct seq_operations dma_chan_cfg_seq_ops = {
	.start = dma_chan_cfg_seq_start,
	.next = dma_chan_cfg_seq_next,
	.stop = dma_chan_cfg_seq_stop,
	.show = dma_chan_cfg_seq_show,
};

static int dma_chan_cfg_seq_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	int ret = seq_open(file, &dma_chan_cfg_seq_ops);

	if (!ret) {
		s = file->private_data;
		s->private = inode->i_private;
	}
	return ret;
}

static const struct file_operations dma_chan_cfg_fops = {
	.open = dma_chan_cfg_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int dma_debugfs_init(struct dma_ctrl *pctrl)
{
	char dma_dir[64] = {0};
	struct dentry *file;

	strlcpy(dma_dir, pctrl->name, sizeof(dma_dir));
	pctrl->debugfs = debugfs_create_dir(dma_dir, NULL);
	if (!pctrl->debugfs)
		return -ENOMEM;

	file = debugfs_create_file("chan_register", 0644, pctrl->debugfs,
				   pctrl, &dma_chan_reg_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("desc_list", 0644, pctrl->debugfs,
				   pctrl, &dma_chan_desc_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("ctrl_port_register", 0644,
				   pctrl->debugfs, pctrl, &dma_ctrl_port_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("ctrl_cfg", 0644, pctrl->debugfs,
				   pctrl, &dma_ctrl_cfg_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("port_cfg", 0644, pctrl->debugfs,
				   pctrl, &dma_port_cfg_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("chan_cfg", 0644, pctrl->debugfs,
				   pctrl, &dma_chan_cfg_fops);
	if (!file)
		goto err;
	return 0;
err:
	debugfs_remove_recursive(pctrl->debugfs);
	return -ENOMEM;
}
#else
static int dma_debugfs_init(struct dma_ctrl *pctrl)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */

static u32 burst_len_to_burst_cfg(int val)
{
	u32 burst = DMA_BURSTL_16DW;

	switch (val) {
	case 2:
		burst = DMA_BURSTL_2DW;
		break;
	case 4:
		burst = DMA_BURSTL_4DW;
		break;
	case 8:
		burst = DMA_BURSTL_8DW;
		break;
	case 16:
		burst = DMA_BURSTL_16DW;
		break;
	case 32:
		burst = DMA_BURSTL_32DW;
		break;
	default:
		burst = DMA_BURSTL_16DW;
		break;
	}
	return burst;
}

static int dma_cfg_init(struct dma_ctrl *pctrl)
{
	int ret;
	int i, j;
	u32 prop;
	u32 chan_fc = 0;
	u32 desc_fod = 0;
	u32 desc_insram = 0;
	u32 dma_drb = 0;
	u32 byte_en = 0;
	u32 txendi;
	u32 rxendi;
	struct dma_port *pport;
	struct dmax_chan *pch;
	struct device_node *node = pctrl->dev->of_node;

	if (pctrl->cid != DMA0)
		pctrl->flags |= DMA_CTL_64BIT;

	if ((pctrl->cid == DMA3) || (pctrl->cid == DMA4))
		pctrl->port_nrs--;

	if (pctrl->flags & DMA_CTL_64BIT) {
		pctrl->burst_mask = DMA_4DW_DESC_MASK;
		pctrl->desc_size = DMA_4DW_DESC_SIZE;
	} else {
		pctrl->burst_mask = DMA_2DW_DESC_MASK;
		pctrl->desc_size = DMA_2DW_DESC_SIZE;
	}

	if (!of_property_read_u32(node, "lantiq,dma-pkt-arb", &prop)) {
		if (prop > DMA_ARB_MAX)
			pctrl->arb_type = DMA_ARB_PKT;
		else
			pctrl->arb_type = prop;

	} else {
		pctrl->arb_type = DMA_ARB_PKT;
	}

	if (!of_property_read_u32(node, "lantiq,dma-chan-fc", &chan_fc)) {
		if (chan_fc)
			pctrl->flags |= DMA_FLCTL;
		else
			pctrl->flags &= ~DMA_FLCTL;
	}

	if (!of_property_read_u32(node, "lantiq,dma-desc-fod", &desc_fod)) {
		if (desc_fod)
			pctrl->flags |= DMA_FTOD;
		else
			pctrl->flags &= ~DMA_FTOD;
	}

	ret = of_property_read_u32(node, "lantiq,dma-desc-in-sram",
				   &desc_insram);
	if (!ret) {
		if (desc_insram)
			pctrl->flags |= DMA_DESC_IN_SRAM;
		else
			pctrl->flags &= ~DMA_DESC_IN_SRAM;
	}

	if (!of_property_read_u32(node, "lantiq,dma-drb", &dma_drb)) {
		if (dma_drb)
			pctrl->flags |= DMA_DRB;
		else
			pctrl->flags &= ~DMA_DRB;
	}

	if (!of_property_read_u32(node, "lantiq,dma-byte-en", &byte_en)) {
		if (byte_en)
			pctrl->flags |= DMA_EN_BYTE_EN;
		else
			pctrl->flags &= ~DMA_EN_BYTE_EN;
	}
	if (!of_property_read_u32(node, "lantiq,dma-polling-cnt", &prop))
		pctrl->pollcnt = prop;
	else
		pctrl->pollcnt = DMA_GLOBAL_POLLING_DEFAULT_INTERVAL;

	if (!of_property_read_u32(node, "lantiq,dma-lab-cnt", &prop))
		pctrl->labcnt = prop;
	else
		pctrl->labcnt = 0;

	if (!of_property_read_u32(node, "lantiq,dma-orrc", &prop))
		pctrl->orrc = prop;
	else
		pctrl->orrc = 0;

	if (!of_property_read_u32(node, "lantiq,dma-txendi", &prop))
		txendi = prop;
	else
		txendi = DMA_DEFAULT_ENDIAN;

	if (!of_property_read_u32(node, "lantiq,dma-rxendi", &prop))
		rxendi = prop;
	else
		rxendi = DMA_DEFAULT_ENDIAN;

	dev_dbg(pctrl->dev, "arb %d fc %d fod %d insram %d drb %d ben %d pcnt %d labcnt %d orrc %d\n",
		pctrl->arb_type, chan_fc, desc_fod, desc_insram,
		dma_drb, byte_en, pctrl->pollcnt, pctrl->labcnt, pctrl->orrc);

	if (!of_property_read_u32(node, "lantiq,budget", &prop))
		pctrl->budget = prop;
	else
		pctrl->budget = DMA_IRQ_BUDGET;

	if (pctrl->cid == DMA0) {
		pctrl->ports = (struct dma_port *)devm_kzalloc(pctrl->dev,
			pctrl->port_nrs * sizeof(*pport), GFP_KERNEL);
		if (!pctrl->ports)
			return -ENOMEM;
		for (i = 0; i < pctrl->port_nrs; i++) {
			pport = &pctrl->ports[i];
			pport->pid = i;
			pport->name = dma0_port[i];
			pport->rxendi = DMA_ENDIAN_TYPE0;
			pport->txendi = DMA_ENDIAN_TYPE0;
			pport->rxbl = DMA_BURSTL_2DW;
			pport->txbl = DMA_BURSTL_2DW;
			pport->txwgt = DMA_TX_PORT_DEFAULT_WEIGHT;
			pport->pkt_drop = DMA_PKT_DROP_DISABLE;
			pport->flush_memcpy = 0;
			pport->chan_nrs = 2;

			if (i == DMA0_MEMCOPY) {
				pport->flush_memcpy = DMA_FLUSH_MEMCPY;
				pport->chan_nrs = 4;
			}
			pport->chans = (struct dmax_chan *)devm_kzalloc(
				pctrl->dev, pport->chan_nrs
				* sizeof(*pch), GFP_KERNEL);
			if (!pport->chans) {
				devm_kfree(pctrl->dev, pctrl->ports);
				return -ENOMEM;
			}
			for (j = 0; j < pport->chan_nrs; j++) {
				pch = &pport->chans[j];
				/* Allocate one by dma controller */
				pch->flags &= ~DEVICE_ALLOC_DESC;
				pch->nr = i * 2 + j;
				if (i == DMA0_MEMCOPY)
					pch->nr = (i * 2) + 6 + j;
				pch->onoff = DMA_CH_OFF;
				pch->rst = DMA_CHAN_RST;
				pch->pkt_size = DMA_PKT_SIZE_DEFAULT;
				pch->opt = NULL;
				pch->lnr = -1;
				pch->cis = 0;
				pch->desc_configured = false;
			}
		}
	} else {
		pctrl->ports = (struct dma_port *)devm_kzalloc(pctrl->dev,
			pctrl->port_nrs * sizeof(*pport), GFP_KERNEL);
		if (!pctrl->ports)
			return -ENOMEM;
		pport = &pctrl->ports[0];
		pport->chan_nrs = pctrl->chans;
		pport->pid = 0;
		pport->name = dma_name[pctrl->cid];
		pport->rxendi = rxendi;
		pport->txendi = txendi;

		if (!of_property_read_u32(node, "lantiq,dma-burst", &prop)) {
			int burst;

			burst = burst_len_to_burst_cfg(prop);
			pport->rxbl = burst;
			pport->txbl = burst;
		} else {
			pport->rxbl = DMA_DEFAULT_BURST;
			pport->txbl = DMA_DEFAULT_BURST;
		}
		pport->txwgt = DMA_TX_PORT_DEFAULT_WEIGHT;
		pport->pkt_drop = DMA_PKT_DROP_DISABLE;
		pport->flush_memcpy = 0; /* Check DMA3 or DMA4 */

		pport->chans = (struct dmax_chan *)devm_kzalloc(pctrl->dev,
			pport->chan_nrs * sizeof(*pch), GFP_KERNEL);
		if (!pport->chans) {
			devm_kfree(pctrl->dev, pctrl->ports);
			return -ENOMEM;
		}
		for (i = 0; i < pport->chan_nrs; i++) {
			pch = &pport->chans[i];
			pch->flags |= DEVICE_ALLOC_DESC;
			pch->nr = i;
			pch->onoff = DMA_CH_OFF;
			pch->rst = DMA_CHAN_RST;
			pch->pkt_size = DMA_PKT_SIZE_DEFAULT;
			pch->opt = NULL;
			pch->lnr = -1;
			pch->cis = 0;
			pch->desc_configured = false;
		}
	}
	return 0;
}

static int ltq_dma_probe(struct platform_device *pdev)
{
	int err;
	struct device_node *node = pdev->dev.of_node;
	struct clk *clk;
	struct resource *memres;
	struct dma_ctrl *pctrl;
	unsigned int id;
	int cidx = 0;

	/* DMA controller physical idx from aliases */
	cidx = of_alias_get_id(node, "dma");
	if (cidx < 0) {
		dev_err(&pdev->dev, "failed to get alias id, errno %d\n", cidx);
		return cidx;
	}

	pdev->id = cidx;

	if (pdev->id < 0 || pdev->id >= DMAMAX)
		return -EINVAL;

	/* NB. Dynamic allocation */
	pctrl = &ltq_dma_controller[pdev->id];

	memset(pctrl, 0, sizeof(*pctrl));

	pctrl->cid = pdev->id;
	pctrl->name = dma_name[pctrl->cid];
	/* Link controller to platform device */
	pctrl->dev = &pdev->dev;
	memres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!memres)
		panic("%s failed to get dma resource\n", __func__);

	/* remap dma register range */
	pctrl->membase = devm_ioremap_resource(&pdev->dev, memres);
	if (IS_ERR(pctrl->membase))
		panic("%s ailed to remap dma resource\n", __func__);

	err = dma_set_mask(pctrl->dev, DMA_BIT_MASK(32));
	if (err) {
		err = dma_set_coherent_mask(pctrl->dev, DMA_BIT_MASK(32));
		if (err) {
			devm_iounmap(&pdev->dev, pctrl->membase);
			dev_err(&pdev->dev,
				"No usable DMA configuration, aborting\n");
			return err;
		}
	}

	if ((pctrl->cid == DMA0) || (pctrl->cid == DMA3)) {
		/* Power up and reset the dma engine */
		clk = devm_clk_get(&pdev->dev, NULL);
		if (IS_ERR(clk))
			panic("Failed to get dma clock");
		clk_prepare_enable(clk);
	}

	id = ltq_dma_r32(pctrl, DMA_ID);
	pctrl->chans = MS(id, DMA_ID_CHNR);
	pctrl->port_nrs = MS(id, DMA_ID_PRTNR);
	pctrl->ver = MS(id, DMA_ID_REV);

	dma_cfg_init(pctrl);

	dma_irq_chip_init(pctrl);

	dev_info(pctrl->dev, "%s base address %p chained_irq %d irq_base %d\n",
		 dma_get_name_by_cid(pctrl->cid), pctrl->membase,
		 pctrl->chained_irq, pctrl->irq_base);

	dma_ctrl_init(pctrl);

	tasklet_init(&pctrl->dma_tasklet, do_dma_tasklet, (unsigned long)pctrl);

	/* Link platform with driver data for retrieving */
	platform_set_drvdata(pdev, pctrl);
	dma_debugfs_init(pctrl);
	dev_info(pctrl->dev, "Init done - rev: %x, ports: %d, channels: %d\n",
		 pctrl->ver, pctrl->port_nrs, pctrl->chans);
	return 0;
}

static const struct of_device_id ltq_dma_match[] = {
	{.compatible = "lantiq,dma-grx500"},
	{},
};

static struct platform_driver ltq_dma_driver = {
	.probe = ltq_dma_probe,
	.driver = {
		.name = "dma-grx500",
		.owner = THIS_MODULE,
		.of_match_table = ltq_dma_match,
	},
};

static int __init ltq_dma_init(void)
{
	return platform_driver_register(&ltq_dma_driver);
}
postcore_initcall(ltq_dma_init);
