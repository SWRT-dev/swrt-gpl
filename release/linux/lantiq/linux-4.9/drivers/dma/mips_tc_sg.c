/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2017 Wu ZhiXian<zhixian.wu@intel.com>
 * Copyright (C) 2017 Intel Corporation.
 * MIPS TC FW Scatter-Gathering
 */
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
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
#include <asm/ltq_vmb.h>

#include <linux/dma/lantiq_dmax.h>
#include <lantiq.h>
#include <lantiq_soc.h>

#include <net/datapath_proc_api.h>
#include <linux/ltq_hwmcpy.h>

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/mips_tc_sg.h>

#include "ltq_hwmcpy_addr.h"
#include "ltq_hwmcpy.h"
#include "net/lantiq_cbm_api.h"

#define MIPS_TC_PRIV_DATA_LEN (1024 * 64)
#define MIPS_TC_MAJOR_VER 0
#define MIPS_TC_MID_VER 0
#define MIPS_TC_MINOR_VER 1

static  void mips_tc_memcpy_dispatch(void);
static  void mips_tc_memcpy_pp(void);

//static inline void mips_tc_test_init(void);
static  void mips_tc_test_results(struct mips_tc_tx_descriptor *, struct mips_tc_rx_descriptor *);
static  void mips_tc_conf_init(struct mips_tc_sg_mem *shared_vir_mem);
static  void mips_tc_memcpy_dma_rx(struct mips_tc_tx_des_soc soc_rx_out_src_des, u32 *soc_rx_out_dst_des_ptr, struct mips_tc_rx_descriptor *sg_dma_rx_des_ptr, struct mips_tc_metadata *metadata_ptr);
static  void mips_tc_memcpy_dma_tx(struct mips_tc_tx_des_soc soc_rx_out_src_des, struct mips_tc_tx_descriptor *sg_dma_tx_des_ptr);
static inline void mips_tc_soc_rx_out_src_rel(struct mips_tc_tx_des_soc *soc_rx_out_src_des_ptr);
static inline void mips_tc_dma_tx_rel(void);
static inline void mips_tc_dma_rx_rel(void);
static  void mips_tc_soc_rx_out_dst_enq(struct mips_tc_rx_des_soc *soc_rx_out_dst_des_ptr);
static  void mips_tc_rx_in_umt(void);
unsigned int mips_tc_cal_des_dist(unsigned int idx1, unsigned int idx2, unsigned int des_num);

#define MAX_PKT_PROCESS 50
#define DMA0_DES_NUM_MASK	0x7FF

#define DMA_2ND_READ	0
#define DMA_2ND_WRITE	1

struct mips_tc_q_cfg_ctxt *soc_rx_out_src_ctxt, *soc_rx_out_dst_ctxt, *sg_dma_tx_ctxt, *sg_dma_rx_ctxt;
struct mips_tc_fw_memcpy_ctxt_t *fw_memcpy_ctxt;
struct mips_tc_fw_pp_ctxt_t *fw_pp_ctxt;
struct mips_tc_info *info;
u32 *rxin_hd_acc_addr;
u32 tc_mode_bonding;
u32 loop_cnt;
struct mips_tc_metadata *metadata;
int prnt_flag = 0;
u32 *dma1_tx_data_ptr_base;
//struct mips_tc_metadata pseudo_metadata[2048];

struct mips_tc_sg_mem *shared_working_vir_mem;
struct mips_tc_sg_mem *shared_vir_uncached_mem;
struct mips_tc_sg_mem *shared_vir_cached_mem;
dma_addr_t shared_phy_mem;

int global_cnt = 0;

/*
 * Linux kernel thread to print out message from DSL FW TC
 * */
static int kernel_debug_thread(void *data)
{
	int i = 0;
	struct mips_tc_msg_param *msg;
	struct mips_tc_ctrl_dbg *ctrl;
	ctrl = &shared_working_vir_mem->ctrl;
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop())
			break;
		//printk("%s: idx[%d] debug flags [%d]\n", __func__,
			//	ctrl->msg_cur_idx, ctrl->debug_flags);
		if (ctrl->dbg_flags) {
			/* check for message in shared memory */
			for (i = 0; i < MIPS_TC_DEBUG_MSG_ARRAY_LEN; i++) {
				msg = &ctrl->msgs[(ctrl->msg_cur_idx + i)
						% MIPS_TC_DEBUG_MSG_ARRAY_LEN];
				//printk("%s: msg_cur_idx [%d]%d-%s\n", __func__, ctrl->msg_cur_idx,
					//msg->flags, msg->message);
				if (msg->flags) {
					pr_info("%s", msg->message);
					msg->flags = 0;
					ctrl->msg_cur_idx = (ctrl->msg_cur_idx + i)
						% MIPS_TC_DEBUG_MSG_ARRAY_LEN;
				} else
					break;
			}
		}
		msleep(100);
	}
	return 0;
}

/*
 * Print debug function from TC, not linux kernel
 */
