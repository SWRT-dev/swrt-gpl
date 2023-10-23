
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include "common/ima_debug_manager.h"
#include "manager/ima_bonding_debugfs.h"
#include "manager/ima_bonding_ioctl_if.h"

#ifdef ENABLE_IOCTL_DEBUG
#include "atm/ima_atm_rx.h"
#include "atm/ima_tx_module.h"
#include "asm/ima_asm.h"

#define TXPRIV (&g_tx_privdata)
#define RXPRIV (&g_privdata)
#define ASMPRIV (&g_asm_privdata)
#define TXCBPRIV (&g_privTxCBData)

#define DEBUG_DELAY(delay)					\
{								\
	if ((delay > 0) && (delay < 1000)) {			\
		udelay(delay);					\
	} else if ((delay >= 1000) && (delay < 1000000)) {	\
		do_div(delay, 1000L);				\
		msleep(delay);					\
	} else if (delay >= 1000000) {				\
		do_div(delay, 1000000L);			\
		ssleep(delay);					\
	}							\
}

struct bond_debug_priv g_dbg_privdata;

static int
init_debug_metadata(void)
{
	memset(&g_dbg_privdata, 0, sizeof(struct bond_debug_priv));

	/* Tx */
	spin_lock_init(&DBGPRIV->tx_dbg.lock);
	DBGPRIV->tx_dbg.xmit_enable = true;
	DBGPRIV->tx_dbg.sid.no_of_cells = 0;
	DBGPRIV->tx_dbg.delay[0] = 0;
	DBGPRIV->tx_dbg.delay[1] = 0;

	/* Rx */
	spin_lock_init(&DBGPRIV->rx_dbg.lock);
	DBGPRIV->rx_dbg.full[0] = disable;
	DBGPRIV->rx_dbg.full[1] = disable;
	DBGPRIV->rx_dbg.sid[0].no_of_cells = 0;
	DBGPRIV->rx_dbg.sid[1].no_of_cells = 0;
	DBGPRIV->rx_dbg.sid_skip[0].no_of_cells = 0;
	DBGPRIV->rx_dbg.sid_skip[1].no_of_cells = 0;
	DBGPRIV->rx_dbg.timeout_delay = 0;

	/* ASM */
	spin_lock_init(&DBGPRIV->asm_dbg.lock);
	DBGPRIV->asm_dbg.asm_print[0] = disable;
	DBGPRIV->asm_dbg.asm_print[1] = disable;

	return 0;
}

static void
cleanup_debug_metadata(void)
{
}

/* APIs exported to core modules, like Tx, Rx and ASM */

u32 debug_get_tx_next_sid(u32 next_sid)
{
	u32 new_next_sid = 0;

	spin_lock_bh(&DBGPRIV->tx_dbg.lock);

	switch (DBGPRIV->tx_dbg.sid.type) {

	case debug_type_tx_sid_offset:
	{
		s16 sid_offset = DBGPRIV->tx_dbg.sid.u.offset;

		if (DBGPRIV->tx_dbg.sid.no_of_cells > 0) {
			/* For +ve offset */
			if (sid_offset > 0) {
				new_next_sid = (next_sid + sid_offset) %
					TXPRIV->config.sid_range;

			/* For -ve offset */
			} else {
				new_next_sid = (TXPRIV->config.sid_range + sid_offset +
					next_sid) % TXPRIV->config.sid_range;
			}
			DBGPRIV->tx_dbg.sid.no_of_cells -= 1;
		} else {
			new_next_sid = next_sid;
			DBGPRIV->tx_dbg.sid.type = debug_type_none;
		}
		break;
	}

	case debug_type_tx_sid_set:
	{
		s16 sid_val = DBGPRIV->tx_dbg.sid.u.value;

		if (DBGPRIV->tx_dbg.sid.no_of_cells > 0) {
			new_next_sid = sid_val % TXPRIV->config.sid_range;
			DBGPRIV->tx_dbg.sid.u.value += 1;
			DBGPRIV->tx_dbg.sid.no_of_cells -= 1;
		} else {
			new_next_sid = next_sid;
			DBGPRIV->tx_dbg.sid.type = debug_type_none;
		}
		break;
	}

	default:
		new_next_sid = next_sid;
	}

	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	return new_next_sid;
}

