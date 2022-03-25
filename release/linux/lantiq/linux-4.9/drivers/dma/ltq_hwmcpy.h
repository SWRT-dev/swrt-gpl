/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2015 Zhu YiXin<yixin.zhu@lantiq.com>
 * Copyright (C) 2016 Intel Corporation.
 */

#ifndef __HWMCPY_H__
#define __HWMCPY_H__

#define MCPY_PORTS_NUM		8
#define UMT_PORTS_NUM		4
#define MCPY_CMD_DEPTH		16
#define MCPY_OWN_BIT		BIT(31)
#define MIN_UMT_PRD		20

enum mcpy_prio {
	MCPY_PRIO_LOW = 0,	/* Can Sleep */
	MCPY_PRIO_HIGH,		/* Cannot Sleep */
};

enum {
	MCPY_CMD_GATHER = 0x1,
	MCPY_CMD_LAST   = 0x2,
	MCPY_CMD_IPC	= 0x4,
	MCPY_CMD_SRC_FLUSH = 0x8,
	MCPY_CMD_SRC_INV   = 0x10,
	MCPY_CMD_DST_FLUSH = 0x20,
	MCPY_CMD_DST_INV   = 0x40,
};

enum mcpy_trunk_size {
	MCPY_TKSZ_512B	= 0,
	MCPY_TKSZ_1KB,
	MCPY_TKSZ_2KB,
	MCPY_TKSZ_4KB,
	MCPY_TKSZ_8KB,
	MCPY_TKSZ_16KB,
	MCPY_TKSZ_32KB,
	MCPY_TKSZ_64KB,
	MCPY_TKSZ_MAX
};

enum mcpy_irq_md {
	MCPY_IRQ_MODE = 0,
	MCPY_YLD_MODE = 1,
};

enum mcpy_cmd_stat {
	MCPY_CMD_PROCESSING	= 1,
	MCPY_CMD_RELEASED	= 0,
};

enum dma_chan_on_off {
	DMA_CH_OFF = 0,	/*!< DMA channel is OFF */
	DMA_CH_ON = 1,	/*!< DMA channel is ON */
};

#define MCPY_DEF_TRKSZ		MCPY_TKSZ_512B
#define MCPY_DEF_IRQ_INTVAL	50
#define UMT_DEF_DMACID		7

struct mcpy_port;
struct mcpy_ctrl;

struct mcpy_cmd {
	struct mcpy_port *port;   /* back pointer */
	u32 context;
	u32 trunk_size;
	enum mcpy_cmd_stat status;
	enum mcpy_type type;
	u32 flags;
	wait_queue_head_t cmdq;
	/*struct semaphore sem; */
	struct mutex mtx_lock;
	struct semaphore sem_lock;
};

struct dma_ch {
	u32 rch;
	u32 tch;
	u32 rch_dbase;
	u32 tch_dbase;
	u32 rch_dnum;
	u32 tch_dnum;
	enum dma_chan_on_off onoff;
	char rch_name[32];
	char tch_name[32];
};

enum mcpy_port_status {
	MCPY_PORT_DISABLED = 1,
	MCPY_PORT_ENABLED = 2,
	MCPY_PORT_RESERVED = 4,
};

struct umt_port {
	struct mcpy_umt *pctrl;
	u32 umt_pid;
	u32 ep_id;
	enum umt_mode umt_mode;
	enum umt_msg_mode msg_mode;
	enum umt_status status;
	u32 umt_period;
	u32 umt_dst;
	u32 cbm_pid; /* CBM WLAN ID (0 - 3) */
	u32 dma_cid; /* DMA Chan ID */
	enum umt_status suspend;
	spinlock_t umt_port_lock;
#if IS_ENABLED(CONFIG_LTQ_UMT_SW_MODE)
	u32 dq_idx;
	u32 umt_ep_dst;
	u32 umtid_map_cbmid;
	u32 umt_remaining_time;
#endif
};

struct mcpy_umt {
	struct mcpy_ctrl *ctrl;  /* back pointer */
	struct umt_port ports[UMT_PORTS_NUM];
	u32 dma_ctrlid;
	enum umt_status status;
	struct proc_dir_entry *proc;
	spinlock_t umt_lock;
	struct clk *clk;
};

struct mcpy_port {
	struct mcpy_ctrl *ctrl;  /* back pointer */
	struct mcpy_port_ctrl *pctrl; /* back pointer */
	const char *name;
	u32 prio;
	int vpe_id;
	u32 pid;
	int irq_no;
	int yld_no;
	u32 yld_pin;
	struct mcpy_cmd cmd;
	struct dma_ch chan;
	enum mcpy_port_status status;
	spinlock_t port_lock;
	enum mcpy_trunk_size trunk_size;
	enum mcpy_irq_md irq_mode;
	u32 irq_intvl;
	/* atomic_t users; remove for performace inprovement*/
	u64 mib_bytes;
	u64 mib_use_times;
};

struct mcpy_port_ctrl {
	struct mcpy_ctrl *ctrl; /*back pointer */
	u32 prio_lo_map; /* available port bitmap for low priority */
	int lo_idx;
	u32 *hi_per_vpe; /*Dedicated mcpy per VPE */
	u32 *lo_per_vpe; /*Dedicated mcpy per VPE */
	struct mcpy_port ports[MCPY_PORTS_NUM];
	spinlock_t mcpy_lock;
};

struct mcpy_ctrl {
	void __iomem *membase;
	u32 phybase;
	u32 dma_ctrl_id;
	u32 dma_port_id;
	struct mcpy_port_ctrl port_ctrl;
	struct mcpy_umt  umt;
	struct device *dev;
	struct proc_dir_entry *proc;
};

struct mcpy_cfg {
	u32 prio;
	int vpe_id;
	u32 en;
};

/* Debug Level */
#undef MCPY_DBG_DEF
#define MCPY_DBG_DEF(name, value)  MCPY_##name = BIT(value),
#define MCPY_DBG_LIST			\
	MCPY_DBG_DEF(ERR, 0)		\
	MCPY_DBG_DEF(EVENT, 1)		\
	MCPY_DBG_DEF(INIT, 2)		\
	MCPY_DBG_DEF(INFO, 3)		\
	MCPY_DBG_DEF(DBG,  4)		\
	MCPY_DBG_DEF(MAX, 10)		\

enum {
	MCPY_DBG_LIST
};
#undef MCPY_DBG_DEF
#define MCPY_DBG_DEF(name, value) DBG_##name,
enum {
	MCPY_DBG_LIST
};
#undef MCPY_DBG_DEF

extern u32 g_mcpy_dbg;
#define mcpy_dbg(dbg_level, fmt, arg...) \
	do { \
		if (unlikely(g_mcpy_dbg & dbg_level)) { \
			if (dbg_level & MCPY_ERR) { \
				pr_err_ratelimited(fmt, ##arg); \
			} else if ((dbg_level & MCPY_INFO) || \
					(dbg_level & MCPY_INIT)) { \
				pr_info_ratelimited(fmt, ##arg); \
			} else { \
				pr_debug_ratelimited(fmt, ##arg); \
			} \
		} \
	} \
	while (0)

int umt_init(struct mcpy_ctrl *ctrl);
struct mcpy_umt *mcpy_get_umt(void);
void setup_percpu_yqmask(u32 mask, int cpu);

#endif  /* __HWMCPY_H__ */