int tc_print(struct mips_tc_sg_mem *shared_vir_mem,
		int level, const char *fmt, ...)
{
	/* check for message in shared memory */
	int i;
	struct mips_tc_msg_param *msg;
	struct mips_tc_ctrl_dbg *ctrl;
	char str[MIPS_TC_DEBUG_MSG_LEN];
	va_list vl;
	va_start(vl, fmt);
	vsnprintf(str, sizeof(str), fmt, vl);
	va_end(vl);
	ctrl = &shared_vir_mem->ctrl;
	*(str + MIPS_TC_DEBUG_MSG_LEN - 1) = 0;
	for (i = 0; i < MIPS_TC_DEBUG_MSG_ARRAY_LEN; i++) {
		msg = &ctrl->msgs[(ctrl->msg_cur_idx + i)
			% MIPS_TC_DEBUG_MSG_ARRAY_LEN];
		if (msg->flags == 0) {
			msg->flags = level;
			memcpy((void *)msg->message, (void *) str, strlen(str));
			break;
		}
	}
	return (MIPS_TC_DEBUG_MSG_ARRAY_LEN - i);
}
int mips_tc_get_shared_mem(struct mips_tc_sg_mem **shared_mem)
{
	if (!shared_working_vir_mem) {
		*shared_mem = NULL;
		return -1;
	}
	*shared_mem = shared_working_vir_mem;
	return 0;
}
EXPORT_SYMBOL(mips_tc_get_shared_mem);
extern struct device *mcpy_get_dev(void);
static struct task_struct *dbg_task;
#ifdef USE_WLAN_SRAM
struct mips_tc_sg_mem g_static_mem;
#define BBCPU_SRAM_ADDR 0xb4000000
#define BBCPU_SRAM_SIZE 0x800000
#define SHARED_MEM_CTXT BBCPU_SRAM_ADDR
#define PRIV_MEM_DATA (BBCPU_SRAM_ADDR + 0x2000)
#endif
void mips_tc_init(u32 *shared_ctxt_mem)
{
	struct device *dev;
	struct mips_tc_ctrl_dbg *ctrl;
	u32 data_mem_vir;
	dma_addr_t data_mem_phy;
	dev = mcpy_get_dev();
	if (!dev) {
		pr_err("%s:%d: HWCPY is Invalid\n", __func__, __LINE__);
		return;
	}
	#ifdef USE_WLAN_SRAM
	shared_vir_uncached_mem = (struct mips_tc_sg_mem *)SHARED_MEM_CTXT;
	shared_vir_cached_mem = (struct mips_tc_sg_mem *)SHARED_MEM_CTXT;
	shared_working_vir_mem = shared_vir_cached_mem;
	memset(shared_working_vir_mem, 0, sizeof(struct mips_tc_sg_mem));
	#else
	shared_vir_uncached_mem = dma_zalloc_coherent(dev,
		sizeof(struct mips_tc_sg_mem),
		&shared_phy_mem,
		GFP_ATOMIC | GFP_DMA
	);
	shared_vir_cached_mem = (struct mips_tc_sg_mem *)
		__va(shared_phy_mem);
	shared_working_vir_mem = shared_vir_cached_mem;
	printk("[%s] dma_zalloc_coherent dev->name=%s sizeof(struct mips_tc_sg_mem)=%d shared_phy_mem=%p\n", __func__, dev->init_name, sizeof(struct mips_tc_sg_mem), shared_phy_mem);
	printk("[%s] dma_zalloc_coherent shared_vir_uncached_mem=%p shared_vir_cached_mem=%p shared_working_vir_mem=%p\n", __func__, shared_vir_uncached_mem, shared_vir_cached_mem, shared_working_vir_mem);
	#endif
	if (!shared_working_vir_mem) {
		pr_err("%s:%d: Invalid shared memory\n", __func__, __LINE__);
		return -1;
	}
#ifdef USE_WLAN_SRAM
	shared_vir_uncached_mem->ctrl.priv_data_vir =
		PRIV_MEM_DATA;
	memset((void *)shared_vir_uncached_mem->ctrl.priv_data_vir,
		0, 2048 * 4);
#else
	shared_vir_uncached_mem->ctrl.priv_data_len = MIPS_TC_PRIV_DATA_LEN;
	shared_vir_uncached_mem->ctrl.priv_data_vir = dma_zalloc_coherent(dev,
		shared_vir_uncached_mem->ctrl.priv_data_len,
		&shared_vir_uncached_mem->ctrl.priv_data_phy,
		GFP_ATOMIC | GFP_DMA
	);
#endif
	printk("=MIPS TC Shared memory: %p %p %p | Data: %08x\n",
			shared_vir_uncached_mem,
			shared_vir_cached_mem,
			shared_working_vir_mem,
			shared_vir_uncached_mem->ctrl.priv_data_vir);
	*shared_ctxt_mem = (u32)shared_working_vir_mem;
	/* firmware information */
	ctrl = &shared_vir_cached_mem->ctrl;
	ctrl->ver_major = MIPS_TC_MAJOR_VER;
	ctrl->ver_mid = MIPS_TC_MID_VER ;
	ctrl->ver_minor = MIPS_TC_MINOR_VER;
	if (IS_ERR(dbg_task)) {
		printk("Unable to start kernel thread.\n");
		dbg_task = NULL;
		return;
	}
	wake_up_process(dbg_task);
}


static  void mips_tc_conf_init(struct mips_tc_sg_mem *shared_vir_mem)
{
	/* Point to cached shared memory */
	soc_rx_out_src_ctxt = (struct mips_tc_q_cfg_ctxt *)(&(shared_vir_mem->conf.soc_rx_out_src_ctxt));
	soc_rx_out_dst_ctxt = (struct mips_tc_q_cfg_ctxt *)(&(shared_vir_mem->conf.soc_rx_out_dst_ctxt));
	sg_dma_tx_ctxt		= (struct mips_tc_q_cfg_ctxt *)(&(shared_vir_mem->conf.soc_sg_dma_tx_ctxt));
	sg_dma_rx_ctxt		= (struct mips_tc_q_cfg_ctxt *)(&(shared_vir_mem->conf.soc_sg_dma_rx_ctxt));
	fw_memcpy_ctxt		= (struct mips_tc_fw_memcpy_ctxt_t *)(&(shared_vir_mem->conf.fw_memcpy_ctxt));
	fw_pp_ctxt			= (struct mips_tc_fw_pp_ctxt_t *)(&(shared_vir_mem->conf.fw_pp_ctxt));
	info				= &shared_vir_mem->mib;
	rxin_hd_acc_addr 	= (u32 *)(&(shared_vir_mem->conf.aca_hw_rxin_hd_acc_addr));
	tc_mode_bonding 	= shared_vir_mem->conf.tc_mode_bonding;
	loop_cnt 	= shared_vir_mem->ctrl.ctrl_max_process_pkt;
	if (loop_cnt == 0) {
		loop_cnt = MAX_PKT_PROCESS;
		shared_vir_mem->ctrl.ctrl_max_process_pkt = loop_cnt;
	}

	//cached address
	metadata = (struct mips_tc_metadata *)__va(shared_vir_mem->ctrl.priv_data_phy);
	dma1_tx_data_ptr_base 	= (u32 *)shared_vir_mem->conf.cache_rxout_ptr;

	//fw_pp_ctxt->dma_tx_ptr = (sg_dma_tx_ctxt->des_base_addr & 0x0FFFFFFF);
	//fw_pp_ctxt->dma_rx_ptr = (sg_dma_rx_ctxt->des_base_addr & 0x0FFFFFFF);

}

unsigned int mips_tc_cal_des_dist(unsigned int idx1, unsigned int idx2, unsigned int des_num)
{
	uint32_t result;

	if (idx1 >= idx2)
		result = (idx1 - idx2);
	else
		result = (des_num + idx1 - idx2);

	return result;
}

static int retries = 10000;
static int index = 0;

