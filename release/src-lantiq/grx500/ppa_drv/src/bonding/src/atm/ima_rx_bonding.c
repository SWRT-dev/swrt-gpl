
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file contains the logic of ATM Layer Bonding's Receiving Entity.
 * Within the IMA Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |   1   | Reassembly FSM Library              | ima_rx_reassembler.c(.h)  |
 *    |   2   | Bonding Library                     | ima_rx_bonding.c(.h)      |<=
 *    |   3   | ATM Cell Decode Library             | ima_rx_atm_cell.h         |
 *    |   4   | Line Queue Abstraction Library      | ima_rx_queue.c(.h)        |
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 *
 *    Unit Tests: 1. src/test/ut_rx_bonding_testgroup_1.c
 *                2. src/test/ut_rx_bonding_testgroup_2.c
 *                3. src/test/ut_rx_bonding_testgroup_3.c
 *                4. src/test/ut_rx_bonding_and_reassembly_testgroup_1.c
 */

#ifdef __KERNEL__
#include <linux/kernel.h>
//#include <linux/compiler.h> // likely()
#include <linux/spinlock.h> // spin_lock_bh(), spin_unlock_bh()
#include <linux/jiffies.h> // get_jiffies_64()
#else
#include <stdint.h> // uint8_t, uint32_t
#include <stddef.h> // NULL
#endif

/* Module headers */
#include "atm/ima_atm_rx.h"
#include "atm/ima_rx_bonding.h"
#include "atm/ima_rx_atm_cell.h"
#include "common/ima_rx_queue.h"
#include "atm/ima_rx_reassembler.h"
#include "common/ima_debug_manager.h"

#ifndef PRIV
#define PRIV (&g_privdata)
#endif

#ifndef IS_ASM
#define IS_ASM(ptr) (GETVPI(((ima_rx_atm_header_t *)(ptr))) == 0 && \
					 GETVCI(((ima_rx_atm_header_t *)(ptr))) == 20)
#endif

#ifndef PROCESS_NONASM
#define PROCESS_NONASM(cell, sid, sidrange, lineid) reassemble_aal5((ima_rx_atm_header_t *)(cell), (sid), (sidrange), (lineid))
#endif

/* TODO: Can't these declarations be removed ? */
void timeout_atm_rx(void);
static void flush_rx_queue0(void);
static void flush_rx_queue1(void);
static inline void flush_rx_queue(u32 linkid);
static void preprocess_rx_queues(void);
static void process_rx_queues(void);
static void process_rx_queue0(void);
static void process_rx_queue1(void);
static void process_no_rx_queues(void);
static inline void process_rx_queue(u32 linkid);
#ifdef RESEQUENCING_TEST
bool is_buffer_ready(u32 lineid, u32 idx);
#endif

//############################################################################################
//############################################################################################
//############################################################################################
//############################################################################################

int32_t
init_rx_bonding(void)
{
#ifdef __KERNEL__
	spin_lock_init(&(PRIV->m_st_device_info[0].rx_spinlock));
	spin_lock_init(&(PRIV->m_st_device_info[1].rx_spinlock));
	spin_lock_init(&(PRIV->stats.spinlock));
#endif

	PRIV->stats.asmcount[0] = PRIV->stats.asmcount[1] = 0;
	PRIV->stats.nonasmcount[0] = PRIV->stats.nonasmcount[1] = 0;
	PRIV->stats.droppedcells[0] = PRIV->stats.droppedcells[1] = 0;
	PRIV->stats.missingcells = 0;
	PRIV->stats.timedoutcells = 0;
	PRIV->stats.acceptedframecount = 0;
	PRIV->stats.discardedframecount[SIDJUMP] = PRIV->stats.discardedframecount[INVALIDLENGTH]
		= PRIV->stats.discardedframecount[INVALIDCRC32] = PRIV->stats.discardedframecount[INVALIDPT2ORCLP] = 0;

#ifdef MEASURE_JIFFIES
	PRIV->stats.start_jiffies = PRIV->stats.jiffies = 0;
#endif

	return 0;
}

void
cleanup_rx_bonding(void)
{
}

