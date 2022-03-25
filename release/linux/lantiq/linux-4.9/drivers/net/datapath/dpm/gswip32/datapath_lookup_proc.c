/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/if_ether.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>

#include <linux/clk.h>
#include <linux/ip.h>
#include <net/ip.h>
#define DATAPATH_HAL_LAYER   /*must put before include datapath_api.h in
			      *order to avoid include another platform's
			      *DMA descriptor and pmac header files
			      */
#include <net/datapath_api.h>
#include <net/datapath_api_gswip32.h>
#include "../datapath.h"
#include "datapath_proc.h"
#include "datapath_ppv4.h"
#include "datapath_misc.h"
#include <net/datapath_proc_api.h>

#define SEQ_PRINTF seq_printf

#define proc_printf(s, fmt, arg...) \
	do { \
		if (!s) \
			pr_info(fmt, ##arg); \
		else \
			seq_printf(s, fmt, ##arg); \
	} while (0)

#define CARE_FLAG      0
#define CARE_NOT_FLAG  1

#define LIST_ALL_CASES(t, mask, not_care)  \
	for (t[0] = 0;  t[0] < ((mask[0] == not_care) ? 2 : 1); t[0]++) \
	for (t[1] = 0;  t[1] < ((mask[1] == not_care) ? 2 : 1); t[1]++) \
	for (t[2] = 0;  t[2] < ((mask[2] == not_care) ? 2 : 1); t[2]++) \
	for (t[3] = 0;  t[3] < ((mask[3] == not_care) ? 2 : 1); t[3]++) \
	for (t[4] = 0;  t[4] < ((mask[4] == not_care) ? 2 : 1); t[4]++) \
	for (t[5] = 0;  t[5] < ((mask[5] == not_care) ? 2 : 1); t[5]++) \
	for (t[6] = 0;  t[6] < ((mask[6] == not_care) ? 2 : 1); t[6]++) \
	for (t[7] = 0;  t[7] < ((mask[7] == not_care) ? 2 : 1); t[7]++) \
	for (t[8] = 0;  t[8] < 1; t[8]++) \
	for (t[9] = 0;  t[9] < 1; t[9]++) \
	for (t[10] = 0; t[10] < 1; t[10]++) \
	for (t[11] = 0; t[11] < 1; t[11]++) \
	for (t[12] = 0; t[12] < ((mask[12] == not_care) ? 2 : 1); t[12]++)

/* The purpose of this file is to find the CBM lookup pattern and
 * print it in the simple way.
 * Otherway it may print up to 16K lines in the console to get lookup setting
 *  Lookup table: flow[1] flow[0] dec end mpe2 mpe1 ep(4) class(4)
 * Idea: We fixed the EP value during finding lookup setting pattern.
 * method:
 *     1st: to find the possible don't care bit from flow[2]/dec/enc/mpe2/mpe1
 *	 and class(4), excluding ep, ie total 10 bits
 *	       API: c_not_care_walkthrought
 *		   Note: from big don't care bit number (ie, maximum don't
 *		   care case) to 1 (minimal don't care case)
 *
 *	  2nd: generate tmp_index based on care bits
 *	       API: list_care_combination
 *
 *    3rd: based on tmp_index, check whether there is really pattern which meet
 *    don't care, ie, mapping to same qid.
 *
 */
#define LOOKUP_FIELD_BITS 13
#define EGFLAG_BIT  12 /* egflag bit */
#define EP_BIT      8  /* EP port starting bit */

static int lookup_mask_n;

/* match state */
#define PATTERN_MATCH_INIT  0
#define PATTERN_MATCH_START 1
#define PATTERN_MATCH_FAIL  2
#define PATTERN_MATCH_PASS  3

#define ENTRY_FILLED 0
#define ENTRY_USED   1

static int pattern_match_flag;	/*1--start matching  2--failed, 3---match 0k */
static unsigned char lookup_mask1[LOOKUP_FIELD_BITS];

#define C_ARRAY_SIZE  20
static int c_tmp_data[C_ARRAY_SIZE];

/*store result */
#define MAX_PATTERN_NUM 1024
static int lookup_match_num;
static unsigned short lookup_match_mask[MAX_PATTERN_NUM];
/*save tmp_index */
static unsigned short lookup_match_index[MAX_PATTERN_NUM];
/*save tmp_index */
static unsigned char lookup_match_qid[MAX_PATTERN_NUM];

static int tmp_pattern_port_id;

static int left_n;
/*10 bits lookup table except 4 bits EP */
static unsigned char lookup_tbl_flags[BIT(LOOKUP_FIELD_BITS)];

static void combine_util(int *arr, int *data, int start, int end, int index,
			 int r);
static int check_pattern(int *data, int r);
static void lookup_table_via_qid(int qid);
static void lookup_table_remap(int old_q, int new_q);
static int find_pattern(int port_id, struct seq_file *s, int qid);
static int get_dont_care_lookup(char *s);
static void lookup_table_recursive(int k, int tmp_index, int set_flag, int qid);

int dp_get_lookup_qid_via_index(struct cbm_lookup *info)
{
	int qid;

	if (!info)
		return 0;
	if (info->index & BIT(EGFLAG_BIT))
		info->egflag = 1;
	else
		info->egflag = 0;
	info->index &= BIT(EGFLAG_BIT) - 1;
	qid = get_lookup_qid_via_index(info);
	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "get egflag=%d index=0x%x qid=%d\n",
		 info->egflag, info->index, qid);
	return qid;
}

void dp_set_lookup_qid_via_index(struct cbm_lookup *info)
{
	if (!info)
		return;
	if (info->index & BIT(EGFLAG_BIT))
		info->egflag = 1;
	else
		info->egflag = 0;
	info->index &= BIT(EGFLAG_BIT) - 1;
	set_lookup_qid_via_index(info);
	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "set egflag=%d index=0x%x qid=%d\n",
		 info->egflag, info->index, info->qid);
}

