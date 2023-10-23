/******************************************************************************
 *                               Copyright (c) 2012
 *                           Lantiq Deutschland GmbH
 *For licensing information, see the file 'LICENSE' in the root folder of
 *this software module.
******************************************************************************/
#include "drv_tmu_api.h"
#include "net/drv_tmu_ll.h"
#include "drv_tmu_reg.h"
#include <net/lantiq_cbm_api.h>
#include "../cqm/grx500/cbm.h"
#include "../cqm/grx500/reg/cbm_ls.h"
#include "../cqm/grx500/reg/cbm.h"
#include "../cqm/grx500/reg/cbm_dqm.h"
#include "../cqm/grx500/reg/cbm_eqm.h"
#include "../cqm/grx500/reg/cbm_desc64b.h"
#include "../cqm/grx500/reg/fsqm.h"
#include <net/datapath_api.h>
#include <lantiq.h>		/*clk_get_ppe */
#include <net/datapath_proc_api.h>
#include <linux/clk.h>

#define tmu_strcpy  strcpy
#define tmu_strlen  strlen
#define tmu_strspn  strspn
#define tmu_strpbrk strpbrk
#define tmu_strchr  strchr
#define tmu_memset  memset

#define INCLUDE_UNUSED_LL_FUNCTIONS 1
#define MHZ 1000000

extern struct clk *g_cbm_clk;

static int flag_print_disable_q;
static spinlock_t tmu_lock;
/*TMU_DEBUG_MSG; TMU_DEBUG_QUEUE1;TMU_DEBUG_ERR; */
unsigned int tmu_debug_lvl = TMU_DEBUG_ERR;
unsigned int tmu_print_num_en;	/*will be called outside */
unsigned int tmu_max_print_num;	/*will be called outside */
static const char *const pdu_color_type[] = {
	"unassigned", "green", "yellow", "red"
};

static const char *const mod[] = {
	"MOD_0(strict color blind)", "MOD_1(RFC2698 color aware)",
	"MOD_2(RFC 4115 color aware)", "MOD_3(loose color blind)"
};

static const char *const tbu[] = {
	"TBU_0(1 Byte)", "TBU_1(16 Bytes)", "TBU_2(256 Bytes)",
	"TBU_3(1024 Bytes)"
};

/*pre_string used for proc print alignment */
static char proc_pre_align_buf[50];

static unsigned int proc_queue_start_id;
static unsigned int proc_queue_end_id;

static unsigned int proc_tbst_start_id;
static unsigned int proc_tbst_end_id;

static unsigned int proc_sbit_start_id;
static unsigned int proc_sbit_end_id;

static unsigned int proc_sbot_start_id;
static unsigned int proc_sbot_end_id;
static char f_q_mib_title_proc;

#define PROC_EQT_MODE_DEFAULT 0
#define PROC_EQT_MODE_MIB_ONLY 1
#define PROC_EQT_MODE_MIB_CLEAR 2

#ifdef CONFIG_LTQ_TMU_DEVICE_TREE
__iomem void *tmu_membase = (__iomem void *)0;
#endif

int g_queue_dump;
struct tmu_reg *tmu;
/*GPON is 625. But for GRX500, it can be 800/600 /300/150/50 Mhz */
static unsigned int TMU_CORECLOCK;
/*0 not initialized yet. 1 already initialized */
static int tmu_init_stat;

static void tmu_basic_init(void);
static void tmu_egress_queue_table_init(void);
static void tmu_egress_port_table_init(void);
static void tmu_sched_blk_in_table_init(void);
static void tmu_sched_blk_out_table_init(void);
static void tmu_token_bucket_shaper_table_init(void);
static void tmu_packet_pointer_table_init(void);

static uint32_t tmu_tbs_tbu_conversion(uint32_t);
static uint32_t tmu_tbs_srm_conversion(uint32_t);
static uint32_t tmu_tbs_rate_conversion(uint32_t, uint32_t);
static void tmu_reset_queue_threshold(uint32_t qid);
static void tmu_reset_global_threshold(void);
static void tmu_reset_eport_threshold(uint32_t ep);
#ifdef INCLUDE_UNUSED_CHIPTEST_API	/*only used during chiptest. no use now */
static bool tmu_queue_delete(uint32_t index);
static bool tmu_scheduler_delete(uint32_t index);
#endif
static void tmu_recursive_del(uint32_t sbid);
static void tmu_del_all(uint32_t epn);
static void tmu_set_wfq(const uint32_t sbin, const uint16_t weight);
static void tmu_token_bucket_shaper_enable(const uint32_t tbid,
					   const uint32_t sbin,
					   uint8_t enable);
static int tmu_shaper_cfg_set(uint32_t index, int32_t enable, uint32_t mode,
			      uint32_t cir, uint32_t pir, uint32_t cbs,
			      uint32_t pbs);

static int get_max_sb_lvl(void);
static unsigned int get_cbm_clock(void);

static uint16_t tmu_max_px[TMU_WRED_CURVE_NUM][EGRESS_QUEUE_ID_MAX];

#if defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
struct tmu_reg simulate_tmu = { 0 };

uint32_t simu_qemt[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qmtc[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qsmt[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qtht0[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qtht1[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qtht2[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qtht3[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qtht4[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qoct0[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qoct1[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qoct2[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qdct0[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qdct1[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qdct2[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qdct3[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qfmt0[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qfmt1[EGRESS_QUEUE_ID_MAX] = { 0 };
uint32_t simu_qfmt2[EGRESS_QUEUE_ID_MAX] = { 0 };

uint32_t simu_epmt[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_epot0[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_epot1[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_eptt0[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_eptt1[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_epdt0[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_epdt1[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_epdt2[EGRESS_PORT_ID_MAX] = { 0 };
uint32_t simu_epdt3[EGRESS_PORT_ID_MAX] = { 0 };

uint32_t simu_sbitr0[SCHEDULER_BLOCK_INPUT_ID_MAX] = { 0 };
uint32_t simu_sbitr1[SCHEDULER_BLOCK_INPUT_ID_MAX] = { 0 };
uint32_t simu_sbitr2[SCHEDULER_BLOCK_INPUT_ID_MAX] = { 0 };
uint32_t simu_sbitr3[SCHEDULER_BLOCK_INPUT_ID_MAX] = { 0 };

uint32_t simu_sbotr0[SCHEDULER_BLOCK_ID_MAX] = { 0 };
uint32_t simu_sbotr1[SCHEDULER_BLOCK_ID_MAX] = { 0 };

uint32_t simu_tbstr0[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr1[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr2[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr3[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr4[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr5[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr6[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr7[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr8[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr9[TOCKEN_BUCKET_ID] = { 0 };
uint32_t simu_tbstr10[TOCKEN_BUCKET_ID] = { 0 };

uint32_t simu_ppt0[PACKET_POINTER_TABLE_INDEX_MAX] = { 0 };
uint32_t simu_ppt1[PACKET_POINTER_TABLE_INDEX_MAX] = { 0 };
uint32_t simu_ppt2[PACKET_POINTER_TABLE_INDEX_MAX] = { 0 };
uint32_t simu_ppt3[PACKET_POINTER_TABLE_INDEX_MAX] = { 0 };

uint32_t simu_ctrl;

#endif

void tmu_enable(bool act)
{
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32_mask(TMU_CTRL_ACT_EN, act ? TMU_CTRL_ACT_EN : 0, ctrl);
#else
	simu_ctrl = act ? 1 : 0;
#endif
}
EXPORT_SYMBOL(tmu_enable);

bool tmu_is_enabled(void)
{
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	return (tmu_r32(ctrl) & TMU_CTRL_ACT_EN) ? true : false;
#else
	return (simu_ctrl & TMU_CTRL_ACT_EN) ? 1 : 0;
#endif
}
EXPORT_SYMBOL(tmu_is_enabled);

struct default_tmu_queue_settting {
	uint16_t port_num;	/*How many port needs to configure */
	uint16_t base_epn;
	uint16_t base_sbid;
	uint16_t base_qid;
	/*queue number per scheduler. Maximum 8 queue per schedule */
	uint16_t q_num_per_sb;
};

static const struct default_tmu_queue_settting flat_default_setting[] = {
	/*cbm eport_num   base_epn base_sbid   base_qid  q_num_per_sb  */
	{1, 0, 0, 16, 8 /*+42 */ },	/*for egree port 0 */
	/*for egree port 1 ~ 17 with 1 queue per port */
	{17, 1, 1, 66, 1},
	/*for egree port 18     with 1 queue per port */
	{1, 18, 18, 0, 8 /*+8 */ },
	/*for egree port 1~17   with 1 queue per port */
	{1, 19, 19, 83, 8 /*+42 */ },
	/*for egree port 20~22  with 1 queue per port */
	{3, 20, 20, 133, 1},
	/*for egree port 1 ~ 17 with 1 queue per port */
	{1, 23, 23, 136, 8 /*+42 */ }
};

int tmu_ll_init(unsigned long base_addr)
{
#ifdef KEEP_GPON_ALL
	struct tmu_equeue_drop_params thx;
#endif
	int res;

	if (!base_addr)
		tmu = (struct tmu_reg *)TMU_BASE;
	else
		tmu = (struct tmu_reg *)base_addr;

	tmu_init_stat = 0;
	tmu_max_print_num = 100;
	tmu_memset(proc_pre_align_buf, 0, sizeof(proc_pre_align_buf));
	proc_queue_start_id = 0;
	proc_queue_end_id = EGRESS_QUEUE_ID_MAX;
	proc_tbst_start_id = 0;
	proc_tbst_end_id = TOCKEN_BUCKET_ID;
	proc_sbit_start_id = 0;
	proc_sbit_end_id = SCHEDULER_BLOCK_INPUT_ID_MAX;
	proc_sbot_start_id = 0;
	proc_sbot_end_id = SCHEDULER_BLOCK_ID_MAX;
	/*GPON is 625. But for GRX500, it can be 800/600 /300/150/50 Mhz */
	TMU_CORECLOCK = get_cbm_clock();
	/* printk("TMU with Register Base=%p\n", TMU_BASE); */
	TMU_PRINT("Initializing TMU with Base Address=%p: ...\n",
		  (void *)TMU_BASE);

	if (tmu_init_stat) {
		TMU_PRINT("\nTmu is already]\n");
		TMU_PRINT("\nTMU TEST: %d\n", tmu_init_stat);
		TMU_PRINT("TMU Initialization Done\n");
		return -1;
	}

	if (tmu_spin_lock_init(&tmu_lock, "tmu") != 0) {
		TMU_PRINT("\nTMU init: -> can't init tmu spinlock\n");
		res = -1;
		return res;
	}

	res = tmu_ll_stack_init();
#ifdef CONFIG_LTQ_TMU_CHIPTEST
	/*tmu_ll2_init(); */
#endif
	tmu_memset(tmu_max_px, 0, sizeof(tmu_max_px));
	tmu_basic_init();
	tmu_egress_queue_table_init();
	tmu_egress_port_table_init();
	tmu_sched_blk_in_table_init();
	tmu_sched_blk_out_table_init();
	tmu_token_bucket_shaper_table_init();
	tmu_packet_pointer_table_init();
#ifdef KEEP_GPON_ALL		/*---------SGH: I think no need for URX500*/
	thx.math0 = 8;		/* 4096 byte queue length */
	thx.math1 = 8;		/* 4096 byte queue length */
	thx.mith0 = 8;		/* 4096 byte queue length */
	thx.mith1 = 8;		/* 4096 byte queue length */
	thx.qtth0 = 8;		/* 4096 byte queue length */
	thx.qtth1 = 8;		/* 4096 byte queue length */
	/* 2048 byte guaranteed reservation, one maximum size OMCI message */
	thx.qrth = 4;
	/*tmu_equeue_drop_params_set(? ? ?, &thx); */
#endif
	tmu_init_stat = 1;
	tmu_enable(true);
	TMU_PRINT("TMU Initialization Done\n");
	return res;
}
EXPORT_SYMBOL(tmu_ll_init);

int tmu_ll_reset(unsigned long base_addr)
{
#ifdef KEEP_GPON_ALL
	struct tmu_equeue_drop_params thx;
#endif
	int res = 0;

	if (!base_addr)
		tmu = (struct tmu_reg *)TMU_BASE;
	else
		tmu = (struct tmu_reg *)base_addr;

	tmu_init_stat = 0;
	TMU_PRINT("Reset TMU with Base Address=%p: ...\n", (void *)TMU_BASE);
	tmu_enable(false);
	/*tmu_memset(tmu_max_px, 0, sizeof(tmu_max_px)); */
	/*tmu_basic_init(); */
	tmu_egress_queue_table_init();
	tmu_egress_port_table_init();
	tmu_sched_blk_in_table_init();
	tmu_sched_blk_out_table_init();
	tmu_token_bucket_shaper_table_init();
	/*tmu_packet_pointer_table_init(); */
	TMU_PRINT("TMU Initialization Done\n");
	tmu_init_stat = 1;
	return res;
}

int tmu_stat_get(void)
{
	return tmu_init_stat;
}

int tmu_ll_exit(void)
{
	int res = 0;

	if (!tmu_init_stat)
		return 0;

	tmu_enable(false);

	if (tmu_spin_lock_delete(&tmu_lock) != 0) {
		TMU_DEBUG(TMU_DEBUG_ERR, "can't delete tmu spinlock");
		res = -1;
	}

	res = tmu_ll_stack_exit();
	tmu_init_stat = 0;
	return res;
}
EXPORT_SYMBOL(tmu_ll_exit);

static void tmu_basic_init(void)
{
	uint32_t i;
	/*
	   - state machine frozen
	   - state machine de-activated
	 */
	tmu_enable(false);
	/*
	   FPL settings are covered in PacketPointerTableInit as they they
	   belong functionally to PPT issues:
	   FPL.TFPP
	   FPL.HFPP
	 */
	tmu_w32(PACKET_POINTER_TABLE_INDEX_MAX, fpcr);
	/* free pointer threshold: 0 = all pointers can be used */
	tmu_w32(0, fpthr);
	/* elapsed time since last token accumulation */
	tmu_w32(0, timer);
	/* random number */
	tmu_w32(1, lfsr);
	/* WRED crawler period (1024 clocks): 0 = disabled */
#ifndef TMU_SOC_REAL_BOARD
	/*sgh: as per inputs from gpon team franz(on 21 may, 2014) cpr set to 1 */
	tmu_w32(0x1, cpr);
#else
	tmu_w32(0x10, cpr);
#endif
	/* current value WRED crawler counter, last queue id served */
	tmu_w32(0, csr);
	/* global fill level (segments) */
	tmu_w32(0, goccr);
	/* all IRN irq acknowledged */
	tmu_w32(0, irncr);
	/* all IRN irq not set */
	tmu_w32(0, irnicr);
	/* all IRN irq disabled */
	tmu_w32(0, irnen);

	for (i = 0; i < 4; i++) {
		/* global occupancy threshold n (color n discard), set to total
		   SSB == 18432 segments in GPON.
		   sgh add: But in GRX500, it is 9K(Normal) + 1K (Jubmo).
		 */
		/*Global Occupancy Threshold Register for different color */
		tmu_w32(TMU_GOTH_DEFAULT, gothr[i]);
		tmu_w32(0, gpdcr[i]);	/*Global PDU Discard Counter Register */
		/*Low Power Idle Configuration for LAN port
		   is no use for GRX500
		 */
		tmu_w32(0, lpic[i]);
		/*Low Power Idle Timer Status is no use for GRX500 */
		tmu_w32(0, lpit[i]);
	}

	/* queue fill status for queues 0..31, 0: all queues not filled */
	/* ... */
	/* queue fill status for queues 224..255, 0: all queues not filled */
	/*Queue Fill Status Register,  one bit one queue  */
	for (i = 0; i < 8; i++)
		tmu_w32(0, qfill[i]);

	/* egress port fill status for queues  0..31, 0: all queues not
	   filled
	 */
	/* ... */
	/* egress port fill status for queues 64..71, 0: all queues not
	   filled
	 */
	/*Egress Port Filled Status Register, one bit one egress port */
	for (i = 0; i < 3; i++)
		tmu_w32(0, epfr[i]);

	/*Token Bucket ID Capture Register:TBID captu
	   ed in case of SRC0N or SRC1N interrupts
	 */
	tmu_w32(0, tbidcr);
	/* QID 254 (OMCI upstream) only */
	/*Queue Occupancy Table Register.
	   SGH: Don't understand the settting ????
	 */
	tmu_w32_mask(TMU_QOCT0_QRTH_MASK, 0x100, qoct0);
	/* enable pipeline in CTRL register */
	tmu_relog_sequential(true);
	/*Disable Token bucket Accumulation crawler i
	   ctrl register. It is for debug purpose
	 */
	tmu_token_accumulation_disable(false);
	/*Set Maximum TBID that can be used for shaping. */
	tmu_max_token_bucket_set(TOKEN_BUCKET_MAX);
	/* random number */
	tmu_random_number_set(0x0815);
	/* set default reset value
	   The WRED crawler period indicates the time interval
	   to elapse between QAVG calculations on consecutive queues.
	   It is specified with a granularity of 1024 clocks.
	   A value of 0 means that crawler is disabled
	 */
	tmu_crawler_period_set(TMU_WRED_CRAWLER_PERIOD_DEFAULT);
	/* Enqueue Request Delay */
	tmu_enqueue_delay_set(TMU_ENQUEUE_REQUEST_DELAY_DEFAULT);
	/* token accumulation period, set to as fast as possible */
	tmu_tacc_period_set(TMU_TOKEN_ACC_PERIOD_DEFAULT);
}

void tmu_qemt_write(const uint32_t qid, const uint32_t epn)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(epn, qemt);
	tmu_w32(TMU_QMTC_QEW | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QEV) == 0)
		continue;

#else
	simu_qemt[qid] = epn;
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qemt_write);

void tmu_qemt_read(const uint32_t qid, uint32_t *epn)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_QMTC_QER | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QEV) == 0)
		continue;

	*epn = tmu_r32(qemt);
#else
	*epn = simu_qemt[qid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qemt_read);

void tmu_qsmt_write(const uint32_t qid, const uint32_t sbin)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(sbin, qsmt);
	tmu_w32(TMU_QMTC_QSW | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QSV) == 0)
		continue;

#else
	simu_qsmt[qid] = sbin;
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qsmt_write);

void tmu_qsmt_read(const uint32_t qid, uint32_t *sbin)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_QMTC_QSR | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QSV) == 0)
		continue;

	*sbin = tmu_r32(qsmt);
#else
	*sbin = simu_qsmt[qid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qsmt_read);

void tmu_qtht_write(const uint32_t qid, const uint32_t *qtht)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(qtht[0], qtht0);
	tmu_w32(qtht[1], qtht1);
	tmu_w32(qtht[2], qtht2);
	tmu_w32(qtht[3], qtht3);
	tmu_w32(qtht[4], qtht4);
	tmu_w32(TMU_QMTC_QTW | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QTV) == 0)
		continue;

#else
	simu_qtht0[qid] = qtht[0];
	simu_qtht1[qid] = qtht[1];
	simu_qtht2[qid] = qtht[2];
	simu_qtht3[qid] = qtht[3];
	simu_qtht4[qid] = qtht[4];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qtht_write);

void tmu_qtht_read(const uint32_t qid, uint32_t *qtht)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_QMTC_QTR | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QTV) == 0)
		continue;

	qtht[0] = tmu_r32(qtht0);
	qtht[1] = tmu_r32(qtht1);
	qtht[2] = tmu_r32(qtht2);
	qtht[3] = tmu_r32(qtht3);
	qtht[4] = tmu_r32(qtht4);
#else
	qtht[0] = simu_qtht0[qid];
	qtht[1] = simu_qtht1[qid];
	qtht[2] = simu_qtht2[qid];
	qtht[3] = simu_qtht3[qid];
	qtht[4] = simu_qtht4[qid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qtht_read);

void tmu_qoct_write(const uint32_t qid, const uint32_t wq,
		    const uint32_t qrth, const uint32_t qocc,
		    const uint32_t qavg)
{
	uint32_t tmp =
	    ((wq << TMU_QOCT0_WQ_OFFSET) & TMU_QOCT0_WQ_MASK) | (qrth &
								 TMU_QOCT0_QRTH_MASK);
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(tmp, qoct0);
	tmu_w32(qocc & TMU_QOCT1_QOCC_MASK, qoct1);
	tmu_w32(qavg & TMU_QOCT2_QAVG_MASK, qoct2);
	tmu_w32(TMU_QMTC_QOW | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QOV) == 0)
		continue;

#else
	simu_qoct0[qid] = tmp;
	simu_qoct1[qid] = qocc & TMU_QOCT1_QOCC_MASK;
	simu_qoct2[qid] = qavg & TMU_QOCT2_QAVG_MASK;
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qoct_write);

void tmu_qoct_read(const uint32_t qid, uint32_t *wq, uint32_t *qrth,
		   uint32_t *qocc, uint32_t *qavg)
{
	uint32_t tmp;
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_QMTC_QOR | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QOV) == 0)
		continue;

	tmp = tmu_r32(qoct0);
	*wq = (tmp & TMU_QOCT0_WQ_MASK) >> TMU_QOCT0_WQ_OFFSET;
	*qrth = tmp & TMU_QOCT0_QRTH_MASK;
	*qocc = tmu_r32(qoct1) & TMU_QOCT1_QOCC_MASK;
	*qavg = tmu_r32(qoct2) & TMU_QOCT2_QAVG_MASK;
#else
	tmp = simu_qoct0[qid];
	*wq = (tmp & TMU_QOCT0_WQ_MASK) >> TMU_QOCT0_WQ_OFFSET;
	*qrth = tmp & TMU_QOCT0_QRTH_MASK;
	*qocc = simu_qoct1[qid] & TMU_QOCT1_QOCC_MASK;
	*qavg = simu_qoct2[qid] & TMU_QOCT2_QAVG_MASK;
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qoct_read);

void tmu_qdct_write(const uint32_t qid, const uint32_t *qdc)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(qdc[0], qdct0);
	tmu_w32(qdc[1], qdct1);
	tmu_w32(qdc[2], qdct2);
	tmu_w32(qdc[3], qdct3);
	tmu_w32(TMU_QMTC_QDW | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QDV) == 0)
		continue;

#else
	simu_qdct0[qid] = qdc[0];
	simu_qdct1[qid] = qdc[1];
	simu_qdct2[qid] = qdc[2];
	simu_qdct3[qid] = qdc[3];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qdct_write);

void tmu_qdct_read(const uint32_t qid, uint32_t *qdc)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_QMTC_QDR | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QDV) == 0)
		continue;

	qdc[0] = tmu_r32(qdct0);
	qdc[1] = tmu_r32(qdct1);
	qdc[2] = tmu_r32(qdct2);
	qdc[3] = tmu_r32(qdct3);
#else
	qdc[0] = simu_qdct0[qid];
	qdc[1] = simu_qdct1[qid];
	qdc[2] = simu_qdct2[qid];
	qdc[3] = simu_qdct3[qid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qdct_read);

void tmu_qfmt_write(const uint32_t qid, const uint32_t *qfm)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(qfm[0], qfmt0);
	tmu_w32(qfm[1], qfmt1);
	tmu_w32(qfm[2], qfmt2);
	tmu_w32(TMU_QMTC_QFW | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QFV) == 0)
		continue;

#else
	simu_qfmt0[qid] = qfm[0];
	simu_qfmt1[qid] = qfm[1];
	simu_qfmt2[qid] = qfm[2];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qfmt_write);

void tmu_qfmt_read(const uint32_t qid, uint32_t *qfm)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_QMTC_QFR | qid, qmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(qmtc) & TMU_QMTC_QFV) == 0)
		continue;

	qfm[0] = tmu_r32(qfmt0);
	qfm[1] = tmu_r32(qfmt1);
	qfm[2] = tmu_r32(qfmt2);
#else
	qfm[0] = simu_qfmt0[qid];
	qfm[1] = simu_qfmt1[qid];
	qfm[2] = simu_qfmt2[qid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_qfmt_read);

/**
   Egress Queue Table (EQT) comprises the following tables:
   QEMT: Queue Egress Mapping Table
   QSMT: Queue Scheduler Mapping Table
   QTHT: Queue Manager and Threshold Table
   QOCT: Queue Occupancy Table
   QDCT: Queue Discard Table
   QFMT: Queue FIFO Manager Table

   all egress queues are considered unused (inactive) after initialization
   an egress queue must be explicitely "created" with
   ref tmu_egress_queue_create in order to participate in the data path
   all unused egress queues are connected to SB Input
   ref NULL_SCHEDULER_INPUT_ID, which is considered reserved
   all egress queues are initially disabled
*/
static void tmu_egress_queue_table_init(void)
{
	uint32_t i;
	uint32_t qtht[5];
	uint32_t qdc[4] = { 0, 0, 0, 0 };
	uint32_t qfm[3] = { 0, 0x3FFF3FFF, 0 };
	/* qtht0 Setting: per queue based settting
	   queue disabled, no admission of packets                 :0x0
	   dropping mode = wred (Weighted Random Early Detection)  :0x1
	   color 3 (red) mapped to Threshold Index 1 (QTTH1)       :0x1
	   color 2 (yellow) mapped to Threshold Index 3 (MATH1)    :0x3
	   color 1 (green) mapped to Threshold Index 2 (MATH0)     :0x2
	   color 0 (unassigned) mapped to Threshold Index 0 (QTTH0):0x0
	 */
	qtht[0] = 0x00011320;
	/* minimum threshold for WRED curve 0/1:
	   in units of 8 segments, 0x900 = 2304, 2304*8=18432 is total SSB
	   SGH change from GPON's 0x09000900
	 */
	qtht[1] = (TMU_QTHT1_DEFAULT << 16) | TMU_QTHT1_DEFAULT;
	/* maximum threshold for WRED curve 0/1:
	   in units of 8 segments, 0x900 = 2304, 2304*8=18432 is total SSB
	   SGH change from GPON's 0x09000900
	 */
	qtht[2] = (TMU_QTHT2_DEFAULT << 16) | TMU_QTHT2_DEFAULT;
	/* slope of WRED curve 0/1: set to max */
	qtht[3] = (TMU_QTHT3_WRED_DEFAULT << 16) | TMU_QTHT3_WRED_DEFAULT;
	/* tail drop threshold 0/1, in units of 8 segments, set to total SSB */
	/* default values for red & unassigned */
	qtht[4] = (TMU_QTHT4_1_DEFAULT << 16) | TMU_QTHT4_0_DEFAULT;

	for (i = 0; i < EGRESS_QUEUE_ID_MAX; i++) {
		/* set invalid egress port ID 72. The valid port id is 0 ~ 71 */
		tmu_qemt_write(i, EPNNULL_EGRESS_PORT_ID);
		/* write QSMT table, assign to reserved scheduler input */
		/*it seems server SB 1023 for all port's
		   SBID from the begining
		 */
		tmu_qsmt_write(i, NULL_SCHEDULER_INPUT_ID);
		/* write QTHT table */
		tmu_qtht_write(i, &qtht[0]);
		/*write QOCT table
		   Weight, used for WRED average calc: 2**(-WQ)
		   queue occupancy, in units of segments, initially empty
		   queue average fill level of WRED, in units of segments,
		   initially empty
		   default value for queue reservation threshold
		   is 320 segments
		 */
#ifndef TMU_SOC_REAL_BOARD
		/*sgh: changed as per input from Schaefer Franz
		   Josef (gpon team) changed on 21 may 2014
		 */
		tmu_qoct_write(i, 0, 0, 0, 0);
#else
		/*tmu_qoct_write(i, 10, 40, 0, 0); Gpon orignal configuration */
		tmu_qoct_write(i, 10, 0, 0, 0);
#endif
		/*  write QDCT table
		   queue discard counter for color 0/1/2/3, counts PDUs,
		   initially zero discards
		 */
		tmu_qdct_write(i, &qdc[0]);
		/*  write QFMT table */
		/* queue filled indication, is initially 0
		   queue occupancy, in units of packets, initially empty = 0
		   tail queue packet pointer, 3FFF=NIL
		   GPON:
		   (0x3fff > 9215 = 0x23FF = PACKET_POINTER_TABLE_INDEX_MAX)
		   sgh: GRX500:
		   (0x3fff > 6144 = 0x1800 = PACKET_POINTER_TABLE_INDEX_MAX)
		   head queue packet pointer, 3FFF=NIL
		   read only, QOS length used for scheduling and shaping
		   PDU color, set to unassigned = 0
		   PDU length in segments, initially set to 0
		 */
		tmu_qfmt_write(i, &qfm[0]);
	}
}

void tmu_epmt_write(const uint32_t epn, const uint32_t epe,
		    const uint32_t sbid)
{
	uint32_t tmp = sbid & TMU_EPMT_SBID_MASK;
	unsigned long flags = 0;

	if (epe)
		tmp |= TMU_EPMT_EPE;

	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(tmp, epmt);
	tmu_w32(TMU_EPMTC_EMW | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_EMV) == 0)
		continue;

#else
	simu_epmt[epn] = tmp;
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_epmt_write);

void tmu_epmt_read(const uint32_t epn, uint32_t *epe, uint32_t *sbid)
{
	uint32_t tmp;
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_EPMTC_EMR | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_EMV) == 0)
		continue;

	tmp = tmu_r32(epmt);
#else
	tmp = simu_epmt[epn];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
	*sbid = tmp & TMU_EPMT_SBID_MASK;
	*epe = (tmp & TMU_EPMT_EPE) ? 1 : 0;
}

void tmu_epot_write(const uint32_t epn, const uint32_t *epoc)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(epoc[0], epot0);
	tmu_w32(epoc[1], epot1);
	tmu_w32(TMU_EPMTC_EOW | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_EOV) == 0)
		continue;

#else
	simu_epot0[epn] = epoc[0];
	simu_epot1[epn] = epoc[1];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_epot_write);

void tmu_epot_read(const uint32_t epn, uint32_t *epoc)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_EPMTC_EOR | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_EOV) == 0)
		continue;

	epoc[0] = tmu_r32(epot0);
	epoc[1] = tmu_r32(epot1);
#else
	epoc[0] = simu_epot0[epn];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_epot_read);

void tmu_eptt_write(const uint32_t epn, const uint32_t *ept)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(ept[0], eptt0);
	tmu_w32(ept[1], eptt1);
	tmu_w32(TMU_EPMTC_ETW | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_ETV) == 0)
		continue;

#else
	simu_eptt0[epn] = ept[0];
	simu_eptt1[epn] = ept[1];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_eptt_write);

void tmu_eptt_read(const uint32_t epn, uint32_t *ept)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_EPMTC_ETR | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_ETV) == 0)
		continue;

	ept[0] = tmu_r32(eptt0);
	ept[1] = tmu_r32(eptt1);
#else
	ept[0] = simu_eptt0[epn];
	ept[1] = simu_eptt1[epn];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_eptt_read);

void tmu_epdt_write(const uint32_t epn, const uint32_t *epd)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(epd[0], epdt0);
	tmu_w32(epd[1], epdt1);
	tmu_w32(epd[2], epdt2);
	tmu_w32(epd[3], epdt3);
	tmu_w32(TMU_EPMTC_EDW | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_EDV) == 0)
		continue;

#else
	simu_epdt0[epn] = epd[0];
	simu_epdt1[epn] = epd[1];
	simu_epdt2[epn] = epd[2];
	simu_epdt3[epn] = epd[3];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_epdt_write);

void tmu_epdt_read(const uint32_t epn, uint32_t *epd)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_EPMTC_EDR | epn, epmtc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(epmtc) & TMU_EPMTC_EDV) == 0)
		continue;

	epd[0] = tmu_r32(epdt0);
	epd[1] = tmu_r32(epdt1);
	epd[2] = tmu_r32(epdt2);
	epd[3] = tmu_r32(epdt3);
#else
	epd[0] = simu_epdt0[epn];
	epd[1] = simu_epdt1[epn];
	epd[2] = simu_epdt2[epn];
	epd[3] = simu_epdt3[epn];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_epdt_read);

/**
   Egress Port Table (EPT) comprises the following tables:
   EPMT: Egress Port Mapping Table
   EPOT: Egress Port Occupancy Table
   EPTT: Egress Port Threshold Table
   EPDT: Egress Port Discard Table

   an egress port must be explicitely "created" in order to participate
   in the data path
   all egress ports are initially disabled for transmission
   each egress port has the reserved scheduler block 127 attached
*/
static void tmu_egress_port_table_init(void)
{
	uint32_t i;
	uint32_t epoc[2] = { 0, 0 };
	/* 4000 segments by default */
#if 0
	/*GPON team: 0x1f4 *8 = 4000 segments,ie about 1/4 of 18432 */
	uint32_t ept[2] = { 0x01f401f4, 0x01f401f4 };
#else
	/*sgh: GRX500: 0x120 * 8 = 2304 segments, ie about 1/4 of 9126 */
	uint32_t ept[2] = {
		(TMU_EPTT0_DEFAULT << 16) | TMU_EPTT0_DEFAULT,
		(TMU_EPTT0_DEFAULT << 16) | TMU_EPTT0_DEFAULT
	};
#endif
	uint32_t epd[4] = { 0, 0, 0, 0 };

	for (i = 0; i < EGRESS_PORT_ID_MAX; i++) {
		/* egress port mapping table, all port disabled scheduler
		   block ID = NULL_SCHEDULER_BLOCK_ID
		 */
		tmu_epmt_write(i, 0, NULL_SCHEDULER_BLOCK_ID);
		/* egress port fill level for color 0/1/2/3, initially empty */
		tmu_epot_write(i, &epoc[0]);
		/* egress port discard threshold for color 0/1/2/3, set to
		   max = total SSB
		 */
		tmu_eptt_write(i, &ept[0]);
		/* number of discarded PDUs for color 0/1/2/3 set to 0 */
		tmu_epdt_write(i, &epd[0]);
	}
}

