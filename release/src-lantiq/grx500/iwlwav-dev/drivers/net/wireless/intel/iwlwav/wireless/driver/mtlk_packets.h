/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 * 
 *
 * Packets structures and packet parsing routines
 *
 * Author: Vasyl' Nikolaenko
 */

#ifndef _MTLK_PACKETS_H_
#define _MTLK_PACKETS_H_

#include "mtlkerr.h"
#include "mtlk_osal.h"
#include "ieee80211defs.h"

#define  MTLK_IDEFS_ON
#define  MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_MTLK_PACKETS
#define LOG_LOCAL_FID   0

#define MTLK_ETH_DGRAM_TYPE_IP              (0x0800)
#define MTLK_ETH_DGRAM_TYPE_ARP             (0x0806)
#define MTLK_ETH_DGRAM_TYPE_IPV6            (0x86DD)
#define MTLK_ETH_RSN_PREAUTH                (0x88C7) /* 802.11i Pre-Authentication */
#define MTLK_ETH_TYPE_VLAN_TAG              (0x8100) /* 802.1Q VLAN tag */

#define MTLK_ETH_FRAME_SIZE                 (1500)
#define MTLK_ETH_TYPE_IPX                   (0x8137)
#define LLC_SNAP_TYPE_OFFSET                (6)
#define MTLK_ETH_DST_ADDR_OFFSET            (0)
#define MTLK_ETH_SRC_ADDR_OFFSET            (6)
#define MTLK_ETH_VLAN_TAG_OFFSET            (12)

/* From [1] section 18.12: the standard 802.2 Logical Link Control (LLC)
and SubNetwork Attachment Point (SNAP) header is as follows:
- LLC  (AA.AA.03)
- OUI1 (00)
- OUI2 (00.00)
- TYPE (08.00) */
#define MTLK_ETH_SIZE_LLC_AND_SNAP_HDR      (8)

/* Ethernet type/length field */
#define MTLK_ETH_SIZEOF_ETHERNET_TYPE       (2)

/* IEEE 802.1Q VLAN tag size */
#define MTLK_ETH_SIZEOF_VLAN_TAG            (4)

#define MTLK_ETH_DST_ADDR(x)                (((uint8*)(x)) + MTLK_ETH_DST_ADDR_OFFSET)
#define MTLK_ETH_SRC_ADDR(x)                (((uint8*)(x)) + MTLK_ETH_SRC_ADDR_OFFSET)
#define MTLK_ETH_VLAN_TAG(x)                (((uint8*)(x)) + MTLK_ETH_VLAN_TAG_OFFSET)
#define MTLK_ETH_TYPE_OFFSET(x)             (ntohs(*((uint16*)((uint8*)(x) + 12))) > MTLK_ETH_FRAME_SIZE ? (12) : (20))
#define MTLK_ETH_TYPE(x)                    ((uint8*)(x) + MTLK_ETH_TYPE_OFFSET(x))

#define MTLK_ETH_LENGTH(x)                  (((uint8*)(x)) + 12)
#define MTLK_ETH_PAYLOAD(x)                 (((uint8*)(x)) + 14)

#define MTLK_ETH_GET_ETHER_TYPE(data)       (*((uint16*)MTLK_ETH_TYPE(data)))

/* [1] Internetworking with TCP/IP Volume 1 Principles, Protocols, and Architectures.
Douglas E. Comer. Fourth Edition, */

/* From [1] section 2.4.10: the size of the ethernet header is 14 bytes in the format: */
/*     Index      Description */
/*     0-5        Recipient hardware address */
/*     6-11       Sender hardware address */
/*     12-13      Ethernet Type/Length field. */
/*     14...      Frame data */
#define MTLK_ETH_HDR_SIZE                   (14)


/*
 *      LLC Header
 */
typedef struct _mtlk_llc_hdr_t
{
  uint8   dsap;           // destination service access point
  uint8   ssap;           // source service access point
  uint8   control;        // control field
} __MTLK_IDATA mtlk_llc_hdr_t;

/*
 *      SNAP Header
 */
typedef struct _mtlk_snap_hdr_t
{
  uint8   out1;           // out1 + out2 = organization code
  uint16  out2;
  uint16  ether_type;     // ethernet type
} __MTLK_IDATA mtlk_snap_hdr_t;


typedef struct _mtlk_eth_hdr_t
{
  uint8 dest[6];
  uint8 src[6];
  uint16 h_proto;
} __MTLK_IDATA mtlk_eth_hdr_t;

typedef struct _mtlk_mac_tx_hdr_t
{
  mtlk_eth_hdr_t eth_hdr;
  uint8          data;
} __MTLK_IDATA mtlk_mac_tx_hdr_t;


static __MTLK_IFUNC __INLINE uint8
get_ip6_tos (const uint8 *pkt)
{
  uint16 data = NET_TO_HOST16(*(uint16 *)pkt);
  data >>= 4;
  return (uint8)data;
}

static __MTLK_IFUNC __INLINE uint8
get_ip4_tos (const uint8 *pkt)
{
  uint8 data = *(pkt + 1);
  return data;
}

