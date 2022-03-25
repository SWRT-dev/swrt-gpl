#include <linux/klogging.h>
#include "cbm_proc.h"
#include "cbm.h"
#include <net/datapath_proc_api.h>
#include <net/drv_tmu_ll.h>

#define REG32(addr)     (*(volatile unsigned int *) (addr))

static struct dp_proc_entry cbm_proc_entries[] = {
/*name single_callback_t multi_callback_t/_start write_callback_t */
	{PROC_FILE_CBM_ENQ, cbm_enqueue_dump, NULL, NULL,
	 NULL},
	{PROC_FILE_CBM_DEQ, cbm_dequeue_dump, NULL, NULL,
	 NULL},
	{PROC_FILE_CBM_LS, cbm_ls_dump, NULL, NULL,
	 NULL},
	{PROC_FILE_CBM_SBA_JBA, cbm_sba_jba_dump, NULL, NULL,
	 NULL},
	{PROC_FILE_CBM_STAT_REG, cbm_status_reg_dump, NULL, NULL,
	 NULL},
	{PROC_FILE_CBM_DES64_INGRESS_REG, cbm_des64_ingress_dump, NULL,
	 NULL, NULL},
	{PROC_FILE_CBM_DES64_EGRESS_REG, cbm_des64_egress_dump, NULL,
	 NULL, NULL},
	/*{PROC_FILE_RCNT_REG, NULL, NULL, NULL,
	 fsqm_rnt_read},*/
	{PROC_FILE_FSC_FSQT_REG, NULL, NULL, NULL,
	 fsqm_freesegment_read},
	{PROC_FILE_CBM_ENQ_OVH_BYTES, cbm_enq_ovh_bytes_read, NULL, NULL,
	 cbm_enq_ovh_bytes_write},
	{PROC_FILE_CBM_COUNTER_MODE, cbm_counter_mode_get_proc, NULL, NULL,
	 cbm_counter_mode_set_proc},
	{PROC_FILE_CBM_Q_THRES, cbm_q_thres_get_proc, NULL, NULL,
	 cbm_q_thres_set_proc},
	{PROC_FILE_CBM_Q_DELAY, NULL, NULL, NULL,
	 cbm_eqm_delay_set_proc},
	 #ifdef QOCC_TEST
	{PROC_FILE_CBM_QOCC_TEST, NULL, NULL, NULL, cbm_qocc_test_steps},
	#endif
	{PROC_FILE_CBM_BUF_STAT, cbm_buff_stat_dump, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL}
};

/**
   Initialize and install the proc entry

   \return
   -1 or 0 on success

   \remarks
   Called by the kernel.
*/
int cbm_proc_install(struct dentry *parent)
{
	struct dentry *driver_proc_node;
	driver_proc_node = debugfs_create_dir(CBM_PROC_PARENT CBM_PROC_NAME, parent);
	if (driver_proc_node != NULL) {
		int i;
		for (i = 0; i < ARRAY_SIZE(cbm_proc_entries); i++)
			dp_proc_entry_create(driver_proc_node,
					     &cbm_proc_entries[i]);
	} else {
		LOGF_KLOG_ERROR("cannot create proc entry");
		return -1;
	}
	return 0;
}
EXPORT_SYMBOL(cbm_proc_install);

/*! Register Offset (relative)*/
#define CBM_STAT 0x210
/*! Register Offset (absolute) for 1st Instance CBM*/
#define CBM_CBM_STAT 0x1E710210u

/*! Register Reset Value*/
#define CBM_STAT_RST 0x00000000u

/*! Field ACT - CBM Activity Status*/
#define CBM_STAT_ACT_POS 0
/*! Field ACT - CBM Activity Status*/
#define CBM_STAT_ACT_MASK 0x1u
/*! Constant INACTIVE - Inactive*/
#define CONST_CBM_STAT_ACT_INACTIVE 0x0
/*! Constant ACTIVE - Active*/
#define CONST_CBM_STAT_ACT_ACTIVE 0x1

/*! Field LSBYP - Hardware SMS lightspeed mode*/
#define CBM_STAT_LSBYP_POS 16
/*! Field LSBYP - Hardware SMS lightspeed mode*/
#define CBM_STAT_LSBYP_MASK 0x10000u
/*! Constant LS - Lightspeed mode enable*/
#define CONST_CBM_STAT_LSBYP_LS 0x0
/*! Constant BYP - Lightspeed mode bypass*/
#define CONST_CBM_STAT_LSBYP_BYP 0x1

/*! Field JSEL - Jumbo Select*/
#define CBM_STAT_JSEL_POS 17
/*! Field JSEL - Jumbo Select*/
#define CBM_STAT_JSEL_MASK 0x20000u
/*! Constant JSEL0 - Jumbo frame size 8 KBytes*/
#define CONST_CBM_STAT_JSEL_JSEL0 0x0
/*! Constant JSEL1 - Jumbo frame size 16 KBytes*/
#define CONST_CBM_STAT_JSEL_JSEL1 0x1

#define REG64(addr)      (*((volatile uint64_t *)(addr)))

void print_reg64(char *name, uint32_t addr)
{
	uint32_t reg1, reg2;
	uint64_t reg;

	reg = REG64(addr);
	reg1 = (uint32_t) (reg >> 32);
	reg2 = (reg & 0xFFFFFFFF);

	LOGF_KLOG_CONT("%s: addr:0x%8x, val[32-63]: 0x%8x, val[0-31]: 0x%8x\n",
		  name, addr, reg1, reg2);
}

static uint32_t cbm_eqm_addr_base_d = KSEG1ADDR(CBM_EQM_MODULE_BASE);
#define CBM_ENQ_BASE_D				cbm_eqm_addr_base_d

