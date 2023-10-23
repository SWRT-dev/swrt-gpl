
/*
 * Lantiq I2C bus adapter
 *
 * Parts based on i2c-designware.c and other i2c drivers from Linux 2.6.33
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Copyright (C) 2012 Thomas Langer <thomas.langer@lantiq.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h> /* for kzalloc, kfree */
#include <linux/i2c.h>
#include <linux/errno.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/i2c.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pm_runtime.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/cpufreq.h>
#include <linux/wait.h>
#include "i2c-lantiq.h"

#define LTQ_I2C_KERNEL_CLOCK (10*1000*1000)


/* #define DEBUG */

/* access macros */
#define i2c_r32(reg)	\
	__raw_readl(&(priv->membase)->reg)
#define i2c_w32(val, reg)	\
	__raw_writel(val, &(priv->membase)->reg)
#define i2c_w32_mask(clear, set, reg)	\
	i2c_w32((i2c_r32(reg) & ~(clear)) | (set), reg)

#define DRV_NAME "i2c-lantiq"
#define DRV_VERSION "1.01"

#define LTQ_I2C_BUSY_TIMEOUT		20 /* ms */

#define LTQ_I2C_RX_FIFO_TIMEOUT		HZ

#ifdef DEBUG
#define LTQ_I2C_XFER_TIMEOUT		(25*HZ)
#else
#define LTQ_I2C_XFER_TIMEOUT		(2*LTQ_I2C_RX_FIFO_TIMEOUT)
#endif


#define LTQ_I2C_IMSC_DEFAULT_MASK	(I2C_IMSC_I2C_P_INT_EN | \
					 I2C_IMSC_I2C_ERR_INT_EN)

#define LTQ_I2C_ARB_LOST		(1 << 0)
#define LTQ_I2C_NACK			(1 << 1)
#define LTQ_I2C_RX_UFL			(1 << 2)
#define LTQ_I2C_RX_OFL			(1 << 3)
#define LTQ_I2C_TX_UFL			(1 << 4)
#define LTQ_I2C_TX_OFL			(1 << 5)

struct ltq_i2c {
	struct mutex mutex;

	struct i2c_bus_recovery_info rinfo;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pinctrl_pins_default;
	struct pinctrl_state *pinctrl_pins_gpio;

	/* active clock settings */
	unsigned int input_clock;	/* clock input for i2c hardware block */
	unsigned int i2c_clock;		/* approximated bus clock in kHz */

	struct clk *clk_gate;
	struct clk *clk_input;


	/* resources (memory and interrupts) */
	unsigned int irq_lb;           /* last burst irq */
	unsigned int irq_b;            /* burst irq */
	unsigned int irq_err;          /* error irq */
	unsigned int irq_prot;         /* protocol irq */

	struct lantiq_reg_i2c __iomem *membase;	/* base of mapped registers */

	struct i2c_adapter adap;
	struct device *dev;

	struct completion cmd_complete;


	/* message transfer data */
	struct i2c_msg *current_msg;	/* current message */
	int msgs_num;		/* number of messages to handle */
	u8 *msg_buf;		/* current buffer */
	u32 msg_buf_len;	/* remaining length of current buffer */
	int msg_err;		/* error status of the current transfer */

	wait_queue_head_t rx_thread_wait_queue;
	u8 rx_thread_force_stop;
	struct mutex rx_lock;
	enum {
		RX_THREAD_STOP = 0,
		RX_THREAD_RUNNING
	} rx_thread_status; /* last burst indicator */
	enum {
		BURST = 0,
		LAST_BURST
	} last_burst; /* last burst indicator */

	/* master status codes */
	enum {
		STATUS_IDLE,
		STATUS_ADDR,	/* address phase */
		STATUS_WRITE,
		STATUS_READ,
		STATUS_READ_END,
		STATUS_STOP
	} status;
};

static irqreturn_t ltq_i2c_isr(int irq, void *dev_id);

/* enable all irqs in Linux */
static void enable_all_irqs(struct ltq_i2c *priv)
{
	enable_irq(priv->irq_err);
	enable_irq(priv->irq_b);
	enable_irq(priv->irq_lb);
	enable_irq(priv->irq_prot);
}

/* disable all irqs in Linux */
static void disable_all_irqs(struct ltq_i2c *priv)
{
	disable_irq(priv->irq_prot);
	disable_irq(priv->irq_lb);
	disable_irq(priv->irq_b);
	disable_irq(priv->irq_err);
}

/**
 * enable burst irq
 *
 * Hardware apparently has a quirk in which it flushes the rx fifo when
 * last-burst interrupt is cleared (before fifo being read in threaded
 * handler). For this reason we avoid using last-burst in rx, and rely on
 * tx-end instead to end transfer.
 */
