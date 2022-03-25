/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2015 Zhu YiXin<yixin.zhu@lantiq.com>
 * Copyright (C) 2016 Intel Corporation.
 *
 *  HISTORY
 *  $Date	$Author	$Version               $Comment
 *  01/02/2015	Zhu YiXin 1.0.0	hwmemcpy driver
 *  21/04/2015	Zhu YiXin 1.0.1   Add GCR workaround for UMT(A11 only)
 *  15/07/2015	Zhu YiXin 1.1.0	optimized hwmcpy driver
 */
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/seq_file.h>
#include <linux/dma/lantiq_dmax.h>
#include <linux/irqchip/mips-gic.h>
#include <asm/mipsmtregs.h>
#include <asm/irq.h>
#include <asm/ltq_vmb.h>

#include <lantiq.h>
#include <lantiq_soc.h>

#include <net/datapath_proc_api.h>
#include <linux/ltq_hwmcpy.h>
#include "ltq_hwmcpy_addr.h"
#include "ltq_hwmcpy.h"

#define MCPY_MIN_LEN		64
#define YLD_PIN_ALLOC		1
#define MCPY_DRV_VERSION	"1.1.0"

u32 g_mcpy_dbg = MCPY_ERR | MCPY_INFO;
void __iomem *g_mcpy_addr_base;
static u32 g_mcpy_min_len = MCPY_MIN_LEN;

static struct mcpy_ctrl ltq_mcpy_ctrl;
static const char *g_trunksz[MCPY_TKSZ_MAX]
	= {"512B", "1KB", "2KB", "4KB", "8KB", "16KB", "32KB", "64KB"};
static const char * const hwmcpy_name[MCPY_PORTS_NUM] = {
	"HWMCPY PORT 0",
	"HWMCPY PORT 1",
	"HWMCPY PORT 2",
	"HWMCPY PORT 3",
	"HWMCPY PORT 4",
	"HWMCPY PORT 5",
	"HWMCPY PORT 6",
	"HWMCPY PORT 7",
};

static const struct mcpy_cfg mcpy_def_cfg[MCPY_PORTS_NUM] = {
	/* Prio,  VPE id,  Enable */
	{1,	0,	MCPY_PORT_ENABLED},
	{1,	1,	MCPY_PORT_ENABLED},
	{1,	2,	MCPY_PORT_ENABLED},
	{1,	3,	MCPY_PORT_ENABLED},
	{0,	-1,	MCPY_PORT_ENABLED},
	{0,	-1,	MCPY_PORT_ENABLED},
	{0,	-1,	MCPY_PORT_ENABLED},
	{0,	-1,	MCPY_PORT_ENABLED},
};

#undef MCPY_DBG_DEF
#define MCPY_DBG_DEF(name, value)       #name,
char *g_mcpy_dbg_list[] = {
	MCPY_DBG_LIST
};
#undef MCPY_DBG_DEF

/* #define HWMCPY_PROFILE_CYCLE_CNT */

#ifdef HWMCPY_PROFILE_CYCLE_CNT
extern void *CycleCounter_Create(char *p);
extern void CycleCounter_Start(void *p);
extern void CycleCounter_End(void *p);
static DEFINE_PER_CPU(void *, hwmcpy_cycles);
static DEFINE_PER_CPU(void *, hw_cycles);
static DEFINE_PER_CPU(void *, sw_cycles);

static void mcpy_profile_cycle_init(void);
#endif

#ifdef HWMCPY_PROFILE_CYCLE_CNT
static void mcpy_profile_cycle_init(void)
{
	int i;
	char name[32];

	for_each_online_cpu(i) {
		sprintf(name, "hwmcpy%02d", i);
		per_cpu(hwmcpy_cycles, i) = CycleCounter_Create(name);
		sprintf(name, "hw%02d", i);
		per_cpu(hw_cycles, i) = CycleCounter_Create(name);
		sprintf(name, "sw%02d", i);
		per_cpu(sw_cycles, i) = CycleCounter_Create(name);
	}
}
#else
#define mcpy_profile_cycle_init()
#endif

struct device *mcpy_get_dev(void)
{
	return ltq_mcpy_ctrl.dev;
}
EXPORT_SYMBOL(mcpy_get_dev);

static const char *mcpy_get_name_by_pid(int pid)
{
	return hwmcpy_name[pid];
}

static void mcpy_sw_reset(void)
{
	int timeout = 10000;

	ltq_mcpy_w32_mask(0x10, 0x10, MCPY_GCTRL);
	/* HW auto clean the reset bit */
	while ((ltq_mcpy_r32(MCPY_GCTRL) & 0x10) == 1 && timeout >= 0)
		timeout--;	/*wait reset to finish */

	if (timeout < 0 && (ltq_mcpy_r32(MCPY_GCTRL) & 0x10) == 1) {
		mcpy_dbg(MCPY_ERR, "MCPY soft reset fail!!\n");
		panic("MCPY soft reset fail!\n");
	}
}

static inline void mcpy_set_port_irq_intvl(u32 pid, u32 intvl)
{
	ltq_mcpy_w32(intvl, PORT_TO_CNT(pid));
}

static inline void mcpy_port_irq_enable(u32 pid)
{
	ltq_mcpy_w32_mask(BIT((pid << 1) + 1),
		BIT((pid << 1) + 1), MCPY_INT_EN);
}

static inline void mcpy_port_irq_disable(u32 pid)
{
	ltq_mcpy_w32_mask(BIT((pid << 1) + 1), 0, MCPY_INT_EN);
}

static inline void mcpy_port_yield_enable(u32 pid)
{
	ltq_mcpy_w32_mask(BIT(pid << 1), BIT(pid << 1), MCPY_INT_EN);
}

static inline void mcpy_port_yield_disable(u32 pid)
{
	ltq_mcpy_w32_mask(BIT(pid << 1), 0, MCPY_INT_EN);
}

static inline void mcpy_port_yield_ack(u32 pid)
{
	ltq_mcpy_w32(BIT(pid << 1), MCPY_INT_STAT);
}

static inline void mcpy_port_irq_ack(u32 pid)
{
	ltq_mcpy_w32(0x3 << (pid << 1), MCPY_INT_STAT);
}

