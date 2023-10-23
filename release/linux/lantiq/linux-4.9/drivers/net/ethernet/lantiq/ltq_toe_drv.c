/******************************************************************************
**
** FILE NAME    : ltq_toe_drv.c
** AUTHOR       : Suresh Nagaraj
** DESCRIPTION  : Lantiq ToE driver for XRX500 series
** COPYRIGHT    :       Copyright (c) 2015
**                      Lantiq Deutschland
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date                $Author                 $Comment
*******************************************************************************/
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <linux/capability.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>

#include <lantiq.h>
#include <lantiq_soc.h>
#include <linux/dma/lantiq_dmax.h>
#include <net/lantiq_cbm_api.h>
#include <net/datapath_proc_api.h>
#include <net/datapath_api.h>

#include "ltq_toe_reg.h"
#include "ltq_toe_drv.h"

#ifdef USE_TIMER_FOR_SESSION_STOP
#include <linux/timer.h>
#include <linux/jiffies.h>
#endif

#define PORT_REQ_CMD_REG0(port)  (PORT_REQ_CMD_REG0_0 + ((port))*0x20)
#define PORT_REQ_CMD_REG1(port)  (PORT_REQ_CMD_REG1_0 + ((port))*0x20)
#define PORT_REQ_CMD_REG2(port)  (PORT_REQ_CMD_REG2_0 + ((port))*0x20)
#define PORT_REQ_CMD_REG3(port)  (PORT_REQ_CMD_REG3_0 + ((port))*0x20)
#define PORT_REQ_CMD_REG4(port)  (PORT_REQ_CMD_REG4_0 + ((port))*0x20)
#define PORT_REQ_CMD_REG5(port)  (PORT_REQ_CMD_REG5_0 + ((port))*0x20)
#define PORT_RES_REG1(port)	(PORT_RES_REG1_0 + ((port))*0x20)
#define PORT_RES_REG0(port)	(PORT_RES_REG0_0 + ((port))*0x20)

#define LRO_FID(port)	   (LRO_FID_0 + ((port)*0x8))
#define LRO_TO_REG(port)   (LRO_TO_REG_0 + ((port)*0x8))
#define LRO_OC_FLAG(port,ctx)  (LRO_OC_FLAG_0 + ((port*2)+ctx)*0x100)
#define LRO_OC_OWNER(port,ctx)  (LRO_OC_OWNER_0 + ((port*2)+ctx)*0x100)

#define LRO_MAX_AGP 20
#define PMAC_HDR_SIZE 8
#define L2_HDR_LEN 14

#define DEFAULT_WAIT_CYCLES 1000

/*unsigned int toe_membase = 0xE2000000;*/
static unsigned char __iomem *ltq_toe_membase; /* Virtual */
/*static const unsigned char __iomem *lro_sram_membase_res0 = (unsigned char *)0xE2013000;*/
static unsigned char __iomem *lro_sram_membase_res0;
/*static unsigned int lro_sram_membase_res1 = 0xE2013100;*/
static u32 g_tso_irq_mode = 1;
static struct proc_dir_entry *g_toe_dir;
static inline int process_tso_results(ltq_tso_port_t* tsoPort);
static inline void tso_write_stats(ltq_tso_port_t* ,struct sk_buff*);
static struct device *g_toe_dev;

#undef LRO_DEBUG
#define ZERO_SRAM_DBG

#ifdef LRO_DEBUG
#define LRO_MAX_DBG_INFO 20000
struct lro_dbg_info {
	u32 except_reason;
	u32 tcp_seq_no;
	u32 aggr_len;
}lro_dbg_info;
static struct lro_dbg_info dbg_info[LRO_MAX_DBG_INFO];
u32 dbg_head = 0;
#endif

static unsigned char ltq_large_buf[NR_CPUS*SG_BUFFER_PER_PORT][65536]__attribute__((aligned(32)));

#define LRO_MAX_EXCEPTION_COUNT 9
static u32 lro_num_except[LRO_MAX_EXCEPTION_COUNT], lro_num_success;
static u32 lro_num_except_entries[32];
static u32 g_unmatched_entry;
static ltq_tso_port_t ltq_tso_port[NR_CPUS];
static ltq_lro_port_t ltq_lro_port[LTQ_MAX_LRO_PORTS];

static int ltq_toe_exit(struct platform_device *pdev);
static int tso_configure_dma(void);
static void configure_tso(void);
static irqreturn_t ltq_tso_tx_int(int irq, void *_port);
static void ltq_tso_tasklet(unsigned long);

#ifdef USE_TIMER_FOR_SESSION_STOP
static void lro_timer_fn(unsigned long data);
static unsigned long lro_time;
#endif

static void ltq_lro_tasklet(unsigned long);
static struct tasklet_struct lro_tasklet[LTQ_MAX_LRO_PORTS];
static struct tasklet_struct lro_exception_tasklet;

static void ltq_lro_ovflow_tasklet(unsigned long);
static struct tasklet_struct lro_ovflow_tasklet;

static void configure_lro(void);
int lro_stop_flow (int session_id, int timeout, int flags);
int lro_start_flow (int *session_id, int timeout, int flags, struct cpumask cpumask);
static void lro_process_output_context(int port, int oc_flag_no);

spinlock_t toe_register_lock;	/*!< spin lock */
spinlock_t lro_register_lock;

#define skb_tso_size(x)       (skb_shinfo(x)->gso_size)

#define ltq_toe_r32(x)			ltq_r32(ltq_toe_membase + (x))
#define ltq_toe_w32(x, y)		ltq_w32(x, ltq_toe_membase + (y))
#define ltq_toe_w32_mask(x, y, z)	\
	ltq_w32_mask(x, y,  ltq_toe_membase + (z))

#define	ltq_tso_w32_cmd(x, y, z)	\
	ltq_w32(x, (ltq_tso_port[z].membase + (y*4)))

#define	ltq_tso_r32(x, y)	\
	ltq_r32(ltq_tso_port[x].membase + (y*4))

#define ltq_lro_sram_mem(x, y, z) \
	ltq_r32(lro_sram_membase_res0 + (0x150 * x) + (0xA8 * y) + (z))

#define ltq_lro_sram_unmatched_mem(x, y) \
	ltq_r32(lro_sram_membase_res0 + 0xA80 + (0x10*(x)) + (y))

#define ltq_lro_sram_except_mem(x, y) \
	ltq_r32(lro_sram_membase_res0 + 0xB00 + (0x10*x) + (y))

#define toe_fill_cmd0(sphy, dphy, ie, g, chunk, len, last, port) do {   \
  unsigned int reg = 0 ;                      \
  reg = (sphy) << PORT_REQ_CMD_REG0_0_SPHY_POS |  \
        (dphy) << PORT_REQ_CMD_REG0_0_DPHY_POS | \
        (ie) << PORT_REQ_CMD_REG0_0_IE_POS | \
        (g) << PORT_REQ_CMD_REG0_0_G_POS | \
        (chunk) << PORT_REQ_CMD_REG0_0_CHUNK_SIZ_POS | \
	(last << PORT_REQ_CMD_REG0_0_LAST_POS) | \
        (len) << PORT_REQ_CMD_REG0_0_LEN_POS ; \
 /*   printk("REG0 = %X\n", reg); */\
		ltq_toe_w32(reg, PORT_REQ_CMD_REG0(port));  \
}while(0)

#define toe_fill_cmd1_frags(srcbuf, cachewb, port, lenn) do { \
	unsigned int physaddr; \
	volatile unsigned int old_data, new_data; \
    if (cachewb) {  \
		old_data = ltq_r32(srcbuf + lenn - 4); \
		physaddr = dma_map_single(g_toe_dev, (void *) srcbuf, lenn, DMA_TO_DEVICE); 	\
			if (dma_mapping_error(g_toe_dev, physaddr)) { \
				pr_err("%s DMA map failed\n", __func__); \
				goto tsoXmitDoneErr;	\
			} \
    }\
	/* CMD1 */  \
	pr_debug("physical address of the src data: %d = %08x and len = %d\n", \
					(unsigned int)srcbuf, (unsigned int)physaddr, lenn);\
/*    printk("REG1 = %X\n", physaddr); */\
	wmb(); \
	new_data = ltq_r32(srcbuf + lenn - 4); \
	WARN_ON(old_data != new_data); \
	wmb(); \
	ltq_toe_w32(physaddr, PORT_REQ_CMD_REG1(port)); \
}while(0)

#define toe_fill_cmd1(srcbuf, cachewb, port, lenn) do { \
	  	unsigned int physaddr; \
	  	volatile unsigned int old_data, new_data; \
	old_data = ltq_r32(srcbuf + lenn - 4); \
    if (cachewb) {  \
		physaddr = dma_map_single(g_toe_dev, (void *) srcbuf, \
							lenn, DMA_BIDIRECTIONAL); \
			if (dma_mapping_error(g_toe_dev, physaddr)) { \
				pr_err("%s DMA map failed\n", __func__); \
				goto tsoXmitDoneErr;	\
			} \
    }\
	/* CMD1 */  \
	pr_debug("physical address of the src data: %d = %08x and len = %d\n", \
					(unsigned int)srcbuf, (unsigned int)physaddr, lenn);\
	wmb(); \
	new_data  = ltq_r32(srcbuf + lenn - 4); \
	WARN_ON(old_data != new_data); \
	wmb(); \
   /* printk("REG1 = %X\n", physaddr); */\
	ltq_toe_w32(physaddr, PORT_REQ_CMD_REG1(port)); \
}while(0)

#define toe_fill_cmd4(buf, port) do { \
			/* CMD4 */ \
	    unsigned int physaddr; \
			physaddr = CPHYSADDR((buf)); \
			pr_debug("address of the large buffer: = %08x \n", (unsigned int)(physaddr)); \
/*      printk("REG4 = %X\n", physaddr); */\
			ltq_toe_w32(physaddr, PORT_REQ_CMD_REG4(port)); \
}while(0)

#define toe_fill_cmd4_sbk(buff, port) do { \
			/* CMD4 */ \
			pr_debug("address of the large buffer: = %08x \n", (unsigned int)(buff)); \
  /*    printk("REG4_skb = %X\n", buff); */\
			ltq_toe_w32(buff, PORT_REQ_CMD_REG4(port)); \
}while(0)

#define toe_fill_cmd2(dw0, port) do { \
		/* CMD2 */ \
  /*  printk("REG2 = %X\n", dw0); */\
		ltq_toe_w32((dw0), PORT_REQ_CMD_REG2(port)); \
}while(0)

#define toe_fill_cmd3(dw1, port) do { \
		/* CMD3 */ \
  /*  printk("REG3 = %X\n", dw1); */\
		ltq_toe_w32((dw1), PORT_REQ_CMD_REG3(port)); \
}while(0)

#define toe_fill_cmd5(tso_mss, tirq, port) do { \
    unsigned int reg; \
		/* CMD5 */ \
		/* Set the segment size */ \
		pr_debug("segment size: %d \n", tso_mss); \
    reg = tso_mss << PORT_REQ_CMD_REG5_0_SEG_LEN_POS; \
		/* Enable TIRQ */ \
    reg |= (tirq) << PORT_REQ_CMD_REG5_0_TIRQ_POS; \
	wmb(); \
    /* Flag TSO to kickstart processing of this CMD */ \
    /* reg |= 1 << PORT_REQ_CMD_REG5_0_OWN_POS; */ \
    /* Write to CMD5 reg */ \
  /*  printk("REG5 = %X\n", reg); */\
		ltq_toe_w32(reg, PORT_REQ_CMD_REG5(port)); \
}while(0)

