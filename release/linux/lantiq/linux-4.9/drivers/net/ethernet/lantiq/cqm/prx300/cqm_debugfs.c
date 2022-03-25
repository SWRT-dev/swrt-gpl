#include <linux/klogging.h>
#include "cqm_debugfs.h"
#include <net/datapath_proc_api.h>
#include <net/lantiq_cbm_api.h>
#include "cqm.h"
#include "../cqm_common.h"

#define PRINTK  pr_err

static ssize_t print_cqm_dbg_cntrs_write(struct file *file,
					 const char __user *buf,
					 size_t count, loff_t *ppos)
{
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

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0)) {
		PRINTK("Help: Please use:\n");
		PRINTK("cat /sys/kernel/debug/cqm/pkt_count\n");
	}
	return count;
}

static int print_cqm_dbg_cntrs(struct seq_file *s, void *v)
{
	int i, j, val;
	u32 rx_cnt = 0;
	u32 tx_cnt = 0;
	u32 free_cnt = 0;
	u32 alloc_cnt = 0;
	u32 rx_cnt_t = 0;
	u32 tx_cnt_t = 0;
	u32 free_cnt_t = 0;
	u32 alloc_cnt_t = 0;
	u32 isr_free_cnt_t = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_puts(s, "Print CQEM debug counters:\n");
	seq_puts(s, "\t\t\trx\t\ttx\t\tfree\t\talloc\t\tdelta (rx+alloc-tx-free)\n");
	for (i = 0; i < CQM_MAX_POLICY_NUM; i++) {
		for (j = 0; j < CQM_MAX_POOL_NUM; j++) {
			if (i == CQM_CPU_POLICY && j == CQM_CPU_POOL)
				continue;
			rx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].rx_cnt);
			tx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].tx_cnt);
			free_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].free_cnt);
			alloc_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].alloc_cnt);

			if (rx_cnt_t || tx_cnt_t || free_cnt_t ||
			    alloc_cnt_t) {
				seq_printf(s, "policy (%d) pool( %d ):", i, j);

				seq_printf(s, "\t0x%08x", rx_cnt_t);
				seq_printf(s, "\t0x%08x", tx_cnt_t);
				seq_printf(s, "\t0x%08x", free_cnt_t);
				seq_printf(s, "\t0x%08x", alloc_cnt_t);

				rx_cnt += rx_cnt_t;
				tx_cnt += tx_cnt_t;
				free_cnt += free_cnt_t;
				alloc_cnt += alloc_cnt_t;
				val = (rx_cnt_t + alloc_cnt_t - tx_cnt_t -
				       free_cnt_t);

				seq_printf(s, "\t%d\n", val);
			}
		}
	}
	seq_puts(s, "Total counters:");
	seq_printf(s, "\t\t0x%08x", rx_cnt);
	seq_printf(s, "\t0x%08x", tx_cnt);
	seq_printf(s, "\t0x%08x", free_cnt);
	seq_printf(s, "\t0x%08x", alloc_cnt);
	val = (rx_cnt + alloc_cnt - tx_cnt - free_cnt);

	seq_printf(s, "\t%d\n", val);
	i = CQM_CPU_POLICY;
	j = CQM_CPU_POOL;
	seq_puts(s, "\nPrint CQEM cpu buffer debug counters:\n");
	seq_puts(s, "\t\t\trx\t\ttx\t\tfree\t\talloc\t\tisr_free\tdelta (rx+alloc+tx+free-isr_free)\n");
	rx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].rx_cnt);
	tx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].tx_cnt);
	free_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].free_cnt);
	alloc_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].alloc_cnt);
	isr_free_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].isr_free_cnt);
	seq_printf(s, "policy (%d) pool( %d ):", i, j);
	seq_printf(s, "\t0x%08x", rx_cnt_t);
	seq_printf(s, "\t0x%08x", tx_cnt_t);
	seq_printf(s, "\t0x%08x", free_cnt_t);
	seq_printf(s, "\t0x%08x", alloc_cnt_t);
	seq_printf(s, "\t0x%08x", isr_free_cnt_t);
	val = (rx_cnt_t + alloc_cnt_t + tx_cnt_t + free_cnt_t) -
	       isr_free_cnt_t;
	seq_printf(s, "\t%d\n", val);
	return 0;
}

static int cqm_dbg_cntr_open(struct inode *inode, struct file *file)
{
	return single_open(file, print_cqm_dbg_cntrs, inode->i_private);
}

static const struct file_operations cqm_dbg_cntrs_fops = {
	.open = cqm_dbg_cntr_open,
	.read = seq_read,
	.write = print_cqm_dbg_cntrs_write,
	.llseek = seq_lseek,
	.release = single_release,
};

uint64_t REG64(void *addr)
{
	u64 readreg;
	u32 reg1, reg2;

	reg1 = cbm_r32(addr);
	reg2 = cbm_r32(addr + 4);
	readreg = (((u64)reg1) << 32) + (u64)reg2;
	return readreg;
}
EXPORT_SYMBOL(REG64);

void print_reg64(char *name, void *addr)
{
	u32 reg1, reg2;
	u64 reg;

	reg = REG64(addr);
	reg1 = (u32)(reg >> 32);
	reg2 = (reg & 0xFFFFFFFF);

	pr_info("%s: addr:%8p, val[32-63]: 0x%08x, val[0-31]: 0x%08x\n",
		name, addr, reg1, reg2);
}
EXPORT_SYMBOL(print_reg64);

/***************debugfs start******************************/
static ssize_t cqm_ls_write(struct file *file, const char __user *buf,
			    size_t count, loff_t *ppos)
{
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

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0))
		PRINTK("Help: Please use: cat /sys/kernel/debug/cqm/cqmls\n");
	return count;
}

