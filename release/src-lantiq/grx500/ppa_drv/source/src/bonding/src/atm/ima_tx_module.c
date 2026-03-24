
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <linux/kernel.h>
#include <linux/string.h>
#include "common/atm_bond.h"
#include "common/ima_tx_queue.h"
#include "atm/ima_tx_module.h"
#include "atm/ima_tx_cell_buffer.h"
#include "atm/ima_rx_atm_cell.h"
#include "common/ima_common.h"
#include "common/ima_debug_manager.h"
 
#ifndef PRIV
#define PRIV (&g_tx_privdata)
#endif

#define GET_TIME_IN_MS() jiffies_to_msecs(get_jiffies_64())
#define HOST_TXIN_BUDGET 2
#define SCHED_BUDGET     64
#define CRC_SEED 0xFFFFFFFF

#define AAL5_TRAILER_LEN 8
#define ATM_CELL_PAYLOAD_LEN 48
#define calculate_cell_count(pktlen) \
	(((pktlen + AAL5_TRAILER_LEN - 1) / ATM_CELL_PAYLOAD_LEN) + 1)

#define tc_read_umtcounter(umtptr) get_umt(umtptr)

TxBondingPrivData_t g_tx_privdata;

void poll_new_packets(void);
u32 read_host_txin_head_cache(struct host_txin_desc_cache **buff_desc);
void process_tx_packet(struct host_txin_desc_cache *pkt_desc,
			u32 pktlen, u32 required_cell_count);

#ifdef TXDELAY
bool shadow_transmit_asm_cells(void);
void shadow_transmit_nonasm_cells(void);
static inline void transmit_cells_after_txdelay(void);
#else
bool transmit_asm_cells(void);
void transmit_nonasm_cells(void);
#endif /* TXDELAY */

void release_free_cell_buffers(void);
static inline void reconfigure_tx_scheduler(void);
static inline bool xmt_data_cell_from_link(u8 *cellbuff, u32 linkid);
static bool xmt_data_cell_from_either_link(u8 *cellbuff);
static bool xmt_data_cell_from_link0(u8 *cellbuff);
static bool xmt_data_cell_from_link1(u8 *cellbuff);
static inline bool xmt_cell(u8 *cellbuff, u32 linkid);

s32
init_device_tx_metadata(void *data)
{
	struct bond_drv_cfg_params *cfg = (struct bond_drv_cfg_params *)data;
	init_txbuff_stack();

	PRIV->host_txin_ring.start = cfg->soc_conn.txin.soc_desc_base;
	PRIV->host_txin_ring.start_phys = cfg->soc_conn.txin.soc_desc_base_phys;
	PRIV->host_txin_ring.size = cfg->soc_conn.txin.soc_desc_num;
	PRIV->host_txin_ring.head = 0;
	PRIV->host_txin_ring.tail = 0;
	PRIV->host_txin_ring.count = 0;
	PRIV->host_txin_ring.umtcounter = 0; // Last value read from host txin umt counter

	PRIV->host_txin_head_cache_is_valid = false;
	PRIV->host_txin_head_cache_desc.buff = NULL;
	PRIV->host_txin_head_cache_desc.bufflen = 0;

	PRIV->host_txout_ring.start = cfg->soc_conn.txout.soc_desc_base;
	PRIV->host_txout_ring.start_phys = cfg->soc_conn.txout.soc_desc_base_phys;
	PRIV->host_txout_ring.size = cfg->soc_conn.txout.soc_desc_num;
	PRIV->host_txout_ring.head = 0;
	PRIV->host_txout_ring.tail = 0;
	PRIV->host_txout_ring.count = 0;

	PRIV->config.sid_format_is_8bit = false;
	PRIV->config.sid_range = 1 << (PRIV->config.sid_format_is_8bit ? 8 : 12);

	PRIV->next_sid = 0;

	PRIV->sched.xmt_data_cell_fn = NULL;
	PRIV->nonasm_queue.size = sizeof(PRIV->nonasm_queue.ptrs)/sizeof(PRIV->nonasm_queue.ptrs[0]);
	PRIV->nonasm_queue.count = 0;
	PRIV->nonasm_queue.head = 0;
	PRIV->nonasm_queue.tail = 0;

	spin_lock_init(&PRIV->asm_queue.spinlock);
	PRIV->asm_queue.size = sizeof(PRIV->asm_queue.ptrs)/sizeof(PRIV->asm_queue.ptrs[0]);
	PRIV->asm_queue.count = 0;
	PRIV->asm_queue.head = 0;
	PRIV->asm_queue.tail = 0;

	bzero(PRIV->stats.counters, sizeof(PRIV->stats.counters));
	bzero(PRIV->stats.epcounters, sizeof(PRIV->stats.epcounters));
	PRIV->stats.lastsid[0] = PRIV->stats.lastsid[1] = -1;
#ifdef TX_FILL_LEVEL
	bzero(PRIV->stats.filllevel[0].count, sizeof(PRIV->stats.filllevel[0].count));
	bzero(PRIV->stats.filllevel[1].count, sizeof(PRIV->stats.filllevel[1].count));
	bzero(PRIV->stats.filllevel[1].count, sizeof(PRIV->stats.filllevel[1].count));
	bzero(PRIV->stats.elapsedcount, sizeof(PRIV->stats.elapsedcount));
	PRIV->stats.cbmsize[0] = PRIV->stats.cbmsize[1] = 0;
#endif /* TX_FILL_LEVEL */

	PRIV->sched.retries = 0; /* Total number of retries for the current cell */
	PRIV->sched.start = GET_TIME_IN_MS(); /* Start time of currently active time window */
	PRIV->sched.timeslots[0] = PRIV->sched.timeslots[1] = 50; /* in ms */
	PRIV->sched.prev_link = 0; /* Start with link-0 */

#if defined TX_FRAME_PER_LINE || defined TX_FRAME_PER_TIMESLOT
	PRIV->sched.framestart = true;
#elif defined TXRATIO
	PRIV->config.txratio[0] = 1;
	PRIV->config.txratio[1] = 1;
#endif

	/* Ensure upstream traffic has a slow start (1mbps) until the line-rates get updated */
	PRIV->config.linerate[0] = 500000;
	PRIV->config.linerate[1] = 500000;

	PRIV->config.shaper_percentage = 99;
	PRIV->config.shaper_threshold = 16;

	spin_lock_init(&PRIV->ep[0].spinlock);
	spin_lock_init(&PRIV->ep[1].spinlock);
#ifdef ASM_LOOPBACK
	PRIV->ep[0].start = cfg->ep_conn[0].rxout.soc_desc_base;
	PRIV->ep[1].start = cfg->ep_conn[1].rxout.soc_desc_base;
	PRIV->ep[0].start_phys = cfg->ep_conn[0].rxout.soc_desc_base_phys;
	PRIV->ep[1].start_phys = cfg->ep_conn[1].rxout.soc_desc_base_phys;
	PRIV->ep[0].size = cfg->ep_conn[0].rxout.soc_desc_num;
	PRIV->ep[1].size = cfg->ep_conn[1].rxout.soc_desc_num;
#else
	PRIV->ep[0].start = cfg->ep_conn[0].txin.soc_desc_base;
	PRIV->ep[1].start = cfg->ep_conn[1].txin.soc_desc_base;
	PRIV->ep[0].start_phys = cfg->ep_conn[0].txin.soc_desc_base_phys;
	PRIV->ep[1].start_phys = cfg->ep_conn[1].txin.soc_desc_base_phys;
	PRIV->ep[0].size = cfg->ep_conn[0].txin.soc_desc_num;
	PRIV->ep[1].size = cfg->ep_conn[1].txin.soc_desc_num;
#endif
	PRIV->ep[0].count = PRIV->ep[1].count = 0;
	PRIV->ep[0].head = PRIV->ep[1].head = 0;
	PRIV->ep[0].tail = PRIV->ep[1].tail = 0;
	PRIV->ep[0].umt_out_addr = cfg->aca_umt_hdparams[0][ACA_TXIN];
	PRIV->ep[1].umt_out_addr = cfg->aca_umt_hdparams[1][ACA_TXIN];
#ifdef ASM_LOOPBACK
	PRIV->ep[0].txout_start = cfg->ep_conn[0].rxout.soc_desc_base;
	PRIV->ep[1].txout_start = cfg->ep_conn[1].rxout.soc_desc_base;
	PRIV->ep[0].txout_start_phys = cfg->ep_conn[0].rxout.soc_desc_base_phys;
	PRIV->ep[1].txout_start_phys = cfg->ep_conn[1].rxout.soc_desc_base_phys;
#else
	PRIV->ep[0].txout_start = cfg->ep_conn[0].txout.soc_desc_base;
	PRIV->ep[1].txout_start = cfg->ep_conn[1].txout.soc_desc_base;
	PRIV->ep[0].txout_start_phys = cfg->ep_conn[0].txout.soc_desc_base_phys;
	PRIV->ep[1].txout_start_phys = cfg->ep_conn[1].txout.soc_desc_base_phys;
#endif

	if (PRIV->ep[0].size >= 16)
		PRIV->config.txin_vsize[0] = 16;
	else
		PRIV->config.txin_vsize[0] = PRIV->ep[0].size;
	
	if (PRIV->ep[1].size >= 16)
		PRIV->config.txin_vsize[1] = 16;
	else
		PRIV->config.txin_vsize[1] = PRIV->ep[1].size;

#ifdef TX_FILL_LEVEL
	PRIV->config.fillpoint1[0] = PRIV->config.txin_vsize[0]/4;
	PRIV->config.fillpoint2[0] = PRIV->config.txin_vsize[0]/2;
	PRIV->config.fillpoint3[0] = PRIV->config.txin_vsize[0] * 3/4;
	PRIV->config.fillpoint1[1] = PRIV->config.txin_vsize[1]/4;
	PRIV->config.fillpoint2[1] = PRIV->config.txin_vsize[1]/2;
	PRIV->config.fillpoint3[1] = PRIV->config.txin_vsize[1] * 3/4;
	PRIV->config.fillpoint1[2] = 64;
	PRIV->config.fillpoint2[2] = 128;
	PRIV->config.fillpoint3[2] = 512;

	PRIV->stats.filllevel[0].point1 = PRIV->config.fillpoint1[0];
	PRIV->stats.filllevel[0].point2 = PRIV->config.fillpoint2[0];
	PRIV->stats.filllevel[0].point3 = PRIV->config.fillpoint3[0];
	PRIV->stats.filllevel[1].point1 = PRIV->config.fillpoint1[1];
	PRIV->stats.filllevel[1].point2 = PRIV->config.fillpoint2[1];
	PRIV->stats.filllevel[1].point3 = PRIV->config.fillpoint3[1];
	PRIV->stats.filllevel[2].point1 = PRIV->config.fillpoint1[2];
	PRIV->stats.filllevel[2].point2 = PRIV->config.fillpoint2[2];
	PRIV->stats.filllevel[2].point3 = PRIV->config.fillpoint3[2];
#endif /* TX_FILL_LEVEL */

	PRIV->config.active[0] = PRIV->config.active[1] = false;
	PRIV->config.usable[0] = PRIV->config.usable[1] = true;
	PRIV->sched.xmt_data_cell_fn = NULL;

#ifdef TXDELAY
	PRIV->ep[0].shadow_txin = kmalloc(PRIV->ep[0].size * sizeof(struct shadow_txin_elem), GFP_KERNEL);
	PRIV->ep[1].shadow_txin = kmalloc(PRIV->ep[1].size * sizeof(struct shadow_txin_elem), GFP_KERNEL);
	if ( PRIV->ep[0].shadow_txin == NULL ||
	     PRIV->ep[1].shadow_txin == NULL ) {
		if( PRIV->ep[0].shadow_txin == NULL ) pr_alert( "Bonding driver: Failed to allocate for shadow-txin-0\n" );
		if( PRIV->ep[1].shadow_txin == NULL ) pr_alert( "Bonding driver: Failed to allocate for shadow-txin-1\n" );
		return -1;
	}

	PRIV->ep[0].xmt = PRIV->ep[1].xmt = 0;
	PRIV->ep[0].xmt_pending_count = PRIV->ep[1].xmt_pending_count = 0;
	PRIV->ep[0].txdelay = PRIV->ep[1].txdelay = 0;
	PRIV->ep[0].next_cell_time =  ktime_get();
	PRIV->ep[1].next_cell_time = ktime_get();
	PRIV->config.txdelay[0] = PRIV->config.txdelay[1] = 0;
#endif /* TXDELAY */

	PRIV->config.change = true;

	return 0;
}