int32_t
init_device_rx_bonding(uint32_t lineidx, void *cfg)
{
	PRIV->config.sid_format_is_8bit = false;
	PRIV->config.sid_range = 1 << (PRIV->config.sid_format_is_8bit ? 8 : 12);
	PRIV->config.active[0] = PRIV->config.active[1] = false;
	PRIV->config.data_accept[0] = PRIV->config.data_accept[1] = false;
	PRIV->config.flush_cb = NULL;
	PRIV->config.process_cb = process_no_rx_queues;
	PRIV->state = LEARNING;
	PRIV->ExpectedSID = 0;
	PRIV->sidtimer.prev_sid0 = PRIV->sidtimer.prev_sid1;
	PRIV->sidtimer.ttl = 0;
#if defined RESEQUENCING_TEST
	PRIV->cache[0].valid = PRIV->cache[1].valid = false;
#endif /*RESEQUENCING_TEST */

	init_rx_reassembler(36);

	init_rx_lineq(0, cfg);
	init_rx_lineq(1, cfg);

	return 0;
}

void
cleanup_device_rx_bonding(uint32_t lineidx)
{
}

void
update_rx_config(bool sid_is_8bit)
{
	spin_lock_bh(&PRIV->config.spinlock);
	PRIV->config.sid_format_is_8bit = sid_is_8bit;
	PRIV->config.sid_range = 1 << (sid_is_8bit ? 8 : 12);
	spin_unlock_bh(&PRIV->config.spinlock);
}

void
activate_rx_link(u32 linkid)
{
	spin_lock_bh(&PRIV->config.spinlock);

	PRIV->config.active[linkid] = true;

	if (PRIV->config.active[linkid?0:1]) { /* => both links are active */
		PRIV->config.process_cb = process_rx_queues;
		pr_crit( "Bonding driver: Switching to double-line bonding mode\n" );
	} else if (linkid) {
		PRIV->config.process_cb = process_rx_queue1;
		pr_crit( "Bonding driver: Switching to single-line bonding mode on line-1\n" );
	} else {
		PRIV->config.process_cb = process_rx_queue0;
		pr_crit( "Bonding driver: Switching to single-line bonding mode on line-0\n" );
	}

	spin_unlock_bh(&PRIV->config.spinlock);
}

void
deactivate_rx_link(u32 linkid)
{
	spin_lock_bh(&PRIV->config.spinlock);

	PRIV->config.active[linkid] = false;

	if (!PRIV->config.active[linkid?0:1]) { /* => both links are inactive */
		if (linkid ) {
			PRIV->config.flush_cb = flush_rx_queue1;
		} else {
			PRIV->config.flush_cb = flush_rx_queue0;
		}
		PRIV->config.process_cb = process_no_rx_queues;
		pr_crit( "Bonding driver: Switching to no-line bonding mode\n" );
	} else if (linkid) {
		PRIV->config.flush_cb = flush_rx_queue1;
		PRIV->config.process_cb = process_rx_queue0;
		pr_crit( "Bonding driver: Switching to single-line bonding mode on line-0\n" );
	} else {
		PRIV->config.flush_cb = flush_rx_queue0;
		PRIV->config.process_cb = process_rx_queue1;
		pr_crit( "Bonding driver: Switching to single-line bonding mode on line-1\n" );
	}

	spin_unlock_bh(&PRIV->config.spinlock);
}

void
set_rx_data_acceptable(u32 linkid)
{
	spin_lock_bh(&PRIV->config.spinlock);

	PRIV->config.data_accept[linkid] = true;

	spin_unlock_bh(&PRIV->config.spinlock);
}

void
set_rx_data_unacceptable(u32 linkid)
{
	spin_lock_bh(&PRIV->config.spinlock);

	PRIV->config.data_accept[linkid] = false;

	spin_unlock_bh(&PRIV->config.spinlock);
}

static void
flush_rx_queue0(void)
{
	flush_rx_queue(0);
}

static void
flush_rx_queue1(void)
{
	flush_rx_queue(1);
}

static void
process_rx_queue0(void)
{
	process_rx_queue(0);
}

static void
process_rx_queue1(void)
{
	process_rx_queue(1);
}

static void
process_no_rx_queues(void)
{
	return;
}

/*
 * Looks for arrival of new cells at the end of any or both the Line Queues
 * Ima bonding driver's polling thread calls this function, in order to verify whether the process_rx_queues tasklet needs to be scheduled or not.
 *
 * Returns:
 *   Total number of new cells
 *
 * No Parameters
 */
