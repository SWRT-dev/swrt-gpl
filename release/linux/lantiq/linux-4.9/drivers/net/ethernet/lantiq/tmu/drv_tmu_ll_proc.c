/******************************************************************************

			Copyright (c) 2012
			Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/** \addtogroup TMU_LL_STACK
   @{
*/

/** \defgroup TMU_LL_STACK Linux Specific Implementation
   @{
*/

#include "drv_tmu_api.h"
#include "net/drv_tmu_ll.h"
#include "drv_tmu_reg.h"
#include <net/datapath_proc_api.h>

ssize_t tmu_proc_tmu_reg_write(struct file *file, const char *buf,
			       size_t count, loff_t *ppos);

static void tmu_proc_version_get(struct seq_file *s)
{
	SEQ_PRINTF(s, "%d.%d.%d_%d\n", TMU_VER_MAJOR, TMU_VER_MINOR,
		   TMU_VER_STEP, TMU_VER_TYPE);
}

int32_t tmu_spin_lock_init(spinlock_t *id, const char *p_name)
{
	spin_lock_init(id);
	return 0;
}

int32_t tmu_spin_lock_delete(spinlock_t *id)
{
	return 0;
}

int32_t tmu_spin_lock_get(spinlock_t *id, unsigned long *flags)
{
	spin_lock_irqsave(id, *flags);
	return 0;
}

int32_t tmu_spin_lock_release(spinlock_t *id, unsigned long flags)
{
	spin_unlock_irqrestore(id, flags);
	return 0;
}

uint32_t tmu_register_read(const void *reg)
{
	uint32_t val = 0;
	val = __raw_readl(reg);
	return val;
}

void tmu_register_write(const void *reg, uint32_t val)
{
	__raw_writel(val, (void __iomem *)reg);
}

#if defined(CONFIG_PROC_FS) && defined(INCLUDE_PROCFS_SUPPORT)

typedef void (*proc_single_callback_t) (struct seq_file *);
typedef int (*proc_callback_t) (struct seq_file *, int);
typedef int (*proc_init_callback_t) (void);
typedef ssize_t(*proc_write_callback_t) (struct file *file,
					 const char __user *input,
					 size_t size, loff_t *loff);

struct proc_file_entry {
	proc_callback_t callback;
	int pos;
};

struct proc_entry {
	char *name;
	proc_single_callback_t single_callback;
	proc_callback_t callback;
	proc_init_callback_t init_callback;
	proc_write_callback_t write_callback;
	struct file_operations ops;
};

int tmu_lookup_start(void)
{
	return 0;
}

int tmu_lookup_dump(struct seq_file *s, int pos)
{
	if (find_pattern(pos, s, -1) < 0)
		return pos;
	if (!seq_has_overflowed(s))
		pos++;
	if (pos >= 16)
		pos = -1;
	return pos;
}