int mips_tc_sg(struct mips_tc_sg_mem *shared_vir_mem)
{
#if 0
	retries--;
	if (retries == 0) {
		tc_print(shared_vir_mem, 1, "Hello from DSL %d\n", index++);
		retries = 10000;

	}
#endif
	if (shared_vir_mem->ctrl.mips_tc_control == MIPS_TC_C_STOP) {
		//tc_print(shared_vir_mem, 1, "\n Hello from DSL, mips_tc_c_stop %s Not Run\n", __FUNCTION__);
		shared_vir_mem->mib.state = MIPS_TC_S_IDLE;
		return 0;
	}
	if (shared_vir_mem->mib.state == MIPS_TC_S_IDLE) {
		mips_tc_conf_init(shared_vir_mem);
		shared_vir_mem->mib.state = MIPS_TC_S_RUNNING;
	}

	if (!(sg_dma_tx_ctxt->des_base_addr) || !(sg_dma_rx_ctxt->des_base_addr) || !(soc_rx_out_src_ctxt->des_base_addr) || !(soc_rx_out_dst_ctxt->des_base_addr)) {
		retries--;
		if (retries == 0) {
			tc_print(shared_vir_mem, 1, "Hello from DSL Not Run%d\n", index++);
			tc_print(shared_vir_mem, 1, "\n sg dma base= %x, sg_dma_tx_ctxt = %x\n", sg_dma_tx_ctxt->des_base_addr, sg_dma_tx_ctxt);
			retries = 1000000;
		}

		return 0;
	} else {
		if (!prnt_flag) {
			//mips_tc_test_init(shared_vir_mem);

			tc_print(shared_vir_mem, 1, "\n Hello from DSL %s \n", __FUNCTION__);
			tc_print(shared_vir_mem, 1, "<%s> SG DMA TX Base = %x and SG DMA RX Base = %x\n", __FUNCTION__, sg_dma_tx_ctxt->des_base_addr, sg_dma_rx_ctxt->des_base_addr);
			prnt_flag = 1;
		}
	}

	#if 0
	if (global_cnt % 2 == 0) {
		mips_tc_memcpy_dispatch();
	} else {
		mips_tc_memcpy_pp();
	}

	global_cnt++;
	#endif
	mips_tc_memcpy_dispatch();
	mips_tc_memcpy_pp();

	return 0;
}

//Module Level Debug Code Start
struct mips_tc_q_cfg_ctxt pseudo_soc_rx_out_src_ctxt, pseudo_soc_rx_out_dst_ctxt, pseudo_sg_dma_tx_ctxt, pseudo_sg_dma_rx_ctxt;
struct mips_tc_fw_memcpy_ctxt_t pseudo_fw_memcpy_ctxt;
struct mips_tc_fw_pp_ctxt_t pseudo_fw_pp_ctxt;
struct mips_tc_tx_des_soc pseudo_soc_rx_out_src_des[255];
struct mips_tc_tx_descriptor pseudo_sg_dma_tx_des[255];
struct mips_tc_rx_des_soc pseudo_soc_rx_out_dst_des[255];
struct mips_tc_rx_descriptor pseudo_sg_dma_rx_des[255];

static void mips_tc_test_init(
	struct mips_tc_sg_mem *shared_vir_mem)
{
	//struct mips_tc_tx_descriptor *tx_des_ptr;
	//struct mips_tc_rx_descriptor *rx_des_ptr;

	soc_rx_out_src_ctxt = &pseudo_soc_rx_out_src_ctxt;//0x7a547800;
	soc_rx_out_dst_ctxt = &pseudo_soc_rx_out_dst_ctxt;
	sg_dma_tx_ctxt 		= &pseudo_sg_dma_tx_ctxt;
	sg_dma_rx_ctxt 		= &pseudo_sg_dma_rx_ctxt;

	fw_memcpy_ctxt		= &pseudo_fw_memcpy_ctxt;
	fw_pp_ctxt			= &pseudo_fw_pp_ctxt;

	soc_rx_out_src_ctxt->des_in_own_val = 1;
	//soc_rx_out_src_ctxt->des_num = 255;
	soc_rx_out_src_ctxt->des_num = 3;
	soc_rx_out_src_ctxt->des_base_addr = &pseudo_soc_rx_out_src_des[0];
	soc_rx_out_src_ctxt->rd_pkt_cnt = 0;
	soc_rx_out_src_ctxt->wr_pkt_cnt = 0;
	soc_rx_out_src_ctxt->rd_frag_cnt = 0;
	soc_rx_out_src_ctxt->wr_frag_cnt = 0;
	soc_rx_out_src_ctxt->rd_byte_cnt = 0;
	soc_rx_out_src_ctxt->wr_byte_cnt = 0;

	soc_rx_out_dst_ctxt->des_in_own_val = 1;
	//soc_rx_out_dst_ctxt->des_num = 255;
	soc_rx_out_dst_ctxt->des_num = 3;
	soc_rx_out_dst_ctxt->des_base_addr = &pseudo_soc_rx_out_dst_des[0];
	soc_rx_out_dst_ctxt->rd_pkt_cnt = 0;
	soc_rx_out_dst_ctxt->wr_pkt_cnt = 0;
	soc_rx_out_dst_ctxt->rd_frag_cnt = 0;
	soc_rx_out_dst_ctxt->wr_frag_cnt = 0;
	soc_rx_out_dst_ctxt->rd_byte_cnt = 0;
	soc_rx_out_dst_ctxt->wr_byte_cnt = 0;

	sg_dma_tx_ctxt->des_in_own_val = 1;
	sg_dma_tx_ctxt->des_num = 255;
	//sg_dma_tx_ctxt->des_num= 2;
	sg_dma_tx_ctxt->des_base_addr = &pseudo_sg_dma_tx_des[0];
	sg_dma_tx_ctxt->rd_pkt_cnt = 0;
	sg_dma_tx_ctxt->wr_pkt_cnt = 0;
	sg_dma_tx_ctxt->rd_frag_cnt = 0;
	sg_dma_tx_ctxt->wr_frag_cnt = 0;
	sg_dma_tx_ctxt->rd_byte_cnt = 0;
	sg_dma_tx_ctxt->wr_byte_cnt = 0;

