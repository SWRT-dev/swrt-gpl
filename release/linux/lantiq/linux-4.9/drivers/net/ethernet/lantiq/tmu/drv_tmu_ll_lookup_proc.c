#include "drv_tmu_api.h"
#include "net/drv_tmu_ll.h"
#include "../cqm/grx500/cbm.h"

#define SEQ_PRINTF seq_printf

#define CARE_FLAG      0
#define CARE_NOT_FLAG  1
#if 1
#define LIST_ALL_CASES(t, mask, not_care) \
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
#else
#define LIST_ALL_CASES(t, mask, not_care)  \
	for (t[13] = 0; t[13] < ((mask[13] == not_care) ? 2 : 1); t[13]++) \
	for (t[12] = 0; t[12] < ((mask[12] == not_care) ? 2 : 1); t[12]++) \
	for (t[11] = 0; t[11] < ((mask[11] == not_care) ? 2 : 1); t[11]++) \
	for (t[10] = 0; t[10] < ((mask[10] == not_care) ? 2 : 1); t[10]++) \
	for (t[9] = 0;  t[9] < ((mask[9] == not_care) ? 2 : 1); t[9]++) \
	for (t[8] = 0;  t[8] < ((mask[8] == not_care) ? 2 : 1); t[8]++) \
	for (t[7] = 0;  t[7] < 1; t[7]++) \
	for (t[6] = 0;  t[6] < 1; t[6]++) \
	for (t[5] = 0;  t[5] < 1; t[5]++) \
	for (t[4] = 0;  t[4] < 1; t[4]++) \
	for (t[3] = 0;  t[3] < ((mask[3] == not_care) ? 2 : 1); t[3]++) \
	for (t[2] = 0;  t[2] < ((mask[2] == not_care) ? 2 : 1); t[2]++) \
	for (t[1] = 0;  t[1] < ((mask[1] == not_care) ? 2 : 1); t[1]++) \
	for (t[0] = 0;  t[0] < ((mask[0] == not_care) ? 2 : 1); t[0]++)
#endif

/* The purpose of this file is to find the CBM lookup pattern and print it in the simple way.
   Otherway it may print up to 16K lines in the console to get the lookup setting.
   Lookup table: flow[1] flow[0] dec end mpe2 mpe1 ep(4) class(4)
   Idea: We fixed the EP value during finding lookup setting pattern.
   method:
      1st: to find the possible don't care bit from flow[2]/dec/enc/mpe2/mpe1 and class(4), excluding ep, ie total 10 bits
	       API: c_not_care_walkthrought
		   Note: from big don't care bit number (ie, maximum don't care case) to 1 (minimal don't care case)

	  2nd: generate tmp_index based on care bits
	       API: list_care_conbination

      3rd: based on tmp_index, check whether there is really pattern which meet don't care, ie, mapping to same qid.

*/

#define LOOKUP_FIELD_BITS 14

static int lookup_mask_n;
#define PATTERN_MATCH_INIT  0
#define PATTERN_MATCH_START 1
#define PATTERN_MATCH_FAIL  2
#define PATTERN_MATCH_PASS  3

#define ENTRY_FILLED 0
#define ENTRY_USED   1

static int pattern_match_flag;	/*1--start matching  2--failed, 3---match 0k */
static unsigned char lookup_mask[LOOKUP_FIELD_BITS];

#define C_ARRAY_SIZE  20
static int c_tmp_data[C_ARRAY_SIZE];

/*store result */
#define MAX_PATTERN_NUM 1024
static int lookup_match_num;
static unsigned short lookup_match_mask[MAX_PATTERN_NUM];
static unsigned short lookup_match_index[MAX_PATTERN_NUM];	/*save tmp_index */
static unsigned char lookup_match_qid[MAX_PATTERN_NUM];	/*save tmp_index */

int print_lookup_title;

static int tmp_pattern_port_id;

static int left_n;
static unsigned char lookup_flags[MAX_PATTERN_NUM * 16];	/*10 bits lookup table except 4 bits EP */

void combinationUtil(int *arr, int *data, int start, int end, int index,
		     int r);

int check_pattern(int *data, int r);