/**
   Scheduler Block Input Table (SBIT)
   all inputs disabled
   no shapers enabled
   default configuration: every scheduler block input has "reserved"
   queue 255 attached.
*/
static void tmu_sched_blk_in_table_init(void)
{
	uint32_t i;
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);

	for (i = 0; i < SCHEDULER_BLOCK_INPUT_ID_MAX; i++) {
		/* input disabled
		   inverse WFQ weight: 0 = strict priority
		   (smaller values for higher prio)
		   queue type: 0 = queue
		 */
		tmu_w32(0xFF, sbitr0);
		/* token bucket disabled, no shaping */
		tmu_w32(255, sbitr1);
		tmu_w32(0, sbitr2);
		tmu_w32(0, sbitr3);
		tmu_w32(TMU_SBITC_RW_W | TMU_SBITC_SEL | i, sbitc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		while ((tmu_r32(sbitc) & TMU_SBITC_VAL) == 0)
			continue;

#else
		simu_sbitr0[i] = 0xFF;
		simu_sbitr1[i] = 255;
		simu_sbitr2[i] = 0;
		simu_sbitr3[i] = 0;
#endif
	}

	tmu_spin_lock_release(&tmu_lock, flags);
}

/**
   Scheduler Block Output Table (SBOT)
   all 128 SB are unused (inactive) after initialization
   all outputs disabled
*/
static void tmu_sched_blk_out_table_init(void)
{
	uint32_t i;
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);

	for (i = 0; i < SCHEDULER_BLOCK_ID_MAX; i++) {
		/* output disabled
		   hierarchy level of the SB: 0 (numbered from ingress side).
		   Note: typically max level is 3. Theoretically max is 8,
		   but this may cause performance problems with high bandwidth.
		   output is connected reserved egress port 72
		 */
		tmu_w32(EPNNULL_EGRESS_PORT_ID, sbotr0);
		/* output initially not filled default winner
		   leaf (local SB input 0 to 7)) NIL winner QID
		 */
		tmu_w32(0xFF, sbotr1);
		tmu_w32(TMU_SBOTC_RW | TMU_SBOTC_SEL | i, sbotc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
			continue;

#else
		simu_sbotr0[i] = EPNNULL_EGRESS_PORT_ID;
		simu_sbotr1[i] = 0xFF;
#endif
	}

	tmu_spin_lock_release(&tmu_lock, flags);
}

/**
   Token Bucket Shaper Table (TBST)
   all TB disabled
   color blind
   NIL scheduler block input ID (SBIN)
*/
static void tmu_token_bucket_shaper_table_init(void)
{
	uint32_t i;
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);

	for (i = 0; i <= TOKEN_BUCKET_MAX; i++) {
		/*
		   color blind token bucket attached to reserved scheduler
		   input 1023
		 */
		tmu_w32(NULL_SCHEDULER_INPUT_ID, tbstr0);
		/* all buckets disabled
		   64 bytes max time between accumulations (lowest rate)
		   1 Byte added to the TBC0 with each SRC0 elapse
		 */
		tmu_w32(0xFFFF, tbstr1);
		tmu_w32(0xFFFF, tbstr2);
		/* bucket 0/1: max size of bucket in bytes,
		   0 will block (lowest rate)
		 */
		tmu_w32(0, tbstr3);
		tmu_w32(0, tbstr4);
		/* status values ... */
		tmu_w32(0, tbstr5);
		tmu_w32(0, tbstr6);
		tmu_w32(0, tbstr7);
		tmu_w32(0, tbstr8);
		tmu_w32(0, tbstr9);
		tmu_w32(0, tbstr10);
		tmu_w32(TMU_TBSTC_RW | TMU_TBSTC_SEL | i, tbstc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
			continue;

#else
		simu_tbstr0[i] = NULL_SCHEDULER_INPUT_ID;
		simu_tbstr1[i] = 0xFFFF;
		simu_tbstr2[i] = 0xFFFF;
		simu_tbstr3[i] = 0;
		simu_tbstr4[i] = 0;
		simu_tbstr5[i] = 0;
		simu_tbstr6[i] = 0;
		simu_tbstr7[i] = 0;
		simu_tbstr8[i] = 0;
		simu_tbstr9[i] = 0;
		simu_tbstr10[i] = 0;
#endif
	}

	tmu_spin_lock_release(&tmu_lock, flags);
}

/**
   Packet Pointer Table (PPT)
   set up linked list
*/
static void tmu_packet_pointer_table_init(void)
{
	uint32_t i, tmp;
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);

	/* loop over all PPT entries */
	for (i = 0; i < PACKET_POINTER_TABLE_INDEX_MAX; i++) {
		tmp = (i + 1) % (PACKET_POINTER_TABLE_INDEX_MAX);
		tmu_w32((tmp << TMU_PPT0_PNEXT_OFFSET) & TMU_PPT0_PNEXT_MASK,
			ppt0);
		tmu_w32(0, ppt1);
		tmu_w32(0, ppt2);
		tmu_w32(0, ppt3);
		tmu_w32(TMU_PPTC_RW | i, pptc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		while ((tmu_r32(pptc) & TMU_PPTC_VAL) == 0)
			continue;

#else
		simu_ppt0[i] =
		    (tmp << TMU_PPT0_PNEXT_OFFSET) & TMU_PPT0_PNEXT_MASK;
		simu_ppt1[i] = 0;
		simu_ppt2[i] = 0;
		simu_ppt3[i] = 0;
#endif
	}

	tmu_spin_lock_release(&tmu_lock, flags);
	tmu_w32(((PACKET_POINTER_TABLE_INDEX_MAX - 1) << TMU_FPL_TFPP_OFFSET)
		& TMU_FPL_TFPP_MASK, fpl);
}

void tmu_equeue_enable(const uint32_t qid, bool ena)
{
	/*QTHT[qid].QE = ena; */
	uint32_t qtht[5];
	tmu_qtht_read(qid, &qtht[0]);

	if (ena == true)
		qtht[0] |= TMU_QTHT0_QE_EN;
	else
		qtht[0] &= ~TMU_QTHT0_QE_EN;

	tmu_qtht_write(qid, &qtht[0]);
}
EXPORT_SYMBOL(tmu_equeue_enable);

bool tmu_is_equeue_enabled(const uint32_t qid)
{
	uint32_t qtht[5];
	tmu_qtht_read(qid, &qtht[0]);
	return (qtht[0] & TMU_QTHT0_QE_EN) ? true : false;
}
EXPORT_SYMBOL(tmu_is_equeue_enabled);

void tmu_equeue_link_set(const uint32_t qid,
			 struct tmu_equeue_link *equeue_link)
{
	/*
	   QEMT[qid].EPN  = equeue_link->epn;
	   QSMT[qid].SBID = equeue_link->sbid;
	   ******* Note: equeue_link.qe is not set at all ******
	 */
	uint32_t qemt;
	uint32_t qsmt;
	tmu_qemt_read(qid, &qemt);
	tmu_qsmt_read(qid, &qsmt);
	set_val(qemt, equeue_link->epn, TMU_QEMT_EPN_MASK,
		TMU_QEMT_EPN_OFFSET);
	set_val(qsmt, equeue_link->sbin, TMU_QSMT_SBIN_MASK,
		TMU_QSMT_SBIN_OFFSET);
	tmu_qemt_write(qid, qemt);
	tmu_qsmt_write(qid, qsmt);
}
EXPORT_SYMBOL(tmu_equeue_link_set);

void tmu_equeue_link_get(const uint32_t qid,
			 struct tmu_equeue_link *equeue_link)
{
	/*
	   equeue_link->qe   = QTHT[qid].QE;
	   equeue_link->epn  = QEMT[qid].EPN;
	   equeue_link->sbid = QSMT[qid].SBID;
	 */
	uint32_t qemt;
	uint32_t qsmt;
	tmu_qemt_read(qid, &qemt);
	tmu_qsmt_read(qid, &qsmt);
	equeue_link->qe = tmu_is_equeue_enabled(qid);
	equeue_link->epn =
	    get_val(qemt, TMU_QEMT_EPN_MASK, TMU_QEMT_EPN_OFFSET);
	equeue_link->sbin =
	    get_val(qsmt, TMU_QSMT_SBIN_MASK, TMU_QSMT_SBIN_OFFSET);
}
EXPORT_SYMBOL(tmu_equeue_link_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_color_thr_map_set(const uint32_t qid,
				  struct tmu_equeue_drop_params *map)
{
	/*
	   QTHT[qid].COL0 = map->col[0];
	   QTHT[qid].COL1 = map->col[1];
	   QTHT[qid].COL2 = map->col[2];
	   QTHT[qid].COL3 = map->col[3];
	 */
	uint32_t qtht[5];
	tmu_qtht_read(qid, &qtht[0]);
	set_val(qtht[0], map->col[0], TMU_QTHT0_COL0_MASK,
		TMU_QTHT0_COL0_OFFSET);
	set_val(qtht[0], map->col[1], TMU_QTHT0_COL1_MASK,
		TMU_QTHT0_COL1_OFFSET);
	set_val(qtht[0], map->col[2], TMU_QTHT0_COL2_MASK,
		TMU_QTHT0_COL2_OFFSET);
	set_val(qtht[0], map->col[3], TMU_QTHT0_COL3_MASK,
		TMU_QTHT0_COL3_OFFSET);
	tmu_qtht_write(qid, &qtht[0]);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_color_thr_map_get(const uint32_t qid,
				  struct tmu_equeue_drop_params *map)
{
	/*
	   map->col[0]  QTHT[qid].COL0;
	   map->col[1]  QTHT[qid].COL1;
	   map->col[2]  QTHT[qid].COL2;
	   map->col[3]  QTHT[qid].COL3;
	 */
	uint32_t qtht[5];
	tmu_qtht_read(qid, &qtht[0]);
	map->col[0] =
	    get_val(qtht[0], TMU_QTHT0_COL0_MASK, TMU_QTHT0_COL0_OFFSET);
	map->col[1] =
	    get_val(qtht[0], TMU_QTHT0_COL1_MASK, TMU_QTHT0_COL1_OFFSET);
	map->col[2] =
	    get_val(qtht[0], TMU_QTHT0_COL2_MASK, TMU_QTHT0_COL2_OFFSET);
	map->col[3] =
	    get_val(qtht[0], TMU_QTHT0_COL3_MASK, TMU_QTHT0_COL3_OFFSET);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

static uint32_t tmu_wred_curve_slope_get(const uint32_t mapr,
					 const uint32_t mith,
					 const uint32_t math)
{
	uint32_t slope;
	slope = math > mith ? (mapr << 7) / (math - mith) : 0xFFF;
	return slope > 0xFFF ? 0xFFF : slope;
}

void tmu_wred_curve_max_px_set(const uint32_t curve, const uint32_t qid,
			       const uint32_t max_pr)
{
	tmu_max_px[curve][qid] = max_pr;
}
EXPORT_SYMBOL(tmu_wred_curve_max_px_set);

uint32_t tmu_wred_curve_max_px_get(const uint32_t curve, const uint32_t qid)
{
	return (uint32_t) tmu_max_px[curve][qid];
}
EXPORT_SYMBOL(tmu_wred_curve_max_px_get);

void tmu_equeue_drop_params_set(uint32_t qid,
				struct tmu_equeue_drop_params *thx)
{
	uint32_t qtht[5] = { 0 };
	uint32_t slope0, slope1;
	uint32_t wq, qrth, qocc, qavg;
	tmu_qoct_read(qid, &wq, &qrth, &qocc, &qavg);

	if (thx->qe == true)
		qtht[0] |= TMU_QTHT0_QE_EN;
	else
		qtht[0] &= ~TMU_QTHT0_QE_EN;

	if (thx->dmod == 1)
		qtht[0] |= TMU_QTHT0_DMOD_WR;
	else
		qtht[0] &= ~TMU_QTHT0_DMOD_WR;

	set_val(qtht[0], thx->col[0], TMU_QTHT0_COL0_MASK,
		TMU_QTHT0_COL0_OFFSET);
	set_val(qtht[0], thx->col[1], TMU_QTHT0_COL1_MASK,
		TMU_QTHT0_COL1_OFFSET);
	set_val(qtht[0], thx->col[2], TMU_QTHT0_COL2_MASK,
		TMU_QTHT0_COL2_OFFSET);
	set_val(qtht[0], thx->col[3], TMU_QTHT0_COL3_MASK,
		TMU_QTHT0_COL3_OFFSET);
	set_val(qtht[1], thx->mith0, TMU_QTHT1_MITH0_MASK,
		TMU_QTHT1_MITH0_OFFSET);
	set_val(qtht[1], thx->mith1, TMU_QTHT1_MITH1_MASK,
		TMU_QTHT1_MITH1_OFFSET);
	set_val(qtht[2], thx->math0, TMU_QTHT2_MATH0_MASK,
		TMU_QTHT2_MATH0_OFFSET);
	set_val(qtht[2], thx->math1, TMU_QTHT2_MATH1_MASK,
		TMU_QTHT2_MATH1_OFFSET);
	slope0 = tmu_wred_curve_slope_get(thx->maxp0, thx->mith0, thx->math0);
	slope1 = tmu_wred_curve_slope_get(thx->maxp1, thx->mith1, thx->math1);
	set_val(qtht[3], slope0, TMU_QTHT3_SLOPE0_MASK,
		TMU_QTHT3_SLOPE0_OFFSET);
	set_val(qtht[3], slope1, TMU_QTHT3_SLOPE1_MASK,
		TMU_QTHT3_SLOPE1_OFFSET);
	set_val(qtht[4], thx->qtth0, TMU_QTHT4_QTTH0_MASK,
		TMU_QTHT4_QTTH0_OFFSET);
	set_val(qtht[4], thx->qtth1, TMU_QTHT4_QTTH1_MASK,
		TMU_QTHT4_QTTH1_OFFSET);
	tmu_qoct_write(qid, thx->wq, thx->qrth, qocc, qavg);
	tmu_qtht_write(qid, &qtht[0]);
}
EXPORT_SYMBOL(tmu_equeue_drop_params_set);

void tmu_equeue_drop_params_get(uint32_t qid,
				struct tmu_equeue_drop_params *thx)
{
	uint32_t wq, qocc, qavg;
	uint32_t qtht[5];
	uint32_t slope0, slope1;
	tmu_qoct_read(qid, &wq, &thx->qrth, &qocc, &qavg);
	tmu_qtht_read(qid, &qtht[0]);
	thx->wq = (uint8_t) wq;
	thx->qe = (qtht[0] & TMU_QTHT0_QE_EN) ? true : false;
	thx->dmod = (qtht[0] & TMU_QTHT0_DMOD_WR) ? 1 : 0;
	thx->col[0] =
	    get_val(qtht[0], TMU_QTHT0_COL0_MASK, TMU_QTHT0_COL0_OFFSET);
	thx->col[1] =
	    get_val(qtht[0], TMU_QTHT0_COL1_MASK, TMU_QTHT0_COL1_OFFSET);
	thx->col[2] =
	    get_val(qtht[0], TMU_QTHT0_COL2_MASK, TMU_QTHT0_COL2_OFFSET);
	thx->col[3] =
	    get_val(qtht[0], TMU_QTHT0_COL3_MASK, TMU_QTHT0_COL3_OFFSET);
	thx->mith0 =
	    get_val(qtht[1], TMU_QTHT1_MITH0_MASK, TMU_QTHT1_MITH0_OFFSET);
	thx->mith1 =
	    get_val(qtht[1], TMU_QTHT1_MITH1_MASK, TMU_QTHT1_MITH1_OFFSET);
	thx->math0 =
	    get_val(qtht[2], TMU_QTHT2_MATH0_MASK, TMU_QTHT2_MATH0_OFFSET);
	thx->math1 =
	    get_val(qtht[2], TMU_QTHT2_MATH1_MASK, TMU_QTHT2_MATH1_OFFSET);
	slope0 =
	    get_val(qtht[3], TMU_QTHT3_SLOPE0_MASK, TMU_QTHT3_SLOPE0_OFFSET);
	slope1 =
	    get_val(qtht[3], TMU_QTHT3_SLOPE1_MASK, TMU_QTHT3_SLOPE1_OFFSET);
	thx->maxp0 = (slope0 * (thx->math0 - thx->mith0)) >> 7;
	thx->maxp1 = (slope1 * (thx->math1 - thx->mith1)) >> 7;
	thx->qtth0 =
	    get_val(qtht[4], TMU_QTHT4_QTTH0_MASK, TMU_QTHT4_QTTH0_OFFSET);
	thx->qtth1 =
	    get_val(qtht[4], TMU_QTHT4_QTTH1_MASK, TMU_QTHT4_QTTH1_OFFSET);
}
EXPORT_SYMBOL(tmu_equeue_drop_params_get);

void tmu_equeue_fill_status_get(uint32_t *qfill)
{
	int reg;

	for (reg = 0; reg < 8; reg++)
		qfill[reg] = tmu_r32(qfill[reg]);
}
EXPORT_SYMBOL(tmu_equeue_fill_status_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_seg_occupancy_get(uint32_t qid, uint32_t *qocc)
{
	uint32_t wq, qrth, qavg;
	tmu_qoct_read(qid, &wq, &qrth, qocc, &qavg);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_pdu_occupancy_get(uint32_t qid, uint32_t *pocc)
{
	uint32_t qfm[3];
	tmu_qfmt_read(qid, &qfm[0]);
	*pocc = get_val(qfm[0], TMU_QFMT0_POCC_MASK, TMU_QFMT0_POCC_OFFSET);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_average_get(uint32_t qid, uint32_t *qavg)
{
	uint32_t wq, qrth, qocc;
	tmu_qoct_read(qid, &wq, &qrth, &qocc, qavg);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_discard_counters_set(uint32_t qid, uint32_t *qdc)
{
	tmu_qdct_write(qid, qdc);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_equeue_discard_counters_get(uint32_t qid, uint32_t *qdc)
{
	tmu_qdct_read(qid, qdc);
}
EXPORT_SYMBOL(tmu_equeue_discard_counters_get);

void tmu_eport_discard_counters_get(const uint32_t epn, uint32_t *epd)
{
	tmu_epdt_read(epn, epd);
}
EXPORT_SYMBOL(tmu_eport_discard_counters_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_equeue_status_get(const uint32_t qid,
			   struct tmu_equeue_status *eqstatus)
{
	uint32_t qfm[3];
	uint32_t wq, qrth;
	uint32_t qfill_reg;
	uint32_t qfill_bit;
	tmu_qoct_read(qid, &wq, &qrth, &eqstatus->qocc, &eqstatus->qavg);
	tmu_qfmt_read(qid, &qfm[0]);
	qfill_reg = qid / 32;
	qfill_bit = 1 << (qid % 32);
	eqstatus->qf = tmu_r32(qfill[qfill_reg]) & qfill_bit ? true : false;
	eqstatus->pocc =
	    get_val(qfm[0], TMU_QFMT0_POCC_MASK, TMU_QFMT0_POCC_OFFSET);
	eqstatus->hqpp =
	    get_val(qfm[1], TMU_QFMT1_HQPP_MASK, TMU_QFMT1_HQPP_OFFSET);
	eqstatus->tqpp =
	    get_val(qfm[1], TMU_QFMT1_TQPP_MASK, TMU_QFMT1_TQPP_OFFSET);
	eqstatus->qosl =
	    get_val(qfm[2], TMU_QFMT2_QOSL_MASK, TMU_QFMT2_QOSL_OFFSET);
	eqstatus->col =
	    get_val(qfm[2], TMU_QFMT2_COL_MASK, TMU_QFMT2_COL_OFFSET);
	eqstatus->segl =
	    get_val(qfm[2], TMU_QFMT2_SEGL_MASK, TMU_QFMT2_SEGL_OFFSET);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_cfgcmd_write(const uint32_t cfgcmd)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(cfgcmd, cfgcmd);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(cfgcmd) & TMU_CFGCMD_VAL) == 0)
		continue;

#else
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_cfgcmd_write);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_cfgcmd_read(uint32_t *cfgcmd)
{
	*cfgcmd = tmu_r32(cfgcmd);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_sbit_write(const uint32_t sbin, const uint32_t *sbit)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(sbit[0], sbitr0);
	tmu_w32(sbit[1], sbitr1);
	tmu_w32(sbit[2], sbitr2);
	tmu_w32(sbit[3], sbitr3);
	tmu_w32(TMU_SBITC_RW_W | TMU_SBITC_SEL | sbin, sbitc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbitc) & TMU_SBITC_VAL) == 0)
		continue;

#else
	simu_sbitr0[sbin] = sbit[0];
	simu_sbitr1[sbin] = sbit[1];
	simu_sbitr2[sbin] = sbit[2];
	simu_sbitr3[sbin] = sbit[3];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}

void tmu_sbit_read(const uint32_t sbin, uint32_t *sbit)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_SBITC_SEL | sbin, sbitc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbitc) & TMU_SBITC_VAL) == 0)
		continue;

	sbit[0] = tmu_r32(sbitr0);
	sbit[1] = tmu_r32(sbitr1);
	sbit[2] = tmu_r32(sbitr2);
	sbit[3] = tmu_r32(sbitr3);
#else
	sbit[0] = simu_sbitr0[sbin];
	sbit[1] = simu_sbitr1[sbin];
	sbit[2] = simu_sbitr2[sbin];
	sbit[3] = simu_sbitr3[sbin];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_sbit_read);

void tmu_sbot_write(const uint32_t sbid, const uint32_t *sbot)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(sbot[0], sbotr0);
	tmu_w32(sbot[1], sbotr1);
	tmu_w32(TMU_SBOTC_RW_W | TMU_SBOTC_SEL | sbid, sbotc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
		continue;

#else
	simu_sbotr0[sbid] = sbot[0];
	simu_sbotr1[sbid] = sbot[1];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_sbot_write);

void tmu_sbot_read(const uint32_t sbid, uint32_t *sbot)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_SBOTC_SEL | sbid, sbotc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
		continue;

	sbot[0] = tmu_r32(sbotr0);
	sbot[1] = tmu_r32(sbotr1);
#else
	simu_sbotr0[sbid] = sbot[0];
	simu_sbotr1[sbid] = sbot[1];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_sbot_read);

void tmu_sbot_write_cfg(const uint32_t sbid, const uint32_t *sbot)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(sbot[0], sbotr0);
	tmu_w32(TMU_SBOTC_RW_W | sbid, sbotc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
		continue;

#else
	simu_sbotr0[sbid] = sbot[0];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_sbot_write_cfg);