void debug_tx_delay_condn(int linkid)
{
	u64 delay = 0;

	spin_lock_bh(&DBGPRIV->tx_dbg.lock);
	delay = DBGPRIV->tx_dbg.delay[linkid];
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	if (delay)
		DEBUG_DELAY(delay);
}

/* This below function is for setting offset and value */
void debug_rx_sid_condn(void)
{
	int loop_count;
	u32 curr_sid;
	u32 last_sid;
	s16 no_of_cells;
	u32 tmp_sid = 0;

	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	for (loop_count = 0; loop_count < 2; loop_count++) {
		curr_sid = GETSID(&(RXPRIV->cache[loop_count].data.cell),
					RXPRIV->config.sid_format_is_8bit);
		last_sid = DBGPRIV->rx_dbg.sid[loop_count].last_sid;

		if (curr_sid == last_sid)
			continue;

		no_of_cells = DBGPRIV->rx_dbg.sid[loop_count].no_of_cells;
		if (debug_type_rx_sid_offset == DBGPRIV->rx_dbg.sid[loop_count].type) {

			if ((no_of_cells > 0) &&
			    (true == RXPRIV->cache[loop_count].valid)) {

				s16 sid_offset = DBGPRIV->rx_dbg.sid[loop_count].u.offset;

				DBGPRIV->rx_dbg.sid[loop_count].no_of_cells -= 1;

				tmp_sid = curr_sid;
				if (sid_offset > 0)
					tmp_sid += sid_offset;
				else {
					if (tmp_sid + sid_offset > 0)
						tmp_sid += sid_offset;
					else
						tmp_sid += RXPRIV->config.sid_range + sid_offset;
				}
				SETSID(&(RXPRIV->cache[loop_count].data.cell), tmp_sid,
					RXPRIV->config.sid_format_is_8bit);
			}
		} else if (debug_type_rx_sid_set == DBGPRIV->rx_dbg.sid[loop_count].type) {
			if ((no_of_cells > 0) &&
			    (true == RXPRIV->cache[loop_count].valid)) {

				tmp_sid = DBGPRIV->rx_dbg.sid[loop_count].u.value;
				SETSID(&(RXPRIV->cache[loop_count].data.cell), tmp_sid,
					RXPRIV->config.sid_format_is_8bit);
				DBGPRIV->rx_dbg.sid[loop_count].no_of_cells -= 1;
				DBGPRIV->rx_dbg.sid[loop_count].u.value += 1;
			}
		}

		DBGPRIV->rx_dbg.sid[loop_count].last_sid =
			GETSID(&(RXPRIV->cache[loop_count].data.cell),
				RXPRIV->config.sid_format_is_8bit);
	}
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);
}

/* This below function is for skipping sid */
void debug_rx_sid_skip_condn(void)
{
	int loop_count = 0;

	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	for (loop_count = 0; loop_count < 2; loop_count++) {
		if ((DBGPRIV->rx_dbg.sid_skip[loop_count].no_of_cells > 0) &&
		    (true == RXPRIV->cache[loop_count].valid)) {
			RXPRIV->cache[loop_count].valid = false;
			DBGPRIV->rx_dbg.sid_skip[loop_count].no_of_cells--;
		}
	}
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);
}

void debug_rx_timeout_condn(void)
{
	u64 delay = 0;

	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	delay = DBGPRIV->rx_dbg.timeout_delay;
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	if (delay > 0) {
		DEBUG_DELAY(delay);
		spin_lock_bh(&DBGPRIV->rx_dbg.lock);
		DBGPRIV->rx_dbg.timeout_delay = 0;
		spin_unlock_bh(&DBGPRIV->rx_dbg.lock);
	}
}

bool debug_is_rx_link_full(int linkid)
{
	bool is_rx_full_en = false;

	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	is_rx_full_en = (DBGPRIV->rx_dbg.full[linkid] == enable);
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	return is_rx_full_en;
}