#define toe_get_cmd_own(port) { \
  unsigned long OwnReg;  \
  do { \
			OwnReg = ltq_toe_r32(PORT_REQ_CMD_REG5(port));  \
   /*   printk("Own = %X\n", OwnReg); */\
  }while (!test_bit(31, &OwnReg)); \
}

static inline int is_tso_IRQMode(void)
{
	return (g_tso_irq_mode == 1);
}

static inline int is_tso_PollingMode(void)
{
	return (g_tso_irq_mode == 0);
}

static inline void tso_set_IRQMode(void)
{
	g_tso_irq_mode = 1;
}

static inline void tso_set_pollingMode(void)
{
	g_tso_irq_mode = 0;
}

static inline int tso_results_pending(ltq_tso_port_t* tsoPort)
{
	return (SG_BUFFER_PER_PORT != atomic_read(&(tsoPort->availBuffs)));
}

static inline int tso_sgbuffs_available(ltq_tso_port_t* tsoPort)
{
	return atomic_read(&(tsoPort->availBuffs));
}

static inline int toe_tso_port_ready(int port, int waitThreshold)
{
	unsigned long OwnReg;

	do {
		OwnReg = ltq_toe_r32(PORT_REQ_CMD_REG5(port));
		--waitThreshold;
	} while ( waitThreshold && !test_bit(31, &OwnReg) );

	return test_bit(31, &OwnReg);
}

static inline u64 maxV(u64 x, u64 y)
{
	return (x > y ? x : y);
}

enum tso_desc_base {
	/* TSO Port 0 */
	DMA3_TOE_MEMCPY_CHAN_4_DESC_BASE = 0xA2010600,
	DMA3_TOE_MEMCPY_CHAN_5_DESC_BASE = 0xA2010620,
	DMA3_TOE_MEMCPY_CHAN_0_DESC_BASE = 0xA2014800,
	DMA3_TOE_MEMCPY_CHAN_1_DESC_BASE = 0xA2014820,
	DMA3_TOE_MEMCPY_CHAN_2_DESC_BASE = 0xA2014840,
	DMA3_TOE_MEMCPY_CHAN_3_DESC_BASE = 0xA2014860,
	DMA3_TOE_MEMCPY_CHAN_8_DESC_BASE = 0xA2015800,
	DMA3_TOE_MEMCPY_CHAN_9_DESC_BASE = 0xA2015810,
	DMA3_TOE_MEMCPY_CHAN_10_DESC_BASE = 0xA2015820,
	DMA3_TOE_MEMCPY_CHAN_11_DESC_BASE = 0xA2015830,

	/* TSO Port 1 */
	DMA3_TOE_MEMCPY_CHAN_6_DESC_BASE = 0xA2010700,
	DMA3_TOE_MEMCPY_CHAN_7_DESC_BASE = 0xA2010720,
	DMA3_TOE_MEMCPY_CHAN_30_DESC_BASE = 0xA2014900,
	DMA3_TOE_MEMCPY_CHAN_31_DESC_BASE = 0xA2014920,
	DMA3_TOE_MEMCPY_CHAN_32_DESC_BASE = 0xA2014940,
	DMA3_TOE_MEMCPY_CHAN_33_DESC_BASE = 0xA2014960,
	DMA3_TOE_MEMCPY_CHAN_38_DESC_BASE = 0xA2015900,
	DMA3_TOE_MEMCPY_CHAN_39_DESC_BASE = 0xA2015910,
	DMA3_TOE_MEMCPY_CHAN_40_DESC_BASE = 0xA2015920,
	DMA3_TOE_MEMCPY_CHAN_41_DESC_BASE = 0xA2015930,

	/* TSO Port 2 */
	DMA3_TOE_MEMCPY_CHAN_34_DESC_BASE = 0xA2010800,
	DMA3_TOE_MEMCPY_CHAN_35_DESC_BASE = 0xA2010820,
	DMA3_TOE_MEMCPY_CHAN_54_DESC_BASE = 0xA2014a00,
	DMA3_TOE_MEMCPY_CHAN_55_DESC_BASE = 0xA2014a20,
	DMA3_TOE_MEMCPY_CHAN_42_DESC_BASE = 0xA2014a40,
	DMA3_TOE_MEMCPY_CHAN_43_DESC_BASE = 0xA2014a60,
	DMA3_TOE_MEMCPY_CHAN_48_DESC_BASE = 0xA2015a00,
	DMA3_TOE_MEMCPY_CHAN_49_DESC_BASE = 0xA2015a10,
	DMA3_TOE_MEMCPY_CHAN_50_DESC_BASE = 0xA2015a20,
	DMA3_TOE_MEMCPY_CHAN_51_DESC_BASE = 0xA2015a30,

	/* TSO Port 3 */
	DMA3_TOE_MEMCPY_CHAN_36_DESC_BASE = 0xA2010900,
	DMA3_TOE_MEMCPY_CHAN_37_DESC_BASE = 0xA2010920,
	DMA3_TOE_MEMCPY_CHAN_62_DESC_BASE = 0xA2014b00,
	DMA3_TOE_MEMCPY_CHAN_63_DESC_BASE = 0xA2014b20,
	DMA3_TOE_MEMCPY_CHAN_52_DESC_BASE = 0xA2014b40,
	DMA3_TOE_MEMCPY_CHAN_53_DESC_BASE = 0xA2014b60,
	DMA3_TOE_MEMCPY_CHAN_58_DESC_BASE = 0xA2015b00,
	DMA3_TOE_MEMCPY_CHAN_59_DESC_BASE = 0xA2015b10,
	DMA3_TOE_MEMCPY_CHAN_60_DESC_BASE = 0xA2015b20,
	DMA3_TOE_MEMCPY_CHAN_61_DESC_BASE = 0xA2015b30
};

static irqreturn_t lro_port_except_isr (int irq, void *priv)
{
	uint32_t except_entries, int_status;
	int i;
	unsigned long tso_rl_flags;

	//printk("except_isr\n");

	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);

	/* Mask the interrupt */
	ltq_toe_w32_mask((1 << TOE_INT_MASK_LRO_EXP_POS), 0, TOE_INT_EN);

	/* Clear the exception and interrupt if there is any */
	int_status = ltq_toe_r32(TOE_INT_STAT);
	if(int_status & TOE_INT_STAT_LRO_EXP_MASK) {
		ltq_toe_w32(TOE_INT_STAT_LRO_EXP_MASK, TOE_INT_STAT);
	} else {
		/* Unmask the interrupt */
		ltq_toe_w32_mask(0, (1 << TOE_INT_MASK_LRO_EXP_POS), TOE_INT_EN);
		spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
		return IRQ_HANDLED;
	}
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);

	except_entries = ltq_toe_r32(LRO_EXP_EFLAG);
	for(i=0; i<32; i++) {
		if(except_entries&(1<<i)) lro_num_except_entries[i]++;
	}

	tasklet_schedule(&lro_exception_tasklet);

	return IRQ_HANDLED;
}

static void ltq_lro_ovflow_tasklet(unsigned long dev)
{
	u32 int_status, i, pos;
	volatile unsigned long except_flag, flag_wr = 0;
	unsigned long tso_rl_flags;

	/* Read the interrupt status */
	int_status = ltq_toe_r32(TOE_INT_STAT);

	/* Read the Exception flag */
	except_flag = ltq_toe_r32(LRO_EXP_EFLAG);

	/* Find the first bit set in the flag */
	for(i=0; i<(2*LTQ_MAX_LRO_PORTS); i++) {
		pos = ffs(except_flag);
		if (!pos) {
			break;
		} else {
			clear_bit(pos-1, &except_flag);
			flag_wr |= 1 << (pos-1);
		}
	}

	spin_lock_irqsave(&lro_register_lock, tso_rl_flags);
	if (flag_wr) {
		pr_info("ecovfl writing: %x bcos except_flag = %x at pos =%d !\n", (unsigned int)flag_wr, (unsigned int)except_flag, pos);
		ltq_toe_w32(flag_wr, LRO_EXP_EFLAG);
	}
	spin_unlock_irqrestore(&lro_register_lock, tso_rl_flags);

	/* unmask the interrupt */
	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
	//ltq_toe_w32_mask((1 << TOE_INT_MASK_S22_POS), 0 , TOE_INT_MASK);
	ltq_toe_w32_mask(0, (1 << TOE_INT_EN_S22_POS), TOE_INT_EN);
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
}

static irqreturn_t lro_port_overflow_isr (int irq, void *priv)
{
	unsigned long tso_rl_flags;

	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
	ltq_toe_w32_mask((1 << TOE_INT_EN_S22_POS), 0, TOE_INT_EN);
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
	return IRQ_HANDLED;
}

static irqreturn_t lro_port_context_isr (int irq, void *priv)
{
	struct ltq_lro_port *pport = (struct ltq_lro_port *)priv;
	unsigned long tso_rl_flags, int_status;

	pr_info_once("%s called with irq: %d\n", __func__, irq);

	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
	int_status = ltq_toe_r32(TOE_INT_STAT);
	if (!(int_status & (1 << (pport->port_num + TOE_INT_MASK_LRO0_POS)))) {
		spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
		return IRQ_NONE;
	}

	/* Mask the interrupt */
	ltq_toe_w32_mask((1 << (pport->port_num + TOE_INT_MASK_LRO0_POS)), 0, TOE_INT_EN);
	//ltq_toe_w32_mask(0, (1 << (pport->port_num + TOE_INT_MASK_LRO0_POS)), TOE_INT_MASK);

	wmb();
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);

	/* Schedule the tasklet for housekeeping */
	tasklet_schedule(&lro_tasklet[pport->port_num]);
	return IRQ_HANDLED;
}