/* The main function that prints all combinations of size r*/
/* in arr[] of size n. This function mainly uses combine_util()*/
static void c_not_care_walkthrought(int *arr, int n, int r)
{
	/* A temporary array data[] to store all combination one by one */

	/* Print all combination using temprary array 'data[]' */
	combine_util(arr, c_tmp_data, 0, n - 1, 0, r);
}

/* arr[]  ---> Input Array
 * data[] ---> Temporary array to store current combination
 * start & end ---> Staring and Ending indexes in arr[]
 * index  ---> Current index in data[]
 * r ---> Size of a combination to be printed
 *
 */
static void combine_util(int *arr, int *data, int start, int end, int index,
			 int r)
{
	int i;

	/* Current combination is ready to be printed, print it */
	if (left_n <= 0)
		return;
	if (index == r) {/*Find one pattern with specified don't care flag */

		check_pattern(data, r);
		/*find a don't care case and need further check */

		return;
	}
	/* replace index with all possible elements. The condition */
	/* "end-i+1 >= r-index" makes sure that including one element */
	/* at index will make a combination with remaining elements */
	/* at remaining positions */
	for (i = start; i <= end && end - i + 1 >= r - index; i++) {
		data[index] = arr[i];
		if (left_n <= 0)
			break;
		combine_util(arr, data, i + 1, end, index + 1, r);
	}
}

/*Note: when call this API, for those cared bits,
 * its value already set in tmp_index.
 */
static void lookup_pattern_match(int tmp_index)
{
	int i;
	int qid;
	static int first_qid;
	int t[LOOKUP_FIELD_BITS] = { 0 };
	int index;
	struct cbm_lookup lookup;

	DP_DEBUG(DP_DBG_FLAG_LOOKUP,
		 "trying with tmp_index=0x%x with lookup_match_num=%d\n",
		 tmp_index, lookup_match_num);
	pattern_match_flag = PATTERN_MATCH_INIT;
	lookup_match_index[lookup_match_num] = tmp_index;

	LIST_ALL_CASES(t, lookup_mask1, CARE_NOT_FLAG) {
		index = tmp_index;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			index |= (t[i] << i);
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "don't care[14]=");
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			DP_DEBUG(DP_DBG_FLAG_LOOKUP, "%d ", t[i]);
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "\n");

		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "don't care index=%x\n", index);

		if (lookup_tbl_flags[index] == ENTRY_USED) {
			pattern_match_flag = PATTERN_MATCH_FAIL;
			goto END;
		}
		lookup.index = index;
		qid = dp_get_lookup_qid_via_index(&lookup);

		if (pattern_match_flag == PATTERN_MATCH_INIT) {
			pattern_match_flag = PATTERN_MATCH_START;
			first_qid = qid;
		} else if (first_qid != qid) {
			pattern_match_flag = PATTERN_MATCH_FAIL;
			DP_DEBUG(DP_DBG_FLAG_LOOKUP,
				 "first_qid(%d) != qid(%d)\n",
				 first_qid, qid);
			goto END;
		}
	}

