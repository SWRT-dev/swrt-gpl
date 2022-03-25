/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/fs.h>
#include<linux/slab.h>
#include <linux/kernel.h>
#include <net/datapath_proc_api.h>
#include <net/datapath_api.h>
#include "datapath.h"

#undef local_dbg
#define local_dbg(fmt, arg...) pr_debug(fmt, ##arg)
int dp_strncmpi(const char *s1, const char *s2, size_t n)
{
	if (!s1 || !s2)
		return 1;
	return strncasecmp(s1, s2, n);
}
EXPORT_SYMBOL(dp_strncmpi);

void dp_replace_ch(char *p, int len, char orig_ch, char new_ch)
{
	int i;

	if (p)
		for (i = 0; i < len; i++) {
			if (p[i] == orig_ch)
				p[i] = new_ch;
		}
}
EXPORT_SYMBOL(dp_replace_ch);

int dp_atoi(unsigned char *str)
{
	long long v = 0;
	char *p = NULL;
	int res;

	if (!str)
		return v;
	dp_replace_ch(str, strlen(str), '.', 0);
	dp_replace_ch(str, strlen(str), ' ', 0);
	dp_replace_ch(str, strlen(str), '\r', 0);
	dp_replace_ch(str, strlen(str), '\n', 0);
	if (str[0] == 0)
		return v;
	if (str[0] == 'b' || str[0] == 'B') {
		p = str + 1;
		res = kstrtoll(p, 2, &v); /* binary */
	} else if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))) {
		p = str + 2;
		res = kstrtoll(p, 16, &v); /* hex */
	} else {
		p = str;
		res = kstrtoll(p, 10, &v); /* dec */
	}
	if (res)
		v = 0;
	return v;
}
EXPORT_SYMBOL(dp_atoi);

/*Split buffer to multiple segment with seperater space.
 *And put pointer to array[].
 *By the way, original buffer will be overwritten with '\0' at some place.
 */
int dp_split_buffer(char *buffer, char *array[], int max_param_num)
{
	int i = 0;

	if (!array)
		return 0;
	memset(array, 0, sizeof(array[0]) * max_param_num);
	if (!buffer)
		return 0;
	while ((array[i] = strsep(&buffer, " ")) != NULL) {
		size_t len = strlen(array[i]);

		dp_replace_ch(array[i], len, ' ', 0);
		dp_replace_ch(array[i], len, '\r', 0);
		dp_replace_ch(array[i], len, '\n', 0);
		len = strlen(array[i]);
		if (!len)
			continue;
		i++;
		if (i == max_param_num)
			break;
	}

	return i;
}
EXPORT_SYMBOL(dp_split_buffer);

void set_start_end_id(unsigned int new_start, unsigned int new_end,
		      unsigned int max_start, unsigned int max_end,
		      unsigned int default_start, unsigned int default_end,
		      unsigned int *start, unsigned int *end)
{
	if (!start || !end)
		return;

	if (new_start > new_end) {
		*start = default_start;
		*end = default_end;
	} else {
		*start = new_start;
		*end = new_end;
	}

	if (*start > max_start)
		*start = default_start;

	if (*end > max_end)
		*end = default_end;
}
EXPORT_SYMBOL(set_start_end_id);

/* cmd: command line, its format should be like
 * cmd_len: the length of command line
 * optcurser:
 */
int dp_getopt(char *cmd[], int cmd_size, int *cmd_offset,
	      char **optarg, const char *optstring)
{
	char *p;
	int offset;
	int i;

	if (!cmd || !cmd_offset || !optstring || !optarg)
		return -1;
	if (*cmd_offset >= cmd_size)
		return 0;
	offset = *cmd_offset;
	while (1) {
		p = cmd[offset];
		if (p[0] != '-') {
			offset++;
			return -1; /*wrong format*/
		}
		for (i = 0; i < strlen(optstring); i++) {
			if (optstring[i] != p[1])
				continue;
			/* match */
			if (optstring[i + 1] == ':') { /*opt + value */
				if (offset + 1 > cmd_size)
					return -1;
				*optarg = cmd[offset + 1];
				offset += 2;
			} else { /*no value */
				*optarg = NULL;
				offset += 1;
			}
			*cmd_offset = offset;
			return (int)optstring[i];
		}
		return -1;
	}
	return -1;
}
EXPORT_SYMBOL(dp_getopt);