	sg_dma_rx_ctxt->des_in_own_val = 1;
	sg_dma_rx_ctxt->des_num = 255;
	//sg_dma_rx_ctxt->des_num = 2;
	sg_dma_rx_ctxt->des_base_addr = &pseudo_sg_dma_rx_des[0];
	sg_dma_rx_ctxt->rd_pkt_cnt = 0;
	sg_dma_rx_ctxt->wr_pkt_cnt = 0;
	sg_dma_rx_ctxt->rd_frag_cnt = 0;
	sg_dma_rx_ctxt->wr_frag_cnt = 0;
	sg_dma_rx_ctxt->rd_byte_cnt = 0;
	sg_dma_rx_ctxt->wr_byte_cnt = 0;

	pseudo_soc_rx_out_src_des[0].dword2_3.own = 1;
	pseudo_soc_rx_out_src_des[0].dword2_3.sop = 1;
	pseudo_soc_rx_out_src_des[0].dword2_3.eop = 0;
	pseudo_soc_rx_out_src_des[0].dword2_3.data_ptr = 0x80000000;
	pseudo_soc_rx_out_src_des[0].dword2_3.data_len = 12;
	pseudo_soc_rx_out_src_des[0].dword2_3.byte_off = 7;

	pseudo_soc_rx_out_src_des[1].dword2_3.own = 1;
	pseudo_soc_rx_out_src_des[1].dword2_3.sop = 0;
	pseudo_soc_rx_out_src_des[1].dword2_3.eop = 0;
	pseudo_soc_rx_out_src_des[1].dword2_3.data_ptr = 0x81000000;
	pseudo_soc_rx_out_src_des[1].dword2_3.data_len = 12;

	pseudo_soc_rx_out_src_des[2].dword2_3.own = 1;
	pseudo_soc_rx_out_src_des[2].dword2_3.sop = 0;
	pseudo_soc_rx_out_src_des[2].dword2_3.eop = 1;
	pseudo_soc_rx_out_src_des[2].dword2_3.data_ptr = 0x82000000;
	pseudo_soc_rx_out_src_des[2].dword2_3.data_len = 12;

	pseudo_soc_rx_out_dst_des[0].dword2_3.data_ptr = 0x90000000;
	pseudo_soc_rx_out_dst_des[0].dword2_3.own = 0;

	pseudo_soc_rx_out_dst_des[1].dword2_3.data_ptr = 0xa0000000;
	pseudo_soc_rx_out_dst_des[1].dword2_3.own = 0;

	pseudo_soc_rx_out_dst_des[2].dword2_3.data_ptr = 0xb0000000;
	pseudo_soc_rx_out_dst_des[2].dword2_3.own = 0;

	tc_print(shared_vir_mem, 1, "<%s> src_des_base = %x, dst_des_base = %x; sg_tx_des_base = %x; sg_rx_des_base = %x\n", __FUNCTION__, &pseudo_soc_rx_out_src_des[0], &pseudo_soc_rx_out_dst_des[0], &pseudo_sg_dma_tx_des[0], &pseudo_sg_dma_rx_des[0]);
	tc_print(shared_vir_mem, 1, "<%s> src_ctxt = %x, dst_ctxt = %x; sg_tx_ctxt = %x; sg_rx_ctxt = %x; \n", __FUNCTION__, &pseudo_soc_rx_out_src_ctxt, &pseudo_soc_rx_out_dst_ctxt, &pseudo_sg_dma_tx_ctxt, &pseudo_sg_dma_rx_ctxt);
	tc_print(shared_vir_mem, 1, "<%s> memcpy_ctxt = %x, pp_ctxt = %x\n", __FUNCTION__, &pseudo_fw_memcpy_ctxt, &pseudo_fw_pp_ctxt);

}

static void mips_tc_test_results(struct mips_tc_tx_descriptor *sg_dma_tx_des_ptr, struct mips_tc_rx_descriptor *sg_dma_rx_des_ptr)
{

	int *sg_dma_tx_des_dw0, *sg_dma_tx_des_dw1, *sg_dma_rx_des_dw0, *sg_dma_rx_des_dw1 ;
	sg_dma_tx_des_dw0 = (int *)(sg_dma_tx_des_ptr);
	sg_dma_tx_des_dw1 = sg_dma_tx_des_dw0 + 4;

	sg_dma_rx_des_dw0 = (int *)(sg_dma_rx_des_ptr);
	sg_dma_rx_des_dw1 = sg_dma_rx_des_dw0 + 4;

	//pr_info("<%s> TX des dw0 = %x, TX des dw1 = %x; RX des dw0 = %x; RX des dw1 = %x\n", __FUNCTION__,*sg_dma_tx_des_dw0, *sg_dma_tx_des_dw1, *sg_dma_rx_des_dw0, *sg_dma_rx_des_dw1);
	//pr_info("<%s> fw_memcpy_ctxt:sop = %x, eop = %x; byte_off = %x; src_data_ptr = %x, dst_data_ptr = %x\n\n", __FUNCTION__, fw_memcpy_ctxt->sop, fw_memcpy_ctxt->eop, fw_memcpy_ctxt->byte_off, fw_memcpy_ctxt->src_data_ptr, fw_memcpy_ctxt->dst_data_ptr);

}

//Module Level Debug Code End

