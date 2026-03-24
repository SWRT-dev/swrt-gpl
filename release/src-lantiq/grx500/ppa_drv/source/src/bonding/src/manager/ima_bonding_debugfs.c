
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>

#include "common/ima_common.h"
#include "atm/ima_atm_rx.h"
#include "atm/ima_tx_module.h"
#include "atm/ima_tx_cell_buffer.h"
#include "asm/ima_asm.h"

#define TXPRIV (&g_tx_privdata)
#define RXPRIV (&g_privdata)
#define ASMPRIV (&g_asm_privdata)
#define TXCBPRIV (&g_privTxCBData)

#define IMA_BOND_DBGFS_NAME "ima_bond"
#define IMA_BOND_DBGFS_PATH "/sys/kernel/debug/ima_bond"

struct ima_bond_dbgfs_list {
	char *name;
	umode_t mode;
	const struct file_operations *fops;
};

struct dentry *g_dbgfs_node;

int dbgfs_write_dbg(struct file *file, const char __user *buff, size_t count, loff_t *offset)
{
	unsigned char input[16] = {0};

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	if (count == 0)
		return -EINVAL;

	memset(input, 0, sizeof(input));
	simple_write_to_buffer(input, sizeof(input) - 1, offset, buff, count - 1);

#ifdef TX_PRINT
	if (strcmp(input, "enable tx") == 0)
		TXPRIV->dbg = 1;
	else if (strcmp(input, "disable tx") == 0)
		TXPRIV->dbg = 0;
	else {
		pr_info("Usage:\n");
		pr_info("	echo <enable|disable> tx > %s/dbg\n", IMA_BOND_DBGFS_PATH);
	}
#endif

	return count;
}

int dbgfs_read_dbg(struct seq_file *seq, void *v)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

#ifdef TX_PRINT
	seq_printf(seq, "Tx enabled : %d\n", TXPRIV->dbg);
#endif

	return 0;
}

static int dbgfs_read_dbg_seq_open(struct inode *inode,
				     struct file *file)
{
	return single_open(file, dbgfs_read_dbg, NULL);
}

static const struct file_operations dbg_ops = {
	.owner		= THIS_MODULE,
	.open		= dbgfs_read_dbg_seq_open,
	.read		= seq_read,
	.write		= dbgfs_write_dbg,
	.llseek		= seq_lseek,
	.release	= single_release,
};

int dbgfs_write_tx(struct file *file, const char __user *buff, size_t count, loff_t *offset)
{
	unsigned char input[64] = {0};
	char msg[256] = "";

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	if (count == 0)
		return -EINVAL;

	memset(input, 0, sizeof(input));
	simple_write_to_buffer(input, sizeof(input) - 1, offset, buff, count - 1);

	if (set_tx_dbgfs_config(input, msg, sizeof(msg))) {
		return count;
	}

	pr_info( "Bonding driver: Usage:\n" );
	pr_info( "Bonding driver:     echo %s > %s/tx\n", msg, IMA_BOND_DBGFS_PATH);

	return count;
}

int dbgfs_read_tx(struct seq_file *seq, void *v)
{
	char msg[512] = "";

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	get_tx_dbgfs_config(msg, sizeof(msg));
	seq_printf(seq, "%s\n", msg);

	return 0;
}

static int dbgfs_read_tx_seq_open(struct inode *inode,
				     struct file *file)
{
	return single_open(file, dbgfs_read_tx, NULL);
}

static const struct file_operations tx_ops = {
	.owner		= THIS_MODULE,
	.open		= dbgfs_read_tx_seq_open,
	.read		= seq_read,
	.write		= dbgfs_write_tx,
	.llseek		= seq_lseek,
	.release	= single_release,
};

