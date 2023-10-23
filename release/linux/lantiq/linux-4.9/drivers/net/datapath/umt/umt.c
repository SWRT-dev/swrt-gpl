// SPDX-License-Identifier: GPL-2.0
/*  Copyright (C) 2018 Intel Corporation.
 *  Wu ZhiXian <zhixian.wu@intel.com>
 *  Zhu Yixin <yixin.zhu@intel.com>
 */

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <net/datapath_api.h>

/* reg definition */
#define UMT_MODE		0x100
#define P0_CNT_MODE_OFF		3
#define UMT_CNT_CTRL		0xE4
#define UMT_TRG_MUX(x)		((x) < 4 ? 0xE0 : 0xF0)
#define UMT_RX_MUX_OFF(x)	((x) < 4 ? (x) * 4 : ((x) - 4) << 2)
#define UMT_TX_MUX_OFF(x)	(UMT_RX_MUX_OFF(x) + 16)
#define UMT_RX0_MUX		0xF4
#define UMT_MUX_WIDTH		4
#define UMT_GCTRL		0x200
#define RST_OFF			4
#define PORT_EN_OFF		8
#define BYTE_SWAP_OFF		28
#define DW_SWAP_OFF		29
#define to_mask(x)		(BIT(x) - 1)
#define SND_DIS_OFF(x)		((x) < 4 ? (x) : (x) + 16)
#define CLR_CNT_OFF(x)		((x) < 4 ? (x) + 4 : (x) + 20)
#define NOT_SND_ZERO_OFF(x)	((x) < 4 ? (x) + 8 : (x) + 24)
#define RX_SND_DIS_OFF(x)	((x) < 4 ? (x) + 12 : (x) + 28)

enum dma_cid {
	DMA0TX,
	DMA1TX,
	DMA2TX,
	DMA_CTRL_MAX
};

enum soc_ver {
	PRX300_SOC,
	LGM_SOC
};

struct umt_soc_data {
	u8            umt_port_num;
	enum soc_ver  ver;
	unsigned long rx_src_cap;
	unsigned long feature_cap;
	unsigned int  enq_base;
	unsigned int  dq_base;
};

struct umt_port {
	struct dp_umt_port cfg;
	int                id;
	bool               allocated;
	u8                 dma_cid;
	u16                dma_chid;
	unsigned long      rx_src_cap;
	unsigned int       dma_chn_cap;
	unsigned long      feature_cap;
};

/**
 * struct umt_priv
 * dev: platform device.
 * membase: UMT register base address.
 * umt_num: number of UMT entries.
 * dw_swap: Control OCP SWAPPER WORD for UMT.
 * byte_swap: Control OCP SWAPPER byte for UMT.
 * umts: umt entry list.
 * umt_lock: protect the umt global resource.
 * clk: clock source.
 * debugfs: debugfs proc.
 */
struct umt_priv {
	struct device             *dev;
	void __iomem              *membase;
	u8                        umt_port_num;
	bool                      dw_swap;
	bool                      byte_swap;
	struct umt_port           *ports;
	struct clk                *clk;
	struct dentry             *debugfs;
	struct umt_ops            ops;
	const struct umt_soc_data *soc;
	spinlock_t                lock; /* lock protection */
};

#define DEFINE_SHOW_ATTRIBUTE(__name)					\
	static int __name ## _open(struct inode *inode, struct file *file) \
{									\
	return single_open(file, __name ## _show, inode->i_private);	\
}									\
									\
static const struct file_operations __name ## _fops = {			\
	.owner		= THIS_MODULE,					\
	.open		= __name ## _open,				\
	.read		= seq_read,					\
	.llseek		= seq_lseek,					\
	.release	= single_release,				\
}

static const char * const umt_mode_str[] = {"Self Counting mode", "User mode"};
static const char * const umt_sw_mode_str[] = {
	"No MSG", "MSG0 Only", "MSG1 only", "MSG0 and MSG1"};

