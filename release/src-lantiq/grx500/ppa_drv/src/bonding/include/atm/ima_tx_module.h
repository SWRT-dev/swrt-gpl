
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_TX_MODULE_H__
#define __IMA_TX_MODULE_H__

#include <linux/interrupt.h>
#include "common/ima_tx_queue.h"
#include "atm/ima_tx_cell_buffer.h"

// To be called from bonding driver's tx-init
s32 init_device_tx_metadata(void *data);

// To be called from bonding driver's tx-device-cleanup
void cleanup_device_tx_metadata(void);

// To be called from bonding driver's tx interrupt handler
void process_tx(void);

// To be called from ASM FSM
bool send_asm_cell(u8 *cellbuff, u32 linkid, void (*cb)(u32, bool));

// To be called from ASM FSM
void update_tx_config(bool sid_is_8bit);

// To be called from debug file system
bool set_tx_dbgfs_config(char *input, char *errmsgbuff, u32 errmsgbufflen);
void get_tx_dbgfs_config(char *msgbuff, u32 msgbufflen);

// To be called from ASM FSM
void activate_tx_link(u32 linkid);

// To be called from ASM FSM
void deactivate_tx_link(u32 linkid);

// To be called from IOCTL
bool set_tx_dsl_info(u32 line0rate_bps, u32 line1rate_bps,
                u32 ilvdelay0_us,  u32 ilvdelay1_us,
				u32 tclineid);
// Pending:
// 1. get stats
// 2. ioctl
/**********************************************************************/
// Macros to make bonding driver code UNIT Test friendly

/**********************************************************************/

///////////////////////////////////////////////////////////////////////
// Macros to make bonding driver code independent of kernel or userspace environment


///////////////////////////////////////////////////////////////////////

typedef enum {
	INSUFFICIENT_TX_BUFFER_STACK_BACKPRESSURE,
	PACKETS_OVERWRITTEN,
	EP_TOTAL_DROPPED,
	PACKETS_SEGMENTED,
	MAX_TX_STAT_TYPES
} TxStats_t;

typedef enum {
	EP_TOTAL_XMT,
	EP_TOTAL_SENT,
	NO_EP_FREESPACE_BACKPRESSURE,
	MAX_TX_EP_STAT_TYPES
} TxEpStats_t;

typedef void (*TxStatusCallback_t)(u32, bool);

