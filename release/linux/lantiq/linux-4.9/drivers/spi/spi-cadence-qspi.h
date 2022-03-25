/*
 * Driver for Cadence QSPI Controller
 *
 * Copyright Altera Corporation (C) 2012-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/reset.h>
#ifndef __CADENCE_QSPI_H__
#define __CADENCE_QSPI_H__
#define CQSPI_MAX_TRANS				(10)
#define CQSPI_MAX_CHIP_SELECT			(16)
#define DC_MODE						0
#define STIG_READ_MODE				1
#define STIG_WRITE_MODE				2
#define STIG_WRITE_READ_MODE		3
#define IDC_READ_MODE				4
#define IDC_WRITE_MODE				5

#define QSPI_FLASH_TYPE_NOR		0
#define QSPI_FLASH_TYPE_NAND	1

#define QUAD_SIO			0
#define QUAD_DIO			1
#define QUAD_QIO			2

#define QUAD_LSB			4

#define MACRONIX_4PP_CMD		0x38
#define MACRONIX_WRSR_CMD		0x01

struct cqspi_flash_pdata {
	unsigned int page_size;
	unsigned int block_size;
	unsigned int flash_type;
	unsigned int rd_quad;
	unsigned int wr_quad;
	unsigned int read_delay;
	unsigned int tshsl_ns;
	unsigned int tsd2d_ns;
	unsigned int tchsh_ns;
	unsigned int tslch_ns;
};

struct cqspi_platform_data {
	unsigned int bus_num;
	unsigned int num_chipselect;
	unsigned int qspi_ahb_phy;
	unsigned int qspi_ahb_size;
	unsigned int qspi_ahb_mask;
	unsigned int master_ref_clk_hz;
	unsigned int ext_decoder;
	unsigned int fifo_depth;
	unsigned int enable_dma;
	unsigned int tx_dma_peri_id;
	unsigned int rx_dma_peri_id;
	struct cqspi_flash_pdata f_pdata[CQSPI_MAX_CHIP_SELECT];
};

struct struct_cqspi {
	struct work_struct work;
	struct workqueue_struct *workqueue;
	wait_queue_head_t waitqueue;
	struct list_head msg_queue;
	struct platform_device *pdev;
	struct clk *clk;
	struct clk *fpi_clk;
	struct reset_control *reset;
	/* lock protects queue and registers */
	spinlock_t lock;
	/* Virtual base address of the controller */
	void __iomem *iobase;
	/* QSPI AHB virtual address */
	void __iomem *qspi_ahb_virt;
	/* phys mem */
	struct resource *res;
	/* AHB phys mem */
	struct resource *res_ahb;
	/* Interrupt */
	int irq;
	/* Interrupt status */
	unsigned int irq_status;
	/* Current chip select */
	int current_cs;
	/* Is queue running */
	bool running;
	/* DMA support */
	struct dma_chan *txchan;
	struct dma_chan *rxchan;
	dma_addr_t dma_addr;
	int dma_done;
};

/* Kernel function hook */
#define CQSPI_WRITEL		__raw_writel
#define CQSPI_READL		__raw_readl
unsigned int cadence_qspi_init_timeout(const unsigned long timeout_in_ms);
unsigned int cadence_qspi_check_timeout(const unsigned long timeout);
#define debugline	// printk("[%s]%d caller address=0x%x\n", __func__, __LINE__, __builtin_return_address(0))
#endif /* __CADENCE_QSPI_H__ */