static struct dp_proc_entry proc_entries[] = {
	/* name             single_callback_t    multi_callbac
	   _t    multi_callback_start    write_callback_t
	 */
	{PROC_FILE_VER, tmu_proc_version_get, NULL, NULL, NULL},
	{PROC_FILE_CBMLOOKUP, NULL, tmu_lookup_dump, tmu_lookup_start,
	 tmu_proc_get_qid_via_index},
	{PROC_FILE_CASCADE, NULL, NULL, NULL, tmu_proc_tmu_cascade_write},

#ifdef CONFIG_LTQ_TMU_CHIPTEST
	{PROC_FILE_TEST, tmu_test_dump, NULL, NULL, tmu_proc_testcase_write},
#endif
	{PROC_FILE_TMU, tmu_dump, NULL, NULL, tmu_proc_tmu_reg_write},
	{PROC_FILE_EQT, tmu_eqt_dump, NULL, NULL, tmu_proc_eqt_write},
	{PROC_FILE_EPT, tmu_ept_dump, NULL, NULL, tmu_proc_port_write},
	{PROC_FILE_SBIT, NULL, tmu_sbit_dump, tmu_sbit_dump_start,
	 tmu_proc_sbit_write},
	{PROC_FILE_SBOT, NULL, tmu_sbot_dump, tmu_sbot_dump_start,
	 tmu_proc_sbot_write},
	/*{PROC_FILE_TBST,      tmu_tbst_dump,       NULL,
	   NULL,                   NULL},
	 */
	{PROC_FILE_TBST, NULL, tmu_tbst_dump, tmu_tbst_dump_start,
	 tmu_proc_tbst_write},
	{PROC_FILE_PPT, NULL, tmu_ppt_dump, tmu_ppt_dump_start, NULL},
	{PROC_FILE_QUEUE, NULL, tmu_queue_raw_dump, tmu_queue_dump_start,
	 tmu_proc_queue_write},
	/*summarized */
	{PROC_FILE_QUEUE1, NULL, tmu_queue_dump, tmu_queue_dump_start, NULL},
	{PROC_FILE_DBG, tmu_proc_debug_dump, NULL, NULL,
	 tmu_proc_debug_write},
	{PROC_FILE_CLK, tmu_proc_clk_dump, NULL, NULL, tmu_proc_clk_write},

	/*the last one for place holder */
	{NULL, NULL, NULL, NULL, NULL}
};

/**
   Initialize and install the proc entry

   \return
   -1 or 0 on success

   \remarks
   Called by the kernel.
*/
int tmu_proc_install(struct dentry *parent)
{
	struct dentry *driver_proc_node;
	driver_proc_node = debugfs_create_dir(TMU_PROC_PARENT TMU_PROC_NAME,
					      parent);
	if (driver_proc_node != NULL) {
		int i;
		for (i = 0; i < ARRAY_SIZE(proc_entries); i++)
			dp_proc_entry_create(driver_proc_node,
					     &proc_entries[i]);
	} else {
		TMU_DEBUG(TMU_DEBUG_ERR, "cannot create proc entry");
		return -1;
	}
	return 0;
}
EXPORT_SYMBOL(tmu_proc_install);

#endif				/* TMU_LINUX_KERNEL */

/**
   Initialize the driver module.

   \return
   - 0 on success
   - Error code

   \remarks
   Called by the kernel.
*/
int tmu_ll_stack_init(void)
{
	int res = 0;
#if defined(CONFIG_PROC_FS) && defined(INCLUDE_PROCFS_SUPPORT)
	tmu_proc_install(NULL);
#endif
	return res;
}

struct proc_reg_entry;
/* cmd 0 read
       1 write
*/

struct proc_reg_entry {
	uint32_t cmd;		/*r/w: read/write */
	char *regname;		/*like ept, sbid */
	uint32_t regid;		/*id since ept/eqt/sbit have multples */
	char *fieldname;	/*file name, like epe, sbid */
	uint32_t bit_offset;	/*internal */
	uint32_t bit_mask;	/*internal */
	/*uint32_t (*callback)(uint32_t cmd, uint32_t regid, uint32_t bit
	   offset, uint32_t bit_mask, uint32_t *value, uint32_t flag);
	 */
	unsigned long callbak;
};

