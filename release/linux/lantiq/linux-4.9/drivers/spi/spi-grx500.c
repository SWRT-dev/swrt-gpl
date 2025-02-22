/*
 * Lantiq SoC SPI controller
 *
 * Copyright (C) 2011 Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>
 * Copyright (C) 2012 John Crispin <blogic@openwrt.org>
 * 
 * This program is free software; you can distribute it and/or modify it
 * under the terms of the GNU General Public License (Version 2) as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/of_irq.h>

#define LTQ_SPI_CLC		0x00	/* Clock control */
#define LTQ_SPI_PISEL		0x04	/* Port input select */
#define LTQ_SPI_ID		0x08	/* Identification */
#define LTQ_SPI_CON		0x10	/* Control */
#define LTQ_SPI_STAT		0x14	/* Status */
#define LTQ_SPI_WHBSTATE	0x18	/* Write HW modified state */
#define LTQ_SPI_TB		0x20	/* Transmit buffer */
#define LTQ_SPI_RB		0x24	/* Receive buffer */
#define LTQ_SPI_RXFCON		0x30	/* Receive FIFO control */
#define LTQ_SPI_TXFCON		0x34	/* Transmit FIFO control */
#define LTQ_SPI_FSTAT		0x38	/* FIFO status */
#define LTQ_SPI_BRT		0x40	/* Baudrate timer */
#define LTQ_SPI_BRSTAT		0x44	/* Baudrate timer status */
#define LTQ_SPI_SFCON		0x60	/* Serial frame control */
#define LTQ_SPI_SFSTAT		0x64	/* Serial frame status */
#define LTQ_SPI_GPOCON		0x70	/* General purpose output control */
#define LTQ_SPI_GPOSTAT		0x74	/* General purpose output status */
#define LTQ_SPI_FGPO		0x78	/* Forced general purpose output */
#define LTQ_SPI_RXREQ		0x80	/* Receive request */
#define LTQ_SPI_RXCNT		0x84	/* Receive count */
#define LTQ_SPI_DMACON		0xEC	/* DMA control */
#define LTQ_SPI_IRNEN		0xF4	/* Interrupt node enable */
#define LTQ_SPI_IRNCR		0xF8	/* Interrupt node control */
#define LTQ_SPI_IRNICR		0xFC	/* Interrupt node interrupt capture */

#define LTQ_SPI_CLC_SMC_SHIFT	16	/* Clock divider for sleep mode */
#define LTQ_SPI_CLC_SMC_MASK	0xFF
#define LTQ_SPI_CLC_RMC_SHIFT	8	/* Clock divider for normal run mode */
#define LTQ_SPI_CLC_RMC_MASK	0xFF
#define LTQ_SPI_CLC_DISS	BIT(1)	/* Disable status bit */
#define LTQ_SPI_CLC_DISR	BIT(0)	/* Disable request bit */

#define LTQ_SPI_ID_TXFS_SHIFT	24	/* Implemented TX FIFO size */
#define LTQ_SPI_ID_TXFS_MASK	0x3F
#define LTQ_SPI_ID_RXFS_SHIFT	16	/* Implemented RX FIFO size */
#define LTQ_SPI_ID_RXFS_MASK	0x3F
#define LTQ_SPI_ID_REV_MASK	0x1F	/* Hardware revision number */
#define LTQ_SPI_ID_CFG		BIT(5)	/* DMA interface support */

#define LTQ_SPI_CON_BM_SHIFT	16	/* Data width selection */
#define LTQ_SPI_CON_BM_MASK	0x1F
#define LTQ_SPI_CON_LTCLKDEL_SHIFT	28	/*Latch clock delay */
#define LTQ_SPI_CON_LTCLKDEL_MASK	0xF
#define LTQ_SPI_CON_RX_SWP	BIT(26)	/* Rx Byte swap */
#define LTQ_SPI_CON_TX_SWP	BIT(25)	/* Tx Byte swap */
#define LTQ_SPI_CON_EM		BIT(24)	/* Echo mode */
#define LTQ_SPI_CON_IDLE	BIT(23)	/* Idle bit value */
#define LTQ_SPI_CON_ENBV	BIT(22)	/* Enable byte valid control */
#define LTQ_SPI_CON_RUEN	BIT(12)	/* Receive underflow error enable */
#define LTQ_SPI_CON_TUEN	BIT(11)	/* Transmit underflow error enable */
#define LTQ_SPI_CON_AEN		BIT(10)	/* Abort error enable */
#define LTQ_SPI_CON_REN		BIT(9)	/* Receive overflow error enable */
#define LTQ_SPI_CON_TEN		BIT(8)	/* Transmit overflow error enable */
#define LTQ_SPI_CON_LB		BIT(7)	/* Loopback control */
#define LTQ_SPI_CON_PO		BIT(6)	/* Clock polarity control */
#define LTQ_SPI_CON_PH		BIT(5)	/* Clock phase control */
#define LTQ_SPI_CON_HB		BIT(4)	/* Heading control */
#define LTQ_SPI_CON_RXOFF	BIT(1)	/* Switch receiver off */
#define LTQ_SPI_CON_TXOFF	BIT(0)	/* Switch transmitter off */

#define LTQ_SPI_STAT_RXBV_MASK	0x7
#define LTQ_SPI_STAT_RXBV_SHIFT	28
#define LTQ_SPI_STAT_BSY	BIT(13)	/* Busy flag */
#define LTQ_SPI_STAT_RUE	BIT(12)	/* Receive underflow error flag */
#define LTQ_SPI_STAT_TUE	BIT(11)	/* Transmit underflow error flag */
#define LTQ_SPI_STAT_AE		BIT(10)	/* Abort error flag */
#define LTQ_SPI_STAT_RE		BIT(9)	/* Receive error flag */
#define LTQ_SPI_STAT_TE		BIT(8)	/* Transmit error flag */
#define LTQ_SPI_STAT_MS		BIT(1)	/* Master/slave select bit */
#define LTQ_SPI_STAT_EN		BIT(0)	/* Enable bit */