void
cleanup_device_tx_metadata(void)
{
	cleanup_txbuff_stack();
}

void
update_tx_config(bool sid_is_8bit)
{
	spin_lock_bh(&PRIV->config.spinlock);
	PRIV->config.sid_format_is_8bit = sid_is_8bit;
	PRIV->config.sid_range = 1 << (sid_is_8bit ? 8 : 12);
	spin_unlock_bh(&PRIV->config.spinlock);
}

u32
gcd( u32 num1, u32 num2 )
{
	u32 i;
	
	if (num1 == 0 || num2 == 0) {
		return 0;
	}

	/* Descending order */
	if (num1 < num2) {
		i = num2;
		num2 = num1;
		num1 = i;
	}

	while( (i = num1 % num2) ) {

		num1 = num2;
		num2 = i;
	}

	return num2;
}

bool
set_tx_dsl_info(u32 line0rate_bps, u32 line1rate_bps,
                u32 ilvdelay0_us,  u32 ilvdelay1_us,
				u32 tclineid)
{

#ifdef TXDELAY
	u32 txdelay[2];
#endif /* TXDELAY */

	spin_lock_bh(&PRIV->config.spinlock);

#ifdef TXDELAY
	txdelay[0] = txdelay[1] = 0;
	if (line0rate_bps && line1rate_bps) {

		if (ilvdelay0_us < ilvdelay1_us) {
			txdelay[0] = ilvdelay1_us - ilvdelay0_us;
		} else {
			txdelay[1] = ilvdelay0_us - ilvdelay1_us;
		}
	}
	PRIV->config.txdelay[0] = txdelay[0];
	PRIV->config.txdelay[1] = txdelay[1];
#endif /* TXDELAY */

	PRIV->config.linerate[0] = line0rate_bps;
	PRIV->config.linerate[1] = line1rate_bps;

#ifdef TXRATIO
	PRIV->config.txratio[0] = line0rate_bps/100;
	PRIV->config.txratio[1] = line1rate_bps/100;
#endif /* TXRATIO */

	PRIV->config.change = true;
	spin_unlock_bh(&PRIV->config.spinlock);

	return true;
}

