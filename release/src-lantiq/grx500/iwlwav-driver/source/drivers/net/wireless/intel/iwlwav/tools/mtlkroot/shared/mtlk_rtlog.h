/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_RTLOG_H__
#define __MTLK_RTLOG_H__

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define IWLWAV_RTLOG_HW_MAX_WLAN_IF    8 /* max numbers of HW wlan interfaces */
#define IWLWAV_RTLOG_FW_MAX_STREAM     2 /* per WLAN IF */

/* LOGGER_NUM_OF_HW_FIFOS */
#define IWLWAV_RTLOG_HW_FIFOS_G6      16 /* Gen6 */
#define IWLWAV_RTLOG_HW_FIFOS_MIN     IWLWAV_RTLOG_HW_FIFOS_G6
#define IWLWAV_RTLOG_HW_FIFOS         IWLWAV_RTLOG_HW_FIFOS_G6

#define IWLWAV_RTLOG_FW_MAX_FILTER     2 /* per WLAN IF */
#define IWLWAV_RTLOG_FW_FLT_TYPE_LOG_LEVEL     0
#define IWLWAV_RTLOG_FW_FLT_TYPE_MOD_BITMAP    1


#define IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS  11 /* packet header length */

#define MTLK_PACK_ON
#include "mtlkpack.h"


typedef enum
{
    IWLWAV_RTLOG_CFG_REQ_ADD_STREAM,
    IWLWAV_RTLOG_CFG_REQ_REM_STREAM,
    IWLWAV_RTLOG_CFG_REQ_ADD_FILTER,
    IWLWAV_RTLOG_CFG_REQ_REM_FILTER,

    IWLWAV_RTLOG_CFG_REQ_MAX
} rtlog_cfg_id_t;


typedef struct rtlog_pck_hdr {
    union {
        struct {
            struct ethhdr   eth;
            struct iphdr    iph;
            struct udphdr   udph;
            uint8           pad2[2];    /* padding after UDP header */
        } __MTLK_PACKED;

        uint32  words[IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS];
    };
    uint32 data[0];
} rtlog_pck_hdr_t;

#define MTLK_PACK_OFF
#include "mtlkpack.h"

/* Network-related defines */
typedef uint32      ip4_addr_t; /* uint32 OR __be32 */
// typedef IEEE_ADDR   mac_addr_t;
typedef struct { uint8 au8Addr[6]; }    mac_addr_t;

/* network config */
typedef struct rtlog_net_cfg {
    mac_addr_t      dst_mac;
    mac_addr_t      src_mac;
    ip4_addr_t      src_ip4;
    ip4_addr_t      dst_ip4;
    uint16          src_port; /* UDP port */
    uint16          dst_port; /* UDP port */
    uint8           tos;
    uint8           ttl;
} rtlog_net_cfg_t;

/* WLAN Driver config */
typedef struct rtlog_drv_cfg {
    BOOL            is_active;
    rtlog_pck_hdr_t pck_hdr;
    //rtlog_net_cfg_t net_cfg;
    uint32          ip_ident;   /* used as ident of IP header */
    /* ... */
} rtlog_drv_cfg_t;


/* WLAN FW config
 * Note: All structures should contain the rtlog_cfg_id_t as first field
 */

typedef struct rtlog_fw_stream_cfg {
    rtlog_cfg_id_t  cfg_id;
    rtlog_pck_hdr_t pck_hdr;
    uint32          chk_sum;
    //rtlog_net_cfg_t net_cfg;
    uint32          fifos;
    uint16          buf_thr;
    uint8           str_id;
    uint8           wlan_if;
    uint8           mode;    /* From LogAgentStateActive_e */

    BOOL            is_active;
} rtlog_fw_stream_cfg_t;

typedef struct rtlog_fw_stream_rem {
    rtlog_cfg_id_t  cfg_id;
    uint16          str_id;
} rtlog_fw_stream_rem_t;

