#include <linux/klogging.h>
#include "cqm_debugfs.h"
#include <net/datapath_proc_api.h>
#include <net/lantiq_cbm_api.h>
#include "cqm.h"
#include "../cqm_common.h"

#define PRINTK  pr_err
#define cqm_dbg_printf(s, fmt, arg...) \
	do { \
		if (!s) \
			printk(fmt, ##arg); \
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
	for (t[4] = 0;  t[4] < 1; t[4]++) \
	for (t[5] = 0;  t[5] < 1; t[5]++) \
	for (t[6] = 0;  t[6] < 1; t[6]++) \
	for (t[7] = 0;  t[7] < 1; t[7]++) \
	for (t[8] = 0;  t[8] < ((mask[8] == not_care) ? 2 : 1); t[8]++) \
	for (t[9] = 0;  t[9] < ((mask[9] == not_care) ? 2 : 1); t[9]++) \
	for (t[10] = 0; t[10] < ((mask[10] == not_care) ? 2 : 1); t[10]++) \
	for (t[11] = 0; t[11] < ((mask[11] == not_care) ? 2 : 1); t[11]++) \
	for (t[12] = 0; t[12] < ((mask[12] == not_care) ? 2 : 1); t[12]++) \
	for (t[13] = 0; t[13] < ((mask[13] == not_care) ? 2 : 1); t[13]++)

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
#define LOOKUP_FIELD_BITS 14
#define CQM_DBG_FLAG_LOOKUP (BIT(21))
#if IS_ENABLED(CONFIG_INTEL_CQM_DBG)
static u32 cqm_max_print_num = -1;
static u32 cqm_print_num_en;
static u32 cqm_dbg_flag;
#define CQM_DEBUG(flags, fmt, arg...)  do { \
if (unlikely((cqm_dbg_flag & (flags)) && (((cqm_print_num_en) && \
	(cqm_max_print_num)) || !cqm_print_num_en))) {\
	pr_info(fmt, ##arg); \
	if ((cqm_print_num_en) && \
	    (cqm_max_print_num)) \
		cqm_max_print_num--; \
	} \
} while (0)
#else
#define CQM_DEBUG(flags, fmt, arg...)
#endif				/* end of CONFIG_INTEL_CQM_DBG */

#define DEBUGFS_LOOKUP "qid_queue_map"

static int lookup_mask_n;
#define PATTERN_MATCH_INIT  0
#define PATTERN_MATCH_START 1
#define PATTERN_MATCH_FAIL  2
#define PATTERN_MATCH_PASS  3

#define ENTRY_FILLED 0
#define ENTRY_USED   1

typedef void (*cqm_dbg_single_callback_t) (struct seq_file *);
typedef int (*cqm_dbg_callback_t) (struct seq_file *, int);
typedef int (*cqm_dbg_init_callback_t) (void);
typedef ssize_t(*cqm_dbg_write_callback_t) (struct file *file,
					     const char __user *input,
					     size_t size, loff_t *loff);

struct cqm_dbg_file_entry {
	cqm_dbg_callback_t multi_callback;
	cqm_dbg_single_callback_t single_callback;
	int pos;
	int single_call_only;
};

struct cqm_dbg_entry {
	char *name;
	cqm_dbg_single_callback_t single_callback;
	cqm_dbg_callback_t multi_callback;
	cqm_dbg_init_callback_t init_callback;
	cqm_dbg_write_callback_t write_callback;
};

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
static unsigned char lookup_tbl_flags[MAX_PATTERN_NUM * 16];

static void combine_util(int *arr, int *data, int start, int end, int index,
			 int r);
static int check_pattern(int *data, int r);
static void lookup_table_via_qid(int qid);
static void lookup_table_remap(int old_q, int new_q);
static int cqm_find_pattern(int port_id, struct seq_file *s, int qid);
static int get_dont_care_lookup(char *s);
static void lookup_table_recursive(int k, int tmp_index, int set_flag, int qid);
static int lookup_start32(void);
static ssize_t cqm_get_qid_queue_map_write(struct file *file,
					   const char __user *buf,
					   size_t count, loff_t *ppos);
static int cqm_dbg_open(struct inode *inode, struct file *file);
static int cqm_qid_queue_map_read(struct seq_file *s, int pos);
static __attribute__((unused)) int cqm_dbg_single_open(struct inode *inode,
						       struct file *file);

/* The main function that prints all combinations of size r*/
/* in arr[] of size n. This function mainly uses combine_util()*/
static int c_not_care_walkthrought(int *arr, int n, int r)
{
	/* A temporary array data[] to store all combination one by one */

	/* Print all combination using temprary array 'data[]' */
	combine_util(arr, c_tmp_data, 0, n - 1, 0, r);
	/* return lookup_match_num here to avoid KW warning */
	return lookup_match_num;
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
	struct cbm_lookup info;

	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
		  "trying with tmp_index=0x%x with lookup_match_num=%d\n",
		  tmp_index, lookup_match_num);
	pattern_match_flag = PATTERN_MATCH_INIT;
	lookup_match_index[lookup_match_num] = tmp_index;

	LIST_ALL_CASES(t, lookup_mask1, CARE_NOT_FLAG) {
		index = tmp_index;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			index |= (t[i] << i);
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "don't care[14]=");
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "%d ", t[i]);
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "\n");

		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "don't care index=%x\n", index);

		if (lookup_tbl_flags[index] == ENTRY_USED) {
			pattern_match_flag = PATTERN_MATCH_FAIL;
			goto END;
		}
		info.index = index;
		qid = get_lookup_qid_via_index(&info);

		if (pattern_match_flag == PATTERN_MATCH_INIT) {
			pattern_match_flag = PATTERN_MATCH_START;
			first_qid = qid;
		} else if (first_qid != qid) {
			pattern_match_flag = PATTERN_MATCH_FAIL;
			CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
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
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
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
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "care index=%x\n", index);
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "care t[14]=");
		for (k = 0; k < LOOKUP_FIELD_BITS; k++)
			CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "%d ", t[k]);
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "\n");
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
	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "data:");
	for (i = 0; i < r; i++) {
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "%d ", data[i]);
		lookup_mask1[data[i]] = CARE_NOT_FLAG;
	}
	lookup_mask_n = r;
	pattern_match_flag = 0;
	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "\n");

	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "Don't care flag: ");
	for (i = 0; i < LOOKUP_FIELD_BITS; i++)
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "%c ",
			  lookup_mask1[i] ? 'x' : '0');
	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "\n");

	list_care_combination(tmp_pattern_port_id << 4);
	return 0;
}