bool
set_tx_dbgfs_config(char *input, char *usagemsgbuff, u32 usagemsgbufflen)
{
	bool rval = true;
	u32 usable_linkid;
	u32 txin_size[2];
	u32 txin_vsize[2];

#ifdef TXRATIO
	u32 i;
	u32 txratio[2];
#endif /* TXRATIO */

#ifdef TXDELAY
	u32 txdelay[2];
#endif /* TXDELAY */

	u32 shaper_percentage;
	u32 shaper_threshold;

#ifdef TX_FILL_LEVEL
	u32 fillpoint1[3];
	u32 fillpoint2[3];
	u32 fillpoint3[3];
#endif /* TX_FILL_LEVEL */

	spin_lock_bh(&PRIV->ep[0].spinlock);
	txin_size[0] = PRIV->ep[0].size;
	spin_unlock_bh(&PRIV->ep[0].spinlock);
	spin_lock_bh(&PRIV->ep[1].spinlock);
	txin_size[1] = PRIV->ep[1].size;
	spin_unlock_bh(&PRIV->ep[1].spinlock);

#if defined TXRATIO && defined TXDELAY
	if (sscanf(input, "%u %u %u %u %u %u %u %u %u", &usable_linkid, &txin_vsize[0], &txin_vsize[1], &txdelay[0], &txdelay[1],
				&txratio[0], &txratio[1], &shaper_percentage, &shaper_threshold) != 9)
#elif defined TXRATIO && !defined TXDELAY
	if (sscanf(input, "%u %u %u %u %u %u %u", &usable_linkid, &txin_vsize[0], &txin_vsize[1], &txratio[0], &txratio[1],
				&shaper_percentage, &shaper_threshold) != 7)
#elif !defined TXRATIO && defined TXDELAY
	if (sscanf(input, "%u %u %u %u %u %u %u", &usable_linkid, &txin_vsize[0], &txin_vsize[1], &txdelay[0], &txdelay[1],
				&shaper_percentage, &shaper_threshold) != 7)
#else
	if (sscanf(input, "%u %u %u %u %u", &usable_linkid, &txin_vsize[0], &txin_vsize[1], &shaper_percentage, &shaper_threshold) != 5)
#endif
	{

#ifdef TX_FILL_LEVEL
		if (sscanf(input, "fill-level %u %u %u %u %u %u %u %u %u",
				&fillpoint1[0], &fillpoint2[0], &fillpoint3[0],
				&fillpoint1[1], &fillpoint2[1], &fillpoint3[1],
				&fillpoint1[2], &fillpoint2[2], &fillpoint3[2]) == 9) {

			/* Collect the prior invisible parameters from config */

			spin_lock_bh(&PRIV->config.spinlock);

			if (PRIV->config.usable[0]) {

				if (PRIV->config.usable[1])
					usable_linkid = 2;
				else
					usable_linkid = 0;

			} else {

				if (PRIV->config.usable[1])
					usable_linkid = 1;
				else {
					pr_crit( "Bonding driver: Both the lines cannot be configured Tx Unusable\n" );
					goto error;
				}
				
			}

			txin_vsize[0] = PRIV->config.txin_vsize[0];
			txin_vsize[1] = PRIV->config.txin_vsize[1];

#ifdef TXDELAY
			txdelay[0] = PRIV->config.txdelay[0];
			txdelay[1] = PRIV->config.txdelay[1];
#endif /* TXDELAY */

#ifdef TXRATIO
			txratio[0] = PRIV->config.txratio[0] * 100;
			txratio[1] = PRIV->config.txratio[1] * 100;
#endif /* TXRATIO */

			shaper_percentage = PRIV->config.shaper_percentage;
			shaper_threshold = PRIV->config.shaper_threshold;

			spin_unlock_bh(&PRIV->config.spinlock);

		} else {
#endif /* TX_FILL_LEVEL */

		goto error;
	}

#ifdef TX_FILL_LEVEL
	} else {

		/* Set default values of the invisible fill-level points */
		fillpoint1[0] = txin_vsize[0]/4;
		fillpoint2[0] = txin_vsize[0]/2;
		fillpoint3[0] = txin_vsize[0] * 3/4;
		fillpoint1[1] = txin_vsize[1]/4;
		fillpoint2[1] = txin_vsize[1]/2;
		fillpoint3[1] = txin_vsize[1] * 3/4;
		spin_lock_bh(&PRIV->config.spinlock);
		fillpoint1[2] = PRIV->config.fillpoint1[2];
		fillpoint2[2] = PRIV->config.fillpoint2[2];
		fillpoint3[2] = PRIV->config.fillpoint3[2];
		spin_unlock_bh(&PRIV->config.spinlock);
	}
#endif /* TX_FILL_LEVEL */

	if (usable_linkid > 2) {
		rval = false;
	}

	if (txin_vsize[0] < 1 || txin_vsize[0] > txin_size[0]) {
		rval = false;
	}

	if (txin_vsize[1] < 1 || txin_vsize[1] > txin_size[1]) {
		rval = false;
	}

#ifdef TXRATIO
	if (0) {
		/* First reduce the ratio to co-prime */
		i = gcd( txratio[0], txratio[1] );
		if (i > 1) {
			txratio[0] /= i;
			txratio[1] /= i;
		}

		if ( txratio[0] > 20000 || txratio[1] > 20000 ) {
			rval = false;
			pr_crit( "Bonding driver: Neither of the ratio components %u:%u must exceed 20000\n",
				txratio[0], txratio[1] );
		}

	} else {

		/* Assume each ratio component to be individual line rates, reduced by a factor of 100.
		   The reduction is in order to keep the strict-ratio algorithm numbers small. */
		txratio[0] /= 100;
		txratio[1] /= 100;
	}
#endif /* TXRATIO */

#ifdef TX_FILL_LEVEL
	if ( fillpoint1[0] > txin_vsize[0] ) {
		rval = false;
		pr_crit( "Bonding driver: Invalid fillpoint1 for line-0: %d\n", fillpoint1[0] );
	}

	if ( fillpoint2[0] > txin_vsize[0] || fillpoint2[0] < fillpoint1[0] ) {
		rval = false;
		pr_crit( "Bonding driver: Invalid fillpoint2 for line-0: %d\n", fillpoint2[0] );
	}

	if ( fillpoint3[0] > txin_vsize[0] || fillpoint3[0] < fillpoint1[0] || fillpoint3[0] < fillpoint2[0] ) {
		rval = false;
		pr_crit( "Bonding driver: Invalid fillpoint3 for line-0: %d\n", fillpoint3[0] );
	}

	if ( fillpoint1[1] > txin_vsize[1] ) {
		rval = false;
		pr_crit( "Bonding driver: Invalid fillpoint1 for line-1: %d\n", fillpoint1[1] );
	}

	if ( fillpoint2[1] > txin_vsize[1] || fillpoint2[1] < fillpoint1[1] ) {
		rval = false;
		pr_crit( "Bonding driver: Invalid fillpoint2 for line-1: %d\n", fillpoint2[1] );
	}

	if ( fillpoint3[1] > txin_vsize[1] || fillpoint3[1] < fillpoint1[1] || fillpoint3[1] < fillpoint2[1] ) {
		rval = false;
		pr_crit( "Bonding driver: Invalid fillpoint3 for line-1: %d\n", fillpoint3[1] );
	}
#endif /* TX_FILL_LEVEL */

	if (rval) {

		spin_lock_bh(&PRIV->config.spinlock);

		switch(usable_linkid) {
			case 0: PRIV->config.usable[0] = true; PRIV->config.usable[1] = false; break;
			case 1: PRIV->config.usable[0] = false; PRIV->config.usable[1] = true; break;
			case 2: PRIV->config.usable[0] = true; PRIV->config.usable[1] = true; break;
		}

		PRIV->config.txin_vsize[0] = txin_vsize[0];
		PRIV->config.txin_vsize[1] = txin_vsize[1];

#ifdef TXDELAY
		PRIV->config.txdelay[0] = txdelay[0];
		PRIV->config.txdelay[1] = txdelay[1];
#endif /* TXDELAY */

#ifdef TXRATIO
		PRIV->config.txratio[0] = txratio[0];
		PRIV->config.txratio[1] = txratio[1];
#endif /* TXRATIO */

		/* Temporary...*/
		PRIV->config.linerate[0] = txratio[0] * 100;
		PRIV->config.linerate[1] = txratio[1] * 100;

		PRIV->config.shaper_percentage = shaper_percentage;
		PRIV->config.shaper_threshold = shaper_threshold;

#ifdef TX_FILL_LEVEL
		PRIV->config.fillpoint1[0] = fillpoint1[0];
		PRIV->config.fillpoint2[0] = fillpoint2[0];
		PRIV->config.fillpoint3[0] = fillpoint3[0];
		PRIV->config.fillpoint1[1] = fillpoint1[1];
		PRIV->config.fillpoint2[1] = fillpoint2[1];
		PRIV->config.fillpoint3[1] = fillpoint3[1];
		PRIV->config.fillpoint1[2] = fillpoint1[2];
		PRIV->config.fillpoint2[2] = fillpoint2[2];
		PRIV->config.fillpoint3[2] = fillpoint3[2];
#endif /* TX_FILL_LEVEL */

		pr_crit( "BONDING DRIVER: US Traffic: [0:%s,1:%s], TXIN: [0:%u, 1:%u]"
#ifdef TXDELAY
		 			", TxDelay: [0:%llu, 1:%llu] usec"
#endif /* TXDELAY */
#ifdef TXRATIO
		 			", TxRatio: %u:%u"
#endif /* TXRATIO */
		 			", Shaper-percentage:%u%%, Shaper-threshold:%u cells\n",
			PRIV->config.usable[0] ? "Y" : "N", PRIV->config.usable[1] ? "Y" : "N",
			PRIV->config.txin_vsize[0], PRIV->config.txin_vsize[1]
#ifdef TXDELAY
			, PRIV->config.txdelay[0], PRIV->config.txdelay[1]
#endif /* TXDELAY */
#ifdef TXRATIO
			, PRIV->config.txratio[0], PRIV->config.txratio[1]
#endif /* TXRATIO */
			, PRIV->config.shaper_percentage, PRIV->config.shaper_threshold
		);

#ifdef TX_FILL_LEVEL
		pr_crit( "BONDING DRIVER: Fill-level points: "
		         "Line-0:[1:%d, 2:%d, 3:%d], Line-1:[1:%d, 2:%d, 3:%d]"
				 "SwQ:[1:%d, 2:%d, 3:%d]\n",
				 fillpoint1[0], fillpoint2[0], fillpoint3[0],
				 fillpoint1[1], fillpoint2[1], fillpoint3[1],
				 fillpoint1[2], fillpoint2[2], fillpoint3[2] );
#endif /* TX_FILL_LEVEL */

		PRIV->config.change = true;
		spin_unlock_bh(&PRIV->config.spinlock);

		reconfigure_tx_scheduler();

		return true;
	}

error:
#if defined TXRATIO && defined TXDELAY
	snprintf( usagemsgbuff, usagemsgbufflen,
				  "<0|1|2> <[1-%u] (in cells)> <[1-%u] (in cells)> <[0..] (in usec)> <[0..] (in usec)> <line0rate (in bps)> <line1rate (in bps)> <shaper-threshold (in cells)>"
#ifdef TX_FILL_LEVEL
	              "\nOR\nfill-level <line-0:point1> <line-0:point2> <line-0:point3> "
	              "<line-1:point1> <line-1:point2> <line-1:point3> "
#endif /* TX_FILL_LEVEL */
				  , txin_size[0], txin_size[1] );
#elif defined TXRATIO && !defined TXDELAY
	snprintf( usagemsgbuff, usagemsgbufflen,
				  "<0|1|2> <[1-%u] (in cells)> <[1-%u] (in cells)> <line0rate (in bps)> <line1rate (in bps)> <shaper-threshold (in cells)>"
#ifdef TX_FILL_LEVEL
	              "\nOR\nfill-level <line-0:point1> <line-0:point2> <line-0:point3> "
	              "<line-1:point1> <line-1:point2> <line-1:point3> "
#endif /* TX_FILL_LEVEL */
				  , txin_size[0], txin_size[1] );
#elif !defined TXRATIO && defined TXDELAY
	snprintf( usagemsgbuff, usagemsgbufflen,
				  "<0|1|2> <[1-%u] (in cells)> <[1-%u] (in cells)> <[0..] (in usec)> <[0..] (in usec)> <shaper-threshold (in cells)>"
#ifdef TX_FILL_LEVEL
	              "\nOR\necho fill-level <line-0:point1> <line-0:point2> <line-0:point3> "
	              "<line-1:point1> <line-1:point2> <line-1:point3> "
#endif /* TX_FILL_LEVEL */
				  , txin_size[0], txin_size[1] );
#else
	snprintf( usagemsgbuff, usagemsgbufflen,
				  "<0|1|2> <[1-%u] (in cells)> <[1-%u] (in cells)> <shaper-threshold (in cells)>"
#ifdef TX_FILL_LEVEL
	              "\nOR\nfill-level <line-0:point1> <line-0:point2> <line-0:point3> "
	              "<line-1:point1> <line-1:point2> <line-1:point3> "
#endif /* TX_FILL_LEVEL */
				  , txin_size[0], txin_size[1] );
#endif
	return false;
}

