// SPDX-License-Identifier: GPL-2.0
/*  Copyright (C) 2018 Intel Corporation.
 *  Wu ZhiXian <zhixian.wu@intel.com>
 */

#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <net/intel_datapath_umt.h>

#define UMT_PERD 0xDC

#define UMT_TRG_MUX_0 0xE0
#define UMT_TRG_MUX_0_UMT_RX0_SEL_POS 0
#define UMT_TRG_MUX_0_UMT_RX0_SEL_MASK 0xFu
#define UMT_TRG_MUX_0_UMT_RX1_SEL_POS 4
#define UMT_TRG_MUX_0_UMT_RX1_SEL_MASK 0xF0u
#define UMT_TRG_MUX_0_UMT_RX2_SEL_POS 8
#define UMT_TRG_MUX_0_UMT_RX2_SEL_MASK 0xF00u
#define UMT_TRG_MUX_0_UMT_RX3_SEL_POS 12
#define UMT_TRG_MUX_0_UMT_RX3_SEL_MASK 0xF000u
#define UMT_TRG_MUX_0_UMT_TX0_SEL_POS 16
#define UMT_TRG_MUX_0_UMT_TX0_SEL_MASK 0xF0000u
#define UMT_TRG_MUX_0_UMT_TX1_SEL_POS 20
#define UMT_TRG_MUX_0_UMT_TX1_SEL_MASK 0xF00000u
#define UMT_TRG_MUX_0_UMT_TX2_SEL_POS 24
#define UMT_TRG_MUX_0_UMT_TX2_SEL_MASK 0xF000000u
#define UMT_TRG_MUX_0_UMT_TX3_SEL_POS 28
#define UMT_TRG_MUX_0_UMT_TX3_SEL_MASK 0xF0000000u

#define UMT_COUNTER_CTRL 0xE4

#define UMT_TRG_MUX_1 0xF0
#define UMT_TRG_MUX_1_UMT_RX0_SEL_POS 0
#define UMT_TRG_MUX_1_UMT_RX0_SEL_MASK 0xFu
#define UMT_TRG_MUX_1_UMT_RX1_SEL_POS 4
#define UMT_TRG_MUX_1_UMT_RX1_SEL_MASK 0xF0u
#define UMT_TRG_MUX_1_UMT_RX2_SEL_POS 8
#define UMT_TRG_MUX_1_UMT_RX2_SEL_MASK 0xF00u
#define UMT_TRG_MUX_1_UMT_RX3_SEL_POS 12
#define UMT_TRG_MUX_1_UMT_RX3_SEL_MASK 0xF000u
#define UMT_TRG_MUX_1_UMT_TX0_SEL_POS 16
#define UMT_TRG_MUX_1_UMT_TX0_SEL_MASK 0xF0000u
#define UMT_TRG_MUX_1_UMT_TX1_SEL_POS 20
#define UMT_TRG_MUX_1_UMT_TX1_SEL_MASK 0xF00000u
#define UMT_TRG_MUX_1_UMT_TX2_SEL_POS 24
#define UMT_TRG_MUX_1_UMT_TX2_SEL_MASK 0xF000000u
#define UMT_TRG_MUX_1_UMT_TX3_SEL_POS 28
#define UMT_TRG_MUX_1_UMT_TX3_SEL_MASK 0xF0000000u

#define UMT_RX0_MUX 0xF4
#define UMT_RX0_MUX_UMT_RX0_SEL0_POS 0
#define UMT_RX0_MUX_UMT_RX0_SEL0_MASK 0xFu
#define UMT_RX0_MUX_UMT_RX0_SEL1_POS 4
#define UMT_RX0_MUX_UMT_RX0_SEL1_MASK 0xF0u
#define UMT_RX0_MUX_UMT_RX0_SEL2_POS 8
#define UMT_RX0_MUX_UMT_RX0_SEL2_MASK 0xF00u
#define UMT_RX0_MUX_UMT_RX0_SEL3_POS 16
#define UMT_RX0_MUX_UMT_RX0_SEL3_MASK 0xF0000u
#define UMT_RX0_MUX_UMT_RX0_SEL4_POS 20
#define UMT_RX0_MUX_UMT_RX0_SEL4_MASK 0xF00000u
#define UMT_RX0_MUX_UMT_RX0_SEL5_POS 24
#define UMT_RX0_MUX_UMT_RX0_SEL5_MASK 0xF000000u
#define UMT_RX0_MUX_UMT_RX0_SEL6_POS 12
#define UMT_RX0_MUX_UMT_RX0_SEL6_MASK 0xF000u
#define UMT_RX0_MUX_UMT_RX0_SEL7_POS 28
#define UMT_RX0_MUX_UMT_RX0_SEL7_MASK 0xF0000000u

#define UMT_MODE_REG 0x100
#define UMT_MODE_REG_UMT_MD_MASK 0x2u
#define UMT_MODE_REG_UMT_CNT_MOD_MASK 0x8u

#define UMT_SW_MODE 0x114
#define UMT_SW_MODE_SW_MSG_MD_MASK 0x3u

#define UMT_GCTRL 0x200

#define UMT_MSG0 0x220

#define UMT_DEST_0 0x280

#define UMT_DEST_1 0x380

#define UMT_MSG0_1 0x390

#define UMT_MSG1_1 0x394

#define UMT_PERIOD_CH1 0x3A0

#define UMT_SW_MODE_CH1 0x3B0

#define UMT_DEST_2 0x480

#define umt_r32(addr_base, x)  readl((addr_base) + (x))
#define umt_w32(addr_base, x, y) writel((x), (addr_base) + (y))
#define umt_w32_mask(addr_base, x, y, z) \
		do { \
			void __iomem *z_ = (addr_base) + (z); \
			writel((readl(z_) & ~(x)) | (y), z_); \
		} while (0)

#define UMT_MSG(x)			(UMT_MSG0 + (x) * 4)

#define UMT_X_SW_MODE		(UMT_SW_MODE_CH1 - UMT_DEST_1)
#define UMT_X_PERIOD		(UMT_PERIOD_CH1 - UMT_DEST_1)
#define UMT_X_DEST			(0)
#define UMT_X_MSG(x)		((x) * 4)

#define UMT_X_ADDR(x, off)	(UMT_DEST_1 + \
				((x) - 1) * (UMT_DEST_2 - UMT_DEST_1) + (off))

/* PRX300 register address */
#define UMT_SW_MODE_PRX			0x218
#define UMT_DEST_PRX			0x230
#define UMT_MSG1_0_PRX			0x400
#define UMT_PERIOD_CH1_PRX		0x420
#define UMT_DEST_1_PRX			0x430
#define UMT_SW_MODE_CH1_PRX		0x434
#define UMT_MSG2_0_PRX			0x500

/* PRX GCTRL */
#define UMT_GCTRL_OCP_UMT_ENDI_B_MASK 0x10000000u
#define UMT_GCTRL_OCP_UMT_ENDI_W_MASK 0x20000000u

#define UMT_X_SW_MODE_PRX		(UMT_SW_MODE_CH1_PRX - UMT_MSG1_0_PRX)
#define UMT_X_PERIOD_PRX		(UMT_PERIOD_CH1_PRX - UMT_MSG1_0_PRX)
#define UMT_X_DEST_PRX		(UMT_DEST_1_PRX - UMT_MSG1_0_PRX)

#define UMT_X_ADDR_PRX(x, off)	(UMT_MSG1_0_PRX + \
				((x) - 1) * (UMT_MSG2_0_PRX - UMT_MSG1_0_PRX) + (off))

#define UMT_PORTS_NUM		3
#define MIN_UMT_PRD		20
#define UMT_DEF_DMACID		13