static void enable_burst_irq(struct ltq_i2c *priv)
{
	int mask;

	if (priv->status == STATUS_READ)
		mask = I2C_IMSC_BREQ_INT_EN;
	else
		mask = I2C_IMSC_LBREQ_INT_EN |
			I2C_IMSC_BREQ_INT_EN;

	i2c_w32_mask(0, mask, imsc);
}

static void disable_burst_irq(struct ltq_i2c *priv)
{
	i2c_w32_mask(I2C_IMSC_LBREQ_INT_EN |
		I2C_IMSC_BREQ_INT_EN |
		I2C_IMSC_LSREQ_INT_EN |
		I2C_IMSC_SREQ_INT_EN,
		0,
		imsc);
}

static void prepare_msg_send_addr(struct ltq_i2c *priv)
{
	struct i2c_msg *msg = priv->current_msg;
	int rd = !!(msg->flags & I2C_M_RD);	/* extends to 0 or 1 */
	u16 addr = msg->addr;

	/* new i2c_msg */
	priv->msg_buf = msg->buf;
	priv->msg_buf_len = msg->len;
	pr_debug("ADDR: rd[%d]msg->flags[%d]\n", rd, msg->flags);
	if (rd)
		priv->status = STATUS_READ;
	else
		priv->status = STATUS_WRITE;

	/* send slave address */
	if (msg->flags & I2C_M_TEN) {
		i2c_w32(0xf0 | ((addr & 0x300) >> 7) | rd, txd);
		i2c_w32(addr & 0xff, txd);
	} else {
		i2c_w32((addr & 0x7f) << 1 | rd, txd);
	}
}

static void ltq_i2c_set_tx_len(struct ltq_i2c *priv)
{
	struct i2c_msg *msg = priv->current_msg;
	int len = (msg->flags & I2C_M_TEN) ? 2 : 1;

	pr_debug("set_tx_len %cX len[%d]\n",
		(msg->flags & I2C_M_RD) ? 'R' : 'T', msg->len);

	priv->status = STATUS_ADDR;

	if (!(msg->flags & I2C_M_RD))
		len += msg->len;
	else
		/* set maximum received packet size (before rx int!) */
		i2c_w32(msg->len, mrps_ctrl);

	i2c_w32(len, tps_ctrl);
	enable_burst_irq(priv);

}

static int ltq_i2c_hw_set_clock(struct i2c_adapter *adap)
{
	struct ltq_i2c *priv = i2c_get_adapdata(adap);
	unsigned int input_clock = clk_get_rate(priv->clk_input);
	u32 dec, inc = 1;
	u32 rmc = 0;

	/* clock changed? */
	if (priv->input_clock == input_clock)
		return 0;

	/* recalculate RMC value */

	rmc = input_clock / LTQ_I2C_KERNEL_CLOCK;

	/*
	 * this formula is only an approximation, found by the recommended
	 * values in the "I2C Architecture Specification 1.7.1"
	 */
	dec = LTQ_I2C_KERNEL_CLOCK / (priv->i2c_clock * 2);

	if (!rmc) {
		dev_warn(priv->dev, "input clock is too low [%u]\n",
			input_clock);
		/* force rmc to 1 and recalculate dec based on input_clock */
		rmc = 1;
		dec = input_clock / (priv->i2c_clock * 2);
	}

	i2c_w32(rmc << I2C_CLC_RMC_OFFSET, clc);

	if (dec <= 6) {
		dev_info(priv->dev, "dec is too small [%d]", dec);
		return -ENXIO;
	}

	i2c_w32(0, fdiv_high_cfg);
	i2c_w32((inc << I2C_FDIV_CFG_INC_OFFSET) |
		(dec << I2C_FDIV_CFG_DEC_OFFSET),
		fdiv_cfg);

	/* dev_info(priv->dev, "in %d kHz, bus %d kHz,rmc =%d dec=%d %d\n",
	 *	input_clock, priv->i2c_clock, rmc, dec, LTQ_I2C_KERNEL_CLOCK);
	 */

	priv->input_clock = input_clock;
	return 0;
}