END:
	/*save the result if necessary here */
	if (pattern_match_flag == PATTERN_MATCH_START) {
		/*pass since still not fail yet */
		pattern_match_flag = PATTERN_MATCH_PASS;

		/*mark the entries */
		LIST_ALL_CASES(t, lookup_mask1, CARE_NOT_FLAG) {
			index = tmp_index;
			for (i = 0; i < LOOKUP_FIELD_BITS; i++)
				index |= (t[i] << i);
			if (lookup_tbl_flags[index] == ENTRY_USED)
				pr_err("why already used\n");
			else
				lookup_tbl_flags[index] = ENTRY_USED;
		}
		/*save status */
		lookup_match_qid[lookup_match_num] = first_qid;
		lookup_match_mask[lookup_match_num] = 0;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			if (lookup_mask1[i])
				lookup_match_mask[lookup_match_num] |= 1 << i;
		lookup_match_num++;
		DP_DEBUG(DP_DBG_FLAG_LOOKUP,
			 "left_n=%d lookup_mask_n=%d. Need reduce=%d\n",
			 left_n, lookup_mask_n, (1 << lookup_mask_n));
		left_n -= (1 << lookup_mask_n);
	} else {
		/*failed */
	}
}

/*k--number of don't care flags
 */
static int list_care_combination(int tmp_index)
{
	int i, k, index;
	int t[14] = { 0 };

	LIST_ALL_CASES(t, lookup_mask1, CARE_FLAG) {
		index = tmp_index;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			index |= (t[i] << i);
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "care index=%x\n", index);
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "care t[14]=");
		for (k = 0; k < LOOKUP_FIELD_BITS; k++)
			DP_DEBUG(DP_DBG_FLAG_LOOKUP, "%d ", t[k]);
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "\n");
		lookup_pattern_match(index);
	}

	return 0;
}

/*based on the don't care list, we try to find the all possible pattern:
 *for example: bit 13 and bit 11 don't care.
 *data---the flag index list which is don't care
 *r -- the flag index length
 */
static int check_pattern(int *data, int r)
{
	int i;

	memset(lookup_mask1, 0, sizeof(lookup_mask1));
	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "data:");
	for (i = 0; i < r; i++) {
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "%d ", data[i]);
		lookup_mask1[data[i]] = CARE_NOT_FLAG;
	}
	lookup_mask_n = r;
	pattern_match_flag = 0;
	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "\n");

	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "Don't care flag: ");
	for (i = 0; i < LOOKUP_FIELD_BITS; i++)
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "%c ",
			 lookup_mask1[i] ? 'x' : '0');
	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "\n");

	list_care_combination(tmp_pattern_port_id << EP_BIT);
	return 0;
}

static void print_title(struct seq_file *s, int ep, int mode)
{
	proc_printf(s,
		    "EP%-2d:%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%s%d\n",
		    ep,
		    "EG", "EP3",
		    "EP2", "EP1", "EP0",
		    "S7", "S6", "S5", "S4", "S3", "S2", "S1", "S0",
		    "qid", "mode", "=", mode);
	if (mode == 0)
		proc_printf(s,
			    "%7s subif[13-8] class[1-0]\n", "");
	else if (mode == 1)
		proc_printf(s,
			    "%7s subif[7-0]\n", "");
	else if (mode == 2)
		proc_printf(s,
			    "%7s subif[11-8] class[3-0]\n", "");
	else if (mode == 3)
		proc_printf(s,
			    "%7s subif[4-0] class[2-0]\n", "");
	else if (mode == 4)
		proc_printf(s,
			    "%7s class[1-0] subif[5-0]\n", "");
	else if (mode == 5)
		proc_printf(s,
			    "%7s subif[15-8]\n", "");
	else if (mode == 6)
		proc_printf(s,
			    "%7s subif[1-0] class[3-0] color[1-0]\n", "");
	else if (mode == 7)
		proc_printf(s,
			    "%7s subif[14-8] class[0]\n", "");
}

/*qid: -1: match all queues
 *      >=0: only match the specified queue
 */