static int cqm_ls_seq_read(struct seq_file *s, void *v)
{
	void *ls_base = cqm_get_ls_base();
	unsigned int reg_r_data, q_len = 0, q_full = 0, q_empty = 0, q_cnt = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_printf(s, "\n%8s%8s,%8s,%8s,%8s\n",
		   "portno:", "qlen", "qfull", "qempty", "cntval");

	reg_r_data = cbm_r32(ls_base + LS_STATUS_PORT0);
	q_len = get_val(reg_r_data, LS_STATUS_PORT0_QUEUE_LEN_MASK,
			LS_STATUS_PORT0_QUEUE_LEN_POS);
	q_full = get_val(reg_r_data, LS_STATUS_PORT0_QUEUE_FULL_MASK,
			 LS_STATUS_PORT0_QUEUE_FULL_POS);
	q_empty = get_val(reg_r_data, LS_STATUS_PORT0_QUEUE_EMPTY_MASK,
			  LS_STATUS_PORT0_QUEUE_EMPTY_POS);
	q_cnt = get_val(reg_r_data, LS_STATUS_PORT0_CNT_VAL_MASK,
			LS_STATUS_PORT0_CNT_VAL_POS);
	seq_printf(s, " port0 :%08d,%08d,%08d,0x%08x\n", q_len, q_full,
		   q_empty, q_cnt);

	reg_r_data = cbm_r32(ls_base + LS_STATUS_PORT1);
	q_len = get_val(reg_r_data, LS_STATUS_PORT1_QUEUE_LEN_MASK,
			LS_STATUS_PORT1_QUEUE_LEN_POS);
	q_full = get_val(reg_r_data, LS_STATUS_PORT1_QUEUE_FULL_MASK,
			 LS_STATUS_PORT1_QUEUE_FULL_POS);
	q_empty = get_val(reg_r_data, LS_STATUS_PORT1_QUEUE_EMPTY_MASK,
			  LS_STATUS_PORT1_QUEUE_EMPTY_POS);
	q_cnt = get_val(reg_r_data, LS_STATUS_PORT1_CNT_VAL_MASK,
			LS_STATUS_PORT1_CNT_VAL_POS);
	seq_printf(s, " port1 :%08d,%08d,%08d,0x%08x\n", q_len, q_full,
		   q_empty, q_cnt);

	reg_r_data = cbm_r32(ls_base + LS_STATUS_PORT2);
	q_len = get_val(reg_r_data, LS_STATUS_PORT2_QUEUE_LEN_MASK,
			LS_STATUS_PORT2_QUEUE_LEN_POS);
	q_full = get_val(reg_r_data, LS_STATUS_PORT2_QUEUE_FULL_MASK,
			 LS_STATUS_PORT2_QUEUE_FULL_POS);
	q_empty = get_val(reg_r_data, LS_STATUS_PORT2_QUEUE_EMPTY_MASK,
			  LS_STATUS_PORT2_QUEUE_EMPTY_POS);
	q_cnt = get_val(reg_r_data, LS_STATUS_PORT2_CNT_VAL_MASK,
			LS_STATUS_PORT2_CNT_VAL_POS);
	seq_printf(s, " port2 :%08d,%08d,%08d,0x%08x\n", q_len, q_full,
		   q_empty, q_cnt);

	reg_r_data = cbm_r32(ls_base + LS_STATUS_PORT3);
	q_len = get_val(reg_r_data, LS_STATUS_PORT3_QUEUE_LEN_MASK,
			LS_STATUS_PORT3_QUEUE_LEN_POS);
	q_full = get_val(reg_r_data, LS_STATUS_PORT3_QUEUE_FULL_MASK,
			 LS_STATUS_PORT3_QUEUE_FULL_POS);
	q_empty = get_val(reg_r_data, LS_STATUS_PORT3_QUEUE_EMPTY_MASK,
			  LS_STATUS_PORT3_QUEUE_EMPTY_POS);
	q_cnt = get_val(reg_r_data, LS_STATUS_PORT3_CNT_VAL_MASK,
			LS_STATUS_PORT3_CNT_VAL_POS);
	seq_printf(s, " port3 :%08d,%08d,%08d,0x%08x\n", q_len, q_full,
		   q_empty, q_cnt);
	return 0;
}

static int cqm_ls_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_ls_seq_read, inode->i_private);
}

static const struct file_operations cqm_ls_fops = {
	.open = cqm_ls_open,
	.read = seq_read,
	.write = cqm_ls_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int cqm_dma_desc_read(struct seq_file *s, void *v)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_puts(s, "echo help > /sys/kernel/debug/cqm/cqm_dmadesc for usage\n");
	return 0;
}

static ssize_t cqm_dma_desc_write(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	u32 para1, para2;
	int len;
	void *dmadesc_base = cqm_get_dma_desc_base();
	u32 i, j, m, n;
	u64 reg64;
	u32 reg1, reg2, reg3, reg4;
	char str[64];
	char *p = (char *)str;
	char *param_list[5] = { 0 };
	int num = 0;
	int num_desc;
	void *baseaddr;
	void *addr, *addr1;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));
	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0)) {
		PRINTK("Example:\n");
		PRINTK("echo 0 port > /sys/kernel/debug/cqm/cqmdma_desc");
		PRINTK("to display DESC_IGP register\n");
		PRINTK("If port <= 15 or port >= 4, displays all reg values\n");
		PRINTK("echo 1 port > /sys/kernel/debug/cqm/cqmdma_desc\n");
		PRINTK("to display DESC_EGP register\n");
		PRINTK("If port <=26 or port >= 7, displays all reg values\n");
	} else if (num >= 1) {
		para1 = dp_atoi(param_list[0]);
		if (para1 > 4) {
			PRINTK("Error parameters. Please see help:");
			PRINTK("input:%s\n", param_list[0]);
		}
		switch (para1) {
		case 0:
			if (num != 2)
				return count;

			para2 = dp_atoi(param_list[1]);
			if ((para2 >= 4) && (para2 < 16)) {
				m = para2 - 4;
				n = m + 1;
			} else {
				PRINTK("Error in port number\n");
				return count;
			}
			for (i = m; i < n; i++) {
				for (j = 0; j < 8; j++) {
					PRINTK("Name: DESC0_%d_IGP_%02d\n",
					       j, para2);
					addr = dmadesc_base +
					       DESC0_0_IGP_4 +
					       (i * 0x1000) +
					       (j * 0x10);
					addr1 = dmadesc_base +
						DESC1_0_IGP_4 +
						(i * 0x1000) +
						(j * 0x10);
					PRINTK("Addr: %8p\n", addr);
					reg64 = REG64(addr);
					reg1 = (u32)(reg64 >> 32);
					reg2 = (reg64 & 0xFFFFFFFF);

					reg64 = REG64(addr1);
					reg3 = (u32)(reg64 >> 32);
					reg4 = (reg64 & 0xFFFFFFFF);
					PRINTK("Val:  0x%08x%08x\n",
					       reg1, reg2);
					PRINTK("Name: DESC1_%d_IGP_%02d\n",
					       j, para2);
					PRINTK("Addr: %8p\n",
					       addr1);
					reg64 = REG64(addr1);
					reg3 = (u32)(reg64 >> 32);
					reg4 = (reg64 & 0xFFFFFFFF);
					PRINTK("Val:  0x%08x%08x\n",
					       reg3, reg4);
				}
			}
		break;
		case 1:
			if (num != 2)
				return count;

			para2 = dp_atoi(param_list[1]);
			if (!((para2 >= 7) && (para2 <= 26))) {
				PRINTK("Error in port number\n");
				return count;
			}
			cqm_read_dma_desc_prep(para2, &baseaddr, &num_desc);

			for (i = 0; i < num_desc; i++) {
				PRINTK("Name: DESC0_0_EGP\n");
				addr = (void *)baseaddr  + ((i * 2) * 8);
				addr1 = (void *)baseaddr + (((i * 2) + 1) * 8);
				PRINTK("Addr: %8p\n", addr);
				reg64 = REG64(addr);
				reg1 = (u32)(reg64 >> 32);
				reg2 = (reg64 & 0xFFFFFFFF);

				reg64 = REG64(addr1);
				reg3 = (u32)(reg64 >> 32);
				reg4 = (reg64 & 0xFFFFFFFF);
				PRINTK("Val:  0x%08x%08x\n", reg1, reg2);
				PRINTK("Name: DESC1_0_EGP\n");
				PRINTK("Addr: %8p\n", addr1);
				PRINTK("Val:  0x%08x%08x\n", reg3, reg4);
			}
			cqm_read_dma_desc_end(para2);
		break;
		default:
		break;
		}
	} else {
		PRINTK("Wrong Param, see help\n");
	}
	return count;
}

