/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/
/* =========================================================================
 * This file incorporates work covered by the following copyright and
 * permission notice:
 * The Synopsys DWC ETHER XGMAC Software Driver and documentation (hereinafter
 * "Software") is an unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto.  Permission is hereby granted,
 * free of charge, to any person obtaining a copy of this software annotated
 * with this license and the Software, to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================= */

#ifndef __XGMAC_COMMON_H__
#define __XGMAC_COMMON_H__

//#define CHIPTEST	0
//#define PC_UTILITY 0
//#define KERNEL_MODE 1
//#define CONFIG_SILVER_WORKAROUND 1

#if defined(PC_UTILITY) && PC_UTILITY
#include <stdio.h>
#include <string.h>
#endif

#ifdef __KERNEL__
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
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
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/irq.h>
#include <linux/seq_file.h>
#include <linux/semaphore.h>
#include <linux/netdevice.h>
#include <linux/net_tstamp.h>
#include <linux/clocksource.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/net_tstamp.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/if_ether.h>
#include <linux/spinlock.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/if.h>
#include <linux/export.h>
#include <net/switch_api/gsw_dev.h>
#else
#include <stddef.h>
#include <lantiq_gsw.h>
#include <mac_ops.h>
#include <adap_ops.h>
#include <types.h>
#endif
#include <mac_tx_fifo.h>

#if defined(PC_UTILITY) && PC_UTILITY
extern int pc_uart_dataread(u32 Offset, u32 *value);
extern int pc_uart_datawrite(u32 Offset, u32 value);
extern int pc_uart_dataread_32(u32 Offset, u32 *value);
extern int pc_uart_datawrite_32(u32 Offset, u32 value);

#endif

#define UPTIME 0
#define MAX_RETRY 2000
#define MAC_EN		1
#define MAC_DIS		0

#if defined(PC_UTILITY) && PC_UTILITY
#define NANOSEC_IN_ONESEC 550000
#else
#define NANOSEC_IN_ONESEC 550000
#endif

#define XGMAC_ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define GET_N_BITS(reg, pos, n) \
	(((reg) >> (pos)) & ((0x1 << (n)) - 1))

#define SET_N_BITS(reg, pos, n, val)                            \
	do {                                                    \
		(reg) &= ~(((0x1 << (n)) - 1) << (pos));               \
		(reg) |= (((val) & ((0x1 << (n)) - 1)) << (pos));     \
	} while (0)


