/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2009~2013 Lei Chuanhua <chuanhua.lei@lantiq.com>
 */
/*!
  \file ltq_ssc.c
  \ingroup IFX_SSC
  \brief ssc bus driver source file
*/
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/seq_file.h>
#include <linux/klogging.h>
#ifdef CONFIG_SOC_GRX500
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#endif

#include <lantiq.h>
#include <lantiq_soc.h>
#include <lantiq_ssc.h>
#include "ltq_ssc.h"

/*
* !<WW: Due to no response from test image, this fix
* shall be categorized as experimental
* JIRA: UGW_SW-8578
* JIRA: UGW_SW-8090
*/
#define EXPERIMENTAL_FIX

#if 1
#undef LTQ_DMA_SUPPORT
#else
#define  LTQ_DMA_SUPPORT
/* !<WW: TODO as of 19-Dec-2017 Linux 4.9 dma0 support for xrx500 isn't there yet */
#include <lantiq_dma.h>
#endif
#define IFX_SSC_VER_MAJOR          2
#define IFX_SSC_VER_MID            5
#define IFX_SSC_VER_MINOR          2

#define IFX_SSC_SPLIT_BAUD_RATE   25000000

#ifdef CONFIG_LANTIQ_SPI_DEBUG
#define INLINE
enum {
	SSC_MSG_TX_FIFO = 0x00000001,
	SSC_MSG_TX_DMA = 0x00000002,
	SSC_MSG_RX_FIFO = 0x00000004,
	SSC_MSG_RX_DMA = 0x00000008,
	SSC_MSG_INT = 0x00000010,  /* Interrupt msg */
	SSC_MSG_CFG = 0x00000020,
	SSC_MSG_THREAD = 0x00000040,
	SSC_MSG_TASKLET = 0x00000080,
	SSC_MSG_DEBUG = 0x00000100,
	SSC_MSG_ERROR = 0x00000200,
	SSC_MSG_INIT = 0x00000400, /* Initialization msg */
	SSC_MSG_QUEUE = 0x00000800,
	SSC_MSG_LOCK = 0x00001000,
	SSC_MSG_CALLBACK = 0x00002000,
	SSC_MSG_DUPLEX = 0x00004000, /* duplex mode */
	SSC_MSG_ANY = 0xffffffff, /* anything */
};

static void ifx_ssc_debug(struct ifx_ssc_port *port, const char *fmt, ...);

#define IFX_SSC_PRINT(_port, _m, _fmt, args...) do {    \
	if ((_port)->ssc_debug & (_m))                  \
		ifx_ssc_debug((_port), (_fmt), ##args); \
} while (0)

#else
#define INLINE  inline
#define IFX_SSC_PRINT(_port, _m, _fmt, ...)
#endif /* CONFIG_LANTIQ_SPI_DEBUG */

#define SSC_KASSERT(exp, msg) do {    \
	if (unlikely(!(exp))) {       \
		printk msg;            \
		BUG();                 \
	}                              \
} while (0)
#define IFX_SSC_NAME             "ltq_ssc"
#if 1
static u32 global_val;
struct device_port_mapping {
	char dev_name[IFX_SSC_MAX_DEVNAME];
	int port;
};
static struct device_port_mapping port_map[IFX_SSC_MAX_MAPPING] = {
	{.dev_name = "ltq_sflash", .port = 0},
#ifdef CONFIG_SOC_GRX500
	{.dev_name = "ltq_nand", .port = 1},
	{.dev_name = "dect_drv", .port = 1},
	{.dev_name = "", .port = -1},
#else
	{.dev_name = "dect_drv", .port = 0},
	{.dev_name = "", .port = -1},
#endif
};
#endif
/**
 * This is the per-channel data structure containing pointers, flags
 * and variables for the port.  ifx_ssc_isp is allocated in ifx_ssc_init()
 * based on the chip version.
 */
static struct ifx_ssc_port *ifx_ssc_isp[IFX_SSC_MAX_PORT_NUM];

static struct proc_dir_entry *ifx_ssc_proc;

#ifdef CONFIG_SOC_GRX500
static u32 legacy_cs_port_num = -1;
#endif /* CONFIG_SOC_GRX500 */

static int find_port(char *dev_name, int *port)
{
	int i;
	*port = -1;

	if (!dev_name) {
		LOGF_KLOG_ERROR("dev_name is NULL \r\n");
		return ENODEV;
	}

	for (i = 0; i < IFX_SSC_MAX_MAPPING; i++) {
		if (port_map[i].port == -1)
			break;
		if (!strcmp(dev_name, port_map[i].dev_name)) {
			*port = port_map[i].port;
			break;
		}
	}
	if (*port == -1) {
		LOGF_KLOG_ERROR("device not found \r\n");
		return ENODEV;
	} else
		return 0;
}
/**
 * \fn static void  ifx_ssc_start_tasklet(struct ifx_ssc_port *port)
 * \brief Trigger different schedule procedures according to different context.
 * if caller is already in tasklet, it will be done in caller's tasklet
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void ifx_ssc_start_tasklet(struct ifx_ssc_port *port)
{
	struct tasklet_struct *ptasklet;

	/*
	 * Calls the internal process to serve the queue. This routine would
	 * immediately return in case the SSC hardware is currently used to
	 * serve another request.
	 */
	ptasklet = &port->ssc_txrxq;
	if (in_irq()) {  /* Hardware irq */
		IFX_SSC_PRINT(port, SSC_MSG_INT,
			"%s hardware irq schedule\n", __func__);
		tasklet_hi_schedule(ptasklet);
	} else if (in_softirq()) { /* Softirq or tasklet */
		IFX_SSC_PRINT(port, SSC_MSG_TASKLET,
			"%s softirq schedule\n", __func__);
		if (tasklet_trylock(ptasklet)) { /* tasklet_trylock for SMP */
			ptasklet->func(ptasklet->data);
			tasklet_unlock(ptasklet);
		} else {
			IFX_SSC_PRINT(port, SSC_MSG_TASKLET,
				"%s should never happen\n", __func__);
		}
	} else { /* Process  context */
		IFX_SSC_PRINT(port, SSC_MSG_THREAD,
			"%s process schedule\n", __func__);
		tasklet_schedule(ptasklet);
	}
}


#ifdef CONFIG_LANTIQ_SPI_DEBUG
/**
 * \fn static void ifx_ssc_debug(struct ifx_ssc_port *port,
 *                               const char *fmt, ...)
 * \brief Debug all kinds of level message
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   fmt     debug output format
 *
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void ifx_ssc_debug(struct ifx_ssc_port *port, const char *fmt, ...)
{
	static char buf[256] = { 0 }; /* XXX */
	va_list ap;
#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
	static const char *p = "tasklet";
#else
	static const char *p = "kthread";
#endif
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	LOGF_KLOG_INFO("%s %s: %s\n", p, port->name, buf);
}
#endif /* CONFIG_LANTIQ_SPI_DEBUG */

/**
 * \fn static INLINE void ifx_ssc_wait_finished(struct ifx_ssc_port *port)
 * \brief Wait for SPI bus becomes idle, FIFO empty doesn't means spi bus idle.
 *        to start another transaction, need to make sure bus is idle
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_wait_finished(struct ifx_ssc_port *port)
{
	while ((IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_BUSY))
		; /* Do nothing */

}

/**
 * \fn static INLINE unsigned int ifx_ssc_get_kernel_clk(struct ifx_ssc_port *port)
 * \brief Get SSC clock speed.
 * Returns the current operating speed of the SSC peripheral, depending on chip
 * specific bus speed and RMC setting in CLC register.
 *
 * \param   port     Pointer to structure #ifx_ssc_port
 * \return  >0       Peripheral speed in HZ
 * \return  0        Error
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE unsigned int
ifx_ssc_get_kernel_clk(struct ifx_ssc_port *port)
{
	/* This function assumes that the CLC register is set with the
	 * appropriate value for RMC.
	 */
	unsigned int rmc;

	rmc = IFX_SSC_GET_CLC_RMC(port);
	if (rmc == 0) {
		LOGF_KLOG_ERROR("%s rmc==0\n", __func__);
		return 0;
	}
	return clk_get_rate(port->fpiclk) / rmc;
}

/**
 * \fn static int ifx_ssc_stats_proc(struct seq_file *s, void *v)
 * \brief SSC proc file read.
 * This function creates the proc file output for the SSC driver statistics.
 *
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_stats_proc(struct seq_file *s, void *v)
{
	int t = 0;
	struct ifx_ssc_port *port;
	struct ifx_ssc_statistics *stats;
	ssc_device_t *dev;

	seq_puts(s,
		"Statistics for Lantiq Synchronous Serial Controller(SSC)\n\n");
	for (t = 0; t < IFX_SSC_MAX_PORT_NUM; t++) {
		port = ifx_ssc_isp[t];

		if (!port)
			continue;

		stats = &port->stats;

		seq_printf(s, "SSC%d\n", port->port_idx);
		seq_printf(s, "RX overflow errors %d\n", stats->rxOvErr);
		seq_printf(s, "RX underflow errors %d\n", stats->rxUnErr);
		seq_printf(s, "TX overflow errors %d\n", stats->txOvErr);
		seq_printf(s, "TX underflow errors %d\n", stats->txUnErr);
		seq_printf(s, "Abort errors %d\n", stats->abortErr);
		seq_printf(s, "Mode errors %d\n", stats->modeErr);
		seq_printf(s, "RX Bytes %llu\n", stats->rxBytes);
		seq_printf(s, "TX Bytes %llu\n", stats->txBytes);
		seq_printf(s, "TX FIFO transaction %llu\n", stats->txFifo);
		seq_printf(s, "TX DMA transaction %llu\n", stats->txDma);
		seq_printf(s, "TX DMA bytes %llu\n", stats->txDmaBytes);
		seq_printf(s, "RX FIFO transaction %llu\n", stats->rxFifo);
		seq_printf(s, "RX DMA transaction %llu\n", stats->rxDma);
		seq_printf(s, "RX DMA bytes %llu\n", stats->rxDmaBytes);
		seq_printf(s, "SSC bus status %s\n",
			port->ssc_cs_locked ? "locked" : "unlocked");
		seq_puts(s, "\n");
		/* Per device statistics */
		IFX_SSC_SEM_LOCK(port->dev_sem);
		TAILQ_FOREACH(dev, &port->ssc_devq, dev_entry) {
			seq_printf(s, "Device %s duplex %s:\n", dev->dev_name,
				(dev->duplex ==
				IFX_SSC_HALF_DUPLEX) ? "Half" : "Full");
			seq_printf(s, "Rx Bytes %llu\n", dev->stats.rxBytes);
			seq_printf(s, "Tx Bytes %llu\n", dev->stats.txBytes);
			seq_printf(s, "Context errors %d\n",
				dev->stats.context_err);
			seq_printf(s, "Duplicated qentry errors %d\n",
				dev->stats.dup_qentries);
			seq_printf(s, "Fragment errors %d\n",
				dev->stats.frag_err);
			seq_printf(s, "Handler errors %d\n",
				dev->stats.handler_err);
			seq_printf(s, "Duplex errors %d\n", dev->stats.dlx_err);
			seq_printf(s, "Enqueue %llu\n", dev->stats.enqueue);
			seq_printf(s, "Dequeue %llu\n", dev->stats.dequeue);
		}
		IFX_SSC_SEM_UNLOCK(port->dev_sem);
	}
	return 0;
}

static int ifx_ssc_stats_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ifx_ssc_stats_proc, NULL);
}

static const struct file_operations ifx_ssc_stats_proc_fops = {
	.open           = ifx_ssc_stats_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

/**
 * \fn static int ifx_ssc_reg_read_proc(struct seq_file *s, void *v)
 * \brief SSC proc file read.
 * This function creates the proc file output for SSC register dump.
 *
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_reg_read_proc(struct seq_file *s, void *v)
{
	int t = 0;
#define IFX_SSC_REG_MAX  20
	u32 stats[IFX_SSC_REG_MAX] = { 0 };
	struct ifx_ssc_port *port;

	seq_puts(s,
		"Register Dump for Lantiq Synchronous Serial Controller(SSC)\n\n");
	for (t = 0; t < IFX_SSC_MAX_PORT_NUM; t++) {
		port = ifx_ssc_isp[t];
		if (!port)
			continue;
		IFX_SSC_IRQ_LOCK(port);
		stats[0] = IFX_SSC_GET_CLC(port);
		stats[1] = IFX_SSC_GET_ID(port);
		stats[2] = IFX_SSC_GET_CON(port);
		stats[3] = IFX_SSC_GET_STATE(port);
		stats[4] = IFX_SSC_GET_TX_WORD(port);
		stats[5] = IFX_SSC_GET_FIFO_STATUS(port);
		stats[6] = IFX_SSC_GET_RX_FIFO_CTRL(port);
		stats[7] = IFX_SSC_GET_TX_FIFO_CTRL(port);
		stats[8] = IFX_SSC_GET_BR(port);
		stats[9] = IFX_SSC_GET_FRAMING_CON(port);
		stats[10] = IFX_SSC_GET_FRAMING_STATUS(port);
		stats[11] = IFX_SSC_GET_GPOCON(port);
		stats[12] = IFX_SSC_GET_GPOSTAT(port);
		stats[13] = IFX_SSC_GET_RXREQ(port);
		stats[14] = IFX_SSC_GET_RXCNT(port);
		stats[15] = IFX_SSC_GET_DMA_CON(port);
		stats[16] = IFX_SSC_GET_IRN_EN(port);
		stats[17] = IFX_SSC_GET_IRN_CR(port);
		stats[18] = IFX_SSC_GET_IRN_ICR(port);
		IFX_SSC_IRQ_UNLOCK(port);
		seq_printf(s, "SSC%d\n", port->port_idx);
		seq_printf(s, "IFX_SSC_CLC     0x%08x\n", stats[0]);
		seq_printf(s, "IFX_SSC_ID      0x%08x\n", stats[1]);
		seq_printf(s, "IFX_SSC_MCON    0x%08x\n", stats[2]);
		seq_printf(s, "IFX_SSC_STATE   0x%08x\n", stats[3]);
		seq_printf(s, "IFX_SSC_TB      0x%08x\n", stats[4]);
		seq_printf(s, "IFX_SSC_FSTAT   0x%08x\n", stats[5]);
		seq_printf(s, "IFX_SSC_RXFCON  0x%08x\n", stats[6]);
		seq_printf(s, "IFX_SSC_TXFCON  0x%08x\n", stats[7]);
		seq_printf(s, "IFX_SSC_BR      0x%08x\n", stats[8]);
		seq_printf(s, "IFX_SSC_SFCON   0x%08x\n", stats[9]);
		seq_printf(s, "IFX_SSC_SFSTAT  0x%08x\n", stats[10]);
		seq_printf(s, "IFX_SSC_GPOCON  0x%08x\n", stats[11]);
		seq_printf(s, "IFX_SSC_GPOSTAT 0x%08x\n", stats[12]);
		seq_printf(s, "IFX_SSC_RXREQ   0x%08x\n", stats[13]);
		seq_printf(s, "IFX_SSC_RXCNT   0x%08x\n", stats[14]);
		seq_printf(s, "IFX_SSC_DMACON  0x%08x\n", stats[15]);
		seq_printf(s, "IFX_SSC_IRN_EN  0x%08x\n", stats[16]);
		seq_printf(s, "IFX_SSC_IRN_CR  0x%08x\n", stats[17]);
		seq_printf(s, "IFX_SSC_IRN_ICR 0x%08x\n", stats[18]);
		seq_puts(s, "\n");
	}
	return 0;
#undef IFX_SSC_REG_MAX
}
static int ifx_ssc_reg_read_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ifx_ssc_reg_read_proc, NULL);
}

static const struct file_operations ifx_ssc_reg_proc_fops = {
	.open           = ifx_ssc_reg_read_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};
/**
 * \fn static void ifx_ssc_dma_setup(struct ifx_ssc_port *port, int dir, int enabled)
 * \brief Setup dma direction and enable/disable
 *
 * \param   port        Pointer to structure #ifx_ssc_port
 * \param   dir         DMA Direction, tx/rx, tx&rx
 * \param   enabled     DMA enable/disable
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void
ifx_ssc_dma_setup(struct ifx_ssc_port *port, int dir, int enabled)
{
	u32 reg = 0;

	IFX_SSC_IRQ_LOCK(port);
	reg = IFX_SSC_GET_DMA_CON(port);
	if (dir == IFX_SSC_DIR_RX) {	/* RX */
		if (enabled)
			reg |= IFX_SSC_DMACON_RXON;
		else
			reg &= ~IFX_SSC_DMACON_RXON;
	} else if (dir == IFX_SSC_DIR_TX) {
		if (enabled)
			reg |= IFX_SSC_DMACON_TXON;
		else
			reg &= ~IFX_SSC_DMACON_TXON;
	} else {
		if (enabled)
			reg |= (IFX_SSC_DMACON_TXON | IFX_SSC_DMACON_RXON);
		else
			reg &= ~(IFX_SSC_DMACON_TXON |
				 IFX_SSC_DMACON_RXON);
	}
	IFX_SSC_SET_DMA_CON(reg, port);
	IFX_SSC_IRQ_UNLOCK(port);
}

#ifdef LTQ_DMA_SUPPORT
/**
 * \fn static INLINE void ifx_ssc_dma_irq_finished(struct ifx_ssc_port *port)
 * \brief DMA<TX/RX> interrupt received, this function calls to reschedule or
 *        wake up sleep kernel thread
 *
 * \param   port        Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_dma_irq_finished(struct ifx_ssc_port *port)
{
	/*
	 * Reset the flag that we are waiting for the DMA to complete
	 * This flag should be reset before the following stuff, otherwise
	 * start_tasklet will stop
	 */
	atomic_set(&port->dma_wait_state, 0);
	smp_wmb();
#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
	ifx_ssc_start_tasklet(port);
#else
	IFX_SSC_WAKEUP_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT,
		port->event_flags);
#endif
}