void tmu_sbot_read_cfg(const uint32_t sbid, uint32_t *sbot)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(sbid, sbotc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
		continue;

	sbot[0] = tmu_r32(sbotr0);	/*SGH: sbot[1] is status, not cfg */
#else
	sbot[0] = simu_sbotr0[sbid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_sbot_read_cfg);

void tmu_tbst_write(const uint32_t tbid, const uint32_t *tbst)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(tbst[0], tbstr0);
	tmu_w32(tbst[1], tbstr1);
	tmu_w32(tbst[2], tbstr2);
	tmu_w32(tbst[3], tbstr3);
	tmu_w32(tbst[4], tbstr4);
	tmu_w32(tbst[5], tbstr5);
	tmu_w32(tbst[6], tbstr6);
	tmu_w32(tbst[7], tbstr7);
	tmu_w32(tbst[8], tbstr8);
	tmu_w32(tbst[9], tbstr9);
	tmu_w32(tbst[10], tbstr10);
	tmu_w32(TMU_TBSTC_RW_W | TMU_TBSTC_SEL | tbid, tbstc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
		continue;

#else
	simu_tbstr0[tbid] = tbst[0];
	simu_tbstr1[tbid] = tbst[1];
	simu_tbstr2[tbid] = tbst[2];
	simu_tbstr3[tbid] = tbst[3];
	simu_tbstr4[tbid] = tbst[4];
	simu_tbstr5[tbid] = tbst[5];
	simu_tbstr6[tbid] = tbst[6];
	simu_tbstr7[tbid] = tbst[7];
	simu_tbstr8[tbid] = tbst[8];
	simu_tbstr9[tbid] = tbst[9];
	simu_tbstr10[tbid] = tbst[10];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_tbst_write);

void tmu_tbst_read(const uint32_t tbid, uint32_t *tbst)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(TMU_TBSTC_SEL | tbid, tbstc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
		continue;

	tbst[0] = tmu_r32(tbstr0);
	tbst[1] = tmu_r32(tbstr1);
	tbst[2] = tmu_r32(tbstr2);
	tbst[3] = tmu_r32(tbstr3);
	tbst[4] = tmu_r32(tbstr4);
	tbst[5] = tmu_r32(tbstr5);
	tbst[6] = tmu_r32(tbstr6);
	tbst[7] = tmu_r32(tbstr7);
	tbst[8] = tmu_r32(tbstr8);
	tbst[9] = tmu_r32(tbstr9);
	tbst[10] = tmu_r32(tbstr10);
#else
	tbst[0] = simu_tbstr0[tbid];
	tbst[1] = simu_tbstr1[tbid];
	tbst[2] = simu_tbstr2[tbid];
	tbst[3] = simu_tbstr3[tbid];
	tbst[4] = simu_tbstr4[tbid];
	tbst[5] = simu_tbstr5[tbid];
	tbst[6] = simu_tbstr6[tbid];
	tbst[7] = simu_tbstr7[tbid];
	tbst[8] = simu_tbstr8[tbid];
	tbst[9] = simu_tbstr9[tbid];
	tbst[10] = simu_tbstr10[tbid];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_tbst_read);

void tmu_tbst_write_cfg(const uint32_t tbid, const uint32_t *tbst)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(tbst[0], tbstr0);
	tmu_w32(tbst[1], tbstr1);
	tmu_w32(tbst[2], tbstr2);
	tmu_w32(tbst[3], tbstr3);
	tmu_w32(tbst[4], tbstr4);
	tmu_w32(TMU_TBSTC_RW_W | tbid, tbstc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
		continue;

#else
	simu_tbstr0[tbid] = tbst[0];
	simu_tbstr1[tbid] = tbst[1];
	simu_tbstr2[tbid] = tbst[2];
	simu_tbstr3[tbid] = tbst[3];
	simu_tbstr4[tbid] = tbst[4];
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}
EXPORT_SYMBOL(tmu_tbst_write_cfg);

void tmu_tbst_read_cfg(const uint32_t tbid, uint32_t *tbst)
{
	unsigned long flags = 0;
	tmu_spin_lock_get(&tmu_lock, &flags);
	tmu_w32(tbid, tbstc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
		continue;

	tbst[0] = tmu_r32(tbstr0);
	tbst[1] = tmu_r32(tbstr1);
	tbst[2] = tmu_r32(tbstr2);
	tbst[3] = tmu_r32(tbstr3);
	tbst[4] = tmu_r32(tbstr4);
	tbst[5] = -1;
	tbst[6] = -1;
	tbst[7] = -1;
	tbst[8] = -1;
	tbst[9] = -1;
	tbst[10] = -1;
#else
	tbst[0] = simu_tbstr0[tbid];
	tbst[1] = simu_tbstr1[tbid];
	tbst[2] = simu_tbstr2[tbid];
	tbst[3] = simu_tbstr3[tbid];
	tbst[4] = simu_tbstr4[tbid];
	tbst[5] = -1;
	tbst[6] = -1;
	tbst[7] = -1;
	tbst[8] = -1;
	tbst[9] = -1;
	tbst[10] = -1;
#endif
	tmu_spin_lock_release(&tmu_lock, flags);
}

void tmu_egress_port_enable(const uint32_t epn, bool ena)
{
	/*
	   EPMT[epn].EPE = ena;
	   Implementation requirement
	   Must do both writes below in the given sequence:
	   1.
	   EPMTR  = 1 << TMU_EPMT_EPE;
	   EPMTR |= (SBID & TMU_EPMT_SBID__MSK) << TMU_EPMT_SBID;
	   TMU_REG_WR(EPMT,EPMTR);
	   TMU_REG_WR(EPMTC,(0x1 << TMU_EPMTC_EMW) | EPN);
	   2.
	   CFGCMD =
	   (TMU_CFGCMD_CMD_EP_ON__VAL &
	   TMU_CFGCMD_CMD__MSK)<<TMU_CFGCMD_CMD;
	   CFGCMD |= (EPN & TMU_CFGEPN_EPN__MSK) << TMU_CFGEPN_EPN;
	   TMU_REG_WR(CFGCMD, CFGCMD);
	   Direct write of EPMT[epn].EPE is not
	   recommended during operation
	 */
	uint32_t cfgcmd = 0;
	uint32_t old_epe, sbid;
	cfgcmd |= ena ? TMU_CFGCMD_CMD_EP_ON : TMU_CFGCMD_CMD_EP_OFF;
	set_val(cfgcmd, epn, TMU_CFGEPN_EPN_MASK, TMU_CFGEPN_EPN_OFFSET);
	tmu_epmt_read(epn, &old_epe, &sbid);
	tmu_epmt_write(epn, ena, sbid);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_cfgcmd_write(cfgcmd);
#else
#endif
}
EXPORT_SYMBOL(tmu_egress_port_enable);

bool tmu_is_egress_port_enabled(const uint32_t epn)
{
	uint32_t epe, sbid;
	tmu_epmt_read(epn, &epe, &sbid);
	return epe ? true : false;
}
EXPORT_SYMBOL(tmu_is_egress_port_enabled);

void tmu_egress_port_link_set(const uint32_t epn, uint32_t sbid)
{
	/*
	   EPMT[epn].SBID = sbid;
	 */
	uint32_t epe, old_sbid;
	tmu_epmt_read(epn, &epe, &old_sbid);
	tmu_epmt_write(epn, epe, sbid);
}
EXPORT_SYMBOL(tmu_egress_port_link_set);

void tmu_egress_port_link_get(const uint32_t epn,
			      struct tmu_eport_link *eport_link)
{
	uint32_t epe, sbid;
	tmu_epmt_read(epn, &epe, &sbid);
	eport_link->epe = epe;
	eport_link->sbid = sbid;
}
EXPORT_SYMBOL(tmu_egress_port_link_get);

void tmu_egress_port_tail_drop_thr_set(uint32_t epn,
				       struct tmu_egress_port_thr *epth)
{
	uint32_t ept[2] = { 0, 0 };
	set_val(ept[0], epth->epth[0], TMU_EPTT0_EPTH0_MASK,
		TMU_EPTT0_EPTH0_OFFSET);
	set_val(ept[0], epth->epth[1], TMU_EPTT0_EPTH1_MASK,
		TMU_EPTT0_EPTH1_OFFSET);
	set_val(ept[1], epth->epth[2], TMU_EPTT1_EPTH2_MASK,
		TMU_EPTT1_EPTH2_OFFSET);
	set_val(ept[1], epth->epth[3], TMU_EPTT1_EPTH3_MASK,
		TMU_EPTT1_EPTH3_OFFSET);
	tmu_eptt_write(epn, &ept[0]);
}
EXPORT_SYMBOL(tmu_egress_port_tail_drop_thr_set);

void tmu_egress_port_tail_drop_thr_get(uint32_t epn,
				       struct tmu_egress_port_thr *epth)
{
	uint32_t ept[2];
	tmu_eptt_read(epn, &ept[0]);
	epth->epth[0] =
	    get_val(ept[0], TMU_EPTT0_EPTH0_MASK, TMU_EPTT0_EPTH0_OFFSET);
	epth->epth[1] =
	    get_val(ept[0], TMU_EPTT0_EPTH1_MASK, TMU_EPTT0_EPTH1_OFFSET);
	epth->epth[2] =
	    get_val(ept[1], TMU_EPTT1_EPTH2_MASK, TMU_EPTT1_EPTH2_OFFSET);
	epth->epth[3] =
	    get_val(ept[1], TMU_EPTT1_EPTH3_MASK, TMU_EPTT1_EPTH3_OFFSET);
}
EXPORT_SYMBOL(tmu_egress_port_tail_drop_thr_get);

void tmu_egress_port_fill_status_get(uint32_t *epfill)
{
	uint32_t i;

	for (i = 0; i < 3; i++)
		epfill[i] = tmu_r32(epfr[i]);
}
EXPORT_SYMBOL(tmu_egress_port_fill_status_get);

void tmu_egress_port_seg_occupancy_get(uint32_t epn, uint32_t *epoc)
{
	uint32_t epot[2];
	tmu_epot_read(epn, &epot[0]);
	epoc[0] =
	    get_val(epot[0], TMU_EPOT0_EPOC0_MASK, TMU_EPOT0_EPOC0_OFFSET);
	epoc[1] =
	    get_val(epot[0], TMU_EPOT0_EPOC1_MASK, TMU_EPOT0_EPOC1_OFFSET);
	epoc[2] =
	    get_val(epot[1], TMU_EPOT1_EPOC2_MASK, TMU_EPOT1_EPOC2_OFFSET);
	epoc[3] =
	    get_val(epot[1], TMU_EPOT1_EPOC3_MASK, TMU_EPOT1_EPOC3_OFFSET);
}
EXPORT_SYMBOL(tmu_egress_port_seg_occupancy_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_egress_port_status_get(uint32_t epn,
				struct tmu_egress_port_status *epstatus)
{
	uint32_t epn_reg;
	uint32_t epn_bit;
	epn_reg = epn / 32;
	epn_bit = 1 << (epn % 32);
	epstatus->epfilled = tmu_r32(epfr[epn_reg]) & epn_bit ? true : false;
	tmu_egress_port_seg_occupancy_get(epn, &epstatus->epoc[0]);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_sched_blk_in_enable(const uint32_t sbin, bool ena)
{
	/*
	   SBIT[sbin].SIE = ena;
	   Implementation requirement
	   MUST use CMD opcode 0 in CFGCMD to do this
	   Direct write of SBIT[sbin].SIE is not recommended during operation
	 */
	uint32_t cfgcmd = 0;
	cfgcmd |=
	    ena ? TMU_CFGCMD_CMD_SB_INPUT_ON : TMU_CFGCMD_CMD_SB_INPUT_OFF;
	set_val(cfgcmd, sbin, TMU_CFGSBIN_SBIN_MASK, TMU_CFGSBIN_SBIN_OFFSET);
	tmu_cfgcmd_write(cfgcmd);
}
EXPORT_SYMBOL(tmu_sched_blk_in_enable);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
bool tmu_is_sched_blk_in_enabled(const uint32_t sbin)
{
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);
	return (sbit[0] & TMU_SBITR0_SIE_EN) ? true : false;
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_sched_blk_in_link_set(const uint32_t sbin,
			       struct tmu_sched_blk_in_link *ilink)
{
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);

	if (ilink->sie == 1)
		sbit[0] |= TMU_SBITR0_SIE_EN;
	else
		sbit[0] &= ~TMU_SBITR0_SIE_EN;

	if (ilink->sit == 1)
		sbit[0] |= TMU_SBITR0_SIT_SBID;
	else
		sbit[0] &= ~TMU_SBITR0_SIT_SBID;

	set_val(sbit[0], ilink->iwgt, TMU_SBITR0_IWGT_MASK,
		TMU_SBITR0_IWGT_OFFSET);
	set_val(sbit[0], ilink->qsid, TMU_SBITR0_QSID_MASK,
		TMU_SBITR0_QSID_OFFSET);
	tmu_sbit_write(sbin, &sbit[0]);
}
EXPORT_SYMBOL(tmu_sched_blk_in_link_set);

void tmu_sched_blk_in_link_get(const uint32_t sbin,
			       struct tmu_sched_blk_in_link *ilink)
{
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);
	ilink->sie = (sbit[0] & TMU_SBITR0_SIE_EN) ? 1 : 0;
	ilink->sit = (sbit[0] & TMU_SBITR0_SIT_SBID) ? 1 : 0;
	ilink->iwgt =
	    get_val(sbit[0], TMU_SBITR0_IWGT_MASK, TMU_SBITR0_IWGT_OFFSET);
	ilink->qsid =
	    get_val(sbit[0], TMU_SBITR0_QSID_MASK, TMU_SBITR0_QSID_OFFSET);
	/*sgh add below two line to return tbid and tbe also */
	ilink->tbid =
	    get_val(sbit[1], TMU_SBITR1_TBID_MASK, TMU_SBITR1_TBID_OFFSET);
	ilink->tbe = sbit[1] & TMU_SBITR1_TBE ? 1 : 0;
}
EXPORT_SYMBOL(tmu_sched_blk_in_link_get);

void tmu_sched_blk_in_weight_set(uint32_t sbin, uint16_t weight)
{
	/*
	   Implementation recommendation
	   First set CMDARG0.IWGT = weight
	   Then use CMD opcode 2 in CFGCMD to do this
	   Direct write of SBIT[sbin].IWGT is possible, but not recommended
	   during operation
	 */
	uint32_t cfgarg0;
	uint32_t cfgcmd;
	cfgarg0 = 0;
	set_val(cfgarg0, weight, TMU_CFGARG0_IWGT_MASK,
		TMU_CFGARG0_IWGT_OFFSET);
	tmu_w32(cfgarg0, cfgarg0);
	cfgcmd = 0;
	cfgcmd |= TMU_CFGCMD_CMD_SB_INPUT_WEIGHT;
	set_val(cfgcmd, sbin, TMU_CFGSBIN_SBIN_MASK, TMU_CFGSBIN_SBIN_OFFSET);
	tmu_w32(cfgcmd, cfgcmd);
}
EXPORT_SYMBOL(tmu_sched_blk_in_weight_set);

void tmu_sched_blk_in_weight_get(uint32_t sbin, uint16_t *weight)
{
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);
	*weight =
	    get_val(sbit[0], TMU_SBITR0_IWGT_MASK, TMU_SBITR0_IWGT_OFFSET);
}
EXPORT_SYMBOL(tmu_sched_blk_in_weight_get);

void tmu_sched_blk_in_weights_set(uint32_t sbid,
				  struct tmu_sched_blk_in_weights *weights)
{
	uint32_t leaf;
	uint32_t sbin;

	for (leaf = 0; leaf < 8; leaf++) {
		sbin = (sbid << 3) + leaf;
		tmu_sched_blk_in_weight_set(sbin, weights->iwgt[leaf]);
	}
}
EXPORT_SYMBOL(tmu_sched_blk_in_weights_set);

void tmu_sched_blk_in_weights_get(uint32_t sbid,
				  struct tmu_sched_blk_in_weights *weights)
{
	uint32_t leaf;
	uint32_t sbin;
	uint16_t weight;

	for (leaf = 0; leaf < 8; leaf++) {
		sbin = (sbid << 3) + leaf;
		tmu_sched_blk_in_weight_get(sbin, &weight);
		weights->iwgt[leaf] = weight;
	}
}
EXPORT_SYMBOL(tmu_sched_blk_in_weights_get);

void tmu_sched_blk_in_shaper_assign_set(const uint32_t sbin,
					struct tmu_sched_blk_in_tbs *tbs)
{
	/*
	   static configuration before activation:
	   SBIT[sbin].TBE =  tbs->tbe;
	   SBIT[sbin].TBID = tbs->tbid;
	   dynamic configuration after activation:
	   First set CMDARG1.TBID = tbid
	   Then use CMD opcode 3 in CFGCMD to do this
	 */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	uint32_t cmdcfg = 0;
	uint32_t cfgarg1 = 0;
	set_val(cfgarg1, tbs->tbid, TMU_CFGARG1_TBID_MASK,
		TMU_CFGARG1_TBID_OFFSET);
	tmu_w32(cfgarg1, cfgarg1);
	cmdcfg |= TMU_CFGCMD_CMD_SB_INPUT_BUCKET_SET;
	set_val(cmdcfg, sbin, TMU_CFGSBIN_SBIN_MASK, TMU_CFGSBIN_SBIN_OFFSET);
	tmu_cfgcmd_write(cmdcfg);
	cmdcfg = 0;
	cmdcfg |=
	    tbs->
	    tbe ? TMU_CFGCMD_CMD_SB_INPUT_BUCKET_ON :
	    TMU_CFGCMD_CMD_SB_INPUT_BUCKET_OFF;
	set_val(cmdcfg, sbin, TMU_CFGSBIN_SBIN_MASK, TMU_CFGSBIN_SBIN_OFFSET);
	tmu_cfgcmd_write(cmdcfg);
	/*
	   SGH ADD comments: very speical handling
	   here for TMU's cfgcmd register
	 */
#else

	if (tbs->tbe)
		simu_sbitr1[sbin] |= TMU_SBITR1_TBE_EN;
	else
		simu_sbitr1[sbin] ^= TMU_SBITR1_TBE_EN;

	simu_sbitr1[sbin] ^= 0xFF;
	simu_sbitr1[sbin] |= (tbs->tbid & 0xFF);
#endif
}
EXPORT_SYMBOL(tmu_sched_blk_in_shaper_assign_set);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_sched_blk_in_shaper_assign_get(const uint32_t sbin,
					struct tmu_sched_blk_in_tbs *tbs)
{
	/*
	   tbs->tbe =  SBIT[sbin].TBE;
	   tbs->tbid = SBIT[sbin].TBID;
	 */
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);
	tbs->tbe = (sbit[1] & TMU_SBITR1_TBE_EN) ? 1 : 0;
	tbs->tbid =
	    get_val(sbit[1], TMU_SBITR1_TBID_MASK, TMU_SBITR1_TBID_OFFSET);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_sched_blk_in_shaper_enable(const uint32_t sbin, bool ena)
{
	/*
	   SBIT[sbin].TBE = ena;
	   Implementation requirement
	   MUST use CMD opcode 4 in CFGCMD to do this
	   Direct write of SBIT[sbin].TBE is not recommended
	   during operation
	 */
	uint32_t cmdcfg = 0;
	cmdcfg |=
	    ena ==
	    true ? TMU_CFGCMD_CMD_SB_INPUT_BUCKET_ON :
	    TMU_CFGCMD_CMD_SB_INPUT_BUCKET_OFF;
	set_val(cmdcfg, sbin, TMU_CFGSBIN_CMDSBIN_MASK,
		TMU_CFGSBIN_CMDSBIN_OFFSET);
	tmu_cfgcmd_write(cmdcfg);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
bool tmu_is_sched_blk_in_shaper_enabled(const uint32_t sbin)
{
	/*ena = SBIT[sbin].TBE; */
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);
	return (sbit[1] & TMU_SBITR1_TBE) ? true : false;
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_sched_blk_in_status_get(const uint32_t sbin,
				 struct tmu_sched_blk_in_status *istatus)
{
	/*t.b.d. */
	uint32_t sbit[4];
	tmu_sbit_read(sbin, &sbit[0]);
	istatus->sif = (sbit[2] & TMU_SBITR2_SIF_FIL) ? true : false;
	istatus->sip = (sbit[2] & TMU_SBITR2_SIP_SIP_1) ? true : false;
	istatus->vdt =
	    get_val(sbit[2], TMU_SBITR2_VDT_MASK, TMU_SBITR2_VDT_OFFSET);
	istatus->col =
	    get_val(sbit[3], TMU_SBITR3_COL_MASK, TMU_SBITR3_COL_OFFSET);
	istatus->qosl =
	    get_val(sbit[3], TMU_SBITR3_QOSL_MASK, TMU_SBITR3_QOSL_OFFSET);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_sched_blk_out_enable(const uint32_t sbid, bool ena)
{
	/*
	   SBOT[sbid].SOE = ena;
	   Implementation requirement
	   MUST use CMD opcode 6 in \ref cfgcmd to do this
	   Direct write of SBOT[sbid].SOE
	   is not recommended during operation
	 */

	/*following code does not work: */
	/*
	   uint32_t cmdcfg = 0;
	   if (ena == true)
	   cmdcfg |= TMU_CFGCMD_CMD_SB_OUTPUT_ON;
	   else
	   cmdcfg |= TMU_CFGCMD_CMD_SB_OUTPUT_OFF;
	   set_val(cmdcfg, sbid, TMU_CFGSBIN_SBIN_MASK,
	   TMU_CFGSBIN_SBIN_OFFSET);
	   tmu_cfgcmd_write(cmdcfg);
	 */

	/* above code replaced by direct SBOT write: */
	uint32_t sbot[1];
	tmu_sbot_read_cfg(sbid, &sbot[0]);

	if (ena == 1)
		sbot[0] |= TMU_SBOTR0_SOE_EN;
	else
		sbot[0] &= ~TMU_SBOTR0_SOE_EN;

	tmu_sbot_write_cfg(sbid, &sbot[0]);
}
EXPORT_SYMBOL(tmu_sched_blk_out_enable);

bool tmu_is_sched_blk_out_enabled(const uint32_t sbid)
{
	/*ena = SBOT[sbid].SOE; */
	uint32_t sbot[2];
	tmu_sbot_read(sbid, &sbot[0]);
	return (sbot[0] & TMU_SBOTR0_SOE_EN) ? true : false;
}
EXPORT_SYMBOL(tmu_is_sched_blk_out_enabled);

void tmu_sched_blk_out_link_set(const uint32_t sbid,
				struct tmu_sched_blk_out_link *olink)
{
	uint32_t sbot[2];
	tmu_sbot_read_cfg(sbid, &sbot[0]);

	if (olink->soe == 1)
		sbot[0] |= TMU_SBOTR0_SOE_EN;
	else
		sbot[0] &= ~TMU_SBOTR0_SOE_EN;

	if (olink->v == 1)
		sbot[0] |= TMU_SBOTR0_V_SBIN;
	else
		sbot[0] &= ~TMU_SBOTR0_V_SBIN;

	set_val(sbot[0], olink->lvl, TMU_SBOTR0_LVL_MASK,
		TMU_SBOTR0_LVL_OFFSET);
	set_val(sbot[0], olink->omid, TMU_SBOTR0_OMID_MASK,
		TMU_SBOTR0_OMID_OFFSET);
	tmu_sbot_write_cfg(sbid, &sbot[0]);
}
EXPORT_SYMBOL(tmu_sched_blk_out_link_set);

void tmu_sched_blk_out_link_get(const uint32_t sbid,
				struct tmu_sched_blk_out_link *olink)
{
	uint32_t sbot[2];
	tmu_sbot_read_cfg(sbid, &sbot[0]);
	olink->soe = (sbot[0] & TMU_SBOTR0_SOE_EN) ? true : false;
	olink->v = (sbot[0] & TMU_SBOTR0_V_SBIN) ? true : false;
	olink->lvl =
	    get_val(sbot[0], TMU_SBOTR0_LVL_MASK, TMU_SBOTR0_LVL_OFFSET);
	olink->omid =
	    get_val(sbot[0], TMU_SBOTR0_OMID_MASK, TMU_SBOTR0_OMID_OFFSET);
}
EXPORT_SYMBOL(tmu_sched_blk_out_link_get);

void tmu_sched_blk_out_status_get(const uint32_t sbid,
				  struct tmu_sched_blk_out_status *ostatus)
{
	uint32_t sbot[2];
	tmu_sbot_read(sbid, &sbot[0]);
	ostatus->sof = (sbot[1] & TMU_SBOTR1_SOF_FIL) ? true : false;
	ostatus->wl =
	    get_val(sbot[1], TMU_SBOTR1_WL_MASK, TMU_SBOTR1_WL_OFFSET);
	ostatus->wqid =
	    get_val(sbot[1], TMU_SBOTR1_WQID_MASK, TMU_SBOTR1_WQID_OFFSET);
}
EXPORT_SYMBOL(tmu_sched_blk_out_status_get);

void tmu_token_bucket_shaper_link_set(const uint32_t tbid,
				      const uint32_t sbin)
{
	uint32_t tbst[11] = { 0 };
	tmu_tbst_read(tbid, &tbst[0]);
	set_val(tbst[0], sbin, TMU_TBSTR0_SBIN_MASK, TMU_TBSTR0_SBIN_OFFSET);
	tmu_tbst_write(tbid, &tbst[0]);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_link_set);

void tmu_token_bucket_shaper_link_get(const uint32_t tbid, uint32_t *sbin)
{
	uint32_t tbst[11] = { 0 };
	tmu_tbst_read(tbid, &tbst[0]);
	*sbin =
	    get_val(tbst[0], TMU_TBSTR0_SBIN_MASK, TMU_TBSTR0_SBIN_OFFSET);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_link_get);

void tmu_token_bucket_shaper_cfg_set(const uint32_t tbid,
				     struct tmu_token_bucket_shaper_params
				     *para)
{
	/*
	   TBST[tbid].MOD  =  para->mod;
	   TBST[tbid].TBE0 =  para->tbe0;
	   TBST[tbid].TBU0 =  tmu_tbs_tbu_conversion (para->cir);
	   TBST[tbid].SRM0 =  tmu_tbs_srm_conversion (para->cir);
	   TBST[tbid].TBE1 =  para->tbe1;
	   TBST[tbid].TBU1 =  tmu_tbs_tbu_conversion (para->pir);
	   TBST[tbid].SRM1 =  tmu_tbs_srm_conversion (para->pir);
	   TBST[tbid].MBS0 =  para->cbs;
	   TBST[tbid].MBS1 =  para->pbs;
	 */
	uint32_t tbst[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t tbu0, tbu1, srm0, srm1;
	tmu_tbst_read_cfg(tbid, &tbst[0]);
	set_val(tbst[0], para->mod, TMU_TBSTR0_MOD_MASK,
		TMU_TBSTR0_MOD_OFFSET);

	if (para->tbe0)
		tbst[1] |= TMU_TBSTR1_TBE0_EN;
	else
		tbst[1] &= ~(TMU_TBSTR1_TBE0_EN);

	tbu0 = tmu_tbs_tbu_conversion(para->cir);
	set_val(tbst[1], tbu0, TMU_TBSTR1_TBU0_MASK, TMU_TBSTR1_TBU0_OFFSET);
	srm0 = tmu_tbs_srm_conversion(para->cir);
	set_val(tbst[1], srm0, TMU_TBSTR1_SRM0_MASK, TMU_TBSTR1_SRM0_OFFSET);

	if (para->tbe1)
		tbst[2] |= TMU_TBSTR2_TBE1_EN;
	else
		tbst[2] &= ~(TMU_TBSTR2_TBE1_EN);

	tbu1 = tmu_tbs_tbu_conversion(para->pir);
	set_val(tbst[2], tbu1, TMU_TBSTR2_TBU1_MASK, TMU_TBSTR2_TBU1_OFFSET);
	srm1 = tmu_tbs_srm_conversion(para->pir);
	set_val(tbst[2], srm1, TMU_TBSTR2_SRM1_MASK, TMU_TBSTR2_SRM1_OFFSET);
	set_val(tbst[3], para->cbs, TMU_TBSTR3_MBS0_MASK,
		TMU_TBSTR3_MBS0_OFFSET);
	set_val(tbst[4], para->pbs, TMU_TBSTR4_MBS1_MASK,
		TMU_TBSTR4_MBS1_OFFSET);
	tmu_tbst_write_cfg(tbid, &tbst[0]);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_cfg_set);

void tmu_token_bucket_shaper_cfg_get(const uint32_t tbid,
				     struct tmu_token_bucket_shaper_params
				     *para)
{
	uint32_t tbst[11];
	uint32_t tbu0, tbu1, srm0, srm1;
	tmu_tbst_read_cfg(tbid, &tbst[0]);
	tbu0 = get_val(tbst[1], TMU_TBSTR1_TBU0_MASK, TMU_TBSTR1_TBU0_OFFSET);
	tbu1 = get_val(tbst[2], TMU_TBSTR2_TBU1_MASK, TMU_TBSTR2_TBU1_OFFSET);
	srm0 = get_val(tbst[1], TMU_TBSTR1_SRM0_MASK, TMU_TBSTR1_SRM0_OFFSET);
	srm1 = get_val(tbst[2], TMU_TBSTR2_SRM1_MASK, TMU_TBSTR2_SRM1_OFFSET);
	para->mod =
	    get_val(tbst[0], TMU_TBSTR0_MOD_MASK, TMU_TBSTR0_MOD_OFFSET);
	para->tbe0 = (tbst[1] & TMU_TBSTR1_TBE0_EN) ? true : false;
	para->cir = tmu_tbs_rate_conversion(tbu0, srm0);
	para->cbs =
	    get_val(tbst[3], TMU_TBSTR3_MBS0_MASK, TMU_TBSTR3_MBS0_OFFSET);
	para->tbe1 = (tbst[2] & TMU_TBSTR2_TBE1_EN) ? true : false;
	para->pir = tmu_tbs_rate_conversion(tbu1, srm1);
	para->pbs =
	    get_val(tbst[4], TMU_TBSTR4_MBS1_MASK, TMU_TBSTR4_MBS1_OFFSET);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_cfg_get);

uint32_t tmu_tbs_tbu_conversion(uint32_t byterate)
{
	uint32_t kbitrate = (byterate << 3) / 1000;

	if (kbitrate <= 500)
		return 0;
	else if (kbitrate <= 8000)
		return 1;
	else if (kbitrate <= 128000)
		return 2;
	else
		return 3;
}

uint32_t tmu_tbs_srm_conversion(uint32_t byterate)
{
	uint32_t tbu;
	uint32_t tbu_exp;
	uint32_t kbitrate = (byterate << 3) / 1000;

	/*sgh:comments here: if byterate == 0, it will cause crash */
	if (byterate == 0)
		TMU_PRINT("Wrong. byterate cannot be zero ??\n");

	tbu = tmu_tbs_tbu_conversion(byterate);

	switch (tbu) {
	case 0:
		tbu_exp = 1;
		break;

	case 1:
		tbu_exp = 16;
		break;

	case 2:
		tbu_exp = 256;
		break;

	case 3:
		tbu_exp = 1024;
		break;

	default:
		tbu_exp = 256;
		break;
	}

	return (1000 * TMU_CORECLOCK * tbu_exp) / (kbitrate << 1);
}

uint32_t tmu_tbs_rate_conversion(uint32_t tbu, uint32_t srm)
{
	uint32_t tbu_exp;
	uint32_t kbitrate;

	switch (tbu) {
	case 0:
		tbu_exp = 1;
		break;

	case 1:
		tbu_exp = 16;
		break;

	case 2:
		tbu_exp = 256;
		break;

	case 3:
		tbu_exp = 1024;
		break;

	default:
		tbu_exp = 256;
		break;
	}

	kbitrate = (1000 * TMU_CORECLOCK * tbu_exp) / (srm << 1);
	return (kbitrate * 1000) >> 3;
}

void tmu_token_bucket_shaper_status_get(const uint32_t tbid,
					struct tmu_token_bucket_shaper_status
					*sts)
{
	uint32_t tbst[11];
	tmu_tbst_read(tbid, &tbst[0]);
	sts->pass0 = (tbst[7] & TMU_TBSTR7_PASS0) ? true : false;
	sts->src0 =
	    get_val(tbst[5], TMU_TBSTR5_SRC0_MASK, TMU_TBSTR5_SRC0_OFFSET);
	sts->tbc0 =
	    get_val(tbst[7], TMU_TBSTR7_TBC0_MASK, TMU_TBSTR7_TBC0_OFFSET);
	sts->pass1 = (tbst[8] & TMU_TBSTR8_PASS1) ? true : false;
	sts->src1 =
	    get_val(tbst[6], TMU_TBSTR6_SRC1_MASK, TMU_TBSTR6_SRC1_OFFSET);
	sts->tbc1 =
	    get_val(tbst[8], TMU_TBSTR8_TBC1_MASK, TMU_TBSTR8_TBC1_OFFSET);
	sts->qosl =
	    get_val(tbst[9], TMU_TBSTR9_QOSL_MASK, TMU_TBSTR9_QOSL_OFFSET);
	sts->col =
	    get_val(tbst[9], TMU_TBSTR9_COL_MASK, TMU_TBSTR9_COL_OFFSET);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_status_get);

void tmu_global_tail_drop_thr_set(struct tmu_global_thr *thx)
{
	uint32_t i;

	for (i = 0; i < 4; i++)
		tmu_w32(thx->goth[i], gothr[i]);
}
EXPORT_SYMBOL(tmu_global_tail_drop_thr_set);

void tmu_global_tail_drop_thr_get(struct tmu_global_thr *thx)
{
	uint32_t i;

	for (i = 0; i < 4; i++)
		thx->goth[i] = tmu_r32(gothr[i]);
}
EXPORT_SYMBOL(tmu_global_tail_drop_thr_get);

void tmu_token_accumulation_disable(bool dta)
{
	tmu_w32_mask(TMU_CTRL_DTA_DTA1, dta ? TMU_CTRL_DTA_DTA1 : 0, ctrl);
}
EXPORT_SYMBOL(tmu_token_accumulation_disable);

bool tmu_is_token_accumulation_disabled(void)
{
	return (tmu_r32(ctrl) & TMU_CTRL_DTA_DTA1) ? true : false;
}
EXPORT_SYMBOL(tmu_is_token_accumulation_disabled);

void tmu_relog_sequential(bool rps)
{
	tmu_w32_mask(TMU_CTRL_RPS_RPS1, rps ? TMU_CTRL_RPS_RPS1 : 0, ctrl);
}

bool tmu_is_relog_sequential(void)
{
	return (tmu_r32(ctrl) & TMU_CTRL_RPS_RPS1) ? true : false;
}

void tmu_max_token_bucket_set(uint32_t maxtb)
{
	uint32_t ctrl;
	ctrl = tmu_r32(ctrl);
	set_val(ctrl, maxtb, TMU_CTRL_MAXTB_MASK, TMU_CTRL_MAXTB_OFFSET);
	tmu_w32(ctrl, ctrl);
}
EXPORT_SYMBOL(tmu_max_token_bucket_set);

void tmu_max_token_bucket_get(uint32_t *maxtb)
{
	uint32_t ctrl;
	ctrl = tmu_r32(ctrl);
	*maxtb = get_val(ctrl, TMU_CTRL_MAXTB_MASK, TMU_CTRL_MAXTB_OFFSET);
}
EXPORT_SYMBOL(tmu_max_token_bucket_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_free_pointer_counter_set(uint32_t fpc)
{
	uint32_t fpcr;
	fpcr = tmu_r32(fpcr);
	set_val(fpcr, fpc, TMU_FPCR_FPC_MASK, TMU_FPCR_FPC_OFFSET);
	tmu_w32(fpcr, fpcr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_free_pointer_counter_get(uint32_t *fpc)
{
	uint32_t fpcr;
	fpcr = tmu_r32(fpcr);
	*fpc = get_val(fpcr, TMU_FPCR_FPC_MASK, TMU_FPCR_FPC_OFFSET);
}
EXPORT_SYMBOL(tmu_free_pointer_counter_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_free_pointer_thr_set(uint32_t fpth)
{
	uint32_t fpthr;
	fpthr = tmu_r32(fpthr);
	set_val(fpthr, fpth, TMU_FPTHR_FPTH_MASK, TMU_FPTHR_FPTH_OFFSET);
	tmu_w32(fpthr, fpthr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_free_pointer_thr_get(uint32_t *fpth)
{
	uint32_t fpthr;
	fpthr = tmu_r32(fpthr);
	*fpth = get_val(fpthr, TMU_FPTHR_FPTH_MASK, TMU_FPTHR_FPTH_OFFSET);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_crawler_period_set(uint32_t cp)
{
	uint32_t cpr;
	cpr = tmu_r32(cpr);
	set_val(cpr, cp, TMU_CPR_CP_MASK, TMU_CPR_CP_OFFSET);
	tmu_w32(cpr, cpr);
}
EXPORT_SYMBOL(tmu_crawler_period_set);

void tmu_crawler_period_get(uint32_t *cp)
{
	uint32_t cpr;
	cpr = tmu_r32(cpr);
	*cp = get_val(cpr, TMU_CPR_CP_MASK, TMU_CPR_CP_OFFSET);
}
EXPORT_SYMBOL(tmu_crawler_period_get);

void tmu_random_number_set(uint32_t rn)
{
	uint32_t lfsr;
	lfsr = tmu_r32(lfsr);
	set_val(lfsr, rn, TMU_LFSR_RN_MASK, TMU_LFSR_RN_OFFSET);
	tmu_w32(lfsr, lfsr);
}
EXPORT_SYMBOL(tmu_random_number_set);

void tmu_random_number_get(uint32_t *rn)
{
	uint32_t lfsr;
	lfsr = tmu_r32(lfsr);
	*rn = get_val(lfsr, TMU_LFSR_RN_MASK, TMU_LFSR_RN_OFFSET);
}
EXPORT_SYMBOL(tmu_random_number_get);

void tmu_enqueue_delay_set(uint32_t erd)
{
	uint32_t erdr;
	erdr = tmu_r32(erdr);
	set_val(erdr, erd, TMU_ERDR_ERD_MASK, TMU_ERDR_ERD_OFFSET);
	tmu_w32(erdr, erdr);
}
EXPORT_SYMBOL(tmu_enqueue_delay_set);

void tmu_enqueue_delay_get(uint32_t *erd)
{
	uint32_t erdr;
	erdr = tmu_r32(erdr);
	*erd = get_val(erdr, TMU_ERDR_ERD_MASK, TMU_ERDR_ERD_OFFSET);
}
EXPORT_SYMBOL(tmu_enqueue_delay_get);

void tmu_tacc_period_set(uint32_t tacp)
{
	uint32_t tacper;
	tacper = tmu_r32(tacper);
	set_val(tacper, tacp, TMU_TACPER_TACP_MASK, TMU_TACPER_TACP_OFFSET);
	tmu_w32(tacper, tacper);
}
EXPORT_SYMBOL(tmu_tacc_period_set);

void tmu_tacc_period_get(uint32_t *tacp)
{
	uint32_t tacper;
	tacper = tmu_r32(tacper);
	*tacp = get_val(tacper, TMU_TACPER_TACP_MASK, TMU_TACPER_TACP_OFFSET);
}
EXPORT_SYMBOL(tmu_tacc_period_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_global_occupancy_set(uint32_t gocc)
{
	uint32_t goccr;
	goccr = tmu_r32(goccr);
	set_val(goccr, gocc, TMU_GOCCR_GOCC_MASK, TMU_GOCCR_GOCC_OFFSET);
	tmu_w32(goccr, goccr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_global_occupancy_get(uint32_t *gocc)
{
	uint32_t goccr;
	goccr = tmu_r32(goccr);
	*gocc = get_val(goccr, TMU_GOCCR_GOCC_MASK, TMU_GOCCR_GOCC_OFFSET);
}
EXPORT_SYMBOL(tmu_global_occupancy_get);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_global_discard_counters_set(uint32_t *gpdc)
{
	uint32_t i;

	for (i = 0; i < 4; i++)
		tmu_w32(gpdc[i], gpdcr[i]);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_global_discard_counters_get(uint32_t *gpdc)
{
	uint32_t i;

	for (i = 0; i < 4; i++)
		gpdc[i] = tmu_r32(gpdcr[i]);
}
EXPORT_SYMBOL(tmu_global_discard_counters_get);

void tmu_interrupt_enable_set(const uint32_t mask_clr,
			      const uint32_t mask_set)
{
	tmu_w32_mask(mask_clr, mask_set, irnen);
}

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
uint32_t tmu_interrupt_enable_get(void)
{
	return tmu_r32(irnen);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_interrupt_control_set(const uint32_t ctrl)
{
	tmu_w32(ctrl, irnicr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_interrupt_control_get(uint32_t *ctrl)
{
	*ctrl = tmu_r32(irnicr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_interrupt_capture_set(const uint32_t capt)
{
	tmu_w32(capt, irncr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_interrupt_capture_get(uint32_t *capt)
{
	*capt = tmu_r32(irncr);
}
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/*=============================================================
High Level Functions
If V = 1, it identifies the SBIN of the next higher SB
(its value equal to sbid << 3 + leaf)
If V = 0, it identifies the EPN of the egress port (its value is 0 ~ 127)
=============================================================
*/

void tmu_sched_blk_create(const uint32_t sbid, const uint8_t lvl,
			  const uint32_t omid, const uint8_t v,
			  const uint16_t weight)
{
	/*
	   This function accesses
	   SBOT[sbid]
	   SBIT[omid] or EPMT[omid] depending on v
	 */
	struct tmu_sched_blk_out_link olink;
	struct tmu_sched_blk_in_link ilink;
	olink.soe = 1;		/* enable scheduler output */
	olink.lvl = lvl;
	olink.omid = omid;
	olink.v = v;
	tmu_sched_blk_out_link_set(sbid, &olink);

	if (v == 0)
		tmu_egress_port_link_set(omid, sbid);
	else {
		ilink.sie = 1;	/* enable next level input */
		ilink.sit = 1;
		ilink.qsid = sbid;
		ilink.iwgt = weight;
		tmu_sched_blk_in_link_set(omid, &ilink);
	}
}
EXPORT_SYMBOL(tmu_sched_blk_create);

/* The tmu_egress_queue_create function is used to assign the scheduler and
      egress port to an egress queue.
      sbin is sb(7bits) plus leaf(3bits)
*/
void tmu_egress_queue_create(uint32_t qid, const uint32_t sbin, uint32_t epn)
{
	/*
	   QEMT[qid].EPN  = epn;
	   QSMT[qid].SBIN = sbin;
	   QTHT[qid].QE   = 1;
	   SBIT[sbin].SIE = 1;
	 */
	struct tmu_equeue_link equeue_link;
	struct tmu_sched_blk_in_link sblink;
	equeue_link.sbin = sbin;
	equeue_link.epn = epn;
	tmu_equeue_link_set(qid, &equeue_link);
	tmu_equeue_enable(qid, 1);
	tmu_sched_blk_in_enable(sbin, 1);
	sblink.qsid = qid;
	sblink.iwgt = 0;
	sblink.sie = 1;		/*default enable sb/scheduler */
	sblink.sit = 0;		/*default is a queue id, not another schedule */
	sblink.tbe = 0;		/*sgh add this setting */
	sblink.tbid = TOKEN_BUCKET_MAX;	/*sgh add this setting */
	tmu_sched_blk_in_link_set(sbin, &sblink);
	tmu_reset_queue_threshold(qid);	/*sgh add this setting */
}
EXPORT_SYMBOL(tmu_egress_queue_create);

void tmu_token_bucket_shaper_create(const uint32_t tbid, const uint32_t sbin)
{
	/*
	   This function accesses
	   TBST[tbid].SBIN
	   SBIT[sbin].TBE
	   SBIT[sbin].TBID
	 */
	struct tmu_sched_blk_in_tbs tbs;
	tmu_token_bucket_shaper_link_set(tbid, sbin);
	tbs.tbe = 1;
	tbs.tbid = tbid;
	tmu_sched_blk_in_shaper_assign_set(sbin, &tbs);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_create);

void tmu_token_bucket_shaper_delete(const uint32_t tbid, const uint32_t sbin)
{
	/*
	   This function accesses
	   TBST[tbid].SBIN
	   SBIT[sbin].TBE
	   SBIT[sbin].TBID
	 */
	uint32_t tbst[11] = { 0 };
	uint32_t sbit[4] = { 0 };
	set_val(tbst[0], 0x3FF, TMU_TBSTR0_SBIN_MASK, TMU_TBSTR0_SBIN_OFFSET);
	tmu_sbit_read(sbin, &sbit[0]);
	sbit[1] &= (~TMU_SBITR1_TBE_EN);
	set_val(sbit[1], 0xFF, TMU_SBITR1_TBID_MASK, TMU_SBITR1_TBID_OFFSET);
	tmu_tbst_write(tbid, &tbst[0]);
	tmu_sbit_write(sbin, &sbit[0]);
}
EXPORT_SYMBOL(tmu_token_bucket_shaper_delete);

void tmu_create_flat_egress_path(const uint16_t num_ports,
				 const uint16_t base_epn,
				 const uint16_t base_sbid,
				 const uint16_t base_qid, uint16_t qid_per_sb)
{
	uint16_t epn;
	uint16_t qid;

	if (qid_per_sb > SCHEDULE_MAX_LEVEL) {
		TMU_PRINT
		    ("Wrong qid_per_sb settting(%d). It should be less than %d\n",
		     qid_per_sb, SCHEDULE_MAX_LEVEL);
		qid_per_sb = SCHEDULE_MAX_LEVEL;
	}

	for (epn = 0; epn < num_ports; epn++) {
		tmu_sched_blk_create(base_sbid + epn, 0, base_epn + epn, 0,
				     0);
		tmu_egress_port_enable(base_epn + epn, true);

		for (qid = epn * qid_per_sb;
		     qid < epn * qid_per_sb + qid_per_sb; qid++) {
			tmu_egress_queue_create(base_qid + qid,
						((base_sbid + epn) << 3) +
						(qid % 8), base_epn + epn);
		}
	}
}
EXPORT_SYMBOL(tmu_create_flat_egress_path);

#if defined(TMU_LOCAL_STAT_DEBUG)
void dumps8(struct seq_file *s, const char *s0, const char *s1,
	    const char *s2, const char *s3, const char *s4, const char *s5,
	    const char *s6, const char *s7)
{
	SEQ_PRINTF(s, "%8s,%8s,%8s,%8s,%8s,%8s,%8s,%8s\n", s0, s1, s2, s3, s4,
		   s5, s6, s7);
}

void dumpx8(struct seq_file *s, const uint32_t v0, const uint32_t v1,
	    const uint32_t v2, const uint32_t v3, const uint32_t v4,
	    const uint32_t v5, const uint32_t v6, const uint32_t v7)
{
	SEQ_PRINTF(s, "%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x\n", v0, v1, v2,
		   v3, v4, v5, v6, v7);
}

void dumps4(struct seq_file *s, const char *s0, const char *s1,
	    const char *s2, const char *s3)
{
	SEQ_PRINTF(s, "%8s,%8s,%8s,%8s\n", s0, s1, s2, s3);
}

void dumpx4(struct seq_file *s, const uint32_t v0, const uint32_t v1,
	    const uint32_t v2, const uint32_t v3)
{
	SEQ_PRINTF(s, "%08x,%08x,%08x,%08x\n", v0, v1, v2, v3);
}

void tmu_dump(struct seq_file *s)
{
	if (!tmu_is_enabled())
		SEQ_PRINTF(s, "TMU not activated\n");

	SEQ_PRINTF(s, "tmu_base=%p\n", (void *)TMU_BASE);
	dumps8(s, "ctrl", "fpcr", "fpthr", "timer", "lfsr", "cpr", "csr",
	       "goccr");
	dumpx8(s, tmu_r32(ctrl), tmu_r32(fpcr), tmu_r32(fpthr),
	       tmu_r32(timer), tmu_r32(lfsr), tmu_r32(cpr), tmu_r32(csr),
	       tmu_r32(goccr));
	dumps4(s, "gothr0", "gothr1", "gothr2", "gothr3");
	dumpx4(s, tmu_r32(gothr[0]), tmu_r32(gothr[1]), tmu_r32(gothr[2]),
	       tmu_r32(gothr[3]));
	dumps4(s, "gpdcr0", "gpdcr1", "gpdcr2", "gpdcr3");
	dumpx4(s, tmu_r32(gpdcr[0]), tmu_r32(gpdcr[1]), tmu_r32(gpdcr[2]),
	       tmu_r32(gpdcr[3]));
	dumps4(s, "lpic0", "lpic1", "lpic2", "lpic3");
	dumpx4(s, tmu_r32(lpic[0]), tmu_r32(lpic[1]), tmu_r32(lpic[2]),
	       tmu_r32(lpic[3]));
	dumps4(s, "lpit0", "lpit1", "lpit2", "lpit3");
	dumpx4(s, tmu_r32(lpit[0]), tmu_r32(lpit[1]), tmu_r32(lpit[2]),
	       tmu_r32(lpit[3]));
	dumps8(s, "qfill0", "qfill1", "qfill2", "qfill3", "qfill4", "qfill5",
	       "qfill6", "qfill7");
	dumpx8(s, tmu_r32(qfill[0]), tmu_r32(qfill[1]), tmu_r32(qfill[2]),
	       tmu_r32(qfill[3]), tmu_r32(qfill[4]), tmu_r32(qfill[5]),
	       tmu_r32(qfill[6]), tmu_r32(qfill[7]));
	dumps4(s, "epfr0", "epfr1", "epfr2", "tbidcr");
	dumpx4(s, tmu_r32(epfr[0]), tmu_r32(epfr[1]), tmu_r32(epfr[2]),
	       tmu_r32(tbidcr));
}

void tmu_eqt_dump(struct seq_file *s)
{
	uint32_t i;
	bool valid;

	if (!tmu_is_enabled())
		SEQ_PRINTF(s, "TMU not activated\n");

	SEQ_PRINTF(s, "Egress Queue Table (EQT)\n");
	SEQ_PRINTF(s,
		   "no ,epn     ,sbin    ,wq      ,qrth    ,qocc    "
		   ",qavg\n");
	SEQ_PRINTF(s, "no ,qtht0   ,qtht1   ,qtht2   ,qtht3   ,qtht4\n");
	SEQ_PRINTF(s,
		   "no ,qdc0    ,qdc1    ,qdc2    ,qdc3    ,qfmt0	,qfmt1	,qfmt2");

	for (i = 0; i < EGRESS_QUEUE_ID_MAX; i++) {
		uint32_t tmp[18], k;
		tmu_qemt_read(i, &tmp[0]);
		tmu_qsmt_read(i, &tmp[1]);
		tmu_qoct_read(i, &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
		tmu_qtht_read(i, &tmp[6]);
		tmu_qdct_read(i, &tmp[11]);
		tmu_qfmt_read(i, &tmp[15]);

		if (tmp[0] == EPNNULL_EGRESS_PORT_ID &&
		    tmp[1] == NULL_SCHEDULER_INPUT_ID)
			valid = false;
		else
			valid = true;

		SEQ_PRINTF(s, "\n%03d", i);

		for (k = 0; k < 6; k++)
			SEQ_PRINTF(s, ",%08x", tmp[k]);

		if (valid)
			SEQ_PRINTF(s, "          V");

		SEQ_PRINTF(s, "\n%03d", i);

		for (; k < 11; k++)
			SEQ_PRINTF(s, ",%08x", tmp[k]);

		if (valid)
			SEQ_PRINTF(s, "                   V");

		SEQ_PRINTF(s, "\n%03d", i);

		for (; k < 18; k++)
			SEQ_PRINTF(s, ",%08x", tmp[k]);

		if (valid)
			SEQ_PRINTF(s, " V");

		SEQ_PRINTF(s, "\n");
	}
}

/* flag:
	flag 0: print mib smartly for disabled queue (minimize the console log)
	flag 1: print mib for disabled queue
	Note: this API is shared cat proc and echo proc.
*/
void tmu_proc_eqt_print(struct seq_file *s, uint32_t index, int flag)
{
	int lvl = -1;
	int need_exit = 0;
	uint32_t qdc[4], enq_c, deq_c;
	struct tmu_equeue_link qlink;
	struct tmu_sched_blk_in_link sbit;
	struct tmu_sched_blk_out_link olink;
	uint32_t sbid, sbin, leaf;
	struct tmu_eport_link eport_link;
	struct tmu_token_bucket_shaper_params para;
	struct tmu_equeue_drop_params thx;
	uint32_t qocc, wq, qrth, qavg, fpth, fpc;
	struct tmu_egress_port_thr epth;
	char flag_buf[20];
	char *flag_s;

	tmu_memset(&epth, 0, sizeof(epth));
	g_queue_dump = 0;
	if (flag == PROC_EQT_MODE_MIB_ONLY) {
		enq_c = get_enq_counter(index);
		deq_c = get_deq_counter(index);
		tmu_equeue_drop_params_get(index, &thx);
		tmu_qdct_read(index, qdc);
		tmu_qoct_read(index, &wq, &qrth, &qocc, &qavg);
		tmu_equeue_link_get(index, &qlink);
		flag_buf[0] = 0;
		flag_s =
		    get_dma_flags_str(qlink.epn, flag_buf, sizeof(flag_buf));
		if ((enq_c || deq_c || (qdc[0] + qdc[1] + qdc[2] + qdc[3]))
		    || qocc || qavg) {
			if (!f_q_mib_title_proc) {
				TMU_PRINT
				    ("%15s %10s %10s %10s (%10s %10s %10s %10s) %10s %10s %10s\n",
				     "Queue MIB    ID", "enq", "deq", "drop",
				     "No-Color", "Green", "Yellow", "Red",
				     "qocc", "qavg", "  DMA  Flag");
				f_q_mib_title_proc = 1;
			}
			TMU_PRINT
			    ("%15d %10u %10u %10u (%10u %10u %10u %10u) %10u %10u %10s\n",
			     index, enq_c, deq_c,
			     (qdc[0] + qdc[1] + qdc[2] + qdc[3]), qdc[0],
			     qdc[1], qdc[2], qdc[3], qocc, (qavg >> 8),
			     flag_s ? flag_s : "");

		}
		return;
	} else if (flag == PROC_EQT_MODE_MIB_CLEAR) {
		tmu_reset_mib(index);
		return;
	}

	if (index >= EGRESS_QUEUE_ID_MAX) {
		tmu_proc_printf(s,
				"Not index value %u. It should less than %u\n",
				index, EGRESS_QUEUE_ID_MAX);
		return;
	}

	tmu_equeue_link_get(index, &qlink);

	if (!qlink.qe)
		goto exit;

	tmu_proc_printf(s, "Queue %u (%s) -> epn %u\n", index,
			qlink.qe ? "Enabled" : "Disabled", qlink.epn);
	sbin = qlink.sbin;
	sbid = sbin >> 3;
	leaf = sbin % 8;

	while (1) {
		tmu_sched_blk_in_link_get(sbin, &sbit);
		tmu_proc_printf(s,
				"   -->SBID/LEAF/sbin/iwgt: %u/%u/%u/%u (%s)\n",
				sbid, leaf, sbin, sbit.iwgt,
				sbit.sie ? "Enabled" : "Disabled");
		tmu_proc_printf(s, "      TBID=%d(%s)\n", sbit.tbid,
				sbit.tbe ? "Enabled" : "Disabled");

		if (sbit.tbe) {
			tmu_token_bucket_shaper_cfg_get(sbid, &para);
			tmu_proc_printf(s,
					"      TB Info:pir(%u)_pbs(%u)_cir(%u)_cbs(%u)\n",
					para.pir, para.pbs, para.cir,
					para.cbs);
		}

		/*print other TBID info */
		if (sbit.sit == 0) {	/*qid */
			if (sbit.qsid != index)
				tmu_proc_printf(s,
						"      Why its sbit.qsid(%u) != %u\n",
						sbit.qsid, index);
		}

		/* Next sb  */
		tmu_sched_blk_out_link_get(sbid, &olink);

		if (lvl > 0) {
			if (lvl != (olink.lvl + 1))
				tmu_proc_printf(s,
						"      why last lvl(%d) + 1 !=currnet lvl(%d)\n",
						lvl, olink.lvl);
		}

		lvl = olink.lvl;
		tmu_proc_printf(s, "      lvl=%u\n", olink.lvl);
		tmu_proc_printf(s, "      omid=%u\n", olink.omid);
		tmu_proc_printf(s, "      sbot=%s\n",
				olink.soe ? "Enabled" : "Disabled");

		if (!olink.v) {	/*OMID means EPN, so it is the end of Loop */
			tmu_egress_port_link_get(olink.omid, &eport_link);

			if (olink.omid != qlink.epn)
				tmu_proc_printf(s,
						"      Why Current olink.omid(%u) !=queue's epn (%u) ??\n",
						olink.omid, qlink.epn);

			tmu_proc_printf(s, "   -->epn %u (%s)\n", olink.omid,
					eport_link.epe ? "Enabled" :
					"Disabled");

			if (eport_link.sbid != sbid)
				tmu_proc_printf(s,
						"      Why Current SBID(%u)!= epn's configuration(%u) ??\n",
						sbid, eport_link.sbid);

			need_exit = 1;
		}

		tmu_proc_printf(s, "\n");
		tmu_sbit_dump(s, sbin);
		tmu_sbot_dump(s, sbid);
		/*if (sbit.tbe) */
		tmu_tbst_dump(s, sbit.tbid);

		if (need_exit)
			break;

		if (sbin == olink.omid) {
			tmu_proc_printf(s,
					"      Why Current SBIN(%u) equal to next SBIN (%u) ??\n",
					sbin, olink.omid);
			break;
		}

		sbin = olink.omid;
		sbid = sbin >> 3;
		leaf = sbin % 8;
	}
	g_queue_dump = 1;
 exit:
	enq_c = get_enq_counter(index);
	deq_c = get_deq_counter(index);
	tmu_equeue_drop_params_get(index, &thx);
	tmu_qdct_read(index, qdc);

	if (!qlink.qe) {
		if (((flag == PROC_EQT_MODE_DEFAULT)
		     && (enq_c || deq_c ||
			 (qdc[0] + qdc[1] + qdc[2] + qdc[3])))) {
			tmu_proc_printf(s, "Queue %u (%s) -> epn %u\n", index,
					qlink.qe ? "Enabled" : "Disabled",
					qlink.epn);
			tmu_proc_printf(s, "     enq/deq/drop:%u/%u/%u\n",
					enq_c, deq_c,
					(qdc[0] + qdc[1] + qdc[2] + qdc[3]));
		}

		return;
	}

	tmu_proc_printf(s, "      enq/deq/drop:%u/%u/%u\n", enq_c, deq_c,
			(qdc[0] + qdc[1] + qdc[2] + qdc[3]));

	if (thx.dmod)
		tmu_proc_printf(s,
				"      qtth(%d,%d) mith(%d,%d) math(%d,%d) col_map(%d_%d_%d_%d) drop_per_col(%d_%d_%d_%d)\n",
				thx.qtth0, thx.qtth1, thx.mith0, thx.mith1,
				thx.math0, thx.math1, thx.col[0], thx.col[1],
				thx.col[2], thx.col[3], qdc[0], qdc[1],
				qdc[2], qdc[3]);
	else
		tmu_proc_printf(s,
				"      qtth(%d,%d) math0(%d,%d) col_map(%d_%d_%d_%d) drop_per_col(%d_%d_%d_%d)\n",
				thx.qtth0, thx.qtth1, thx.math0, thx.math1,
				thx.col[0], thx.col[1], thx.col[2],
				thx.col[3], qdc[0], qdc[1], qdc[2], qdc[3]);

	tmu_free_pointer_counter_get(&fpc);
	tmu_free_pointer_thr_get(&fpth);
	tmu_egress_port_tail_drop_thr_get(qlink.epn, &epth);
	tmu_qoct_read(index, &wq, &qrth, &qocc, &qavg);
	tmu_proc_printf(s,
			"           qocc= 0x%08x ,qrth= 0x%08x ,qavg=0x%08x,wq=0x%08x\n",
			qocc, qrth, qavg, wq);
	tmu_proc_printf(s,
			"           fpc= 0x%08x ,fpth =0x%08x, ept=(%d,%d,%d,%d)\n",
			fpc, fpth, epth.epth[0], epth.epth[1], epth.epth[2],
			epth.epth[3]);

	return;
}

void tmu_proc_eqt_write_help(void)
{
	TMU_PRINT
	    ("Only display non_zero queue's mib counters: echo m queue_id > eqt\n");
	TMU_PRINT
	    ("Only display enabledqueue info or with non_zero mibcounter");
	TMU_PRINT("echo d queue_id > eqt\n");
	TMU_PRINT("Clear mib counter: echo c queue_id > eqt\n");
}

#define LOOKUP_FIELD_BITS 14
u8 lookup_flags[LOOKUP_FIELD_BITS];
u8 lookup_mask[LOOKUP_FIELD_BITS];
/*return 0: get correct bit mask
		-1: no */
int get_dont_care_lookup(char *s)
{
	int len, i, j;
	int flag = 0;

	if (!s)
		return -1;
	len = strlen(s);
	dp_replace_ch(s, strlen(s), ' ', 0);
	dp_replace_ch(s, strlen(s), '\r', 0);
	dp_replace_ch(s, strlen(s), '\n', 0);
	if (s[0] == 0)
		return -1;
	memset(lookup_flags, 0, sizeof(lookup_flags));
	memset(lookup_mask, 0, sizeof(lookup_mask));
	if ((s[0] != 'b') && (s[0] != 'B'))
		return -1;

	if (len >= LOOKUP_FIELD_BITS + 1)
		len = LOOKUP_FIELD_BITS + 1;
	PRINTK("len=%d\n", len);
	for (i = len - 1, j = 0; i >= 1; i--, j++) {
		if ((s[i] == 'x') || (s[i] == 'X')) {
			lookup_mask[j] = 1;
			flag = 1;
		} else if (('0' <= s[i]) && (s[i] <= '9'))
			lookup_flags[j] = s[i] - '0';
		else if (('A' <= s[i]) && (s[i] <= 'F'))
			lookup_flags[j] = s[i] - 'A' + 10;
		else if (('a' <= s[i]) && (s[i] <= 'f'))
			lookup_flags[j] = s[i] - '1' + 10;
		else
			return -1;
	}
	if (flag) {
		PRINTK("\nGet lookup flag: ");
		for (i = LOOKUP_FIELD_BITS - 1; i >= 0; i--) {
			if (lookup_mask[i])
				PRINTK("x");
			else
				PRINTK("%d", lookup_flags[i]);
		}
		PRINTK("\n");

		return 0;
	} else
		return -1;
}

#if 0
int lookup_table_recursive(int k, int tmp_index, int set_flag, int qid)
{
	int i;

	if (k > LOOKUP_FIELD_BITS) {
		if (set_flag) {
			set_lookup_qid_via_index(tmp_index, qid);
			PRINTK("Set lookup[%d, 0x%x] -> TMU queue[%d]\n",
			       tmp_index, tmp_index, qid);
		} else {
			qid = get_lookup_qid_via_index(tmp_index);
			PRINTK("Get lookup[%d 0x%x] -> TMU queue[%d]\n",
			       tmp_index, tmp_index, qid);
		}
		return;
	}

	if (lookup_mask[k]) {
		for (i = 0; i < 2; i++)
			lookup_table_recursive(k + 1, tmp_index + (i << k),
					       set_flag, qid);
		return;
	}

	lookup_table_recursive(k + 1, tmp_index + (lookup_flags[k] << k),
			       set_flag, qid);
}
#endif

void lookup_table_via_qid(int qid)
{
	uint32_t index, tmp, i, j, k, f = 0;

	TMU_DEBUG(TMU_DEBUG_MSG,
		  "Try to find all lookup flags mapped to qid %d\n", qid);
	for (i = 0; i < 16; i++) {	/*ep */
		for (j = 0; j < 16; j++) {	/*class */
			for (k = 0; k < 64; k++) {	/*flow id/dec/enc/mpe2/mpe1 */
				index = (k << 8) | (i << 4) | j;
				tmp = get_lookup_qid_via_index(index);
				if (tmp != qid)
					continue;
				f = 1;
				PRINTK
				    ("Get lookup[%05u 0x%04x] -> TMU queue[%d]\n",
				     index, index, qid);
			}
		}
	}
	if (!f)
		PRINTK("No mapping to queue id %d yet ?\n", qid);
}

void lookup_table_remap(int old_q, int new_q)
{
	uint32_t index, tmp, i, j, k, f = 0;
	struct cbm_lookup cbm_lu = {0};

	TMU_DEBUG(TMU_DEBUG_MSG,
		  "Try to remap all lookup flags mapped from old_q %d to new_q %d\n",
		  old_q, new_q);
	for (i = 0; i < 16; i++) {	/*ep */
		for (j = 0; j < 16; j++) {	/*class */
			for (k = 0; k < 64; k++) {	/*flow id/dec/enc/mpe2/mpe1 */
				index = (k << 8) | (i << 4) | j;
				tmp = get_lookup_qid_via_index(index);
				if (tmp != old_q)
					continue;
				cbm_lu.qid = new_q;
				cbm_lu.index = index;
				set_lookup_qid_via_index(&cbm_lu);
				f = 1;
				PRINTK
				    ("Remap lookup[%05u 0x%04x] -> TMU queue[%d]\n",
				     index, index, new_q);
			}
		}
	}
	if (!f)
		PRINTK("No mapping to queue id %d yet \n", new_q);
}

void lookup_table_recursive(int k, int tmp_index, int set_flag, int qid)
{
	struct cbm_lookup cbm_lu = {0};
	int i;

	if (k < 0) {		/*finish recursive and start real read/set action */
		if (set_flag) {
			cbm_lu.qid = qid;
			cbm_lu.index = tmp_index;
			set_lookup_qid_via_index(&cbm_lu);
			PRINTK("Set lookup[%05u 0x%04x] -> TMU queue[%d]\n",
			       tmp_index, tmp_index, qid);
		} else {
			qid = get_lookup_qid_via_index(tmp_index);
			PRINTK("Get lookup[%05u 0x%04x] -> TMU queue[%d]\n",
			       tmp_index, tmp_index, qid);
		}
		return;
	}

	if (lookup_mask[k]) {
		for (i = 0; i < 2; i++)
			lookup_table_recursive(k - 1, tmp_index + (i << k),
					       set_flag, qid);
		return;
	}

	lookup_table_recursive(k - 1, tmp_index + (lookup_flags[k] << k),
			       set_flag, qid);
}

ssize_t tmu_proc_get_qid_via_index(struct file *file, const char *buf,
				   size_t count, loff_t *ppos)
{
	int err = 0, len = 0;
	char data[100];
	unsigned int lookup_index;
	unsigned int qid = 0;
	char *param_list[10];
	int num;
	struct cbm_lookup cbm_lu = {0};

	len = (count >= sizeof(data)) ? (sizeof(data) - 1) : count;
	TMU_DEBUG(TMU_DEBUG_MSG, "len=%d\n", len);

	if (len <= 0) {
		err = -EFAULT;
		TMU_PRINT("Wrong len value (%d)\n", len);
		return count;
	}

	if (copy_from_user(data, buf, len)) {
		err = -EFAULT;
		TMU_PRINT("copy_from_user fail");
		return count;
	}

	data[len - 1] = 0;	/* Make string */
	num = dp_split_buffer(data, param_list, ARRAY_SIZE(param_list));

	if (num <= 1)
		goto help;
	if (!param_list[1])
		goto help;

	lookup_index = dp_atoi(param_list[1]);

	if ((dp_strncmpi(param_list[0],
			 "set",
			 strlen("set"))
			 == 0) ||
	    (dp_strncmpi(param_list[0],
			 "write",
			 strlen("write"))
			 == 0)) {
		if (!param_list[2]) {
			TMU_PRINT("wrong command\n");
			return count;
		}
		qid = dp_atoi(param_list[2]);

		/*workaround for mask support */
		if (get_dont_care_lookup(param_list[1]) == 0) {
			lookup_table_recursive(LOOKUP_FIELD_BITS - 1, 0, 1,
					       qid);
			return count;
		}
		cbm_lu.qid = qid;
		cbm_lu.index = lookup_index;
		set_lookup_qid_via_index(&cbm_lu);
		TMU_PRINT("Set lookup[%u 0x%x] -> TMU queue[%u]\n",
			  lookup_index, lookup_index, qid);
	} else if ((dp_strncmpi(param_list[0], "get", strlen("get")) == 0) ||
		   (dp_strncmpi(param_list[0], "read", strlen("read")) == 0)) {
		if (get_dont_care_lookup(param_list[1]) == 0) {
			lookup_table_recursive(LOOKUP_FIELD_BITS - 1, 0, 0,
					       0);
			return count;
		}
		qid = get_lookup_qid_via_index(lookup_index);
		TMU_PRINT("Get lookup[%05u 0x%04x] -> TMU queue[%u]\n",
			  lookup_index, lookup_index, qid);
	} else if (dp_strncmpi(param_list[0], "find", strlen("find")) == 0) {
		/*read out its all flags for specified qid */
		int i;
		qid = dp_atoi(param_list[1]);
		for (i = 0; i < 16; i++)
			find_pattern(i, NULL, qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "find2", strlen("find2")) == 0) {
		/*read out its all flags for specified qid */
		qid = dp_atoi(param_list[1]);
		lookup_table_via_qid(qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "remap", strlen("remap")) == 0) {
		int old_q = dp_atoi(param_list[1]);
		int new_q = dp_atoi(param_list[2]);
		lookup_table_remap(old_q, new_q);
		return count;
	} else
		goto help;
	return count;
 help:
	TMU_PRINT
	    ("Usage: echo set lookup_flags queue_id > /proc/tmu/lookup\n");
	TMU_PRINT("     : echo get lookup_flags > /proc/tmu/lookup\n");
	TMU_PRINT("     : echo find  <x> > /proc/tmu/lookup\n");
	TMU_PRINT("     : echo find2 <x> > /proc/tmu/lookup\n");
	TMU_PRINT("     : echo remap <old_q> <new_q> > /proc/tmu/lookup\n");
	TMU_PRINT("  Hex example: echo set 0x10 10 > /proc/tmu/lookup\n");
	TMU_PRINT("  Dec:example: echo set 16 10 > /proc/tmu/lookup\n");
	TMU_PRINT("  Bin:example: echo set b10000 10 > /proc/tmu/lookup\n");

	TMU_PRINT
	    ("Special for BIN(Don't care bit): echo set b1xxxx 10 > /proc/tmu/lookup\n");
	TMU_PRINT("Lookup format:\n");
	TMU_PRINT
	    ("  Bits Index: | 13   12 |  11  |  10  |  9   |  8   |7   4 | 3   0 |\n");
	TMU_PRINT
	    ("  Fields:     | Flow ID | DEC  | ENC  | MPE2 | MPE1 |  EP  | CLASS |\n");

	return count;
}

ssize_t tmu_proc_eqt_write(struct file *file, const char *buf, size_t count,
			   loff_t *ppos)
{
	int len;
	char str[64];
	int qid = 0;
	int value = 0, num;
	char *param_list[5];
	int print_mode = PROC_EQT_MODE_DEFAULT;
	struct cbm_lookup cbm_lu = {0};

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num < 2) {
		TMU_PRINT("Wrong command\n");
		TMU_PRINT("Usage:\n");
		TMU_PRINT
		    ("Display queue mib:  echo m queue-id > /proc/tmu/eqt\n");
		TMU_PRINT
		    ("     Note: only display those with none-zero mib\n");
		TMU_PRINT
		    ("Clear   queue mib:  echo c queue-id > /proc/tmu/eqt\n");
		TMU_PRINT
		    ("Display queue info: echo d queue-id > /proc/tmu/eqt\n");
		TMU_PRINT("Note: if queue-id is -1, it means all queues\n");
		TMU_PRINT
		    ("Flush one queue (For test only): echo f queue-id ep-value > /proc/tmu/eqt\n");

		return count;
	}

	qid = dp_atoi(param_list[1]);

	if (qid >= EGRESS_QUEUE_ID_MAX) {
		TMU_PRINT("Wrong queue id %u, it should less than %u\n",
			  value, EGRESS_QUEUE_ID_MAX);
		return count;
	}

	/*default with queue link info */
	if (dp_strncmpi(param_list[0], "d", 1) == 0)
		print_mode = PROC_EQT_MODE_DEFAULT;
	/*only display mib counter */
	else if (dp_strncmpi(param_list[0], "m", 1) == 0)
		print_mode = PROC_EQT_MODE_MIB_ONLY;
	/*only clear cbm enqueu/dequeue mib counter */
	else if (dp_strncmpi(param_list[0], "c", 1) == 0)
		print_mode = PROC_EQT_MODE_MIB_CLEAR;
	else if (dp_strncmpi(param_list[0], "e", 1) == 0) {
		/*only with mib counter */
		value = dp_atoi(param_list[2]);

		if (value)
			tmu_equeue_enable(qid, 1);
		else
			tmu_equeue_enable(qid, 0);

		return count;
	} else if (dp_strncmpi(param_list[0], "f", 1) == 0) {
			/*flush one queue:
			* first change all queue's lookup table to drop queue
			* then disable all other queue's scheduler except the flushed one ---seeem not allowed
			* change to: disable all other queue
			* then read qocc until zero
			* last to recover all queue related setting
			*/
#define DISABLE_SCHED_INPUT

#ifdef DISABLE_SCHED_INPUT
			/*struct tmu_sched_blk_in_link ilink;*/
			/*struct cbm_qidt_elm qid_set;*/
			/*struct cbm_qidt_mask qid_mask;*/
#endif
			struct tmu_equeue_link elink;
			int tmp_num = 0, i;
			uint32_t tmp_qid[8];
			uint32_t wq, qrth, qocc, qavg, last_qocc;
			u64 last_tick = 0;
			int ep;
			int times = 50000;

			ep = dp_atoi(param_list[2]);
			TMU_PRINT("Need Flush q=%d with ep=%d\n", qid, ep);

			/*lookup table */
#if 0
			memset(&qid_set, 0, sizeof(qid_set));
			qid_mask.classid_mask = 1;
			/* qid_mask.ep_mask = 1; */
			qid_mask.mpe1_mask = 1;
			qid_mask.mpe2_mask = 1;
			qid_mask.enc_mask = 1;
			qid_mask.dec_mask = 1;
			qid_mask.flowid_lmask = 1;
			qid_mask.flowid_hmask = 1;
			qid_set.ep = ep;
			qid_mask.ep_mask = 0;
			cbm_qidt_set(&qid_set, &qid_mask, EGRESS_QUEUE_ID_MAX - 1);
#else
			for (i = 0; i < 0x4000; i++) {
				if (get_lookup_qid_via_index(i) != qid)
					continue;
				cbm_lu.index = i;
				cbm_lu.qid = EGRESS_QUEUE_ID_MAX - 1;
				set_lookup_qid_via_index(&cbm_lu);
			}
#endif

#ifdef DISABLE_SCHED_INPUT
			/* disable other link */
			tmu_equeue_link_get(qid, &elink);
			tmu_queue_list(elink.epn, tmp_qid, ARRAY_SIZE(tmp_qid), &tmp_num, NULL);
			for (i = 0; i < tmp_num; i++) {
				if (tmp_qid[i] == qid)
					continue;
				tmu_equeue_link_get(tmp_qid[i], &elink);
				/*tmu_sched_blk_in_link_get(elink.sbin, &ilink);
				ilink.sie = 0;
				tmu_sched_blk_in_link_set(elink.sbin, &ilink);*/
				tmu_sched_blk_in_enable(elink.sbin, 0);
				TMU_PRINT("Disable q %u's scheduler input %u\n",
					tmp_qid[i], elink.sbin);
			}
#endif
			tmu_qoct_read(qid, &wq, &qrth, &last_qocc, &qavg);
			do {
				times--;
				tmu_qoct_read(qid, &wq, &qrth, &qocc, &qavg);
				if (qocc < last_qocc) {
					last_qocc = qocc;
					last_tick = 0;
				} else if (qocc == last_qocc) {
					if (!last_tick)
						last_tick = jiffies_64;
					else if ((jiffies_64 - last_tick) >= HZ * 5) {
							TMU_PRINT("Why qocc no chnage %u for qid %d\n",
								qocc, qid);
							last_tick = 0;
					}
				} else
					TMU_PRINT("Why current qocc %u > last_qocc %u\n",
						qocc, last_qocc);
				mdelay(10);
			} while (qocc && times);
#ifdef DISABLE_SCHED_INPUT
			/* recover other link */
			for (i = 0; i < tmp_num; i++) {
				if (tmp_qid[i] == qid)
					continue;
				tmu_equeue_link_get(tmp_qid[i], &elink);
				/*tmu_sched_blk_in_link_get(elink.sbin, &ilink);
				ilink.sie = 1;
				tmu_sched_blk_in_link_set(elink.sbin, &ilink);
				*/
				tmu_sched_blk_in_enable(elink.sbin, 1);
			}
#endif
			if (qocc != 0)
				TMU_PRINT("Faied to flush queue %d\n", qid);
			else
				TMU_PRINT("Succeed to flush queue %d\n", qid);
			return count;
	} else if (dp_strncmpi(param_list[0], "r", 1) == 0) {
			/*recover one flushed queue, mainly to enable its */
			value = dp_atoi(param_list[1]);
	} else {
		TMU_PRINT("Wrong Command. Supported command is d/m/c/e\n");
		return count;
	}

	TMU_PRINT("\n");

	f_q_mib_title_proc = 0;
	if (qid >= 0)
		tmu_proc_eqt_print(NULL, qid, print_mode);
	else {
		for (qid = 0; qid < EGRESS_QUEUE_ID_MAX; qid++)
			tmu_proc_eqt_print(NULL, qid, print_mode);
	}

	return count;
}

void tmu_ept_dump(struct seq_file *s)
{
	uint32_t i;

	if (!tmu_is_enabled())
		SEQ_PRINTF(s, "TMU not activated\n");

	SEQ_PRINTF(s, "Egress Port Table (EPT)\n");
	SEQ_PRINTF(s, "no ,epe     ,sbid    ,epoc0   ,epoc1   ,ept0    ,");
	SEQ_PRINTF(s, "ept1    ,epd0    ,epd1    ,epd2    ,epd3\n");

	for (i = 0; i < EGRESS_PORT_ID_MAX; i++) {
		uint32_t tmp[10], k;
		tmu_epmt_read(i, &tmp[0], &tmp[1]);
		tmu_epot_read(i, &tmp[2]);
		tmu_eptt_read(i, &tmp[4]);
		tmu_epdt_read(i, &tmp[6]);
		SEQ_PRINTF(s, "%03d", i);

		for (k = 0; k < 10; k++)
			SEQ_PRINTF(s, ",%08x", tmp[k]);

		SEQ_PRINTF(s, "\n");
	}
}

static int tmu_sbit_dump_title;
static int tmu_sbot_dump_title;
int tmu_sbit_dump_start(void)
{
	tmu_sbit_dump_title = 1;
	proc_pre_align_buf[0] = '\0';
	return 0;
}

int tmu_sbot_dump_start(void)
{
	tmu_sbot_dump_title = 1;
	proc_pre_align_buf[0] = '\0';
	return 0;
}

int tmu_sbit_dump(struct seq_file *s, int pos)
{
	uint32_t i = pos + proc_sbit_start_id;
	uint32_t tmp[4], k;

	if (tmu_sbit_dump_title) {
		tmu_proc_printf(s, "%sScheduler Block Input Table (SBIT)\n",
				proc_pre_align_buf);
		tmu_proc_printf(s,
				"%sno ,sbitr0  ,sbitr1  ,sbitr2  ,sbitr3\n",
				proc_pre_align_buf);
		tmu_sbit_dump_title = 0;
	}

	tmu_w32(TMU_SBITC_SEL | i, sbitc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbitc) & TMU_SBITC_VAL) == 0)
		continue;

	tmp[0] = tmu_r32(sbitr0);
	tmp[1] = tmu_r32(sbitr1);
	tmp[2] = tmu_r32(sbitr2);
	tmp[3] = tmu_r32(sbitr3);
#else
	tmp[0] = simu_sbitr0[i];
	tmp[1] = simu_sbitr1[i];
	tmp[2] = simu_sbitr2[i];
	tmp[3] = simu_sbitr3[i];
#endif
	tmu_proc_printf(s, "%ssbit[%03d]", proc_pre_align_buf, i);

	for (k = 0; k < 4; k++)
		tmu_proc_printf(s, "%08x", tmp[k]);

	tmu_proc_printf(s, "\n");
	/*parse sbitr0 */
	tmu_proc_printf(s,
			"%s  sbitr0:Input=%s[0x%05x], SIE=Scheduler %-8s, IWGT=0x%03x\n",
			proc_pre_align_buf,
			(tmp[0] & TMU_SBITR0_SIT) ? "QSID" : "SBID",
			get_val(tmp[0], TMU_SBITR0_QSID_MASK,
				TMU_SBITR0_QSID_OFFSET),
			(tmp[0] & TMU_SBITR0_SIE) ? "Enabled" : "Disabled",
			get_val(tmp[0], TMU_SBITR0_IWGT_MASK,
				TMU_SBITR0_IWGT_OFFSET));
	/*parse sbitr1 */
	tmu_proc_printf(s, "%s  sbitr1:TBID=0x%02x,   TBE=%-8s\n",
			proc_pre_align_buf, get_val(tmp[1],
						    TMU_SBITR1_TBID_MASK,
						    TMU_SBITR1_TBID_OFFSET),
			(tmp[1] & TMU_SBITR1_TBE) ? "Enabled" : "Disabled");
	/*parse sbitr2 */
	tmu_proc_printf(s,
			"%s  sbitr2:VDT=0x%05x, SIP(INPUT PASS)=%-8s, SIF(INPUT Fill)=%-8s\n",
			proc_pre_align_buf, get_val(tmp[2],
						    TMU_SBITR2_VDT_MASK,
						    TMU_SBITR2_VDT_OFFSET),
			(tmp[2] & TMU_SBITR2_SIP) ? "Enabled" : "Disabled",
			(tmp[2] & TMU_SBITR2_SIF) ? "Filled" : "Empty");
	/*parse sbitr3 */
	tmu_proc_printf(s, "%s  sbitr3:QOSL=0x%04x, color=%-8s\n",
			proc_pre_align_buf, get_val(tmp[2],
						    TMU_SBITR3_QOSL_MASK,
						    TMU_SBITR3_QOSL_OFFSET),
			pdu_color_type[get_val
				       (tmp[2], TMU_SBITR3_COL_MASK,
					TMU_SBITR3_COL_OFFSET)]);
	pos++;

	if ((pos + proc_sbit_start_id >= SCHEDULER_BLOCK_INPUT_ID_MAX) ||
	    (pos + proc_sbit_start_id > proc_sbit_end_id))
		pos = -1;	/*end of the loop */

	return pos;
}

int tmu_sbot_dump(struct seq_file *s, int pos)
{
	uint32_t i = pos + proc_sbot_start_id;
	uint32_t tmp[2];

	if (!tmu_is_enabled())
		TMU_PRINT("TMU not activated\n");

	if (tmu_sbot_dump_title) {
		tmu_proc_printf(s, "%sScheduler Block Output Table (SBOT)\n",
				proc_pre_align_buf);
		tmu_proc_printf(s, "%sno ,sbotr0  ,sbotr1\n",
				proc_pre_align_buf);
		tmu_sbot_dump_title = 0;
	}

	tmu_w32(TMU_SBOTC_SEL | i, sbotc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
		continue;

	tmp[0] = tmu_r32(sbotr0);
	tmp[1] = tmu_r32(sbotr1);
#else
	tmp[0] = simu_sbotr0[i];
	tmp[1] = simu_sbotr1[i];
#endif
	tmu_proc_printf(s, "%ssbot[%03d]:%08x,%08x\n", proc_pre_align_buf, i,
			tmp[0], tmp[1]);
	tmu_proc_printf(s,
			"%s  sbotr0:OMID=Map to %s[%u], LVL=%u,      SOE=Output %s\n",
			proc_pre_align_buf,
			(tmp[0] & TMU_SBOTR0_V) ? "SBID" : "EPN",
			get_val(tmp[0], TMU_SBOTR0_OMID_MASK,
				TMU_SBOTR0_OMID_OFFSET), get_val(tmp[0],
								 TMU_SBOTR0_LVL_MASK,
								 TMU_SBOTR0_LVL_OFFSET),
			(tmp[0] & TMU_SBOTR0_SOE) ? "Enabled" : "Disabled");
	/*parse sbotr1 */
	tmu_proc_printf(s,
			"%s  sbotr1:WQID(Winner)=%u,    WL(LEAF)=%u, SOF=Output %-6s\n",
			proc_pre_align_buf, get_val(tmp[1],
						    TMU_SBOTR1_WQID_MASK,
						    TMU_SBOTR1_WQID_OFFSET),
			get_val(tmp[1], TMU_SBOTR1_WL_MASK,
				TMU_SBOTR1_WL_OFFSET),
			(tmp[1] & TMU_SBOTR1_SOF) ? "Filled" : "Empty");
	pos++;

	if ((pos + proc_sbot_start_id >= SCHEDULER_BLOCK_ID_MAX) ||
	    (pos + proc_sbot_start_id > proc_sbot_end_id))
		pos = -1;	/*end of the loop */

	return pos;
}

int tmu_tbst_dump_start(void)
{
	proc_pre_align_buf[0] = '\0';
	return 0;
}

int tmu_tbst_dump(struct seq_file *s, int pos)
{
	int k, i = pos + proc_tbst_start_id;
	uint32_t tmp[11], field[2];

	if (!tmu_is_enabled())
		TMU_PRINT("TMU not activated\n");

	tmu_w32(TMU_TBSTC_SEL | i, tbstc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
		continue;

	tmp[0] = tmu_r32(tbstr0);
	tmp[1] = tmu_r32(tbstr1);
	tmp[2] = tmu_r32(tbstr2);
	tmp[3] = tmu_r32(tbstr3);
	tmp[4] = tmu_r32(tbstr4);
	tmp[5] = tmu_r32(tbstr5);
	tmp[6] = tmu_r32(tbstr6);
	tmp[7] = tmu_r32(tbstr7);
	tmp[8] = tmu_r32(tbstr8);
	tmp[9] = tmu_r32(tbstr9);
	tmp[10] = tmu_r32(tbstr10);
#else
	tmp[0] = simu_tbstr0[i];
	tmp[1] = simu_tbstr1[i];
	tmp[2] = simu_tbstr2[i];
	tmp[3] = simu_tbstr3[i];
	tmp[4] = simu_tbstr4[i];
	tmp[5] = simu_tbstr5[i];
	tmp[6] = simu_tbstr6[i];
	tmp[7] = simu_tbstr7[i];
	tmp[8] = simu_tbstr8[i];
	tmp[9] = simu_tbstr9[i];
	tmp[10] = simu_tbstr10[i];
#endif
	tmu_proc_printf(s, "%sTBID[%03d]:", proc_pre_align_buf, i);

	for (k = 0; k < 5; k++)
		tmu_proc_printf(s, "%08x,", tmp[k]);

	tmu_proc_printf(s, "\n");
	tmu_proc_printf(s, "          %s", proc_pre_align_buf);

	for (; k < 11; k++)
		tmu_proc_printf(s, "%08x,", tmp[k]);

	tmu_proc_printf(s, "\n");
	field[0] =
	    get_val(tmp[0], TMU_TBSTR0_MOD_MASK, TMU_TBSTR0_MOD_OFFSET);
	field[1] =
	    get_val(tmp[0], TMU_TBSTR0_SBIN_MASK, TMU_TBSTR0_SBIN_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR0:sbin=0x%04x,   mod=%-30s\n",
			proc_pre_align_buf, field[1],
			(field[0] < 4) ? mod[field[0]] : "ERROR");
	field[0] =
	    get_val(tmp[1], TMU_TBSTR1_TBU0_MASK, TMU_TBSTR1_TBU0_OFFSET);
	field[1] =
	    get_val(tmp[1], TMU_TBSTR1_SRM0_MASK, TMU_TBSTR1_SRM0_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR1:tbe0=%-8s, tbu0=%-16s, srm0=0x%04x\n",
			proc_pre_align_buf,
			(tmp[1] & TMU_TBSTR1_TBE0) ? "Enable" : "Disable",
			(field[0] < 4) ? tbu[field[0]] : "ERROR", field[1]);
	field[0] =
	    get_val(tmp[2], TMU_TBSTR2_TBU1_MASK, TMU_TBSTR2_TBU1_OFFSET);
	field[1] =
	    get_val(tmp[2], TMU_TBSTR2_SRM1_MASK, TMU_TBSTR2_SRM1_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR2:tbe1=%-8s, tbu1=%-16s, srm1=0x%04x\n",
			proc_pre_align_buf,
			(tmp[1] & TMU_TBSTR1_TBE0) ? "Enable" : "Disable",
			(field[0] < 4) ? tbu[field[0]] : "ERROR", field[1]);
	field[0] =
	    get_val(tmp[3], TMU_TBSTR3_MBS0_MASK, TMU_TBSTR3_MBS0_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR3:mbs0=0x%06x\n", proc_pre_align_buf,
			field[0]);
	field[0] =
	    get_val(tmp[4], TMU_TBSTR4_MBS1_MASK, TMU_TBSTR4_MBS1_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR4:mbs1=0x%06x\n", proc_pre_align_buf,
			field[0]);
	field[0] =
	    get_val(tmp[5], TMU_TBSTR5_SRC0_MASK, TMU_TBSTR5_SRC0_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR5:src0=0x%05x\n", proc_pre_align_buf,
			field[0]);
	field[0] =
	    get_val(tmp[6], TMU_TBSTR6_SRC1_MASK, TMU_TBSTR6_SRC1_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR6:src1=0x%05x\n", proc_pre_align_buf,
			field[0]);
	field[0] =
	    get_val(tmp[7], TMU_TBSTR7_TBC0_MASK, TMU_TBSTR7_TBC0_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR7:tbc0=0x%07x, pass0=%s\n",
			proc_pre_align_buf, field[0],
			(tmp[7] & TMU_TBSTR7_PASS0) ? "Enable" : "Disable");
	field[0] =
	    get_val(tmp[8], TMU_TBSTR8_TBC1_MASK, TMU_TBSTR8_TBC1_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR8:tbc1=0x%07x, pass1=%s\n",
			proc_pre_align_buf, field[0],
			(tmp[8] & TMU_TBSTR8_PASS1) ? "Enable" : "Disable");
	field[0] =
	    get_val(tmp[9], TMU_TBSTR9_COL_MASK, TMU_TBSTR9_COL_OFFSET);
	field[1] =
	    get_val(tmp[9], TMU_TBSTR9_QOSL_MASK, TMU_TBSTR9_QOSL_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR9:col=0x%x, qosl=0x%04x\n",
			proc_pre_align_buf, field[0], field[1]);
	field[0] =
	    get_val(tmp[10], TMU_TBSTR10_TS_MASK, TMU_TBSTR10_TS_OFFSET);
	tmu_proc_printf(s, "%s  TBSTR10:ts=0x%04x\n", proc_pre_align_buf,
			field[0]);
	pos++;

	if ((pos + proc_tbst_start_id >= TOCKEN_BUCKET_ID) ||
	    (pos + proc_tbst_start_id > proc_tbst_end_id))
		pos = -1;	/*end of the loop */

	return pos;
}

int tmu_ppt_dump_start(void)
{
	proc_pre_align_buf[0] = '\0';
	return (tmu_r32(fpl) & TMU_FPL_HFPP_MASK) >> TMU_FPL_HFPP_OFFSET;
}

int tmu_ppt_dump(struct seq_file *s, int pos)
{
	static uint32_t loopcnt;
	uint32_t ppt[4];
	uint32_t fpl, fplen, tfpp, hfpp;
	uint32_t pnext, offs, hdrl;
	uint32_t qosl, pdut, segl;
	uint32_t col, gpix, bdyl;
	uint32_t hlsa, tlsa;

	if (!tmu_is_enabled())
		SEQ_PRINTF(s, "TMU not activated\n");

	fpl = tmu_r32(fpl);
	fplen = tmu_r32(fpcr);
	tfpp = (fpl & TMU_FPL_TFPP_MASK) >> TMU_FPL_TFPP_OFFSET;
	hfpp = (fpl & TMU_FPL_HFPP_MASK) >> TMU_FPL_HFPP_OFFSET;

	if (pos == (int)hfpp) {
		SEQ_PRINTF(s, "Packet Pointer Table (PPT) Free List\n");
		SEQ_PRINTF(s, "Length = %4u TFPP = 0x%04x HFPP = 0x%04x\n",
			   fplen, tfpp, hfpp);
		SEQ_PRINTF(s,
			   "line - pcurr  pnext  offs hdrl qosl  pdut segl "
			   "col  gpix bdyl  hlsa   tlsa\n");
		loopcnt = 0;
	}

	tmu_w32(pos, pptc);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	while ((tmu_r32(pptc) & TMU_PPTC_VAL) == 0)
		continue;

	ppt[0] = tmu_r32(ppt0);
	ppt[1] = tmu_r32(ppt1);
	ppt[2] = tmu_r32(ppt2);
	ppt[3] = tmu_r32(ppt3);
#else
	ppt[0] = simu_ppt0[pos];
	ppt[1] = simu_ppt1[pos];
	ppt[2] = simu_ppt2[pos];
	ppt[3] = simu_ppt3[pos];
#endif
	pnext = (ppt[0] & TMU_PPT0_PNEXT_MASK) >> TMU_PPT0_PNEXT_OFFSET;
	offs = (ppt[0] & TMU_PPT0_OFFS_MASK) >> TMU_PPT0_OFFS_OFFSET;
	hdrl = (ppt[0] & TMU_PPT0_HDRL_MASK) >> TMU_PPT0_HDRL_OFFSET;
	qosl = (ppt[1] & TMU_PPT1_QOSL_MASK) >> TMU_PPT1_QOSL_OFFSET;
	pdut = (ppt[1] & TMU_PPT1_PDUT_MASK) >> TMU_PPT1_PDUT_OFFSET;
	segl = (ppt[1] & TMU_PPT1_SEGL_MASK) >> TMU_PPT1_SEGL_OFFSET;
	col = (ppt[2] & TMU_PPT2_COL_MASK) >> TMU_PPT2_COL_OFFSET;
	gpix = (ppt[2] & TMU_PPT2_GPIX_MASK) >> TMU_PPT2_GPIX_OFFSET;
	bdyl = (ppt[2] & TMU_PPT2_BDYL_MASK) >> TMU_PPT2_BDYL_OFFSET;
	hlsa = (ppt[3] & TMU_PPT3_HLSA_MASK) >> TMU_PPT3_HLSA_OFFSET;
	tlsa = (ppt[3] & TMU_PPT3_TLSA_MASK) >> TMU_PPT3_TLSA_OFFSET;
	SEQ_PRINTF(s, "%04u - 0x%04x 0x%04x %4u %4u %5u %4u %4u %4u %4u "
		       "%5u 0x%04x 0x%04x\n", loopcnt + 1, pos, pnext, offs,
		       hdrl, qosl, pdut, segl, col, gpix, bdyl, hlsa, tlsa);

	loopcnt++;

	if (pos != (int)hfpp && pnext == hfpp)
		return -1;

	if (pos == (int)tfpp)
		return -1;

	return pnext;
}

#endif

#ifdef TMU_LOCAL_STAT_DEBUG

#define MAX_QUEUE_PER_PORT 64

#define SB_BLOCK_START        1
#define SB_BLOCK_END          2
#define SB_BLOCK_LINK_LINE    4	/*for print sb link arrow */
struct stat_sb_info {		/*including sbot and sbin */
	uint32_t flag;
	uint8_t filled:1;
	uint8_t soe:1;		/* sbot enable flag */
	uint8_t sie:1;		/* sbit enable flag */
	uint8_t sit:1;		/* 1 means next is sbin, 0 menas qid */
	uint8_t tbe:1;		/* 1 means tb is enabled, 0 disabled */
	uint16_t sbin;
	uint16_t tbid;
	uint16_t qsid;
	uint16_t iwgt;
	/*other shaper info */
};

#define Q_STAT_DBG_HEAD      1
#define Q_STAT_DBG_SELECTED  2
#define Q_STAT_DBG_LINKED    4
#define Q_STAT_DBG_PRINTTED  8

struct queue_valid_info {

	uint32_t flag;
	 /**/ uint32_t qid;
	uint8_t filled:1;
	uint8_t qe:1;
	uint16_t epn;
	/*uint16_t max_lvl;   maximum sb level for this queue */
	struct stat_sb_info sb_info[SCHEDULE_MAX_LEVEL];
};

struct stat_info {
	/*read from TMU registers */
	uint16_t max_lvl;
	uint16_t qnum;
	struct queue_valid_info q_info[EGRESS_QUEUE_ID_MAX];
};

#define MAX_PRINT_LEN_PER_LINE 1000

struct tmp_queue1 {
	unsigned char buf_sb_blk_start[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf1[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_queue_th[MAX_PRINT_LEN_PER_LINE];
	unsigned char queueid_buf[MAX_PRINT_LEN_PER_LINE];
	unsigned char queueid_en[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_tm[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_pir[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_cir[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_pbs[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_cbs[MAX_PRINT_LEN_PER_LINE];
	unsigned char buf_sb_blk_last[MAX_PRINT_LEN_PER_LINE];
	/*int q_buff_num; */
	/*struct q_link tmp_q_buff[EGRESS_QUEUE_ID_MAX]; */

};

struct stat_info stat = { 0 };

int tmu_max_lvl;
struct tmp_queue1 tmp_stat;
static char first_print;
int q_buff_num;
uint32_t tmp_q_buff[EGRESS_QUEUE_ID_MAX];

void replace_ch(char *p, int len, char orig_ch, char new_ch)
{
	int i;

	if (p)
		for (i = 0; i < len; i++) {
			if (p[i] == orig_ch)
				p[i] = new_ch;
		}
}

#define SB_BLOCK_OFFSET      8
#define SPACE_PER_SB    24
#define SPACE_PER_BLOCK 3
#define MINUS_INDET_TM  0
#define MINUS_RESV_SB_SPACE 1

int print_raw_stat(struct seq_file *s, int epn)
{
	int j;
	int offset;

	if (epn == 0) {
		SEQ_PRINTF(s, "qnum=%d, max_lvl=%d\n", stat.qnum,
			   stat.max_lvl);
		/*
		   Print title like below format
		   Queue SBID:L    SBID:L    SBID:L    SBID:L    Port
		 */
		tmu_memset(tmp_stat.buf1, ' ', sizeof(tmp_stat.buf1) - 2);
		sprintf(tmp_stat.buf1, "Queue");
		offset = SB_BLOCK_OFFSET;

		for (j = stat.max_lvl; j >= 0; j--) {
			sprintf(tmp_stat.buf1 + offset, "SBID:L/W");
			offset += SPACE_PER_SB;
		}

		sprintf(tmp_stat.buf1 + offset, "Port\n");
		dp_replace_ch(tmp_stat.buf1, offset, 0, ' ');
		SEQ_PRINTF(s, "%s", tmp_stat.buf1);
	}

	/*print queue info one by one */
	if (!stat.q_info[epn].qe)
		if (!flag_print_disable_q)
			goto EXIT;

	tmu_memset(tmp_stat.buf1, ' ', sizeof(tmp_stat.buf1) - 2);
	tmu_memset(tmp_stat.buf_tm, ' ', sizeof(tmp_stat.buf1) - 2);
	sprintf(tmp_stat.buf1, "%c%3d", stat.q_info[epn].qe ? '+' : '-', epn);
	offset = SB_BLOCK_OFFSET;

	for (j = stat.max_lvl; j >= 0; j--) {
		if (stat.q_info[epn].sb_info[j].filled) {
			sprintf(tmp_stat.buf1 + offset, "%3d:%1d/%-4d",
				stat.q_info[epn].sb_info[j].sbin >> 3,
				stat.q_info[epn].sb_info[j].sbin % 8,
				stat.q_info[epn].sb_info[j].iwgt);

			if (stat.q_info[epn].sb_info[j].tbe)
				sprintf(tmp_stat.buf_tm + offset -
					MINUS_INDET_TM, "%3d",
					stat.q_info[epn].sb_info[j].tbid);
		}

		offset += SPACE_PER_SB;
	}

	sprintf(tmp_stat.buf1 + offset, "%2d\n", stat.q_info[epn].epn);
	sprintf(tmp_stat.buf_tm + offset, "\n");
	dp_replace_ch(tmp_stat.buf1, offset, 0, ' ');
	dp_replace_ch(tmp_stat.buf_tm, offset, 0, ' ');
	SEQ_PRINTF(s, "%s", tmp_stat.buf1);
	SEQ_PRINTF(s, "%s", tmp_stat.buf_tm);
 EXIT:
	epn++;

	if (epn >= EGRESS_PORT_ID_MAX)
		epn = -1;

	return epn;
}

int marking_queue(void)
{
	int i, j, k;

	for (i = 0; i < stat.qnum; i++) {	/*queue loop */
		if (!stat.q_info[i].filled)
			if (!flag_print_disable_q)
				continue;

		for (j = stat.max_lvl; j >= 0; j--) {
			/*SB staring block */
			if (i == 0) {	/*first queue */
				/*first queue's */
				if (stat.q_info[i].sb_info[j].filled) {
					stat.q_info[i].sb_info[j].flag |=
					    SB_BLOCK_START;
					TMU_DEBUG(TMU_DEBUG_QUEUE1,
						  "SB_BLOCK_START 1 for SBID %u for QID %u flag %u\n",
						  stat.q_info[i].
						  sb_info[j].sbin >> 3,
						  stat.q_info[i].qid,
						  stat.q_info[i].
						  sb_info[j].flag);
				}
			} else {	/*compare with previous one */
				if (stat.q_info[i].sb_info[j].filled) {
					if ((stat.q_info[i].sb_info[j].
					     filled !=
					     stat.q_info[i -
							 1].sb_info[j].filled)
					    ||
					    ((stat.q_info[i].
					      sb_info[j].sbin >> 3) !=
					     (stat.q_info[i - 1].
					      sb_info[j].sbin >> 3))) {
						stat.q_info[i].
						    sb_info[j].flag |=
						    SB_BLOCK_START;
						TMU_DEBUG(TMU_DEBUG_QUEUE1,
							  "SB_BLOCK_START 2 for SBID %u for QID %u flag %u\n",
							  stat.
							  q_info[i].sb_info
							  [j].sbin >> 3,
							  stat.q_info[i].qid,
							  stat.
							  q_info[i].sb_info
							  [j].flag);
					}
				}
			}

			/*SB ending block */
			if (i == stat.qnum - 1) {	/*last queue */
				if (stat.q_info[i].sb_info[j].filled) {
					stat.q_info[i].sb_info[j].flag |=
					    SB_BLOCK_END;
					TMU_DEBUG(TMU_DEBUG_QUEUE1,
						  "SB_BLOCK_END 1 for SBID %u for QID %u flag %u\n",
						  stat.q_info[i].
						  sb_info[j].sbin >> 3,
						  stat.q_info[i].qid,
						  stat.q_info[i].
						  sb_info[j].flag);
				}
			} else {	/*compare with next one */
				if (stat.q_info[i].sb_info[j].filled) {
					/*TMU_PRINT("here\n"); */
					if ((stat.q_info[i].sb_info[j].
					     filled !=
					     stat.q_info[i +
							 1].sb_info[j].filled)
					    ||
					    ((stat.q_info[i].
					      sb_info[j].sbin >> 3) !=
					     (stat.q_info[i + 1].
					      sb_info[j].sbin >> 3))) {
						stat.q_info[i].
						    sb_info[j].flag |=
						    SB_BLOCK_END;
						TMU_DEBUG(TMU_DEBUG_QUEUE1,
							  "SB_BLOCK_END 2 for SBID %u for QID %u flag %u\n",
							  stat.
							  q_info[i].sb_info
							  [j].sbin >> 3,
							  stat.q_info[i].qid,
							  stat.
							  q_info[i].sb_info
							  [j].flag);
					}
				}
			}
		}
	}

	for (j = stat.max_lvl; j >= 0; j--) {
		for (i = 0; i < stat.qnum; i++) {	/*queue loop */
			int num;

			if (stat.q_info[i].sb_info[j].flag & SB_BLOCK_START) {
				num = 0;

				if (!
				    (stat.q_info[i].
				     sb_info[j].flag & SB_BLOCK_END))
					for (k = i + 1; k < stat.qnum; k++) {
						if (!
						    (stat.
						     q_info[k].sb_info[j].
						     flag & SB_BLOCK_END))
							num++;
					}

				num = i + (num + 1) / 2;
				stat.q_info[num].sb_info[j].flag |=
				    SB_BLOCK_LINK_LINE;
				TMU_DEBUG(TMU_DEBUG_QUEUE1,
					  "SB_BLOCK_LINK_LINE for SBID %u for QID %u flag=%u\n",
					  stat.q_info[num].sb_info[j].
					  sbin >> 3, stat.q_info[num].qid,
					  stat.q_info[num].sb_info[j].flag);
			}
		}
	}

	return 0;
}

#define INVALID_DMA_CH 255
char *get_dma_flags_str(uint32_t epn, char *buf, int buf_len)
{
	char tmp[30];		/*must be static */
	uint32_t flags;
	uint32_t tx_ch, k;
	u8 f_found;

	if (!buf || (buf_len < 1))
		return NULL;
	tx_ch = 0;
	flags = 0;
	tmp[0] = '\0';
	f_found = 0;
	if (cbm_get_egress_port_info(epn, &tx_ch, &flags) != 0)
		goto EXIT;

	sprintf(tmp, "--");
	if (tx_ch != INVALID_DMA_CH) {
		if (!(flags & DP_F_FAST_DSL))	/*DSL upstrem no DMA1/2 TX CHannel */
			sprintf(tmp + strlen(tmp), "CH%02d ", tx_ch);
		else
			sprintf(tmp + strlen(tmp), "CHXX ");
	} else
		sprintf(tmp + strlen(tmp), "CHXX ");
	if (tmu_debug_lvl & TMU_DEBUG_MSG)
		sprintf(tmp + strlen(tmp), "flag=0x%08x ", flags);

	if (flags == 0) {
		if (epn < 4)
			sprintf(tmp + strlen(tmp), "CPU");
		else
			sprintf(tmp + strlen(tmp), "Flag0");

		goto EXIT;
	}
	for (k = 0; k < get_dp_port_type_str_size(); k++) {
		if (flags & get_dp_port_flag(k)) {
			sprintf(tmp + strlen(tmp), "%s ",
				get_dp_port_type_str(k));
			f_found = 1;
		}
	}
	if ((f_found == 1) && (flags == DP_F_FAST_ETH_LAN)) {	/*try to find its ep */
		uint32_t i, num, j;
		cbm_tmu_res_t *res;
		u32 *port_flag;
		for (i = 1; i <= 6; i++) {	/*6 LAN port */
			num = 0;
			port_flag = get_port_flag(0, i);
			if (!port_flag)
				continue;
			if (cbm_dp_port_resources_get
			    (&i, &num, &res, *port_flag))
				continue;
			for (j = 0; j < num; j++) {
				if (res[j].tmu_port != epn)	/* not match */
					continue;
				sprintf(tmp + strlen(tmp), "%d", i);
			}
			kfree(res);
		}
	}
	if (!f_found)
		sprintf(tmp + strlen(tmp), "Unknown[0x%x]\n", flags);

 EXIT:
	strncpy(buf, tmp, buf_len);
	return buf;
}
EXPORT_SYMBOL(get_dma_flags_str);

int print_queue1(struct seq_file *s, uint8_t b_first)
{
	int i = 0, j = 0;
	int param_print = 0;
	uint32_t qdc[4] = { 0 }, enq_counter = 0, deq_counter = 0;
	struct tmu_egress_port_thr epth;
	int offset = 0, buf_sb_blk_start_print_f =
	    0, buf_sb_blk_last_print_f = 0;
	int buf_sb_blk_link_line_f = 0, buf_queue_th_counter = 0;
	struct tmu_token_bucket_shaper_params para = { 0 };
	struct tmu_equeue_drop_params thx = { 0 };
	struct tmu_eport_link eport_link;
	tmu_memset(&tmp_stat, 0, sizeof(tmp_stat));
	tmu_memset(&epth, 0, sizeof(epth));
	marking_queue();
#if 0

	for (i = 0; i < stat.qnum; i++) {
		printf("%03u ->", stat.q_info[i].qid);

		for (j = stat.max_lvl; j >= 0; j--) {
			if (stat.q_info[i].sb_info[j].filled)
				printf("SB%03u:%c%c%c->",
				       (stat.q_info[i].sb_info[j].sbin >> 3),
				       (stat.q_info[i].
					sb_info[j].flag & SB_BLOCK_START) ?
				       's' : ' ',
				       (stat.q_info[i].
					sb_info[j].flag & SB_BLOCK_END) ? 'e'
				       : ' ',
				       (stat.q_info[i].
					sb_info[j].flag & SB_BLOCK_LINK_LINE)
				       ? 'l' : ' ');
			else
				printf("           ");
		}

		printf("%02u\n", stat.q_info[i].epn);
	}

#endif

	if (b_first) {
		/*SEQ_PRINTF(s, "qnum=%d, max_lvl=%d\n", stat.qnum,  stat.max_lvl); */
		tmu_proc_printf(s,
				"           Qinfo:Col(4 colormap)QT(qth0,mith0~math0,mith1~math1,qth1)ept(epth0,1,2,3)\n");
		/*Print title like below format
		   Queue SBID:L    SBID:L    SBID:L    SBID:L    Port
		 */
		tmu_memset(tmp_stat.buf1, ' ', sizeof(tmp_stat.buf1) - 2);
		sprintf(tmp_stat.buf1, "Queue");
		offset = SB_BLOCK_OFFSET;

		for (j = stat.max_lvl; j >= 0; j--) {
			sprintf(tmp_stat.buf1 + offset, "SBID:L/W");
			offset += SPACE_PER_SB + SPACE_PER_BLOCK;
		}

		sprintf(tmp_stat.buf1 + offset, "Port\n");
		dp_replace_ch(tmp_stat.buf1, offset, 0, ' ');
		tmu_proc_printf(s, "%s", tmp_stat.buf1);
	}

	buf_queue_th_counter = 0;

	/*print queue info one by one */
	for (i = 0; i < stat.qnum; i++) {	/*port loop */
#if 0
		if (!stat.q_info[qid].filled)
			if (!flag_print_disable_q)
				continue;

#endif
		tmu_memset(tmp_stat.buf_sb_blk_start, ' ',
			   sizeof(tmp_stat.buf_sb_blk_start) - 2);
		tmu_memset(tmp_stat.buf1, ' ', sizeof(tmp_stat.buf1) - 2);
		tmu_memset(tmp_stat.buf_tm, ' ', sizeof(tmp_stat.buf_tm) - 2);
		tmu_memset(tmp_stat.buf_pir, ' ',
			   sizeof(tmp_stat.buf_pir) - 2);
		tmu_memset(tmp_stat.buf_cir, ' ',
			   sizeof(tmp_stat.buf_cir) - 2);
		tmu_memset(tmp_stat.buf_pbs, ' ',
			   sizeof(tmp_stat.buf_pbs) - 2);
		tmu_memset(tmp_stat.buf_cbs, ' ',
			   sizeof(tmp_stat.buf_cbs) - 2);
		tmu_memset(tmp_stat.buf_sb_blk_last, ' ',
			   sizeof(tmp_stat.buf_sb_blk_last) - 2);
		buf_sb_blk_start_print_f = 0;
		buf_sb_blk_last_print_f = 0;
		buf_sb_blk_link_line_f = 0;
		sprintf(tmp_stat.buf1, "%c%3d", stat.q_info[i].qe ? '+' : '-',
			stat.q_info[i].qid);
		offset = SB_BLOCK_OFFSET;
		tmu_memset(tmp_stat.buf1 + offset, '-', SPACE_PER_BLOCK);
		offset += SPACE_PER_BLOCK;
		*(tmp_stat.queueid_buf + buf_queue_th_counter) =
		    stat.q_info[i].qid;
		*(tmp_stat.queueid_en + buf_queue_th_counter) = 1;
		buf_queue_th_counter++;

		for (j = stat.max_lvl; j >= 0; j--) {
			if (stat.q_info[i].sb_info[j].filled) {
				if ((stat.q_info[i].
				     sb_info[j].flag & SB_BLOCK_START)
				    || i == 0)
					sprintf(tmp_stat.buf1 + offset,
						"|%-3d:%1d/%-4d",
						stat.q_info[i].
						sb_info[j].sbin >> 3,
						stat.q_info[i].sb_info[j].
						sbin % 8,
						stat.q_info[i].sb_info[j].
						iwgt);
				/*?? */
				else if ((stat.q_info[i].sb_info[j].qsid) !=
					 (stat.q_info[i - 1].sb_info[j].qsid))
					sprintf(tmp_stat.buf1 + offset,
						"|---:%1d/%-4d",
						stat.q_info[i].sb_info[j].
						sbin % 8,
						stat.q_info[i].sb_info[j].
						iwgt);
				else
					tmp_stat.buf1[offset] = '|';

				tmp_stat.buf1[offset + SPACE_PER_SB -
					      MINUS_RESV_SB_SPACE] = '|';

				if (stat.q_info[i].sb_info[j].tbe &&
				    ((stat.q_info[i].
				      sb_info[j].flag & SB_BLOCK_START)
				     || ((stat.q_info[i].sb_info[j].tbid) !=
					 (stat.q_info[i - 1].
					  sb_info[j].tbid)))) {
					param_print = 1;
					tmu_memset(&para, 0, sizeof(para));
					/*change */
					tmu_token_bucket_shaper_cfg_get(stat.
									q_info
									[i].
									sb_info
									[j].
									tbid,
									&para);
					/*para.cir *=8; change to bits/s */
					/*para.pir *=8; change to bits/s */
					/*para.cbs *=8; change to bits/s */
					/*para.pbs *=8; change to bits/s */
					/*TMU_PRINT("queue1
					   read tbs:%u_%u_%u_%u for
					   tbid %u\n", para.pir,
					   para.pbs, para.cir, para.cbs,
					   stat.q_info[i].sb_info[j].tbid);
					 */
					sprintf(tmp_stat.buf_tm + offset,
						"|   tid= %d",
						stat.q_info[i].sb_info[j].
						tbid);
					sprintf(tmp_stat.buf_pir + offset,
						"|   pir= %d%s", para.pir,
						para.tbe1 ? "" : "(X)");
					sprintf(tmp_stat.buf_cir + offset,
						"|   cir= %d%s", para.cir,
						para.tbe0 ? "" : "(X)");
					sprintf(tmp_stat.buf_pbs + offset,
						"|   pbs= %d", para.pbs);
					sprintf(tmp_stat.buf_cbs + offset,
						"|   cbs= %d", para.cbs);
				} else
					tmp_stat.buf_tm[offset] = '|';

				tmp_stat.buf_tm[offset + SPACE_PER_SB -
						MINUS_RESV_SB_SPACE] = '|';
				tmp_stat.buf_pir[offset] = '|';
				tmp_stat.buf_pir[offset + SPACE_PER_SB -
						 MINUS_RESV_SB_SPACE] = '|';
				tmp_stat.buf_cir[offset] = '|';
				tmp_stat.buf_cir[offset + SPACE_PER_SB -
						 MINUS_RESV_SB_SPACE] = '|';
				tmp_stat.buf_pbs[offset] = '|';
				tmp_stat.buf_pbs[offset + SPACE_PER_SB -
						 MINUS_RESV_SB_SPACE] = '|';
				tmp_stat.buf_cbs[offset] = '|';
				tmp_stat.buf_cbs[offset + SPACE_PER_SB -
						 MINUS_RESV_SB_SPACE] = '|';
			} else
				tmu_memset(tmp_stat.buf1 + offset, '-',
					   SPACE_PER_SB + SPACE_PER_BLOCK);

			if (stat.q_info[i].sb_info[j].flag & SB_BLOCK_START) {
				tmu_memset(tmp_stat.buf_sb_blk_start +
					   offset + 1, '-', SPACE_PER_SB - 2);
				buf_sb_blk_start_print_f = 1;
			} else if (stat.q_info[i].sb_info[j].filled) {
				tmp_stat.buf_sb_blk_start[offset] = '|';
				tmp_stat.buf_sb_blk_start[offset +
							  SPACE_PER_SB - 1] =
				    '|';
			}

			if (stat.q_info[i].sb_info[j].flag & SB_BLOCK_END) {
				tmu_memset(tmp_stat.buf_sb_blk_last + offset +
					   1, '-', SPACE_PER_SB - 2);
				buf_sb_blk_last_print_f = 1;
			} else if (stat.q_info[i].sb_info[j].filled) {
				tmp_stat.buf_sb_blk_last[offset] = '|';
				tmp_stat.buf_sb_blk_last[offset +
							 SPACE_PER_SB - 1] =
				    '|';
			}

			if (stat.q_info[i].sb_info[j].
			    flag & SB_BLOCK_LINK_LINE) {
				tmu_memset(tmp_stat.buf1 + offset +
					   SPACE_PER_SB, '-',
					   SPACE_PER_BLOCK);
				/*only keep last valid SB_BLOCK_LINK_LINE flag */
				buf_sb_blk_link_line_f = 1;
			} else
				buf_sb_blk_link_line_f = 0;

			offset += SPACE_PER_SB + SPACE_PER_BLOCK;
		}

		/*add\n chacter at the end of buffer */
		sprintf(tmp_stat.buf_sb_blk_start + offset, "\n");

		if (buf_sb_blk_link_line_f) {
			char *flag_s;
			char flag_buf[20];
			tmu_egress_port_link_get(stat.q_info[i].epn,
						 &eport_link);
			flag_s =
			    get_dma_flags_str(stat.q_info[i].epn, flag_buf,
					      sizeof(flag_buf));
			sprintf(tmp_stat.buf1 + offset, "%2d%s%s\n",
				stat.q_info[i].epn,
				eport_link.epe ? "" : "(disabled)",
				flag_s ? flag_s : "");
		} else
			sprintf(tmp_stat.buf1 + offset, "\n");

		sprintf(tmp_stat.buf_tm + offset, "\n");
		sprintf(tmp_stat.buf_pir + offset, "\n");
		sprintf(tmp_stat.buf_cir + offset, "\n");
		sprintf(tmp_stat.buf_pbs + offset, "\n");
		sprintf(tmp_stat.buf_cbs + offset, "\n");
		sprintf(tmp_stat.buf_sb_blk_last + offset, "\n");
		/*remove \0 in-between of the buffer */
		dp_replace_ch(tmp_stat.buf_sb_blk_start, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf1, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf_tm, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf_pir, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf_cir, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf_pbs, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf_cbs, offset, 0, ' ');
		dp_replace_ch(tmp_stat.buf_sb_blk_last, offset, 0, ' ');

		/*print the buffer */
		if (buf_sb_blk_start_print_f)
			tmu_proc_printf(s, "%s", tmp_stat.buf_sb_blk_start);

		tmu_proc_printf(s, "%s", tmp_stat.buf1);

		if (param_print) {
			tmu_proc_printf(s, "%s", tmp_stat.buf_tm);
			tmu_proc_printf(s, "%s", tmp_stat.buf_pir);
			tmu_proc_printf(s, "%s", tmp_stat.buf_cir);
			tmu_proc_printf(s, "%s", tmp_stat.buf_pbs);
			tmu_proc_printf(s, "%s", tmp_stat.buf_cbs);
		}

		if (buf_sb_blk_last_print_f)
			tmu_proc_printf(s, "%s", tmp_stat.buf_sb_blk_last);
	}

	if (1) {
		for (i = 0; *(tmp_stat.queueid_en + i) != 0; i++) {
			if ((i != 0)
			    && (*(tmp_stat.queueid_buf + (i - 1)) ==
				*(tmp_stat.queueid_buf + i)))
				i++;
			else {
				tmu_equeue_drop_params_get(*
							   (tmp_stat.queueid_buf
							    + i), &thx);
				tmu_qdct_read(*(tmp_stat.queueid_buf + i),
					      qdc);
				for (j = 0; j < stat.qnum; j++) {
					tmu_egress_port_tail_drop_thr_get
					    (stat.q_info[j].epn, &epth);
				}
				tmu_memset(tmp_stat.buf_queue_th, ' ',
					   sizeof(tmp_stat.buf_queue_th) - 2);
				enq_counter =
				    get_enq_counter(*
						    (tmp_stat.queueid_buf +
						     i));
				deq_counter =
				    get_deq_counter(*
						    (tmp_stat.queueid_buf +
						     i));

				if (thx.dmod)
					tmu_proc_printf(s,
							"           Q%03d:col(%d_%d_%d_%d)QT(%d,%d~%d,%d~%d,%d)ept(%d,%d,%d,%d)\n",
							*(tmp_stat.
							  queueid_buf + i),
							thx.col[0],
							thx.col[1],
							thx.col[2],
							thx.col[3], thx.qtth0,
							thx.mith0, thx.math0,
							thx.mith1, thx.math1,
							thx.qtth1,
							epth.epth[0],
							epth.epth[1],
							epth.epth[2],
							epth.epth[3]);
				else
					tmu_proc_printf(s,
							"           Q%03d:col(%d_%d_%d_%d)QT(%d,%d,%d,%d)ept(%d,%d,%d,%d)\n",
							*(tmp_stat.
							  queueid_buf + i),
							thx.col[0],
							thx.col[1],
							thx.col[2],
							thx.col[3], thx.qtth0,
							thx.math0, thx.math1,
							thx.qtth1,
							epth.epth[0],
							epth.epth[1],
							epth.epth[2],
							epth.epth[3]);

				param_print = 0;
			}
		}
	}
	return 0;
}

int collect_queue_info(struct seq_file *s, uint32_t *q_buff, int num)
{
	int i, index;
	int loops;
	struct tmu_sched_blk_out_link olink;
	struct tmu_sched_blk_in_link ilink;
	struct tmu_equeue_link qlink;
	struct tmu_token_bucket_shaper_params para;
	uint16_t lvl, sbid, sbin, max_lvl, leaf, qnum;
	uint16_t epn;
	uint8_t last_sb_per_queue = 1;
	qnum = 0;
	max_lvl = 0;
	tmu_memset(&stat, 0, sizeof(stat));
	i = 0;

	/*find all queue under exgress port epn */
	for (index = 0; index < num; index++) {
		loops = 0;
		tmu_equeue_link_get(q_buff[index], &qlink);
		last_sb_per_queue = 1;
		stat.q_info[i].filled = 1;
		stat.q_info[i].qid = q_buff[index];
		stat.q_info[i].qe = qlink.qe;
		stat.q_info[i].epn = qlink.epn;
		epn = qlink.epn;
		sbin = qlink.sbin;
		sbid = sbin >> 3;
		leaf = sbin % 8;

		while (1) {
			tmu_sched_blk_out_link_get(sbid, &olink);
			lvl = olink.lvl;

			if (lvl > tmu_max_lvl)
				TMU_PRINT
				    ("why lvl %u bigger than the maximum lvl %u\n",
				     lvl, tmu_max_lvl);

			if (stat.q_info[i].sb_info[lvl].filled)
				TMU_PRINT
				    ("why stat.q_info[%u].sb_info[%u].filled=%u already\n",
				     i, lvl,
				     stat.q_info[i].
				     sb_info[lvl].filled);

			stat.q_info[i].sb_info[lvl].filled = 1;
			stat.q_info[i].sb_info[lvl].soe = olink.soe;
			stat.q_info[i].sb_info[lvl].sbin = sbin;
			/*get sbit info */
			tmu_sched_blk_in_link_get(sbin, &ilink);
			stat.q_info[i].sb_info[lvl].sie = ilink.sie;
			stat.q_info[i].sb_info[lvl].sit = ilink.sit;
			stat.q_info[i].sb_info[lvl].tbe = ilink.tbe;
			stat.q_info[i].sb_info[lvl].iwgt = ilink.iwgt;
			stat.q_info[i].sb_info[lvl].qsid = ilink.qsid;
			stat.q_info[i].sb_info[lvl].tbid = ilink.tbid;
#if 0

			if (ilink.tbe)
				TMU_PRINT("ilink[%d].tbid=%d\n", sbin,
					  ilink.tbid);

			TMU_PRINT("ilink[%d].iwgt=%d\n", sbin,
				  ilink.iwgt);
#endif

			/*check tb's sbid whether correct or not ? */
			if (ilink.tbe) {
				uint32_t tmp_sbin;
				tmu_token_bucket_shaper_link_get
				    (ilink.tbid, &tmp_sbin);
				/*change */
				tmu_token_bucket_shaper_cfg_get(ilink.
								tbid,
								(struct
								 tmu_token_bucket_shaper_params
								 *)
								&para);

				if (tmp_sbin != sbin) {
					SEQ_PRINTF(s,
						   "Wrong sbin in tocken bucket cfg (%d) with current sbid(%d)\n",
						   tmp_sbin, sbin);
					/*break;
					   don't know it will
					   hang sometimes if it happens
					 */
				}
			}

			if (ilink.sie) {
				/*sit 1 means sbid */
				if (ilink.sit) {
					struct tmu_sched_blk_out_link
					    tmp_olink;
					tmu_sched_blk_out_link_get
					    (ilink.qsid, &tmp_olink);

					if (sbin != tmp_olink.omid)
						SEQ_PRINTF(s,
							   "Wrong qsid mapping: ilink.qsid=%d and sbid/sbin=%d/%d tmp_olink.sbid=%d\n",
							   ilink.qsid,
							   sbid, sbin,
							   tmp_olink.omid);
				}
				/*sit 0 means qid */
				else if (ilink.qsid != q_buff[index])
					SEQ_PRINTF(s,
						   "Wrong qsid configration in ilink(%d) and current queue id (%d)\n",
						   ilink.qsid, i);
			}

			if (!olink.v) {
				/*OMID means EPN. End of the search */
				if (olink.omid != epn &&
				    epn != EGRESS_PORT_ID_MAX) {
					SEQ_PRINTF(s,
						"Wrong EPN:olink(%d) epn(%d)\n",
						   olink.omid, epn);
				}

				/*stat.q_info[i].max_lvl =
				   max_lvl_per_queue;
				 */
				break;
			} else {
				/*prepare next SB */
				sbin = olink.omid;
				sbid = sbin >> 3;
				leaf = sbin % 8;

				/* SB hierarchy is not allowed more than
				   8 layers
				 */
				loops++;

				if (loops >= 8) {
					TMU_PRINT("Why QoS over 8 layers)\n");
					break;
				}
			}

			/*printf("stat.q_info[0].epn1=%u\n"
			   , stat.q_info[i].epn);
			 */
		}	/*egress port */

		i++;
	}

	stat.qnum = i;
	stat.max_lvl = tmu_max_lvl;
	/*if (stat.qnum)
	   printf("Get %u queus for port %u with max_lvl=%u\n"
	   , stat.qnum, stat.q_info[0].epn , stat.max_lvl);
	 */
	return i;
}

#define QUEUE_SB_BLOCK_OFFSET 8
#define QUEUE_SPACE_PER_SB 24
#define QUEUE_SPACE_PER_BLOCK 3
#define QUEUE_MINUS_INDET_TM  0
#define QUEUE_MINUS_RESV_SB_SPACE 1
char queue_print_buf[200];

int get_max_sb_lvl(void)
{
	int i;
	int max_lvl = 0;
	struct tmu_sched_blk_out_link olink;

	for (i = 0; i < SCHEDULER_BLOCK_ID_MAX; i++) {
		tmu_sched_blk_out_link_get(i, &olink);

		if (olink.soe) {
			if (olink.lvl > max_lvl)
				max_lvl = olink.lvl;
		}
	}

	return max_lvl;
}

int tmu_queue_dump_start(void)
{
	tmu_memset(&stat, 0, sizeof(stat));
	tmu_memset(queue_print_buf, 0, sizeof(queue_print_buf));
	first_print = 1;
	tmu_max_lvl = get_max_sb_lvl();
	return 0;
}

void tmu_queue_raw_dump_sb(uint32_t sbid, uint32_t curr_lvl)
{
	int offset, len;
	tmu_memset(queue_print_buf, ' ', sizeof(queue_print_buf) - 2);
	queue_print_buf[sizeof(queue_print_buf) - 1] = 0;
	offset = QUEUE_SB_BLOCK_OFFSET + curr_lvl * QUEUE_SPACE_PER_SB;
	queue_print_buf[offset] = '|';
	offset++;
	len = sprintf(queue_print_buf + offset, "%03u", sbid);
	queue_print_buf[offset + len] = ' ';	/*remove \0 */
	offset += QUEUE_SPACE_PER_SB - 1;
	queue_print_buf[offset] = '|';
	offset++;
	queue_print_buf[offset] = 0;
	TMU_PRINT("%s\n", queue_print_buf);
}

void tmu_queue_raw_print(uint32_t qid, uint32_t curr_lvl)
{
	int offset, len;
	tmu_memset(queue_print_buf, ' ', sizeof(queue_print_buf) - 2);
	queue_print_buf[sizeof(queue_print_buf) - 1] = 0;
	offset = QUEUE_SB_BLOCK_OFFSET + curr_lvl * QUEUE_SPACE_PER_SB;
	len = sprintf(queue_print_buf + offset, "->%03u", qid);
	offset += QUEUE_SB_BLOCK_OFFSET;
	queue_print_buf[offset] = 0;
	TMU_PRINT("%s\n", queue_print_buf);
}

#define NON_VALID_SB -1
void tmu_queue_raw_recursive(uint32_t tmu_egp, uint32_t sbid,
			     uint32_t curr_lvl, uint32_t *q_buff,
			     int q_buff_size, int *q_num)
{
	int i, k = sbid << 3;
	int j;
	uint32_t tmp_sbid[8];
	struct tmu_sched_blk_in_link sbit;
	struct tmu_sched_blk_out_link olink;

	if (curr_lvl >= SCHEDULE_MAX_LEVEL) {
		TMU_PRINT
		    ("Over 8 scheduler level not supported for tmu port %u\n",
		     tmu_egp);
		return;
	}
	tmu_sched_blk_out_link_get(sbid, &olink);
	if (olink.lvl != curr_lvl) {
		TMU_PRINT("Wrong lvl(%d) in sb(%d) of port %u.shoudl be %d\n",
		     olink.lvl, sbid, tmu_egp, curr_lvl);
		return;
	}

	/*tmu_queue_raw_dump_sb(sbid, curr_lvl); */
	for (i = 0; i < 8; i++, k++) {
		/*traversal each leaf based on current sbid */
		tmu_sched_blk_in_link_get(k, &sbit);
		tmp_sbid[i] = (uint32_t) (NON_VALID_SB);

		/*If scheduler Input is disabled, then check next sbin,ie,k+1 */
		if (sbit.sie == 0)
			continue;

		/*SGH:  means the input is SBID, not QID, the
		   continue search its parent SB's queue
		 */
		if (sbit.sit == 1) {
			/*iterate all its parent sbid and enable the enqueue
			   accordingly
			 */
			tmp_sbid[i] = sbit.qsid;
			TMU_DEBUG(TMU_DEBUG_MSG,
				"current sbid=%u lf=%u,sbid=%u tmu_port=%u\n",
				  sbid, i, sbit.qsid, tmu_egp);
			tmu_queue_raw_recursive(tmu_egp, sbit.qsid,
						curr_lvl + 1, q_buff,
						q_buff_size, q_num);
			continue;
		}
		if (*q_num >= (q_buff_size - 1)) {
			PR_ERR("buffer %d not enough for tmu port %u\n",
			       q_buff_size, tmu_egp);
			break;
		}
		TMU_DEBUG(TMU_DEBUG_MSG,
			"sbid(%u) leaf(%d) is %s.queue is %u in tmu port %u\n",
			  sbid, i, sbit.sie ? "enabled" : "disabled",
			  sbit.qsid, tmu_egp);
		q_buff[(*q_num)++] = sbit.qsid;
	}
	for (i = 0; i < 8; i++) {
		if (tmp_sbid[i] == -1)
			continue;
		for (j = 0; j < i; j++) {
			if (tmp_sbid[j] == NON_VALID_SB)
				continue;
			if (tmp_sbid[j] != tmp_sbid[i])
				continue;
			TMU_DEBUG(TMU_DEBUG_MSG,
				  "Wrong config:leaf(%u,%u) in SBID(%u) %u same as SBID(%u)\n",
				  i, j, sbid, tmu_egp, tmp_sbid[j]);
		}
	}
}

int tmu_queue_raw_dump(struct seq_file *s, int pos)
{
	int i = pos + proc_queue_start_id;
	struct tmu_global_thr thr;

	if (!tmu_is_enabled()) {
		TMU_PRINT("TMU is disabled.\n");
		pos = -1;	/*end of the loop */
		return pos;
	}

	/*if (!epmt.epe) goto exit; */

	tmu_strcpy(proc_pre_align_buf, "        ");
	tmu_proc_eqt_print(s, i, 0);
	if (g_queue_dump == 1) {
		SEQ_PRINTF(s, "\n");
	}
	if (seq_has_overflowed(s))
		return pos;
	pos++;
	if ((pos + proc_queue_start_id >= EGRESS_QUEUE_ID_MAX) ||
	    (pos + proc_queue_start_id > proc_queue_end_id)) {

		tmu_global_tail_drop_thr_get(&thr);
		tmu_proc_printf(s, "           goth= (%d, %d, %d, %d)\n",
				thr.goth[0], thr.goth[1], thr.goth[2],
				thr.goth[3]);
		pos = -1;	/*end of the loop */
	}
	return pos;
}

int tmu_queue_dump(struct seq_file *s, int pos)
{
	struct tmu_global_thr thr;
	int i;
	/*TMU_PRINT("pos=%d\n", pos); */
	/*pos=6;  test */
	/*if (!epmt.epe) goto exit; */
	q_buff_num = 0;
	stat.qnum = 0;
	tmu_queue_list(pos, tmp_q_buff, ARRAY_SIZE(tmp_q_buff), &q_buff_num,
		       s);
	if (q_buff_num <= 0)
		goto EXIT;
	collect_queue_info(s, tmp_q_buff, q_buff_num);

	if (stat.qnum <= 0)
		goto EXIT;

	if (stat.qnum >= 80) {
		TMU_PRINT("Too many queue (%u) for egress port %u\n",
			  q_buff_num, pos);
		pos = EGRESS_PORT_ID_MAX;
		goto EXIT;
	}

	for (i = 0; i < stat.qnum; i++) {
		if (stat.q_info[i].epn != pos) {
			TMU_DEBUG(TMU_DEBUG_MSG,
				  "Wrong epn configured in queue[%d]\n", i);
			goto EXIT;
		}
	}

	/*Order the queues and link them together according to associated SBID */
	print_queue1(s, first_print);
	first_print = 0;
EXIT:
	pos++;
	if (stat.qnum)
		SEQ_PRINTF(s, "\n");
	if (pos >= EGRESS_PORT_ID_MAX) {
		tmu_global_tail_drop_thr_get(&thr);
		tmu_proc_printf(s, "           goth=(%d, %d, %d, %d)\n",
				thr.goth[0], thr.goth[1], thr.goth[2],
				thr.goth[3]);
	}
	if (seq_has_overflowed(s))
		pos -= 1; /* return original pos */
	if (pos >= EGRESS_PORT_ID_MAX) {
		pos = -1; /* end of the loop */
	}
	return pos;
}

int tmu_queue_list(int tmu_egp, uint32_t *q_buff, int q_buff_size,
		   int *q_num, struct seq_file *s)
{
	struct tmu_eport_link epmt;
	int i, j;

	if (!q_num || !q_buff || (tmu_egp >= EGRESS_PORT_ID_MAX))
		return -1;
	*q_num = 0;
	tmu_egress_port_link_get(tmu_egp, &epmt);
	tmu_queue_raw_recursive(tmu_egp, epmt.sbid, 0, q_buff, q_buff_size,
				q_num);
	if (*q_num <= 0)
		return 0;
	for (i = 0; i < *q_num; i++) {
		for (j = 0; j < *q_num; j++) {
			if (i == j)
				continue;
			if (q_buff[i] != q_buff[j])	/*normally should not same */
				continue;
			tmu_proc_printf(s,
				"Error: duplicated queue[%d] in tmu port %d\n",
					(int)q_buff[i], tmu_egp);
		}
	}
	return 0;
}
EXPORT_SYMBOL(tmu_queue_list);

struct tmu_dgb_info {
	char *cmd;
	char *description;
	uint32_t flag;
};

static struct tmu_dgb_info dbg_enable_mask_str[] = {
	{"err", "error print", TMU_DEBUG_ERR},
	{"dbg", "dbg print", TMU_DEBUG_MSG},

	/*the last one */
	{"all", "enable all debug", -1}
};

void tmu_proc_port_write_help(void)
{
	TMU_PRINT("usage for proc %s:\n", PROC_BASE PROC_FILE_EPT);
	TMU_PRINT("  echo help > %s\n", PROC_BASE PROC_FILE_EPT);
	TMU_PRINT
	    ("  Delete all queues via egress port: echo del port_id > %s\n",
	     PROC_BASE PROC_FILE_EPT);
	TMU_PRINT
	    ("         if port_id is -1, then delete all queues    > %s\n",
	     PROC_BASE PROC_FILE_EPT);
}

ssize_t tmu_proc_port_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
	char *param_list[20] = { 0 };
	int len;
	char str[64];
	int value = 0, num = 0;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (!len)
		return count;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num <= 1 || dp_strncmpi(param_list[0], "help", strlen("help")) == 0)
		tmu_proc_port_write_help();
	else if (dp_strncmpi(param_list[0], "del", strlen("del")) == 0) {
		value = dp_atoi(param_list[1]);

		if (value < 0) {
			int i;
			TMU_PRINT("Try to delete all TMU ports\n");

			for (i = 0; i < 72; i++)
				tmu_del_all(i);
		} else {
			TMU_PRINT("Try to delete TMU port %d\n", value);
			tmu_del_all(value);
		}
	} else
		tmu_proc_port_write_help();

	return count;
}

void tmu_proc_debug_dump(struct seq_file *s)
{
	int i;
	TMU_PRINT("tmu_debug_lvl=%x\n", tmu_debug_lvl);

	for (i = 0; i < ARRAY_SIZE(dbg_enable_mask_str) - 1; i++) {
		/*skip -1 */
		SEQ_PRINTF(s, "%-10s(%-20s):     %-5s\n",
			       dbg_enable_mask_str[i].cmd,
			       dbg_enable_mask_str[i].description,
			       (tmu_debug_lvl & dbg_enable_mask_str[i].flag) ?
			       "enabled" : "disabled");
	}
}

ssize_t tmu_proc_debug_write(struct file *file, const char *buf, size_t count,
			     loff_t *ppos)
{
	char *param_list[3] = { 0 };
	int len;
	char str[64];
	int f_enable = 0;
	int i, num = 0;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (!len)
		return count;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num != 2) {
		TMU_PRINT("Wrong parameter:\n");
		TMU_PRINT("echo <enable/disable> [");

		for (i = 0; i < ARRAY_SIZE(dbg_enable_mask_str); i++)
			TMU_PRINT("%s/", dbg_enable_mask_str[i].cmd);

		TMU_PRINT("]\n");
	} else if (dp_strncmpi(param_list[0], "enable", strlen("enable")) == 0)
		f_enable = 1;
	else if (dp_strncmpi(param_list[0], "disable", strlen("disable")) == 0)
		f_enable = -1;
	else if (dp_strncmpi(param_list[0], "help", strlen("help")) == 0) {
		TMU_PRINT("echo <enable/disable> [");

		for (i = 0; i < ARRAY_SIZE(dbg_enable_mask_str); i++)
			TMU_PRINT("%s/", dbg_enable_mask_str[i].cmd);

		TMU_PRINT("]\n");
	}

	if (f_enable) {
		if (dp_strncmpi(param_list[1], "all", strlen("all")) == 0) {
			if (f_enable > 0)
				tmu_debug_lvl |= TMU_DEBUG_ENABLE_MASK_ALL;
			else
				tmu_debug_lvl &= ~TMU_DEBUG_ENABLE_MASK_ALL;
		} else {
			for (i = 0; i < ARRAY_SIZE(dbg_enable_mask_str); i++) {
				if (dp_strncmpi
				    (param_list[1],
				     dbg_enable_mask_str[i].cmd, strlen(dbg_enable_mask_str[i].cmd)) == 0) {
					if (f_enable > 0)
						tmu_debug_lvl |=
						    dbg_enable_mask_str[i].
						    flag;
					else
						tmu_debug_lvl &=
						    ~dbg_enable_mask_str
						    [i].flag;

					break;
				}
			}
		}
	}

	return count;
}

#define CBM_CLK_ADDR  0xb6200008

struct cbm_clk {
	char *clk_str;
	u8 clk_v;
};
struct cbm_clk cbm_clk_list[] = {
	{"37.5", 0xf}, {"50", 0xe}, {"60", 0xd}, {"75", 0xc},
	{"100", 0xb}, {"120", 0xa}, {"150", 0x9}, {"200", 0x8},
	{"240", 0x7}, {"300", 0x6}, {"400", 0x5}, {"480", 0x4},
	{"600", 0x3}, {"800", 0x2}, {"1200", 0x1}
};

void tmu_proc_clk_dump(struct seq_file *s)
{
	u32 clk_addr;
	u32 i;
	clk_addr = reg_r32((void *)CBM_CLK_ADDR);
	clk_addr &= 0xF;	/*last 4 bits */
	PRINTK("CGO value=0x%x\n", clk_addr);
	for (i = 0; i < ARRAY_SIZE(cbm_clk_list); i++) {
		if (clk_addr == cbm_clk_list[i].clk_v) {
			TMU_PRINT("cbm clock: %s MHz TMU_CORECLOCK:%d\n",
				  cbm_clk_list[i].clk_str, TMU_CORECLOCK);
			break;
		}
	}
}

ssize_t tmu_proc_clk_write(struct file *file, const char *buf, size_t count,
			   loff_t *ppos)
{
	int len;
	char str[64];
	u32 clk = 0, reg_v, i;
	u32 match = 0;
	struct tmu_token_bucket_shaper_params *shaper;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (!len)
		goto HELP;

	reg_v = reg_r32((void *)CBM_CLK_ADDR);
	PRINTK("old cbm reg_v=0x%x\n", reg_v);
	reg_v &= ~0xF;		/*remove last 4 bits */

	for (i = 0; i < ARRAY_SIZE(cbm_clk_list); i++) {
		if (strncmp(str, cbm_clk_list[i].clk_str, (len - 1)) == 0) {
			reg_v |= cbm_clk_list[i].clk_v;
			match = 1;
			break;
		}
	}
	clk = dp_atoi(str);
	PRINTK("clk=%u\n", clk);

	shaper = (struct tmu_token_bucket_shaper_params *)
	    kmalloc((sizeof(struct tmu_token_bucket_shaper_params) *
		     TOKEN_BUCKET_MAX), GFP_KERNEL);
	if (shaper) {
		for (i = 0; i < TOKEN_BUCKET_MAX; i++)
			tmu_token_bucket_shaper_cfg_get(i, &shaper[i]);
	} else {
		TMU_PRINT("%s Error in allocating memory\n", __func__);
		return -ENOMEM;
	}
	if (match) {
#if 1
		PRINTK("new cbm reg_v=0x%x\n", reg_v);
		reg_w32(reg_v, (void *)CBM_CLK_ADDR);
#else
		cbm_clk = clk_get_ppe();
		PR_INFO("CBM Clock: %ldHz=%ldMHZ\n", clk_get_rate(cbm_clk),
			clk_get_rate(cbm_clk) / MHZ);
		cbm_clk->set_rate(cbm_clk * MHZ);
		PR_INFO("CBM Clock: %ldHz=%ldMHZ\n", clk_get_rate(cbm_clk),
			clk_get_rate(cbm_clk) / MHZ);
#endif
		TMU_CORECLOCK = clk;
		for (i = 0; i < TOKEN_BUCKET_MAX; i++) {
			if (shaper[i].pir != 0 && shaper[i].cir != 0) {
				tmu_token_bucket_shaper_cfg_set(i,
								&shaper[i]);
			}
		}
		kfree(shaper);
	} else {
		kfree(shaper);
 HELP:
		PRINTK("Wrong clk %d. Supported clock in Mhz is:\n", clk);

		for (i = 0; i < ARRAY_SIZE(cbm_clk_list); i++)
			PRINTK("%s ", cbm_clk_list[i].clk_str);

		PRINTK("\n");
	}
	return count;
}

void tmu_egress_queue_delete(const uint32_t qid)
{
	struct tmu_equeue_link queue_link;
	uint32_t sbit[4];

	/*TMU Register: QTHT0 - Queue Threshold Table Register 0
	* (Set 0 to QE field to disable the queue)
	* FIRST : disable Queue
	*/
	tmu_equeue_enable(qid, false);

	/*TMU Register: QEMT - Queue Egress Mapping Table Register
	   (Reset the EPN field with Default/NONE valid EPN value)
	   TMU Register: QSMT - Queue Scheduler Mapping Table Register
	   (Reset the SBIN field with Default/NONE valid SBIN value)
	*/

	/*Disable qid's sbin link input and write
	   none valid id to it's QSID field
	 */
	tmu_equeue_link_get(qid, &queue_link);
	tmu_sbit_read(queue_link.sbin, &sbit[0]);

	/*disable first */
	sbit[0] &= ~TMU_SBITR0_SIE_EN;
	tmu_sbit_write(queue_link.sbin, &sbit[0]);

	/*next set non-valid queue id for next SB */
	set_val(sbit[0], NULL_QUEUE_ID, TMU_SBITR0_QSID_MASK,
		TMU_SBITR0_QSID_OFFSET);
	tmu_sbit_write(queue_link.sbin, &sbit[0]);

	/*Reset the QEMT and QSMT to non-vaild EPN/SB */
	queue_link.epn = EPNNULL_EGRESS_PORT_ID;
	queue_link.sbin = NULL_SCHEDULER_INPUT_ID;
	tmu_equeue_link_set(qid, &queue_link);
}
EXPORT_SYMBOL(tmu_egress_queue_delete);

int tmu_sched_blk_delete(const uint32_t sbid)
{
	uint32_t sbot[2];
	uint32_t sbit[4];
	uint32_t sbin;
	uint32_t leaf;
	uint32_t omid;
	uint32_t tbid;
	uint8_t v;
	struct tmu_sched_blk_in_link sb_inlink;

	/*check SB inputs (read sbit0 for every leaf)
	   get qsid from sbit
	   if any of the SB inlink has valid SB/Queue id
	   SB can not be removed/deleted
	   return ERROR
	 */
	for (leaf = 0; leaf < 8; leaf++) {
		sbin = (sbid << 3) + leaf;
		/* read input table reg for SB leaf) */
		tmu_sched_blk_in_link_get(sbin, &sb_inlink);

		if (sb_inlink.qsid != NULL_QUEUE_ID) {
			TMU_PRINT
			    ("SB[sbid=%d]-in_link has a vaild SB/Queue ID%d\n",
			     sbid, sb_inlink.qsid);
			return ERROR_SB_CANNOT_BE_REMOVED;
		}
	}

	/* Disable scheduler block output */
	/* read sbot */
	tmu_sbot_read(sbid, &sbot[0]);
	sbot[0] &= ~TMU_SBOTR0_SOE_EN;
	set_val(sbot[0], 0, TMU_SBOTR0_LVL_MASK, TMU_SBOTR0_LVL_OFFSET);
	tmu_sbot_write(sbid, &sbot[0]);

	/* disable next SB's input link or disable port */
	v = (sbot[0] & TMU_SBOTR0_V_SBIN) ? true : false;
	omid = get_val(sbot[0], TMU_SBOTR0_OMID_MASK, TMU_SBOTR0_OMID_OFFSET);

	if (v == 0) {
		tmu_egress_port_enable(omid, false);
		tmu_egress_port_link_set(omid, NULL_SCHEDULER_BLOCK_ID);
	} else {
		tmu_sbit_read(omid, &sbit[0]);
		sbit[0] &= ~TMU_SBITR0_SIE_EN;
		tmu_sbit_write(omid, &sbit[0]);
		set_val(sbit[0], NULL_QUEUE_ID, TMU_SBITR0_QSID_MASK,
			TMU_SBITR0_QSID_OFFSET);
		tmu_sbit_write(omid, &sbit[0]);
	}

	/* Disable all scheduler block inputs for this scheduler (0-7) */
	/*TMU Register: SBITR0 - SB Input Table Register 0
	   (Reset the QSID field with Default/NONE valid QID value
	   and Set 0 to SIE field)
	 */
	for (leaf = 0; leaf < 8; leaf++) {
		sbin = (sbid << 3) + leaf;
		/* read SB input table reg for SB leaf) */
		tmu_sbit_read(sbin, &sbit[0]);
		/* FIRST : disable SB input */
		sbit[0] &= ~TMU_SBITR0_SIE_EN;
		tmu_sbit_write(sbin, &sbit[0]);

		/*NEXT : write none valid id to QSID field */
		set_val(sbit[0], NULL_QUEUE_ID, TMU_SBITR0_QSID_MASK,
			TMU_SBITR0_QSID_OFFSET);
		tmu_sbit_write(sbin, &sbit[0]);
		/* delete token buket attached to the sb leaf */
		tbid =
		    get_val(sbit[1], TMU_SBITR1_TBID_MASK,
			    TMU_SBITR1_TBID_OFFSET);

		if (tbid != NULL_TOCKEN_BUCKET_ID) {
			TMU_PRINT("tbid %d removed from sbin %d\n", tbid,
				  sbin);
			tmu_token_bucket_shaper_delete(tbid, sbin);
		}
	}

	return SB_DEL_SUCCESS;
}
EXPORT_SYMBOL(tmu_sched_blk_delete);

void tmu_proc_tmu_create_cascade_help(void)
{
	TMU_PRINT("Cascade Command Help:\n\n");
	TMU_PRINT("  SET WFQ(param): echo set_wfq wfq_value sbid:leaf > cascade\n");
	TMU_PRINT("  Delete Queue: echo del_q qid > cascade\n");
	TMU_PRINT("  Delete SB: echo del_sb sbid > cascade\n");
	TMU_PRINT("  Attach Token bucket: echo attach_tb tbid sbid:leaf > cascade\n");
	TMU_PRINT("  Remove Token bucket: echo remove_tb tbid sbid:leaf > cascade\n");
	TMU_PRINT("  CFG Token Bucket: echo cfg_tb tbid cir pir cbs pbs mode > cascade\n");
	TMU_PRINT("  Create Queue: echo create qid sbid:leaf sbid:leaf sbid:leaf epn > cascade\n\n");
}

ssize_t tmu_proc_tmu_cascade_write(struct file *file, const char *buf,
				   size_t count, loff_t *ppos)
{
	int len;
	char str[100];
	int sbit[SCHEDULE_MAX_LEVEL];
	int leaf[SCHEDULE_MAX_LEVEL];
	char *param_list[15] = { 0 };
	char *temp_list[3] = { 0 };
	int i;
	unsigned int LINK_A = 0, LINK_B = 0, LEAF, egress_port = 0, qid = 0;
	unsigned int level = 0, num = 0, tb = 0, sbid = 0;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (!len)
		return count;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	level = num - 3;

	if (num <= 1 || num > ARRAY_SIZE(param_list)
	    || dp_strncmpi(param_list[0],
			   "help",
			   strlen("help"))
			   == 0)
		tmu_proc_tmu_create_cascade_help();
	else if (dp_strncmpi(param_list[0],
			     "set_wfq",
				 strlen("set_wfq"))
				 == 0) {
		struct tmu_sched_blk_in_link ilink;

		if (num != 3) {
			TMU_PRINT
			    ("Wrong (try help): echo help > cascade\n");
			return count;
		}

		dp_replace_ch(param_list[2], tmu_strlen(param_list[2]), ':',
			      ' ');
		dp_split_buffer(param_list[2], temp_list,
				ARRAY_SIZE(temp_list));
		LINK_B = dp_atoi(temp_list[0]);

		if (LINK_B > (SCHEDULER_BLOCK_ID_MAX - 1)) {
			TMU_PRINT("Wrong Parameter: SBID[%d]>SB_ID =0~%d]\n",
			     LINK_B, (SCHEDULER_BLOCK_ID_MAX - 1));
			return count;
		}

		LEAF = dp_atoi(temp_list[1]);

		if (LEAF > (SCHEDULER_MAX_LEAF - 1)) {
			TMU_PRINT
			    ("Wrong: SBID:LEAF[%d] > [LEAF = 0 to 7]\n",
			     LEAF);
			return count;
		}

		tmu_sched_blk_in_link_get(((LINK_B + 0) << 3) + LEAF, &ilink);
		ilink.iwgt = dp_atoi(param_list[1]);

		if (ilink.iwgt >
		    (TMU_SBITR0_IWGT_MASK >> TMU_SBITR0_IWGT_OFFSET)) {
			TMU_PRINT("Wrong IWGT out of range [IWGT = 0 to 1023]\n");
			TMU_PRINT("IWGT set to MAX value = %d\n",
				  (TMU_SBITR0_IWGT_MASK >>
				   TMU_SBITR0_IWGT_OFFSET));
			ilink.iwgt =
			    (TMU_SBITR0_IWGT_MASK >> TMU_SBITR0_IWGT_OFFSET);
		}

		tmu_sched_blk_in_link_set(((LINK_B + 0) << 3) + LEAF, &ilink);
	} else if (dp_strncmpi(param_list[0],
				   "del_q",
				   strlen("del_q"))
				   == 0) {
		qid = dp_atoi(param_list[1]);
		tmu_egress_queue_delete(qid);
		TMU_PRINT("\nQueue %d deleted\n\n", qid);
	} else if (dp_strncmpi(param_list[0],
				   "del_sb",
				   strlen("del_sb"))
				   == 0) {
		sbid = dp_atoi(param_list[1]);

		if (tmu_sched_blk_delete(sbid)) {
			TMU_PRINT("\nCan not delete SB %d", sbid);
			TMU_PRINT("(one of the SB's leaf input link ");
			TMU_PRINT("has a vaild SB/Queue id)\n\n");
		} else
			TMU_PRINT("\nSB %d deleted\n\n", sbid);
	} else if (dp_strncmpi(param_list[0],
				   "attach_tb",
				   strlen("attach_tb"))
				   == 0) {
		if (num != 3) {
			TMU_PRINT("Wrong:echo help > cascade\n");
			return count;
		}

		dp_replace_ch(param_list[2], tmu_strlen(param_list[2]), ':',
			      ' ');
		dp_split_buffer(param_list[2], temp_list,
				ARRAY_SIZE(temp_list));
		LINK_B = dp_atoi(temp_list[0]);

		if (LINK_B > (SCHEDULER_BLOCK_ID_MAX - 1)) {
			TMU_PRINT("Wrong:SBID[%d]>[SB_ID=0 to %d]\n",
			     LINK_B, (SCHEDULER_BLOCK_ID_MAX - 1));
			return count;
		}

		LEAF = dp_atoi(temp_list[1]);

		if (LEAF > (SCHEDULER_MAX_LEAF - 1)) {
			TMU_PRINT("Wrong: SBID:LEAF[%d] > [LEAF = 0 to 7]\n",
			     LEAF);
			return count;
		}

		tb = dp_atoi(param_list[1]);

		if (tb > (TOCKEN_BUCKET_ID - 1)) {
			TMU_PRINT
			    ("Wrong Parameter: TBID[%d] > [TBID = 0 to %d]\n",
			     tb, (TOCKEN_BUCKET_ID - 1));
			return count;
		}

		tmu_token_bucket_shaper_create(tb, ((LINK_B << 3) + LEAF));
	} else if (dp_strncmpi(param_list[0],
				   "remove_tb",
				   strlen("remove_tb"))
				   == 0) {
		if (num != 3) {
			TMU_PRINT("Wrong: echo help > cascade\n");
			return count;
		}

		dp_replace_ch(param_list[2], tmu_strlen(param_list[2]), ':',
			      ' ');
		dp_split_buffer(param_list[2], temp_list,
				ARRAY_SIZE(temp_list));
		LINK_B = dp_atoi(temp_list[0]);

		if (LINK_B > (SCHEDULER_BLOCK_ID_MAX - 1)) {
			TMU_PRINT("Wrong:SD[%d]>SCHEDULER_BLOCK_ID_MAX=0~%d\n",
			     LINK_B, (SCHEDULER_BLOCK_ID_MAX - 1));
			return count;
		}

		LEAF = dp_atoi(temp_list[1]);

		if (LEAF > (SCHEDULER_MAX_LEAF - 1)) {
			TMU_PRINT("Wrong:SBID:LF[%d] > [LEAF = 0 to 7]\n",
			     LEAF);
			return count;
		}

		tb = dp_atoi(param_list[1]);

		if (tb > (TOCKEN_BUCKET_ID - 1)) {
			TMU_PRINT("Wrong:TB[%d]>[TBID=0~%d]\n",
			     tb, (TOCKEN_BUCKET_ID - 1));
			return count;
		}

		tmu_token_bucket_shaper_delete(tb, ((LINK_B << 3) + LEAF));
	} else if (dp_strncmpi(param_list[0],
				   "cfg_tb",
				   strlen("cfg_tb"))
				   == 0) {
		uint32_t tbid, cir, pir, cbs, pbs, mode;

		if (num < 7) {
			TMU_PRINT("Wrong Parameter(try help): echo help > cascade\n");
			return count;
		}

		tbid = dp_atoi(param_list[1]);

		if (tbid > (TOCKEN_BUCKET_ID - 1)) {
			TMU_PRINT
			    ("Wrong Parameter: TBID[%d] out of range [TBID = 0 to %d]\n",
			     tbid, (TOCKEN_BUCKET_ID - 1));
			return count;
		}

		cir = dp_atoi(param_list[2]);
		pir = dp_atoi(param_list[3]);
		cbs = dp_atoi(param_list[4]);
		pbs = dp_atoi(param_list[5]);
		mode = dp_atoi(param_list[6]);
		tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
	} else if (dp_strncmpi(param_list[0],
				   "create",
				   strlen("create"))
				   == 0) {
		if (num < 4) {
			TMU_PRINT("Wrong Parameter(try help): echo help > cascade\n");
			return count;
		}

		qid = dp_atoi(param_list[1]);

		if (qid > (EGRESS_QUEUE_ID_MAX - 1)) {
			TMU_PRINT("Wrong Parameter: QID[%d] out of range [QID = 0 to %d]\n",
			     qid, (EGRESS_QUEUE_ID_MAX - 1));
			return count;
		}

		egress_port = dp_atoi(param_list[num - 1]);

		if (egress_port > (EGRESS_PORT_ID_MAX - 1)) {
			TMU_PRINT("Wrong Parameter: egress_port[%d] out of range [egress_port = 0 to %d]\n",
			     egress_port, EGRESS_PORT_ID_MAX - 1);
			return count;
		}

		for (i = 2; ((num - 1) > i && i < 10); i++) {
			dp_replace_ch(param_list[i],
				      tmu_strlen(param_list[i]), ':', ' ');
			dp_split_buffer(param_list[i], temp_list,
					ARRAY_SIZE(temp_list));
			LINK_B = dp_atoi(temp_list[0]);
			LEAF = dp_atoi(temp_list[1]);
			sbit[i - 2] = LINK_B;
			leaf[i - 2] = LEAF;

			if (LINK_B > (SCHEDULER_BLOCK_ID_MAX - 1)) {
				TMU_PRINT("Wrong Parameter: SBID[%d] out of range [SCHEDULER_BLOCK_ID_MAX = 0 to %d]\n",
				     LINK_B, (SCHEDULER_BLOCK_ID_MAX - 1));
				return count;
			}

			if (LEAF > (SCHEDULER_MAX_LEAF - 1)) {
				TMU_PRINT("Wrong:SB[%d]:LF[%d]>[%d:LF=0~7\n",
				     LINK_B, LEAF, LINK_B);
				return count;
			}
		}

		for (i = 2; ((num - 1) > i && i < 10); i++) {
			LINK_B = sbit[i - 2];
			LEAF = leaf[i - 2];

			if (i == 2) {
				LINK_A = qid;
				TMU_PRINT("QID %d->SB %d:LEAF %d->", LINK_A,
					  LINK_B, LEAF);
				tmu_egress_queue_create(LINK_A,
							((LINK_B << 3) +
							 LEAF), egress_port);
				tmu_set_wfq(((LINK_B << 3) + LEAF), 0);
				level--;
			} else if (i != 2) {
				TMU_PRINT("SB %d:LEAF %d->", LINK_B, LEAF);
				tmu_sched_blk_create(LINK_A, level,
						     ((LINK_B << 3) + LEAF),
						     1, 0);
				level--;
			}

			LINK_A = LINK_B;
		}

		TMU_PRINT("EP %d\n", egress_port);
		tmu_sched_blk_create(LINK_A, level, egress_port, 0, 0);
		tmu_egress_port_enable(egress_port, true);
	} else {
		TMU_PRINT("Wrong Parameter:\n");
		tmu_proc_tmu_create_cascade_help();
	}

	return count;
}

ssize_t tmu_proc_queue_write(struct file *file, const char *buf,
			     size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *s[2] = { 0 };
	unsigned int num[2] = { -1 };
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (dp_split_buffer(str, s, 2) < 2)
		goto help;

	if (s[0] && tmu_strlen(s[0]))
		num[0] = dp_atoi(s[0]);

	if (s[1] && tmu_strlen(s[1]))
		num[1] = dp_atoi(s[1]);

	set_start_end_id(num[0], num[1], EGRESS_QUEUE_ID_MAX - 1,
			 EGRESS_QUEUE_ID_MAX - 1, 0, -1, &proc_queue_start_id,
			 &proc_queue_end_id);
	TMU_PRINT("proc_queue_start_id=%d, proc_queue_end_id=%d\n",
		  proc_queue_start_id, proc_queue_end_id);
	return count;

 help:
	TMU_PRINT("ussage echo start_id end_id > /proc/tmu/queue\n");
	TMU_PRINT("       then display sbot info via cat /proc/tmu/queue\n");
	return count;
}

ssize_t tmu_proc_sbit_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
	int len;
	char str[64];
	char *s[2] = { 0 };
	unsigned int num[2] = { -1 };
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (dp_split_buffer(str, s, 2) < 2)
		goto help;

	if (s[0] && tmu_strlen(s[0]))
		num[0] = dp_atoi(s[0]);

	if (s[1] && tmu_strlen(s[1]))
		num[1] = dp_atoi(s[1]);

	set_start_end_id(num[0], num[1], SCHEDULER_BLOCK_INPUT_ID_MAX - 1,
			 SCHEDULER_BLOCK_INPUT_ID_MAX - 1, 0, -1,
			 &proc_sbit_start_id, &proc_sbit_end_id);
	TMU_PRINT("proc_sbit_start_id=%d, proc_sbit_end_id=%d\n",
		  proc_sbit_start_id, proc_sbit_end_id);
	return count;

 help:
	TMU_PRINT("ussage echo start_id end_id > /proc/tmu/sbit\n");
	TMU_PRINT("       read selected sbot info via cat /proc/tmu/sbit\n");
	return count;
}

ssize_t tmu_proc_sbot_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
	int len;
	char str[64];
	char *s[2] = { 0 };
	unsigned int num[2] = { -1 };
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (dp_split_buffer(str, s, 2) < 2)
		goto help;

	if (s[0] && tmu_strlen(s[0]))
		num[0] = dp_atoi(s[0]);

	if (s[1] && tmu_strlen(s[1]))
		num[1] = dp_atoi(s[1]);

	set_start_end_id(num[0], num[1], SCHEDULER_BLOCK_ID_MAX - 1,
			 SCHEDULER_BLOCK_ID_MAX - 1, 0, -1,
			 &proc_sbot_start_id, &proc_sbot_end_id);
	TMU_PRINT("proc_sbot_start_id=%d, proc_sbot_end_id=%d\n",
		  proc_sbot_start_id, proc_sbot_end_id);
	return count;

 help:
	TMU_PRINT("ussage echo start_id end_id > /proc/tmu/sbit\n");
	TMU_PRINT("       read selected sbot info via cat /proc/tmu/sbot\n");
	return count;

}

ssize_t tmu_proc_tbst_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
	int len;
	char str[64];
	char *s[2] = { 0 };
	unsigned int num[2] = { -1 };
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (dp_split_buffer(str, s, 2) < 2)
		goto help;

	if (s[0] && tmu_strlen(s[0]))
		num[0] = dp_atoi(s[0]);

	if (s[1] && tmu_strlen(s[1]))
		num[1] = dp_atoi(s[1]);

	set_start_end_id(num[0], num[1], SCHEDULER_BLOCK_INPUT_ID_MAX - 1,
			 SCHEDULER_BLOCK_INPUT_ID_MAX - 1, 0, -1,
			 &proc_tbst_start_id, &proc_tbst_end_id);
	TMU_PRINT("proc_tbst_start_id=%d, proc_tbst_end_id=%d\n",
		  proc_tbst_start_id, proc_tbst_end_id);
	return count;
 help:
	TMU_PRINT("ussage echo start_id end_id > /proc/tmu/sbit\n");
	TMU_PRINT("       read selected sbot info via cat /proc/tmu/tbst\n");
	return count;

}

#endif				/* TMU_LOCAL_STAT_DEBUG */

/*Note: buf must be 19 bytes or puls */
void index_to_bin_lookup_bits(int index, char *buf)
{
	/*cbm lookup table format:
	   FlowID(2)       DEC ENC       MPE2 MPE1       EP        Class
	   2       space   1   1  space   1   1   space 4  space
	   4     = 14 + 4(space) = 18
	 */
#define LOOKUP_BITS_LEN 18
	int i = 0, offset = 0;

	if (!buf)
		return;

	tmu_strcpy(buf, "00 00 00 0000 0000");
	index &= 0x3FFF;
	offset = LOOKUP_BITS_LEN - 1;

	while (index > 0) {
		buf[offset] = index % 2 + '0';
		i++;
		offset--;
		index = index >> 1;

		if (i == 4 || i == 8 || i == 10 || i == 12) {
			/*slot in on space */
			buf[offset] = ' ';
			offset--;
		}
	}

	if (offset < -1)
		TMU_PRINT
		    ("Buffer overflow in index_to_bin_lookup_bits API:%d\n",
		     offset);
}

uint32_t read_write_reg_tbst(uint32_t set_cmd, uint32_t tbid,
			     char *tsbst_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_TBSTC_SEL | tbid, tbstc);

	while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
		continue;

	if (dp_strncmpi(tsbst_reg_name,
			"TBSTR0",
			strlen("TBSTR0"))
			== 0)
		reg_r_data = tmu_r32(tbstr0);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR1",
				 strlen("TBSTR1"))
				 == 0)
		reg_r_data = tmu_r32(tbstr1);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR2",
				 strlen("TBSTR2"))
				 == 0)
		reg_r_data = tmu_r32(tbstr2);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR3",
				 strlen("TBSTR3"))
				 == 0)
		reg_r_data = tmu_r32(tbstr3);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR4",
				 strlen("TBSTR4"))
				 == 0)
		reg_r_data = tmu_r32(tbstr4);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR5",
				 strlen("TBSTR5"))
				 == 0)
		reg_r_data = tmu_r32(tbstr5);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR6",
				 strlen("TBSTR6"))
				 == 0)
		reg_r_data = tmu_r32(tbstr6);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR7",
				 strlen("TBSTR7"))
				 == 0)
		reg_r_data = tmu_r32(tbstr7);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR8",
				 strlen("TBSTR8"))
				 == 0)
		reg_r_data = tmu_r32(tbstr8);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR9",
				 strlen("TBSTR9"))
				 == 0)
		reg_r_data = tmu_r32(tbstr9);
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR10",
				 strlen("TBSTR10"))
				 == 0)
		reg_r_data = tmu_r32(tbstr10);
	else {
		TMU_PRINT("Not valid register name %s?\n", tsbst_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(tsbst_reg_name,
			"TBSTR0",
			strlen("TBSTR0"))
			== 0)
		reg_r_data = simu_tbstr0[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR1",
				 strlen("TBSTR1"))
				 == 0)
		reg_r_data = simu_tbstr1[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR2",
				 strlen("TBSTR2"))
				 == 0)
		reg_r_data = simu_tbstr2[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR3",
				 strlen("TBSTR3"))
				 == 0)
		reg_r_data = simu_tbstr3[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR4",
				 strlen("TBSTR4"))
				 == 0)
		reg_r_data = simu_tbstr4[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR5",
				 strlen("TBSTR5"))
				 == 0)
		reg_r_data = simu_tbstr5[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR6",
				 strlen("TBSTR6"))
				 == 0)
		reg_r_data = simu_tbstr6[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR7",
				 strlen("TBSTR7"))
				 == 0)
		reg_r_data = simu_tbstr7[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR8",
				 strlen("TBSTR8"))
				 == 0)
		reg_r_data = simu_tbstr8[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR9",
				 strlen("TBSTR9"))
				 == 0)
		reg_r_data = simu_tbstr9[tbid];
	else if (dp_strncmpi(tsbst_reg_name,
			     "TBSTR10",
				 strlen("TBSTR10"))
				 == 0)
		reg_r_data = simu_tbstr10[tbid];
	else {
		TMU_PRINT("Not valid register name %s?\n", tsbst_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(tsbst_reg_name,
				"TBSTR0",
				strlen("TBSTR0"))
				== 0)
			tmu_w32(reg_w_data, tbstr0);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR1",
					 strlen("TBSTR1"))
					 == 0)
			tmu_w32(reg_w_data, tbstr1);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR2",
					 strlen("TBSTR2"))
					 == 0)
			tmu_w32(reg_w_data, tbstr2);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR3",
					 strlen("TBSTR3"))
					 == 0)
			tmu_w32(reg_w_data, tbstr3);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR4",
					 strlen("TBSTR4"))
					 == 0)
			tmu_w32(reg_w_data, tbstr4);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR5",
					 strlen("TBSTR5"))
					 == 0)
			tmu_w32(reg_w_data, tbstr5);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR6",
					 strlen("TBSTR6"))
					 == 0)
			tmu_w32(reg_w_data, tbstr6);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR7",
					 strlen("TBSTR7"))
					 == 0)
			tmu_w32(reg_w_data, tbstr7);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR8",
					 strlen("TBSTR8"))
					 == 0)
			tmu_w32(reg_w_data, tbstr8);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR9",
					 strlen("TBSTR9"))
					 == 0)
			tmu_w32(reg_w_data, tbstr9);
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR10",
					 strlen("TBSTR10"))
					 == 0)
			tmu_w32(reg_w_data, tbstr10);

		tmu_w32(TMU_TBSTC_RW_W | tbid, tbstc);

		while ((tmu_r32(tbstc) & TMU_TBSTC_VAL) == 0)
			continue;