void
get_tx_dbgfs_config(char *msgbuff, u32 msgbufflen)
{
	u32 txin_vsize[2];
	u32 usable[2];

#ifdef TXDELAY
	u32 txdelay[2];
#endif /* TXDELAY */

#if defined TXRATIO
	u32 txratio[2];
#endif /* TXRATIO */

	u32 shaper_percentage;
	u32 shaper_threshold;

#ifdef TX_FILL_LEVEL
	u32 fillpoint1[3];
	u32 fillpoint2[3];
	u32 fillpoint3[3];
#endif /* TX_FILL_LEVEL */

	spin_lock_bh(&PRIV->config.spinlock);

	usable[0] = PRIV->config.usable[0];
	usable[1] = PRIV->config.usable[1];
	txin_vsize[0] = PRIV->config.txin_vsize[0];
	txin_vsize[1] = PRIV->config.txin_vsize[1];

#ifdef TXDELAY
	txdelay[0] = PRIV->config.txdelay[0];
	txdelay[1] = PRIV->config.txdelay[1];
#endif /* TXDELAY */

#ifdef TXRATIO
	txratio[0] = PRIV->config.txratio[0];
	txratio[1] = PRIV->config.txratio[1];
#endif /* TXRATIO */

	shaper_percentage = PRIV->config.shaper_percentage;
	shaper_threshold = PRIV->config.shaper_threshold;

#ifdef TX_FILL_LEVEL
	fillpoint1[0] = PRIV->config.fillpoint1[0];
	fillpoint2[0] = PRIV->config.fillpoint2[0];
	fillpoint3[0] = PRIV->config.fillpoint3[0];
	fillpoint1[1] = PRIV->config.fillpoint1[1];
	fillpoint2[1] = PRIV->config.fillpoint2[1];
	fillpoint3[1] = PRIV->config.fillpoint3[1];
	fillpoint1[2] = PRIV->config.fillpoint1[2];
	fillpoint2[2] = PRIV->config.fillpoint2[2];
	fillpoint3[2] = PRIV->config.fillpoint3[2];
#endif /* TX_FILL_LEVEL */

	spin_unlock_bh(&PRIV->config.spinlock);

#if defined TXRATIO && defined TXDELAY
	snprintf( msgbuff, msgbufflen,
				  "usable: [0:%s, 1:%s], buffer vsize: [0:%u, 1:%u], transmission delay: [0:%u, 1:%u], distribution ratio: %u:%u, "
				  "shaper-percentage:%u%%, shaper-threshold: %u cells\n",
				  usable[0] ? "Y" : "N",
				  usable[1] ? "Y" : "N",
				  txin_vsize[0], txin_vsize[1],
				  txdelay[0], txdelay[1],
				  txratio[0], txratio[1],
				  shaper_percentage, shaper_threshold );
#elif defined TXRATIO && !defined TXDELAY
	snprintf( msgbuff, msgbufflen,
				  "usable: [0:%s, 1:%s], buffer vsize: [0:%u, 1:%u], distribution ratio: %u:%u, "
				  "shaper-percentage:%u%%, shaper-threshold: %u cells\n",
				  usable[0] ? "Y" : "N",
				  usable[1] ? "Y" : "N",
				  txin_vsize[0], txin_vsize[1],
				  txratio[0], txratio[1],
				  shaper_percentage, shaper_threshold );
#elif !defined TXRATIO && defined TXDELAY
	snprintf( msgbuff, msgbufflen,
				  "usable: [0:%s, 1:%s], buffer vsize: [0:%u, 1:%u], transmission delay: [0:%u, 1:%u], "
				  "shaper-percentage:%u%%, shaper-threshold: %u cells\n",
				  usable[0] ? "Y" : "N",
				  usable[1] ? "Y" : "N",
				  txin_vsize[0], txin_vsize[1],
				  txdelay[0], txdelay[1],
				  shaper_percentage, shaper_threshold );
#else
	snprintf( msgbuff, msgbufflen,
				  "usable: [0:%s, 1:%s], buffer vsize: [0:%u, 1:%u], ", 
				  "shaper-percentage: %u%%, shaper-threshold: %u cells\n",
				  usable[0] ? "Y" : "N",
				  usable[1] ? "Y" : "N",
				  txin_vsize[0], txin_vsize[1],
				  shaper_percentage, shaper_threshold);
#endif

#ifdef TX_FILL_LEVEL
	snprintf( msgbuff+strlen(msgbuff), msgbufflen - strlen(msgbuff),
				  ", fill-level points: line-0:[1:%d, 2:%d, 3:%d], line-1:[1:%d, 2:%d, 3:%d], swq:[1:%d, 2:%d, 3:%d]\n",
				 fillpoint1[0], fillpoint2[0], fillpoint3[0],
				 fillpoint1[1], fillpoint2[1], fillpoint3[1],
				 fillpoint1[2], fillpoint2[2], fillpoint3[2]);
#endif /* TX_FILL_LEVEL */

	return;
}

static inline void
reconfigure_tx_scheduler(void)
{
	spin_lock_bh(&PRIV->config.spinlock);

	if (PRIV->config.active[0]) {

		if (PRIV->config.active[1]) {

			/* Both links are active. Now to select based on usable-flag */
			if (PRIV->config.usable[0]) {
				if (PRIV->config.usable[1]) {
					PRIV->sched.xmt_data_cell_fn = xmt_data_cell_from_either_link;
				} else {
					PRIV->sched.xmt_data_cell_fn = xmt_data_cell_from_link0;
				}
			} else {
				if (PRIV->config.usable[1]) {
					PRIV->sched.xmt_data_cell_fn = xmt_data_cell_from_link1;
				} else {
					PRIV->sched.xmt_data_cell_fn = NULL;
				}
			}

		} else {
			PRIV->sched.xmt_data_cell_fn = xmt_data_cell_from_link0;
		}

	} else {

		if (PRIV->config.active[1]) {
			PRIV->sched.xmt_data_cell_fn = xmt_data_cell_from_link1;
		} else {
			PRIV->sched.xmt_data_cell_fn = NULL;
		}
	}

	spin_unlock_bh(&PRIV->config.spinlock);
}

void
activate_tx_link(u32 linkid)
{
	spin_lock_bh(&PRIV->config.spinlock);
	PRIV->config.active[linkid] = true;
	spin_unlock_bh(&PRIV->config.spinlock);
	reconfigure_tx_scheduler();
}

void
deactivate_tx_link(u32 linkid)
{
	spin_lock_bh(&PRIV->config.spinlock);
	PRIV->config.active[linkid] = false;
	spin_unlock_bh(&PRIV->config.spinlock);
	PRIV->stats.lastsid[linkid] = -1;
	reconfigure_tx_scheduler();
}

static inline void
release_host_txin_head_cache(struct host_txin_desc_cache *desc)
{
#if 0 /* FIXME: release handling is better? */
	write_host_txout_tail(PRIV->host_txin_head_cache_desc.bufflen,
						  PRIV->host_txin_head_cache_desc.buff,
						  PRIV->host_txin_head_cache_desc.buff_phys);
#else
	release_cbm_buffer((void *)PRIV->host_txin_head_cache_desc.buff_phys);
#endif
	PRIV->host_txin_head_cache_is_valid = false;
}