/* Address calculate for different SoC */
static bool is_prx300_soc(struct umt_priv *priv)
{
	return (priv->soc->ver == PRX300_SOC);
}

static unsigned int reg_addr_sw_mode(struct umt_priv *priv, unsigned int pid)
{
	if (is_prx300_soc(priv))
		return !pid ? 0x218 : 0x334 + pid * 0x100;

	return !pid ? 0x114 : 0x2b0 + pid * 0x100;
}

static unsigned int reg_addr_period(struct umt_priv *priv, unsigned int pid)
{
	if (is_prx300_soc(priv))
		return !pid ? 0xdc : 0x320 + pid * 0x100;

	return !pid ? 0xdc : 0x2a0 + pid * 0x100;
}

static unsigned int reg_addr_dst(struct umt_priv *priv, unsigned int pid)
{
	if (is_prx300_soc(priv))
		return !pid ? 0x230 : 0x330 + pid * 0x100;

	return !pid ? 0x280 : 0x280 + pid * 0x100;
}

static unsigned int reg_addr_mode(struct umt_priv *priv)
{
	return is_prx300_soc(priv) ? UMT_GCTRL : UMT_MODE;
}

static unsigned int reg_mode_off(struct umt_priv *priv, unsigned int pid)
{
	if (is_prx300_soc(priv))
		return !pid ? 1 : 16 + (pid - 1) * 3;

	return (pid << 2) + 1;
}

static unsigned int reg_en_off(struct umt_priv *priv, unsigned int pid)
{
	if (is_prx300_soc(priv))
		return !pid ? 2 : 17 + (pid - 1) * 3;

	return pid << 2;
}

static u32 umt_reg_read(struct umt_priv *priv, unsigned int reg)
{
	return readl(priv->membase + reg);
}

static void
umt_reg_write(struct umt_priv *priv, unsigned int reg, unsigned int val)
{
	writel(val, priv->membase + reg);
}

static void
umt_reg_update_bit(struct umt_priv *priv, unsigned int reg,
		   unsigned int off, unsigned int val)
{
	u32 reg_val;

	val = !!val;
	reg_val = umt_reg_read(priv, reg) & ~BIT(off);
	reg_val |= val << off;
	umt_reg_write(priv, reg, reg_val);
}

static void
umt_reg_write_mask(struct umt_priv *priv, unsigned int reg,
		   unsigned int mask, unsigned int off, unsigned int val)
{
	u32 reg_val;

	reg_val = umt_reg_read(priv, reg) & ~(mask << off);
	reg_val |= (val & mask) << off;
	umt_reg_write(priv, reg, reg_val);
}

/* input in term of microseconds */
static u32 umt_us_to_cnt(struct umt_priv *priv, unsigned int usec)
{
	unsigned long clk;
	u64 period;

	clk = clk_get_rate(priv->clk);

	period = (u64)usec * clk;
	do_div(period, 100000);
	if (period > 0xFFFFFFFFULL) {
		dev_info(priv->dev, "UMT period exceeds max value! set to 0xFFFFFFFF.\n");
		return 0xFFFFFFFF;
	}

	return (u32)period;
}

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
static void umt_hw_init(struct umt_priv *priv)
{
	/* UMT reset */
	umt_reg_update_bit(priv, UMT_GCTRL, RST_OFF, 1);
	usleep_range(10, 50);
	umt_reg_update_bit(priv, UMT_GCTRL, RST_OFF, 0);

	/* Enable UMT PORT */
	umt_reg_write_mask(priv, UMT_GCTRL, BIT(priv->umt_port_num) - 1,
			   PORT_EN_OFF, BIT(priv->umt_port_num) - 1);

	umt_reg_update_bit(priv, UMT_GCTRL, BYTE_SWAP_OFF, priv->byte_swap);
	umt_reg_update_bit(priv, UMT_GCTRL, DW_SWAP_OFF, priv->dw_swap);
}