#else

		if (dp_strncmpi(tsbst_reg_name,
				"TBSTR0",
				strlen("TBSTR0"))
				== 0)
			simu_tbstr0[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR1",
					 strlen("TBSTR1"))
					 == 0)
			simu_tbstr1[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR2",
					 strlen("TBSTR2"))
					 == 0)
			simu_tbstr2[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR3",
					 strlen("TBSTR3"))
					 == 0)
			simu_tbstr3[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR4",
					 strlen("TBSTR4"))
					 == 0)
			simu_tbstr4[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR5",
					 strlen("TBSTR5"))
					 == 0)
			simu_tbstr5[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR6",
					 strlen("TBSTR6"))
					 == 0)
			simu_tbstr6[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR7",
					 strlen("TBSTR7"))
					 == 0)
			simu_tbstr7[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR8",
					 strlen("TBSTR8"))
					 == 0)
			simu_tbstr8[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR9",
					 strlen("TBSTR9"))
					 == 0)
			simu_tbstr9[tbid] = reg_w_data;
		else if (dp_strncmpi(tsbst_reg_name,
				     "TBSTR10",
					 strlen("TBSTR10"))
					 == 0)
			simu_tbstr10[tbid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_sbit(uint32_t set_cmd, uint32_t sbin,
			     char *sbit_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_SBITC_SEL | sbin, sbitc);

	while ((tmu_r32(sbitc) & TMU_SBITC_VAL) == 0)
		continue;

	if (dp_strncmpi(sbit_reg_name,
			"SBITR0",
			strlen("SBITR0"))
			== 0)
		reg_r_data = tmu_r32(sbitr0);
	else if (dp_strncmpi(sbit_reg_name,
			     "SBITR1",
				 strlen("SBITR1"))
				 == 0)
		reg_r_data = tmu_r32(sbitr1);
	else if (dp_strncmpi(sbit_reg_name,
			     "SBITR2",
				 strlen("SBITR2"))
				 == 0)
		reg_r_data = tmu_r32(sbitr2);
	else if (dp_strncmpi(sbit_reg_name,
			     "SBITR3",
				 strlen("SBITR3"))
				 == 0)
		reg_r_data = tmu_r32(sbitr3);
	else {
		TMU_PRINT("Not valid register name %s?\n", sbit_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(sbit_reg_name,
			"SBITR0",
			strlen("SBITR0"))
			== 0)
		reg_r_data = simu_sbitr0[sbin];
	else if (dp_strncmpi(sbit_reg_name,
			     "SBITR1",
				 strlen("SBITR1"))
				 == 0)
		reg_r_data = simu_sbitr1[sbin];
	else if (dp_strncmpi(sbit_reg_name,
			     "SBITR2",
				 strlen("SBITR2"))
				 == 0)
		reg_r_data = simu_sbitr2[sbin];
	else if (dp_strncmpi(sbit_reg_name,
			     "SBITR3",
				 strlen("SBITR3"))
				 == 0)
		reg_r_data = simu_sbitr3[sbin];
	else {
		TMU_PRINT("Not valid register name %s?\n", sbit_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(sbit_reg_name,
				"SBITR0",
				strlen("SBITR0"))
				== 0)
			tmu_w32(reg_w_data, sbitr0);
		else if (dp_strncmpi(sbit_reg_name,
				     "SBITR1",
					 strlen("SBITR1"))
					 == 0)
			tmu_w32(reg_w_data, sbitr1);
		else if (dp_strncmpi(sbit_reg_name,
				     "SBITR2",
					 strlen("SBITR2"))
					 == 0)
			tmu_w32(reg_w_data, sbitr2);
		else if (dp_strncmpi(sbit_reg_name,
				     "SBITR3",
					 strlen("SBITR3"))
					 == 0)
			tmu_w32(reg_w_data, sbitr3);

		tmu_w32(TMU_SBITC_RW_W | TMU_SBITC_SEL | sbin, sbitc);

		while ((tmu_r32(sbitc) & TMU_SBITC_VAL) == 0)
			continue;

#else

		if (dp_strncmpi(sbit_reg_name,
				"SBITR0",
				strlen("SBITR0"))
				== 0)
			simu_sbitr0[sbin] = reg_w_data;
		else if (dp_strncmpi(sbit_reg_name,
				     "SBITR1",
					 strlen("SBITR1"))
					 == 0)
			simu_sbitr1[sbin] = reg_w_data;
		else if (dp_strncmpi(sbit_reg_name,
				     "SBITR2",
					 strlen("SBITR2"))
					 == 0)
			simu_sbitr2[sbin] = reg_w_data;
		else if (dp_strncmpi(sbit_reg_name,
				     "SBITR3",
					 strlen("SBITR3"))
					 == 0)
			simu_sbitr3[sbin] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_sbot(uint32_t set_cmd, uint32_t sbid,
			     char *sbot_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_SBOTC_SEL | sbid, sbotc);

	while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
		continue;

	if (dp_strncmpi(sbot_reg_name,
			"SBOTR0",
			strlen("SBOTR0"))
			== 0)
		reg_r_data = tmu_r32(sbotr0);
	else if (dp_strncmpi(sbot_reg_name,
			     "SBOTR1",
				 strlen("SBOTR1"))
				 == 0)
		reg_r_data = tmu_r32(sbotr1);
	else {
		TMU_PRINT("Not valid register name %s?\n", sbot_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(sbot_reg_name,
			"SBOTR0",
			strlen("SBOTR0"))
			== 0)
		reg_r_data = simu_sbotr0[sbid];
	else if (dp_strncmpi(sbot_reg_name,
			     "SBOTR1",
				 strlen("SBOTR1"))
				 == 0)
		reg_r_data = simu_sbotr1[sbid];
	else {
		TMU_PRINT("Not valid register name %s?\n", sbot_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(sbot_reg_name,
				"SBOTR0",
				strlen("SBOTR0"))
				== 0)
			tmu_w32(reg_w_data, sbotr0);
		else if (dp_strncmpi(sbot_reg_name,
				     "SBOTR1",
					 strlen("SBOTR1"))
					 == 0)
			tmu_w32(reg_w_data, sbotr1);

		tmu_w32(TMU_SBOTC_RW_W | TMU_SBOTC_SEL | sbid, sbotc);

		while ((tmu_r32(sbotc) & TMU_SBOTC_VAL) == 0)
			continue;

#else

		if (dp_strncmpi(sbot_reg_name,
				"SBOTR0",
				strlen("SBOTR0"))
				== 0)
			simu_sbotr0[sbid] = reg_w_data;
		else if (dp_strncmpi(sbot_reg_name,
				     "SBOTR1",
					 strlen("SBOTR1"))
					 == 0)
			simu_sbotr1[sbid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_epot(uint32_t set_cmd, uint32_t epn,
			     char *epot_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_EPMTC_EOR | epn, epmtc);

	while ((tmu_r32(epmtc) & TMU_EPMTC_EOV) == 0)
		continue;

	if (dp_strncmpi(epot_reg_name,
			"EPOT0",
			strlen("EPOT0"))
			== 0)
		reg_r_data = tmu_r32(epot0);
	else if (dp_strncmpi(epot_reg_name,
			     "EPOT1",
				 strlen("EPOT1"))
				 == 0)
		reg_r_data = tmu_r32(epot1);
	else {
		TMU_PRINT("Not valid register name %s?\n", epot_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(epot_reg_name,
			"EPOT0",
			strlen("EPOT0"))
			== 0)
		reg_r_data = simu_epot0[epn];
	else if (dp_strncmpi(epot_reg_name,
			     "EPOT1",
				 strlen("EPOT1"))
				 == 0)
		reg_r_data = simu_epot1[epn];
	else {
		TMU_PRINT("Not valid register name %s?\n", epot_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(epot_reg_name,
				"EPOT0",
				strlen("EPOT0"))
				== 0)
			tmu_w32(reg_w_data, epot0);
		else if (dp_strncmpi(epot_reg_name,
				     "EPOT1",
					 strlen("EPOT1"))
					 == 0)
			tmu_w32(reg_w_data, epot1);

		tmu_w32(TMU_EPMTC_EOW | epn, epmtc);

		while ((tmu_r32(epmtc) & TMU_EPMTC_EOV) == 0)
			continue;

#else

		if (dp_strncmpi(epot_reg_name,
				"EPOT0",
				strlen("EPOT0"))
				== 0)
			simu_epot0[epn] = reg_w_data;
		else if (dp_strncmpi(epot_reg_name,
				     "EPOT1",
					 strlen("EPOT1"))
					 == 0)
			simu_epot1[epn] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_eptt(uint32_t set_cmd, uint32_t epn,
			     char *eptt_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_EPMTC_ETR | epn, epmtc);

	while ((tmu_r32(epmtc) & TMU_EPMTC_ETV) == 0)
		continue;

	if (dp_strncmpi(eptt_reg_name, "EPTT0", strlen("EPTT0")) == 0)
		reg_r_data = tmu_r32(eptt0);
	else if (dp_strncmpi(eptt_reg_name, "EPTT1", strlen("EPTT1")) == 0)
		reg_r_data = tmu_r32(eptt1);
	else {
		TMU_PRINT("Not valid register name %s?\n", eptt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(eptt_reg_name, "EPTT0", strlen("EPTT0")) == 0)
		reg_r_data = simu_eptt0[epn];
	else if (dp_strncmpi(eptt_reg_name, "EPTT1", strlen("EPTT1")) == 0)
		reg_r_data = simu_eptt1[epn];
	else {
		TMU_PRINT("Not valid register name %s?\n", eptt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
		if (dp_strncmpi(eptt_reg_name,
				"EPTT0",
				strlen("EPTT0"))
				== 0)
			tmu_w32(reg_w_data, eptt0);
		else if (dp_strncmpi(eptt_reg_name,
				"EPTT1",
				strlen("EPTT1"))
				== 0)
			tmu_w32(reg_w_data, eptt1);

		tmu_w32(TMU_EPMTC_ETW | epn, epmtc);

		while ((tmu_r32(epmtc) & TMU_EPMTC_ETV) == 0)
			continue;

#else

		if (dp_strncmpi(eptt_reg_name,
				"EPTT0",
				strlen("EPTT0"))
				== 0)
			simu_eptt0[epn] = reg_w_data;
		else if (dp_strncmpi(eptt_reg_name,
				     "EPTT1",
					 strlen("EPTT1"))
					 == 0)
			simu_eptt1[epn] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_epdt(uint32_t set_cmd, uint32_t epn,
			     char *epdt_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_EPMTC_EDR | epn, epmtc);

	while ((tmu_r32(epmtc) & TMU_EPMTC_EDV) == 0)
		continue;

	if (dp_strncmpi(epdt_reg_name, "EPDT0", strlen("EPDT0")) == 0)
		reg_r_data = tmu_r32(epdt0);
	else if (dp_strncmpi(epdt_reg_name, "EPDT1", strlen("EPDT1")) == 0)
		reg_r_data = tmu_r32(epdt1);
	else if (dp_strncmpi(epdt_reg_name, "EPDT2", strlen("EPDT2")) == 0)
		reg_r_data = tmu_r32(epdt2);
	else if (dp_strncmpi(epdt_reg_name, "EPDT3", strlen("EPDT3")) == 0)
		reg_r_data = tmu_r32(epdt3);
	else {
		TMU_PRINT("Not valid register name %s?\n", epdt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(epdt_reg_name,
			"EPDT0",
			strlen("EPDT0"))
			== 0)
		reg_r_data = simu_epdt0[epn];
	else if (dp_strncmpi(epdt_reg_name,
			     "EPDT1",
				 strlen("EPDT1"))
				 == 0)
		reg_r_data = simu_epdt1[epn];
	else if (dp_strncmpi(epdt_reg_name,
			     "EPDT2",
				 strlen("EPDT2"))
				 == 0)
		reg_r_data = simu_epdt2[epn];
	else if (dp_strncmpi(epdt_reg_name,
			     "EPDT3",
				 strlen("EPDT3"))
				 == 0)
		reg_r_data = simu_epdt3[epn];
	else {
		TMU_PRINT("Not valid register name %s?\n", epdt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(epdt_reg_name,
				"EPDT0",
				strlen("EPDT0"))
				== 0)
			tmu_w32(reg_w_data, epdt0);
		else if (dp_strncmpi(epdt_reg_name,
				     "EPDT1",
					 strlen("EPDT1"))
					 == 0)
			tmu_w32(reg_w_data, epdt1);
		else if (dp_strncmpi(epdt_reg_name,
				     "EPDT2",
					 strlen("EPDT2"))
					 == 0)
			tmu_w32(reg_w_data, epdt2);
		else if (dp_strncmpi(epdt_reg_name,
				     "EPDT3",
					 strlen("EPDT3"))
					 == 0)
			tmu_w32(reg_w_data, epdt3);

		tmu_w32(TMU_EPMTC_EDW | epn, epmtc);

		while ((tmu_r32(epmtc) & TMU_EPMTC_EDV) == 0)
			continue;

#else

		if (dp_strncmpi(epdt_reg_name,
				"EPDT0",
				strlen("EPDT0"))
				== 0)
			simu_epdt0[epn] = reg_w_data;
		else if (dp_strncmpi(epdt_reg_name,
				     "EPDT1",
					 strlen("EPDT1"))
					 == 0)
			simu_epdt1[epn] = reg_w_data;
		else if (dp_strncmpi(epdt_reg_name,
				     "EPDT2",
					 strlen("EPDT2"))
					 == 0)
			simu_epdt2[epn] = reg_w_data;
		else if (dp_strncmpi(epdt_reg_name,
				     "EPDT3",
					 strlen("EPDT3"))
					 == 0)
			simu_epdt3[epn] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_epmt(uint32_t set_cmd, uint32_t epn,
			     char *epmt_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_EPMTC_EMR | epn, epmtc);

	while ((tmu_r32(epmtc) & TMU_EPMTC_EMV) == 0)
		continue;

	if (dp_strncmpi(epmt_reg_name, "EPMT", strlen("EPMT")) == 0)
		reg_r_data = tmu_r32(epmt);
	else {
		TMU_PRINT("Not valid register name %s?\n", epmt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(epmt_reg_name, "EPMT", strlen("EPMT")) == 0)
		reg_r_data = simu_epmt[epn];
	else {
		TMU_PRINT("Not valid register name %s?\n", epmt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(epmt_reg_name, "EPMT", strlen("EPMT")) == 0)
			tmu_w32(reg_w_data, epmt);

		tmu_w32(TMU_EPMTC_EMW | epn, epmtc);

		while ((tmu_r32(epmtc) & TMU_EPMTC_EMV) == 0)
			continue;

#else

		if (dp_strncmpi(epmt_reg_name, "EPMT", strlen("EPMT")) == 0)
			simu_epmt[epn] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qtht(uint32_t set_cmd, uint32_t qid,
			     char *qtht_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_QMTC_QTR | qid, qmtc);

	while ((tmu_r32(qmtc) & TMU_QMTC_QTV) == 0)
		continue;

	if (dp_strncmpi(qtht_reg_name, "QTHT0", strlen("QTHT0")) == 0)
		reg_r_data = tmu_r32(qtht0);
	else if (dp_strncmpi(qtht_reg_name, "QTHT1", strlen("QTHT1")) == 0)
		reg_r_data = tmu_r32(qtht1);
	else if (dp_strncmpi(qtht_reg_name, "QTHT2", strlen("QTHT2")) == 0)
		reg_r_data = tmu_r32(qtht2);
	else if (dp_strncmpi(qtht_reg_name, "QTHT3", strlen("QTHT3")) == 0)
		reg_r_data = tmu_r32(qtht3);
	else if (dp_strncmpi(qtht_reg_name, "QTHT4", strlen("QTHT4")) == 0)
		reg_r_data = tmu_r32(qtht4);
	else {
		TMU_PRINT("Not valid register name %s?\n", qtht_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qtht_reg_name, "QTHT0", strlen("QTHT0")) == 0)
		reg_r_data = simu_qtht0[qid];
	else if (dp_strncmpi(qtht_reg_name, "QTHT1", strlen("QTHT1")) == 0)
		reg_r_data = simu_qtht1[qid];
	else if (dp_strncmpi(qtht_reg_name, "QTHT2", strlen("QTHT2")) == 0)
		reg_r_data = simu_qtht2[qid];
	else if (dp_strncmpi(qtht_reg_name, "QTHT3", strlen("QTHT3")) == 0)
		reg_r_data = simu_qtht3[qid];
	else if (dp_strncmpi(qtht_reg_name, "QTHT4", strlen("QTHT4")) == 0)
		reg_r_data = simu_qtht4[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qtht_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qtht_reg_name,
				"QTHT0",
				strlen("QTHT0"))
				== 0)
			tmu_w32(reg_w_data, qtht0);
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT1",
					 strlen("QTHT1"))
					 == 0)
			tmu_w32(reg_w_data, qtht1);
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT2",
					 strlen("QTHT2"))
					 == 0)
			tmu_w32(reg_w_data, qtht2);
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT3",
					 strlen("QTHT3"))
					 == 0)
			tmu_w32(reg_w_data, qtht3);
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT4",
					 strlen("QTHT4"))
					 == 0)
			tmu_w32(reg_w_data, qtht4);

		tmu_w32(TMU_QMTC_QTW | qid, qmtc);

		while ((tmu_r32(qmtc) & TMU_QMTC_QTV) == 0)
			continue;

#else

		if (dp_strncmpi(qtht_reg_name,
				"QTHT0",
				strlen("QTHT0"))
				== 0)
			simu_qtht0[qid] = reg_w_data;
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT1",
					 strlen("QTHT1"))
					 == 0)
			simu_qtht1[qid] = reg_w_data;
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT2",
					 strlen("QTHT2"))
					 == 0)
			simu_qtht2[qid] = reg_w_data;
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT3",
					 strlen("QTHT3"))
					 == 0)
			simu_qtht3[qid] = reg_w_data;
		else if (dp_strncmpi(qtht_reg_name,
				     "QTHT4",
					 strlen("QTHT4"))
					 == 0)
			simu_qtht4[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qoct(uint32_t set_cmd, uint32_t qid,
			     char *qoct_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_QMTC_QOR | qid, qmtc);

	while ((tmu_r32(qmtc) & TMU_QMTC_QOV) == 0)
		continue;

	if (dp_strncmpi(qoct_reg_name, "QOCT0", strlen("QOCT0")) == 0)
		reg_r_data = tmu_r32(qoct0);
	else if (dp_strncmpi(qoct_reg_name, "QOCT1", strlen("QOCT1")) == 0)
		reg_r_data = tmu_r32(qoct1);
	else if (dp_strncmpi(qoct_reg_name, "QOCT2", strlen("QOCT2")) == 0)
		reg_r_data = tmu_r32(qoct2);
	else {
		TMU_PRINT("Not valid register name %s?\n", qoct_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qoct_reg_name, "QOCT0", strlen("QOCT0")) == 0)
		reg_r_data = simu_qoct0[qid];
	else if (dp_strncmpi(qoct_reg_name, "QOCT1", strlen("QOCT1")) == 0)
		reg_r_data = simu_qoct1[qid];
	else if (dp_strncmpi(qoct_reg_name, "QOCT2", strlen("QOCT2")) == 0)
		reg_r_data = simu_qoct2[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qoct_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qoct_reg_name,
				"QOCT0",
				strlen("QOCT0"))
				== 0)
			tmu_w32(reg_w_data, qoct0);
		else if (dp_strncmpi(qoct_reg_name,
				     "QOCT1",
					 strlen("QOCT1"))
					 == 0)
			tmu_w32(reg_w_data, qoct1);
		else if (dp_strncmpi(qoct_reg_name,
				     "QOCT2",
					 strlen("QOCT2"))
					 == 0)
			tmu_w32(reg_w_data, qoct2);

		tmu_w32(TMU_QMTC_QOW | qid, qmtc);

		while ((tmu_r32(qmtc) & TMU_QMTC_QOV) == 0)
			continue;

