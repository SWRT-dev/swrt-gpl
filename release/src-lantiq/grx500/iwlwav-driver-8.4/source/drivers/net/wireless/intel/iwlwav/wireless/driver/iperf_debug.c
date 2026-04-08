/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
* $Id$
*
* 
*
* Iperf OOO debugging technique support functions
*
*/
#include "mtlkinc.h"

#if (defined MTLK_DEBUG_IPERF_PAYLOAD_RX) || (defined MTLK_DEBUG_IPERF_PAYLOAD_TX)

#include "iperf_debug.h"

#define LOG_LOCAL_GID   GID_IPERF_DEBUG
#define LOG_LOCAL_FID   1

//TODO: how to initialize structure "Platform independetly"?
struct mtlk_debug_iperf_payload_private_t debug_iperf_priv={0};

/*****************************************************************************
**
** NAME         debug_ooo_analyze_packet
**
** PARAMETERS   on_receive          Modify payload on rx or on tx
**              pkt                 packet which payload to be modified
**
** RETURNS      none
**
** DESCRIPTION  This function modifies payload of UDP packet if it is sent/
**              received on port 30000. Used for debugging with iperf, which
**              uses only 12-bytes of payload for itself.
**
******************************************************************************/

mtlk_iperf_payload_t *
debug_ooo_is_iperf_pkt(uint8 * pkt)
{
  mtlk_eth_hdr *ethh;  
  mtlk_ipV4_hdr *ipiph;  
  mtlk_udp_hdr *uh;
  unsigned short dest_port;
  mtlk_iperf_payload_t *payload;

  /* sanity check */
  if (pkt == NULL)
    return NULL;

  /* we do not want anything but IP */
  ethh = (mtlk_eth_hdr *)pkt;

  if (NET_TO_HOST16(ethh->h_proto) != ETH_P_IP)
    return NULL;

  ipiph = (mtlk_ipV4_hdr *)((mtlk_eth_hdr *)pkt + 1);

  /* we do not want anything but UDP */
  if (ipiph->protocol != IPPROTO_UDP)
    return NULL;

  /* check destination port */
  uh = (mtlk_udp_hdr *)((char *)ipiph + (int)ipiph->ihl * sizeof(long));
  dest_port=NET_TO_HOST16(uh->dest);
  if ((dest_port < MTLK_DEBUG_IPERF_PORT_START) || (dest_port > MTLK_DEBUG_IPERF_PORT_STOP))
    return NULL;

  /* this is our payload */
  payload = (mtlk_iperf_payload_t *)(uh + 1);

  /* disable checksum */
  uh->check = 0;  

  return payload;
}

void
debug_ooo_analyze_packet (int on_receive, mtlk_nbuf_t * hPktDesc, unsigned short seq)
{
  /*TODO: Add lengths checks*/
  mtlk_iperf_payload_t *iperf;
  uint8 pu8PktBuffer[IPERF_PKT_PAYLOAD_LEN];
  
  if(MTLK_ERR_OK != mtlk_bufmgr_read(hPktDesc,0,IPERF_PKT_PAYLOAD_LEN,pu8PktBuffer))
  {
	  /*pkt is too small (so it's not iperf pkt) or failed to obtain data. */
	  return; 
  }
  
  /* is pkt of iperf type?*/
  iperf = debug_ooo_is_iperf_pkt(pu8PktBuffer);
  if (iperf == NULL)
  {
	/*pkt is not of iperf type*/
    return;
  }

   /*modifying pkt buffer to write RX or TX tags*/
   iperf->ts.seq_no = HOST_TO_NET16(seq);
   if (on_receive)
   {
		/*we are going to modify the payload on Rx side*/
		iperf->ts.tag_rx = HOST_TO_NET32(debug_iperf_priv.tag_rx);
		debug_iperf_priv.tag_rx++;
   } 
   else 
   {
		/* Tx side */
		uint32 iperf_id;
		iperf_id = NET_TO_HOST32(iperf->iperf_id);

		/* save iperf id for the first packet? */
		if (debug_iperf_priv.iperf_tx_first_packet) 
		{
			debug_iperf_priv.iperf_tx_first_packet = 0;
			debug_iperf_priv.iperf_tx_prev_id = iperf_id;
		} 
		else 
		{
			if (iperf_id <= debug_iperf_priv.iperf_tx_prev_id)
			{
				  WLOG_DD("WARNING: iperf sends ooo?: received id=%ld, prev id=%ld\n",
					(unsigned long)iperf_id, (unsigned long)debug_iperf_priv.iperf_tx_prev_id);
			}      
			/* save current id */
			debug_iperf_priv.iperf_tx_prev_id = iperf_id;
		}

		iperf->ts.tag_tx = HOST_TO_NET32(debug_iperf_priv.tag_tx);
		debug_iperf_priv.tag_tx++;

	}

   /*updating hPktDesc to reflect changes in the pktBuffer */
   mtlk_bufmgr_write(hPktDesc,0,IPERF_PKT_PAYLOAD_LEN,pu8PktBuffer);
}

void __MTLK_IFUNC
mtlk_debug_process_iperf_payload_rx(reordering_queue *prod_queue, 
									int slot)
{
	uint8 pu8PktData[IPERF_PKT_PAYLOAD_LEN];
	mtlk_iperf_payload_t *iperf;

	//If buffer is null or cannot read data from buffer.
	if(NULL == prod_queue->buff[slot] || 
	   MTLK_ERR_OK != mtlk_bufmgr_read(prod_queue->buff[slot],0,IPERF_PKT_PAYLOAD_LEN,pu8PktData))
	{		
		return;
	}

	/* Is current pkt from/for iperf?*/
	iperf = debug_ooo_is_iperf_pkt(pu8PktData);

	/* Update debug info inside the pkt*/
	if (iperf != NULL)
	{
		iperf->ts.rod_queue_head = HOST_TO_NET16((uint16)prod_queue->head);
		iperf->ts.rod_queue_count = HOST_TO_NET16((uint16)prod_queue->count);
		iperf->ts.rod_queue_pos = HOST_TO_NET16((uint16)slot);
		iperf->ts.tag_tx_to_os = HOST_TO_NET32(debug_iperf_priv.tag_tx_to_os);
		debug_iperf_priv.tag_tx_to_os++;				

		/*write changes to buffer*/
		mtlk_bufmgr_write(prod_queue->buff[slot],0,IPERF_PKT_PAYLOAD_LEN,pu8PktData);
	}
}

#else //#if (defined MTLK_DEBUG_IPERF_PAYLOAD_RX) || (defined MTLK_DEBUG_IPERF_PAYLOAD_TX)

#    ifdef _MSC_VER
#        pragma warning( disable : 4206 ) 
#    endif

#endif //#if (defined MTLK_DEBUG_IPERF_PAYLOAD_RX) || (defined MTLK_DEBUG_IPERF_PAYLOAD_TX)
