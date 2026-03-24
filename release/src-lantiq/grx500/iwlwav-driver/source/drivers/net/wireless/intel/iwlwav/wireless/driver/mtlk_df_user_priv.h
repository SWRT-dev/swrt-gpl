/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*****************************************************************************/
/*! \file mtlk_df_user_priv.h
 * \brief Driver framework implementation for Linux
 */

#ifndef __DF_USER_PRIVATE_H__
#define __DF_USER_PRIVATE_H__

#include <net/iw_handler.h>
#include <net/cfg80211.h>
#include "mtlk_vap_manager.h"
#include "mtlkhal.h"
#include "mtlk_dcdp.h"
#include "mtlk_coreui.h"
#include "mac80211.h"

#define MAX_PROC_STR_LEN        32

#ifdef MTLK_LGM_PLATFORM_FPGA
#define _DF_STAT_POLL_PERIOD  (200/10)
#else
#define _DF_STAT_POLL_PERIOD  (200)
#endif

#ifdef CPTCFG_IWLWAV_DEBUG
#define AOCS_DEBUG
#endif

#define LOG_LOCAL_GID   GID_DFUSER
#define LOG_LOCAL_FID   0

typedef struct _mtlk_df_user_t mtlk_df_user_t;
typedef mtlk_df_user_t* mtlk_df_user_p;

enum {
  PRM_ID_FIRST = 0x7fff, /* Range 0x0000 - 0x7fff reserved for MIBs */
  /* FW Recovery */
  PRM_ID_FW_RECOVERY = (PRM_ID_FIRST + 1), /* number is fixed for applications usage */
  PRM_ID_RECOVERY_STATS,

  /* Per core configuration parameters */
  /* WDS configuration */
  PRM_ID_ADD_PEERAP,
  PRM_ID_DEL_PEERAP,
  PRM_ID_PEERAP_KEY_IDX,
  PRM_ID_PEERAP_LIST,

  /* General Core configuration */
  PRM_ID_BRIDGE_MODE,
  PRM_ID_RELIABLE_MULTICAST,
  PRM_ID_AP_FORWARDING,
  PRM_ID_ADMISSION_CAPACITY,
  PRM_ID_RX_THRESHOLD,

  /* Four address configuration */
  PRM_ID_4ADDR_STA_ADD,
  PRM_ID_4ADDR_STA_DEL,
  PRM_ID_4ADDR_STA_LIST,
  PRM_ID_CORE_LAST,
  /* End of core configuration */

  /* Per radio configuration parameters */
  /* AP Capabilities */
  PRM_ID_AP_CAPABILITIES_MAX_STAs,
  PRM_ID_AP_CAPABILITIES_MAX_VAPs,

  /* 11H configuration */
  PRM_ID_11H_CHANNEL_AVAILABILITY_CHECK_TIME,
  PRM_ID_11H_EMULATE_RADAR_DETECTION,
  PRM_ID_11H_RADAR_DETECTION,

  /* General Radio configuration */
  PRM_ID_DBG_SW_WD_ENABLE,
  PRM_ID_NETWORK_MODE,
  PRM_ID_CHANNEL,
  PRM_ID_NICK_NAME,
  PRM_ID_ESSID,
  PRM_ID_BSSID,
  PRM_ID_MU_OPERATION,
  PRM_ID_RTS_MODE,
  PRM_ID_BF_MODE,
  PRM_ID_PIE_CFG,
  PRM_ID_ETSI_PPDU_LIMITS,
  PRM_ID_MU_OFDMA_BF,
  PRM_ID_PROBE_REQ_LIST,

  /* EEPROM configuration */
  PRM_ID_EEPROM,

  /* TPC config */
  PRM_ID_TPC_LOOP_TYPE,

#if MTLK_USE_DIRECTCONNECT_DP_API
  /* DC DP LitePath configuration */
  PRM_ID_DCDP_API_LITEPATH,
#endif
#if MTLK_USE_PUMA6_UDMA
  /* UDMA configuration */
  PRM_ID_UDMA_API,
  PRM_ID_UDMA_API_EXT,
  PRM_ID_UDMA_VLAN_ID,
  PRM_ID_UDMA_VLAN_ID_EXT,
#endif
  /* DataPath mode */
  PRM_ID_DCDP_API_DATAPATH_MODE,

  /* COC configuration */
  PRM_ID_COC_POWER_MODE,
  PRM_ID_COC_AUTO_PARAMS,

