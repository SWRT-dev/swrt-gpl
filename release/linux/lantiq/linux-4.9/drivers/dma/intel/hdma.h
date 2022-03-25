/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2014 ~ 2015 Lei Chuanhua <chuanhua.lei@lantiq.com>
 *  Copyright (C) 2016 ~ 2017 Intel Corporation.
 */

#ifndef LTQ_HDMA_H
#define LTQ_HDMA_H
#include <linux/dma/lantiq_dmax.h>
#include <linux/debugfs.h>

#define DMA_CHAN_MAGIC		-1

#define DMA_FLUSH_MEMCPY		1
#define DMA_CHAN_RST			1

enum dma_global_poll {
	DMA_CHAN_GLOBAL_POLLING_DIS = 0,
	DMA_CHAN_GLOBAL_POLLING_EN,
};

enum dma_chan_on_off {
	DMA_CH_OFF = 0,
	DMA_CH_ON = 1,
};

enum dma_port_txwgt {
	DMA_PORT_TXWGT0 = 0,
	DMA_PORT_TXWGT1,
	DMA_PORT_TXWGT2,
	DMA_PORT_TXWGT3,
	DMA_PORT_TXWGT4,
	DMA_PORT_TXWGT5,
	DMA_PORT_TXWGT6,
	DMA_PORT_TXWGT7,
	DMA_PORT_TXWGTMAX,
};

enum dma_chan_txwgt {
	DMA_CHAN_TXWGT0 = 0,
	DMA_CHAN_TXWGT1,
	DMA_CHAN_TXWGT2,
	DMA_CHAN_TXWGT3,
#define DMA_V22_TXWGTMAX	DMA_CHAN_TXWGT3
	DMA_CHAN_TXWGT4,
	DMA_CHAN_TXWGT5,
	DMA_CHAN_TXWGT6,
	DMA_CHAN_TXWGT7,
	DMA_CHAN_TXWGTMAX,
};

enum dma_arb {
	DMA_ARB_BURST = 0,
	DMA_ARB_MUL_BURST,
	DMA_ARB_PKT,
	DMA_ARB_MAX,
};

#define DMA_VER_31		0x31
#define DMA_VER_30		0x30
#define DMA_VER_22		0x0A

#define DMA_ORRC_MAX_CNT	31

#define DMA_ARB_MUL_BURST_DEFAULT	4

#define DMA_IRQ_BUDGET			20

#define DMA_GLOBAL_POLLING_DEFAULT_INTERVAL	4

#define DMA_MAX_DESC_NUM	4095
#define DMAV3_MAX_DESC_NUM	8191

#define DMA_MAX_PKT_SIZE	65535

#define DMA_PKT_SIZE_DEFAULT	2048

#define DMA_TX_PORT_DEFAULT_WEIGHT	1
/* Default Port Transmit weight value */
#define DMA_TX_CHAN_DEFAULT_WEIGHT	1

#define DMA_4DW_DESC_SIZE		16
#define DMA_4DW_DESC_MASK		0x7
#define DMA_2DW_DESC_SIZE		8
#define DMA_2DW_DESC_MASK		0x3

#define DMA_CHAN_HDRM_MAX		255
#define DMA_CHAN_BOFF_MAX		255
#define DMA_CHAN_COAL_MAX		255

enum {
	DMA_CTRL_UNINITIALIZED = 0,
	DMA_CTRL_INITIALIZED,
};

#ifdef CONFIG_CPU_BIG_ENDIAN
#define DMA_DEFAULT_ENDIAN	DMA_ENDIAN_TYPE3
#else
#define DMA_DEFAULT_ENDIAN	DMA_ENDIAN_TYPE0
#endif

#define DMA_DEFAULT_BURST	DMA_BURSTL_16DW
#define DMA_DEFAULT_PKDROP	DMA_PKT_DROP_DISABLE
#define DMA_DEFAULT_DESC_LEN	1

#define DMA_OWN		1
#define CPU_OWN		0
#define DESC_OWN_DMA	0x80000000
#define DESC_CPT_SET	0x40000000
#define DESC_SOP_SET	0x20000000

struct dma_ctrl;
struct dma_port;
struct dmax_chan {
	struct dma_ctrl *controller;
	struct dma_port *port; /* back pointer */
	int nr;			/* Channel number */
#define DMA_TX_CH		0x1
#define DMA_RX_CH		0x2
#define DEVICE_ALLOC_DESC	0x4 /* Client allocate descriptor itself */
#define CHAN_IN_USE		0x8 /* Channel is in use already */
#define DEVICE_CTRL_CHAN	0x10 /* Device handle all dma actions itself */
#define DMA_HW_DESC		0x20 /* CBM or other hardware descriptor */
	u32 flags; /* central way or channel based way */
	enum dma_chan_on_off onoff;
	int rst;
	int irq;	/*!< DMA channel IRQ number */
	void *data;
	int lpbk_en;
	int lpbk_ch_nr;
	int pden;
	int curr_desc;
	int prev_desc;
	dma_addr_t desc_phys;	/* Descriptor base physical address */
	u32 desc_base;
	int desc_len;
	enum dma_chan_txwgt txwgt;
	int pkt_size;
	int p2pcpy;
	int global_buffer_len; /* Used for peri_to_peri */
	struct mutex ch_lock;
	const char *device_id;
	spinlock_t irq_lock; /* sahred resource exclusive in irq context */
	void **opt; /* Dynamic allocate */
	buffer_alloc_t alloc;
	buffer_free_t  free;
	intr_handler_t intr_handler;
	u32  cis;  /* Record channel interrupt status for later usage */
	int  lnr;   /* Logical channel number */
	u32 nonarb_cnt;
	u32 arb_cnt;
	u32 hdrm_len;
	u32 boff_len;
	u32 coal_len;
	bool sw_poll;
	bool data_endian_en;
	u32 data_endian;
	bool desc_endian_en;
	u32 desc_endian;
	bool desc_configured;
	void *priv; /* Client specific info */
};

struct dma_port {
	struct dma_ctrl *controller; /* back pointer */
	int pid;
	const char *name;
	enum dma_endian rxendi;
	enum dma_endian txendi;
	enum dma_burst rxbl;
	enum dma_burst txbl;
	enum dma_port_txwgt txwgt;
	enum dma_pkt_drop pkt_drop;
	int flush_memcpy; /* change to flags */
	int chan_nrs; /* For the current port */
	struct dmax_chan *chans; /* Dynamic allocate */
	spinlock_t port_lock;
};

struct dma_ctrl {
	int cid;
	const char *name;
#define DMA_CTL_64BIT		0x1
#define DMA_FLCTL		0x2
#define DMA_FTOD		0x4
#define DMA_DESC_IN_SRAM	0x8
#define DMA_DRB			0x10
#define DMA_EN_BYTE_EN		0x20
	u32 flags;
	void __iomem *membase;
	u32 chained_irq;
	u32 irq_base;
	u32 port_nrs; /* For the current controller */
	u32 chans;
	u32 ver;
	struct dma_port *ports; /* Dynamic allocate */
	u32 arb_type;
	u32 labcnt; /* Look ahead buffer counter */
	u32 pollcnt;
	u32 orrc; /* Outstanding read count */
	spinlock_t ctrl_lock; /* DMA controller register execlusive */
	struct tasklet_struct dma_tasklet;
	DECLARE_BITMAP(dma_int_status, MAX_DMA_CHAN_PER_PORT);
	atomic_t dma_in_process;
	u32 burst_mask;
	u32 desc_size;
	struct device *dev;
	struct dentry *debugfs;
	u32 budget;
};
#endif /* LTQ_HDMA_H */