#define LTQ_SPI_WHBSTATE_SETTUE	BIT(15)	/* Set transmit underflow error flag */
#define LTQ_SPI_WHBSTATE_SETAE	BIT(14)	/* Set abort error flag */
#define LTQ_SPI_WHBSTATE_SETRE	BIT(13)	/* Set receive error flag */
#define LTQ_SPI_WHBSTATE_SETTE	BIT(12)	/* Set transmit error flag */
#define LTQ_SPI_WHBSTATE_CLRTUE	BIT(11)	/* Clear transmit underflow error
						flag */
#define LTQ_SPI_WHBSTATE_CLRAE	BIT(10)	/* Clear abort error flag */
#define LTQ_SPI_WHBSTATE_CLRRE	BIT(9)	/* Clear receive error flag */
#define LTQ_SPI_WHBSTATE_CLRTE	BIT(8)	/* Clear transmit error flag */
#define LTQ_SPI_WHBSTATE_SETME	BIT(7)	/* Set mode error flag */
#define LTQ_SPI_WHBSTATE_CLRME	BIT(6)	/* Clear mode error flag */
#define LTQ_SPI_WHBSTATE_SETRUE	BIT(5)	/* Set receive underflow error flag */
#define LTQ_SPI_WHBSTATE_CLRRUE	BIT(4)	/* Clear receive underflow error flag */
#define LTQ_SPI_WHBSTATE_SETMS	BIT(3)	/* Set master select bit */
#define LTQ_SPI_WHBSTATE_CLRMS	BIT(2)	/* Clear master select bit */
#define LTQ_SPI_WHBSTATE_SETEN	BIT(1)	/* Set enable bit (operational mode) */
#define LTQ_SPI_WHBSTATE_CLREN	BIT(0)	/* Clear enable bit (config mode */
#define LTQ_SPI_WHBSTATE_CLR_ERRORS	0x0F50

#define LTQ_SPI_RXFCON_RXFITL_SHIFT	8 /* FIFO interrupt trigger level */
#define LTQ_SPI_RXFCON_RXFITL_MASK	0x3F
#define LTQ_SPI_RXFCON_RXFLU		BIT(1)	/* FIFO flush */
#define LTQ_SPI_RXFCON_RXFEN		BIT(0)	/* FIFO enable */

#define LTQ_SPI_TXFCON_TXFITL_SHIFT	8 /* FIFO interrupt trigger level */
#define LTQ_SPI_TXFCON_TXFITL_MASK	0x3F
#define LTQ_SPI_TXFCON_TXFLU		BIT(1)	/* FIFO flush */
#define LTQ_SPI_TXFCON_TXFEN		BIT(0)	/* FIFO enable */

#define LTQ_SPI_FSTAT_RXFFL_MASK	0x3f
#define LTQ_SPI_FSTAT_RXFFL_SHIFT	0
#define LTQ_SPI_FSTAT_TXFFL_MASK	0x3f
#define LTQ_SPI_FSTAT_TXFFL_SHIFT	8

#define LTQ_SPI_GPOCON_ISCSBN_SHIFT	8
#define LTQ_SPI_GPOCON_INVOUTN_SHIFT	0

#define LTQ_SPI_FGPO_SETOUTN_SHIFT	8
#define LTQ_SPI_FGPO_CLROUTN_SHIFT	0

#define LTQ_SPI_RXREQ_RXCNT_MASK	0xFFFF	/* Receive count value */
#define LTQ_SPI_RXCNT_TODO_MASK		0xFFFF	/* Recevie to-do value */

#define LTQ_SPI_IRNEN_F		BIT(3)	/* Frame end interrupt request */
#define LTQ_SPI_IRNEN_E		BIT(2)	/* Error end interrupt request */
#define LTQ_SPI_IRNEN_T		BIT(0)	/* Transmit end interrupt request */
#define LTQ_SPI_IRNEN_R		BIT(1)	/* Receive end interrupt request */
#define LTQ_SPI_IRNEN_T_XWAY	BIT(1)	/* Transmit end interrupt request */
#define LTQ_SPI_IRNEN_R_XWAY	BIT(0)	/* Receive end interrupt request */
#define LTQ_SPI_IRNEN_ALL	0xF

#define LTQ_SPI_IRNCR_F		BIT(3)	/* Frame end interrupt request */
#define LTQ_SPI_IRNCR_E		BIT(2)	/* Error end interrupt request */
#define LTQ_SPI_IRNCR_T		BIT(0)	/* Transmit end interrupt request */
#define LTQ_SPI_IRNCR_R		BIT(1)	/* Receive end interrupt request */
#define LTQ_SPI_IRNCR_ALL	0xF

struct ltq_spi {
	struct spi_bitbang	bitbang;
	struct completion	done;
	spinlock_t		lock;

	struct device		*dev;
	void __iomem		*base;
	struct clk		*fpiclk;
	struct clk		*spiclk;

	int			status;
	int			irq[3];

	const u8		*tx;
	u8			*rx;
	u32			tx_cnt;
	u32			rx_cnt;
	u32			len;
	struct spi_transfer	*curr_transfer;

	u32			bits_per_word;
	u32			speed_hz;
	u32			mode;

	u16			txfs;
	u16			rxfs;
	unsigned		dma_support:1;
	unsigned		cfg_mode:1;
	unsigned		lsb_first:1;

	u32			irnen_t;
	u32			irnen_r;
	
	u32			rx_trig_lvl;
};