static void lro_process_output_context(int port, int oc_flag_no)
{
	u32 oc_flag;
	u8 *data_ptr;
	int i, j, data_len, temp_len, real_len, no_segs;
	u16 frag_len, offset;
	u8 *frag_addr;
	struct sk_buff *skb;
	u32 desc0, desc1, desc2, desc3, out_of_sequence = 0;
	u32 except_flag = 0, except_reason = LRO_MAX_EXCEPTION_COUNT + 1;
#ifdef LRO_DEBUG
	struct tcphdr *tcp_hdr;
#endif
	struct sk_buff *frag_skb = NULL, *last_frag_skb = NULL;
	unsigned long tso_rl_flags;

	oc_flag = ltq_toe_r32(LRO_OC_FLAG(port, oc_flag_no));
	pr_debug("LRO done and OC_FLAG: %x\n", oc_flag);
	no_segs = (oc_flag & LRO_OC_FLAG_0_SEG_NR_MASK) >> LRO_OC_FLAG_0_SEG_NR_POS;
	pr_debug("First look into LRO success and later exception.. \n");

	if (oc_flag & LRO_OC_FLAG_0_EXCPT_MASK) {
			except_flag = ltq_toe_r32(LRO_EXP_EFLAG);

			if (except_flag & (1 << ((port*2) + oc_flag_no)))
				except_flag = 1;
			else
				except_flag = 0;
			pr_debug("LRO exception with EXP_EFLAG = %x ..\n", except_flag);
			except_reason = (oc_flag & LRO_OC_FLAG_0_EXP_CASE_MASK) >> LRO_OC_FLAG_0_EXP_CASE_POS;
			if (except_flag && except_reason != 6)
				out_of_sequence = 1;
			if (except_reason < LRO_MAX_EXCEPTION_COUNT)
				lro_num_except[except_reason]++;
			else
				pr_err("spurious exception !!\n");
			if ((except_reason == 0) && (no_segs == 0))
				pr_info("flush exception with no_segs = %d for OC..%d\n", no_segs, oc_flag_no);
	}

	if (no_segs > 0) {

		if (no_segs == LRO_MAX_AGP) {
			lro_num_success++;
		} else if (except_reason == (LRO_MAX_EXCEPTION_COUNT + 1)) {
			pr_info("BUG! no exception no_segs = %d and OC_FLAG = %x!\n", no_segs, oc_flag);
		}
		desc0 = ltq_lro_sram_mem(port, oc_flag_no, 0);
		desc1 = ltq_lro_sram_mem(port, oc_flag_no, 4);
		desc2 = ltq_lro_sram_mem(port, oc_flag_no, 8);
		desc3 = ltq_lro_sram_mem(port, oc_flag_no, 0xc);

#ifdef ZERO_SRAM_DBG
		ltq_w32(0xffffffff, lro_sram_membase_res0 + (0x150 * (port)) + (0xA8 * (oc_flag_no)) + (8));
#endif
		/* Build the SKB */
		data_len = desc3 & 0x0000FFFF;
		data_ptr = (unsigned char *) __va(desc2);
		offset = (desc3 & 0x3800000) >> 23;
		dma_cache_inv((unsigned long) data_ptr+offset, data_len);

		temp_len = data_len + 128 + NET_IP_ALIGN + NET_SKB_PAD;
		real_len = SKB_DATA_ALIGN(temp_len) + SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		/*first_len = ltq_toe_r32(lro_sram_membase_res0 + 0xc) & 0x0000FFFF;*/
		if (port == 0xff)
			pr_info("buffer pointer of the first packet: %x and length:%d no_segs = %d\n", (unsigned int)data_ptr, real_len, no_segs);
		//skb = build_skb((void *) (data_ptr + ((desc3 & 0x3800000) >> 23) - 128 - NET_IP_ALIGN), real_len);
		skb = cbm_build_skb((void *) (data_ptr + (offset - 128 - NET_IP_ALIGN - NET_SKB_PAD)), real_len, GFP_ATOMIC);

		/* Get the Descriptor words */
		if (skb) {
			skb_reserve(skb,  128 + NET_IP_ALIGN + NET_SKB_PAD);
			skb->DW0 = desc0;
			skb->DW1 = desc1;
			/*skb->DW2 = desc2;*/
			skb->DW2 = (u32)data_ptr;
			skb->DW3 = desc3;
			skb_put(skb, data_len);
		} else {
			cbm_buffer_free(smp_processor_id(), data_ptr, 0);
			pr_err("%s: failure in allocating skb\r\n", __func__);
		}

		for (i = 1, j = 0; i < no_segs; i++, j++) {
			frag_addr = (uint8_t *) __va((ltq_lro_sram_mem(port, oc_flag_no , 0x14 + (j*8))));
#ifdef ZERO_SRAM_DBG
			ltq_w32(0xffffffff, lro_sram_membase_res0 + (0x150 * (port)) + (0xA8 * (oc_flag_no)) + (0x14 + (j*8)));
#endif
			if (port == 0xff)
				pr_info("Buffer pointer of the %i packet: %x\n", i, (unsigned int)frag_addr);
			if (!skb) {
				cbm_buffer_free(smp_processor_id(), frag_addr, 0);
				continue;
			}
			frag_len = ltq_lro_sram_mem(port, oc_flag_no, 0x10 + (j*8)) & 0x0000FFFF;
			offset = (ltq_lro_sram_mem(port, oc_flag_no, 0x10 + (j*8)) & 0xFFFF0000) >> 16;
			if (port == 0xff)
				pr_info("Offset: %d Fragment Length: %d\n", offset, frag_len);
			dma_cache_inv((unsigned long) frag_addr+offset, frag_len);

			real_len =  frag_len + SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
			frag_skb = cbm_build_skb((void *) (frag_addr + offset), real_len, GFP_ATOMIC);
			if (frag_skb) {
				skb_put(frag_skb, frag_len);
				if (last_frag_skb)
					last_frag_skb->next = frag_skb;
				else
					skb_shinfo(skb)->frag_list = frag_skb;

				last_frag_skb = frag_skb;
				skb->len += frag_len;
				skb->data_len += frag_len;
				skb->truesize += frag_skb->truesize;
				if (except_reason == 88)
					print_hex_dump(KERN_DEBUG, "data: ", DUMP_PREFIX_ADDRESS, 16, 1,
						 frag_skb->data, 100, false);
			} else {
				cbm_buffer_free(smp_processor_id(), frag_addr, 0);
				if (skb) {
					dev_kfree_skb_any(skb);
					skb = NULL;
				}
				pr_err("%s: cbm_build_skb failed !!\n", __func__);
			}
		}
		if (skb) {
#ifdef LRO_DEBUG
			tcp_hdr = (struct tcphdr *) (skb->data + 42);
			dbg_info[dbg_head].tcp_seq_no = ntohl(tcp_hdr->seq);
			dbg_info[dbg_head].except_reason = except_reason;
			dbg_info[dbg_head].aggr_len = skb->len - 62;
			dbg_head = (dbg_head + 1) % LRO_MAX_DBG_INFO;
#endif
			/*
			 * TCP/IP header checksum is wrong here,
			 * since HW has modified the TCP Payload and IP header's total_len field.
			 *
			 * So, mark skb->ip_summed as CHECKSUM_UNNECESSARY.
			 */
			skb->ip_summed = CHECKSUM_UNNECESSARY;

			/* Send it to datapath library */
			dp_rx(skb, 0);
		}
	}

	if (out_of_sequence) {

		desc0 = ltq_lro_sram_except_mem(port, 0);
		desc1 = ltq_lro_sram_except_mem(port, 4);
		desc2 = ltq_lro_sram_except_mem(port, 8);
		desc3 = ltq_lro_sram_except_mem(port, 0xc);

		/* Build the SKB */
		data_len = desc3 & 0x0000FFFF;
		data_ptr = (unsigned char *) __va(desc2);
		offset = (desc3 & 0x3800000) >> 23;
		dma_cache_inv((unsigned long) data_ptr+offset, data_len);

		temp_len = data_len + 128 + NET_IP_ALIGN + NET_SKB_PAD;
		real_len = SKB_DATA_ALIGN(temp_len) + SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		/*first_len = ltq_toe_r32(lro_sram_membase_res0 + 0xc) & 0x0000FFFF;*/

		if (port == 0xff)
			pr_debug("buffer pointer of the exception packet: %x and length:%d \n", (unsigned int)data_ptr, data_len);
		//skb = build_skb((void *) (data_ptr + ((desc3 & 0x3800000) >> 23) - 128 - NET_IP_ALIGN), real_len);
		skb = cbm_build_skb((void *) (data_ptr + (offset - 128 - NET_IP_ALIGN - NET_SKB_PAD)), real_len, GFP_ATOMIC);

		/* Get the Descriptor words */
		if (skb) {
			skb_reserve(skb,  128 + NET_IP_ALIGN + NET_SKB_PAD);
			skb->DW0 = desc0;
			skb->DW1 = desc1;
			/*skb->DW2 = desc2;*/
			skb->DW2 = (u32)data_ptr;
			skb->DW3 = desc3;
			skb_put(skb, data_len);

			if (except_reason == 88)
				print_hex_dump(KERN_DEBUG, "data: ", DUMP_PREFIX_ADDRESS, 16, 1,
								 skb->data, 100, false);
#ifdef LRO_DEBUG
			dbg_info[dbg_head].except_reason = except_reason;
			tcp_hdr = (struct tcphdr *) (skb->data + 42);
			dbg_info[dbg_head].tcp_seq_no = ntohl(tcp_hdr->seq);
			dbg_info[dbg_head].aggr_len = skb->len - 62;
			dbg_head = (dbg_head + 1) % LRO_MAX_DBG_INFO;
#endif

			dp_rx(skb, 0);

			asm("sync");
			if (except_reason == 0)
				pr_debug("eflag = %x\n", ltq_toe_r32(LRO_EXP_EFLAG));
		} else {
			cbm_buffer_free(smp_processor_id(), data_ptr, 0);
			pr_err("failure in allocating skb\r\n");
		}
	}

	spin_lock_irqsave(&lro_register_lock, tso_rl_flags);

	/* Give the ownership back to LRO */
	ltq_toe_w32(LRO_OC_OWNER_0_OWNER_MASK, LRO_OC_OWNER(port,oc_flag_no));
	wmb();

	/* Give the exception ownership back to LRO */
	if (except_flag) {
		ltq_toe_w32(1 << ((port*2) + oc_flag_no), LRO_EXP_EFLAG);
	}
	asm("sync");

	spin_unlock_irqrestore(&lro_register_lock, tso_rl_flags);

#if 0
	i=0;
	do {
		owner = ltq_toe_r32(LRO_OC_OWNER(port + oc_flag_no));
	} while (((owner & LRO_OC_OWNER_0_OWNER_MASK) == LRO_OC_OWNER_0_OWNER_MASK) && ((i++) < DEFAULT_WAIT_CYCLES));
	owner = ltq_toe_r32(LRO_OC_OWNER(port,oc_flag_no));
	if ((owner & LRO_OC_OWNER_0_OWNER_MASK) == LRO_OC_OWNER_0_OWNER_MASK)
		pr_err("couldn't give the ownership back to LRO..\n");
#endif
}

static void ltq_lro_exception_tasklet(unsigned long dev __maybe_unused)
{
	uint32_t entries, clear_entries=0;
	uint32_t desc0, desc1, desc2, desc3;
	int i, temp_len, real_len, data_len;
	unsigned long tso_rl_flags;
	static int read_pos = 0;
	u8 *data_ptr;
	struct sk_buff *skb;
	u16 offset;

	entries = ltq_toe_r32(LRO_EXP_EFLAG);

	if(entries & ~(LRO_EXP_EFLAG_UNMATCH_MASK | LRO_EXP_EFLAG_EXP_MASK)) {
		pr_err("flush entries found (EFLAG: %x)\n", entries);
	}

	for(i = read_pos; entries & (1 << (i + LRO_EXP_EFLAG_UNMATCH_POS)); i=(i+1)%8) {
		//if(i>23) printk("PHU: unexpected flush entries\n");
		desc0 = ltq_lro_sram_unmatched_mem(i, 0);
		desc1 = ltq_lro_sram_unmatched_mem(i, 4);
		desc2 = ltq_lro_sram_unmatched_mem(i, 8);
		desc3 = ltq_lro_sram_unmatched_mem(i, 0xc);
		printk("read unmatched buffer %#x from index %d because: %x\n", desc2, i, entries & (1 << (i + LRO_EXP_EFLAG_UNMATCH_POS)));
#ifdef ZERO_SRAM_DBG
		ltq_w32(0xffffffff, lro_sram_membase_res0 + 0xA80 + (0x10*i) + (8));
#endif
		spin_lock_irqsave(&lro_register_lock, tso_rl_flags);

		clear_entries = 1 << (i + LRO_EXP_EFLAG_UNMATCH_POS);
		ltq_toe_w32(clear_entries, LRO_EXP_EFLAG);

		asm("sync");

		spin_unlock_irqrestore(&lro_register_lock, tso_rl_flags);

		/* Build the SKB */
		data_len = desc3 & 0x0000FFFF;
		data_ptr = (unsigned char *) __va(desc2);
		offset = (desc3 & 0x3800000) >> 23;
		dma_cache_inv((unsigned long) data_ptr+offset, data_len);

		temp_len = data_len + 128 + NET_IP_ALIGN + NET_SKB_PAD;
		real_len = SKB_DATA_ALIGN(temp_len) + SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

		skb = cbm_build_skb((void *) (data_ptr + (offset - 128 - NET_IP_ALIGN - NET_SKB_PAD)), real_len, GFP_ATOMIC);

		/* Get the Descriptor words */
		if (skb) {
			skb_reserve(skb,  128 + NET_IP_ALIGN + NET_SKB_PAD);
			skb->DW0 = desc0;
			skb->DW1 = desc1;
			/*skb->DW2 = desc2;*/
			skb->DW2 = (u32)data_ptr;
			skb->DW3 = desc3;
			skb_put(skb, data_len);

			dp_rx(skb, 0);
		} else {
			cbm_buffer_free(smp_processor_id(), data_ptr, 0);
			pr_err("%s: failure in allocating skb\r\n", __func__);
		}

		g_unmatched_entry++;
		/* Read the EEFLAG back */
		entries = ltq_toe_r32(LRO_EXP_EFLAG);
	}

	read_pos = i;

	/* Unmask the interrupt */
	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
	ltq_toe_w32_mask(0, (1 << TOE_INT_MASK_LRO_EXP_POS), TOE_INT_EN);
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
	return;
}