/*
 * Handles 4 events:
 * 0. Availability of new configuration
 * 1. Availability of free cell buffers
 * 2. Availability of unsent packets in host-txin ring
 * 3. Availability of unsent cells in the 2 software queues
 */

#ifdef TX_FILL_LEVEL
	#define UPDATE_TX_FILLLEVEL_STATS(lineid, when, where, queuemaxsize, value) \
			spin_lock_bh(&PRIV->stats.spinlock); \
			if ((value) > (queuemaxsize)) PRIV->stats.filllevel[(lineid)].count[(when)][(where)][6] += 1; \
			else if ((value) == (queuemaxsize)) PRIV->stats.filllevel[(lineid)].count[(when)][(where)][5] += 1; \
			else if ((value) >= PRIV->stats.filllevel[(lineid)].point3) PRIV->stats.filllevel[(lineid)].count[(when)][(where)][4] += 1; \
			else if ((value) >= PRIV->stats.filllevel[(lineid)].point2) PRIV->stats.filllevel[(lineid)].count[(when)][(where)][3] += 1; \
			else if ((value) >= PRIV->stats.filllevel[(lineid)].point1) PRIV->stats.filllevel[(lineid)].count[(when)][(where)][2] += 1; \
			else if ((value) == 0) PRIV->stats.filllevel[(lineid)].count[(when)][(where)][0] += 1; \
			else PRIV->stats.filllevel[(lineid)].count[(when)][(where)][1] += 1; \
			spin_unlock_bh(&PRIV->stats.spinlock);
#else
	#define UPDATE_TX_FILLLEVEL_STATS(lineid, when, where, queuemaxsize, value)
#endif /* TX_FILL_LEVEL */

void
process_tx(void)
{
	u32 pktlen;
	struct host_txin_desc_cache *pkt_desc;
	u32 required_cell_count;
	int i = 0;
	ktime_t now;
	u32 elapsed;

	bool changedconfig = false;
	u32 config_vsize[2];

#ifdef TXDELAY
	u64 config_txdelay[2];
#endif /* TXDELAY */

#ifdef TX_FILL_LEVEL
	u32 fillpoint1[3];
	u32 fillpoint2[3];
	u32 fillpoint3[3];
	u32 old_txincount[2];
	u32 old_shadowtxincount[2];
	u32 old_nonasmqueuecount;
#endif /* TX_FILL_LEVEL */

	/* Retrieve new configuration, if any */
	spin_lock_bh(&PRIV->config.spinlock);
	if (unlikely(PRIV->config.change)) {

		changedconfig = true;
		config_vsize[0] = PRIV->config.txin_vsize[0];
		config_vsize[1] = PRIV->config.txin_vsize[1];

#ifdef TXDELAY
		config_txdelay[0] = PRIV->config.txdelay[0];
		config_txdelay[1] = PRIV->config.txdelay[1];
#endif /* TXDELAY */

#ifdef TXRATIO
		PRIV->sched.txratio[0] = PRIV->config.txratio[0];
		PRIV->sched.txratio[1] = PRIV->config.txratio[1];
		PRIV->sched.counter[0] = PRIV->sched.counter[1]
			= PRIV->sched.txratio[0] * PRIV->sched.txratio[1];
#endif /* TXRATIO */

		if (PRIV->config.shaper_percentage) {
			PRIV->shaper.disabled = false;
			PRIV->shaper.rate = PRIV->config.shaper_percentage *
								((PRIV->config.linerate[0]
							 	+ PRIV->config.linerate[1])/1000)/100;
			PRIV->shaper.threshold = PRIV->config.shaper_threshold * 53 * 8;
		} else {
			PRIV->shaper.disabled = true;
		}

#ifdef TX_FILL_LEVEL
		fillpoint1[0] = PRIV->config.fillpoint1[0];
		fillpoint2[0] = PRIV->config.fillpoint2[0];
		fillpoint3[0] = PRIV->config.fillpoint3[0];
		fillpoint1[1] = PRIV->config.fillpoint1[1];
		fillpoint2[1] = PRIV->config.fillpoint2[1];
		fillpoint3[1] = PRIV->config.fillpoint3[1];
		fillpoint1[2] = PRIV->config.fillpoint1[2];
		fillpoint2[2] = PRIV->config.fillpoint2[2];
		fillpoint3[2] = PRIV->config.fillpoint3[2];
#endif /* TX_FILL_LEVEL */

		PRIV->config.change = false;
	}
	spin_unlock_bh(&PRIV->config.spinlock);

	/* Apply configuration changes, if any */
	if (unlikely(changedconfig)) {

		spin_lock_bh(&PRIV->ep[0].spinlock);
		PRIV->ep[0].vsize = config_vsize[0] < PRIV->ep[0].size ? config_vsize[0] : PRIV->ep[0].size;
#ifdef TXDELAY
		PRIV->ep[0].txdelay = config_txdelay[0];
#endif /* TXDELAY */
		spin_unlock_bh(&PRIV->ep[0].spinlock);

		spin_lock_bh(&PRIV->ep[1].spinlock);
		PRIV->ep[1].vsize = config_vsize[1] < PRIV->ep[1].size ? config_vsize[1] : PRIV->ep[1].size;
#ifdef TXDELAY
		PRIV->ep[1].txdelay = config_txdelay[1];
#endif /* TXDELAY */
		spin_unlock_bh(&PRIV->ep[1].spinlock);

#ifdef TX_FILL_LEVEL
		spin_lock_bh(&PRIV->stats.spinlock);
		PRIV->stats.filllevel[0].point1 = fillpoint1[0];
		PRIV->stats.filllevel[0].point2 = fillpoint2[0];
		PRIV->stats.filllevel[0].point3 = fillpoint3[0];
		PRIV->stats.filllevel[1].point1 = fillpoint1[1];
		PRIV->stats.filllevel[1].point2 = fillpoint2[1];
		PRIV->stats.filllevel[1].point3 = fillpoint3[1];
		PRIV->stats.filllevel[2].point1 = fillpoint1[2];
		PRIV->stats.filllevel[2].point2 = fillpoint2[2];
		PRIV->stats.filllevel[2].point3 = fillpoint3[2];
		spin_unlock_bh(&PRIV->stats.spinlock);
#endif /* TX_FILL_LEVEL */

	}

	/* Re-evaluate shaper-quota based upon time elapsed since invocation */
	elapsed = ktime_to_us(ktime_sub((now=ktime_get()), PRIV->shaper.prev_wake));
	PRIV->shaper.quota += ((PRIV->shaper.rate * elapsed) / 1000);

	/* Ensure quota does not become burst friendly */
	if (PRIV->shaper.quota > PRIV->shaper.threshold)
		PRIV->shaper.quota = PRIV->shaper.threshold;

	PRIV->shaper.prev_wake = now;

#ifdef TX_FILL_LEVEL
	/* Update time-elapsed slab counters */
	spin_lock_bh(&PRIV->stats.spinlock);
	if (unlikely(elapsed <= 500)) PRIV->stats.elapsedcount[0] += 1;
	else if (elapsed <= 750) PRIV->stats.elapsedcount[1] += 1;
	else if (elapsed <= 1000) PRIV->stats.elapsedcount[2] += 1;
	else if (elapsed <= 1500) PRIV->stats.elapsedcount[3] += 1;
	else if (elapsed <= 2000) PRIV->stats.elapsedcount[4] += 1;
	else if (elapsed <= 2500) PRIV->stats.elapsedcount[5] += 1;
	else if (elapsed <= 3000) PRIV->stats.elapsedcount[6] += 1;
	else if (elapsed <= 5000) PRIV->stats.elapsedcount[7] += 1;
	else if (elapsed <= 10000) PRIV->stats.elapsedcount[8] += 1;
	else if (elapsed > 10000) PRIV->stats.elapsedcount[9] += 1;
	spin_unlock_bh(&PRIV->stats.spinlock);
#endif /* TX_FILL_LEVEL */

		// cell buffer releaser
		release_free_cell_buffers();

#ifdef TX_FILL_LEVEL
		old_txincount[0] = PRIV->ep[0].count;
		old_txincount[1] = PRIV->ep[1].count;
		old_shadowtxincount[0] = PRIV->ep[0].xmt_pending_count;
		old_shadowtxincount[1] = PRIV->ep[1].xmt_pending_count;
		old_nonasmqueuecount = PRIV->nonasm_queue.count;

		/* Update fill-level counters for line-0: */
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/0, /*Both,Shadow or real?*/0, PRIV->ep[0].vsize, PRIV->ep[0].count);

	#ifdef TXDELAY
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/0, /*Both,Shadow or real?*/1, PRIV->ep[0].vsize, PRIV->ep[0].xmt_pending_count);
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/0, /*Both,Shadow or real?*/2, PRIV->ep[0].vsize,
									(PRIV->ep[0].count - PRIV->ep[0].xmt_pending_count));
	#endif /* TXDELAY */

		/* Update fill-level counters for line-1: */
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/0, /*Both,Shadow or real?*/0, PRIV->ep[1].vsize, PRIV->ep[1].count);

	#ifdef TXDELAY
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/0, /*Both,Shadow or real?*/1, PRIV->ep[1].vsize, PRIV->ep[1].xmt_pending_count);
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/0, /*Both,Shadow or real?*/2, PRIV->ep[1].vsize,
									(PRIV->ep[1].count - PRIV->ep[1].xmt_pending_count));
	#endif /* TXDELAY */

		/* Update fill-Level counters for non-ASM SwQ: */
		UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/2, /*Which moment?*/0, /*Both,Shadow or real?*/0, 1024, PRIV->nonasm_queue.count);