  /* MBSS configuration */
  PRM_ID_VAP_ADD,
  PRM_ID_VAP_DEL,

  /* Interference Detection */
  PRM_ID_INTERFER_THRESH,
  PRM_ID_EMULATE_INTERFERER,

  /* 11B configuration */
  PRM_ID_11B_ANTENNA_SELECTION,

  PRM_ID_OUT_OF_SCAN_CACHING,
  PRM_ID_ALLOW_SCAN_DURING_CAC,
  PRM_ID_AGG_RATE_LIMIT,
  PRM_ID_RX_DUTY_CYCLE,
  PRM_ID_TX_POWER_LIMIT_OFFSET,
  PRM_ID_PROTECTION_METHOD,
  PRM_ID_TEMPERATURE_SENSOR,
  PRM_ID_TX_POWER,
  PRM_ID_QAMPLUS_MODE,
  PRM_ID_RADIO_MODE,
  PRM_ID_ACS_UPDATE_TO,
  PRM_ID_AGGR_CONFIG,
  PRM_ID_AMSDU_NUM,
  PRM_ID_CCA_THRESHOLD,
  PRM_ID_CCA_ADAPT,
  PRM_ID_RADAR_RSSI_TH,
  PRM_ID_MAX_MPDU_LENGTH,
  PRM_ID_FIXED_RATE,
  PRM_ID_ACTIVE_ANT_MASK,

  PRM_ID_STATIC_PLAN_CONFIG,
  PRM_ID_TXOP_CONFIG,
  PRM_ID_SSB_MODE,
  PRM_ID_COEX_CFG,

  /* Multicast handling */
  PRM_ID_MCAST_RANGE_SETUP,
  PRM_ID_MCAST_RANGE_SETUP_IPV6,
  PPM_ID_FORWARD_UNKNOWN_MCAST_FLAG,

  PRM_ID_ONLINE_CALIBRATION_ALGO_MASK,
  PRM_ID_CALIBRATION_ALGO_MASK,
  PRM_ID_FREQ_JUMP_MODE,
  PRM_ID_RESTRICTED_AC_MODE,
  PRM_ID_PD_THRESHOLD,
  PRM_ID_FAST_DROP,
  PRM_ID_ERP,
  PRM_ID_PVT_SENSOR,
  PRM_ID_SOFTBLOCK_THRESHOLD,

  PRM_ID_RTS_RATE,
  PRM_ID_ZWDFS_ANT_CONFIG,

  PRM_ID_NFRP_CFG,

  PRM_ID_DYNAMIC_MU_TYPE,
  PRM_ID_HE_MU_FIXED_PARAMTERS,
  PRM_ID_HE_MU_DURATION,

  PRM_ID_AX_DEFAULT_PARAMS,

  /* PHY in band power */
  PRM_ID_PHY_INBAND_POWER,
  PRM_ID_PREAMBLE_PUNCTURE_CFG,
  PRM_ID_STATIONS_STATS,
  PRM_ID_AP_RETRY_LIMIT,
  PRM_ID_AP_EXCE_RETRY_LIMIT,

  PRM_ID_RTS_THRESHOLD,

  PRM_ID_CTS_TO_SELF_TO,

  PRM_ID_TX_AMPDU_DENSITY,
  PRM_ID_UNSOLICITED_FRAME_TX,
  PRM_ID_DYNAMIC_MC_RATE,
  PRM_ID_SCAN_PARAMS,
  PRM_ID_SCAN_PARAMS_BG,
  PRM_ID_DFS_DEBUG,
  PRM_ID_BSS_LOAD,
  PRM_ID_MESH_MODE,
  PRM_ID_SFTBLCK_DIS,
  PRM_ID_MBSSID_VAP,
  PRM_ID_VAPS_IN_GRP,
  PRM_ID_MGMT_RATE,
  PRM_ID_HE_BEACON,
  PRM_ID_PROBING_MASK,
  PRM_ID_SCAN_MODIFS,
  PRM_ID_SCAN_PAUSE_BG_CACHE,
  PRM_ID_ADVERTISE_BTWT_SCHEDULE,
  PRM_ID_TERMINATE_BTWT_SCHEDULE,
  PRM_ID_TX_TWT_TEARDOWN,
  PRM_ID_SB_TIMER_ACL,
  PRM_ID_FIXED_LTF_AND_GI,
  PRM_ID_WHM_CONFIG,
  PRM_ID_WHM_RESET,
  PRM_ID_WHM_TRIGGER,

#ifdef CONFIG_WAVE_DEBUG
  /* Select WLAN counters source: WAVEFW / DCDP / Auto */
  PRM_ID_SWITCH_COUNTERS_SRC,