static void ltq_lro_tasklet (unsigned long dev)
{
	uint32_t port, context_ready;
	uint32_t current_context = 0;
	struct ltq_lro_port *pport = (struct ltq_lro_port *)dev;
	uint8_t budget = 20;
	unsigned long tso_rl_flags;

	port = pport->port_num;

	/* Clear the INT status */
	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
	ltq_toe_w32((1 << (port + TOE_INT_MASK_LRO0_POS)), TOE_INT_STAT);
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);

	context_ready = ltq_toe_r32(LRO_OC_OWNER(port, current_context));
	if (!context_ready) {
		current_context = !current_context;
		context_ready = ltq_toe_r32(LRO_OC_OWNER(port, current_context));
	}

	/* Keep checking the ownership bit till its not HW */
	while (context_ready && budget) {
		lro_process_output_context(port, current_context);
		current_context = !current_context;
		context_ready = ltq_toe_r32(LRO_OC_OWNER(port, current_context));
		--budget;
	}

	if (ltq_toe_r32(LRO_OC_OWNER(port, 0)) ||
	    ltq_toe_r32(LRO_OC_OWNER(port, 1))) {
		/* Keep interrupt disabled, re-schedule tasklet to process the IRQ */
		tasklet_schedule(&lro_tasklet[port]);
		return;
	}

	/* Unmask the interrupt for other output context */
	spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
	ltq_toe_w32_mask(0, (1 << (port + TOE_INT_MASK_LRO0_POS)), TOE_INT_EN);
	wmb();
	spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
}

static struct ltq_lro_port *ltq_allocate_lro_port (void)
{
	int i;

	for (i = 0; i < LTQ_MAX_LRO_PORTS; i++) {
		if (!ltq_lro_port[i].in_use) {
			ltq_lro_port[i].in_use = 1;
			return &ltq_lro_port[i];
		}
	}
	return NULL;
}

int lro_start_flow (int *session_id, int timeout, int flags, struct cpumask cpumask)
{
	struct ltq_lro_port *pport;
	u32 port;
	unsigned long tso_rl_flags;

	/* Allocate a LRO port for the session */
	pport = ltq_allocate_lro_port();

	if (!pport) {
		pr_debug("no more LRO ports \n");
		return -EBUSY;
	}

	port = pport->port_num;

	/*Mark the session id as port number for uniqueness*/
	*session_id = port;

	if((ltq_toe_r32(LRO_FID(port)) & LRO_FID_0_OWNER_MASK) == 0) {
		pr_debug("FID ownership still with the HW !\n");
		pport->in_use = 0;
		return -EBUSY;
	}

	pport->session_id = *session_id & LRO_FID_0_LRO_FID_MASK;

	/*pr_info("%s called with session_id = %x and port is: %d \n", __func__,*session_id & LRO_FID_0_LRO_FID_MASK, port);*/

	spin_lock_irqsave(&lro_register_lock, tso_rl_flags);
	ltq_toe_w32(timeout, LRO_TO_REG(port));

	/* Set the Flow ID */
	ltq_toe_w32((*session_id & LRO_FID_0_LRO_FID_MASK) << LRO_FID_0_LRO_FID_POS, LRO_FID(port));
	spin_unlock_irqrestore(&lro_register_lock, tso_rl_flags);

	wmb ();

	pr_info("started flow %u for session id = %x\n", port, ltq_toe_r32(LRO_FID(port)));

	/* Set the IRQ affinity */
	cpumask.bits[0] = pport->affinity;
	if (irq_set_affinity(pport->irq_num, &cpumask))
		pr_warn("%s: can not set affinity for IRQ - %d", __func__, pport->irq_num);

	return 0;
}
EXPORT_SYMBOL(lro_start_flow);

static struct ltq_lro_port *ltq_lookup_lro_port(int session_id)
{
	int i;

	for (i = 0; i < LTQ_MAX_LRO_PORTS; i++) {
		if (ltq_lro_port[i].session_id == session_id) {
			return &ltq_lro_port[i];
		}
	}
	return NULL;
}

#ifdef USE_TIMER_FOR_SESSION_STOP
int lro_real_stop_flow (int session_id, int timeout, int flags)
{
	struct ltq_lro_port *pport;
	u32 port;
	unsigned long tso_rl_flags;

	/* Allocate a LRO port for the session */
	pport = ltq_lookup_lro_port(session_id & LRO_FID_0_LRO_FID_MASK);

	if (!pport)
		return -EINVAL;

	/*pr_info("%s called\n", __func__);*/

	/* Disable the interrupt */
	//disable_irq(pport->irq_num);

	port = pport->port_num;

	spin_lock_irqsave(&lro_register_lock, tso_rl_flags);

	/* Set the S_END */
	ltq_toe_w32_mask(0, 1 << LRO_FID_0_S_END_POS, LRO_FID(port));

	/* Clear the OWNER */
	ltq_toe_w32_mask(LRO_FID_0_OWNER_MASK, 0,  LRO_FID(port));

	spin_unlock_irqrestore(&lro_register_lock, tso_rl_flags);
	wmb();
	mdelay(5);

	pr_info("stopped flow %u for session_id = %x\n", port, ltq_toe_r32(LRO_FID(port)));

#if 0
	while((ltq_toe_r32(LRO_FID(port)) & LRO_FID_0_OWNER_MASK) == 0) {
		pr_err_ratelimited("not owner of LRO_FID(%d): %x! (INT_STAT: %x DBG_INFO: %x EFLAG: %x OC_OWNER: %x/%x OC_FLAG: %x/%x)\n",
				   port,
				   ltq_toe_r32(LRO_FID(port)),
				   ltq_toe_r32(TOE_INT_STAT),
				   ltq_toe_r32(LRO_DBG_INFO),
				   ltq_toe_r32(LRO_EXP_EFLAG),
				   ltq_toe_r32(LRO_OC_OWNER(port, 0)),
				   ltq_toe_r32(LRO_OC_OWNER(port, 1)),
				   ltq_toe_r32(LRO_OC_FLAG(port, 0)),
				   ltq_toe_r32(LRO_OC_FLAG(port, 1))
				  );
		mdelay(200);
	}
#endif

	/* Make the port available */
	pport->in_use = 0;

	return 0;
}

static void lro_timer_fn (unsigned long data)
{
	struct ltq_lro_port *pport = (struct ltq_lro_port *)data;

	WARN_ON(!pport->in_use);

	pr_debug("LRO timer fired, session to be stopped !\n");
	lro_real_stop_flow(pport->session_id, 0, 0);
}

int lro_stop_flow (int session_id, int timeout, int flags)
{
	struct ltq_lro_port *pport;
	uint32_t port;

	/* Lookup the LRO port */
	pport = ltq_lookup_lro_port(session_id & LRO_FID_0_LRO_FID_MASK);
	port = pport->port_num;

	if (!pport) {
		pr_err("couldn't find the LRO port for session id: %d\n", session_id);
		return -EINVAL;
	}

	pr_info("triggering the stop the flow for %d\n", session_id);

	/* Flag the session should be stopped */
	mod_timer(&pport->lro_timer, jiffies + lro_time);

	return 0;
}
EXPORT_SYMBOL(lro_stop_flow);

#else

int lro_stop_flow (int session_id, int timeout, int flags)
{
	struct ltq_lro_port *pport;
	u32 port;
	unsigned long tso_rl_flags;

	/* Allocate a LRO port for the session */
	pport = ltq_lookup_lro_port(session_id & LRO_FID_0_LRO_FID_MASK);

	if (!pport)
		return -EINVAL;

	/*pr_info("%s called\n", __func__);*/

	/* Disable the interrupt */
	//disable_irq(pport->irq_num);

	port = pport->port_num;

#if 1
	while((ltq_toe_r32(LRO_FID(port)) & LRO_FID_0_OWNER_MASK) == 0) {
		pr_err_once("not owner of LRO_FID in stop !\n");
	}
#endif

	spin_lock_irqsave(&lro_register_lock, tso_rl_flags);

	/* Set the S_END */
	ltq_toe_w32_mask(0, 1 << LRO_FID_0_S_END_POS, LRO_FID(port));

	/* Clear the OWNER */
	ltq_toe_w32_mask(LRO_FID_0_OWNER_MASK, 0,  LRO_FID(port));

	spin_unlock_irqrestore(&lro_register_lock, tso_rl_flags);
	wmb();

	pr_debug("stopped flow %u for session_id = %x\n", port, ltq_toe_r32(LRO_FID(port)));
	mdelay(5);

	/* Make the port available */
	pport->in_use = 0;

	return 0;
}
EXPORT_SYMBOL(lro_stop_flow);
#endif