static void *dp_seq_start(struct seq_file *s, loff_t *pos)
{
	struct dp_proc_file_entry *p = s->private;

	if (p->pos < 0)
		return NULL;

	return p;
}

static void *dp_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct dp_proc_file_entry *p = s->private;

	*pos = p->pos;

	if (p->pos >= 0)
		return p;
	else
		return NULL;
}

static void dp_seq_stop(struct seq_file *s, void *v)
{
}

static int dp_seq_show(struct seq_file *s, void *v)
{
	struct dp_proc_file_entry *p = s->private;

	if (p->pos >= 0) {
		if (p->multi_callback) {
			local_dbg("multiple call");
			p->pos = p->multi_callback(s, p->pos);
		} else if (p->single_callback) {
			local_dbg("single call: %px", p->single_callback);
			p->single_callback(s);
			p->pos = -1;
		}
	}
	return 0;
}

static const struct seq_operations dp_seq_ops = {
	.start = dp_seq_start,
	.next = dp_seq_next,
	.stop = dp_seq_stop,
	.show = dp_seq_show
};

void dummy_single_show(struct seq_file *s)
{
	seq_puts(s, "Cat Not implemented yet !\n");
}

static int dp_proc_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	struct dp_proc_file_entry *p;
	struct dp_proc_entry *entry;
	int ret;

	ret = seq_open(file, &dp_seq_ops);
	if (ret)
		return ret;

	s = file->private_data;
	p = kmalloc(sizeof(*p), GFP_KERNEL);

	if (!p) {
		(void)seq_release(inode, file);
		return -ENOMEM;
	}
	memset(p, 0, sizeof(*p));

	entry = inode->i_private;

	if (entry->multi_callback)
		p->multi_callback = entry->multi_callback;
	if (entry->single_callback)
		p->single_callback = entry->single_callback;
	else
		p->single_callback = dummy_single_show;

	if (entry->init_callback)
		p->pos = entry->init_callback();
	else
		p->pos = 0;

	s->private = p;

	return 0;
}

static int dp_proc_release(struct inode *inode, struct file *file)
{
	struct seq_file *s;

	s = file->private_data;
	kfree(s->private);

	return seq_release(inode, file);
}

static int dp_seq_single_show(struct seq_file *s, void *v)
{
	struct dp_proc_entry *p = s->private;

	p->single_callback(s);
	return 0;
}

static int dp_proc_single_open(struct inode *inode, struct file *file)
{
	return single_open(file, dp_seq_single_show, inode->i_private);
}

#define MEM_ZERO(x, y) memset(&(x), 0, y)
void dp_proc_entry_create(struct dentry *parent_node,
			  struct dp_proc_entry *proc_entry)
{
	struct dentry *file;

	if (!proc_entry || !proc_entry->name)
		return;
	MEM_ZERO(proc_entry->ops, sizeof(proc_entry->ops));
	proc_entry->ops.owner = THIS_MODULE;

	if (proc_entry->single_callback) {
		proc_entry->ops.open = dp_proc_single_open;
		proc_entry->ops.release = single_release;
	} else if (proc_entry->multi_callback) {
		proc_entry->ops.open = dp_proc_open;
		proc_entry->ops.release = dp_proc_release;
	} else { /*regard as single call with dummy show*/
		proc_entry->ops.open = dp_proc_single_open;
		proc_entry->ops.release = single_release;
		proc_entry->single_callback = dummy_single_show;
	}

	proc_entry->ops.read = seq_read;
	proc_entry->ops.llseek = seq_lseek;
	proc_entry->ops.write = proc_entry->write_callback;
	/*Don't know why checkpatch propose to use 0444
	 *Instead of S_IRUGO ???
	 */
	file = debugfs_create_file(proc_entry->name, 0644/*(S_IFREG | 0444)*/,
				   parent_node,
				   proc_entry, &proc_entry->ops);
}
EXPORT_SYMBOL(dp_proc_entry_create);