void dump_asm_cell(uint32_t lineidx, u8 *cell, char *desc, int direction)
{
	char hdr[64];
	en_status_t is_dump_dis = true;

	spin_lock_bh(&DBGPRIV->asm_dbg.lock);
	is_dump_dis = (DBGPRIV->asm_dbg.asm_print[direction] == disable);
	spin_unlock_bh(&DBGPRIV->asm_dbg.lock);

	if (is_dump_dis)
		return;

	/* dump the packet */
	snprintf(hdr, sizeof(hdr), "%s on link-%d:", desc, lineidx);
	print_hex_dump(KERN_INFO, hdr, DUMP_PREFIX_OFFSET, 16, 1, (char *)cell, 56, false);
}

/* APIs exported to debug interface, like IOCTL */

/**************************************************
 * Add a (configurable, signed) offset to the SID
 * of N consecutive Non-ASM cells
 **************************************************/
u8 ima_debug_tx_sid_offset(s16 offset, u32 no)
{
	spin_lock_bh(&DBGPRIV->tx_dbg.lock);
	DBGPRIV->tx_dbg.sid.u.offset = offset;
	DBGPRIV->tx_dbg.sid.no_of_cells = no;
	DBGPRIV->tx_dbg.sid.type = debug_type_tx_sid_offset;
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	return 0;
}

/*********************************************************************
 * Set the SID of next Non-ASM cell to configured value and (N-1)
 * consecutive Non-ASM cells (before distribution to both Tx queues)
 *********************************************************************/
u8 ima_debug_tx_sid_set(u16 value, u32 no)
{
	spin_lock_bh(&DBGPRIV->tx_dbg.lock);
	DBGPRIV->tx_dbg.sid.u.value = value;
	DBGPRIV->tx_dbg.sid.no_of_cells = no;
	DBGPRIV->tx_dbg.sid.type = debug_type_tx_sid_set;
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	return 0;
}

/******************************************************
 * Handle TX queue delay compensation - Queue frames
 * with much higher speed than the link rate
 ******************************************************/
u8 ima_debug_tx_qdc(u64 delay)
{
	return 0;
}

/********************************************
 * Abort - Don't transmit - Empty buffer:
 * Asynchronous event
 ********************************************/
u8 ima_debug_tx_abort(void)
{
	spin_lock_bh(&DBGPRIV->tx_dbg.lock);
	DBGPRIV->tx_dbg.xmit_enable = false;
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	return 0;
}

/********************************************
 * Flush out - transmit out - graceful exit
 ********************************************/
u8 ima_debug_tx_flush(void)
{
	spin_lock_bh(&DBGPRIV->tx_dbg.lock);
	DBGPRIV->tx_dbg.xmit_enable = true;
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	return 0;
}

/********************************
 * Force link specific Tx Delay
 ********************************/
u8 ima_debug_tx_delay(u16 link, u64 delay)
{
	spin_lock_bh(&DBGPRIV->tx_dbg.lock);
	DBGPRIV->tx_dbg.delay[link] = delay;
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock);

	return 0;
}

/*******************************************************
 * Add a (configurable, signed) offset value to the
 * SID of N consecutive Non-ASM cells in the selected
 * receive queue
 *******************************************************/
u8 ima_debug_rx_sid_offset(s16 offset, u32 no_of_cells, u16 link)
{
	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	memset((void *)(&DBGPRIV->rx_dbg.sid[link]), 0, sizeof(DBGPRIV->rx_dbg.sid[link]));
	DBGPRIV->rx_dbg.sid[link].u.offset = offset;
	DBGPRIV->rx_dbg.sid[link].no_of_cells = no_of_cells;
	DBGPRIV->rx_dbg.sid[link].type = debug_type_rx_sid_offset;
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	return 0;
}

/*******************************************************************
 * Set the SID of next Non-ASM cell to configured value and (N-1)
 * consecutive Non-ASM cells to following (+1) values in the
 * selected receive queue
 *******************************************************************/