#endif /* TX_FILL_LEVEL */

		poll_new_packets();

		for (i = 0; i < HOST_TXIN_BUDGET && PRIV->host_txin_ring.count; ++i) {
			// Start with the first one

			pktlen = read_host_txin_head_cache(&pkt_desc);
			required_cell_count = calculate_cell_count(pktlen);

			//pr_crit("[%s:%d] TX PKT: %u, TX CELL COUNT: %u\n", __func__, __LINE__, pktlen, required_cell_count);

			if (reserve_from_txbuff_stack(required_cell_count)) {
				// Sufficient tx buffers available...

				process_tx_packet(pkt_desc, pktlen,
					required_cell_count);

				// unreserve_from_txbuff_stack(ASM_ASSURANCE_BACKPRESSURE);

				release_host_txin_head_cache(pkt_desc);

				UPDATE_TX_STATS(PACKETS_SEGMENTED, 1);

			} else {

				// Backpressure
				UPDATE_TX_STATS(INSUFFICIENT_TX_BUFFER_STACK_BACKPRESSURE, 1);

				// TODO: retry sending it N number of times, then discard
			}
		}

		// transmission-procedure
#ifdef TXDELAY
		if (!shadow_transmit_asm_cells()) {
			transmit_cells_after_txdelay();
			goto end;
		}

		shadow_transmit_nonasm_cells();

		transmit_cells_after_txdelay();
#else
		if (!transmit_asm_cells()) goto end;

		transmit_nonasm_cells();
#endif /* TXDELAY */

end:

#ifdef TX_FILL_LEVEL
	/* Update filled-level counters for line-0: */
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/1, /*Both,Shadow or real?*/0, PRIV->ep[0].vsize,
				PRIV->ep[0].count - old_txincount[0]);
	#ifdef TXDELAY
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/1, /*Both,Shadow or real?*/1, PRIV->ep[0].vsize, 
				PRIV->ep[0].xmt_pending_count - old_shadowtxincount[0]);
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/1, /*Both,Shadow or real?*/2, PRIV->ep[0].vsize, 
				PRIV->ep[0].count - PRIV->ep[0].xmt_pending_count - old_txincount[0] + old_shadowtxincount[0]);
	#endif /* TXDELAY */

	/* Update filled-level counters for line-1: */
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/1, /*Both,Shadow or real?*/0, PRIV->ep[1].vsize,
				PRIV->ep[1].count - old_txincount[1]);
	#ifdef TXDELAY
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/1, /*Both,Shadow or real?*/1, PRIV->ep[1].vsize, 
				PRIV->ep[1].xmt_pending_count - old_shadowtxincount[1]);
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/1, /*Both,Shadow or real?*/2, PRIV->ep[1].vsize, 
				PRIV->ep[1].count - PRIV->ep[1].xmt_pending_count - old_txincount[1] + old_shadowtxincount[1]);
	#endif /* TXDELAY */

	/* Update filled-level counters for Non-ASM SwQ: */
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/2, /*Which moment?*/1, /*Both,Shadow or real?*/0, 1024,
				PRIV->nonasm_queue.count - old_nonasmqueuecount);

	/* ==================================================================================== */
	/* Update fill-level counters for line-0: */
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/2, /*Both,Shadow or real?*/0, PRIV->ep[0].vsize, PRIV->ep[0].count);

	#ifdef TXDELAY
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/2, /*Both,Shadow or real?*/1, PRIV->ep[0].vsize, PRIV->ep[0].xmt_pending_count);
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/0, /*Which moment?*/2, /*Both,Shadow or real?*/2, PRIV->ep[0].vsize,
									(PRIV->ep[0].count - PRIV->ep[0].xmt_pending_count));
	#endif /* TXDELAY */

	/* Update fill-level counters for line-1: */
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/2, /*Both,Shadow or real?*/0, PRIV->ep[1].vsize, PRIV->ep[1].count);

	#ifdef TXDELAY
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/2, /*Both,Shadow or real?*/1, PRIV->ep[1].vsize, PRIV->ep[1].xmt_pending_count);
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/1, /*Which moment?*/2, /*Both,Shadow or real?*/2, PRIV->ep[1].vsize,
									(PRIV->ep[1].count - PRIV->ep[1].xmt_pending_count));
	#endif /* TXDELAY */

	/* Update fill-Level counters for non-ASM SwQ: */
	UPDATE_TX_FILLLEVEL_STATS( /*Which line?*/2, /*Which moment?*/2, /*Both,Shadow or real?*/0, 1024, PRIV->nonasm_queue.count);
#endif

	return;
}

/*
 * Reads host txin head descriptor cache. If cache is empty,
 * dequeue txin head into cache, and then read the cache.
 * This is being done because reading the host txin head automatically
 * dequeues the head.
 * And therefore it must be cached first, if the head needs to be
 * re-processed.
 */
u32
read_host_txin_head_cache(struct host_txin_desc_cache **buff_desc)
{
	u32 bufflen;

	if (!PRIV->host_txin_head_cache_is_valid) {

		// Cache is empty. This means that Host txin descriptor
		// is being read for the first time.
		// So cache it. And consider the head as dequeued.
		PRIV->host_txin_head_cache_desc =
			read_host_txin_head_desc(PRIV->host_txin_ring.head);
		PRIV->host_txin_head_cache_is_valid = true;
		//pr_info("[%s:%d] host txin packet dequeued\n", __func__, __LINE__);

		// Move host txin head
		PRIV->host_txin_ring.count -= 1;
		PRIV->host_txin_ring.head =
			(PRIV->host_txin_ring.head + 1) %
				PRIV->host_txin_ring.size;
	}

	*buff_desc = &PRIV->host_txin_head_cache_desc;
	bufflen = PRIV->host_txin_head_cache_desc.bufflen;

	return bufflen;
}

/*
 * Updates host txin ring's tail and count
 */
void
poll_new_packets(void)
{
	s32 ret;
	u32 umtcounter;
	u64 virt_umtcounter;
	u32 newpktcount;
	u32 freespace = 0;

	ret = tc_read_umtcounter(&umtcounter);
	if (ret)
		return;

	if (umtcounter == PRIV->host_txin_ring.umtcounter)
		/* no new arrival. hostring-tail and count remains undisturbed */
		return;

	// Handle UMT counter rollover
	virt_umtcounter = umtcounter;
	if (unlikely(umtcounter < PRIV->host_txin_ring.umtcounter))
		virt_umtcounter += 0xFFFFFFFF;

	/* Prepare to extend host-ring tail */
	newpktcount = virt_umtcounter - PRIV->host_txin_ring.umtcounter;
	freespace = PRIV->host_txin_ring.size - PRIV->host_txin_ring.count;
	PRIV->host_txin_ring.umtcounter = umtcounter;

	if (unlikely(newpktcount > freespace)) {

		// Funny situation. Someone upstairs has pushed in more packets than host-ring can accomodate in its freespace
		// And funnily the host-ring seems to have allowed it. And got some of its unsent packets overwritten
		// And so we will miss those packets even before they reached us
		UPDATE_TX_STATS(PACKETS_OVERWRITTEN, newpktcount - freespace);
		
		newpktcount = freespace; // reset expectations
	}

	/* Extend host-ring tail */
	PRIV->host_txin_ring.count += newpktcount;
	PRIV->host_txin_ring.tail = (PRIV->host_txin_ring.tail + newpktcount) % PRIV->host_txin_ring.size;
}

/*
 * Segmentation, Sequencing and pushing into scheduler-ring
 */