int32_t
poll_rx_bonding(void)
{
	uint32_t totalnew = 0;

	totalnew += enqueue_rx_lineq(0); // Line-0
	totalnew += enqueue_rx_lineq(1); // Line-1

	return totalnew;
}

#ifdef __KERNEL__
extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
#endif
char *
getstats_rx_string(void)
{
	static char msg[640];
	static struct {
		uint32_t head, count, tail;
	} lines[2];

#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->m_st_device_info[0].rx_spinlock) );
#endif
	lines[0].head = PRIV->m_st_device_info[0].rx_head;
	lines[0].count = PRIV->m_st_device_info[0].rx_count;
	lines[0].tail = PRIV->m_st_device_info[0].rx_tail;
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->m_st_device_info[0].rx_spinlock) );
#endif

#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->m_st_device_info[1].rx_spinlock) );
#endif
	lines[1].head = PRIV->m_st_device_info[1].rx_head;
	lines[1].count = PRIV->m_st_device_info[1].rx_count;
	lines[1].tail = PRIV->m_st_device_info[1].rx_tail;
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->m_st_device_info[1].rx_spinlock) );
#endif

#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->stats.spinlock));
#endif
	sprintf(msg,
			 "IMA BONDING DRIVER(RX)> AAL5 Frames=> Accepted:%llu, discarded:[SID:%llu, CRC/Length:%llu, CLP/PT2:%llu]\n"
			 "IMA BONDING DRIVER(RX)>   ATM Cells=> ASM:[0:%llu, 1:%llu], Non-ASM:[0:%llu, 1:%llu], Dropped:[0:%llu, 1:%llu], Timed-out:%llu, Missed:%llu\n"
			 "IMA BONDING DRIVER(RX)> Line Queues=> Head[0]:%u, Cells[0]:%u, Tail[0]:%u, Head[1]:%u, Cells[1]:%u, Tail[1]:%u\n",
			 PRIV->stats.acceptedframecount, PRIV->stats.discardedframecount[SIDJUMP],
			 PRIV->stats.discardedframecount[INVALIDCRC32] + PRIV->stats.discardedframecount[INVALIDLENGTH],
			 PRIV->stats.discardedframecount[INVALIDPT2ORCLP],
			 PRIV->stats.asmcount[0], PRIV->stats.asmcount[1],
			 PRIV->stats.nonasmcount[0], PRIV->stats.nonasmcount[1],
			 PRIV->stats.droppedcells[0], PRIV->stats.droppedcells[1],
			 PRIV->stats.timedoutcells, PRIV->stats.missingcells,
			 lines[0].head, lines[0].count, lines[0].tail, lines[1].head, lines[1].count, lines[1].tail
			 );
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->stats.spinlock));
#endif
	return msg;
}

#if defined RESEQUENCING_TEST || defined GETBUFFER_TEST
void
process_rx(void)
{
	void (*flush_rx_fn)(void);
	void (*process_rx_fn)(void);

	spin_lock_bh(&(PRIV->config.spinlock));
	flush_rx_fn = PRIV->config.flush_cb;
	PRIV->config.flush_cb = NULL; /* one time handler, so disable it */
	process_rx_fn = PRIV->config.process_cb;
	PRIV->sid_format_is_8bit = PRIV->config.sid_format_is_8bit;
	PRIV->sid_range = PRIV->config.sid_range;
	PRIV->data_accept[0] = PRIV->config.data_accept[0];
	PRIV->data_accept[1] = PRIV->config.data_accept[1];
	spin_unlock_bh(&(PRIV->config.spinlock));

	if (unlikely(flush_rx_fn)) {
		flush_rx_fn();
	}
	process_rx_fn();
}

bool is_buffer_ready(u32 lineid, u32 idx)
{
	struct desc_4dw *desc;

	desc = (struct desc_4dw *)PRIV->m_st_device_info[lineid].start;
	desc += idx; //PRIV->m_st_device_info[lineid].rx_head;

	return (desc->status.field.own == 1);
}