int dbgfs_read_queue(struct seq_file *seq, void *v)
{
	int ep;

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	seq_printf(seq, "Host:\n");
	seq_printf(seq, "\t ---------------------------------------|------------------|\n");
	seq_printf(seq, "\t ------------------- | TXIN             | TXOUT            |\n");
	seq_printf(seq, "\t ---------------------------------------|------------------|\n");
	seq_printf(seq, "\t base                |   0x%-8x     |   0x%-8x     |\n",
			TXPRIV->host_txin_ring.start,
			TXPRIV->host_txout_ring.start);
	seq_printf(seq, "\t phys base           |   0x%-8x     |   0x%-8x     |\n",
			TXPRIV->host_txin_ring.start_phys,
			TXPRIV->host_txout_ring.start_phys);
	seq_printf(seq, "\t element size (in DW)|   %-10d     |   %-10d     |\n", 4, 1);
	seq_printf(seq, "\t size                |   %-10u     |   %-10u     |\n",
			TXPRIV->host_txin_ring.size,
			TXPRIV->host_txout_ring.size);
	seq_printf(seq, "\t umt address         |   0x%-8x     |   0x%-8x     |\n", 0, 0);
	spin_lock_bh(&TXPRIV->stats.spinlock);
	seq_printf(seq, "\t head index          |   %-10u     |   %-10u     |\n",
			TXPRIV->host_txin_ring.head, TXPRIV->host_txin_ring.head);
	seq_printf(seq, "\t tail index          |   %-10u     |   %-10u     |\n",
			TXPRIV->host_txin_ring.tail, TXPRIV->host_txin_ring.tail);
	seq_printf(seq, "\t used                |   %-10u     |   %-10u     |\n",
			TXPRIV->host_txin_ring.count, TXPRIV->host_txin_ring.count);
	seq_printf(seq, "\t free                |   %-10u     |   %-10u     |\n",
			TXPRIV->host_txin_ring.size - TXPRIV->host_txin_ring.count,
			TXPRIV->host_txin_ring.size - TXPRIV->host_txin_ring.count);
	seq_printf(seq, "\t total               |   %-10u     |   %-10u     |\n",
			TXPRIV->host_txin_ring.umtcounter, TXPRIV->host_txin_ring.umtcounter);
	spin_unlock_bh(&TXPRIV->stats.spinlock);
	seq_printf(seq, "\t ---------------------------------------|------------------|\n");

	for (ep = 0; ep < 2; ep++) {
		seq_printf(seq, "EP%d:\n", ep);
		seq_printf(seq, "\t ---------------------------------------|------------------|\n");
		seq_printf(seq, "\t ------------------- | TXIN             | TXOUT            |\n");
		seq_printf(seq, "\t ---------------------------------------|------------------|\n");
		seq_printf(seq, "\t base                |   0x%-8x     |   0x%-8x     |\n",
				TXPRIV->ep[ep].start,
				TXPRIV->ep[ep].txout_start);
		seq_printf(seq, "\t phys base           |   0x%-8x     |   0x%-8x     |\n",
				TXPRIV->ep[ep].start_phys,
				TXPRIV->ep[ep].txout_start_phys);
		seq_printf(seq, "\t element size (in DW)|   %-10d     |   %-10d     |\n", 4, 1);
		seq_printf(seq, "\t size                |   %-10u     |   %-10u     |\n",
				TXPRIV->ep[ep].size,
				TXPRIV->ep[ep].size);
		seq_printf(seq, "\t umt address         |   0x%-8x     |   0x%-8x     |\n",
				TXPRIV->ep[ep].umt_out_addr, 0);
		spin_lock_bh(&TXPRIV->stats.spinlock);
		seq_printf(seq, "\t head index          |   %-10u     |   %-10u     |\n",
			TXPRIV->ep[ep].head, TXPRIV->ep[ep].head);
		seq_printf(seq, "\t tail index          |   %-10u     |   %-10u     |\n",
			TXPRIV->ep[ep].tail, TXPRIV->ep[ep].tail);
		seq_printf(seq, "\t used                |   %-10u     |   %-10u     |\n",
			TXPRIV->ep[ep].count, TXPRIV->ep[ep].count);
		seq_printf(seq, "\t free                |   %-10u     |   %-10u     |\n",
			TXPRIV->ep[ep].size - TXPRIV->ep[ep].count,
			TXPRIV->ep[ep].size - TXPRIV->ep[ep].count);
		seq_printf(seq, "\t total               |   %-10llu     |   %-10llu     |\n",
			TXPRIV->stats.epcounters[ep][EP_TOTAL_XMT],
			TXPRIV->stats.epcounters[ep][EP_TOTAL_SENT]);
		spin_unlock_bh(&TXPRIV->stats.spinlock);

		seq_printf(seq, "\t ---------------------------------------|------------------|\n");
		seq_printf(seq, "\t ------------------- | RXOUT            | RXIN             |\n");
		seq_printf(seq, "\t ---------------------------------------|------------------|\n");
		seq_printf(seq, "\t base                |   0x%-8x     |   0x%-8x     |\n",
				RXPRIV->m_st_device_info[ep].start,
				RXPRIV->m_st_device_info[ep].start);
		seq_printf(seq, "\t phys base           |   0x%-8x     |   0x%-8x     |\n",
				RXPRIV->m_st_device_info[ep].start_phys,
				RXPRIV->m_st_device_info[ep].start_phys);
		seq_printf(seq, "\t element size (in DW)|   %-10d     |   %-10d     |\n", 4, 4);
		seq_printf(seq, "\t size                |   %-10u     |   %-10u     |\n",
				RXPRIV->m_st_device_info[ep].rx_size,
				RXPRIV->m_st_device_info[ep].rx_size);
		seq_printf(seq, "\t umt address         |   0x%-8x     |   0x%-8x     |\n",
				0, RXPRIV->m_st_device_info[ep].umt_out_addr);
		spin_lock_bh(&RXPRIV->m_st_device_info[ep].rx_spinlock);
		seq_printf(seq, "\t head index          |   %-10u     |   %-10u     |\n",
				RXPRIV->m_st_device_info[ep].rx_head, 0);
		seq_printf(seq, "\t tail index          |   %-10u     |   %-10u     |\n",
				RXPRIV->m_st_device_info[ep].rx_tail, 0);
		seq_printf(seq, "\t used                |   %-10u     |   %-10u     |\n",
				RXPRIV->m_st_device_info[ep].rx_count, 0);
		seq_printf(seq, "\t free                |   %-10u     |   %-10u     |\n",
				(RXPRIV->m_st_device_info[ep].rx_size - RXPRIV->m_st_device_info[ep].rx_count), 0);
		seq_printf(seq, "\t total               |   %-10u     |   %-10u     |\n", 0, 0);
		spin_unlock_bh(&RXPRIV->m_st_device_info[ep].rx_spinlock);
		seq_printf(seq, "\t ---------------------------------------|------------------|\n");
	}

	return 0;
}