/*qid: -1: match all queues
 *      >=0: only match the specified queue
 */
int cqm_mode_mask0[] = {13, 12, 11, 10, 9, 8,/*7, 6, 5, 4,*/ 3, 2, 1, 0 };
int cqm_mode_mask1[] = {13, 12, 11, 10, 9, 8,/* 7, 6, 5, 4,*/ 3, 2, 1, 0 };
int cqm_mode_mask2[] = {13, 12, 11, 10, 9, 8, /*7, 6, 5, 4, */3, 2, 1, 0 };
int cqm_mode_mask3[] = {13, 12, 11, 10, 9, 8, /*7, 6, 5, 4, 3 , */2, 1, 0 };
static void cqm_print_title(struct seq_file *s, int ep, int mode, int qid)
{
	if (mode == 0)
		cqm_dbg_printf(s,
			       "%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%s%d\n",
			       "F2", "F1",
			       "DEC", "ENC", "MPE2", "MPE1", "EP3",
			       "EP2", "EP1", "EP0", "CLS3", "CLS2", "CLS1",
			       "CLS0", "qid", "id", "mode", "=", mode);

	else if (mode == 1)
		cqm_dbg_printf(s,
			       "%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%s%d\n",
			       "S7", "S6",
			       "S5", "S4", "MPE2", "MPE1", "EP3",
			       "EP2", "EP1", "EP0", "S3", "S2", "S1",
			       "S0", "qid", "id", "mode", "=", mode);

	else if (mode == 2)
		cqm_dbg_printf(s,
			       "%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%s%d\n",
			       "S11", "S10",
			       "S9", "S8", "MPE2", "MPE1", "EP3",
			       "EP2", "EP1", "EP0", "CLS3", "CLS2", "CLS1",
			       "CLS0", "qid", "id", "mode", "=", mode);

	else if (mode == 3)
		cqm_dbg_printf(s,
			       "%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%s%d\n",
			       "S4", "S3",
			       "S2", "S1", "MPE2", "MPE1", "EP3",
			       "EP2", "EP1", "EP0", "S0", "CLS2", "CLS1",
			       "CLS0", "qid", "id", "mode", "=", mode);
	else
		return;
}