typedef struct rtlog_fw_filter_cfg {
    rtlog_cfg_id_t  cfg_id;
    BOOL            is_active;
    uint8           flt_type;
    uint8           log_lvl;
    uint32          bitmap;
} rtlog_fw_filter_cfg_t;

typedef struct rtlog_fw_filter_rem {
    rtlog_cfg_id_t  cfg_id;
    uint8           flt_type;
} rtlog_fw_filter_rem_t;

typedef struct rtlog_fw_trigger_cfg {
    uint32          words[4];
} rtlog_fw_trigger_cfg_t;

typedef struct rtlog_hw_fifo_cfg {
    uint16          config[IWLWAV_RTLOG_HW_FIFOS];
} rtlog_hw_fifo_cfg_t;

typedef struct rtlog_fw_cfg {
    BOOL                    is_active;
    uint8                   wlan_if;
    uint8                   max_nof_streams;
    rtlog_hw_fifo_cfg_t     hw_fifo_cfg;
    rtlog_fw_stream_cfg_t   stream_cfg[IWLWAV_RTLOG_FW_MAX_STREAM];
    rtlog_fw_filter_cfg_t   filter_cfg[IWLWAV_RTLOG_FW_MAX_FILTER];
    rtlog_fw_trigger_cfg_t  start_trigger;
    rtlog_fw_trigger_cfg_t  stop_trigger;
} rtlog_fw_cfg_t;

typedef void (*rtlog_clb_t)(mtlk_handle_t clb_ctx, rtlog_cfg_id_t req_id, void *data, uint32 size);

typedef struct rtlog_clb_entry {
    rtlog_clb_t     clb;
    //mtlk_handle_t   clb;
    mtlk_handle_t   ctx;
} rtlog_clb_entry_t;


/* Common RTLOG config */
typedef struct rtlog_cfg {
    rtlog_drv_cfg_t         drv_cfg;
    rtlog_fw_cfg_t          fw_cfg[IWLWAV_RTLOG_HW_MAX_WLAN_IF];
    rtlog_clb_entry_t       fw_clb[IWLWAV_RTLOG_HW_MAX_WLAN_IF];

    mtlk_osal_spinlock_t    data_lock; /* data protection */
} rtlog_cfg_t;


/* ========================== API ========================== */

uint32 __MTLK_IFUNC
mtlk_rtlog_get_wlanif_from_string(char *str);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
/* Get RT-Logger packet header (copy to word buffer) */
int __MTLK_IFUNC
mtlk_rtlog_get_packet_header(uint32 *wbuf);

/* Get RT-Logger FW config by wlan if number */
int __MTLK_IFUNC
mtlk_rtlog_fw_get_config(uint32 wlan_if, rtlog_fw_cfg_t *cfg);

/* Get RT-Logger HW FIFO config by wlan if number */
int __MTLK_IFUNC
mtlk_rtlog_fw_get_hw_fifo(uint32 wlan_if, rtlog_hw_fifo_cfg_t *cfg);

/* Forward ready LOG packet */
void __MTLK_IFUNC
mtlk_rtlog_forward_log_packet(struct sk_buff *skb);

/* Register RTLOG callback */
int  __MTLK_IFUNC
rtlog_fw_clb_register(int wlan_if, rtlog_clb_t clb, mtlk_handle_t ctx);

/* Unregister RTLOG callback */
void __MTLK_IFUNC
rtlog_fw_clb_unregister(int wlan_if);

/* Sync TSF */
void __MTLK_IFUNC
mtlk_log_tsf_sync_msg(int hw_idx, uint32 drv_ts, int32 fw_offset);

/* Check if FW offline logging enabled */
BOOL __MTLK_IFUNC
rtlog_fw_offline_logging_enabled(const char *ifname);

void __MTLK_IFUNC
mtlk_ndev_send_log_packet(void *data, size_t data_len);
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

#endif /* __MTLK_RTLOG_H__ */