static int umt_dma_check(struct umt_priv *priv, u32 dma_id)
{
	u8 cid;
	u16 chid;

	dp_dma_parse_id(dma_id, &cid, NULL, &chid);

	if (priv->soc->ver == LGM_SOC) {
		if (cid == DMA0TX && chid <= 7)
			return 0;
		if (cid == DMA1TX && chid >= 2 && chid <= 5)
			return 0;
		if (cid == DMA2TX && chid >= 2 && chid <= 5)
			return 0;
	} else if (cid == DMA1TX && chid < 16) {
		return 0;
	}

	return -EINVAL;
}

static int
umt_check_cfg(struct umt_priv *priv, struct umt_port *umt_p,
	      struct dp_umt_port *port)
{
	struct device *dev = priv->dev;
	struct umt_port_ctl *ctl = &port->ctl;
	struct umt_port_res *res = &port->res;

	/* check UMT RX CAP */
	if (!(BIT(res->rx_src) & umt_p->rx_src_cap)) {
		dev_err(dev, "UMT RX src: %u not supported!\n", res->rx_src);
		return -EINVAL;
	}

	/* check UMT DMA CAP */
	if (res->rx_src == UMT_RX_SRC_DMA) {
		if (res->dma_ch_num > umt_p->dma_chn_cap) {
			dev_err(dev, "UMT RX DMA CHNUM: %u not supported on pid: %u\n",
				res->dma_ch_num, umt_p->id);
			return -EINVAL;
		}

		/* check UMT DMA Ctrl/channel CAP */
		if (umt_dma_check(priv, res->dma_id)) {
			dev_err(dev, "UMT RX DMA: %u not supported on pid: %u\n",
				res->dma_id, umt_p->id);
			return -EINVAL;
		}
	}

	/* check UMT feature CAP */
	if ((ctl->fflag & umt_p->feature_cap) != ctl->fflag) {
		dev_err(dev,
			"UMT feature not supported: req:0x%lx, hwcap:0x%lx\n",
			ctl->fflag, umt_p->feature_cap);
		return -EINVAL;
	}

	return 0;
}

static int umt_find_pid(struct umt_priv *priv, struct dp_umt_port *port)
{
	int i;

	for (i = priv->umt_port_num - 1; i >= 0; i--) {
		if (priv->ports[i].allocated)
			continue;

		if (!umt_check_cfg(priv, &priv->ports[i], port))
			return i;
	}

	return -1;
}

static void umt_set_sw_mode(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_ctl *ctl = &umt_p->cfg.ctl;
	unsigned int id = umt_p->id;

	umt_reg_write(priv, reg_addr_sw_mode(priv, id), ctl->sw_msg);
}

static void umt_set_period(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_ctl *ctl = &umt_p->cfg.ctl;
	unsigned int id = umt_p->id;
	u32 period = umt_us_to_cnt(priv, ctl->msg_interval);

	umt_reg_write(priv, reg_addr_period(priv, id), period);
}

static void umt_set_dst(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_ctl *ctl = &umt_p->cfg.ctl;
	unsigned int id = umt_p->id;

	umt_reg_write(priv, reg_addr_dst(priv, id), (u32)ctl->daddr);
}

static void umt_set_mode(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_ctl *ctl = &umt_p->cfg.ctl;
	unsigned int id = umt_p->id;

	umt_reg_update_bit(priv, reg_addr_mode(priv),
			   reg_mode_off(priv, id), ctl->msg_mode);
}