static __MTLK_IFUNC __INLINE int
mtlk_wlan_get_hdrlen(uint16 fc)
{
  int hdrlen = IEEE80211_3ADDR_LEN;
  uint16 stype = WLAN_FC_GET_STYPE(fc);

  switch (WLAN_FC_GET_TYPE(fc)) {
  case IEEE80211_FTYPE_DATA:
    if ((fc & IEEE80211_FCTL_FROMDS) && (fc & IEEE80211_FCTL_TODS))
      hdrlen = IEEE80211_4ADDR_LEN;
    if (stype & IEEE80211_STYPE_QOS_DATA)
      hdrlen += 2;
    break;
  case IEEE80211_FTYPE_CTL:
    switch (WLAN_FC_GET_STYPE(fc)) {
    case IEEE80211_STYPE_CTS:
    case IEEE80211_STYPE_ACK:
      hdrlen = IEEE80211_1ADDR_LEN;
        break;
    default:
      hdrlen = IEEE80211_2ADDR_LEN;
      break;
    }
    break;
  }

  return hdrlen;
}

static __MTLK_IFUNC __INLINE uint16
mtlk_wlan_pkt_get_frame_ctl (const uint8 *data)
{
  /* 
   * MAC gives us frame_ctl in Little Endian format
   * (not in Network endiannes that is Big Endian).
   * That's why we can't use here ntohs()
   */
  uint16 frame_ctl = *(uint16*)data;
  frame_ctl = WLAN_TO_HOST16(frame_ctl);

  return frame_ctl;
}

static __MTLK_IFUNC __INLINE uint16
mtlk_wlan_pkt_get_qos_ctl (uint8 *data, int hdr_len)
{
  uint16 qos_ctl = *(uint16*) (data + hdr_len - 2);   /* two last bytes in .11n QoS header */
  /* the same Little endiannes as w/ frame_ctl (see above) */
  qos_ctl = WLAN_TO_HOST16(qos_ctl);

  return qos_ctl;
}

static __MTLK_IFUNC __INLINE uint16
mtlk_wlan_pkt_get_seq (uint8 *data)
{
  uint16 seq_ctl, seq;

  seq_ctl = *(uint16*) WLAN_GET_SEQ_CTL(data);
  seq_ctl = WLAN_TO_HOST16(seq_ctl);
  seq = (seq_ctl & IEEE80211_SCTL_SEQ) >> 4; /* 0x000F - frag#, 0xFFF0 - seq */

  return seq;
}

#define mtlk_wlan_pkt_is_802_1X(eth_type_net_order)   ( HOST_TO_NET16_CONST(ETH_P_PAE) == (eth_type_net_order) )
#define mtlk_wlan_pkt_is_pre_auth(eth_type_net_order) ( HOST_TO_NET16_CONST(MTLK_ETH_RSN_PREAUTH) == (eth_type_net_order) )

/*****************************************************************************
**
** NAME         mtlk_wlan_drop_llc
**
** PARAMETERS   pucLLC
**
** RETURNS      1 if the LLC header should not be sent to the upper layer, 0 
**              otherwise
** DESCRIPTION  The function  may be expanded for various LLC header types, IPX is
**              one case in which the LLC header will not be dropped and will be
**              sent to the upper layer.
******************************************************************************/
static __MTLK_IFUNC __INLINE int
mtlk_wlan_drop_llc(uint8 * data)
{
    int drop = 1;
    /* Little Endian from the network must be transposed to host endianess */
    uint16 eth_type = NET_TO_HOST16(*(uint16 *)(data + LLC_SNAP_TYPE_OFFSET));

    switch( eth_type ) 
    {
    case MTLK_ETH_TYPE_IPX:                             
        ILOG2_V("INFO: pLLCType = MTLK_ETH_TYPE_IPX\n");
        drop = 0;
        break;
    
    default:
        break;
    }

    return drop;
}

/*****************************************************************************
**
** NAME         mtlk_wlan_get_mac_addrs
**
** PARAMETERS   packet	 	Pointer to data packet
**              fromDS   	From distribution system field
**              toDS     	To distribution system field
**              src_addr	Source address
**              dst_addr    Destination address
**
** DESCRIPTION  The function analyzes from distribution system
**              and to distribution system fields and selects which 
**              source address and destination address will be used.
******************************************************************************/
static __MTLK_IFUNC __INLINE void
mtlk_wlan_get_mac_addrs(uint8 *packet, uint8 fromDS, uint8 toDS, uint8 *src_addr, uint8 *dst_addr)
{
  uint8 tmp_src_addr[6], tmp_dst_addr[6];
  if (fromDS == 1 && toDS == 0) {
    mtlk_osal_copy_eth_addresses(tmp_dst_addr, WLAN_GET_ADDR1(packet));
    mtlk_osal_copy_eth_addresses(tmp_src_addr, WLAN_GET_ADDR3(packet));
  } else if (fromDS == 0 && toDS == 1) {
    mtlk_osal_copy_eth_addresses(tmp_dst_addr, WLAN_GET_ADDR3(packet));
    mtlk_osal_copy_eth_addresses(tmp_src_addr, WLAN_GET_ADDR2(packet));
  } else if (fromDS == 1 && toDS == 1) {
    mtlk_osal_copy_eth_addresses(tmp_dst_addr, WLAN_GET_ADDR3(packet));
    mtlk_osal_copy_eth_addresses(tmp_src_addr, WLAN_GET_ADDR4(packet));
  } else { /* (fromDS == 0 && toDS == 0) */
    mtlk_osal_copy_eth_addresses(tmp_dst_addr, WLAN_GET_ADDR1(packet));
    mtlk_osal_copy_eth_addresses(tmp_src_addr, WLAN_GET_ADDR2(packet));
  }
  mtlk_osal_copy_eth_addresses(dst_addr, tmp_dst_addr);
  mtlk_osal_copy_eth_addresses(src_addr, tmp_src_addr);
  ILOG4_Y("802.11n rx DA: %Y", dst_addr);
  ILOG4_Y("802.11n rx SA: %Y", src_addr);
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif // _MTLK_PACKETS_H_