  PRM_ID_UNCONNECTED_STA_SCAN_TIME,
  PRM_ID_FIXED_POWER,
  PRM_ID_IRE_CTRL_B,
  PRM_ID_CPU_DMA_LATENCY,
  PRM_ID_BEAMFORM_EXPLICIT,
  PRM_ID_TASKLET_LIMITS,
  PRM_ID_GENL_FAMILY_ID,
  PRM_ID_SCAN_EXP_TIME,
  /* Traffic Analyzer */
  PRM_ID_TA_TIMER_RESOLUTION,
  PRM_ID_TA_DBG,

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
  /* PCOC configuration */
  PRM_ID_PCOC_POWER_MODE,
  PRM_ID_PCOC_AUTO_PARAMS,
  PRM_ID_PCOC_PMCU_DEBUG,
#endif

  PRM_ID_WDS_HOST_TIMEOUT,
  PRM_ID_MAC_WATCHDOG_TIMEOUT_MS,
  PRM_ID_MAC_WATCHDOG_PERIOD_MS,
  PRM_ID_11H_NOP,
  PRM_ID_11H_BEACON_COUNT,
  PRM_ID_TEST_BUS,
  PRM_ID_DBG_CLI,
  PRM_ID_FW_DEBUG,
  PRM_ID_FW_LOG_SEVERITY,
  PRM_ID_DBG_CMD_FW,
  PRM_ID_MTLK_DBG_LVL,
#endif /* CONFIG_WAVE_DEBUG */

  PRM_ID_RADIO_LAST
};

/********************************************************************
 * Helper macro for df_user checking
 ********************************************************************/
void mtlk_df_user_assert(mtlk_slid_t caller_slid);

#define MTLK_CHECK_PTR_VOID(__ptr) { \
  if (!(__ptr)) {                     \
    mtlk_assert_log_s(MTLK_SLID, #__ptr); \
      return; \
  } \
}

#define MTLK_CHECK_PTR_RES(__ptr) { \
  if (!(__ptr)) {                     \
    mtlk_assert_log_s(MTLK_SLID, #__ptr); \
    return -EINVAL; \
  } \
}

#define MTLK_RET_CHECK_PTR(__ptr) { \
  if (!(__ptr)) {                     \
    mtlk_assert_log_s(MTLK_SLID, #__ptr); \
    return MTLK_ERR_UNKNOWN; \
  } \
}

#define MTLK_CHECK_DF_USER(__df_user) { \
    if (!(__df_user)) {                     \
        mtlk_df_user_assert(MTLK_SLID);     \
        return -EFAULT; \
    } \
}

#define MTLK_CHECK_DF_USER_NORES(__df_user) { \
    if (!(__df_user)) {                     \
        mtlk_df_user_assert(MTLK_SLID);     \
        return; \
    } \
}

#define MTLK_CHECK_DF(__df)       MTLK_CHECK_DF_USER(__df)
#define MTLK_CHECK_DF_NORES(__df) MTLK_CHECK_DF_USER_NORES(__df)

#define MTLK_RET_CHECK_DF(__df) { \
    if (!(__df)) {                     \
        mtlk_df_user_assert(MTLK_SLID);     \
        return MTLK_ERR_UNKNOWN; \
    } \
}

/*********************************************************************
 *               iw/iwpriv parameters
 *********************************************************************/

/* Maximal length of vector */
#define TEXT_SIZE               IW_PRIV_SIZE_MASK /* 2047 */
#define INTVEC_SIZE             256
#define ADDRVEC_SIZE            64

/* Type of item */
#define TEXT_ITEM               0
#define INT_ITEM                1
#define ADDR_ITEM               2

/* Size of item */
#define TEXT_ITEM_SIZE          (sizeof(char))
#define INT_ITEM_SIZE           (sizeof(int))
#define ADDR_ITEM_SIZE          (sizeof(struct sockaddr))

typedef uint16 mtlk_iwpriv_params_t;
#define MTLK_IW_PRIW_PARAM(type, length)    ((((mtlk_iwpriv_params_t)(length)) & 0x0FFF) | (((type) & 0x0f)<<12))
#define MTLK_IW_PRIV_TYPE(param)            (((param) >> 12) & 0x0f)
#define MTLK_IW_PRIV_LENGTH(param)          ((param) & 0x0fff)
#define MTLK_IW_PRIV_SIZE(param)            (mtlk_iw_priv_item_size(param) * MTLK_IW_PRIV_LENGTH(param))