static inline void mcpy_irq_ack_mcpy_error(void)
{
	u32 err = ltq_mcpy_r32(MCPY_INT_STAT);

	mcpy_dbg(MCPY_ERR, "MCPY_INT_STAT: 0x%x, CMD Err: %s, Len Err: %s\n",
		err, (err & MCPY_CMD_ERR) ? "Yes" : "No",
		(err & MCPY_LEN_ERR) ? "Yes" : "No");
	ltq_mcpy_w32((MCPY_CMD_ERR | MCPY_LEN_ERR), MCPY_INT_STAT);
}

static inline int mcpy_inquire_irq_status(u32 pid)
{
	return ltq_mcpy_r32(MCPY_INT_STAT) & (BIT(pid * 2) | BIT(pid * 2 + 1));
}

static void mcpy_intr_setup(struct mcpy_port *pport)
{
	if (pport->irq_mode == MCPY_YLD_MODE) { /* Enable Yield */
		mcpy_port_irq_disable(pport->pid);
		mcpy_port_yield_enable(pport->pid);
	} else {
/* PRIO low -> Enable Interrupt, Note: Enable interrupt must Enable Yield */
		mcpy_port_yield_enable(pport->pid);
		mcpy_port_irq_enable(pport->pid);
	}
}

static int mcpy_dma_init(struct mcpy_port *pport)
{
	u32 dma_rxch, dma_txch;

	pport->chan.rch = MCPY_DMA_RX_CID + pport->pid * 2;
	pport->chan.tch = MCPY_DMA_TX_CID + pport->pid * 2;
	pport->chan.rch_dnum = 2;
	pport->chan.tch_dnum = 2;
	pport->chan.rch_dbase =
		pport->ctrl->phybase + MCPY_DBASE
		+ pport->pid * MCPY_DBASE_OFFSET;
	pport->chan.tch_dbase
		= pport->chan.rch_dbase + pport->chan.rch_dnum * DMA_DESC_SIZE;
	pport->chan.onoff     = DMA_CH_ON;
	sprintf(pport->chan.rch_name, "MCPY Port%d RXCH", pport->pid);
	sprintf(pport->chan.tch_name, "MCPY Port%d TXCH", pport->pid);

	dma_rxch = _DMA_C(pport->ctrl->dma_ctrl_id,
			pport->ctrl->dma_port_id,
			pport->chan.rch);
	mcpy_dbg(MCPY_INIT, "dma_ch: 0x%x, rch_name:%s\n",
			dma_rxch, pport->chan.rch_name);
	if (ltq_request_dma(dma_rxch, pport->chan.rch_name) < 0) {
		mcpy_dbg(MCPY_ERR, "request dma chan [0x%x] fail\n", dma_rxch);
		goto __MCPY_PORT_FAIL;
	}
	if (ltq_dma_chan_desc_cfg(dma_rxch,
			pport->chan.rch_dbase, pport->chan.rch_dnum) < 0) {
		mcpy_dbg(MCPY_ERR, "setup dma chan [0x%x] fail\n", dma_rxch);
		goto __MCPY_PORT_FAIL;
	}

	dma_txch = _DMA_C(pport->ctrl->dma_ctrl_id,
			pport->ctrl->dma_port_id,
			pport->chan.tch);
	mcpy_dbg(MCPY_INIT, "dma_ch: 0x%x, rch_name:%s\n",
		    dma_txch, pport->chan.tch_name);
	if (ltq_request_dma(dma_txch, pport->chan.tch_name) < 0) {
		mcpy_dbg(MCPY_ERR, "request dma chan [0x%x] fail\n", dma_txch);
		goto __MCPY_PORT_FAIL;
	}
	if (ltq_dma_chan_desc_cfg(dma_txch,
		    pport->chan.tch_dbase, pport->chan.tch_dnum) < 0) {
		mcpy_dbg(MCPY_ERR, "setup dma chan [0x%x] fail\n", dma_txch);
		goto __MCPY_PORT_FAIL;
	}

	if (pport->chan.onoff == DMA_CH_ON) {
		ltq_dma_chan_on(dma_rxch);
		ltq_dma_chan_on(dma_txch);
	} else {
		ltq_dma_chan_off(dma_rxch);
		ltq_dma_chan_off(dma_txch);
	}

	return 0;

__MCPY_PORT_FAIL: /* Disable Flag in MCPY means malfunction */
	return -ENODEV;
}

static inline int mcpy_done(u32 pid)
{
	u32 resp;

	resp = MCPY_GET_RESPONSE(pid);
	if (unlikely(!(resp & MRES_DONE))) {
		mcpy_dbg(MCPY_ERR,
			"port: %d done bit not set after get interrupt\n",
			pid);
		return -1;
	}

	if (unlikely((resp & MRES_ERROR) != 0)) {
		mcpy_dbg(MCPY_ERR, "MCPY[%d]: MCPY Error, resp: 0x%x!!!\n",
			pid, resp);
		return -1;
	}
	return 0;
}

/* This function should not be called */
static irqreturn_t mcpy_yld_dummy_handler(int irq, void *dev_id)
{
	struct mcpy_port *pport = (struct mcpy_port *)dev_id;

	mcpy_dbg(MCPY_ERR,
		"MCPY PID[%d]: Yield Dummy handler called!!!\n", pport->pid);
	return IRQ_HANDLED;
}

static irqreturn_t mcpy_irq_handler(int irq, void *dev_id)
{
	struct mcpy_port *pport;

	pport = (struct mcpy_port *)dev_id;

	/* mcpy_port_irq_disable(pport->pid); */
	if (unlikely(mcpy_done(pport->pid) < 0))
		mcpy_irq_ack_mcpy_error();

	/*Clear status and Enable the interrupt */
	mcpy_port_irq_ack(pport->pid);

	/* Wake up the waiting process */
	pport->cmd.status = MCPY_CMD_RELEASED;
	barrier();
	wake_up_interruptible(&pport->cmd.cmdq);

	/* mcpy_port_irq_enable(pport->pid); */

	return IRQ_HANDLED;
}

static inline void set_yield_mask(void *mask)
{
	u32 yqmask = read_c0_yqmask();

	yqmask |= (*(u32 *)mask);
	write_c0_yqmask(yqmask);
}

void setup_percpu_yqmask(u32 mask, int cpu)
{
	preempt_disable();
	if (cpu != smp_processor_id())
		smp_call_function_single(cpu,
			(smp_call_func_t)set_yield_mask,
			(void *)&mask, 1);
	else
		set_yield_mask(&mask);
	preempt_enable();
}
EXPORT_SYMBOL(setup_percpu_yqmask);