void
process_tx_packet(struct host_txin_desc_cache *pkt_desc,
		  u32 pktlen, u32 required_cell_count)
{
	int i;
	ima_rx_atm_header_t *cell = NULL;
	u32 crc = 0;
	u32 remaininglen = pktlen;
	u32 payloadlen;
	u32 next_sid = PRIV->next_sid;
	bool sid_format_is_8bit;
	u32 sid_range;

#ifdef TX_FILL_LEVEL
	spin_lock_bh(&PRIV->stats.spinlock);
	if (PRIV->stats.cbmsize[0] > pktlen || PRIV->stats.cbmsize[0] == 0) {
		PRIV->stats.cbmsize[0] = pktlen;
	}
	if (PRIV->stats.cbmsize[1] < pktlen) {
		PRIV->stats.cbmsize[1] = pktlen;
	}
	spin_unlock_bh(&PRIV->stats.spinlock);
#endif /* TX_FILL_LEVEL */

	spin_lock_bh(&PRIV->config.spinlock);
	sid_format_is_8bit = PRIV->config.sid_format_is_8bit;
	sid_range = PRIV->config.sid_range;
	spin_unlock_bh(&PRIV->config.spinlock);

	crc = crc32_be(CRC_SEED, NULL, 0);

	/* Scan cell to cell */
	for (i = 0; i < required_cell_count; ++i) {

		// Prepare
		if (remaininglen > 48) {
			payloadlen = 48;
			remaininglen -= 48;

		} else {
			payloadlen = remaininglen;
			remaininglen = 0;
		}

		// Create
		cell = (ima_rx_atm_header_t *)pop_from_txbuff_stack();
		if (unlikely( cell == NULL)) {
			pr_crit( "Pop_from_txbuff_stack() returned NULL!\n" );
			continue;
		}
		bzero(cell, 56); //sizeof(ima_rx_atm_header_t)); // Automatically applies AAL5 padding

		// Copy
		memcpy(cell->payload,
			pkt_desc->buff + ATM_CELL_PAYLOAD_LEN * i, payloadlen);

#ifdef ENABLE_IOCTL_DEBUG
		debug_tx_sid_condn(next_sid);
#else
		// SID tag
		SETSID(cell, next_sid, sid_format_is_8bit);
#endif
		next_sid = (next_sid + 1) % sid_range;

		if (i == required_cell_count - 1) { // Last cell for this packet

			/* End-of-packet */
			cell->pt1 = 1;

			// AAL5 trailer:
			*((u16 *)(cell->payload + 42)) = htons(pktlen); // Len field
			crc = crc32_be(crc, cell->payload, 44) ^ CRC_SEED;
			*((u32 *)(cell->payload + 44)) = htonl(crc); // CRC field
			// TODO: other trailer fields

		} else {

			// Intermediate cell, CRC32 update
			crc = crc32_be(crc, cell->payload, 48);
		}

		// copy other pkt_desc meta information like ep, dest_id etc.
		*((u32 *)(cell->payload + 48)) = required_cell_count;
		*((u16 *)(cell->payload + 52)) = pkt_desc->ep;
		*((u16 *)(cell->payload + 54)) = pkt_desc->dest_id;

		// Push into scheduler-ring
		PRIV->nonasm_queue.ptrs[PRIV->nonasm_queue.tail] = (u32 *)cell;
		PRIV->nonasm_queue.tail = (PRIV->nonasm_queue.tail + 1) % PRIV->nonasm_queue.size;
		PRIV->nonasm_queue.count += 1;
	}

	// Housekeeping for next packet
	PRIV->next_sid = next_sid;
}

void
release_free_cell_buffers(void)
{
	int i = 0;
	int j = 0;
	int usedspace = 0;
	u8* freebuffptr = NULL;

	for (i = 0; i < 2; ++i) { // for each endpoint
		spin_lock_bh(&PRIV->ep[i].spinlock);
		usedspace = PRIV->ep[i].count;

		// Has EP released the earliest tx buffer ?
		for (j = 0; j < usedspace && NULL != (freebuffptr = read_device_txout_head(i, PRIV->ep[i].head)); ++j) {
			/* Now it is certain that head of txin ring has been transmitted successfully */
			UPDATE_TX_EP_STATS(EP_TOTAL_SENT, i, 1);

			// Push free buffer into stack
			push_into_txbuff_stack(freebuffptr);

			// And dequeue it from txout
			PRIV->ep[i].count -= 1;
			PRIV->ep[i].head = (PRIV->ep[i].head + 1) % PRIV->ep[i].size;
		}
		spin_unlock_bh(&PRIV->ep[i].spinlock);
	}
}

/*
 * Transmit cells from scheduler-ring via the link selected by the scheduler FSM
 */
#ifdef TX_FRAME_PER_LINE
static bool
xmt_data_cell_from_either_link(u8 *cellbuff)
{
	bool rval = true;
	ima_rx_atm_header_t *cell = (ima_rx_atm_header_t *)cellbuff;
	u32 count;

	if (PRIV->sched.framestart) {

		/* Decide the link for this cell and all its followers who belong to the same AAL5 frame */
		PRIV->sched.link = PRIV->sched.link ? 0 : 1;

		count = *((u32 *)(cell->payload + 48));

		if (PRIV->ep[PRIV->sched.link].size - PRIV->ep[PRIV->sched.link].count
					< count + ASM_TXIN_RESERVE) {

			PRIV->sched.link = PRIV->sched.link ? 0 : 1;

			if (PRIV->ep[PRIV->sched.link].size - PRIV->ep[PRIV->sched.link].count
					< count + ASM_TXIN_RESERVE) {

				UPDATE_TX_STATS(EP_TOTAL_DROPPED, 1);
				return false;
			}
		}
		PRIV->sched.framestart = false;
	}

	if (xmt_cell(cellbuff, PRIV->sched.link)) {

		/* Update total xmt-ed count-stats of link */
		UPDATE_TX_EP_STATS(EP_TOTAL_XMT, PRIV->sched.link, 1);

	} else { /* No freespace in the selected link */

   		rval = false;
	}

	/* Remember AAL5 end-of-frame encounter */
	if (cell->pt1) PRIV->sched.framestart = true;

	return rval;
}

#elif defined TX_FRAME_PER_TIMESLOT

static bool
xmt_data_cell_from_either_link(u8 *cellbuff)
{
	ima_rx_atm_header_t *cell = (ima_rx_atm_header_t *)cellbuff;
	u32 count;
	u32 linkid = 0;
	u32 otherlinkid = 0;

	if (PRIV->sched.framestart) {

		/* Check availability of cumulative free space in both lines
		   to enable the entire frame to be sent in single burst */

		count = *((u32 *)(cell->payload + 48));

		if (PRIV->ep[0].size + PRIV->ep[1].size 
				- PRIV->ep[0].count - PRIV->ep[1].count
					< count + (2*ASM_TXIN_RESERVE)) {

			UPDATE_TX_STATS(EP_TOTAL_DROPPED, 1);
			return false;
		}

		PRIV->sched.framestart = false;
	}

	linkid = scheduler_fsm_get_link();

	if (xmt_cell(cellbuff, linkid)) {

		/* Update total xmt-ed count-stats of link */
		UPDATE_TX_EP_STATS(EP_TOTAL_XMT, linkid, 1);

	} else { /* No freespace in the selected link */

		/* Try switching to other link */
		otherlinkid = (linkid == 0) ? 1 : 0;
		if (xmt_cell(cellbuff, otherlinkid)) {

			/* Update total xmt-ed count-stats for that link */
			UPDATE_TX_EP_STATS(EP_TOTAL_XMT, otherlinkid, 1);

			/* Inform Scheduler FSM that link has been switched over */
			scheduler_fsm_switch_link(otherlinkid);

		} else {

			/* couldnt be sent */
			return false;
		}
	}

	/* Remember AAL5 end-of-frame encounter */
	if (cell->pt1) PRIV->sched.framestart = true;

	return true;
}

#elif defined TXRATIO /* Neither TX_FRAME_PER_LINE nor TX_FRAME_PER_TIMESLOT */
static bool
xmt_data_cell_from_either_link(u8 *cellbuff)
{
	u32 target_linkid;

	if (PRIV->sched.counter[0] == 0 && PRIV->sched.counter[1] == 0 ) {
		PRIV->sched.counter[0] = PRIV->sched.counter[1]
			= PRIV->sched.txratio[0] * PRIV->sched.txratio[1];
	}

	if (PRIV->sched.counter[0] > PRIV->sched.counter[1]) {
		target_linkid = 0;
	} else {
		target_linkid = 1;
	}

	if (xmt_cell(cellbuff, target_linkid)) {

		/* Update total xmt counter of target link */
		UPDATE_TX_EP_STATS(EP_TOTAL_XMT, target_linkid, 1);
		PRIV->sched.counter[target_linkid] -= PRIV->sched.txratio[target_linkid ? 0 : 1];

	} else { /* No freespace in the target link */

		return false;
	}

	return true;
}
#else
static bool
xmt_data_cell_from_either_link(u8 *cellbuff)
{
	u32 target_linkid = PRIV->sched.prev_link ? 0 : 1;

	if (xmt_cell(cellbuff, target_linkid)) {

		/* Update total xmt counter of target link */
		UPDATE_TX_EP_STATS(EP_TOTAL_XMT, target_linkid, 1);
		PRIV->sched.prev_link = target_linkid;

	} else { /* No freespace in the target link */

		/* Re-use the previous link */
		if (xmt_cell(cellbuff, PRIV->sched.prev_link)) {

			/* Update total xmt counter of previous link */
			UPDATE_TX_EP_STATS(EP_TOTAL_XMT, PRIV->sched.prev_link, 1);

		} else {

			/* couldnt be sent */
			return false;
		}
	}

	return true;
}
#endif /* TX_FRAME_PER_LINE */