#else

		if (dp_strncmpi(qoct_reg_name,
				"QOCT0",
				strlen("QOCT0"))
				== 0)
			simu_qoct0[qid] = reg_w_data;
		else if (dp_strncmpi(qoct_reg_name,
				     "QOCT1",
					 strlen("QOCT1"))
					 == 0)
			simu_qoct1[qid] = reg_w_data;
		else if (dp_strncmpi(qoct_reg_name,
				     "QOCT2",
					 strlen("QOCT2"))
					 == 0)
			simu_qoct2[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qdct(uint32_t set_cmd, uint32_t qid,
			     char *qdct_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_QMTC_QDR | qid, qmtc);

	while ((tmu_r32(qmtc) & TMU_QMTC_QDV) == 0)
		continue;

	if (dp_strncmpi(qdct_reg_name, "QDCT0", strlen("QDCT0")) == 0)
		reg_r_data = tmu_r32(qdct0);
	else if (dp_strncmpi(qdct_reg_name, "QDCT1", strlen("QDCT1")) == 0)
		reg_r_data = tmu_r32(qdct1);
	else if (dp_strncmpi(qdct_reg_name, "QDCT2", strlen("QDCT2")) == 0)
		reg_r_data = tmu_r32(qdct2);
	else if (dp_strncmpi(qdct_reg_name, "QDCT3", strlen("QDCT3")) == 0)
		reg_r_data = tmu_r32(qdct3);
	else {
		TMU_PRINT("Not valid register name %s?\n", qdct_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qdct_reg_name, "QDCT0", strlen("QDCT0")) == 0)
		reg_r_data = simu_qdct0[qid];
	else if (dp_strncmpi(qdct_reg_name, "QDCT1", strlen("QDCT1")) == 0)
		reg_r_data = simu_qdct1[qid];
	else if (dp_strncmpi(qdct_reg_name, "QDCT2", strlen("QDCT2")) == 0)
		reg_r_data = simu_qdct2[qid];
	else if (dp_strncmpi(qdct_reg_name, "QDCT3", strlen("QDCT3")) == 0)
		reg_r_data = simu_qdct3[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qdct_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qdct_reg_name,
				"QDCT0",
				strlen("QDCT0"))
				== 0)
			tmu_w32(reg_w_data, qdct0);
		else if (dp_strncmpi(qdct_reg_name,
				     "QDCT1",
					 strlen("QDCT1"))
					 == 0)
			tmu_w32(reg_w_data, qdct1);
		else if (dp_strncmpi(qdct_reg_name,
				     "QDCT2",
					 strlen("QDCT2"))
					 == 0)
			tmu_w32(reg_w_data, qdct2);
		else if (dp_strncmpi(qdct_reg_name,
				     "QDCT3",
					 strlen("QDCT3"))
					 == 0)
			tmu_w32(reg_w_data, qdct3);

		tmu_w32(TMU_QMTC_QDW | qid, qmtc);

		while ((tmu_r32(qmtc) & TMU_QMTC_QDV) == 0)
			continue;