static int cqm_dma_desc_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_dma_desc_read, inode->i_private);
}

static const struct file_operations cqm_dma_desc_fops = {
	.open = cqm_dma_desc_seq_open,
	.read = seq_read,
	.write = cqm_dma_desc_write,
	.llseek = seq_lseek,
	.release = seq_release,
};

static ssize_t cqm_ctrl_write(struct file *file, const char __user *buf,
			      size_t count, loff_t *ppos)
{
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

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0))
		PRINTK("Help: cat /sys/kernel/debug/cqm/cqmctrl\n");
	return count;
}

static int cqm_ctrl_seq_read(struct seq_file *s, void *v)
{
	void *ctrl_base = cqm_get_ctrl_base();
	u32 i;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_puts(s, "\n");
	for (i = 0; i < 8; i++) {
		seq_printf(s, "Name: CBM_IRNCR_%d CBM_IRNICR_%d CBM_IRNEN_%d\n",
			   i, i, i);
		seq_printf(s, "Addr: 0x%8p  0x%8p   0x%8p\n",
			   ctrl_base + CBM_IRNCR_0 + (i * 0x40),
			   ctrl_base + CBM_IRNICR_0 + (i * 0x40),
			   ctrl_base + CBM_IRNEN_0 + (i * 0x40));
		seq_printf(s, "Val:  0x%08x  0x%08x   0x%08x\n",
			   cbm_r32(ctrl_base + CBM_IRNCR_0 + (i * 0x40)),
			   cbm_r32(ctrl_base + CBM_IRNICR_0 + (i * 0x40)),
			   cbm_r32(ctrl_base + CBM_IRNEN_0 + (i * 0x40)));

		seq_printf(s, "Name: IGP_IRNCR_%d IGP_IRNICR_%d IGP_IRNEN_%d\n",
			   i, i, i);
		seq_printf(s, "Addr: 0x%8p  0x%8p   0x%8p\n",
			   ctrl_base + IGP_IRNCR_0 + (i * 0x40),
			   ctrl_base + IGP_IRNICR_0 + (i * 0x40),
			   ctrl_base + IGP_IRNEN_0 + (i * 0x40));
		seq_printf(s, "Val:  0x%08x  0x%08x   0x%08x\n",
			   cbm_r32(ctrl_base + IGP_IRNCR_0 + (i * 0x40)),
			   cbm_r32(ctrl_base + IGP_IRNICR_0 + (i * 0x40)),
			   cbm_r32(ctrl_base + IGP_IRNEN_0 + (i * 0x40)));

		seq_printf(s, "Name: EGP_IRNCR_%d EGP_IRNICR_%d EGP_IRNEN_%d\n",
			   i, i, i);
		seq_printf(s, "Addr: 0x%8p  0x%8p   0x%8p\n",
			   ctrl_base + EGP_IRNCR_0 + (i * 0x40),
			   ctrl_base + EGP_IRNICR_0 + (i * 0x40),
			   ctrl_base + EGP_IRNEN_0 + (i * 0x40));
		seq_printf(s, "Val:  0x%08x  0x%08x   0x%08x\n",
			   cbm_r32(ctrl_base + EGP_IRNCR_0 + (i * 0x40)),
			   cbm_r32(ctrl_base + EGP_IRNICR_0 + (i * 0x40)),
			   cbm_r32(ctrl_base + EGP_IRNEN_0 + (i * 0x40)));
	}
	seq_puts(s, "Name: VERSION_REG CBM_BUF_SIZE CBM_PB_BASE CBM_CTRL\n");
	seq_printf(s, "Addr: 0x%8p  0x%8p   0x%8p  0x%8p\n",
		   ctrl_base + CBM_VERSION_REG,
		   ctrl_base + CBM_BUF_SIZE,
		   ctrl_base + CBM_PB_BASE,
		   ctrl_base + CBM_CTRL);
	seq_printf(s, "Val:  0x%08x  0x%08x   0x%08x  0x%08x\n",
		   cbm_r32(ctrl_base + CBM_VERSION_REG),
		   cbm_r32(ctrl_base + CBM_BUF_SIZE),
		   cbm_r32(ctrl_base + CBM_PB_BASE),
		   cbm_r32(ctrl_base + CBM_CTRL));

	seq_puts(s, "Name: CBM_LL_DBG CBM_IP_VAL CBM_BSL_CTRL\n");
	seq_printf(s, "Addr: 0x%8p 0x%8p 0x%8p\n",
		   ctrl_base + CBM_LL_DBG,
		   ctrl_base + CBM_IP_VAL,
		   ctrl_base + CBM_BSL_CTRL);
	seq_printf(s, "Val:  0x%08x 0x%08x 0x%08x\n",
		   cbm_r32(ctrl_base + CBM_LL_DBG),
		   cbm_r32(ctrl_base + CBM_IP_VAL),
		   cbm_r32(ctrl_base + CBM_BSL_CTRL));
	seq_puts(s, "Name: CBM_QID_MODE_SEL_REG_0 CBM_QID_MODE_SEL_REG_1\n");
	seq_printf(s, "Addr: 0x%8p             0x%8p\n",
		   ctrl_base + CBM_QID_MODE_SEL_REG_0,
		   ctrl_base + CBM_QID_MODE_SEL_REG_0 + 4);
	seq_printf(s, "Val:  0x%08x             0x%08x\n",
		   cbm_r32(ctrl_base + CBM_QID_MODE_SEL_REG_0),
		   cbm_r32(ctrl_base + CBM_QID_MODE_SEL_REG_0 + 4));
	seq_puts(s, "Name: CBM_QID_MODE_SEL_REG_2 CBM_QID_MODE_SEL_REG_3\n");
	seq_printf(s, "Addr: 0x%8p             0x%8p\n",
		   ctrl_base + CBM_QID_MODE_SEL_REG_0 + 8,
		   ctrl_base + CBM_QID_MODE_SEL_REG_0 + 12);
	seq_printf(s, "Val:  0x%08x             0x%08x\n",
		   cbm_r32(ctrl_base + CBM_QID_MODE_SEL_REG_0 + 8),
		   cbm_r32(ctrl_base + CBM_QID_MODE_SEL_REG_0 + 12));

	seq_puts(s, "Name: CBM_BM_BASE CBM_WRED_BASE\n");
	seq_printf(s, "Addr: 0x%8p  0x%8p\n",
		   ctrl_base + CBM_BM_BASE,
		   ctrl_base + CBM_WRED_BASE);
	seq_printf(s, "Val:  0x%08x  0x%08x\n",
		   cbm_r32(ctrl_base + CBM_BM_BASE),
		   cbm_r32(ctrl_base + CBM_WRED_BASE));
	seq_puts(s, "Name: CBM_QPUSH_BASE CBM_TX_CREDIT_BASE\n");
	seq_printf(s, "Addr: 0x%8p     0x%8p\n",
		   ctrl_base + CBM_QPUSH_BASE,
		   ctrl_base + CBM_TX_CREDIT_BASE);
	seq_printf(s, "Val:  0x%08x     0x%08x\n",
		   cbm_r32(ctrl_base + CBM_QPUSH_BASE),
		   cbm_r32(ctrl_base + CBM_TX_CREDIT_BASE));
	seq_puts(s, "Name: CPU_POOL_ADDR CPU_POOL_NUM CPU_POOL_CNT\n");
	seq_printf(s, "Addr: 0x%8p    0x%8p   0x%8p\n",
		   ctrl_base + CBM_CPU_POOL_BUF_RTRN_START_ADDR,
		   ctrl_base + CBM_CPU_POOL_BUF_ALW_NUM,
		   ctrl_base + CBM_CPU_POOL_ENQ_CNT);
	seq_printf(s, "Val:  0x%08x    0x%08x   0x%08x\n",
		   cbm_r32(ctrl_base + CBM_CPU_POOL_BUF_RTRN_START_ADDR),
		   cbm_r32(ctrl_base + CBM_CPU_POOL_BUF_ALW_NUM),
		   cbm_r32(ctrl_base + CBM_CPU_POOL_ENQ_CNT));
	seq_puts(s, "Name: CPU_POOL_BUF     CPU_POOL_ENQ_DEC\n");
	seq_printf(s, "Addr: 0x%8p        0x%8p\n",
		   ctrl_base + CBM_CPU_POOL_BUF_RTRN_CNT,
		   ctrl_base + CBM_CPU_POOL_ENQ_DEC);
	seq_printf(s, "Val:  0x%08x        0x%08x\n",
		   cbm_r32(ctrl_base + CBM_CPU_POOL_BUF_RTRN_CNT),
		   cbm_r32(ctrl_base + CBM_CPU_POOL_ENQ_DEC));
	return 0;
}