u8 ima_debug_rx_sid_set(u16 value, u32 no_of_cells, u16 link)
{
	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	memset((void *)(&DBGPRIV->rx_dbg.sid[link]), 0, sizeof(DBGPRIV->rx_dbg.sid[link]));
	DBGPRIV->rx_dbg.sid[link].u.value = value;
	DBGPRIV->rx_dbg.sid[link].no_of_cells = no_of_cells;
	DBGPRIV->rx_dbg.sid[link].type = debug_type_rx_sid_set;
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	return 0;
}

/**********************************************************
 * Discard a configurable number of Non-ASM cells in the
 * selected receive queue
 **********************************************************/
u8 ima_debug_rx_sid_skip(u32 no_of_cells, u16 link)
{
	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	DBGPRIV->rx_dbg.sid_skip[link].no_of_cells = no_of_cells;
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	return 0;
}

/************************************************
 * Wait for configured time for arrival of next
 * SID to arrive
 ************************************************/
u8 ima_debug_rx_timeout(u64 delay)
{
	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	DBGPRIV->rx_dbg.timeout_delay = delay;
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	return 0;
}

/**************************************************************
 * Bonding driver to not sent any frames to VRX driver and
 * create the RX buffer full scenario. Bonding driver
 * reassembly feature to detect buffer full
 **************************************************************/
u8 ima_debug_rx_full(u16 link, en_status_t status)
{
	spin_lock_bh(&DBGPRIV->rx_dbg.lock);
	DBGPRIV->rx_dbg.full[link] = status;
	spin_unlock_bh(&DBGPRIV->rx_dbg.lock);

	return 0;
}

/***********************************************************
 * Get debug statistics & other counters/ state of driver
 * in user space
 ***********************************************************/