#else

		if (dp_strncmpi(qdct_reg_name,
				"QDCT0",
				strlen("QDCT0"))
				== 0)
			simu_qdct0[qid] = reg_w_data;
		else if (dp_strncmpi(qdct_reg_name,
				     "QDCT1",
					 strlen("QDCT1"))
					 == 0)
			simu_qdct1[qid] = reg_w_data;
		else if (dp_strncmpi(qdct_reg_name,
				     "QDCT2",
					 strlen("QDCT2"))
					 == 0)
			simu_qdct2[qid] = reg_w_data;
		else if (dp_strncmpi(qdct_reg_name,
				     "QDCT3",
					 strlen("QDCT3"))
					 == 0)
			simu_qdct3[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qfmt(uint32_t set_cmd, uint32_t qid,
			     char *qfmt_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_QMTC_QFR | qid, qmtc);

	while ((tmu_r32(qmtc) & TMU_QMTC_QFV) == 0)
		continue;

	if (dp_strncmpi(qfmt_reg_name, "QFMT0", strlen("QFMT0")) == 0)
		reg_r_data = tmu_r32(qfmt0);
	else if (dp_strncmpi(qfmt_reg_name, "QFMT1", strlen("QFMT1")) == 0)
		reg_r_data = tmu_r32(qfmt1);
	else if (dp_strncmpi(qfmt_reg_name, "QFMT2", strlen("QFMT2")) == 0)
		reg_r_data = tmu_r32(qfmt2);
	else {
		TMU_PRINT("Not valid register name %s?\n", qfmt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qfmt_reg_name, "QFMT0", strlen("QFMT0")) == 0)
		reg_r_data = simu_qfmt0[qid];
	else if (dp_strncmpi(qfmt_reg_name, "QFMT1", strlen("QFMT1")) == 0)
		reg_r_data = simu_qfmt1[qid];
	else if (dp_strncmpi(qfmt_reg_name, "QFMT2", strlen("QFMT2")) == 0)
		reg_r_data = simu_qfmt2[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qfmt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qfmt_reg_name,
				"QFMT0",
				strlen("QFMT0"))
				== 0)
			tmu_w32(reg_w_data, qfmt0);
		else if (dp_strncmpi(qfmt_reg_name,
				     "QFMT1",
					 strlen("QFMT1"))
					 == 0)
			tmu_w32(reg_w_data, qfmt1);
		else if (dp_strncmpi(qfmt_reg_name,
				     "QFMT2",
					 strlen("QFMT2"))
					 == 0)
			tmu_w32(reg_w_data, qfmt2);

		tmu_w32(TMU_QMTC_QFW | qid, qmtc);

		while ((tmu_r32(qmtc) & TMU_QMTC_QFV) == 0)
			continue;