/**
 * \fn static int ifx_ssc_dma_int_handler( struct dma_device_info* dma_dev,
 *                                         int status)
 * \brief Pseudo Interrupt handler for DMA.
 * This function processes DMA interrupts notified to the SSC device driver.
 * It is installed at the DMA core as interrupt handler for the SSC DMA device
 * and handles the following DMA interrupts:
 * - In case of a DMA receive interrupt the received data is passed to the
 *   upper layer.
 *
 * \param   dma_dev pointer to DMA device structure
 * \param   status  type of interrupt being notified (RCV_INT: DMA receive
 *                  interrupt, TX_BUF_FULL_INT: transmit buffer full interrupt,
 *                  TRANSMIT_CPT_INT: transmission complete interrupt)
 * \return  OK      In case of successful data reception from DMA
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_dma_int_handler(struct dma_device_info *dma_dev, int status)
{
	int i;
	struct ifx_ssc_port *port;
	u32 dma_burst_len;
	u32 dma_burst_len_mask;
	port = (struct ifx_ssc_port *) dma_dev->priv;

	IFX_SSC_PRINT(port, SSC_MSG_INT, "Interrupt status %d\n", status);
	switch (status) {
	case RCV_INT:
		IFX_SSC_PRINT(port, SSC_MSG_INT, "RCV_INT\n");
		dma_burst_len = (dma_dev->rx_burst_len << 2);
		dma_burst_len_mask = dma_burst_len - 1;
		ifx_ssc_dma_setup(port, IFX_SSC_DIR_RX, IFX_SSC_DMA_DISABLE);

		/*
		 * NB, we have to reset SPI DMA Rx channel every time, just it
		 * is not enough to clear the central DMA global dma status
		 * variable. Because we should clear the DMA channel interrupt
		 * status bit at the same time. However, if we don't reset
		 * DMA descriptor because upper layer application may not take
		 * the data yet. Therefore, DMA tasklet will continue to raise
		 * pseudo interrupt. In order to reach the above two goals, it
		 * is the most simple to close the channel. It will be openned
		 * again when some data need to receive.
		 */
		(dma_dev->rx_chan[dma_dev->current_rx_chan])->
			reset(dma_dev->rx_chan[dma_dev->current_rx_chan]);

		/* HW WAR, if packet length is indivisible by burst length,
		 * memory copy must be used */
		if ((port->actual_rx_len & dma_burst_len_mask)) {
			memcpy((char *) port->rxbuf_ptr, port->dma_rxbuf,
				port->actual_rx_len);
			/* Reset these for FIFO usage */
			port->rxbuf_ptr = NULL;
			port->actual_rx_len = 0;
		}
		ifx_ssc_dma_irq_finished(port);
		break;

	case TX_BUF_FULL_INT:
		for (i = 0; i < dma_dev->max_tx_chan_num; i++) {
			if ((dma_dev->tx_chan[i])->control == IFX_DMA_CH_ON) {
				dma_dev->tx_chan[i]->enable_irq
					(dma_dev->tx_chan[i]);
			}
		}
		IFX_SSC_PRINT(port, SSC_MSG_INT, "TX_BUF_FULL_INT\n");
		break;

	case TRANSMIT_CPT_INT:
		for (i = 0; i < dma_dev->max_tx_chan_num; i++)
			dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);

		IFX_SSC_PRINT(port, SSC_MSG_INT, "TRANSMIT_CPT_INT\n");

		/* 1. XXX, where is the best place because of HW limitation
		 * NB, all cases should wait because it just indicates DMA has
		 * finished transfered data to SSC FIFO, but the data could be
		 * still in SSC FIFO.
		 * 2. If FDX is used, only RCV_INT will schedule the proccess/
		 * tasklet. HDX device may use FDX mode for RX operation.
		 * However, for TX, only HDX can be used for HDX device.
		 */
		if (port->dma_is_in_half_duplex == 1) {
			ifx_ssc_wait_finished(port);
			ifx_ssc_dma_irq_finished(port);
		}
		break;

	default:
		LOGF_KLOG_ERROR("%s unknow interrupt %d\n", __func__,
			status);
		break;
	}
	return 0;
}

/**
 * \fn static u8* ifx_ssc_dma_buffer_alloc(int len, int* byte_offset,
 *                                         void** opt)
 * \brief Allocates buffer for SSC DMA.
 * This function is installed as DMA callback function to be called
 * when the DMA needs to allocate a new buffer.
 *
 * \param   len          Length of packet
 * \param   byte_offset Pointer to byte offset
 * \param   opt        unused
 * \return  NULL         In case of buffer allocation fails
 * \return  buffer       Pointer to allocated memory
 * \ingroup IFX_SSC_INTERNAL
 */
static u8 *ifx_ssc_dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
	return NULL;
}

/**
 * \fn static int ifx_ssc_dma_buffer_free(u8* dataptr,void* opt)
 * \brief Free DMA buffer.
 * This function frees a buffer previously allocated by the DMA.
 *
 * \param   dataptr Pointer to data buffer
 * \param   opt     unused
 * \return  0        OK
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_dma_buffer_free(u8 *dataptr, void *opt)
{
	/* SPI will be in charge of memory free if necessary */
	return 0;
}

/**
 * \fn static int ifx_ssc_init_dma_device(int line, struct dma_device_info *dma_dev)
 * \brief InitializeSC DMA device.
 * This function initializes the passed DMA device structure for usage as
 * SSC DMA device.
 *
 * \param   line     SSC device (0)
 * \param   dma_dev Pointer to dma device structure to be initialized
 * \return  0        OK
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_init_dma_device(int line, struct dma_device_info *dma_dev)
{
	int i;

	dma_dev->priv = ifx_ssc_isp[line];
	dma_dev->num_tx_chan = DEFAULT_SSC_TX_CHANNEL_NUM;
	dma_dev->num_rx_chan = DEFAULT_SSC_RX_CHANNEL_NUM;
	dma_dev->tx_burst_len = DEFAULT_SSC_TX_BURST_LEN;
	dma_dev->rx_burst_len = DEFAULT_SSC_RX_BURST_LEN;
	dma_dev->port_tx_weight = 1;
	dma_dev->tx_endianness_mode = IFX_DMA_ENDIAN_TYPE0;
	dma_dev->rx_endianness_mode = IFX_DMA_ENDIAN_TYPE0;
	dma_dev->port_packet_drop_enable = 0;
	for (i = 0; i < dma_dev->num_tx_chan; i++) {
		dma_dev->tx_chan[i]->desc_len =
			DEFAULT_SSC_TX_CHANNEL_DESCR_NUM;
		dma_dev->tx_chan[i]->control = IFX_DMA_CH_ON;
		dma_dev->tx_chan[i]->packet_size = DEFAULT_SSC_FRAGMENT_SIZE;

	}
	for (i = 0; i < dma_dev->num_rx_chan; i++) {
		dma_dev->rx_chan[i]->desc_len =
			DEFAULT_SSC_RX_CHANNEL_DESCR_NUM;
		dma_dev->rx_chan[i]->packet_size = DEFAULT_SSC_FRAGMENT_SIZE;
		dma_dev->rx_chan[i]->control = IFX_DMA_CH_ON;
		dma_dev->rx_chan[i]->byte_offset = 0;
	}
	dma_dev->current_tx_chan = 0;
	dma_dev->current_rx_chan = 0;
	/*
	 * set DMA handler functions for rx-interrupts,
	 * buffer allocation and release
	 */
	dma_dev->intr_handler = ifx_ssc_dma_int_handler;
	dma_dev->buffer_alloc = ifx_ssc_dma_buffer_alloc;
	dma_dev->buffer_free = ifx_ssc_dma_buffer_free;
	dma_device_register(dma_dev);
	return 0;
}
#endif
/**
 * \fn static INLINE int ifx_ssc_rxtx_mode_set(struct ifx_ssc_port *port,
 *                                             unsigned int val)
 * \brief Rx/Tx mode set.
 * Set the transmission mode while SSC is idle
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   val     Rx/Tx mode
 * \return  0       OK
 * \return  -EINVAL Invalid parameters supplied
 * \return  -EBUSY  Transmission or reception ongoing
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE int
ifx_ssc_rxtx_mode_set(struct ifx_ssc_port *port, unsigned int val)
{
	u32 reg;

	if (!(port) || (val & ~(IFX_SSC_MODE_MASK)))
		return -EINVAL;

	/* check BUSY and RXCNT */
	if ((IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_BUSY)) {
		LOGF_KLOG_ERROR("%s state busy\n", __func__);
		return -EBUSY;
	}
	if (IFX_SSC_RX_TO_RECEIVED(port)) {
		LOGF_KLOG_ERROR("%s rx todo busy\n", __func__);
		return -EBUSY;
	}
	IFX_SSC_IRQ_LOCK(port);
	reg = IFX_SSC_GET_CON(port);
	reg &= ~(IFX_SSC_CON_RX_OFF | IFX_SSC_CON_TX_OFF);
	reg |= val;
	IFX_SSC_SET_CON(reg, port);
	port->opts.modeRxTx = val;
	IFX_SSC_IRQ_UNLOCK(port);
	return 0;
}

/**
 * \fn static int ifx_ssc_sethwopts(struct ifx_ssc_port *port)
 * \brief SSC set hardware options.
 * This routine intializes the SSC appropriately depending on slave/master and
 * full-/half-duplex mode. It assumes that the SSC is disabled and the fifo's
 * and buffers are flushed later on.
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  0       OK
 * \return  -EINVAL Invalid hardware options supplied
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_sethwopts(struct ifx_ssc_port *port)
{
	unsigned long bits;
	u32 reg;
	struct ifx_ssc_hwopts *opts = &port->opts;

	/* sanity checks */
	if ((opts->dataWidth < IFX_SSC_MIN_DATA_WIDTH) ||
	    (opts->dataWidth > IFX_SSC_MAX_DATA_WIDTH)) {
		LOGF_KLOG_ERROR("%s: sanity check failed\n", __func__);
		return -EINVAL;
	}
	bits = IFX_SSC_ENCODE_DATA_WIDTH(opts->dataWidth);
	/* TXB+2, TXB+3 trigger FPI Addr */
	bits |= IFX_SSC_CON_ENABLE_BYTE_VALID;

	if (opts->rxOvErrDetect)
		bits |= IFX_SSC_CON_RX_OFL_CHECK;
	if (opts->rxUndErrDetect)
		bits |= IFX_SSC_CON_RX_UFL_CHECK;
	if (opts->txOvErrDetect)
		bits |= IFX_SSC_CON_TX_OFL_CHECK;
	if (opts->txUndErrDetect)
		bits |= IFX_SSC_CON_TX_UFL_CHECK;
	if (opts->loopBack)
		bits |= IFX_SSC_CON_LOOPBACK_MODE;
	if (opts->echoMode)
		bits |= IFX_SSC_CON_ECHO_MODE_ON;
	if (opts->headingControl)
		bits |= IFX_SSC_CON_MSB_FIRST;
	if (opts->clockPhase)
		bits |= IFX_SSC_CON_PH;
	if (opts->clockPolarity)
		bits |= IFX_SSC_CON_PO;
	switch (opts->modeRxTx) {
	case IFX_SSC_MODE_TX:
		bits |= IFX_SSC_CON_RX_OFF;
		break;
	case IFX_SSC_MODE_RX:
		bits |= IFX_SSC_CON_TX_OFF;
		break;
	}
	IFX_SSC_IRQ_LOCK(port);
	IFX_SSC_SET_CON(bits, port);

	reg = (port->opts.gpoCs << IFX_SSC_GPOCON_ISCSB0_POS) |
		(port->opts.gpoInv << IFX_SSC_GPOCON_INVOUT0_POS);

	IFX_SSC_SET_GPOCON(reg, port);

	reg = port->opts.gpoCs << IFX_SSC_WHBGPOSTAT_SETOUT0_POS;

	IFX_SSC_SET_FGPO(reg, port);

	if (opts->masterSelect)
		reg = IFX_SSC_WHBSTATE_MASTER_MODE;
	else
		reg = IFX_SSC_WHBSTATE_SLAVE_MODE;
	IFX_SSC_SET_WHBSTATE(reg, port);
	IFX_SSC_IRQ_UNLOCK(port);
	port->tx_fifo_size_words = IFX_SSC_TX_FIFO_SIZE(port);
	port->tx_fifo_size_bytes = port->tx_fifo_size_words << 2;
	port->rx_fifo_size_words = IFX_SSC_RX_FIFO_SIZE(port);
	port->rx_fifo_size_bytes = port->rx_fifo_size_words << 2;

	return 0;
}

/**
 * \fn int ifx_ssc_cs_low(u32 pin)
 * \brief Chip select enable.
 * This function sets the given chip select for SSC0 to low.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_ssc_cs_low(u32 pin)
{
	int ret;
	u32 reg;


	struct ifx_ssc_port *port = ifx_ssc_isp[0]; /* XXX */

	#ifdef CONFIG_SOC_GRX500
	if ( -1  != legacy_cs_port_num &&
		NULL != ifx_ssc_isp[legacy_cs_port_num] )
		port  = ifx_ssc_isp[legacy_cs_port_num];
	#endif /* CONFIG_SOC_GRX500 */

	if (pin > IFX_SSC_MAX_GPO_OUT)
		ret = -EINVAL;
	else {
		reg = (1 << (pin + IFX_SSC_WHBGPOSTAT_CLROUT0_POS));
		IFX_SSC_IRQ_LOCK(port);
		IFX_SSC_SET_FGPO(reg, port);
		IFX_SSC_IRQ_UNLOCK(port);
		smp_wmb();
		ret = 0;
	}
	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_low);

/**
 * \fn int ifx_ssc_cs_high(u32 pin)
 * \brief Chip select disable.
 * This function sets the given chip select for SSC0 to high.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_ssc_cs_high(u32 pin)
{
	int ret;
	u32 reg;

	struct ifx_ssc_port *port = ifx_ssc_isp[0]; /* XXX */

	#ifdef CONFIG_SOC_GRX500
	if ( -1  != legacy_cs_port_num &&
		NULL != ifx_ssc_isp[legacy_cs_port_num] )
		port  = ifx_ssc_isp[legacy_cs_port_num];
	#endif /* CONFIG_SOC_GRX500 */

	if (pin > IFX_SSC_MAX_GPO_OUT)
		ret = -EINVAL;
	else {
		/* XXX, at least half of cycle of baudrate delay is needed */
		udelay(1);
		reg = 1 << (pin + IFX_SSC_WHBGPOSTAT_SETOUT0_POS);
		IFX_SSC_IRQ_LOCK(port);
		IFX_SSC_SET_FGPO(reg, port);
		IFX_SSC_IRQ_UNLOCK(port);
		smp_wmb();
		ret = 0;
	}
	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_high);

/**
 * \fn int ifx_ssc_cs_low_port0(u32 pin)
 * \brief Chip select enable.
 * This function sets the given chip select for SSC0 to low.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_ssc_cs_low_port0(u32 pin)
{
	int ret;
	u32 reg;

	struct ifx_ssc_port *port = ifx_ssc_isp[0]; /* XXX */
	if (pin > IFX_SSC_MAX_GPO_OUT)
		ret = -EINVAL;
	else {
		reg = (1 << (pin + IFX_SSC_WHBGPOSTAT_CLROUT0_POS));
		IFX_SSC_IRQ_LOCK(port);
		IFX_SSC_SET_FGPO(reg, port);
		IFX_SSC_IRQ_UNLOCK(port);
		smp_wmb();
		ret = 0;
	}
	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_low_port0);

/**
 * \fn int ifx_ssc_cs_high_port0(u32 pin)
 * \brief Chip select disable.
 * This function sets the given chip select for SSC0 to high.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_ssc_cs_high_port0(u32 pin)
{
	int ret;
	u32 reg;

	struct ifx_ssc_port *port = ifx_ssc_isp[0]; /* XXX */
	if (pin > IFX_SSC_MAX_GPO_OUT)
		ret = -EINVAL;
	else {
		/* XXX, at least half of cycle of baudrate delay is needed */
		udelay(1);
		reg = 1 << (pin + IFX_SSC_WHBGPOSTAT_SETOUT0_POS);
		IFX_SSC_IRQ_LOCK(port);
		IFX_SSC_SET_FGPO(reg, port);
		IFX_SSC_IRQ_UNLOCK(port);
		smp_wmb();
		ret = 0;
	}
	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_high_port0);


/**
 * \fn int ifx_ssc_cs_low_port1(u32 pin)
 * \brief Chip select enable.
 * This function sets the given chip select for SSC0 to low.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_ssc_cs_low_port1(u32 pin)
{
	int ret;
	u32 reg;
	struct ifx_ssc_port *port = ifx_ssc_isp[1]; /* XXX */
	#if 0
	int port_num = (pin + 1) / IFX_SSC_MAX_GPO_OUT;
	pin = (pin > IFX_SSC_MAX_GPO_OUT) ? (pin - IFX_SSC_MAX_GPO_OUT-1) : pin;
	struct ifx_ssc_port *port = ifx_ssc_isp[port_num]; /* XXX */
	#endif

	if (pin > IFX_SSC_MAX_GPO_OUT)
		ret = -EINVAL;
	else {
		reg = (1 << (pin + IFX_SSC_WHBGPOSTAT_CLROUT0_POS));
		IFX_SSC_IRQ_LOCK(port);
		IFX_SSC_SET_FGPO(reg, port);
		IFX_SSC_IRQ_UNLOCK(port);
		smp_wmb();
		ret = 0;
	}
	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_low_port1);

/**
 * \fn int ifx_ssc_cs_high_port1(u32 pin)
 * \brief Chip select disable.
 * This function sets the given chip select for SSC0 to high.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_ssc_cs_high_port1(u32 pin)
{
	int ret;
	u32 reg;
	struct ifx_ssc_port *port = ifx_ssc_isp[1]; /* XXX */
	#if 0
	int port_num = (pin + 1) / IFX_SSC_MAX_GPO_OUT;
	pin = (pin > IFX_SSC_MAX_GPO_OUT) ? (pin - IFX_SSC_MAX_GPO_OUT-1) : pin;
	struct ifx_ssc_port *port = ifx_ssc_isp[port_num]; /* XXX */
	#endif
	if ((pin) > IFX_SSC_MAX_GPO_OUT)
		ret = -EINVAL;
	else {
		/* XXX, at least half of cycle of baudrate delay is needed */
		udelay(1);
		reg = 1 << (pin + IFX_SSC_WHBGPOSTAT_SETOUT0_POS);
		IFX_SSC_IRQ_LOCK(port);
		IFX_SSC_SET_FGPO(reg, port);
		IFX_SSC_IRQ_UNLOCK(port);
		smp_wmb();
		ret = 0;
	}
	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_high_port1);

/**
 * \fn static int ifx_ssc_set_baudrate (struct ifx_ssc_port *port,
 *                                      unsigned int baudrate)
 * \brief SSC set baudrate.
 * Sets the baudrate of the corresponding port according to the passed
 * rate after reading out the current module speed.
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   baudrate  Desired baudrate
 * \return  0       OK
 * \return  -EINVAL Could not retrieve system clock or invalid baudrate setting
 * \ingroup IFX_SSC_INTERNAL
 */