u8 *get_buffer(u32 lineid, u8 *buffer, u32 bufferlen)
{
	struct desc_4dw *desc;
	u8 *buf;
	dma_addr_t buf_phys;

	desc = (struct desc_4dw *)PRIV->m_st_device_info[lineid].start;
	desc += PRIV->m_st_device_info[lineid].rx_head;

	if (desc->status.field.own == 0)
		return NULL;

	/* Read the buffer from desc head */
	buf = (u8 *)phys_to_virt(desc->data_pointer);
	buf += desc->status.field.byte_offset;

	buf_phys = dma_map_single(NULL, (void *)buf,
								56, DMA_FROM_DEVICE);
	if (dma_mapping_error(NULL, buf_phys)) {
		pr_err("[%s:%d] dma_map_single() failed\n", __func__, __LINE__);
		return NULL;
	}

	/* copy the buffer */
	memcpy(buffer, buf, 56);

	/* copy metadata including qid */
	*((u16 *)(buffer+5)) = desc->dw0.field.dest_id;

	/* Now, release the buffer with desc head */
	desc->status.field.own = 0;
	wmb();
	if ((PRIV->m_st_device_info[lineid].rx_head + 1) % 4 == 0)
		*((u32 *)PRIV->m_st_device_info[lineid].umt_out_addr) = 4;

	/* Move lineid's head to next element in the ring */
	PRIV->m_st_device_info[lineid].rx_head
		= (PRIV->m_st_device_info[lineid].rx_head + 1)
			% PRIV->m_st_device_info[lineid].rx_size;

	return buffer;
}

#else /* if neither RESEQUENCING_TEST nor GETBUFFER_TEST is defined */
This code should not be used. It would be removed shortly.
void
process_rx(void)
{
#ifdef MEASURE_JIFFIES
	u64 time_of_entry = get_jiffies_64(), time_of_exit;
	if ( ( unlikely( PRIV->stats.start_jiffies == 0))) UPDATE_STATS( start_jiffies, get_jiffies_64());
#endif

    if( poll_rx_bonding() ) process_rx_queues();
    if( SID_TIMEDOUT() ) timeout_atm_rx();

#ifdef MEASURE_JIFFIES
	time_of_exit = get_jiffies_64();
	UPDATE_STATS( jiffies, (time_of_exit - time_of_entry));
#endif
}
#endif /* if neither RESEQUENCING_TEST nor GETBUFFER_TEST is defined */

#if defined SID_TEST || (defined RESEQUENCING_TEST && RESEQUENCING_TEST > 0) || defined GETBUFFER_TEST

#if defined SID_TEST
#define CAPTURECOUNT SID_TEST
#elif defined RESEQUENCING_TEST
#define CAPTURECOUNT RESEQUENCING_TEST
#elif defined GETBUFFER_TEST
#define CAPTURECOUNT GETBUFFER_TEST
#endif

u32 sidcapturelen = 0;
u32 sidcapture[CAPTURECOUNT][2];
#ifdef DEBUG_255
#define HDRCOUNT 80000
u32 hdrcapturelen = 0;
u8 hdrcapture[HDRCOUNT][8];
#endif /* DEBUG_255 */

static inline void
capturesid(u32 sid0, u32 sid1)
{
	int i;
#ifdef DEBUG_255
	int hidx = 0;
#endif /* DEBUG_255 */

	if (sidcapturelen < CAPTURECOUNT) {

		sidcapture[sidcapturelen][0] = sid0;
		sidcapture[sidcapturelen][1] = sid1;
		sidcapturelen++;

	} else if (sidcapturelen == CAPTURECOUNT) {
		for (i=0; i<sidcapturelen; ++i) {
#ifdef DEBUG_255
			pr_crit("SID Pair #%-5d: %-3d,%-3d}}", i+1, sidcapture[i][0], sidcapture[i][1]);
			if (hidx < (hdrcapturelen - 1)) {
				pr_cont("{{");
				if (sidcapture[i][0] == 255) {
					pr_cont("%02x %02x %02x %02x %02x %02x %02x %02x",
							hdrcapture[hidx][0], hdrcapture[hidx][1],
							hdrcapture[hidx][2], hdrcapture[hidx][3],
							hdrcapture[hidx][4], hdrcapture[hidx][5],
							hdrcapture[hidx][6], hdrcapture[hidx][7]);
					hidx++;
				}
				pr_cont("},{");
				if (sidcapture[i][1] == 255) {
					pr_cont("%02x %02x %02x %02x %02x %02x %02x %02x",
							hdrcapture[hidx][0], hdrcapture[hidx][1],
							hdrcapture[hidx][2], hdrcapture[hidx][3],
							hdrcapture[hidx][4], hdrcapture[hidx][5],
							hdrcapture[hidx][6], hdrcapture[hidx][7]);
					hidx++;
				}
				pr_cont("}}");
			}
			pr_cont("\n");
#else /* DEBUG_255 */
		    pr_crit("SID Pair #%-5d: %-3d,%-3d}}\n", i+1, sidcapture[i][0], sidcapture[i][1]);
#endif /* DEBUG_255 */
		}
		sidcapturelen++;
	} 
}