#else

		if (dp_strncmpi(qfmt_reg_name,
				"QFMT0",
				strlen("QFMT0"))
				== 0)
			simu_qfmt0[qid] = reg_w_data;
		else if (dp_strncmpi(qfmt_reg_name,
				     "QFMT1",
					 strlen("QFMT1"))
					 == 0)
			simu_qfmt1[qid] = reg_w_data;
		else if (dp_strncmpi(qfmt_reg_name,
				     "QFMT2",
					 strlen("QFMT2")) == 0)
			simu_qfmt2[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qemt(uint32_t set_cmd, uint32_t qid,
			     char *qemt_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_QMTC_QER | qid, qmtc);

	while ((tmu_r32(qmtc) & TMU_QMTC_QEV) == 0)
		continue;

	if (dp_strncmpi(qemt_reg_name, "QEMT", strlen("QEMT")) == 0)
		reg_r_data = tmu_r32(qemt);
	else {
		TMU_PRINT("Not valid register name %s?\n", qemt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qemt_reg_name, "QEMT", strlen("QEMT")) == 0)
		reg_r_data = simu_qemt[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qemt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qemt_reg_name, "QEMT", strlen("QEMT")) == 0)
			tmu_w32(reg_w_data, qemt);

		tmu_w32(TMU_QMTC_QEW | qid, qmtc);

		while ((tmu_r32(qmtc) & TMU_QMTC_QEV) == 0)
			continue;