#define IFX_GPHY1_CFG                            0x0040
#define IFX_SSC_HIGH_BAUD_DELAY_MASK             0x00000060
#define IFX_SSC_HIGH_BAUD_DELAY_TWO_HALF_CLOCK   0x00000020
#define IFX_SSC_HIGH_BAUD_DELAY_THREE_CLOCK      0x00000040
#define IFX_SSC_HIGH_BAUD_DELAY_THREE_HALF_CLOCK 0x00000060

static int
ifx_ssc_set_baudrate(struct ifx_ssc_port *port, unsigned int baudrate)
{
	unsigned int ifx_ssc_clock;
	unsigned int br;
	int enabled;

	ifx_ssc_clock = ifx_ssc_get_kernel_clk(port);
	if (ifx_ssc_clock == 0)
		return -EINVAL;

	/* Baud rate or kernel clock change needs to calculate the new
	 * baud rate */
	if ((port->prev_baudrate == baudrate)
	    && (port->prev_ssc_clk == ifx_ssc_clock))
		return 0;
#if 0
	if (of_machine_is_compatible("lantiq,vr9")) {
		if (ltq_get_soc_type() == SOC_TYPE_VR9_2) {
			/* VR9 A21 high baudrate support */
			u32 reg;

			IFX_SSC_IRQ_LOCK(port);
			if (baudrate > IFX_SSC_SPLIT_BAUD_RATE) {
				reg = ltq_cgu_r32(IFX_GPHY1_CFG);
				reg &= ~IFX_SSC_HIGH_BAUD_DELAY_MASK;
				reg |= IFX_SSC_HIGH_BAUD_DELAY_THREE_CLOCK;
				ltq_cgu_w32(reg, IFX_GPHY1_CFG);
			} else {
				reg = ltq_cgu_r32(IFX_GPHY1_CFG);
				reg &= ~IFX_SSC_HIGH_BAUD_DELAY_MASK;
				ltq_cgu_w32(reg, IFX_GPHY1_CFG);
			}
			IFX_SSC_IRQ_UNLOCK(port);
		}
	}
#endif
	/* Compute divider */
	br = (((ifx_ssc_clock >> 1) + baudrate / 2) / baudrate) - 1;
	if (br > 0xffff || ((br == 0) &&
		(IFX_SSC_GET_STATE(port) &
		IFX_SSC_STATE_MASTER))) {
		LOGF_KLOG_ERROR("%s: illegal baudrate %u br %d\n",
			__func__, baudrate, br);
		return -EINVAL;
	}
	IFX_SSC_IRQ_LOCK(port);
	/* Have to disable theSC to set the baudrate */
	enabled =
		(IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_ENABLED) ? 1 : 0;

	IFX_SSC_CONFIG_MODE(port);

	IFX_SSC_SET_BR(br, port);
	smp_wmb();
	if (of_machine_is_compatible("lantiq,ar10")
		|| of_machine_is_compatible("lantiq,grx390")
		|| of_machine_is_compatible("lantiq,grx500")) {
		u32 reg;

		if (baudrate > IFX_SSC_SPLIT_BAUD_RATE) {
			reg = IFX_SSC_GET_CON(port);
			reg &= ~IFX_SSC_CON_CLK_DELAY;
			reg |= SM(IFX_SSC_CON_CLK_DELAY_DEFAULT,
				  IFX_SSC_CON_CLK_DELAY);
			IFX_SSC_SET_CON(reg, port);
		} else {
			reg = IFX_SSC_GET_CON(port);
			reg &= ~IFX_SSC_CON_CLK_DELAY;
			IFX_SSC_SET_CON(reg, port);
		}
	}
	if (enabled)
		IFX_SSC_RUN_MODE(port);

	IFX_SSC_IRQ_UNLOCK(port);
	port->prev_baudrate = baudrate;
	port->baudrate = baudrate;
	port->prev_ssc_clk = ifx_ssc_clock;
	return 0;
}

/**
 * \fn static int ifx_ssc_hwinit (struct ifx_ssc_port *port)
 * \brief SSC hardware initialization.
 * Initializes the SSC port hardware with the desired baudrate and transmission
 * options.
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  0       OK
 * \return  -EINVAL Error during initialization
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_hwinit(struct ifx_ssc_port *port)
{
	u32 reg;
	int enabled;

	/* have to disable the SSC */
	enabled =
		(IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_ENABLED) ? 1 : 0;

	IFX_SSC_CONFIG_MODE(port);
	if (ifx_ssc_sethwopts(port) < 0) {
		LOGF_KLOG_ERROR("%s: setting the hardware options failed\n", __func__);
		return -EINVAL;
	}

	if (ifx_ssc_set_baudrate(port, port->baudrate) < 0) {
		LOGF_KLOG_ERROR("%s: setting the baud rate failed\n", __func__);
		return -EINVAL;
	}
	IFX_SSC_IRQ_LOCK(port);
	/*
	 * With this setting it is assured, that if the DMA stalls (RX cannot
	 * write data and TX cannot read data) at a certain point in time,
	 * only one additional TX burst (2 words) + one word from the burst
	 * that is just processing at maximum will be forwarded by the SSC TX
	 * FIFO towards the line side. In this case I assume that at maximum
	 * 4 words should be available within the RX FIFO (2 words from ongoing
	 * TX burst + 2 additional words from the last TX burst requested
	 * before the DMA stall (requested as the TX FIFO level reached 1).
	 * In worst case timing may be 6 words remain within the RX FIFO,
	.* but at least we should not face any overflow.
	 */

	/* TX FIFO filling level, half of FIFO size */
	reg = SM(1 /*(port->tx_fifo_size_words >> 1) */ ,
		 IFX_SSC_XFCON_ITL) |
		IFX_SSC_XFCON_FIFO_FLUSH | IFX_SSC_XFCON_FIFO_ENABLE;
	IFX_SSC_TX_FIFO_CTRL(reg, port);

	/* RX FIFO filling level, half of FIFO size */
	if (port->dma_support) {
		/* Change to DMA RX trigger level to 2 to trigger DMA earlier
		 * half FIFO size will be stuck */
		reg = SM(2, IFX_SSC_XFCON_ITL);
	} else
		reg = SM((port->rx_fifo_size_words >> 1), IFX_SSC_XFCON_ITL);

	reg |= IFX_SSC_XFCON_FIFO_FLUSH | IFX_SSC_XFCON_FIFO_ENABLE;
	IFX_SSC_RX_FIFO_CTRL(reg, port);
	if (enabled)
		IFX_SSC_RUN_MODE(port);

	IFX_SSC_IRQ_UNLOCK(port);
	return 0;
}

/**
 * \fn static int ifx_ssc_start_rx_last_word(struct ifx_ssc_port *port,
 *                                           char *buf, size_t size)
 * \brief Called to transmit last word <1 ~ 4> bytes for full duplex operation.
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   buf        Pointer to store the received data packet
 * \param   size       Amount of bytes to receive
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void
ifx_ssc_start_rx_last_word(struct ifx_ssc_port *port, char *buf, size_t size)
{
	int i;
	char *p;
	int rx_cnt = size;
	ifx_ssc_rxd_t rxd_data;

	/* Last complete word or incomplete word */
	rxd_data.rxd_word = IFX_SSC_GET_RX_WORD(port);
	for (i = 4 - rx_cnt, p = (char *) buf; i < 4; i++, p++)
		*(u8 *) p = rxd_data.rxd_byte.byte[i];
}

/**
 * \fn static int ifx_ssc_start_tx_last_word(struct ifx_ssc_port *port,
 *                                           char *buf, size_t size)
 * \brief Called to transmit last word <1 ~ 4> bytes for full duplex operation.
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   buf        Pointer to the data packet to transmit
 * \param   size       Amount of bytes to transmit
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void
ifx_ssc_start_tx_last_word(struct ifx_ssc_port *port, char *buf, size_t size)
{
	switch (size) {
	case 1:
		IFX_SSC_TX_BYTE(*(u8 *) buf, port);
		break;
	case 2:
		IFX_SSC_TX_HALFWORD(*(u16 *) buf, port);
		break;
	case 3:
		IFX_SSC_TX_HALFWORD(*(u16 *) buf, port);
		IFX_SSC_TX_BYTE(*(u8 *) (buf + 2), port);
		break;
	case 4:
		IFX_SSC_TX_WORD(*(u32 *) buf, port);
		break;
	default:
		break;
	}
}

/**
 * \fn static void ifx_ssc_start_txfifo(struct ifx_ssc_port *port)
 * \brief Start FIFO data transmision.
 * This function copies remaining data in the transmit buffer into the FIFO
 *
 * \param   port     Pointer to structure #ifx_ssc_port
 * \return  number of bytes transmitted
 * Description:
 * If txsize is not equal to zero, ssc driver will generate dummy data according
 * to different cases.
 * If txsize is equal to zero, just send dummy data whose length is equal to
 * rxsize for clock generation.
 * \ingroup IFX_SSC_INTERNAL
 */

static int
ifx_ssc_start_txfifo(struct ifx_ssc_port *port, char *txbuf, size_t txsize)
{
	int i;
	char *p;
	u32 eff_bytes, eff_word;
	u32 tx_cnt;
	u32 *ptxbuf;
	int total = txsize;
	ifx_ssc_dma_setup(port, IFX_SSC_DIR_TX, IFX_SSC_DMA_DISABLE);
	IFX_SSC_IRQ_LOCK(port);
	while (1) {
		/* Wait for TX FIFO empty */
		while (IFX_SSC_TX_FIFO_FILL_LEVEL(port) != 0)
			;

		if (total <= 4) {
			eff_bytes = total;
			p = (char *) txbuf;
			ifx_ssc_start_tx_last_word(port, p, eff_bytes);
		} else if (total > port->tx_fifo_size_bytes) {
			eff_bytes = port->tx_fifo_size_bytes;
			eff_word = port->tx_fifo_size_words;
			for (i = 0, ptxbuf = (u32 *) txbuf; i < eff_word;
			     i++, ptxbuf++) {
				IFX_SSC_TX_WORD(*ptxbuf, port);
			}
		} else { /* 5 ~ 32 bytes */
			eff_bytes = total;
			eff_word = (eff_bytes >> 2) + ((eff_bytes & 0x3) >
				0 ? 1 : 0);

			/* Transmit the complete word */
			for (i = 0, ptxbuf = (u32 *) txbuf;
			     i < eff_word - 1; i++, ptxbuf++) {
				IFX_SSC_TX_WORD(*ptxbuf, port);
			}

			tx_cnt = (eff_bytes & 0x3);

			/* Still one complete word */
			if (tx_cnt == 0)
				tx_cnt = 4;

			ifx_ssc_start_tx_last_word(port, (char *) ptxbuf,
				tx_cnt);
		}
		txbuf += eff_bytes;
		total -= eff_bytes;
		if (total == 0)
			break;
	}
	IFX_SSC_IRQ_UNLOCK(port);
	return txsize;
}

/**
 * \fn static void ifx_ssc_start_rxfifo(struct ifx_ssc_port *port)
 * \brief Start FIFO data reception.
 * This function processes received data. It will read data from the FIFO
 *
 * \param   port     Pointer to structure #ifx_ssc_port
 * \return  number of bytes received
 * Description:
 * In Tx/Rx mode, to void memory copy, where rx data starts must be determined
 * several special cases
 * 1) If txsize is divisable by 4, all tx data will be skipped.
 * 2) If txsize is not divisable by 4,including less than 4 bytes.
 * The remaining 1~3 bytes have to do swap.
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_start_rxfifo(struct ifx_ssc_port *port, char *rxbuf, size_t rxsize)
{
	int i;
	int rx_cnt;
	u32 eff_word;
	u32 eff_bytes;
	u32 *prxbuf;
	int total = rxsize;

	ifx_ssc_dma_setup(port, IFX_SSC_DIR_RX, IFX_SSC_DMA_DISABLE);
	IFX_SSC_IRQ_LOCK(port);
	while (1) {
		/* In this case, no need to check last complete or
		 * incomplete word */
		if (total >= port->rx_fifo_size_bytes) {
			eff_bytes = port->rx_fifo_size_bytes;
			eff_word = port->rx_fifo_size_words;

			/* Configure RX request number to start receiving */
			IFX_SSC_SET_RXREQ(eff_bytes, port);

			while (IFX_SSC_RX_FIFO_FILL_LEVEL(port) != eff_word)
				;

			for (i = 0, prxbuf = (u32 *) rxbuf; i < eff_word;
			     i++, prxbuf++) {
				*prxbuf = IFX_SSC_GET_RX_WORD(port);
			}
		} else {
			eff_bytes = total;
			eff_word = (eff_bytes >> 2) + ((eff_bytes & 0x3) >
				0 ? 1 : 0);

			IFX_SSC_SET_RXREQ(eff_bytes, port);

			while (IFX_SSC_RX_FIFO_FILL_LEVEL(port) != eff_word)
				;

			/* Receive complete word */
			for (i = 0, prxbuf = (u32 *) rxbuf;
			     i < eff_word - 1; i++, prxbuf++) {
				*prxbuf = IFX_SSC_GET_RX_WORD(port);
			}

			/* SPI Master know how many bytes to be received */
			rx_cnt = total - ((eff_word - 1) << 2);

			ifx_ssc_start_rx_last_word(port, (char *) prxbuf,
				rx_cnt);
		}
		total -= eff_bytes;
		rxbuf += eff_bytes;
		if (total == 0)
			break;
	}
	IFX_SSC_IRQ_UNLOCK(port);
	return rxsize;
}

/**
 * \fn static int ifx_ssc_txrx_fifo_fdx(struct ifx_ssc_port *port, char *txbuf,
 *                                      u32 txsize, char *rxbuf, u32 rxsize)
 * \brief Called to transmit/receive to/from SSC in one step using FIFO mode.
 *
 * \param   port      Pointer to structure #ifx_ssc_port
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize    Amount of Bytes to transmit
 * \param   rxbuf     Pointer to store the received data packet
 * \param   rxsize    Amount of Bytes to receive.
 * \return  >= 0      Number of bytes received (if rxbuf != 0) or transmitted
 * \return  < 0       error number
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_txrx_fifo_fdx(struct ifx_ssc_port *port, char *txbuf, u32 txsize,
		      char *rxbuf, u32 rxsize)
{
	int i;
	int tx_fifo_space;
	int rx_fill_space;
	u32 txtotal = txsize;
	u32 rxtotal = rxsize;
	u32 eff_bytes, eff_word;
	int tx_cnt, rx_cnt;
	u32 *ptxbuf;
	u32 *prxbuf;

	ifx_ssc_dma_setup(port, IFX_SSC_DIR_TXRX, IFX_SSC_DMA_DISABLE);

	IFX_SSC_IRQ_LOCK(port);
	while (1) {

		tx_fifo_space =
			port->tx_fifo_size_words -
			IFX_SSC_TX_FIFO_FILL_LEVEL(port);
		/* No space, wait */
		if (tx_fifo_space == 0)
			continue;

		/* Still have more data to transmit, whole words */
		if (txtotal > (tx_fifo_space << 2)) {
			eff_bytes = (tx_fifo_space << 2);
			eff_word = tx_fifo_space;
			for (i = 0, ptxbuf = (u32 *) txbuf; i < eff_word;
			     i++, ptxbuf++) {
				IFX_SSC_TX_WORD(*ptxbuf, port);
			}
		} else {
			eff_bytes = txtotal;
			eff_word = (eff_bytes >> 2) + ((eff_bytes & 0x3) >
					0 ? 1 : 0);

			/* Transmit the complete word */
			for (i = 0, ptxbuf = (u32 *) txbuf;
			     i < eff_word - 1; i++, ptxbuf++) {
				IFX_SSC_TX_WORD(*ptxbuf, port);
			}

			tx_cnt = (eff_bytes & 0x3);

			/* Still one complete word */
			if (tx_cnt == 0)
				tx_cnt = 4;

			ifx_ssc_start_tx_last_word(port, (char *) ptxbuf,
				tx_cnt);
		}
		txbuf += eff_bytes;
		txtotal -= eff_bytes;

		/* NB, wait for data to come in, not efficient,
		 * but no RX overrun issue */
		while (IFX_SSC_RX_FIFO_FILL_LEVEL(port) != eff_word)
			;

		rx_fill_space = IFX_SSC_RX_FIFO_FILL_LEVEL(port);

		if (rxtotal > (rx_fill_space << 2)) {
			eff_bytes = (rx_fill_space << 2);
			eff_word = rx_fill_space;
			for (i = 0, prxbuf = (u32 *) rxbuf; i < eff_word;
			     i++, prxbuf++) {
				*prxbuf = IFX_SSC_GET_RX_WORD(port);
			}
		} else {	/* At least one word */
			eff_bytes = rxtotal;
			eff_word = (eff_bytes >> 2) + ((eff_bytes & 0x3) >
					0 ? 1 : 0);
			/* Receive complete word */
			for (i = 0, prxbuf = (u32 *) rxbuf;
			     i < eff_word - 1; i++, prxbuf++) {
				*prxbuf = IFX_SSC_GET_RX_WORD(port);
			}
			/* SPI Master know how many bytes to be received */
			rx_cnt = rxtotal - ((eff_word - 1) << 2);
			ifx_ssc_start_rx_last_word(port, (char *) prxbuf,
				rx_cnt);
		}
		rxbuf += eff_bytes;
		rxtotal -= eff_bytes;

		/* Both RX/TX finished, break */
		if ((txtotal == 0) && (rxtotal == 0))
			break;
	}
	IFX_SSC_IRQ_UNLOCK(port);
	return txsize;
}

