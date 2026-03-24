/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "mtlkbfield.h"

#include <linux/module.h>
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog.h"
#endif

#define LOG_LOCAL_GID   GID_MTLKNDEV
#define LOG_LOCAL_FID   1

#define MTLK_PACK_ON
#include "mtlkpack.h"

#ifdef CONFIG_WAVE_RTLOG_REMOTE

/* Wireless Frame Info according to RFC-5416.
 */
typedef struct _mtlk_ndev_capwap_wifi_info {
  int8    rssi; /* RSSI (signed value), in dBm */
  uint8   snr;  /* Signal-to-noise ratio, in dB */
  uint16  rate; /* Data Rate, in units of 0.1 Mbps */
} __MTLK_PACKED mtlk_ndev_capwap_wifi_info_t;

/* CAPWAP defines according to RFC-5415.
*/
/*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 31 BE
   31 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  LE
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |CAPWAP Preamble|  HLEN   |   RID   | WBID    |T|F|L|W|M|K|Flags|  word0
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Fragment ID          |     Frag Offset         |Rsvd |  word1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                 (optional) Radio MAC Address                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            (optional) Wireless Specific Information           |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
typedef struct _mtlk_ndev_capwap_hdr {
  union {
    uint32 word0;
    struct {
      uint8  preamble;
      uint16 begin;
      uint8  second;
    } __MTLK_PACKED;
  };
  union {
    uint32 word1;
    struct {
      uint16 frag_id;
      uint16 frag_off;
    } __MTLK_PACKED;
  };

  /* Radio MAC address is not used */

  /* Wireless Specific Information */
  uint8  wireless_len;
  union {
    uint32 wireless_data;
    mtlk_ndev_capwap_wifi_info_t wifi_info;
  };

  uint8  padding[3]; /* 4-byte alignment */
} __MTLK_PACKED mtlk_ndev_capwap_hdr_t;

#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#define CAPWAP_UDP_PORT             (5247)

#define CAPWAP_WORD0_INFO_FLAGS_RES MTLK_BFIELD_INFO( 0,  3) /*  3 bits Flags reserved bits          */
#define CAPWAP_WORD0_INFO_FLAG_K    MTLK_BFIELD_INFO( 3,  1) /*  1 bit  Keep-alive packet            */
#define CAPWAP_WORD0_INFO_FLAG_M    MTLK_BFIELD_INFO( 4,  1) /*  1 bit  Radio MAC address is present */
#define CAPWAP_WORD0_INFO_FLAG_W    MTLK_BFIELD_INFO( 5,  1) /*  1 bit  WirelessSpecInfo  is present */
#define CAPWAP_WORD0_INFO_FLAG_L    MTLK_BFIELD_INFO( 6,  1) /*  1 bit  Last fragment of fragmented  */
#define CAPWAP_WORD0_INFO_FLAG_F    MTLK_BFIELD_INFO( 7,  1) /*  1 bit  Fragmented                   */
#define CAPWAP_WORD0_INFO_FLAG_T    MTLK_BFIELD_INFO( 8,  1) /*  1 bit  Type: 0 - 802.3, 1 - by WBID */
#define CAPWAP_WORD0_INFO_WBID      MTLK_BFIELD_INFO( 9,  5) /*  5 bits Wireless binding identifier  */
#define CAPWAP_WORD0_INFO_RID       MTLK_BFIELD_INFO(14,  5) /*  5 bits Radio ID number              */
#define CAPWAP_WORD0_INFO_HLEN      MTLK_BFIELD_INFO(19,  5) /*  5 bits HeaderLength in 4-byte words */
#define CAPWAP_WORD0_INFO_PREAMBLE  MTLK_BFIELD_INFO(24,  8) /*  8 bits Preamble                     */

#define CAPWAP_WORD0_WBID_IEEE80211 (1)
#define CAPWAP_WORD0_WBID_EPCGLOBAL (3)

#define CAPWAP_WORD0_TYPE_ETHER     (0)
#define CAPWAP_WORD0_TYPE_WBID      (1)

#define CAPWAP_WORD1_INFO_RESERVED  MTLK_BFIELD_INFO( 0,  3)  /*  3 bits                              */
#define CAPWAP_WORD1_INFO_FRAG_OFFS MTLK_BFIELD_INFO( 3, 13)  /* 13 bits Fragment offset              */
#define CAPWAP_WORD1_INFO_FRAG_ID   MTLK_BFIELD_INFO(16,  8)  /* 16 bits Fragment ID                  */

/* === Implementation === */

extern struct net_device        *rtlog_netdev;
extern struct net_device_stats  *rtlog_stats;

static unsigned short ip_ident  = 0x1300;
static unsigned       pck_cntr  = 0;

void dump_skb(struct sk_buff *skb);