static int tso_configure_dma (void)
{
	int rxchan;
	int txchan;

	/* ----------------------------*/
	/*      TSO Port 0 channels   */
	/* ----------------------------*/

	pr_debug ("Initializing TSO Port 0 channels..\n");

	rxchan = DMA3_TOE_MEMCOPY_CLASS2_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS2_TX;

	/* Configure Pre-Processing Channels (4,5) */
	if ((ltq_request_dma(rxchan, "dma3 rx 4")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 4 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 5")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 5 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_4_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 4\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_5_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 5\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg0 channels (0,1) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS0_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS0_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 0")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 0 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 1")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 1 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_0_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 0\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_1_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 1\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg1 channels (2,3) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS1_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS1_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 2")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 2 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 3")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 3 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_2_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 0\r\n", __func__);
    }

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_3_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 1\r\n", __func__);
    }

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 0 channels (8,9) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS4_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS4_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 8")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 8 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 9")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 9 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_8_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 8\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_9_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 9\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 1 channels (10,11) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS5_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS5_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 10")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 10 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 11")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 11 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_10_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 10\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_11_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 11\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	pr_debug ("Initializing TSO Port 0 channels: Done !\n");

	/* ----------------------------*/
	/*      TSO Port 1 channels   */
	/* ----------------------------*/

	pr_debug ("Initializing TSO Port 1 channels !\n");

	rxchan = DMA3_TOE_MEMCOPY_CLASS3_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS3_TX;

	/* Configure Pre-Processing Channels (6,7) */
	if ((ltq_request_dma(rxchan, "dma3 rx 6")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 6 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 7")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 7 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_6_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 6\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_7_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 7\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg0 channels (30,31) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS15_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS15_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 30")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 30 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 31")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 31 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_30_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 0\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_31_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 1\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg1 channels (32,33) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS16_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS16_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 32")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 32 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 33")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 33 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_32_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 32\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_33_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 33\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 0 channels (38,39) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS19_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS19_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 38")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 38 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 39")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 39 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_38_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 38\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_39_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 39\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 1 channels (40,41) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS20_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS20_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 40")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 40 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 41")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 41 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_40_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 40\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_41_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 41\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	pr_debug ("Initializing TSO Port 1 channels: Done !\n");

	/* ----------------------------*/
	/*      TSO Port 2 channels   */
	/* ----------------------------*/
	pr_debug ("Initializing TSO Port 2 channels ! \n");

	rxchan = DMA3_TOE_MEMCOPY_CLASS17_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS17_TX;

	/* Configure Pre-Processing Channels (34,35) */
	if ((ltq_request_dma(rxchan, "dma3 rx 34")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 34 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 35")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 35 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_34_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 34\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_35_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 35\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg0 channels (54,55) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS27_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS27_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 54")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 54 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 55")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 55 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_54_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 54\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_55_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 55\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg1 channels (42,43) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS21_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS21_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 42")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 42 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 43")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 43 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_42_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 42\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_43_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 43\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 0 channels (48,49) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS24_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS24_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 48")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 48 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 49")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 49 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_48_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 48\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t) DMA3_TOE_MEMCPY_CHAN_49_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 49\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 1 channels (50,51) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS25_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS25_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 50")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 50 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 51")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 51 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_50_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 50\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_51_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 51\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	pr_debug ("Initializing TSO Port 2 channels: Done! \n");

	/* ----------------------------*/
	/*      TSO Port 3 channels   */
	/* ----------------------------*/

	pr_debug ("Initializing TSO Port 3 channels ! \n");

	rxchan = DMA3_TOE_MEMCOPY_CLASS18_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS18_TX;

	/* Configure Pre-Processing Channels (36,37) */
	if ((ltq_request_dma(rxchan, "dma3 rx 36")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 36 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 37")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 37 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_36_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 36\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_37_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 37\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg0 channels (62,63) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS31_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS31_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 62")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 62 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 63")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 63 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_62_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 62\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_63_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 63\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Configure Seg1 channels (52,53) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS26_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS26_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 52")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 52 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 53")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 53 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_52_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 52\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_53_DESC_BASE, 2)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 53\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);

	/* Enqueue 0 channels (58,59) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS29_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS29_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 58")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 58 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 49")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 59 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_58_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 58\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_59_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 59\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);


	/* Enqueue 1 channels (60,61) */
	rxchan = DMA3_TOE_MEMCOPY_CLASS30_RX;
	txchan = DMA3_TOE_MEMCOPY_CLASS30_TX;

	if ((ltq_request_dma(rxchan, "dma3 rx 60")) < 0)
		pr_debug(" %s failed to open chan for dma3 rx chan 60 \n", __func__);

	if ((ltq_request_dma(txchan, "dma3 tx 61")) < 0)
		pr_debug(" %s failed to open chan for dma3 tx chan 61 \n", __func__);

	if ((ltq_dma_chan_desc_cfg(rxchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_60_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for rxchan 60\r\n", __func__);
	}

	if ((ltq_dma_chan_desc_cfg(txchan, (dma_addr_t)DMA3_TOE_MEMCPY_CHAN_61_DESC_BASE, 1)) < 0) {
		pr_debug(" %s failed to setup chan desc for txchan 61\r\n", __func__);
	}

	ltq_dma_chan_irq_enable(rxchan);
	ltq_dma_chan_irq_enable(txchan);

	ltq_dma_chan_on(rxchan);
	ltq_dma_chan_on(txchan);


	pr_debug ("Initializing TSO Port 3 channels: Done! \n");
	return 0;
}

static void configure_lro(void)
{
	int i;
	u32 lro_en = 0;

	/* Maximum aggregation size to 2 */
	ltq_toe_w32_mask(TSO_GCTRL_LRO_MAX_AGP_MASK, (LRO_MAX_AGP << TSO_GCTRL_LRO_MAX_AGP_POS), TSO_GCTRL);

	/* Enable all the LRO ports */
	for (i = 0; i < LTQ_MAX_LRO_PORTS; i++)
		lro_en |= (1 << (TSO_GCTRL_LRO0_EN_POS + i));
	ltq_toe_w32_mask(0, lro_en, TSO_GCTRL);

	/* Enable the interrupts */
	for (i = 0; i < LTQ_MAX_LRO_PORTS; i++)
		lro_en |= (1 << (TOE_INT_EN_LRO0_POS + i));
	ltq_toe_w32_mask(0, lro_en, TOE_INT_EN);

	ltq_toe_w32_mask(0, (1 << TOE_INT_EN_LRO_EXP_POS), TOE_INT_EN);
#if 0
	ltq_toe_w32_mask(0, (1 << TOE_INT_EN_S20_POS), TOE_INT_EN);
	ltq_toe_w32_mask(0, (1 << TOE_INT_EN_S21_POS), TOE_INT_EN);
	ltq_toe_w32_mask(0, (1 << TOE_INT_EN_S22_POS), TOE_INT_EN);
	ltq_toe_w32_mask(0, (1 << TOE_INT_EN_S23_POS), TOE_INT_EN);
#endif

	/* Unmask LRO interrupts */
	lro_en = 0;
	for (i = 0; i < LTQ_MAX_LRO_PORTS; i++)
		lro_en |= (1 << (TOE_INT_MASK_LRO0_POS + i));
	ltq_toe_w32_mask(lro_en, 0, TOE_INT_MASK);

	ltq_toe_w32_mask((1 << TOE_INT_MASK_LRO_EXP_POS), 0, TOE_INT_MASK);
#if 0
	ltq_toe_w32_mask((1 << TOE_INT_MASK_S20_POS), 0, TOE_INT_MASK);
	ltq_toe_w32_mask((1 << TOE_INT_MASK_S21_POS), 0, TOE_INT_MASK);
	ltq_toe_w32_mask((1 << TOE_INT_MASK_S22_POS), 0, TOE_INT_MASK);
	ltq_toe_w32_mask((1 << TOE_INT_MASK_S23_POS), 0, TOE_INT_MASK);
#endif
}

static void configure_tso (void)
{
	pr_debug ("Enabling the TSO..\n");

	/* Enable the TSO */
	ltq_toe_w32_mask(0, (1 << TSO_GCTRL_ENA_P0_POS), TSO_GCTRL);
	ltq_toe_w32_mask(0, (1 << TSO_GCTRL_ENA_P1_POS), TSO_GCTRL);
	ltq_toe_w32_mask(0, (1 << TSO_GCTRL_ENA_P2_POS), TSO_GCTRL);
	ltq_toe_w32_mask(0, (1 << TSO_GCTRL_ENA_P3_POS), TSO_GCTRL);

	pr_debug ("Setup the SRAM address as scratchpad.. \n");
	/* Setup the SRAM addresses for scratchpad */
	ltq_toe_w32(0x1F806000, HDR_BASE_SEG0);
	/*ltq_toe_w32(0x1F806800, toe_membase + HDR_BASE_SEG1);*/
	ltq_toe_w32(0x1F807000, HDR_BASE_SEG1);

	/* Disable the swap */
	/*ltq_toe_w32_mask(TSO_GCTRL_OCP_CTRL_ENDI_B_MASK, 0, TSO_GCTRL);*/
	/* Enable the word swap */
	ltq_toe_w32_mask(0, (1 << TSO_GCTRL_OCP_CTRL_ENDI_W_POS), TSO_GCTRL);

	/* Enable the DMA MCOPY Interrupts */
	ltq_toe_w32_mask(0, (1 << TSO_INTL_INT_EN_MCPY0_DONE_POS), TSO_INTL_INT_EN);
	ltq_toe_w32_mask(0, (1 << TSO_INTL_INT_EN_MCPY1_DONE_POS), TSO_INTL_INT_EN);
	ltq_toe_w32_mask(0, (1 << TSO_INTL_INT_EN_MCPY2_DONE_POS), TSO_INTL_INT_EN);
	ltq_toe_w32_mask(0, (1 << TSO_INTL_INT_EN_MCPY3_DONE_POS), TSO_INTL_INT_EN);

	if (is_tso_IRQMode()) {
		/* Enable the interrupts */
		ltq_toe_w32_mask(0, (1 << TOE_INT_EN_TOE0_POS), TOE_INT_EN);
		ltq_toe_w32_mask(0, (1 << TOE_INT_EN_TOE1_POS), TOE_INT_EN);
		ltq_toe_w32_mask(0, (1 << TOE_INT_EN_TOE2_POS), TOE_INT_EN);
		ltq_toe_w32_mask(0, (1 << TOE_INT_EN_TOE3_POS), TOE_INT_EN);

		/* Unmask only TSO interrupts */
		ltq_toe_w32_mask((1 << TOE_INT_MASK_TOE0_POS), 0, TOE_INT_MASK);
		ltq_toe_w32_mask((1 << TOE_INT_MASK_TOE1_POS), 0, TOE_INT_MASK);
		ltq_toe_w32_mask((1 << TOE_INT_MASK_TOE2_POS), 0, TOE_INT_MASK);
		ltq_toe_w32_mask((1 << TOE_INT_MASK_TOE3_POS), 0, TOE_INT_MASK);
	}
}

static inline struct toe_sg_buffer* tso_get_sgbuff(ltq_tso_port_t* tsoPort)
{
	struct toe_sg_buffer* sgBuff;

	if (0 == atomic_read(&(tsoPort->availBuffs))) {
		return NULL;
	}

	atomic_dec(&(tsoPort->availBuffs));
	sgBuff = (tsoPort->sgBuffs+tsoPort->rPos);
	tsoPort->rPos = ((tsoPort->rPos+1)%SG_BUFFER_PER_PORT);

	return sgBuff;
}

static inline void tso_free_toebuf(ltq_tso_port_t* tsoPort, struct toe_sg_buffer* sgBuff, int error)
{
	/*
	 * Expected to be exucted on one CPU and no scheduling expected when
	 * this function is invoked.
	 */

	dev_kfree_skb_any(sgBuff->skb);
	sgBuff->skb = NULL;
	if (error)  {
		pr_info("TSO: TSO transmit error\n");
		++tsoPort->TxHwError;
	} else {
		++tsoPort->TxDonePackets;
	}
	tsoPort->wPos = ((tsoPort->wPos + 1) % SG_BUFFER_PER_PORT);
	atomic_inc(&(tsoPort->availBuffs));
}

int tso_enqueue_hw(ltq_tso_port_t* tsoPort,
			struct toe_sg_buffer* sgBuff)
{
	int nfrags, len;
	void *frag_addr;
	const skb_frag_t *frag;
	int port;
	struct skb_shared_info *shinfo = NULL;
	unsigned int toe_g = 1;
	unsigned int toe_sioc = 0;
	unsigned int toe_last = 0;
	unsigned int ie = 1;
	unsigned short gso_size;
	int ret = 0;
	struct sk_buff *skb= sgBuff->skb;

	port = tsoPort->port_number;
	shinfo = skb_shinfo(skb);


	if (!toe_tso_port_ready(port,10000)) {
		pr_err("TSO Error: TSO port(%d) is not ready\n",port);
		ret = -1; //TSO port not available
		goto tsoXmitDoneErr;
	}

	if (!(shinfo->nr_frags)) {
		 toe_g = 0;
  	}
	gso_size = ((shinfo->gso_size) > 1546) ? 1546 : (shinfo->gso_size);

 	/* Setup 1st command of gather in cmd registers */
 	/* Check that CMD port is available */
 	len = skb->len - skb->data_len;
	toe_fill_cmd0(1, 1, ie, toe_g, 2, len, toe_last, port);
 	toe_fill_cmd1(skb->data, ~toe_sioc, port, len);
 	toe_fill_cmd2(skb->DW0, port);
 	toe_fill_cmd3(skb->DW1, port);

	if (likely(toe_g)) {
		toe_fill_cmd4((sgBuff->sgBuffer), port);
	} else {
		toe_fill_cmd4_sbk((u32)(sgBuff), port);
	}
	wmb();
	toe_fill_cmd5(gso_size, 1, port);

	/* Write the command registers to start the gathering*/
	for (nfrags = 0; nfrags < shinfo->nr_frags ; nfrags++) {
			/* Check if last segment of Gather */
			if (nfrags == (shinfo->nr_frags - 1)) {
				toe_last = 1 ;
			}
			frag = &shinfo->frags[nfrags];

			/* Check that CMD port is available */
			while (!toe_tso_port_ready(port,3000)) {
				pr_err("TSO: Waiting to finish SG job for %d port frags=%d/%d \n DEBUG_CFG=0x%X Internal INT_STAT=0x%X\n",
						port, nfrags,shinfo->nr_frags,ltq_toe_r32(TSO_DEBUG_CFG),ltq_toe_r32(TSO_INTL_INT_STAT));
			}
			/* CMD0 - Fill frag length */
			toe_fill_cmd0(1, 1, ie, 1, 2, skb_frag_size(frag), toe_last, port);

			/* CMD1 - Fill frag  */
			frag_addr = skb_frag_address(frag);
			toe_fill_cmd1_frags(frag_addr, ~toe_sioc, port, skb_frag_size(frag));

			/* */
			toe_fill_cmd4_sbk((u32)sgBuff, port);
			wmb();

			/* CMD5 */
			toe_fill_cmd5(gso_size, 1, port);

			pr_debug ("TSO: SG for frag:%d with G.. \n", nfrags);
	}

	tso_write_stats(tsoPort,skb);

	return 0;

tsoXmitDoneErr:  //FIX: If TSO commands return with error, then sgBuffer is lost
	++tsoPort->TxErrorPackets;
	return ret;
}

#if 0 /* unused */
static void tso_timer(unsigned long data)
{
	ltq_tso_port_t* tsoPort = (ltq_tso_port_t*)(data);

	pr_info_once("TSO timer for = %d\n", tsoPort->port_number);

	if (skb_queue_len(&tsoPort->processQ) ||
			tso_results_pending(tsoPort))
				tasklet_schedule(&tsoPort->tasklet);

	return;
}
#endif

static void ltq_tso_tasklet(unsigned long dev)
{
	ltq_tso_port_t *tsoPort = (ltq_tso_port_t *) dev;
	u32 port;
	int tsoBudget = SG_BUFFER_PER_PORT;
	struct toe_sg_buffer* sgBuff = NULL;

	port = tsoPort->port_number;

	pr_info_once("TSO tasklet port = %d\n", port);

	wmb();
	process_tso_results(tsoPort); /* Process TSO result"S" of this port*/
	tsoPort->processQ_maxL = maxV(skb_queue_len(&tsoPort->processQ),
					tsoPort->processQ_maxL);

	while (skb_queue_len(&tsoPort->processQ) &&
			tso_sgbuffs_available(tsoPort) &&
			tsoBudget) {

		sgBuff = tso_get_sgbuff(tsoPort);
		BUG_ON(sgBuff == NULL);
		sgBuff->skb = skb_dequeue(&tsoPort->processQ);
		if (tso_enqueue_hw(tsoPort, sgBuff)) {
			tso_free_toebuf(tsoPort, sgBuff, 1);
			break;
		}
		--tsoBudget;
	}

	return;
}

#define TSO_USE_TASKLET_IN_IRQ 1
static irqreturn_t ltq_tso_tx_int(int irq, void *data)
{
	unsigned long tso_rl_flags;
	struct ltq_tso_port *tsoPort = (struct ltq_tso_port *)(data);
	int port;

	port = tsoPort->port_number;

	if (ltq_toe_r32(TOE_INT_STAT) & (1 << port)) {
#ifndef TSO_USE_TASKLET_IN_IRQ
		spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
		/* Mask the interrupt */
		ltq_toe_w32_mask((1<<port), 0, TOE_INT_EN);

		wmb();
		spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);

		process_tso_results(tsoPort); /* Process TSO result"S" of this port */

		spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
		ltq_toe_w32((1<<port), TOE_INT_STAT);	/* Clear the interrupt */
		ltq_toe_w32_mask(0, (1<<port), TOE_INT_EN);	/* Unmask the interrupt */
		wmb();
#error "Unexcpted compilation"
		spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
#else
		spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
		/* Mask the interrupt */
		ltq_toe_w32_mask((1 << port), 0, TOE_INT_EN);
		wmb();
		spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
		/* Schedule the tasklet for housekeeping */
		tasklet_schedule(&tsoPort->tasklet);
#endif
	} else {
		pr_err("TSO: Interrupt error. Unexpected on CPU %d ( stat = %X) !!\n", (1 << port), ltq_toe_r32(TOE_INT_STAT));
	}
	return IRQ_HANDLED;
}