/**
 * \fn static INLINE int ifx_ssc_txfifo(struct ifx_ssc_port *port,
 *                                      char *txbuf, u32 txsize)
 * \brief Called to transmit data to SSC using FIFO mode.
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE int
ifx_ssc_txfifo(struct ifx_ssc_port *port, char *txbuf, u32 txsize)
{
	SSC_KASSERT(port->opts.modeRxTx == IFX_SSC_MODE_TX,
		("%s invalid txrx mode\n", __func__));
	return ifx_ssc_start_txfifo(port, txbuf, txsize);
}

/**
 * \fn static INLINE int ifx_ssc_rxfifo(struct ifx_ssc_port *port,
 *                                      char *rxbuf, u32 rxsize)
 * \brief   Called to receive from SSC using FIFO mode.
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   rxbuf      Pointer to store the received data packet
 * \param   rxsize     Amount of Bytes to receive.
 * \return  >= 0       Number of bytes received
 * \return  < 0        error number
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE int
ifx_ssc_rxfifo(struct ifx_ssc_port *port, char *rxbuf, u32 rxsize)
{
	SSC_KASSERT(port->opts.modeRxTx == IFX_SSC_MODE_RX,
		("%s invalid txrx mode\n", __func__));
	return ifx_ssc_start_rxfifo(port, rxbuf, rxsize);
}

/**
 * \fn static INLINE int ifx_ssc_set_spi_mode(ssc_device_t *dev)
 * \brief SSC set ssc_ mode
 * Sets the spi mode of the corresponding device. SSC mode is per device
 *      parameter. It is initialized during registeration
 *
 * \param   dev    Pointer to device
 * \return  0       OK
 * \return  -EBUSY  could not set ssc mode because the system is busy
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE int ifx_ssc_set_spi_mode(ssc_device_t *dev)
{
	u32 reg;
	int val = IFX_SSC_CON_PH;

	IFX_SSC_CONFIGURE_t *ssc_cfg;
	struct ifx_ssc_port *port;

	ssc_cfg = &dev->conn_id;
	port = dev->port;

	if (port->prev_ssc_mode == ssc_cfg->ssc_mode)
		return 0;

	if ((IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_BUSY)
		|| IFX_SSC_RX_TO_RECEIVED(port) > 0) {
		LOGF_KLOG_ERROR("%s failed to set spi mode\n", __func__);
		return -EBUSY;
	}
	switch (ssc_cfg->ssc_mode) {
	case IFX_SSC_MODE_0:
		val = IFX_SSC_CON_PH;
		break;

	case IFX_SSC_MODE_1:
		val = 0;
		break;

	case IFX_SSC_MODE_2:
		val = IFX_SSC_CON_PO | IFX_SSC_CON_PH;
		break;

	case IFX_SSC_MODE_3:
		val = IFX_SSC_CON_PO;
		break;

	default:
		break;
	}
	IFX_SSC_IRQ_LOCK(port);

	IFX_SSC_CONFIG_MODE(port);

	reg = IFX_SSC_GET_CON(port);
	reg &= ~(IFX_SSC_CON_PO | IFX_SSC_CON_PH);
	reg |= (val);
	IFX_SSC_SET_CON(reg, port);
	smp_wmb();
	IFX_SSC_RUN_MODE(port);
	IFX_SSC_IRQ_UNLOCK(port);
	port->prev_ssc_mode = ssc_cfg->ssc_mode;
	return 0;
}

/**
 * \fn static INLINE void ifx_ssc_txrx_setup( struct ifx_ssc_port *port)
 * \brief SSC set txrx direction if using FIFO mode.
 * To use FIFO for transmission and reception. Firstly, rx/tx mode muse be set
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_txrx_setup(struct ifx_ssc_port *port)
{
	if (port->opts.modeRxTx != IFX_SSC_MODE_RXTX)
		ifx_ssc_rxtx_mode_set(port, IFX_SSC_MODE_RXTX);
}

/**
 * \fn static INLINE void ifx_ssc_tx_setup( struct ifx_ssc_port *port)
 * \brief SSC set Tx mode
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_tx_setup(struct ifx_ssc_port *port)
{
	if (port->opts.modeRxTx != IFX_SSC_MODE_TX)
		ifx_ssc_rxtx_mode_set(port, IFX_SSC_MODE_TX);
}

/**
 * \fn static INLINE void ifx_ssc_rx_setup( struct ifx_ssc_port *port)
 * \brief SSC set Rx mode
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_rx_setup(struct ifx_ssc_port *port)
{
	if (port->opts.modeRxTx != IFX_SSC_MODE_RX)
		ifx_ssc_rxtx_mode_set(port, IFX_SSC_MODE_RX);
}

/**
 * \fn static void ifx_ssc_enqueue(IFX_SSC_QUEUE_t *queue)
 * \brief SSC add queue entry to priority queue
 *
 * \param   queue    Pointer to structure #IFX_SSC_QUEUE_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void ifx_ssc_enqueue(IFX_SSC_QUEUE_t *queue)
{
	ssc_device_t *dev;
	struct ifx_ssc_port *port;

	SSC_KASSERT((queue != NULL),
		("%s should never happen\n", __func__));
	dev = queue->dev;
	port = dev->port;
	IFX_SSC_PRINT(port, SSC_MSG_QUEUE, "%s dev %s prio %d enqueued\n",
		__func__, dev->dev_name, dev->dev_prio);
	IFX_SSC_Q_LOCK_BH(port);
	if (dev->dev_prio == IFX_SSC_PRIO_ASYNC)
		TAILQ_INSERT_TAIL(&port->ssc_asyncq, queue, q_next);
	else
		TAILQ_INSERT_TAIL(&port->ssc_syncq[dev->dev_prio], queue,
				q_next);
	dev->stats.enqueue++;
	IFX_SSC_Q_UNLOCK_BH(port);
}

/**
 * \fn static void ifx_ssc_dequeue(IFX_SSC_QUEUE_t *queue)
 * \brief SSC remove queue entry from priority queue
 *
 * \param   queue    Pointer to structure #IFX_SSC_QUEUE_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void ifx_ssc_dequeue(IFX_SSC_QUEUE_t *queue)
{
	ssc_device_t *dev;
	struct ifx_ssc_port *port;

	SSC_KASSERT((queue != NULL),
		("%s should never happen\n", __func__));
	dev = queue->dev;
	port = dev->port;
	IFX_SSC_PRINT(port, SSC_MSG_QUEUE, "%s dev %s prio %d dequeued\n",
		__func__, dev->dev_name, dev->dev_prio);
	IFX_SSC_Q_LOCK_BH(port);
	if (dev->dev_prio == IFX_SSC_PRIO_ASYNC) {
		if (!TAILQ_EMPTY(&port->ssc_asyncq))
			TAILQ_REMOVE(&port->ssc_asyncq, queue, q_next);
	} else {
		if (!TAILQ_EMPTY(&port->ssc_syncq[dev->dev_prio]))
			TAILQ_REMOVE(&port->ssc_syncq[dev->dev_prio],
				queue, q_next);
	}
	dev->stats.dequeue++;
	IFX_SSC_Q_UNLOCK_BH(port);
}

/**
 * \fn static void ifx_ssc_cs_lock(ssc_device_t *dev)
 * \brief SSC chip select function, set spi mode, baudrate, call registered
 *     device-specific cs set function.
 *
 * \param   dev    Pointer to structure #ssc_device_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void ifx_ssc_cs_lock(ssc_device_t *dev)
{
	IFX_SSC_CONFIGURE_t *ssc_cfg;
	struct ifx_ssc_port *port;

	port = dev->port;
	if (port->ssc_cs_locked == true) {
		LOGF_KLOG_ERROR("Fatal error: %s locked already before\n", __func__);
		return;
	}
	IFX_SSC_PRINT(port, SSC_MSG_LOCK, "%s %s enter\n", __func__,
		dev->dev_name);

	port->ssc_cs_locked = true;
	ssc_cfg = &dev->conn_id;
	ifx_ssc_set_spi_mode(dev);
	ifx_ssc_set_baudrate(port, ssc_cfg->baudrate);
	if (ssc_cfg->csset_cb != NULL)
		ssc_cfg->csset_cb(IFX_SSC_CS_ON, ssc_cfg->cs_data);
}

/**
 * \fn static INLINE void ifx_ssc_cs_unlock(ssc_device_t *dev)
 * \brief SSC chip un select function,  call registered device-specific cs reset
 *        function.
 *
 * \param   dev    Pointer to structure #ssc_device_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_cs_unlock(ssc_device_t *dev)
{
	IFX_SSC_CONFIGURE_t *ssc_cfg;
	struct ifx_ssc_port *port;

	port = dev->port;
	if (port->ssc_cs_locked == false) {
		LOGF_KLOG_ERROR("Fatal error: %s unlocked already before\n", __func__);
		return;
	}
	ssc_cfg = &dev->conn_id;
	if (ssc_cfg->csset_cb != NULL)
		ssc_cfg->csset_cb(IFX_SSC_CS_OFF, ssc_cfg->cs_data);
	port->ssc_cs_locked = false;
	IFX_SSC_PRINT(port, SSC_MSG_LOCK, "%s %s exit\n", __func__,
		dev->dev_name);
}

/**
 *\fn int ifx_sscLock(IFX_SSC_HANDLE handler)
 *\brief  Called to lock and reserve the whole SSC interface
 * for the given 'handler'
 *
 * The chipselect, belonging to this SSC session is already
 * activated. This means the chipselect callback is called.
 * After complete data transmission and reception, ifx_sscUnLock
 * has to be called to release the SSC interface again for
 * other clients.
 *
 * \param   handler    Handle of the connection where to make the
 *                     configuration on
 * \return  0          Locked succesfully
 * \return  -1         Failed to lock the ssc bus
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscLock(IFX_SSC_HANDLE handler)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg;
	struct ifx_ssc_port *port;
	IFX_SSC_QUEUE_t *pqueue;

	SSC_KASSERT((handler != NULL),
		("%s Invalid parameter\n", __func__));

	dev = (ssc_device_t *) handler;
	pqueue = &dev->queue;
	ssc_cfg = &dev->conn_id;
	port = dev->port;

	if (port->lock_qentry == pqueue)
		return -1;

	/*
	 * Check if the queue entry of the ConnId is already queued with
	 * a request but this request is not served yet. Every ConnId
	 * can only queue up one request at one time.
	 */
	if (atomic_read(&pqueue->isqueued) == 1)
		return -1;

	IFX_SSC_Q_LOCK_BH(port);
	/* Initialize unused variable */
	pqueue->txbuf = NULL;
	pqueue->txsize = 0;
	pqueue->rxbuf = NULL;
	pqueue->rxsize = 0;

	/* Place a lock request in the queue */
	pqueue->request_lock = true;
	IFX_SSC_Q_UNLOCK_BH(port);
	atomic_set(&pqueue->isqueued, 1);

	/* Add queue entry to priority queue */
	ifx_ssc_enqueue(pqueue);

	/*
	 * If no async support is avaiable, trigger the SSC kernel thread and
	 * wait pending till job is done.
	 */
#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
	IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s raise fake interrupt\n",
		__func__);
	ifx_ssc_start_tasklet(port);
#else
	IFX_SSC_PRINT(port, SSC_MSG_THREAD,
		"%s wake up ssc kernel thread\n", __func__);
	/* Kick off SSC kernel thread */
	IFX_SSC_WAKEUP_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT,
		port->event_flags);
#endif
	/* Wait till wake up from SSC kernel thread */
	IFX_SSC_WAIT_EVENT(dev->dev_thread_wait, IFX_SSC_DEV_THREAD_EVENT,
		dev->event_flags);
	IFX_SSC_PRINT(port, SSC_MSG_THREAD,
		"%s wakeup received from ssc kernel thread\n",
		__func__);
	return 0;
}
EXPORT_SYMBOL(ifx_sscLock);

/**
 *\fn int ifx_sscUnlock(IFX_SSC_HANDLE handler)
 * \brief  This function releases the SSC lock that was placed before by
 *         calling ifx_SscLock.
 *
 * This function also inactivate the chipselect signal, which was set in
 * ifx_SscLock.
 *
 * \param   handler    Handle of the connection where to make the
 *                     configuration on
 * \return  0          UnLocked succesfully
 * \return  -1         Failed to unlock the ssc bus
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscUnlock(IFX_SSC_HANDLE handler)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg = NULL;
	struct ifx_ssc_port *port = NULL;
	IFX_SSC_QUEUE_t *pqueue;

	SSC_KASSERT((handler != NULL),
		    ("%s Invalid parameter\n", __func__));

	dev = (ssc_device_t *) handler;
	pqueue = &dev->queue;
	ssc_cfg = &dev->conn_id;
	port = dev->port;

	if (port->lock_qentry != &dev->queue)
		return -1;

	/* Just forget about the lock, then the SSC driver would just take it
	 * as a normel queue entry
	 */
	ifx_ssc_cs_unlock(dev);
	IFX_SSC_Q_LOCK_BH(port);
	pqueue->txbuf = NULL;
	pqueue->txsize = 0;
	pqueue->rxbuf = NULL;
	pqueue->rxsize = 0;
	port->lock_qentry = NULL;
	IFX_SSC_Q_UNLOCK_BH(port);
	return 0;
}
EXPORT_SYMBOL(ifx_sscUnlock);

/**
 * \fn int ifx_sscSetBaud(IFX_SSC_HANDLE handler, unsigned int baud)
 * \brief Configures the Baudrate of a given connection.
 *
 * The baudrate can also be change multiple times
 * for a single connection. The baudrate change
 * will take place for the next call of ifx_SscTx,
 * ifx_SscRx or ifx_SscTxRx.
 *
 * \param   handler    Handle of the connection where to make the
 *                     configuration on
 * \param   baud       Baudrate to configure. This value can be rounded
 *                     during the calculation of the SSC clock divider
 *
 * \return  = 0        OK
 * \return  < 0        error number
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscSetBaud(IFX_SSC_HANDLE handler, unsigned int baud)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg;

	/* Sanity check */
	SSC_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

	dev = (ssc_device_t *) handler;
	ssc_cfg = &dev->conn_id;
	/* XXX, protection because of being used in other places */
	ssc_cfg->baudrate = baud;
	return 0;
}
EXPORT_SYMBOL(ifx_sscSetBaud);

#ifdef LTQ_DMA_SUPPORT
/**
 * \fn static int ifx_ssc_txdma(struct ifx_ssc_port *port, char *txbuf,
 *     int txsize)
 * \brief Called to transmit the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   txbuf      Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_txdma(struct ifx_ssc_port *port, char *txbuf, int txsize)
{
	int i, j;
	int retval = 0;
	int k, m;
	char *p;
	char *pbuf;
	struct dma_device_info *dma_dev;

	ifx_ssc_dma_setup(port, IFX_SSC_DIR_TX, IFX_SSC_DMA_ENABLE);

	/*
	 * HW WAR, last 1~3 byte must do swapping, but the wanted bytes could be
	 * out of orignal tx buffer, so pre-allocated buffer used, and what's
	 * more, memcpy involved.
	 * XXX, actually, potential issues include data length which is a
	 * multiple of DMA burst length.
	 */
	i = txsize >> 2;  /* divided by 4 */
	j = txsize & 0x3; /* modulo */
	if (j != 0) {  /* Hit last 1~3 byte case */
		memcpy(port->dma_txbuf, txbuf, txsize);
		p = port->dma_txbuf + (i << 2);
		for (m = 0, k = j - 1; k >= 0; k--, m++)
			p[4 - m - 1] = p[k];
		pbuf = port->dma_txbuf;
	} else
		pbuf = txbuf;

	dma_dev = port->dma_dev;

	/**
	* Set a flag that we are waiting for the DMA to complete. This flag
	* will be reseted again in the DMA interrupt.
	* NB, it must be ahead of the following stuff, because once descriptor
	* is prepared, interrupt may come back immediately
	*/
	atomic_set(&port->dma_wait_state, 1);

	/* TX DMA only half duplex is used */
	port->dma_is_in_half_duplex = 1;
	/**
	* Run in tasklet or kernel thread, DMA tasklet may run the same function
	* Lock must be used.
	*/
	retval = dma_device_write(dma_dev, pbuf, txsize, NULL);
	SSC_KASSERT(retval == txsize, ("%s retval %d != txsize %d\n",
		__func__, retval, txsize));

	return retval;
}

/**
 * \fn static int ifx_ssc_dma_fdx(struct ifx_ssc_port *port, char *txbuf,
 *                                int txsize, char *rxbuf, int rxsize)
 * \brief Called to transmit/receive the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   txbuf      Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \param   rxbuf      Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_dma_fdx(struct ifx_ssc_port *port, char *txbuf, int txsize,
		char *rxbuf, int rxsize)
{
	int retval = 0;
	struct dma_device_info *dma_dev;
	dma_dev = port->dma_dev;

	/*
	 * FDX required TX/RX has the same amount of data and they must be
	 * a multiple of of DMA burst length
	 */
	if (((rxsize & ((dma_dev->rx_burst_len << 2) - 1)) != 0) ||
	    ((txsize & ((dma_dev->tx_burst_len << 2) - 1)) != 0)) {
		LOGF_KLOG_ERROR("%s data length is not a multiple of DMA burst length %d\n",
			__func__, rxsize);
		return -EINVAL;
	}

	/*
	 * Backup original buffer, so that later we can find it in dma handler
	 * Borrow two variables from FIFO usage
	 */
	port->rxbuf_ptr = rxbuf;
	port->actual_rx_len = rxsize;

	/* Prepare RX descriptor */
	dma_device_desc_setup(dma_dev, rxbuf, rxsize);

	/* Enable SSC TX/RX DMA */
	ifx_ssc_dma_setup(port, IFX_SSC_DIR_TXRX, IFX_SSC_DMA_ENABLE);

	/*
	 * NB, Enable SPI DMA channel every time. There are two reasons as
	 * follow,
	 * 1. In synchronous DMA application, there is no pre-allocation
	 *    memory for SPI bus driver. When it registers with central
	 *    DMA driver, it will not open RX channel for reception. So
	 *    at least, we need to open channel once before we use it.
	 * 2. We have to close this channel after we receive the packet
	 *    since we have only one descriptor, and we can't clear the
	 *    descriptor. This will be explained further in close channel part.
	 */
	(dma_dev->rx_chan[dma_dev->current_rx_chan])->
		open(dma_dev->rx_chan[dma_dev->current_rx_chan]);

	/* Set a flag that we are waiting for the DMA to complete. This flag
	 * will be reseted again in the DMA interrupt.
	 */
	atomic_set(&port->dma_wait_state, 1);

	/* Full Duplex is used */
	port->dma_is_in_half_duplex = 0;

	/* TX real data sent, RX real data will come back */

	retval = dma_device_write(dma_dev, txbuf, txsize, NULL);

	SSC_KASSERT(retval == txsize, ("%s retval %d != txsize %d\n",
				__func__, retval, txsize));

	return retval;
}

/**
 * \fn static int ifx_ssc_rxdma(struct ifx_ssc_port *port, char *rxbuf,
 *                              int rxsize)
 * \brief Called to receive the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   rxbuf      Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes received
 * \return   < 0       error number
 * \ingroup IFX_SSC_INTERNAL
 */