static int umt_get_rxmux(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_res *res = &umt_p->cfg.res;
	const unsigned int cqm_pbase = priv->soc->enq_base;
	int rxmux;

	if (is_prx300_soc(priv))
		return umt_p->dma_chid;

	/* set RX0 MUX */
	if (res->rx_src == UMT_RX_SRC_CQEM) {
		if (res->cqm_enq_pid < cqm_pbase) {
			dev_err(priv->dev, "CQM invalid EQ PID: %u",
				res->cqm_enq_pid);
			return -EINVAL;
		} else {
			return res->cqm_enq_pid - cqm_pbase;
		}
	}

	switch (umt_p->dma_cid) {
	case DMA0TX:
		rxmux = 8 + umt_p->dma_chid;
		break;
	case DMA1TX:
		rxmux = 2 + umt_p->dma_chid;
		break;
	case DMA2TX:
		rxmux = umt_p->dma_chid - 2;
		break;
	default:
		rxmux = -1;
		break;
	}

	return rxmux;
}

static int umt_rx0_mux_off(struct umt_priv *priv, unsigned int chid)
{
	if (chid < 3)
		return chid << 2;
	else if (chid >= 3 && chid < 6)
		return ((chid - 3) << 2) + 16;
	else if (chid == 6)
		return 12;
	else
		return 28;
}

static int umt_set_rxmux(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_res *res = &umt_p->cfg.res;
	const unsigned int max_mux = 0xf;
	unsigned int id = umt_p->id;
	int i, rxmux;

	rxmux = umt_get_rxmux(priv, umt_p);
	if (rxmux < 0 || rxmux > max_mux) {
		dev_err(priv->dev, "UMT Port %u invalid rxmux: %d\n",
			id, rxmux);
		return -EINVAL;
	}

	/* set RX0 MUX */
	if (!id && res->rx_src == UMT_RX_SRC_DMA) {
		if (res->dma_ch_num == 1) {
			umt_reg_update_bit(priv, UMT_MODE, P0_CNT_MODE_OFF, 0);
		} else {
			umt_reg_update_bit(priv, UMT_MODE, P0_CNT_MODE_OFF, 1);
			for (i = 0; i < res->dma_ch_num; i++) {
				umt_reg_write_mask(priv, UMT_RX0_MUX,
						   to_mask(UMT_MUX_WIDTH),
						   umt_rx0_mux_off(priv, i),
						   rxmux + i);
			}
			return 0;
		}
	}

	umt_reg_write_mask(priv, UMT_TRG_MUX(id), to_mask(UMT_MUX_WIDTH),
			   UMT_RX_MUX_OFF(id), rxmux);

	return 0;
}

static int umt_get_txmux(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_res *res = &umt_p->cfg.res;
	const int cqm_dpbase = priv->soc->dq_base;

	if (res->cqm_dq_pid < cqm_dpbase)
		return -EINVAL;

	return res->cqm_dq_pid - cqm_dpbase;
}

static int umt_set_txmux(struct umt_priv *priv, struct umt_port *umt_p)
{
	const unsigned int max_mux = 0xf;
	unsigned int id = umt_p->id;
	int txmux;

	txmux = umt_get_txmux(priv, umt_p);
	if (txmux < 0 || txmux > max_mux) {
		dev_err(priv->dev, "UMT Port %u invalid txmux: %d\n",
			id, txmux);
		return -EINVAL;
	}

	umt_reg_write_mask(priv, UMT_TRG_MUX(id), to_mask(UMT_MUX_WIDTH),
			   UMT_TX_MUX_OFF(id), txmux);
	return 0;
}

static void
umt_set_feature(struct umt_priv *priv, struct umt_port *umt_p,
		unsigned long flag_mask, unsigned long vflag)
{
	unsigned int id = umt_p->id;
	bool set;

	if (flag_mask & UMT_SND_DIS) {
		set = !!(vflag & UMT_SND_DIS);
		umt_reg_update_bit(priv, UMT_CNT_CTRL, SND_DIS_OFF(id), set);
	}
	if (flag_mask & UMT_CLEAR_CNT) {
		set = !!(vflag & UMT_CLEAR_CNT);
		umt_reg_update_bit(priv, UMT_CNT_CTRL, CLR_CNT_OFF(id), set);
	}
	if (flag_mask & UMT_NOT_SND_ZERO_CNT) {
		set = !!(vflag & UMT_NOT_SND_ZERO_CNT);
		umt_reg_update_bit(priv, UMT_CNT_CTRL,
				   NOT_SND_ZERO_OFF(id), set);
	}
	if (flag_mask & UMT_SND_RX_CNT_ONLY) {
		set = !!(vflag & UMT_SND_RX_CNT_ONLY);
		umt_reg_update_bit(priv, UMT_CNT_CTRL, RX_SND_DIS_OFF(id), set);
	}
}

