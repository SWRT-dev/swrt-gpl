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
 * Core module definitions
 *
 */
#ifndef __CORE_H__
#define __CORE_H__

/**
*\file core.h
*\brief Core module acting as mediator to organize all driver activities
*/

#include "mtlk_vap_manager.h"
#include "mtlkhal.h"

struct nic;

#include "mtlkhal.h"
#include "mcast.h"
#include "mtlkqos.h"


#include "stadb.h"
#include "wds.h"

#include "mtlk_serializer.h"

#include "mtlk_psdb.h"
#include "mtlk_core_iface.h"

#include "mtlk_coc.h"
#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
#include "mtlk_pcoc.h"
#endif /*CPTCFG_IWLWAV_PMCU_SUPPORT*/
#include "frame.h"
#include "scan_support.h"
#include "channels.h"
#include "core_priv.h"

#include "mtlk_wss.h"
#include "beacon_manager.h"
#include "wave_radio.h"

// FIXME the state names and historic comments about them seem to be complete nonsense;
// and they are VAP-specific, not driver specific
/* driver is halted - in case if was not initialized yet
   or after critical error */
#define NET_STATE_HALTED         (1 << 0) // FIXME "not initialized yet" would more naturally be a 0
/* driver is initializing */
#define NET_STATE_IDLE           (1 << 1)
/* driver has been initialized */
#define NET_STATE_READY          (1 << 2)
/* activation request was sent - waiting for CFM */
#define NET_STATE_ACTIVATING     (1 << 3) // This will be the "VAP added but BSS not set" state
#define NET_STATE_CONNECTED      (1 << 4)
/* disconnect started */
#define NET_STATE_DEACTIVATING   (1 << 5) // This will be the "VAP traffic stopped but VAP not removed"
// and it might be that DEACTIVATING is in no way different from ACTIVATING from the FW point of view

const char *mtlk_net_state_to_string(uint32 state);

#define INVALID_DEACTIVATE_TIMESTAMP ((uint32)-1)

#define MTLK_VALUE_TO_PERCENT(value, max_value)   (((uint32)value * 100) / (max_value))
#define MTLK_PERCENT_TO_VALUE(percent, max_value) (((uint32)percent * max_value) / (100))

// amount of time - needed by firmware to send vap removal
// indication to the driver.
#define VAP_REMOVAL_TIMEOUT 10000 /* msec */
enum ts_priorities {
  TS_PRIORITY_BE,
  TS_PRIORITY_BG,
  TS_PRIORITY_VIDEO,
  TS_PRIORITY_VOICE,
  TS_PRIORITY_LAST
};

#define STA_EXCE_RETRY_THRESHOLD 50

/***************************************************/

#define LOG_LOCAL_GID   GID_CORE
#define LOG_LOCAL_FID   0


/* FW statistic to be collected by HW */
#define MTLK_CORE_CNT_FW_STAT_CALC_NUM  (MTLK_CORE_CNT_PACKETS_RECEIVED_64 - MTLK_CORE_CNT_BYTES_SENT_64 + 1)
#define MTLK_CORE_CNT_POLL_STAT_FIRST   (MTLK_CORE_CNT_UNICAST_PACKETS_SENT)
#define MTLK_CORE_CNT_POLL_STAT_LAST    (MTLK_CORE_CNT_BROADCAST_BYTES_RECEIVED)
#define MTLK_CORE_CNT_POLL_STAT_NUM     (MTLK_CORE_CNT_POLL_STAT_LAST - MTLK_CORE_CNT_POLL_STAT_FIRST + 1)

/* acessors for configuration data passed from DF to Core */
#define MTLK_CFG_START_CHEK_ITEM_AND_CALL() do{
#define MTLK_CFG_END_CHEK_ITEM_AND_CALL() }while(0);