static int ltq_i2c_hw_init(struct i2c_adapter *adap)
{
	int ret = 0;
	struct ltq_i2c *priv = i2c_get_adapdata(adap);

	/* disable bus */
	i2c_w32_mask(I2C_RUN_CTRL_RUN_EN, 0, run_ctrl);

	/* setup clock */
	ret = ltq_i2c_hw_set_clock(adap);
	if (ret != 0) {
		dev_warn(priv->dev, "invalid clock settings\n");
		return ret;
	}

	/* configure fifo */
	i2c_w32(I2C_FIFO_CFG_TXFC | /* tx fifo as flow controller */
		I2C_FIFO_CFG_RXFC | /* rx fifo as flow controller */
		I2C_FIFO_CFG_TXFA_TXFA0 | /* tx fifo byte aligned */
		I2C_FIFO_CFG_RXFA_RXFA0 | /* rx fifo byte aligned */
		I2C_FIFO_CFG_TXBS_TXBS0 | /* tx fifo burst size is 1 word */
		I2C_FIFO_CFG_RXBS_RXBS0,  /* rx fifo burst size is 1 word */
		fifo_cfg);


	/* configure address */
	if (priv->msgs_num == 1)
		i2c_w32(I2C_ADDR_CFG_SOPE_EN |
			/* generate stop when no more data in the fifo*/
			I2C_ADDR_CFG_SONA_EN |
			/* generate stop when NA received */
			I2C_ADDR_CFG_MnS_EN |
			/* we are master device */
			0,/* our slave address (not used!) */
		addr_cfg);
	else
		i2c_w32(
			I2C_ADDR_CFG_SONA_EN |
			/* generate stop when NA received */
			I2C_ADDR_CFG_MnS_EN |/* we are master device */
			0,/* our slave address (not used!) */
			addr_cfg);


	/* enable bus */
	i2c_w32_mask(0, I2C_RUN_CTRL_RUN_EN, run_ctrl);

	return 0;
}

static int ltq_i2c_wait_bus_not_busy(struct ltq_i2c *priv)
{
	unsigned long timeout;
	u32 stat = 0;

	timeout = jiffies + msecs_to_jiffies(LTQ_I2C_BUSY_TIMEOUT);

	do {
		stat = i2c_r32(bus_stat);

		if ((stat & I2C_BUS_STAT_BS_MASK) == I2C_BUS_STAT_BS_FREE)
			return 0;

		cond_resched();
	} while (!time_after_eq(jiffies, timeout));

	dev_err(priv->dev, "timeout waiting for bus ready (stat 0x%08X)\n",
		stat);
	return -ETIMEDOUT;
}

/**
 * Send addr and tx in word (LSB first)
 *
 * Address and tx data are sent together in word (4-bytes), to eliminate/reduce
 * the dependency on tx interrupt.
 */
static void ltq_i2c_tx(struct ltq_i2c *priv)
{
	struct i2c_msg *msg = priv->current_msg;
	u16 addr = msg->addr;
	u32 word = 0;
	u32 tmp;
	const int bytes_per_word = 4;
	int i = 0;

	if (priv->status == STATUS_ADDR) {
		/* new i2c_msg */
		priv->msg_buf = msg->buf;
		priv->msg_buf_len = msg->len;
		pr_debug("ADDR: msg->flags[%d] addr 0x%x\n", msg->flags, addr);

		/* send slave address */
		if (msg->flags & I2C_M_TEN) {
			word = 0xf0 | ((addr & 0x300) >> 7);
			word |= ((addr & 0xff) << 8);
			i += 2;
		} else {
			word = (addr & 0x7f) << 1;
			i += 1;
		}
		priv->status = STATUS_WRITE;
	}

	for (; i < bytes_per_word; i++) {
		if (priv->msg_buf_len && priv->msg_buf) {
			tmp = ((u32)*priv->msg_buf) << (8 * i);
			word |= tmp;
			if (--priv->msg_buf_len)
				priv->msg_buf++;
			else
				priv->msg_buf = NULL;
		} else {
			priv->last_burst = LAST_BURST;
		}
	}

	/* write as word */
	i2c_w32(word, txd);

	if (priv->last_burst)
		disable_burst_irq(priv);
}

/**
 * threaded rx routine
 *
 * rps register does not produce consistent result with fifo (ffs). E.g.
 * sometimes ffs increased without rps being increased. Therefore here we
 * ignore rps and rely solely on ffs.
 */
static int ltq_i2c_rx(struct ltq_i2c *priv)
{
	u32 fifo_filled;
	int ret = 0;
	int i;
	u32 data;

	if (priv->msg_buf_len && priv->msg_buf) {
		if (priv->rx_thread_force_stop != 1) {
			fifo_filled = i2c_r32(ffs_stat);
			while (fifo_filled > 0) {
				data = i2c_r32(rxd);
				for (i = 0; i < 4; i++) {
					*priv->msg_buf = (data >> (8 * i))
							& 0xff;

					if (--priv->msg_buf_len) {
						priv->msg_buf++;
					} else {
						priv->last_burst = LAST_BURST;
						priv->msg_buf = NULL;
						break;
					}
				}
				fifo_filled--;
				if (!priv->msg_buf)
					break;
			}
		} else {
			dev_err(priv->dev, "I2C: Force to stop\n");
			priv->last_burst = LAST_BURST;
			goto err;
		}
	} else {
		priv->last_burst = LAST_BURST;
	}
err:
	if (priv->last_burst) {
		ret = 1;
		disable_burst_irq(priv);
		if (priv->status == STATUS_READ_END) {
			/*
			 * do the STATUS_STOP and complete() here, as sometimes
			 * the tx_end is already seen before this is finished
			 */
			priv->status = STATUS_STOP;
			complete(&priv->cmd_complete);
		} else {
			priv->status = STATUS_READ_END;
			/* make sure that bus is not free
			 * before sending out SETEND
			 */
			if ((i2c_r32(bus_stat) & I2C_BUS_STAT_BS_MASK)
				== I2C_BUS_STAT_BS_BM) {
				if (i2c_r32(bus_stat) !=
					I2C_BUS_STAT_RNW_READ)
					i2c_w32(I2C_ENDD_CTRL_SETEND,
						endd_ctrl);
			}
		}
	}
	return ret;
}