static int
ifx_ssc_rxdma(struct ifx_ssc_port *port, char *rxbuf, int rxsize)
{
	char *pbuf;
	int dma_rxsize;
	int retval = 0;
	u32 dma_burst_len;
	u32 dma_burst_len_mask;
	struct dma_device_info *dma_dev;
	dma_dev = port->dma_dev;
	dma_burst_len = (dma_dev->rx_burst_len << 2);
	dma_burst_len_mask = dma_burst_len - 1;

	/*
	 * Backup original buffer, so that later we can find it in dma handler
	 * Borrow two variables from FIFO usage
	 */
	port->rxbuf_ptr = rxbuf;
	port->actual_rx_len = rxsize;

	if ((rxsize & dma_burst_len_mask)) {
		pbuf = port->dma_rxbuf;
		dma_rxsize = (rxsize & ~dma_burst_len_mask) + dma_burst_len;
		SSC_KASSERT(dma_rxsize <= DEFAULT_SSC_FRAGMENT_SIZE,
			("%s fragment %d out of range\n", __func__,
			dma_rxsize));
	} else {
		pbuf = rxbuf;
		dma_rxsize = rxsize;
	}

	dma_device_desc_setup(dma_dev, pbuf, dma_rxsize);

	ifx_ssc_dma_setup(port, IFX_SSC_DIR_TXRX, IFX_SSC_DMA_ENABLE);

	/*
	 * NB, Enable SPI DMA channel every time. There are two reasons as
	 * follow,
	 * 1. In synchronous DMA application, there is no pre-allocation
	 *    memory for SPI bus driver. When it registers with central
	 *    DMA driver, it will not open RX channel for reception. So
	 *    at least, we need to open channel once before we use it.
	 * 2. We have to close this channel after we receive the packet
	 *    since we have only one descriptor, and we can't clear the
	 *    descriptor. This will be explained further in close channel part.
	 */

	(dma_dev->rx_chan[dma_dev->current_rx_chan])->
		open(dma_dev->rx_chan[dma_dev->current_rx_chan]);

	/* Set a flag that we are waiting for the DMA to complete. This flag
	 * will be reseted again in the DMA interrupt.
	 */
	atomic_set(&port->dma_wait_state, 1);
	port->dma_is_in_half_duplex = 0;

	/* TX dummy data sent */

	retval = dma_device_write(dma_dev, port->dma_txbuf, dma_rxsize, NULL);

	SSC_KASSERT(retval == dma_rxsize, ("%s retval %d != txsize %d\n",
		__func__, retval, dma_rxsize));

	return rxsize;
}
#endif
/**
 *\fn int ifx_sscTxRx (IFX_SSC_HANDLE handler, char *txbuf, u32 txsize,
 *                     char *rxbuf, u32 rxsize)
 *\brief Called to transmit/receive to/from SSC in one step.

 * This means that the data transmission and reception is done in parallel.
 * No DMA is possible here. The SSC driver sets the chipselect when the
 * data transmission starts and resets it when the transmission is
 * completed. The transmit and receive buffer memory allocation and
 * de-allocation is done by the SSC client.
 *
 * \param   handler   Handle of the connection where to make the
 *                    configuration on
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize    Amount of Bytes to transmit
 * \param   rxbuf     Pointer to store the received data packet
 * \param   rxsize    Amount of Bytes to receive.
 * \return  >= 0      Number of bytes received (if rxbuf != 0) or transmitted
 * \return  < 0       error number
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscTxRx(IFX_SSC_HANDLE handler, char *txbuf, u32 txsize, char *rxbuf,
	    u32 rxsize)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg;
	IFX_SSC_QUEUE_t *pqueue;
	struct ifx_ssc_port *port;
	struct ifx_ssc_device_stats *stats;

	/*anity check */
	SSC_KASSERT((handler != NULL),
		("%s Invalid parameter\n", __func__));

	SSC_KASSERT(!((rxbuf == NULL) && (rxsize == 0)
		&& (txbuf == NULL) && (txsize == 0)),
		("%s Invalid parameter\n", __func__));

	dev = (ssc_device_t *) handler;
	stats = &dev->stats;
	port = dev->port;

	if (in_interrupt()) {
		stats->context_err++;
		LOGF_KLOG_ERROR("%s can't be called in interupt context < irq, softirq, tasklet>\n",
			__func__);
		return 0;
	}
	ssc_cfg = &dev->conn_id;
	pqueue = &dev->queue;

	if (txsize > ssc_cfg->fragSize || rxsize > ssc_cfg->fragSize) {
		stats->frag_err++;
		LOGF_KLOG_ERROR("%s Device driver must do its own fragmentation tx %d rx %d > %d\n",
			__func__, txsize, rxsize, ssc_cfg->fragSize);
		return 0;
	}

	/* Full dupex mode sanity check  */
	if ((txsize != 0) && (rxsize != 0)) {
		if (ssc_cfg->duplex_mode == IFX_SSC_FULL_DUPLEX) {
			if (txsize != rxsize) {
				stats->dlx_err++;
				LOGF_KLOG_ERROR("%s full duplex required TX/RX has the same amount of data tx %d rx %d\n",
					__func__, txsize, rxsize);
				return 0;
			}
		}
	}

	/*
	 * Ensure that only asynchronous SSC Handles could enqueue a
	 * synchronous request. The parameter 'handle_type' is set during the
	 * ConnId allocation process.
	 */
	if (pqueue->handle_type != IFX_SSC_HANDL_TYPE_SYNC) {
		stats->handler_err++;
		LOGF_KLOG_ERROR("%s must use sync handler\n", __func__);
		return 0;
	}

	/*
	 * Check if the queue entry of the ConnId is already queued with
	 * a request but this request is not served yet. Every ConnId can
	 * only queue up one request at one time.
	 */
	if (atomic_read(&pqueue->isqueued) == 1) {
		stats->dup_qentries++;
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s same queue has queued more than once\n",
			__func__);
		return 0;
	}
	/* Add pointer and sizes to the queue entry of this SSC handle. */
	IFX_SSC_Q_LOCK_BH(port);
	pqueue->txbuf = txbuf;
	pqueue->txsize = txsize;
	pqueue->rxbuf = rxbuf;
	pqueue->rxsize = rxsize;
	atomic_set(&pqueue->isqueued, 1);
	pqueue->exchange_bytes = 0;
	pqueue->request_lock = false;
	memset(&pqueue->callback, 0, sizeof(IFX_SSC_ASYNC_CALLBACK_t));
	IFX_SSC_Q_UNLOCK_BH(port);
	/* Add queue entry to priority queue */
	ifx_ssc_enqueue(pqueue);

	/* If no async support is avaiable, trigger the SSC kernel thread and
	 * wait pending till job is done.
	 */
#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
	IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s raise fake interrupt\n",
		__func__);
	ifx_ssc_start_tasklet(port);
#else
	IFX_SSC_PRINT(port, SSC_MSG_THREAD,
		"%s wake up ssc kernel thread\n", __func__);
	/* Kick off SSC kernel thread */
	IFX_SSC_WAKEUP_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT,
		port->event_flags);
#endif
	/* Wait till wakeup from SSC kernel thread */
	IFX_SSC_WAIT_EVENT(dev->dev_thread_wait, IFX_SSC_DEV_THREAD_EVENT,
		dev->event_flags);
	IFX_SSC_PRINT(port, SSC_MSG_THREAD,
		"%s wakeup event received from ssc kernel thread\n",
		__func__);

	/* Reset queue pointer */
	IFX_SSC_Q_LOCK_BH(port);
	pqueue->txbuf = NULL;
	pqueue->rxbuf = NULL;
	IFX_SSC_Q_UNLOCK_BH(port);
	return pqueue->txsize + pqueue->rxsize;
}
EXPORT_SYMBOL(ifx_sscTxRx);

/**
 *\fn int ifx_sscTx(IFX_SSC_HANDLE handler, char *txbuf, u32 txsize)
 *\brief Called to transmit the data.

 * transmission starts and resets it when the transmission
 * the transmit buffer is done by the SSC client.
 *
 * \param   handler    Handle of the connection where to make the
 *                     configuration on
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscTx(IFX_SSC_HANDLE handler, char *txbuf, u32 txsize)
{
	return ifx_sscTxRx(handler, txbuf, txsize, NULL, 0);
}
EXPORT_SYMBOL(ifx_sscTx);

/**
 *\fn int ifx_sscRx(IFX_SSC_HANDLE handler, char *rxbuf, u32 rxsize)
 *\brief Called to receive the data.

 * The SSC driver sets the chipselect when the data reception starts and
 * resets it when the reception is completed. The memory allocation and
 * de-allocation of the receive buffer is done by the SSC client.
 *
 * \param   handler    Handle of the connection where to make the
 *                     configuration on
 * \param   rxbuf     Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes received
 * \return  < 0        error number
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscRx(IFX_SSC_HANDLE handler, char *rxbuf, u32 rxsize)
{
	return ifx_sscTxRx(handler, NULL, 0, rxbuf, rxsize);
}
EXPORT_SYMBOL(ifx_sscRx);

/**
 * \fn static int ifx_ssc_serve_qentry(struct ifx_ssc_port *port)
 * \brief Called to serve every queue entry and it is a common function for
 * SSC kernel thread and tasklet
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \return  1          Continue to loop this function until return 0
 * \return  0         Immediately exit this function. For kernel thread,
 *                     it will sleep, for tasklet, it will exit dynamically
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_serve_qentry(struct ifx_ssc_port *port)
{
	IFX_SSC_QUEUE_t *qentry = NULL;
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg = NULL;
	struct dma_device_info *dma_dev;
	/*
	 * Don't serve the current or next queue entry in case we are currently
	 * waiting for the DMA interrupt to report the transmission completion.
	 */
	if (atomic_read(&port->dma_wait_state) == 1) {
		/*
		 * DMA interrupt will spawn tasklet or wake up kernel thread
		 * in order to continue.
		 */
		return 0;
	}

	/* Identify the queue entry to serve */
	if (port->serve_qentry != NULL) {
		/* Continues serving the queue that was served before */
		qentry = port->serve_qentry;
	} else if (port->lock_qentry != NULL) {
		/*
		 * If one queue holds the lock, only serve this one element
		 * and ignore all others.
		 */
		qentry = port->lock_qentry;
		port->serve_qentry = qentry;
	} else {
		IFX_SSC_Q_LOCK_BH(port);
		if (!TAILQ_EMPTY(&port->ssc_asyncq)) {
			qentry = TAILQ_FIRST(&port->ssc_asyncq);
		} else {
			int i;
			/* Choose the highest queue entry first */
			for (i = IFX_SSC_PRIO_HIGH; i >= IFX_SSC_PRIO_LOW;
			     i--) {
				if (!TAILQ_EMPTY(&port->ssc_syncq[i])) {
					qentry = TAILQ_FIRST
						(&port->ssc_syncq[i]);
					break;
				}
			}
		}
		IFX_SSC_Q_UNLOCK_BH(port);

		/* Remember that we are working on this queue now */
		port->serve_qentry = qentry;

		/*
		 * NB, Don't remove the qentry here since we may never come
		 * here once callers called sequence. It may cause enqueue
		 * /dequeue mismatch
		 */
	}
	/* No queue found that should be served */
	if (qentry == NULL)
		return 0;

	/* Get connection handle */
	dev = qentry->dev;
	ssc_cfg = &dev->conn_id;
	dma_dev = port->dma_dev;

	#ifdef EXPERIMENTAL_FIX
	/* suspect the tasklet still run after the dect driver is freed */
	/* check if queue->dev still exist on port->ssc_devs */
	/* if it doesn't, print out a warning ... and safely return */
	if (port->ssc_ndevs > 0) {
		ssc_device_t *q, *next;
		unsigned int found = 0;

		IFX_SSC_SEM_LOCK(port->dev_sem);
		TAILQ_FOREACH_SAFE(q, &port->ssc_devq, dev_entry, next) {
			if (q == dev)
				found = 1;
		}
		IFX_SSC_SEM_UNLOCK(port->dev_sem);

		if (!found) {
			IFX_SSC_PRINT(port, SSC_MSG_ERROR, "[%s] NO MATCHING DRIVER!!! INVALID QUEUE\n", __func__);
			return 0;
		}
	}
	else {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR, "[%s] EMPTY REGISTERED DRIVER FOR PORT!!! INVALID QUEUE!!\n", __func__);
		return 0;
	}
	#endif /* EXPERIMENTAL_FIX */

	if ((ssc_cfg->duplex_mode == IFX_SSC_FULL_DUPLEX)
		&& (qentry->txbuf != NULL)
		&& (qentry->rxbuf != NULL)) {

		if ((port->ssc_cs_locked == false)
			&& (port->lock_qentry == NULL)) {
			/* Call the set callback of theSC-Handle */
			ifx_ssc_cs_lock(dev);
		}
		ifx_ssc_txrx_setup(port);

		if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
			int tx_dma_aligned = ((((u32) qentry->txbuf) &
			 ((dma_dev->tx_burst_len << 2) - 1)) == 0) ? 1 : 0;
			int rx_dma_aligned = ((((u32) qentry->rxbuf) &
			((dma_dev->rx_burst_len << 2) - 1)) == 0) ? 1 : 0;

			/* If buffer not aligned on DMA burst length,
			 * fall back to FIFO */
			if ((qentry->txsize > ssc_cfg->maxFIFOSize)
				&& (tx_dma_aligned == 1)
				&& (qentry->rxsize > ssc_cfg->maxFIFOSize)
				&& (rx_dma_aligned == 1)) {

				IFX_SSC_PRINT(port, SSC_MSG_DUPLEX,
					"%s TX/RX DMA enter\n", __func__);
				ifx_ssc_dma_fdx(port, qentry->txbuf,
					qentry->txsize, qentry->rxbuf,
					qentry->rxsize);

				port->stats.txDma++;
				port->stats.rxDma++;
				/*
				 * Reset the data pointer, because this data are
				 * done on the SSC hardware.
				 */
				qentry->txbuf = NULL;
				/* Count the number of transmitted bytes for
				 * this queue entry */
				qentry->exchange_bytes = qentry->txsize;

				qentry->rxbuf = NULL;
				qentry->exchange_bytes += qentry->rxsize;

				dev->stats.txBytes += qentry->txsize;
				port->stats.txBytes += qentry->txsize;
				port->stats.txDmaBytes += qentry->txsize;
				dev->stats.rxBytes += qentry->rxsize;
				port->stats.rxBytes += qentry->rxsize;
				port->stats.rxDmaBytes += qentry->rxsize;
				return 0;
			}  else
				goto fdxfifo;

#endif
		} else {
fdxfifo:
			ifx_ssc_txrx_fifo_fdx(port, qentry->txbuf,
				qentry->txsize, qentry->rxbuf, qentry->rxsize);
			port->stats.txFifo++;
			port->stats.rxFifo++;
			IFX_SSC_PRINT(port, SSC_MSG_DUPLEX,
				"%s TX/RX FIFO enter\n", __func__);

			/*
			 * Reset the data pointer, because this data are
			 * done on the SSC hardware.
			 */
			qentry->txbuf = NULL;
			/* Count the number of transmitted bytes for
			 * this queue entry */
			qentry->exchange_bytes = qentry->txsize;
			qentry->rxbuf = NULL;
			qentry->exchange_bytes += qentry->rxsize;

			dev->stats.txBytes += qentry->txsize;
			port->stats.txBytes += qentry->txsize;
			dev->stats.rxBytes += qentry->rxsize;
			port->stats.rxBytes += qentry->rxsize;
			return 1;
		}

	} else if (qentry->txbuf != NULL) {
		if ((port->ssc_cs_locked == false)
			&& (port->lock_qentry == NULL)) {
			/* Call the set callback of the SSC-Handle */
			ifx_ssc_cs_lock(dev);
		}

		/* If buffer not aligned on DMA burst length,
		 * fall back to FIFO */
		if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
			int tx_dma_aligned = ((((u32) qentry->txbuf)&
			((dma_dev->tx_burst_len << 2) - 1)) == 0) ? 1 : 0;

			if ((qentry->txsize > ssc_cfg->maxFIFOSize)
				&& (tx_dma_aligned == 1)) {
				IFX_SSC_PRINT(port, SSC_MSG_TX_DMA,
					"%s TX DMA enter\n", __func__);
				ifx_ssc_tx_setup(port);
				ifx_ssc_txdma(port, qentry->txbuf,
					qentry->txsize);
				port->stats.txDma++;
				/*
				 * Reset the data pointer, because this data
				 * are done on theSC hardware.
				 */
				qentry->txbuf = NULL;
				/* Count the number of transmitted bytes for
				 * this queue entry */
				qentry->exchange_bytes = qentry->txsize;
				dev->stats.txBytes += qentry->txsize;
				port->stats.txBytes += qentry->txsize;
				port->stats.txDmaBytes += qentry->txsize;
				return 0;
			} else
#endif
				goto txfifo;
		} else {
txfifo:
			ifx_ssc_tx_setup(port);
			ifx_ssc_txfifo(port, qentry->txbuf,
				qentry->txsize);
			port->stats.txFifo++;
			IFX_SSC_PRINT(port, SSC_MSG_TX_FIFO,
				"%s TX FIFO enter\n", __func__);

			/*
			 * Reset the data pointer, because this data are done
			 * on the SSC hardware.
			 */
			qentry->txbuf = NULL;
			/* Count the number of transmitted bytes for this
			 * queue entry */
			qentry->exchange_bytes = qentry->txsize;
			dev->stats.txBytes += qentry->txsize;
			port->stats.txBytes += qentry->txsize;
			/* NB, Make sure data has been sent out */
			ifx_ssc_wait_finished(port);
			return 1;
		}
	} else if (qentry->rxbuf != NULL) {
		if ((port->ssc_cs_locked == false)
			&& (port->lock_qentry == NULL)) {
			/* Call the set callback of the SSC-Handle */
			ifx_ssc_cs_lock(dev);
		}
		/* If buffer not aligned on DMA burst length,
		 * fall back to FIFO */
		if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
			int rx_dma_aligned = ((((u32) qentry->rxbuf) &
			 ((dma_dev->rx_burst_len << 2) - 1)) == 0) ? 1 : 0;

			if ((qentry->rxsize > ssc_cfg->maxFIFOSize)
				&& (rx_dma_aligned == 1)) {
				IFX_SSC_PRINT(port, SSC_MSG_RX_DMA,
					"%s RX DMA enter\n", __func__);
				ifx_ssc_txrx_setup(port);
				ifx_ssc_rxdma(port, qentry->rxbuf,
					qentry->rxsize);
				port->stats.txDma++;
				port->stats.rxDma++;

				/* Reset the data pointer, because this data are
				 * done on the SSC hardware.
				 */
				qentry->rxbuf = NULL;
				/* Count the number of transmitted bytes for
				 * this queue entry */
				qentry->exchange_bytes += qentry->rxsize;
				dev->stats.rxBytes += qentry->rxsize;
				port->stats.rxBytes += qentry->rxsize;
				port->stats.rxDmaBytes += qentry->rxsize;
				dev->stats.txBytes += qentry->rxsize;
				port->stats.txBytes += qentry->rxsize;
				port->stats.txDmaBytes += qentry->rxsize;
				return 0;
			} else
#endif
				goto rxfifo;
		} else {
rxfifo:
			ifx_ssc_rx_setup(port);
			ifx_ssc_rxfifo(port, qentry->rxbuf,
				       qentry->rxsize);
			port->stats.rxFifo++;
			IFX_SSC_PRINT(port, SSC_MSG_RX_FIFO,
				"%s RX FIFO enter\n", __func__);

			/* Reset the data pointer, because this data are done
			 * on the SSC hardware.
			 */
			qentry->rxbuf = NULL;
			/* Count the number of recevied bytes for this
			 * queue entry */
			qentry->exchange_bytes += qentry->rxsize;
			dev->stats.rxBytes += qentry->rxsize;
			port->stats.rxBytes += qentry->rxsize;
			/* NB, Make sure data has been sent out */
			ifx_ssc_wait_finished(port);
			return 1;
		}
	} else if (qentry->request_lock) {
		/* A lock request found */
		IFX_SSC_PRINT(port, SSC_MSG_LOCK,
			"%s request lock enter\n", __func__);
		port->lock_qentry = qentry;
		qentry->request_lock = false;

		/* Serving the current queue entry is done */
		port->serve_qentry = NULL;

		/*
		 * Reset the flag in the queue element that this one is
		 * queued with a request to be served.
		 */
		atomic_set(&qentry->isqueued, 0);

		/* syncAPI has the last watchdog branch but nested callback
		 * has to dequeue it */
		if (qentry->handle_type == IFX_SSC_HANDL_TYPE_ASYNC)
			ifx_ssc_dequeue(qentry);

		/* Call the Chipselect set callback of the SSC-Handle */
		ifx_ssc_cs_lock(dev);

		if (qentry->callback.pFunction) {
			/*
			 * Store the callback parameter local to cleanup the
			 * queue entry before calling the callback.
			 */
			IFX_SSC_ASYNC_CALLBACK_t callback =
				qentry->callback;

			qentry->callback.pFunction = NULL;
			IFX_SSC_PRINT(port, SSC_MSG_CALLBACK,
				"%s line%d request callback\n",
				__func__, __LINE__);

			callback.pFunction(callback.functionHandle, 0);
		}
		return 1;
	} else if (qentry->callback.pFunction != NULL) {
		/*
		 * Store the callback parameter local to cleanup the queue
		 * entry before calling the callback.
		 */
		IFX_SSC_ASYNC_CALLBACK_t callback = qentry->callback;

		qentry->callback.pFunction = NULL;

		/* Serving the current queue entry is done */
		port->serve_qentry = NULL;

		/*
		 * Reset the flag in the queue element that this one is queued
		 * with a request to be served.
		 */
		atomic_set(&qentry->isqueued, 0);

		ifx_ssc_dequeue(qentry);

		if ((port->ssc_cs_locked == true)
		    && (port->lock_qentry == NULL)) {
			/* Call the reset callback of the SSC-Handle */
			ifx_ssc_cs_unlock(dev);
		}

		IFX_SSC_PRINT(port, SSC_MSG_CALLBACK,
			"%s line%d direct callback\n", __func__, __LINE__);

		/* Callback to call */
		callback.pFunction(callback.functionHandle,
			qentry->exchange_bytes);
		return 1;
	} else if (qentry->handle_type == IFX_SSC_HANDL_TYPE_SYNC) {
		if ((port->ssc_cs_locked == true)
		    && (port->lock_qentry == NULL)) {
			/* Call the reset callback of the SSC-Handle */
			ifx_ssc_cs_unlock(dev);
		}


		/* Serving the current queue entry is done */
		port->serve_qentry = NULL;

		/*
		 * Reset the flag in the queue element that this one is queued
		 * with a request to be served.
		 */
		atomic_set(&qentry->isqueued, 0);

		ifx_ssc_dequeue(qentry);
		/* Wake up the pending thread */
		IFX_SSC_WAKEUP_EVENT(dev->dev_thread_wait,
			IFX_SSC_DEV_THREAD_EVENT, dev->event_flags);
		IFX_SSC_PRINT(port, SSC_MSG_THREAD,
			"%s wakeup SSC client kernel thread\n", __func__);
		/* syncAPI has no callback, we have to differentiate it */
		if (port->ssc_cs_locked == true)
			return 0;
		else
			return 1;
	} else if ((qentry->handle_type == IFX_SSC_HANDL_TYPE_ASYNC)
		   && (port->ssc_cs_locked == true)) {
		/* Serving the current queue entry is done */
		port->serve_qentry = NULL;
		/* Don't let it continue, but must restart tasklet
		 * in somewhere else */
		return 0;
	}
	return 0;
}