static __INLINE
size_t mtlk_iw_priv_item_size(const mtlk_iwpriv_params_t param) {
  switch MTLK_IW_PRIV_TYPE(param) {
    case TEXT_ITEM : return TEXT_ITEM_SIZE;
    case INT_ITEM  : return INT_ITEM_SIZE;
    case ADDR_ITEM : return ADDR_ITEM_SIZE;
  }
  return 0;
}

static __INLINE
const char* mtlk_iw_priv_item_type(const mtlk_iwpriv_params_t param) {
  switch MTLK_IW_PRIV_TYPE(param) {
    case TEXT_ITEM : return "TEXT";
    case INT_ITEM  : return "INT";
    case ADDR_ITEM : return "ADDR";
  }
  return 0;
}

/*********************************************************************
 *               DF User management interface
 *********************************************************************/
mtlk_df_user_t*
mtlk_df_user_create(mtlk_df_t *df, const char *name, struct net_device *ndev_p);

void
mtlk_df_user_delete(mtlk_df_user_t* df_user);

int
mtlk_df_user_set_iftype(mtlk_df_user_t* df_user, enum nl80211_iftype iftype);

int
mtlk_df_user_start(mtlk_df_t *df, mtlk_df_user_t *df_user);

void
mtlk_df_user_stop(mtlk_df_user_t *df_user);

/* User-friendly interface/device name */
const char*
mtlk_df_user_get_name(mtlk_df_user_t *df_user);
const unsigned char*
mtlk_df_user_get_addr(mtlk_df_user_t *df_user);
uint32
mtlk_df_user_get_flags(mtlk_df_user_t *df_user);
struct net_device *
mtlk_df_user_get_ndev(mtlk_df_user_t *df_user);
void
mtlk_df_user_set_ndev(mtlk_df_user_t *df_user, struct net_device *ndev);
void
mtlk_df_user_set_backhaul_sta(mtlk_df_user_t *df_user, sta_entry *backhaul_sta);
sta_entry *
mtlk_df_user_get_backhaul_sta(mtlk_df_user_t *df_user);
BOOL
mtlk_df_user_backhaul_sta_is_set(mtlk_df_user_t *df_user);
BOOL
mtlk_df_user_is_equal_backhaul_sta(mtlk_df_user_t *df_user, sta_entry * sta);
void
mtlk_secondary_df_user_set_vap_idx(mtlk_df_user_t *df_user, uint8 vap_index);
uint8
mtlk_secondary_df_user_get_vap_idx(mtlk_df_user_t *df_user);
void
mtlk_secondary_df_user_set_mac_addr(mtlk_df_user_t *df_user,
                                    const unsigned char *addr);
BOOL
mtlk_df_user_is_secondary(mtlk_df_user_t *df_user);

struct wireless_dev *
mtlk_df_user_get_wdev(mtlk_df_user_t *df_user);
mtlk_df_user_t *
mtlk_df_user_ndev_to_df_user (struct net_device *ndev);
int
mtlk_df_user_set_flags(mtlk_df_user_t *df_user, uint32 newflags);

mtlk_df_t *
mtlk_df_user_get_df(mtlk_df_user_t *df_user);
mtlk_df_t *
mtlk_df_user_get_master_df (mtlk_df_user_t *df_user);
void
mtlk_df_user_set_mngmnt_filter(mtlk_df_user_t *df_user, unsigned subtype, BOOL reg);
BOOL
mtlk_df_user_get_mngmnt_filter(mtlk_df_user_t *df_user, unsigned subtype);

void
mtlk_df_user_stats_timer_set(mtlk_df_user_t *df_user, uint32 value);

void mtlk_df_stop_stat_timers (mtlk_df_user_t* df_user);
void mtlk_df_resume_stat_timers (mtlk_df_user_t* df_user);

/*********************************************************************
 * IW/IWPRIV config setter/getter
 *********************************************************************/
int __MTLK_IFUNC
mtlk_df_ui_set_cfg (mtlk_df_user_t* df_user, uint32 param_id, char* data,
    mtlk_iwpriv_params_t params);

int __MTLK_IFUNC
mtlk_df_ui_get_cfg (mtlk_df_user_t* df_user, uint32 param_id, char* data,
    mtlk_iwpriv_params_t params, uint16* length);