#define TSO_DELAY() asm("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop")

static inline int process_tso_results(ltq_tso_port_t* tsoPort)
{
	u32 port = tsoPort->port_number;
        u32 resReg1 = 0;
	int results = 0;
	struct toe_sg_buffer* sgBuff_prev = NULL;
	struct toe_sg_buffer* sgBuff_curr = NULL;
	unsigned long tso_rl_flags;
	int done;

	do {
		done = 0;
		sgBuff_curr = (struct toe_sg_buffer *) ltq_toe_r32(PORT_RES_REG0(port));
		resReg1 = ltq_toe_r32(PORT_RES_REG1(port));
		if ((ltq_toe_r32(TOE_INT_STAT) & (1 << port))) {
			/* Make sure current buffer is not same as previous
			 * released buffer */
			if (sgBuff_curr)
				tso_free_toebuf(tsoPort, sgBuff_curr, (resReg1 & PORT_RES_REG1_0_ERR_MASK));
			else
				pr_err("TSO: Process Result has no packet buffer info\n");
			sgBuff_prev = sgBuff_curr;
			done = 1;
			++results;

			spin_lock_irqsave(&toe_register_lock, tso_rl_flags);
			/* Clear the interrupt */
			ltq_toe_w32((1 << port), TOE_INT_STAT);
			/* Unmask the interrupt */
			ltq_toe_w32_mask(0, (1 << port), TOE_INT_EN);
			spin_unlock_irqrestore(&toe_register_lock, tso_rl_flags);
		}
		TSO_DELAY(); /* delay before read next TSO result*/
	} while (done);

	return results;
}


static inline void tso_write_stats(ltq_tso_port_t* tsoPort,struct sk_buff* skb)
{
	int frIndex = skb_shinfo(skb)->nr_frags;

	++tsoPort->TxPackets;
	frIndex = (frIndex >=  MAX_TSO_FRAGS_STATS) ? (MAX_TSO_FRAGS_STATS) : (frIndex);
	++tsoPort->fragsStats[frIndex].TxPackets;
	tsoPort->fragsStats[frIndex].TxBytes += skb->len;
	tsoPort->fragsStats[frIndex].TxMaxBytes =
		maxV(skb->len, tsoPort->fragsStats[frIndex].TxMaxBytes);

}

int ltq_tso_xmit(struct sk_buff *skb,
			struct pmac_tx_hdr *pmac,
			int hdr_size, int flags )
{
	int port;
	ltq_tso_port_t *tsoPort;

	tsoPort = (ltq_tso_port + smp_processor_id());
	port = tsoPort->port_number;

	pr_debug("%s: called.. with len:%d data:%x on port: %d nr_frags: %d\n",
			__FUNCTION__, skb->len,
			(unsigned int)skb->data,
			port,
			skb_shinfo(skb)->nr_frags);


	/* if there is no headroom for PMAC header, try to expand */
	if (skb_headroom(skb) < hdr_size) {
		pr_err("TSO: headroom is lesser than expeted (port=%d)\n",port);
		if (pskb_expand_head(skb, hdr_size, 0, GFP_ATOMIC)) {
			goto tsoErr;
		}
	}

	ip_hdr(skb)->check = 0;
	tcp_hdr(skb)->check = 0;
	/* copy the pmac header to the tx data */
	skb_push(skb, hdr_size);
	memcpy(skb->data, pmac, hdr_size);
	/* enqueue to processQ */
	skb_queue_tail(&tsoPort->processQ, skb);
	tasklet_schedule(&tsoPort->tasklet);

	return 0;

tsoErr:
	dev_kfree_skb_any(skb);
	++tsoPort->TxErrorPackets;
	return -1;
}
EXPORT_SYMBOL(ltq_tso_xmit);