/* UMT port 0~7 */
#define DP_UMT_ENTRY_NUM_LGM	8
#define DP_UMT_ENTRY_NUM_PRX	3
/* if request port id = 0xFF, automatically allocate id */
#define UMT_PORT_AUTO_ALLOCATE		0xFF
#define MIN_UMT_PRD		20
#define DP_UMT_RX0_MAX_DMA_NUM	8

#if IS_ENABLED(CONFIG_CPU_BIG_ENDIAN)
	#define UMT_DEF_DW_SWAP		1
	#define UMT_DEF_BYTE_SWAP	0
#else
	#define UMT_DEF_DW_SWAP		1
	#define UMT_DEF_BYTE_SWAP	1
#endif

#define DEFAULT_CLK (0x17AC * 50000ULL)

#define REG_LGM		1
#define REG_PRX		0

/* Add for DMA macro */
#define _DMA_CHANBITS	16
#define _DMA_PORTBITS	8
#define _DMA_CTRLBITS	8

#define _DMA_CHANMASK	((1 << _DMA_CHANBITS) - 1)
#define _DMA_PORTMASK	((1 << _DMA_PORTBITS) - 1)
#define _DMA_CTRLMASK	((1 << _DMA_CTRLBITS) - 1)

#define _DMA_CHANSHIFT	0
#define _DMA_PORTSHIFT	(_DMA_CHANSHIFT + _DMA_CHANBITS)
#define _DMA_CTRLSHIFT	(_DMA_PORTSHIFT + _DMA_PORTBITS)

#define _DMA_CONTROLLER(nr) (((nr) >> _DMA_CTRLSHIFT) & _DMA_CTRLMASK)
#define _DMA_PORT(nr)		(((nr) >> _DMA_PORTSHIFT) & _DMA_PORTMASK)
#define _DMA_CHANNEL(nr)	(((nr) >> _DMA_CHANSHIFT) & _DMA_CHANMASK)

static struct dp_umt_priv *g_dp_umt_priv;
static struct dp_umt_entry g_dp_umt_entry[DP_UMT_ENTRY_NUM_LGM];
static int g_max_umt_port;

struct intel_match_data {
	u32 reg_type;
};

static void umt_set_mode(struct dp_umt_param *umt)
{
	int pos;
	u8 mode;
	u32 val;
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port || umt->msg_mode >= DP_UMT_MODE_MAX)
		return;

	if (priv->reg_type == REG_LGM) {
		pos = (umt->id * 4) + 1;
		mode = (u8)umt->msg_mode;
		umt_w32_mask(priv->membase, (1 << pos), mode << pos, UMT_MODE_REG);
	} else {
		mode = (u8)umt->msg_mode;
		if (!umt->id) {
			umt_w32_mask(priv->membase, 0x2, ((u32)mode) << 1, UMT_GCTRL);
		} else {
			pos = 16 + (umt->id - 1) * 3;
			val = umt_r32(priv->membase, UMT_GCTRL) & ~(BIT(pos));
			umt_w32(priv->membase, val | (((u32)mode) << pos), UMT_GCTRL);
		}
	}

}

static void umt_set_msgmode(struct dp_umt_param *umt)
{
	u32 reg_addr;
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port || umt->sw_msg >= DP_UMT_MSG_MAX)
		return;

	if (priv->reg_type == REG_LGM) {
		if (umt->id)
			reg_addr = UMT_X_ADDR(umt->id, UMT_X_SW_MODE);
		else
			reg_addr = UMT_SW_MODE;
	} else {
		if (umt->id)
			reg_addr = UMT_X_ADDR_PRX(umt->id, UMT_X_SW_MODE_PRX);
		else
			reg_addr = UMT_SW_MODE_PRX;
	}

	umt_w32(priv->membase, (u32)umt->sw_msg, reg_addr);
}

/* input in term of microseconds */
static u32 umt_us_to_cnt(u32 usec)
{
	unsigned long cpuclk;
	struct clk *clk;
	unsigned long long usec_tmp;
	struct dp_umt_priv *priv;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	clk = priv->clk;
	if (!clk)
		cpuclk = DEFAULT_CLK;
	else
		cpuclk = clk_get_rate(clk);

	usec_tmp = (unsigned long long)usec *
			((unsigned long)cpuclk / 1000000ULL);
	if (usec_tmp > 0xFFFFFFFFULL) {
		dev_err(dev, "UMT period exceeds max value! set to 0xFFFFFFFF.\n");
		return 0xFFFFFFFF;
	}
	return (u32)usec_tmp;
}

static void umt_set_period(struct dp_umt_param *umt)
{
	u32 period, reg_addr;
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	period = umt_us_to_cnt(umt->period);
	if (umt->id)
		if (priv->reg_type == REG_LGM)
			reg_addr = UMT_X_ADDR(umt->id, UMT_X_PERIOD);
		else
			reg_addr = UMT_X_ADDR_PRX(umt->id, UMT_X_PERIOD_PRX);
	else
		reg_addr = UMT_PERD;

	umt_w32(priv->membase, period, reg_addr);
}

static void umt_set_dst(struct dp_umt_param *umt)
{
	u32 reg_addr;
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	if (priv->reg_type == REG_LGM) {
		if (umt->id)
			reg_addr = UMT_X_ADDR(umt->id, UMT_X_DEST);
		else
			reg_addr = UMT_DEST_0;
	} else {
		if (umt->id)
			reg_addr = UMT_X_ADDR_PRX(umt->id, UMT_X_DEST_PRX);
		else
			reg_addr = UMT_DEST_PRX;
	}

	umt_w32(priv->membase, umt->daddr, reg_addr);
}

static int umt_allocate(struct dp_umt_param *umt, u32 *mode)
{
	u32 dma_cid, dma_pid, dma_ch;
	int i, pid;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

/* DP_UMT_RX_FROM_CQEM:
 * UMT_TRG_MUX_0/UMT_TRG_MUX_1:
 * UMT RX Counter:
 * CQM enqueue port from 16--->UMT trigger value from 0
 * UMT TX Counter:
 * CQM DQ PORT 19----> UMT 0
 *
 * DP_UMT_RX_FROM_DMA:
 * UMT PORT 0 (DOCSIS mode, set at UMT_MODE_REG UMT_CNT_MOD):
 * UMT_RX0_MUX:
 * DMATX0 CH0-7 ---> UMT 8-0xf
 * UMT_TRG_MUX_0/UMT_TRG_MUX_1:
 * UMT Rx Counter (DMA only to RX Counter):
 * DMATX2 CH2-5 ---> UMT 0-3
 * DMATX1 CH2-5 ---> UMT 4-7
 */
	if (!umt)
		return -EINVAL;
	if (umt->id < g_max_umt_port) {
		if (priv->umts[umt->id].flag_bit.flag_bit.alloced == 0)
			pid = umt->id;
		else
			return -EINVAL;
	} else if (umt->id == UMT_PORT_AUTO_ALLOCATE) {
		pid = -1;
		spin_lock_bh(&priv->umt_lock);
		for (i = g_max_umt_port - 1; i >= 0; i--) {
			entry = &priv->umts[i];
			if (entry->flag_bit.flag_bit.alloced == 0) {
				pid = i;
				break;
			}
		}
		spin_unlock_bh(&priv->umt_lock);

		if (pid < 0 || pid >= g_max_umt_port) {
			dev_err(dev, "No free UMT port! umt id = %d\n", umt->id);
			return -ENODEV;
		}
	} else {
		dev_err(dev, "No free UMT port! umt id = %d\n", umt->id);
		return -ENODEV;
	}
	if (umt->rx_src == DP_UMT_RX_FROM_CQEM) {
		if (umt->cqm_enq_pid < 16 || umt->cqm_enq_pid > 23 ||
			umt->cqm_dq_pid < 19 || umt->cqm_dq_pid > 26)
			return -EINVAL;
		entry = &priv->umts[pid];
		*mode = DP_UMT_MUX_CQEM;
	} else if (umt->rx_src == DP_UMT_RX_FROM_DMA) {
		dma_cid = _DMA_CONTROLLER(umt->dma_id);
		dma_pid = _DMA_PORT(umt->dma_id);
		dma_ch = _DMA_CHANNEL(umt->dma_id);
		if (umt->dma_ch_num == 8) {
			if (priv->umts[0].flag_bit.flag_bit.alloced != 0) {
				dev_err(dev, "%s: umt dma ch num %d but UMT PORT 0 is not available.\n",
					__func__, umt->dma_ch_num);
				return -ENODEV;
			}
			/* only UMT PORT 0 support multiple DMA CH so allocate it */
			if (dma_cid == 0 && dma_ch >= 0 && dma_ch <= 7) {
				pid = 0;
				umt->id = 0;
				*mode = DP_UMT_MUX_RX0;
			} else {
				return -EINVAL;
			}
		} else {
			if (priv->reg_type == REG_LGM) {
				if ((dma_cid == 2 || dma_cid == 1) && dma_ch >= 2 && dma_ch <= 5) {
					entry = &priv->umts[pid];
					*mode = DP_UMT_MUX_DMA;
				} else {
					return -EINVAL;
				}
			} else {
				if (dma_ch <= 15) {
					entry = &priv->umts[pid];
					*mode = DP_UMT_MUX_DMA;
				} else {
					return -EINVAL;
				}
			if ((umt->cqm_dq_pid < 4) || (umt->cqm_dq_pid > 6)) {
					return -EINVAL;
				}
			}
		}
	}

	umt->id = pid;
	return 0;
}