static int cqm_ctrl_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_ctrl_seq_read, inode->i_private);
}

static const struct file_operations cqm_ctrl_fops = {
	.open = cqm_ctrl_open,
	.read = seq_read,
	.write = cqm_ctrl_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static inline void disp_deq_pon_reg(void *deq_base, u32 j)
{
	unsigned long tmp = 0x1A000 + ((j - 26) * 0x400);
	unsigned long tmp1 = j * 0x1000;
	unsigned long tmp2 = 0x19F00 + ((j - 26) * 0x400);
	unsigned long tmp4;
	unsigned long desc0 = DESC0_0_CPU_EGP_0;
	int i;

	PRINTK("Name: CFG_PON_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + CFG_CPU_EGP_0 + tmp);
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + CFG_CPU_EGP_0 + tmp));
	PRINTK("Name: IRNCR_PON_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNCR_CPU_EGP_0 + tmp);
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNCR_CPU_EGP_0 + tmp));
	PRINTK("Name: IRNICR_PON_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNICR_CPU_EGP_0 + tmp);
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNICR_CPU_EGP_0 + tmp));
	PRINTK("Name: IRNEN_PON_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNEN_CPU_EGP_0 + tmp);
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNEN_CPU_EGP_0 + tmp));

	if (j == 26) {
		PRINTK("Name: DPTR_PON_EGP_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + DPTR_CPU_EGP_0 + (tmp1));
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + DPTR_CPU_EGP_0 + (tmp1)));
		PRINTK("Name: BPRC_PON_EGP_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + BPRC_CPU_EGP_0 + (tmp1));
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + BPRC_CPU_EGP_0 + (tmp1)));
		PRINTK("Name: PTR_RTN_PON_DW2_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + PTR_RTN_CPU_DW2_EGP_0 - 0x80 + (tmp1));
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + PTR_RTN_CPU_DW2_EGP_0 - 0x80
		       + (tmp1)));
		PRINTK("Name: PTR_RTN_PON_DW3_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + PTR_RTN_CPU_DW3_EGP_0 - 0x80 + (tmp1));
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + PTR_RTN_CPU_DW3_EGP_0 - 0x80
		       + (tmp1)));
	} else {
		PRINTK("Name: DPTR_PON_EGP_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + DPTR_CPU_EGP_0 + tmp);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + DPTR_CPU_EGP_0 + tmp));
	}

	for (i = 0; i < 8; i++) {
		tmp4 = i * 16;
		PRINTK("Name: DESC0_%d_PON_EGP_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + (tmp4) + tmp2));
		PRINTK("Name: DESC1_%d_PON_EGP_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 4 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 4 + (tmp4) + tmp2));
		PRINTK("Name: DESC2_%d_PON_EGP_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 8 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 8 + (tmp4) + tmp2));
		PRINTK("Name: DESC3_%d_PON_EGP_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 12 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 12 + (tmp4) + tmp2));
	}

	for (i = 0; i < 8; i++) {
		tmp4 = i * 16;
		PRINTK("Name: DESC0_%d_PON_EGP_S_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 0x100 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 0x100 + (tmp4) + tmp2));
		PRINTK("Name: DESC1_%d_PON_EGP_S_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 0x100 + 4 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 0x100 + 4 + (tmp4) + tmp2));
		PRINTK("Name: DESC2_%d_PON_EGP_S_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 0x100 + 8 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 0x100 + 8 + (tmp4) + tmp2));
		PRINTK("Name: DESC3_%d_PON_EGP_S_%02d\n", i, j);
		PRINTK("Addr: 0x%8p\n",
		       deq_base + desc0 + 0x100 + 12 + (tmp4) + tmp2);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + desc0 + 0x100 + 12 + (tmp4) + tmp2));
	}
}

static inline void disp_deq_dma_reg(void *deq_base, u32 j)
{
	PRINTK("Name: CFG_DMA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + CFG_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + CFG_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DQPC_DMA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DQPC_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DQPC_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNCR_DMA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNCR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNCR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNICR_DMA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNICR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNICR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNEN_DMA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNEN_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNEN_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DPTR_DMA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DPTR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DPTR_CPU_EGP_0 + (j * 0x1000)));
}

static inline void disp_deq_aca_reg(void *deq_base, u32 j)
{
	PRINTK("Name: CFG_ACA_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + CFG_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + CFG_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DQPC_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DQPC_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DQPC_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNCR_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNCR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNCR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNICR_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNICR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNICR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNEN_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNEN_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNEN_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DPTR_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DPTR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DPTR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: BPRC_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + BPRC_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + BPRC_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: BRPTR_SCPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + BPRC_CPU_EGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + BPRC_CPU_EGP_0 + 4 + (j * 0x1000)));
}

static inline void disp_deq_reg(void *deq_base, u32 j)
{
	PRINTK("Name: CFG_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + CFG_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + CFG_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DQPC_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DQPC_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DQPC_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNCR_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNCR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNCR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNICR_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNICR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNICR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: IRNEN_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + IRNEN_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + IRNEN_CPU_EGP_0 + (j * 0x1000)));

	PRINTK("Name: DPTR_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DPTR_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DPTR_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: BPRC_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + BPRC_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + BPRC_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: PTR_RTN_CPU_DW2_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + PTR_RTN_CPU_DW2_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + PTR_RTN_CPU_DW2_EGP_0 + (j * 0x1000)));
	PRINTK("Name: PTR_RTN_CPU_DW3_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + PTR_RTN_CPU_DW3_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + PTR_RTN_CPU_DW3_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DESC0_0_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_0_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_0_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DESC1_0_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_0_CPU_EGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_0_CPU_EGP_0 + 4 + (j * 0x1000)));
	PRINTK("Name: DESC2_0_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_0_CPU_EGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_0_CPU_EGP_0 + 8 + (j * 0x1000)));
	PRINTK("Name: DESC3_0_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_0_CPU_EGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_0_CPU_EGP_0 + 12 + (j * 0x1000)));

	PRINTK("Name: DESC0_1_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_1_CPU_EGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_1_CPU_EGP_0 + (j * 0x1000)));
	PRINTK("Name: DESC1_1_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_1_CPU_EGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_1_CPU_EGP_0 + 4 + (j * 0x1000)));
	PRINTK("Name: DESC2_1_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_1_CPU_EGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_1_CPU_EGP_0 + 8 + (j * 0x1000)));
	PRINTK("Name: DESC3_1_CPU_EGP_%02d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       deq_base + DESC0_1_CPU_EGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(deq_base + DESC0_1_CPU_EGP_0 + 12 + (j * 0x1000)));
}

static ssize_t cqm_deq_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *ppos)
{
	u32 index;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[5] = { 0 };
	int num = 0;
	void *deq_base = cqm_get_deq_base();
	u32 j, m, n;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));
	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0)) {
		PRINTK("echo port_num > /sys/kernel/debug/cqm/cqmdeq\n");
		PRINTK("If port < 90, it will display that port registers.\n");
		PRINTK("Port 0-3, CPU; 4~6, ACA; 7~25, DMA; 26~89, PON.\n");
	} else if (num == 1) {
		index = dp_atoi(param_list[0]);
		PRINTK("Name: CBM_DQM_CTRL\n");
		PRINTK("Addr: 0x%8p\n",
		       deq_base + CBM_DQM_CTRL);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(deq_base + CBM_DQM_CTRL));

		if (index >= 90) {
			PRINTK("Wrong Param(try): see help\n");
			return count;
		}
		m = index;
		n = index + 1;

		for (j = m; j < n; j++) {
			if (j < 4)
				disp_deq_reg(deq_base, j);
			else if ((j >= 4) && (j <= 6))
				disp_deq_aca_reg(deq_base, j);
			else if ((j >= 7) && (j <= 25))
				disp_deq_dma_reg(deq_base, j);
			else
				disp_deq_pon_reg(deq_base, j);
		}
	} else {
		PRINTK("Wrong Param(try): see help");
	}
	return count;
}

static int cqm_deq_seq_read(struct seq_file *s, void *v)
{
	void *deq_base = cqm_get_deq_base();

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_puts(s, "Name: CBM_DQM_CTRL\n");
	seq_printf(s, "Addr: 0x%8p\n", deq_base + CBM_DQM_CTRL);
	seq_printf(s, "Val:  0x%08x\n", cbm_r32(deq_base + CBM_EQM_CTRL));
	return 0;
}

static int cqm_deq_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_deq_seq_read, inode->i_private);
}

static const struct file_operations cqm_deq_fops = {
	.open = cqm_deq_seq_open,
	.read = seq_read,
	.write = cqm_deq_write,
	.llseek = seq_lseek,
	.release = seq_release,
};

static inline void disp_enq_help(void)
{
	PRINTK("To display CQM enq register try\n");
	PRINTK("echo port_num > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq ctrl registers try\n");
	PRINTK("echo ctrl > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq den registers try\n");
	PRINTK("echo den > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq ovh registers try\n");
	PRINTK("echo ovh > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq threshold enable registers try\n");
	PRINTK("echo thres_en > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq IP occupancy registers try\n");
	PRINTK("echo ip_occ > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq HIGH watermark registers try\n");
	PRINTK("echo hwm > /sys/kernel/debug/cqm/cqmenq\n");
	PRINTK("To display CQM enq HIGH watermark registers try\n");
	PRINTK("echo lwm > /sys/kernel/debug/cqm/cqmenq\n");
}

static inline void disp_enq_thres(void *enq_base)
{
	PRINTK("Name: DMA_RDY_EN EP_THRSD_EN_0 EP_THRSD_EN_1 EP_THRSD_EN_2\n");
	PRINTK("Addr: 0x%8p 0x%8p    0x%8p    0x%8p\n",
	       enq_base + DMA_RDY_EN,
	       enq_base + EP_THRSD_EN_0,
	       enq_base + EP_THRSD_EN_0 + 4,
	       enq_base + EP_THRSD_EN_0 + 8);
	PRINTK("Val:  0x%08x 0x%08x    0x%08x    0x%08x\n",
	       cbm_r32(enq_base + DMA_RDY_EN),
	       cbm_r32(enq_base + EP_THRSD_EN_0),
	       cbm_r32(enq_base + EP_THRSD_EN_0 + 4),
	       cbm_r32(enq_base + EP_THRSD_EN_0 + 8));
	PRINTK("Name: EP_THRSD_EN_3 IP_OCC_EN   IP_THRSD_EN IP_BP_STATUS\n");
	PRINTK("Addr: 0x%8p    0x%8p  0x%8p  0x%8p\n",
	       enq_base + EP_THRSD_EN_0 + 12,
	       enq_base + IP_OCC_EN,
	       enq_base + IP_THRSD_EN,
	       enq_base + IP_BP_STATUS);
	PRINTK("Val:  0x%08x    0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + EP_THRSD_EN_0 + 12),
	       cbm_r32(enq_base + IP_OCC_EN),
	       cbm_r32(enq_base + IP_THRSD_EN),
	       cbm_r32(enq_base + IP_BP_STATUS));
}

static inline void disp_enq_ip_occ(void *enq_base)
{
	enq_base += IP_OCC_0;
	PRINTK("Name: IP_OCC_0   IP_OCC_1   IP_OCC_2   IP_OCC_3\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base,
	       enq_base + 4,
	       enq_base + 8,
	       enq_base + 12);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base),
	       cbm_r32(enq_base + 4),
	       cbm_r32(enq_base + 8),
	       cbm_r32(enq_base + 12));

	PRINTK("Name: IP_OCC_4   IP_OCC_5   IP_OCC_6   IP_OCC_7\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 16,
	       enq_base + 20,
	       enq_base + 24,
	       enq_base + 28);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 16),
	       cbm_r32(enq_base + 20),
	       cbm_r32(enq_base + 24),
	       cbm_r32(enq_base + 28));

	PRINTK("Name: IP_OCC_8   IP_OCC_9   IP_OCC_10  IP_OCC_11\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 32,
	       enq_base + 36,
	       enq_base + 40,
	       enq_base + 44);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 32),
	       cbm_r32(enq_base + 36),
	       cbm_r32(enq_base + 40),
	       cbm_r32(enq_base + 44));
	PRINTK("Name: IP_OCC_12  IP_OCC_13  IP_OCC_14  IP_OCC_15\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 48,
	       enq_base + 52,
	       enq_base + 56,
	       enq_base + 60);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 48),
	       cbm_r32(enq_base + 52),
	       cbm_r32(enq_base + 56),
	       cbm_r32(enq_base + 60));
}