#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
/**
 * \fn static INLINE void ifx_ssc_tasklet_serve_queue
 *                        (struct ifx_ssc_port *port)
 * \brief Called to serve every queue entry in tasklet
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_tasklet_serve_queue(struct ifx_ssc_port *port)
{
	/*
	 * Serve queue entries till no queue entry anymore to serve,
	 * we wait for DMA or the lock entry is not in the queue.
	 */
	while (ifx_ssc_serve_qentry(port))
		;
}

/**
 * \fn static INLINE void ifx_ssc_tasklet(unsigned long arg)
 * \brief SSC tasklet implementation
 *
 * \param   arg       cast to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_tasklet(unsigned long arg)
{
	struct ifx_ssc_port *port = (struct ifx_ssc_port *) arg;

	IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s enter\n", __func__);
	ifx_ssc_tasklet_serve_queue(port);
}

/**
 * \fn static INLINE void ifx_ssc_tasklet_init(struct ifx_ssc_port *port)
 * \brief SSC tasklet initialization
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void ifx_ssc_tasklet_init(struct ifx_ssc_port *port)
{
	tasklet_init(&port->ssc_txrxq, ifx_ssc_tasklet, (unsigned long) port);
}

#else

/**
 * \fn static int ifx_ssc_kthread(void *arg)
 * \brief SSC kernel thread implementation function
 *
 * \param   arg       cast to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_kthread(void *arg)
{
	struct ifx_ssc_port *port = (struct ifx_ssc_port *) arg;

	set_current_state(TASK_INTERRUPTIBLE);
	while (!kthread_should_stop()) {
		/* Wait for DMA interrupt or sync queue to wakes us up */
		IFX_SSC_WAIT_EVENT(port->ssc_thread_wait,
			IFX_SSC_THREAD_EVENT, port->event_flags);
		IFX_SSC_PRINT(port, SSC_MSG_THREAD,
			"%s DMA or sync queue event received\n",
			__func__);
		__set_current_state(TASK_RUNNING);
		/* Serve queue entries till no more queue entry to serve, */
		while (ifx_ssc_serve_qentry(port))
			;
		set_current_state(TASK_INTERRUPTIBLE);
	}
	return 0;
}

/**
 * \fn static INLINE int ifx_ssc_thread_init(struct ifx_ssc_port *port)
 * \brief SSC kernel thread initialization
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE int ifx_ssc_thread_init(struct ifx_ssc_port *port)
{
	struct task_struct *p;
	int error = 0;

	p = kthread_run(ifx_ssc_kthread, (void *) port, "ssc_kthread");
	if (IS_ERR(p))
		error = PTR_ERR(p);
	else
		port->ssc_tsk = p;
	return error;
}
#endif /* CONFIG_LANTIQ_SPI_ASYNCHRONOUS */

static int ifx_validate_params(IFX_SSC_HANDLE handler, char *txbuf, int txsize,
			       char *rxbuf, int rxsize)
{
	int status = -EINVAL;

	if (!handler) {
		LOGF_KLOG_ERROR("%s: invalid handler\n", __func__);
		goto out;
	}

	if (!rxbuf && rxsize == 0 && !txbuf && txsize == 0) {
		LOGF_KLOG_ERROR("%s: invalid data\n");
		goto out;
	}

	status = 0;

out:
	return status;
}

/**
 * \fn int ifx_sscAsyncTxRx(IFX_SSC_HANDLE handler,
 *                          IFX_SSC_ASYNC_CALLBACK_t *pCallback,
 *                          char *txbuf, int txsize, char *rxbuf, int rxsize)
 * \brief Transmit/receive to/from SSC in one step. It performs the data
 *  transmission and then the data reception.

 * The SSC driver sets the chipselect when the data transmission starts and
 * resets it when the transmission is completed.
 * This routine is called to start an asynchronous data transmission.
 * The provided callback routine is called after the transmission is done.
 *
 * \param   handler    Handle of the connection where to make the configuration
 *                     on
 * \param   pCallback  Function callback that is called after the request is
 *                     performed.
 * \param   txbuf      Pointer to the data packet to transmit.
 * \param   txsize     Amount of Bytes to transmit.
 * \param   rxbuf      Pointer to store the received data packet.
 * \param   rxsize     Amount of Bytes to receive.

 * \return Returns the amount of bytes that are transmitted in case of
 *  successful transmission. In case of an error the function returns (-1).
 *
 * \remarks This function activates the chipselect before data transmission and
 *  deactivates it after transmission (when function returns). This is done
 *  with  the callback function. The chipselect is not modified in case
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscAsyncTxRx(IFX_SSC_HANDLE handler,
		 IFX_SSC_ASYNC_CALLBACK_t *pCallback, char *txbuf,
		 int txsize, char *rxbuf, int rxsize)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg;
	IFX_SSC_QUEUE_t *pqueue;
	struct ifx_ssc_port *port;
	struct ifx_ssc_device_stats *stats;

	if (ifx_validate_params(handler, txbuf, txsize, rxbuf, rxsize) < 0)
		return -EINVAL;

	dev = (ssc_device_t *) handler;
	stats = &dev->stats;
	port = dev->port;

	if (!in_interrupt()) {
		stats->context_err++;
		LOGF_KLOG_ERROR("%s must be called in interrupt context <irq, softirq, tasklet>\n",
			__func__);
		return -1;
	}

	ssc_cfg = &dev->conn_id;
	pqueue = &dev->queue;

	if (txsize > ssc_cfg->fragSize || rxsize > ssc_cfg->fragSize) {
		stats->frag_err++;
		LOGF_KLOG_ERROR("%s Device driver must do its own fragmentation tx %d rx %d > %d\n",
			__func__, txsize, rxsize, ssc_cfg->fragSize);
		return -1;
	}

	if ((txsize != 0) && (rxsize != 0)) {
		if (ssc_cfg->duplex_mode == IFX_SSC_FULL_DUPLEX) {
			if (txsize != rxsize) {
				stats->dlx_err++;
				LOGF_KLOG_ERROR("%s full duplex required TX/RX has the same amount of data tx %d rx %d\n",
					__func__, txsize, rxsize);
				return -1;
			}
		}
	}

	/*
	 * Ensure that only asynchronous SSC Handles could enqueue an
	 * asynchronous request. The parameter 'sscHandleType' is set during
	 * the ConnId allocation process.
	 */
	if (pqueue->handle_type != IFX_SSC_HANDL_TYPE_ASYNC) {
		stats->handler_err++;
		LOGF_KLOG_ERROR("%s must use async handler\n", __func__);
		return -1;
	}

	/*
	 * Check if the queue entry of the ConnId is already queued with a
	 * request but this request is not served yet. Every ConnId can only
	 * queue up one request at the time.
	 */
	if (atomic_read(&pqueue->isqueued) == 1) {
		stats->dup_qentries++;
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s same queue has queued more than once\n",
			__func__);
		return -1;
	}
	IFX_SSC_Q_LOCK_BH(port);
	/* Add pointer and sizes to the queue entry of this SSC handle. */
	pqueue->txbuf = txbuf;
	pqueue->txsize = txsize;
	pqueue->rxbuf = rxbuf;
	pqueue->rxsize = rxsize;
	if (pCallback != NULL)
		pqueue->callback = *pCallback;

	pqueue->request_lock = false;
	pqueue->exchange_bytes = 0;
	atomic_set(&pqueue->isqueued, 1);
	IFX_SSC_Q_UNLOCK_BH(port);

	/* Add queue entry to priority synchronous queue */
	ifx_ssc_enqueue(pqueue);

	/*
	 * Trigger schedule or tasklet or fake interrupt according to different
	 * cases.
	 */
	ifx_ssc_start_tasklet(port);
	return 0;
}
EXPORT_SYMBOL(ifx_sscAsyncTxRx);

/**
 * \fn int ifx_sscAsyncTx(IFX_SSC_HANDLE handler,
 *                        IFX_SSC_ASYNC_CALLBACK_t *pCallback,
 *                        char *txbuf, int txsize)
 * \brief transmit the data, located at "txbuf". The "txsize" amount of bytes
 * is transmitted over SSC.

 * The SSC driver sets the chipselect when the data transmission starts and
 * resets it when the transmission is completed.
 * This routine is called to start an asynchronous data transmission.
 * The provided callback routine is called after the transmission is done.
 *
 * \param   handler    Handle of the connection where to make the configuration
 *                     on
 * \param   pCallback  Function callback that is called after the request is
 *                     performed.
 * \param   txbuf      Pointer to the data packet to transmit.
 * \param   txsize     Amount of Bytes to transmit.

 * \return Returns the amount of bytes that are transmitted in case of
 *  successful transmission. In case of an error the function returns (-1).
 *
 * \remarks This function activates the chipselect before data transmission and
 *  deactivates it after transmission (when function returns). This is done
 *  with the callback function. The chipselect is not modified in case
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscAsyncTx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
	     char *txbuf, int txsize)
{
	return ifx_sscAsyncTxRx(handler, pCallback, txbuf, txsize, NULL, 0);
}
EXPORT_SYMBOL(ifx_sscAsyncTx);

/**
 * \fn int ifx_sscAsyncRx(IFX_SSC_HANDLE handler,
 *                        IFX_SSC_ASYNC_CALLBACK_t *pCallback,
 *                        char *rxbuf, int rxsize)
 * \brief Receive from SSC. The received data are stored at "rxbuf".
 * The "rxsize" describes the amount of bytes to receive from SSC.

 * The SSC driver sets the chipselect when the data reception starts and
 * resets it when the reception is completed.
 *
 * \param   handler    Handle of the connection where to make the
 *                     configuration on
 * \param   pCallback  Function callback that is called after the request is
 *                     performed.
 * \param   rxbuf      Pointer to store the received data packet.
 * \param   rxsize     Amount of Bytes to receive.

 * \return  Returns the amount of bytes that are received in case of successful
   transmission. In case of an error the function returns (-1). (-1).
 *
 * \remarks This function activates the chipselect before data transmission and
 *  deactivates it after transmission (when function returns). This is done
 *  with the callback function. The chipselect is not modified in case
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscAsyncRx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
	     char *rxbuf, int rxsize)
{
	return ifx_sscAsyncTxRx(handler, pCallback, NULL, 0, rxbuf, rxsize);
}
EXPORT_SYMBOL(ifx_sscAsyncRx);

/**
 * \fn int ifx_sscAsyncLock(IFX_SSC_HANDLE handler,
 *                          IFX_SSC_ASYNC_CALLBACK_t *pCallback)
 *
 * \brief This function locks and reserves the whole SSC interface for the
 * given 'handler'.

 *  The chipselect, belonging to this SSC session is already
 *  activated. This means the chipselect callback is called.
 *  After complete data transmission and reception,
 *  This routine is called to get the lock on tasklet level.
 *  The provided callback routine is called after the lock is set.
 *
 * \param handler Handle of the connection.
 * \param pCallback Function callback that is called after the request is
 *        performed.
 *
 * \return Return (0) in case of success, otherwise (-1) in case of errors.
 *
 * \remarks This function actives the SSC chipselect of this 'handler' by
 * calling the callback function.
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscAsyncLock(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg;
	struct ifx_ssc_port *port;
	IFX_SSC_QUEUE_t *pqueue;

	SSC_KASSERT((handler != NULL),
		("%s Invalid parameter\n", __func__));
	SSC_KASSERT((pCallback != NULL),
		("%s Invalid parameter\n", __func__));

	dev = (ssc_device_t *) handler;
	pqueue = &dev->queue;
	ssc_cfg = &dev->conn_id;
	port = dev->port;

	if (port->lock_qentry == pqueue) {
		/* We hold the lock already -> nothing to request here! */
		return -1;
	}

	/*
	 * Check if the queue entry of the ConnId is already queued with a
	 * request but this request is not served yet. Every ConnId can
	 * only queue up one request at the time.
	 */
	if (atomic_read(&pqueue->isqueued) == 1)
		return -1;

	IFX_SSC_Q_LOCK_BH(port);
	pqueue->txbuf = NULL;
	pqueue->txsize = 0;
	pqueue->rxbuf = NULL;
	pqueue->rxsize = 0;
	/* Place a lock request in the queue */
	pqueue->request_lock = true;
	pqueue->callback = *pCallback;
	pqueue->exchange_bytes = 0;
	atomic_set(&pqueue->isqueued, 1);
	IFX_SSC_Q_UNLOCK_BH(port);

	/* Add queue entry to priority synchronous queue */
	ifx_ssc_enqueue(pqueue);

	/*
	 * Calls the internal process to serve the queue. This routine would
	 * immediately return in case  the SSC hardware is currently used to
	 * serve another request.
	 */
	ifx_ssc_start_tasklet(port);
	return 0;
}
EXPORT_SYMBOL(ifx_sscAsyncLock);