static void umt_set_mux(struct dp_umt_param *umt, enum dp_umt_mux_mode mode)
{
	u32 mux_sel, reg_addr;
	u8 umt_rx_counter, umt_tx_counter, dma_cid, dma_ch, temp_id, i;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	if (umt->id >= g_max_umt_port || mode >= DP_UMT_MUX_MODE_MAX)
		return;

	priv = g_dp_umt_priv;
	entry = &priv->umts[umt->id];
	dev = &priv->dev->dev;

	if (mode == DP_UMT_MUX_CQEM) {
		if (priv->reg_type == REG_LGM) {
			if (umt->id >= 4) {
				temp_id = umt->id - 4;
				reg_addr = UMT_TRG_MUX_1;
			} else {
				temp_id = umt->id;
				reg_addr = UMT_TRG_MUX_0;
			}
		} else {
			if (umt->id >= 3) {
				dev_err(dev, "%s: umt->id=%d is error.\n", __func__, umt->id);
				return;
			} else {
				temp_id = umt->id;
				reg_addr = UMT_TRG_MUX_0;
			}
		}
		umt_rx_counter = umt->cqm_enq_pid - 16;
		umt_tx_counter = umt->cqm_dq_pid - 19;
		/* set UMT Multiple Counter Mode to 0 to support one UMT-Rx counters */
		umt_w32_mask(priv->membase, BIT(3), ~BIT(3), UMT_MODE_REG);
		mux_sel = umt_r32(priv->membase, reg_addr) &
				(~((0xF000F) << (temp_id * 4)));
		mux_sel |= ((umt_rx_counter << (temp_id * 4)) |
				(umt_tx_counter << (16 + (temp_id * 4))));
		umt_w32(priv->membase, mux_sel, reg_addr);
	} else if (mode == DP_UMT_MUX_RX0) {
		/* UMT PORT 0 RX Only */
		dma_ch = _DMA_CHANNEL(umt->dma_id);
		if (priv->reg_type != REG_LGM) {
			dev_err(dev, "%s: mode=%d is error.\n", __func__, mode);
			return;
		}
		reg_addr = UMT_RX0_MUX;
		/* set UMT Multiple Counter Mode to 1 to support eight UMT-Rx counters */
		umt_w32_mask(priv->membase, BIT(3), BIT(3), UMT_MODE_REG);
		for (i = 0; i < umt->dma_ch_num; i++) {
			umt_rx_counter = dma_ch + 8 + i;
			mux_sel = umt_r32(priv->membase, reg_addr) &
				(~((0x0F) << (i * 4)));
			mux_sel |= (umt_rx_counter << (i * 4));
			umt_w32(priv->membase, mux_sel, reg_addr);
		}
	} else {
		dma_cid = _DMA_CONTROLLER(umt->dma_id);
		dma_ch = _DMA_CHANNEL(umt->dma_id);
		if (priv->reg_type == REG_LGM) {
			if (umt->id >= 4) {
				temp_id = umt->id - 4;
				reg_addr = UMT_TRG_MUX_1;
			} else {
				temp_id = umt->id;
				reg_addr = UMT_TRG_MUX_0;
			}
			if (dma_cid == 2)
				umt_rx_counter = dma_ch - 2;
			else if (dma_cid == 1)
				umt_rx_counter = dma_ch + 2;
			else
				return;
			umt_tx_counter = umt->cqm_dq_pid - 19;
		} else {
			if (umt->id >= 3) {
				dev_err(dev, "%s: umt->id=%d is error.\n", __func__, umt->id);
				return;
			} else {
				temp_id = umt->id;
				reg_addr = UMT_TRG_MUX_0;
			}
			umt_rx_counter = dma_ch;
			if ((umt->cqm_dq_pid >= 4) && (umt->cqm_dq_pid <= 6))
				umt_tx_counter = umt->cqm_dq_pid - 4;
			else
				return;
		}

		/* set UMT Multiple Counter Mode to 0 to support one UMT-Rx counters */
		umt_w32_mask(priv->membase, BIT(3), ~BIT(3), UMT_MODE_REG);
		mux_sel = umt_r32(priv->membase, reg_addr) &
				(~((0xF000F) << (temp_id * 4)));
		mux_sel |= ((umt_rx_counter << (temp_id * 4)) |
				(umt_tx_counter << (16 + (temp_id * 4))));
		umt_w32(priv->membase, mux_sel, reg_addr);
	}
}

static inline void umt_set_endian(int dw_swp, int byte_swp)
{
	u32 val;
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;
	val = umt_r32(priv->membase, UMT_GCTRL);
	if (byte_swp)
		val |= UMT_GCTRL_OCP_UMT_ENDI_B_MASK;
	else
		val &= ~(UMT_GCTRL_OCP_UMT_ENDI_B_MASK);

	if (dw_swp)
		val |= UMT_GCTRL_OCP_UMT_ENDI_W_MASK;
	else
		val &= ~(UMT_GCTRL_OCP_UMT_ENDI_W_MASK);

	umt_w32(priv->membase, val, UMT_GCTRL);
}

static inline void umt_en_endian_mode(void)
{
	if (IS_ENABLED(CONFIG_CPU_BIG_ENDIAN))
		umt_set_endian(1, 0);
	else
		umt_set_endian(1, 1);
}