/* The main function that prints all combinations of size r*/
/* in arr[] of size n. This function mainly uses combinationUtil()*/
void c_not_care_walkthrought(int *arr, int n, int r)
{
	/* A temporary array data[] to store all combination one by one */

	/* Print all combination using temprary array 'data[]' */
	combinationUtil(arr, c_tmp_data, 0, n - 1, 0, r);
}

/* arr[]  ---> Input Array
   data[] ---> Temporary array to store current combination
   start & end ---> Staring and Ending indexes in arr[]
   index  ---> Current index in data[]
   r ---> Size of a combination to be printed */
void combinationUtil(int *arr, int *data, int start, int end, int index,
		     int r)
{
	int i;

	/* Current combination is ready to be printed, print it */
	if (left_n <= 0)
		return;
	if (index == r) {	/*Find one pattern with specified don't care flag */

		check_pattern(data, r);	/*find a don't care case and need further check */

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
		combinationUtil(arr, data, i + 1, end, index + 1, r);
	}
}

/*Note: when call this API, for those cared bits, its value already set in tmp_index.*/
void lookup_pattern_match(int tmp_index)
{
	int i;
	int qid;
	static int first_qid;
	int t[LOOKUP_FIELD_BITS] = { 0 };
	int index;

	TMU_DEBUG(TMU_DEBUG_MSG,
		  "trying with tmp_index=0x%x with lookup_match_num=%d\n",
		  tmp_index, lookup_match_num);
	pattern_match_flag = PATTERN_MATCH_INIT;
	lookup_match_index[lookup_match_num] = tmp_index;

	LIST_ALL_CASES(t, lookup_mask, CARE_NOT_FLAG) {
		index = tmp_index;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			index |= (t[i] << i);
		TMU_DEBUG(TMU_DEBUG_MSG, "don't care[14]=");
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			TMU_DEBUG(TMU_DEBUG_MSG, "%d ", t[i]);
		TMU_DEBUG(TMU_DEBUG_MSG, "\n");

		TMU_DEBUG(TMU_DEBUG_MSG, "don't care index=%x\n", index);

		if (lookup_flags[index] == ENTRY_USED) {
			pattern_match_flag = PATTERN_MATCH_FAIL;
			goto END;
		}

		qid = get_lookup_qid_via_index(index);

		if (pattern_match_flag == PATTERN_MATCH_INIT) {
			pattern_match_flag = PATTERN_MATCH_START;
			first_qid = qid;
		} else if (first_qid != qid) {
			pattern_match_flag = PATTERN_MATCH_FAIL;
			TMU_DEBUG(TMU_DEBUG_MSG, "first_qid(%d) != qid(%d)\n",
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
		LIST_ALL_CASES(t, lookup_mask, CARE_NOT_FLAG) {
			index = tmp_index;
			for (i = 0; i < LOOKUP_FIELD_BITS; i++)
				index |= (t[i] << i);
			if (lookup_flags[index] == ENTRY_USED)
				PRINTK("why already used \n");
			else
				lookup_flags[index] = ENTRY_USED;
		}
		/*save status */
		lookup_match_qid[lookup_match_num] = first_qid;
		lookup_match_mask[lookup_match_num] = 0;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			if (lookup_mask[i])
				lookup_match_mask[lookup_match_num] |= 1 << i;
		lookup_match_num++;
		TMU_DEBUG(TMU_DEBUG_MSG,
			  "left_n=%d lookup_mask_n=%d. Need reduce=%d\n",
			  left_n, lookup_mask_n, (1 << lookup_mask_n));
		left_n -= (1 << lookup_mask_n);
	} else {
		/*failed */
	}
}

/*k--number of don't care flags
*/
int list_care_conbination(int tmp_index)
{
	int i, k, index;
	int t[14] = { 0 };

	LIST_ALL_CASES(t, lookup_mask, CARE_FLAG) {
		index = tmp_index;
		for (i = 0; i < LOOKUP_FIELD_BITS; i++)
			index |= (t[i] << i);
		TMU_DEBUG(TMU_DEBUG_MSG, "care index=%x\n", index);
		TMU_DEBUG(TMU_DEBUG_MSG, "care t[14]=");
		for (k = 0; k < LOOKUP_FIELD_BITS; k++)
			TMU_DEBUG(TMU_DEBUG_MSG, "%d ", t[k]);
		TMU_DEBUG(TMU_DEBUG_MSG, "\n");
		lookup_pattern_match(index);
	}

	return 0;
}

/*based on the don't care list, we try to find the all possible pattern:
  for example: bit 13 and bit 11 don't care.
  data---the flag index list which is don't care
  r -- the flag index length

  #define LOOKUP_FIELD_BITS 14
u8 lookup_mask[LOOKUP_FIELD_BITS];
*/
int check_pattern(int *data, int r)
{
	int i;

	memset(lookup_mask, 0, sizeof(lookup_mask));
	TMU_DEBUG(TMU_DEBUG_MSG, "data:");
	for (i = 0; i < r; i++) {
		TMU_DEBUG(TMU_DEBUG_MSG, "%d ", data[i]);
		lookup_mask[data[i]] = CARE_NOT_FLAG;
	}
	lookup_mask_n = r;
	pattern_match_flag = 0;
	TMU_DEBUG(TMU_DEBUG_MSG, "\n");

	TMU_DEBUG(TMU_DEBUG_MSG, "Don't care flag: ");
	for (i = 0; i < LOOKUP_FIELD_BITS; i++)
		TMU_DEBUG(TMU_DEBUG_MSG, "%c ", lookup_mask[i] ? 'x' : '0');
	TMU_DEBUG(TMU_DEBUG_MSG, "\n");

	list_care_conbination(tmp_pattern_port_id << 4);
	return 0;
}

/*qid: -1: match all queues
       >=0: only match the specified queue
*/
int find_pattern(int port_id, struct seq_file *s, int qid)
{
	int r, i, j, n;
	int res = 0;
	int f = 0;
	char *flag_s;
	char flag_buf[40];
	struct tmu_equeue_link equeue_link;
	int arr[] = { 13, 12, 11, 10, 9, 8, /*7,6,5,4, */ 3, 2, 1, 0 };	/*remove port id */

	left_n = 1 << (LOOKUP_FIELD_BITS - 4);	/*maximum lookup entried */
	lookup_match_num = 0;
	tmp_pattern_port_id = port_id;
	memset(lookup_flags, 0, sizeof(lookup_flags));
	n = sizeof(arr) / sizeof(arr[0]);
	for (r = n; r >= 0; r--) {	/*list all pattern, ie, don't care numbers from 10 to 1 */
		if (left_n <= 0)
			break;
		c_not_care_walkthrought(arr, n, r);
		TMU_DEBUG(TMU_DEBUG_MSG, "left_n=%d\n", left_n);
		if (!left_n)
			break;
	}

	for (i = 0; i < lookup_match_num; i++) {
		if ((qid >= 0) && (qid != lookup_match_qid[i])) {
			/*tmu_proc_printf(s,"qid(%d) does not match the required one (%d)\n",
			   qid, lookup_match_qid[i]);
			*/
			continue;
		}
		if (!f) {
			f = 1;
			tmu_proc_printf(s,
					"EP%-2d:%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s%5s\n",
					tmp_pattern_port_id, "F2", "F1",
					"DEC", "ENC", "MPE2", "MPE1", "EP3",
					"EP2", "EP1", "EP0", "C3", "C2", "C1",
					"C0", "qid", "id");
		}
		tmu_equeue_link_get(lookup_match_qid[i], &equeue_link);
		flag_s =
		    get_dma_flags_str(equeue_link.epn, flag_buf,
				      sizeof(flag_buf));

		tmu_proc_printf(s, "    ");
		for (j = LOOKUP_FIELD_BITS - 1; j >= 0; j--) {
			if ((lookup_match_mask[i] >> j) & 1)
				tmu_proc_printf(s, "%5c", 'x');
			else
				tmu_proc_printf(s, "%5d",
						(lookup_match_index[i] >> j) &
						1);
		}
		tmu_proc_printf(s, "->%-3d(0x%04x)%s\n", lookup_match_qid[i],
				lookup_match_index[i], flag_s);
	}
	if (s)
		SEQ_PRINTF(s, "\n");

	return res;
}