static inline void disp_enq_HWM(void *enq_base)
{
	enq_base += IP_THRESHOLD_HWM_0;
	PRINTK("Name: IP_TH_HWM0  IP_TH_HWM1  IP_TH_HWM2  IP_TH_HWM3\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base,
	       enq_base + 4,
	       enq_base + 8,
	       enq_base + 12);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base),
	       cbm_r32(enq_base + 4),
	       cbm_r32(enq_base + 8),
	       cbm_r32(enq_base + 12));
	PRINTK("Name: IP_TH_HWM4  IP_TH_HWM5  IP_TH_HWM6  IP_TH_HWM7\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base + 16,
	       enq_base + 20,
	       enq_base + 24,
	       enq_base + 28);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + 16),
	       cbm_r32(enq_base + 20),
	       cbm_r32(enq_base + 24),
	       cbm_r32(enq_base + 28));
	PRINTK("Name: IP_TH_HWM8  IP_TH_HWM9  IP_TH_HWM10 IP_TH_HWM11\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base + 32,
	       enq_base + 36,
	       enq_base + 40,
	       enq_base + 44);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + 32),
	       cbm_r32(enq_base + 36),
	       cbm_r32(enq_base + 40),
	       cbm_r32(enq_base + 44));
	PRINTK("Name: IP_TH_HWM12 IP_TH_HWM13 IP_TH_HWM14 IP_TH_HWM15\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base + 48,
	       enq_base + 52,
	       enq_base + 56,
	       enq_base + 60);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + 48),
	       cbm_r32(enq_base + 52),
	       cbm_r32(enq_base + 56),
	       cbm_r32(enq_base + 60));
}