static void setup_tc_percpu(void *data, int cpu)
{
	preempt_disable();
	if (cpu != smp_processor_id())
		smp_call_function_single(cpu,
			(smp_call_func_t)umt_init,
			(void *)data, 1);
	else
		umt_init(data);
	preempt_enable();
}

static inline void mcpy_yield(u32 yield_pin)
{
	mips_mt_yield(1 << yield_pin);
}

static inline void mcpy_yield_handler(struct mcpy_port *pport)
{
	if (unlikely(mcpy_done(pport->pid) < 0))
		mcpy_irq_ack_mcpy_error();

	mcpy_port_yield_ack(pport->pid);
	/* gic_intr_ack(pport->yield_no); */
}

static void mcpy_intr_init(void)
{
	ltq_mcpy_w32(0x3FFFF, MCPY_INTERNAL_INT_EN);
	ltq_mcpy_w32(0x0,     MCPY_INTERNAL_INT_MASK);
	ltq_mcpy_w32(0x0,     MCPY_INT_EN);
	ltq_mcpy_w32(0x0,     MCPY_INT_MASK);
}

inline void mcpy_eva_cfg(void)
{
	 u32 cfg_reg;

	 cfg_reg = read_c0_segctl0();
	 ltq_mcpy_w32(cfg_reg & 0xFFFF, MCPY_MIPS_CFG(0));
	 ltq_mcpy_w32((cfg_reg >> 16) & 0xFFFF, MCPY_MIPS_CFG(1));

	 cfg_reg = read_c0_segctl1();
	 ltq_mcpy_w32(cfg_reg & 0xFFFF, MCPY_MIPS_CFG(2));
	 ltq_mcpy_w32((cfg_reg >> 16) & 0xFFFF, MCPY_MIPS_CFG(3));

	 cfg_reg = read_c0_segctl2();
	 ltq_mcpy_w32(cfg_reg & 0xFFFF, MCPY_MIPS_CFG(4));
	 ltq_mcpy_w32((cfg_reg >> 16) & 0xFFFF, MCPY_MIPS_CFG(5));
}

static void mcpy_cmd_init(struct mcpy_cmd *pcmd, struct mcpy_port *pport)
{
	pcmd->port = pport;
	mutex_init(&pcmd->mtx_lock);
	init_waitqueue_head(&pcmd->cmdq);
	pcmd->status = MCPY_CMD_RELEASED;
}

#if defined(YLD_PIN_ALLOC) && YLD_PIN_ALLOC
static int mcpy_get_yield_pin(unsigned int vpe_id)
{
	unsigned int yld_pin_bitmap;
	int i;

	yld_pin_bitmap = vmb_yr_get(vpe_id, 1);
	if (!(yld_pin_bitmap & 0xFFFF))
		return -1;
	for (i = 0; i < 16; i++) {
		if (yld_pin_bitmap & (1 << i))
			return i;
	}

	return -1;
}
#else
static int mcpy_get_yield_pin(unsigned int vpe_id)
{
	static int yld_pin = 0xF;

	return yld_pin--;
}

#endif

static int mcpy_port_init(struct mcpy_port *pport, u32 pid)
{
	int yld_pin;

	pport->pid = pid;
	pport->name = mcpy_get_name_by_pid(pid);

	if (pport->status & MCPY_PORT_DISABLED)
		goto mcpy_port_err;

	if (pport->irq_mode == MCPY_YLD_MODE) {
		if (!cpu_online(pport->vpe_id)) {
			mcpy_dbg(MCPY_INIT, "HWMCPY vpe: %d not online!\n",
				pport->vpe_id);
			goto mcpy_port_err;
		}
		yld_pin = mcpy_get_yield_pin(pport->vpe_id);
		if (yld_pin == -1) { /* No Yield resource */
			mcpy_dbg(MCPY_INIT, "HWMCPY Allocate Yield resource Failure !\n");
			goto mcpy_port_err;
		}
		pport->yld_pin = yld_pin;
	}

	if (mcpy_dma_init(pport) < 0) {
		mcpy_dbg(MCPY_ERR, "HWMCPY DMA init failure!\n");
		goto mcpy_port_err;
	}

	mcpy_intr_setup(pport);
	mcpy_set_port_irq_intvl(pid, pport->irq_intvl);
	spin_lock_init(&pport->port_lock);
	mcpy_cmd_init(&pport->cmd, pport);

	if (pport->irq_mode == MCPY_YLD_MODE) {
		mcpy_dbg(MCPY_INIT, "Register Yield: irq:%d\n",
				pport->yld_no);
		if (devm_request_irq(pport->ctrl->dev, pport->yld_no,
			mcpy_yld_dummy_handler,
			0, pport->name, (void *)pport)) {
			mcpy_dbg(MCPY_ERR, "%s: Request yield irq: %d fail\n",
				pport->name, pport->irq_no);
			goto mcpy_port_err;
		}
		setup_percpu_yqmask((1 << pport->yld_pin), pport->vpe_id);
	}
	if (pport->irq_no != 0) {
		mcpy_dbg(MCPY_INIT,
			"Register interrupt: irq:%d\n", pport->irq_no);
		if (devm_request_irq(pport->ctrl->dev, pport->irq_no,
			mcpy_irq_handler,
			0, pport->name, (void *)pport)) {
			mcpy_dbg(MCPY_ERR, "%s: Request irq: %d fail\n",
				pport->name, pport->irq_no);
			goto mcpy_port_err;
		}
	}
	if (pport->irq_mode == MCPY_YLD_MODE) {
		disable_irq(pport->yld_no);
	/*
	 *NOTE: gic_yield_setup API must be called after register the irq
	 *handler as it will clear the percpu mask in gic while request_irq
	 *will set the per cpu mask via set irq affinity function.
	 *For Yield interrupt, it must be cleared so that gic will not handle
	 *Yield interrupt as it will set RMASK in gic
	 */
		if (gic_yield_setup(pport->vpe_id,
			pport->yld_pin, pport->yld_no)) {
			mcpy_dbg(MCPY_INIT, "Yield Setup Fail: vpe:%d, pin:%d, yld_no:%d\n",
				pport->vpe_id, pport->yld_pin, pport->yld_no);
			goto mcpy_port_err;
		}
	}

	return 0;

mcpy_port_err:
	pport->status = MCPY_PORT_DISABLED;
	return -ENODEV;
}

