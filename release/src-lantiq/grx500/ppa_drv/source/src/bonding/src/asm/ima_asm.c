
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file contains the ASM state machine of the bonding driver.
 * It exposes 6 APIs to the remaining bonding driver code:
 * - init_device_asm_metadata(): Gets called from the device-init cb function (common/ima_vrx518_interface.c)
 * - cleanup_device_asm_metadata(): Gets called from the device-deinit cb function (common/ima_vrx518_interface.c)
 * - process_asm(): Gets called from the asm-workqueue-worker-function (common/ima_workqueue.c)
 * - asm_showtime_enter(): Gets called from the showtime-enter cb function (common/ima_vrx518_interface.c)
 * - asm_showtime_exit(): Gets called from the showtime-exit cb function (common/ima_vrx518_interface.c)
 * - asm_cell_received(): Gets called from the process_asm function (atm/ima_rx_bonding.c)
 */

#ifndef __UNIT_TESTING__

#include <linux/kernel.h>
#include <linux/spinlock.h> // spin_lock_init(), spin_lock_bh(), spin_unlock_bh()
#include <linux/crc32.h>

#include "common/atm_bond.h"
#include "common/ima_common.h"
#include "atm/ima_rx_atm_cell.h"
#include "atm/ima_tx_module.h"
#include "atm/ima_tx_cell_buffer.h"
#include "atm/ima_rx_bonding.h"
#include "asm/ima_asm.h"
#include "common/ima_debug_manager.h"

#define UPDATE_STATS(field, quantity) \
	do { \
		spin_lock_bh(&(PRIV->stats.spinlock)); \
		PRIV->stats.field += (quantity); \
		spin_unlock_bh(&(PRIV->stats.spinlock)); \
	} while (0)

#else /* not __UNIT_TESTING__ */
#define UPDATE_STATS(field, quantity)  (PRIV->stats.field += (quantity))
#endif /* __UNIT_TESTING__ */

#define ASM_XMT_INTERVAL_USECS         500000

/* ASM FSM States */
typedef enum {
        LEARNING = 0,
		STEADY,
        STOPPED
} ASMFSMState_e;

/* ASM Module & FSM Event types */
typedef enum {
        STOP = 0,
        SHOWTIME_ENTRY,
        SHOWTIME_EXIT,
		RECEIVE,
		RESET_ASM,
		NORMAL_ASM,
		TRANSMIT
} ASMFSMEvent_e;

/* ASM Tx-Link Status FSM States */
typedef enum {
        TX_SHOULDNT_BE_USED = 0,
		TX_ACCEPTABLE,
        TX_SELECTED,
		TX_ACTIVE
} ASMTxLinkStatusFSMState_e;

/* ASM Tx-Link Status FSM Events */
typedef enum {
        ev_TX_SHOWTIME_ENTRY,
        ev_TX_SHOWTIME_EXIT,
		ev_TX_RESET,
		ev_RX_NON_PROVISIONED,
		ev_RX_SHOULDNT_BE_USED,
		ev_RX_ACCEPTABLE,
        ev_RX_SELECTED,
} ASMTxLinkStatusFSMEvent_e;

/* ASM Rx-Link Status FSM States */
typedef enum {
        RX_UNUSABLE = 0,
		RX_SHOULDNT_BE_USED,
		RX_ACCEPTABLE,
        RX_SELECTED
} ASMRxLinkStatusFSMState_e;

/* ASM Rx-Link Status FSM Events */
typedef enum {
        ev_RX_SHOWTIME_ENTRY,
        ev_RX_SHOWTIME_EXIT,
		ev_RX_RESET,
		ev_TX_NON_PROVISIONED,
		ev_TX_SHOULDNT_BE_USED,
		ev_TX_ACCEPTABLE,
        ev_TX_SELECTED,
} ASMRxLinkStatusFSMEvent_e;

/* ASM Cell: */
typedef struct {
	ima_rx_atm_header_t hdr;
	u8 msg_type;
	u8 asm_id;
	u8 insuff_buff:1;
	u8 rsvd0:2;
	u8 tx_lnk_no:5;
	u8 num_links;
	u8 rx_link0_status:2;
	u8 rx_link1_status:2;
	u8 unused0:4;
	u8 unused1[7];
	u8 tx_link0_status:2;
	u8 tx_link1_status:2;
	u8 unused2:4;
	u8 unused3[7];
	u16 grpid;
	u8 lnk0_rx_asm_status:1;
	u8 lnk1_rx_asm_status:1;
	u8 unused4:6;
	u8 unused5[3];
	u8 grp_lost_cell;
	u8 rsvd1;
	u32 ts;
	u16 req_tx_delay;
	u16 act_tx_delay;
	u8 rsvd3[6];
	u16 len;
	u32 crc;
} __attribute__((packed)) asm_cell_t;

ASMPrivData_t g_asm_privdata;

static inline void asm_fsm( ASMFSMEvent_e event, u32 linkid, void *data );
 
#ifndef PRIV
#define PRIV (&g_asm_privdata)
#endif

void
print_and_check_aal5_trailer_crc(void)
{
	u32 crc;
	u8 emptyaal5[] = {
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00,
					 };

	crc = crc32_be(~0, emptyaal5, 44) ^ ~0;
	pr_info("Bonding Driver CRC32 check#1: 0x%08X\n", crc);

	crc = crc32_be(~0, NULL, 0);
	crc = crc32_be(crc, emptyaal5, 44);
	crc ^= ~0;
	pr_info("Bonding Driver CRC32 check#2: 0x%08X\n", crc);
}

void inline dump_asm(uint32_t lineidx, u8 *cell, char *desc)
{
  char hdr[64];

  if (PRIV->asmcount > 100)
    return;
  PRIV->asmcount++;

  /* dump the packet */
  snprintf(hdr, sizeof(hdr), "%s on link-%d:",
           desc, lineidx );
  print_hex_dump(KERN_INFO, hdr, DUMP_PREFIX_OFFSET, 16, 1, (char *)cell, 56, false);

  //pr_crit( "CPE Link-0 = CO Link %u, CPE Link-1 = CO Link %u\n", PRIV->bonding_grp_cfg.link_map[0], PRIV->bonding_grp_cfg.link_map[1] );
  //pr_crit( "CO Link-0 = CPE Link %u, CO Link-1 = CPE Link %u\n", PRIV->bonding_grp_cfg.link_rmap[0], PRIV->bonding_grp_cfg.link_rmap[1] );
} 