static int toe_reg_read_proc(struct seq_file *s, void *v)
{
	int port;
	int regNo;
	struct {
		char* name;
		uint32_t RegAddr;
	} TSO_CmdReg[8] = {
		{"REQ_CMD_REG0",  	PORT_REQ_CMD_REG0_0 },
		{"REQ_CMD_REG1",  	PORT_REQ_CMD_REG1_0 },
		{"REQ_CMD_REG2",  	PORT_REQ_CMD_REG2_0 },
		{"REQ_CMD_REG3",  	PORT_REQ_CMD_REG3_0 },
		{"REQ_CMD_REG4",  	PORT_REQ_CMD_REG4_0 },
		{"REQ_CMD_REG5",  	PORT_REQ_CMD_REG5_0 },
		{"Result  RES-0",	PORT_RES_REG0_0 },
		{"Result  RES-1", 	PORT_RES_REG1_0 }
	};

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	seq_puts(s, "===============TOE GLobal Registers ==================\n");
	seq_printf(s, "ToE base address 0x%08x\n", (unsigned int) ltq_toe_membase);
	seq_printf(s, "TSO_GCTRL: addr 0x%08x value 0x%08x\t\n", (unsigned int)(ltq_toe_membase + TSO_GCTRL), ltq_toe_r32(TSO_GCTRL));
	seq_printf(s, "HDR_BASE_SEG0: addr 0x%08x value 0x%08x\t\n", (unsigned int) (ltq_toe_membase + HDR_BASE_SEG0), ltq_toe_r32(HDR_BASE_SEG0));
	seq_printf(s, "HDR_BASE_SEG1: addr 0x%08x value 0x%08x\t\n", (unsigned int) (ltq_toe_membase + HDR_BASE_SEG1), ltq_toe_r32(HDR_BASE_SEG1));
	seq_printf(s, "TOE_INT_MASK: addr 0x%08x value 0x%08x\t\n", (unsigned int)(ltq_toe_membase + TOE_INT_MASK), ltq_toe_r32(TOE_INT_MASK));
	seq_printf(s, "TOE_INT_STAT: addr 0x%08x value 0x%08x\t\n", (unsigned int)(ltq_toe_membase + TOE_INT_STAT), ltq_toe_r32(TOE_INT_STAT));
	seq_printf(s, "TOE_INT_EN: addr 0x%08x value 0x%08x\t\n", (unsigned int)(ltq_toe_membase + TOE_INT_EN), ltq_toe_r32(TOE_INT_EN));

	seq_printf(s, "\n========== LRO FSM Info ============");
	seq_printf(s, "\nLRO_DBG_INFO: 0x%08X\n\n" ,ltq_toe_r32(LRO_DBG_INFO));
	seq_printf(s, "\nPort\tActive\t\tFID\t\tTimeout\t\tOC-1-OWN/OC_REG\t\tOC-2-OWN/OC_REG");
	for (port = 0; port < LTQ_MAX_LRO_PORTS; port++) {
		seq_printf(s, "\n%d\t%s\t\t0x%08X\t0x%08X\t0x%08X/0x%08X\t0x%08X/0x%08X",
			port,
			(ltq_lro_port[port].in_use ? "YES" : "NO"),
			ltq_toe_r32(LRO_FID(port)),
			ltq_toe_r32(LRO_TO_REG(port)),
			ltq_toe_r32(LRO_OC_OWNER(port,0)),ltq_toe_r32(LRO_OC_FLAG(port,0)),
			ltq_toe_r32(LRO_OC_OWNER(port,1)),ltq_toe_r32(LRO_OC_FLAG(port,1)) );
	}

	seq_puts(s, "\n\n========================================================\n\n");

	seq_printf(s, "\nTSO_DEBUG_CFG=0x%08X\tINTL_INT_STAT=0x%08X\nTSO_INTL_INT_EN=0x%08X\tTSO_INTL_INT_MASK=0x%08X\n",
			ltq_toe_r32(TSO_DEBUG_CFG),
			ltq_toe_r32(TSO_INTL_INT_STAT),
			ltq_toe_r32(TSO_INTL_INT_EN),
			ltq_toe_r32(TSO_INTL_INT_MASK));

	seq_printf(s, "RegName\t\tPort-0\t\tPort-1\t\tPort-2\t\tPort-3");
	for (regNo = 0; regNo < 8; regNo++) {
		seq_printf(s, "\n%s:  0x%08x\t0x%08x\t0x%08x\t0x%08x",
				TSO_CmdReg[regNo].name,
				ltq_toe_r32(TSO_CmdReg[regNo].RegAddr),
				ltq_toe_r32(TSO_CmdReg[regNo].RegAddr+0x20),
				ltq_toe_r32(TSO_CmdReg[regNo].RegAddr+0x20*2),
				ltq_toe_r32(TSO_CmdReg[regNo].RegAddr+0x20*3));
	}
	seq_puts(s, "\n");
	return 0;
}

static int ltq_reg_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, toe_reg_read_proc, NULL);
}

static const struct file_operations ltq_toe_reg_proc_fops = {
	.open = ltq_reg_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

ssize_t ltq_stats_write (struct file *file, const char *user_buf,
	size_t count, loff_t *ppos)
{
	int len = 0, tmp = 0, cpu;
	char str[25] = {0};
	char *p = NULL;
	ltq_tso_port_t *tsoPort;

        if (!capable(CAP_NET_ADMIN)) {
             printk ("Write Permission denied");
             return 0;
        }

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= copy_from_user(str, user_buf, len);
	while (len && str[len - 1] <= ' ')
		len--;

	str[len] = 0;

	for (p = str; *p && *p <= ' '; p++, len--)
	;

	if (!*p)
		return count;

	kstrtol(p, 10, (long *)&tmp);

	if (tmp == 0) {
		for_each_online_cpu(cpu) {
			tsoPort = (ltq_tso_port + cpu);
			memset(tsoPort->fragsStats, 0, sizeof(struct tso_frags_stats) * MAX_TSO_FRAGS_STATS);
		}
		g_unmatched_entry = 0;
	} else {
		pr_info("Invalid input!\n");
		pr_info("echo 0 > /proc/driver/ltq_toe/stats to clear\n");
	}
	return len;
}

static int lro_stats_read_proc(struct seq_file *s, void *v)
{
	int port;
	int cpu;
	int frFrags;

	char *execption_name[9] = {"Flush based on GSWIP", "TCP checksum err", "FID mismatch",
					"TCP zero length pkt", "TCP timestamp check error",
					"Excessive length", "Session timeout",
					"TCP seq number out of order", "TCP type error"};

	seq_puts(s, "===============LRO Stats==================\n");
	seq_puts(s, "\n-------------- LRO Exceptions -----------\n");
	for (port = 0; port < LRO_MAX_EXCEPTION_COUNT; port++)
		seq_printf(s, "%s: \t\t%d\n", execption_name[port], (unsigned int) lro_num_except[port]);

	seq_printf(s, "\nNumber of success: \t\t%d\n", (unsigned int) lro_num_success);
	seq_printf(s, "lro unmatched pkt count: \t\t%d\n", (unsigned int) g_unmatched_entry);
	seq_puts(s, "========================================================\n\n");
	for_each_online_cpu(cpu) {
		ltq_tso_port_t *tsoPort = (ltq_tso_port + cpu);
		seq_printf(s, "=============== TSO port %d Stats==================\n", cpu);
		seq_printf(s, "TxPkts: %d\tTxDone: %d\tTxErr: %d\tTxHWErr: %d\tSG Buffs: %d\tPrcsQ len: %u\tPrcsQ MaxL: %u\n\n",
			tsoPort->TxPackets,
			tsoPort->TxDonePackets,
			tsoPort->TxErrorPackets,
			tsoPort->TxHwError,
			atomic_read(&(tsoPort->availBuffs)),
			skb_queue_len(&tsoPort->processQ),
			tsoPort->processQ_maxL);
		for (frFrags = 0; frFrags <  MAX_TSO_FRAGS_STATS + 1; ++frFrags) {
			if (tsoPort->fragsStats[frFrags].TxPackets != 0) {
				seq_printf(s, "[%u]:\t %u\t %llu\t %u\n",
				frFrags,
				tsoPort->fragsStats[frFrags].TxPackets,
				tsoPort->fragsStats[frFrags].TxBytes,
				tsoPort->fragsStats[frFrags].TxMaxBytes);
			}
		}
	}
	seq_puts(s, "\n");
	return 0;
}

static int ltq_stats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, lro_stats_read_proc, NULL);
}

ssize_t lro_proc_write(struct file *file, const char *buf, size_t count,
			     loff_t *ppos)
{
	u32 flow_id;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	/* Read the Flow ID */
	flow_id = ltq_toe_r32(LRO_FID(0)) & LRO_FID_0_LRO_FID_MASK;
	pr_info("current flow id = %d\n", flow_id);

	/* Stop the flow */
	if (lro_stop_flow(flow_id, 0, 0) < 0)
		pr_err("session couldn't be deleted..\n");
	else
		pr_info("session %d deleted..\n", flow_id);

	return count;
}

static int lro_lroFlow_read_proc(struct seq_file *s, void *v)
{
	seq_puts(s, "LRO Flow\n");
	return 0;
}

static int ltq_lroFlow_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, lro_lroFlow_read_proc, NULL);
}

static const struct file_operations ltq_lro_proc_fops = {
	.open = ltq_lroFlow_seq_open,
	.read = seq_read,
	.write = lro_proc_write,
	.llseek = seq_lseek,
	.release = seq_release,
};
static const struct file_operations ltq_stats_proc_fops = {
	.open = ltq_stats_seq_open,
	.read = seq_read,
	.write = ltq_stats_write,
	.llseek = seq_lseek,
	.release = seq_release,
};

#ifdef LRO_DEBUG
ssize_t lrodbg_proc_write(struct file *file, const char *user_buf, size_t count,
			     loff_t *ppos)
{
	int len = 0, port = 0;
	char str[25] = {0};
	char *p = NULL;

        if (!capable(CAP_NET_ADMIN)) {
             printk ("Write Permission denied");
             return 0;
        }

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= copy_from_user(str, user_buf, len);
	while (len && str[len - 1] <= ' ')
		len--;

	str[len] = 0;

	for (p = str; *p && *p <= ' '; p++, len--)
	;

	if (!*p)
		return count;

	kstrtol(p, 10, (long *)&port);

	if ((port >= 0) && (port < 8)) {
		pr_info("starting flow %u for session_id = %x\n", port, ltq_toe_r32(LRO_FID(port)));
		tasklet_schedule(&lro_tasklet[port]);
	} else {
		pr_info("Invalid input!\n");
		pr_info("echo <lroport> > /proc/driver/ltq_toe/lrodbg to clear\n");
	}
	return len;
}

static int lro_lrodbg_read_proc(struct seq_file *s, void *v)
{
	seq_puts(s, "Not implemented\n");
	return 0;
}

static int ltq_lrodbg_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, lro_lrodbg_read_proc, NULL);
}

static const struct file_operations ltq_lrodbg_proc_fops = {
	.open = ltq_lrodbg_seq_open,
	.read = seq_read,
	.write = lrodbg_proc_write,
	.llseek = seq_lseek,
	.release = seq_release,
};
#endif

/* TSO XMIT mode related proc ops */

static int tso_xmit_mode_read_proc(struct seq_file *s, void *v)
{
	seq_printf(s, "TSO is in %s\n", is_tso_IRQMode()?("IRQ Mode"):("POLLING Mode") );
	return 0;
}

static int tso_xmit_mode_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, tso_xmit_mode_read_proc, NULL);
}

ssize_t tso_xmit_mode_proc_write(struct file *file, const char *user_buf, size_t count,
			     loff_t *ppos)
{
	char buf[256];
	int num;
	char *param_list[2];
	unsigned char *str;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (copy_from_user(buf, user_buf, min(count, (sizeof(buf) - 1))))
		return -EFAULT;

	num = dp_split_buffer(buf, param_list, ARRAY_SIZE(param_list));

	if (num <= 0 || num >= 2 || dp_strncmpi(param_list[0], "help", strlen("help")) == 0) {
		pr_info("echo irq/polling > /proc/driver/ltq_toe/tso_xmit_mode \n");
		return count;
	}

	str = param_list[0];

	if(strcmp(str, "irq") == 0) {
		int cpu;
		struct cpumask cpumask;

		tso_set_IRQMode();
		for_each_online_cpu(cpu) {
			enable_irq(ltq_tso_port[cpu].irq);
			ltq_toe_w32_mask(0, (1 << cpu), TOE_INT_EN);
			ltq_toe_w32_mask((1 << cpu), 0, TOE_INT_MASK);
			cpumask.bits[0] = (1 << cpu);
			if (irq_set_affinity(ltq_tso_port[cpu].irq, &cpumask))
				pr_err("TSO: FAILED. Can not set affinity for IRQ - %d and port %d\n", ltq_tso_port[cpu].irq, cpu);
			else
				pr_err("TSO: Interrupt affinity for irq %d to cpu %d\n", ltq_tso_port[cpu].irq, (1 << cpu));
		}

	} else if(strcmp(str, "polling") == 0) {
		int cpu;

		tso_set_pollingMode();
		for_each_online_cpu(cpu) {
			disable_irq(ltq_tso_port[cpu].irq);
			/* Disable the irqs at HW level */
			ltq_toe_w32_mask((1 << cpu), 0, TOE_INT_EN);
			ltq_toe_w32_mask(0, (1 << cpu), TOE_INT_MASK);
		}
	} else {
		pr_info("Unknown TSO xmit mode !\n");
		pr_info("echo irq/polling > /proc/driver/ltq_toe/tso_xmit_mode \n");
	}

	return count;
}