static void umt_enable(struct dp_umt_param *umt)
{
	u32 val, pos;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	if (priv->reg_type == REG_LGM) {
		entry = &priv->umts[umt->id];
		pos = umt->id * 4;
		if ((umt->flag & DP_UMT_ENABLE_FLAG) == 0) {
			entry->flag_bit.flag_bit.enabled = 0;
			entry->param.flag &= ~DP_UMT_ENABLE_FLAG;
			umt_w32_mask(priv->membase, (1 << pos), (0) << pos, UMT_MODE_REG);
		} else {
			entry->flag_bit.flag_bit.enabled = 1;
			entry->param.flag |= DP_UMT_ENABLE_FLAG;
			umt_w32_mask(priv->membase, (1 << pos), (1) << pos, UMT_MODE_REG);
		}
	} else {
		entry = &priv->umts[umt->id];
		if ((umt->flag & DP_UMT_ENABLE_FLAG) == 0) {
			entry->flag_bit.flag_bit.enabled = 0;
			entry->param.flag &= ~DP_UMT_ENABLE_FLAG;
		} else {
			entry->flag_bit.flag_bit.enabled = 1;
			entry->param.flag |= DP_UMT_ENABLE_FLAG;
		}
		if (!umt->id) {
			umt_w32_mask(priv->membase, 0x4, ((u32)entry->flag_bit.flag_bit.enabled) << 2, UMT_GCTRL);
		} else {
			pos = 17 + (umt->id - 1) * 3;
			val = (umt_r32(priv->membase, UMT_GCTRL) & ~BIT(pos))
					| (((u32)entry->flag_bit.flag_bit.enabled) << pos);
			umt_w32(priv->membase, val, UMT_GCTRL);
		}
	}
}

static void umt_suspend(struct dp_umt_param *umt)
{
	u32 pos;
	u8 halted;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	entry = &priv->umts[umt->id];
	if ((umt->flag & DP_UMT_SUSPEND_SENDING_COUNT) == 0) {
		entry->flag_bit.flag_bit.halted = 0;
		entry->param.flag &= ~DP_UMT_SUSPEND_SENDING_COUNT;
	} else {
		entry->flag_bit.flag_bit.halted = 1;
		entry->param.flag |= DP_UMT_SUSPEND_SENDING_COUNT;
	}

	if (priv->reg_type == REG_LGM) {
		if (umt->id > 3)
			pos = umt->id + 12;
		else
			pos = umt->id;
	} else {
		if (umt->id > 2)
			return;
		else
			pos = umt->id;
	}

	halted = entry->flag_bit.flag_bit.halted;
	umt_w32_mask(priv->membase, (1 << pos), halted << pos, UMT_COUNTER_CTRL);
}

static void umt_counter_clear(struct dp_umt_param *umt, u32 status)
{
	u32 pos;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	entry = &priv->umts[umt->id];

	if (priv->reg_type == REG_LGM) {
		if (umt->id > 3)
			pos = umt->id + 16;
		else
			pos = umt->id + 4;
	} else {
		if (umt->id > 2)
			return;
		else
			pos = umt->id + 4;
	}

	umt_w32_mask(priv->membase, (1 << pos), status << pos, UMT_COUNTER_CTRL);
}

static void umt_zero_send_disable(struct dp_umt_param *umt)
{
	u32 pos;
	u8 val;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;

	priv = g_dp_umt_priv;
	if (priv->reg_type != REG_LGM)
		return;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	entry = &priv->umts[umt->id];
	if ((umt->flag & DP_UMT_NOT_SENDING_ZERO_COUNT) == 0) {
		entry->flag_bit.flag_bit.not_snd_zero_cnt = 0;
		entry->param.flag &= ~DP_UMT_NOT_SENDING_ZERO_COUNT;
	} else {
		entry->flag_bit.flag_bit.not_snd_zero_cnt = 1;
		entry->param.flag |= DP_UMT_NOT_SENDING_ZERO_COUNT;
	}

	if (umt->id > 3)
		pos = umt->id + 20;
	else
		pos = umt->id + 8;

	val = (u8)entry->flag_bit.flag_bit.not_snd_zero_cnt;
	umt_w32_mask(priv->membase, (1 << pos), val << pos, UMT_COUNTER_CTRL);
}

static void umt_rx_only_send(struct dp_umt_param *umt)
{
	u32 pos;
	u8 val;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;

	priv = g_dp_umt_priv;
	if (priv->reg_type != REG_LGM)
		return;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;

	entry = &priv->umts[umt->id];
	if ((umt->flag & DP_UMT_SENDING_RX_COUNT_ONLY) == 0) {
		entry->flag_bit.flag_bit.snd_rx_only = 0;
		entry->param.flag &= ~DP_UMT_SENDING_RX_COUNT_ONLY;
	} else {
		entry->flag_bit.flag_bit.snd_rx_only = 1;
		entry->param.flag |= DP_UMT_SENDING_RX_COUNT_ONLY;
	}

	if (umt->id > 3)
		pos = umt->id + 24;
	else
		pos = umt->id + 12;

	val = (u8)entry->flag_bit.flag_bit.snd_rx_only;
	umt_w32_mask(priv->membase, (1 << pos), val << pos, UMT_COUNTER_CTRL);
}

/*This function will disable umt */
static void umt_reset_umt(struct dp_umt_param *umt)
{
	u32 mode, pos, flag, mode1;
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;

	if (!umt)
		return;
	if (umt->id >= g_max_umt_port)
		return;
	flag = umt->flag;
	mode1 = umt->msg_mode;

	if (priv->reg_type == REG_LGM) {
		/* disable umt */
		umt->flag &= ~DP_UMT_ENABLE_FLAG;
		umt_enable(umt);

		pos = (umt->id * 4) + 1;
		mode = (umt_r32(priv->membase, UMT_MODE_REG) & BIT(pos)) >> pos;

		umt_set_mode(umt);
		umt_set_mode(umt);

		umt_w32_mask(priv->membase, BIT(4), BIT(4), UMT_GCTRL);
		umt_w32_mask(priv->membase, BIT(4), ~BIT(4), UMT_GCTRL);
	} else {
		umt->flag &= ~DP_UMT_ENABLE_FLAG;
		umt_enable(umt);

		/* Bit 1 for UMT0
		 * Bit 16 for UMT1
		 * Bit 19 for UMT2
		 */
		if (!umt->id)
			mode = (umt_r32(priv->membase, UMT_GCTRL) & BIT(1)) >> 1;
		else
			mode = (umt_r32(priv->membase, UMT_GCTRL) & BIT(16 + 3 * (umt->id - 1))) >> (16 + 3 * (umt->id - 1));

		umt->msg_mode = !mode;
		umt_set_mode(umt);
		umt->msg_mode = mode;
		umt_set_mode(umt);
	}
	umt->flag = flag;
	umt->msg_mode = mode1;
}

/* read parameters from device tree node */
static void umt_entry_init(struct device_node *node, int pid)
{
	struct dp_umt_entry *entry;

	if (pid < g_dp_umt_priv->umt_num)
		entry = &g_dp_umt_priv->umts[pid];
	else
		return;

	entry->param.id = pid;

	entry->param.flag |= DP_UMT_ENABLE_FLAG;
	entry->param.msg_mode = DP_UMT_USER_MSG_MODE;
	entry->flag_bit.flag_bit.enabled = 1;
	entry->flag_bit.flag_bit.alloced = 0;
	entry->param.period = 200;
	if (pid == 0)
		entry->max_dma_ch_num = 8;
	else
		entry->max_dma_ch_num = 1;

	umt_set_mode(&entry->param);
	umt_set_msgmode(&entry->param);
	umt_set_dst(&entry->param);
	umt_set_period(&entry->param);
	umt_enable(&entry->param);
	umt_suspend(&entry->param);
	umt_zero_send_disable(&entry->param);
	umt_rx_only_send(&entry->param);
	umt_counter_clear(&entry->param, 1);
	umt_counter_clear(&entry->param, 0);
}

/**
 * debug proc should support:
 * 1. register content dump
 * 2. RX/TX trigger
 * 3. UMT status
 */

/**
 * intput:
 * @umt
 * @period: measured in microseconds.
 * ret:  Fail < 0 / Success: 0
 */