#define MAC_GET_VAL(var, reg, field)				\
	GET_N_BITS((var),					\
		   reg##_##field##_POS, 			\
		   reg##_##field##_WIDTH)

#define MAC_SET_VAL(var, reg, field, val)			\
	SET_N_BITS((var),					\
		   reg##_##field##_POS, 			\
		   reg##_##field##_WIDTH, (val))

#define N_EXT_TS 2
#define mac_r32(reg)       readl(reg)
#define mac_w32(val, reg)  writel(val, reg)

#if defined(PC_UTILITY) && PC_UTILITY

/* UART inetrface suppot function */
static inline void pcuart_reg_rd(u32 regaddr, u32 *data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_dataread(regaddr, data);
}

static inline void pcuart_reg_wr(u32 regaddr, u32 data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_datawrite(regaddr, data);
}

/* UART inetrface suppot function */
static inline void pcuart_reg_rd_32(u32 regaddr, u32 *data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_dataread_32(regaddr, data);
}

static inline void pcuart_reg_wr_32(u32 regaddr, u32 data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_datawrite_32(regaddr, data);
}

#endif /* PC_UTILITY */

#if defined(PC_UTILITY) || defined(CHIPTEST)
#ifdef SWITCH_SERVER
extern FILE *g_fp;
FILE *get_fp(void);

#define mac_printf(...)		\
	do { FILE *fp;			\
		fp = (FILE *) get_fp();	\
		fprintf(fp, __VA_ARGS__); \
	} while (0)

#define mac_dbg mac_printf
#else
#define mac_printf printf
#define mac_dbg printf
#endif
#else
#define mac_printf pr_debug
#define mac_dbg pr_debug
#endif


enum {
	PAUSE_AUTO = 0,
	PAUSE_EN,
	PAUSE_RES,
	PAUSE_DIS
};

enum {
	MAC_AUTO_DPLX = 0,
	MAC_FULL_DPLX,
	MAC_RES_DPLX,
	MAC_HALF_DPLX,
};

enum {
	SPEED_10M = 0,
	SPEED_100M,
	SPEED_1G,
	SPEED_10G,
	SPEED_2G5,
	SPEED_5G,
	SPEED_FLEX,
	SPEED_AUTO
};

enum {
	SPEED_LMAC_10M = 0,
	SPEED_LMAC_100M,
	SPEED_LMAC_200M,
	SPEED_LMAC_1G,
	SPEED_XGMAC_10M,
	SPEED_XGMAC_100M,
	SPEED_XGMAC_1G,
	SPEED_XGMII_25G,
	SPEED_XGMAC_5G,
	SPEED_XGMAC_10G,
	SPEED_GMII_25G,
	SPEED_MAC_AUTO,
};

/* In Linux below struct and enum is defined in net_tstamp.h */
#if defined(PC_UTILITY) || defined(CHIPTEST)
struct hwtstamp_config {
	int flags;
	int tx_type;
	int rx_filter;
};

/* possible values for hwtstamp_config->tx_type */
enum hwtstamp_tx_types {
	/* No outgoing packet will need hardware time stamping;
	 * should a packet arrive which asks for it, no hardware
	 * time stamping will be done.
	 */
	HWTSTAMP_TX_OFF,

	/* Enables hardware time stamping for outgoing packets;
	 * the sender of the packet decides which are to be
	 * time stamped by setting %SOF_TIMESTAMPING_TX_SOFTWARE
	 * before sending the packet.
	 */
	HWTSTAMP_TX_ON,

	/* Enables time stamping for outgoing packets just as
	 * HWTSTAMP_TX_ON does, but also enables time stamp insertion
	 * directly into Sync packets. In this case, transmitted Sync
	 * packets will not received a time stamp via the socket error
	 * queue.
	 */
	HWTSTAMP_TX_ONESTEP_SYNC,
};

/* possible values for hwtstamp_config->rx_filter */
enum hwtstamp_rx_filters {
	/* time stamp no incoming packet at all */
	HWTSTAMP_FILTER_NONE = 0,

	/* time stamp any incoming packet */
	HWTSTAMP_FILTER_ALL = 1,

	/* return value: time stamp all packets requested plus some others */
	HWTSTAMP_FILTER_SOME = 2,

	/* PTP v1, UDP, any kind of event packet */
	HWTSTAMP_FILTER_PTP_V1_L4_EVENT = 3,
	/* PTP v1, UDP, Sync packet */
	HWTSTAMP_FILTER_PTP_V1_L4_SYNC = 4,
	/* PTP v1, UDP, Delay_req packet */
	HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ = 5,
	/* PTP v2, UDP, any kind of event packet */
	HWTSTAMP_FILTER_PTP_V2_L4_EVENT = 6,
	/* PTP v2, UDP, Sync packet */
	HWTSTAMP_FILTER_PTP_V2_L4_SYNC = 7,
	/* PTP v2, UDP, Delay_req packet */
	HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ = 8,

	/* 802.AS1, Ethernet, any kind of event packet */
	HWTSTAMP_FILTER_PTP_V2_L2_EVENT = 9,
	/* 802.AS1, Ethernet, Sync packet */
	HWTSTAMP_FILTER_PTP_V2_L2_SYNC = 10,
	/* 802.AS1, Ethernet, Delay_req packet */
	HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ = 11,

	/* PTP v2/802.AS1, any layer, any kind of event packet */
	HWTSTAMP_FILTER_PTP_V2_EVENT = 12,
	/* PTP v2/802.AS1, any layer, Sync packet */
	HWTSTAMP_FILTER_PTP_V2_SYNC = 13,
	/* PTP v2/802.AS1, any layer, Delay_req packet */
	HWTSTAMP_FILTER_PTP_V2_DELAY_REQ = 14,
};

#endif

enum {
	PTP_RX_OVER_IPV4_UDP = 0x1,
	PTP_RX_OVER_IPV6_UDP = 0x2,
	PTP_RX_OVER_ETH      = 0x4,
	PTP_RX_OVER_ANY_LYR  = 0x8,
	PTP_RX_V2			 = 0x10,
	PTP_RX_MSG_TYPE		 = 0x20,
	PTP_RX_EVNT		     = 0x40,
	PTP_RX_SNAP		     = 0x80,
	PTP_RX_MSTR		     = 0x100,
	PTP_RX_EN_ALL		 = 0x400,
};

struct ptp_flags {
	u32 ptp_rx_en;
	u32 ptp_tx_en;
};

struct mac_irq_hdl {
	u32 irq_event;
	void (*cb)(void *);
	void *param;
};

/* Structure to poll for Rx Fifo Error and set adaptation layer */
struct mac_rxfifo_poll {
	struct delayed_work work;
	u64 rxfifo_err_cnt[MAC_LAST];
	int rxfifo_err_threshold;
};

/* do forward declaration of private data structure */
struct mac_prv_data;

struct mac_prv_data {
	struct mac_ops ops;

	/* XGMAC registers for indirect accessing */
	u32 xgmac_ctrl_reg;
	u32 xgmac_data0_reg;
	u32 xgmac_data1_reg;

#ifdef __KERNEL__
	/* Adaption layer private data */
	void __iomem *ss_addr_base;
	/* Lmac addr base */
	void __iomem *lmac_addr_base;
#else
	/* Adaption layer private data */
	u32 ss_addr_base;
	/* Lmac addr base */
	u32 lmac_addr_base;

#endif
	/* Index to point XGMAC 2/3/4 */
	u32 mac_idx;

	/* TX/RX Queues supported by HW */
	u32 tx_q_count;
	u32 rx_q_count;

	/* Tx settings */
	u32 tx_sf_mode;
	u32 tx_threshold;

	/* Rx settings */
	u32 rx_sf_mode;
	u32 rx_threshold;

	/* Flow control settings */
	u32 pause_autoneg;
	u32 tx_pause;
	u32 rx_pause;
	u32 pause_time;
	u32 pause_frm_enable;
	u32 pause_filter;

	u32 mac_en;

	/* MAC Addr */
	u8 mac_addr[6];

	/* Rmon related settings */
	struct xgmac_mmc_stats mmc_stats;
	u32 rmon_reset;

	/* Timestamp support */
	struct hwtstamp_config tstamp_config;
	struct ptp_flags ptp_flgs;
	u32 def_addend;
	u32 tstamp_addend;
	u64 tx_tstamp;
	u32 sec;
	u32 nsec;
	u32 one_nsec_accuracy;
	u32 cic;
	u32 rec_id;
	bool systime_initialized;
	struct mac_fifo_entry ts_fifo[MAX_FIFO_ENTRY];
	bool ext_ref_time;
	s32 ig_corr;
	s32 eg_corr;

#ifdef __KERNEL__
	/* will be pointing to skb which is
	 * queued for transmission and device
	 * will take timesstamp for this skb
	 */
	struct platform_device *pdev;
	struct device *dev;
	struct sk_buff *ptp_tx_skb;
	struct work_struct ptp_tx_work;
	struct ptp_clock_info ptp_clk_info;
	spinlock_t ptp_lock;
	struct ptp_clock *ptp_clock;
	spinlock_t mac_lock;
	spinlock_t rw_lock;
	struct mii_bus *mii;
	struct phy_device *phydev;
	struct tasklet_struct mac_tasklet;
	struct clk *ker_ptp_clk;

	/* Platform Device for Ingress MACsec (EIP160) slice. */
	struct platform_device *ig_pdev;
	/* Platform Device for Egress MACsec (EIP160) slice. */
	struct platform_device *eg_pdev;
#endif
	u32 exts_enabled[N_EXT_TS];
	u32 snaptype;
	u32 tsmstrena;
	u32 tsevntena;

	/* Hardware features of the device */
	struct xgmac_hw_features hw_feat;

	/* MAC rx mode */
	u32 promisc_mode;
	u32 all_mcast_mode;

	u32 rfa;
	u32 rfd;

	u32 tx_mtl_alg;
	u32 rx_mtl_alg;

	u32 mtu;

	u32 rx_checksum_offload;

	/* Reg Read/Write */
	u16 reg_off;
	u32 reg_val;

	/* Phy status */
	u32 phy_speed;
	u32 phymode_speed;
	u32 phy_link;
	const char *phy_mode;

	/* Loopback */
	u32 loopback;

	/* EEE mode */
	u32 eee_enable;
	u32 twt;
	u32 lst;
	u32 lpitxa;

	/* CRC stripping */
	u32 crc_strip;
	u32 crc_strip_type;
	u32 padcrc_strip;

	/* Debug Mode */
	u32 dbg_en;
	u32 dbg_pktie;
	u32 dbg_rst_sel;
	u32 dbg_rst_all;
	u32 dbg_pktstate;
	u32 dbg_data;
	u32 dbg_mode;
	u32 dbg_byteen;

	/* Forward Error paackets */
	u32 fef;
	u32 fup;

	/* interrupts */
	u32 enable_mac_int;
	u32 enable_mtl_int;

	/* IPG */
	u32 ipg;

	/* PMT */
	u32 magic_pkt_en;
	u32 rwk_pkt_en;
	u32 rwk_filter_values[8];
	u32 rwk_filter_length;
	u32 gucast;

	u32 extcfg;

	u32 set_all;
	u32 duplex_mode;

	u32 jd;
	u32 wd;

	u32 linksts;

	u32 fcsgen;

	u32 mdio_cl;
	u32 dev_adr;
	u32 phy_id;
	u32 phy_reg;
	u32 phy_reg_st;
	u32 phy_reg_end;
	u32 phy_data;
	u32 mdio_int;

	u32 phyadr;
	u32 bus_id;

	u32 ptp_clk;

	/* PCH */
	u32 pch_en;
	u32 pch_tx;
	u32 pch_rx;

	/* PFC enable/disable */
	u32 pfc_en;

	/* Mac Cli */
	GSW_MAC_Cli_t *mac_cli;

	/* Maximum number of MAC present */
	u32 max_mac;

	/* MAC IRQ Number */
	u32 irq_num;

	/* haps=1, if board is HAPS, else 0 */
	u32 haps;

	struct lmac_rmon_cnt lrmon_shadow;
	struct xgmac_mmc_stats xrmon_shadow;
	struct mac_rmon rmon_shadow;

	/* Special Tag Operation mode */
	u32 tx_sptag;
	u32 rx_sptag;

	/* MAC IRQ handler */
	struct mac_irq_hdl *irq_hdl;

	/* MAC RxFifo Err Poll */
	struct mac_rxfifo_poll rxfifo_err;
};

extern struct mac_prv_data prv_data[10];

#if defined(PC_UTILITY) || defined(CHIPTEST)
#define container_of(ptr, type, member) ({                      \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

static inline struct mac_prv_data *GET_MAC_PDATA(void *pdev)
{
	struct mac_prv_data *pdata;

	struct mac_ops *mac_op = (struct mac_ops *)pdev;
	pdata = container_of(mac_op, struct mac_prv_data, ops);

	return pdata;
}

#if defined(PC_UTILITY) || defined(CHIPTEST)
struct adap_ops *gsw_get_adap_ops(u32 devid);
struct mac_ops *gsw_get_mac_ops(u32 devid, u32 mac_idx);
u32 gsw_get_mac_subifcnt(u32 devid);
#endif

int xgmac_main(u32 argc, u8 *argv[]);
int gswss_cfg_main(GSW_MAC_Cli_t *);
int lmac_cfg_main(GSW_MAC_Cli_t *);
int lmac_wr_reg(void *pdev, u32 reg_off, u32 reg_val);
int lmac_rd_reg(void *pdev, u32 reg_off);
int xgmac_wr_reg(void *pdev, u32 reg_off, u32 reg_val);
int xgmac_rd_reg(void *pdev, u32 reg_off, u32 *pval);
int gswss_wr_reg(void *pdev, u32 reg_off, u32 reg_val);
int gswss_rd_reg(void *pdev, u32 reg_off);
void mac_init_fn_ptrs(struct mac_ops *mac_op);
int xgmac_init(void *pdev);
int xgmac_exit(void *pdev);
void xgmac_init_pdata(struct mac_prv_data *pdata, int idx);
int xgmac_get_all_hw_settings(void *pdev);
void xgmac_cli_init(void);
int populate_filter_frames(void *pdev);

#endif