static int mcpy_port_cfg_init(struct platform_device *pdev,
			struct mcpy_port *port, int pid)
{
	struct device_node *np = pdev->dev.of_node;
	char res_name[32];
	u32 cfg_res[5];
	struct mcpy_port_ctrl *port_ctrl = port->pctrl;

	sprintf(res_name, "lantiq,mcpy-ch%d", pid);
	if (of_property_read_u32_array(np, res_name,
			cfg_res, ARRAY_SIZE(cfg_res)) < 0) {
		port->prio = mcpy_def_cfg[pid].prio;
		port->vpe_id = mcpy_def_cfg[pid].vpe_id;
		port->status = mcpy_def_cfg[pid].en;
		port->trunk_size = MCPY_DEF_TRKSZ;
		port->irq_intvl = MCPY_DEF_IRQ_INTVAL;
	} else {
		port->prio = cfg_res[0] == MCPY_PRIO_LOW ?
				MCPY_PRIO_LOW : MCPY_PRIO_HIGH;
		port->vpe_id = cfg_res[1] < num_possible_cpus() ?
				(int)cfg_res[1] : -1;
		port->trunk_size = cfg_res[2] >= MCPY_TKSZ_MAX ?
				MCPY_DEF_TRKSZ : cfg_res[2];
		port->irq_intvl = cfg_res[3] == 0 ?
				MCPY_DEF_IRQ_INTVAL : cfg_res[3];
		port->status = cfg_res[4] == 1 ?
				MCPY_PORT_ENABLED : MCPY_PORT_DISABLED;
	}

	port->pid = pid;
	sprintf(res_name, "yld%d", pid);
	port->yld_no = platform_get_irq_byname(pdev, res_name);
	if (port->yld_no <= 0) {
		mcpy_dbg(MCPY_ERR,
			"Cannot get mcpy ch %d yield irq!!\n", pid);
		return -ENODEV;
	}
	sprintf(res_name, "irq%d", pid);
	port->irq_no = platform_get_irq_byname(pdev, res_name);
	if (port->irq_no <= 0) {
		mcpy_dbg(MCPY_ERR,
			"Cannot get mcpy ch %d irq !!\n", pid);
		return -ENODEV;
	}
	port->irq_mode = port->prio;

	if (port->status & MCPY_PORT_DISABLED)
		return -ENODEV;
	if (port->vpe_id >= 0 && port->vpe_id < num_possible_cpus()) {
		if (port->prio)
			port_ctrl->hi_per_vpe[port->vpe_id] = pid;
		else
			port_ctrl->lo_per_vpe[port->vpe_id] = pid;
		port->status |= MCPY_PORT_RESERVED;
	} else {
		if (port->prio == MCPY_PRIO_LOW)
			port_ctrl->prio_lo_map |= BIT(pid);
	}

	return 0;
}

static void mcpy_init(struct platform_device *pdev, struct mcpy_ctrl *pctrl)
{
	int i;
	struct mcpy_port *pport;
	struct mcpy_port_ctrl *port_ctrl;
	u32 port_map = 0;

	pctrl->dma_ctrl_id = DMA3;
	pctrl->dma_port_id = DMA3_PORT;

	mcpy_sw_reset();
	mcpy_intr_init();

	port_ctrl = &pctrl->port_ctrl;
	port_ctrl->ctrl = pctrl;
	spin_lock_init(&port_ctrl->mcpy_lock);

	port_ctrl->hi_per_vpe = kmalloc(num_possible_cpus() * sizeof(u32),
					GFP_KERNEL);
	port_ctrl->lo_per_vpe = kmalloc(num_possible_cpus() * sizeof(u32),
					GFP_KERNEL);
	if (!port_ctrl->hi_per_vpe || !port_ctrl->lo_per_vpe) {
		mcpy_dbg(MCPY_ERR, "alloc hi/lo array fail!\n");
		return;
	}
	memset(port_ctrl->hi_per_vpe, -1, num_possible_cpus() * sizeof(u32));
	memset(port_ctrl->lo_per_vpe, -1, num_possible_cpus() * sizeof(u32));
	port_ctrl->lo_idx = 0;

	for (i = 0; i < MCPY_PORTS_NUM; i++) {
		pport = &port_ctrl->ports[i];
		pport->ctrl = pctrl;
		pport->pctrl = port_ctrl;
		if (mcpy_port_cfg_init(pdev, pport, i) < 0)
			continue;

		if (mcpy_port_init(pport, i) < 0) {
			port_ctrl->prio_lo_map &= ~(BIT(i));
			continue;
		}
		port_map |= BIT(i);
	}
	ltq_mcpy_w32_mask((0xFF << 8), (port_map << 8 | 1), MCPY_GCTRL);

	/* Copy GCR register to MCPY register */
	mcpy_eva_cfg();
}

static inline struct mcpy_port *mcpy_get_port_by_id(u32 pid)
{
	return &ltq_mcpy_ctrl.port_ctrl.ports[pid];
}

static inline struct mcpy_port *mcpy_get_resv_port(u32 pid, u32 prio)
{
	struct mcpy_port *pport;

	if (unlikely(pid >= MCPY_PORTS_NUM))
		return NULL;

	pport = mcpy_get_port_by_id(pid);
	if (unlikely(pport->prio != prio ||
		!(pport->status & MCPY_PORT_RESERVED)))
		return NULL;

	return pport;
}
/*
 *  Find an available hwmcpy port
 *  1. if Prio is high, then search dedicated port for that VPE.
 *  High prio MCPY channel doesn't support dynamic allocate
 *  due to the Yield feature.
 *  2. if prio is low, then search dedicated port for that VPE, else search
 *     on the prio_lo_map for the free hwmcpy port.
 */
static struct mcpy_port *mcpy_get_port_by_prio(u32 prio, u32 cpuid)
{
	int i;
	struct mcpy_port *pport = NULL;
	struct mcpy_port_ctrl *pctrl;

	pctrl = &ltq_mcpy_ctrl.port_ctrl;
	if (prio == MCPY_PRIO_HIGH) {
		if (likely(pctrl->hi_per_vpe[cpuid] < MCPY_PORTS_NUM))
			return &pctrl->ports[pctrl->hi_per_vpe[cpuid]];
		else
			return NULL;
	} else { /* LOW prio */
		if (pctrl->lo_per_vpe[cpuid] < MCPY_PORTS_NUM)
			pport = &pctrl->ports[pctrl->hi_per_vpe[cpuid]];
		else if (pctrl->prio_lo_map) {
			spin_lock_bh(&pctrl->mcpy_lock);
			for (i = pctrl->lo_idx; i < MCPY_PORTS_NUM; i++) {
				if (pctrl->prio_lo_map & BIT(i)) {
					pport = &pctrl->ports[i];
					pctrl->lo_idx
						= (i + 1) % MCPY_PORTS_NUM;
					break;
				}
			}
			if (!pport) {
				for (i = 0; i < pctrl->lo_idx; i++) {
					if (pctrl->prio_lo_map & BIT(i)) {
						pport = &pctrl->ports[i];
						pctrl->lo_idx = i + 1;
						break;
					}
				}
			}
			spin_unlock_bh(&pctrl->mcpy_lock);
		}
		return pport;
	}