#define MTLK_CFG_CHECK_ITEM_VOID(obj,name) \
  if(1!=(obj)->name##_filled) {break;}

#define MTLK_CFG_CHECK_ITEM_AND_CALL(obj,name,func,func_args,func_res) \
  if(1==(obj)->name##_filled){func_res=func func_args;if(MTLK_ERR_OK != func_res)break;}

#define MTLK_CFG_CHECK_ITEM_AND_CALL_VOID MTLK_CFG_GET_ITEM_BY_FUNC_VOID

#define MTLK_CFG_CHECK_ITEM_AND_CALL_EX(obj,name,func,func_args,func_res,etalon_res) \
  if(1==(obj)->name##_filled) {\
    func_res=func func_args;\
    if(etalon_res != func_res) {\
      func_res = MTLK_ERR_UNKNOWN;\
      break;\
    }else {\
      func_res=MTLK_ERR_OK;\
    }\
  }

typedef enum
{
  MTLK_CORE_CNT_BYTES_SENT_64,
  MTLK_CORE_CNT_BYTES_RECEIVED_64,
  MTLK_CORE_CNT_PACKETS_SENT_64,
  MTLK_CORE_CNT_PACKETS_RECEIVED_64,

  /* FW statistic to be collected by HW */
  /* Should be the same order as in mtlk_mhi_stat_vap_e */
  MTLK_CORE_CNT_UNICAST_PACKETS_SENT,
  MTLK_CORE_CNT_MULTICAST_PACKETS_SENT,
  MTLK_CORE_CNT_BROADCAST_PACKETS_SENT,
  MTLK_CORE_CNT_UNICAST_BYTES_SENT,
  MTLK_CORE_CNT_MULTICAST_BYTES_SENT,
  MTLK_CORE_CNT_BROADCAST_BYTES_SENT,

  MTLK_CORE_CNT_UNICAST_PACKETS_RECEIVED,
  MTLK_CORE_CNT_MULTICAST_PACKETS_RECEIVED,
  MTLK_CORE_CNT_BROADCAST_PACKETS_RECEIVED,
  MTLK_CORE_CNT_UNICAST_BYTES_RECEIVED,
  MTLK_CORE_CNT_MULTICAST_BYTES_RECEIVED,
  MTLK_CORE_CNT_BROADCAST_BYTES_RECEIVED,

  MTLK_CORE_CNT_ERROR_PACKETS_SENT,
  MTLK_CORE_CNT_ERROR_PACKETS_RECEIVED,

  /* FW statistic collected by STA */
  MTLK_CORE_CNT_DISCARD_PACKETS_RECEIVED,

  /* others */
  MTLK_CORE_CNT_TX_PROBE_RESP_SENT,
  MTLK_CORE_CNT_TX_PROBE_RESP_DROPPED,
  MTLK_CORE_CNT_BSS_MGMT_TX_QUE_FULL,

  /* Statistics required for waiting confirmations
   * from FW during core deactivate */
  MTLK_CORE_CNT_MAN_FRAMES_RES_QUEUE,
  MTLK_CORE_CNT_MAN_FRAMES_SENT,
  MTLK_CORE_CNT_MAN_FRAMES_CONFIRMED,

/* minimal statistic */
#if (!(MTLK_MTIDL_WLAN_STAT_FULL))
  MTLK_CORE_CNT_LAST,
#endif /* MTLK_MTIDL_WLAN_STAT_FULL */

  MTLK_CORE_CNT_RX_PACKETS_DISCARDED_DRV_DUPLICATE,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_NO_PEERS,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DRV_UNKNOWN_DESTINATION_DIRECTED,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DRV_UNKNOWN_DESTINATION_MCAST,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DRV_NO_RESOURCES,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_SQ_OVERFLOW,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DROP_ALL_FILTER,
  MTLK_CORE_CNT_TX_PACKETS_DISCARDED_TX_QUEUE_OVERFLOW,
  MTLK_CORE_CNT_802_1X_PACKETS_RECEIVED,
  MTLK_CORE_CNT_802_1X_PACKETS_SENT,
  MTLK_CORE_CNT_802_1X_PACKETS_DISCARDED,
  MTLK_CORE_CNT_PAIRWISE_MIC_FAILURE_PACKETS,
  MTLK_CORE_CNT_GROUP_MIC_FAILURE_PACKETS,
  MTLK_CORE_CNT_UNICAST_REPLAYED_PACKETS,
  MTLK_CORE_CNT_MULTICAST_REPLAYED_PACKETS,
  MTLK_CORE_CNT_MANAGEMENT_REPLAYED_PACKETS,
  MTLK_CORE_CNT_FWD_RX_PACKETS,
  MTLK_CORE_CNT_FWD_RX_BYTES,
  MTLK_CORE_CNT_DAT_FRAMES_RECEIVED,
  MTLK_CORE_CNT_CTL_FRAMES_RECEIVED,
  MTLK_CORE_CNT_MAN_FRAMES_RECEIVED,
  MTLK_CORE_CNT_RX_MAN_FRAMES_RETRY_DROPPED,
  MTLK_CORE_CNT_MAN_FRAMES_FAILED,
  MTLK_CORE_CNT_CHANNEL_SWITCH_20_TO_40,
  MTLK_CORE_CNT_CHANNEL_SWITCH_40_TO_20,
  MTLK_CORE_CNT_TX_PACKETS_TO_UNICAST_DGAF_DISABLED,
  MTLK_CORE_CNT_TX_PACKETS_SKIPPED_DGAF_DISABLED,

/* DEBUG statistic */
#if MTLK_MTIDL_WLAN_STAT_FULL
  MTLK_CORE_CNT_LAST
#endif /* MTLK_MTIDL_WLAN_STAT_FULL */

} mtlk_core_wss_cnt_id_e;

#if 1 /* to cleanup */
#define MTLK_CORE_CNT_PACKETS_SENT_64_ALLOWED                                       MTLK_WWSS_WLAN_STAT_ID_PACKETS_SENT_64_ALLOWED
#define MTLK_CORE_CNT_PACKETS_RECEIVED_64_ALLOWED                                   MTLK_WWSS_WLAN_STAT_ID_PACKETS_RECEIVED_64_ALLOWED
#define MTLK_CORE_CNT_BYTES_RECEIVED_64_ALLOWED                                     MTLK_WWSS_WLAN_STAT_ID_BYTES_RECEIVED_64_ALLOWED
#define MTLK_CORE_CNT_BYTES_SENT_64_ALLOWED                                         MTLK_WWSS_WLAN_STAT_ID_BYTES_SENT_64_ALLOWED
#define MTLK_CORE_CNT_MULTICAST_BYTES_SENT_ALLOWED                                  MTLK_WWSS_WLAN_STAT_ID_MULTICAST_BYTES_SENT_ALLOWED
#define MTLK_CORE_CNT_MULTICAST_BYTES_RECEIVED_ALLOWED                              MTLK_WWSS_WLAN_STAT_ID_MULTICAST_BYTES_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_BROADCAST_BYTES_SENT_ALLOWED                                  MTLK_WWSS_WLAN_STAT_ID_BROADCAST_BYTES_SENT_ALLOWED
#define MTLK_CORE_CNT_BROADCAST_BYTES_RECEIVED_ALLOWED                              MTLK_WWSS_WLAN_STAT_ID_BROADCAST_BYTES_RECEIVED_ALLOWED

#define MTLK_CORE_CNT_UNICAST_PACKETS_SENT_ALLOWED                                  MTLK_WWSS_WLAN_STAT_ID_UNICAST_PACKETS_SENT_ALLOWED
#define MTLK_CORE_CNT_UNICAST_PACKETS_RECEIVED_ALLOWED                              MTLK_WWSS_WLAN_STAT_ID_UNICAST_PACKETS_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_MULTICAST_PACKETS_SENT_ALLOWED                                MTLK_WWSS_WLAN_STAT_ID_MULTICAST_PACKETS_SENT_ALLOWED
#define MTLK_CORE_CNT_MULTICAST_PACKETS_RECEIVED_ALLOWED                            MTLK_WWSS_WLAN_STAT_ID_MULTICAST_PACKETS_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_BROADCAST_PACKETS_SENT_ALLOWED                                MTLK_WWSS_WLAN_STAT_ID_BROADCAST_PACKETS_SENT_ALLOWED
#define MTLK_CORE_CNT_BROADCAST_PACKETS_RECEIVED_ALLOWED                            MTLK_WWSS_WLAN_STAT_ID_BROADCAST_PACKETS_RECEIVED_ALLOWED

#define MTLK_CORE_CNT_ERROR_PACKETS_SENT_ALLOWED                                    MTLK_WWSS_WLAN_STAT_ID_ERROR_PACKETS_SENT_ALLOWED
#define MTLK_CORE_CNT_ERROR_PACKETS_RECEIVED_ALLOWED                                MTLK_WWSS_WLAN_STAT_ID_ERROR_PACKETS_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_DISCARD_PACKETS_RECEIVED_ALLOWED                              MTLK_WWSS_WLAN_STAT_ID_DISCARD_PACKETS_RECEIVED_ALLOWED
#endif

/* Statistic ALLOWED flags */
#define MTLK_CORE_CNT_RX_PACKETS_DISCARDED_DRV_DUPLICATE_ALLOWED                    MTLK_WWSS_WLAN_STAT_ID_RX_PACKETS_DISCARDED_DRV_DUPLICATE_ALLOWED

#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_NO_PEERS_ALLOWED                         MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_NO_PEERS_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DRV_UNKNOWN_DESTINATION_DIRECTED_ALLOWED MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_UNKNOWN_DESTINATION_DIRECTED_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DRV_UNKNOWN_DESTINATION_MCAST_ALLOWED    MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_UNKNOWN_DESTINATION_MCAST_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DRV_NO_RESOURCES_ALLOWED                 MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_NO_RESOURCES_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_SQ_OVERFLOW_ALLOWED                      MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_SQ_OVERFLOW_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_DROP_ALL_FILTER_ALLOWED                  MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_DROP_ALL_FILTER_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_DISCARDED_TX_QUEUE_OVERFLOW_ALLOWED                MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_TX_QUEUE_OVERFLOW_ALLOWED
#define MTLK_CORE_CNT_802_1X_PACKETS_RECEIVED_ALLOWED                               MTLK_WWSS_WLAN_STAT_ID_802_1X_PACKETS_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_802_1X_PACKETS_SENT_ALLOWED                                   MTLK_WWSS_WLAN_STAT_ID_802_1X_PACKETS_SENT_ALLOWED
#define MTLK_CORE_CNT_802_1X_PACKETS_DISCARDED_ALLOWED                              MTLK_WWSS_WLAN_STAT_ID_802_1X_PACKETS_DISCARDED_ALLOWED
#define MTLK_CORE_CNT_PAIRWISE_MIC_FAILURE_PACKETS_ALLOWED                          MTLK_WWSS_WLAN_STAT_ID_PAIRWISE_MIC_FAILURE_PACKETS_ALLOWED
#define MTLK_CORE_CNT_GROUP_MIC_FAILURE_PACKETS_ALLOWED                             MTLK_WWSS_WLAN_STAT_ID_GROUP_MIC_FAILURE_PACKETS_ALLOWED
#define MTLK_CORE_CNT_UNICAST_REPLAYED_PACKETS_ALLOWED                              MTLK_WWSS_WLAN_STAT_ID_UNICAST_REPLAYED_PACKETS_ALLOWED
#define MTLK_CORE_CNT_MULTICAST_REPLAYED_PACKETS_ALLOWED                            MTLK_WWSS_WLAN_STAT_ID_MULTICAST_REPLAYED_PACKETS_ALLOWED
#define MTLK_CORE_CNT_MANAGEMENT_REPLAYED_PACKETS_ALLOWED                           MTLK_WWSS_WLAN_STAT_ID_MANAGEMENT_REPLAYED_PACKETS_ALLOWED
#define MTLK_CORE_CNT_FWD_RX_PACKETS_ALLOWED                                        MTLK_WWSS_WLAN_STAT_ID_FWD_RX_PACKETS_ALLOWED
#define MTLK_CORE_CNT_FWD_RX_BYTES_ALLOWED                                          MTLK_WWSS_WLAN_STAT_ID_FWD_RX_BYTES_ALLOWED
#define MTLK_CORE_CNT_DAT_FRAMES_RECEIVED_ALLOWED                                   MTLK_WWSS_WLAN_STAT_ID_DAT_FRAMES_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_CTL_FRAMES_RECEIVED_ALLOWED                                   MTLK_WWSS_WLAN_STAT_ID_CTL_FRAMES_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_MAN_FRAMES_RES_QUEUE_ALLOWED                                  MTLK_WWSS_WLAN_STAT_ID_MAN_FRAMES_RES_QUEUE_ALLOWED
#define MTLK_CORE_CNT_MAN_FRAMES_SENT_ALLOWED                                       MTLK_WWSS_WLAN_STAT_ID_MAN_FRAMES_SENT_ALLOWED
#define MTLK_CORE_CNT_MAN_FRAMES_CONFIRMED_ALLOWED                                  MTLK_WWSS_WLAN_STAT_ID_MAN_FRAMES_CONFIRMED_ALLOWED
#define MTLK_CORE_CNT_MAN_FRAMES_RECEIVED_ALLOWED                                   MTLK_WWSS_WLAN_STAT_ID_MAN_FRAMES_RECEIVED_ALLOWED
#define MTLK_CORE_CNT_RX_MAN_FRAMES_RETRY_DROPPED_ALLOWED                           MTLK_WWSS_WLAN_STAT_ID_RX_MAN_FRAMES_RETRY_DROPPED_ALLOWED
#define MTLK_CORE_CNT_MAN_FRAMES_FAILED_ALLOWED                                     MTLK_WWSS_WLAN_STAT_ID_MAN_FRAMES_FAILED_ALLOWED

#define MTLK_CORE_CNT_CHANNEL_SWITCH_20_TO_40_ALLOWED                               MTLK_WWSS_WLAN_STAT_ID_NOF_CHANNEL_SWITCH_20_TO_40_ALLOWED
#define MTLK_CORE_CNT_CHANNEL_SWITCH_40_TO_20_ALLOWED                               MTLK_WWSS_WLAN_STAT_ID_NOF_CHANNEL_SWITCH_40_TO_20_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_TO_UNICAST_DGAF_DISABLED_ALLOWED                   MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_TO_UNICAST_DGAF_DISABLED_ALLOWED
#define MTLK_CORE_CNT_TX_PACKETS_SKIPPED_DGAF_DISABLED_ALLOWED                      MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_SKIPPED_DGAF_DISABLED_ALLOWED

#define MTLK_CORE_CNT_TX_PROBE_RESP_SENT_ALLOWED                                    MTLK_WWSS_WLAN_STAT_ID_TX_PROBE_RESP_SENT_ALLOWED
#define MTLK_CORE_CNT_TX_PROBE_RESP_DROPPED_ALLOWED                                 MTLK_WWSS_WLAN_STAT_ID_TX_PROBE_RESP_DROPPED_ALLOWED
#define MTLK_CORE_CNT_BSS_MGMT_TX_QUE_FULL_ALLOWED                                  MTLK_WWSS_WLAN_STAT_ID_BSS_MGMT_TX_QUE_FULL_ALLOWED

// private statistic counters
struct priv_stats {

  uint64  tx_bytes;         /* total bytes transmitted */
  uint64  rx_bytes;         /* total bytes received */

  uint64  tx_packets;       /* total packets transmitted */
  uint64  rx_packets;       /* total packets received */

  uint64  rx_multicast_packets; /*!< multicast packets received */

  // TX consecutive dropped packets counter
  uint32 tx_cons_drop_cnt;

  // Maximum number of packets dropped consecutively
  uint32 tx_max_cons_drop;

  // Applicable only to STA:
  uint32 sta_session_rx_packets; // Packets received in this session
  uint32 sta_session_tx_packets; // Packets transmitted in this session

  // Dropped Tx packets counters per priority queue
  uint32 ac_dropped_counter[NTS_TIDS_GEN6];
  uint32 ac_rx_counter[NTS_TIDS_GEN6];
  uint32 ac_tx_counter[NTS_TIDS_GEN6];

  // AP forwarding statistics
  uint32 fwd_tx_packets;
  uint32 fwd_tx_bytes;
  uint32 fwd_dropped;
  uint32 fwd_cannot_clone;

  // Reliable Multicast statistics
  uint32 rmcast_dropped;    /* FIXME: should be taken from FW */

  // Used Tx packets per priority queue
  uint32 ac_used_counter[NTS_TIDS_GEN6];

  // Received BAR frames
  uint32 bars_cnt;

  //trasmitted broadcast/non-reliable multicast packets
  uint32 tx_bcast_nrmcast;

  //number of disconnections
  uint32 num_disconnects;

  /* number of Rx : Wrong nwid/essid */
  uint32 discard_nwi;

  /* Missed beacons/superframe */
  uint32 missed_beacon;

  /* tx errors statistics */
  uint32 txerr_drop_all_filter;
  uint32 txerr_undef_mc_index;
  uint32 txerr_swpath_overruns;
  uint32 txerr_swpath_hwsend;
  uint32 txerr_dc_xmit;


  unsigned long tx_overruns;      /*!< total tx queue overruns */
};

/* core to DF UI interface*/
typedef struct _mtlk_core_general_stats_t {
  struct priv_stats   core_priv_stats;
  mtlk_mhi_stats_vap_t  mhi_vap_stat;
  uint32  tx_msdus_free;
  uint32  tx_msdus_usage_peak;
  uint32  bist_check_passed;
  uint16  net_state;
  uint8   max_rssi;
  uint8   noise;
  uint8   channel_load; /* percent */
  uint8   channel_util; /* percent */
  uint8   airtime;      /* percent */
  uint32  airtime_efficiency; /* bytes per sec */
  IEEE_ADDR bssid;
  uint32  txmm_sent;
  uint32  txmm_cfmd;
  uint32  txmm_peak;
  uint32  txdm_sent;
  uint32  txdm_cfmd;
  uint32  txdm_peak;
  uint32  fw_logger_packets_processed;
  uint32  fw_logger_packets_dropped;

  uint64  tx_bytes;         /*!< total bytes transmitted */
  uint64  rx_bytes;         /*!< total bytes received */
  uint64  tx_packets;       /*!< total packets transmitted */
  uint64  rx_packets;       /*!< total packets received */
  uint64  rx_multicast_packets; /*!< multicast packets received */

  uint32  pairwise_mic_failure_packets;
  uint32  group_mic_failure_packets;
  uint32  unicast_replayed_packets;
  uint32  multicast_replayed_packets;
  uint32  management_replayed_packets;

  uint32  fwd_rx_packets;
  uint32  fwd_rx_bytes;

  // Received data, control and management 802.11 frames from MAC
  uint32 rx_dat_frames;
  uint32 rx_ctl_frames;
  uint32 tx_man_frames_res_queue;
  uint32 tx_man_frames_sent;
  uint32 tx_man_frames_confirmed;
  uint32 rx_man_frames;
  uint32 rx_man_frames_retry_dropped;
  uint32 rx_man_frames_fail;

  uint32 tx_max_cfm_in_tasklet;
  uint32 rx_max_pck_in_tasklet;
  uint64 tx_max_time_int_to_tasklet;
  uint64 tx_max_time_int_to_pck;
  uint64 rx_max_time_int_to_tasklet;
  uint64 rx_max_time_int_to_pck;

  uint32 dgaf_disabled_tx_pck_dropped;
  uint32 dgaf_disabled_tx_pck_converted;

  uint32 radars_detected;

  /* Number of sent probe responses */
  uint32 tx_probe_resp_sent_cnt;
  uint32 tx_probe_resp_sent_cnt_per_radio;
  /* Number of dropped probe responses */
  uint32 tx_probe_resp_dropped_cnt;
  uint32 tx_probe_resp_dropped_cnt_per_radio;
  /* Number of dropped BSS mgmt msgs due to tx que full */
  uint32 bss_mgmt_tx_que_full_cnt;
  uint32 bss_mgmt_tx_que_full_cnt_per_radio;
  /* Max.size of bss mgmt bds queue */
  uint32 bss_mgmt_bds_max_num;
  uint32 bss_mgmt_bds_max_num_res;
  /* Number of free entries in bss mgmt bds queue */
  uint32 bss_mgmt_bds_free_num;
  uint32 bss_mgmt_bds_free_num_res;
  /* Peak usage of bss mgmt bds queue */
  uint32 bss_mgmt_bds_usage_peak;
  uint32 bss_mgmt_bds_usage_peak_res;
#ifdef CPTCFG_IWLWAV_LEGACY_INT_RECOVERY_MON
  /* Interrupt recovery */
  uint32 isr_lost_suspect; /* Number of lost interrupt suspicions */
  uint32 isr_recovered;    /* Number of confirmed interrupt loses */
#endif
} mtlk_core_general_stats_t;

struct nic;

#define CORE_KEY_SEQ_LEN     8    /* Key sequence length */

typedef struct _core_key_t
{
  uint8   key[WAVE_MAX_KEY_LEN];  /* Key data */
  uint8   key_len;                /* Key length */
  uint8   seq[CORE_KEY_SEQ_LEN];  /* key sequence */
  uint8   seq_len;                /* sequence number length */
  uint32  cipher;                 /* CipherSuite for this key */
} core_key_t;

#define MAX_SCAN_WAIT_RETRIES 600
#define MAX_VAP_WAIT_RETRIES 600
#define AMSDU_VNUM_DFLT   5
#define AMSDU_HENUM_DFLT  7

#define ST_NONE     (ST_LAST) /* add-on to ST_LAST from mhi_umi_propr.h */
#define ST_ERROR    (ST_LAST + 1)

static __INLINE
const char *switchtype2string(unsigned int type)
{
  const char *printable_types[] = {
    "NORMAL", "SCAN", "CSA", "RSSI",
    "NORMAL_AFTER_RSSI",
    "CALIBRATE","DPP",
    "NONE", "ERROR" };

  MTLK_STATIC_ASSERT(ST_ERROR + 1 == ARRAY_SIZE(printable_types));
  return printable_types[(type > ST_LAST) ? ST_ERROR : type];
}

struct nic_slow_ctx {
  struct nic *nic;
  sta_db stadb;
  hst_db hstdb;

  /* used inorder to get peer ap and when we are in
   * MESH_MODE_BACKHAUL_AP (then a single station is connected)
   * to get that station
   */
  IEEE_ADDR last_sta_addr;

  /* WDS peers manager */
  wds_t wds_mng;

  tx_limit_t  tx_limits;

  /* actual BSS data */
  bss_data_t         bss_data;

  // 802.11i (security) stuff
  core_key_t    keys[WAVE_NUM_KEYS];
  uint8     seq[8];
  uint8 default_key;        /* default key index */
  uint8 default_mgmt_key;   /* default mgmt key index */
  bool  wep_enabled;
  bool  rsn_enabled;
  uint8 group_cipher;
  struct crypto_cipher *wep_tx; /* crypto engine */
  struct crypto_cipher *wep_rx; /* crypto engine */
  uint32 wep_iv;                /* crypto engine */

  /* WDS security */
  uint8 peerAPs_key_idx;    /* WEP key index for Peer APs (0 - disabled) */
  core_key_t    wds_keys[WAVE_WEP_NUM_DEFAULT_KEYS];

#ifdef MTLK_DEBUG_CHARIOT_OOO
  uint16 seq_prev_sent[NTS_TIDS];
#endif

  mtlk_osal_timer_t mac_watchdog_timer;

  mtlk_serializer_t serializer;

  int mac_stuck_detected_by_sw;

  mtlk_osal_timestamp_t last_arp_probe;

  beacon_manager_t beacon_man_data;

  u8 *probe_resp_templ;
  u8 *probe_resp_templ_non_he;
  unsigned probe_resp_templ_len;
  unsigned probe_resp_templ_non_he_len;

  mgmt_frame_filter_t mgmt_frame_filter;

  mtlk_osal_timer_t cca_step_down_timer;
  struct mtlk_cca_adapt cca_adapt;

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;

  BOOL is_block_tx;
  BOOL ap_started; /* to know if we are ap & up for probe responses*/
};

typedef struct _mtlk_dut_handlers_t mtlk_dut_handlers_t;

typedef struct _mtlk_dut_core_t
{
  mtlk_dut_handlers_t *dut_handlers;
  mtlk_irbd_handle_t  *dut_start_handle;
  mtlk_irbd_handle_t  *dut_stop_handle;
  BOOL                isStarted;

  MTLK_DECLARE_START_STATUS;
} mtlk_dut_core_t;

typedef struct _ieee_addr_list_t {
  mtlk_osal_spinlock_t      ieee_addr_lock;
  mtlk_hash_t               hash;
} ieee_addr_list_t;

int __MTLK_IFUNC mtlk_dut_core_register(mtlk_core_t *core);
void __MTLK_IFUNC mtlk_dut_core_unregister(mtlk_core_t *core);

struct nic {

  struct nic_slow_ctx *slow_ctx;

  struct priv_stats pstats;

  mtlk_mhi_stats_vap_t  mhi_vap_stat;

  poll_stat_obj_t       poll_stat;  /* statistics for update after polling mhi_vap_stat */
  uint32                poll_stat_last[MTLK_CORE_CNT_POLL_STAT_NUM]; /* values of last update */

  /* MBSS instance ID  - it is assigned by the hypervisor and should be used when communicating with HAL*/
  mtlk_vap_handle_t   vap_handle;

  // reliable multicast context
  mcast_ctx_t mcast;

  mtlk_atomic_t net_state;

  // FIXME aren't these doubling the net_state to a large degree?
  BOOL  is_stopping;
  BOOL  is_iface_stopping;
  BOOL  is_stopped;

  mtlk_txmm_msg_t           txmm_async_eeprom_msgs[MAX_NUM_TX_ANTENNAS]; /* must be moved to EEPROM module ASAP */

  mtlk_core_hot_path_param_handles_t  pdb_hot_path_handles;

  mtlk_wss_t               *radio_wss;
  mtlk_wss_t               *wss;
  mtlk_wss_cntr_handle_t   *wss_hcntrs[MTLK_CORE_CNT_LAST];

  mtlk_dut_core_t           dut_core;

  mtlk_atomic_t             unconfirmed_data;

  uint32                    storedCalibrationChannelBitMap[CALIBR_BITMAP_SIZE];

  BOOL                      vap_in_fw_is_active;

  /* DSCP-to-UP mapping table */
  uint8                     dscp_table[DSCP_NUM];
  BOOL                      dgaf_disabled;
  #ifdef WAVE_DCDP_DGAF_SUPPORTED
  int8                      dgaf_disabled_update;
  #endif

  uint32                    acs_updated_ts;

  /* entries with zero reject status code */
  ieee_addr_list_t          widan_blacklist;
  /* entries with non-zero reject status code */
  ieee_addr_list_t          multi_ap_blacklist;
  ieee_addr_list_t          four_addr_sta_list;

  ieee_addr_list_t          broadcast_probe_resp_sta_list;
  ieee_addr_list_t          unicast_probe_resp_sta_list;
  ieee_addr_list_t          wds_wpa_sta_list;

  /* occurs when NDP sent from STA interface to AP has been acknowledged */
  mtlk_osal_event_t         ndp_acked;
  BOOL                      waiting_for_ndp_ack;

  /* occurs when EAPOL frame sent from STA VAP to far AP has been acknowledged */
  mtlk_osal_event_t         eapol_acked;
  BOOL                      waiting_for_eapol_ack;

  /* statistics auto polling period */
  uint32                    stats_poll_period;

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_INIT_LOOP(TXMM_EEPROM_ASYNC_MSGS_INIT);
  MTLK_DECLARE_START_STATUS;
};

typedef struct _blacklist_snr_info_t {
  uint8 snrProbeHWM;
  uint8 snrProbeLWM;
} blacklist_snr_info_t;

typedef struct _ieee_addr_entry_t {
  MTLK_HASH_ENTRY_T(ieee_addr) hentry;
  uint8 data[0];
} ieee_addr_entry_t;

typedef enum
  {
    CFG_INFRA_STATION,
    CFG_ADHOC_STATION,
    CFG_ACCESS_POINT,
    CFG_TEST_MAC,
    CFG_NUM_NET_TYPES
  } CFG_NETWORK_TYPE;

typedef enum bridge_mode {
  BR_MODE_NONE        = 0,
  BR_MODE_WDS         = 1,
  BR_MODE_L2NAT       = 2,
  BR_MODE_MAC_CLONING = 3,
  BR_MODE_LAST
} bridge_mode_t;

typedef enum _mtlk_core_4addr_mode_t
{
  MTLK_CORE_4ADDR_NONE = 0,
  MTLK_CORE_4ADDR_STATIC,
  MTLK_CORE_4ADDR_DYNAMIC,
  MTLK_CORE_4ADDR_LIST,
  MTLK_CORE_4ADDR_LAST
} mtlk_core_four_addr_mode_t;

typedef enum _mesh_mode_t {
  MESH_MODE_FRONTHAUL_AP = 0, /* regular mode, 3addr stations only or legacy WDS */
  MESH_MODE_BACKHAUL_AP  = 1, /* vap for 4addr stations only, will create virtual netdevs if max_num_sta > 1 */
  MESH_MODE_HYBRID       = 2, /* vap for both 3addr and 4addr stations */
  MESH_MODE_EXT_HYBRID   = 3, /* vap for both 3addr and 4addr stations, creates virtual netdevs for 4addr statinos */
  MESH_MODE_RESERVED     = 4,
  MESH_MODE_LAST
} mesh_mode_t;

void mtlk_core_record_xmit_err (struct nic *nic, struct sk_buff *skb, uint32 nbuf_flags)
  __MTLK_INT_HANDLER_SECTION;

int mtlk_set_hw_state(mtlk_core_t *nic, mtlk_hw_state_e st);
BOOL mtlk_core_is_hw_halted(mtlk_core_t *nic);

mtlk_handle_t __MTLK_IFUNC mtlk_core_get_tx_limits_handle(mtlk_handle_t nic);

/* Size of this structure must be multiple of sizeof(void*) because   */
/* it is immediately followed by data of the request, and the data,   */
/* generally speaking, must be sizeof(void*)-aligned.                 */
/* There is corresponding assertion in the code.                      */
typedef struct __core_async_exec_t
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_command_t        cmd;
  mtlk_handle_t         receiver;
  uint32                data_size;
  mtlk_core_task_func_t func;
  mtlk_user_request_t  *user_req;
  mtlk_vap_handle_t     vap_handle;
  mtlk_ability_id_t     ability_id;
} _core_async_exec_t;

static __INLINE BOOL
mtlk_is_dut_core_active (mtlk_core_t *core)
{
  MTLK_ASSERT(core != NULL);
  return core->dut_core.isStarted;
}

/* counters will be modified with checking ALLOWED option */
#define mtlk_core_dec_cnt(core, id)         { if (id##_ALLOWED) __mtlk_core_dec_cnt(core, id); }
#define mtlk_core_inc_cnt(core, id)         { if (id##_ALLOWED) __mtlk_core_inc_cnt(core, id); }
#define mtlk_core_add_cnt(core, id, val)    { if (id##_ALLOWED) __mtlk_core_add_cnt(core, id, val); }
#define mtlk_core_dec64_cnt(core, id)       { if (id##_ALLOWED) __mtlk_core_dec64_cnt(core, id); }
#define mtlk_core_inc64_cnt(core, id)       { if (id##_ALLOWED) __mtlk_core_inc64_cnt(core, id); }
#define mtlk_core_add64_cnt(core, id, val)  { if (id##_ALLOWED) __mtlk_core_add64_cnt(core, id, val); }


static __INLINE void
__mtlk_core_dec_cnt (mtlk_core_t       *core,
                   mtlk_core_wss_cnt_id_e cnt_id)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_CORE_CNT_LAST);

  mtlk_wss_cntr_dec(core->wss_hcntrs[cnt_id]);
}

