
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file contains the logic of AAL5 Reassembly component of IMA+ Bonding
 * Driver. Within the IMA+ Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |   1   | Reassembly FSM Library              | ima_rx_reassembler.c(.h)  |<=
 *    |   2   | Bonding Library                     | ima_rx_bonding.c(.h)      |
 *    |   3   | ATM Cell Decode Library             | ima_rx_atm_cell.h         |
 *    |   4   | Line Queue Abstraction Library      | ima_rx_queue.c(.h)        |
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 *
 *    Unit Tests: 1. src/test/ut_rx_reassembly_testgroup_1.c
 *                2. src/test/ut_rx_bonding_and_reassembly_testgroup_1.c
 */

#ifdef __KERNEL__
#include <linux/skbuff.h>
#include <linux/byteorder/generic.h>
#include <linux/spinlock.h>
#include <linux/crc32.h>
#else
#include <stdint.h>
#include <stddef.h> // NULL
#include <arpa/inet.h> // ntohs
#endif
#ifdef CONFIG_LTQ_CBM
#include <net/lantiq_cbm_api.h>
#endif

#include "atm/ima_atm_rx.h"
#include "atm/ima_rx_atm_cell.h"

#ifdef __KERNEL__
#ifndef PRIV
#define PRIV (&g_privdata)
#endif

u8 *AAL5FrameAlloc(uint32_t cellcount)
{
	struct sk_buff *frame;

#ifdef CONFIG_LTQ_CBM
	frame = cbm_alloc_skb((cellcount * 48), GFP_ATOMIC);
#else
	frame = alloc_skb(128 + (cellcount)*48, GFP_ATOMIC);
	if (likely(frame))
		skb_reserve(frame, 128);
#endif

	return (u8 *)frame;
}

uint32_t AAL5FrameCopyCell(uint8_t *frame, uint8_t *cell)
{
	memcpy(skb_put((struct sk_buff *)frame, 48), cell, 48);
	return 0;
}

void AAL5FrameDiscard(uint8_t *frame)
{
	kfree_skb((struct sk_buff *)frame);
}

u8 *AAL5FrameGetData(uint8_t *frame)
{
	return ((struct sk_buff *)frame)->data;
}

void CallTCDriverCB(int destid, uint8_t *aal5frame, uint32_t aal5framelen)
{
	struct atm_aal5_t atm_rx_pkt = {0};

	atm_rx_pkt.skb = (struct sk_buff *)aal5frame;
	atm_rx_pkt.conn = (destid >> 3) & 0xf;

	send_to_vrx518_tc_driver(&atm_rx_pkt);
}

#ifndef AAL5FRAME_ALLOC
#define AAL5FRAME_ALLOC(frame, cellcount) ((frame) = AAL5FrameAlloc((cellcount)))
#endif

#ifndef AAL5FRAME_COPY_CELL
#define AAL5FRAME_COPY_CELL(frame, data, len) AAL5FrameCopyCell((frame), (data))
#endif

#ifndef AAL5FRAME_DISCARD
#define AAL5FRAME_DISCARD(frame) AAL5FrameDiscard((frame))
#endif

#ifndef AAL5FRAME_DATA
#define AAL5FRAME_DATA(frame) AAL5FrameGetData((frame))
#endif

#define PRINT(format, ...)

#ifndef CALL_TCDRIVER_CB
#define CALL_TCDRIVER_CB(destid, aal5frame, aal5framelen) CallTCDriverCB((destid), (aal5frame), (aal5framelen))
#endif

#endif

void
init_rx_reassembler( unsigned int maxcellcount )
{
	PRIV->reassemble.rejectionmode = 0;
	PRIV->reassemble.cellcount = 0;
	PRIV->reassemble.frame = NULL;
	PRIV->reassemble.maxcellcount = maxcellcount; 
	/* Others...    *
	 * PRIV->reassemble.lastsid
	 */
}