static  void mips_tc_memcpy_dma_rx(struct mips_tc_tx_des_soc soc_rx_out_src_des, u32 *soc_rx_out_dst_des_ptr, struct mips_tc_rx_descriptor *sg_dma_rx_des_ptr, struct mips_tc_metadata *metadata_ptr)
{
	struct mips_tc_rx_descriptor sg_dma_rx_des;
	u32 tmp_dst_data_ptr;

	fw_pp_ctxt->status = 2;

	//Save sop and eop information to context
	fw_memcpy_ctxt->sop = soc_rx_out_src_des.dword2_3.sop;
	fw_memcpy_ctxt->eop = soc_rx_out_src_des.dword2_3.eop;
	fw_memcpy_ctxt->src_data_ptr = soc_rx_out_src_des.dword2_3.data_ptr;

	sg_dma_rx_des.sop_frag = soc_rx_out_src_des.dword2_3.sop;
	sg_dma_rx_des.eop_frag = soc_rx_out_src_des.dword2_3.eop;

	//Workaround Memcpy Channel do not support metadata and data_len zero issue
	metadata_ptr->sop_frag = soc_rx_out_src_des.dword2_3.sop;
	metadata_ptr->eop_frag = soc_rx_out_src_des.dword2_3.eop;
	metadata_ptr->data_len = soc_rx_out_src_des.dword2_3.data_len;
	metadata_ptr->src_data_ptr = soc_rx_out_src_des.dword2_3.data_ptr;

	sg_dma_rx_des.sop = 1;
	sg_dma_rx_des.eop = 1;

	//Workaround memcpy DMA split issue
	//sg_dma_rx_des.data_len = soc_rx_out_src_des.dword2_3.data_len;
	sg_dma_rx_des.data_len = 0x800;

	if (fw_memcpy_ctxt->sop == 1) {
		//sg_dma_rx_des.data_ptr = soc_rx_out_dst_des.dword2_3.data_ptr;
		sg_dma_rx_des.data_ptr = *soc_rx_out_dst_des_ptr;
		sg_dma_rx_des.byte_off = 0;
	} else {
		tmp_dst_data_ptr = fw_memcpy_ctxt->dst_data_ptr;

		//Adjust data_ptr based on previous fragment data length and byte_off
		if (fw_memcpy_ctxt->byte_off)
			tmp_dst_data_ptr = tmp_dst_data_ptr + fw_memcpy_ctxt->data_len + fw_memcpy_ctxt->byte_off;
		else
			tmp_dst_data_ptr = tmp_dst_data_ptr + fw_memcpy_ctxt->data_len;

		sg_dma_rx_des.data_ptr = (tmp_dst_data_ptr/4) * 4;
		sg_dma_rx_des.byte_off = tmp_dst_data_ptr % 4;

	}

	//Update context for next descriptor process
	fw_memcpy_ctxt->dst_data_ptr = sg_dma_rx_des.data_ptr;
	fw_memcpy_ctxt->byte_off = sg_dma_rx_des.byte_off;
	fw_memcpy_ctxt->data_len = soc_rx_out_src_des.dword2_3.data_len;

	//Save to metadata to reduce cpu load, avoid reading descriptor in post-processing
	metadata_ptr->dst_data_ptr = sg_dma_rx_des.data_ptr;
	metadata_ptr->byte_off = sg_dma_rx_des.byte_off;

	#if DMA_2ND_WRITE
	sg_dma_rx_des.own = !(sg_dma_rx_ctxt->des_in_own_val);

	#else

	sg_dma_rx_des.own = sg_dma_rx_ctxt->des_in_own_val;

	#endif

	//Write to SG_DMA_DES_LIST in DDR
	*sg_dma_rx_des_ptr = sg_dma_rx_des;
	//sg_dma_rx_des_ptr->own = sg_dma_rx_ctxt->des_in_own_val;
	wmb();

	//Increment the counters
	sg_dma_rx_ctxt->wr_frag_cnt++;

	sg_dma_rx_ctxt->wr_byte_cnt += soc_rx_out_src_des.dword2_3.data_len;

	if (soc_rx_out_src_des.dword2_3.eop == 1)
		sg_dma_rx_ctxt->wr_pkt_cnt++;

	soc_rx_out_dst_ctxt->rd_frag_cnt++;

	soc_rx_out_dst_ctxt->rd_byte_cnt += soc_rx_out_src_des.dword2_3.data_len;

	if (soc_rx_out_src_des.dword2_3.eop == 1)
		soc_rx_out_dst_ctxt->rd_pkt_cnt++;

	#if DMA_2ND_WRITE
	sg_dma_rx_des_ptr->own = sg_dma_rx_ctxt->des_in_own_val;
	wmb();

	#endif

}

static  void mips_tc_memcpy_dma_tx(struct mips_tc_tx_des_soc soc_rx_out_src_des, struct mips_tc_tx_descriptor *sg_dma_tx_des_ptr)
{
	struct mips_tc_tx_descriptor sg_dma_tx_des;

	fw_pp_ctxt->status = 3;

	sg_dma_tx_des = soc_rx_out_src_des.dword2_3;

	//Copy the sop and eop to the metadata sop_frag and eop_frag in the descriptor
	sg_dma_tx_des.sop_frag = soc_rx_out_src_des.dword2_3.sop;
	sg_dma_tx_des.eop_frag = soc_rx_out_src_des.dword2_3.eop;

	sg_dma_tx_des.sop = 1;
	sg_dma_tx_des.eop = 1;

	#if DMA_2ND_WRITE

	sg_dma_tx_des.own = !(sg_dma_tx_ctxt->des_in_own_val);

	#else

	sg_dma_tx_des.own = sg_dma_tx_ctxt->des_in_own_val;

	#endif

	//Write to SG_DMA_DES_LIST in DDR
	*sg_dma_tx_des_ptr = sg_dma_tx_des;
	wmb();

	//Increment counters
	sg_dma_tx_ctxt->wr_frag_cnt++;

	sg_dma_tx_ctxt->wr_byte_cnt += soc_rx_out_src_des.dword2_3.data_len;

	if (soc_rx_out_src_des.dword2_3.eop == 1)
		sg_dma_tx_ctxt->wr_pkt_cnt++;

	soc_rx_out_src_ctxt->rd_frag_cnt++;

	soc_rx_out_src_ctxt->rd_byte_cnt += soc_rx_out_src_des.dword2_3.data_len;

	if (soc_rx_out_src_des.dword2_3.eop == 1)
		soc_rx_out_src_ctxt->rd_pkt_cnt++;

	#if DMA_2ND_WRITE
	sg_dma_tx_des_ptr->own = sg_dma_tx_ctxt->des_in_own_val;
	wmb();
		#endif
}

static __inline__ void my_copy_2dw(volatile void *src_addr, void *dst_addr)
{
	asm("lw $8, 0(%0) ;  lw $9, 4(%0) ;   sw $8, 0(%1) ; sw $9, 4(%1)"
	  : /*no output register*/
	  : "r" (src_addr), "r" (dst_addr)
	  : "$8", "$9"
	  );
}