/**
 * \fn int ifx_sscAsyncUnLock(IFX_SSC_HANDLE handler)
 * \brief This function releases the SSC lock that was placed before by calling
 * \ref ifx_sscAsyncLock. This function also inactivate the chipselect signal,
 *  which was set in \ref ifx_sscAsyncLock.

 * \param handler Handle of the connection.
 *
 * \return Return (0) in case of success, otherwise (-1) in case of errors.
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscAsyncUnLock(IFX_SSC_HANDLE handler)
{
	ssc_device_t *dev;
	IFX_SSC_CONFIGURE_t *ssc_cfg = NULL;
	struct ifx_ssc_port *port = NULL;
	IFX_SSC_QUEUE_t *pqueue;

	SSC_KASSERT((handler != NULL),
		("%s Invalid parameter\n", __func__));

	dev = (ssc_device_t *) handler;
	ssc_cfg = &dev->conn_id;
	pqueue = &dev->queue;
	port = dev->port;

	if (port->lock_qentry != pqueue) {
		/* We do not hold the lock, therefore we can not release it! */
		return -1;
	}

	/* Just forget about the lock, then the SSC driver would just take it
	 * as a normel queue entry
	 */
	ifx_ssc_cs_unlock(dev);
	/* XXX, Possible race condition if called in in_irq() */
	IFX_SSC_Q_LOCK_BH(port);
	pqueue->txbuf = NULL;
	pqueue->txsize = 0;
	pqueue->rxbuf = NULL;
	pqueue->rxsize = 0;
	pqueue->exchange_bytes = 0;
	memset(&pqueue->callback, 0, sizeof(IFX_SSC_ASYNC_CALLBACK_t));
	port->lock_qentry = NULL;
	IFX_SSC_Q_UNLOCK_BH(port);
	/* Restart tasklet since we are async release SPI bus */
	ifx_ssc_start_tasklet(port);
	return 0;
}
EXPORT_SYMBOL(ifx_sscAsyncUnLock);

#ifdef CONFIG_SYSCTL

#define SSC_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(struct ctl_table * ctl, int write, \
	void __user *buffer, size_t *lenp, loff_t *ppos)
#define SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, buffer, lenp, ppos)

/* SSC Client driver proc entry for parameter configuration */
enum {
	IFX_SSC_PRIV_FRAGMENT_SIZE = 1,
	IFX_SSC_PRIV_FIFO_SIZE = 2,
	IFX_SSC_PRIV_BAUDRATE = 3,
	IFX_SSC_PRIV_MODE = 4,
};

static int
SSC_SYSCTL_DECL(ssc_sysctl_private, ctl, write, filp, buffer, lenp, ppos)
{
	ssc_device_t *dev = ctl->extra1;
	IFX_SSC_CONFIGURE_t *ssc_cfg = &dev->conn_id;
	struct ifx_ssc_port *port;
	int ret;

	port = dev->port;
	if (write) {
		ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,
				lenp, ppos);
		if (ret == 0) {
			switch ((long) ctl->extra2) {
			case IFX_SSC_PRIV_FRAGMENT_SIZE:
				if (global_val < IFX_SSC_MIN_FRAGSIZE
				    || global_val > IFX_SSC_MAX_FRAGSIZE)
					return -EINVAL;
				ssc_cfg->fragSize = global_val;
				port->ssc_fragSize = global_val;
				break;

			case IFX_SSC_PRIV_FIFO_SIZE:
				if (global_val < IFX_SSC_FIFO_MIN_THRESHOULD
				    || global_val > IFX_SSC_FIFO_MAX_THRESHOULD)
					return -EINVAL;
				ssc_cfg->maxFIFOSize = global_val;
				break;

			case IFX_SSC_PRIV_BAUDRATE:
				/* XXX, sanity check */
				ssc_cfg->baudrate = global_val;
				break;

			case IFX_SSC_PRIV_MODE:
				ret = -EINVAL;
				break;

			default:
				return -EINVAL;
			}
		}
	} else {
		switch ((long) ctl->extra2) {
		case IFX_SSC_PRIV_FRAGMENT_SIZE:
			global_val = ssc_cfg->fragSize;
			break;

		case IFX_SSC_PRIV_FIFO_SIZE:
			global_val = ssc_cfg->maxFIFOSize;
			break;

		case IFX_SSC_PRIV_BAUDRATE:
			global_val = ssc_cfg->baudrate;
			break;

		case IFX_SSC_PRIV_MODE:
			global_val = ssc_cfg->ssc_mode;
			break;

		default:
			return -EINVAL;
		}
		ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,
				lenp, ppos);
	}
	return ret;
}

static const struct ctl_table ssc_sysctl_template[] = {
	/* NB: must be last entry before NULL */
	{IFX_INIT_CTL_NAME(CTL_AUTO)
	 .procname = "fragment_size",
	 .mode = 0644,
	 .data = &global_val,
	 .maxlen = sizeof(global_val),
	 .proc_handler = ssc_sysctl_private,
	 .extra2 = (void *) IFX_SSC_PRIV_FRAGMENT_SIZE,
	 },
	{IFX_INIT_CTL_NAME(CTL_AUTO)
	 .procname = "fifosize",
	 .mode = 0644,
	 .data = &global_val,
	 .maxlen = sizeof(global_val),
	 .proc_handler = ssc_sysctl_private,
	 .extra2 = (void *) IFX_SSC_PRIV_FIFO_SIZE,
	 },
	{IFX_INIT_CTL_NAME(CTL_AUTO)
	 .procname = "baudrate",
	 .mode = 0644,
	 .data = &global_val,
	 .maxlen = sizeof(global_val),
	 .proc_handler = ssc_sysctl_private,
	 .extra2 = (void *) IFX_SSC_PRIV_BAUDRATE,
	 },
	{IFX_INIT_CTL_NAME(CTL_AUTO)
	 .procname = "spimode",
	 .mode = 0644,
	 .data = &global_val,
	 .maxlen = sizeof(global_val),
	 .proc_handler = ssc_sysctl_private,
	 .extra2 = (void *) IFX_SSC_PRIV_MODE,
	 },
	{0}
};

static void ifx_ssc_sysctl_attach(ssc_device_t *dev)
{
	int i, space;

	space = 5 * sizeof(struct ctl_table) + sizeof(ssc_sysctl_template);
	dev->ssc_sysctls = kmalloc(space, GFP_KERNEL);
	if (dev->ssc_sysctls == NULL) {
		LOGF_KLOG_ERROR("%s: no memory for sysctl table!\n", __func__);
		return;
	}

	/* setup the table */
	memset(dev->ssc_sysctls, 0, space);
	IFX_SET_CTL_NAME(dev->ssc_sysctls[0], CTL_DEV);
	dev->ssc_sysctls[0].procname = "dev";
	dev->ssc_sysctls[0].mode = 0555;
	dev->ssc_sysctls[0].child = &dev->ssc_sysctls[2];
	/* [1] is NULL terminator */
	IFX_SET_CTL_NAME(dev->ssc_sysctls[2], CTL_AUTO);
	dev->ssc_sysctls[2].procname = dev->dev_name;
	dev->ssc_sysctls[2].mode = 0555;
	dev->ssc_sysctls[2].child = &dev->ssc_sysctls[4];
	/* [3] is NULL terminator */
	/* copy in pre-defined data */
	memcpy(&dev->ssc_sysctls[4], ssc_sysctl_template,
		sizeof(ssc_sysctl_template));

	/* add in dynamic data references */
	for (i = 4; dev->ssc_sysctls[i].procname; i++) {
		if (dev->ssc_sysctls[i].extra1 == NULL)
			dev->ssc_sysctls[i].extra1 = dev;
	}

	/* tack on back-pointer to parent device */
	dev->ssc_sysctls[i - 1].data = dev->dev_name;

	/* and register everything */
	dev->ssc_sysctl_header =
		IFX_REGISTER_SYSCTL_TABLE(dev->ssc_sysctls);
	if (dev->ssc_sysctl_header == NULL) {
		LOGF_KLOG_ERROR("%s: failed to register sysctls!\n", dev->dev_name);
		kfree(dev->ssc_sysctls);
		dev->ssc_sysctls = NULL;
	}
}

static void ifx_ssc_sysctl_detach(ssc_device_t *dev)
{
	if (dev->ssc_sysctl_header != NULL) {
		unregister_sysctl_table(dev->ssc_sysctl_header);
		dev->ssc_sysctl_header = NULL;
	}
	if (dev->ssc_sysctls != NULL) {
		kfree(dev->ssc_sysctls);
		dev->ssc_sysctls = NULL;
	}
}

/* SSC Driver itself proc support for debug and future configuration */
enum {
	IFX_SSC_PRIV_DEBUG = 1,
};

static int
SSC_SYSCTL_DECL(port_sysctl_private, ctl, write, filp, buffer, lenp, ppos)
{
	struct ifx_ssc_port *port = ctl->extra1;
	int ret;

	if (write) {
		ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,
					lenp, ppos);
		if (ret == 0) {
			switch ((long) ctl->extra2) {
			case IFX_SSC_PRIV_DEBUG:
				port->ssc_debug = global_val;
				break;

			default:
				return -EINVAL;
			}
		}
	} else {
		switch ((long) ctl->extra2) {
		case IFX_SSC_PRIV_DEBUG:
			global_val = port->ssc_debug;
			break;

		default:
			return -EINVAL;
		}
		ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,
				lenp, ppos);
	}
	return ret;
}

static const struct ctl_table port_sysctl_template[] = {
	/* NB: must be last entry before NULL */
	{IFX_INIT_CTL_NAME(CTL_AUTO)
	 .procname = "debug",
	 .mode = 0644,
	 .proc_handler = port_sysctl_private,
	 .extra2 = (void *) IFX_SSC_PRIV_DEBUG,
	 },
	{0}
};

static void ifx_ssc_port_sysctl_attach(struct ifx_ssc_port *port)
{
	int i, space;

	space = 5 * sizeof(struct ctl_table) +
		sizeof(port_sysctl_template);
	port->port_sysctls = kmalloc(space, GFP_KERNEL);
	if (port->port_sysctls == NULL) {
		LOGF_KLOG_ERROR("%s: no memory for sysctl table!\n", __func__);
		return;
	}

	/* setup the table */
	memset(port->port_sysctls, 0, space);
	IFX_SET_CTL_NAME(port->port_sysctls[0], CTL_DEV);
	port->port_sysctls[0].procname = "dev";
	port->port_sysctls[0].mode = 0555;
	port->port_sysctls[0].child = &port->port_sysctls[2];
	/* [1] is NULL terminator */
	IFX_SET_CTL_NAME(port->port_sysctls[2], CTL_AUTO);
	port->port_sysctls[2].procname = port->name;
	port->port_sysctls[2].mode = 0555;
	port->port_sysctls[2].child = &port->port_sysctls[4];
	/* [3] is NULL terminator */
	/* copy in pre-defined data */
	memcpy(&port->port_sysctls[4], port_sysctl_template,
		sizeof(port_sysctl_template));

	/* add in dynamic data references */
	for (i = 4; port->port_sysctls[i].procname; i++) {
		if (port->port_sysctls[i].extra1 == NULL)
			port->port_sysctls[i].extra1 = port;
	}

	/* tack on back-pointer to parent device */
	port->port_sysctls[i - 1].data = port->name;

	/* and register everything */
	port->port_sysctl_header =
		IFX_REGISTER_SYSCTL_TABLE(port->port_sysctls);
	if (port->port_sysctl_header == NULL) {
		LOGF_KLOG_ERROR("%s: failed to register sysctls!\n", port->name);
		kfree(port->port_sysctls);
		port->port_sysctls = NULL;
	}
}

static void ifx_ssc_port_sysctl_detach(struct ifx_ssc_port *port)
{
	if (port->port_sysctl_header != NULL) {
		unregister_sysctl_table(port->port_sysctl_header);
		port->port_sysctl_header = NULL;
	}
	if (port->port_sysctls != NULL) {
		kfree(port->port_sysctls);
		port->port_sysctls = NULL;
	}
}
#endif /* CONFIG_SYSCTL */

/**
 *\fn IFX_SSC_HANDLE ifx_sscAllocConnection (char *dev_name,
 *    IFX_SSC_CONFIGURE_t *connid)
 *\brief Allocate and create a Connection ID "ConnId"
 *
 * Allocate and create a Connection ID "ConnId" to communicate over SSC.
 * This ConnId is needed for all remaining SSC driver API calls. This
 * ConnId is a handle that helps the SSC driver to find the configuration
 * that belongs to the connection. ConnId specific parameters are e.g.
 * Baudrate, Priority, Chipselect Callback, etc.
 *
 * \param   dev_name    unique name for this connection. If null, will alloc
 *                      one unique name automatically
 * \param   connid      Connectin id
 * \return  a handle "IFX_SSC_HANDLE" in case the allocation was successful.
 *          In case of an error, the return handle is zero (NULL).
 * \ingroup IFX_SSC_FUNCTIONS
 */
IFX_SSC_HANDLE
ifx_sscAllocConnection(char *dev_name, IFX_SSC_CONFIGURE_t *connid)
{
	struct ifx_ssc_port *port;
	ssc_device_t *p;
	ssc_device_t *q;
	IFX_SSC_QUEUE_t *queue;
	char buf[IFX_SSC_MAX_DEVNAME] = { 0 };
	char *pName;
	int port_num, result = 0;

	result = find_port(dev_name, &port_num);
	if (result == ENODEV) {
		port_num = 0;
	}
	if (ifx_ssc_isp[port_num] == NULL) {
		LOGF_KLOG_ERROR("%s ssc driver must be loaded first!\n",
			__func__);
		return NULL;
	}

	port = ifx_ssc_isp[port_num];	/* XXX */

	if (port->ssc_ndevs >= IFX_SSC_MAX_DEVICE) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s device number out of range\n", __func__);
		return NULL;
	}

	if (connid == NULL) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s must provide connection portrmation!\n",
			__func__);
		return NULL;
	}

	if ((connid->ssc_mode < IFX_SSC_MODE_0)
	    || (connid->ssc_mode > IFX_SSC_MODE_3)) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s invalid spi mode <%d~%d>!\n", __func__,
			IFX_SSC_MODE_0, IFX_SSC_MODE_3);
		return NULL;
	}

	if (connid->ssc_prio < IFX_SSC_PRIO_LOW
	    || (connid->ssc_prio > IFX_SSC_PRIO_MAX)) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s invalid priority <%d~%d>!\n", __func__,
			IFX_SSC_PRIO_LOW, IFX_SSC_PRIO_MAX);
	}


	if (connid->csset_cb == NULL) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s must provide cs function\n", __func__);
		return NULL;
	}

	if (connid->fragSize < IFX_SSC_MIN_FRAGSIZE
	    || connid->fragSize > IFX_SSC_MAX_FRAGSIZE) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s %d invalid fragment size <%d~%d>!\n",
			__func__, connid->fragSize,
			IFX_SSC_MIN_FRAGSIZE, IFX_SSC_MAX_FRAGSIZE);
		return NULL;
	}

	if (connid->maxFIFOSize < IFX_SSC_FIFO_MIN_THRESHOULD
	    || connid->maxFIFOSize > IFX_SSC_FIFO_MAX_THRESHOULD) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s %d invalid fifo size <%d~%d>!\n",
			__func__, connid->maxFIFOSize,
			IFX_SSC_FIFO_MIN_THRESHOULD,
			IFX_SSC_FIFO_MAX_THRESHOULD);
		return NULL;
	}

	if (connid->duplex_mode != IFX_SSC_FULL_DUPLEX
	    && connid->duplex_mode != IFX_SSC_HALF_DUPLEX) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s %d invalid duplex mode <%d~%d>!\n",
			__func__, connid->duplex_mode,
			IFX_SSC_FULL_DUPLEX, IFX_SSC_HALF_DUPLEX);
		return NULL;
	}
	/* If no name specified, will assign one name for identification */
	if (dev_name == NULL) {
		sprintf(buf, "ssc%d", port->ssc_ndevs);
		pName = buf;
	} else {
		if (strlen(dev_name) > (IFX_SSC_MAX_DEVNAME - 1)) {
			IFX_SSC_PRINT(port, SSC_MSG_ERROR,
				"%s device name is too long\n",
				__func__);
			return NULL;
		}
		pName = dev_name;
	}

	p = kmalloc(sizeof(ssc_device_t), GFP_KERNEL);
	if (p == NULL) {
		IFX_SSC_PRINT(port, SSC_MSG_ERROR,
			"%s failed to allocate memory\n", __func__);
		return NULL;
	}
	memset(p, 0, sizeof(ssc_device_t));

	IFX_SSC_SEM_LOCK(port->dev_sem);
	TAILQ_FOREACH(q, &port->ssc_devq, dev_entry) {
		if (strcmp(q->dev_name, pName) == 0) {
			kfree(p);
			IFX_SSC_SEM_UNLOCK(port->dev_sem);
			IFX_SSC_PRINT(port, SSC_MSG_ERROR,
				"%s device registered already!\n",
				__func__);
			return NULL;
		}
	}
	IFX_SSC_SEM_UNLOCK(port->dev_sem);

	/* Follow net device driver name rule */
	memcpy(p->dev_name, pName, IFX_SSC_MAX_DEVNAME);
	p->duplex = connid->duplex_mode;
	memcpy((char *) &p->conn_id, (char *) connid,
		sizeof(IFX_SSC_CONFIGURE_t));

	queue = &p->queue;
	/* Queue handler type converted from priority */
	if (connid->ssc_prio == IFX_SSC_PRIO_ASYNC)
		queue->handle_type = IFX_SSC_HANDL_TYPE_ASYNC;
	else
		queue->handle_type = IFX_SSC_HANDL_TYPE_SYNC;

	/* Back pointer to later usage */
	queue->dev = p;
	atomic_set(&queue->isqueued, 0);
	queue->request_lock = false;
	/*
	 * Just for fast access, priority based on device, instead of packet
	 * Still keep per packet priority there for future change.
	 */
	p->dev_prio = connid->ssc_prio;

	IFX_SSC_WAKELIST_INIT(p->dev_thread_wait);

	p->port = port; /* back pointer to port for easy reference later */
	port->ssc_ndevs++;
#ifdef CONFIG_SYSCTL
	ifx_ssc_sysctl_attach(p);
#endif /* CONFIG_SYSCTL */
	IFX_SSC_SEM_LOCK(port->dev_sem);
	TAILQ_INSERT_TAIL(&port->ssc_devq, p, dev_entry);
	IFX_SSC_SEM_UNLOCK(port->dev_sem);

	/* Make sure very device CS in default state on registration */
	if (connid->csset_cb != NULL)
		connid->csset_cb(IFX_SSC_CS_OFF, connid->cs_data);

	IFX_SSC_PRINT(port, SSC_MSG_INIT,
		"%s: device %s register sucessfully!\n", __func__,
		p->dev_name);
	return (IFX_SSC_HANDLE) p;
}
EXPORT_SYMBOL(ifx_sscAllocConnection);