static inline struct ltq_spi *ltq_spi_to_hw(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static inline u32 ltq_spi_reg_read(struct ltq_spi *hw, u32 reg)
{
	return __raw_readl(hw->base + reg);
}

static inline void ltq_spi_reg_write(struct ltq_spi *hw, u32 val, u32 reg)
{
	__raw_writel(val, hw->base + reg);
}

static inline void ltq_spi_reg_setbit(struct ltq_spi *hw, u32 bits, u32 reg)
{
	u32 val;

	val = ltq_spi_reg_read(hw, reg);
	val |= bits;
	ltq_spi_reg_write(hw, val, reg);
}

static inline void ltq_spi_reg_clearbit(struct ltq_spi *hw, u32 bits, u32 reg)
{
	u32 val;

	val = ltq_spi_reg_read(hw, reg);
	val &= ~bits;
	ltq_spi_reg_write(hw, val, reg);
}

static void ltq_spi_hw_enable(struct ltq_spi *hw)
{
	u32 clc;

	/* Power-up module */
	clk_prepare_enable(hw->spiclk);

	/*
	 * Set clock divider for run mode to 1 to
	 * run at same frequency as FPI bus
	 */
	clc = (1 << LTQ_SPI_CLC_RMC_SHIFT);
	ltq_spi_reg_write(hw, clc, LTQ_SPI_CLC);
}

static void ltq_spi_hw_disable(struct ltq_spi *hw)
{
	/* Set clock divider to 0 and set module disable bit */
	ltq_spi_reg_write(hw, LTQ_SPI_CLC_DISS, LTQ_SPI_CLC);

	/* Power-down module */
	clk_disable(hw->spiclk);
}

static void ltq_spi_reset_fifos(struct ltq_spi *hw)
{
	u32 val;

	/*
	 * Enable and flush FIFOs. Set interrupt trigger level to
	 * half of FIFO count implemented in hardware.
	 */
	if (hw->txfs > 1) {
		val = hw->txfs << (LTQ_SPI_TXFCON_TXFITL_SHIFT - 1);
		val |= LTQ_SPI_TXFCON_TXFEN | LTQ_SPI_TXFCON_TXFLU;
		ltq_spi_reg_write(hw, val, LTQ_SPI_TXFCON);
	}

	if (hw->rxfs > 1) {
		val = hw->rxfs << (LTQ_SPI_RXFCON_RXFITL_SHIFT - 1);
		val |= LTQ_SPI_RXFCON_RXFEN | LTQ_SPI_RXFCON_RXFLU;
		ltq_spi_reg_write(hw, val, LTQ_SPI_RXFCON);
		hw->rx_trig_lvl = hw->rxfs << 1;
	}
}

static inline int ltq_spi_wait_ready(struct ltq_spi *hw)
{
	u32 stat;
	unsigned long timeout = jiffies + msecs_to_jiffies(1000);

	do {
		stat = ltq_spi_reg_read(hw, LTQ_SPI_STAT);
		if (!(stat & LTQ_SPI_STAT_BSY))
			return 0;

		cond_resched();
	} while (!time_after_eq(jiffies, timeout));

	dev_err(hw->dev, "SPI wait ready timed out stat: %x\n", stat);

	return -ETIMEDOUT;
}

static void ltq_spi_config_mode_set(struct ltq_spi *hw)
{
	if (hw->cfg_mode)
		return;

	/*
	 * Putting the SPI module in config mode is only safe if no
	 * transfer is in progress as indicated by busy flag STATE.BSY.
	 */
	if (ltq_spi_wait_ready(hw)) {
		ltq_spi_reset_fifos(hw);
		hw->status = -ETIMEDOUT;
	}
	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_CLREN, LTQ_SPI_WHBSTATE);

	hw->cfg_mode = 1;
}

static void ltq_spi_run_mode_set(struct ltq_spi *hw)
{
	if (!hw->cfg_mode)
		return;

	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_SETEN, LTQ_SPI_WHBSTATE);

	hw->cfg_mode = 0;
}

/**
 * Transform bytes to ssc word
 *
 * We need to take care of shift-register direction, according to SSC spec
 * section 2.1 the direction will follow LSB/MSB-first option:
 * SPI standard: MSB first
 * 8051-like serial interface: LSB first
 */
static u32 ltq_spi_tx_word(struct ltq_spi *hw, u32 num_of_bytes)
{
	const u8 *tx = hw->tx;
	u32 data = 0;
	int i;
	int start;
	int step;

	if (hw->lsb_first) {
		start = num_of_bytes - 1;
		step = -1;
	} else {
		start = 0;
		step = 1;
	}

	for (i = start; i < num_of_bytes && i >= 0; i += step) {
		data = data << 8;
		data |= (u32)tx[i];
	}

	hw->tx_cnt += num_of_bytes;
	hw->tx += num_of_bytes;
	return data;
}

/**
 * Transform ssc word to bytes
 */
static void ltq_spi_rx_word(struct ltq_spi *hw, u32 num_of_bytes, u32 data)
{
	u8 *rx = hw->rx;
	int i;
	int start;
	int step;

	if (hw->lsb_first) {
		start = 0;
		step = 1;
	} else {
		start = num_of_bytes - 1;
		step = -1;
	}

	for (i = start; i < num_of_bytes && i >= 0; i += step) {
		rx[i] = data & 0xFF;
		data  = data >> 8;
	}

	hw->rx_cnt += num_of_bytes;
	hw->rx += num_of_bytes;
}

static void ltq_spi_bits_per_word_set(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 bm;
	u8 bits_per_word = spi->bits_per_word;

	/*
	 * Use either default value of SPI device or value
	 * from current transfer.
	 */
	if (hw->curr_transfer && hw->curr_transfer->bits_per_word)
		bits_per_word = hw->curr_transfer->bits_per_word;

	/* CON.BM value = bits_per_word - 1 */
	bm = (bits_per_word - 1) << LTQ_SPI_CON_BM_SHIFT;

	ltq_spi_reg_clearbit(hw, LTQ_SPI_CON_BM_MASK <<
			     LTQ_SPI_CON_BM_SHIFT, LTQ_SPI_CON);
	ltq_spi_reg_setbit(hw, bm, LTQ_SPI_CON);
}