#ifdef DEBUG_255
static inline void
capturehdr(u32 sid, u8 *hdr)
{
	if ((hdrcapturelen >= HDRCOUNT) ||
		(sidcapturelen >= CAPTURECOUNT))
		return;

	if (sid == 255) {
		memcpy(hdrcapture[hdrcapturelen], hdr, 8);
		hdrcapturelen++;
	}
}
#endif /* DEBUG_255 */
#else /* SID_TEST / RESEQUENCING_TEST > 0 / GETBUFFER_TEST */
#define capturesid(sid0, sid1)
#endif

#if defined SID_TEST
This code should not be used. It would be removed shortly.
void
timeout_atm_rx(void)
{
}

static void
process_rx_queues(void)
{
    unsigned int lineid;
    unsigned char *cellptr;
	s32 sid[2];
	bool nonempty[2];
	u8 cellbuff[56];

	do
	{
		// Keep looping as long as one or both rings are non-empty
		for( nonempty[0] = nonempty[1] = true;
				nonempty[0] || nonempty[1];
					capturesid(sid[0], sid[1])) {

			// Read both heads
    		for( lineid = 0; lineid < 2; ++lineid ) {

    			if (NULL != (cellptr = read_rx_lineq_head(lineid) ) ) { /* make it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */

        			if (IS_ASM(cellptr ) ) {
            			// ASM Cell...
            			PROCESS_ASM(cellptr, lineid ); // Pass it to ASM module
						sid[lineid] = 12345678;
            			dequeue_rx_lineq(lineid );     // Release asm
            			UPDATE_STATS(asmcount[lineid], 1);

        			} else {
            			// Non-ASM Cell...
            			sid[lineid] = GETSID(((ima_rx_atm_header_t *)cellptr), true );
						memcpy(cellbuff, cellptr, 56);

#ifdef DEBUG_255
						capturehdr(sid[lineid], cellbuff);
#endif /* DEBUG_255 */

						// FIXME: Need to store local qid and ep ?
            			dequeue_rx_lineq(lineid );      // Release cell
       	     			UPDATE_STATS(nonasmcount[lineid], 1);
       	 			}

    			} else {
					sid[lineid] = -1;
					nonempty[lineid] = false;
				}
			}
		}

    } while( poll_rx_bonding() ); // remove this API, remove this outer loop
}

#elif defined RESEQUENCING_TEST

#define PROCESS_RX_BUDGET 8000

static inline void
process_rx_queue(u32 linkid)
{
	int i;
	u32 sid;
	u32 diff;

	for (i=0; i<PROCESS_RX_BUDGET; ++i) {
		debug_rx_full_condn(linkid);

		// Read the head, if not done already
		while ( ! PRIV->cache[linkid].valid ) {

			if (get_buffer(linkid, PRIV->cache[linkid].data.buff, 56) != NULL ) { /* made it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */

				PRIV->cache[linkid].valid = true;

				// ASM Cell...
				if (IS_ASM(PRIV->cache[linkid].data.buff ) ) {

					PROCESS_ASM(PRIV->cache[linkid].data.buff, linkid ); // Pass it to ASM module
					UPDATE_STATS(asmcount[linkid], 1);
					PRIV->cache[linkid].valid = false;

				} else if ( unlikely(!PRIV->data_accept[linkid] )) {

					/* Link status not yet ready to receive data traffic */
					PRIV->cache[linkid].valid = false;
					UPDATE_STATS( droppedcells[linkid], 1);

				} else if ( GETSID( &(PRIV->cache[linkid].data.cell), true ) == 255
				            && GETVPI( &(PRIV->cache[linkid].data.cell)) == 255
				            && GETVCI( &(PRIV->cache[linkid].data.cell)) == 255 ) {

					/* Invalid SID-255 cell */
					PRIV->cache[linkid].valid = false;
					UPDATE_STATS( droppedcells[linkid], 1);
				}

			} else {

				break;
			}
		}

		debug_rx_sid_condn();
		debug_rx_sid_skip_condn();

		if (!PRIV->cache[linkid].valid) {
			// job done
			break;
		}
		
		sid = GETSID( &(PRIV->cache[linkid].data.cell), PRIV->sid_format_is_8bit);

		capturesid(sid, -1);
		
		if (PRIV->ExpectedSID == sid) {

			debug_rx_timeout_condn();
			PROCESS_NONASM(&(PRIV->cache[linkid].data.cell), sid, PRIV->sid_range, linkid );
			PRIV->cache[linkid].valid = false;
			PRIV->ExpectedSID = (PRIV->ExpectedSID + 1) % PRIV->sid_range;
			UPDATE_STATS(nonasmcount[linkid], 1);

		} else {

			/* Calculate next-sid */
			diff = sid > PRIV->ExpectedSID ? sid - PRIV->ExpectedSID : PRIV->sid_range + sid - PRIV->ExpectedSID;
			UPDATE_STATS( missingcells, diff );

			PRIV->ExpectedSID = sid;
		}
	}
}