static inline void disp_enq_LWM(void *enq_base)
{
	enq_base += IP_THRESHOLD_LWM_0;
	PRINTK("Name: IP_TH_LWM0  IP_TH_LWM1  IP_TH_LWM2  IP_TH_LWM3\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base,
	       enq_base + 4,
	       enq_base + 8,
	       enq_base + 12);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base),
	       cbm_r32(enq_base + 4),
	       cbm_r32(enq_base + 8),
	       cbm_r32(enq_base + 12));
	PRINTK("Name: IP_TH_LWM4  IP_TH_LWM5  IP_TH_LWM6  IP_TH_LWM7\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base + 16,
	       enq_base + 20,
	       enq_base + 24,
	       enq_base + 28);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + 16),
	       cbm_r32(enq_base + 20),
	       cbm_r32(enq_base + 24),
	       cbm_r32(enq_base + 28));
	PRINTK("Name: IP_TH_LWM8  IP_TH_LWM9  IP_TH_LWM10 IP_TH_LWM11\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p  0x%8p\n",
	       enq_base + 32,
	       enq_base + 36,
	       enq_base + 40,
	       enq_base + 44);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + 32),
	       cbm_r32(enq_base + 36),
	       cbm_r32(enq_base + 40),
	       cbm_r32(enq_base + 44));
	PRINTK("Name: IP_TH_LWM12 IP_TH_LWM13 IP_TH_LWM14 IP_TH_LWM15\n");
	PRINTK("Addr: 0x%8p  0x%8p  0x%8p 0x%8p\n",
	       enq_base + 48,
	       enq_base + 52,
	       enq_base + 56,
	       enq_base + 60);
	PRINTK("Val:  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       cbm_r32(enq_base + 48),
	       cbm_r32(enq_base + 52),
	       cbm_r32(enq_base + 56),
	       cbm_r32(enq_base + 60));
}

static inline void disp_enq_delay(void *enq_base)
{
	enq_base += DEN_0;
	PRINTK("Name:DEN_0      DEN_1      DEN_2      DEN_3\n");
	PRINTK("Addr:0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base,
	       enq_base + 4,
	       enq_base + 8,
	       enq_base + 12);
	PRINTK("Val:0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base),
	       cbm_r32(enq_base + 4),
	       cbm_r32(enq_base + 8),
	       cbm_r32(enq_base + 12));
	PRINTK("Name:DEN_4      DEN_5      DEN_6      DEN_7\n");
	PRINTK("Addr:0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 16,
	       enq_base + 20,
	       enq_base + 24,
	       enq_base + 28);
	PRINTK("Val:0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 16),
	       cbm_r32(enq_base + 20),
	       cbm_r32(enq_base + 24),
	       cbm_r32(enq_base + 28));
}

static inline void disp_enq_ovh(void *enq_base)
{
	enq_base += OVH_0;
	PRINTK("Name: OVH_0      OVH_1      OVH_2      OVH_3\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base,
	       enq_base + 4,
	       enq_base + 8,
	       enq_base + 12);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base),
	       cbm_r32(enq_base + 4),
	       cbm_r32(enq_base + 8),
	       cbm_r32(enq_base + 12));
	PRINTK("Name:OVH_4      OVH_5      OVH_6      OVH_7\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 16,
	       enq_base + 20,
	       enq_base + 24,
	       enq_base + 28);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 16),
	       cbm_r32(enq_base + 20),
	       cbm_r32(enq_base + 24),
	       cbm_r32(enq_base + 28));
	PRINTK("Name: OVH_8      OVH_9      OVH_10     OVH_11\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 32,
	       enq_base + 36,
	       enq_base + 40,
	       enq_base + 44);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 32),
	       cbm_r32(enq_base + 36),
	       cbm_r32(enq_base + 40),
	       cbm_r32(enq_base + 44));
	PRINTK("Name: OVH_12     OVH_13     OVH_14     OVH_15\n");
	PRINTK("Addr: 0x%8p 0x%8p 0x%8p 0x%8p\n",
	       enq_base + 48,
	       enq_base + 52,
	       enq_base + 56,
	       enq_base + 60);
	PRINTK("Val:  0x%08x 0x%08x 0x%08x 0x%08x\n",
	       cbm_r32(enq_base + 48),
	       cbm_r32(enq_base + 52),
	       cbm_r32(enq_base + 56),
	       cbm_r32(enq_base + 60));
}