static void ltq_spi_speed_set(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 br, max_speed_hz, spi_clk;
	u32 speed_hz = spi->max_speed_hz;

	pr_debug("[%s]\n", __func__);
	/*
	 * SPI module clock is derived from FPI bus clock dependent on
	 * divider value in CLC.RMS which is always set to 1.
	 */
	spi_clk = clk_get_rate(hw->fpiclk);

	/*
	 * Use either default value of SPI device or value
	 * from current transfer.
	 */
	if (hw->curr_transfer && hw->curr_transfer->speed_hz)
		speed_hz = hw->curr_transfer->speed_hz;

	/*
	 * Maximum SPI clock frequency in master mode is half of
	 * SPI module clock frequency. Maximum reload value of
	 * baudrate generator BR is 2^16.
	 */
	max_speed_hz = spi_clk / 2;
	if (speed_hz >= max_speed_hz)
		br = 0;
	else
		br = ((max_speed_hz + speed_hz / 2) / speed_hz) - 1;

	if (br > 0xFFFF)
		br = 0xFFFF;

	ltq_spi_reg_write(hw, br, LTQ_SPI_BRT);
}

static void ltq_spi_clockmode_set(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 con;

	con = ltq_spi_reg_read(hw, LTQ_SPI_CON);

	/*
	 * SPI mode mapping in CON register:
	 * Mode CPOL CPHA CON.PO CON.PH
	 *  0    0    0      0      1
	 *  1    0    1      0      0
	 *  2    1    0      1      1
	 *  3    1    1      1      0
	 */
	if (spi->mode & SPI_CPHA)
		con &= ~LTQ_SPI_CON_PH;
	else
		con |= LTQ_SPI_CON_PH;

	if (spi->mode & SPI_CPOL)
		con |= LTQ_SPI_CON_PO;
	else
		con &= ~LTQ_SPI_CON_PO;

	/* Set heading control.
	 * This affects the direction of transmit/receive in the shift-register
	 * and TB/RB, i.e.: for MSB-first, we need to put 1st-byte as MSB,
	 * regardless of CPU endianness. Refer to SSC spec section 2.1.
	 */
	if (spi->mode & SPI_LSB_FIRST) {
		con &= ~LTQ_SPI_CON_HB;
		hw->lsb_first = 1;
	} else {
		con |= LTQ_SPI_CON_HB;
		hw->lsb_first = 0;
	}

	ltq_spi_reg_write(hw, con, LTQ_SPI_CON);
}

static void ltq_spi_xmit_set(struct ltq_spi *hw, struct spi_transfer *t)
{
	u32 con;

	con = ltq_spi_reg_read(hw, LTQ_SPI_CON);

	if (t) {
		if (t->tx_buf && t->rx_buf) {
			con &= ~(LTQ_SPI_CON_TXOFF | LTQ_SPI_CON_RXOFF);
		} else if (t->rx_buf) {
			con &= ~LTQ_SPI_CON_RXOFF;
			con |= LTQ_SPI_CON_TXOFF;
		} else if (t->tx_buf) {
			con &= ~LTQ_SPI_CON_TXOFF;
			con |= LTQ_SPI_CON_RXOFF;
		}
	} else
		con |= (LTQ_SPI_CON_TXOFF | LTQ_SPI_CON_RXOFF);

	ltq_spi_reg_write(hw, con, LTQ_SPI_CON);
}

static void ltq_spi_internal_cs_activate(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 fgpo;

	fgpo = (1 << (spi->chip_select + LTQ_SPI_FGPO_CLROUTN_SHIFT));
	ltq_spi_reg_setbit(hw, fgpo, LTQ_SPI_FGPO);
}

static void ltq_spi_internal_cs_deactivate(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 fgpo;

	fgpo = (1 << (spi->chip_select + LTQ_SPI_FGPO_SETOUTN_SHIFT));
	ltq_spi_reg_setbit(hw, fgpo, LTQ_SPI_FGPO);
}

static void ltq_spi_chipselect(struct spi_device *spi, int cs)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);

	if (ltq_spi_wait_ready(hw))
		dev_err(&spi->dev, "wait failed\n");

	switch (cs) {
	case BITBANG_CS_ACTIVE:
		ltq_spi_internal_cs_activate(spi);
		break;

	case BITBANG_CS_INACTIVE:
		ltq_spi_internal_cs_deactivate(spi);
		break;
	}
}

static int ltq_spi_setup_transfer(struct spi_device *spi,
				  struct spi_transfer *t)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u8 bits_per_word = spi->bits_per_word;
	u32 speed_hz = spi->max_speed_hz;

	hw->curr_transfer = t;

	if (t && t->bits_per_word)
		bits_per_word = t->bits_per_word;

	if (bits_per_word > 32)
		return -EINVAL;

	if (t && t->speed_hz)
		speed_hz = t->speed_hz;

	if (speed_hz > spi->max_speed_hz)
		return -EINVAL;

	if (hw->bits_per_word != bits_per_word ||
	    hw->speed_hz != speed_hz ||
	    hw->mode != spi->mode) {
		ltq_spi_config_mode_set(hw);
		ltq_spi_bits_per_word_set(spi);
		ltq_spi_speed_set(spi);
		ltq_spi_clockmode_set(spi);
		ltq_spi_run_mode_set(hw);

		hw->bits_per_word = bits_per_word;
		hw->speed_hz = speed_hz;
		hw->mode = spi->mode;
	}

	return 0;
}

static int ltq_spi_setup(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 gpocon, fgpo;

	/* Set default word length to 8 if not set */
	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	if (spi->bits_per_word > 32)
		return -EINVAL;

	/* Set the Polarity */
	ltq_spi_clockmode_set(spi);

	/* GPIOs are used for CS */
	if (gpio_is_valid(spi->cs_gpio))
		return 0;

	/*
	 * Up to six GPIOs can be connected to the SPI module
	 * via GPIO alternate function to control the chip select lines.
	 */
	gpocon = (1 << (spi->chip_select +
			LTQ_SPI_GPOCON_ISCSBN_SHIFT));

	if (spi->mode & SPI_CS_HIGH)
		gpocon |= (1 << spi->chip_select);

	fgpo = (1 << (spi->chip_select + LTQ_SPI_FGPO_SETOUTN_SHIFT));

	ltq_spi_reg_setbit(hw, gpocon, LTQ_SPI_GPOCON);
	ltq_spi_reg_setbit(hw, fgpo, LTQ_SPI_FGPO);

	return 0;
}

