/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

For licensing information, see the file 'LICENSE' in the root folder of
this software module.
******************************************************************************/




#ifndef _SW_MCAST_H_
#define _SW_MCAST_H_

enum _ret {
	FAIL = -8,
	NO_VALID_HASHENTRY = -7,
	TABLE_ERROR = -6,
	TBL_EMPTY = -5,
	HASH_CAL_FAIL = -4,
	TBL_FULL = -3,
	MATCH_NOT_FOUND = -2,
	ENTRY_CANNOT_REMOVE = -1,
	MATCH_FOUND = 0,
	REMOVED_ENTRY = 1,
	ENTRY_FOUND = 2,
	UPDATED_BR_PMAP = 3,
};

typedef struct mcast_hash_key {
	GSW_IP_t dstip;        /*!< destination IP address */
	GSW_IP_t srcip;        /*!< source IP address */
	u8 fid;
} MCAST_HASHTBL_PTN;

typedef struct mcast_action_ptr {
	u16 br_portmap[16];
	u16 subifid;
} MCAST_HASHTBL_ACT;

typedef struct compare_hash {
	u32 idx;
	u32 ip_type;
	u32 valid;
	u32 first_idx;   /*!< first valid compare entry index for this hash index*/
	u32 nxt_idx;       /*!< next compare entry index in this hash index */
	GSW_IGMP_MemberMode_t src_ip_mode;
	u32 excl_src_ip;
	MCAST_HASHTBL_PTN key;
	MCAST_HASHTBL_ACT action;
} MCAST_HASHTBL;

#define MAX_STEP_CNTR 16

#define MCAST_HASHTBL_SIZE sizeof(MCAST_HASHTBL)

#define MCAST_TABLE_SIZE	512

#define MCAST_HWSNOOP_EN	1
#define MCAST_HWSNOOP_DIS	0

#define SET 1
#define CLR 0

int gsw_init_hash_table(void *cdev);
int gsw_insert_hashtable_entry(void *cdev, GSW_multicastTable_t *parm);
int gsw_search_hashtable_entry(void *cdev, GSW_multicastTable_t *parm, GSW_multicastTableRead_t *read_parm, u32 *loc);
int gsw_remove_hashtable_entry(void *cdev, GSW_multicastTable_t *parm);
int gsw_check_hashtable_entry(void *cdev);
int gsw_get_swmcast_entry(void *cdev, GSW_multicastTableRead_t *parm, u32 loc);
//void print_table();

#endif