static  void mips_tc_memcpy_dispatch(void)
{
	struct mips_tc_tx_des_soc *soc_rx_out_src_des_ptr;
	//struct mips_tc_rx_des_soc *soc_rx_out_dst_des_ptr;
	u32 *soc_rx_out_dst_des_ptr;
	struct mips_tc_tx_descriptor *sg_dma_tx_des_ptr;
	struct mips_tc_rx_descriptor *sg_dma_rx_des_ptr;
	struct mips_tc_metadata *metadata_ptr;

	struct mips_tc_tx_des_soc soc_rx_out_src_des;
	//struct mips_tc_rx_des_soc soc_rx_out_dst_des;
	u32 proc_frag_cnt = 0;

	u32 soc_rx_out_src_des_base = 0;
	//u32 soc_rx_out_dst_des_base = 0;
	u32 sg_dma_tx_des_base = 0;
	u32 sg_dma_rx_des_base = 0;

	fw_pp_ctxt->status = 1;

	soc_rx_out_src_des_base = soc_rx_out_src_ctxt->des_base_addr;
	//soc_rx_out_dst_des_base = soc_rx_out_dst_ctxt->des_base_addr;
	sg_dma_tx_des_base = sg_dma_tx_ctxt->des_base_addr;
	sg_dma_rx_des_base = sg_dma_rx_ctxt->des_base_addr;

	while (proc_frag_cnt <= loop_cnt) {
		soc_rx_out_src_des_ptr = (struct mips_tc_tx_des_soc *)(soc_rx_out_src_des_base + soc_rx_out_src_ctxt->rd_idx);
		//soc_rx_out_dst_des_ptr = (struct mips_tc_rx_des_soc *)(soc_rx_out_dst_des_base + soc_rx_out_dst_ctxt->rd_idx);
		//soc_rx_out_dst_des_ptr = (u32 *)(soc_rx_out_dst_des_base + soc_rx_out_dst_ctxt->rd_idx);
		soc_rx_out_dst_des_ptr 	= dma1_tx_data_ptr_base + (soc_rx_out_dst_ctxt->rd_idx / 16);
		sg_dma_tx_des_ptr = (struct mips_tc_tx_descriptor *)(sg_dma_tx_des_base + sg_dma_tx_ctxt->wr_idx);
		sg_dma_rx_des_ptr = (struct mips_tc_rx_descriptor *)(sg_dma_rx_des_base + sg_dma_rx_ctxt->wr_idx);
		metadata_ptr = (struct mips_tc_metadata *)(metadata + ((sg_dma_rx_ctxt->wr_idx)/8));
#if 0
		//DMA TX descriptor handling
		//Read SRC descriptor into local variable
		soc_rx_out_src_des = *soc_rx_out_src_des_ptr;
		//soc_rx_out_dst_des = *soc_rx_out_dst_des_ptr;
#else
	    my_copy_2dw((volatile void *)soc_rx_out_src_des_ptr + 8, (void *)(&soc_rx_out_src_des) + 8);
#endif
		if ((soc_rx_out_src_des.dword2_3.own == soc_rx_out_src_ctxt->des_in_own_val) && ((soc_rx_out_src_ctxt->rd_frag_cnt - soc_rx_out_src_ctxt->wr_frag_cnt) < (soc_rx_out_src_ctxt->des_num))) {
			if ((soc_rx_out_dst_ctxt->rd_pkt_cnt - soc_rx_out_dst_ctxt->wr_pkt_cnt) < (soc_rx_out_dst_ctxt->des_num)) {
				//Assume number of descriptors in sg_dma_tx is same with sg_dma_rx
				if ((sg_dma_tx_ctxt->wr_frag_cnt - sg_dma_tx_ctxt->rd_frag_cnt) < ((sg_dma_tx_ctxt->des_num) - 3)) {
					if ((sg_dma_rx_ctxt->wr_frag_cnt - sg_dma_rx_ctxt->rd_frag_cnt) < ((sg_dma_rx_ctxt->des_num) - 3)) {
						//Read one more time, it is possible that own bit is changed before other fields
						//due to 64-bit bus access, read other fields after confirm the own bit

						#if DMA_2ND_READ
						soc_rx_out_src_des = *soc_rx_out_src_des_ptr;
						soc_rx_out_dst_des = *soc_rx_out_dst_des_ptr;

						#endif

						//Need to update DMA RX followed by DMA TX to avoid DMA blocking due to unavailable RX Des
						mips_tc_memcpy_dma_rx(soc_rx_out_src_des, soc_rx_out_dst_des_ptr, sg_dma_rx_des_ptr, metadata_ptr);

						mips_tc_memcpy_dma_tx(soc_rx_out_src_des, sg_dma_tx_des_ptr);

						//Update process fragment count and index
						proc_frag_cnt++;

						soc_rx_out_src_ctxt->rd_idx += rx_out_des_size;
						if ((soc_rx_out_src_ctxt->rd_idx) == (soc_rx_out_src_ctxt->des_num * rx_out_des_size))
							soc_rx_out_src_ctxt->rd_idx = 0;

						if (soc_rx_out_src_des.dword2_3.eop == 1) {
							soc_rx_out_dst_ctxt->rd_idx += rx_out_des_size;

							if ((soc_rx_out_dst_ctxt->rd_idx) == (soc_rx_out_dst_ctxt->des_num * rx_out_des_size))
								soc_rx_out_dst_ctxt->rd_idx = 0;

						}

						sg_dma_tx_ctxt->wr_idx += 8;
						if ((sg_dma_tx_ctxt->wr_idx) == (sg_dma_tx_ctxt->des_num * 8))
							sg_dma_tx_ctxt->wr_idx = 0;

						sg_dma_rx_ctxt->wr_idx += 8;
						if ((sg_dma_rx_ctxt->wr_idx) == (sg_dma_rx_ctxt->des_num * 8))
							sg_dma_rx_ctxt->wr_idx = 0;

						//mips_tc_test_results(sg_dma_tx_des_ptr,sg_dma_rx_des_ptr );

					} else
						break;
				} else
					break;
			} else
				break;
		} else
			break;

	}

	fw_pp_ctxt->status = 4;
}

static inline void mips_tc_soc_rx_out_src_rel(struct mips_tc_tx_des_soc *soc_rx_out_src_des_ptr)
{
	struct mips_tc_tx_des_soc soc_rx_out_src_des;

	fw_pp_ctxt->status = 6;

	soc_rx_out_src_des.dword2_3.own = !(soc_rx_out_src_ctxt->des_in_own_val);

	//Write to DDR
	*soc_rx_out_src_des_ptr = soc_rx_out_src_des;

	wmb();

	soc_rx_out_src_ctxt->wr_frag_cnt++;

	//soc_rx_out_src_ctxt->wr_byte_cnt += sg_dma_rx_des.data_len;
	soc_rx_out_src_ctxt->wr_byte_cnt += fw_pp_ctxt->data_len;

	if (fw_pp_ctxt->eop == 1)
		soc_rx_out_src_ctxt->wr_pkt_cnt++;

}