#else

		if (dp_strncmpi(qemt_reg_name, "QEMT", strlen("QEMT")) == 0)
			simu_qemt[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qsmt(uint32_t set_cmd, uint32_t qid,
			     char *qsmt_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)
	tmu_w32(TMU_QMTC_QSR | qid, qmtc);

	while ((tmu_r32(qmtc) & TMU_QMTC_QSV) == 0)
		continue;

	if (dp_strncmpi(qsmt_reg_name, "QSMT", strlen("QSMT")) == 0)
		reg_r_data = tmu_r32(qsmt);
	else {
		TMU_PRINT("Not valid register name %s?\n", qsmt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qsmt_reg_name, "QSMT", strlen("QSMT")) == 0)
		reg_r_data = simu_qsmt[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qsmt_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qsmt_reg_name, "QSMT", strlen("QSMT")) == 0)
			tmu_w32(reg_w_data, qsmt);

		tmu_w32(TMU_QMTC_QSW | qid, qmtc);

		while ((tmu_r32(qmtc) & TMU_QMTC_QSV) == 0)
			continue;

#else

		if (dp_strncmpi(qsmt_reg_name, "QSMT", strlen("QSMT")) == 0)
			simu_qsmt[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_qmtc(uint32_t set_cmd, uint32_t qid,
			     char *qmtc_reg_name, uint32_t bit_offset,
			     uint32_t bit_mask, uint32_t *value,
			     uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	if (dp_strncmpi(qmtc_reg_name, "QMTC", strlen("QMTC")) == 0)
		reg_r_data = tmu_r32(qmtc);
	else {
		TMU_PRINT("Not valid register name %s?\n", qmtc_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else

	if (dp_strncmpi(qmtc_reg_name, "QMTC", strlen("QMTC")) == 0)
		reg_r_data = simu_qmtc[qid];
	else {
		TMU_PRINT("Not valid register name %s?\n", qmtc_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(qmtc_reg_name, "QMTC", strlen("QMTC")) == 0)
			tmu_w32(reg_w_data, qmtc);

#else

		if (dp_strncmpi(qmtc_reg_name, "QMTC", strlen("QMTC")) == 0)
			simu_qmtc[qid] = reg_w_data;

#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_gothr(uint32_t set_cmd, uint32_t qid,
			      char *gothr_reg_name, uint32_t bit_offset,
			      uint32_t bit_mask, uint32_t *value,
			      uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	if (dp_strncmpi(gothr_reg_name, "GOTHR0", strlen("GOTHR0")) == 0)
		reg_r_data = tmu_r32(gothr[0]);
	else if (dp_strncmpi(gothr_reg_name, "GOTHR1", strlen("GOTHR1")) == 0)
		reg_r_data = tmu_r32(gothr[1]);
	else if (dp_strncmpi(gothr_reg_name, "GOTHR2", strlen("GOTHR2")) == 0)
		reg_r_data = tmu_r32(gothr[2]);
	else if (dp_strncmpi(gothr_reg_name, "GOTHR3", strlen("GOTHR3")) == 0)
		reg_r_data = tmu_r32(gothr[3]);
	else {
		TMU_PRINT("Not valid register name %s?\n", gothr_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else
	TMU_PRINT("This REG is Not Applicable for simulation  %s?\n",
		  gothr_reg_name);
	return 0xFFFFFFFF;	/*REG NOT FOUND */
#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(gothr_reg_name,
				"GOTHR0",
				strlen("GOTHR0"))
				== 0)
			tmu_w32(reg_w_data, gothr[0]);
		else if (dp_strncmpi(gothr_reg_name,
				     "GOTHR1",
					 strlen("GOTHR1"))
					 == 0)
			tmu_w32(reg_w_data, gothr[1]);
		else if (dp_strncmpi(gothr_reg_name,
				     "GOTHR2",
					 strlen("GOTHR2"))
					 == 0)
			tmu_w32(reg_w_data, gothr[2]);
		else if (dp_strncmpi(gothr_reg_name,
				     "GOTHR3",
					 strlen("GOTHR3"))
					 == 0)
			tmu_w32(reg_w_data, gothr[3]);

#else
		TMU_PRINT("This REG is Not Applicable for simulation  %s?\n",
			  gothr_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_cpr(uint32_t set_cmd, uint32_t qid,
			    char *cpr_reg_name, uint32_t bit_offset,
			    uint32_t bit_mask, uint32_t *value,
			    uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	if (dp_strncmpi(cpr_reg_name, "CPR", strlen("CPR")) == 0)
		reg_r_data = tmu_r32(cpr);
	else {
		TMU_PRINT("Not valid register name %s?\n", cpr_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else
	TMU_PRINT("This REG is Not Applicable for simulation  %s?\n",
		  cpr_reg_name);
	return 0xFFFFFFFF;	/*REG NOT FOUND */
#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(cpr_reg_name, "CPR", strlen("CPR")) == 0)
			tmu_w32(reg_w_data, cpr);

#else
		TMU_PRINT("This REG is Not Applicable for simulation  %s?\n",
			  cpr_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}

uint32_t read_write_reg_csr(uint32_t set_cmd, uint32_t qid,
			    char *csr_reg_name, uint32_t bit_offset,
			    uint32_t bit_mask, uint32_t *value,
			    uint32_t flag)
{
	uint32_t reg_r_data = 0, reg_w_data = 0, reg_display_data = 0;
	/*read back */
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

	if (dp_strncmpi(csr_reg_name, "CSR", strlen("CSR")) == 0)
		reg_r_data = tmu_r32(cpr);
	else {
		TMU_PRINT("Not valid register name %s?\n", csr_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
	}

#else
	TMU_PRINT("This REG setting is not applicable for simulation  %s?\n",
		  csr_reg_name);
	return 0xFFFFFFFF;	/*REG NOT FOUND */
#endif
	reg_display_data = get_val(reg_r_data, bit_mask, bit_offset);

	if (set_cmd) {		/*write */
		reg_w_data = reg_r_data;
		set_val(reg_w_data, *value, bit_mask, bit_offset);
#if !defined(CONFIG_LTQ_TMU_DDR_SIMULATE_REG)

		if (dp_strncmpi(csr_reg_name, "CSR", strlen("CSR")) == 0)
			tmu_w32(reg_w_data, cpr);

#else
		TMU_PRINT
		    ("This REG setting is not applicable for simulation  %s?\n",
		     csr_reg_name);
		return 0xFFFFFFFF;	/*REG NOT FOUND */
#endif
	}

	if (set_cmd)
		return *value;
	else
		return reg_display_data;
}


/*-------------sgh add below APIs which is not in orignal GPON API------start*/
void tmu_reset_global_threshold(void)
{
	struct tmu_global_thr tmugoth;
	tmugoth.goth[0] = TMU_GOTH_DEFAULT;
	tmugoth.goth[1] = TMU_GOTH_DEFAULT;
	tmugoth.goth[2] = TMU_GOTH_DEFAULT;
	tmugoth.goth[3] = TMU_GOTH_DEFAULT;
	tmu_global_tail_drop_thr_set(&tmugoth);
}

/*sgh add this api */
void tmu_reset_queue_threshold(uint32_t qid)
{
	uint32_t qtht[5];
	uint32_t en_flag = 0;
	tmu_qtht_read(qid, &qtht[0]);	/*read queue's enable/disable flag */

	if (qtht[0] & TMU_QTHT0_QE_EN)
		en_flag = TMU_QTHT0_QE_EN;

	qtht[0] = 0x00011320 | en_flag;
	/**/ qtht[1] = (TMU_QTHT1_DEFAULT << 16) | TMU_QTHT1_DEFAULT;
	/* maximum threshold for WRED curve 0/1:
	   in units of 8 segments, 0x900 = 2304, 2304*8=18432 is total SSB
	 */
	qtht[2] = (TMU_QTHT2_DEFAULT << 16) | TMU_QTHT2_DEFAULT;
	/* slope of WRED curve 0/1: set to max */
	qtht[3] = (TMU_QTHT3_WRED_DEFAULT << 16) | TMU_QTHT3_WRED_DEFAULT;
	/* tail drop threshold 0/1, in units of 8 segments, set to total SSB */
	/* default values for red & unassigned */
	qtht[4] = (TMU_QTHT4_1_DEFAULT << 16) | TMU_QTHT4_0_DEFAULT;
	/* write QTHT table */
	tmu_qtht_write(qid, &qtht[0]);
}

/*sgh add this api */
void tmu_reset_eport_threshold(uint32_t ep)
{
	/*GRX500: 0x120 *8 = 2304 segments, ie about 1/4 of 9126 */
	uint32_t ept[2] = { (TMU_EPTT0_DEFAULT << 16) | TMU_EPTT0_DEFAULT,
		(TMU_EPTT0_DEFAULT << 16) | TMU_EPTT0_DEFAULT
	};
	tmu_eptt_write(ep, &ept[0]);
}

#ifdef INCLUDE_UNUSED_CHIPTEST_API	/*only used during chiptest. no use now */
/*sgh add this api */
bool tmu_queue_delete(uint32_t index)
{
	struct tmu_equeue_link queue_link;
	TMU_DEBUG_ASSERT(index >= EGRESS_QUEUE_ID_MAX);

	/* not allowed if TMU is enabled */
	if (tmu_is_enabled()) {
		TMU_PRINT("Not allowed action during TMU enable\n");
		return false;
	}

	tmu_qemt_write(index, EPNNULL_EGRESS_PORT_ID);
	tmu_qsmt_write(index, NULL_SCHEDULER_INPUT_ID);
	/*sgh add below */
	/*SGH add this line to disable this queue */
	tmu_equeue_enable(index, false);
	queue_link.qe = 0;
	queue_link.epn = EPNNULL_EGRESS_PORT_ID;	/*none valid id */
	queue_link.sbin = index;
	tmu_equeue_link_set(index, &queue_link);
	return true;
}

/*sgh add this api */
bool tmu_scheduler_delete(uint32_t index)
{
	uint32_t sbot[2];
	uint32_t sbit[4];
	uint32_t omid, leaf, connected_scheduler_index;
	uint32_t sbin;

	/* not allowed if TMU is enabled */
	if (tmu_is_enabled()) {
		TMU_PRINT("Not allowed action during TMU enable\n");
		/*SGH: I think should return  PE_QOS_STATUS_NOT_AVAILABLE */
		return false;
	}

	TMU_DEBUG_ASSERT(index >= SCHEDULER_BLOCK_ID_MAX);
	tmu_sbot_read(index, &sbot[0]);

	/* Remove leaf from the connected scheduler leaf mask */
	if (sbot[0] & TMU_SBOTR0_V_SBIN) {
		omid =
		    get_val(sbot[0], TMU_SBOTR0_OMID_MASK,
			    TMU_SBOTR0_OMID_OFFSET);
		leaf = omid & 7;
		connected_scheduler_index = omid >> 3;
	}

	/* Disable scheduler block output */
	sbot[0] &= ~TMU_SBOTR0_SOE_EN;
	/*added by shaoguohua to clear its
	   lvl for  proc queue statistics collection
	 */
	set_val(sbot[0], 0, TMU_SBOTR0_LVL_MASK, TMU_SBOTR0_LVL_OFFSET);
	tmu_sbot_write(index, &sbot[0]);

	/* Disable all scheduler block inputs for this scheduler */
	for (leaf = 0; leaf < 8; leaf++) {
		sbin = (index << 3) + leaf;
		tmu_sbit_read(sbin, &sbit[0]);
		sbit[0] &= ~TMU_SBITR0_SIE_EN;
		tmu_sbit_write(sbin, &sbit[0]);
	}

	return true;
}
#endif

/*sgh add this api */
void tmu_recursive_del(uint32_t sbid)
{
	int i, k = sbid << 3;
	struct tmu_sched_blk_in_link sbit;

	for (i = 0; i < 8; i++, k++) {
		/*traversal each leaf based on current sbid */
		tmu_sched_blk_in_link_get(k, &sbit);
		TMU_DEBUG(TMU_DEBUG_MSG, "tbid(%d) is %s. Its queue is %u\n",
			  sbit.tbid, sbit.sie ? "enabled" : "disabled",
			  sbit.qsid);

		/*If scheduler Input is disabled,then check next sbin,ie, k+1 */
		if (sbit.sie == 0)
			continue;

		/*SGH:  means the input is SBID, not QID, the
		   continue search its parent SB's queue
		 */
		if (sbit.sit == 1) {
			TMU_DEBUG(TMU_DEBUG_MSG, "delete queue %u\n",
				  sbit.qsid);
			/*iterate all its parent
			   sbid and enable the enqueue accordingly
			 */
			tmu_recursive_del(sbit.qsid);
			continue;
		}

		tmu_egress_queue_delete(sbit.qsid);	/*delete queue */
		TMU_DEBUG(TMU_DEBUG_MSG, "delete queue %u\n", sbit.qsid);
	}

	tmu_sched_blk_delete(sbid);
	TMU_DEBUG(TMU_DEBUG_MSG, "delete scheduler %u\n", sbid);
}

/*sgh add the api: delete queue/sb/tm for an enabled egress port*/
void tmu_del_all(uint32_t epn)
{
	struct tmu_eport_link epmt;
	bool tmu_status = tmu_is_enabled();

	if (tmu_status) {	/*disable TMU it if it is enabled now */
		tmu_enable(false);
		TMU_DEBUG(TMU_DEBUG_MSG,
			  "Disable TMU for deleting purpose temparily\n");
	}

	tmu_egress_port_link_get(epn, &epmt);
	TMU_DEBUG(TMU_DEBUG_MSG,
		  "The sbid is %u which connected to the egress port %u\n",
		  epmt.sbid, epn);
	tmu_recursive_del(epmt.sbid);
	tmu_egress_port_enable(epn, false);

	if (tmu_status)		/*recover the enable status */
		tmu_enable(tmu_status);
}

/*sgh add the api*/
void tmu_set_wfq(const uint32_t sbin, const uint16_t weight)
{
	struct tmu_sched_blk_in_link ilink;
	tmu_sched_blk_in_link_get(sbin, &ilink);
	ilink.iwgt = weight;
	tmu_sched_blk_in_link_set(sbin, &ilink);
}

/*added by sgh */
void tmu_token_bucket_shaper_enable(const uint32_t tbid, const uint32_t sbin,
				    uint8_t enable)
{
	struct tmu_sched_blk_in_tbs tbs;
	tbs.tbe = enable;
	tbs.tbid = tbid;
	tmu_sched_blk_in_shaper_assign_set(sbin, &tbs);
}

/* The qos_shaper_cfg_set function is used to configure a Token
    Bucket Scheduler (TBS) for CIR/CBS or PIR/PBS, respectively. If only a
    single shaper shall be configured, both indexes are set to the same value.
*/
/*added by sgh */
int tmu_shaper_cfg_set(uint32_t index, int32_t enable, uint32_t mode,
		       uint32_t cir, uint32_t pir, uint32_t cbs, uint32_t pbs)
{
	struct tmu_token_bucket_shaper_params tbs;
	uint32_t tmp_pir = 0, tmp_cir = 0, tmp_pbs = 0;
	uint32_t tmp_cbs = 0;
	TMU_DEBUG_ASSERT(index >= EGRESS_QUEUE_ID_MAX);
	/*TMU_PRINT("write  tbs:%u_%u_%u_%u\n", pir, pbs, cir, cbs); */
	/* The default value 0 accepts the TMU's factory default policy */
	tmp_pir = (pir == 0) ? TMU_TBS_PIR_DEF : pir;
	/* The default value 0 accepts the TMU's factory default policy */
	tmp_pbs = (pbs == 0) ? TMU_TBS_PBS_DEF : pbs;
	/*sgh add this line for default cbs */
	tmp_cbs = (cbs == 0) ? TMU_TBS_CBS_DEF : cbs;
	/* Treat values < TMU_TBS_PIR_MIN as TMU_TBS_PIR_MIN */
	tmp_pir = (tmp_pir < TMU_TBS_PIR_MIN) ? TMU_TBS_PIR_MIN : tmp_pir;
	/* Treat values < TMU_TBS_CIR_MIN as TMU_TBS_CIR_MIN */
	tmp_cir = cir < TMU_TBS_CIR_MIN ? TMU_TBS_CIR_MIN : cir;
	TMU_PRINT("tmp_pir=%u. TMU_TBS_PIR_MAX=%u\n", (unsigned int)tmp_pir,
		  (unsigned int)TMU_TBS_PIR_MAX);
	TMU_DEBUG_ASSERT(tmp_pir > TMU_TBS_PIR_MAX);
	TMU_DEBUG_ASSERT(tmp_pbs > TMU_TBS_PBS_MAX);
	TMU_DEBUG_ASSERT(tmp_cir > TMU_TBS_CIR_MAX);
	TMU_DEBUG_ASSERT(tmp_cbs > TMU_TBS_CBS_MAX);
	tbs.tbe0 = enable;
	tbs.tbe1 = enable;
	tbs.mod = mode;
	tbs.pir = tmp_pir;
	tbs.pbs = tmp_pbs;
	tbs.cir = tmp_cir;
	tbs.cbs = tmp_cbs;
	TMU_DEBUG(TMU_DEBUG_MSG,
		  "pir=%d, pbs=%d, cir=%d, cbs=%d for tbid %u\n", tbs.pir,
		  tbs.pbs, tbs.cir, tbs.cbs, index);
	tmu_token_bucket_shaper_cfg_set(index, &tbs);
	tmu_token_bucket_shaper_cfg_get(index, &tbs);
	TMU_DEBUG(TMU_DEBUG_MSG, "read  tbs:%u_%u_%u_%u for tbid %u\n",
		  tbs.pir, tbs.pbs, tbs.cir, tbs.cbs, index);
	return true;
}

void tmu_unused_warning(void)
{
	/*workaround for unused APIs warning message during compile */
	tmu_reset_global_threshold();
	tmu_reset_eport_threshold(0);
	tmu_token_bucket_shaper_enable(0, 0, 0);
}

static unsigned int get_cbm_clock(void)
{
#ifndef TMU_SOC_REAL_BOARD
	/* Original is 600M, but scaled down 50 times in HAPS/PD */
#define TMU_DEFAULT_CLOCK 12
	return TMU_DEFAULT_CLOCK;
#else
	/*Need to adjust with real board, or get from CGU once the API ready */
#define TMU_DEFAULT_CLOCK 600
	PR_INFO("CBM Clock: %ldHz=%ldMHZ\n", clk_get_rate(g_cbm_clk), clk_get_rate(g_cbm_clk) / MHZ);

	if (clk_get_rate(g_cbm_clk) / MHZ < 2) {
		PR_INFO("Wrong cbm clock\n");
		return TMU_DEFAULT_CLOCK;
	} else
		return clk_get_rate(g_cbm_clk) / MHZ;

#endif
}

int tmu_reset_mib(u32 index)
{
	uint32_t qdc[4];

	if (index >= EGRESS_QUEUE_ID_MAX)
		return -1;
	reset_enq_counter(index);
	reset_deq_counter(index);
	tmu_memset(qdc, 0, sizeof(qdc));
	tmu_qdct_write(index, qdc);
	return 0;
}

int tmu_reset_mib_all(u32 flag)
{
	u32 i;

	for (i = 0; i < EGRESS_QUEUE_ID_MAX; i++)
		tmu_reset_mib(i);
	return 0;
}
EXPORT_SYMBOL(tmu_reset_mib_all);