static void ltq_spi_cleanup(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);

	ltq_spi_config_mode_set(hw);
}

static void ltq_spi_txfifo_write(struct ltq_spi *hw)
{
	u32 fstat, data;
	u16 fifo_space;
	u32 bytes_per_word;

	/* Determine how much FIFOs are free for TX data */
	fstat = ltq_spi_reg_read(hw, LTQ_SPI_FSTAT);
	fifo_space = hw->txfs - ((fstat >> LTQ_SPI_FSTAT_TXFFL_SHIFT) &
					LTQ_SPI_FSTAT_TXFFL_MASK);

	if (!fifo_space)
		return;

	if (hw->bits_per_word <= 8) {
		bytes_per_word = 1;
	} else if (hw->bits_per_word <= 16) {
		bytes_per_word = 2;
	} else if (hw->bits_per_word <= 32) {
		bytes_per_word = 4;
	} else {
		dev_err(hw->dev, "invalid bits_per_word %u\n",
			hw->bits_per_word);
		return;
	}

	while (hw->tx_cnt < hw->len && fifo_space) {
		data = ltq_spi_tx_word(hw, bytes_per_word);
		ltq_spi_reg_write(hw, data, LTQ_SPI_TB);
		fifo_space--;
	}
}

static void ltq_spi_rxfifo_read(struct ltq_spi *hw)
{
	u32 fstat;
	u16 fifo_fill;
	u8 rxbv;

	/* Determine how much FIFOs are filled with RX data */
	fstat = ltq_spi_reg_read(hw, LTQ_SPI_FSTAT);
	fifo_fill = ((fstat >> LTQ_SPI_FSTAT_RXFFL_SHIFT)
			& LTQ_SPI_FSTAT_RXFFL_MASK);

	if (!fifo_fill)
		return;

	/*
	 * The 32 bit FIFO is always used completely independent from the
	 * bits_per_word value. Thus four bytes have to be read at once
	 * per FIFO.
	 */
	while (hw->len - hw->rx_cnt >= 4 && fifo_fill) {
		ltq_spi_rx_word(hw, 4, ltq_spi_reg_read(hw, LTQ_SPI_RB));
		fifo_fill--;
	}

	/*
	 * If there are remaining bytes, read byte count from STAT.RXBV
	 * register and read the data byte-wise.
	 */
	while (fifo_fill && hw->rx_cnt < hw->len) {
		rxbv = (ltq_spi_reg_read(hw, LTQ_SPI_STAT) >>
			LTQ_SPI_STAT_RXBV_SHIFT) & LTQ_SPI_STAT_RXBV_MASK;

		if( rxbv != hw->len - hw->rx_cnt)
		{
			pr_warn("%s WARNING last %d bytes might be incorrect!\n", __func__, hw->len - hw->rx_cnt - rxbv);
			rxbv = hw->len - hw->rx_cnt;/* observe rxbv not the same as remaining bytes, so we do this workaround */
		}

		ltq_spi_rx_word(hw, rxbv, ltq_spi_reg_read(hw, LTQ_SPI_RB));
		fifo_fill--;
	}
}

static void ltq_spi_rxreq_set(struct ltq_spi *hw)
{
	u32 rxreq, rxreq_max, rxtodo;

	/*
	 * !<TODO: WW; check if this is correct, this is one bit less in grx500 ssc controller
	 * and start at bit 1, compare to vr9 16 bit and start from 0, but it doesn't make sense. 
	 * this seem correct interpretation in my puny brain
	 */
	rxtodo = ltq_spi_reg_read(hw, LTQ_SPI_RXCNT) & LTQ_SPI_RXCNT_TODO_MASK;

	/*
	 * In RX-only mode the serial clock is activated only after writing
	 * the expected amount of RX bytes into RXREQ register.
	 * To avoid receive overflows at high clocks it is better to request
	 * only the amount of bytes that fits into all FIFOs. This value
	 * depends on the FIFO size implemented in hardware.
	 */
	rxreq = hw->len - hw->rx_cnt;
	rxreq_max = hw->rx_trig_lvl; /* same as rx intr trigger level */
	rxreq = min(rxreq_max, rxreq);

	if (!rxtodo && rxreq)
		ltq_spi_reg_write(hw, rxreq, LTQ_SPI_RXREQ);
}

static inline void ltq_spi_complete(struct ltq_spi *hw)
{
	complete(&hw->done);
}

irqreturn_t ltq_spi_tx_irq(int irq, void *data)
{
	struct ltq_spi *hw = data;
	unsigned long flags;
	int completed = 0;

	#ifndef CONFIG_SPI_GRX500_POLL
	spin_lock_irqsave(&hw->lock, flags);

	/* handle only when requested , seen case of dummy irq */
	if( ltq_spi_reg_read(hw, LTQ_SPI_IRNCR) & LTQ_SPI_IRNCR_T)
	{
		ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_T, LTQ_SPI_IRNEN);
		ltq_spi_reg_write(hw, LTQ_SPI_IRNCR_T, LTQ_SPI_IRNCR);
		if (hw->tx_cnt < hw->len)
			ltq_spi_txfifo_write(hw);

		/* avoid enabling interrupt if completed */
		if (hw->tx_cnt == hw->len)
			completed = 1;
		else
			ltq_spi_reg_setbit(hw, LTQ_SPI_IRNEN_T, LTQ_SPI_IRNEN);
	}

	spin_unlock_irqrestore(&hw->lock, flags);

	if (completed)
		ltq_spi_complete(hw);

	#endif /* CONFIG_SPI_GRX500_POLL */

	return IRQ_HANDLED;
}