static void umt_enable(struct umt_priv *priv, struct umt_port *umt_p)
{
	struct umt_port_ctl *ctl = &umt_p->cfg.ctl;
	unsigned int id = umt_p->id;

	umt_reg_update_bit(priv, reg_addr_mode(priv),
			   reg_en_off(priv, id), ctl->enable);
}

/**
 * API to request and allocate UMT port
 */
static void umt_cfg(struct umt_priv *priv, struct umt_port *umt_p)
{
	spin_lock(&priv->lock);
	umt_set_sw_mode(priv, umt_p);
	umt_set_period(priv, umt_p);
	umt_set_dst(priv, umt_p);
	umt_set_mode(priv, umt_p);
	umt_set_rxmux(priv, umt_p);
	umt_set_txmux(priv, umt_p);
	umt_set_feature(priv, umt_p,
			umt_p->cfg.ctl.fflag, umt_p->cfg.ctl.fflag);
	umt_enable(priv, umt_p);
	spin_unlock(&priv->lock);
}

static int umt_port_enable(struct device *dev, unsigned int id, bool en)
{
	struct umt_port *umt_p;
	struct umt_priv *priv;
	struct umt_port_ctl *ctl;

	if (!dev) {
		dev_err(dev, "umt device pointer is NULL!\n");
		return -EINVAL;
	}

	priv = dev_get_drvdata(dev);
	if (id >= priv->umt_port_num) {
		dev_err(dev, "umt ID is invalid: %u\n", id);
		return -EINVAL;
	}

	umt_p = &priv->ports[id];
	if (!umt_p->allocated) {
		dev_err(dev, "umt port not allocated: %u\n", id);
		return -EINVAL;
	}

	ctl = &umt_p->cfg.ctl;
	spin_lock(&priv->lock);
	ctl->enable = en;
	umt_enable(priv, umt_p);
	spin_unlock(&priv->lock);

	return 0;
}

static int
umt_port_set_ctrl(struct device *dev, unsigned int id,
		  unsigned long flag_mask, unsigned long vflag)
{
	struct umt_port_ctl *ctl;
	struct umt_port *umt_p;
	struct umt_priv *priv;
	int i;

	if (!dev) {
		dev_err(dev, "umt device pointer is NULL!\n");
		return -EINVAL;
	}

	priv = dev_get_drvdata(dev);
	if (id >= priv->umt_port_num) {
		dev_err(dev, "umt ID is invalid: %u\n", id);
		return -EINVAL;
	}

	umt_p = &priv->ports[id];
	if (!umt_p->allocated) {
		dev_err(dev, "umt port not allocated: %u\n", id);
		return -EINVAL;
	}

	ctl = &umt_p->cfg.ctl;
	for (i = 0; BIT(i) <= UMT_F_MAX; i++) {
		if (flag_mask & BIT(i)) {
			if (vflag & BIT(i))
				ctl->fflag |= BIT(i);
			else
				ctl->fflag &= ~BIT(i);
		}
	}
	spin_lock(&priv->lock);
	umt_set_feature(priv, umt_p, flag_mask, vflag);
	spin_unlock(&priv->lock);

	return 0;
}