static __INLINE mtlk_df_user_t *
mtlk_df_user_from_wdev (struct wireless_dev *wdev)
{
  return mtlk_df_user_ndev_to_df_user(wdev->netdev);
}

static __INLINE mtlk_df_user_t *
mtlk_df_user_from_ndev (struct net_device *ndev)
{
  return mtlk_df_user_ndev_to_df_user(ndev);
}

int __MTLK_IFUNC
_mtlk_df_user_iwpriv_get_core_param(mtlk_df_user_t* df_user, uint32 param_id, char* data, uint16* length);

int __MTLK_IFUNC
mtlk_df_user_data_tx(mtlk_df_user_t* df_user, struct sk_buff *skb);
void __MTLK_IFUNC mtlk_df_user_get_stats(mtlk_df_user_t *df_user,
                                         struct rtnl_link_stats64 *stats);

int __MTLK_IFUNC
mtlk_df_ui_validate_wep_key (const uint8 *key, size_t length);

int __MTLK_IFUNC
_mtlk_df_mtlk_to_linux_error_code(int mtlk_res);

void __MTLK_IFUNC
mtlk_df_user_group_init (mtlk_df_user_t* df_user);

void __MTLK_IFUNC
mtlk_df_user_group_uninit (mtlk_df_user_t* df_user);

int __MTLK_IFUNC
mtlk_df_mcast_notify_sta_connected (mtlk_df_t *df);

#if MTLK_USE_DIRECTCONNECT_DP_API
BOOL __MTLK_IFUNC
mtlk_df_user_litepath_is_available (mtlk_df_user_t* df_user);

BOOL __MTLK_IFUNC
mtlk_df_user_litepath_is_registered (mtlk_df_user_t* df_user);

/***********************************************************************/
/*!
 * \brief Transmit packet on an interface using LitePath
 * \param[in]  rx_dev - RX net device
 * \param[in]  tx_dev - TX net device
 * \param[in]  skb - pointer to sk_buff of the packet
 * \param[in]  len Length of the packet
 * \return Linux system error code
 */
int __MTLK_IFUNC
mtlk_df_user_litepath_start_xmit (struct net_device *rx_dev,
                              struct net_device *tx_dev,
                              struct sk_buff *skb,
                              int len);

int __MTLK_IFUNC
mtlk_df_ui_dp_disconnect_mac (mtlk_df_t *df, const uint8 *mac_addr);
#ifdef WAVE_DCDP_DGAF_SUPPORTED
/* Update DC DP DGAF flag */
mtlk_error_t __MTLK_IFUNC
mtlk_df_ui_dp_dgaf_disable(mtlk_df_t *df, BOOL disabled);
#endif

int __MTLK_IFUNC
mtlk_df_ui_dp_prepare_and_xmit (mtlk_vap_handle_t vap_handle, mtlk_core_handle_tx_data_t *data,
    BOOL wds, uint16 sta_sid, int mc_index, uint32 nbuf_flags);

void __MTLK_IFUNC
mtlk_df_dcdp_release_buflist(mtlk_dcdp_dev_t *dp_dev);
void __MTLK_IFUNC
mtlk_df_dcdp_datapath_dev_init (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev);
int __MTLK_IFUNC
mtlk_df_dcdp_datapath_dev_register (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev, mtlk_dcdp_init_info_t *dp_init /*, BOOL is_recovery */);
int __MTLK_IFUNC
mtlk_df_dcdp_datapath_dev_unregister (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev, BOOL is_recovery);
int __MTLK_IFUNC
mtlk_df_dcdp_handle_rx_ring_sw (mtlk_dcdp_dev_t *dp_dev);
int __MTLK_IFUNC
mtlk_df_dcdp_handle_txout_ring_sw (mtlk_dcdp_dev_t *dp_dev);
int __MTLK_IFUNC
mtlk_df_dcdp_handle_frag_wa_ring (mtlk_dcdp_dev_t *dp_dev);

int __MTLK_IFUNC
mtlk_df_user_set_priority_to_qos (mtlk_df_user_t* df_user, uint8 *dscp_table);
#endif

int  __MTLK_IFUNC    mtlk_df_ui_phy_rx_status_create(mtlk_df_user_t *df_user, unsigned size);
void __MTLK_IFUNC    mtlk_df_ui_phy_rx_status_delete(mtlk_df_user_t *df_user);

#undef  LOG_LOCAL_GID
#undef  LOG_LOCAL_FID

#endif /*__DF_USER_PRIVATE_H__*/