static int cqm_find_pattern(int port_id, struct seq_file *s, int qid)
{
	int r, i, j, n;
	int f = 0;
	cbm_queue_map_entry_t entry;
	int *arr = NULL;
	int ret, data_index;
	char data_str[14];
	u32 mode;
	int match = 0;

	entry.ep = port_id;
	ret = cqm_mode_table_get(0, &mode, &entry, 0);
	if (ret != 0)
		return ret;

	if (mode == 0) {
		arr = cqm_mode_mask0;
		n = ARRAY_SIZE(cqm_mode_mask0);
	} else if (mode == 1) {
		arr = cqm_mode_mask1;
		n = ARRAY_SIZE(cqm_mode_mask1);
	} else if (mode == 2) {
		arr = cqm_mode_mask2;
		n = ARRAY_SIZE(cqm_mode_mask2);
	} else if (mode == 3) {
		arr = cqm_mode_mask3;
		n = ARRAY_SIZE(cqm_mode_mask3);
	} else {
		return -1;
	}

	left_n = 1 << (LOOKUP_FIELD_BITS - 4);	/*maximum lookup entried */
	lookup_match_num = 0;
	tmp_pattern_port_id = port_id;
	memset(lookup_tbl_flags, 0, sizeof(lookup_tbl_flags));
	/*list all pattern, ie, don't care numbers from 10 to 1 */
	for (r = n; r >= 0; r--) {
		if (left_n <= 0)
			break;
		match = c_not_care_walkthrought(arr, n, r);
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "left_n=%d\n", left_n);
		if (!left_n)
			break;
	}

	for (i = 0; i < match; i++) {
		if (((qid >= 0) && (qid != lookup_match_qid[i])) ||
		    (!lookup_match_qid[i]))
			continue;
		if (!f) {
			f = 1;
			cqm_print_title(s, tmp_pattern_port_id,
					mode, qid);
		}
		data_index = 0;
		for (j = LOOKUP_FIELD_BITS - 1; j >= 0; j--) {
			if ((lookup_match_mask[i] >> j) & 1)
				data_str[data_index] = 'x';
			else
				data_str[data_index]
					= ((lookup_match_index[i] >> j) & 1)
					+ '0';
			data_index++;
		}
		cqm_dbg_printf(s,
			       "%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c  %-3d(0x%04x)\n",
			       data_str[0], data_str[1], data_str[2],
			       data_str[3], data_str[4], data_str[5],
			       data_str[6], data_str[7], data_str[8],
			       data_str[9], data_str[10], data_str[11],
			       data_str[12], data_str[13],
			       lookup_match_qid[i],
			       lookup_match_index[i]);
	}

	if (s && seq_has_overflowed(s))
		return -1;

	return 0;
}

ssize_t cqm_get_qid_queue_map_write(struct file *file, const char __user *buf,
				    size_t count, loff_t *ppos)
{
	size_t len;
	char data[100];
	unsigned int lookup_index;
	unsigned int qid = 0;
	char *param_list[10];
	int num;
	struct cbm_lookup cbm_lu = {0};
	struct cbm_lookup info;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	len = (count >= sizeof(data)) ? (sizeof(data) - 1) : count;
	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "len=%d\n", len);
	len -= copy_from_user(data, buf, len);
	data[len] = 0; /* Make string */
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
		printk("Set to queue[%u] done\n", qid);
		cbm_lu.qid = qid;
		cbm_lu.index = lookup_index;
		set_lookup_qid_via_index(&cbm_lu);
		return count;
	} else if ((dp_strncmpi(param_list[0], "get", strlen("get")) == 0) ||
		   (dp_strncmpi(param_list[0], "read", strlen("read")) == 0)) {
		if (get_dont_care_lookup(param_list[1]) == 0) {
			lookup_table_recursive(LOOKUP_FIELD_BITS - 1, 0, 0,
					       0);
			return count;
		}
		info.index = lookup_index;
		qid = get_lookup_qid_via_index(&info);
		cqm_dbg_printf(NULL,
			       "Get lookup[%05u 0x%04x] ->     queue[%u]\n",
			       lookup_index, lookup_index, qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "find", strlen("find") + 1)
			       == 0) {
		/*read out its all flags for specified qid */
		int i;

		qid = dp_atoi(param_list[1]);
		//for (i = 0; i < 16; i++)
		cqm_find_pattern(i, NULL, qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "find2", strlen("find2") + 1)
			       == 0) {
		/*read out its all flags for specified qid */
		qid = dp_atoi(param_list[1]);
		lookup_table_via_qid(qid);
		return count;
	} else if (dp_strncmpi(param_list[0], "remap", strlen("remap")) == 0) {
		int old_q = dp_atoi(param_list[1]);
		int new_q = dp_atoi(param_list[2]);

		lookup_table_remap(old_q, new_q);
		cqm_dbg_printf(NULL,
			       "remap queue[%d] to queue[%d] done\n",
			       old_q, new_q);
		return count;
	}

	goto help;