irqreturn_t ltq_spi_rx_irq(int irq, void *data)
{
	struct ltq_spi *hw = data;
	unsigned long flags;
	int completed = 0;

	#ifndef CONFIG_SPI_GRX500_POLL
	spin_lock_irqsave(&hw->lock, flags);

	/* handle only when requested , seen case of dummy irq */
	if( ltq_spi_reg_read(hw, LTQ_SPI_IRNCR) & LTQ_SPI_IRNCR_R)
	{
		ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_R, LTQ_SPI_IRNEN);
		ltq_spi_reg_write(hw, LTQ_SPI_IRNCR_R, LTQ_SPI_IRNCR);

		if (hw->rx_cnt < hw->len) {
			ltq_spi_rxfifo_read(hw);

			if (hw->tx && hw->tx_cnt < hw->len)
				ltq_spi_txfifo_write(hw);
		}

		if (hw->rx_cnt == hw->len)
			completed = 1;
		else if (!hw->tx)
			ltq_spi_rxreq_set(hw);

		/* avoid enabling interrupt if completed */
		if (!completed)
			ltq_spi_reg_setbit(hw, LTQ_SPI_IRNEN_R, LTQ_SPI_IRNEN);
	}
	spin_unlock_irqrestore(&hw->lock, flags);

	if (completed)
		ltq_spi_complete(hw);

	#endif /* CONFIG_SPI_GRX500_POLL */

	return IRQ_HANDLED;
}

irqreturn_t ltq_spi_err_irq(int irq, void *data)
{
	struct ltq_spi *hw = data;
	unsigned long flags;

	#ifndef CONFIG_SPI_GRX500_POLL
	spin_lock_irqsave(&hw->lock, flags);

	/* handle only when requested , seen case of dummy irq */
	if( ltq_spi_reg_read(hw, LTQ_SPI_IRNCR) & LTQ_SPI_IRNCR_E)
	{
		/* clear all interrupt table */
		ltq_spi_reg_write(hw, LTQ_SPI_IRNCR_ALL, LTQ_SPI_IRNCR);

		/* Disable all interrupts */
		ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_ALL, LTQ_SPI_IRNEN);

		/* Clear all error flags */
		ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_CLR_ERRORS, LTQ_SPI_WHBSTATE);

		/* Flush FIFOs */
		ltq_spi_reg_setbit(hw, LTQ_SPI_RXFCON_RXFLU, LTQ_SPI_RXFCON);
		ltq_spi_reg_setbit(hw, LTQ_SPI_TXFCON_TXFLU, LTQ_SPI_TXFCON);

		hw->status = -EIO;
	}

	spin_unlock_irqrestore(&hw->lock, flags);

	ltq_spi_complete(hw);

	#endif /* CONFIG_SPI_GRX500_POLL */

	return IRQ_HANDLED;
}

#ifdef CONFIG_SPI_GRX500_POLL

static void
ltq_spi_tx_poll(struct ltq_spi *hw)
{
	unsigned long flags;

	spin_lock_irqsave(&hw->lock, flags);

	while (1) {
		/* fill TX FIFO with as much data as possible */
		if (hw->tx_cnt < hw->len)
			ltq_spi_txfifo_write(hw);

		if (hw->tx_cnt == hw->len)
			break;
	}

	spin_unlock_irqrestore(&hw->lock, flags);

	/* try to wait until tx really finished */
	while ( (ltq_spi_reg_read(hw, LTQ_SPI_STAT) & LTQ_SPI_STAT_BSY) != 0 )
		;

	return;
}

static u32 ltq_spi_rxreq_poll_set(struct ltq_spi *hw)
{
	u32 rxreq, rxreq_max;

	/*
	 * In RX-only mode the serial clock is activated only after writing
	 * the expected amount of RX bytes into RXREQ register.
	 * To avoid receive overflows at high clocks it is better to request
	 * only the amount of bytes that fits into all FIFOs. This value
	 * depends on the FIFO size implemented in hardware.
	 */
	rxreq = hw->len - hw->rx_cnt;
	rxreq_max = hw->rxfs << 2;
	rxreq = min(rxreq_max, rxreq);

	if (rxreq){
		ltq_spi_reg_write(hw, rxreq, LTQ_SPI_RXREQ);
		return rxreq;
	}
	
	return 0;
}

/**
 * \fn static void ltq_spi_rx_poll(struct ltq_spi *hw)
 * \brief Start FIFO data reception.
 * This function processes received data. It will read data from the FIFO
 *
 * \param   hw     Pointer to structure #ltq_spi
 * \return  NULL
 * Description:
 * In Rx mode, to void memory copy, where rx data starts must be determined
 * several special cases
 * 1) If rxsize is divisable by 4, all tx data will be skipped.
 * 2) If rxsize is not divisable by 4,including less than 4 bytes.
 * The remaining 1~3 bytes have to do swap.
 * \ingroup IFX_SSC_INTERNAL
 */
static void
ltq_spi_rx_poll(struct ltq_spi *hw)
{
	u32 rxreq_fifo_fill, rxreq_bytes;
	unsigned long flags;

	/* dma con disable rx , not needed since nobody touch the LTQ_SPI_DMACON register */
	// ltq_spi_reg_setbit(hw, LTQ_SPI_DMACON_RXON, LTQ_SPI_DMACON);

	/* sort of spin_lock moreever with irq disabled, wonder if really needed
	* now dat we run in bitbang work queue, work queue can sleep!
	* and we are out of irq context now + it is single work queue per controller. */
	spin_lock_irqsave(&hw->lock, flags);

	while (1) {
		/* request & start Rx clock */
		rxreq_bytes     = ltq_spi_rxreq_poll_set(hw);
		rxreq_fifo_fill = ( rxreq_bytes >> 2 ) + ((rxreq_bytes & 0x3) > 0 ? 1 : 0); //get fifo level needed for num of bytes

		/* wait till filling level in rx fifo achieve all our request */
		while( rxreq_fifo_fill != ((ltq_spi_reg_read(hw, LTQ_SPI_FSTAT) >> LTQ_SPI_FSTAT_RXFFL_SHIFT) & LTQ_SPI_FSTAT_RXFFL_MASK))
			;

		ltq_spi_rxfifo_read(hw);

		/* check if read complete */
		if (hw->rx_cnt == hw->len)
			break;
	}

	spin_unlock_irqrestore(&hw->lock, flags);

	return;
}
#endif /* CONFIG_SPI_GRX500_POLL */