#ifdef MTLK_USE_DIRECTPATH_API
extern  BOOL    rtlog_ppa_send_packet(struct sk_buff *skb); /* FIXME: prototype */
#endif  /* MTLK_USE_DIRECTPATH_API */

static void
send_ready_udp_packet(struct sk_buff *skb)
{
    int res;

#ifdef MTLK_USE_DIRECTPATH_API
    /* try to send via PPA */
    if (TRUE == rtlog_ppa_send_packet(skb))
        return;
#endif  /* MTLK_USE_DIRECTPATH_API */

    /* send without PPA */
    skb->protocol = eth_type_trans(skb, skb->dev);
    res = netif_rx(skb);
    if (0 != res) {
        mtlk_osal_emergency_print("%s: netif_rx ret code %d", __FUNCTION__, res);
    }
}

void __MTLK_IFUNC
mtlk_rtlog_set_iph_check(struct iphdr *iph)
{
    uint16  csum_old, csum_new;

    csum_old    = iph->check;
    iph->check  = 0;
    csum_new    = ip_fast_csum((unsigned char *)iph, iph->ihl);
    iph->check  = csum_new;
#ifdef CONFIG_WAVE_DEBUG
    if (csum_old != csum_new) {
        mtlk_osal_emergency_print("%s: Change iph->check from %04X to %04X", __FUNCTION__, (int)csum_old, (int)csum_new);
    }
#endif
}

static int send_udp_packet(struct net_device *dev, void *buff, size_t data_len)
{
    struct sk_buff  *skb;
    struct ethhdr   *eth;
    struct iphdr    *iph;
    struct udphdr   *udph;
    unsigned char   *cp;
    unsigned short   ip_len;
    unsigned short   udp_len;
    unsigned long    pck_len;
    int res;

    ++pck_cntr;
    if (0 == data_len)
        return MTLK_ERR_OK; /* FIXME: ? */

    udp_len = data_len + sizeof(*udph);
    udp_len += 2; /* include padding */

    ip_len  = udp_len  + sizeof(*iph);
    pck_len = ip_len   + sizeof(*eth);

    skb = alloc_skb(pck_len, GFP_ATOMIC);
    if (!skb)
        return MTLK_ERR_NO_MEM;

    skb->dev = dev;

    { /* get packet header */
        int hdr_len = sizeof(rtlog_pck_hdr_t);
        rtlog_pck_hdr_t    *skb_pck_hdr = (rtlog_pck_hdr_t *)skb->data;

        if (MTLK_ERR_OK != (res = mtlk_rtlog_get_packet_header(skb_pck_hdr->words)))
        {
            dev_kfree_skb(skb);
            goto FAIL;
        }

        skb_reserve(skb, hdr_len);

        eth  = &skb_pck_hdr->eth;
        iph  = &skb_pck_hdr->iph;
        udph = &skb_pck_hdr->udph;
    }

    /* copy data */
    cp = skb_put(skb, data_len);
    wave_memcpy(skb->data, data_len, buff, data_len);

    skb_push(skb, sizeof(((rtlog_pck_hdr_t *) NULL)->pad2)); /* padding after UDP header */

    skb_push(skb, sizeof(*udph));
    //skb_reset_transport_header(skb);
    udph->len    = htons(udp_len);
    udph->check  = 0;
    udph->check  = csum_tcpudp_magic(iph->saddr, // source_ip,
                                     iph->daddr, //remote_ip,
                                     udp_len, IPPROTO_UDP,
                                     csum_partial(udph, udp_len, 0));

    if (udph->check == 0)
            udph->check = CSUM_MANGLED_0;

    skb_push(skb, sizeof(*iph));
    //skb_reset_network_header(skb);

    iph->tot_len  = htons(ip_len);

                    ip_ident++;
                    //atomic_inc_return(&ip_ident);
    iph->id       = htons(ip_ident);

    iph->check    = 0;
    iph->check    = ip_fast_csum((unsigned char *)iph, iph->ihl);

    skb_push(skb, ETH_HLEN);
    skb_reset_mac_header(skb);

    skb->protocol = eth->h_proto;

    send_ready_udp_packet(skb);

    return MTLK_ERR_OK;

FAIL:
    return res;
}