static void ltq_i2c_xfer_init(struct ltq_i2c *priv)
{
	/* enable interrupts */
	struct i2c_adapter *adap;

	adap = &priv->adap;
	ltq_i2c_hw_init(adap);
	i2c_w32(LTQ_I2C_IMSC_DEFAULT_MASK, imsc);
	/* trigger transfer of first msg */
	ltq_i2c_set_tx_len(priv);
}
static void dump_msgs(struct i2c_msg msgs[], int num, int rx)
{
#if defined(DEBUG)
	int i, j;

	pr_debug("Messages %d %s\n", num, rx ? "out" : "in");
	for (i = 0; i < num; i++) {
		pr_debug("%2d %cX Msg(%d) addr=0x%X: ", i,
			(msgs[i].flags & I2C_M_RD) ? 'R' : 'T',
			msgs[i].len, msgs[i].addr);
		if (!(msgs[i].flags & I2C_M_RD) || rx) {
			for (j = 0; j < msgs[i].len; j++)
				pr_debug("%02X ", msgs[i].buf[j]);
		}
		pr_debug("\n");
	}
#endif
}

static void ltq_i2c_release_bus(struct ltq_i2c *priv)
{
	if ((i2c_r32(bus_stat) & I2C_BUS_STAT_BS_MASK) == I2C_BUS_STAT_BS_BM)
		i2c_w32(I2C_ENDD_CTRL_SETEND, endd_ctrl);
}

static int ltq_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[],
			   int num)
{
	struct ltq_i2c *priv = i2c_get_adapdata(adap);
	int ret;

	ret = pm_runtime_get_sync(priv->dev);
	if (ret < 0)
		goto out;

	dev_dbg(priv->dev, "xfer %u messages\n", num);
	dump_msgs(msgs, num, 0);

	mutex_lock(&priv->mutex);

	init_completion(&priv->cmd_complete);
	priv->current_msg = msgs;
	priv->msgs_num = num;
	priv->msg_err = 0;
	priv->status = STATUS_IDLE;
	priv->rx_thread_status = RX_THREAD_STOP;
	priv->rx_thread_force_stop = 0;

	enable_all_irqs(priv);

	/* wait for the bus to become ready */
	ret = ltq_i2c_wait_bus_not_busy(priv);
	if (ret) {
		dev_err(priv->dev, "%s: bus is busy %d\n", __func__, ret);
		i2c_recover_bus(adap);
		ltq_i2c_hw_init(adap);
		goto done;
	}

	while (priv->msgs_num) {
		if (priv->current_msg->len > 0) {
			/* start the transfers */
			ltq_i2c_xfer_init(priv);
			/* wait for transfers to complete */
			ret = wait_for_completion_interruptible_timeout(
				&priv->cmd_complete, LTQ_I2C_XFER_TIMEOUT);
			if (ret == 0) {
				dev_err(priv->dev, "controller timed out 0x%x\n",
					priv->msg_err);
				i2c_recover_bus(adap);
				ltq_i2c_hw_init(adap);
				ret = -ETIMEDOUT;
				goto done;
			} else if (ret < 0)
				goto done;

			if (priv->msg_err) {
				if (priv->msg_err & LTQ_I2C_NACK)
					ret = -ENXIO;
				else {
					dev_err(priv->dev, "xfer error:\n");
					if (priv->msg_err & LTQ_I2C_TX_OFL)
						dev_err(priv->dev, " tx overflow\n");
					if (priv->msg_err & LTQ_I2C_TX_UFL)
						dev_err(priv->dev, " tx underflow\n");
					if (priv->msg_err & LTQ_I2C_RX_OFL)
						dev_err(priv->dev, " rx overflow\n");
					if (priv->msg_err & LTQ_I2C_RX_UFL)
						dev_err(priv->dev, " rx underflow\n");

					ret = -EREMOTEIO;
				}

				goto done;
			}
		}

		if (--priv->msgs_num)
			priv->current_msg++;
	}
	/* no error? */
	ret = num;

done:
	disable_all_irqs(priv);
	ltq_i2c_release_bus(priv);
	if (priv->rx_thread_status != RX_THREAD_STOP) {
		priv->rx_thread_force_stop = 1;
		if (wait_event_interruptible_timeout(
		    priv->rx_thread_wait_queue,
		    priv->rx_thread_status == RX_THREAD_STOP,
		    LTQ_I2C_RX_FIFO_TIMEOUT*3) <= 0)
			dev_err(priv->dev, "RX Threaded could not stop!!!\n");
	}
	/* ensure that really no interrupts will access
	 * "msg_buf" or "current_mgs" after this function returns
	 */
	priv->msg_buf = NULL;
	priv->current_msg = NULL;

	mutex_unlock(&priv->mutex);

	if (ret >= 0)
		dump_msgs(msgs, num, 1);

out:
	pm_runtime_mark_last_busy(priv->dev);
	pm_runtime_put_autosuspend(priv->dev);

	pr_debug("XFER ret %d\n", ret);
	return ret;
}