int find_pattern(int port_id, struct seq_file *s, int qid)
{
	int r, i, j, n;
	int f = 0;
	char *flag_s;
	char flag_buf[40];
	int deq_port;
	int arr[] = {12, /*11, 10, 9, 8,*/ 7, 6, 5, 4, 3, 2, 1, 0 };
	int inst = 0;
	struct hal_priv *priv = (struct hal_priv *)dp_port_prop[inst].priv_hal;
	u32 mode = get_dp_port_info(inst, port_id)->cqe_lu_mode;

	left_n = 1 << (LOOKUP_FIELD_BITS - 4); /* maximum lookup entries */
	lookup_match_num = 0;
	tmp_pattern_port_id = port_id;
	memset(lookup_tbl_flags, 0, sizeof(lookup_tbl_flags));
	n = ARRAY_SIZE(arr);
	/*list all pattern, ie, don't care numbers from 10 to 1 */
	for (r = n; r >= 0; r--) {
		if (left_n <= 0)
			break;
		c_not_care_walkthrought(arr, n, r);
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "left_n=%d\n", left_n);
		if (!left_n)
			break;
	}

	for (i = 0; i < lookup_match_num; i++) {
		if ((qid >= 0) && (qid != lookup_match_qid[i]))
			continue;
		if (!f) {
			f = 1;
			print_title(s, tmp_pattern_port_id, mode);
		}
		deq_port = priv->qos_queue_stat[lookup_match_qid[i]].deq_port;
		flag_s = get_dma_flags_str32(deq_port, flag_buf,
					     sizeof(flag_buf));

		if (lookup_match_qid[i] != priv->ppv4_drop_q) {
			proc_printf(s, "    ");
			for (j = LOOKUP_FIELD_BITS - 1; j >= 0; j--) {
				if ((lookup_match_mask[i] >> j) & 1)
					proc_printf(s, "%5c", 'x');
				else
					proc_printf(s, "%5d",
						    (lookup_match_index[i] >> j)
									& 1);
			}

			proc_printf(s, "  ->%-3d(0x%04x)%s\n",
				    lookup_match_qid[i],
				    lookup_match_index[i], flag_s);
		}
	}
	if (s && seq_has_overflowed(s))
		return -1;

	return 0;
}

int lookup_start32(void)
{
	return 0;
}

int lookup_dump32(struct seq_file *s, int pos)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (find_pattern(pos, s, -1) < 0)
		return pos;
	pos++;
	if (pos >= 16)
		pos = -1;
	return pos;
}