static int
_mtlk_ndev_send_capwap_packet (struct net_device *dev, void *buff, size_t data_len, void *hdr, size_t hdr_len, uint8 is_wifi)
{
    struct sk_buff  *skb;
    struct ethhdr   *eth;
    struct iphdr    *iph;
    struct udphdr   *udph;
    unsigned char   *cp;
    rtlog_pck_hdr_t *skb_pck_hdr;
    unsigned short   ip_len;
    unsigned short   udp_len;
    unsigned long    pck_len;
    uint8 tmp;
    int res;

    ++pck_cntr;
    if (0 == data_len)
        return MTLK_ERR_OK; /* FIXME: ret_code ? */

    udp_len = sizeof(*udph) + data_len + hdr_len;
    ip_len  = sizeof(*iph)  + udp_len;
    pck_len = sizeof(*eth)  + ip_len;

    skb = alloc_skb(pck_len, GFP_ATOMIC);
    if (!skb)
        return MTLK_ERR_NO_MEM;

    skb->dev = dev;

    cp = skb_put(skb, sizeof(*eth) + sizeof(*iph) + sizeof(*udph));
    skb_pck_hdr = (rtlog_pck_hdr_t *)cp;
    if (MTLK_ERR_OK != (res = mtlk_rtlog_get_packet_header(skb_pck_hdr->words))) {
      dev_kfree_skb(skb);
      goto FAIL;
    }

    eth  = &skb_pck_hdr->eth;
    iph  = &skb_pck_hdr->iph;
    udph = &skb_pck_hdr->udph;

    /* copy CAPWAP header */
    cp = skb_put(skb, hdr_len);
    wave_memcpy(cp, hdr_len, hdr, hdr_len);

    /* copy 802.11 frame */
    cp = skb_put(skb, data_len);
    wave_memcpy(cp, data_len, buff, data_len);

    if (is_wifi){
      tmp = cp[0];
      cp[0] = cp[1];
      cp[1] = tmp;
    }

    udph->len    = htons(udp_len);
    udph->dest   = htons(CAPWAP_UDP_PORT);
    udph->check  = CSUM_MANGLED_0; /* FIXME: according to RFC, MUST be set to zero */

    iph->tot_len = htons(ip_len);
    iph->id      = htons(ip_ident);
    iph->check   = 0; /* Validation disabled */

    skb_reset_mac_header(skb);
    skb->protocol = eth->h_proto;

    send_ready_udp_packet(skb);

    return MTLK_ERR_OK;

FAIL:
    return res;
}

/* ============================================== */

#define HEX_DUMP_LINE_LEN (6 + 3 * 8 + 1 + 3 * 8 + 1 /* null */) /* = 56 bytes */


static __INLINE BOOL _mtlk_check_snprintf(int res, size_t maxsize)
{
  return (res < 0 || (size_t)res >= maxsize) ? FALSE : TRUE;
}

static void
_print_hex_ex (const void *buf, unsigned int l, const char *str)
{
  unsigned int i, j;
  unsigned char lbuf[HEX_DUMP_LINE_LEN];
  int res, lbuf_pos = 0;

  if (l > 208)
      l = 208;

  res = mtlk_snprintf(lbuf, sizeof(lbuf), "%s: cp= 0x%p l=%d", str, buf, l);
  if (!_mtlk_check_snprintf(res, sizeof(lbuf)))
    return;
  mtlk_osal_emergency_print("%s", lbuf);

  for (i = 0; i < l;) {
    res = mtlk_snprintf(lbuf, sizeof(lbuf), "%04x :", i);
    if (!_mtlk_check_snprintf(res, sizeof(lbuf)))
      return;
    lbuf_pos = res;
    for (j = 0; (j < 16) && (i < l); j++)
    {
      if (j == 8)
        res = mtlk_snprintf(&lbuf[lbuf_pos], sizeof(lbuf)-lbuf_pos, "  %02x" , ((const unsigned char*)buf)[i]);
      else
        res = mtlk_snprintf(&lbuf[lbuf_pos], sizeof(lbuf)-lbuf_pos,  " %02x" , ((const unsigned char*)buf)[i]);
      if (!_mtlk_check_snprintf(res, sizeof(lbuf)-lbuf_pos))
        return;
      lbuf_pos += res;
      i++;
    }
    mtlk_osal_emergency_print("%s", lbuf);
  }
}

void
dump_skb(struct sk_buff *skb)
{
#ifdef NET_SKBUFF_DATA_USES_OFFSET
    mtlk_osal_emergency_print("skb %px: head %px, data %px (len 0x%02X), tail %u, end %u",
                        skb, skb->head, skb->data, skb->len, skb->tail, skb->end);
#else
    mtlk_osal_emergency_print("skb %px: head %px, data %px (len 0x%02X), tail %px, end %px",
                        skb, skb->head, skb->data, skb->len, skb->tail, skb->end);
#endif

    /* print out 16 bytes per line */
    _print_hex_ex (skb->head, skb->data - skb->head, "skb_head");
    _print_hex_ex (skb->data, skb->len, "skb_data");

}

/* ============================================== */
/* API */

#undef IWLWAV_RTLOG_DEBUG_FWLOGS_CHECK    /* DEBUG: perform several checks of FW LOG messages */

#ifdef  IWLWAV_RTLOG_DEBUG_FWLOGS_CHECK
extern  rtlog_cfg_t     g_rtlog_cfg;