static int ltq_spi_txrx_bufs(struct spi_device *spi, struct spi_transfer *t)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 irq_flags = 0;
	unsigned long long ms;

	#ifndef CONFIG_SPI_GRX500_POLL
	unsigned long flags;
	#endif

	hw->tx = t->tx_buf;
	hw->rx = t->rx_buf;
	hw->len = t->len;
	hw->tx_cnt = 0;
	hw->rx_cnt = 0;
	hw->status = 0;
	reinit_completion(&(hw->done));

	ltq_spi_xmit_set(hw, t);

	#ifdef CONFIG_SPI_GRX500_POLL

	/* Enable error interrupts */
	//ltq_spi_reg_setbit(hw, LTQ_SPI_IRNEN_E, LTQ_SPI_IRNEN);

	/* !<WW: not pretty at all, it will busy looping inside, could be a concern for
	* big blocksize like spinand (2K ), observed some rcu_preempt cpu stall
	* and seem like need to do spin_lock_irqsave to disable Interrupt, else
	* the nasty rcu stall detector ( it is from hrtimer interrupt though ) come in while
	* reading or setting register bit and mess up my work_queue
	*/
	if (hw->tx) {
		ltq_spi_tx_poll(hw);
	} else if (hw->rx) {
		ltq_spi_rx_poll(hw);
	}

	/* Disable all interrupts */
	//ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_ALL, LTQ_SPI_IRNEN);

	#else /* CONFIG_SPI_GRX500_POLL */

	/* Enable error interrupts */
	ltq_spi_reg_setbit(hw, LTQ_SPI_IRNEN_E, LTQ_SPI_IRNEN);

	if (hw->tx) {
		spin_lock_irqsave(&hw->lock, flags);

		/* Initially fill TX FIFO with as much data as possible */
		ltq_spi_txfifo_write(hw);

		spin_unlock_irqrestore(&hw->lock, flags);

		irq_flags |= hw->irnen_t;

		/* Always enable RX interrupt in Full Duplex mode */
		if (hw->rx)
			irq_flags |= hw->irnen_r;
	} else if (hw->rx) {
		/* Start RX clock */
		ltq_spi_rxreq_set(hw);

		/* Enable RX interrupt to receive data from RX FIFOs */
		irq_flags |= hw->irnen_r;
	}

	/* Enable TX or RX interrupts */
	ltq_spi_reg_setbit(hw, irq_flags, LTQ_SPI_IRNEN);

	/* Wait for interrupt. Timeout calculation is based on spi framework
	 * algorithm.
	 */
	ms = 8LL * 1000LL * hw->len;
	do_div(ms, t->speed_hz);
	ms += ms + 200; /* some tolerance */
	if (ms > UINT_MAX)
		ms = UINT_MAX;

	if (!wait_for_completion_timeout(&hw->done, msecs_to_jiffies(ms))) {
		pr_err("%s - Timedout waiting for interrupt\n", __func__);
		hw->status = -ETIMEDOUT;
	}

	/* Disable all interrupts */
	ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_ALL, LTQ_SPI_IRNEN);
	ltq_spi_reg_write(hw, LTQ_SPI_IRNCR_ALL, LTQ_SPI_IRNCR);

	/* try to wait until tx really finished */
	if (!hw->status)
		ltq_spi_wait_ready(hw);

	#endif /* CONFIG_SPI_GRX500_POLL */

	/*
	 * Return length of current transfer for bitbang utility code if
	 * no errors occured during transmission.
	 */
	if (!hw->status)
		hw->status = hw->len;

	return hw->status;
}

static const struct ltq_spi_irq_map {
	char *name;
	irq_handler_t handler;
} ltq_spi_irqs[] = {
	{ "spi_rx", ltq_spi_rx_irq },
	{ "spi_tx", ltq_spi_tx_irq },
	{ "spi_err", ltq_spi_err_irq },
};