int dp_umt_set_period(struct dp_umt_param *umt, unsigned long flag)
{
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	if (!umt) {
		dev_err(dev, "UMT param is NULL!\n");
		return -EINVAL;
	}
	if (umt->period < MIN_UMT_PRD) {
		dev_err(dev, "Period (%d) is below min requirement!\n", umt->period);
		return -EINVAL;
	}

	if (umt->id >= g_max_umt_port) {
		dev_err(dev, "umit id (%d) is out of range !\n", umt->id);
		return -EINVAL;
	}

	entry = &priv->umts[umt->id];

	if (((entry->param.flag & DP_UMT_ENABLE_FLAG) == 0) || entry->flag_bit.flag_bit.enabled == 0) {
		dev_err(dev, "UMT is not initialized!\n");
		return -EINVAL;
	}

	if (entry->param.period != umt->period) {
		entry->param.period = umt->period;
		umt_set_period(umt);
	}

	return 0;
}
EXPORT_SYMBOL(dp_umt_set_period);

/**
 * API to configure the UMT port.
 * input:
 * @umt
 * @umt_mode:  0-self-counting mode, 1-user mode.
 * @msg_mode:  0-No MSG, 1-MSG0 Only, 2-MSG1 Only, 3-MSG0 & MSG1.
 * @dst:  Destination PHY address.
 * @period(ms): only applicable when set to self-counting mode.
 *              self-counting interval time. if 0, use the original setting.
 * @enable: 1-Enable/0-Disable
 * @ret:  Fail < 0 , SUCCESS:0
 */
int dp_umt_set(struct dp_umt_param *umt, unsigned long flag)
{
	u32 sw_msg;
	u32 umt_mode;
	u32 phy_dst;
	u32 period;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	if (!umt) {
		dev_err(dev, "UMT param is NULL!\n");
		return -EINVAL;
	}

	umt_mode = umt->msg_mode;
	sw_msg = umt->sw_msg;
	phy_dst = umt->daddr;
	period = umt->period;

	if (umt_mode >= (u32)DP_UMT_MODE_MAX || sw_msg >= (u32)DP_UMT_MSG_MAX ||
		phy_dst == 0 || period == 0 || umt->id >= g_max_umt_port) {
		dev_err(dev, "umt_id: %d, umt_mode: %d, sw_msg: %d.\n",
			umt->id, umt_mode, sw_msg);
		dev_err(dev, "flag: %lu phy_dst: 0x%x period: %d\n",
			umt->flag, phy_dst, period);
		return -EINVAL;
	}

	entry = &priv->umts[umt->id];
	if (entry->flag_bit.flag_bit.enabled == 0) {
		dev_err(dev, "UMT is not initialized!!\n");
		return -ENODEV;
	}

	umt_reset_umt(umt);

	entry->param.msg_mode = (enum dp_umt_msg_mode)umt_mode;
	entry->param.sw_msg = (enum dp_umt_sw_msg)sw_msg;
	entry->param.daddr = phy_dst;
	entry->param.period = period;
	entry->param.flag = umt->flag;
	if ((entry->param.flag & DP_UMT_ENABLE_FLAG) == 0)
		entry->flag_bit.flag_bit.enabled = DP_UMT_STATUS_DISABLE;
	else
		entry->flag_bit.flag_bit.enabled = DP_UMT_STATUS_ENABLE;

	if ((entry->param.flag & DP_UMT_NOT_SENDING_ZERO_COUNT) == 0)
		entry->flag_bit.flag_bit.not_snd_zero_cnt = 0;
	else
		entry->flag_bit.flag_bit.not_snd_zero_cnt = 1;

	if ((entry->param.flag & DP_UMT_SENDING_RX_COUNT_ONLY) == 0)
		entry->flag_bit.flag_bit.snd_rx_only = 0;
	else
		entry->flag_bit.flag_bit.snd_rx_only = 1;

	if ((entry->param.flag & DP_UMT_SUSPEND_SENDING_COUNT) == 0)
		entry->flag_bit.flag_bit.halted = 0;
	else
		entry->flag_bit.flag_bit.halted = 1;

	umt_set_mode(umt);
	umt_set_msgmode(umt);
	umt_set_dst(umt);
	umt_set_period(umt);
	umt_enable(umt);
	umt_suspend(umt);
	umt_zero_send_disable(umt);
	umt_rx_only_send(umt);
	umt_counter_clear(umt, 1);
	umt_counter_clear(umt, 0);

	return 0;
}
EXPORT_SYMBOL(dp_umt_set);

/**
 * API to enable/disable umt port
 * input:
 * @umt
 * @enable: Enable: 1 / Disable: 0
 * ret:  Fail < 0, Success: 0
 */
int dp_umt_enable(struct dp_umt_param *umt, unsigned long flag, int enable)
{
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	if (!umt) {
		dev_err(dev, "UMT param is NULL!\n");
		return -EINVAL;
	}

	if (umt->id >= g_max_umt_port)
		return -EINVAL;

	entry = &priv->umts[umt->id];

	if (umt->daddr == 0) {
		dev_err(dev, "input daddr: 0x%x\n", entry->param.daddr);
		return -EINVAL;
	}

	if (enable) {
		entry->flag_bit.flag_bit.enabled = DP_UMT_STATUS_ENABLE;
		entry->param.flag |= DP_UMT_ENABLE_FLAG;
	} else {
		entry->flag_bit.flag_bit.enabled = DP_UMT_STATUS_DISABLE;
		entry->param.flag &= ~DP_UMT_ENABLE_FLAG;
	}

	umt_enable(umt);

	return 0;
}
EXPORT_SYMBOL(dp_umt_enable);

/**
 * API to suspend/resume umt US/DS counter
 * input:
 * @umt
 * @halt: suspend: 1 / resume: 0
 * ret:  Fail < 0, Success: 0
 */
int dp_umt_suspend_sending(struct dp_umt_param *umt,
			   unsigned long flag, int halt)
{
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	if (umt->id >= g_max_umt_port)
		return -EINVAL;

	entry = &priv->umts[umt->id];

	if (halt >= (u32)DP_UMT_STATUS_MAX || entry->flag_bit.flag_bit.enabled == 0)
		return -ENODEV;

	if (umt->daddr == 0) {
		dev_err(dev, "input umt_dst: 0x%x\n", umt->daddr);
		return -EINVAL;
	}

	if (halt == 0) {
		entry->flag_bit.flag_bit.halted = 0;
		entry->param.flag &= ~DP_UMT_SUSPEND_SENDING_COUNT;
		umt->flag &= ~DP_UMT_SUSPEND_SENDING_COUNT;
	} else {
		entry->flag_bit.flag_bit.halted = 1;
		entry->param.flag |= DP_UMT_SUSPEND_SENDING_COUNT;
		umt->flag |= DP_UMT_SUSPEND_SENDING_COUNT;
	}
	umt_enable(umt);
	umt_suspend(umt);

	return 0;
}
EXPORT_SYMBOL(dp_umt_suspend_sending);

/**
 * API to request and allocate UMT port
 * input:
 * @umt:
 * @flag:
 * output:
 * ret: Fail: < 0,  Success: 0
 */
int dp_umt_request(struct dp_umt_param *umt, unsigned long flag)
{
	u32 mode = DP_UMT_MODE_MAX;
	int ret;
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	if (umt->daddr == 0) {
		dev_err(dev, "input umt_dst: 0x%x\n", umt->daddr);
		return -EINVAL;
	}

	if (umt->id >= g_max_umt_port &&
		umt->id != UMT_PORT_AUTO_ALLOCATE) {
		dev_err(dev, "%s: umt id %d must be in ranage(0 - %d) or auto allocate\n",
				__func__, umt->id, g_max_umt_port);
		return -EINVAL;
	}
	if (umt->dma_ch_num >= DP_UMT_RX0_MAX_DMA_NUM) {
		dev_err(dev, "%s: umt dma ch num %d must be in ranage(0 - %d)\n",
				__func__, umt->dma_ch_num, DP_UMT_RX0_MAX_DMA_NUM);
		return -EINVAL;
	}

	ret = umt_allocate(umt, &mode);
	if (ret < 0)
		return ret;

	entry = &priv->umts[umt->id];
	entry->flag_bit.flag_bit.alloced = 1;
	umt->flag |= DP_UMT_ENABLE_FLAG;
	memcpy(&entry->param, umt, sizeof(*umt));
	umt_enable(umt);
	umt_set_mux(umt, mode);

	return 0;
}
EXPORT_SYMBOL(dp_umt_request);