static void
process_rx_queues(void)
{
	int i;
	u32 linkid, nextsid;
	u32 sid0, sid1;
	u32 diff0, diff1;
	bool lineread[2];

	preprocess_rx_queues();

	for (i=0; i<PROCESS_RX_BUDGET; ++i) {

		// Read both heads, if not done already
		lineread[0] = lineread[1] = false;
		for( linkid = 0; linkid < 2; ++linkid ) {

			while ( ! PRIV->cache[linkid].valid ) {

				debug_rx_full_condn(linkid);

				if (get_buffer(linkid, PRIV->cache[linkid].data.buff, 56) != NULL ) { /* made it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */

					lineread[linkid] = true;
					PRIV->cache[linkid].valid = true;

					// ASM Cell...
					if (IS_ASM(PRIV->cache[linkid].data.buff ) ) {

						PROCESS_ASM(PRIV->cache[linkid].data.buff, linkid ); // Pass it to ASM module
						UPDATE_STATS(asmcount[linkid], 1);
						PRIV->cache[linkid].valid = false;

					} else if ( unlikely(!PRIV->data_accept[linkid] )) {

						/* Link status not yet ready to receive data traffic */
						PRIV->cache[linkid].valid = false;
						UPDATE_STATS( droppedcells[linkid], 1);

					} else if ( GETSID( &(PRIV->cache[linkid].data.cell), true ) == 255
					            && GETVPI( &(PRIV->cache[linkid].data.cell)) == 255
					            && GETVCI( &(PRIV->cache[linkid].data.cell)) == 255 ) {

						/* Invalid SID-255 cell */
						PRIV->cache[linkid].valid = false;
						UPDATE_STATS( droppedcells[linkid], 1);
					}

				} else {
					break;
				}
			}
		}

		debug_rx_sid_condn();
		debug_rx_sid_skip_condn();

		sid0 = PRIV->cache[0].valid ? GETSID( &(PRIV->cache[0].data.cell), PRIV->sid_format_is_8bit ) : -1;
		sid1 = PRIV->cache[1].valid ? GETSID( &(PRIV->cache[1].data.cell), PRIV->sid_format_is_8bit ) : -1;

		capturesid(lineread[0]?sid0:-1, lineread[1]?sid1:-1);

		if (sid0 == -1 && sid1 == -1) {
			// job done
			break;

		} else if (PRIV->ExpectedSID == sid0) {

			debug_rx_timeout_condn();
			PROCESS_NONASM(&(PRIV->cache[0].data.cell), sid0, PRIV->sid_range, 0 );
			PRIV->cache[0].valid = false;
			PRIV->ExpectedSID = (PRIV->ExpectedSID + 1) % PRIV->sid_range;
			UPDATE_STATS(nonasmcount[0], 1);

		} else if (PRIV->ExpectedSID == sid1) {

			debug_rx_timeout_condn();
			PROCESS_NONASM(&(PRIV->cache[1].data.cell), sid1, PRIV->sid_range, 1 );
			PRIV->cache[1].valid = false;
			PRIV->ExpectedSID = (PRIV->ExpectedSID + 1) % PRIV->sid_range;
			UPDATE_STATS(nonasmcount[1], 1);

		} else if (sid0 != -1 && sid1 != -1) {

			/* Calculate next-sid */
			diff0 = sid0 > PRIV->ExpectedSID ? sid0 - PRIV->ExpectedSID : PRIV->sid_range + sid0 - PRIV->ExpectedSID;
			diff1 = sid1 > PRIV->ExpectedSID ? sid1 - PRIV->ExpectedSID : PRIV->sid_range + sid1 - PRIV->ExpectedSID;

			if (diff0 < diff1) {
				nextsid = sid0;
			} else {
				nextsid = sid1;
				diff0 = diff1;
			}

			UPDATE_STATS( missingcells, diff0 );

			PRIV->ExpectedSID = nextsid;

		} else {

			/* Arrived cells waiting on the non-empty line for too long ? i.e. more than 100 cells waiting ? */
			if (is_buffer_ready(sid0 == -1?1:0,
					(PRIV->m_st_device_info[sid0==-1?1:0].rx_head + 512/*FIXME*/)
						% PRIV->m_st_device_info[sid0==-1?1:0].rx_size)) {

				nextsid = sid0 == -1 ? sid1 : sid0;

				UPDATE_STATS( timedoutcells, ( ( nextsid + PRIV->sid_range - PRIV->ExpectedSID ) % PRIV->sid_range ) );

				PRIV->ExpectedSID = nextsid;
			}
		}
	}
}