static int umt_port_release(struct device *dev, unsigned int id)
{
	struct dp_umt_port *port;
	struct umt_port *umt_p;
	struct umt_priv *priv;

	if (!dev) {
		dev_err(dev, "umt device pointer is NULL!\n");
		return -EINVAL;
	}

	priv = dev_get_drvdata(dev);
	if (id >= priv->umt_port_num) {
		dev_err(dev, "umt ID is invalid: %u\n", id);
		return -EINVAL;
	}

	umt_p = &priv->ports[id];
	if (!umt_p->allocated) {
		dev_err(dev, "umt port not allocated: %u\n", id);
		return -EINVAL;
	}
	port = &umt_p->cfg;

	spin_lock(&priv->lock);
	memset(port, 0, sizeof(*port));
	umt_p->allocated = 0;
	spin_unlock(&priv->lock);

	return 0;
}

/**
 * API to request and allocate UMT port
 * input:
 * @dev: umt device
 * @umt: UMT port info.
 * UMT always dynamic allocate port
 * output:
 * ret: Fail: < 0,  Success: 0
 */
static int umt_port_request(struct device *dev, struct dp_umt_port *port)
{
	struct umt_port *umt_p;
	struct umt_priv *priv;
	struct umt_port_ctl *ctl;
	struct umt_port_res *res;
	int id;

	if (!port || !dev) {
		dev_err(dev, "umt port/dev point is NULL!\n");
		return -EINVAL;
	}
	priv = dev_get_drvdata(dev);

	ctl = &port->ctl;
	res = &port->res;

	id = umt_find_pid(priv, port);
	if (id < 0) {
		dev_err(dev, "Failed to find a suitable port ID\n");
		return -EINVAL;
	}

	umt_p = &priv->ports[id];
	memcpy(&umt_p->cfg, port, sizeof(*port));
	ctl->id = id;

	dp_dma_parse_id(res->dma_id, &umt_p->dma_cid, NULL, &umt_p->dma_chid);
	umt_p->allocated = 1;
	umt_cfg(priv, umt_p);

	return 0;
}

#ifdef CONFIG_DEBUG_FS
static void *umt_port_seq_start(struct seq_file *s, loff_t *pos)
{
	struct umt_priv *priv = s->private;

	if (*pos >= priv->umt_port_num)
		return NULL;

	return &priv->ports[*pos];
}

static void *umt_port_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct umt_priv *priv = s->private;

	if (++(*pos) >= priv->umt_port_num)
		return NULL;

	return &priv->ports[*pos];
}

static void umt_port_seq_stop(struct seq_file *s, void *v)
{
}

static int umt_port_seq_show(struct seq_file *s, void *v)
{
	struct umt_port *umt_p = v;
	struct umt_port_ctl *ctl = &umt_p->cfg.ctl;
	struct umt_port_res *res = &umt_p->cfg.res;

	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "UMT port %d configuration\n", umt_p->id);
	seq_printf(s, "UMT Mode:	%s\n", umt_mode_str[ctl->msg_mode]);
	seq_printf(s, "UMT SW Mode:	%s\n", umt_sw_mode_str[ctl->sw_msg]);
	seq_printf(s, "UMT DEST:	0x%x\n", (u32)ctl->daddr);
	seq_printf(s, "UMT Period:	%u(us)\n", ctl->msg_interval);
	seq_printf(s, "UMT Status:	%s\n", ctl->enable ?
		   "Enable" : "Disable");
	seq_printf(s, "UMT RX from:	%s\n",
		   res->rx_src == UMT_RX_SRC_CQEM ? "CQEM" : "DMA");
	if (res->rx_src == UMT_RX_SRC_CQEM) {
		seq_printf(s, "UMT RX:		CQEM ENQ PID: %u\n",
			   res->cqm_enq_pid);
	} else {
		seq_printf(s, "UMT RX DMA:	DMA%dTXC%d\n",
			   umt_p->dma_cid, umt_p->dma_chid);
		seq_printf(s, "UMT RX DMA NUM:	%d\n", res->dma_ch_num);
	}
	seq_printf(s, "UMT TX:		CQM DEQ PID: %d\n", res->cqm_dq_pid);
	seq_printf(s, "UMT Alloc:	%s\n", umt_p->allocated ? "Yes" : "No");

	return 0;
}

