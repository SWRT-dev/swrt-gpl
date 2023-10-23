// SPDX-License-Identifier: GPL-2.0
/*******************************************************************************
         Copyright (c) 2020 - 2021, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2012 - 2014 Lantiq Deutschland GmbH

*******************************************************************************/

#ifndef DATAPATH_PROC_H
#define DATAPATH_PROC_H
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
#include <net/datapath_proc_api_grx500.h>
#else
#include <linux/kernel.h>	/*kmalloc */
#include <linux/ctype.h>
#include <linux/debugfs.h>	/*file_operations */
#include <linux/seq_file.h>	/*seq_file */
#include <linux/uaccess.h>	/*copy_from_user */

#define set_ltq_dbg_flag(v, e, f) do {;\
	if ((e) > 0)\
		(v) |= (uint32_t)(f);\
	else\
		(v) &= (uint32_t)(~f); } \
	while (0)

typedef void (*dp_proc_single_callback_t) (struct seq_file *);
typedef int (*dp_proc_callback_t) (struct seq_file *, int);
typedef int (*dp_proc_init_callback_t) (void);
typedef ssize_t(*dp_proc_write_callback_t) (struct file *file,
					     const char __user *input,
					     size_t size, loff_t *loff);

struct dp_proc_file_entry {
	dp_proc_callback_t multi_callback;
	dp_proc_single_callback_t single_callback;
	int pos;
	int single_call_only;
};

struct dp_proc_entry {
	char *name;
	dp_proc_single_callback_t single_callback;
	dp_proc_callback_t multi_callback;
	dp_proc_init_callback_t init_callback;
	dp_proc_write_callback_t write_callback;
	struct file_operations ops;
};

int dp_getopt(char *cmd[], int cmd_size, int *cmd_offset,
	      char **optarg, const char *optstring);

void dp_proc_entry_create(struct dentry *parent_node,
			  struct dp_proc_entry *proc_entry);

int dp_atoi(unsigned char *str);
int dp_strncmpi(const char *s1, const char *s2, size_t n);
void dp_replace_ch(char *p, int len, char orig_ch, char new_ch);

/*Split buffer to multiple segment with separator space.
 *And put pointer to array[].
 *By the way, original buffer will be overwritten with '\0' at some place.
 */
int dp_split_buffer(char *buffer, char *array[], int max_param_num);

/* below ltq_xxx api is for back-compatible only */
#define ltq_atoi dp_atoi
#define ltq_strncmpi dp_strncmpi
#define ltq_replace_ch dp_replace_ch
#define ltq_remove_leading_whitespace dp_remove_leading_whitespace
#define ltq_split_buffer dp_split_buffer
#define set_start_end_id dp_set_start_end_id
void dp_set_start_end_id(unsigned int new_start, unsigned int new_end,
			 unsigned int max_start, unsigned int max_end,
			 unsigned int default_start, unsigned int default_end,
			 unsigned int *start, unsigned int *end);
#endif /* IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30 */
#endif				/*DATAPATH_PROC_H */