static void
preprocess_rx_queues(void)
{
	u32 linkid;
	u32 sid0, sid1, nextsid;
	u32 prev_sid0 = PRIV->sidtimer.prev_sid0,
		prev_sid1 = PRIV->sidtimer.prev_sid1;

	// Read both heads, if not done already
	for( linkid = 0; linkid < 2; ++linkid ) {

		while ( ! PRIV->cache[linkid].valid ) {

			if (get_buffer(linkid, PRIV->cache[linkid].data.buff, 56) != NULL ) { /* made it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */

				PRIV->cache[linkid].valid = true;

				// ASM Cell...
				if (IS_ASM(PRIV->cache[linkid].data.buff ) ) {

					PROCESS_ASM(PRIV->cache[linkid].data.buff, linkid ); // Pass it to ASM module
					UPDATE_STATS(asmcount[linkid], 1);
					PRIV->cache[linkid].valid = false;

				} else if ( unlikely(!PRIV->data_accept[linkid] )) {

					/* Link status not yet ready to receive data traffic */
					PRIV->cache[linkid].valid = false;
					UPDATE_STATS( droppedcells[linkid], 1);

				} else if ( GETSID( &(PRIV->cache[linkid].data.cell), true ) == 255
				            && GETVPI( &(PRIV->cache[linkid].data.cell)) == 255
				            && GETVCI( &(PRIV->cache[linkid].data.cell)) == 255 ) {

					/* Invalid SID-255 cell */
					PRIV->cache[linkid].valid = false;
					UPDATE_STATS( droppedcells[linkid], 1);
				}

			} else {
				break;
			}
		}
	}

	sid0 = PRIV->cache[0].valid ? GETSID( &(PRIV->cache[0].data.cell), PRIV->sid_format_is_8bit) : -1;
	sid1 = PRIV->cache[1].valid ? GETSID( &(PRIV->cache[1].data.cell), PRIV->sid_format_is_8bit) : -1;

	PRIV->sidtimer.prev_sid0 = sid0;
	PRIV->sidtimer.prev_sid1 = sid1;

	if ( ( sid0 == -1 && sid1 == -1) /* Both lines empty, nothing to do */ 
	   || ( sid0 != -1 && sid1 != -1) /* Both lines non-empty, nothing to do */ 
	   || ( sid0 != prev_sid0 || sid1 != prev_sid1 ) ) { /* Different from previous, nothing to timeout */

	   	PRIV->sidtimer.ttl = 20; /* Made larger than 5 to accommodate differential delay of 6ms max */

		return;
	}

	/* Same as last */

	/* Locked condition. Move expected-SID closer to timeout */
	if (PRIV->sidtimer.ttl) {
		PRIV->sidtimer.ttl--;
		return;
	}

	/* SID has timed out. Update ExpectedSID */
	nextsid = sid0 == -1 ? sid1 : sid0;

	pr_info( "Bonding driver: timed out expectedsid %u, changed it to %u\n", PRIV->ExpectedSID, nextsid );

	UPDATE_STATS( timedoutcells, ( ( nextsid + PRIV->sid_range - PRIV->ExpectedSID ) % PRIV->sid_range ) );

	PRIV->ExpectedSID = nextsid;
}

