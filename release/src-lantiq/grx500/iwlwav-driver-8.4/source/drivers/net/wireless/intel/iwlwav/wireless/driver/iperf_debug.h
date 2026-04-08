/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/* $Id$ */

#ifndef _IPERF_DEBUG_HDR
#define _IPERF_DEBUG_HDR

#if (defined MTLK_DEBUG_IPERF_PAYLOAD_RX) || (defined MTLK_DEBUG_IPERF_PAYLOAD_TX)

#include "rod.h"
#include "bufmgr.h"

/* range of ports used by iperf*/
#define MTLK_DEBUG_IPERF_PORT_START     30000
#define MTLK_DEBUG_IPERF_PORT_STOP      30010

#define  MTLK_IDEFS_ON

#define  MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"


#ifndef ETH_P_IP
#define ETH_P_IP 0x0800
#endif

#ifndef IPPROTO_UDP
#define IPPROTO_UDP 17
#endif

#define ETHER_ADDR_LEN 6
typedef struct _mtlk_eth_hdr 
{
   uint8 mac_dst[ETHER_ADDR_LEN];
   uint8 mac_src[ETHER_ADDR_LEN];
   uint16 h_proto;
}__MTLK_IDATA mtlk_eth_hdr;

typedef struct _mtlk_ipV4_hdr
{

#if defined __LITTLE_ENDIAN
	uint8 ihl:4;
	uint8 version:4;
#elif defined  __BIG_ENDIAN
	uint8 version:4;
	uint8 ihl:4;
#endif
	uint8  tos;
	uint16 len; 
	uint16 id;
	uint16 frag_off; //flags & frg_offset;
	uint8  ttl;
	uint8  protocol;
	uint16 checksum;
	uint32 src_addr;
	uint32 dst_addr;
}__MTLK_IDATA mtlk_ipV4_hdr;

typedef struct _mtlk_udp_hdr
{
	uint16 source;
	uint16 dest;
	uint16 pkt_len;
	uint16 check;
}__MTLK_IDATA mtlk_udp_hdr;


typedef struct _mtlk_payload_ts_debug_info_t {
  uint16 seq_no;
  uint32 tag_rx;
  uint32 tag_tx;
  uint16 rod_queue_head;
  uint16 rod_queue_pos;
  uint16 rod_queue_count;
  uint32 tag_tx_to_os;
} __MTLK_IDATA mtlk_payload_ts_debug_info_t;

struct mtlk_debug_iperf_payload_private_t {
  /* ordinal number of the received packet */
  uint32 tag_rx;
  /* ordinal number of the transmitted packet */
  uint32 tag_tx;
  /* first tx packet indicator */
  uint8 iperf_tx_first_packet;
  /* previous tx'ed packet */
  uint32 iperf_tx_prev_id;
  /* ordinal number of the packet that will be sent up */
  uint32 tag_tx_to_os;
};

typedef struct _mtlk_iperf_payload_t {
  /* iperf uses only 3 fields of this structure */
  uint32 iperf_id;
  uint32 iperf_tv_sec;
  uint32 iperf_tv_usec;
  /* driver uses this fields */
  mtlk_payload_ts_debug_info_t ts;
  /* MAC debug data goes below */
} __MTLK_IDATA mtlk_iperf_payload_t;

/*sum of all headers sizes of iperf pkt*/
#define IPERF_PKT_PAYLOAD_LEN (sizeof(mtlk_eth_hdr) + sizeof(mtlk_ipV4_hdr)+sizeof(mtlk_udp_hdr) + sizeof(mtlk_iperf_payload_t))

extern struct mtlk_debug_iperf_payload_private_t debug_iperf_priv;


mtlk_iperf_payload_t *
debug_ooo_is_iperf_pkt(uint8 * pkt);

void 
debug_ooo_analyze_packet(int on_receive, 
			 mtlk_nbuf_t * hPktDesc, 
			 unsigned short seq);

void __MTLK_IFUNC
mtlk_debug_process_iperf_payload_rx(reordering_queue *prod_queue, 
									int slot);

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#else /*#if (defined MTLK_DEBUG_IPERF_PAYLOAD_RX) || (defined MTLK_DEBUG_IPERF_PAYLOAD_TX)*/

#define mtlk_debug_process_iperf_payload_rx(X,Y)

#endif /*#if (defined MTLK_DEBUG_IPERF_PAYLOAD_RX) || (defined MTLK_DEBUG_IPERF_PAYLOAD_TX)*/

#endif /*#ifndef _IPERF_DEBUG_HDR*/