/**
 * API to release umt port
 * input:
 * @umt
 *
 * ret:  Fail < 0, Success: 0
 */
int dp_umt_release(struct dp_umt_param *umt, unsigned long flag)
{
	struct dp_umt_priv *priv;
	struct dp_umt_entry *entry;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	if (umt->id >= g_max_umt_port)
		return -ENODEV;

	entry = &priv->umts[umt->id];
	if ((entry->param.flag & DP_UMT_ENABLE_FLAG) == 0) {
		dev_err(dev, "UMT is not initialized!!\n");
		return -ENODEV;
	}

	memset(entry, 0, sizeof(*entry));
	umt->flag &= ~DP_UMT_ENABLE_FLAG;
	umt_enable(umt);

	return 0;
}
EXPORT_SYMBOL(dp_umt_release);

#ifdef CONFIG_DEBUG_FS
static void *dp_umt_port_seq_start(struct seq_file *s, loff_t *pos)
{
	if (*pos >= g_max_umt_port)
		return NULL;
	return &g_dp_umt_priv->umts[*pos];
}

static void *dp_umt_port_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	if (++*pos >= g_max_umt_port)
		return NULL;
	return &g_dp_umt_priv->umts[*pos];
}

static void dp_umt_port_seq_stop(struct seq_file *s, void *v)
{
}