static int dbgfs_read_queue_seq_open(struct inode *inode,
				     struct file *file)
{
	return single_open(file, dbgfs_read_queue, NULL);
}

static const struct file_operations queue_ops = {
	.owner		= THIS_MODULE,
	.open		= dbgfs_read_queue_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

int dbgfs_write_stats(struct file *file, const char __user *buff, size_t count, loff_t *offset)
{
	unsigned char input[16] = {0};

#ifdef TX_FILL_LEVEL
	u32 fillpoint1[3],
	    fillpoint2[3],
	    fillpoint3[3];
#endif /* TX_FILL_LEVEL */

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	if (count == 0)
		return -EINVAL;

	memset(input, 0, sizeof(input));
	simple_write_to_buffer(input, sizeof(input) - 1, offset, buff, count - 1);

	if (strcmp(input, "0") == 0) {

		spin_lock_bh(&TXPRIV->stats.spinlock);
#ifdef TX_FILL_LEVEL
		fillpoint1[0] = TXPRIV->stats.filllevel[0].point1;
		fillpoint2[0] = TXPRIV->stats.filllevel[0].point2;
		fillpoint3[0] = TXPRIV->stats.filllevel[0].point3;
		fillpoint1[1] = TXPRIV->stats.filllevel[1].point1;
		fillpoint2[1] = TXPRIV->stats.filllevel[1].point2;
		fillpoint3[1] = TXPRIV->stats.filllevel[1].point3;
		fillpoint1[2] = TXPRIV->stats.filllevel[2].point1;
		fillpoint2[2] = TXPRIV->stats.filllevel[2].point2;
		fillpoint3[2] = TXPRIV->stats.filllevel[2].point3;
#endif /* TX_FILL_LEVEL */
		memset(((unsigned char *)&TXPRIV->stats) + sizeof(TXPRIV->stats.spinlock), 0, sizeof(TXPRIV->stats) - sizeof(TXPRIV->stats.spinlock));
#ifdef TX_FILL_LEVEL
		TXPRIV->stats.filllevel[0].point1 = fillpoint1[0];
		TXPRIV->stats.filllevel[0].point2 = fillpoint2[0];
		TXPRIV->stats.filllevel[0].point3 = fillpoint3[0];
		TXPRIV->stats.filllevel[1].point1 = fillpoint1[1];
		TXPRIV->stats.filllevel[1].point2 = fillpoint2[1];
		TXPRIV->stats.filllevel[1].point3 = fillpoint3[1];
		TXPRIV->stats.filllevel[2].point1 = fillpoint1[2];
		TXPRIV->stats.filllevel[2].point2 = fillpoint2[2];
		TXPRIV->stats.filllevel[2].point3 = fillpoint3[2];
#endif /* TX_FILL_LEVEL */
		spin_unlock_bh(&TXPRIV->stats.spinlock);

		spin_lock_bh(&RXPRIV->stats.spinlock);
		memset(((unsigned char *)&RXPRIV->stats) + sizeof(RXPRIV->stats.spinlock), 0, sizeof(RXPRIV->stats) - sizeof(RXPRIV->stats.spinlock));
		spin_unlock_bh(&RXPRIV->stats.spinlock);

		spin_lock_bh(&ASMPRIV->stats.spinlock);
		memset(((unsigned char *)&ASMPRIV->stats) + sizeof(ASMPRIV->stats.spinlock), 0, sizeof(ASMPRIV->stats) - sizeof(ASMPRIV->stats.spinlock));
		spin_unlock_bh(&ASMPRIV->stats.spinlock);
	}

	return count;
}

#ifdef TX_FILL_LEVEL
	#define SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(lineid, when, queuemaxsize) \
		seq_printf(seq, "[ 0:%lld {%lld, %lld}, (0,%d):%lld {%lld, %lld}, [%d,%d):%lld {%lld, %lld}, " \
			                                  "[%d,%d):%lld {%lld, %lld}, [%d,%d):%lld {%lld, %lld}, " \
								                   "%d:%lld {%lld, %lld}, >%d:%lld {%lld, %lld}]", \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][0], TXPRIV->stats.filllevel[(lineid)].count[(when)][1][0], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][2][0], \
			TXPRIV->stats.filllevel[(lineid)].point1, TXPRIV->stats.filllevel[(lineid)].count[(when)][0][1], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][1][1], TXPRIV->stats.filllevel[(lineid)].count[(when)][2][1], \
			TXPRIV->stats.filllevel[(lineid)].point1, TXPRIV->stats.filllevel[(lineid)].point2, \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][2], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][1][2], TXPRIV->stats.filllevel[(lineid)].count[(when)][2][2], \
			TXPRIV->stats.filllevel[(lineid)].point2, TXPRIV->stats.filllevel[(lineid)].point3, \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][3], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][1][3], TXPRIV->stats.filllevel[(lineid)].count[(when)][2][3], \
			TXPRIV->stats.filllevel[(lineid)].point3, (queuemaxsize), \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][4], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][1][4], TXPRIV->stats.filllevel[(lineid)].count[(when)][2][4], \
			(queuemaxsize), TXPRIV->stats.filllevel[(lineid)].count[(when)][0][5], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][1][5], TXPRIV->stats.filllevel[(lineid)].count[(when)][2][5], \
			(queuemaxsize), TXPRIV->stats.filllevel[(lineid)].count[(when)][0][6], \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][1][6], TXPRIV->stats.filllevel[(lineid)].count[(when)][2][6] \
		);

	#define SEQPRINTF_TX_SIMPLE_FILLLEVEL_STATS(lineid, when, queuemaxsize) \
		seq_printf(seq, "[ 0:%lld, (0,%d):%lld, [%d,%d):%lld, " \
			                     "[%d,%d):%lld, [%d,%d):%lld, " \
								      "%d:%lld, >%d:%lld]", \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][0], \
			TXPRIV->stats.filllevel[(lineid)].point1, TXPRIV->stats.filllevel[(lineid)].count[(when)][0][1], \
			TXPRIV->stats.filllevel[(lineid)].point1, TXPRIV->stats.filllevel[(lineid)].point2, \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][2], \
			TXPRIV->stats.filllevel[(lineid)].point2, TXPRIV->stats.filllevel[(lineid)].point3, \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][3], \
			TXPRIV->stats.filllevel[(lineid)].point3, (queuemaxsize), \
			TXPRIV->stats.filllevel[(lineid)].count[(when)][0][4], \
			(queuemaxsize), TXPRIV->stats.filllevel[(lineid)].count[(when)][0][5], \
			(queuemaxsize), TXPRIV->stats.filllevel[(lineid)].count[(when)][0][6] \
		);
#endif /* TX_FILL_LEVEL */

int dbgfs_read_stats(struct seq_file *seq, void *v)
{
	int cellbuff_mallocd, cellbuff_free, cellbuff_locked;
	u32 txinvsize[2];
	u32 txincount[2];
	u32 shadowtxincount[2];
	u64 asmsent[2];
	u64 asmdropped[2];
	u64 asmdiscarded[2];
	u32 lastsid[2];
	u64 grouplostcells;
	u32 txlinkstatus[2],
	    rxlinkstatus[2];

	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;

	spin_lock_bh(&ASMPRIV->stats.spinlock);
	asmsent[0] = ASMPRIV->stats.asmsent[0];
	asmsent[1] = ASMPRIV->stats.asmsent[1];
	asmdropped[0] = ASMPRIV->stats.asmdropped[0];
	asmdropped[1] = ASMPRIV->stats.asmdropped[1];
	asmdiscarded[0] = ASMPRIV->stats.asmdiscarded[0];
	asmdiscarded[1] = ASMPRIV->stats.asmdiscarded[1];
	grouplostcells = ASMPRIV->stats.grouplostcells;
	spin_unlock_bh(&ASMPRIV->stats.spinlock);
	txlinkstatus[0] = ASMPRIV->fsm.txlinkstatus[0];
	txlinkstatus[1] = ASMPRIV->fsm.txlinkstatus[1];
	rxlinkstatus[0] = ASMPRIV->fsm.rxlinkstatus[0];
	rxlinkstatus[1] = ASMPRIV->fsm.rxlinkstatus[1];

	spin_lock_bh(&TXCBPRIV->spinlock);
	cellbuff_mallocd = TXCBPRIV->malloc_count;
	cellbuff_free = TXCBPRIV->free_count;
	cellbuff_locked = TXCBPRIV->lock_count;
	spin_unlock_bh(&TXCBPRIV->spinlock);

	spin_lock_bh(&TXPRIV->ep[0].spinlock);
#ifdef TXDELAY
	shadowtxincount[0] = TXPRIV->ep[0].xmt_pending_count;
#endif
	txincount[0] = TXPRIV->ep[0].count;
	txinvsize[0] = TXPRIV->ep[0].vsize;
	spin_unlock_bh(&TXPRIV->ep[0].spinlock);

	spin_lock_bh(&TXPRIV->ep[1].spinlock);
#ifdef TXDELAY
	shadowtxincount[1] = TXPRIV->ep[1].xmt_pending_count;
#endif
	txincount[1] = TXPRIV->ep[1].count;
	txinvsize[1] = TXPRIV->ep[1].vsize;
	spin_unlock_bh(&TXPRIV->ep[1].spinlock);

	spin_lock_bh(&TXPRIV->stats.spinlock);
	lastsid[0] = TXPRIV->stats.lastsid[0];
	lastsid[1] = TXPRIV->stats.lastsid[1];

	seq_printf(seq,
			"(TX)> AAL5 Frames=> Transmitted:%llu, Waiting:%d, BackPressure:%llu, "
			"Cell Buffers=> Malloc:%d, Free:%d, Lock:%d, SwQ:[ASM:%d, Non-ASM:%d], "
			"Shaper:[Enabled:%s, Rate:%d kbps, Threshold:%d, Quota:%d]\n",
			TXPRIV->stats.counters[PACKETS_SEGMENTED], 
			TXPRIV->host_txin_ring.count, TXPRIV->stats.counters[INSUFFICIENT_TX_BUFFER_STACK_BACKPRESSURE],
			cellbuff_mallocd, cellbuff_free, cellbuff_locked,
			TXPRIV->asm_queue.count, TXPRIV->nonasm_queue.count,
			TXPRIV->shaper.disabled ? "no" :"yes",
			TXPRIV->shaper.rate, TXPRIV->shaper.threshold, TXPRIV->shaper.quota
	);

#ifdef TX_FILL_LEVEL
	seq_printf(seq, "(TX)>   Sleep time(ms)=> (0,.5]:%lld, (.5,.75]:%lld, (.75,1]:%lld, (1,1.5]:%lld, "
	                                        "(1.5,2]:%lld, (2,2.5]:%lld, (2.5,3]:%lld, (3,5]:%lld, "
											"(5,10]:%lld, >10:%lld\n",
			TXPRIV->stats.elapsedcount[0], TXPRIV->stats.elapsedcount[1],
			TXPRIV->stats.elapsedcount[2], TXPRIV->stats.elapsedcount[3],
			TXPRIV->stats.elapsedcount[4], TXPRIV->stats.elapsedcount[5],
			TXPRIV->stats.elapsedcount[6], TXPRIV->stats.elapsedcount[7],
			TXPRIV->stats.elapsedcount[8], TXPRIV->stats.elapsedcount[9]
	);
	seq_printf(seq, "(TX)>   Fill level after wakeup=> TXIN-0:");
	SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(0, 0, txinvsize[0]);
	seq_printf(seq, ", TXIN-1:");
	SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(1, 0, txinvsize[1]);
	seq_printf(seq, ", Data SwQ:");
	SEQPRINTF_TX_SIMPLE_FILLLEVEL_STATS(2, 0, 1024);
	seq_printf(seq, ", CBM:[min:%d bytes, max:%d bytes]\n",
			TXPRIV->stats.cbmsize[0], TXPRIV->stats.cbmsize[1]);
	seq_printf(seq, "(TX)>   Filling level while woken up=> TXIN-0:");
	SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(0, 1, txinvsize[0]);
	seq_printf(seq, ", TXIN-1:");
	SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(1, 1, txinvsize[1]);
	seq_printf(seq, ", Data SwQ:");
	SEQPRINTF_TX_SIMPLE_FILLLEVEL_STATS(2, 1, 1024);
	seq_printf(seq, "\n");
	seq_printf(seq, "(TX)>   Fill level before sleep=> TXIN-0:");
	SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(0, 2, txinvsize[0]);
	seq_printf(seq, ", TXIN-1:");
	SEQPRINTF_TX_COMPLEX_FILLLEVEL_STATS(1, 2, txinvsize[1]);
	seq_printf(seq, ", Data SwQ:");
	SEQPRINTF_TX_SIMPLE_FILLLEVEL_STATS(2, 2, 1024);
	seq_printf(seq, "\n");
#endif /* TX_FILL_LEVEL */

	seq_printf(seq,
			"(TX)>   ATM Cells=> ASM:[0:%llu, 1:%llu], Non-ASM:[0:%llu, 1:%llu], Dropped:[ASM0:%llu, ASM1:%llu, Non-ASM:%llu], Last-SID:[0:%d, 1:%d] BackPressure:[0:%llu, 1:%llu]"
#ifdef TXDELAY
			", TXIN-0:[Past:%d, Future:%d], TXIN-1:[Past:%d, Future:%d]\n"
#else
			", TXIN-0:%d, TXIN-1:%d]\n"
#endif
			,(u64)asmsent[0], (u64)asmsent[1],
			TXPRIV->stats.epcounters[0][EP_TOTAL_XMT], TXPRIV->stats.epcounters[1][EP_TOTAL_XMT],
			(u64)asmdropped[0], (u64)asmdropped[1],
			TXPRIV->stats.counters[EP_TOTAL_DROPPED],
			lastsid[0], lastsid[1],
			TXPRIV->stats.epcounters[0][NO_EP_FREESPACE_BACKPRESSURE], TXPRIV->stats.epcounters[1][NO_EP_FREESPACE_BACKPRESSURE],
#ifdef TXDELAY
			txincount[0] - shadowtxincount[0], shadowtxincount[0],
			txincount[1] - shadowtxincount[1], shadowtxincount[1]
#else
			txincount[0], txincount[1]
#endif
		);
	spin_unlock_bh(&TXPRIV->stats.spinlock);

	seq_printf(seq,
			"(CO)>     TX:[0:%c, 1:%c], RX:[0:%c, 1:%c], Group lost cells:%llu\n",
			rxlinkstatus[0] == 3 ? 'Y' :
        		rxlinkstatus[0] == 2 ? 'A' :
					rxlinkstatus[0] == 1 ? 'N' :
						rxlinkstatus[0] == 0 ? '0' : '-',
			rxlinkstatus[1] == 3 ? 'Y' :
        		rxlinkstatus[1] == 2 ? 'A' :
					rxlinkstatus[1] == 1 ? 'N' :
						rxlinkstatus[1] == 0 ? '0' : '-',
			txlinkstatus[0] == 3 ? 'Y' :
        		txlinkstatus[0] == 2 ? 'A' :
					txlinkstatus[0] == 1 ? 'N' :
						txlinkstatus[0] == 0 ? '0' : '-',
			txlinkstatus[1] == 3 ? 'Y' :
        		txlinkstatus[1] == 2 ? 'A' :
					txlinkstatus[1] == 1 ? 'N' :
						txlinkstatus[1] == 0 ? '0' : '-',
			grouplostcells
		);

	spin_lock_bh(&(RXPRIV->stats.spinlock));
	seq_printf(seq,
			"(RX)>   ATM Cells=> ASM:[0:%llu, 1:%llu], Discarded: [0:%llu, 1:%llu], Non-ASM:[0:%llu, 1:%llu], Discarded: [0:%llu, 1:%llu], Timed-out:%llu, Missed:%llu\n"
			"(RX)> AAL5 Frames=> Accepted:%llu, Discarded: [Congestion:%llu, SID:%llu, CRC:%llu, Length:%llu, Alloc-failure:%llu, Size-Overflow:%llu]\n",
			RXPRIV->stats.asmcount[0], RXPRIV->stats.asmcount[1],
			asmdiscarded[0], asmdiscarded[1],
			RXPRIV->stats.nonasmcount[0], RXPRIV->stats.nonasmcount[1],
			RXPRIV->stats.droppedcells[0], RXPRIV->stats.droppedcells[1],
			RXPRIV->stats.timedoutcells, RXPRIV->stats.missingcells,
			RXPRIV->stats.acceptedframecount, 
			RXPRIV->stats.discardedframecount[INVALIDPT2ORCLP],
			RXPRIV->stats.discardedframecount[SIDJUMP],
			RXPRIV->stats.discardedframecount[INVALIDCRC32], RXPRIV->stats.discardedframecount[INVALIDLENGTH],
			RXPRIV->stats.discardedframecount[ALLOCFAILURE], RXPRIV->stats.discardedframecount[SIZEOVERFLOW]
		);

#ifdef MEASURE_JIFFIES
	seq_printf(seq,
			"(RX)> process_rx()=> Jiffies@Start:%llu, Jiffies@Now:%llu, Total Jiffies Spent: %llu\n",
			RXPRIV->stats.start_jiffies, get_jiffies_64(), RXPRIV->stats.jiffies);
#endif

	spin_unlock_bh(&(RXPRIV->stats.spinlock));

	return 0;
}

static int dbgfs_read_stats_seq_open(struct inode *inode,
				     struct file *file)
{
	return single_open(file, dbgfs_read_stats, NULL);
}

static const struct file_operations stats_ops = {
	.owner		= THIS_MODULE,
	.open		= dbgfs_read_stats_seq_open,
	.read		= seq_read,
	.write		= dbgfs_write_stats,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static struct ima_bond_dbgfs_list ima_bond_dbgfs_entries[] = {
	/*{"ver",	0644,	&ver_ops},*/
	/*{"dbg",	0644,	&dbg_ops},*/
	/*{"mib",	0644,	&mib_ops},*/
	{"dbg",		0644,	&dbg_ops},
	{"tx",		0644,	&tx_ops},
	{"queue",	0644,	&queue_ops},
	{"stats",	0644,	&stats_ops},
};

int init_debug_interface(void)
{
	int ret = 0;
	int i;
	struct ima_bond_dbgfs_list *p;
	struct dentry *file;

	g_dbgfs_node = debugfs_create_dir(IMA_BOND_DBGFS_NAME, NULL);
	if (!g_dbgfs_node) {
		pr_err("failed to create %s", IMA_BOND_DBGFS_NAME);
		ret = -ENOMEM;
		goto err_out;
	}

	for (i = 0; i < ARRAY_SIZE(ima_bond_dbgfs_entries); i++) {
		p = &ima_bond_dbgfs_entries[i];
		file = debugfs_create_file(p->name, p->mode,
					   g_dbgfs_node,
					   NULL, p->fops);
		if (!file) {
			pr_err("failed to create %s", p->name);
			ret = -ENOMEM;
			goto err_out_file;
		}
	}

	return ret;

err_out_file:
	debugfs_remove_recursive(g_dbgfs_node);

err_out:
	return ret;
}

void cleanup_debug_interface(void)
{
	debugfs_remove_recursive(g_dbgfs_node);
}