help:
	cqm_dbg_printf(NULL,
		       "Usage: echo set lookup_flags queue_id > %s\n",
		       DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL,
		       "     : echo get lookup_flags > %s\n",
		       DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL,
		       "     : echo find  <x> > %s\n", DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL, "     : echo find2 <x> > %s\n", DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL, "     : echo remap <old_q> <new_q> > %s\n",
		       DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL, "  Hex example: echo set 0x10 10 > %s\n",
		       DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL, "  Dec:example: echo set 16 10 > %s\n",
		       DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL, "  Bin:example: echo set b10000 10 > %s\n",
		       DEBUGFS_LOOKUP);

	cqm_dbg_printf(NULL, "%s: echo set b1xxxx 10 > %s\n",
		       "Special for BIN(Don't care bit)", DEBUGFS_LOOKUP);
	cqm_dbg_printf(NULL, "Lookup format:\n");
	cqm_dbg_printf(NULL, "  Bits Index: | %s\n",
		       "13   12 |  11  |  10  |  9   |  8   |7   4 | 3   0 |");
	cqm_dbg_printf(NULL,
		       "  Fields:     | %s\n",
		       "Flow ID | DEC  | ENC  | MPE2 | MPE1 |  EP  | CLASS |");
	return count;
}

void lookup_table_via_qid(int qid)
{
	u32 index, tmp, i, j, k, f = 0;
	struct cbm_lookup info;

	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
		  "Try to find all lookup flags mapped to qid %d\n", qid);

	for (i = 0; i < 16; i++) {	/*ep */
		for (j = 0; j < 16; j++) {	/*class */
			for (k = 0; k < 64; k++) {/*flow id/dec/enc/mpe2/mpe1 */
				index = (k << 8) | (i << 4) | j;
				info.index = index;
				tmp = get_lookup_qid_via_index(&info);
				if (tmp != qid)
					continue;
				f = 1;
				cqm_dbg_printf(NULL,
					       "Get lookup[%05u 0x%04x]%s[%d]\n",
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
	struct cbm_lookup cbm_lu = {0};
	struct cbm_lookup info;

	CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
		  "Try to remap lookup flags mapped from old_q %d to new_q %d\n",
		  old_q, new_q);
	for (i = 0; i < 16; i++) {	/*ep */
		for (j = 0; j < 16; j++) {	/*class */
			for (k = 0; k < 64; k++) {/*flow id/dec/enc/mpe2/mpe1 */
				index = (k << 8) | (i << 4) | j;
				info.index = index;
				tmp = get_lookup_qid_via_index(&info);
				if (tmp != old_q)
					continue;
				cbm_lu.qid = new_q;
				cbm_lu.index = index;
				set_lookup_qid_via_index(&cbm_lu);
				f = 1;
				CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
					  "Remap lookup[%05u 0x%04x] %s[%d]\n",
					  index, index,
					  "->     queue", new_q);
			}
		}
	}
	if (!f)
		pr_info("No mapping to queue id %d yet\n", new_q);
}

#define LOOKUP_FIELD_BITS 14
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
		CQM_DEBUG(DP_DBG_FLAG_LOOKUP, "\nGet lookup flag: ");
		for (i = LOOKUP_FIELD_BITS - 1; i >= 0; i--) {
			if (lookup_mask2[i])
				CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "x");
			else
				CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "%d",
					  lookup_flags2[i]);
		}
		CQM_DEBUG(CQM_DBG_FLAG_LOOKUP, "\n");

		return 0;
	} else {
		return -1;
	}
}