static irqreturn_t ltq_i2c_rx_thread_fn(int irq, void *dev_id)
{
	struct ltq_i2c *priv = dev_id;

	if (!priv)
		return IRQ_HANDLED;
	mutex_lock(&priv->rx_lock);
	priv->rx_thread_status = RX_THREAD_RUNNING;
	if (ltq_i2c_rx(priv) == 0)
		enable_burst_irq(priv);
	priv->rx_thread_status = RX_THREAD_STOP;
	wake_up_interruptible(&priv->rx_thread_wait_queue);
	mutex_unlock(&priv->rx_lock);
	return IRQ_HANDLED;
}
static irqreturn_t ltq_i2c_isr_burst(int irq, void *dev_id)
{
	struct ltq_i2c *priv = dev_id;
	struct i2c_msg *msg = priv->current_msg;

	int ret = IRQ_HANDLED;

	priv->last_burst = (irq == priv->irq_lb) ? LAST_BURST : BURST;
	if (priv->last_burst)
		pr_debug("===LB: flags[%x] status[%x]\n",
			msg->flags, priv->status);
	else
		pr_debug("===B: flags[%x] status[%x]\n",
			msg->flags, priv->status);

	if (msg->flags & I2C_M_RD) {
		switch (priv->status) {
		case STATUS_ADDR:
			pr_debug("===X===\n");
			disable_burst_irq(priv);
			prepare_msg_send_addr(priv);
			break;
		case STATUS_READ:
		case STATUS_READ_END:
			pr_debug("===R===\n");
			disable_burst_irq(priv);
			/* we need to handle RX in kernel thread */
			ret  = IRQ_WAKE_THREAD;
			break;
		default:
			disable_burst_irq(priv);
			pr_debug("===Status R %d\n", priv->status);
			break;
		}
	} else {
		switch (priv->status) {
		case STATUS_ADDR:
		case STATUS_WRITE:
			pr_debug("===w===\n");
			ltq_i2c_tx(priv);
			break;
		default:
			disable_burst_irq(priv);
			pr_debug("===Status W %d\n", priv->status);
			break;
		}
	}

	i2c_w32(I2C_ICR_BREQ_INT_CLR | I2C_ICR_LBREQ_INT_CLR, icr);
	return ret;
}

static irqreturn_t ltq_i2c_isr_prot(struct ltq_i2c *priv)
{
	u32 i_pro = i2c_r32(p_irqss);
	int ret = IRQ_HANDLED;

	i2c_w32_mask(LTQ_I2C_IMSC_DEFAULT_MASK, 0, imsc);

	if (i_pro)
		i2c_w32(i_pro, p_irqsc);

	pr_debug("i2c-p");

	/* not acknowledge */
	if (i_pro & I2C_P_IRQSS_NACK) {
		priv->msg_err |= LTQ_I2C_NACK;
		pr_debug(" nack");
	}

	/* arbitration lost */
	if (i_pro & I2C_P_IRQSS_AL) {
		priv->msg_err |= LTQ_I2C_ARB_LOST;
		pr_err(" arb-lost");
	}
	/* tx -> rx switch */
	if (i_pro & I2C_P_IRQSS_RX)
		pr_debug(" rx");

	/* tx end */
	if (i_pro & I2C_P_IRQSS_TX_END)
		pr_debug(" txend");
	pr_debug("\n");

	if (!priv->msg_err) {
		/* tx -> rx switch */
		if (i_pro & I2C_P_IRQSS_RX) {
			priv->status = STATUS_READ;
			enable_burst_irq(priv);
		}
		if (i_pro & I2C_P_IRQSS_TX_END) {
			if (priv->status == STATUS_READ) {
				/*
				 * Get data in FIFO before
				 * with threaded RX function before
				 * closing the transaction.
				 */
				priv->status = STATUS_READ_END;
				priv->last_burst = LAST_BURST;
				disable_burst_irq(priv);
				ret = IRQ_WAKE_THREAD;
			} else {
				disable_burst_irq(priv);
				priv->status = STATUS_STOP;
				pr_debug("----0x%x\n", i_pro);
			}
		}
	}

	i2c_w32_mask(0, LTQ_I2C_IMSC_DEFAULT_MASK, imsc);
	return ret;
}