static const struct file_operations tso_xmit_mode_proc_fops = {
	.open = tso_xmit_mode_seq_open,
	.read = seq_read,
	.write = tso_xmit_mode_proc_write,
	.llseek = seq_lseek,
	.release = seq_release,
};

/* Debug info related proc */
#ifdef LRO_DEBUG
static void *lro_dbg_info_seq_start(struct seq_file *s, loff_t *pos)
{
	if (*pos > dbg_head)
		return NULL;
	return &dbg_info[*pos];
}

static void *lro_dbg_info_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	if (++*pos > dbg_head)
		return NULL;
	return &dbg_info[*pos];
}

static void lro_dbg_info_seq_stop(struct seq_file *s, void *v)
{

}

static int lro_dbg_info_seq_show(struct seq_file *s, void *v)
{
	struct lro_dbg_info *pinfo = (struct lro_dbg_info *)v;

	seq_puts(s, "===============LRO Debug info==================\n");
	seq_printf(s, "Except reason: %d \t\n", pinfo->except_reason);
	seq_printf(s, "TCP sequence no: %x \t\n", pinfo->tcp_seq_no);
	seq_printf(s, "Total aggr len: %d \t\n", pinfo->aggr_len);
	seq_puts(s, "<===============>\n");
	return 0;
}

static const struct seq_operations lro_debug_info_seq_ops = {
	.start = lro_dbg_info_seq_start,
	.next = lro_dbg_info_seq_next,
	.stop = lro_dbg_info_seq_stop,
	.show = lro_dbg_info_seq_show,
};

static int ltq_lro_dbg_info_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &lro_debug_info_seq_ops);
}

static const struct file_operations ltq_lro_debug_info_proc_fops = {
	.open = ltq_lro_dbg_info_seq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};
#endif

/** create proc for debug  info, used ifx_dma_init_module */
static int ltq_toe_proc_init (void)
{
	struct proc_dir_entry *entry;

	g_toe_dir = proc_mkdir("driver/ltq_toe", NULL);
	if (!g_toe_dir)
		return -ENOMEM;

	entry = proc_create("register", 0, g_toe_dir, &ltq_toe_reg_proc_fops);
	if (!entry)
		goto err1;

	entry = proc_create("stats", 0, g_toe_dir, &ltq_stats_proc_fops);
	if (!entry)
		goto err1;
	entry = proc_create("lroflow", 0, g_toe_dir, &ltq_lro_proc_fops);
	if (!entry)
		goto err1;
#ifdef LRO_DEBUG
	entry = proc_create("lrodbg", 0, g_toe_dir, &ltq_lrodbg_proc_fops);
	if (!entry)
		goto err1;

	entry = proc_create("lro_dbg_info", 0, g_toe_dir, &ltq_lro_debug_info_proc_fops);
	if (!entry)
		goto err1;
#endif

	entry = proc_create("tso_xmit_mode", 0, g_toe_dir, &tso_xmit_mode_proc_fops);
	if (!entry)
		goto err1;

	return 0;
err1:
	remove_proc_entry("driver/ltq_toe", NULL);
	return -ENOMEM;
}

static int ltq_toe_init(struct platform_device *pdev)
{
	struct resource *r;
	struct resource irqres[15];
	uint8_t cpu_mask[NR_CPUS], cpu_available = 0;
	ltq_tso_port_t *tsoPort;
	struct cpumask cpumask;
	struct device_node *node = pdev->dev.of_node;
	int ret, ret_val, i, cpu, sgNo;

	/* Get the TOE base address */
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		dev_err(&pdev->dev, "platform_get_resource\n");
		ret_val = -ENOENT;
		return ret_val;
	}

	r = devm_request_mem_region(&pdev->dev, r->start, resource_size(r),
			pdev->name);
	if (!r) {
		dev_err(&pdev->dev, "failed to request memory region\n");
		ret_val = -ENXIO;
		return ret_val;
	}

	pr_debug ("ltq_toe_membase from DT: %x \n", (unsigned int) r->start);
	ltq_toe_membase = devm_ioremap_nocache(&pdev->dev,
		r->start, resource_size(r));

	if (!ltq_toe_membase) {
		dev_err(&pdev->dev, "failed to remap memory region\n");
		ret_val = -ENXIO;
		return ret_val;
	}

	/* Get the LRO output context base address */
	r = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (r == NULL) {
		dev_err(&pdev->dev, "platform_get_resource failed for LRO\n");
		ret_val = -ENOENT;
		return ret_val;
	}

	r = devm_request_mem_region(&pdev->dev, r->start, resource_size(r),
			pdev->name);
	if (!r) {
		dev_err(&pdev->dev, "failed to request LRO memory region\n");
		ret_val = -ENXIO;
		return ret_val;
	}

	pr_info ("lro_sram_membase_res0 from DT: %x \n", (unsigned int) r->start);
	lro_sram_membase_res0 = devm_ioremap_nocache(&pdev->dev,
		r->start, resource_size(r));

	if (!lro_sram_membase_res0) {
		dev_err(&pdev->dev, "failed to remap memory region for LRO\n");
		ret_val = -ENXIO;
		return ret_val;
	}

	/* Get the interrupt numbers */
	ret_val = of_irq_to_resource_table(node, irqres, 15);
	if (ret_val != 15) {
		dev_err(&pdev->dev,
			"failed to get irq for toe port\n");
		return -ENODEV;
	}

	pr_info("ltq_toe_membase: %x and lro_sram_membase_res0: %x\n",
			(unsigned int)ltq_toe_membase,
			(unsigned int)lro_sram_membase_res0);

	/* Initialise the 4 ports */
	for_each_online_cpu(cpu) {
		/* cpu mask is for LRO irq load distribution among
		all CPUs, this part needs to be handles seperately
		if any modifications done later on TSO front, here */
		cpu_mask[cpu_available++] = 1 << cpu;

		tsoPort = (ltq_tso_port + cpu);
		tsoPort->membase = ltq_toe_membase + (cpu*0x20);
		tsoPort->port_number = cpu;
		atomic_set( &(tsoPort->availBuffs),SG_BUFFER_PER_PORT);
		tsoPort->rPos = tsoPort->wPos = 0;
		for(sgNo = 0; sgNo < SG_BUFFER_PER_PORT; ++sgNo) {
			tsoPort->sgBuffs[sgNo].sgBuffer = ltq_large_buf[(cpu*SG_BUFFER_PER_PORT)+sgNo];
			tsoPort->sgBuffs[sgNo].skb =  NULL;
		}
		tsoPort->irq = irqres[cpu+1].start;
		skb_queue_head_init(&tsoPort->processQ);

		/* Register the interrupt handler for TSO */
		ret_val = request_irq(tsoPort->irq, ltq_tso_tx_int, 0, "tso_irq", tsoPort);
		if (ret_val) {
			pr_err("failed to request tso_tx_irq \n");
			return ret_val;
		}

		/* Set the affinity */
		cpumask.bits[0] = (1 << cpu);
		if (irq_set_affinity(tsoPort->irq, &cpumask))
			pr_err("can not set affinity for IRQ - %d", tsoPort->irq);

		tasklet_init(&tsoPort->tasklet, ltq_tso_tasklet, (unsigned long)(tsoPort));
		if (is_tso_PollingMode())
			disable_irq(tsoPort->irq);
	}

	/* Register the interrupt handlers for the LRO */
	for (i = 0; i < LTQ_MAX_LRO_PORTS; ++i) {
		ltq_lro_port[i].port_num = i;
		ltq_lro_port[i].irq_num = irqres[i + 7].start;
		ltq_lro_port[i].affinity = cpu_mask[i % cpu_available];
		ret_val = request_irq(irqres[i + 7].start, lro_port_context_isr,
					0, "lro_irq", &ltq_lro_port[i]);
		if (ret_val) {
			pr_err("failed to request lro_irq\n");
			return ret_val;
		}
		tasklet_init(&lro_tasklet[i],
			ltq_lro_tasklet, (unsigned long) &ltq_lro_port[i]);
#ifdef USE_TIMER_FOR_SESSION_STOP
		init_timer(&ltq_lro_port[i].lro_timer);
		lro_time = msecs_to_jiffies(5000);
		ltq_lro_port[i].lro_timer.function = lro_timer_fn;
		ltq_lro_port[i].lro_timer.expires = jiffies + lro_time;
		ltq_lro_port[i].lro_timer.data = (unsigned int)&ltq_lro_port[i];
#endif
	}

	ret_val = request_irq(irqres[6].start, lro_port_except_isr,
					0, "lro_except_irq", NULL);

	cpumask.bits[0] = 0x1;
	ret = irq_set_affinity(irqres[6].start, &cpumask);
	if (ret_val || ret) {
		pr_err("failed to request lro exception irq with ret_val = %d\n", ret_val);
		return ret_val;
	}
	tasklet_init(&lro_exception_tasklet, ltq_lro_exception_tasklet, 1);
	ret_val = request_irq(irqres[5].start, lro_port_overflow_isr,
						0, "lro_ovflow_irq", NULL);
	ret = irq_set_affinity(irqres[5].start, &cpumask);
	if (ret_val || ret) {
		pr_err("failed to request lro overflow irq \n");
		return ret_val;
	}
	tasklet_init(&lro_ovflow_tasklet, ltq_lro_ovflow_tasklet, 0);

	/* Set up the DMA3 for ToE */
	tso_configure_dma();

	/* Enable the ToE at top level */
	ltq_toe_w32_mask(0, (CONST_TSO_GCTRL_ENABLE_EN << TSO_GCTRL_ENABLE_POS), TSO_GCTRL);

	/* Mask all the interrupts */
	ltq_toe_w32(0xffffffff, TOE_INT_MASK);

	/* Configure and enable the TSO */
	configure_tso();

	/* Configure and enable the LRO */
	configure_lro();

	/* Initialise the proc filesystem */
	ltq_toe_proc_init();

	spin_lock_init(&toe_register_lock);
	spin_lock_init(&lro_register_lock);

	g_toe_dev = &pdev->dev;

	pr_info("TOE Init Done !!\n");
	return 0;
}

static int ltq_toe_exit(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id ltq_toe_match[] = {
	{ .compatible = "lantiq,toe-xrx500" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_toe_match);

static struct platform_driver ltq_toe_driver = {
	.probe = ltq_toe_init,
	.remove = ltq_toe_exit,
	.driver = {
		.name = "toe-xrx500",
		.owner = THIS_MODULE,
		.of_match_table = ltq_toe_match,
	},
};

module_platform_driver(ltq_toe_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Suresh Nagaraj, suresh.nagaraj@lantiq.com");
MODULE_DESCRIPTION("Lantiq TOE driver");
MODULE_SUPPORTED_DEVICE("Lantiq TOE IP module");