void lookup_table_recursive(int k, int tmp_index, int set_flag, int qid)
{
	int i;
	struct cbm_lookup cbm_lu = {0};
	struct cbm_lookup info;

	if (k < 0) {	/*finish recursive and start real read/set action */
		if (set_flag) {
			cbm_lu.qid = qid;
			cbm_lu.index = tmp_index;
			set_lookup_qid_via_index(&cbm_lu);
			CQM_DEBUG(CQM_DBG_FLAG_LOOKUP,
				  "Set lookup[%05u/0x%04x] ->     queue[%d]\n",
				  tmp_index, tmp_index, qid);
		} else {
			info.index = tmp_index;
			qid = get_lookup_qid_via_index(&info);
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
	u32 ring_buff_cnt = 0;
	u32 rx_cnt_t = 0;
	u32 tx_cnt_t = 0;
	u32 free_cnt_t = 0;
	u32 alloc_cnt_t = 0;
	u32 isr_free_cnt_t = 0;
	u32 ring_buff_cnt_t = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	seq_puts(s, "Print CQEM debug counters:\n");
	seq_puts(s, "\t\t\trx\t\ttx\t\tfree\t\talloc\t\tring_b\t\tdelta (rx+alloc-tx-free)\n");
	for (i = 0; i < CQM_MAX_POLICY_NUM; i++) {
		for (j = 0; j < CQM_MAX_POOL_NUM; j++) {
			if (i == CQM_CPU_POLICY && j == CQM_CPU_POOL)
				continue;
			rx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].rx_cnt);
			tx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].tx_cnt);
			free_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].free_cnt);
			alloc_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].alloc_cnt);
			ring_buff_cnt_t = STATS_GET(cqm_dbg_cntrs[i]
						    [j].dma_ring_buff_cnt);

			if (rx_cnt_t || tx_cnt_t || free_cnt_t ||
			    alloc_cnt_t) {
				seq_printf(s, "policy (%d) pool( %d ):", i, j);

				seq_printf(s, "\t0x%08x", rx_cnt_t);
				seq_printf(s, "\t0x%08x", tx_cnt_t);
				seq_printf(s, "\t0x%08x", free_cnt_t);
				seq_printf(s, "\t0x%08x", alloc_cnt_t);
				seq_printf(s, "\t0x%08x", ring_buff_cnt_t);

				rx_cnt += rx_cnt_t;
				tx_cnt += tx_cnt_t;
				free_cnt += free_cnt_t;
				alloc_cnt += alloc_cnt_t;
				ring_buff_cnt += ring_buff_cnt_t;
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
	seq_printf(s, "\t0x%08x", ring_buff_cnt);
	val = (rx_cnt + alloc_cnt - tx_cnt - free_cnt);

	seq_printf(s, "\t%d\n", val);
	i = CQM_CPU_POLICY;
	j = CQM_CPU_POOL;
	seq_puts(s, "\nPrint CQEM cpu buffer debug counters:\n");
	seq_puts(s, "\t\t\trx\t\ttx\t\tfree\t\talloc\t\tring_b\t\tisr_free\tdelta (rx+alloc+tx+free-isr_free)\n");
	rx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].rx_cnt);
	tx_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].tx_cnt);
	free_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].free_cnt);
	alloc_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].alloc_cnt);
	ring_buff_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].dma_ring_buff_cnt);
	isr_free_cnt_t = STATS_GET(cqm_dbg_cntrs[i][j].isr_free_cnt);
	seq_printf(s, "policy (%d) pool( %d ):", i, j);
	seq_printf(s, "\t0x%08x", rx_cnt_t);
	seq_printf(s, "\t0x%08x", tx_cnt_t);
	seq_printf(s, "\t0x%08x", free_cnt_t);
	seq_printf(s, "\t0x%08x", alloc_cnt_t);
	seq_printf(s, "\t0x%08x", ring_buff_cnt_t);
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
		PRINTK("Help: Please use: cat /sys/kernel/debug/cqm/cqm_ls\n");
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
		PRINTK("echo 0 port > /sys/kernel/debug/cqm/dma_desc");
		PRINTK("to display DESC_IGP register\n");
		PRINTK("If port <= 15 or port >= 4, displays all reg values\n");
		PRINTK("echo 1 port > /sys/kernel/debug/cqm/dma_desc\n");
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
		PRINTK("Help: cat /sys/kernel/debug/cqm/ctrl\n");
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
	void *pib_base = cqm_get_pib_base();
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
		PRINTK("Name: DQPC_PON_EGP_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       pib_base + CQM_PON_CNTR(j));
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(pib_base + CQM_PON_CNTR(j)));
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
		PRINTK("Name: DQPC_PON_EGP_%02d\n", j);
		PRINTK("Addr: 0x%8p\n",
		       pib_base + CQM_PON_CNTR(j));
		PRINTK("Val:  0x%08x\n",
		       cbm_r32(pib_base + CQM_PON_CNTR(j)));
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
		PRINTK("echo port_num > /sys/kernel/debug/cqm/deq\n");
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
	PRINTK("echo port_num > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq ctrl registers try\n");
	PRINTK("echo ctrl > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq den registers try\n");
	PRINTK("echo den > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq ovh registers try\n");
	PRINTK("echo ovh > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq threshold enable registers try\n");
	PRINTK("echo thres_en > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq IP occupancy registers try\n");
	PRINTK("echo ip_occ > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq HIGH watermark registers try\n");
	PRINTK("echo hwm > /sys/kernel/debug/cqm/enq\n");
	PRINTK("To display CQM enq HIGH watermark registers try\n");
	PRINTK("echo lwm > /sys/kernel/debug/cqm/enq\n");
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
		PRINTK("echo port > /sys/kernel/debug/cqm/enq\n");
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
		PRINTK("cat /sys/kernel/debug/cqm/dqpc\n");
	return count;
}

