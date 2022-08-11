/* Copyright (c) 2022, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LEGACY_IRQ_INTERNAL_H
#define __LEGACY_IRQ_INTERNAL_H

#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/seq_file.h>
#include <linux/moduleparam.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include "../qmi.h"
#include "../pci.h"
#include "legacyirq.h"

#define INTX_INT_STS0_REG_OFFSET        0x31B4
#define INTX_INT_STS1_REG_OFFSET        0x31B8
#define INTX_INT_STS2_REG_OFFSET        0x31BC
#define INTX_INT_STS3_REG_OFFSET        0x31C0
#define INTX_INT_STS4_REG_OFFSET        0x31C4

#define INTX_MAX_INTERRUPTS             160
#define LEGACY_INTX_EN_SET_BANK0        0x0318C
#define LEGACY_INTX_EN_CLR_BANK0        0x031A0
#define INTX_COMMON_REG_OFFSET          0x31CC

#define INTX_MAX_LEVEL                  8
#define INTX_MAX_INTERRUPTS_PER_LEVEL   32
#define IRQ_TO_REG_OFFSET(irq) ((irq / 32) * 4)
#define IRQ_BIT_IN_REG(irq) (1 << (irq % 32))

#define BAR                            ((void *)lvirq->regbase)
#define STAT_INC(statname, irq)        (lvirq->stats[statname][irq]++)

#define INTERRUPT_TIME_0                0
#define INTERRUPT_TIME_1                5
#define INTERRUPT_TIME_2                10
#define INTERRUPT_TIME_3                15
#define INTERRUPT_TIME_4                20
#define INTERRUPT_TIME_5                25
#define INTERRUPT_TIME_6                30
#define INTERRUPT_TIME_7                35

#define QCN9224_LEGACY_INTX_COMMON                        0x31CC
#define QCN9224_ENABLE_LEGACY_INTTERUPT_BIT               0x1

#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK0                 0x03178
#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK0_VAL             0xFFFFFFE0

#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK1                 0x0317C
#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK1_VAL             0xFFFFFFFF

#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK2                 0x03180
#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK2_VAL             0x000FFFFF

#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK3                 0x03184
#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK3_VAL             0x07FFFFFF

#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK4                 0x03188
#define PCIE_LOCALREG_LEGACY_INTX_EN_BNK4_VAL             0xE000


char intx_irqname[][350] = {
"INT_STATUS_WORD0",
"INT_STATUS_WORD1",
"INT_STATUS_WORD2",
"INT_STATUS_WORD3",
"INT_STATUS_WORD4",
"ce0",
"ce1",
"ce2",
"ce3",
"ce4",
"ce5",
"ce6",
"ce7",
"ce8",
"ce9",
"ce10",
"ce11",
"ce12",
"ce13",
"ce14",
"ce15",
"wbm2sw0_release",
"wbm2sw1_release",
"wbm2sw2_release",
"wbm2sw3_release",
"reo2sw1",
"reo2sw2",
"reo2sw3",
"reo2sw4",
"reo2sw5",
"reo2sw6",
"reo2sw0",
"wbm2sw4_release",
"wbm2sw0_release_2",
"wbm2sw1_release2",
"wbm2sw2_release2",
"wbm2sw3_release2",
"wbm2sw4_release2",
"wbm2sw5_release2",
"wbm2sw6_release2",
"wbm2sw5_release",
"wbm2sw6_release",
"reo2sw1_intr2",
"reo2sw2_intr2",
"reo2sw3_intr2",
"reo2sw4_intr2",
"reo2sw5_intr2",
"reo2sw6_intr2",
"reo2sw7_intr2",
"reo2sw8_intr2",
"reo2sw0_intr2",
"reo2sw7_intr",
"reo2sw8_intr",
"ppe2tcl1",
"reo2ppe",
"reo2ppe_intr2",
"txmon2sw_p0_dest0",
"txmon2sw_p0_dest1",
"rxmon2sw_p0_dest0",
"rxmon2sw_p0_dest1",
"txmon2sw_p1_intr0",
"txmon2sw_p1_intr1",
"rxmon2sw_p1_intr0",
"rxmon2sw_p1_intr1",
"ppe_release",
"sw_release",
"wbm2sw_link",
"reo_cmd",
"sw2reo1",
"sw2reo",
"reo2fw",
"reo_status",
"sw2tcl1",
"sw2tcl2",
"sw2tcl3",
"sw2tcl_credit",
"misc_1",
"misc_0",
"wbm_error_release",
"reo2fw_intr2",
"reo2status_intr2",
"sw2tcl4",
"sw2tcl5",
"sw2tcl_credit2",
"wbm2wbm_out_4",
"wbm2wbm_out_3",
"wbm2wbm_out_2",
"wbm2wbm_out_1",
"wbm2wbm_in_2",
"wbm2wbm_in_1",
"tqm2tqm_out_4",
"tqm2tqm_out_3",
"tqm2tqm_out_2",
"tqm2tqm_out_1",
"tqm2tqm_in_2",
"tqm2tqm_in_1",
"reo2rxdma_out_2",
"reo2rxdma_out_1",
"sw2wbm1",
"sw2reo5",
"sw2reo6",
"misc_2",
"misc_3",
"misc_4",
"misc_5",
"misc_6",
"misc_7",
"misc_8",
"sw2rxdma_0",
"sw2rxdma_1_src_ring",
"sw2rxdma_2_src_ring",
"rxdma2release_dst_ring0",
"rxdma2sw_dst_ring0",
"rxdma2reo_dst_ring0",
"rxdma2release_dst_ring1",
"rxdma2sw_dst_ring1",
"rxdma2reo_dst_ring1",
"sw2txmon_src_ring",
"sw2rxmon_src_ring",
"rxdma2reo_mlo_0_dst_ring0",
"rxdma2reo_mlo_1_dst_ring0",
"rxdma2reo_mlo_0_dst_ring1",
"rxdma2reo_mlo_1_dst_ring1",
"fw2rxdma_p0_src_ring",
"fw2rxdma_p0_statbuf_status",
"fw2rxdma_link_p0",
"rxdma2fw_p0_dst_ring0",
"rxdma2fw_p0_dst_ring1",
"fw2rxdma_p1_source_ring_interrupt",
"fw2rxdma_p1_statbuf_status",
"fw2rxdma_link_p1",
"rxdma2fw_p1_ring0",
"rxdma2fw_p1_ring1",
"reserved1",
"reserved2",
"reserved3",
"reserved4",
"reserved5",
"reserved6",
"reserved7",
"reserved8",
"mhi0",
"mhi1",
"mhi2",
"reserved9",
"reserved10",
"reserved11",
"reserved12",
"reserved13",
"reserved14",
"reserved15",
"reserved16",
"reserved17",
"reserved18",
"reserved19",
"reserved20",
"reserved21",
"reserved22",
"reserved23",
"reserved24",
};

enum intx_interrupt_status_bits {
	/* Word 0 */
	INT_STATUS_WORD0,
	INT_STATUS_WORD1,
	INT_STATUS_WORD2,
	INT_STATUS_WORD3,
	INT_STATUS_WORD4,
	ce0,
	ce1,
	ce2,
	ce3,
	ce4,
	ce5,
	ce6,
	ce7,
	ce8,
	ce9,
	ce10,
	ce11,
	ce12,
	ce13,
	ce14,
	ce15,
	wbm2sw0_release,
	wbm2sw1_release,
	wbm2sw2_release,
	wbm2sw3_release,
	reo2sw1,
	reo2sw2,
	reo2sw3,
	reo2sw4,
	reo2sw5,
	reo2sw6,
	reo2sw0,

	/* Word 1 */
	wbm2sw4_release,
	wbm2sw0_release_2,
	wbm2sw1_release2,
	wbm2sw2_release2,
	wbm2sw3_release2,
	wbm2sw4_release2,
	wbm2sw5_release2,
	wbm2sw6_release2,
	wbm2sw5_release,
	wbm2sw6_release,
	reo2sw1_intr2,
	reo2sw2_intr2,
	reo2sw3_intr2,
	reo2sw4_intr2,
	reo2sw5_intr2,
	reo2sw6_intr2,
	reo2sw7_intr2,
	reo2sw8_intr2,
	reo2sw0_intr2,
	reo2sw7_intr,
	reo2sw8_intr,
	ppe2tcl1,
	reo2ppe,
	reo2ppe_intr2,
	txmon2sw_p0_dest0,
	txmon2sw_p0_dest1,
	rxmon2sw_p0_dest0,
	rxmon2sw_p0_dest1,
	txmon2sw_p1_intr0,
	txmon2sw_p1_intr1,
	rxmon2sw_p1_intr0,
	rxmon2sw_p1_intr1,

	/* Word 2*/
	ppe_release,
	sw_release,
	wbm2sw_link,
	reo_cmd,
	sw2reo1,
	sw2reo,
	reo2fw,
	reo_status,
	sw2tcl1,
	sw2tcl2,
	sw2tcl3,
	sw2tcl_credit,
	misc_1,
	misc_0,
	wbm_error_release,
	reo2fw_intr2,
	reo2status_intr2,
	sw2tcl4,
	sw2tcl5,
	sw2tcl_credit2,
	wbm2wbm_out_4,
	wbm2wbm_out_3,
	wbm2wbm_out_2,
	wbm2wbm_out_1,
	wbm2wbm_in_2,
	wbm2wbm_in_1,
	tqm2tqm_out_4,
	tqm2tqm_out_3,
	tqm2tqm_out_2,
	tqm2tqm_out_1,
	tqm2tqm_in_2,
	tqm2tqm_in_1,

	/* Word 3 */
	reo2rxdma_out_2,
	reo2rxdma_out_1,
	sw2wbm1,
	sw2reo5,
	sw2reo6,
	misc_2,
	misc_3,
	misc_4,
	misc_5,
	misc_6,
	misc_7,
	misc_8,
	sw2rxdma_0,
	sw2rxdma_1_src_ring,
	sw2rxdma_2_src_ring,
	rxdma2release_dst_ring0,
	rxdma2sw_dst_ring0,
	rxdma2reo_dst_ring0,
	rxdma2release_dst_ring1,
	rxdma2sw_dst_ring1,
	rxdma2reo_dst_ring1,
	sw2txmon_src_ring,
	sw2rxmon_src_ring,
	rxdma2reo_mlo_0_dst_ring0,
	rxdma2reo_mlo_1_dst_ring0,
	rxdma2reo_mlo_0_dst_ring1,
	rxdma2reo_mlo_1_dst_ring1,
	fw2rxdma_p0_src_ring,
	fw2rxdma_p0_statbuf_status,
	fw2rxdma_link_p0,
	rxdma2fw_p0_dst_ring0,
	rxdma2fw_p0_dst_ring1,

	/* Word 4 */
	fw2rxdma_p1_source_ring_interrupt,
	fw2rxdma_p1_statbuf_status,
	fw2rxdma_link_p1,
	rxdma2fw_p1_ring0,
	rxdma2fw_p1_ring1,
	reserved1,
	reserved2,
	reserved3,
	reserved4,
	reserved5,
	reserved6,
	reserved7,
	reserved8,
	mhi0,
	mhi1,
	mhi2,
	reserved9,
	reserved10,
	reserved11,
	reserved12,
	reserved13,
	reserved14,
	reserved15,
	reserved16,
	reserved17,
	reserved18,
	reserved19,
	reserved20,
	reserved21,
	reserved22,
	reserved23,
	reserved24,
};