	return NULL;
}

int mcpy_set_cpy_cmd(struct mcpy_cmd *pcmd, u32 dst,
		u32 src, u32 len, enum mcpy_type mode)
{
	u32 dioc, sioc, ie, gather, last, ipc;
	u32 cmd0, cmd1, cmd2, cmd3;
	u32 pid;
	const u32 sphy = 1; /* fixed use physical address */
	const u32 dphy = 1;

	src = __pa(src);
	dst = __pa(dst);
	mcpy_dbg(MCPY_DBG,
		   "dst: 0x%x, src:0x%x, len:%d, mode: %d\n",
		   dst, src, len, mode);

	pid = pcmd->port->pid;
	gather = pcmd->flags & MCPY_CMD_GATHER ? 1 : 0;
	last   = pcmd->flags & MCPY_CMD_LAST   ? 1 : 0;
	ipc    = pcmd->flags & MCPY_CMD_IPC    ? 1 : 0;

	if (gather == 0 || last == 1)
		ie = 1;
	else
		ie = 0;

	dioc = sioc = 0;
	switch (mode) {
	case MCPY_PHY_TO_PHY:
		dioc = sioc = 0;
		break;
	case MCPY_PHY_TO_IOCU:
		dioc = 1;
		sioc = 0;
		break;
	case MCPY_IOCU_TO_PHY:
		sioc = 1;
		dioc = 0;
		break;
	case MCPY_IOCU_TO_IOCU:
		dioc = sioc = 1;
		break;
	default:
		mcpy_dbg(MCPY_ERR, "MCPY type err!!\n");
		return -EINVAL;
	}

	cmd0 = ((sphy & 0x1) << 31) | ((dphy & 0x1) << 30)
		   | ((ie & 0x1) << 29) | ((pid & 0x7) << 26)
		   | ((gather & 0x1) << 25) | ((last & 0x1) << 24)
		   | ((ipc & 0x1) << 23)
		   | ((pcmd->trunk_size & 0x7) << 20) | (len & 0xFFFFF);
	cmd1 = src;
	cmd2 = dst;
	cmd3 = ((dioc & 0x1) << 30) | ((sioc & 0x1) << 29)
		   | (pcmd->context & 0x1FFFFFFF);

	mcpy_dbg(MCPY_DBG,
		   "Pid:%d, CMD0:0x%x, CMD1:0x%x, CMD2: 0x%x, CMD3: 0x%x\n",
		   pid, cmd0, cmd1, cmd2, cmd3);

	/* Enable irq before issue the command */
	if (in_interrupt() && ie)
		enable_irq(pcmd->port->yld_no);

	while (!(ltq_mcpy_r32(MCPY_CMD(3, pid)) & MCPY_OWN_BIT))
		;

	ltq_mcpy_w32(cmd0, MCPY_CMD(0, pid));
	ltq_mcpy_w32(cmd1, MCPY_CMD(1, pid));
	ltq_mcpy_w32(cmd2, MCPY_CMD(2, pid));
	/* memory barrier */
	wmb();

#ifdef HWMCPY_PROFILE_CYCLE_CNT
	CycleCounter_Start(per_cpu(hw_cycles, smp_processor_id()));
#endif

	ltq_mcpy_w32(cmd3, MCPY_CMD(3, pid));

	return 0;
}

static inline void mcpy_reset_cmd(struct mcpy_cmd *pcmd)
{
	pcmd->flags = 0;
	pcmd->status = MCPY_CMD_RELEASED;
}

static inline void mcpy_port_release(struct mcpy_port *pport)
{
	mcpy_reset_cmd(&pport->cmd);
}

/*
 * HW memcpy API: reserve one HW memcopy port
 * Only free Low priority MCPY can be reserved
 * return: pid<0-7>-Success/-1 -Failure
 */
int ltq_mcpy_reserve(void)
{
	struct mcpy_port_ctrl *pctrl;
	int pid = -1;
	int i;

	pctrl = &ltq_mcpy_ctrl.port_ctrl;
	spin_lock_bh(&pctrl->mcpy_lock);

	if (pctrl->prio_lo_map) {
		for (i = 0; i < MCPY_PORTS_NUM; i++) {
			if (pctrl->prio_lo_map & BIT(i)) {
				pid = i;
				pctrl->prio_lo_map &= ~BIT(i);
				pctrl->ports[i].status |= MCPY_PORT_RESERVED;
			}
		}
	}

	spin_unlock_bh(&pctrl->mcpy_lock);

	return pid;
}
EXPORT_SYMBOL(ltq_mcpy_reserve);

/*
 * HW memcpy API: release one HW memcopy port
 * @pid: reserved HW memcopy port id
 */
void ltq_mcpy_release(u32 pid)
{
	struct mcpy_port_ctrl *pctrl;

	if (pid >= MCPY_PORTS_NUM)
		return;

	pctrl = &ltq_mcpy_ctrl.port_ctrl;
	spin_lock_bh(&pctrl->mcpy_lock);
	pctrl->prio_lo_map |= BIT(pid);
	pctrl->ports[pid].status &= ~MCPY_PORT_RESERVED;
	spin_unlock_bh(&pctrl->mcpy_lock);
}
EXPORT_SYMBOL(ltq_mcpy_release);

/*
 * HW memcopy API
 *   @dst:      Destionation address
 *   @src:      Source address
 *   @len:      Number of bytes to be copied
 *   @pid:      HWMCPY Port ID if reserved flag is provided
 *                 otherwise it will be ignored
 *   @mode:   PHY_to_PHY, PHY_to_L2Cache,
 *                 L2Cache_to_PHY, L2Cache_to_L2Cache
 *                 (MCPY_PHY_TO_PHY, MCPY_PHY_TO_IOCU,
 *                 MCPY_IOCU_TO_PHY,MCPY_IOCU_TO_IOCU)
 *   @flags:    IPC/trunksize/port reserved
 *   Note:       Non-Sleep process must use Yield Enabled Port and vice versa.
 */