static const struct seq_operations umt_port_seq_ops = {
	.start = umt_port_seq_start,
	.next = umt_port_seq_next,
	.stop = umt_port_seq_stop,
	.show = umt_port_seq_show,
};

static int umt_port_debugfs_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	int ret;

	ret = seq_open(file, &umt_port_seq_ops);
	if (!ret) {
		s = file->private_data;
		s->private = inode->i_private;
	}

	return ret;
}

static const struct file_operations umt_port_fops = {
	.open		= umt_port_debugfs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static int umt_reg_info_show(struct seq_file *s, void *v)
{
	struct umt_priv *priv = s->private;
	int i, idx;

	seq_puts(s, "------UMT Controller Configuration-------\n");
	seq_printf(s, "UMT Port Num:	%u\n", priv->umt_port_num);
	seq_printf(s, "Byte swap:	%u\n", priv->byte_swap);
	seq_printf(s, "Dword swap:	%u\n", priv->dw_swap);
	seq_puts(s, "------UMT REG Configuration--------------\n");
	seq_printf(s, "GCTRL:		0x%08x\n",
		   umt_reg_read(priv, UMT_GCTRL));
	seq_printf(s, "CNT CTRL:	0x%08x\n",
		   umt_reg_read(priv, UMT_CNT_CTRL));

	if (!is_prx300_soc(priv)) {
		seq_printf(s, "UMT MODE:	0x%08x\n",
			   umt_reg_read(priv, UMT_MODE));
		seq_printf(s, "RX0 MUX0:	0x%08x\n",
			   umt_reg_read(priv, UMT_RX0_MUX));
		seq_printf(s, "TRG MUX1:	0x%08x\n",
			   umt_reg_read(priv, UMT_TRG_MUX(4)));
	}
	seq_printf(s, "TRG MUX0:	0x%08x\n",
		   umt_reg_read(priv, UMT_TRG_MUX(0)));

	seq_puts(s, "------UMT Port REG Configuration---------\n");
	for (i = 0; i < priv->umt_port_num; i++) {
		seq_printf(s, "Port idx:	%d\n", i);
		seq_printf(s, "Period:		0x%x\n",
			   umt_reg_read(priv, reg_addr_period(priv, i)));
		seq_printf(s, "Dst addr:	0x%x\n",
			   umt_reg_read(priv, reg_addr_dst(priv, i)));
		idx = umt_reg_read(priv, reg_addr_sw_mode(priv, i));
		seq_printf(s, "SW MSG mode:	%s\n", umt_sw_mode_str[idx]);
		seq_puts(s, "\n");
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(umt_reg_info);

static int umt_debugfs_init(struct umt_priv *priv)
{
	struct dentry *file;

	priv->debugfs = debugfs_create_dir("umt", NULL);
	if (!priv->debugfs)
		return -ENOMEM;

	file = debugfs_create_file("umt_info", 0400, priv->debugfs,
				   priv, &umt_port_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("umt_reg", 0400, priv->debugfs,
				   priv, &umt_reg_info_fops);
	if (!file)
		goto err;

	return 0;

err:
	debugfs_remove_recursive(priv->debugfs);
	priv->debugfs = NULL;
	dev_err(priv->dev, "UMT debugfs create fail!\n");
	return -ENOMEM;
}
#endif

static void umt_port_init(struct umt_priv *priv)
{
	struct umt_port *umt_p;
	int i;

	for (i = 0; i < priv->umt_port_num; i++) {
		umt_p = &priv->ports[i];

		umt_p->id = i;
		umt_p->rx_src_cap = priv->soc->rx_src_cap;
		if (priv->soc->ver == PRX300_SOC)
			umt_p->dma_chn_cap = 1;
		else
			umt_p->dma_chn_cap = i == 0 ? 8 : 1;
		umt_p->feature_cap = priv->soc->feature_cap;
	}
}

static const struct umt_soc_data lgm_umt_data = {
	.ver          = LGM_SOC,
	.umt_port_num = 8,
	.rx_src_cap   = BIT(UMT_RX_SRC_CQEM) | BIT(UMT_RX_SRC_DMA),
	.feature_cap  = UMT_SND_DIS | UMT_CLEAR_CNT
			| UMT_NOT_SND_ZERO_CNT | UMT_SND_RX_CNT_ONLY,
	.enq_base     = 16,
	.dq_base      = 19,
};

static const struct umt_soc_data prx300_umt_data = {
	.ver          = PRX300_SOC,
	.umt_port_num = 3,
	.rx_src_cap   = BIT(UMT_RX_SRC_DMA),
	.feature_cap  = UMT_SND_DIS | UMT_CLEAR_CNT
			| UMT_NOT_SND_ZERO_CNT | UMT_SND_RX_CNT_ONLY,
	.dq_base      = 4,
};

static const struct of_device_id of_umt_drv_match[] = {
	{ .compatible = "intel,lgm-umt", .data = &lgm_umt_data },
	{ .compatible = "intel,prx300-umt", .data = &prx300_umt_data },
	{}
};
MODULE_DEVICE_TABLE(of, of_umt_drv_match);

static int umt_drv_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct of_device_id *match;
	struct umt_priv *priv;
	struct resource *res;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;

	match = of_match_device(of_umt_drv_match, dev);
	if (!match)
		return -ENODEV;
	priv->soc = match->data;

	priv->clk = devm_clk_get(dev, "freq");
	if (IS_ERR(priv->clk)) {
		dev_err(dev, "No UMT clock\n");
		return -EINVAL;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->membase = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->membase)) {
		dev_err(dev, "ioremap mem resource failed!\n");
		return -ENOMEM;
	}

	if (device_property_read_bool(dev, "intel,umt-dw-swap"))
		priv->dw_swap = 1;

	if (device_property_read_bool(dev, "intel,umt-byte-swap"))
		priv->byte_swap = 1;

	priv->umt_port_num = priv->soc->umt_port_num;
	priv->ports =
		devm_kzalloc(dev, sizeof(*priv->ports) * priv->umt_port_num,
			     GFP_KERNEL);
	if (!priv->ports)
		return -ENOMEM;

	spin_lock_init(&priv->lock);
	platform_set_drvdata(pdev, priv);
	priv->ops.umt_dev      = dev;
	priv->ops.umt_request  = umt_port_request;
	priv->ops.umt_enable   = umt_port_enable;
	priv->ops.umt_set_ctrl = umt_port_set_ctrl;
	priv->ops.umt_release  = umt_port_release;

	umt_hw_init(priv);
	umt_port_init(priv);

#ifdef CONFIG_DEBUG_FS
	umt_debugfs_init(priv);
#endif
	dp_register_ops(0, DP_OPS_UMT, (void *)&priv->ops);

	dev_info(dev, "UMT driver init success!\n");

	return 0;
}

static int umt_drv_remove(struct platform_device *pdev)
{
	struct umt_priv *priv;

	priv = platform_get_drvdata(pdev);

#ifdef CONFIG_DEBUG_FS
	debugfs_remove_recursive(priv->debugfs);
#endif
	dev_info(&pdev->dev, "Intel DATAPATH UMT driver remove!\n");
	return 0;
}

static struct platform_driver umt_driver = {
	.probe = umt_drv_probe,
	.remove = umt_drv_remove,
	.driver = {
		.name = "intel,umt",
		.of_match_table = of_match_ptr(of_umt_drv_match),
		.owner = THIS_MODULE,
	},
};

module_platform_driver(umt_driver);

MODULE_LICENSE("GPL v2");