/**
 *\fn int ifx_sscFreeConnection (IFX_SSC_HANDLE handler)
 *\brief Release ssc connnection
 *
 * Release a ConnId handle that was allocated by the function
 * ifx_SscAllocConnection before. An allocated ConnId has to
 * be released by the client driver module when the SSC driver
 * is not used anymore. Note that all allocated ConnId's should
 * be released before the SSC driver is unloaded from the kernel.
 *
 * \param   handler    ConnId handle allocated by ifx_SscAllocConnection
 * \returns (0) in case of success, otherwise (-1) in case of errors.
 * \ingroup IFX_SSC_FUNCTIONS
 */
int ifx_sscFreeConnection(IFX_SSC_HANDLE handler)
{
	ssc_device_t *p;
	struct ifx_ssc_port *port;
	ssc_device_t *q, *next;

	SSC_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

	p = (ssc_device_t *) handler;
	port = p->port;
	IFX_SSC_SEM_LOCK(port->dev_sem);
	TAILQ_FOREACH_SAFE(q, &port->ssc_devq, dev_entry, next) {
		if (strcmp(q->dev_name, p->dev_name) == 0) {
			#ifdef EXPERIMENTAL_FIX
			/* extra check if someone call ifx_sscLock or ifx_sscAsyncLock to */
			/* force lock the port to his driver but forgot to release it */
			if (port->lock_qentry != NULL &&
			    port->lock_qentry == &p->queue) {
				IFX_SSC_PRINT(port, SSC_MSG_ERROR,
				"[%s] device hold the queue lock, Force Unlocking... \n",
				p->dev_name);
				ifx_sscUnlock(handler);
				//ifx_sscAsyncUnLock(handler); /* will restart the tasklet as well??? */
			}
			#endif /* EXPERIMENTAL_FIX */
			TAILQ_REMOVE(&port->ssc_devq, q, dev_entry);
		#ifdef CONFIG_SYSCTL
			ifx_ssc_sysctl_detach(q);
		#endif /* CONFIG_SYSCTL */
			kfree(q);
			port->ssc_ndevs--;
			IFX_SSC_SEM_UNLOCK(port->dev_sem);
			IFX_SSC_PRINT(port, SSC_MSG_INIT,
				"%s device %s unregistered\n",
				__func__, p->dev_name);
			return 0;
		}
	}
	IFX_SSC_SEM_UNLOCK(port->dev_sem);
	return -1;
}
EXPORT_SYMBOL(ifx_sscFreeConnection);

/**
 * \fn static void ifx_ssc_proc_init(void)
 * \brief Create ssc proc directory and file when module initialized.
 *
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_proc_init(int port)
{
	struct proc_dir_entry *entry;
	char buf[20];
	snprintf(buf, sizeof(buf), "%s%d", "driver/ltq_ssc", port);
	ifx_ssc_proc = proc_mkdir(buf, NULL);
	if (!ifx_ssc_proc)
		return -ENOMEM;

	entry = proc_create("stats", 0,
			ifx_ssc_proc, &ifx_ssc_stats_proc_fops);
	if (!entry)
		goto err1;

	entry = proc_create("reg", 0,
			ifx_ssc_proc, &ifx_ssc_reg_proc_fops);
	if (!entry)
		goto err2;
	return 0;
err2:
	remove_proc_entry("stats", ifx_ssc_proc);
err1:
	remove_proc_entry(buf, NULL);
	return -ENOMEM;
}

/**
 * \fn static void ifx_ssc_proc_exit(void)
 * \brief Delete ssc proc directory and file.
 *
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void ifx_ssc_proc_exit(int port)
{

	char buf[20];
	snprintf(buf, sizeof(buf), "%s%d", "driver/ltq_ssc", port);
	remove_proc_entry("stats", ifx_ssc_proc);

	remove_proc_entry("reg", ifx_ssc_proc);

	remove_proc_entry(buf, NULL);
}

/**
 * \fn static int __init ifx_ssc_init (void)
 * \brief SSC module Initialization.
 *
 * \return -ENOMEM  Failed to allocate memory
 * \return -EBUSY   Failed to iomap register space
 * \return 0        OK
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_init(struct platform_device *pdev)
{
	u32 reg;
	struct ifx_ssc_port *port;
	int j;
	int ret_val = -ENOMEM;
	/*static int ifx_ssc_initialized;*/
	struct clk *clk;
	struct resource *r;
	u32 id;
	static int ifx_ssc_init_count;
	char buf[16];

#ifdef CONFIG_SOC_GRX500
	const __be32 *port_num = of_get_property(pdev->dev.of_node, "lantiq,ssc-port", NULL);
	if (!port_num) {
		ifx_ssc_init_count = 0;
		LOGF_KLOG_ERROR("node lantiq,ssc-port not found\r\n");
	} else
		ifx_ssc_init_count = *port_num;

	const __be32 *cs_port_num = of_get_property(pdev->dev.of_node, "legacy,cs-api-port", NULL);
	if ( cs_port_num &&
		 *cs_port_num <= (IFX_SSC_MAX_PORT_NUM - 1)) {
		legacy_cs_port_num = ((s32)(*cs_port_num));
		LOGF_KLOG_INFO("[%s] force ifx_ssc_cs_low to PORT %d\r\n",  __func__, legacy_cs_port_num);
		LOGF_KLOG_INFO("[%s] force ifx_ssc_cs_high to PORT %d\r\n", __func__, legacy_cs_port_num);
	}

#endif
	LOGF_KLOG_INFO("%s ifx_ssc_init_count %d\r\n", __func__, ifx_ssc_init_count);

	if (ifx_ssc_init_count > (IFX_SSC_MAX_PORT_NUM - 1)) {
		ret_val = -ENODEV;
		return ret_val;
	}

	ifx_ssc_isp[ifx_ssc_init_count] = kzalloc(sizeof(struct ifx_ssc_port), GFP_KERNEL);
	if (ifx_ssc_isp[ifx_ssc_init_count] == NULL) {
		LOGF_KLOG_ERROR("%s: no memory for isp\n", __func__);
		return ret_val;
	}
	port = ifx_ssc_isp[ifx_ssc_init_count];
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		LOGF_KLOG_DEV_ERROR(&pdev->dev, "platform_get_resource\n");
		ret_val = -ENOENT;
		return ret_val;
	}

	r = devm_request_mem_region(&pdev->dev, r->start, resource_size(r),
			pdev->name);
	if (!r) {
		LOGF_KLOG_DEV_ERROR(&pdev->dev, "failed to request memory region\n");
		ret_val = -ENXIO;
		return ret_val;
	}
	port->membase = devm_ioremap_nocache(&pdev->dev,
		r->start, resource_size(r));
	if (!port->membase) {
		LOGF_KLOG_DEV_ERROR(&pdev->dev, "failed to remap memory region\n");
		ret_val = -ENXIO;
		return ret_val;
	}
	#if 0
	if (ifx_ssc_initialized == 1)
		return 0;
	else
		ifx_ssc_initialized = 1;
	#endif
	port->port_idx = ifx_ssc_init_count;
	/* default values for the HwOpts */
	port->opts.abortErrDetect = IFX_SSC_DEF_ABRT_ERR_DETECT;
	port->opts.rxOvErrDetect = IFX_SSC_DEF_RO_ERR_DETECT;
	port->opts.rxUndErrDetect = IFX_SSC_DEF_RU_ERR_DETECT;
	port->opts.txOvErrDetect = IFX_SSC_DEF_TO_ERR_DETECT;
	port->opts.txUndErrDetect = IFX_SSC_DEF_TU_ERR_DETECT;
	port->opts.loopBack = IFX_SSC_DEF_LOOP_BACK;
	port->opts.echoMode = IFX_SSC_DEF_ECHO_MODE;
	port->opts.idleValue = IFX_SSC_DEF_IDLE_DATA;
	port->opts.clockPolarity = IFX_SSC_DEF_CLOCK_POLARITY;
	port->opts.clockPhase = IFX_SSC_DEF_CLOCK_PHASE;
	port->opts.headingControl = IFX_SSC_DEF_HEADING_CONTROL;
	port->opts.dataWidth = IFX_SSC_DEF_DATA_WIDTH;
	port->opts.modeRxTx = IFX_SSC_DEF_MODE_RXTX;
	port->opts.gpoCs = IFX_SSC_DEF_GPO_CS;
	port->opts.gpoInv = IFX_SSC_DEF_GPO_INV;
	port->opts.masterSelect = IFX_SSC_DEF_MASTERSLAVE;
	port->prev_ssc_clk = 0;
	port->baudrate = IFX_SSC_DEF_BAUDRATE;
	port->prev_baudrate = 0;
	port->prev_ssc_mode = IFX_SSC_MODE_UNKNOWN;
	port->ssc_ndevs = 0;
	port->ssc_fragSize = DEFAULT_SSC_FRAGMENT_SIZE;
	port->fpiclk = of_clk_get_by_name(pdev->dev.of_node, "freq");;
	if (IS_ERR(port->fpiclk)) {
		LOGF_KLOG_ERROR("%s Failed to get fpi clock\n", __func__);
		ret_val = PTR_ERR(port->fpiclk);
		goto err_clk;
	}

	/* Activate SSC */
	#if 0
	clk = clk_get_sys("1e100800.spi", NULL);
	#endif

	clk = of_clk_get_by_name(pdev->dev.of_node, "spi");;
	if (IS_ERR(clk)) {
		LOGF_KLOG_DEV_ERROR(&pdev->dev, "Failed to get clock\n");
		return PTR_ERR(clk);
	}
	clk_prepare_enable(clk);

	reg = IFX_SSC_GET_CLC(port);
	reg |= SM(IFX_SSC_DEF_RMC, IFX_SSC_CLC_RMC);
	reg &= ~IFX_SSC_CLC_DIS;
	IFX_SSC_SET_CLC(reg, port);

	id = IFX_SSC_GET_ID(port);
	if (of_machine_is_compatible("lantiq,ar10")
		|| of_machine_is_compatible("lantiq,grx390")
		|| of_machine_is_compatible("lantiq,ase"))
		port->dma_support = 0;
	else
		port->dma_support = 1;
	/* Either SSC tasklet or SSC kernel thread support, not both */
#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
	ifx_ssc_tasklet_init(port);
#else
	if (ifx_ssc_thread_init(port)) {
		LOGF_KLOG_ERROR("%s: unable to start ssc_kthread\n", __func__);
		goto errout1;
	}
#endif /* CONFIG_LANTIQ_SPI_ASYNCHRONOUS */
	if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
		char spi_name[16] = {0};
		if (port->port_idx == 0)
			sprintf(spi_name, "SPI");
		else
			sprintf(spi_name, "SPI%d", port->port_idx);
		/* Register with DMA engine */
		port->dma_dev = dma_device_reserve(spi_name);
		if (port->dma_dev == NULL) {
			LOGF_KLOG_ERROR("%s: Failed to reserve dma device!\n", __func__);
			goto errout1;
		}
		ifx_ssc_init_dma_device(port->port_idx, port->dma_dev);
#endif
	}

	snprintf(buf, sizeof(buf), "%s%d", IFX_SSC_NAME, ifx_ssc_init_count);
	strcpy(port->name, buf);
	LOGF_KLOG_INFO("port name %s", port->name);
	port->ssc_cs_locked = false;
	port->lock_qentry = NULL;
	port->serve_qentry = NULL;
#ifdef CONFIG_LANTIQ_SPI_DEBUG
	port->ssc_debug = SSC_MSG_ERROR;
#else
	port->ssc_debug = 0;
#endif /* CONFIG_LANTIQ_SPI_DEBUG */
	port->dma_is_in_half_duplex = 1;
	atomic_set(&port->dma_wait_state, 0);

	if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
		/* The following buffer allocation for HW WAR, last 1~3 bytes
		 * in DMA, It will make sure buffer will align on dma
		 * burst length
		 */
		port->dma_orig_txbuf = kmalloc(DEFAULT_SSC_FRAGMENT_SIZE
			+ ((port->dma_dev->tx_burst_len << 2) - 1),
			GFP_KERNEL);
		if (port->dma_orig_txbuf == NULL) {
			LOGF_KLOG_ERROR("%s: no memory for dma_orig_txbuf\n", __func__);
			goto errout2;
		}
		port->dma_txbuf = (char *) (((u32) (port->dma_orig_txbuf +
			((port->dma_dev->tx_burst_len << 2) - 1)))&~
			((port->dma_dev->tx_burst_len << 2) - 1));


		port->dma_orig_rxbuf = kmalloc(DEFAULT_SSC_FRAGMENT_SIZE
			+ ((port->dma_dev->rx_burst_len << 2) - 1), GFP_KERNEL);
		if (port->dma_orig_rxbuf == NULL) {
			LOGF_KLOG_ERROR("%s: no memory for dma_orig_rxbuf\n", __func__);
			goto errout3;
		}
		port->dma_rxbuf = (char *) (((u32) (port->dma_orig_rxbuf +
			((port->dma_dev->rx_burst_len << 2) - 1)))&~
			((port->dma_dev->rx_burst_len << 2) - 1));
#endif
	}

	/* Queue initialization */
	TAILQ_INIT(&port->ssc_devq);
	TAILQ_INIT(&port->ssc_asyncq);
	for (j = 0; j < IFX_SSC_PRIO_MAX; j++)
		TAILQ_INIT(&port->ssc_syncq[j]);

	IFX_SSC_Q_LOCK_INIT(port);
	IFX_SSC_SEM_INIT(port->dev_sem);
	IFX_SSC_WAKELIST_INIT(port->ssc_thread_wait);
	IFX_SSC_IRQ_LOCK_INIT(port, "ifx_ssc_lock");

	/* Disable SSC module level real hardware interrupts */
	IFX_SSC_SET_IRN_EN(0, port);

	/* init serial framing register */
	IFX_SSC_SET_FRAMING_CON(IFX_SSC_DEF_SFCON, port);

	if (ifx_ssc_hwinit(port) < 0) {
		LOGF_KLOG_ERROR("%s: hardware init failed\n", __func__);
		goto errout4;
	}
#ifdef CONFIG_SYSCTL
	ifx_ssc_port_sysctl_attach(port);
#endif /* CONFIG_SYSCTL */
	goto out;
errout4:
	if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
		kfree(port->dma_orig_rxbuf);
errout3:
		kfree(port->dma_orig_txbuf);
		dma_device_unregister(port->dma_dev);
		dma_device_release(port->dma_dev);
#endif
	}

	kthread_stop(port->ssc_tsk);
errout2:
	if (clk) {
		clk_disable(clk);
		clk_put(clk);
	}
errout1:
	if (port->fpiclk)
		clk_put(port->fpiclk);
err_clk:
	kfree(ifx_ssc_isp[ifx_ssc_init_count]);
	return ret_val;

out:
	ifx_ssc_proc_init(ifx_ssc_init_count);

	LOGF_KLOG_INFO("Lantiq SoC SSC controller rev %u (TXFS %u, RXFS %u, DMA %u)\n",
		id & IFX_SSC_ID_REV, port->tx_fifo_size_words,
		port->rx_fifo_size_words, port->dma_support);
	ifx_ssc_init_count++;

	return 0;
}

/**
 * \fn static void __exit ifx_ssc_exit (void)
 * \brief SSC Module Cleanup.
 *
 * Upon removal of the SSC module this function will free all allocated
 * resources and unregister devices.
 * \return none
 * \ingroup IFX_SSC_INTERNAL
 */
static int ifx_ssc_exit(struct platform_device *pdev)
{
	struct ifx_ssc_port *port;
	struct clk *clk;
	static int ifx_ssc_exit_count;

	/* free up any allocated memory */
	if (ifx_ssc_exit_count > (IFX_SSC_MAX_PORT_NUM - 1)) {
		return -ENODEV;
	}
	port = ifx_ssc_isp[ifx_ssc_exit_count];
	/* Disable the SSC */
	IFX_SSC_CONFIG_MODE(port);

	IFX_SSC_SEM_LOCK(port->dev_sem);
	if (!TAILQ_EMPTY(&port->ssc_devq)) {
		IFX_SSC_SEM_UNLOCK(port->dev_sem);
		LOGF_KLOG_ERROR("%s SSC devices still attached, please release them first\n",
			__func__);
		return -1;
	}
	IFX_SSC_SEM_UNLOCK(port->dev_sem);
#ifdef CONFIG_LANTIQ_SPI_ASYNCHRONOUS
	tasklet_kill(&port->ssc_txrxq);
#else
	if (port->ssc_tsk) {
		kthread_stop(port->ssc_tsk);
		port->ssc_tsk = NULL;
	}

#endif /* CONFIG_LANTIQ_SPI_ASYNCHRONOUS */
	IFX_SSC_IRQ_LOCK_DESTROY(port);
	IFX_SSC_Q_LOCK_DESTROY(port);
	if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
		struct dma_device_info *dma_dev;
		dma_dev = port->dma_dev;
		if (dma_dev != NULL) {
			dma_device_unregister(dma_dev);
			dma_device_release(dma_dev);
		}
#endif
	}
#ifdef CONFIG_SYSCTL
	ifx_ssc_port_sysctl_detach(port);
#endif /* CONFIG_SYSCTL */

	if (port->dma_support) {
#ifdef LTQ_DMA_SUPPORT
		kfree(port->dma_orig_rxbuf);
		kfree(port->dma_orig_txbuf);
#endif
	}
	iounmap(port->membase);

	kfree(ifx_ssc_isp[ifx_ssc_exit_count]);
	ifx_ssc_proc_exit(ifx_ssc_exit_count);
	clk = clk_get_sys("1e100800.spi", NULL);
	clk_disable(clk);
	clk_put(clk);
	ifx_ssc_exit_count++;
	return 0;
}

static const struct of_device_id ltq_spi_match[] = {
	{ .compatible = "lantiq,spi-ssc" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_spi_match);

static struct platform_driver ltq_spi_driver = {
	.probe = ifx_ssc_init,
	.remove = ifx_ssc_exit,
	.driver = {
		.name = "spi-ssc",
		.owner = THIS_MODULE,
		.of_match_table = ltq_spi_match,
	},
};

module_platform_driver(ltq_spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Chuanhua, chuanhua.lei@infineon.com");
MODULE_DESCRIPTION("IFX SSC driver");
MODULE_SUPPORTED_DEVICE("IFX SSC IP module");