static inline bool
xmt_data_cell_from_link(u8 *cellbuff, u32 linkid)
{
	if (xmt_cell(cellbuff, linkid)) {
		UPDATE_TX_EP_STATS(EP_TOTAL_XMT, linkid, 1);
		return true;
	} else
		return false;
}

static bool
xmt_data_cell_from_link0(u8 *cellbuff)
{
	return xmt_data_cell_from_link(cellbuff, 0);
}

static bool
xmt_data_cell_from_link1(u8 *cellbuff)
{
	return xmt_data_cell_from_link(cellbuff, 1);
}

/* Whether current quota can accomodate an ATM cell, i.e. 53 x 8 bits */
static inline bool
ok_with_shaper(void)
{
	return PRIV->shaper.disabled ? true : PRIV->shaper.quota >= 53 * 8 ? true : false;
}

/* Inform shaper that one ATM cell, i.e. 53 x 8 bits have been lined up for transmission */
static inline void
update_shaper(void) {
	PRIV->shaper.quota -= 53 * 8;
}

bool
#ifdef TXDELAY
shadow_transmit_asm_cells(void)
#else
transmit_asm_cells(void)
#endif /* TXDELAY */
{
	u32 count;

	spin_lock_bh(&PRIV->asm_queue.spinlock);
	count = PRIV->asm_queue.count;
	spin_unlock_bh(&PRIV->asm_queue.spinlock);

	while (count) {

		if (!ok_with_shaper()
			|| !xmt_cell((u8 *)(PRIV->asm_queue.ptrs[PRIV->asm_queue.head]),
						    PRIV->asm_queue.linkids[PRIV->asm_queue.head])) {
			return false;
		}

		update_shaper();

		// Convey success to ASM Module
		PRIV->asm_queue.cb[PRIV->asm_queue.head](PRIV->asm_queue.linkids[PRIV->asm_queue.head], true);

		/* Move to the next ASM cell in ASM software queue */
		spin_lock_bh(&PRIV->asm_queue.spinlock);
		PRIV->asm_queue.head = (PRIV->asm_queue.head + 1) % (PRIV->asm_queue.size);
		count = (PRIV->asm_queue.count -= 1);
		spin_unlock_bh(&PRIV->asm_queue.spinlock);
	}

	return true;
}

void
#ifdef TXDELAY
shadow_transmit_nonasm_cells(void)
#else
transmit_nonasm_cells(void)
#endif /* TXDELAY */
{
//	u32 i;
	bool (*xmt_fn)(u8 *);

	/* Check links which are usable */
	spin_lock_bh(&PRIV->config.spinlock);
	xmt_fn = PRIV->sched.xmt_data_cell_fn;
	spin_unlock_bh(&PRIV->config.spinlock);

	if (unlikely(!xmt_fn)) {

		/* Both lines inactive */
		// TODO: Update stats for cells-dropped due to tx-links unusable
//pr_crit( "Bonding driver: NO TX LINK" );
		return;
	}

	while (PRIV->nonasm_queue.count) /*for (i = 0; i < SCHED_BUDGET && PRIV->nonasm_queue.count; ++i)*/ {

		if (!ok_with_shaper()
			|| !xmt_fn((u8 *)(PRIV->nonasm_queue.ptrs[PRIV->nonasm_queue.head]))) {

			/* No freespace in usable links */

			// Will retry in the next polling-loop
			return;
		}

		update_shaper();

		/* Move to the next cell in non-asm software queue */
		PRIV->nonasm_queue.head = (PRIV->nonasm_queue.head + 1) % (PRIV->nonasm_queue.size);
		PRIV->nonasm_queue.count -= 1;
	}
}

#ifndef IS_ASM
#define IS_ASM(ptr) (GETVPI(((ima_rx_atm_header_t *)(ptr))) == 0 && \
					 GETVCI(((ima_rx_atm_header_t *)(ptr))) == 20)
#endif

#ifdef TX_PRINT
void inline dump_tx(uint32_t lineidx, u8 *cell, char *desc)
{
	char hdr[64];

	if (!PRIV->dbg)
		return;

#if 0
	if (IS_ASM(cell))
		return;
#endif

	/* dump the packet */
	snprintf(hdr, sizeof(hdr), "%s on link-%d:",
		desc, lineidx);
	print_hex_dump(KERN_INFO, hdr, DUMP_PREFIX_OFFSET, 16, 1, (char *)cell, 56, false);
}
#else
#define dump_tx(...)
#endif

/*
 * Enqueues the cell buffer to txin-ring for transmission.
 * Provided the txin-ring has free space.
 *
 * Returns true on success. Otherwise false.
 * 
 */
static inline bool
xmt_cell(u8 *cellbuff, u32 linkid)
{
	bool ret = true;

	s32 sid;
	if (IS_ASM(cellbuff)) sid = -1;
	else sid = GETSID( ((ima_rx_atm_header_t *)cellbuff), false );

	spin_lock_bh(&PRIV->ep[linkid].spinlock);

	/* does the ep txin ring have any free space within its virtual size ? */
	if (PRIV->ep[linkid].vsize <= PRIV->ep[linkid].count) {

		UPDATE_TX_EP_STATS(NO_EP_FREESPACE_BACKPRESSURE, linkid, 1);
		ret = false;
		goto end;
	}

	dump_tx(linkid, cellbuff, "Bonding Driver: ATM <TX>" );

#ifdef TXDELAY
	/* pin the cell to the tail of the shadow-txin-ring */
	PRIV->ep[linkid].shadow_txin[PRIV->ep[linkid].tail].cellptr = cellbuff;
	PRIV->ep[linkid].shadow_txin[PRIV->ep[linkid].tail].ts = ktime_add_us( ktime_get(), PRIV->ep[linkid].txdelay );
	PRIV->ep[linkid].xmt_pending_count += 1;
#else
	/* pin the cell to the tail of the ep-txin-ring */
	write_device_txin_tail(linkid, PRIV->ep[linkid].tail, cellbuff);
#endif /* TXDELAY */

	/* elongate tail of the ep-txin-ring */
	PRIV->ep[linkid].count += 1;
	PRIV->ep[linkid].tail = (PRIV->ep[linkid].tail + 1) % PRIV->ep[linkid].size;

	if (sid != -1) PRIV->stats.lastsid[linkid] = sid;

end:
	spin_unlock_bh(&PRIV->ep[linkid].spinlock);
	return ret;
}

bool
send_asm_cell(u8 *cellbuff, u32 linkid, void (*cb)(u32, bool))
{
	u32 used, size;

	/* Is space available inside ASM Software-Q ? */
	spin_lock_bh(&PRIV->asm_queue.spinlock);
	size = PRIV->asm_queue.size;
	used = PRIV->asm_queue.count;
	spin_unlock_bh(&PRIV->asm_queue.spinlock);

	if (used >= size) return false; // ASM Software-Q already full

	/* Push into ASM Software-Q */
	PRIV->asm_queue.ptrs[PRIV->asm_queue.tail] = (u32 *)cellbuff;
	PRIV->asm_queue.linkids[PRIV->asm_queue.tail] = linkid;
	PRIV->asm_queue.cb[PRIV->asm_queue.tail] = cb;
	PRIV->asm_queue.tail = (PRIV->asm_queue.tail + 1) % PRIV->asm_queue.size;
	spin_lock_bh(&PRIV->asm_queue.spinlock);
	PRIV->asm_queue.count += 1;
	spin_unlock_bh(&PRIV->asm_queue.spinlock);

	return true;
}

#ifdef TXDELAY
static inline void
transmit_cells_after_txdelay(void)
{
	int i;

	// For each line...
	for( i=0; i<2; ++i ) {
		spin_lock_bh(&PRIV->ep[i].spinlock);

		while (PRIV->ep[i].xmt_pending_count) {

			// Time to transmit ?
			if (ktime_before( ktime_get(), PRIV->ep[i].shadow_txin[PRIV->ep[i].xmt].ts )) {

				// No. Nothing to transmit from this line for now
				break;
			}

			/* Put it in the real line-queue */
			write_device_txin_tail(i, PRIV->ep[i].xmt, PRIV->ep[i].shadow_txin[PRIV->ep[i].xmt].cellptr);

			PRIV->ep[i].xmt = ( PRIV->ep[i].xmt + 1 ) % PRIV->ep[i].size;
			PRIV->ep[i].xmt_pending_count -= 1;
		}

		spin_unlock_bh(&PRIV->ep[i].spinlock);
	}
}
#endif /* TXDELAY */