void
reassemble_aal5(ima_rx_atm_header_t *cell, u32 sid, u32 sid_range, u32 linkid)
{
	int goodmood = 1;
	uint32_t crc;
	uint16_t trailer_length;

#if 0
	if( cell->pt2 ) // Congestion. Reset current AAL5 frame
	{
		//print_hex_dump(KERN_CRIT, "Bonding Driver: DUMPING CONGESTION ATM Cell: ", DUMP_PREFIX_OFFSET, 16, 1, cell, 56, false);
		// Update cell-stats by incrementing dropped cell count
		UPDATE_STATS( droppedcells[linkid], 1);
		if( PRIV->reassemble.cellcount ) {

			PRIV->reassemble.cellcount = 0;
			AAL5FRAME_DISCARD( PRIV->reassemble.frame );
			PRIV->reassemble.frame = NULL;
			UPDATE_STATS( discardedframecount[INVALIDPT2ORCLP], 1);
		}

		return;
	}
#endif

	if (PRIV->reassemble.rejectionmode) {
		// In bad mood...

		if (cell->pt1)
		{
			// Sunshine

			PRIV->reassemble.rejectionmode = 0;
			PRIV->reassemble.cellcount = 0;
			PRIV->reassemble.frame = NULL;
		}

		// And thats it
		return;
	}

	// In good mood

	if ( PRIV->reassemble.cellcount && ( PRIV->reassemble.lastsid != ( sid ? (sid - 1) : (sid_range - 1) ) ) )
	{
		UPDATE_STATS( discardedframecount[SIDJUMP], 1); //++(PRIV->stats.discardedframecount[SIDJUMP]);
		PRINT("Frame discarded. SID jump count goes up to %u\n", PRIV->stats.discardedframecount[SIDJUMP]);
		goodmood = 0;

	} else if ( cell->pt1 ) // Last cell. Frame to depart.
	{
		int validframe = 1;

		if( !PRIV->reassemble.frame ) { // also the first cell

			AAL5FRAME_ALLOC( PRIV->reassemble.frame, PRIV->reassemble.maxcellcount );
			PRIV->reassemble.cellcount = 0;

			if( unlikely( !PRIV->reassemble.frame ) ) { // Allocation failed for this unicellular AAL5 frame

				PRINT("Frame discarded. Allocation failure\n" );
				UPDATE_STATS( discardedframecount[ALLOCFAILURE], 1);

				// Reset counters
				PRIV->reassemble.frame = NULL;
				PRIV->reassemble.cellcount = 0;
				return;
			}
		}

		++(PRIV->reassemble.cellcount);
#ifdef BADFRAME_THEN_DUMPITSCELLS
		memcpy( PRIV->celldumpbuffer + (PRIV->reassemble.cellcount-1)*64, (u8 *)cell, 56);
#endif /* BADFRAME_THEN_DUMPITSCELLS */
		AAL5FRAME_COPY_CELL( PRIV->reassemble.frame, cell->payload, 48 );

		// Validate frame
		trailer_length = /*ntohs*/( *((uint16_t *)((AAL5FRAME_DATA(PRIV->reassemble.frame)) + (PRIV->reassemble.cellcount)*48 - 6)) );
		if( ( ( trailer_length + 8 - 1 ) / 48 ) != ( PRIV->reassemble.cellcount - 1) )
		{
			/* Invalid Length */
			pr_info("Bonding Driver: Invalid AAL5 Trailer length (%d) given number of cells in the AAL5 frame is %d\n", trailer_length, PRIV->reassemble.cellcount);
#ifdef BADFRAME_THEN_DUMPITSCELLS
			print_hex_dump(KERN_CRIT, "Bonding Driver: AAL5 Cells: ", DUMP_PREFIX_OFFSET, 16, 1, PRIV->celldumpbuffer, PRIV->reassemble.cellcount*64, false);
#endif /* BADFRAME_THEN_DUMPITSCELLS */
			UPDATE_STATS( discardedframecount[INVALIDLENGTH], 1); // ++(PRIV->stats.discardedframecount[INVALIDLENGTH]);
			PRINT("Frame discarded. Invalid length count goes up to %u\n", PRIV->stats.discardedframecount[INVALIDLENGTH]);
			validframe = 0;

		} else {

			crc = crc32_be(0xFFFFFFFF, AAL5FRAME_DATA(PRIV->reassemble.frame), (PRIV->reassemble.cellcount*48) - 4 ) ^ 0xFFFFFFFF;
			if( crc != ntohl( *((uint32_t *)((AAL5FRAME_DATA(PRIV->reassemble.frame)) + (PRIV->reassemble.cellcount)*48 - 4))))
			{
				/* Invalid CRC */
				/*pr_info("Invalid CRC: calculated crc = 0x%08X trailer crc = 0x%08X\n", (ntohl(crc)), \
					*((uint32_t *)((AAL5FRAME_DATA(PRIV->reassemble.frame)) + (PRIV->reassemble.cellcount)*48 - 4)) ); */
				UPDATE_STATS( discardedframecount[INVALIDCRC32], 1); // ++(PRIV->stats.discardedframecount[INVALIDCRC32]);
				PRINT("Frame discarded. Invalid CRC32 count goes up to %u\n", PRIV->stats.discardedframecount[INVALIDCRC32]);
				validframe = 0;
			}
		}

		if( validframe )
		{
			// Goodbye and good luck
			REPORT( "Frame accepted", PRIV->reassemble.cellcount, 0 );
			UPDATE_STATS( acceptedframecount, 1); //++(PRIV->stats.acceptedframecount);
#ifdef DISABLE_RX_ATMSTACK
			AAL5FRAME_DISCARD( PRIV->reassemble.frame );
#else
			CALL_TCDRIVER_CB( *(u16 *)(cell->padding), PRIV->reassemble.frame, 48* (PRIV->reassemble.cellcount ));
#endif
			PRINT( "AAL5 frame with trailer length %u received\n", trailer_length );

		} else {

			// Else discard the frame
			REPORT( "Frame discarded", 1, PRIV->reassemble.cellcount );
			PRINT("Frame discarded\n");
			AAL5FRAME_DISCARD( PRIV->reassemble.frame );
		}

		// Reset counters
		PRIV->reassemble.frame = NULL;
		PRIV->reassemble.cellcount = 0;
		return;

	} else if( PRIV->reassemble.maxcellcount <= (PRIV->reassemble.cellcount + 1) ) // Houseful -- cannot accomodate next cell
	{
		PRINT("Frame discarded. Buffer overflow\n" );
		UPDATE_STATS( discardedframecount[SIZEOVERFLOW], 1);
		goodmood = 0;
	}

	if( !goodmood )
	{

		PRIV->reassemble.rejectionmode = 1;
		if( PRIV->reassemble.frame )
		{
			REPORT( "Frame discarded", 2, PRIV->reassemble.cellcount );
			AAL5FRAME_DISCARD( PRIV->reassemble.frame );
			PRIV->reassemble.frame = NULL;
		}

		return;
	}

	// Good mood

	if( PRIV->reassemble.frame ) // Not first cell. Growing frame
	{
		PRIV->reassemble.lastsid = sid;
		++(PRIV->reassemble.cellcount);
#ifdef BADFRAME_THEN_DUMPITSCELLS
		memcpy( PRIV->celldumpbuffer + (PRIV->reassemble.cellcount-1)*64, (u8 *)cell, 56);
#endif /* BADFRAME_THEN_DUMPITSCELLS */
		AAL5FRAME_COPY_CELL( PRIV->reassemble.frame, cell->payload, 48 );

	} else { // First cell. New Frame will be born thus

		AAL5FRAME_ALLOC( PRIV->reassemble.frame, PRIV->reassemble.maxcellcount );
		if( unlikely( !PRIV->reassemble.frame ) ) { // Allocation failure

			// Keep rejecting all cells till the end of this AAL5 frame
			PRIV->reassemble.rejectionmode = 1;
			UPDATE_STATS( discardedframecount[ALLOCFAILURE], 1);
			PRINT("Frame discarded. Allocation failure\n" );
			return;
		}

		PRIV->reassemble.lastsid = sid;
		PRIV->reassemble.cellcount = 1;
#ifdef BADFRAME_THEN_DUMPITSCELLS
		memcpy( PRIV->celldumpbuffer + (PRIV->reassemble.cellcount-1)*64, (u8 *)cell, 56);
#endif /* BADFRAME_THEN_DUMPITSCELLS */
		AAL5FRAME_COPY_CELL( PRIV->reassemble.frame, cell->payload, 48 );
	}

}