void cbm_enqueue_dump(struct seq_file *s)
{
	if (!capable(CAP_SYS_ADMIN))
		return;

	seq_printf(s, "CBM_ENQ_BASEe=0x%x\n", CBM_ENQ_BASE_D);

	seq_printf(s, "%8s,%8s,%8s,%8s,%8s,%8s\n", "portno :", "pocc", "eqpc",
	"disc_desc1", "disc_desc2", "disc_desc3");
	seq_printf(s, "cpu_p0: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_CPU_IGP_0),
	REG32(CBM_ENQ_BASE_D + EQPC_CPU_IGP_0),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_CPU_IGP_0),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_CPU_IGP_0),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_CPU_IGP_0));

	seq_printf(s, "cpu_p1: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_CPU_IGP_1),
	REG32(CBM_ENQ_BASE_D + EQPC_CPU_IGP_1),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_CPU_IGP_1),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_CPU_IGP_1),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_CPU_IGP_1));

	seq_printf(s, "cpu_p2: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_CPU_IGP_2),
	REG32(CBM_ENQ_BASE_D + EQPC_CPU_IGP_2),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_CPU_IGP_2),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_CPU_IGP_2),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_CPU_IGP_2));

	seq_printf(s, "cpu_p3: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_CPU_IGP_3),
	REG32(CBM_ENQ_BASE_D + EQPC_CPU_IGP_3),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_CPU_IGP_3),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_CPU_IGP_3),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_CPU_IGP_3));

	seq_printf(s, "cpu_p4: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_CPU_IGP_4),
	REG32(CBM_ENQ_BASE_D + EQPC_CPU_IGP_4),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_CPU_IGP_4),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_CPU_IGP_4),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_CPU_IGP_4));

	seq_printf(s, "dma_p5: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_5),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_5),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_5),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_5),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_5));

	seq_printf(s, "dma_p6: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_6),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_6),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_6),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_6),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_6));

	seq_printf(s, "dma_p7: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_7),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_7),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_7),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_7),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_7));

	seq_printf(s, "dma_p8: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_8),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_8),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_8),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_8),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_8));

	seq_printf(s, "dma_p9: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_9),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_9),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_9),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_9),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_9));

	seq_printf(s, "dma_p10: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_10),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_10),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_10),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_10),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_10));

	seq_printf(s, "dma_p11: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_11),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_11),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_11),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_11),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_11));

	seq_printf(s, "dma_p12: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_12),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_12),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_12),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_12),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_12));

	seq_printf(s, "dma_p13: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_13),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_13),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_13),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_13),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_13));

	seq_printf(s, "dma_p14: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_14),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_14),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_14),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_14),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_14));

	seq_printf(s, "dma_p15: 0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
	REG32(CBM_ENQ_BASE_D + POCC_DMA_IGP_15),
	REG32(CBM_ENQ_BASE_D + EQPC_DMA_IGP_15),
	REG32(CBM_ENQ_BASE_D + DISC_DESC1_DMA_IGP_15),
	REG32(CBM_ENQ_BASE_D + DISC_DESC2_DMA_IGP_15),
	REG32(CBM_ENQ_BASE_D + DISC_DESC3_DMA_IGP_15));
}

static uint32_t cbm_deqm_addr_base_d = KSEG1ADDR(CBM_DQM_MODULE_BASE);
#define CBM_DEQ_BASE_D				cbm_deqm_addr_base_d

void cbm_dequeue_dump(struct seq_file *s)
{
	if (!capable(CAP_SYS_ADMIN))
		return;

	seq_printf(s, "CBM_DEQ_BASEe=0x%x\n", CBM_DEQ_BASE_D);

	seq_printf(s, "%8s\n", "cpu_p0_dqpc");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_CPU_EGP_0));

	seq_printf(s, "%8s\n", "cpu_p1_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_CPU_EGP_1));

	seq_printf(s, "%8s\n", "cpu_p2_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_CPU_EGP_2));

	seq_printf(s, "%8s\n", "cpu_p3_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_CPU_EGP_3));

	seq_printf(s, "%8s\n", "cpu_p4_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_CPU_EGP_4));

	seq_printf(s, "%8s\n", "cpu_p5_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_CPU_EGP_5));

	seq_printf(s, "%8s\n", "dma_p6_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_6));

	seq_printf(s, "%8s\n", "dma_p7_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_7));

	seq_printf(s, "%8s\n", "dma_p8_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_8));

	seq_printf(s, "%8s\n", "dma_p9_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_9));

	seq_printf(s, "%8s\n", "dma_p10_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_10));

	seq_printf(s, "%8s\n", "dma_p11_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_11));

	seq_printf(s, "%8s\n", "dma_p12_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_12));

	seq_printf(s, "%8s\n", "dma_p13_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_13));

	seq_printf(s, "%8s\n", "dma_p14_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_14));

	seq_printf(s, "%8s\n", "dma_p15_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_15));

	seq_printf(s, "%8s\n", "dma_p16_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_16));

	seq_printf(s, "%8s\n", "dma_p17_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_17));

	seq_printf(s, "%8s\n", "dma_p18_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_18));

	seq_printf(s, "%8s\n", "dma_p19_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_19));

	seq_printf(s, "%8s\n", "dma_p20_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_20));

	seq_printf(s, "%8s\n", "dma_p21_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_21));

	seq_printf(s, "%8s\n", "dma_p22_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_22));

	seq_printf(s, "%8s\n", "dma_p23_DQPC");
	seq_printf(s, "0x%08x\n", REG32(CBM_DEQ_BASE_D + DQPC_DMA_EGP_23));

}

static uint32_t cbm_ls_status_port0_base_d =
KSEG1ADDR(CBM_LS_LS_STATUS_PORT0);
static uint32_t cbm_ls_status_port1_base_d =
KSEG1ADDR(CBM_LS_LS_STATUS_PORT1);
static uint32_t cbm_ls_status_port2_base_d =
KSEG1ADDR(CBM_LS_LS_STATUS_PORT2);
static uint32_t cbm_ls_status_port3_base_d =
KSEG1ADDR(CBM_LS_LS_STATUS_PORT3);

#define CBM_LS_P0_D					cbm_ls_status_port0_base_d
#define CBM_LS_P1_D					cbm_ls_status_port1_base_d
#define CBM_LS_P2_D					cbm_ls_status_port2_base_d
#define CBM_LS_P3_D					cbm_ls_status_port3_base_d

void cbm_ls_dump(struct seq_file *s)
{
	unsigned int reg_r_data, q_len0 = 0, q_full0 = 0, q_empty0 =
	0, q_cnt0 = 0, q_len1 = 0, q_full1 = 0, q_empty1 = 0, q_cnt1 = 0;
	unsigned int q_len2 = 0, q_full2 = 0, q_empty2 = 0, q_cnt2 =
	0, q_len3 = 0, q_full3 = 0, q_empty3 = 0, q_cnt3 = 0;

	if (!capable(CAP_SYS_ADMIN))
		return;

	seq_printf(s, "%8s,%8s,%8s,%8s,%8s\n", "portno:", "qlen", "qfull",
	"qempty", "cntval");
	reg_r_data = 0;
	reg_r_data = REG32(CBM_LS_P0_D);
	q_len0 =
	get_val(reg_r_data, LS_STATUS_PORT0_QUEUE_LEN_MASK,
	LS_STATUS_PORT0_QUEUE_LEN_POS);
	q_full0 =
	get_val(reg_r_data, LS_STATUS_PORT0_QUEUE_FULL_MASK,
	LS_STATUS_PORT0_QUEUE_FULL_POS);
	q_empty0 =
	get_val(reg_r_data, LS_STATUS_PORT0_QUEUE_EMPTY_MASK,
	LS_STATUS_PORT0_QUEUE_EMPTY_POS);
	q_cnt0 =
	get_val(reg_r_data, LS_STATUS_PORT0_CNT_VAL_MASK,
	LS_STATUS_PORT0_CNT_VAL_POS);
	seq_printf(s, "port0:%08d,%08d,%08d,0x%08x\n", q_len0, q_full0,
	q_empty0, q_cnt0);

	reg_r_data = 0;
	reg_r_data = REG32(CBM_LS_P1_D);
	q_len1 =
	get_val(reg_r_data, LS_STATUS_PORT1_QUEUE_LEN_MASK,
	LS_STATUS_PORT1_QUEUE_LEN_POS);
	q_full1 =
	get_val(reg_r_data, LS_STATUS_PORT1_QUEUE_FULL_MASK,
	LS_STATUS_PORT1_QUEUE_FULL_POS);
	q_empty1 =
	get_val(reg_r_data, LS_STATUS_PORT1_QUEUE_EMPTY_MASK,
	LS_STATUS_PORT1_QUEUE_EMPTY_POS);
	q_cnt1 =
	get_val(reg_r_data, LS_STATUS_PORT1_CNT_VAL_MASK,
	LS_STATUS_PORT1_CNT_VAL_POS);
	seq_printf(s, "port1:%08d,%08d,%08d,0x%08x\n", q_len1, q_full1,
	q_empty1, q_cnt1);

	reg_r_data = 0;
	reg_r_data = REG32(CBM_LS_P2_D);
	q_len2 =
	get_val(reg_r_data, LS_STATUS_PORT2_QUEUE_LEN_MASK,
	LS_STATUS_PORT2_QUEUE_LEN_POS);
	q_full2 =
	get_val(reg_r_data, LS_STATUS_PORT2_QUEUE_FULL_MASK,
	LS_STATUS_PORT2_QUEUE_FULL_POS);
	q_empty2 =
	get_val(reg_r_data, LS_STATUS_PORT2_QUEUE_EMPTY_MASK,
	LS_STATUS_PORT2_QUEUE_EMPTY_POS);
	q_cnt2 =
	get_val(reg_r_data, LS_STATUS_PORT2_CNT_VAL_MASK,
	LS_STATUS_PORT2_CNT_VAL_POS);
	seq_printf(s, "port2:%08d,%08d,%08d,0x%08x\n", q_len2, q_full2,
	q_empty2, q_cnt2);

	reg_r_data = 0;
	reg_r_data = REG32(CBM_LS_P3_D);
	q_len3 =
	get_val(reg_r_data, LS_STATUS_PORT3_QUEUE_LEN_MASK,
	LS_STATUS_PORT3_QUEUE_LEN_POS);
	q_full3 =
	get_val(reg_r_data, LS_STATUS_PORT3_QUEUE_FULL_MASK,
	LS_STATUS_PORT3_QUEUE_FULL_POS);
	q_empty3 =
	get_val(reg_r_data, LS_STATUS_PORT3_QUEUE_EMPTY_MASK,
	LS_STATUS_PORT3_QUEUE_EMPTY_POS);
	q_cnt3 =
	get_val(reg_r_data, LS_STATUS_PORT3_CNT_VAL_MASK,
	LS_STATUS_PORT3_CNT_VAL_POS);
	seq_printf(s, "port3:%08d,%08d,%08d,0x%08x\n", q_len3, q_full3,
	q_empty3, q_cnt3);
}

static uint32_t cbm_sba0_base_d = KSEG1ADDR(CBM_CBM_SBA_0);
static uint32_t cbm_sba1_base_d = KSEG1ADDR(CBM_CBM_SBA_1);
static uint32_t cbm_jba0_base_d = KSEG1ADDR(CBM_CBM_JBA_0);
static uint32_t cbm_jba1_base_d = KSEG1ADDR(CBM_CBM_JBA_1);

#define CBM_SBA0_D			cbm_sba0_base_d
#define CBM_SBA1_D			cbm_sba1_base_d
#define CBM_JBA0_D			cbm_jba0_base_d
#define CBM_JBA1_D			cbm_jba1_base_d

void cbm_sba_jba_dump(struct seq_file *s)
{
	unsigned int sba0 = 0, sba1 = 0, jba0 = 0, jba1 = 0;

	if (!capable(CAP_SYS_ADMIN))
		return;

	sba0 = REG32(CBM_SBA0_D);
	sba1 = REG32(CBM_SBA1_D);
	jba0 = REG32(CBM_JBA0_D);
	jba1 = REG32(CBM_JBA1_D);

	seq_printf(s, "%8s,%8s,%8s,%8s\n", "sba0", "sba1", "jba0", "jba1");
	seq_printf(s, "0x%08x,0x%08x,0x%08x,0x%08x\n", sba0, sba1, jba0,
		jba1);
}

static uint32_t cbm_status_base_d = KSEG1ADDR(CBM_CBM_STAT);
#define CBM_STAT_D				cbm_status_base_d

void cbm_status_reg_dump(struct seq_file *s)
{
	unsigned int reg_r_data, act_stat = 0, lsbyp = 0, jsel = 0;

	if (!capable(CAP_SYS_ADMIN))
		return;

	reg_r_data = 0;
	reg_r_data = REG32(CBM_STAT_D);
	act_stat = get_val(reg_r_data, CBM_STAT_ACT_MASK, CBM_STAT_ACT_POS);
	lsbyp = get_val(reg_r_data, CBM_STAT_LSBYP_MASK, CBM_STAT_LSBYP_POS);
	jsel = get_val(reg_r_data, CBM_STAT_JSEL_MASK, CBM_STAT_JSEL_POS);
	seq_printf(s, "%8s,%8s,%8s\n", "activity_status", "LSBYP", "JSEL");
	seq_printf(s, "0x%08x,0x%08x,0x%08x\n", act_stat, lsbyp, jsel);
}
#if 0
static unsigned int cbm_std_frame_size = 0x800;	/*  2k    */
static unsigned int cbm_jbo_frame_size_8k = 0x2000;	/*  16k  */
static unsigned int cbm_jbo_frame_size_16k = 0x4000;	/*  16k  */
#endif
static uint32_t fsqm0_base_d = KSEG1ADDR(FSQM0_MODULE_BASE);
static uint32_t fsqm1_base_d = KSEG1ADDR(FSQM1_MODULE_BASE);
#define FSQM0_BASE_D			fsqm0_base_d
#define FSQM1_BASE_D			fsqm1_base_d
#define FSQM_BASE_D(idx)		(idx == 0 ? fsqm0_base_d : fsqm1_base_d)

ssize_t cbm_enq_ovh_bytes_write(struct file *file, const char *buf,
			size_t count, loff_t *ppos)
{
	uint32_t port;
	int8_t ovh_bytes;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[5] = { 0 };
	int num = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));

	if (num == 2) {
		port = dp_atoi(param_list[0]);
		ovh_bytes = dp_atoi(param_list[1]);
		if ((port <= 15)
		&& ((ovh_bytes <= 127) && (ovh_bytes >= -127)))
			cbm_enqueue_port_overhead_set(port, ovh_bytes);
	} else if (num == 1) {
		if (dp_strncmpi(param_list[0], "help", strlen("help")) == 0)
			goto help;
		port = dp_atoi(param_list[0]);
		if (port <= 15) {
			cbm_enqueue_port_overhead_get(port, &ovh_bytes);
			LOGF_KLOG_CONT("%d\n", ovh_bytes);
		}
	} else
		goto help;
	return count;
 help:
	LOGF_KLOG_CONT("Usage: \n");
	LOGF_KLOG_CONT("To Set: echo port ovh > cbmenqovh\n");
	LOGF_KLOG_CONT("To Get: echo port > cbmenqovh\n");
	return count;
}

void  cbm_enq_ovh_bytes_read(struct seq_file *s)
{
	int i;
	int8_t ovh_bytes;

	if (!capable(CAP_SYS_ADMIN))
		return;

	for (i = 0; i <= 15; i++) {
		cbm_enqueue_port_overhead_get(i, &ovh_bytes);
		LOGF_KLOG_CONT("%d \t", ovh_bytes);
	}
	LOGF_KLOG_CONT("\n");
}

void  cbm_q_thres_get_proc(struct seq_file *s)
{
	uint32_t length;

	if (!capable(CAP_SYS_ADMIN))
		return;

	cbm_q_thres_get(&length);
	LOGF_KLOG_CONT("EQM QUEUE THRESHOLD %d\n", length);
}
ssize_t cbm_q_thres_set_proc(struct file *file, const char *buf,
			size_t count, loff_t *ppos)
{
	uint8_t length;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[1] = { 0 };
	int num = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;
	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));
	if (num == 1) {
		if (dp_strncmpi(param_list[0], "help", strlen("help")) == 0)
			goto help;
		length = dp_atoi(param_list[0]);
		if (length <= 7) {
			cbm_q_thres_set(length);
		} else {
			LOGF_KLOG_CONT("invalid Queue Length\n");
		}
	} else
		goto help;

	return count;
 help:
	LOGF_KLOG_CONT("Usage: \n");
	LOGF_KLOG_CONT
		("Set Queue Threshold:  echo queue-length > cbmqthres\n");
	return count;
}
ssize_t cbm_eqm_delay_set_proc(struct file *file, const char *buf,
			size_t count, loff_t *ppos)
{
	int delay, port_id;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[3] = { 0 };
	int num = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;
	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));
	if (num == 3) {
		port_id = dp_atoi(param_list[1]);
		delay = dp_atoi(param_list[2]);
		if (delay <= 0)
			goto help;
		if (dp_strncmpi(param_list[0], "set", strlen("set")) == 0)
			cbm_igp_delay_set(port_id, delay);
		else
			goto help;
	} else if (num == 2) {
		port_id = dp_atoi(param_list[1]);
		if (dp_strncmpi(param_list[0], "get", strlen("get")) == 0) {
			cbm_igp_delay_get(port_id, &delay);
			LOGF_KLOG_CONT("Delay %d\n", delay);
		} else
			goto help;
	} else
		goto help;

	return count;

 help:
   LOGF_KLOG_CONT("Usage: \n");
   LOGF_KLOG_CONT
	   ("Set Delay:  echo set port-id  delay > cbmqdelay\n");
   LOGF_KLOG_CONT
	   ("Get Delay:  echo get port-id > cbmqdelay\n");

	return count;
}
void  cbm_counter_mode_get_proc(struct seq_file *s)
{
	int mode;

	if (!capable(CAP_SYS_ADMIN))
		return;

	cbm_counter_mode_get(0, &mode);
	LOGF_KLOG_CONT("EQM MSEL %s\n", (mode > 0) ? "BYTE" : "PKT");
	cbm_counter_mode_get(1, &mode);
	LOGF_KLOG_CONT("DQM MSEL %s\n", (mode > 0) ? "BYTE" : "PKT");
}

ssize_t cbm_counter_mode_set_proc(struct file *file, const char *buf,
			size_t count, loff_t *ppos)
{
	uint8_t eqm;
	uint8_t mode;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[2] = { 0 };
	int num = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));

	if (num == 2) {
		eqm = dp_atoi(param_list[0]);
		mode = dp_atoi(param_list[1]);
		if ((eqm <= 1)
		&& (mode <= 1))
			cbm_counter_mode_set(eqm, mode);
	} else {
		LOGF_KLOG_CONT("Usage: \n");
		LOGF_KLOG_CONT
		    ("echo Value[0->EQM 1->DQM] Mode[0->PKT 1->BYTE]> cbmcntrmode\n");
			LOGF_KLOG_CONT("For Ex: echo 0 1 > cbmcntrmode, for EQM BYTE Mode\n");
		LOGF_KLOG_CONT("cat cbmcntrmode\n");
		return count;
	}
	return count;
}
#ifdef QOCC_TEST
void tmu_set_wfq(const uint32_t sbin, const uint16_t weight)
{
	struct tmu_sched_blk_in_link ilink;
	tmu_sched_blk_in_link_get(sbin, &ilink);
	ilink.iwgt = weight;
	tmu_sched_blk_in_link_set(sbin, &ilink);
}

int tmu_shaper_cfg_set(uint32_t index, int32_t enable, uint32_t mode,
		       uint32_t cir, uint32_t pir, uint32_t cbs, uint32_t pbs)
{
	struct tmu_token_bucket_shaper_params tbs;
	uint32_t tmp_pir = 0, tmp_cir = 0, tmp_pbs = 0;
	uint32_t tmp_cbs = 0;
	/*TMU_DEBUG_ASSERT(index >= EGRESS_QUEUE_ID_MAX);*/
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
	/*TMU_PRINT("tmp_pir=%u. TMU_TBS_PIR_MAX=%u\n", (unsigned int)tmp_pir,
		(unsigned int)TMU_TBS_PIR_MAX);
	TMU_DEBUG_ASSERT(tmp_pir > TMU_TBS_PIR_MAX);
	TMU_DEBUG_ASSERT(tmp_pbs > TMU_TBS_PBS_MAX);
	TMU_DEBUG_ASSERT(tmp_cir > TMU_TBS_CIR_MAX);
	TMU_DEBUG_ASSERT(tmp_cbs > TMU_TBS_CBS_MAX);*/
	tbs.tbe0 = enable;
	tbs.tbe1 = enable;
	tbs.mod = mode;
	tbs.pir = tmp_pir;
	tbs.pbs = tmp_pbs;
	tbs.cir = tmp_cir;
	tbs.cbs = tmp_cbs;
	/*TMU_DEBUG(TMU_DEBUG_MSG,
		"pir=%d, pbs=%d, cir=%d, cbs=%d for tbid %u\n", tbs.pir,
		tbs.pbs, tbs.cir, tbs.cbs, index);*/
	tmu_token_bucket_shaper_cfg_set(index, &tbs);
	tmu_token_bucket_shaper_cfg_get(index, &tbs);
	/*TMU_DEBUG(TMU_DEBUG_MSG, "read  tbs:%u_%u_%u_%u for tbid %u\n",
		tbs.pir, tbs.pbs, tbs.cir, tbs.cbs, index);*/
	return true;
}
void create_queue(uint32_t qid, uint32_t egress_port, int *sbit, int *leaf, uint32_t level)
{
	unsigned int LINK_A = 0, LINK_B = 0, LEAF;
	unsigned int num = 0, i;
	num = level + 3;

	for (i = 2; ((num - 1) > i && i < 10); i++) {
		LINK_B = sbit[i - 2];
		LEAF = leaf[i - 2];
		if (i == 2) {
			LINK_A = qid;
			/*pr_info("QID %d->SB %d:LEAF %d->", LINK_A,
			LINK_B, LEAF);*/
			tmu_egress_queue_create(LINK_A,
			((LINK_B << 3) +
			LEAF), egress_port);
			tmu_set_wfq(((LINK_B << 3) + LEAF), 1);
			level--;
		} else if (i != 2) {
			/*pr_info("SB %d:LEAF %d->", LINK_B, LEAF);*/
			tmu_sched_blk_create(LINK_A, level,
			((LINK_B << 3) + LEAF),
			1, 0);
			level--;
		}
		LINK_A = LINK_B;
	}
	/*pr_info("EP %d\n", egress_port);*/
	tmu_sched_blk_create(LINK_A, level, egress_port, 0, 0);
	tmu_egress_port_enable(egress_port, true);
}
ssize_t cbm_qocc_test_steps(struct file *file, const char *buf,
			size_t count, loff_t *ppos)
{
	uint8_t step;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[2] = { 0 };
	int num = 0;
    int sbit[8];
    int leaf[8];
	int j, i, k;
	uint32_t queues[8] = {28, 100, 101, 102, 103, 104, 105, 106};
	uint32_t tbids[8] = {200, 201, 202, 203, 204, 205, 206, 207};
	uint32_t class[8] = {1, 0, 2, 3, 4, 5, 6, 7};
	unsigned int LINK_B = 0, LEAF;
	unsigned int tb = 0;
	cbm_queue_map_entry_t entry;
	cbm_queue_map_entry_t *temp_entry;
	int32_t num_entries = 0;
	static uint32_t class_deleted[8];
	static int class_count;
	uint32_t l, sbin;
	struct tmu_sched_blk_in_link ilink = {0};
	struct tmu_sched_blk_out_link olink = {0};
	uint32_t tbid, cir, pir, cbs, pbs, mode;
	uint32_t temp_qmap;
	uint32_t qmap_flags = CBM_QUEUE_MAP_F_MPE2_DONTCARE
		| CBM_QUEUE_MAP_F_MPE1_DONTCARE
		| CBM_QUEUE_MAP_F_DE_DONTCARE
		| CBM_QUEUE_MAP_F_EN_DONTCARE
		| CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE
		| CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	temp_qmap = qmap_flags;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));

	if (num == 1) {
		step = dp_atoi(param_list[0]);
		switch (step) {
		case 0:
			for (j = 0; j <= step; j++) {
				/*pr_info("delete %d\n", queues[j]);*/
				cbm_dp_q_enable (0, 15, queues[j], 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
				tmu_egress_queue_delete(queues[j]);
			}
			LEAF = step + 1;
			for (j = 0; j <= step+1; j++) {
				sbit[0] = 12; leaf[0] = LEAF; sbit[1] = 100; leaf[1] = 0;
				create_queue (queues[j], 19, sbit, leaf, 2);
				tbid = tbids[j];	cir = 1250000; pir = 1250000; cbs = 4000; pbs = 4000; mode = 0;
				tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
				tmu_token_bucket_shaper_create(tbids[j], ((sbit[0] << 3) + LEAF));
				LEAF--;
				entry.tc = class[j]; entry.ep = 15;
				/*pr_info("map set class %d queue %d\n", entry.tc, queues[j]);*/
				cbm_queue_map_set(0, queues[j], &entry, qmap_flags);
			}
			memset(&entry, 0, sizeof(cbm_queue_map_entry_t));
			entry.ep = 15;
			qmap_flags |= CBM_QUEUE_MAP_F_TC_DONTCARE;
			cbm_queue_map_set(0, queues[1], &entry, qmap_flags);
			/*echo cfg_tb 100 1250000 1250000 4000 4000 0 >/proc/tmu/cascade*/
			tbid = 100;	cir = 1250000; pir = 1250000; cbs = 4000; pbs = 4000; mode = 0;
			tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
			/*echo attach_tb 100 100:0 > /proc/tmu/cascade*/
			LINK_B = 100; LEAF = 0; tb = 100;
			tmu_token_bucket_shaper_create(tb, ((LINK_B << 3) + LEAF));
		break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			LEAF = step + 1;
			for (j = 0; j <= step; j++) {
				qmap_flags = temp_qmap;
				/*pr_info("delete %d\n", queues[j]);*/
				cbm_dp_q_enable (0, 15, queues[j], 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
				sbit[0] = 12; leaf[0] = LEAF; sbit[1] = 100; leaf[1] = 0;
				tmu_token_bucket_shaper_delete(tbids[j], ((sbit[0] << 3) + (step - j)));
				tmu_egress_queue_delete(queues[j]);
				create_queue (queues[j], 19, sbit, leaf, 2);
				tbid = tbids[j]; cir = 1250000; pir = 1250000; cbs = 4000; pbs = 4000; mode = 0;
				tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
				tmu_token_bucket_shaper_create(tbids[j], ((sbit[0] << 3) + LEAF));
				LEAF--;
				entry.tc = class[j]; entry.ep = 15;
				/*pr_info("map set class %d queue %d\n", entry.tc, queues[j]);*/
				udelay(20000);
				if (j == 1) {
					qmap_flags |= CBM_QUEUE_MAP_F_TC_DONTCARE;
				}
				cbm_queue_map_set(0, queues[j], &entry, qmap_flags);
			}
			qmap_flags = temp_qmap;
			/*LEAF = step + 1;
			/for (j = 0; j <= step+1; j++) {*/
				sbit[0] = 12; leaf[0] = LEAF; sbit[1] = 100; leaf[1] = 0;
				create_queue (queues[step+1], 19, sbit, leaf, 2);
			tbid = tbids[j];	cir = 1250000; pir = 1250000; cbs = 4000; pbs = 4000; mode = 0;
			tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
			tmu_token_bucket_shaper_create(tbids[j], ((sbit[0] << 3) + LEAF));
				LEAF--;
				entry.tc = class[j]; entry.ep = 15;
			/*pr_info("map set class %d queue %d\n", entry.tc, queues[j]);*/
			udelay(20000);
				cbm_queue_map_set(0, queues[j], &entry, qmap_flags);
			/*}*/
		break;
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			step -= 7;
			LEAF = 0;
			i = 0;
			/*pr_info("delete %d\n", queues[7 - step]);*/
			cbm_dp_q_enable (0, 15, queues[7 - step], 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			class_deleted[class_count] = class[7 - step];
			class_count++;
			memset(&entry, 0, sizeof(cbm_queue_map_entry_t));
			entry.tc = class[7 - step]; entry.ep = 15;
			udelay(20000);
			cbm_queue_map_set(0, 100, &entry, qmap_flags);
			sbit[0] = 12;
			tmu_token_bucket_shaper_delete(tbids[7 - step], ((sbit[0] << 3) + (i)));
			i++;
			tmu_egress_queue_delete(queues[7 - step]);
			for (j = 7 - step - 1; j >= 0; j--) {
				/*pr_info("delete %d\n", queues[j]);*/
				cbm_dp_q_enable (0, 15, queues[j], 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
				sbit[0] = 12;
				tmu_token_bucket_shaper_delete(tbids[j], ((sbit[0] << 3) + (i)));
				i++;
				tmu_egress_queue_delete(queues[j]);
				sbit[0] = 12; leaf[0] = LEAF; sbit[1] = 100; leaf[1] = 0;
				create_queue (queues[j], 19, sbit, leaf, 2);
				tbid = tbids[j];	cir = 1250000; pir = 1250000; cbs = 4000; pbs = 4000; mode = 0;
				tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
				tmu_token_bucket_shaper_create(tbids[j], ((sbit[0] << 3) + LEAF));
				LEAF++;
				entry.tc = class[j]; entry.ep = 15;
				/*pr_info("map set class %d queue %d\n", entry.tc, queues[j]);*/
				udelay(20000);
				if (j != 1)
					cbm_queue_map_set(0, queues[j], &entry, qmap_flags);
				else {
					cbm_queue_map_set(0, queues[j], &entry, qmap_flags);
					for (k = 0; k < class_count; k++) {
						memset(&entry, 0, sizeof(cbm_queue_map_entry_t));
						entry.tc = class_deleted[k]; entry.ep = 15;
						udelay(20000);
						cbm_queue_map_set(0, 100, &entry, qmap_flags);
					}
				}
			}
		break;
		case 13:
		/*echo del_q 100 > /proc/tmu/cascade*/
			step -= 7;
		cbm_dp_q_enable (0, 15, 100, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			sbit[0] = 12;
			tmu_token_bucket_shaper_delete(tbids[1], ((sbit[0] << 3) + 0));
		tmu_egress_queue_delete(100);
		/*echo del_q 28 > /proc/tmu/cascade*/
		cbm_dp_q_enable (0, 15, 28, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			tmu_token_bucket_shaper_delete(tbids[0], ((sbit[0] << 3) + (1)));
		tmu_egress_queue_delete(28);
		/*echo attach_tb 100 100:0 > /proc/tmu/cascade*/
		LINK_B = 100; LEAF = 0; tb = 100;
		tmu_token_bucket_shaper_delete(tb, ((LINK_B << 3) + LEAF));
		/*echo create 28 12:0 100:0 19 > /proc/tmu/cascade*/
		sbit[0] = 12; leaf[0] = 0; /*sbit[1] = 100; leaf[1] = 0;*/
		create_queue (28, 19, sbit, leaf, 1);
		/*echo set 0 28 > /proc/tmu/lookup*/
		entry.tc = 0; entry.ep = 15;
			udelay(20000);
		cbm_queue_map_set(0, 28, &entry, qmap_flags);
		break;
		case 14:
			cbm_dp_q_enable (0, 15, 100, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			tmu_egress_queue_delete(100);
			cbm_dp_q_enable (0, 15, 28, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			tmu_egress_queue_delete(28);
			LINK_B = 100; LEAF = 0; tb = 100;
			sbit[0] = 12; leaf[0] = 0; /*sbit[1] = 100; leaf[1] = 0;*/
			create_queue (28, 19, sbit, leaf, 1);
			entry.tc = 0; entry.ep = 15;
			udelay(20000);
			cbm_queue_map_set(0, 28, &entry, qmap_flags);
			break;
		case 15:
			cbm_dp_q_enable (0, 15, -1, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			udelay(20000);
			cbm_dp_q_enable (0, 15, -1, 19, -1, 100000, 1, 0);
			udelay(20000);
			num_entries = 0;
			cbm_queue_map_get(0, 28, &num_entries, &temp_entry, 0);
			pr_info("num entries 28 %d\n", num_entries);
			if (num_entries)
				kfree(temp_entry);
			cbm_queue_map_get(0, 100, &num_entries, &temp_entry, 0);
			pr_info("num entries 100 %d\n", num_entries);
			if (num_entries)
				kfree(temp_entry);
			break;
		case 16:
			cbm_dp_q_enable (0, 15, -1, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			udelay(20000);
			printk("Delete Port rate shaper\n");
			tmu_token_bucket_shaper_delete(100, ((100 << 3)));
			printk("Disable scheduler id 100 output\n");
			olink.soe  = 0;/*disable scheduler output*/
			olink.lvl  = 0;
			olink.omid = 0xFF;
			olink.v    = 0;
			tmu_sched_blk_out_link_set(100, &olink);
			printk("Disable scheduler id 100 intput\n");
			for (l = 0; l < 8; l++) {
				sbin = (100 << 3) + l;
				ilink.sie  = 0;/* disable the input */
				ilink.sit  = 0;
				ilink.qsid = 0xFF;
				ilink.iwgt = 0;
				tmu_sched_blk_in_link_set(sbin, &ilink);
			}
			printk("Connect ouput of 12 to Port 19\n");
			olink.soe  = 1;/* enable scheduler output */
			olink.lvl  = 0;
			olink.omid = 19;
			olink.v    = 0;
			tmu_sched_blk_out_link_set(12, &olink);
			ilink.sie  = 1;/* enable next level input */
			ilink.sit  = 1;
			ilink.qsid = 12;
			ilink.iwgt = 0;
			printk("<%s>Port Link Set: Omid: 19   sbid: 12  \n", __FUNCTION__);
			tmu_egress_port_link_set(19, 12);
			cbm_dp_q_enable (0, 15, -1, 19, -1, 100000, 1, 0);
			udelay(20000);
			break;
		case 17:
			cbm_dp_q_enable (0, 15, -1, 19, -1, 100000, 0, (CBM_Q_F_DISABLE | CBM_Q_F_FLUSH));
			udelay(20000);
			printk("Add Port rate shaper\n");
			tmu_sched_blk_create(100, 0, 19, 0, 0);
			tmu_egress_port_enable(19, true);
			printk("Connect ouput of 12 to Scheduler 100\n");
			olink.soe  = 1;
			olink.lvl  = 1;
			olink.omid = 100 << 3;
			olink.v    = 1;
			tmu_sched_blk_out_link_set(12, &olink);
			ilink.sie  = 1;/* enable next level input */
			ilink.sit  = 1;
			ilink.qsid = 12;
			ilink.iwgt = 0;
			printk("<%s>SB Link Set: Omid: 100 sbid: 12  \n", __FUNCTION__);
			tmu_sched_blk_in_link_set(100 << 3, &ilink);
			tbid = 100;	cir = 1250000; pir = 1250000; cbs = 4000; pbs = 4000; mode = 0;
			tmu_shaper_cfg_set(tbid, 1, mode, cir, pir, cbs, pbs);
			LINK_B = 100; LEAF = 0; tb = 100;
			tmu_token_bucket_shaper_create(tb, ((LINK_B << 3) + LEAF));
			cbm_dp_q_enable (0, 15, -1, 19, -1, 100000, 1, 0);
			udelay(20000);
			break;
		default:
		break;
		};
	} else {
		LOGF_KLOG_CONT("Wrong Parameter\n");
		return count;
	}
	return count;
}
#endif

void cbm_buff_stat_dump(struct seq_file *s)
{
	struct cbm_buff_stat *buff_stat;

	if (!capable(CAP_SYS_ADMIN))
		return;

	buff_stat = cbm_buff_stat_get();
	seq_printf(s, "Std      buf alloc error = %d\n", buff_stat->std_alloc_err);
	seq_printf(s, "Jumbo    buf alloc error = %d\n", buff_stat->jbo_alloc_err);
	seq_printf(s, "Transmit buf alloc error = %d\n", buff_stat->xmit_alloc_err);
}

ssize_t fsqm_freesegment_read(struct file *file, const char *buf,
			size_t count, loff_t *ppos)
{
	uint32_t index;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[5] = { 0 };
	int num = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));

	if (num == 1) {
		if (dp_strncmpi(param_list[0], "help", strlen("help")) == 0)
			goto help;
		index = dp_atoi(param_list[0]);
		LOGF_KLOG_CONT("%8s,%8s,%8s,%8s,%8s,%8s\n", "FSC", "FSQT0",
			  "FSQT1", "FSQT2", "FSQT3", "FSQT4");
		LOGF_KLOG_CONT("0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n",
			  REG32(FSQM_BASE_D(index)
				+ OFSC), REG32(FSQM_BASE_D(index) + FSQT0),
			  REG32(FSQM_BASE_D(index) + FSQT1),
			  REG32(FSQM_BASE_D(index) + FSQT2),
			  REG32(FSQM_BASE_D(index) + FSQT3),
			  REG32(FSQM_BASE_D(index) + FSQT4));
	} else
		goto help;

	return count;
help:
		LOGF_KLOG_CONT("Usage: echo fsqmbase_index >  fsqmfsc\n");
		LOGF_KLOG_CONT("0->Standard 1->Jumbo\n");
		LOGF_KLOG_CONT("For Ex: echo 0 > fsqmfsc\n");
		return count;
}
#if 0
ssize_t fsqm_rnt_read(struct file *file, const char *buf, size_t count,
		loff_t *ppos)
{
	uint32_t buf_type, idx, reg_r_data, jsel = 0;
	static unsigned int std_buffer_addr_base;
	static unsigned int jbo_buffer_addr_base;
	uint32_t buf_addr;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[20] = { 0 };
	int num = 0;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));

	if (num == 1) {
		buf_addr = dp_atoi(param_list[0]);
		LOGF_KLOG_CONT("buf_addr: 0x%08x\n", buf_addr);
		std_buffer_addr_base = REG32(CBM_SBA0_D);
		LOGF_KLOG_CONT("std_buffer_addr_base: 0x%08x\n",
			  std_buffer_addr_base);
		jbo_buffer_addr_base = REG32(CBM_JBA0_D);
		LOGF_KLOG_CONT("jbo_buffer_addr_base: 0x%08x\n",
			  jbo_buffer_addr_base);
		buf_type = check_ptr_validation(buf_addr);
		LOGF_KLOG_CONT("buf_type: 0x%08x\n", buf_type);

		if (buf_type == SBA0_STD || buf_type == SBA1_STD) {
			idx =
			    (buf_addr -
			     std_buffer_addr_base) / cbm_std_frame_size;
		} else if (buf_type == SBA0_JBO || buf_type == SBA1_JBO) {
			reg_r_data = 0;
			reg_r_data = REG32(CBM_STAT_D);
			jsel =
			    get_val(reg_r_data, CBM_STAT_JSEL_MASK,
				    CBM_STAT_JSEL_POS);
			LOGF_KLOG_CONT("jsel: %d\n", jsel);

			if (jsel)
				idx =
				    (buf_addr -
				     jbo_buffer_addr_base) /
				    cbm_jbo_frame_size_16k;
			else
				idx =
				    (buf_addr -
				     jbo_buffer_addr_base) /
				    cbm_jbo_frame_size_8k;

		} else {
			LOGF_KLOG_CONT("ERROR: buffer type not found\n");
			return count;

		}
		LOGF_KLOG_CONT("idx: %d\n", idx);
		LOGF_KLOG_CONT("RCNT: 0x%08x\n",
			  REG32(FSQM_BASE_D(idx) + RCNT + (idx * 4)));
	} else {
		LOGF_KLOG_CONT("Wrong Parameter(try): echo buffer_address > fsqmrnt\n");
		return count;
	}
		return count;
}
#endif
static uint32_t cbm_desc64_base_d = KSEG1ADDR(CBM_DESC64B_MODULE_BASE);
#define CBM_DES64_D				cbm_desc64_base_d

void cbm_des64_ingress_dump(struct seq_file *s)
{
	if (!capable(CAP_SYS_ADMIN))
		return;

	print_reg64("SDESC0_0_IGP_5:	", CBM_DES64_D + SDESC0_0_IGP_5);
	print_reg64("SDESC1_0_IGP_5:	", CBM_DES64_D + SDESC1_0_IGP_5);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_5:	", CBM_DES64_D + SDESC0_1_IGP_5);
	print_reg64("SDESC0_1_IGP_5:	", CBM_DES64_D + SDESC1_1_IGP_5);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_5:	", CBM_DES64_D + JDESC0_0_IGP_5);
	print_reg64("JDESC1_0_IGP_5:	", CBM_DES64_D + JDESC1_0_IGP_5);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_5:	", CBM_DES64_D + JDESC0_1_IGP_5);
	print_reg64("JDESC1_1_IGP_5:	", CBM_DES64_D + JDESC1_1_IGP_5);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_6:	", CBM_DES64_D + SDESC0_0_IGP_6);
	print_reg64("SDESC1_0_IGP_6:	", CBM_DES64_D + SDESC1_0_IGP_6);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_6:	", CBM_DES64_D + SDESC0_1_IGP_6);
	print_reg64("SDESC1_1_IGP_6:	", CBM_DES64_D + SDESC1_1_IGP_6);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_6:	", CBM_DES64_D + JDESC0_0_IGP_6);
	print_reg64("JDESC1_0_IGP_6:	", CBM_DES64_D + JDESC1_0_IGP_6);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_6:	", CBM_DES64_D + JDESC0_1_IGP_6);
	print_reg64("JDESC1_1_IGP_6:	", CBM_DES64_D + JDESC1_1_IGP_6);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_7:	", CBM_DES64_D + SDESC0_0_IGP_7);
	print_reg64("SDESC1_0_IGP_7:	", CBM_DES64_D + SDESC1_0_IGP_7);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_7:	", CBM_DES64_D + SDESC0_1_IGP_7);
	print_reg64("SDESC1_1_IGP_7:	", CBM_DES64_D + SDESC1_1_IGP_7);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_7:	", CBM_DES64_D + JDESC0_0_IGP_7);
	print_reg64("JDESC1_0_IGP_7:	", CBM_DES64_D + JDESC1_0_IGP_7);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_7:	", CBM_DES64_D + JDESC0_1_IGP_7);
	print_reg64("JDESC1_1_IGP_7:	", CBM_DES64_D + JDESC1_1_IGP_7);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_8:	", CBM_DES64_D + SDESC0_0_IGP_8);
	print_reg64("SDESC1_0_IGP_8:	", CBM_DES64_D + SDESC1_0_IGP_8);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_8:	", CBM_DES64_D + SDESC0_1_IGP_8);
	print_reg64("SDESC1_1_IGP_8:	", CBM_DES64_D + SDESC1_1_IGP_8);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_8:	", CBM_DES64_D + JDESC0_0_IGP_8);
	print_reg64("JDESC1_0_IGP_8:	", CBM_DES64_D + JDESC1_0_IGP_8);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_8:	", CBM_DES64_D + JDESC0_1_IGP_8);
	print_reg64("JDESC1_1_IGP_8:	", CBM_DES64_D + JDESC1_1_IGP_8);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_9:	", CBM_DES64_D + SDESC0_0_IGP_9);
	print_reg64("SDESC1_0_IGP_9:	", CBM_DES64_D + SDESC1_0_IGP_9);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_9:	", CBM_DES64_D + SDESC0_1_IGP_9);
	print_reg64("SDESC1_1_IGP_9:	", CBM_DES64_D + SDESC1_1_IGP_9);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_9:	", CBM_DES64_D + JDESC0_0_IGP_9);
	print_reg64("JDESC1_0_IGP_9:	", CBM_DES64_D + JDESC1_0_IGP_9);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_9:	", CBM_DES64_D + JDESC0_1_IGP_9);
	print_reg64("JDESC1_1_IGP_9:	", CBM_DES64_D + JDESC1_1_IGP_9);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_10:	", CBM_DES64_D + SDESC0_0_IGP_10);
	print_reg64("SDESC1_0_IGP_10:	", CBM_DES64_D + SDESC1_0_IGP_10);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_10:	", CBM_DES64_D + SDESC0_1_IGP_10);
	print_reg64("SDESC1_1_IGP_10:	", CBM_DES64_D + SDESC1_1_IGP_10);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_10:	", CBM_DES64_D + JDESC0_0_IGP_10);
	print_reg64("JDESC1_0_IGP_10:	", CBM_DES64_D + JDESC1_0_IGP_10);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_10:	", CBM_DES64_D + JDESC0_1_IGP_10);
	print_reg64("JDESC1_1_IGP_10:	", CBM_DES64_D + JDESC1_1_IGP_10);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_11:	", CBM_DES64_D + SDESC0_0_IGP_11);
	print_reg64("SDESC1_0_IGP_11:	", CBM_DES64_D + SDESC1_0_IGP_11);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_11:	", CBM_DES64_D + SDESC0_1_IGP_11);
	print_reg64("SDESC1_1_IGP_11:	", CBM_DES64_D + SDESC1_1_IGP_11);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_11:	", CBM_DES64_D + JDESC0_0_IGP_11);
	print_reg64("JDESC1_0_IGP_11:	", CBM_DES64_D + JDESC1_0_IGP_11);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_11:	", CBM_DES64_D + JDESC0_1_IGP_11);
	print_reg64("JDESC1_1_IGP_11:	", CBM_DES64_D + JDESC1_1_IGP_11);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_12:	", CBM_DES64_D + SDESC0_0_IGP_12);
	print_reg64("SDESC1_0_IGP_12:	", CBM_DES64_D + SDESC1_0_IGP_12);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_12:	", CBM_DES64_D + SDESC0_1_IGP_12);
	print_reg64("SDESC1_1_IGP_12:	", CBM_DES64_D + SDESC1_1_IGP_12);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_12:	", CBM_DES64_D + JDESC0_0_IGP_12);
	print_reg64("JDESC1_0_IGP_12:	", CBM_DES64_D + JDESC1_0_IGP_12);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_12:	", CBM_DES64_D + JDESC0_1_IGP_12);
	print_reg64("JDESC1_1_IGP_12:	", CBM_DES64_D + JDESC1_1_IGP_12);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_13:	", CBM_DES64_D + SDESC0_0_IGP_13);
	print_reg64("SDESC1_0_IGP_13:	", CBM_DES64_D + SDESC1_0_IGP_13);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_13:	", CBM_DES64_D + SDESC0_1_IGP_13);
	print_reg64("SDESC1_1_IGP_13:	", CBM_DES64_D + SDESC1_1_IGP_13);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_13:	", CBM_DES64_D + JDESC0_0_IGP_13);
	print_reg64("JDESC1_0_IGP_13:	", CBM_DES64_D + JDESC1_0_IGP_13);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_13:	", CBM_DES64_D + JDESC0_1_IGP_13);
	print_reg64("JDESC1_1_IGP_13:	", CBM_DES64_D + JDESC1_1_IGP_13);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_0_IGP_14:	", CBM_DES64_D + SDESC0_0_IGP_14);
	print_reg64("SDESC1_0_IGP_14:	", CBM_DES64_D + SDESC1_0_IGP_14);
	LOGF_KLOG_CONT("\n");
	print_reg64("SDESC0_1_IGP_14:	", CBM_DES64_D + SDESC0_1_IGP_14);
	print_reg64("SDESC1_1_IGP_14:	", CBM_DES64_D + SDESC1_1_IGP_14);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_0_IGP_14:	", CBM_DES64_D + JDESC0_0_IGP_14);
	print_reg64("JDESC1_0_IGP_14:	", CBM_DES64_D + JDESC1_0_IGP_14);
	LOGF_KLOG_CONT("\n");
	print_reg64("JDESC0_1_IGP_14:	", CBM_DES64_D + JDESC0_1_IGP_14);
	print_reg64("JDESC1_1_IGP_14:	", CBM_DES64_D + JDESC1_1_IGP_14);
	LOGF_KLOG_CONT("\n");
}

void cbm_des64_egress_dump(struct seq_file *s)
{
	if (!capable(CAP_SYS_ADMIN))
		return;

	print_reg64("DESC0_0_EGP_5:	", CBM_DES64_D + DESC0_0_EGP_5);
	print_reg64("DESC1_0_EGP_5:	", CBM_DES64_D + DESC1_0_EGP_5);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_5:	", CBM_DES64_D + DESC0_1_EGP_5);
	print_reg64("DESC1_1_EGP_5:	", CBM_DES64_D + DESC1_1_EGP_5);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_6:	", CBM_DES64_D + DESC0_0_EGP_6);
	print_reg64("DESC1_0_EGP_6:	", CBM_DES64_D + DESC1_0_EGP_6);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_6:	", CBM_DES64_D + DESC0_1_EGP_6);
	print_reg64("DESC1_1_EGP_6:	", CBM_DES64_D + DESC1_1_EGP_6);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_7:	", CBM_DES64_D + DESC0_0_EGP_7);
	print_reg64("DESC1_0_EGP_7:	", CBM_DES64_D + DESC1_0_EGP_7);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_7:	", CBM_DES64_D + DESC0_1_EGP_7);
	print_reg64("DESC1_1_EGP_7:	", CBM_DES64_D + DESC1_1_EGP_7);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_8:	", CBM_DES64_D + DESC0_0_EGP_8);
	print_reg64("DESC1_0_EGP_8:	", CBM_DES64_D + DESC1_0_EGP_8);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_8:	", CBM_DES64_D + DESC0_1_EGP_8);
	print_reg64("DESC1_1_EGP_8:	", CBM_DES64_D + DESC1_1_EGP_8);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_9:	", CBM_DES64_D + DESC0_0_EGP_9);
	print_reg64("DESC1_0_EGP_9:	", CBM_DES64_D + DESC1_0_EGP_9);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_9:	", CBM_DES64_D + DESC0_1_EGP_9);
	print_reg64("DESC1_1_EGP_9:	", CBM_DES64_D + DESC1_1_EGP_9);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_10:	", CBM_DES64_D + DESC0_0_EGP_10);
	print_reg64("DESC1_0_EGP_10:	", CBM_DES64_D + DESC1_0_EGP_10);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_10:	", CBM_DES64_D + DESC0_1_EGP_10);
	print_reg64("DESC1_1_EGP_10:	", CBM_DES64_D + DESC1_1_EGP_10);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_11:	", CBM_DES64_D + DESC0_0_EGP_11);
	print_reg64("DESC1_0_EGP_11:	", CBM_DES64_D + DESC1_0_EGP_11);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_11:	", CBM_DES64_D + DESC0_1_EGP_11);
	print_reg64("DESC1_1_EGP_11:	", CBM_DES64_D + DESC1_1_EGP_11);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_12:	", CBM_DES64_D + DESC0_0_EGP_12);
	print_reg64("DESC1_0_EGP_12:	", CBM_DES64_D + DESC1_0_EGP_12);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_12:	", CBM_DES64_D + DESC0_1_EGP_12);
	print_reg64("DESC1_1_EGP_12:	", CBM_DES64_D + DESC1_1_EGP_12);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_13:	", CBM_DES64_D + DESC0_0_EGP_13);
	print_reg64("DESC1_0_EGP_13:	", CBM_DES64_D + DESC1_0_EGP_13);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_13:	", CBM_DES64_D + DESC0_1_EGP_13);
	print_reg64("DESC1_1_EGP_13:	", CBM_DES64_D + DESC1_1_EGP_13);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_14:	", CBM_DES64_D + DESC0_0_EGP_14);
	print_reg64("DESC1_0_EGP_14:	", CBM_DES64_D + DESC1_0_EGP_14);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_14:	", CBM_DES64_D + DESC0_1_EGP_14);
	print_reg64("DESC1_1_EGP_14:	", CBM_DES64_D + DESC1_1_EGP_14);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_15:	", CBM_DES64_D + DESC0_0_EGP_15);
	print_reg64("DESC1_0_EGP_15:	", CBM_DES64_D + DESC1_0_EGP_15);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_15:	", CBM_DES64_D + DESC0_1_EGP_15);
	print_reg64("DESC1_1_EGP_15:	", CBM_DES64_D + DESC1_1_EGP_15);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_16:	", CBM_DES64_D + DESC0_0_EGP_16);
	print_reg64("DESC1_0_EGP_16:	", CBM_DES64_D + DESC1_0_EGP_16);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_16:	", CBM_DES64_D + DESC0_1_EGP_16);
	print_reg64("DESC1_1_EGP_16:	", CBM_DES64_D + DESC1_1_EGP_16);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_17:	", CBM_DES64_D + DESC0_0_EGP_17);
	print_reg64("DESC1_0_EGP_17:	", CBM_DES64_D + DESC1_0_EGP_17);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_17:	", CBM_DES64_D + DESC0_1_EGP_17);
	print_reg64("DESC1_1_EGP_17:	", CBM_DES64_D + DESC1_1_EGP_17);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_18:	", CBM_DES64_D + DESC0_0_EGP_18);
	print_reg64("DESC1_0_EGP_18:	", CBM_DES64_D + DESC1_0_EGP_18);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_18:	", CBM_DES64_D + DESC0_1_EGP_18);
	print_reg64("DESC1_1_EGP_18:	", CBM_DES64_D + DESC1_1_EGP_18);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_19:	", CBM_DES64_D + DESC0_0_EGP_19);
	print_reg64("DESC1_0_EGP_19:	", CBM_DES64_D + DESC1_0_EGP_19);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_19:	", CBM_DES64_D + DESC0_1_EGP_19);
	print_reg64("DESC1_1_EGP_19:	", CBM_DES64_D + DESC1_1_EGP_19);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_20:	", CBM_DES64_D + DESC0_0_EGP_20);
	print_reg64("DESC1_0_EGP_20:	", CBM_DES64_D + DESC1_0_EGP_20);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_20:	", CBM_DES64_D + DESC0_1_EGP_20);
	print_reg64("DESC1_1_EGP_20:	", CBM_DES64_D + DESC1_1_EGP_20);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_21:	", CBM_DES64_D + DESC0_0_EGP_21);
	print_reg64("DESC1_0_EGP_21:	", CBM_DES64_D + DESC1_0_EGP_21);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_21:	", CBM_DES64_D + DESC0_1_EGP_21);
	print_reg64("DESC1_1_EGP_21:	", CBM_DES64_D + DESC1_1_EGP_21);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_0_EGP_22:	", CBM_DES64_D + DESC0_0_EGP_22);
	print_reg64("DESC1_0_EGP_22:	", CBM_DES64_D + DESC1_0_EGP_22);
	LOGF_KLOG_CONT("\n");
	print_reg64("DESC0_1_EGP_22:	", CBM_DES64_D + DESC0_1_EGP_22);
	print_reg64("DESC1_1_EGP_22:	", CBM_DES64_D + DESC1_1_EGP_22);
	LOGF_KLOG_CONT("\n");
}