static inline void disp_enq_reg(void *enq_base, int j)
{
	PRINTK("Name: CFG_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + CFG_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + CFG_CPU_IGP_0 + (j * 0x1000)));
	PRINTK("Name: EQPC_CPU_IGP__%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + EQPC_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + EQPC_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC0_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC1_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + 4 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC2_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + 8 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC3_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + 12 + (j * 0x1000)));

	PRINTK("Name: IRNCR_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + IRNCR_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + IRNCR_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: IRNICR_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + IRNICR_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + IRNICR_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: IRNEN_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + IRNEN_CPU_IGP_0 + (j * 0x1000));

	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + IRNEN_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DICC_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DICC_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DICC_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DROP_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DROP_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DROP_IGP_0 + (j * 0x1000)));

	PRINTK("Name: NPBPC_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + NPBPC_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + NPBPC_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: NS0PC_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + NS0PC_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + NS0PC_CPU_IGP_0 + (j * 0x1000)));
	PRINTK("Name: NS1PC_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + NS0PC_CPU_IGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + NS0PC_CPU_IGP_0 + 4 + (j * 0x1000)));
	PRINTK("Name: NS2PC_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + NS0PC_CPU_IGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + NS0PC_CPU_IGP_0 + 8 + (j * 0x1000)));
	PRINTK("Name: NS3PC_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + NS0PC_CPU_IGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + NS0PC_CPU_IGP_0 + 12 + (j * 0x1000)));

	PRINTK("Name: DCNTR_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n", enq_base + DCNTR_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DCNTR_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DESC0_0_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_0_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_0_CPU_IGP_0 + (j * 0x1000)));
	PRINTK("Name: DESC1_0_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_0_CPU_IGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_0_CPU_IGP_0 + 4 + (j * 0x1000)));
	PRINTK("Name: DESC2_0_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_0_CPU_IGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_0_CPU_IGP_0 + 8 + (j * 0x1000)));
	PRINTK("Name: DESC3_0_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_0_CPU_IGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_0_CPU_IGP_0 + 12 + (j * 0x1000)));

	PRINTK("Name: DESC0_1_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_1_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_1_CPU_IGP_0 + (j * 0x1000)));
	PRINTK("Name: DESC1_1_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_1_CPU_IGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_1_CPU_IGP_0 + 4 + (j * 0x1000)));
	PRINTK("Name: DESC2_1_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_1_CPU_IGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_1_CPU_IGP_0 + 8 + (j * 0x1000)));
	PRINTK("Name: DESC3_1_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DESC0_1_CPU_IGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DESC0_1_CPU_IGP_0 + 12 + (j * 0x1000)));
}

static inline void disp_enq_dma_reg(void *enq_base, int j)
{
	PRINTK("Name: CFG_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + CFG_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + CFG_CPU_IGP_0 + (j * 0x1000)));
	PRINTK("Name: EQPC_DMA_IGP__%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + EQPC_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + EQPC_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC0_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC1_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + 4 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC2_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + 8 + (j * 0x1000)));

	PRINTK("Name: DISC_DESC3_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DISC_DESC0_CPU_IGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DISC_DESC0_CPU_IGP_0 + 12 + (j * 0x1000)));

	PRINTK("Name: IRNCR_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + IRNCR_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + IRNCR_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: IRNICR_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + IRNICR_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + IRNICR_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: IRNEN_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + IRNEN_CPU_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + IRNEN_CPU_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DPTR_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + 0x10030 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + 0x10030 + (j * 0x1000)));

	PRINTK("Name: DICC_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DICC_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DICC_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DROP_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DROP_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DROP_IGP_0 + (j * 0x1000)));

	PRINTK("Name: DROP_DESC0_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DROP_IGP_0 + 4 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DROP_IGP_0 + 4 + (j * 0x1000)));
	PRINTK("Name: DROP_DESC1_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DROP_IGP_0 + 8 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DROP_IGP_0 + 8 + (j * 0x1000)));
	PRINTK("Name: DROP_DESC2_CPU_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DROP_IGP_0 + 12 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DROP_IGP_0 + 12 + (j * 0x1000)));
	PRINTK("Name: DROP_DESC3_DMA_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n",
	       enq_base + DROP_IGP_0 + 16 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DROP_IGP_0 + 16 + (j * 0x1000)));
	PRINTK("Name: DCNTR_IGP_%d\n", j);
	PRINTK("Addr: 0x%8p\n", enq_base + DCNTR_IGP_0 + (j * 0x1000));
	PRINTK("Val:  0x%08x\n",
	       cbm_r32(enq_base + DCNTR_IGP_0 + (j * 0x1000)));
}

static ssize_t cqm_enq_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *ppos)
{
	u32 index;
	int len;
	char str[64];
	char *p = (char *)str;
	char *param_list[5] = { 0 };
	int num = 0;
	void *enq_base = cqm_get_enq_base();

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (!len)
		return count;

	num = dp_split_buffer(p, param_list, ARRAY_SIZE(param_list));

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0)) {
		disp_enq_help();
	} else if ((num == 1) && (strncmp(param_list[0], "ctrl", 4) == 0)) {
		PRINTK("\nCQEM_ENQ_BASE=0x%p\n", enq_base);
		PRINTK("Name: CBM_EQM_CTRL\n");
		PRINTK("Addr: 0x%8p\n",
		       enq_base + CBM_EQM_CTRL);
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(enq_base + CBM_EQM_CTRL));
	} else if ((num == 1) && (strncmp(param_list[0], "den", 3) == 0)) {
		disp_enq_delay(enq_base);
	} else if ((num == 1) && (strncmp(param_list[0], "ovh", 3) == 0)) {
		disp_enq_ovh(enq_base);
	} else if ((num == 1) && (strncmp(param_list[0], "thres_en", 8) == 0)) {
		disp_enq_thres(enq_base);
	} else if ((num == 1) && (strncmp(param_list[0], "ip_occ", 6) == 0)) {
		disp_enq_ip_occ(enq_base);
	} else if ((num == 1) && (strncmp(param_list[0], "hwm", 3) == 0)) {
		disp_enq_HWM(enq_base);
	} else if ((num == 1) && (strncmp(param_list[0], "lwm", 3) == 0)) {
		disp_enq_LWM(enq_base);
	} else if (num == 1) {
		index = dp_atoi(param_list[0]);
		if (index >= 16) {
			PRINTK("Error in input\n");
			return count;
		}

		if (index < 4)
			disp_enq_reg(enq_base, index);
		else
			disp_enq_dma_reg(enq_base, index);

	} else {
		PRINTK("Wrong Param(try):\n");
		PRINTK("echo port > /sys/kernel/debug/cqm/cqmenq\n");
	}
	return count;
}

static int cqm_enq_seq_read(struct seq_file *s, void *v)
{
	void *enq_base = cqm_get_enq_base();

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_puts(s, "Name: CBM_EQM_CTRL\n");
	seq_printf(s, "Addr: 0x%8p\n", enq_base + CBM_EQM_CTRL);
	seq_printf(s, "Val:  0x%08x\n", cbm_r32(enq_base + CBM_EQM_CTRL));
	return 0;
}

static int cqm_enq_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_enq_seq_read, inode->i_private);
}

static const struct file_operations cqm_enq_fops = {
	.open = cqm_enq_open,
	.read = seq_read,
	.write = cqm_enq_write,
	.llseek = seq_lseek,
	.release = seq_release,
};

static ssize_t cqm_dqpc_write(struct file *file, const char __user *buf,
			      size_t count, loff_t *ppos)
{
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

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0))
		PRINTK("cat /sys/kernel/debug/cqm/cqmdqpc\n");
	return count;
}

static int cqm_dqpc_seq_read(struct seq_file *s, void *v)
{
	void *deq_base = cqm_get_deq_base();
	u32 j;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	deq_base += DQPC_CPU_EGP_0;
	for (j = 0; j < 3; j++) {
		seq_printf(s, "Name: DQPC_CPU_EGP__%02d  DQPC_CPU_EGP__%02d\n",
			   2 * j, (2 * j) + 1);
		seq_printf(s, "Addr: 0x%8p        0x%8p\n",
			   deq_base + ((2 * j) * 0x1000),
			   deq_base + (((2 * j) + 1) * 0x1000));
		seq_printf(s, "Val:  0x%08x        0x%08x\n",
			   cbm_r32(deq_base + ((2 * j) * 0x1000)),
			   cbm_r32(deq_base + (((2 * j) + 1) * 0x1000)));
	}
	seq_puts(s, "Name: DQPC_CPU_EGP__06  DQPC_DMA_EGP__07\n");
	seq_printf(s, "Addr: 0x%8p        0x%8p\n",
		   deq_base + (6 * 0x1000),
		   deq_base + (7 * 0x1000));
	seq_printf(s, "Val:  0x%08x        0x%08x\n",
		   cbm_r32(deq_base + (6 * 0x1000)),
		   cbm_r32(deq_base + (7 * 0x1000)));
	for (j = 4; j < 13; j++) {
		seq_printf(s, "Name: DQPC_DMA_EGP__%02d  DQPC_DMA_EGP__%02d\n",
			   2 * j, (2 * j) + 1);
		seq_printf(s, "Addr: 0x%8p        0x%8p\n",
			   deq_base + ((2 * j) * 0x1000),
			   deq_base + (((2 * j) + 1) * 0x1000));
		seq_printf(s, "Val:  0x%08x        0x%08x\n",
			   cbm_r32(deq_base + ((2 * j) * 0x1000)),
			   cbm_r32(deq_base + (((2 * j) + 1) * 0x1000)));
	}
	return 0;
}

static int cqm_dqpc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_dqpc_seq_read, inode->i_private);
}

static const struct file_operations cqm_dqpc_fops = {
	.open = cqm_dqpc_open,
	.read = seq_read,
	.write = cqm_dqpc_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static ssize_t cqm_eqpc_write(struct file *file, const char __user *buf,
			      size_t count, loff_t *ppos)
{
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

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0))
		PRINTK("Help: cat /sys/kernel/debug/cqm/cqmeqpc\n");
	return count;
}

static int cqm_eqpc_seq_read(struct seq_file *s, void *v)
{
	void *enq_base = cqm_get_enq_base();
	u32 j;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	enq_base += EQPC_CPU_IGP_0;
	for (j = 0; j < 2; j++) {
		seq_printf(s, "Name: EQPC_CPU_IGP__%02d  EQPC_CPU_IGP__%02d\n",
			   2 * j, (2 * j) + 1);
		seq_printf(s, "Addr: 0x%8p        0x%8p\n",
			   enq_base + ((2 * j) * 0x1000),
			   enq_base + (((2 * j) + 1) * 0x1000));
		seq_printf(s, "Val:  0x%08x        0x%08x\n",
			   cbm_r32(enq_base + ((2 * j) * 0x1000)),
			   cbm_r32(enq_base + (((2 * j) + 1) * 0x1000)));
	}
	for (j = 2; j < 8; j++) {
		seq_printf(s, "Name: EQPC_DMA_IGP__%02d  EQPC_DMA_IGP__%02d\n",
			   2 * j, (2 * j) + 1);
		seq_printf(s, "Addr: 0x%8p        0x%8p\n",
			   enq_base + ((2 * j) * 0x1000),
			   enq_base + (((2 * j) + 1) * 0x1000));
		seq_printf(s, "Val:  0x%08x        0x%08x\n",
			   cbm_r32(enq_base + ((2 * j) * 0x1000)),
			   cbm_r32(enq_base + (((2 * j) + 1) * 0x1000)));
	}
	return 0;
}

static int cqm_eqpc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_eqpc_seq_read, inode->i_private);
}

static const struct file_operations cqm_eqpc_fops = {
	.open = cqm_eqpc_open,
	.read = seq_read,
	.write = cqm_eqpc_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static ssize_t cqm_ofsc_write(struct file *file, const char __user *buf,
			      size_t count, loff_t *ppos)
{
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

	if ((num == 1) && (strncmp(param_list[0], "help", 4) == 0)) {
		PRINTK("Help: Please use:\n");
		PRINTK("cat /sys/kernel/debug/cqm/ofsc\n");
	}
	return count;
}

static int cqm_ofsc_read(struct seq_file *s, void *v)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_printf(s, "%8s\n", "FSC");
	seq_printf(s, "0x%08x\n", get_fsqm_ofsc());
	return 0;
}

static int cqm_ofsc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_ofsc_read, inode->i_private);
}

static const struct file_operations cqm_ofsc_fops = {
	.open = cqm_ofsc_open,
	.read = seq_read,
	.write = cqm_ofsc_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int cqm_ofsq_read(struct seq_file *s, void *v)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_printf(s, "0x%08x\n", fsqm_check(0));
	return 0;
}

static int cqm_ofsq_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_ofsq_read, inode->i_private);
}

static const struct file_operations cqm_ofsq_fops = {
	.open = cqm_ofsq_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

int cqm_debugfs_init(struct cqm_ctrl *pctrl)
{
	char cqm_dir[64] = {0};
	struct dentry *file;

	strlcpy(cqm_dir, pctrl->name, sizeof(cqm_dir));
	pctrl->debugfs = debugfs_create_dir(cqm_dir, NULL);
	if (!pctrl->debugfs)
		return -ENOMEM;

	file = debugfs_create_file("cqmeqpc", 0400, pctrl->debugfs,
				   pctrl, &cqm_eqpc_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("cqmdqpc", 0400, pctrl->debugfs,
				   pctrl, &cqm_dqpc_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("cqmenq", 0400, pctrl->debugfs,
				   pctrl, &cqm_enq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("cqmdeq", 0400, pctrl->debugfs,
				   pctrl, &cqm_deq_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("cqmdma_desc", 0400,
				   pctrl->debugfs, pctrl, &cqm_dma_desc_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("cqmctrl", 0400, pctrl->debugfs,
				   pctrl, &cqm_ctrl_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("cqmls", 0400, pctrl->debugfs,
				   pctrl, &cqm_ls_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("ofsc", 0400, pctrl->debugfs,
				   pctrl, &cqm_ofsc_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("check_fsqm", 0400, pctrl->debugfs,
				   pctrl, &cqm_ofsq_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("pkt_count", 0400, pctrl->debugfs,
				   pctrl, &cqm_dbg_cntrs_fops);
	if (!file)
		goto err;
	return 0;
err:
	debugfs_remove_recursive(pctrl->debugfs);
	return -ENOMEM;
}
EXPORT_SYMBOL(cqm_debugfs_init);

/****************debugfs end ***************************/