static int cqm_dqpc_seq_read(struct seq_file *s, void *v)
{
	void *deq_base = cqm_get_deq_base();
	u32 j;
	void *pon_base = cqm_get_pib_base();

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
	for (j = 26; j <= 88; j = j + 2) {
		seq_printf(s, "Name: DQPC_PON_EGP__%02d  DQPC_PON_EGP__%02d\n",
			   j, j + 1);
		seq_printf(s, "Addr: 0x%8p        0x%8p\n",
			   pon_base + CQM_PON_CNTR(j),
			   pon_base + CQM_PON_CNTR(j + 1));
		seq_printf(s, "Val:  0x%08x        0x%08x\n",
			   cbm_r32(pon_base + CQM_PON_CNTR(j)),
			   cbm_r32(pon_base + CQM_PON_CNTR(j + 1)));
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
		PRINTK("Help: cat /sys/kernel/debug/cqm/eqpc\n");
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

static int cqm_epon_queue_read(struct seq_file *s, void *v)
{
	int startq, endq, base_port;
	u32 epon_mode_reg_value = epon_mode_reg_get();

	startq = epon_mode_reg_value & EPON_EPON_MODE_REG_STARTQ_MASK;
	endq = (epon_mode_reg_value & EPON_EPON_MODE_REG_ENDQ_MASK)
		>> EPON_EPON_MODE_REG_ENDQ_POS;
	base_port = (epon_mode_reg_value
		& EPON_EPON_MODE_REG_EPONBASEPORT_MASK)
		>> EPON_EPON_MODE_REG_EPONBASEPORT_POS;

	seq_puts(s, "EPON_QUEUE_SETTING:\n");
	seq_printf(s, "base port:	%d\n", base_port);
	seq_printf(s, "starting qid:	%d\n", startq);
	seq_printf(s, "total num:	%d\n", 1 + endq - startq);
	return 0;
}

static int cqm_epon_queue_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_epon_queue_read, inode->i_private);
}

static const struct file_operations cqm_epon_queue_fops = {
	.open = cqm_epon_queue_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int cqm_qid_to_ep_read(struct seq_file *s, void *v)
{
	int port, qid_i;

	seq_puts(s, "qid to ep port mapping table\n");
	seq_puts(s, "---------------\n");
	seq_puts(s, "|  qid | port |\n");

	for (qid_i = 0; qid_i < MAX_QOS_QUEUES; qid_i++) {
		cqm_qid2ep_map_get(qid_i, &port);
		if (port != 0) {
			seq_puts(s, "---------------\n");
			seq_printf(s, "| %4u | %4u |\n", qid_i, port);
		}
	}
	return 0;
}

static int cqm_qid_to_ep_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_qid_to_ep_read, inode->i_private);
}

static const struct file_operations cqm_qid_to_ep_fops = {
	.open = cqm_qid_to_ep_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

int lookup_start32(void)
{
	return 0;
}

static int cqm_qid_queue_map_read(struct seq_file *s, int pos)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	if (cqm_find_pattern(pos, s, -1) < 0)
		return pos;
	pos++;
	if (pos >= 16)
		pos = -1;
	return pos;
}