static irqreturn_t ltq_i2c_isr(int irq, void *dev_id)
{
	u32 i_raw, i_err = 0;
	struct ltq_i2c *priv = dev_id;
	int ret = IRQ_HANDLED;

	if (pm_runtime_suspended(priv->dev))
		return IRQ_NONE;

	i_raw = i2c_r32(mis);
	if (i_raw)
		i2c_w32(i_raw, icr);
	pr_debug("i_raw 0x%08X\n", i_raw);


	/* error interrupt */
	if (i_raw & I2C_RIS_I2C_ERR_INT_INTOCC) {
		i_err = i2c_r32(err_irqss);
		if (i_err)
			i2c_w32(i_err, err_irqsc);
		pr_debug("i_err 0x%08X bus_stat 0x%04X\n",
			i_err, i2c_r32(bus_stat));

		/* tx fifo overflow (8) */
		if (i_err & I2C_ERR_IRQSS_TXF_OFL)
			priv->msg_err |= LTQ_I2C_TX_OFL;


		/* tx fifo underflow (4) */
		if (i_err & I2C_ERR_IRQSS_TXF_UFL)
			priv->msg_err |= LTQ_I2C_TX_UFL;


		/* rx fifo overflow (2) */
		if (i_err & I2C_ERR_IRQSS_RXF_OFL)
			priv->msg_err |= LTQ_I2C_RX_OFL;

		/* rx fifo underflow (1) */
		if (i_err & I2C_ERR_IRQSS_RXF_UFL)
			priv->msg_err |= LTQ_I2C_RX_UFL;

	}

	/* protocol interrupt */
	if (i_raw & I2C_RIS_I2C_P_INT_INTOCC)
		ret = ltq_i2c_isr_prot(priv);
	else {
		i_err = i2c_r32(err_irqss);
		if (i_err) {
			i2c_w32(i_err, p_irqsc);
			/* pr_err("%s: ERROR\n", __func__); */
		}
	}

	if ((priv->msg_err) || (priv->status == STATUS_STOP))
		complete(&priv->cmd_complete);

	return ret;
}

static u32 ltq_i2c_functionality(struct i2c_adapter *adap)
{
	return	I2C_FUNC_I2C |
		I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm ltq_i2c_algorithm = {
	.master_xfer	= ltq_i2c_xfer,
	.functionality	= ltq_i2c_functionality,
};

static void ltq_i2c_prepare_recovery(struct i2c_adapter *adap)
{
	struct ltq_i2c *priv = i2c_get_adapdata(adap);

	pinctrl_select_state(priv->pinctrl, priv->pinctrl_pins_gpio);
}

static void ltq_i2c_unprepare_recovery(struct i2c_adapter *adap)
{
	struct ltq_i2c *priv = i2c_get_adapdata(adap);

	pinctrl_select_state(priv->pinctrl, priv->pinctrl_pins_default);
}

static int ltq_i2c_init_recovery_info(struct ltq_i2c *priv,
				      struct platform_device *pdev)
{
	struct i2c_bus_recovery_info *rinfo = &priv->rinfo;

	priv->pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR_OR_NULL(priv->pinctrl)) {
		dev_info(&pdev->dev,
			 "can't get pinctrl, bus recovery not supported\n");
		return PTR_ERR(priv->pinctrl);
	}

	priv->pinctrl_pins_default =
		pinctrl_lookup_state(priv->pinctrl, PINCTRL_STATE_DEFAULT);
	priv->pinctrl_pins_gpio = pinctrl_lookup_state(priv->pinctrl,
						       "gpio");
	rinfo->sda_gpio = of_get_named_gpio(pdev->dev.of_node, "sda-gpios", 0);
	rinfo->scl_gpio = of_get_named_gpio(pdev->dev.of_node, "scl-gpios", 0);

	if (rinfo->sda_gpio == -EPROBE_DEFER ||
	    rinfo->scl_gpio == -EPROBE_DEFER) {
		return -EPROBE_DEFER;
	} else if (!gpio_is_valid(rinfo->sda_gpio) ||
		   !gpio_is_valid(rinfo->scl_gpio) ||
		   IS_ERR(priv->pinctrl_pins_default) ||
		   IS_ERR(priv->pinctrl_pins_gpio)) {
		dev_dbg(&pdev->dev, "recovery information incomplete\n");
		return 0;
	}

	dev_dbg(&pdev->dev, "using scl-gpio %d and sda-gpio %d for recovery\n",
		rinfo->sda_gpio, rinfo->scl_gpio);

	rinfo->prepare_recovery = ltq_i2c_prepare_recovery;
	rinfo->unprepare_recovery = ltq_i2c_unprepare_recovery;
	rinfo->recover_bus = i2c_generic_gpio_recovery;
	priv->adap.bus_recovery_info = rinfo;

	return 0;
}