static  void mips_tc_soc_rx_out_dst_enq(struct mips_tc_rx_des_soc *soc_rx_out_dst_des_ptr)
{
	struct mips_tc_rx_des_soc soc_rx_out_dst_des;
	u32 tmp_pkt_data_len = 0;
	int delay = 0;

	fw_pp_ctxt->status = 7;

	if (fw_pp_ctxt->sop == 1) {
		//tmp_pkt_data_len = sg_dma_rx_des.data_len;
		tmp_pkt_data_len = fw_pp_ctxt->data_len;
	} else {
		//tmp_pkt_data_len = fw_pp_ctxt->pkt_data_len + sg_dma_rx_des.data_len;
		tmp_pkt_data_len = fw_pp_ctxt->pkt_data_len + fw_pp_ctxt->data_len;

	}

	fw_pp_ctxt->pkt_data_len = tmp_pkt_data_len;

	//Enqueue to soc_rx_out_dst descriptor list for DMATX1 (to switch)
	if (fw_pp_ctxt->eop == 1) {
		soc_rx_out_dst_des.dword2_3.sop = 1;
		soc_rx_out_dst_des.dword2_3.eop = 1;
		soc_rx_out_dst_des.dword2_3.data_len = tmp_pkt_data_len;

		#if DMA_2ND_WRITE

		soc_rx_out_dst_des.dword2_3.own = !(soc_rx_out_dst_ctxt->des_in_own_val);

		#else

		soc_rx_out_dst_des.dword2_3.own = soc_rx_out_dst_ctxt->des_in_own_val;

		#endif

		//Write to soc_rx_out_dst descriptor list in DDR
		*soc_rx_out_dst_des_ptr = soc_rx_out_dst_des;
		//soc_rx_out_dst_des_ptr->dword2_3.own = soc_rx_out_dst_ctxt->des_in_own_val;
		wmb();

		for (delay = 0; delay <= 10; delay++) {
			//add delay for writing the own bit
		}

		#if DMA_2ND_WRITE
		soc_rx_out_dst_des_ptr->dword2_3.own = soc_rx_out_dst_ctxt->des_in_own_val;
		wmb();

		#endif
	}

	soc_rx_out_dst_ctxt->wr_frag_cnt++;

	//soc_rx_out_dst_ctxt->wr_byte_cnt += sg_dma_rx_des.data_len;
	soc_rx_out_dst_ctxt->wr_byte_cnt += fw_pp_ctxt->data_len;

	if (fw_pp_ctxt->eop == 1)
		soc_rx_out_dst_ctxt->wr_pkt_cnt++;

}

static inline void mips_tc_dma_tx_rel(void)
{
	fw_pp_ctxt->status = 8;

	sg_dma_tx_ctxt->rd_frag_cnt++;

	//sg_dma_tx_ctxt->rd_byte_cnt += sg_dma_rx_des.data_len;
	sg_dma_tx_ctxt->rd_byte_cnt += fw_pp_ctxt->data_len;

	if (fw_pp_ctxt->eop == 1)
		sg_dma_tx_ctxt->rd_pkt_cnt++;
}

static inline void mips_tc_dma_rx_rel(void)
{
	fw_pp_ctxt->status = 9;

	sg_dma_rx_ctxt->rd_frag_cnt++;

	//sg_dma_rx_ctxt->rd_byte_cnt += sg_dma_rx_des.data_len;
	sg_dma_rx_ctxt->rd_byte_cnt += fw_pp_ctxt->data_len;

	if (fw_pp_ctxt->eop == 1)
		sg_dma_rx_ctxt->rd_pkt_cnt++;
}

static  void mips_tc_rx_in_umt(void)
{
	//need to define is_bonding; which pcie base address
	u32 *rx_in_hd_acc_add_reg_ptr;

	fw_pp_ctxt->status = 10;

	if (tc_mode_bonding == 1) {
		rx_in_hd_acc_add_reg_ptr = *rxin_hd_acc_addr;

		#if 0
		if (fw_pp_ctxt->rx_in_hd_accum > 16) {
			*rx_in_hd_acc_add_reg_ptr = 16;
			*rx_in_hd_acc_add_reg_ptr = (fw_pp_ctxt->rx_in_hd_accum) - 16;
			fw_pp_ctxt->rx_in_hd_accum = 0;
		}

		#endif

		*rx_in_hd_acc_add_reg_ptr = fw_pp_ctxt->rx_in_hd_accum;
		fw_pp_ctxt->rx_in_hd_accum = 0;

	}

}