static void *cqm_seq_start(struct seq_file *s, loff_t *pos)
{
	struct cqm_dbg_file_entry *p = s->private;

	if (p->pos < 0)
		return NULL;

	return p;
}

static void *cqm_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct cqm_dbg_file_entry *p = s->private;

	*pos = p->pos;

	if (p->pos >= 0)
		return p;
	else
		return NULL;
}

static void cqm_seq_stop(struct seq_file *s, void *v)
{
}

static int cqm_seq_show(struct seq_file *s, void *v)
{
	struct cqm_dbg_file_entry *p = s->private;

	if (p->pos >= 0) {
		if (p->multi_callback) {
			//pr_info("multiple call");
			p->pos = p->multi_callback(s, p->pos);
		} else if (p->single_callback) {
			//pr_info("single call: %px", p->single_callback);
			p->single_callback(s);
			p->pos = -1;
		}
	}
	return 0;
}

static const struct seq_operations cqm_seq_ops = {
	.start = cqm_seq_start,
	.next = cqm_seq_next,
	.stop = cqm_seq_stop,
	.show = cqm_seq_show
};

void cqm_dummy_single_show(struct seq_file *s)
{
	seq_puts(s, "Cat Not implemented yet !\n");
}

static int cqm_dbg_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	struct cqm_dbg_file_entry *p;
	struct cqm_dbg_entry *entry;
	int ret;

	ret = seq_open(file, &cqm_seq_ops);
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
		p->single_callback = cqm_dummy_single_show;

	if (entry->init_callback)
		p->pos = entry->init_callback();
	else
		p->pos = 0;

	s->private = p;

	return 0;
}

static int cqm_dbg_release(struct inode *inode, struct file *file)
{
	struct seq_file *s;

	s = file->private_data;
	kfree(s->private);

	return seq_release(inode, file);
}

static int cqm_seq_single_show(struct seq_file *s, void *v)
{
	struct cqm_dbg_entry *p = s->private;

	p->single_callback(s);
	return 0;
}

static int cqm_dbg_single_open(struct inode *inode, struct file *file)
{
	return single_open(file, cqm_seq_single_show, inode->i_private);
}

static const struct file_operations cqm_queue_map_lookup_fops = {
	.owner = THIS_MODULE,
	.open = cqm_dbg_open,
	.read = seq_read,
	.write = cqm_get_qid_queue_map_write,
	.llseek = seq_lseek,
	.release = cqm_dbg_release,
};

static struct cqm_dbg_entry cqm_dbg_entries[] = {
	{"qid_queue_map", NULL, cqm_qid_queue_map_read,
		lookup_start32, cqm_get_qid_queue_map_write},
	/*the last place holder */
	{NULL, NULL, NULL, NULL, NULL}
};

int cqm_debugfs_init(struct cqm_ctrl *pctrl)
{
	char cqm_dir[64] = {0};
	struct dentry *file;

	strlcpy(cqm_dir, pctrl->name, sizeof(cqm_dir));
	pctrl->debugfs = debugfs_create_dir(cqm_dir, NULL);
	if (!pctrl->debugfs)
		return -ENOMEM;

	file = debugfs_create_file("eqpc", 0400, pctrl->debugfs,
				   pctrl, &cqm_eqpc_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("dqpc", 0400, pctrl->debugfs,
				   pctrl, &cqm_dqpc_fops);
	if (!file)
		goto err;
	file = debugfs_create_file("enq", 0400, pctrl->debugfs,
				   pctrl, &cqm_enq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("deq", 0400, pctrl->debugfs,
				   pctrl, &cqm_deq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("dma_desc", 0400,
				   pctrl->debugfs, pctrl, &cqm_dma_desc_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("ctrl", 0400, pctrl->debugfs,
				   pctrl, &cqm_ctrl_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("cqm_ls", 0400, pctrl->debugfs,
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

	file = debugfs_create_file("qid_to_ep", 0400, pctrl->debugfs,
				   pctrl, &cqm_qid_to_ep_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("epon_queue", 0400, pctrl->debugfs,
				   pctrl, &cqm_epon_queue_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("qid_queue_map", 0400, pctrl->debugfs,
				   &cqm_dbg_entries[0],
				   &cqm_queue_map_lookup_fops);
	if (!file)
		goto err;

	return 0;
err:
	debugfs_remove_recursive(pctrl->debugfs);
	return -ENOMEM;
}
EXPORT_SYMBOL(cqm_debugfs_init);

/****************debugfs end ***************************/