/*command example:
	echo w TBST 0  MBS0 value > /proc/tmu/tmu
	echo r TBST 0  MBS0       > /proc/tmu/tmu
	0  1   2   3   4
*/
static struct proc_reg_entry proc_reg_entries[] = {
	/*cmd   regname regid   fieldname       bit_offset
	   bit_mask                     callback
	 */
	/*Register read/write */
	/*Token Bucket Shaper Table REG */
	{0, "TBSTR0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR0", 0, "MOD", TMU_TBSTR0_MOD_OFFSET, TMU_TBSTR0_MOD_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR0", 0, "SBIN", TMU_TBSTR0_SBIN_OFFSET, TMU_TBSTR0_SBIN_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR1", 0, "SRM0", TMU_TBSTR1_SRM0_OFFSET, TMU_TBSTR1_SRM0_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR1", 0, "TBU0", TMU_TBSTR1_TBU0_OFFSET, TMU_TBSTR1_TBU0_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR1", 0, "TBE0", 31, TMU_TBSTR1_TBE0,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR2", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR2", 0, "SRM1", TMU_TBSTR2_SRM1_OFFSET, TMU_TBSTR2_SRM1_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR2", 0, "TBU1", TMU_TBSTR2_TBU1_OFFSET, TMU_TBSTR2_TBU1_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR2", 0, "TBE1", 31, TMU_TBSTR2_TBE1,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR3", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR3", 0, "MBS0", TMU_TBSTR3_MBS0_OFFSET, TMU_TBSTR3_MBS0_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR4", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR4", 0, "MBS1", TMU_TBSTR4_MBS1_OFFSET, TMU_TBSTR4_MBS1_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR5", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR5", 0, "SRC0", TMU_TBSTR5_SRC0_OFFSET, TMU_TBSTR5_SRC0_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR6", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR6", 0, "SRC1", TMU_TBSTR6_SRC1_OFFSET, TMU_TBSTR6_SRC1_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR7", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR7", 0, "PASS0", 31, TMU_TBSTR7_PASS0,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR7", 0, "TBC0", TMU_TBSTR7_TBC0_OFFSET, TMU_TBSTR7_TBC0_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR8", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR8", 0, "PASS1", 31, TMU_TBSTR7_PASS0,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR8", 0, "TBC1", TMU_TBSTR8_TBC1_OFFSET, TMU_TBSTR8_TBC1_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR9", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR9", 0, "QOSL", TMU_TBSTR9_QOSL_OFFSET, TMU_TBSTR9_QOSL_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR9", 0, "COL", TMU_TBSTR9_COL_OFFSET, TMU_TBSTR9_COL_MASK,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR10", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_tbst},
	{0, "TBSTR10", 0, "TS", TMU_TBSTR10_TS_OFFSET, TMU_TBSTR10_TS_MASK,
	 (unsigned long)&read_write_reg_tbst},

	/*SB Input Table REG */
	{0, "SBITR0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR0", 0, "SIE", 31, TMU_SBITR0_SIE,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR0", 0, "IWGT", TMU_SBITR0_IWGT_OFFSET, TMU_SBITR0_IWGT_MASK,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR0", 0, "SIT", 15, TMU_SBITR0_SIT,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR0", 0, "QSID", TMU_SBITR0_QSID_OFFSET, TMU_SBITR0_QSID_MASK,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR1", 0, "TBE", 31, TMU_SBITR1_TBE,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR1", 0, "TBID", TMU_SBITR1_TBID_OFFSET, TMU_SBITR1_TBID_MASK,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR2", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR2", 0, "SIF", 31, TMU_SBITR2_SIF,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR2", 0, "SIP", 30, TMU_SBITR2_SIP,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR2", 0, "VDT", TMU_SBITR2_VDT_OFFSET, TMU_SBITR2_VDT_MASK,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR3", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR3", 0, "QOSL", TMU_SBITR3_QOSL_OFFSET, TMU_SBITR3_QOSL_MASK,
	 (unsigned long)&read_write_reg_sbit},
	{0, "SBITR3", 0, "COL", TMU_SBITR3_COL_OFFSET, TMU_SBITR3_COL_MASK,
	 (unsigned long)&read_write_reg_sbit},