u8 ima_debug_stats(st_stats_t *user_stats)
{
	int cellbuff_mallocd, cellbuff_free, cellbuff_locked;
	u64 asmsent[2];
	u64 asmdropped[2];
	u64 grouplostcells;
	u32 txlinkstatus[2];
	u32 rxlinkstatus[2];
	int loop_count = 0;

	spin_lock_bh(&ASMPRIV->stats.spinlock);
	asmsent[0] = ASMPRIV->stats.asmsent[0];
	asmsent[1] = ASMPRIV->stats.asmsent[1];
	asmdropped[0] = ASMPRIV->stats.asmdropped[0];
	asmdropped[1] = ASMPRIV->stats.asmdropped[1];
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

	spin_lock_bh(&TXPRIV->stats.spinlock);
	user_stats->host_txin_ring_count = TXPRIV->host_txin_ring.count;
	user_stats->insufficient_tx_buffer_stack_backpressure =
		TXPRIV->stats.counters[INSUFFICIENT_TX_BUFFER_STACK_BACKPRESSURE];
	user_stats->cell_buff_mallocd = cellbuff_mallocd;
	user_stats->cell_buff_free = cellbuff_free;
	user_stats->cell_buff_locked = cellbuff_locked;
	user_stats->sched_ring_count = TXPRIV->sched_ring.count;
	user_stats->tx_ep_count[0] = TXPRIV->ep[0].count;
	user_stats->tx_ep_count[1] = TXPRIV->ep[1].count;
	user_stats->tx_asm_count[0] = (u64)asmsent[0];
	user_stats->tx_asm_count[1] = (u64)asmsent[1];
	user_stats->tx_atm_count[0] = TXPRIV->stats.epcounters[0][EP_TOTAL_XMT];
	user_stats->tx_atm_count[1] = TXPRIV->stats.epcounters[1][EP_TOTAL_XMT];
	user_stats->tx_asm_dropped_count[0] = (u64)asmdropped[0];
	user_stats->tx_asm_dropped_count[1] = (u64)asmdropped[1];
	user_stats->tx_total_atm_dropped_count = TXPRIV->stats.counters[EP_TOTAL_DROPPED];
	user_stats->tx_no_ep_freespace_backpressure[0] =
		TXPRIV->stats.epcounters[0][NO_EP_FREESPACE_BACKPRESSURE];
	user_stats->tx_no_ep_freespace_backpressure[1] =
		TXPRIV->stats.epcounters[1][NO_EP_FREESPACE_BACKPRESSURE];
	spin_unlock_bh(&TXPRIV->stats.spinlock);

	user_stats->co_rx_link_status[0] = rxlinkstatus[0] == 3 ? 'Y' : rxlinkstatus[0] == 2 ?
	  'A' : rxlinkstatus[0] == 1 ? 'N' : rxlinkstatus[0] == 0 ? '0' : '-';
	user_stats->co_rx_link_status[1] = rxlinkstatus[1] == 3 ? 'Y' : rxlinkstatus[1] == 2 ?
	  'A' : rxlinkstatus[1] == 1 ? 'N' : rxlinkstatus[1] == 0 ? '0' : '-';
	user_stats->co_tx_link_status[0] = txlinkstatus[0] == 3 ? 'Y' : txlinkstatus[0] == 2 ?
	  'A' : txlinkstatus[0] == 1 ? 'N' : txlinkstatus[0] == 0 ? '0' : '-';
	user_stats->co_tx_link_status[1] = txlinkstatus[1] == 3 ? 'Y' : txlinkstatus[1] == 2 ?
	  'A' : txlinkstatus[1] == 1 ? 'N' : txlinkstatus[1] == 0 ? '0' : '-';

	user_stats->co_group_lost_cell = grouplostcells;

	spin_lock_bh(&(RXPRIV->stats.spinlock));
	user_stats->rx_asm_count[0] = RXPRIV->stats.asmcount[0];
	user_stats->rx_asm_count[1] = RXPRIV->stats.asmcount[1];
	user_stats->rx_atm_count[0] = RXPRIV->stats.nonasmcount[0];
	user_stats->rx_atm_count[1] = RXPRIV->stats.nonasmcount[1];
	user_stats->rx_dropped_cells = RXPRIV->stats.droppedcells[0] + RXPRIV->stats.droppedcells[1];
	user_stats->rx_timedout_cells = RXPRIV->stats.timedoutcells;
	user_stats->rx_missing_cells = RXPRIV->stats.missingcells;

	user_stats->rx_accepted_frame_count = RXPRIV->stats.acceptedframecount;

	for (loop_count = SID_JUMP; loop_count < MAX_TYPES; loop_count++) {
		user_stats->rx_discarded_frame_count[loop_count] =
			RXPRIV->stats.discardedframecount[loop_count];
	}

	spin_unlock_bh(&(RXPRIV->stats.spinlock));

	return 0;
}

/*******************************
 * Output Rx ASM cell content
 *******************************/
u8 ima_debug_rx_asm_get(en_status_t status)
{
	spin_lock_bh(&DBGPRIV->asm_dbg.lock);
	DBGPRIV->asm_dbg.asm_print[RX] = status;
	spin_unlock_bh(&DBGPRIV->asm_dbg.lock);

	return 0;
}

/*******************************
 * Output Tx ASM cell content
 *******************************/
u8 ima_debug_tx_asm_get(en_status_t status)
{
	spin_lock_bh(&DBGPRIV->asm_dbg.lock);
	DBGPRIV->asm_dbg.asm_print[TX] = status;
	spin_unlock_bh(&DBGPRIV->asm_dbg.lock);

	return 0;
}

#else /* ENABLE_IOCTL_DEBUG */
#define init_debug_metadata(...)
#define cleanup_debug_metadata(...)
#endif /* ENABLE_IOCTL_DEBUG */

int init_debug_manager(void)
{
	int ret;

	ret = init_ioctl_interface();
	if (ret != RS_SUCCESS)
		goto err_out;

	ret = init_debug_interface();
	if (ret != RS_SUCCESS)
		goto err_debugfs_if_out;

	init_debug_metadata();

	return 0;

err_debugfs_if_out:
	cleanup_ioctl_interface();
err_out:
	return ret;
}

void cleanup_debug_manager(void)
{
	cleanup_debug_metadata();
	cleanup_ioctl_interface();
	cleanup_debug_interface();
}