int
init_device_asm_metadata(struct bond_drv_cfg_params *cfg)
{
#ifndef __UNIT_TESTING__
	print_and_check_aal5_trailer_crc();
#endif

	PRIV->asmcount = 0;

	/* ASM FSM */
	PRIV->fsm.state = LEARNING;
	PRIV->fsm.txlinkstatus[0]
		= PRIV->fsm.txlinkstatus[1]
			= TX_SHOULDNT_BE_USED;
	PRIV->fsm.rxlinkstatus[0]
		= PRIV->fsm.rxlinkstatus[1]
			= RX_UNUSABLE;

	/* event queue */
	spin_lock_init(&PRIV->eventqueue.spinlock);
	PRIV->eventqueue.head
		= PRIV->eventqueue.tail
			= 0;
	PRIV->eventqueue.count = 0;
	PRIV->eventqueue.size = sizeof(PRIV->eventqueue.elements)/sizeof(PRIV->eventqueue.elements[0]);
	PRIV->eventqueue.prev_asm_received[0]
		= PRIV->eventqueue.prev_asm_received[1]
			= false;
	PRIV->eventqueue.asm_id_registered = false;
	PRIV->eventqueue.lastasmcellconsumed = true;
	PRIV->eventqueue.last_group_lost_cells = 0;

	/* ASM xmt timers per link */
	PRIV->asm_transmit.next_tx_asm_id = 0;
	PRIV->asm_transmit.timers[0].enabled
		= PRIV->asm_transmit.timers[1].enabled
			= false;
	PRIV->asm_transmit.timers[0].asmcell
		= PRIV->asm_transmit.timers[1].asmcell
			= NULL;

	/* stats: */
	spin_lock_init(&PRIV->stats.spinlock);
	PRIV->stats.asmsent[0]
		= PRIV->stats.asmsent[1]
			= 0;
	PRIV->stats.asmdropped[0]
		= PRIV->stats.asmdropped[1]
			= 0;
	PRIV->stats.asmdiscarded[0]
		= PRIV->stats.asmdiscarded[1]
			= 0;
	PRIV->stats.showtimeentry[0]
		= PRIV->stats.showtimeentry[1]
			= 0;
	PRIV->stats.showtimeexit[0]
		= PRIV->stats.showtimeexit[1]
			= 0;

	PRIV->stats.grouplostcells = 0;

	return 0;
}

static inline bool
asm_fsm_push_event(u32 event, u32 linkid)
{
	bool retval = true;

	spin_lock_bh(&PRIV->eventqueue.spinlock);
	do {
		if (PRIV->eventqueue.count >= PRIV->eventqueue.size) {
			retval = false;
			break;
		}

		PRIV->eventqueue.elements[PRIV->eventqueue.tail].event = event;
		PRIV->eventqueue.elements[PRIV->eventqueue.tail].linkid = linkid;
		PRIV->eventqueue.tail = (PRIV->eventqueue.tail + 1) % PRIV->eventqueue.size;
		PRIV->eventqueue.count += 1;

	} while(0);

	spin_unlock_bh(&PRIV->eventqueue.spinlock);
	return retval;
}

void
cleanup_device_asm_metadata(void)
{
	// Inject STOP event
	if (!asm_fsm_push_event(STOP, 0)) {
		// TODO: Update stats on too many events
		pr_crit( "bonding driver: %s: ASM Event Q full. Could not inject STOP event\n", __func__ );
		return;
	}
}

void
asm_showtime_enter(u32 linkid)
{
	// Inject SHOWTIME_ENTRY event
	if (!asm_fsm_push_event(SHOWTIME_ENTRY, linkid)) {
		// TODO: Update stats on too many events
		pr_crit( "bonding driver: %s: ASM Event Q full. Could not inject SHOWTIME_ENTRY event for link-%u\n", __func__, linkid );
		return;
	}
//pr_crit( "bonding driver: %s: showtime_entry event generated\n", __func__ );
}

void
asm_showtime_exit(u32 linkid)
{
	// Inject SHOWTIME_EXIT event
	if (!asm_fsm_push_event(SHOWTIME_EXIT, linkid)) {
		// TODO: Update stats on too many events
		pr_crit( "bonding driver: %s: ASM Event Q full. Could not inject SHOWTIME_EXIT event for link-%u\n", __func__, linkid );
		return;
	}
}

void
asm_cell_received(u8 *buff, u32 linkid)
{
#if defined(ASM_PRINT)
	dump_asm(linkid, buff, "Bonding Driver: ASM <RX>" );
#elif defined(ENABLE_IOCTL_DEBUG)
	dump_asm_cell(linkid, buff, "Bonding Driver: ASM <RX>", RX);
#endif

	spin_lock_bh(&PRIV->eventqueue.spinlock);
	if (PRIV->eventqueue.count >= PRIV->eventqueue.size) {
		// TODO: Update stats on too many events
		pr_crit( "bonding driver: %s: ASM Event Q full. Could not inject RECEIVE event for link-%u\n", __func__, linkid );
		goto end;
	}

	// Copy the cell
	memcpy( PRIV->eventqueue.lastasmcellbuff, buff, sizeof(PRIV->eventqueue.lastasmcellbuff) );
	PRIV->eventqueue.lastasmlinkid = linkid;
	PRIV->eventqueue.lastasmcellconsumed = false;

	// Inject RECEIVE event
	PRIV->eventqueue.elements[PRIV->eventqueue.tail].event = RECEIVE;
	PRIV->eventqueue.elements[PRIV->eventqueue.tail].linkid = linkid;
	PRIV->eventqueue.tail = (PRIV->eventqueue.tail + 1) % PRIV->eventqueue.size;
	PRIV->eventqueue.count += 1;

end:
	spin_unlock_bh(&PRIV->eventqueue.spinlock);
}

/*
 * Iterate through all events, fire FSM for each.
 * Additionally, check current time and generate TRANSMIT event, if required, before that.
 */