unsigned char intx_prioritymap[INTX_MAX_LEVEL][INTX_MAX_INTERRUPTS_PER_LEVEL] =  {
	{ /* Level 0- Highest Priority */
		mhi0,
		mhi1,
		mhi2,

	},
	{ /* Level 1 */
		ce2,
		ce3,
		ce5,
		ce1,
		ce4,
		ce7,
	},
	{ /* Level 2 */
		ce0,
		ce1,
		ce4,
		ce6,
		ce8,
		ce9,
		ce10,
		ce11,
		ce12,
		ce13,
		ce14,
		ce15,
	},
	{ /* Level 3 */
		reo2sw1,
		reo2sw2,
		reo2sw3,
		reo2sw4,
		reo2sw5,
		reo2sw0,
		reo2sw6,
		reo2sw7_intr,
		reo2sw8_intr,
		reo2sw1_intr2,
		reo2sw2_intr2,
		reo2sw3_intr2,
		reo2sw4_intr2,
		reo2sw5_intr2,
		reo2sw6_intr2,
		reo2sw7_intr2,
		reo2sw8_intr2,
		reo2sw0_intr2,
		reo_status,
	},
	{ /* Level 4 */
		wbm2sw0_release,
		wbm2sw1_release,
		wbm2sw2_release,
		wbm2sw3_release,
		wbm2sw4_release,
		wbm2sw5_release,
		wbm2sw6_release,
		wbm2sw0_release_2,
		wbm2sw1_release2,
		wbm2sw2_release2,
		wbm2sw3_release2,
		wbm2sw4_release2,
		wbm2sw5_release2,
		wbm2sw6_release2,
		wbm_error_release,
	},
	{ /* Level 5 */
		rxdma2reo_mlo_0_dst_ring0,
		rxdma2reo_mlo_1_dst_ring0,
		rxdma2reo_mlo_0_dst_ring1,
		rxdma2reo_mlo_1_dst_ring1,
		rxdma2release_dst_ring0,
		rxdma2sw_dst_ring0,
		rxdma2reo_dst_ring0,
		rxdma2release_dst_ring1,
		rxdma2sw_dst_ring1,
	},
	{ /* Level 6 */
	},
	{ /* Level 7 */
	},
};