void *ltq_hwmemcpy(void *dst, const void *src, u32 len,
				u32 pid, enum mcpy_type mode, u32 flags)
{
	struct mcpy_port *pport;
	enum mcpy_prio prio = MCPY_PRIO_HIGH;
	u32 cpuid = smp_processor_id();
	int sleep_en = !in_interrupt();

#ifdef HWMCPY_PROFILE_CYCLE_CNT
	CycleCounter_Start(per_cpu(hwmcpy_cycles, smp_processor_id()));
#endif
	if ((u32)src < (u32)dst && ((u32)src + len) > (u32)dst)
		goto __SW_MEMMOVE_PATH;

	if (len < g_mcpy_min_len)
		goto __SW_MEMCPY_PATH;

	if (unlikely(mode >= MCPY_SW_CPY))
		goto __SW_MEMCPY_PATH;
	/* TODO: Check TLB address */

	if (unlikely(sleep_en))
		prio = MCPY_PRIO_LOW;

	if (unlikely((flags & HWMCPY_F_RESERVED) != 0)) {
		pport = mcpy_get_resv_port(pid, prio);
		if (unlikely(!pport))
			goto __SW_MEMCPY_PATH;
	} else {
		pport = mcpy_get_port_by_prio((u32)prio, cpuid);
		if (!pport) {
			mcpy_dbg(MCPY_ERR,
				"Cannot find a valid port: prio: %d\n",	prio);
			goto __SW_MEMCPY_PATH;
		}
	}

	if (sleep_en)
		mutex_lock(&pport->cmd.mtx_lock);

	/*generate cmd and wait */
	pport->cmd.status = MCPY_CMD_PROCESSING;
	if (flags & HWMCPY_F_IPC)
		pport->cmd.flags |= MCPY_CMD_IPC;

	if (flags & HWMCPY_F_CHKSZ_SET)
		pport->cmd.trunk_size = (flags & 0x1C) >> 2;
	else
		pport->cmd.trunk_size = pport->trunk_size;

	mcpy_set_cpy_cmd(&pport->cmd, (u32)dst, (u32)src, len, mode);

	/*Sleep or Yield */
	if (sleep_en) {
		wait_event_interruptible(pport->cmd.cmdq,
			pport->cmd.status == MCPY_CMD_RELEASED);
		mcpy_dbg(MCPY_DBG, "pid: %d wakeup\n", pport->pid);
	} else {
		do {
			mcpy_dbg(MCPY_DBG, "HWMEMCPY Using Yield\n");
			mcpy_yield(pport->yld_pin);
		} while (!mcpy_inquire_irq_status(pport->pid));
	}

#ifdef HWMCPY_PROFILE_CYCLE_CNT
	CycleCounter_End(per_cpu(hw_cycles, smp_processor_id()));
#endif

	/*Wake up and clean function */
	if (sleep_en) {
		if (unlikely(pport->cmd.status != MCPY_CMD_RELEASED)) {
			mcpy_dbg(MCPY_ERR, "Prio Low: hw memcopy interrupted!!\n");
			/*TODO:  Should recover */
			mcpy_port_release(pport);
			mutex_unlock(&pport->cmd.mtx_lock);
			return NULL;
		}
		mutex_unlock(&pport->cmd.mtx_lock);
	} else { /* Yield Wake up*/
		mcpy_yield_handler(pport);
		/* Disable Yield irq if not in use */
		disable_irq(pport->yld_no);
	}

	pport->mib_bytes += len;
	pport->mib_use_times += 1;
	mcpy_port_release(pport);

#ifdef HWMCPY_PROFILE_CYCLE_CNT
	CycleCounter_End(per_cpu(hwmcpy_cycles, smp_processor_id()));
#endif
	return dst;

__SW_MEMCPY_PATH:

#ifdef HWMCPY_PROFILE_CYCLE_CNT
	CycleCounter_Start(per_cpu(sw_cycles, smp_processor_id()));
#endif
	memcpy(dst, src, len);

#ifdef HWMCPY_PROFILE_CYCLE_CNT
	CycleCounter_End(per_cpu(sw_cycles, smp_processor_id()));
#endif
	return dst;
__SW_MEMMOVE_PATH:
	return memmove(dst, src, len);
}
EXPORT_SYMBOL(ltq_hwmemcpy);

/*
 * HW Memcpy API
 * Copy small pieces scattered memory content into a whole continues memory
 * Note: User only required call this function once to do the gathering.
 *
 *   @dst:            Destionation address
 *   @src:            Source fragment list
 *   @frag_num:    Number of fragments to be copied
 *   @pid:            HWMCPY Port ID if reserved flag is provided
 *                        otherwise it will be ignored
 *   @mode:   PHY_to_PHY, PHY_to_L2Cache,
 *                 L2Cache_to_PHY,  L2Cache_to_L2Cache
 *                 (MCPY_PHY_TO_PHY, MCPY_PHY_TO_IOCU,
 *                 MCPY_IOCU_TO_PHY,MCPY_IOCU_TO_IOCU)
 *   @flags:    IPC/trunksize/port reserved
 *   return:     0 if success
 *                 -1 if failure
 *   @limitation:  This API is not able to handle memory overlap copy.
 *
 */