static __INLINE void
__mtlk_core_inc_cnt (mtlk_core_t       *core,
                   mtlk_core_wss_cnt_id_e cnt_id)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_CORE_CNT_LAST);

  mtlk_wss_cntr_inc(core->wss_hcntrs[cnt_id]);
}

static __INLINE void
__mtlk_core_add_cnt (mtlk_core_t       *core,
                   mtlk_core_wss_cnt_id_e cnt_id,
                   uint32            val)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_CORE_CNT_LAST);

  mtlk_wss_cntr_add(core->wss_hcntrs[cnt_id], val);
}

static __INLINE void
__mtlk_core_dec64_cnt (mtlk_core_t       *core,
                   mtlk_core_wss_cnt_id_e cnt_id)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_CORE_CNT_LAST);

  mtlk_wss_cntr_dec64(core->wss_hcntrs[cnt_id]);
}

static __INLINE void
__mtlk_core_inc64_cnt (mtlk_core_t       *core,
                   mtlk_core_wss_cnt_id_e cnt_id)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_CORE_CNT_LAST);

  mtlk_wss_cntr_inc64(core->wss_hcntrs[cnt_id]);
}

static __INLINE void
__mtlk_core_add64_cnt (mtlk_core_t       *core,
                   mtlk_core_wss_cnt_id_e cnt_id,
                   uint64            val)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_CORE_CNT_LAST);

  mtlk_wss_cntr_add64(core->wss_hcntrs[cnt_id], val);
}