void
process_asm(void)
{
	u32 effective_asm_id;
	u32 linkid, asmlinkid;
	u32 eventcount;
	asm_cell_t asmcell;
	bool isvalidcell;
	ASMFSMEvent_e event;

	/* Check whether its needed to generate TRANSMIT event on any link */
	for (linkid = 0; linkid < 2; ++linkid ) {

		if (PRIV->asm_transmit.timers[linkid].enabled) {

        	if (get_jiffies_64() >= PRIV->asm_transmit.timers[linkid].jiffies) {

				// Inject TRANSMIT event
				if (asm_fsm_push_event(TRANSMIT, linkid)) {
					PRIV->asm_transmit.timers[linkid].enabled = false;
				} else {
					// TODO: Update stats on too many events
					pr_crit( "bonding driver: %s: ASM Event Q full. Could not inject TRANSMIT event for link-%u\n", __func__, linkid );
				}

			} else if (unlikely(!PRIV->asm_transmit.timers[linkid].asmcell)) {

				if (reserve_from_txbuff_stack(1)) {

					PRIV->asm_transmit.timers[linkid].asmcell = (u8 *)pop_from_txbuff_stack();
					if (!PRIV->asm_transmit.timers[linkid].asmcell) {

						// TODO: update stats that pop returned NULL free buffer 
						pr_alert("[%s:%s:%d] Pop_from_txbuff_stack() returned NULL!\n", __FILE__, __func__, __LINE__);

						unreserve_from_txbuff_stack(1);
					}
				}
			}
		}
	}

	// Iterate through all events and fire FSM for each:
	while(1) {

		spin_lock_bh(&PRIV->eventqueue.spinlock);
		eventcount = PRIV->eventqueue.count;
		spin_unlock_bh(&PRIV->eventqueue.spinlock);

		if (!eventcount) break;

		event = PRIV->eventqueue.elements[PRIV->eventqueue.head].event;
		linkid = PRIV->eventqueue.elements[PRIV->eventqueue.head].linkid;

//pr_crit( "bonding driver: %s: event: %u, linkid: %u\n", __func__, event, linkid );
		if (event == RECEIVE) {

			/* First valid ASM cell received by this link in the bonding mode's life-cycle */
			if (!PRIV->eventqueue.prev_asm_received[linkid]) {

				/* Notify TC driver */
				first_asm_received(linkid);
				PRIV->eventqueue.prev_asm_received[linkid] = true;
			}

			isvalidcell = true;

			spin_lock_bh(&PRIV->eventqueue.spinlock);
			if (PRIV->eventqueue.lastasmcellconsumed) {

				/* Most recently received cell can be processed by process_asm().
				   The other RECEIVE events get ignored */
				isvalidcell = false;

			} else {
				memcpy((void *)&asmcell, PRIV->eventqueue.lastasmcellbuff, sizeof(asm_cell_t));
				asmlinkid = PRIV->eventqueue.lastasmlinkid;
				PRIV->eventqueue.lastasmcellconsumed = true;
			}
			spin_unlock_bh(&PRIV->eventqueue.spinlock);

			/* new asm cell available. validate it. */
			if (isvalidcell) {

				asmcell.len = ntohs(asmcell.len);
				asmcell.crc = ntohl(asmcell.crc);
				asmcell.grpid = ntohs(asmcell.grpid);
				
				// Check congestion
				if (asmcell.hdr.pt2) {
					//pr_crit( "Bonding driver: ASM cell validation failed: congestion PT bit set\n");
					//TODO: Update stats on invalid ASM Cell with congestion
					isvalidcell = false;
					UPDATE_STATS(asmdiscarded[linkid], 1);
				}

				// Check CRC
				if (asmcell.crc != (crc32_be(~0, ((u8 *)&asmcell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0)) {
					//pr_crit( "Bonding driver: ASM cell validation failed: incorrect crc\n");
					//TODO: Update stats on invalid ASM Cell with incorrect crc
					isvalidcell = false;
					UPDATE_STATS(asmdiscarded[linkid], 1);
				}

				// Check Length
				if (asmcell.len != 0x28) {
					//pr_crit( "Bonding driver: ASM cell validation failed: incorrect length: %u\n", asmcell.len);
					//TODO: Update stats on invalid ASM Cell with incorrect length
					isvalidcell = false;
					UPDATE_STATS(asmdiscarded[linkid], 1);
				}

				// Check msg type is one of 00, 01 and FF
				if (asmcell.msg_type != 0 && asmcell.msg_type != 1 && asmcell.msg_type != 0xff) {
					//pr_crit( "Bonding driver: ASM cell validation failed: incorrect msg type: %02X\n", asmcell.msg_type);
					//TODO: Update stats on invalid ASM Cell with incorrect msg-type
					isvalidcell = false;
					UPDATE_STATS(asmdiscarded[linkid], 1);
				}

				/* Verify ASM ID */
				if (asmcell.msg_type != 0xFF && PRIV->eventqueue.asm_id_registered) {

					/* Verify whether this asm cell's identifier is "later" than the last one processed.
					   This is not as simple as checking greater-than or lesser-than.
					   Because the asm-identifier rolls over.
					   How to decide whether 16 is earlier or later than 250 ?
					   */
					effective_asm_id = asmcell.asm_id <= PRIV->eventqueue.last_rx_asm_id ? 
										asmcell.asm_id + 256 : asmcell.asm_id;
					if ((effective_asm_id - PRIV->eventqueue.last_rx_asm_id) > 128) {

						//pr_info( "Bonding driver: Skipping ASM cell since its ASM ID (%02X) is out-of-order, last ASM ID rcvd: %02X\n", asmcell.asm_id, PRIV->eventqueue.last_rx_asm_id);
						//TODO: Update stats on discarding ASM Cell with earlier ASM ID
						isvalidcell = false;
						UPDATE_STATS(asmdiscarded[linkid], 1);
					}
				}
			}

			if (isvalidcell) {

				PRIV->eventqueue.asm_id_registered = true;
				PRIV->eventqueue.last_rx_asm_id = asmcell.asm_id;
				UPDATE_STATS(grouplostcells, (asmcell.grp_lost_cell + 256 - PRIV->eventqueue.last_group_lost_cells)%256);
				PRIV->eventqueue.last_group_lost_cells = asmcell.grp_lost_cell;

				if (asmcell.msg_type == 0xFF) {
					event = RESET_ASM;
					pr_info( "bonding driver: received RESET ASM from CO on link-%d having Message Type 0xFF\n",
								asmlinkid);
				} else if (PRIV->fsm.state != LEARNING &&
							(asmcell.grpid != PRIV->bonding_grp_cfg.grpid
				           	 || asmcell.msg_type != PRIV->bonding_grp_cfg.msg_type
						   	 || asmcell.num_links != PRIV->bonding_grp_cfg.num_links
						   	 /* FIXME: || asmcell.tx_lnk_no != PRIV->bonding_grp_cfg.link_map[linkid] */ )) {
					event = RESET_ASM;
					pr_info( "bonding driver: received RESET ASM from CO on link-%d having Message Type "
					         "0x%02X, Expected Msg Type: 0x%02X, Group ID: %u, Expected Group"
							 " ID: %u, Number of Links: %u, Expected Number of Links: %u\n",
							 asmlinkid,
							 asmcell.msg_type,  PRIV->bonding_grp_cfg.msg_type,
							 asmcell.grpid,     PRIV->bonding_grp_cfg.grpid,
							 asmcell.num_links, PRIV->bonding_grp_cfg.num_links);
				} else {
					event = NORMAL_ASM;
				}

				asm_fsm( event, asmlinkid, (void *)&asmcell);
			}

		} else {

			/* Ensure that the tc-driver's first-asm callback gets called upon receiving
			   the first asm cell after showtime-entry */
			if (event == SHOWTIME_ENTRY) {
				PRIV->eventqueue.prev_asm_received[linkid] = false;
			}

			asm_fsm( event, linkid, NULL );
		}

		PRIV->eventqueue.head = (PRIV->eventqueue.head + 1) % PRIV->eventqueue.size;
		spin_lock_bh(&PRIV->eventqueue.spinlock);
		PRIV->eventqueue.count -= 1;
		spin_unlock_bh(&PRIV->eventqueue.spinlock);
	}
}

static inline void
asm_fsm_enable_txtimer( u32 linkid )
{
	PRIV->asm_transmit.timers[linkid].enabled = true;

	/* If other link is active, skew this link's timer by half second
	   after that else fire it after a second from now */
	if (PRIV->asm_transmit.timers[linkid?0:1].enabled) {
		PRIV->asm_transmit.timers[linkid].jiffies
			= PRIV->asm_transmit.timers[linkid?0:1].jiffies 
				+ usecs_to_jiffies(ASM_XMT_INTERVAL_USECS/2);
	} else {
		PRIV->asm_transmit.timers[linkid].jiffies
			= get_jiffies_64()
				+ usecs_to_jiffies(ASM_XMT_INTERVAL_USECS);
	}
}

static inline void
asm_fsm_disable_txtimer( u32 linkid )
{
	PRIV->asm_transmit.timers[linkid].enabled = false;
}

static inline u8
asm_get_tx_linkstatus( u32 co_link_idx)
{
	u8 retval = 0;

	u32 fsm_txlinkstatus
		= PRIV->fsm.txlinkstatus[
			PRIV->bonding_grp_cfg.link_rmap[co_link_idx]];

	switch (fsm_txlinkstatus) {
		case TX_SHOULDNT_BE_USED: retval = 1; break;
		case TX_ACCEPTABLE: retval = 2; break;
		case TX_SELECTED:
		case TX_ACTIVE: retval = 3; break;
		default: retval = 2;
	}

	return retval;
}

static inline u8
asm_get_rx_linkstatus( u32 co_link_idx )
{
	u8 retval = 0;

	u32 fsm_rxlinkstatus
		= PRIV->fsm.rxlinkstatus[
			PRIV->bonding_grp_cfg.link_rmap[co_link_idx]];

	switch (fsm_rxlinkstatus) {
		case RX_UNUSABLE: retval = 1; break;
		case RX_SHOULDNT_BE_USED: retval = 1; break;
		case RX_ACCEPTABLE: retval = 2; break;
        case RX_SELECTED: retval = 3; break;
		default: retval = 1;
	}

	return retval;
}

static void
asm_xmt_status_handler( u32 linkid, bool status )
{
	if( status ) {
		spin_lock_bh(&PRIV->stats.spinlock);
		PRIV->stats.asmsent[linkid] += 1;
		spin_unlock_bh(&PRIV->stats.spinlock);
	} else {
		spin_lock_bh(&PRIV->stats.spinlock);
		PRIV->stats.asmdropped[linkid] += 1;
		spin_unlock_bh(&PRIV->stats.spinlock);
		pr_info( "[Bonding driver:%s:%s:%d] ASM cell couldnt be xmt'd from link-%d because queue was full\n", __FILE__, __func__, __LINE__, linkid);
	}
}

static inline void
asm_fsm_create_and_transmit( u32 linkid )
{
	asm_cell_t *asmcell;
	u32 crc;
	u32 ts;
	u64 ts64;

	if (!PRIV->asm_transmit.timers[linkid].asmcell) {

		if (reserve_from_txbuff_stack(1)) {

			PRIV->asm_transmit.timers[linkid].asmcell = (u8 *)pop_from_txbuff_stack();

			if (!PRIV->asm_transmit.timers[linkid].asmcell) {
				// TODO: update stats that pop returned NULL free buffer 
				pr_alert("[%s:%s:%d] Pop_from_txbuff_stack() returned NULL!\n", __FILE__, __func__, __LINE__);

				unreserve_from_txbuff_stack(1);
				return;
			}

		} else {

			// TODO: update stats that no free buffer available for transmitting ASM cell
			pr_info("[%s:%s:%d] No free buffer available to transmit ASM cell from link-%d!\n", __FILE__, __func__, __LINE__, linkid);

			return;
		}
	}


	asmcell = (asm_cell_t *)(PRIV->asm_transmit.timers[linkid].asmcell);

			/* Populate ASM cell */
			memset(asmcell, 0, 56);

			SETSID(&asmcell->hdr, 0, false);
			SETVPI(&asmcell->hdr, 0);
			SETVCI(&asmcell->hdr, 20);
			asmcell->hdr.pt1 = 1;

			asmcell->msg_type = PRIV->bonding_grp_cfg.msg_type;
			asmcell->asm_id = PRIV->asm_transmit.next_tx_asm_id;
			PRIV->asm_transmit.next_tx_asm_id = (PRIV->asm_transmit.next_tx_asm_id + 1) % 0x100;
			asmcell->insuff_buff = 0;
			asmcell->tx_lnk_no = PRIV->bonding_grp_cfg.link_map[linkid];
			asmcell->num_links = PRIV->bonding_grp_cfg.num_links;
			asmcell->rx_link0_status = asm_get_rx_linkstatus(0);
			asmcell->rx_link1_status = asm_get_rx_linkstatus(1);
			asmcell->tx_link0_status = asm_get_tx_linkstatus(0);
			asmcell->tx_link1_status = asm_get_tx_linkstatus(1);
			asmcell->grpid = htons(PRIV->bonding_grp_cfg.grpid);
			asmcell->lnk0_rx_asm_status = 0;
			asmcell->lnk1_rx_asm_status = 0;
			asmcell->grp_lost_cell = 0;

			ts64 = jiffies64_to_nsecs(get_jiffies_64());
			do_div(ts64, 100000L);
			ts = (u32)ts64;
			asmcell->ts = htonl(ts);

			asmcell->req_tx_delay = 0;
			asmcell->act_tx_delay = 0;
			asmcell->len = htons(0x28);

			crc = crc32_be(~0, ((u8 *)asmcell) + 8, 44) ^ ~0;
			asmcell->crc = htonl(crc);

#if defined(ASM_PRINT)
			dump_asm(linkid, (u8*)asmcell, "Bonding Driver: ASM <TX>" );
#elif defined(ENABLE_IOCTL_DEBUG)
			dump_asm_cell(linkid, (u8*)asmcell, "Bonding Driver: ASM <TX>", TX);
#endif

			/* Push it to ASM software-Q for transmission in the immediate future */
			if (!send_asm_cell( (u8 *)asmcell, linkid, asm_xmt_status_handler)) {

				// No space in ASM software-Q. First release the cell-buffer
				push_into_txbuff_stack((u8 *)asmcell);
				pr_crit("Bonding driver: [%s:%s:%d] send_asm_cell() returned false!\n", __FILE__, __func__, __LINE__);
			}

	PRIV->asm_transmit.timers[linkid].asmcell = NULL;
}

#ifdef ASM_LOOPBACK
/* Loopback-FSM */
static inline void
asm_fsm( ASMFSMEvent_e event, u32 linkid, void *data )
{
//pr_crit( "bonding driver: %s: event: %u, linkid: %u\n", __func__, event, linkid );
	switch (PRIV->fsm.state) {

		case LEARNING:
			switch (event) {
				case SHOWTIME_ENTRY: PRIV->fsm.state = STEADY;
					PRIV->fsm.txlinkstatus[0]
						= PRIV->fsm.txlinkstatus[1]
							= TX_ACTIVE;
					PRIV->fsm.rxlinkstatus[0]
						= PRIV->fsm.rxlinkstatus[1]
							= RX_SELECTED;
				    set_rx_data_acceptable(0);
				    set_rx_data_acceptable(1);

					activate_tx_link(linkid);
					activate_rx_link(linkid);

					PRIV->bonding_grp_cfg.grpid = 10;
					PRIV->bonding_grp_cfg.msg_type = 0x1;
					PRIV->bonding_grp_cfg.num_links = 2;
					PRIV->bonding_grp_cfg.link_map[0] = 1;
					PRIV->bonding_grp_cfg.link_map[1] = 0;
					PRIV->bonding_grp_cfg.link_rmap[0] = 1;
					PRIV->bonding_grp_cfg.link_rmap[1] = 0;

					update_tx_config( PRIV->bonding_grp_cfg.msg_type == 0x01);
					update_rx_config( PRIV->bonding_grp_cfg.msg_type == 0x01);

					asm_fsm_enable_txtimer( linkid);

					break;

				case SHOWTIME_EXIT: PRIV->fsm.state = LEARNING;
					deactivate_tx_link(linkid);
					deactivate_rx_link(linkid);
					asm_fsm_disable_txtimer( linkid);
					break;

				case TRANSMIT: PRIV->fsm.state = LEARNING;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Event TRANSMIT (%u) not expected in state LEARNING\n", event);
					break;

				case RESET_ASM: PRIV->fsm.state = LEARNING;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Event RESET_ASM (%u) not expected in state LEARNING\n", event);
					break;

				case NORMAL_ASM: PRIV->fsm.state = LEARNING;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Event NORMAL_ASM (%u) not expected in state LEARNING\n", event);
					break;

				case STOP: PRIV->fsm.state = STOPPED;
					asm_fsm_disable_txtimer(0);
					asm_fsm_disable_txtimer(1);
					break;

				case RECEIVE: PRIV->fsm.state = LEARNING;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Event RECEIVE (%u) not expected\n", event);
					break;

				default: PRIV->fsm.state = LEARNING;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Unknown event (%u)\n", event);
			}
			break;

		case STEADY:
			switch (event) {

				case SHOWTIME_ENTRY: PRIV->fsm.state = STEADY;
					activate_tx_link(linkid);
					activate_rx_link(linkid);
					asm_fsm_enable_txtimer( linkid);
					break;

				case SHOWTIME_EXIT: PRIV->fsm.state = STEADY;
					deactivate_tx_link(linkid);
					deactivate_rx_link(linkid);
					asm_fsm_disable_txtimer( linkid);
					break;

				case TRANSMIT: PRIV->fsm.state = STEADY;
					asm_fsm_create_and_transmit( linkid );
					asm_fsm_enable_txtimer( linkid);
					break;

				case RESET_ASM: PRIV->fsm.state = STEADY;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Event RESET_ASM (%u) not expected in state STEADY\n", event);
					break;

				case NORMAL_ASM: PRIV->fsm.state = STEADY;
					break;

				case STOP: PRIV->fsm.state = STOPPED;
					asm_fsm_disable_txtimer(0);
					asm_fsm_disable_txtimer(1);
					break;

				case RECEIVE: PRIV->fsm.state = STEADY;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Event RECEIVE (%u) not expected\n", event);
					break;

				default: PRIV->fsm.state = STEADY;
					pr_warn( "Bonding driver: ASM FSM LOOPBACK: Unknown event (%u)\n", event);
			}
			break;

		case STOPPED: PRIV->fsm.state = LEARNING;
			pr_warn( "Bonding driver: ASM FSM LOOPBACK: State (%u) not expected\n", PRIV->fsm.state);
			break;

		default:
			pr_warn( "Bonding driver: ASM FSM LOOPBACK: Unknown State (%u)\n", PRIV->fsm.state);
	}
}

#else

static inline char *
stringify_tx_link_state( u32 txlinkstate )
{
	static char retval[32];

	switch(txlinkstate) {
		case TX_SHOULDNT_BE_USED: strcpy( retval, "TX_SHOULDNT_BE_USED" ); break;
		case TX_ACCEPTABLE: strcpy( retval, "TX_ACCEPTABLE" ); break;
		case TX_SELECTED: strcpy( retval, "TX_SELECTED" ); break;
        case TX_ACTIVE: strcpy( retval, "TX_ACTIVE" ); break;
		default: strcpy( retval, "UNKNOWN" ); break;
	}

	return retval;
}

static inline char *
stringify_tx_link_event( u32 event )
{
	static char retval[64];

	switch((ASMTxLinkStatusFSMEvent_e)event) {
        case ev_TX_SHOWTIME_ENTRY: strcpy( retval, "SHOWTIME_ENTRY" ); break;
        case ev_TX_SHOWTIME_EXIT: strcpy( retval, "SHOWTIME_EXIT" ); break;
		case ev_TX_RESET: strcpy( retval, "receiving RESET-ASM from CO" ); break;
		case ev_RX_NON_PROVISIONED: strcpy( retval, "receiving RX_NON_PROVISIONED from CO" ); break;
		case ev_RX_SHOULDNT_BE_USED: strcpy( retval, "receiving RX_SHOULDNT_BE_USED from CO" ); break;
		case ev_RX_ACCEPTABLE: strcpy( retval, "receiving RX_ACCEPTABLE from CO" ); break;
        case ev_RX_SELECTED: strcpy( retval, "receiving RX_SELECTED from CO" ); break;
		default: strcpy( retval, "UNKNOWN" ); break;
	}

	return retval;
}

static inline char *
stringify_rx_link_state( u32 rxlinkstate )
{
	static char retval[32];

	switch(rxlinkstate) {
		case RX_UNUSABLE: strcpy( retval, "RX_UNUSABLE" ); break;
		case RX_SHOULDNT_BE_USED: strcpy( retval, "RX_SHOULDNT_BE_USED" ); break;
		case RX_ACCEPTABLE: strcpy( retval, "RX_ACCEPTABLE" ); break;
        case RX_SELECTED: strcpy( retval, "RX_SELECTED" ); break;
		default: strcpy( retval, "UNKNOWN" ); break;
	}

	return retval;
}

static inline char *
stringify_rx_link_event( u32 event )
{
	static char retval[64];

	switch((ASMRxLinkStatusFSMEvent_e)event) {
        case ev_RX_SHOWTIME_ENTRY: strcpy( retval, "SHOWTIME_ENTRY"); break;
        case ev_RX_SHOWTIME_EXIT: strcpy( retval, "SHOWTIME_EXIT"); break;
		case ev_RX_RESET: strcpy( retval, "receiving RESET-ASM from CO"); break;
		case ev_TX_NON_PROVISIONED: strcpy( retval, "receiving TX_NON_PROVISIONED from CO"); break;
		case ev_TX_SHOULDNT_BE_USED: strcpy( retval, "receiving TX_SHOULDNT_BE_USED from CO"); break;
		case ev_TX_ACCEPTABLE: strcpy( retval, "receiving TX_ACCEPTABLE from CO"); break;
        case ev_TX_SELECTED: strcpy( retval, "receiving TX_SELECTED from CO"); break;
		default: strcpy( retval, "UNKNOWN" ); break;
	}

	return retval;
}

static inline void
print_link_state_xn( bool tx, u32 linkid, u32 prev_state, u32 next_state, u32 event )
{
	static char msg[256];
	static char *dir;
	static char *(* state_stringifier)(u32 state);
	static char *(* event_stringifier)(u32 event);

	if (tx) {
	
		dir = "tx";
		state_stringifier = stringify_tx_link_state;
		event_stringifier = stringify_tx_link_event;

	} else {

		dir = "rx";
		state_stringifier = stringify_rx_link_state;
		event_stringifier = stringify_rx_link_event;
	}

	snprintf( msg, sizeof(msg), "Bonding driver: %s link state of link-%d changed from %s to ",
				dir, linkid, state_stringifier(prev_state));
	snprintf( msg + strlen(msg), sizeof(msg) - strlen(msg), "%s on %s",
				state_stringifier(next_state), event_stringifier(event) );
	pr_crit( "%s\n", msg );
}

static inline void
print_tx_link_state_xn( u32 linkid, u32 prev_state, u32 next_state, u32 event)
{
	print_link_state_xn( true, linkid, prev_state, next_state, event);
}

static inline void
print_rx_link_state_xn( u32 linkid, u32 prev_state, u32 next_state, u32 event)
{
	print_link_state_xn( false, linkid, prev_state, next_state, event);
}

static void
asm_tx_link_fsm( ASMTxLinkStatusFSMEvent_e event, u32 linkid )
{
	u32 prev_state = PRIV->fsm.txlinkstatus[linkid];

	switch (PRIV->fsm.txlinkstatus[linkid]) {

		case TX_SHOULDNT_BE_USED:
			
			switch (event) {
				case ev_TX_SHOWTIME_ENTRY: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
				case ev_RX_NON_PROVISIONED: PRIV->fsm.txlinkstatus[linkid] = TX_SHOULDNT_BE_USED;
					pr_info( "Bonding driver: ASM Tx Link Status for link-%u: RX::NON_PROVISIONED received from CO. Current state: TX_SHOULDNT_BE_USED\n", linkid);
					break;
        		case ev_TX_SHOWTIME_EXIT:
				case ev_TX_RESET:
				case ev_RX_SHOULDNT_BE_USED:
				case ev_RX_ACCEPTABLE:
        		case ev_RX_SELECTED:
					PRIV->fsm.txlinkstatus[linkid] = TX_SHOULDNT_BE_USED;
					break;
				default: PRIV->fsm.txlinkstatus[linkid] = TX_SHOULDNT_BE_USED;
					pr_crit( "Bonding driver: ASM Tx Link Status for link-%u: Unknown event (%u)\n", linkid, event);
			}
			break;

		case TX_ACCEPTABLE:
			switch (event) {
				case ev_TX_SHOWTIME_ENTRY: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
        		case ev_TX_SHOWTIME_EXIT: PRIV->fsm.txlinkstatus[linkid] = TX_SHOULDNT_BE_USED;
					break;
				case ev_TX_RESET: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
				case ev_RX_NON_PROVISIONED: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					pr_info( "Bonding driver: ASM Tx Link Status for link-%u: RX::NON_PROVISIONED received from CO. Current state: TX_ACCEPTABLE\n", linkid);
					break;
				case ev_RX_SHOULDNT_BE_USED: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
				case ev_RX_ACCEPTABLE: PRIV->fsm.txlinkstatus[linkid] = TX_SELECTED;
					break;
        		case ev_RX_SELECTED: PRIV->fsm.txlinkstatus[linkid] = TX_ACTIVE;
					activate_tx_link(linkid);
					break;
				default: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					pr_crit( "Bonding driver: ASM Tx Link Status for link-%u: Unknown event (%u)\n", linkid, event);
			}
			break;

        case TX_SELECTED:
			switch (event) {
				case ev_TX_SHOWTIME_ENTRY: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
        		case ev_TX_SHOWTIME_EXIT: PRIV->fsm.txlinkstatus[linkid] = TX_SHOULDNT_BE_USED;
					break;
				case ev_TX_RESET: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
				case ev_RX_NON_PROVISIONED: PRIV->fsm.txlinkstatus[linkid] = TX_SELECTED;
					pr_info( "Bonding driver: ASM Tx Link Status for link-%u: RX::NON_PROVISIONED received from CO. Current state: TX_SELECTED\n", linkid);
					break;
				case ev_RX_SHOULDNT_BE_USED: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					break;
				case ev_RX_ACCEPTABLE: PRIV->fsm.txlinkstatus[linkid] = TX_SELECTED;
					break;
        		case ev_RX_SELECTED: PRIV->fsm.txlinkstatus[linkid] = TX_ACTIVE;
					activate_tx_link(linkid);
					break;
				default: PRIV->fsm.txlinkstatus[linkid] = TX_SELECTED;
					pr_crit( "Bonding driver: ASM Tx Link Status for link-%u: Unknown event (%u)\n", linkid, event);
			}
			break;

		case TX_ACTIVE:
			switch (event) {
				case ev_TX_SHOWTIME_ENTRY: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					deactivate_tx_link(linkid);
					break;
        		case ev_TX_SHOWTIME_EXIT: PRIV->fsm.txlinkstatus[linkid] = TX_SHOULDNT_BE_USED;
					deactivate_tx_link(linkid);
					break;
				case ev_TX_RESET: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					deactivate_tx_link(0);
					deactivate_tx_link(1);
					break;
				case ev_RX_NON_PROVISIONED: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					deactivate_tx_link(linkid);
					pr_info( "Bonding driver: ASM Tx Link Status for link-%u: RX::NON_PROVISIONED received from CO. Current state: TX_ACTIVE\n", linkid);
					break;
				case ev_RX_SHOULDNT_BE_USED: PRIV->fsm.txlinkstatus[linkid] = TX_ACCEPTABLE;
					deactivate_tx_link(linkid);
					break;
				case ev_RX_ACCEPTABLE: PRIV->fsm.txlinkstatus[linkid] = TX_SELECTED;
					deactivate_tx_link(linkid);
					break;
        		case ev_RX_SELECTED: PRIV->fsm.txlinkstatus[linkid] = TX_ACTIVE;
					break;
				default: PRIV->fsm.txlinkstatus[linkid] = TX_ACTIVE;
					pr_crit( "Bonding driver: ASM Tx Link Status for link-%u: Unknown event (%u)\n", linkid, event);
			}
			break;
		default:
			pr_crit( "Bonding driver: ASM Tx Link Status for link-%u: Unknown State (%u)\n", linkid, PRIV->fsm.txlinkstatus[linkid]);
	}

	if( event == ev_TX_RESET || prev_state != PRIV->fsm.txlinkstatus[linkid] ) {
		print_tx_link_state_xn( linkid, prev_state, PRIV->fsm.txlinkstatus[linkid], (u32)event);
	}
}

static void
asm_rx_link_fsm( ASMRxLinkStatusFSMEvent_e event, u32 linkid )
{
	u32 prev_state = PRIV->fsm.rxlinkstatus[linkid];

	switch (PRIV->fsm.rxlinkstatus[linkid]) {

		case RX_UNUSABLE:
			
			switch (event) {
				case ev_RX_SHOWTIME_ENTRY: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
				    activate_rx_link(linkid);
					break;
        		case ev_RX_SHOWTIME_EXIT: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
				    deactivate_rx_link(linkid);
					break;
				case ev_TX_NON_PROVISIONED: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
					pr_info( "Bonding driver: ASM Rx Link Status for link-%u: TX::NON_PROVISIONED received from CO. Current state: RX_UNUSABLE\n", linkid);
					break;
				case ev_RX_RESET:
				case ev_TX_SHOULDNT_BE_USED:
				case ev_TX_ACCEPTABLE:
        		case ev_TX_SELECTED: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
					break;
				default: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
					pr_warn( "Bonding driver: ASM Rx Link Status for link-%u: Unknown event (%u) for state RX_UNUSABLE\n", linkid, event);
			}
			break;

		case RX_SHOULDNT_BE_USED:
			switch (event) {
				case ev_RX_SHOWTIME_ENTRY: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
				    activate_rx_link(linkid);
					break;
        		case ev_RX_SHOWTIME_EXIT: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
				    deactivate_rx_link(linkid);
					break;
				case ev_RX_RESET: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
					break;
				case ev_TX_NON_PROVISIONED: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
					pr_info( "Bonding driver: ASM Rx Link Status for link-%u: TX::NON_PROVISIONED received from CO. Current state: RX_SHOULDNT_BE_USED\n", linkid);
					break;
				case ev_TX_SHOULDNT_BE_USED: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
					break;
				case ev_TX_ACCEPTABLE: PRIV->fsm.rxlinkstatus[linkid] = RX_ACCEPTABLE;
					break;
        		case ev_TX_SELECTED: PRIV->fsm.rxlinkstatus[linkid] = RX_SELECTED;
				    set_rx_data_acceptable(linkid);
					break;
				default: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
					pr_warn( "Bonding driver: ASM Rx Link Status for link-%u: Unknown event (%u) for state RX_SHOULDNT_BE_USED\n", linkid, event);
			}
			break;

        case RX_ACCEPTABLE:
			switch (event) {
				case ev_RX_SHOWTIME_ENTRY: PRIV->fsm.rxlinkstatus[linkid] = RX_ACCEPTABLE;
				    activate_rx_link(linkid);
					break;
        		case ev_RX_SHOWTIME_EXIT: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
				    deactivate_rx_link(linkid);
					break;
				case ev_RX_RESET: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
					break;
				case ev_TX_NON_PROVISIONED: PRIV->fsm.rxlinkstatus[linkid] = RX_ACCEPTABLE;
					pr_info( "Bonding driver: ASM Rx Link Status for link-%u: TX::NON_PROVISIONED received from CO. Current state: RX_ACCEPTABLE\n", linkid);
					break;
				case ev_TX_SHOULDNT_BE_USED: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
					break;
				case ev_TX_ACCEPTABLE: PRIV->fsm.rxlinkstatus[linkid] = RX_ACCEPTABLE;
					break;
        		case ev_TX_SELECTED: PRIV->fsm.rxlinkstatus[linkid] = RX_SELECTED;
				    set_rx_data_acceptable(linkid);
					break;
				default: PRIV->fsm.rxlinkstatus[linkid] = RX_ACCEPTABLE;
					pr_warn( "Bonding driver: ASM Rx Link Status for link-%u: Unknown event (%u) for state RX_ACCEPTABLE\n", linkid, event);
			}
			break;

		case RX_SELECTED:
			switch (event) {
				case ev_RX_SHOWTIME_ENTRY: PRIV->fsm.rxlinkstatus[linkid] = RX_SELECTED;
				    activate_rx_link(linkid);
					break;
        		case ev_RX_SHOWTIME_EXIT: PRIV->fsm.rxlinkstatus[linkid] = RX_UNUSABLE;
				    set_rx_data_unacceptable(linkid);
				    deactivate_rx_link(linkid);
					break;
				case ev_RX_RESET: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
				    set_rx_data_unacceptable(linkid);
					break;
				case ev_TX_NON_PROVISIONED: PRIV->fsm.rxlinkstatus[linkid] = RX_SELECTED;
					pr_info( "Bonding driver: ASM Rx Link Status for link-%u: TX::NON_PROVISIONED received from CO. Current state: RX_SELECTED\n", linkid);
					break;
				case ev_TX_SHOULDNT_BE_USED: PRIV->fsm.rxlinkstatus[linkid] = RX_SHOULDNT_BE_USED;
				    set_rx_data_unacceptable(linkid);
					break;
				case ev_TX_ACCEPTABLE: PRIV->fsm.rxlinkstatus[linkid] = RX_ACCEPTABLE;
				    set_rx_data_unacceptable(linkid);
					break;
        		case ev_TX_SELECTED: PRIV->fsm.rxlinkstatus[linkid] = RX_SELECTED;
					break;
				default: PRIV->fsm.rxlinkstatus[linkid] = RX_SELECTED;
					pr_warn( "Bonding driver: ASM Rx Link Status for link-%u: Unknown event (%u) for state RX_SELECTED\n", linkid, event);
			}
			break;
		default:
			pr_warn( "Bonding driver: ASM Rx Link Status for link-%u: Unknown State (%u)\n", linkid, PRIV->fsm.rxlinkstatus[linkid]);
	}

	if( event == ev_RX_RESET || prev_state != PRIV->fsm.rxlinkstatus[linkid] ) {
		print_rx_link_state_xn( linkid, prev_state, PRIV->fsm.rxlinkstatus[linkid], (u32)event);
	}
}

static inline ASMTxLinkStatusFSMEvent_e
asm_get_tx_linkstatus_event( asm_cell_t *asmcell, u32 co_link_idx )
{
	u8 tx_link_status = (co_link_idx==0 ? asmcell->tx_link0_status : asmcell->tx_link1_status);
	ASMTxLinkStatusFSMEvent_e retval = ev_TX_NON_PROVISIONED;

	switch (tx_link_status) {
		case 1: retval = ev_TX_SHOULDNT_BE_USED; break;
		case 2: retval = ev_TX_ACCEPTABLE; break;
		case 3: retval = ev_TX_SELECTED; break;
		default: retval = ev_TX_NON_PROVISIONED; break;
	}

	return retval;
}

static inline ASMRxLinkStatusFSMEvent_e
asm_get_rx_linkstatus_event( asm_cell_t *asmcell, u32 co_link_idx )
{
	u8 rx_link_status = (co_link_idx==0 ? asmcell->rx_link0_status : asmcell->rx_link1_status);
	ASMRxLinkStatusFSMEvent_e retval = ev_RX_NON_PROVISIONED;

	switch (rx_link_status) {
		case 1: retval = ev_RX_SHOULDNT_BE_USED; break;
		case 2: retval = ev_RX_ACCEPTABLE; break;
		case 3: retval = ev_RX_SELECTED; break;
		default: retval = ev_RX_NON_PROVISIONED; break;
	}

	return retval;
}

/* Regular-FSM */
static inline void
asm_fsm( ASMFSMEvent_e event, u32 linkid, void *data )
{
	asm_cell_t *asmcell = (asm_cell_t *)data;

//pr_crit( "bonding driver: %s: event: %u, linkid: %u\n", __func__, event, linkid );

	switch (PRIV->fsm.state) {

		case LEARNING:
			switch (event) {
				case SHOWTIME_ENTRY: PRIV->fsm.state = LEARNING;
					asm_tx_link_fsm( ev_TX_SHOWTIME_ENTRY, linkid );
					asm_rx_link_fsm( ev_RX_SHOWTIME_ENTRY, linkid );
					asm_fsm_enable_txtimer( linkid);
					break;

				case SHOWTIME_EXIT: PRIV->fsm.state = LEARNING;
					asm_tx_link_fsm( ev_TX_SHOWTIME_EXIT, linkid );
					asm_rx_link_fsm( ev_RX_SHOWTIME_EXIT, linkid );
					asm_fsm_disable_txtimer( linkid);
					break;

				case TRANSMIT: PRIV->fsm.state = LEARNING;
					asm_fsm_enable_txtimer( linkid);
					break;

				case RESET_ASM: PRIV->fsm.state = LEARNING;
					asm_tx_link_fsm( ev_TX_RESET, 0 );
					asm_rx_link_fsm( ev_RX_RESET, 0 );
					asm_tx_link_fsm( ev_TX_RESET, 1 );
					asm_rx_link_fsm( ev_RX_RESET, 1 );
					break;

				case NORMAL_ASM: PRIV->fsm.state = STEADY;
					PRIV->bonding_grp_cfg.grpid = asmcell->grpid;
					PRIV->bonding_grp_cfg.msg_type = asmcell->msg_type;
					PRIV->bonding_grp_cfg.num_links = asmcell->num_links;
					PRIV->bonding_grp_cfg.link_map[linkid] = asmcell->tx_lnk_no;
					PRIV->bonding_grp_cfg.link_map[linkid?0:1] = asmcell->tx_lnk_no ? 0 : 1;
					PRIV->bonding_grp_cfg.link_rmap[asmcell->tx_lnk_no] = linkid;
					PRIV->bonding_grp_cfg.link_rmap[asmcell->tx_lnk_no?0:1] = linkid ? 0 : 1;

					update_tx_config( PRIV->bonding_grp_cfg.msg_type == 0x01);
					update_rx_config( PRIV->bonding_grp_cfg.msg_type == 0x01);

					asm_tx_link_fsm( asm_get_rx_linkstatus_event(asmcell, 0), PRIV->bonding_grp_cfg.link_rmap[0]);
					asm_tx_link_fsm( asm_get_rx_linkstatus_event(asmcell, 1), PRIV->bonding_grp_cfg.link_rmap[1]);
					asm_rx_link_fsm( asm_get_tx_linkstatus_event(asmcell, 0), PRIV->bonding_grp_cfg.link_rmap[0]);
					asm_rx_link_fsm( asm_get_tx_linkstatus_event(asmcell, 1), PRIV->bonding_grp_cfg.link_rmap[1]);

					break;

				case STOP: PRIV->fsm.state = STOPPED;
					asm_fsm_disable_txtimer(0);
					asm_fsm_disable_txtimer(1);
					break;

				case RECEIVE: PRIV->fsm.state = LEARNING;
					pr_crit( "Bonding driver: ASM FSM: Invalid Event RECEIVE (%u) while in LEARNING state\n", event);
					break;

				default: PRIV->fsm.state = LEARNING;
					pr_crit( "Bonding driver: ASM FSM: Unknown event (%u) while in LEARNING state\n", event);
			}
			break;

		case STEADY:
			switch (event) {

				case SHOWTIME_ENTRY: PRIV->fsm.state = STEADY;
					asm_tx_link_fsm( ev_TX_SHOWTIME_ENTRY, linkid );
					asm_rx_link_fsm( ev_RX_SHOWTIME_ENTRY, linkid );
					asm_fsm_enable_txtimer( linkid);
					break;

				case SHOWTIME_EXIT: PRIV->fsm.state = STEADY;
					asm_tx_link_fsm( ev_TX_SHOWTIME_EXIT, linkid );
					asm_rx_link_fsm( ev_RX_SHOWTIME_EXIT, linkid );
					asm_fsm_disable_txtimer( linkid);
					break;

				case TRANSMIT: PRIV->fsm.state = STEADY;
					/* Transmit ASM provided link is in SHOWTIME */
					if (PRIV->fsm.txlinkstatus[linkid] > TX_SHOULDNT_BE_USED) {
						asm_fsm_create_and_transmit( linkid );
						asm_fsm_enable_txtimer( linkid);
					}
					break;

				case RESET_ASM: PRIV->fsm.state = LEARNING;
					asm_tx_link_fsm( ev_TX_RESET, 0 );
					asm_rx_link_fsm( ev_RX_RESET, 0 );
					asm_tx_link_fsm( ev_TX_RESET, 1 );
					asm_rx_link_fsm( ev_RX_RESET, 1 );
					break;

				case NORMAL_ASM: PRIV->fsm.state = STEADY;
					asm_tx_link_fsm( asm_get_rx_linkstatus_event(asmcell, 0), PRIV->bonding_grp_cfg.link_rmap[0]);
					asm_tx_link_fsm( asm_get_rx_linkstatus_event(asmcell, 1), PRIV->bonding_grp_cfg.link_rmap[1]);
					asm_rx_link_fsm( asm_get_tx_linkstatus_event(asmcell, 0), PRIV->bonding_grp_cfg.link_rmap[0]);
					asm_rx_link_fsm( asm_get_tx_linkstatus_event(asmcell, 1), PRIV->bonding_grp_cfg.link_rmap[1]);
					break;

				case STOP: PRIV->fsm.state = STOPPED;
					asm_fsm_disable_txtimer(0);
					asm_fsm_disable_txtimer(1);
					break;

				case RECEIVE: PRIV->fsm.state = STEADY;
					pr_crit( "Bonding driver: ASM FSM: Invalid Event RECEIVE (%u) while in STEADY state\n", event);
					break;

				default: PRIV->fsm.state = STEADY;
					pr_crit( "Bonding driver: ASM FSM: Unknown event (%u) while in STEADY state\n", event);
			}
			break;

		case STOPPED: PRIV->fsm.state = LEARNING;
			pr_crit( "Bonding driver: ASM FSM: STOPPED State (%u) not expected\n", PRIV->fsm.state);
			break;

		default:
			pr_crit( "Bonding driver: ASM FSM: Unknown State (%u)\n", PRIV->fsm.state);
	}
}
#endif /* not ASM_LOOPBACK */