static int dp_umt_port_seq_show(struct seq_file *s, void *v)
{
	struct dp_umt_entry *entry = v;
	int pid = entry->param.id;
	u32 val;
	struct dp_umt_priv *priv;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	seq_printf(s, "\nUMT port %d configuration\n", pid);
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "UMT Mode: \t%s\n",
			entry->param.msg_mode == DP_UMT_SELFCNT_MODE ?
			"UMT SelfCounting Mode" : "UMT User Mode");
	seq_printf(s, "UMT Allocated: \t%s\n", entry->flag_bit.flag_bit.alloced ?
			"Allocated" : "Free");
	switch (entry->param.sw_msg) {
	case DP_UMT_NO_MSG:
		seq_puts(s, "UMT MSG Mode: \tUMT NO MSG\n");
		break;
	case DP_UMT_MSG0_ONLY:
		seq_puts(s, "UMT MSG Mode: \tUMT MSG0 Only\n");
		break;
	case DP_UMT_MSG1_ONLY:
		seq_puts(s, "UMT MSG Mode: \tUMT MSG1 Only\n");
		break;
	case DP_UMT_MSG0_MSG1:
		seq_puts(s, "UMT MSG Mode: \tUMT_MSG0_And_MSG1\n");
		break;
	default:
		seq_printf(s, "UMT MSG Mode Error! Msg_mode: %d\n",
			   entry->param.sw_msg);
	}
	seq_printf(s, "UMT DST: \t0x%x\n", entry->param.daddr);
	if (entry->param.msg_mode == DP_UMT_SELFCNT_MODE)
		seq_printf(s, "UMT Period: \t%d(us)\n", entry->param.period);
	seq_printf(s, "UMT Status: \t%s\n",
		(entry->flag_bit.flag_bit.enabled != 0) ? "Enable" :
		"Disable");
	seq_printf(s, "UMT DMA CID: \t0x%x\n", entry->param.dma_id);
	seq_printf(s, "UMT DMA CH NUM: \t%d\n", entry->param.dma_ch_num);
	seq_printf(s, "UMT CQM ENQ PID: \t%d\n", entry->param.cqm_enq_pid);
	seq_printf(s, "UMT CQM DEQ PID: \t%d\n", entry->param.cqm_dq_pid);

	seq_printf(s, "++++Register dump of umt port: %d++++\n", pid);
	if (pid == 0) {
		if (priv->reg_type == REG_LGM) {
			val = umt_r32(priv->membase, UMT_MODE_REG);
			seq_printf(s, "UMT Mode: \t0x%x \t%s\n", val,
					((val & UMT_MODE_REG_UMT_MD_MASK) != 0) ? "UMT User Mode" :
					"UMT SelfCounting Mode");
			val = umt_r32(priv->membase, UMT_SW_MODE);
			switch (val & UMT_SW_MODE_SW_MSG_MD_MASK) {
			case DP_UMT_NO_MSG:
				seq_printf(s, "UMT MSG Mode: UMT NO MSG \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG0 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG1_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG1 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_MSG1:
				seq_printf(s, "UMT MSG Mode: UMT_MSG0_And_MSG1 \t0x%x\n", val);
				break;
			default:
				seq_printf(s, "UMT MSG Mode Error! Msg_mode: %d\n", val);
			}
			val = umt_r32(priv->membase, UMT_DEST_0);
			seq_printf(s, "UMT Dst: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_PERD);
			seq_printf(s, "UMT Period: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_MSG(0));
			seq_printf(s, "UMT MSG0: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_MSG(1));
			seq_printf(s, "UMT MSG1: \t0x%x\n", val);
			if (umt_r32(priv->membase, UMT_MODE_REG) &
				UMT_MODE_REG_UMT_CNT_MOD_MASK) {
				val = umt_r32(priv->membase, UMT_RX0_MUX);
				seq_printf(s, "UMT_RX0_MUX: \t0x%x\n", val);
				seq_printf(s, "UMT_RX0_SEL0: \t%d UMT_RX0_SEL1: \t%d\n", (val & UMT_RX0_MUX_UMT_RX0_SEL0_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL0_POS, (val & UMT_RX0_MUX_UMT_RX0_SEL1_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL1_POS);
				seq_printf(s, "UMT_RX0_SEL2: \t%d UMT_RX0_SEL3: \t%d\n", (val & UMT_RX0_MUX_UMT_RX0_SEL2_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL2_POS, (val & UMT_RX0_MUX_UMT_RX0_SEL3_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL3_POS);
				seq_printf(s, "UMT_RX0_SEL4: \t%d UMT_RX0_SEL5: \t%d\n", (val & UMT_RX0_MUX_UMT_RX0_SEL4_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL4_POS, (val & UMT_RX0_MUX_UMT_RX0_SEL5_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL5_POS);
				seq_printf(s, "UMT_RX0_SEL6: \t%d UMT_RX0_SEL7: \t%d\n", (val & UMT_RX0_MUX_UMT_RX0_SEL6_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL6_POS, (val & UMT_RX0_MUX_UMT_RX0_SEL7_MASK) >> UMT_RX0_MUX_UMT_RX0_SEL7_POS);
			} else {
				val = umt_r32(priv->membase, UMT_TRG_MUX_0);
				seq_printf(s, "UMT_TRG_MUX_0: \t0x%x\n", val);
				seq_printf(s, "UMT_RX0_SEL: \t%d UMT_TX0_SEL: \t%d\n", (val & UMT_TRG_MUX_0_UMT_RX0_SEL_MASK) >> UMT_TRG_MUX_0_UMT_RX0_SEL_POS, (val & UMT_TRG_MUX_0_UMT_TX0_SEL_MASK) >> UMT_TRG_MUX_0_UMT_TX0_SEL_POS);
			}
		} else {
			seq_printf(s, "UMT Status: \t%s\n",
				   (umt_r32(priv->membase, UMT_GCTRL) & BIT(2)) != 0 ?
				   "Enable" : "Disable");
			seq_printf(s, "UMT Mode: \t%s\n",
				   (umt_r32(priv->membase, UMT_GCTRL) & BIT(1)) != 0 ?
				   "UMT User MSG mode" : "UMT SelfCounting mode");
			val = umt_r32(priv->membase, UMT_SW_MODE_PRX);
			switch (val & UMT_SW_MODE_SW_MSG_MD_MASK) {
			case DP_UMT_NO_MSG:
				seq_printf(s, "UMT MSG Mode: UMT NO MSG \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG0 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG1_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG1 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_MSG1:
				seq_printf(s, "UMT MSG Mode: UMT_MSG0_And_MSG1 \t0x%x\n", val);
				break;
			default:
				seq_printf(s, "UMT MSG Mode Error! Msg_mode: %d\n", val);
			}
			val = umt_r32(priv->membase, UMT_DEST_PRX);
			seq_printf(s, "UMT Dst: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_PERD);
			seq_printf(s, "UMT Period: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_MSG(0));
			seq_printf(s, "UMT MSG0: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_MSG(1));
			seq_printf(s, "UMT MSG1: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_TRG_MUX_0);
			seq_printf(s, "UMT_TRG_MUX_0: \t0x%x\n", val);
			seq_printf(s, "UMT_RX0_SEL: \t%d UMT_TX0_SEL: \t%d\n", (val & UMT_TRG_MUX_0_UMT_RX0_SEL_MASK) >> UMT_TRG_MUX_0_UMT_RX0_SEL_POS, (val & UMT_TRG_MUX_0_UMT_TX0_SEL_MASK) >> UMT_TRG_MUX_0_UMT_TX0_SEL_POS);
		}
	} else {
		if (priv->reg_type == REG_LGM) {
			val = umt_r32(priv->membase, UMT_MODE_REG);
			seq_printf(s, "UMT Mode: \t0x%x \t%s\n", val,
				((val & (UMT_MODE_REG_UMT_MD_MASK << (pid * 4))) != 0) ? "UMT User Mode" :
				"UMT SelfCounting Mode");
			val = umt_r32(priv->membase, UMT_X_ADDR(pid, UMT_X_SW_MODE));
			switch (val & UMT_SW_MODE_SW_MSG_MD_MASK) {
			case DP_UMT_NO_MSG:
				seq_printf(s, "UMT MSG Mode: UMT NO MSG \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG0 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG1_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG1 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_MSG1:
				seq_printf(s, "UMT MSG Mode: UMT_MSG0_And_MSG1 \t0x%x\n", val);
				break;
			default:
				seq_printf(s, "UMT MSG Mode Error! Msg_mode: %d\n", val);
			}
			val = umt_r32(priv->membase, UMT_X_ADDR(pid, UMT_X_DEST));
			seq_printf(s, "UMT Dst: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_X_ADDR(pid, UMT_X_PERIOD));
			seq_printf(s, "UMT Period: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_X_ADDR(pid, UMT_X_MSG(0)));
			seq_printf(s, "UMT MSG0: \t0x%x\n", val);
			val = umt_r32(priv->membase, UMT_X_ADDR(pid, UMT_X_MSG(1)));
			seq_printf(s, "UMT MSG1: \t0x%x\n", val);
			switch (pid) {
			case 1:
				val = umt_r32(priv->membase, UMT_TRG_MUX_0);
				seq_printf(s, "UMT_TRG_MUX_0: \t0x%x\n", val);
				seq_printf(s, "UMT_RX1_SEL: \t%d UMT_TX1_SEL: \t%d\n", (val & UMT_TRG_MUX_0_UMT_RX1_SEL_MASK) >> UMT_TRG_MUX_0_UMT_RX1_SEL_POS, (val & UMT_TRG_MUX_0_UMT_TX1_SEL_MASK) >> UMT_TRG_MUX_0_UMT_TX1_SEL_POS);
				break;
			case 2:
				val = umt_r32(priv->membase, UMT_TRG_MUX_0);
				seq_printf(s, "UMT_TRG_MUX_0: \t0x%x\n", val);
				seq_printf(s, "UMT_RX2_SEL: \t%d UMT_TX2_SEL: \t%d\n", (val & UMT_TRG_MUX_0_UMT_RX2_SEL_MASK) >> UMT_TRG_MUX_0_UMT_RX2_SEL_POS, (val & UMT_TRG_MUX_0_UMT_TX2_SEL_MASK) >> UMT_TRG_MUX_0_UMT_TX2_SEL_POS);
				break;
			case 3:
				val = umt_r32(priv->membase, UMT_TRG_MUX_0);
				seq_printf(s, "UMT_TRG_MUX_0: \t0x%x\n", val);
				seq_printf(s, "UMT_RX3_SEL: \t%d UMT_TX3_SEL: \t%d\n", (val & UMT_TRG_MUX_0_UMT_RX3_SEL_MASK) >> UMT_TRG_MUX_0_UMT_RX3_SEL_POS, (val & UMT_TRG_MUX_0_UMT_TX3_SEL_MASK) >> UMT_TRG_MUX_0_UMT_TX3_SEL_POS);
				break;
			case 4:
				val = umt_r32(priv->membase, UMT_TRG_MUX_1);
				seq_printf(s, "UMT_TRG_MUX_1: \t0x%x\n", val);
				seq_printf(s, "UMT_RX4_SEL: \t%d UMT_TX4_SEL: \t%d\n", (val & UMT_TRG_MUX_1_UMT_RX0_SEL_MASK) >> UMT_TRG_MUX_1_UMT_RX0_SEL_POS, (val & UMT_TRG_MUX_1_UMT_TX0_SEL_MASK) >> UMT_TRG_MUX_1_UMT_TX0_SEL_POS);
				break;
			case 5:
				val = umt_r32(priv->membase, UMT_TRG_MUX_1);
				seq_printf(s, "UMT_TRG_MUX_1: \t0x%x\n", val);
				seq_printf(s, "UMT_RX5_SEL: \t%d UMT_TX5_SEL: \t%d\n", (val & UMT_TRG_MUX_1_UMT_RX1_SEL_MASK) >> UMT_TRG_MUX_1_UMT_RX1_SEL_POS, (val & UMT_TRG_MUX_1_UMT_TX1_SEL_MASK) >> UMT_TRG_MUX_1_UMT_TX1_SEL_POS);
				break;
			case 6:
				val = umt_r32(priv->membase, UMT_TRG_MUX_1);
				seq_printf(s, "UMT_TRG_MUX_1: \t0x%x\n", val);
				seq_printf(s, "UMT_RX6_SEL: \t%d UMT_TX6_SEL: \t%d\n", (val & UMT_TRG_MUX_1_UMT_RX2_SEL_MASK) >> UMT_TRG_MUX_1_UMT_RX2_SEL_POS, (val & UMT_TRG_MUX_1_UMT_TX2_SEL_MASK) >> UMT_TRG_MUX_1_UMT_TX2_SEL_POS);
				break;
			case 7:
				val = umt_r32(priv->membase, UMT_TRG_MUX_1);
				seq_printf(s, "UMT_TRG_MUX_1: \t0x%x\n", val);
				seq_printf(s, "UMT_RX7_SEL: \t%d UMT_TX7_SEL: \t%d\n", (val & UMT_TRG_MUX_1_UMT_RX3_SEL_MASK) >> UMT_TRG_MUX_1_UMT_RX3_SEL_POS, (val & UMT_TRG_MUX_1_UMT_TX3_SEL_MASK) >> UMT_TRG_MUX_1_UMT_TX3_SEL_POS);
				break;
			default:
				break;
			}
		} else {
			if (pid == 1) {
				seq_printf(s, "UMT Status: \t%s\n",
					   (umt_r32(priv->membase, UMT_GCTRL) & BIT(17)) != 0 ?
					   "Enable" : "Disable");
				seq_printf(s, "%s reg_addr=0x%08x val=0x%08x\n", __func__, UMT_GCTRL, umt_r32(priv->membase, UMT_GCTRL));
				seq_printf(s, "UMT Mode: \t%s\n",
					   (umt_r32(priv->membase, UMT_GCTRL) & BIT(16)) != 0 ?
					   "UMT User MSG mode" : "UMT SelfCounting mode");
			} else {
				seq_printf(s, "UMT Status: \t%s\n",
					   (umt_r32(priv->membase, UMT_GCTRL) & BIT(20)) != 0 ?
					   "Enable" : "Disable");
				seq_printf(s, "%s reg_addr=0x%08x val=0x%08x\n", __func__, UMT_GCTRL, umt_r32(priv->membase, UMT_GCTRL));
				seq_printf(s, "UMT Mode: \t%s\n",
					   (umt_r32(priv->membase, UMT_GCTRL) & BIT(19)) != 0 ?
					   "UMT User MSG mode" : "UMT SelfCounting mode");
			}
			val = umt_r32(priv->membase, UMT_X_ADDR_PRX(pid, UMT_X_SW_MODE_PRX));
			seq_printf(s, "UMT_SW_MODE_PRX=0x%08x: \t0x%08x\n", UMT_X_ADDR_PRX(pid, UMT_X_SW_MODE_PRX), val);
			switch (val & UMT_SW_MODE_SW_MSG_MD_MASK) {
			case DP_UMT_NO_MSG:
				seq_printf(s, "UMT MSG Mode: UMT NO MSG \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG0 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG1_ONLY:
				seq_printf(s, "UMT MSG Mode: UMT MSG1 Only \t0x%x\n", val);
				break;
			case DP_UMT_MSG0_MSG1:
				seq_printf(s, "UMT MSG Mode: UMT_MSG0_And_MSG1 \t0x%x\n", val);
				break;
			default:
				seq_printf(s, "UMT MSG Mode Error! Msg_mode: %d\n", val);
			}
			val = umt_r32(priv->membase, UMT_X_ADDR_PRX(pid, UMT_X_DEST_PRX));
			seq_printf(s, "UMT Dst=0x%08x: \t0x%x\n", UMT_X_ADDR_PRX(pid, UMT_X_DEST_PRX), val);
			val = umt_r32(priv->membase, UMT_X_ADDR_PRX(pid, UMT_X_PERIOD_PRX));
			seq_printf(s, "UMT Period=0x%08x: \t0x%x\n", UMT_X_ADDR_PRX(pid, UMT_X_PERIOD_PRX), val);
			val = umt_r32(priv->membase, UMT_X_ADDR_PRX(pid, UMT_X_MSG(0)));
			seq_printf(s, "UMT MSG0=0x%08x: \t0x%x\n", UMT_X_ADDR_PRX(pid, UMT_X_MSG(0)), val);
			val = umt_r32(priv->membase, UMT_X_ADDR_PRX(pid, UMT_X_MSG(1)));
			seq_printf(s, "UMT MSG1=0x%08x: \t0x%x\n", UMT_X_ADDR_PRX(pid, UMT_X_MSG(1)), val);
			val = umt_r32(priv->membase, UMT_TRG_MUX_0);
			seq_printf(s, "UMT_TRG_MUX_0=0x%08x: \t0x%x\n", UMT_TRG_MUX_0, val);
		}
	}

	return 0;
}

static const struct seq_operations dp_umt_port_seq_ops = {
	.start = dp_umt_port_seq_start,
	.next = dp_umt_port_seq_next,
	.stop = dp_umt_port_seq_stop,
	.show = dp_umt_port_seq_show,
};

static int dp_umt_cfg_read_debugfs_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &dp_umt_port_seq_ops);
}

static const struct file_operations dp_umt_debugfs_fops = {
	.open		= dp_umt_cfg_read_debugfs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static int dp_umt_debugfs_init(void)
{
	struct dentry *entry;
	struct dp_umt_priv *priv;
	struct device *dev;

	priv = g_dp_umt_priv;
	dev = &priv->dev->dev;

	priv->debugfs = debugfs_create_dir("umt", NULL);
	if (!priv->debugfs)
		return -ENOMEM;

	entry = debugfs_create_file("umt_info", 0644, priv->debugfs,
				    NULL, &dp_umt_debugfs_fops);
	if (!entry)
		goto err1;

	return 0;
err1:
	debugfs_remove_recursive(priv->debugfs);
	priv->debugfs = NULL;
	dev_err(dev, "UMT debugfs create fail!\n");
	return -ENOMEM;
}
#endif

static int dp_umt_drv_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	int i;
	struct resource *res;
	struct dp_umt_priv *priv;
	void __iomem *addr_base;
	struct device *dev;
	const struct intel_match_data *data;

	if (pdev == NULL) {
		pr_err("Wrong pdev\n");
		return -EINVAL;
	}
	dev = &pdev->dev;

	data = of_device_get_match_data(dev);
	if (!data) {
		dev_err(&pdev->dev, "Error: No device match found\n");
		return -ENODEV;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		dev_err(dev, "Alloc umt priv fail!\n");
		return -ENOMEM;
	}
	g_dp_umt_priv = priv;
	priv->dev = pdev;
	priv->reg_type = data->reg_type;
	if (priv->reg_type == REG_LGM)
		g_max_umt_port = DP_UMT_ENTRY_NUM_LGM;
	else
		g_max_umt_port = DP_UMT_ENTRY_NUM_PRX;

	priv->clk = of_clk_get_by_name(node, "freq");

	if (IS_ERR_VALUE(priv->clk)) {
		dev_err(dev, "umt clock is missing.\n");
		priv->clk = NULL;
	} else {
		dev_dbg(dev, "UMT clock rate is %lu\n", clk_get_rate(priv->clk));
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "failed to get the umt resource!\n");
		goto error_device;
	}

	addr_base = devm_ioremap_resource(&pdev->dev, res);
	if (!addr_base) {
		dev_err(dev, "failed to request amd map the umt io range!\n");
		goto error_device;
	}
	priv->membase = addr_base;

	umt_en_endian_mode();

	priv->umt_num = g_max_umt_port;
	priv->umts = &g_dp_umt_entry[0];
	for (i = 0; i < g_max_umt_port; i++)
		umt_entry_init(node, i);

#ifdef CONFIG_DEBUG_FS
	dp_umt_debugfs_init();
#endif

	dev_info(dev, "INTEL DATAPATH UMT driver initialize success on processor: %d!\n",
		smp_processor_id());

	spin_lock_init(&priv->umt_lock);

	return 0;
error_device:
	kfree(priv);
	return -ENODEV;
}

static int dp_umt_drv_remove(struct platform_device *pdev)
{
	struct dp_umt_priv *priv;

	priv = g_dp_umt_priv;
#ifdef CONFIG_DEBUG_FS
	debugfs_remove_recursive(priv->debugfs);
#endif
	kfree(g_dp_umt_priv);
	dev_info(&pdev->dev, "Intel DATAPATH UMT driver remove!\n");
	return 0;
}

static struct intel_match_data lgm = { .reg_type = REG_LGM };
static struct intel_match_data prx = { .reg_type = REG_PRX };

static const struct of_device_id dp_umt_drv_match[] = {
	{ .compatible = "intel,lgm-datapath-umt", .data = &lgm},
	{ .compatible = "intel,prx300-datapath-umt", .data = &prx},
	{},
};
MODULE_DEVICE_TABLE(of, dp_umt__drv_match);

static struct platform_driver dp_umt_driver = {
	.probe = dp_umt_drv_probe,
	.remove = dp_umt_drv_remove,
	.driver = {
		.name = "intel,datapath-umt",
		.of_match_table = dp_umt_drv_match,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(dp_umt_driver);

MODULE_LICENSE("GPL v2");