static  uint32  rtlog_debug_fwlogs_drop = 0;
#endif

void __MTLK_IFUNC
mtlk_ndev_send_log_packet(void *data, size_t data_len)
{
    if (rtlog_netdev &&
        (MTLK_ERR_OK == send_udp_packet(rtlog_netdev, data, data_len))) {
            ++rtlog_stats->rx_packets;
    } else {
            ++rtlog_stats->rx_dropped;
    }
}
EXPORT_SYMBOL(mtlk_ndev_send_log_packet);

void __MTLK_IFUNC
mtlk_ndev_send_capwap_packet (uint8 hwid, void *data, size_t data_len, uint8 is_wifi, int rssi)
{
    mtlk_ndev_capwap_hdr_t  capwap_hdr;
    uint32  word;

    MTLK_STATIC_ASSERT(0 == (sizeof(capwap_hdr) % sizeof(uint32))); /* word aligned */
    MTLK_ASSERT(NULL != data);

    memset(&capwap_hdr, 0, sizeof(capwap_hdr));

    word = MTLK_BFIELD_VALUE(CAPWAP_WORD0_INFO_HLEN,
                             (sizeof(capwap_hdr) / sizeof(uint32)), uint32) | /* num of words */
           MTLK_BFIELD_VALUE(CAPWAP_WORD0_INFO_RID,    hwid, uint32) | /* RadioID */
           MTLK_BFIELD_VALUE(CAPWAP_WORD0_INFO_FLAG_W, 1,    uint32) | /* Wireless Specific Info */
           MTLK_BFIELD_VALUE(CAPWAP_WORD0_INFO_WBID,   CAPWAP_WORD0_WBID_IEEE80211, uint32) |
           MTLK_BFIELD_VALUE(CAPWAP_WORD0_INFO_FLAG_T, is_wifi ?
                             CAPWAP_WORD0_TYPE_WBID : CAPWAP_WORD0_TYPE_ETHER, uint32); /* Type */

    capwap_hdr.word0 = htonl(word);

    capwap_hdr.wireless_len = sizeof(capwap_hdr.wireless_data); /* num of bytes */
    capwap_hdr.wifi_info.rssi = rssi;

    if (rtlog_netdev &&
      (MTLK_ERR_OK == _mtlk_ndev_send_capwap_packet(rtlog_netdev, data, data_len, &capwap_hdr, sizeof(capwap_hdr), is_wifi))) {
          ++rtlog_stats->rx_packets;
    } else {
          ++rtlog_stats->rx_dropped;
    }
}
EXPORT_SYMBOL(mtlk_ndev_send_capwap_packet);

void __MTLK_IFUNC
mtlk_rtlog_forward_log_packet(struct sk_buff *skb)
{
    rtlog_pck_hdr_t    *skb_pck_hdr = (rtlog_pck_hdr_t *)skb->data;

    MTLK_ASSERT(skb);

    if (rtlog_netdev) {
        skb_reset_mac_header(skb);
        skb->protocol = skb_pck_hdr->eth.h_proto;

        skb->dev = rtlog_netdev;

#ifdef  IWLWAV_RTLOG_DEBUG_FWLOGS_CHECK

        /* Currently we have only wlan0/stream0 */
        {
            rtlog_pck_hdr_t    *cfg_pck_hdr = &g_rtlog_cfg.fw_cfg[0].stream_cfg[0].pck_hdr;
            uint32              udp_len = ntohs(skb_pck_hdr->udph.len);
            uint32              udp_id  = ntohs(skb_pck_hdr->iph.id);

            ILOG1_DDD("LOG pck: IP id 0x%04x, udp_len 0x%04x (%d)", udp_id, udp_len, udp_len);

            if (1512 < udp_len /* HW limited length */ ||
                0 != memcmp(cfg_pck_hdr, skb_pck_hdr, 16)   /* compare header with stream config data */
                ) {

              ++rtlog_debug_fwlogs_drop;

              if (16 > rtlog_debug_fwlogs_drop) {
                _print_hex_ex (skb, sizeof(*skb), "BAD sk_buff, drop");
                dump_skb(skb);
              }

              dev_kfree_skb(skb);

            } else {
                mtlk_rtlog_set_iph_check(&skb_pck_hdr->iph);
                send_ready_udp_packet(skb);
            }
        }
    
#else /* IWLWAV_RTLOG_DEBUG_FWLOGS_CHECK */

        send_ready_udp_packet(skb);

#endif /* IWLWAV_RTLOG_DEBUG_FWLOGS_CHECK */
    }
}
EXPORT_SYMBOL(mtlk_rtlog_forward_log_packet);
#endif /* CONFIG_WAVE_RTLOG_REMOTE */