ssize_t proc_get_qid_via_index32(struct file *file, const char *buf,
				 size_t count, loff_t *ppos)
{
	int err = 0, len = 0;
	char data[100];
	unsigned int lookup_index;
	unsigned int qid = 0;
	char *param_list[10];
	int num;
	struct cbm_lookup lookup;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	len = (count >= sizeof(data)) ? (sizeof(data) - 1) : count;
	DP_DEBUG(DP_DBG_FLAG_LOOKUP, "len=%d\n", len);

	if (len <= 0) {
		err = -EFAULT;
		pr_err("Wrong len value (%d)\n", len);
		return count;
	}

	if (copy_from_user(data, buf, len)) {
		err = -EFAULT;
		pr_err("copy_from_user fail");
		return count;
	}

	data[len - 1] = 0; /* Make string */
	num = dp_split_buffer(data, param_list, ARRAY_SIZE(param_list));

	if (num <= 1)
		goto help;
	if (!param_list[1])
		goto help;

	lookup_index = dp_atoi(param_list[1]);

	if ((dp_strncmpi(param_list[0], "set", strlen("set")) == 0) ||
	    (dp_strncmpi(param_list[0], "write", strlen("write")) == 0)) {
		if (!param_list[2]) {
			pr_err("wrong command\n");
			return count;
		}
		qid = dp_atoi(param_list[2]);
		/*workaround for mask support */
		if (get_dont_care_lookup(param_list[1]) == 0) {
			lookup_table_recursive(LOOKUP_FIELD_BITS - 1, 0, 1,
					       qid);
			return count;
		}
		pr_info("Set to queue[%u] done\n", qid);
		lookup.index = lookup_index;
		lookup.qid = qid;
		dp_set_lookup_qid_via_index(&lookup);
		return count;
	} else if ((dp_strncmpi(param_list[0], "get", strlen("get")) == 0) ||
		   (dp_strncmpi(param_list[0], "read", strlen("read")) == 0)) {
		if (get_dont_care_lookup(param_list[1]) == 0) {
			lookup_table_recursive(LOOKUP_FIELD_BITS - 1, 0, 0,
					       0);
			return count;
		}
		lookup.index = lookup_index;
		qid = dp_get_lookup_qid_via_index(&lookup);
		pr_info("Get lookup[%05u 0x%04x] ->     queue[%u]\n",
			lookup_index, lookup_index, qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "find",
		   strlen("find") + 1) == 0) {
		/*read out its all flags for specified qid */
		int i;

		qid = dp_atoi(param_list[1]);
		for (i = 0; i < 16; i++)
			find_pattern(i, NULL, qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "find2",
		   strlen("find2") + 1) == 0) {
		/*read out its all flags for specified qid */
		qid = dp_atoi(param_list[1]);
		lookup_table_via_qid(qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "remap", strlen("remap")) == 0) {
		int old_q = dp_atoi(param_list[1]);
		int new_q = dp_atoi(param_list[2]);

		lookup_table_remap(old_q, new_q);
		pr_info("remap queue[%d] to queue[%d] done\n",
			old_q, new_q);
		return count;
	}  else if (dp_strncmpi(param_list[0], "test",
		   strlen("test") + 1) == 0) {
		cbm_queue_map_entry_t lookup = {0};
		int inst = 0;
		int qid = dp_atoi(param_list[1]);
		u32 flag = CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			   CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			   CBM_QUEUE_MAP_F_SUBIF_DONTCARE |
			   CBM_QUEUE_MAP_F_EN_DONTCARE |
			   CBM_QUEUE_MAP_F_DE_DONTCARE |
			   CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			   CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			   CBM_QUEUE_MAP_F_TC_DONTCARE |
			   CBM_QUEUE_MAP_F_COLOR_DONTCARE;

		lookup.egflag = 0;
		lookup.ep = 3;
		cbm_queue_map_set(dp_port_prop[inst].cbm_inst, qid,
				  &lookup, flag);
		pr_info("cbm_queue_map_set ep=%d egflag=%d qid=%d flag=0x%x\n",
			lookup.ep, lookup.egflag, qid, flag);
		lookup.egflag = 1;
		cbm_queue_map_set(dp_port_prop[inst].cbm_inst, qid,
				  &lookup, flag);
		pr_info("cbm_queue_map_set ep=%d egflag=%d qid=%d flag=0x%x\n",
			lookup.ep, lookup.egflag, qid, flag);
		return count;
	}
	goto help;
help:
	pr_info("Usage: echo set lookup_flags queue_id > /proc/dp/lookup\n");
	pr_info("     : echo get lookup_flags > /proc/dp/lookup\n");
	pr_info("     : echo find  <x> > /proc/dp/lookup\n");
	pr_info("     : echo find2 <x> > /proc/dp/lookup\n");
	pr_info("     : echo remap <old_q> <new_q> > /proc/dp/lookup\n");
	pr_info("     : echo test <qid> > /proc/dp/lookup\n");
	pr_info("  Hex example: echo set 0x10 10 > /proc/dp/lookup\n");
	pr_info("  Dec:example: echo set 16 10 > /proc/dp/lookup\n");
	pr_info("  Bin:example: echo set b10000 10 > /proc/dp/lookup\n");

	pr_info("%s: echo set b1xxxx 10 > /proc/dp/lookup\n",
		"Special for BIN(Don't care bit)");
	pr_info("Lookup format:\n");
	pr_info("  Bits Index: | %s\n",
		"13   12 |  11  |  10  |  9   |  8   |7   4 | 3   0 |");
	pr_info("  Fields:     | %s\n",
		"Flow ID | DEC  | ENC  | MPE2 | MPE1 |  EP  | CLASS |");
	return count;
}

void lookup_table_via_qid(int qid)
{
	u32 index, tmp, i, j, k, f = 0;
	struct cbm_lookup lookup;

	DP_DEBUG(DP_DBG_FLAG_LOOKUP,
		 "Try to find all lookup flags mapped to qid %d\n", qid);
	for (i = 0; i < 16; i++) { /*ep: 4 bits */
		for (j = 0; j < 256; j++) { /*selected fields: 8 bits */
			for (k = 0; k < 2; k++) {/*eg flag: 1 bit */
				index = (k << EGFLAG_BIT) | (i << EP_BIT) | j;
				lookup.index = index;
				tmp = dp_get_lookup_qid_via_index(&lookup);
				if (tmp != qid)
					continue;
				f = 1;
				pr_info("Get lookup[%05u 0x%04x]%s[%d]\n",
					index, index,
					" ->     queue", qid);
			}
		}
	}
	if (!f)
		pr_err("No mapping to queue id %d yet ?\n", qid);
}