int ltq_hwmcpy_sg(void *dst, const struct mcpy_frag *src, u32 frag_num,
				u32 pid, enum mcpy_type mode, u32 flags)
{
	struct mcpy_port *pport;
	enum mcpy_prio prio = MCPY_PRIO_HIGH;
	u32 cpuid = smp_processor_id();
	int sleep_en = !in_interrupt();
	int i, total_len = 0;

	/* TODO: Check TLB address */
	if (unlikely(sleep_en))
		prio = MCPY_PRIO_LOW;

	if (flags & HWMCPY_F_RESERVED) {
		pport = mcpy_get_resv_port(pid, prio);
		if (unlikely(!pport))
			return -1;
	} else {
		pport = mcpy_get_port_by_prio(prio, cpuid);
		if (!pport) {
			mcpy_dbg(MCPY_ERR,
				"Cannot find a valid port: prio: %d\n", prio);
			return -1;
		}
	}

	if (sleep_en)
		mutex_lock(&pport->cmd.mtx_lock);

	pport->cmd.status = MCPY_CMD_PROCESSING;
	for (i = 0; i < frag_num; i++) {
		/*generate cmd and wait */
		/* ??? required for every segment or only for last one? */
		if (flags & HWMCPY_F_IPC)
			pport->cmd.flags |= MCPY_CMD_IPC;

		if ((flags & HWMCPY_F_CHKSZ_SET) != 0)
			pport->cmd.trunk_size = (flags & 0x1C) >> 2;
		else
			pport->cmd.trunk_size = pport->trunk_size;

		pport->cmd.flags |= MCPY_CMD_GATHER;

		if (unlikely(i + 1 >= frag_num))
			pport->cmd.flags |= MCPY_CMD_LAST;

		mcpy_set_cpy_cmd(&pport->cmd, (u32)dst,
			(u32)(src[i].ptr + src[i].offset),
			src[i].size, mode);
		total_len += src[i].size;
	}

	/*Sleep or Yield */
	if (sleep_en)
		wait_event_interruptible(pport->cmd.cmdq,
			pport->cmd.status == MCPY_CMD_RELEASED);

	else {
		do {
			mcpy_dbg(MCPY_DBG, "HWMEMCPY Using Yield\n");
			mcpy_yield(pport->yld_pin);
		} while (!mcpy_inquire_irq_status(pport->pid));
	}

	/*Wake up and clean function */
	if (sleep_en) {
		if (pport->cmd.status != MCPY_CMD_RELEASED) {
			mcpy_dbg(MCPY_ERR, "Prio Low: hw memcopy gathering interrupted!!\n");
			mcpy_port_release(pport);
			mutex_unlock(&pport->cmd.mtx_lock);
			return -1;
		}
		mutex_unlock(&pport->cmd.mtx_lock);
	} else { /* Yield Wake up*/
		mcpy_yield_handler(pport);
		disable_irq(pport->yld_no);
	}

	pport->mib_bytes += total_len;
	pport->mib_use_times += 1;
	mcpy_port_release(pport);

	return 0;
}
EXPORT_SYMBOL(ltq_hwmcpy_sg);

/**
 *  Proc Functions
 */
static int mcpy_pctrl_read_proc(struct seq_file *s, void *v)
{
	struct mcpy_ctrl *pctrl = s->private;
	struct mcpy_port_ctrl *port_ctrl = &pctrl->port_ctrl;
	int i;

	seq_puts(s, "\n MCPY Port Control\n");
	seq_puts(s, "-----------------------------------------\n");
	seq_printf(s, "MCPY base addr:0x%x, physical base addr: 0x%x\n",
		(u32)pctrl->membase, pctrl->phybase);
	seq_printf(s, "Low prio free mcpy port bitmap: 0x%x\n",
		port_ctrl->prio_lo_map);
	for (i = 0; i < num_possible_cpus(); i++) {
		seq_printf(s, "VPE id: %d, Reserved: ", i);
		if (port_ctrl->hi_per_vpe[i] <= MCPY_PORTS_NUM) {
			seq_printf(s, "High Prio Port: %d ",
				port_ctrl->hi_per_vpe[i]);
		} else if (port_ctrl->lo_per_vpe[i] <= MCPY_PORTS_NUM) {
			seq_printf(s, "Low Prio Port: %d ",
				port_ctrl->lo_per_vpe[i]);
		} else
			seq_puts(s, "No mcpy ports");

		seq_puts(s, "\n");
	}
	seq_printf(s, "MCPY Min Len threshold: %d\n", g_mcpy_min_len);

	return 0;
}


static int mcpy_pctrl_read_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcpy_pctrl_read_proc, PDE_DATA(inode));
}

static const struct file_operations mcpy_pctrl_proc_fops = {
	.open           = mcpy_pctrl_read_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};


static int mcpy_dbg_read_proc(struct seq_file *s, void *v)
{
	int i;

	seq_puts(s, "MCPY DBG Enable: ");
	for (i = 0; i < DBG_MAX; i++) {
		if (g_mcpy_dbg & BIT(i))
			seq_printf(s, "%s ", g_mcpy_dbg_list[i]);
	}
	seq_puts(s, "\n");

	return 0;
}


static int mcpy_dbg_read_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mcpy_dbg_read_proc, PDE_DATA(inode));
}

static int mcpy_strncmpi(const char *s1, const char *s2, size_t n)
{
	if (!s1 || !s2)
		return 1;
	return strncasecmp(s1, s2, n);
}

static ssize_t mcpy_dbg_write(struct file *file, const char __user *buf,
			size_t count, loff_t *data)
{
	char str[32];
	int len, rlen, i, j;
	int num, enable = 0;
	char *param_list[20];

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	str[rlen] = 0;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if ((mcpy_strncmpi(param_list[0], "enable", strlen("enable")) == 0)
		|| (mcpy_strncmpi(param_list[0], "en", strlen("en")) == 0)) {
		enable = 1;
	} else if (mcpy_strncmpi(param_list[0], "disable", strlen("disable")) == 0
		|| (mcpy_strncmpi(param_list[0], "dis", strlen("dis")) == 0)) {
		enable = 0;
	} else {
		goto proc_dbg_help;
	}
	if (num <= 1) {
		g_mcpy_dbg = 0;
		return count;
	}
	for (i = 1; i < num; i++) {
		for (j = 0; j < ARRAY_SIZE(g_mcpy_dbg_list); j++) {
			if (mcpy_strncmpi(param_list[i],
					g_mcpy_dbg_list[j], strlen(g_mcpy_dbg_list[j])) == 0) {
				if (enable)
					g_mcpy_dbg |= BIT(j);
				else
					g_mcpy_dbg &= ~(BIT(j));
				break;
			}
		}
	}

	return count;

proc_dbg_help:
	mcpy_dbg(MCPY_INFO, "echo <enable/disable> err/event/init/info/dbg > dbg\n");
	return count;
}

static const struct file_operations mcpy_dbg_proc_fops = {
	.open           = mcpy_dbg_read_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.write		= mcpy_dbg_write,
	.release        = single_release,
};

static void *mcpy_port_seq_start(struct seq_file *s, loff_t *pos)
{
	struct mcpy_ctrl *pctrl = s->private;
	struct mcpy_port *pport;

	if (*pos >= MCPY_PORTS_NUM)
		return NULL;

	pport = &pctrl->port_ctrl.ports[*pos];

	return pport;
}