struct host_txin_desc_cache;
#ifdef TXDELAY
struct shadow_txin_elem {
	u8 *cellptr;
	ktime_t ts;
};
#endif /* TXDELAY */
typedef struct {

	struct {
		u32 start;
		u32 start_phys;
		u32 size; /* Ring circumference */
		u32 head;
		u32 tail;
		u32 count; /* Ring population */
		u32 umtcounter; /* Last value read from host txin umt counter */
	} host_txin_ring;

	struct {
		u32 start;
		u32 start_phys;
		u32 size; /* Ring circumference */
		u32 head;
		u32 tail;
		u32 count; /* Ring population */
	} host_txout_ring;

	bool host_txin_head_cache_is_valid;
	struct host_txin_desc_cache host_txin_head_cache_desc;
	u32 next_sid;

	struct {
		spinlock_t spinlock;
		bool sid_format_is_8bit;
		u32 sid_range;
		bool change; /* to indicate whether tx-module configuration has changed or not */
        bool active[2]; /* to indicate whether tx-linkstatus is selected or not */
        bool usable[2]; /* to indicate whether upstream traffic flow permitted by user or not */
		u32 txin_vsize[2]; /* Virtual size of each TXIN queue. Must be <= actual size */
		u32 linerate[2]; /* Tx Line rates in bps */
		u32 shaper_percentage; /* Shaper rate as a percentage of total line rate. 0%=> No shaper */
		u32 shaper_threshold; /* Maximum possible value of quota */
#ifdef TXDELAY
		u64 txdelay[2]; /* Represented in microsec */
#endif /* TXDELAY */

#ifdef TXRATIO
		u32 txratio[2];
#endif /* TXRATIO */

#ifdef TX_FILL_LEVEL
		u32 fillpoint1[3],
		    fillpoint2[3],
			fillpoint3[3];
#endif /* TX_FILL_LEVEL */

	} config;

	struct {
		u32 size;
		u32 head;
		u32 tail;
		u32 count;
		u32 *ptrs[2*CELL_BUFFER_POOL_SIZE];
	} nonasm_queue;

	struct {
		spinlock_t spinlock;
		u32 size;
		u32 head;
		u32 tail;
		u32 count;
		u32 *ptrs[2*CELL_BUFFER_POOL_SIZE];
		u32 linkids[2*CELL_BUFFER_POOL_SIZE];
		TxStatusCallback_t cb[2*CELL_BUFFER_POOL_SIZE];
	} asm_queue;

	struct {
		bool (*xmt_data_cell_fn)(u8 *);
		u8 retries;
		u32 start;
		u32 timeslots[2];
		u32 prev_link;

#if defined TX_FRAME_PER_LINE || defined TX_FRAME_PER_TIMESLOT
		bool framestart;
#elif defined TXRATIO
		u32 txratio[2];
		u32 counter[2];
#endif /* TXRATIO */

	} sched;

	struct {
		bool disabled;
		u32 rate;   /* shaper-rate in bits per millisecond */
		ktime_t prev_wake; /* last time shaper woke up */
		u32 quota;  /* number of bits remaining to be transmitted in next millisecond */
		u32 threshold;  /* upper limit of quota */
	} shaper;

	struct {
#ifdef __KERNEL__
		spinlock_t spinlock;
#endif
		u32 start;
		u32 start_phys;
		u32 size;
		u32 vsize; /* Virtual size as specified in configuration. It cannot exceed real size */
		u32 head;
		u32 tail;
		u32 count;
		u32 umt_out_addr;
		u32 txout_start;
		u32 txout_start_phys;

#ifdef TXDELAY
		struct shadow_txin_elem *shadow_txin; /* Intermediary TXIN in order to add Tx-Delay */
		u32 xmt; /* Which cell from shadow_txin to be transmitted next */
		u32 xmt_pending_count; /* How many cells within shadow_txin waiting to be transmitted */
		ktime_t next_cell_time; /* Absolute time of transmission of the next cell */
		u64 txdelay; /* Tx Delay for this line, in microsec */
#endif /* TXDELAY */

	} ep[2];

#ifdef TX_PRINT
	u32 dbg;
#endif

	// Statistics & Debug
	struct {
#ifdef __KERNEL__
		spinlock_t     spinlock;
#endif

		u64 counters[MAX_TX_STAT_TYPES];
		u64 epcounters[2][MAX_TX_EP_STAT_TYPES];
		s32 lastsid[2];

#ifdef TX_FILL_LEVEL
		struct {

			u32 point1, /* Lowest threshold. Default: 25% of TXIN */
				point2, /* Mid-level threshold. Default: 50% of TXIN */
				point3; /* Highest threshold. Default: 75% of TXIN */

			/*
			  3D Array of counters=>
				Axis-0: Idx 0: Taken after waking up; Idx 1: Taken during processing; Idx 2: Taken before going to sleep
				Axis-1: Idx 0: Total number of cells within real and shadow TXINs; Idx 1: Shadow cells only; Idx 2: TXIN cells only
				Axis-2: 7 Fill-level Thresholds: 0=>empty; 1=>(0..a); 2=>[a..b); 3=>[b..c); 4=>[c..full); 5=>full; 6=>more than full
				*/
			u64 count[3][3][7];

		} filllevel[3]; /* Idx 0: TXIN-0, Idx 1: TXIN-1, Idx 2: SWQ */

		/* Idx 0: <= 500; Idx 1: <=750; Idx 2: <=1ms; Idx 3:<= 1.5; Idx 4: <= 2;
		   Idx 5: <= 2.5; Idx 6: <= 3; Idx 7 <= 5; Idx 8 <= 10; Idx 9: > 10 */
		u64 elapsedcount[10];

		u32 cbmsize[2]; /* Idx 0: Min Pkt Size, Idx 1: Max Pkt Size */

#endif /* TX_FILL_LEVEL */

	} stats;

} TxBondingPrivData_t;

extern TxBondingPrivData_t g_tx_privdata;

#ifdef __KERNEL__
#define bzero(ptr, len) memset(ptr, 0, len)
#define UPDATE_TX_STATS(field, quantity) \
		spin_lock_bh(&(g_tx_privdata.stats.spinlock)); \
		g_tx_privdata.stats.counters[field] += (quantity); \
		spin_unlock_bh(&(g_tx_privdata.stats.spinlock));

#define UPDATE_TX_EP_STATS(field, link, quantity) \
		spin_lock_bh(&(g_tx_privdata.stats.spinlock)); \
		g_tx_privdata.stats.epcounters[link][field] += (quantity); \
		spin_unlock_bh(&(g_tx_privdata.stats.spinlock));
#else
#define read_host_txin_head_desc(...) NULL
#define read_device_txout_head(...) NULL
#define write_device_txin_tail(...)
#define bzero(ptr, len) memset(ptr, 0, len)
#define UPDATE_TX_EP_STATS(...)
#endif

#endif /* __IMA_TX_MODULE_H__ */