enum intx_stat_type {
	enableirq,
	disableirq,
	registeredirq,
	unregisteredirq,
	raisedirq,
	pendingirq,
	pendingirqdelivered,
	maxirq,
	executiontime,
	schedlatency
};

enum intx_sched_latency_bucket {
	BUCKET_B0, /* 0 to 5 ms*/
	BUCKET_B1, /* 6 to 10 ms */
	BUCKET_B2, /* 11 to 15 ms */
	BUCKET_B3, /* 16 to 20 ms */
	BUCKET_B4, /* 21 to 25 ms */
	BUCKET_B5, /* 26 to 30 ms */
	BUCKET_B6, /* 31 to 35 ms */
	BUCKET_B7, /* > 35 ms */
	INTX_MAX_BUCKET,
};

enum intx_priority_level {
	LEVEL0, /* MHI Highest Priority */
	LEVEL1, /* CE */
	LEVEL2, /* DP Exception */
	LEVEL3, /* DP Rx */
	LEVEL4, /* Dp Tx Completion */
	LEVEL5, /* DP Tx */
	LEVEL6, /* Monitor */
	LEVEL7, /* Lowest Priority */
};

/* Legacy to Virutal IRQ data */
struct legacy2virtual_irqdata {
	struct list_head node;
	const char *name;
	struct irq_domain *domain;
	int qrtr_node_id;
	DECLARE_BITMAP(irq_enabled, INTX_MAX_INTERRUPTS);
	DECLARE_BITMAP(irq_pending, INTX_MAX_INTERRUPTS);
	DECLARE_BITMAP(irq_requested, INTX_MAX_INTERRUPTS);
	DECLARE_BITMAP(irq_scheduled, INTX_MAX_INTERRUPTS);
	struct platform_device *pdev;
	int pci_legacy_irq;     //linux irq for the actual PCI intx IRQ
	void *regbase;          //virtual BAR
	int stats[maxirq][INTX_MAX_INTERRUPTS];
	int schedlatency[INTX_MAX_INTERRUPTS][INTX_MAX_BUCKET];
	int executiontime[INTX_MAX_INTERRUPTS][INTX_MAX_BUCKET];
};

struct dentry *irq_root_dentry;
struct legacy2virtual_irqdata *lvirq_list[4];
static int lvirq_index;

#endif /* __LEGACY_IRQ_INTERNAL_H */