	/*SB Out Table REG */
	{0, "SBOTR0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR0", 0, "SOE", 31, TMU_SBOTR0_SOE,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR0", 0, "LVL", TMU_SBOTR0_LVL_OFFSET, TMU_SBOTR0_LVL_MASK,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR0", 0, "V", 15, TMU_SBOTR0_V,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR0", 0, "OMID", TMU_SBOTR0_OMID_OFFSET, TMU_SBOTR0_OMID_MASK,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR1", 0, "SOF", 31, TMU_SBOTR1_SOF,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR1", 0, "WL", TMU_SBOTR1_WL_OFFSET, TMU_SBOTR1_WL_MASK,
	 (unsigned long)&read_write_reg_sbot},
	{0, "SBOTR1", 0, "WQID", TMU_SBOTR1_WQID_OFFSET, TMU_SBOTR1_WQID_MASK,
	 (unsigned long)&read_write_reg_sbot},

	/*Egress Port Occupancy Table REG */
	{0, "EPOT0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epot},
	{0, "EPOT0", 0, "EPOC1", TMU_EPOT0_EPOC1_OFFSET, TMU_EPOT0_EPOC1_MASK,
	 (unsigned long)&read_write_reg_epot},
	{0, "EPOT0", 0, "EPOC0", TMU_EPOT0_EPOC0_OFFSET, TMU_EPOT0_EPOC0_MASK,
	 (unsigned long)&read_write_reg_epot},
	{0, "EPOT1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epot},
	{0, "EPOT1", 0, "EPOC3", TMU_EPOT1_EPOC3_OFFSET, TMU_EPOT1_EPOC3_MASK,
	 (unsigned long)&read_write_reg_epot},
	{0, "EPOT1", 0, "EPOC2", TMU_EPOT1_EPOC2_OFFSET, TMU_EPOT1_EPOC2_MASK,
	 (unsigned long)&read_write_reg_epot},

	/*Egress Port Threshold Table REG */
	{0, "EPTT0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_eptt},
	{0, "EPTT0", 0, "ETTH1", TMU_EPTT0_EPTH1_OFFSET, TMU_EPTT0_EPTH1_MASK,
	 (unsigned long)&read_write_reg_eptt},
	{0, "EPTT0", 0, "ETTH0", TMU_EPTT0_EPTH0_OFFSET, TMU_EPTT0_EPTH0_MASK,
	 (unsigned long)&read_write_reg_eptt},
	{0, "EPTT1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_eptt},
	{0, "EPTT1", 0, "ETTH3", TMU_EPTT1_EPTH3_OFFSET, TMU_EPTT1_EPTH3_MASK,
	 (unsigned long)&read_write_reg_eptt},
	{0, "EPTT1", 0, "ETTH2", TMU_EPTT1_EPTH2_OFFSET, TMU_EPTT1_EPTH2_MASK,
	 (unsigned long)&read_write_reg_eptt},

	/*Egress Port Discard Table REG */
	{0, "EPDT0", 0, "EPDC0", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epdt},
	{0, "EPDT1", 0, "EPDC1", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epdt},
	{0, "EPDT2", 0, "EPDC2", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epdt},
	{0, "EPDT3", 0, "EPDC3", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epdt},

	/*Egress Port Mapping Table REG */
	{0, "EPMT", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_epmt},
	{0, "EPMT", 0, "EPE", 31, TMU_EPMT_EPE,
	 (unsigned long)&read_write_reg_epmt},
	{0, "EPMT", 0, "SBID", TMU_EPMT_SBID_OFFSET, TMU_EPMT_SBID_MASK,
	 (unsigned long)&read_write_reg_epmt},

	/* Queue Threshold Table REG */
	{0, "QTHT0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT0", 0, "COL0", TMU_QTHT0_COL0_OFFSET, TMU_QTHT0_COL0_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT0", 0, "COL1", TMU_QTHT0_COL1_OFFSET, TMU_QTHT0_COL1_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT0", 0, "COL2", TMU_QTHT0_COL2_OFFSET, TMU_QTHT0_COL2_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT0", 0, "COL3", TMU_QTHT0_COL3_OFFSET, TMU_QTHT0_COL3_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT0", 0, "DMOD", 16, TMU_QTHT0_DMOD,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT0", 0, "QE", 31, TMU_QTHT0_QE,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT1", 0, "MITH0", TMU_QTHT1_MITH0_OFFSET, TMU_QTHT1_MITH0_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT1", 0, "MITH1", TMU_QTHT1_MITH1_OFFSET, TMU_QTHT1_MITH1_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT2", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT2", 0, "MATH0", TMU_QTHT2_MATH0_OFFSET, TMU_QTHT2_MATH0_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT2", 0, "MATH1", TMU_QTHT2_MATH1_OFFSET, TMU_QTHT2_MATH1_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT3", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT3", 0, "SLOPE0", TMU_QTHT3_SLOPE0_OFFSET,
	 TMU_QTHT3_SLOPE0_MASK, (unsigned long)&read_write_reg_qtht},
	{0, "QTHT3", 0, "SLOPE1", TMU_QTHT3_SLOPE1_OFFSET,
	 TMU_QTHT3_SLOPE1_MASK, (unsigned long)&read_write_reg_qtht},
	{0, "QTHT3", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT4", 0, "QTTH0", TMU_QTHT4_QTTH0_OFFSET, TMU_QTHT4_QTTH0_MASK,
	 (unsigned long)&read_write_reg_qtht},
	{0, "QTHT4", 0, "QTTH1", TMU_QTHT4_QTTH1_OFFSET, TMU_QTHT4_QTTH1_MASK,
	 (unsigned long)&read_write_reg_qtht},

	/* Queue Occupancy Table REG */
	{0, "QOCT0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qoct},
	{0, "QOCT0", 0, "QRTH", TMU_QOCT0_QRTH_OFFSET, TMU_QOCT0_QRTH_MASK,
	 (unsigned long)&read_write_reg_qoct},
	{0, "QOCT0", 0, "WQ", TMU_QOCT0_WQ_OFFSET, TMU_QOCT0_WQ_MASK,
	 (unsigned long)&read_write_reg_qoct},
	{0, "QOCT1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qoct},
	{0, "QOCT1", 0, "QOCC", TMU_QOCT1_QOCC_OFFSET, TMU_QOCT1_QOCC_MASK,
	 (unsigned long)&read_write_reg_qoct},
	{0, "QOCT2", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qoct},
	{0, "QOCT2", 0, "QAVG", TMU_QOCT2_QAVG_OFFSET, TMU_QOCT2_QAVG_MASK,
	 (unsigned long)&read_write_reg_qoct},

	/* Queue Discard CounterTable REG */
	{0, "QDCT0", 0, "QDC0", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qdct},
	{0, "QDCT1", 0, "QDC1", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qdct},
	{0, "QDCT2", 0, "QDC2", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qdct},
	{0, "QDCT3", 0, "QDC3", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qdct},

	/* Queue FIFO Manager Table REG */
	{0, "QFMT0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT0", 0, "POCC", TMU_QFMT0_POCC_OFFSET, TMU_QFMT0_POCC_MASK,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT0", 0, "QF", 31, TMU_QFMT0_QF,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT1", 0, "HQPP", TMU_QFMT1_HQPP_OFFSET, TMU_QFMT1_HQPP_MASK,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT1", 0, "TQPP", TMU_QFMT1_TQPP_OFFSET, TMU_QFMT1_TQPP_MASK,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT2", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT2", 0, "SEGL", TMU_QFMT2_SEGL_OFFSET, TMU_QFMT2_SEGL_MASK,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT2", 0, "COL", TMU_QFMT2_COL_OFFSET, TMU_QFMT2_COL_MASK,
	 (unsigned long)&read_write_reg_qfmt},
	{0, "QFMT2", 0, "QOSL", TMU_QFMT2_QOSL_OFFSET, TMU_QFMT2_QOSL_MASK,
	 (unsigned long)&read_write_reg_qfmt},

	/* Queue Manager Table REG */
	{0, "QMTC", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QID", TMU_QMTC_QID_OFFSET, TMU_QMTC_QID_MASK,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QEW", 8, TMU_QMTC_QEW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QSW", 9, TMU_QMTC_QSW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QTW", 10, TMU_QMTC_QTW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QOW", 11, TMU_QMTC_QOW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QCW", 12, TMU_QMTC_QCW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QDW", 13, TMU_QMTC_QDW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QFW", 14, TMU_QMTC_QFW,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QER", 16, TMU_QMTC_QER,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QSR", 17, TMU_QMTC_QSR,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QTR", 18, TMU_QMTC_QTR,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QOR", 19, TMU_QMTC_QOR,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QCR", 20, TMU_QMTC_QCR,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QDR", 21, TMU_QMTC_QDR,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QFR", 22, TMU_QMTC_QFR,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QEV", 24, TMU_QMTC_QEV,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QSV", 25, TMU_QMTC_QSV,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QTV", 26, TMU_QMTC_QTV,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QOV", 27, TMU_QMTC_QOV,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QCV", 28, TMU_QMTC_QCV,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QDV", 29, TMU_QMTC_QDV,
	 (unsigned long)&read_write_reg_qmtc},
	{0, "QMTC", 0, "QFV", 30, TMU_QMTC_QFV,
	 (unsigned long)&read_write_reg_qmtc},

	/* Queue Egress Mapping Table REG */
	{0, "QEMT", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qemt},
	{0, "QEMT", 0, "EPN", TMU_QEMT_EPN_OFFSET, TMU_QEMT_EPN_MASK,
	 (unsigned long)&read_write_reg_qemt},

	/* Queue Schedular Mapping Table REG */
	{0, "QSMT", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_qsmt},
	{0, "QSMT", 0, "SBIN", TMU_QSMT_SBIN_OFFSET, TMU_QSMT_SBIN_MASK,
	 (unsigned long)&read_write_reg_qsmt},

	/* Global Occupancy Threshold Table REG */
	{0, "GOTHR0", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR0", 0, "GOTH0", TMU_GOTHR0_GOTH0_OFFSET,
	 TMU_GOTHR0_GOTH0_MASK,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR1", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR1", 0, "GOTH1", TMU_GOTHR1_GOTH1_OFFSET,
	 TMU_GOTHR1_GOTH1_MASK,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR2", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR2", 0, "GOTH2", TMU_GOTHR2_GOTH2_OFFSET,
	 TMU_GOTHR2_GOTH2_MASK,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR3", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_gothr},
	{0, "GOTHR3", 0, "GOTH3", TMU_GOTHR3_GOTH3_OFFSET,
	 TMU_GOTHR3_GOTH3_MASK,
	 (unsigned long)&read_write_reg_gothr},

	/* Crawler Period REG */
	{0, "CPR", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_cpr},
	{0, "CPR", 0, "CP", TMU_CPR_CP_OFFSET, TMU_CPR_CP_MASK,
	 (unsigned long)&read_write_reg_cpr},

	/* Crawler Status REG */
	{0, "CSR", 0, "all", 0, 0xFFFFFFFF,
	 (unsigned long)&read_write_reg_csr},
	{0, "CSR", 0, "LQS", TMU_CSR_LQS_OFFSET, TMU_CSR_LQS_MASK,
	 (unsigned long)&read_write_reg_csr},
	{0, "CSR", 0, "CNT", TMU_CSR_CNT_OFFSET, TMU_CSR_CNT_MASK,
	 (unsigned long)&read_write_reg_csr},

};

void tmu_proc_tmu_reg_write_help(void)
{
	int i;
	for (i = 0;
	     i < sizeof(proc_reg_entries) / sizeof(proc_reg_entries[0]);
	     i++) {
		TMU_PRINT("echo r %s id %s > %s\n",
			  proc_reg_entries[i].regname,
			  proc_reg_entries[i].fieldname, PROC_FILE_TMU);
		TMU_PRINT("echo w %s id %s new_value > %s\n",
			  proc_reg_entries[i].regname,
			  proc_reg_entries[i].fieldname, PROC_FILE_TMU);
	}
}

ssize_t tmu_proc_tmu_reg_write(struct file *file, const char *buf,
			       size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *param_list[9] = { 0 };
	int i, num, cmd, entry_found = 0;
	uint32_t value = 0, regid = 0;
	uint32_t(*func) (uint32_t cmd, uint32_t regid, char *reg_name,
			 uint32_t bit_offset, uint32_t bit_mask,
			 uint32_t *value, uint32_t flag);
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	if (dp_strncmpi(param_list[0], "help", strlen("help")) == 0)
		tmu_proc_tmu_reg_write_help();
	if (dp_strncmpi(param_list[0], "r", 1) == 0)
		cmd = 0;
	else if (dp_strncmpi(param_list[0], "w", 1) == 0)
		cmd = 1;
	else {
		TMU_PRINT("Wrong comamnd:%s\n", param_list[0]);
		goto EXIT;
	}
	for (i = 0;
	     i < sizeof(proc_reg_entries) / sizeof(proc_reg_entries[0]);
	     i++) {
		if ((dp_strncmpi(proc_reg_entries[i].regname,
				 param_list[1],
				 strlen(param_list[1]))
				 == 0) &&
		    (dp_strncmpi(proc_reg_entries[i].fieldname,
				 param_list[3],
				 strlen(param_list[3]))
				 == 0)) {
			regid = dp_atoi(param_list[2]);
			/**/ if (cmd)
				value = dp_atoi(param_list[4]);

			func = (void *)proc_reg_entries[i].callbak;
			if (func) {
				entry_found = 1;
				value =
				    func(cmd, regid,
					 proc_reg_entries[i].regname,
					 proc_reg_entries[i].bit_offset,
					 proc_reg_entries[i].bit_mask, &value,
					 0);
			}
			TMU_PRINT
			    ("Cmd:%s 0x%x %s %s[%d].%s with offset(0x%x)/mask(0x%x)\n",
			     cmd ? "Write" : "Read Data = ", value,
			     cmd ? "to register" : "from register",
			     proc_reg_entries[i].regname, regid,
			     proc_reg_entries[i].fieldname,
			     proc_reg_entries[i].bit_offset,
			     proc_reg_entries[i].bit_mask);
			break;
		}
	}
	if (!entry_found)
		TMU_PRINT
		    ("Entry not found:Wrong regname(%s) or feildname(%s) or Command -try help\n",
		     param_list[1], param_list[3]);
 EXIT:
	return count;
}

/**
   Clean up the module if unloaded.

   \remarks
   Called by the kernel.
*/
int tmu_ll_stack_exit(void)
{
#if defined(CONFIG_PROC_FS) && defined(INCLUDE_PROCFS_SUPPORT)
	char buf[64];
	int i;
#endif
	int res = 0;
#if defined(CONFIG_PROC_FS) && defined(INCLUDE_PROCFS_SUPPORT)
	for (i = 0; i < ARRAY_SIZE(proc_entries); i++) {
		sprintf(buf, TMU_PROC_PARENT TMU_PROC_NAME "/%s",
			proc_entries[i].name);
		remove_proc_entry(buf, 0);
	}
	remove_proc_entry(TMU_PROC_PARENT TMU_PROC_NAME, 0);
#endif
	if (res)
		TMU_DEBUG(TMU_DEBUG_MSG, "cleanup successful");
	return res;
}

#ifdef TMU_LL_ONLY
int main(int argc, char **argv)
{
	tmu_ll_init();
	tmu_ll_exit();
}
#endif

/*! @} */

/*! @} */