static __INLINE BOOL
mtlk_core_scan_is_running(struct nic *nic)
{
  return wave_radio_scan_is_running(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE mtlk_scan_support_t*
__wave_core_scan_support_get(struct nic *nic)
{
  return wave_radio_scan_support_get(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE mtlk_coc_t*
__wave_core_coc_mgmt_get(struct nic *nic)
{
  return wave_radio_coc_mgmt_get(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE mtlk_erp_t*
__wave_core_erp_mgmt_get(struct nic *nic)
{
  return wave_radio_erp_mgmt_get(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE UMI_HDK_CONFIG*
__wave_core_hdkconfig_get(struct nic *nic)
{
  return wave_radio_hdkconfig_get(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE struct mtlk_chan_def*
__wave_core_chandef_get(struct nic *nic)
{
  return wave_radio_chandef_get(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE int
__wave_core_chan_switch_type_get(struct nic *nic)
{
  return wave_radio_chan_switch_type_get(wave_vap_radio_get(nic->vap_handle));
}

static __INLINE void
__wave_core_chan_switch_type_set(struct nic *nic, int value)
{
  return wave_radio_chan_switch_type_set(wave_vap_radio_get(nic->vap_handle), value);
}

/* Looking for beacons can't interrupt the FW until set_channel is sent with type ST_NORMAL */
static __INLINE BOOL
mtlk_core_is_block_tx_mode(struct nic *mcore)
{
  MTLK_ASSERT(mcore == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(mcore->vap_handle)));

  return (mcore->slow_ctx->is_block_tx == TRUE);
}

/* FW is in scan mode until set_channel is sent with type ST_NORMAL */
static __INLINE BOOL
mtlk_core_is_in_scan_mode(struct nic *mcore)
{
  MTLK_ASSERT(mcore == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(mcore->vap_handle)));

  return (__wave_core_chan_switch_type_get(mcore) == ST_SCAN);
}

static __INLINE BOOL
mtlk_core_is_in_oper_mode(struct nic *mcore)
{
  int type;

  MTLK_ASSERT(mcore == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(mcore->vap_handle)));

  type = __wave_core_chan_switch_type_get(mcore);
  return (type == ST_NORMAL || type == ST_NORMAL_AFTER_RSSI || type == ST_DPP_ACTION);
}

/* Checks whether we really know for sure what channel we're on */
static __INLINE
unsigned is_channel_certain(const struct mtlk_chan_def *cd)
{
  return cd->center_freq1;
}

static __INLINE struct set_chan_param_data * mtlk_core_chan_param_data_get (struct nic *mcore)
{
  mtlk_vap_manager_t *vap_manager = mtlk_vap_get_manager(mcore->vap_handle);

  MTLK_ASSERT(mcore == mtlk_vap_manager_get_master_core(vap_manager));

  return (struct set_chan_param_data *)wave_rcvry_chanparam_get(mtlk_vap_manager_get_hw(vap_manager), vap_manager);
}

static __INLINE BOOL mtlk_core_is_chan_switch_type_csa_with_block_tx (struct nic *mcore)
{
  struct set_chan_param_data *cpd = mtlk_core_chan_param_data_get(mcore);

  if (cpd)
    return (BOOL)((ST_CSA == cpd->switch_type) && cpd->block_tx_post);

  return FALSE;
}

mtlk_core_t * __MTLK_IFUNC
mtlk_core_get_master(mtlk_core_t *core);

int __MTLK_IFUNC
mtlk_core_get_net_state(mtlk_core_t *core) __MTLK_INT_HANDLER_SECTION;

mtlk_hw_state_e __MTLK_IFUNC
mtlk_core_get_hw_state (mtlk_core_t *nic);

tx_limit_t* __MTLK_IFUNC
mtlk_core_get_tx_limits(mtlk_core_t *core);

static __INLINE hst_db* __MTLK_IFUNC
mtlk_core_get_hstdb(mtlk_core_t *core){
  MTLK_ASSERT(core);
  return &core->slow_ctx->hstdb;
}

static __INLINE sta_db* __MTLK_IFUNC
mtlk_core_get_stadb(mtlk_core_t *core){
  MTLK_ASSERT(core);
  return &core->slow_ctx->stadb;
}

void __MTLK_IFUNC mtlk_core_analyze_and_send_up(mtlk_core_t* nic, mtlk_core_handle_tx_data_t *tx_data, sta_entry *src_sta)
__MTLK_INT_HANDLER_SECTION;

struct disreq_status_stuff
{
  IEEE_ADDR addr;
  uint64 cookie;
  uint8 *virt_addr;
  uint32 size;
  uint8 status;
};

typedef struct _mtlk_core_dev_spec_info_t {
  int mcf;
  int mc_index;
  int sta_id, vap_id;
  uint8 frame_type;
} mtlk_core_dev_spec_info_t;

typedef struct mtlk_core_interferer_emu {
  UMI_CONTINUOUS_INTERFERER interf_data;
  BOOL emulating_interferer;
} mtlk_core_interferer_emu_t;

typedef struct _mtlk_core_radar_emu {
  UMI_RADAR_DETECTION radar_det;
  BOOL emulating_radar;
} mtlk_core_radar_emu_t;

int __MTLK_IFUNC mtlk_core_get_dev_spec_info(struct net_device *dev, mtlk_nbuf_t *nbuf, mtlk_core_dev_spec_info_t *dev_spec_info);

uint16 __MTLK_IFUNC mtlk_core_get_sid_by_addr(mtlk_core_t *nic, uint8 *addr);
int __MTLK_IFUNC mtlk_core_set_bss(mtlk_core_t *core, mtlk_core_t *context_core, UMI_SET_BSS *fw_params);
int core_change_bss(mtlk_handle_t hcore, const void* data, uint32 data_size);
int core_ap_stop_traffic(struct nic *nic, uint16 sid, IEEE_ADDR *sta_addr);
int core_ap_remove_sta(struct nic *nic, uint16 sid, IEEE_ADDR *sta_addr);
uint32 __MTLK_IFUNC wave_core_param_db_basic_rates_get(mtlk_core_t *core, mtlk_hw_band_e band, uint8 *rates_buf, uint32 rates_buf_len);
int mtlk_mbss_send_vap_activate(struct nic *nic, mtlk_hw_band_e band);
int mtlk_handle_eapol(mtlk_vap_handle_t vap_handle, void *data, int data_len);
int mtlk_core_is_chandef_identical(struct mtlk_chan_def *chan1, struct mtlk_chan_def *chan2);
int mtlk_core_is_band_supported(mtlk_core_t *nic, mtlk_hw_band_e band);
int mtlk_core_update_network_mode(mtlk_core_t* nic, uint8 net_mode);
int finish_fw_set_chan_if_any(mtlk_core_t *core);
int finish_and_prevent_fw_set_chan(mtlk_core_t *core, BOOL yes);
BOOL __MTLK_IFUNC mtlk_core_crypto_decrypt(mtlk_core_t *core, uint32 key_idx, uint8 *fbuf, uint8 *data, int32 data_len);

uint32 mtlk_core_get_max_tx_antennas (struct nic* nic);
uint32 mtlk_core_get_max_rx_antennas (struct nic* nic);

uint32 mtlk_core_get_max_tx_ampdu_density (struct nic* nic, uint32 sta_ampdu_density);

void __MTLK_IFUNC mtlk_core_on_bss_tx(mtlk_core_t *nic, uint32 subtype);
void __MTLK_IFUNC mtlk_core_on_bss_cfm(mtlk_core_t *nic, IEEE_ADDR *peer, uint32 extra_processing, uint32 subtype, BOOL is_broadcast);
#ifndef WAVE_BSS_TX_DPP_EASY_CONNECT
void __MTLK_IFUNC mtlk_core_on_bss_dpp_acked(mtlk_core_t *nic, uint32 subtype, uint8 action_code ,uint8 action_field, BOOL is_dpp);
#endif
void __MTLK_IFUNC mtlk_core_on_bss_res_queued(mtlk_core_t *nic);
void __MTLK_IFUNC mtlk_core_on_bss_res_freed(mtlk_core_t *nic);

int __MTLK_IFUNC mtlk_core_set_coc_actual_power_mode(mtlk_core_t *core);
int __MTLK_IFUNC mtlk_core_set_coc_pause_power_mode(mtlk_core_t *core);
int __MTLK_IFUNC mtlk_core_radio_enable_if_needed(mtlk_core_t *core);
int __MTLK_IFUNC mtlk_core_radio_disable_if_needed(mtlk_core_t *core);
int current_chandef_init(mtlk_core_t *core, struct mtlk_chan_def *ccd);

BOOL __MTLK_IFUNC mtlk_core_is_looped_packet(mtlk_core_t* nic, mtlk_nbuf_t *nbuf) __MTLK_INT_HANDLER_SECTION;

void
_mtlk_core_flush_ieee_addr_list (mtlk_core_t *nic,
                                 ieee_addr_list_t *list, char *name);
BOOL
_mtlk_core_ieee_addr_entry_exists (mtlk_core_t *nic, const IEEE_ADDR *addr,
                                   ieee_addr_list_t *list, char *name);
void*
__mtlk_core_add_ieee_addr_entry_internal (mtlk_core_t *nic, const IEEE_ADDR *addr,
                                   ieee_addr_list_t *list, char *name, size_t extra_size);
int
_mtlk_core_add_ieee_addr_entry (mtlk_core_t* nic, const IEEE_ADDR *addr,
                                   ieee_addr_list_t *list, char *name);
int
_mtlk_core_del_ieee_addr_entry (mtlk_core_t* nic, const IEEE_ADDR *addr,
                                   ieee_addr_list_t *list, char *name,
                                   BOOL entry_expected);
int
_mtlk_core_dump_ieee_addr_list (mtlk_core_t *nic,
                                ieee_addr_list_t *list, char *name,
                                const void* data, uint32 data_size);

int __MTLK_IFUNC mtlk_core_get_channel(mtlk_core_t *core);

uint16 __MTLK_IFUNC wave_hw_get_all_sta_sid(mtlk_hw_t* hw);
void  __MTLK_IFUNC mtlk_core_get_tr181_wlan_stats (mtlk_core_t* core, mtlk_wssa_drv_tr181_wlan_stats_t* stats);

int fill_channel_data(mtlk_core_t *core, struct intel_vendor_channel_data *ch_data);
BOOL _mtlk_core_cca_is_above_configured (mtlk_core_t *core, iwpriv_cca_th_t *cca_th_params);

void __MTLK_IFUNC
wave_core_cfg_set_whm_hw_logger_cfg (struct nic *nic, wave_whm_hwlogger_cfg_t *hw_logger, uint32 data_size);

#ifdef CONFIG_WAVE_DEBUG
void  __MTLK_IFUNC wave_core_get_wlan_host_if_qos_stats (mtlk_core_t* core, mtlk_wssa_drv_wlan_host_if_qos_t* stats);
void  __MTLK_IFUNC wave_core_get_wlan_host_if_stats (mtlk_core_t* core, mtlk_wssa_drv_wlan_host_if_t* stats);
void  __MTLK_IFUNC wave_core_get_wlan_rx_stats (mtlk_core_t* core, mtlk_wssa_drv_wlan_rx_stats_t* stats);
void  __MTLK_IFUNC wave_core_get_wlan_baa_stats (mtlk_core_t* core, mtlk_wssa_drv_wlan_baa_stats_t* stats);
int __MTLK_IFUNC wave_hw_get_dbg_pn_reset(mtlk_hw_t* hw);
#endif /* CONFIG_WAVE_DEBUG */

int __MTLK_IFUNC mtlk_core_send_iwpriv_config_to_fw (mtlk_core_t *core);
int mtlk_core_set_essid_by_cfg(mtlk_core_t *core, mtlk_gen_core_cfg_t *core_cfg);
uint8 __MTLK_IFUNC mtlk_total_airtime_get (uint8 utilization, uint8 load);

#ifdef CONFIG_WAVE_DEBUG
void mtlk_core_bswap_bcl_request(UMI_BCL_REQUEST *req, BOOL hdr_only);

static __INLINE int wave_core_get_dbg_pn_reset (mtlk_core_t *core)
{
  MTLK_ASSERT(NULL != core);
  return wave_hw_get_dbg_pn_reset(mtlk_vap_get_hw(core->vap_handle));
}
#endif

BOOL __MTLK_IFUNC
mtlk_core_get_ieee80211_radar_detected(mtlk_core_t* core, uint32 freq);

void __MTLK_IFUNC
mtlk_core_save_chan_statistics_info (mtlk_core_t *core, struct intel_vendor_channel_data *ch_data);

static __INLINE void wave_core_sync_hostapd_done_set (mtlk_core_t *core, BOOL value)
{
  MTLK_ASSERT(NULL != core);
  wave_radio_sync_hostapd_done_set(wave_vap_radio_get(core->vap_handle), value);
}

static __INLINE BOOL wave_core_sync_hostapd_done_get (mtlk_core_t *core)
{
  MTLK_ASSERT(NULL != core);
  return wave_radio_is_sync_hostapd_done(wave_vap_radio_get(core->vap_handle));
}

static __INLINE uint16 wave_core_get_all_sta_sid (mtlk_core_t *core)
{
  MTLK_ASSERT(NULL != core);
  return wave_hw_get_all_sta_sid(mtlk_vap_get_hw(core->vap_handle));
}

static __INLINE void wave_core_qos_adjust_priority (mtlk_core_t *core, mtlk_nbuf_t *nbuf)
{
  MTLK_ASSERT(NULL != core);
  mtlk_qos_adjust_priority(core->dscp_table, nbuf);
}

void __MTLK_IFUNC wave_core_sta_disconnect(mtlk_vap_handle_t vap_handle, IEEE_ADDR *addr);


int _mtlk_core_add_bcast_probe_resp_entry (mtlk_core_t* nic, const IEEE_ADDR *addr);

int _mtlk_core_del_bcast_probe_resp_entry (mtlk_core_t* nic, const IEEE_ADDR *addr);

BOOL __MTLK_IFUNC
mtlk_core_convert_dhcp_bcast_to_ucast(mtlk_nbuf_t *nbuf);

bool mtlk_core_broadcast_probe_req_enhanced_check(mtlk_hw_band_e band, mtlk_pdb_t *param_db_core, u8 *buf, u8 buf_size, IEEE_ADDR *bssid, BOOL *wildcard_ssid);

sta_entry * __MTLK_IFUNC  wave_core_find_sta (mtlk_core_t *core, const unsigned char *mac);

u8 * _wave_reset_ssid_in_probe_response_templ(uint8 *buf, size_t len);

int mtlk_core_get_tx_rx_warn(mtlk_handle_t hcore, const void* data, uint32 data_size);

int __MTLK_IFUNC
mtlk_core_get_temperature_req(mtlk_core_t *core, uint32 *temperature, uint32 calibrate_mask);

mtlk_error_t __MTLK_IFUNC
wave_core_handle_sta_excessive_retries (mtlk_handle_t core_object, const void *payload, uint32 size);
#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif
