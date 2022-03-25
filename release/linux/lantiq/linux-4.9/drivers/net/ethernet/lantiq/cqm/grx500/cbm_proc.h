/******************************************************************************

	Copyright (c) 2012
	Lantiq Deutschland GmbH

	For licensing information, see the file 'LICENSE' in the root folder of
	this software module.

******************************************************************************/

#ifndef _DRV_CBM_API_H_
#define _DRV_CBM_API_H_

#include <asm/div64.h>
#include <linux/ctype.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/sort.h>
#include <linux/version.h>
#include <net/datapath_proc_api.h>
#include <net/lantiq_cbm_api.h>
#include "reg/cbm_ls.h"
#include "reg/cbm.h"
#include "reg/cbm_dqm.h"
#include "reg/cbm_eqm.h"
#include "reg/cbm_desc64b.h"
#include "reg/fsqm.h"
//#include "asm/gic.h"

#define QOCC_TEST 1

#define CBM_PROC_NAME    "cbm"
#define PROC_BASE        " /proc/"CBM_PROC_NAME"/"
#define CBM_PROC_PARENT ""
#define PROC_FILE_CBM_ENQ "cbmenq"
#define PROC_FILE_CBM_DEQ "cbmdeq"
#define PROC_FILE_CBM_LS "cbmls"
#define PROC_FILE_CBM_SBA_JBA "cbmsbajba"
#define PROC_FILE_CBM_STAT_REG "cbmstat"
#define PROC_FILE_CBM_DES64_INGRESS_REG "cbmdes64ingress"
#define PROC_FILE_CBM_DES64_EGRESS_REG "cbmdes64egress"
#define PROC_FILE_RCNT_REG "fsqmrnt"
#define PROC_FILE_FSC_FSQT_REG "fsqmfsc"
#define PROC_FILE_CBM_ENQ_OVH_BYTES "cbmenqovh"
#define PROC_FILE_CBM_COUNTER_MODE "cbmcntrmode"
#define PROC_FILE_CBM_Q_THRES "cbmqthres"
#define PROC_FILE_CBM_Q_DELAY "cbmqdelay"
#ifdef QOCC_TEST
#define PROC_FILE_CBM_QOCC_TEST "cbmqocctest"
#endif
#define PROC_FILE_CBM_BUF_STAT "cbmbuffstat"

#define get_val(val, mask, offset) (((val) & (mask)) >> (offset))

#define cbm_memset  memset

typedef void (*proc_single_callback_t) (struct seq_file *);
typedef int (*proc_callback_t) (struct seq_file *, int);
typedef int (*proc_init_callback_t) (void);
typedef ssize_t(*proc_write_callback_t) (struct file *file,
					 const char __user *input, size_t size,
					 loff_t *loff);
void cbm_enqueue_dump(struct seq_file *s);
void cbm_dequeue_dump(struct seq_file *s);
void cbm_ls_dump(struct seq_file *s);
void cbm_sba_jba_dump(struct seq_file *s);
void cbm_status_reg_dump(struct seq_file *s);
void cbm_des64_ingress_dump(struct seq_file *s);
void cbm_des64_egress_dump(struct seq_file *s);
ssize_t fsqm_rnt_read(struct file *file, const char *buf, size_t count, loff_t *ppos);
ssize_t fsqm_freesegment_read(struct file *file, const char *buf, size_t count, loff_t *ppos);
void cbm_enq_ovh_bytes_read(struct seq_file *s);
ssize_t cbm_enq_ovh_bytes_write(struct file *file, const char *buf, size_t count, loff_t *ppos);
void cbm_counter_mode_get_proc(struct seq_file *s);
ssize_t cbm_counter_mode_set_proc(struct file *file, const char *buf, size_t count, loff_t *ppos);
void cbm_q_thres_get_proc(struct seq_file *s);
ssize_t cbm_q_thres_set_proc(struct file *file, const char *buf, size_t count, loff_t *ppos);
ssize_t cbm_eqm_delay_set_proc(struct file *file, const char *buf, size_t count, loff_t *ppos);
#ifdef QOCC_TEST
ssize_t cbm_qocc_test_steps(struct file *file, const char *buf, size_t count, loff_t *ppos);
#endif
void cbm_buff_stat_dump(struct seq_file *s);
#define CBM_PRINT printk
#endif