static  void mips_tc_memcpy_pp(void)
{
	struct mips_tc_tx_des_soc *soc_rx_out_src_des_ptr;
	struct mips_tc_rx_des_soc *soc_rx_out_dst_des_ptr;
	//struct mips_tc_tx_descriptor *sg_dma_tx_des_ptr;
	//struct mips_tc_rx_descriptor *sg_dma_rx_des_ptr;
	struct mips_tc_metadata *metadata_ptr;
	struct mips_tc_metadata local_metadata;

	//struct mips_tc_tx_descriptor sg_dma_tx_des;
	//struct mips_tc_rx_descriptor sg_dma_rx_des;
	u32 soc_rx_out_src_des_base = 0;
	u32 soc_rx_out_dst_des_base = 0;

	u32 rel_frag_cnt = 0;

	u32 *dma_cs_reg_ptr;
	u32 *dma_cdptnrd_reg_ptr;
	u32 dma_rx_curr_ptr;
	u32 dma_rx_dist = 0;

	fw_pp_ctxt->status = 5;

	soc_rx_out_src_des_base = soc_rx_out_src_ctxt->des_base_addr;
	soc_rx_out_dst_des_base = soc_rx_out_dst_ctxt->des_base_addr;

	dma_cs_reg_ptr 		= MIPS_TC_DMA0_CS_REG;
	dma_cdptnrd_reg_ptr = MIPS_TC_DMA_CDPTNRD_REG;

	while (rel_frag_cnt <= loop_cnt) {
		soc_rx_out_src_des_ptr = (struct mips_tc_tx_des_soc *)(soc_rx_out_src_des_base + soc_rx_out_src_ctxt->wr_idx);
		soc_rx_out_dst_des_ptr = (struct mips_tc_rx_des_soc *)(soc_rx_out_dst_des_base + soc_rx_out_dst_ctxt->wr_idx);
		//sg_dma_tx_des_ptr = (struct mips_tc_tx_descriptor *)(sg_dma_tx_ctxt->des_base_addr + sg_dma_tx_ctxt->rd_idx);
		//sg_dma_rx_des_ptr = (struct mips_tc_rx_descriptor *)(sg_dma_rx_ctxt->des_base_addr + sg_dma_rx_ctxt->rd_idx);
		metadata_ptr = (struct mips_tc_metadata *)(metadata + ((sg_dma_rx_ctxt->rd_idx)/8));

		local_metadata = *metadata_ptr;

		if (dma_rx_dist < 8) {
			*dma_cs_reg_ptr = 12;
			dma_rx_curr_ptr = ((*dma_cdptnrd_reg_ptr) << 1) & 0x7FF;
			dma_rx_dist = mips_tc_cal_des_dist(dma_rx_curr_ptr, sg_dma_rx_ctxt->rd_idx, sg_dma_rx_ctxt->des_num * 8);
		}

		if (sg_dma_tx_ctxt->rd_frag_cnt != sg_dma_tx_ctxt->wr_frag_cnt) {
			//if((dma_tx_curr_ptr != fw_pp_ctxt->dma_tx_ptr) && (dma_rx_curr_ptr != fw_pp_ctxt->dma_rx_ptr))
			if ((dma_rx_dist >= 8)) {
				//Read one more time, it is possible that own bit is changed before other fields
				//due to 64-bit bus access

				#if DMA_2ND_READ
				sg_dma_tx_des = *sg_dma_tx_des_ptr;
				sg_dma_rx_des = *sg_dma_rx_des_ptr;

				#endif

				//Update the dma pointers in the fw_pp_ctxt for next use
				fw_pp_ctxt->prev_dma_rx_ptr = dma_rx_curr_ptr;
				dma_rx_dist -= 8;

				fw_pp_ctxt->sop = local_metadata.sop_frag;
				fw_pp_ctxt->eop = local_metadata.eop_frag;

				//Workaround memcpy channel RX data_len zero issue
				//fw_pp_ctxt->data_len = sg_dma_rx_des.data_len;
				fw_pp_ctxt->data_len = local_metadata.data_len;

				//Avoid accessing dma descriptor to reduce cpu load
				fw_pp_ctxt->byte_off = local_metadata.byte_off;
				fw_pp_ctxt->dst_data_ptr = local_metadata.dst_data_ptr;
				fw_pp_ctxt->src_data_ptr = local_metadata.src_data_ptr;

				#if 0
				//Debug code to detect DMA split error
				if (sg_dma_rx_des.sop == 1 && sg_dma_rx_des.eop == 0)
					(fw_pp_ctxt->split_error)++;

				#endif

				mips_tc_soc_rx_out_src_rel(soc_rx_out_src_des_ptr);
				mips_tc_dma_tx_rel();
				mips_tc_dma_rx_rel();
				mips_tc_soc_rx_out_dst_enq(soc_rx_out_dst_des_ptr);

				rel_frag_cnt++;

				//Update pointers

				soc_rx_out_src_ctxt->wr_idx += rx_out_des_size;

				if ((soc_rx_out_src_ctxt->wr_idx) == (soc_rx_out_src_ctxt->des_num * rx_out_des_size))
					soc_rx_out_src_ctxt->wr_idx = 0;

				if (fw_pp_ctxt->eop == 1) {
					soc_rx_out_dst_ctxt->wr_idx += rx_out_des_size;

					if ((soc_rx_out_dst_ctxt->wr_idx) == (soc_rx_out_dst_ctxt->des_num * rx_out_des_size))
						soc_rx_out_dst_ctxt->wr_idx = 0;
				}

				sg_dma_tx_ctxt->rd_idx += 8;

				if ((sg_dma_tx_ctxt->rd_idx) == (sg_dma_tx_ctxt->des_num * 8))
					sg_dma_tx_ctxt->rd_idx = 0;

				sg_dma_rx_ctxt->rd_idx += 8;
				if ((sg_dma_rx_ctxt->rd_idx) == (sg_dma_rx_ctxt->des_num * 8))
					sg_dma_rx_ctxt->rd_idx = 0;

			} else
				break;
		} else
			break;

	}

	if (rel_frag_cnt) {
		fw_pp_ctxt->rx_in_hd_accum += rel_frag_cnt;
		mips_tc_rx_in_umt();
	}

	fw_pp_ctxt->status = 11;

}

int __init tc_sg_init (void)
{

	/* create kernel thread to print debug message */
	dbg_task = kthread_create(kernel_debug_thread, NULL, "debug_task");
}
early_initcall(tc_sg_init);

#ifdef USE_CACHED_MECHANISM
#define CACHED_LINE_SIZE 32
#define CACHED_SIZE 1024
#define mips_tc_round32(x) ((x+31) & ~31)
extern void mips_dcache_flush(u32 size, u32 cache_size, u32 start_addr);
extern void mips_dcache_invalidate(u32 size, u32 cache_size, u32 start_addr);
extern void mips_l2cache_flush(u32 size, u32 cache_size, u32 start_addr);
extern void mips_l2cache_invalidate(u32 size, u32 cache_size, u32 start_addr);
static void align_addr_size(u32 *start_addr, u32 *size)
{
	u32 offset = 0;
	offset = (*start_addr) % CACHED_LINE_SIZE;

	if (offset) {
		*start_addr -= offset;
		*size += offset;
	}

	*size = mips_tc_round32(*size);

	return;
}

int cpu_dcache_flush(u32 start_addr, u32 size)
{
	if (size > CACHED_SIZE)
		return 0;

	mips_dcache_flush(size, CACHED_LINE_SIZE, start_addr);

	return 1;
}

int cpu_l2cache_flush(u32 start_addr, u32 size)
{
	if (size > CACHED_SIZE)
		return 0;

	mips_l2cache_flush(size, CACHED_LINE_SIZE, start_addr);

	return 1;
}

int cpu_dcache_invalidate(u32 start_addr, u32 size)
{
	if (size > CACHED_SIZE)
		return 0;

	mips_dcache_invalidate(size, CACHED_LINE_SIZE, start_addr);

	return 1;
}

int cpu_l2cache_invalidate(u32 start_addr, u32 size)
{
	if (size > CACHED_SIZE)
		return 0;

	mips_l2cache_invalidate(size, CACHED_LINE_SIZE, start_addr);

	return 1;
}


int mips_tc_cache_flush(u32 start_addr, u32 size)
{
	u32 addr = start_addr;

	align_addr_size(&addr, &size);

	cpu_dcache_flush(addr, size);

	cpu_l2cache_flush(addr, size);
	return 0;
}

int mips_tc_cache_invalidate(u32 start_addr, u32 size)
{
	u32 addr = start_addr;

	align_addr_size(&addr, &size);

	cpu_dcache_invalidate(addr, size);

	cpu_l2cache_invalidate(addr, size);
	return 0;
}
#endif /* USE_CACHED_MECHANISM */