static int ltq_spi_probe(struct platform_device *pdev)
{
	struct resource irqres[3];
	struct spi_master *master;
	struct resource *r;
	struct ltq_spi *hw;
	int ret, i;
	u32 data, id;

	if (of_irq_to_resource_table(pdev->dev.of_node, irqres, 3) != 3) {
		dev_err(&pdev->dev, "IRQ settings missing in device tree\n");
		return -EINVAL;
	}

	master = spi_alloc_master(&pdev->dev, sizeof(struct ltq_spi));
	if (!master) {
		dev_err(&pdev->dev, "spi_alloc_master\n");
		ret = -ENOMEM;
		goto err;
	}

	hw = spi_master_get_devdata(master);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		dev_err(&pdev->dev, "platform_get_resource\n");
		ret = -ENOENT;
		goto err_master;
	}

	r = devm_request_mem_region(&pdev->dev, r->start, resource_size(r),
			pdev->name);
	if (!r) {
		dev_err(&pdev->dev, "failed to request memory region\n");
		ret = -ENXIO;
		goto err_master;
	}

	hw->base = devm_ioremap_nocache(&pdev->dev, r->start, resource_size(r));
	if (!hw->base) {
		dev_err(&pdev->dev, "failed to remap memory region\n");
		ret = -ENXIO;
		goto err_master;
	}

	memset(hw->irq, 0, sizeof(hw->irq));
	for (i = 0; i < ARRAY_SIZE(ltq_spi_irqs); i++) {
		hw->irq[i] = irqres[i].start;
		ret = request_irq(hw->irq[i], ltq_spi_irqs[i].handler,
				  0, ltq_spi_irqs[i].name, hw);
		if (ret) {
			dev_err(&pdev->dev, "failed to request %s irq (%d)\n",
					ltq_spi_irqs[i].name, hw->irq[i]);
			goto err_irq;
		}
	}

	hw->fpiclk = of_clk_get_by_name(pdev->dev.of_node, "freq");
	if (IS_ERR(hw->fpiclk)) {
		dev_err(&pdev->dev, "failed to get fpi clock\n");
		ret = PTR_ERR(hw->fpiclk);
		goto err_clk;
	}

	hw->spiclk = of_clk_get_by_name(pdev->dev.of_node, "spi");
	if (IS_ERR(hw->spiclk)) {
		dev_err(&pdev->dev, "failed to get spi clock gate\n");
		ret = PTR_ERR(hw->spiclk);
		goto err_clk;
	}

	hw->bitbang.master = spi_master_get(master);
	hw->bitbang.chipselect = ltq_spi_chipselect;
	hw->bitbang.setup_transfer = ltq_spi_setup_transfer;
	hw->bitbang.txrx_bufs = ltq_spi_txrx_bufs;

	if (of_machine_is_compatible("lantiq,ase")) {
		master->num_chipselect = 3;

		hw->irnen_t = LTQ_SPI_IRNEN_T_XWAY;
		hw->irnen_r = LTQ_SPI_IRNEN_R_XWAY;
	} else {
		master->num_chipselect = 6;

		hw->irnen_t = LTQ_SPI_IRNEN_T;
		hw->irnen_r = LTQ_SPI_IRNEN_R;
	}

	master->bus_num = pdev->id;
	master->bits_per_word_mask = SPI_BPW_RANGE_MASK(2, 8) |
				     SPI_BPW_MASK(16) | SPI_BPW_MASK(32);
	master->setup = ltq_spi_setup;
	master->cleanup = ltq_spi_cleanup;
	master->dev.of_node = pdev->dev.of_node;

	hw->dev = &pdev->dev;
	init_completion(&hw->done);
	spin_lock_init(&hw->lock);

	ltq_spi_hw_enable(hw);

	/* Read module capabilities */
	id = ltq_spi_reg_read(hw, LTQ_SPI_ID);
	hw->txfs = (id >> LTQ_SPI_ID_TXFS_SHIFT) & LTQ_SPI_ID_TXFS_MASK;
	hw->rxfs = (id >> LTQ_SPI_ID_RXFS_SHIFT) & LTQ_SPI_ID_RXFS_MASK;
	hw->dma_support = (id & LTQ_SPI_ID_CFG) ? 1 : 0;

	ltq_spi_config_mode_set(hw);

	/* Enable error checking, disable TX/RX, set idle value high */
	data = LTQ_SPI_CON_AEN |
	    LTQ_SPI_CON_TEN | LTQ_SPI_CON_REN |
	    LTQ_SPI_CON_TXOFF | LTQ_SPI_CON_RXOFF | LTQ_SPI_CON_IDLE;
	ltq_spi_reg_write(hw, data, LTQ_SPI_CON);

	/* Enable master mode and clear error flags */
	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_SETMS |
			  LTQ_SPI_WHBSTATE_CLR_ERRORS, LTQ_SPI_WHBSTATE);

	/* Reset GPIO/CS registers */
	ltq_spi_reg_write(hw, 0x0, LTQ_SPI_GPOCON);
	ltq_spi_reg_write(hw, 0xFF00, LTQ_SPI_FGPO);

	/* Enable and flush FIFOs */
	ltq_spi_reset_fifos(hw);

	ret = spi_bitbang_start(&hw->bitbang);
	if (ret) {
		dev_err(&pdev->dev, "spi_bitbang_start failed\n");
		goto err_bitbang;
	}

	platform_set_drvdata(pdev, hw);

	pr_info("Lantiq SoC SPI controller rev %u (TXFS %u, RXFS %u, DMA %u)\n",
		id & LTQ_SPI_ID_REV_MASK, hw->txfs, hw->rxfs, hw->dma_support);

	return 0;

err_bitbang:
	ltq_spi_hw_disable(hw);

err_clk:
	if (hw->fpiclk)
		clk_put(hw->fpiclk);
	if (hw->spiclk)
		clk_put(hw->spiclk);

err_irq:
	clk_put(hw->fpiclk);

	for (i = 0; i < ARRAY_SIZE(ltq_spi_irqs); i++)
		free_irq(hw->irq[i], hw);

err_master:
	spi_master_put(master);

err:
	return ret;
}

static int ltq_spi_remove(struct platform_device *pdev)
{
	struct ltq_spi *hw = platform_get_drvdata(pdev);
	int i;

	spi_bitbang_stop(&hw->bitbang);

	platform_set_drvdata(pdev, NULL);

	ltq_spi_config_mode_set(hw);
	ltq_spi_hw_disable(hw);

	for (i = 0; i < ARRAY_SIZE(hw->irq); i++)
		if (0 < hw->irq[i])
			free_irq(hw->irq[i], hw);

	if (hw->fpiclk)
		clk_put(hw->fpiclk);
	if (hw->spiclk)
		clk_put(hw->spiclk);

	spi_master_put(hw->bitbang.master);

	return 0;
}

static const struct of_device_id ltq_spi_match[] = {
	{ .compatible = "lantiq,spi-xway" },
	{ .compatible = "lantiq,spi-lantiq-ssc" },
	{ .compatible = "lantiq,spi-grx500" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_spi_match);

static struct platform_driver ltq_spi_driver = {
	.probe = ltq_spi_probe,
	.remove = ltq_spi_remove,
	.driver = {
		.name = "spi-lantiq-ssc",
		.owner = THIS_MODULE,
		.of_match_table = ltq_spi_match,
	},
};

module_platform_driver(ltq_spi_driver);

MODULE_DESCRIPTION("Lantiq SoC SPI controller driver");
MODULE_AUTHOR("Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:spi-xway");
MODULE_ALIAS("platform:spi-lantiq-ssc");
MODULE_ALIAS("platform:spi-grx500");