static int ltq_i2c_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct ltq_i2c *priv;
	struct i2c_adapter *adap;
	struct resource *mmres, irqres[4];
	int ret = 0;

	dev_dbg(&pdev->dev, "probing\n");

	mmres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ret = of_irq_to_resource_table(node, irqres, 4);
	if (!mmres || (ret != 4)) {
		dev_err(&pdev->dev, "no resources\n");
		return -ENODEV;
	}

	/* allocate private data */
	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "can't allocate private data\n");
		return -ENOMEM;
	}

	adap = &priv->adap;
	i2c_set_adapdata(adap, priv);
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_HWMON | I2C_CLASS_SPD;
	strlcpy(adap->name, DRV_NAME "-adapter", sizeof(adap->name));
	adap->algo = &ltq_i2c_algorithm;
	adap->dev.parent = &pdev->dev;

	if (of_property_read_u32(node, "clock-frequency", &priv->i2c_clock)) {
		dev_warn(&pdev->dev, "No I2C speed selected, using 100kHz\n");
		priv->i2c_clock = 100000;
	}

	init_completion(&priv->cmd_complete);
	mutex_init(&priv->mutex);
	mutex_init(&priv->rx_lock);
	init_waitqueue_head(&priv->rx_thread_wait_queue);
	priv->membase = devm_ioremap_resource(&pdev->dev, mmres);
	if (priv->membase == NULL)
		return -ENOMEM;

	priv->dev = &pdev->dev;

	priv->irq_lb = irqres[0].start;
	priv->irq_b = irqres[1].start;
	priv->irq_err = irqres[2].start;
	priv->irq_prot = irqres[3].start;

	ret = devm_request_threaded_irq(&pdev->dev,
		irqres[0].start,
		ltq_i2c_isr_burst,
		ltq_i2c_rx_thread_fn,
		IRQF_ONESHOT,
		"i2c lb",
		priv);

	if (ret) {
		dev_err(&pdev->dev, "can't get last burst IRQ %d\n",
			irqres[0].start);
		return -ENODEV;
	}

	ret = devm_request_threaded_irq(&pdev->dev,
			irqres[1].start,
			ltq_i2c_isr_burst,
			ltq_i2c_rx_thread_fn,
		IRQF_ONESHOT,
		"i2c b",
		priv);

	if (ret) {
		dev_err(&pdev->dev, "can't get burst IRQ %d\n",
			irqres[1].start);
		return -ENODEV;
	}

	ret = devm_request_threaded_irq(&pdev->dev,
			irqres[2].start,
			ltq_i2c_isr,
			ltq_i2c_rx_thread_fn,
		IRQF_ONESHOT,
		"i2c err",
		priv);

	if (ret) {
		dev_err(&pdev->dev, "can't get error IRQ %d\n",
			irqres[2].start);
		return -ENODEV;
	}

	ret = devm_request_threaded_irq(&pdev->dev,
			irqres[3].start,
			ltq_i2c_isr,
			ltq_i2c_rx_thread_fn,
		IRQF_ONESHOT,
		"i2c p",
		priv);

	if (ret) {
		dev_err(&pdev->dev, "can't get protocol IRQ %d\n",
			irqres[3].start);
		return -ENODEV;
	}
	disable_all_irqs(priv);

	dev_dbg(&pdev->dev, "mapped io-space to %p\n", priv->membase);
	dev_dbg(&pdev->dev, "use IRQs %d, %d, %d, %d\n", irqres[0].start,
		irqres[1].start, irqres[2].start, irqres[3].start);

	/* this is a static clock, which has no refcounting */
	priv->clk_input = devm_clk_get(&pdev->dev, "freq");
	if (IS_ERR(priv->clk_input)) {
		dev_err(&pdev->dev, "failed to get fpi clk\n");
		return -ENOENT;
	}

	priv->clk_gate = devm_clk_get(&pdev->dev, "i2c");
	if (IS_ERR_OR_NULL(priv->clk_gate)) {
		dev_err(&pdev->dev, "failed to get i2c clk\n");
		return -ENOENT;
	}
	clk_prepare_enable(priv->clk_gate);

	dev_dbg(&pdev->dev, "clock rate %lx\n", clk_get_rate(priv->clk_input));

	adap->dev.of_node = node;

	/* optional bus recovery */
	ltq_i2c_init_recovery_info(priv, pdev);

	platform_set_drvdata(pdev, priv);

	pm_runtime_enable(priv->dev);
	pm_runtime_set_autosuspend_delay(priv->dev, 1000);
	pm_runtime_use_autosuspend(priv->dev);

	ret = pm_runtime_get_sync(priv->dev);
	if (ret < 0)
		goto out;

	/* print module version information */
	dev_dbg(&pdev->dev, "module id=%u revision=%u\n",
		(i2c_r32(id) & I2C_ID_ID_MASK) >> I2C_ID_ID_OFFSET,
		(i2c_r32(id) & I2C_ID_REV_MASK) >> I2C_ID_REV_OFFSET);
	/* initialize HW */
	ret = ltq_i2c_hw_init(adap);
	if (ret) {
		dev_err(&pdev->dev, "can't configure adapter\n");
		i2c_del_adapter(adap);
		platform_set_drvdata(pdev, NULL);
	} else {
		dev_info(&pdev->dev, "version %s\n", DRV_VERSION);
	}
	//i2c_add_adapter(adap);
	/* add our adapter to the i2c stack */
	adap->nr = pdev->id;
	ret = i2c_add_numbered_adapter(adap);
	if (ret) {
		dev_err(&pdev->dev, "can't register I2C adapter\n");
		goto out;
	}

	pm_runtime_mark_last_busy(priv->dev);
	pm_runtime_put_autosuspend(priv->dev);

	return ret;