static void *mcpy_port_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct mcpy_ctrl *pctrl = s->private;
	struct mcpy_port *pport;

	if (++*pos >= MCPY_PORTS_NUM)
		return NULL;
	pport = &pctrl->port_ctrl.ports[*pos];
	return pport;
}

static void mcpy_port_seq_stop(struct seq_file *s, void *v)
{
}

static int mcpy_port_seq_show(struct seq_file *s, void *v)
{
	struct mcpy_port *pport = (struct mcpy_port *)v;

	seq_printf(s, "----------HWMCPY Port[%d] info----------\n",
		   pport->pid);
	seq_printf(s, "irq no: %d, yield no: %d, irq interval: %d\n",
		   pport->irq_no, pport->yld_no, pport->irq_intvl);
	seq_printf(s, "priority: %s\n",
		   (pport->prio == MCPY_PRIO_HIGH) ?
		   "High Priority" : "Low Priority");
	if (pport->irq_mode == MCPY_YLD_MODE)
		seq_printf(s, "IRQ mode: Yield, Yield pin:%d, map to CPU:%d\n",
			   pport->yld_pin, pport->vpe_id);
	else
		seq_puts(s, "IRQ mode: Interrupt\n");
	seq_printf(s, "port status: %s %s\n",
		   (pport->status & MCPY_PORT_DISABLED) ? "Disabled" : "Enabled",
		   (pport->status & MCPY_PORT_RESERVED) ? "Reserved" : "");
	seq_printf(s, "dma rxch_id: %d, rch_base: 0x%x, rch_des_num: %d\n",
		   pport->chan.rch, pport->chan.rch_dbase,
		   pport->chan.rch_dnum);
	seq_printf(s, "dma txch_id: %d, tch_base: 0x%x, tch_des_num: %d\n",
		   pport->chan.tch, pport->chan.tch_dbase,
		   pport->chan.tch_dnum);
	seq_printf(s, "dma chan on/off: %s\n",
		   pport->chan.onoff == DMA_CH_ON ? "ON" : "OFF");
	seq_printf(s, "trunk size: %s\n", g_trunksz[pport->trunk_size]);
	seq_printf(s, "mib: use times: %llu, copied bytes: %llu\n",
		   pport->mib_use_times, pport->mib_bytes);

	return 0;
}

static const struct seq_operations mcpy_port_seq_ops = {
	.start = mcpy_port_seq_start,
	.next = mcpy_port_seq_next,
	.stop = mcpy_port_seq_stop,
	.show = mcpy_port_seq_show,
};

static int mcpy_port_seq_open(struct inode *inode, struct file *file)
{
	int ret = seq_open(file, &mcpy_port_seq_ops);

	if (ret == 0) {
		struct seq_file *m = file->private_data;

		m->private = PDE_DATA(inode);
	}
	return ret;
}

static const struct file_operations mcpy_port_proc_fops = {
	.open = mcpy_port_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int mcpy_proc_init(struct mcpy_ctrl *pctrl)
{
	char proc_name[64] = {0};
	struct proc_dir_entry *entry;

	strcpy(proc_name, "driver/hwmcpy");
	pctrl->proc = proc_mkdir(proc_name, NULL);
	if (!pctrl->proc)
		return -ENOMEM;

	entry = proc_create_data("mcpy_ctrl", 0x0, pctrl->proc,
				 &mcpy_pctrl_proc_fops, pctrl);
	if (!entry)
		goto err1;
	entry = proc_create_data("port_info", 0x0, pctrl->proc,
				 &mcpy_port_proc_fops, pctrl);
	if (!entry)
		goto err2;

	entry = proc_create_data("dbg", 0x0, pctrl->proc,
				 &mcpy_dbg_proc_fops, &pctrl);
	if (!entry)
		goto err3;

	return 0;

err3:
	remove_proc_entry("port_info", pctrl->proc);
err2:
	remove_proc_entry("mcpy_ctrl", pctrl->proc);
err1:
	remove_proc_entry(proc_name, NULL);
	return -ENOMEM;
}

struct mcpy_umt *mcpy_get_umt(void)
{
	return &ltq_mcpy_ctrl.umt;
}

static int mcpy_xrx500_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct mcpy_ctrl *pctrl;
	void __iomem *mcpy_addr_base;
	struct device_node *np = pdev->dev.of_node;

	pctrl = &ltq_mcpy_ctrl;
	memset(pctrl, 0, sizeof(ltq_mcpy_ctrl));

	/* load the memory ranges */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		panic("Failed to get HWMCPY resources\n");

	pctrl->phybase = res->start;
	mcpy_addr_base = devm_ioremap_resource(&pdev->dev, res);

	if (IS_ERR(mcpy_addr_base))
		panic("Failed to remap HWMCPY resources\n");

	pctrl->membase = mcpy_addr_base;
	g_mcpy_addr_base = mcpy_addr_base;
	if (of_property_read_u32(np, "lantiq,mcpy-minlen",
				 &g_mcpy_min_len) < 0) {
		g_mcpy_min_len = MCPY_MIN_LEN;
	}

	pctrl->dev = &pdev->dev;

	mcpy_init(pdev, pctrl);
	/* Link platform with driver data for retrieving */
	platform_set_drvdata(pdev, pctrl);
	mcpy_proc_init(pctrl);
	#if 0 // def CONFIG_LTQ_UMT_518_FW_SG
	setup_tc_percpu((void *)pctrl, 0);
	#else
	umt_init(pctrl);
	#endif

	mcpy_dbg(MCPY_INFO, "HW MCPY driver: Version: %s, Init Done!\n",
		 MCPY_DRV_VERSION);

	mcpy_profile_cycle_init();

	return 0;
}

static int  mcpy_xrx500_release(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id mcpy_xrx500_match[] = {
	{ .compatible = "lantiq,mcpy-xrx500" },
	{},
};

static struct platform_driver mcpy_xrx500_driver = {
	.probe = mcpy_xrx500_probe,
	.remove = mcpy_xrx500_release,
	.driver = {
		.name = "mcpy-xrx500",
		.owner = THIS_MODULE,
		.of_match_table = mcpy_xrx500_match,
	},
};

int __init mcpy_xrx500_init(void)
{
	return platform_driver_register(&mcpy_xrx500_driver);
}

device_initcall(mcpy_xrx500_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yixin.Zhu@lantiq.com");
MODULE_DESCRIPTION("LTQ Hardware Memcpy Driver");
MODULE_SUPPORTED_DEVICE("LTQ CPE Devices GRX35X, GRX5XX");