static inline void
flush_rx_queue(u32 linkid)
{
	u32 sid;
	u32 asmcellsdropped = 0, datacellsdropped = 0;

	pr_info("Bonding driver: link-%u flushing starts\n", linkid );
	while (true) {

		// Read the head, if not done already
		while ( ! PRIV->cache[linkid].valid ) {

			if (get_buffer(linkid, PRIV->cache[linkid].data.buff, 56) != NULL ) { /* made it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */
				PRIV->cache[linkid].valid = true;

			} else {
				break;
			}
		}

		if (!PRIV->cache[linkid].valid) {
			// job done
			break;
		}
		
		if (IS_ASM(PRIV->cache[linkid].data.buff ) ) {
			++asmcellsdropped;
			pr_devel("Bonding driver: dropping ASM from link-%u\n", linkid );
		} else {
			++datacellsdropped;
			sid = GETSID( &(PRIV->cache[linkid].data.cell), PRIV->sid_format_is_8bit);
			pr_devel("Bonding driver: dropping sid %u from link-%u\n", sid, linkid );
		}

		// Drop the cell
		PRIV->cache[linkid].valid = false;
		UPDATE_STATS( droppedcells[linkid], 1);

	}

	pr_info("Bonding driver: flushed %u ASM cells, %u data cells from link-%d\n", asmcellsdropped, datacellsdropped, linkid);
}

#elif defined GETBUFFER_TEST

static inline void
process_rx_queue(u32 linkid)
{
	s32 sid;
	bool nonempty;
	u8 cellbuff[56];

	// Keep looping as long as the ring is non-empty
	for( nonempty = true; nonempty; capturesid(sid, -1)) {

		nonempty = false;

		// Read head
		if (get_buffer(linkid, cellbuff, 56) != NULL ) { /* make it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */

			nonempty = true;

			if (IS_ASM(cellbuff) ) {

				// ASM Cell...
				PROCESS_ASM(cellbuff, linkid ); // Pass it to ASM module
				sid = 12345678;
				UPDATE_STATS(asmcount[linkid], 1);

			} else {
				// Non-ASM Cell...
				sid = GETSID(((ima_rx_atm_header_t *)cellbuff), true );

#ifdef DEBUG_255
				capturehdr(sid, cellbuff);
#endif /* DEBUG_255 */

				UPDATE_STATS(nonasmcount[linkid], 1);
			}
		}
	}
}

static void
process_rx_queues(void)
{
	unsigned int lineid;
	s32 sid[2];
	bool nonempty[2];
	u8 cellbuff[56];

	// Keep looping as long as one or both rings are non-empty
	for( nonempty[0] = nonempty[1] = true;
			nonempty[0] || nonempty[1];
				capturesid(sid[0], sid[1])) {

		// Read both heads
		nonempty[0] = nonempty[1] = true;
		for( lineid = 0; lineid < 2; ++lineid ) {

			if (get_buffer(lineid, cellbuff, 56) != NULL ) { /* make it atomic: if OWN-bit set, read into buffer and dequeue. else return NULL */

				if (IS_ASM(cellbuff) ) {
					// ASM Cell...
					PROCESS_ASM(cellbuff, lineid ); // Pass it to ASM module
					sid[lineid] = 12345678;
					UPDATE_STATS(asmcount[lineid], 1);

				} else {
					// Non-ASM Cell...
					sid[lineid] = GETSID(((ima_rx_atm_header_t *)cellbuff), true );

#ifdef DEBUG_255
					capturehdr(sid[lineid], cellbuff);
#endif /* DEBUG_255 */

					UPDATE_STATS(nonasmcount[lineid], 1);
				}

			} else {
				sid[lineid] = -1;
				nonempty[lineid] = false;
			}
		}
	}
}

#else /* neither SID_TEST nor RESEQUENCING_TEST nor GETBUFFER_TEST */
/* DELETED */
#endif /* SID_TEST / RESEQUENCING_TEST */