out:
	/* if init failed, we need to deactivate the clock gate */
	if (ret)
		clk_disable_unprepare(priv->clk_gate);

	pm_runtime_dont_use_autosuspend(&pdev->dev);
	pm_runtime_put_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	return ret;
}

static int ltq_i2c_remove(struct platform_device *pdev)
{
	struct ltq_i2c *priv = platform_get_drvdata(pdev);
	int ret;

	ret = pm_runtime_get_sync(&pdev->dev);
	if (ret < 0)
		return ret;

	/* make sure that RX thread is done */
	if (priv->rx_thread_status != RX_THREAD_STOP) {
		priv->rx_thread_force_stop = 1;
		if (wait_event_interruptible_timeout(
			priv->rx_thread_wait_queue,
			priv->rx_thread_status == RX_THREAD_STOP,
			LTQ_I2C_RX_FIFO_TIMEOUT*3) <= 0)
			dev_err(priv->dev, "RX Threaded could not stop!!!\n");
	}

	/* disable bus */
	i2c_w32_mask(I2C_RUN_CTRL_RUN_EN, 0, run_ctrl);

	/* power down the core */
	clk_disable_unprepare(priv->clk_gate);


	/* remove driver */
	i2c_del_adapter(&priv->adap);
	/* free priv data */
	devm_kfree(&pdev->dev, priv);

	dev_dbg(&pdev->dev, "removed\n");
	platform_set_drvdata(pdev, NULL);

	pm_runtime_dont_use_autosuspend(&pdev->dev);
	pm_runtime_put_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	return 0;
}

#ifdef CONFIG_PM
static int ltq_i2c_runtime_suspend(struct device *dev)
{
	struct ltq_i2c *priv = dev_get_drvdata(dev);

	/* Disable i2c adapter */
	i2c_w32_mask(I2C_RUN_CTRL_RUN_EN, 0, run_ctrl);

	pinctrl_pm_select_sleep_state(dev);

	clk_disable_unprepare(priv->clk_gate);

	return 0;
}

static int ltq_i2c_runtime_resume(struct device *dev)
{
	struct ltq_i2c *priv = dev_get_drvdata(dev);

	clk_prepare_enable(priv->clk_gate);

	/* Enable i2c adapter */
	pinctrl_pm_select_default_state(dev);
	i2c_w32_mask(0, I2C_RUN_CTRL_RUN_EN, run_ctrl);

	return 0;
}
#endif /* CONFIG_PM */

static const struct dev_pm_ops ltq_i2c_pm_ops = {
	SET_RUNTIME_PM_OPS(ltq_i2c_runtime_suspend,
			   ltq_i2c_runtime_resume, NULL)
};

static const struct of_device_id ltq_i2c_match[] = {
	{ .compatible = "lantiq,lantiq-i2c" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_i2c_match);

static struct platform_driver ltq_i2c_driver = {
	.probe	= ltq_i2c_probe,
	.remove	= ltq_i2c_remove,
	.driver	= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
		.pm = &ltq_i2c_pm_ops,
		.of_match_table = ltq_i2c_match,
	},
};

static int __init ltq_i2c_init(void)
{
	int ret;

	ret = platform_driver_register(&ltq_i2c_driver);
	if (ret)
		pr_err("ltq-i2c: probe failed: %d\n", ret);

	return ret;
}

subsys_initcall(ltq_i2c_init);

/* module_platform_driver(ltq_i2c_driver); */

MODULE_DESCRIPTION("Lantiq I2C bus adapter");
MODULE_AUTHOR("Thomas Langer <thomas.langer@lantiq.com>");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