void lookup_table_remap(int old_q, int new_q)
{
	u32 index, tmp, i, j, k, f = 0;
	struct cbm_lookup lookup;

	DP_DEBUG(DP_DBG_FLAG_LOOKUP,
		 "Try to remap lookup flags mapped from old_q %d to new_q %d\n",
		 old_q, new_q);
	for (i = 0; i < 16; i++) { /* ep */
		for (j = 0; j < 256; j++) { /*  select fields */
			for (k = 0; k < 2; k++) { /* eg flag */
				index = (k << EGFLAG_BIT) | (i << EP_BIT) | j;
				lookup.index = index;
				tmp = dp_get_lookup_qid_via_index(&lookup);
				if (tmp != old_q)
					continue;
				lookup.index = index;
				lookup.qid = new_q;
				dp_set_lookup_qid_via_index(&lookup);
				f = 1;
				DP_DEBUG(DP_DBG_FLAG_LOOKUP,
					 "Remap lookup[%05u 0x%04x] %s[%d]\n",
					 index, index,
					 "->     queue", new_q);
			}
		}
	}
	if (!f)
		pr_info("No mapping to queue id %d yet\n", new_q);
}

static u8 lookup_flags2[LOOKUP_FIELD_BITS];
static u8 lookup_mask2[LOOKUP_FIELD_BITS];

/*return 0: get correct bit mask
 * -1: no
 */
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
	memset(lookup_flags2, 0, sizeof(lookup_flags2));
	memset(lookup_mask2, 0, sizeof(lookup_mask2));
	if ((s[0] != 'b') && (s[0] != 'B'))
		return -1;

	if (len >= LOOKUP_FIELD_BITS + 1)
		len = LOOKUP_FIELD_BITS + 1;
	for (i = len - 1, j = 0; i >= 1; i--, j++) {
		if ((s[i] == 'x') || (s[i] == 'X')) {
			lookup_mask2[j] = 1;
			flag = 1;
		} else if (('0' <= s[i]) && (s[i] <= '9')) {
			lookup_flags2[j] = s[i] - '0';
		} else if (('A' <= s[i]) && (s[i] <= 'F')) {
			lookup_flags2[j] = s[i] - 'A' + 10;
		} else if (('a' <= s[i]) && (s[i] <= 'f')) {
			lookup_flags2[j] = s[i] - '1' + 10;
		} else {
			return -1;
		}
	}
	if (flag) {
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "\nGet lookup flag: ");
		for (i = LOOKUP_FIELD_BITS - 1; i >= 0; i--) {
			if (lookup_mask2[i])
				DP_DEBUG(DP_DBG_FLAG_LOOKUP, "x");
			else
				DP_DEBUG(DP_DBG_FLAG_LOOKUP, "%d",
					 lookup_flags2[i]);
		}
		DP_DEBUG(DP_DBG_FLAG_LOOKUP, "\n");

		return 0;
	} else {
		return -1;
	}
}

void lookup_table_recursive(int k, int tmp_index, int set_flag, int qid)
{
	int i;
	struct cbm_lookup lookup = {0};

	if (k < 0) {	/*finish recursive and start real read/set action */
		if (set_flag) {
			lookup.index = tmp_index;
			lookup.qid = qid;
			dp_set_lookup_qid_via_index(&lookup);
			DP_DEBUG(DP_DBG_FLAG_LOOKUP,
				 "Set lookup[%05u/0x%04x] ->     queue[%d]\n",
				 tmp_index, tmp_index, qid);
		} else {
			lookup.index = tmp_index;
			qid = dp_get_lookup_qid_via_index(&lookup);
			pr_info("Get lookup[%05u/0x%04x] ->     queue[%d]\n",
				tmp_index, tmp_index, qid);
		}
		return;
	}

	if (lookup_mask2[k]) {
		for (i = 0; i < 2; i++)
			lookup_table_recursive(k - 1, tmp_index + (i << k),
					       set_flag, qid);
		return;
	}

	lookup_table_recursive(k - 1, tmp_index + (lookup_flags2[k] << k),
			       set_flag, qid);
}

