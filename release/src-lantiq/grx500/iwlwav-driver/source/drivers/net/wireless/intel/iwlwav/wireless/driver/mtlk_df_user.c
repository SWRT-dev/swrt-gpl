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
/*! \file       mtlk_df_user.c
 * \brief       Driver framework implementation for Linux
 *
 */

#include "mtlkinc.h"
#include "mtlk_clipboard.h"
#include "mtlk_coreui.h"
#include "mtlk_df_priv.h"
#include "core.h"
#include "core_config.h"
#include "mtlk_snprintf.h"
#include "mtlk_df.h"
#include "mtlk_df_nbuf.h"
#include "mtlk_hs20.h"
#include "mac80211.h"
#include "mhi_umi.h"
#include "cfg80211.h"

#include <linux/module.h>
#include <linux/wireless.h>
#include <linux/if_arp.h>
#include <linux/inet.h>
#include <net/iw_handler.h>
#include <asm/unaligned.h>
#include <net/cfg80211.h>
#include <net/mac80211.h>

#if MTLK_USE_PUMA6_UDMA
#include <linux/udma_api.h> /* PUMA6 UDMA API*/
#include <linux/if_vlan.h>
#ifndef MTLK_UDMA_REGISTER
#define MTLK_UDMA_REGISTER       0   /* don't register at start */
#endif
/* #define UDMA_DEBUG_PRINT */ /* Enable printouts for UDMA debugging */
#endif

#if MTLK_USE_DIRECTCONNECT_DP_API
#ifndef MTLK_LITEPATH_REGISTER
#define MTLK_LITEPATH_REGISTER   0   /* don't register at start */
#endif
#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

#include "dataex.h"

#include "mtlkaux.h"
#include "mtlkwlanirbdefs.h"
#include "mtlk_packets.h"

#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog.h"
#endif

#define LOG_LOCAL_GID   GID_DFUSER
#define LOG_LOCAL_FID   1

#define DF_USER_DEFAULT_IWPRIV_LIM_VALUE (-1)

#define MTLK_NIP6(addr) \
        ntohs((addr).s6_addr16[0]), \
        ntohs((addr).s6_addr16[1]), \
        ntohs((addr).s6_addr16[2]), \
        ntohs((addr).s6_addr16[3]), \
        ntohs((addr).s6_addr16[4]), \
        ntohs((addr).s6_addr16[5]), \
        ntohs((addr).s6_addr16[6]), \
        ntohs((addr).s6_addr16[7])

#define MTLK_NIP6_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"

#define MTLK_NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define MTLK_NIPQUAD_FMT "%u.%u.%u.%u"

/* Debug */
#ifdef MTLK_DEBUG
    /* A semicolon ';' at the end of line is required for proper LOG macro generation */
    /* Format string is also required, even an empty one at the end of the 1st param */
    #define TRACE_PARAM_NOTE(str)       do { ILOG1_V(str ""); } while (0)
    #define TRACE_PARAM_INT(value)      do { ILOG1_SD("%s: 0x%08X", #value, (uint32)(value));                } while (0)
    #define TRACE_PARAM_PTR(value)      do { ILOG1_SP("%s: 0x%p",   #value, (value));                        } while (0)
    #define TRACE_PARAM_STR(value)      do { ILOG1_SS("%s: %s",     #value, (value));                        } while (0)
    #define TRACE_PARAM_MAC16(value)    do { ILOG1_SD("%s: 0x%04X", #value, MAC_TO_HOST16((uint16)(value))); } while (0)
    #define TRACE_PARAM_MAC32(value)    do { ILOG1_SD("%s: 0x%08X", #value, MAC_TO_HOST32((uint32)(value))); } while (0)
#else
    #define TRACE_PARAM_NOTE(str)
    #define TRACE_PARAM_INT(value)
    #define TRACE_PARAM_PTR(value)
    #define TRACE_PARAM_STR(value)
    #define TRACE_PARAM_MAC16(value)
    #define TRACE_PARAM_MAC32(value)
#endif

#define WAVE_CHECK_AND_SET_INTVEC_LENGTH(max, needed, out_length) {       \
  MTLK_ASSERT(max >= needed);                                             \
  if(__UNLIKELY(max < needed)) {                                          \
    out_length = 0;                                                       \
    return;                                                               \
  }                                                                       \
  else {                                                                  \
    out_length = needed;                                                  \
  }                                                                       \
}

#define IS_VALID_RANGE(val, min, max) (((int32)val >= min) && ((int32)val <= max))

static const IEEE_ADDR EMPTY_MAC_MASK = { {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

#ifdef CONFIG_WAVE_DEBUG
/*********************************************************************
 * IOCTL handlers used by BCL only
 *********************************************************************/
static int __MTLK_IFUNC
mtlk_df_ui_iw_bcl_mac_data_get (struct net_device *ndev,
                                struct iw_request_info *info,
                                union iwreq_data *wrqu,
                                char *extra);


static int __MTLK_IFUNC
mtlk_df_ui_iw_bcl_mac_data_set (struct net_device *ndev,
                                struct iw_request_info *info,
                                union iwreq_data *wrqu, char *extra);

static int __MTLK_IFUNC
mtlk_df_ui_linux_ioctl_bcl_drv_data_exchange (struct net_device *ndev,
                                              struct iw_request_info *info,
                                              union iwreq_data *wrqu, char *extra);

/********************************************************************
 * IW private BCL only IOCTL handlers table
 ********************************************************************/
static const iw_handler mtlk_linux_bcl_only_private_handler[] = {
  [20] = mtlk_df_ui_iw_bcl_mac_data_get,
  [21] = mtlk_df_ui_iw_bcl_mac_data_set,
  [22] = mtlk_df_ui_linux_ioctl_bcl_drv_data_exchange,
};

static struct iw_handler_def wave_linux_iw_core_handler_bcl_only = {
  .num_standard = 0,
  .num_private = ARRAY_SIZE(mtlk_linux_bcl_only_private_handler),
  .num_private_args = 0,
  .standard = NULL,
  .private = (iw_handler *) mtlk_linux_bcl_only_private_handler,
  .private_args = NULL,
  .get_wireless_stats = NULL,
};
#endif /* CONFIG_WAVE_DEBUG */

#define MAX_DF_UI_STAT_NAME_LENGTH 256

/* slow context of DF user */
struct _mtlk_df_ui_slow_ctx_t
{
  /* Core status & statistic */
  mtlk_core_general_stats_t core_general_stats;

  struct iw_statistics    iw_stats;

  /* Network device statistics */
  struct rtnl_link_stats64 linux_stats;

  /**** BCL data *****/
#ifdef CONFIG_WAVE_DEBUG
  uint32      *dbg_general_pkt_cnts;
  uint32      dbg_general_pkt_cnts_num;
  IEEE_ADDR   *dbg_general_pkt_addr;
  uint32      dbg_general_pkt_addr_num;
  uint32      *dbg_rr_cnts;
  uint32      dbg_rr_cnts_num;
  IEEE_ADDR   *dbg_rr_addr;
  uint32      dbg_rr_addr_num;
#endif
  /*******************/

  mtlk_osal_timer_t       stat_timer;

  mtlk_df_proc_fs_node_t  *proc_df_node;
  mtlk_df_proc_fs_node_t  *proc_df_debug_node;
#ifdef MTLK_PER_RATE_STAT
  mtlk_df_proc_fs_node_t *proc_per_rate_stats_node;
  mtlk_df_proc_fs_node_t *proc_per_rate_stats_tx_node;
  mtlk_df_proc_fs_node_t *proc_per_rate_stats_rx_node;
  mtlk_df_proc_fs_node_t *proc_per_rate_stats_packet_error_rate_node;
#endif
  mtlk_df_proc_fs_node_t  *proc_df_fw_node;

};

typedef struct _mtlk_df_user_litepath_stats_t
{
  uint32 tx_processed;
  uint32 tx_forward;
  uint32 tx_forward_fail;
  uint32 tx_discarded_no_peers;
  uint32 tx_dropped;
  uint32 tx_sent_up;
  uint32 rx_accepted;
  uint32 rx_rejected;
} mtlk_df_user_litepath_stats_t;

#if MTLK_USE_DIRECTCONNECT_DP_API
typedef enum _mtlk_df_user_dcdp_mode_t {
   DC_DP_MODE_UNREGISTERED = 0x00,
   DC_DP_MODE_SWPATH       = 0x01,
   DC_DP_MODE_FASTPATH     = 0x02,
   DC_DP_MODE_LITEPATH     = 0x04,
   DC_DP_MODE_LITEPATH_QOS = 0x08,
   DC_DP_MODE_INTERNAL_SWPATH_ONLY = 0x10,
} mtlk_df_user_dcdp_mode_t;

typedef struct _mtlk_df_user_dcdp_t {
    mtlk_df_user_dcdp_mode_t      mode;  /* Current mode of subif */
    BOOL                          mcast_registered;
    struct dp_subif               subif_id;
    mtlk_df_user_litepath_stats_t litepath_stats;
} mtlk_df_user_dcdp_t;
#endif /* MTLK_USE_DIRECTCONNECT_DP_API */


#if MTLK_USE_PUMA6_UDMA
typedef uint16 mtlk_vlan_id_t;

typedef struct _mtlk_df_user_udma_stats_t
{
  unsigned tx_processed;
  unsigned tx_rejected_no_vlan_tag;
  unsigned tx_rejected_no_vlan_registered;
  unsigned rx_accepted;
  unsigned rx_rejected;
  unsigned rx_tagging_error;
} mtlk_df_user_udma_stats_t;

typedef enum _mtlk_df_user_udma_flags {
  MTLK_UDMA_REGISTERED  = 0x01,
  MTLK_UDMA_ACTIVE      = 0x02,
} mtlk_df_user_udma_flags;

typedef struct _mtlk_df_user_udma_t
{
  rx_callback_t             rx_fn;
  int32                     port_id;
  mtlk_vlan_id_t            vlan_id;
  unsigned                  start_vap_id;
  mtlk_df_user_t           *df_master_user;
  mtlk_df_user_udma_stats_t stats;
  BOOL                      is_master;
  volatile mtlk_df_user_udma_flags flags;
  volatile unsigned         users;
} mtlk_df_user_udma_t;
#endif /* MTLK_USE_PUMA6_UDMA */

/* Flags for field "destructor_flags" */
#define WAVE_DESTRUCT_NETDEV_DESTRUCTOR_CALLED  0x01
#define WAVE_DESTRUCT_DFUSER_DELETE_CALLED      0x02

struct _mtlk_df_user_t
{
  struct _mtlk_df_t *df;
  struct wireless_dev wdev;
  struct net_device *ndev;
  /* support for many bSTAs under a single FW/HW Vap */
  BOOL               ndev_registered;
  BOOL               is_secondary_df;
  uint32             secondary_vap_id;
  sta_entry         *backhaul_sta;

  char   name[IFNAMSIZ];
  BOOL   frame_registered[FRAME_SUBTYPE_SIZE];
  unsigned  destructor_flags;

#if MTLK_USE_DIRECTCONNECT_DP_API
  mtlk_df_user_dcdp_t dcdp;
#endif
#if MTLK_USE_PUMA6_UDMA
  mtlk_df_user_udma_t udma;
#endif

  struct net_device_ops dev_ops;
  struct _mtlk_df_ui_slow_ctx_t *slow_ctx;

  BOOL                 hs20_enabled;
  mtlk_hs20_notify_entry_t hs20_notify_entry;

  MTLK_DECLARE_INIT_LOOP(PROC_INIT);
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
};

/********************************************************************
 * Tools API definitions
 ********************************************************************/

/*****************************************************************************
 *****************************************************************************/

void __MTLK_IFUNC mtlk_df_user_assert (mtlk_slid_t caller_slid)
{
    /* Can't use macros inside format string due to logprep limitations */
    /* Instead, the exact definition of the MTLK_SLID_FMT macro is used */
    ELOG_DDDD("ERROR, cannot to get private data " "[SLID:%u:%u:%u:%u]",
        mtlk_slid_get_oid(caller_slid),
        mtlk_slid_get_gid(caller_slid),
        mtlk_slid_get_fid(caller_slid),
        mtlk_slid_get_lid(caller_slid)
        );
    MTLK_ASSERT(FALSE);
}

/*****************************************************************************/

static __INLINE uint32
_mtlk_df_user_get_core_slave_vap_index_by_iwpriv_param (uint32 iwpriv_slave_vap_index)
{
  /* DF iwpriv commands use 0-based VAP index for Slave VAPs, while Core
   * uses 0-based numeration for *all* the VAPs, including the Master.
   * This function translates the DFs Slave VAP Index to a Cores one.
   */
  return (iwpriv_slave_vap_index + 1);
}

int __MTLK_IFUNC
_mtlk_df_mtlk_to_linux_error_code(int mtlk_res)
{
  int linux_res;

  switch (mtlk_res) {

    case MTLK_ERR_OK:
      linux_res = 0;
      break;

    case MTLK_ERR_PARAMS:
    case MTLK_ERR_VALUE:
      linux_res = -EINVAL;
      break;

    case MTLK_ERR_NO_MEM:
      linux_res = -ENOMEM;
      break;

    case MTLK_ERR_PENDING:
      linux_res = -EINPROGRESS;
      break;

    case MTLK_ERR_BUSY:
      linux_res = -EBUSY;
      break;

    case MTLK_ERR_EEPROM:
    case MTLK_ERR_HW:
    case MTLK_ERR_FW:
    case MTLK_ERR_UMI:
    case MTLK_ERR_MAC:
      linux_res = -EFAULT;
      break;

    case MTLK_ERR_TIMEOUT:
      linux_res = -ETIME;
      break;

    case MTLK_ERR_NOT_READY:
      linux_res = -EAGAIN;
      break;

    case MTLK_ERR_NOT_SUPPORTED:
      linux_res = -EOPNOTSUPP;
      break;

    case MTLK_ERR_NOT_IN_USE:
    case MTLK_ERR_NO_RESOURCES:
    case MTLK_ERR_SCAN_FAILED:
    case MTLK_ERR_AOCS_FAILED:
    case MTLK_ERR_PROHIB:
    case MTLK_ERR_BUF_TOO_SMALL:
    case MTLK_ERR_PKT_DROPPED:
    case MTLK_ERR_DC_DP_XMIT:
    case MTLK_ERR_FILEOP:
    case MTLK_ERR_CANCELED:
    case MTLK_ERR_NOT_HANDLED:
    case MTLK_ERR_UNKNOWN:
      linux_res = -EFAULT;
      break;

    case MTLK_ERR_DATA_TOO_BIG:
      linux_res = -E2BIG;
      break;

    case MTLK_ERR_WRONG_CONTEXT:
      linux_res = -EILSEQ;
      break;

    case MTLK_ERR_NO_ENTRY:
      linux_res = -ENODATA;
      break;

    default :
      linux_res = -EFAULT;
      break;
  }

  return linux_res;
}

/* User-friendly interface/device name */
const char*
mtlk_df_user_get_name(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);
  return df_user->name;
}

const unsigned char *
mtlk_df_user_get_addr(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(NULL != df_user->ndev);
  return df_user->ndev->dev_addr;
}

uint32
mtlk_df_user_get_flags (mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(NULL != df_user->ndev);

  return df_user->ndev->flags;
}

struct net_device *
mtlk_df_user_get_ndev(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);

  return df_user->ndev;
}

void
mtlk_df_user_set_ndev(mtlk_df_user_t *df_user, struct net_device *ndev) {
  MTLK_ASSERT(NULL != df_user);

  df_user->ndev = ndev;
}

void
mtlk_df_user_set_backhaul_sta(mtlk_df_user_t *df_user, sta_entry *backhaul_sta)
{
  MTLK_ASSERT(NULL != df_user);
  if (backhaul_sta) {
    if (df_user->backhaul_sta) {
        ELOG_V("backhaul_sta is already set");
        mtlk_sta_decref(df_user->backhaul_sta);
    }
    df_user->backhaul_sta = backhaul_sta;
    mtlk_sta_incref(backhaul_sta);
  }
  else {
    if (df_user->backhaul_sta)
      mtlk_sta_decref(df_user->backhaul_sta);
    else
      ELOG_V("backhaul_sta is not set");
    df_user->backhaul_sta = backhaul_sta;
  }

}

sta_entry *
mtlk_df_user_get_backhaul_sta(mtlk_df_user_t *df_user)
{
  sta_entry * sta;
  MTLK_ASSERT(NULL != df_user);

  sta = df_user->backhaul_sta;
  if (sta)
    mtlk_sta_incref(sta);

  return sta;
}

BOOL
mtlk_df_user_backhaul_sta_is_set(mtlk_df_user_t *df_user)
{
  return (df_user->backhaul_sta != NULL);
}

BOOL
mtlk_df_user_is_equal_backhaul_sta(mtlk_df_user_t *df_user, sta_entry * sta)
{
  return (df_user->backhaul_sta == sta);
}

void
mtlk_secondary_df_user_set_vap_idx(mtlk_df_user_t *df_user, uint8 vap_index)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(df_user->is_secondary_df);

  df_user->secondary_vap_id = vap_index;
}

uint8
mtlk_secondary_df_user_get_vap_idx(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(df_user->is_secondary_df);

  return df_user->secondary_vap_id;
}

void
mtlk_secondary_df_user_set_mac_addr(mtlk_df_user_t *df_user,
                                    const unsigned char *addr)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(df_user->is_secondary_df);
  MTLK_ASSERT(df_user->ndev_registered);

  mtlk_osal_copy_eth_addresses(df_user->ndev->dev_addr, addr);
}

BOOL
mtlk_df_user_is_secondary(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);

  return df_user->is_secondary_df;
}

mtlk_df_user_t *
mtlk_df_user_dfuser_from_skb(struct sk_buff *skb)
{
  mtlk_df_user_t *df_user;
  struct wireless_dev *wdev;
  struct net_device *ndev;

  MTLK_ASSERT(NULL != skb);
  ndev = skb->dev;
  MTLK_ASSERT(NULL != ndev);

  wdev = (struct wireless_dev *)ndev->ieee80211_ptr;
  MTLK_ASSERT(NULL != wdev);
  df_user = mtlk_df_user_from_wdev(wdev);
  MTLK_ASSERT(NULL != df_user);
  return df_user;
}

struct wireless_dev *
mtlk_df_user_get_wdev(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);

  return df_user->ndev->ieee80211_ptr;
};

static int
_mtlk_secondary_df_user_iface_open(struct net_device *ndev)
{
  mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(ndev);

  MTLK_CHECK_DF_USER(df_user);
  ILOG1_SSD("%s: Invoked from %s (%i)", ndev->name, current->comm, current->pid);

  return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_OK);
}

static int
_mtlk_secondary_df_user_iface_stop(struct net_device *ndev)
{
  mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(ndev);

  MTLK_CHECK_DF_USER(df_user);
  ILOG1_SSD("%s: Invoked from %s (%i)", ndev->name, current->comm, current->pid);

  return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_OK);
}

static int
_mtlk_secondary_df_user_set_mac_addr(struct net_device *ndev, void* p)
{
  mtlk_df_user_t  *df_user = mtlk_df_user_from_ndev(ndev);
  struct sockaddr *addr = (struct sockaddr *)p;
  int              res = MTLK_ERR_OK;

  MTLK_CHECK_DF_USER(df_user);

  /* Validate address family */
  if ((addr->sa_family != ARPHRD_IEEE80211) && (addr->sa_family != ARPHRD_ETHER)) {
    WLOG_S("%s: Can't set MAC address - invalid sa_family", ndev->name);
    res = MTLK_ERR_PARAMS;
    goto finish;
  }

  mtlk_secondary_df_user_set_mac_addr(df_user, addr->sa_data);

finish:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

int _mtlk_df_user_linux_start_tx (struct sk_buff *skb, struct net_device *ndev);

static const struct net_device_ops sub_netdev_ops = {
  .ndo_start_xmit      = _mtlk_df_user_linux_start_tx,
  .ndo_open            = _mtlk_secondary_df_user_iface_open,
  .ndo_stop            = _mtlk_secondary_df_user_iface_stop,
  .ndo_set_mac_address = _mtlk_secondary_df_user_set_mac_addr,
};

static void _mtlk_secondary_df_user_ndev_setup(struct net_device *dev)
{
  ether_setup(dev);
  dev->netdev_ops = &sub_netdev_ops;
  dev->ieee80211_ptr = NULL;
  netdev_set_def_destructor(dev);

#if LINUX_VERSION_IS_GEQ(4,3,0)
  dev->priv_flags |= IFF_NO_QUEUE;
  dev->tx_queue_len = 0;
#endif
}

static __INLINE mtlk_df_user_t *
_mtlk_df_user_internal_ndev_to_df_user (struct net_device *ndev)
{
  MTLK_ASSERT(NULL != ndev);
  return *(mtlk_df_user_p*)netdev_priv(ndev);
}

mtlk_df_user_t *
mtlk_df_user_ndev_to_df_user(struct net_device *ndev)
{
  if (__UNLIKELY(!ndev))
    return NULL;

  if (ndev->ieee80211_ptr == NULL)
    /* ndev priv is df_user ptr */
    return _mtlk_df_user_internal_ndev_to_df_user(ndev);
  else
    /* ndev priv is mac80211 sdata */
    return wv_ieee80211_ndev_to_dfuser(ndev);
}

void
mtlk_df_user_set_mngmnt_filter(mtlk_df_user_t *df_user, unsigned subtype, BOOL reg)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(FRAME_SUBTYPE_SIZE > subtype);

  df_user->frame_registered[subtype] = reg;
}

BOOL
mtlk_df_user_get_mngmnt_filter(mtlk_df_user_t *df_user, unsigned subtype)
{
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(FRAME_SUBTYPE_SIZE > subtype);

  return df_user->frame_registered[subtype];
}

int
mtlk_df_user_set_flags(mtlk_df_user_t *df_user, uint32 newflags)
{
  int res;
  MTLK_ASSERT(NULL != df_user);
  MTLK_ASSERT(NULL != df_user->ndev);

  rtnl_lock();
#if LINUX_VERSION_IS_GEQ(5,0,0)
  res = dev_change_flags(df_user->ndev, newflags,NULL);
#else
  res = dev_change_flags(df_user->ndev, newflags);
#endif
  rtnl_unlock();

  return (res < 0) ? MTLK_ERR_UNKNOWN : MTLK_ERR_OK;
}

mtlk_df_t *
mtlk_df_user_get_df(mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);
  return df_user->df;
}

mtlk_df_t *
mtlk_df_user_get_master_df (mtlk_df_user_t *df_user)
{
  MTLK_ASSERT(NULL != df_user);
  return mtlk_vap_manager_get_master_df(mtlk_vap_get_manager(mtlk_df_get_vap_handle(df_user->df)));
}

BOOL __MTLK_IFUNC
mtlk_df_ui_is_promiscuous(mtlk_df_t *df)
{
  MTLK_ASSERT(NULL != df);
  return mtlk_df_get_user(df)->ndev->promiscuity ? TRUE : FALSE;
}

void __MTLK_IFUNC
mtlk_df_ui_notify_tx_start(mtlk_df_t *df)
{
  MTLK_ASSERT(NULL != df);
  netif_trans_update(mtlk_df_get_user(df)->ndev);
}

void mtlk_df_user_stats_timer_set (mtlk_df_user_t *df_user, uint32 value)
{
  mtlk_osal_timer_set(&df_user->slow_ctx->stat_timer, value);
}

MTLK_INIT_STEPS_LIST_BEGIN(df_user)
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, ALLOC_SLOW_CTX)
#ifdef CONFIG_WAVE_DEBUG
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, DEBUG_BCL)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, CREATE_CARD_DIR)
#ifdef CONFIG_WAVE_DEBUG
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, CREATE_DEBUG_DIR)
#endif
#ifdef MTLK_PER_RATE_STAT
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, CREATE_PER_RATE_STAT_DIR)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, STAT_TIMER)
#if MTLK_USE_PUMA6_UDMA
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, UDMA_INIT)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, PRE_PROC_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(df_user, PROC_INIT)
MTLK_INIT_INNER_STEPS_BEGIN(df_user)
MTLK_INIT_STEPS_LIST_END(df_user);

#if MTLK_USE_DIRECTCONNECT_DP_API
static __INLINE BOOL
__mtlk_df_user_litepath_is_registered (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(df_user != NULL);
  return ((DC_DP_MODE_LITEPATH & df_user->dcdp.mode) != 0);
}

BOOL __MTLK_IFUNC
mtlk_df_user_litepath_is_registered (mtlk_df_user_t* df_user)
{
  return __mtlk_df_user_litepath_is_registered(df_user);
}
#endif

static __INLINE int
_mtlk_df_user_start_skb_tx (struct sk_buff *skb, mtlk_df_user_t *df_user)
{
#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
  mtlk_nbuf_priv_init(mtlk_nbuf_priv(skb));
#endif
  mtlk_nbuf_start_tracking(skb);
  mtlk_core_handle_tx_data(mtlk_df_get_vap_handle(df_user->df), skb);
  return NETDEV_TX_OK;
}

int __MTLK_IFUNC
_mtlk_df_user_start_tx (struct sk_buff *skb, struct net_device *ndev)
{
  mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(skb->dev);
  if (NULL == df_user) {
    dev_kfree_skb_any(skb);
    return NETDEV_TX_OK;
  }

  return _mtlk_df_user_start_skb_tx(skb, df_user);
}
/* Public: to be called from wave_wifi_l2f.ko */
EXPORT_SYMBOL(_mtlk_df_user_start_tx);

/* Ensure that we got a packet from the correct network device */
static __INLINE BOOL
_mtlk_validate_wlan_netdev (struct net_device *netdev)
{
#ifdef CPTCFG_IWLWAV_VALIDATE_IFNAME
  char buf[IFNAMSIZ+1];
#endif /* CPTCFG_IWLWAV_VALIDATE_IFNAME */
  if(__UNLIKELY(NULL == netdev))
    return FALSE;
#ifdef CPTCFG_IWLWAV_VALIDATE_IFNAME
  if (sizeof(uint32) == sizeof(MTLK_NDEV_NAME)-1) {
    /* Optimized version */
    if (*(uint32*)netdev->name == *(uint32*)MTLK_NDEV_NAME)
      return TRUE;
  }
  else if (memcmp(netdev->name, MTLK_NDEV_NAME, sizeof(MTLK_NDEV_NAME)-1) == 0)
    return TRUE;

  /* Try not to crash if name is not null-terminated */
  wave_strcopy(buf, netdev->name, sizeof(buf));
  ILOG0_S("Got a packet from a wrong network device: \"%s\"", buf);
  return FALSE;
#else
  return TRUE;
#endif
}

int __MTLK_IFUNC
_mtlk_df_user_linux_start_tx (struct sk_buff *skb, struct net_device *ndev)
{
  mtlk_df_user_t *df_user;

  if (__UNLIKELY(!_mtlk_validate_wlan_netdev(ndev))) {
    dev_kfree_skb_any(skb);
    return NETDEV_TX_OK;
  }

  df_user = mtlk_df_user_from_ndev(ndev);

  return mtlk_df_user_data_tx(df_user, skb);
}
EXPORT_SYMBOL(_mtlk_df_user_linux_start_tx);

int __MTLK_IFUNC
mtlk_df_user_data_tx(mtlk_df_user_t* df_user, struct sk_buff *skb)
{
  if (__UNLIKELY(NULL == df_user)) {
    dev_kfree_skb_any(skb);
    skb = NULL;
    return NETDEV_TX_OK;
  }

#if MTLK_USE_DIRECTCONNECT_DP_API
  if (__mtlk_df_user_litepath_is_registered(df_user) && (df_user->dcdp.mode & DC_DP_MODE_LITEPATH_QOS)){
    int res;
    mtlk_core_t *core = mtlk_vap_get_core(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user)));

    if (__LIKELY(mtlk_core_has_connections(core))) {
      /* send packets to the LitePath QoS */
      res = dc_dp_xmit(NULL, NULL, &df_user->dcdp.subif_id, skb, skb->len, DC_DP_F_XMIT_LITEPATH);
      switch (res) {
        case DC_DP_SUCCESS:
          ++df_user->dcdp.litepath_stats.tx_forward;
          return NETDEV_TX_OK;

        case DC_DP_FAILURE:
          ++df_user->dcdp.litepath_stats.tx_forward_fail;
          return NETDEV_TX_OK;

        default:
          /* Value -2: LitePath QoS is not supported. Reset QoS flag and continue usual tx flow */
          df_user->dcdp.mode &= ~DC_DP_MODE_LITEPATH_QOS;
          break;
      }
    }
    else
      ++df_user->dcdp.litepath_stats.tx_discarded_no_peers;
  }
#endif
  return _mtlk_df_user_start_skb_tx(skb, df_user);
}


/********************************************************************
 * DirectConnect DP supporting functionality BEGIN
 ********************************************************************/
#if MTLK_USE_DIRECTCONNECT_DP_API
uint32 _mtlk_df_user_litepath_tx_dropped = 0;

static __INLINE BOOL
__mtlk_df_user_litepath_is_available (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(df_user != NULL);
  return (((DC_DP_MODE_FASTPATH | DC_DP_MODE_INTERNAL_SWPATH_ONLY) & df_user->dcdp.mode) == 0);
}

BOOL __MTLK_IFUNC
mtlk_df_user_litepath_is_available (mtlk_df_user_t* df_user)
{
  return __mtlk_df_user_litepath_is_available(df_user);
}

static int
_mtlk_df_user_litepath_tx (struct net_device *tx_dev, struct sk_buff *skb, int len)
{
    mtlk_df_user_t* df_user;
    MTLK_ASSERT(tx_dev);

    df_user = mtlk_df_user_from_ndev(tx_dev);
    if (NULL == df_user) {
      dev_kfree_skb_any(skb);
      return -1; /* skb is consumed anyway */
    }

    MTLK_ASSERT(df_user->dcdp.mode & (DC_DP_MODE_SWPATH|DC_DP_MODE_LITEPATH));

    ++df_user->dcdp.litepath_stats.tx_processed;
    if (DC_DP_MODE_LITEPATH == (df_user->dcdp.mode & (DC_DP_MODE_SWPATH|DC_DP_MODE_LITEPATH))) {
      /* The LitePath only is supported by DirectConnect.
       * SW path must be processed by wlan driver */
      skb->dev = tx_dev;
      return (NETDEV_TX_OK == _mtlk_df_user_start_skb_tx(skb, df_user)) ? 0 : -1;
    } else {
      /* Both LitePath + SW are supported by DirectConnect.
       * CB function returns 1: skb was not consumed by driver; DC will take care about
       * correct processing and delivery */
      return 1;
    }
}

static int
_mtlk_df_user_litepath_rx (struct net_device *rx_dev, struct sk_buff *skb, int len)
{
    mtlk_df_user_t* df_user;
    mtlk_core_t* nic;
    MTLK_ASSERT(rx_dev);

    df_user = mtlk_df_user_from_ndev(rx_dev);
    if (NULL == df_user) {
      ++_mtlk_df_user_litepath_tx_dropped;
      dev_kfree_skb_any(skb);
      return -1;
    }

    nic = mtlk_vap_get_core(mtlk_df_get_vap_handle(df_user->df));
    if (mtlk_core_is_looped_packet(nic, skb)) {
      ++_mtlk_df_user_litepath_tx_dropped;
      dev_kfree_skb_any(skb);
      return -1;
    }

    ++df_user->dcdp.litepath_stats.tx_sent_up;
    skb->dev = rx_dev;
    /* as usual shift the eth header with skb->data */
    skb->protocol = eth_type_trans(skb, skb->dev);
    /* push up to protocol stacks */
    return (NET_RX_SUCCESS == netif_rx(skb)) ? 0 : -1;
}


int __MTLK_IFUNC
mtlk_df_user_litepath_start_xmit (struct net_device *rx_dev,
                                  struct net_device *tx_dev,
                                  struct sk_buff *skb,
                                  int len)
{
  if (tx_dev != NULL) {
    return _mtlk_df_user_litepath_tx(tx_dev, skb, len);
  }
  else if (rx_dev != NULL) {
    return _mtlk_df_user_litepath_rx(rx_dev, skb, len);
  }
  else {
    ++_mtlk_df_user_litepath_tx_dropped;
    dev_kfree_skb_any(skb);
    return -1;
  }
}
EXPORT_SYMBOL(mtlk_df_user_litepath_start_xmit);

/* For compatibility with old names */
int __MTLK_IFUNC
_mtlk_df_user_ppa_start_xmit (struct net_device *rx_dev,
                              struct net_device *tx_dev,
                              struct sk_buff *skb,
                              int len)
{
  return mtlk_df_user_litepath_start_xmit(rx_dev, tx_dev, skb, len);
}
EXPORT_SYMBOL(_mtlk_df_user_ppa_start_xmit);

/* Fn to get vapid from subif param through DCDP API*/
int
mtlk_get_dcdp_subif_vapid(struct dp_subif *subif_id, int32_t port_id)
{
  struct dc_dp_subif dc_dp_subif;
  memset(&dc_dp_subif, 0, sizeof(dc_dp_subif));
  if(subif_id != NULL){
    dc_dp_subif.port_id = port_id;
    dc_dp_subif.subif = subif_id->subif;
    dc_dp_get_subif_param(&dc_dp_subif);
    return dc_dp_subif.vap_id;
  } else {
    return MTLK_ERR_PARAMS;
  }
}

static int
_mtlk_df_user_litepath_register (mtlk_df_user_t* df_user)
{
    int vap_id;
    MTLK_ASSERT(df_user != NULL);
    MTLK_ASSERT(__mtlk_df_user_litepath_is_registered(df_user) == FALSE);

    if (!__mtlk_df_user_litepath_is_available(df_user)) {
      return MTLK_ERR_NOT_SUPPORTED; /* not using LitePath with FastPath */
    }

    vap_id = mtlk_get_dcdp_subif_vapid(&df_user->dcdp.subif_id, df_user->dcdp.subif_id.port_id);
    if (vap_id == MTLK_ERR_PARAMS){
      ILOG0_V("Error in getting vap_id from dcdp get API");
    }

    /* Register LitePath subinterface */
    ILOG0_SDD("%s: Register LitePath subinterface (port_id=%d, vap_id=%u)",
      mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,vap_id);

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, NULL, &df_user->dcdp.subif_id, DC_DP_F_REGISTER_LITEPATH)) {
#else
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, &df_user->dcdp.subif_id, DC_DP_F_REGISTER_LITEPATH)) {
#endif
      ELOG_SDD("%s: Unable to register subinterface as LitePath (port_id=%d, vap_id=%u)",
        mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,vap_id);
      return MTLK_ERR_UNKNOWN;
    }

    /* Switch mode. At this point we can send packets via LitePath. LitePath can be combined with SW path mode */
    df_user->dcdp.mode |= (DC_DP_MODE_LITEPATH | DC_DP_MODE_LITEPATH_QOS);

    ILOG0_SDD("%s: Subinterface is registered as LitePath (port_id=%d, vap_id=%u)",
      mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,vap_id);

    return MTLK_ERR_OK;
}

static void
_mtlk_df_user_litepath_unregister (mtlk_df_user_t* df_user)
{
    int vap_id;
    MTLK_ASSERT(df_user != NULL);
    MTLK_ASSERT(__mtlk_df_user_litepath_is_registered(df_user) == TRUE);

    vap_id = mtlk_get_dcdp_subif_vapid(&df_user->dcdp.subif_id, df_user->dcdp.subif_id.port_id);
    if (vap_id == MTLK_ERR_PARAMS){
      ILOG0_V("Error in getting vap_id from dcdp get API");
    }

    ILOG0_SDD("%s: Unregister LitePath subinterface (port_id=%d, vap_id=%u)",
      mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id, vap_id);

    /* Stop sending packets via LitePath before unregistration */
    df_user->dcdp.mode &= ~DC_DP_MODE_LITEPATH;

    /* remove subinterface from DC DP routing table */
    dc_dp_disconn_if(NULL, &df_user->dcdp.subif_id, NULL, 0);

    /* Unregister LitePath subinterface */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, NULL, &df_user->dcdp.subif_id, DC_DP_F_DEREGISTER_LITEPATH)) {
#else
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, &df_user->dcdp.subif_id, DC_DP_F_DEREGISTER_LITEPATH)) {
#endif
      ELOG_SDD("%s: Unable to unregister LitePath subinterface (port_id=%d, vap_id=%u)",
        mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id, vap_id);
    }
    else {
      ILOG0_SDD("%s: LitePath subinterface is unregistered (port_id=%d, vap_id=%u)",
        mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id, vap_id);
    }
    /* Reset LitePath QOS flag only after confirmed deregistration */
    df_user->dcdp.mode &= ~DC_DP_MODE_LITEPATH_QOS;
}

#ifdef CONFIG_WAVE_DEBUG
static void
_mtlk_df_user_dcdp_zero_stats(mtlk_df_user_t* df_user)
{
    MTLK_ASSERT(df_user != NULL);
    memset(&df_user->dcdp.litepath_stats, 0, sizeof(df_user->dcdp.litepath_stats));
    dc_dp_clear_netif_stats(mtlk_df_user_get_ndev(df_user), &df_user->dcdp.subif_id, DC_DP_F_SUBIF_LOGICAL);
}
#endif

static void
_mtlk_df_user_litepath_get_stats (mtlk_df_user_t *df_user, mtlk_df_user_litepath_stats_t *stats)
{
  MTLK_ASSERT(df_user != NULL);
  MTLK_ASSERT(stats != NULL);
  *stats = df_user->dcdp.litepath_stats;
}

int __MTLK_IFUNC
mtlk_df_user_set_priority_to_qos(mtlk_df_user_t* df_user, uint8 *dscp_table)
{
  /* TODO check if needed: if (mtlk_mmb_fastpath_available(mtlk_vap_get_hw(mtlk_df_get_vap_handle(df_user->df)))) */
  {
    uint8 prio2qos[DC_DP_MAX_DEV_CLASS];
    int i;

    MTLK_ASSERT(dscp_table);
    MTLK_STATIC_ASSERT(DSCP_NUM == (DC_DP_MAX_DEV_CLASS * DSCP_PER_UP));

    for (i = 0; i < DC_DP_MAX_DEV_CLASS; i++) {
      prio2qos[i] = dscp_table[i * DSCP_PER_UP];
    }

    mtlk_dump(1, prio2qos, sizeof(prio2qos), "dump of prio2qos");

    if (DC_DP_SUCCESS != dc_dp_map_prio_devqos_class(df_user->dcdp.subif_id.port_id, df_user->ndev, prio2qos)) {
      return MTLK_ERR_UNKNOWN;
    }
  }

  return MTLK_ERR_OK;
}
#endif  /* MTLK_USE_DIRECTCONNECT_DP_API */

#if MTLK_USE_PUMA6_UDMA
/*************************************************
 * PUMA6 UDMA implementation
 *************************************************/
static int _mtlk_df_user_udma_register (mtlk_df_user_t* df_user);
static void _mtlk_df_user_udma_unregister (mtlk_df_user_t* df_user);

static void _mtlk_df_user_udma_init (mtlk_df_user_t* df_user)
{
  memset(&df_user->udma, 0, sizeof(df_user->udma));
  df_user->udma.port_id = -1;
}

static __INLINE BOOL
__mtlk_df_user_udma_is_available (mtlk_df_user_t* df_user)
{
  MTLK_UNREFERENCED_PARAM(df_user);
  return (TRUE);
}

static __INLINE BOOL
__mtlk_df_user_udma_is_registered (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(df_user != NULL);
  return (df_user->udma.flags & MTLK_UDMA_REGISTERED);
}

static __INLINE BOOL
__mtlk_df_user_udma_is_active (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(df_user != NULL);
  return (df_user->udma.flags == (MTLK_UDMA_REGISTERED | MTLK_UDMA_ACTIVE));
}

struct _mtlk_df_user_sta_dev_cfg_t {
  char dev_name[IFNAMSIZ];
  int  value;
};
typedef struct _mtlk_df_user_sta_dev_cfg_t mtlk_df_user_sta_dev_cfg_t;

static int
_mtlk_df_user_udma_get_sta_dev_cfg_by_text (mtlk_df_user_sta_dev_cfg_t *dev_cfg, char *str)
{
  char buf[30];
  char *next_token = str;
  int res = MTLK_ERR_PARAMS;

  MTLK_ASSERT(dev_cfg);
  memset(dev_cfg, 0, sizeof(*dev_cfg));

  if (!str[0]) {
    goto end;
  }

  /* Parse device name */
  next_token = mtlk_get_token(next_token, dev_cfg->dev_name, sizeof(dev_cfg->dev_name), ',');
  ILOG2_S("dev_name: %s", dev_cfg->dev_name);

  /* Parse net address */
  if (next_token) {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    dev_cfg->value = mtlk_osal_str_atol(buf);
    ILOG2_D("value: %d", dev_cfg->value);
    res = MTLK_ERR_OK;
  } else {
    ELOG_V("value is not specified");
    goto end;
  }

end:
  return res;
}
/* This function maps VLAN_ID to VAP_ID */
static mtlk_df_user_t*
_mtlk_df_user_udma_map_vlan_to_user (mtlk_df_user_t *df_master_user, mtlk_vlan_id_t vlan_id)
{
    mtlk_vap_manager_t *vap_mgr;
    mtlk_vap_handle_t vap_handle;
    unsigned vaps_count, vap_id, vap_num;

    /* Get vap manager */
    vap_mgr = mtlk_df_get_vap_manager(mtlk_df_user_get_df(df_master_user));

    /* Find VAP. Simple search optimization:
      - the search begins with the value in udma.start_vap_id. In the beginning this value is 0
      - in a cycle for all VAPs, look for a match in the desired VLAN_ID.
      - If a match is found, update df_master_user->udma.start_vap_id with current value and return the found df_master_user
    */
    vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
#ifdef UDMA_DEBUG_PRINT
    ILOG1_D("Search VAP starting from %u", df_master_user->udma.start_vap_id);
#endif
    for (vap_num = 0, vap_id = df_master_user->udma.start_vap_id; vap_num < vaps_count; vap_num++)
    {
        if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_id, &vap_handle) == MTLK_ERR_OK) {
          mtlk_df_user_t *df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
          if (df_user->udma.vlan_id == vlan_id) {
            df_master_user->udma.start_vap_id = vap_id;
            return df_user;
          }
        }
        if (++vap_id >= vaps_count) vap_id = 0;
    }
    return NULL;
}

static mtlk_df_user_t*
_mtlk_df_user_udma_map_devname_to_user (mtlk_df_user_t *df_master_user, char *dev_name)
{
  mtlk_vap_manager_t *vap_mgr;
  mtlk_vap_handle_t vap_handle;
  unsigned vaps_count, vap_id;

  if (!dev_name || !dev_name[0]) {
    return NULL;
  }

  /* Get vap manager */
  vap_mgr = mtlk_df_get_vap_manager(mtlk_df_user_get_df(df_master_user));

  /* Find device */
  vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
  for (vap_id = 0; vap_id < vaps_count; vap_id++)
  {
    ILOG2_D("vap_id:%u", vap_id);
    if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_id, &vap_handle) == MTLK_ERR_OK) {
      mtlk_df_user_t *df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
      ILOG2_SS("dev_name:%s, %s", mtlk_df_user_get_name(df_user), df_user->ndev->name);
      if (0 == strcmp(dev_name, mtlk_df_user_get_name(df_user))) {
        return df_user;
      }
    }
  }
  return NULL;
}

static __INLINE int32
_mtlk_df_user_udma_get_vlan_id (mtlk_df_user_t* df_user)
{
  return df_user->udma.vlan_id;
}

static int
_mtlk_df_user_udma_set_vlan_id (mtlk_df_user_t* df_user, int32 value)
{
  /* Update vlan_id, associated with VAP */
  mtlk_vlan_id_t vlan_id = (mtlk_vlan_id_t)value;

  /* check for the correct vlan_id range */
  if ((uint32)value > 0x0FFF) {
    ELOG_D("vlan_id %d exceed range 0..4095", value);
    return MTLK_ERR_PARAMS;
  }

  /* Check if this vlan_id is not associated yet with another vap */
  if (vlan_id != 0) {
    mtlk_df_user_t *df_master_user = mtlk_df_get_user(mtlk_df_user_get_master_df(df_user));
    mtlk_df_user_t *df_mapped_user = _mtlk_df_user_udma_map_vlan_to_user(df_master_user, vlan_id);
    if (df_mapped_user &&(df_mapped_user != df_user)) {
      ELOG_DSD("vlan_id %u already associated with %s (CID-%04x)", vlan_id, df_mapped_user->name, mtlk_vap_get_oid(mtlk_df_get_vap_handle(df_mapped_user->df)));
      return MTLK_ERR_PARAMS;
    }
  }
  df_user->udma.vlan_id = vlan_id;

  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_udma_get_vlan_id_ext (mtlk_df_user_t* df_master_user, char *buffer, uint16 *length, size_t max_len)
{
  mtlk_vap_manager_t *vap_mgr;
  mtlk_vap_handle_t   vap_handle;
  unsigned            vaps_count, vap_id;
  size_t              used_len = 0;

  MTLK_ASSERT(buffer);

  if (!mtlk_vap_is_master(mtlk_df_get_vap_handle(df_master_user->df))) {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  /* Get vap manager */
  vap_mgr = mtlk_df_get_vap_manager(mtlk_df_user_get_df(df_master_user));

  /* Loop over all vaps's */
  vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
  for (vap_id = 0; vap_id < vaps_count; vap_id++) {
    if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_id, &vap_handle) == MTLK_ERR_OK) {
      mtlk_df_user_t *df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
      used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\nvap_id:%2u, vlan_id:%4u, device:%s",
          vap_id, df_user->udma.vlan_id, mtlk_df_user_get_name(df_user));
    }
    if (used_len >= max_len) {
      used_len = max_len-1;
      break;
    }
  }

  used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\n");
  *length = used_len;
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_udma_set_vlan_id_ext (mtlk_df_user_t* df_master_user, char *data)
{
  mtlk_df_user_sta_dev_cfg_t dev_cfg;
  int res;

  if (!mtlk_vap_is_master(mtlk_df_get_vap_handle(df_master_user->df))) {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  res = _mtlk_df_user_udma_get_sta_dev_cfg_by_text(&dev_cfg, data);
  if (MTLK_ERR_OK == res) {
    mtlk_df_user_t *df_user = _mtlk_df_user_udma_map_devname_to_user(df_master_user, dev_cfg.dev_name);
    if (!df_user) {
      ELOG_S("cannot set vlan_id to the device '%s'", dev_cfg.dev_name);
      return MTLK_ERR_PARAMS;
    }
    res = _mtlk_df_user_udma_set_vlan_id(df_user, dev_cfg.value);
  }
  return res;
}

static int
_mtlk_df_user_udma_get_registered_ext (mtlk_df_user_t* df_master_user, char *buffer, uint16 *length, size_t max_len)
{
  mtlk_vap_manager_t *vap_mgr;
  mtlk_vap_handle_t   vap_handle;
  unsigned            vaps_count, vap_id;
  size_t              used_len = 0;

  MTLK_ASSERT(buffer);

  if (!mtlk_vap_is_master(mtlk_df_get_vap_handle(df_master_user->df))) {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  /* Get vap manager */
  vap_mgr = mtlk_df_get_vap_manager(mtlk_df_user_get_df(df_master_user));

  /* Loop over all vaps's */
  vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
  for (vap_id = 0; vap_id < vaps_count; vap_id++) {
    if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_id, &vap_handle) == MTLK_ERR_OK) {
      mtlk_df_user_t *df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
      used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\nvap_id:%2u, UDMA enabled:%u, device:%s",
          vap_id, __mtlk_df_user_udma_is_registered(df_user), mtlk_df_user_get_name(df_user));
    }
    if (used_len >= max_len) {
      used_len = max_len-1;
      break;
    }
  }

  used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\n");
  *length = used_len;
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_udma_set_registered (mtlk_df_user_t* df_user, BOOL enabled)
{
  if (!__mtlk_df_user_udma_is_available(df_user)) {
    return MTLK_ERR_NOT_SUPPORTED;
  } else if (!enabled && __mtlk_df_user_udma_is_registered(df_user)) {
    _mtlk_df_user_udma_unregister(df_user);
    return MTLK_ERR_OK;
  } else if (enabled && !__mtlk_df_user_udma_is_registered(df_user)) {
    return _mtlk_df_user_udma_register(df_user);
  } else {
    return MTLK_ERR_OK;
  }
}

static int
_mtlk_df_user_udma_set_registered_ext (mtlk_df_user_t* df_master_user, char *data)
{
  mtlk_df_user_sta_dev_cfg_t dev_cfg;
  int res;

  if (!mtlk_vap_is_master(mtlk_df_get_vap_handle(df_master_user->df))) {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  res = _mtlk_df_user_udma_get_sta_dev_cfg_by_text(&dev_cfg, data);
  if (MTLK_ERR_OK == res) {
    mtlk_df_user_t *df_user = _mtlk_df_user_udma_map_devname_to_user(df_master_user, dev_cfg.dev_name);
    if (!df_user) {
      ELOG_S("cannot enable UDMA for the device '%s'", dev_cfg.dev_name);
      return MTLK_ERR_PARAMS;
    }
    res = _mtlk_df_user_udma_set_registered(df_user, dev_cfg.value);
  }
  return res;
}

/******************************************************************************/
/*! \brief      A callback function for the TX path for the UDMA
 *              driver on the Puma6 platform, which provides partial acceleration
 *              of the data path.
 *
 *  \param[in]  skb               skbuf pointer to transmit.
 *  \param[in]  master_netdev     Pointer to destination net device.
 *
 *  \return     void
*/

void __MTLK_IFUNC
mtlk_df_user_udma_rx_callback (struct sk_buff *skb, struct net_device *master_netdev)
{
  uint16     vlan_tci;
  unsigned   vlan_id = 0;
  struct vlan_ethhdr *vlan_header;
  struct net_device *netdev;
  mtlk_df_user_t *df_user;
  mtlk_df_user_t *df_master_user = mtlk_df_user_from_ndev(master_netdev);
  if (__UNLIKELY(!df_master_user))
    goto drop_frame;

  if (__UNLIKELY(!_mtlk_validate_wlan_netdev(master_netdev)))
    goto drop_frame;

  /* Check for 802.1Q VLAN tag. vlan_get_tag() returns 0 if successful */
  if (__UNLIKELY(vlan_get_tag(skb, &vlan_tci) < 0)) {
    #ifdef UDMA_DEBUG_PRINT
    ILOG1_S("UDMA(%s): no vlan tag. Drop frame", master_netdev->name);
    #endif
    df_master_user->udma.stats.tx_rejected_no_vlan_tag++;
    goto drop_frame;
  }

  vlan_header = (struct vlan_ethhdr *)skb->data;
  vlan_id = vlan_tci & 0xfff;

  #ifdef UDMA_DEBUG_PRINT
  ILOG1_SDDD("UDMA(%s): [DS -->] eth_type=0x%04X, VID=0x%04X, len=%04u | ", master_netdev->name,
          NET_TO_HOST16(vlan_header->h_vlan_encapsulated_proto),
          NET_TO_HOST16(vlan_header->h_vlan_TCI),
          skb->len);
  ILOG1_YY("DST:%Y | SRC:%Y", vlan_header->h_dest, vlan_header->h_source);
  #endif

  /* now un-tag header */
  memmove(skb->data + VLAN_HLEN, skb->data, MTLK_ETH_VLAN_TAG_OFFSET);
  skb_pull(skb, VLAN_HLEN);
  skb->protocol = vlan_header->h_vlan_encapsulated_proto;

  if (__UNLIKELY(0 == vlan_id)) {
    #ifdef UDMA_DEBUG_PRINT
    ILOG1_S("UDMA(%s): no vlan_id. Drop frame", master_netdev->name);
    #endif
    df_master_user->udma.stats.tx_rejected_no_vlan_tag++;
    goto drop_frame;
  }

  /* map vlan->vap and redirect to appropriate net device */
  df_user = _mtlk_df_user_udma_map_vlan_to_user(df_master_user, vlan_id);
  if (__UNLIKELY(NULL == df_user)) {
    #ifdef UDMA_DEBUG_PRINT
    ILOG1_SD("UDMA(%s): vlan_id %u is not registered. Drop frame", master_netdev->name, vlan_id);
    #endif
    df_master_user->udma.stats.tx_rejected_no_vlan_registered++;
    goto drop_frame;
  }

  if (! __mtlk_df_user_udma_is_active(df_user)) {
    /* Drop frame if UDMA is not active for this VAP. All frames for this VAP must flow trough regular way */
    goto drop_frame;
  }

  netdev = mtlk_df_user_get_ndev(df_user);
  #ifdef UDMA_DEBUG_PRINT
  ILOG1_SS("UDMA(%s): redirect frame to %s",master_netdev->name, netdev->name);
  #endif
  skb->dev = netdev;
  df_user->udma.stats.tx_processed++;
  _mtlk_df_user_start_skb_tx(skb, df_user);
  return;

drop_frame:
  /* Otherwise drop skb */
  dev_kfree_skb_any(skb);
}
EXPORT_SYMBOL(mtlk_df_user_udma_rx_callback);

static int
_mtlk_df_user_udma_register_master_handler (mtlk_df_user_t* df_master_user)
{
  uint32 udma_res;
  mtlk_vap_handle_t vap_handle;

  MTLK_ASSERT(df_master_user);
  /* Only master vap can register a callback function */
  MTLK_ASSERT(df_master_user->udma.is_master);

  if (0 == df_master_user->udma.users) {
    /* Register UDMA handler first time */

    /* Get UDMA port id. All slave AP's have the same port id as master ap */
    vap_handle = (mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_master_user)));
    df_master_user->udma.port_id = mtlk_vap_get_hw_idx(vap_handle);

    df_master_user->udma.rx_fn = mtlk_df_user_udma_rx_callback;

    udma_res = udma_register_handler(df_master_user->udma.port_id, df_master_user->ndev, df_master_user->udma.rx_fn);
    if (UDMA_OK != udma_res) {
      df_master_user->udma.rx_fn = NULL;
      ELOG_D("Can't register UDMA device function (err=%d)", udma_res);
      return MTLK_ERR_UNKNOWN;
    }

    ILOG0_D("UDMA port is registered (port=%d)", df_master_user->udma.port_id);
  }

  /* Increase number of registered users */
  df_master_user->udma.users++;
  return MTLK_ERR_OK;
}


static void
_mtlk_df_user_udma_unregister_master_handler (mtlk_df_user_t* df_master_user)
{
  MTLK_ASSERT(df_master_user != NULL);
  MTLK_ASSERT(df_master_user->udma.is_master);

  if (0 == --df_master_user->udma.users) {
    /*  Flush the pending requests and release UDMA port.*/
    udma_flush(df_master_user->udma.port_id);
    ILOG0_D("UDMA port is flushed (port=%d). UDMA for all slave VAPs is deactivated", df_master_user->udma.port_id);
    df_master_user->udma.rx_fn = NULL;
    df_master_user->udma.port_id = -1;
    df_master_user->udma.df_master_user = NULL;
  }
}

static int
_mtlk_df_user_udma_register (mtlk_df_user_t* df_user)
{
  int res;
  mtlk_df_user_t* df_master_user;

  MTLK_ASSERT(df_user != NULL);
  MTLK_ASSERT(__mtlk_df_user_udma_is_registered(df_user) == FALSE);

  if (__mtlk_df_user_udma_is_available(df_user) == FALSE) {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  /* Find a master df_user */
  df_user->udma.df_master_user = df_master_user = mtlk_df_get_user(mtlk_df_user_get_master_df(df_user));
  MTLK_ASSERT(df_user->udma.df_master_user != NULL);
  if (!df_user->udma.df_master_user) {
    return MTLK_ERR_UNKNOWN;
  }

  /* Is it master df_user ?*/
  df_user->udma.is_master = (df_user == df_master_user);

  /* Register UDMA handler in master df_user */
  res = _mtlk_df_user_udma_register_master_handler(df_master_user);
  if (MTLK_ERR_OK != res) {
    return res;
  }

  /* Get UDMA port id. All slave AP's have the same port id as master ap */
  if (!df_user->udma.is_master) {
    df_user->udma.port_id = df_master_user->udma.port_id;
    df_user->udma.rx_fn = df_master_user->udma.rx_fn;
  }

  /* At this point we can send packets via UDMA */
  df_user->udma.flags = (MTLK_UDMA_REGISTERED | MTLK_UDMA_ACTIVE);

  ILOG0_DDD("UDMA device function is registered (port=%d, VapID=%u, is_master:%d)",
      df_user->udma.port_id,
      mtlk_vap_get_id(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user))),
      df_user->udma.is_master);
  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_udma_unregister (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(df_user != NULL);
  MTLK_ASSERT(__mtlk_df_user_udma_is_registered(df_user) == TRUE);

  /*  Flush the pending requests and releases a UDMA port.*/
  df_user->udma.flags &= ~MTLK_UDMA_ACTIVE;
  ILOG0_DDD("UDMA device function is unregistered (port=%d, VapID=%u is_master:%d)",
      df_user->udma.port_id,
      mtlk_vap_get_id(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user))),
      df_user->udma.is_master);

  _mtlk_df_user_udma_unregister_master_handler(df_user->udma.df_master_user);

  if (!df_user->udma.is_master) {
    df_user->udma.rx_fn = NULL;
    df_user->udma.port_id = -1;
    df_user->udma.df_master_user = NULL;
  }

  df_user->udma.flags &= ~MTLK_UDMA_REGISTERED;
}

static void
_mtlk_df_user_udma_get_stats (mtlk_df_user_t*           df_user,
                              mtlk_df_user_udma_stats_t *stats)
{
  MTLK_ASSERT(df_user != NULL);
  MTLK_ASSERT(stats != NULL);
  wave_memcpy(stats, sizeof(*stats), &df_user->udma.stats, sizeof(df_user->udma.stats));
}

#ifdef CONFIG_WAVE_DEBUG
static void
_mtlk_df_user_udma_zero_stats (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(df_user != NULL);
  memset(&df_user->udma.stats, 0, sizeof(df_user->udma.stats));
}
#endif /* CONFIG_WAVE_DEBUG */
#endif /* MTLK_USE_PUMA6_UDMA */

static int
_mtlk_df_user_priv_subxface_start (mtlk_df_user_t* df_user)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  mtlk_hw_t *hw = mtlk_vap_get_hw(vap_handle);
  uint32 vap_id = mtlk_vap_get_id(vap_handle);
  struct dc_dp_subif dc_dp_subif_id;
  /* port idx always equal to radio id */
  uint8 port_idx = wave_vap_radio_id_get(vap_handle);

  if (df_user->is_secondary_df) {
    /* registration for this netdev is done using a secondery vap id */
    vap_id = df_user->secondary_vap_id;
  }

  df_user->dcdp.mode = DC_DP_MODE_UNREGISTERED;

  memset(&dc_dp_subif_id, 0, sizeof(dc_dp_subif_id));
  dc_dp_subif_id.port_id = mtlk_hw_mmb_get_dp_port_id(hw, port_idx);
  dc_dp_subif_id.vap_id = vap_id;

  /* API to SET subif params in DCDP */
  dc_dp_set_subif_param(&dc_dp_subif_id);
  ILOG1_D("DCDP subif : 0x%08x",dc_dp_subif_id.subif);

  df_user->dcdp.subif_id.port_id = dc_dp_subif_id.port_id;
  df_user->dcdp.subif_id.subif = dc_dp_subif_id.subif;

  /* set port id in private VAP context for fast access later */
  wave_vap_dcdp_port_set(vap_handle, df_user->dcdp.subif_id.port_id);

  /* Register FastPath/SW Path subinterface only if any of them are available via DirectConnect */
  if (mtlk_mmb_dcdp_path_available(hw)) {
    mtlk_hw_dcdp_mode_t dp_mode = mtlk_hw_mmb_get_dp_mode(hw);
    mtlk_df_user_dcdp_mode_t mode;

  #ifdef MTLK_LGM_PLATFORM_FPGA
    if (DC_DP_FAILURE == df_user->dcdp.subif_id.port_id) {
      /* On LGM FPGA, if DC DP port wasn't allocated, not a problem, just continue... */
      ELOG_SDD("%s: Can't to register DirectConnect subinterface(port_id=%d, vap_id=%u) - port wasn't initialized",
          mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,
          dc_dp_subif_id.vap_id);
      return MTLK_ERR_OK;
    }
  #endif
    /* set correct DirectConnect mode */
    switch (dp_mode) {
      case MTLK_DC_DP_MODE_FASTPATH:
        mode = DC_DP_MODE_FASTPATH;
        break;
      case MTLK_DC_DP_MODE_SWPATH:
      case MTLK_DC_DP_MODE_LITEPATH:
        mode = DC_DP_MODE_SWPATH;
        break;
      default:
        ELOG_D("Wrong DC DP mode: %d", dp_mode);
        return MTLK_ERR_UNKNOWN;
    }

    ILOG0_SDD("%s: Register DirectConnect subinterface (port_id=%d, vap_id=%u)",
      mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,
      dc_dp_subif_id.vap_id);

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, NULL, &df_user->dcdp.subif_id, 0)) {
#else
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, &df_user->dcdp.subif_id, 0)) {
#endif
      ELOG_SDD("%s: Unable to register DirectConnect subinterface(port_id=%d, vap_id=%u)",
          mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,
          dc_dp_subif_id.vap_id);
      return MTLK_ERR_UNKNOWN;
    }

    df_user->dcdp.mode = mode;

    ILOG0_SDD("%s: DirectConnect subinterface is registered (port_id=%d, vap_id=%u)",
        mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,
        dc_dp_subif_id.vap_id);

    /* Update QoS for DirectConnect DP Driver */
    {
      mtlk_core_t *core = mtlk_vap_get_core(vap_handle);
      /* Here we can take dscp_table directly from core */
      if (MTLK_ERR_OK != mtlk_df_user_set_priority_to_qos(df_user, core->dscp_table)) {
        WLOG_S("%s: Unable set priority to WMM", mtlk_df_user_get_name(df_user));
      }
    }
  }

  #if MTLK_LITEPATH_REGISTER
  /* LitePath subinterface is registered always (if enabled) */
  _mtlk_df_user_litepath_register(df_user);
  #endif
#endif

#if MTLK_USE_PUMA6_UDMA
  #if MTLK_UDMA_REGISTER
  _mtlk_df_user_udma_register(df_user);
  #endif
#endif /* MTLK_USE_PUMA6_UDMA */

  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_priv_subxface_stop (mtlk_df_user_t* df_user)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
    uint32 flags;
    int vap_id;

    if (DC_DP_MODE_UNREGISTERED == df_user->dcdp.mode) {
      ILOG0_S("%s: Unregistration of the subinterface is not required", mtlk_df_user_get_name(df_user));
      return MTLK_ERR_OK;
    }

    vap_id = mtlk_get_dcdp_subif_vapid(&df_user->dcdp.subif_id, df_user->dcdp.subif_id.port_id);
    if (vap_id == MTLK_ERR_PARAMS){
      ILOG0_V("Error in getting vap_id from dcdp get API");
    }

    ILOG0_SDD("%s: Unregister DirectConnect subinterface (port_id=%d, vap_id=%u)",
      mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,vap_id);

    /* remove subinterface from DC DP routing table */
    dc_dp_disconn_if(NULL, &df_user->dcdp.subif_id, NULL, 0);

    /* Completely unregister subinterface (including LitePath) */
    flags = DC_DP_F_DEREGISTER;
    if (df_user->dcdp.mode & DC_DP_MODE_LITEPATH) {
      flags |= DC_DP_F_DEREGISTER_LITEPATH;
    }

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, NULL, &df_user->dcdp.subif_id, DC_DP_F_DEREGISTER)) {
#else
    if (DC_DP_FAILURE == dc_dp_register_subif(THIS_MODULE, df_user->ndev, &df_user->dcdp.subif_id, DC_DP_F_DEREGISTER)) {
#endif
      ELOG_SDD("%s: Unable to unregister DirectConnect subinterface (port_id=%d, vap_id=%u)",
        mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,vap_id);
    }
    else {
      ILOG0_SDD("%s: DirectConnect subinterface is unregistered (port_id=%d, vap_id=%u)",
        mtlk_df_user_get_name(df_user), df_user->dcdp.subif_id.port_id,vap_id);
    }
    df_user->dcdp.mode = DC_DP_MODE_UNREGISTERED;

#endif

#if MTLK_USE_PUMA6_UDMA
    if (__mtlk_df_user_udma_is_registered(df_user)) {
      _mtlk_df_user_udma_unregister(df_user);
    }
#endif /* MTLK_USE_PUMA6_UDMA */

    return MTLK_ERR_OK;
}

static void
_mtlk_df_user_priv_xface_start (mtlk_df_user_t* df_user)
{
}

static void
_mtlk_df_user_priv_xface_stop (mtlk_df_user_t* df_user)
{
  mtlk_hs20_notify_unregister(&df_user->hs20_notify_entry);
}

static void
_mtlk_df_update_litepath (mtlk_df_user_t* df_user)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  if (__mtlk_df_user_litepath_is_registered(df_user)) {
    _mtlk_df_user_litepath_unregister(df_user);
    _mtlk_df_user_litepath_register(df_user);
  }
#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

#if MTLK_USE_PUMA6_UDMA
  if (__mtlk_df_user_udma_is_registered(df_user)) {
    _mtlk_df_user_udma_unregister(df_user);
    _mtlk_df_user_udma_register(df_user);
  }
#endif /* MTLK_USE_PUMA6_UDMA */
}

/* Recovery: subinterface start */
int __MTLK_IFUNC
mtlk_df_on_rcvry_subxface_start (mtlk_df_t* df)
{
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  int res = _mtlk_df_user_priv_subxface_start(df_user);
  if (MTLK_ERR_OK == res) {
    mtlk_df_user_group_init(df_user);
  }
  return res;
}

/* Recovery: subinterface stop */
int __MTLK_IFUNC
mtlk_df_on_rcvry_subxface_stop (mtlk_df_t* df)
{
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  mtlk_df_user_group_uninit(df_user);
  return _mtlk_df_user_priv_subxface_stop(df_user);
}

/********************************************************************
 * DirectConnect DP supporting functionality END
 ********************************************************************/

static void
_mtlk_df_update_netdev_cb (mtlk_df_user_t* df_user)
{
  void *new_callback = mtlk_hs20_get_netdev_callback();

  if (df_user->hs20_enabled && new_callback)
  {
    df_user->dev_ops.ndo_start_xmit = new_callback;
  }
  else
  {
    df_user->dev_ops.ndo_start_xmit = _mtlk_df_user_linux_start_tx;
  }
}

/* Called from wave_wifi_l2f.ko rmmod/insmod context */
static void _mtlk_df_user_hs20_notify(void *data)
{
  mtlk_df_user_t* df_user = (mtlk_df_user_t*) data;

  if (df_user->hs20_enabled)
  {
    _mtlk_df_update_litepath(df_user);
    _mtlk_df_update_netdev_cb(df_user);
  }
}

#define _DF_USER_GET_PARAM_MAP_CASE_FAKE_START() \
  {                                              \
    __hreq_data_in_map = MTLK_INVALID_HANDLE;    \
    {

#define _DF_USER_GET_PARAM_MAP_START(df_user, param_id_var, int_res) \
  { \
    uint32 __get_param_macro_mix_guard__ = 0; \
    uint32 __req_data_length_in_map; \
    mtlk_df_user_t* __df_user_in_map = df_user; \
    int* __result_in_map = &int_res; \
    mtlk_handle_t __hreq_data_in_map = MTLK_INVALID_HANDLE; \
    *__result_in_map = MTLK_ERR_UNKNOWN; \
    switch(param_id_var) { \
      default: { { \
          MTLK_ASSERT(!"Unknown parameter id in get request"); \
          {

#define _DF_USER_GET_PARAM_MAP_END() \
          } \
        } \
        if(MTLK_INVALID_HANDLE != __hreq_data_in_map) \
        { \
          _mtlk_df_user_free_core_data(__df_user_in_map->df, __hreq_data_in_map); \
        } \
      } \
    } \
    MTLK_UNREFERENCED_PARAM(__get_param_macro_mix_guard__); \
  }

#define _DF_USER_GET_ON_PARAM(param_id, core_req_id, is_void_request, cfg_struct_type, cfg_struct_name) \
        } \
      } \
      if(MTLK_INVALID_HANDLE != __hreq_data_in_map) \
      { \
        _mtlk_df_user_free_core_data(__df_user_in_map->df, __hreq_data_in_map); \
      } \
      MTLK_UNREFERENCED_PARAM(__get_param_macro_mix_guard__); \
      break; \
    } \
    case (param_id): { \
      cfg_struct_type *cfg_struct_name; \
      *__result_in_map = _mtlk_df_user_pull_core_data(__df_user_in_map->df, (core_req_id), (is_void_request), (void**) &cfg_struct_name, &__req_data_length_in_map, &__hreq_data_in_map); \
      if(MTLK_ERR_OK == *__result_in_map) \
      { \
        MTLK_ASSERT(sizeof(cfg_struct_type) == __req_data_length_in_map); \
        {

#define _DF_USER_SET_PARAM_MAP_START(df_user, param_id_var, int_res) \
  { \
    uint32 __set_param_macro_mix_guard__ = 0; \
    mtlk_df_user_t* __df_user_in_map = df_user; \
    int* __result_in_map = &int_res; \
    void* __core_data_in_map = NULL; \
    uint32 __core_data_size_in_map = 0; \
    uint32 __core_req_id_in_map = 0; \
    uint32 __is_void_request_in_map = FALSE; \
    *__result_in_map = MTLK_ERR_OK; \
    switch(param_id_var) { \
      default: { \
        { \
            MTLK_ASSERT(!"Unknown parameter id in set request");

#define _DF_USER_SET_PARAM_MAP_END() \
        } \
          if((NULL != __core_data_in_map))\
          { \
             if(MTLK_ERR_OK == *__result_in_map) { \
               *__result_in_map = _mtlk_df_user_push_core_data(__df_user_in_map->df, __core_req_id_in_map, __is_void_request_in_map, __core_data_in_map, __core_data_size_in_map); \
             } else { \
               mtlk_osal_mem_free(__core_data_in_map); \
             } \
          } \
      } \
    } \
    MTLK_UNREFERENCED_PARAM(__set_param_macro_mix_guard__); \
  }

#define _DF_USER_SET_ON_PARAM(param_id, core_req_id, is_void_request, cfg_struct_type, cfg_struct_name) \
      } \
      if((NULL != __core_data_in_map))\
      { \
         if(MTLK_ERR_OK == *__result_in_map) { \
           *__result_in_map = _mtlk_df_user_push_core_data(__df_user_in_map->df, __core_req_id_in_map, __is_void_request_in_map, __core_data_in_map, __core_data_size_in_map); \
         } else { \
           mtlk_osal_mem_free(__core_data_in_map); \
         } \
      } \
    } \
    MTLK_UNREFERENCED_PARAM(__set_param_macro_mix_guard__); \
    break; \
    case (param_id): { \
      cfg_struct_type *cfg_struct_name = _mtlk_df_user_alloc_core_data(__df_user_in_map->df, sizeof(cfg_struct_type)); \
      __core_data_size_in_map = sizeof(cfg_struct_type); \
      __core_data_in_map = cfg_struct_name; \
      __core_req_id_in_map = core_req_id; \
      __is_void_request_in_map = is_void_request; \
      if(NULL == __core_data_in_map) { \
        *__result_in_map = MTLK_ERR_NO_MEM; \
      } else {

#define _DF_USER_GET_ON_PARAM_MEMBER(param_id, core_req_id, is_void_request, cfg_struct_type, cfg_struct_name, member) \
        } \
      } \
      if(MTLK_INVALID_HANDLE != __hreq_data_in_map) \
      { \
        _mtlk_df_user_free_core_data(__df_user_in_map->df, __hreq_data_in_map); \
      } \
      MTLK_UNREFERENCED_PARAM(__get_param_macro_mix_guard__); \
      break; \
    } \
    case (param_id): { \
      cfg_struct_type *cfg_struct_name = _mtlk_df_user_alloc_core_data(__df_user_in_map->df, sizeof(cfg_struct_type)); \
      if(NULL == cfg_struct_name) { \
        *__result_in_map = MTLK_ERR_NO_MEM; \
      } else { \
        (cfg_struct_name)->member##_requested = TRUE; \
        *__result_in_map = mtlk_df_user_push_pull_core_data(__df_user_in_map->df, core_req_id, is_void_request, (void**) &cfg_struct_name, sizeof(cfg_struct_type), &__hreq_data_in_map); \
      } \
      if(MTLK_ERR_OK == *__result_in_map) \
      { \
        {


#define _DF_USER_GET_ON_PARAM_MEMBER(param_id, core_req_id, is_void_request, cfg_struct_type, cfg_struct_name, member) \
        } \
      } \
      if(MTLK_INVALID_HANDLE != __hreq_data_in_map) \
      { \
        _mtlk_df_user_free_core_data(__df_user_in_map->df, __hreq_data_in_map); \
      } \
      MTLK_UNREFERENCED_PARAM(__get_param_macro_mix_guard__); \
      break; \
    } \
    case (param_id): { \
      cfg_struct_type *cfg_struct_name = _mtlk_df_user_alloc_core_data(__df_user_in_map->df, sizeof(cfg_struct_type)); \
      if(NULL == cfg_struct_name) { \
        *__result_in_map = MTLK_ERR_NO_MEM; \
      } else { \
        (cfg_struct_name)->member##_requested = TRUE; \
        *__result_in_map = mtlk_df_user_push_pull_core_data(__df_user_in_map->df, core_req_id, is_void_request, (void**) &cfg_struct_name, sizeof(cfg_struct_type), &__hreq_data_in_map); \
      } \
      if(MTLK_ERR_OK == *__result_in_map) \
      { \
        {

static int
_mtlk_df_user_fill_ether_address (IEEE_ADDR *mac_addr, struct sockaddr *sa)
{
  MTLK_STATIC_ASSERT(ETH_ALEN == IEEE_ADDR_LEN);
  if (sa->sa_family != ARPHRD_ETHER) {
    ILOG2_D("Unsupported type of address: %u", sa->sa_family);
    return MTLK_ERR_PARAMS;
  }

  if (!mtlk_osal_is_valid_ether_addr(sa->sa_data)) {
    ELOG_V("Incorrect MAC address provided (zero, multicast or broadcast)");
  }

  ieee_addr_set(mac_addr, sa->sa_data);
  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_fill_sockaddr (struct sockaddr *sa, IEEE_ADDR *mac_addr)
{
  MTLK_STATIC_ASSERT(ETH_ALEN == IEEE_ADDR_LEN);
  sa->sa_family = ARPHRD_ETHER;
  ieee_addr_get(sa->sa_data, mac_addr);
}


static int
_mtlk_df_user_fill_tpc_cfg(mtlk_tpc_cfg_t *cfg,  uint32 data)
{
  if (data != TPC_CLOSED_LOOP && data != TPC_OPEN_LOOP)
  {
    ELOG_D("Incorrect TPC Loop Type: %u", data);
    return MTLK_ERR_VALUE;
  }

  cfg->loop_type = data;

  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_fill_peer_ap_key_idx (uint8 *peer_ap_key_idx, uint32 data)
{

  if (data > WAVE_WEP_NUM_DEFAULT_KEYS)
  {
    ELOG_DD("Wrong key index %d. Usage: <[1-%d]|0> (0 means Open)", data, WAVE_WEP_NUM_DEFAULT_KEYS);
    return MTLK_ERR_VALUE;
  }

  *peer_ap_key_idx = (uint8)data;

  return MTLK_ERR_OK;
}

static uint32
_mtlk_df_user_print_eeprom(mtlk_eeprom_data_cfg_t *eeprom, char *buffer, uint32 buf_len)
{
  uint8 *buf;
  uint32 used_len = 0;
  uint32 max_len = buf_len;

  if (0 == max_len) {
    goto nomem;
  }

  buf = mtlk_osal_mem_alloc(max_len, MTLK_MEM_TAG_EEPROM);
  if (NULL == buf) {
    goto nomem;
  }

  memset(buf, 0, max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                           "\nEEPROM type   : %s\n", eeprom->eeprom_type_str);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                           "EEPROM version: %i.%i\n", eeprom->eeprom_version/0x100, eeprom->eeprom_version%0x100);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                           "EEPROM header : %d bytes\n", eeprom->hdr_size);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                           "EEPROM MAC    : " MAC_PRINTF_FMT "\n", MAC_PRINTF_ARG(eeprom->mac_address));
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                            "CAL_FILE type : %s\n",
                            wave_eeprom_cal_file_type_to_string(eeprom->cal_file_type));
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len, "HW ID         : 0x%x,0x%x,0x%x,0x%x\n",
    eeprom->vendor_id, eeprom->device_id,
    eeprom->sub_vendor_id, eeprom->sub_device_id);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                           "HW type       : 0x%02X\n", eeprom->type);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                            "HW revision   : 0x%02X (%c)\n", eeprom->revision, eeprom->revision);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
                            "is_asic       : %u\n"
                            "is_pdxp       : %u\n"
                            "is_fpga       : %u\n"
                            "is_phy_dummy  : %u\n",
                            eeprom->is_asic, eeprom->is_emul, eeprom->is_fpga, eeprom->is_phy_dummy);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len, "Serial number : %02x%02x%02x\n",
    eeprom->sn[2], eeprom->sn[1], eeprom->sn[0]);
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len, "Week/Year     : %02d/%02d\n",
    eeprom->production_week, eeprom->production_year);
  MTLK_ASSERT(used_len < max_len);
  MTLK_ASSERT(strlen(buf) == used_len);

  if (used_len < buf_len) {
    wave_strncopy(buffer, buf, buf_len, used_len);
  }
  else {
    used_len = 0;
  }

  mtlk_osal_mem_free(buf);
nomem:
  return used_len;
}

static uint32
_mtlk_df_user_print_raw_eeprom_header (uint8 *raw_eeprom, int header_size, char *buffer, uint32 buf_len)
{
  uint8 *buf;
  uint32 used_len = 0;
  uint32 max_len = buf_len;

  if (0 == max_len) {
    goto nomem;
  }

  buf = mtlk_osal_mem_alloc(max_len, MTLK_MEM_TAG_EEPROM);
  if (NULL == buf) {
    goto nomem;
  }

  memset(buf, 0, max_len);

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len, "\nEEPROM header binary dump:\n");
  MTLK_ASSERT(used_len < max_len);

  used_len += mtlk_shexdump(buf+used_len, raw_eeprom, header_size);

  if (used_len < buf_len) {
    wave_strncopy(buffer, buf, buf_len, used_len);
  }
  else {
    used_len = 0;
  }

  mtlk_osal_mem_free(buf);
nomem:
  return used_len;
}

static int
_mtlk_df_user_datapath_text_get_cfg (mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params, uint16* length)
{
  const char *str_mode;
  mtlk_hw_t *hw;
  size_t max_size = MTLK_IW_PRIV_SIZE(params);

  MTLK_ASSERT(df_user != NULL);

  if (PRM_ID_DCDP_API_DATAPATH_MODE == subcmd) {
    MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == TEXT_ITEM);
#if MTLK_USE_DIRECTCONNECT_DP_API
    if (df_user->dcdp.mode == DC_DP_MODE_UNREGISTERED || df_user->dcdp.mode == DC_DP_MODE_INTERNAL_SWPATH_ONLY) {
      str_mode = "1 (SW path, intf isn't registered in DCDP)";
    } else if (df_user->dcdp.mode & DC_DP_MODE_SWPATH) {
      str_mode = "2 (SW path emulated by DCDP)";
    } else if (df_user->dcdp.mode & DC_DP_MODE_FASTPATH) {
      str_mode = "3 (Fastpath)";
    } else if (df_user->dcdp.mode & DC_DP_MODE_LITEPATH) {
      str_mode = "4 (Litepath)";
    } else {
      str_mode = "??? wrong flags";
      MTLK_ASSERT(FALSE);
    }
#else
    str_mode = "0 (pure SW path, no DCDP support)";
#endif
    hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df_user->df));
    *length = mtlk_snprintf(data, max_size, "%s, FW mode: %s", str_mode, wave_hw_get_dc_mode_name(hw));
    return (*length == 0) ? MTLK_ERR_UNKNOWN : MTLK_ERR_OK;
  } else {
    return MTLK_ERR_NOT_HANDLED;
  }
}

#if MTLK_USE_DIRECTCONNECT_DP_API
static int
_mtlk_df_user_litepath_int_get_cfg (mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params, uint16* length)
{
  MTLK_ASSERT(df_user != NULL);

  if (PRM_ID_DCDP_API_LITEPATH == subcmd) {
      if (!__mtlk_df_user_litepath_is_available(df_user)) {
        ELOG_V("Litepath not available");
        return MTLK_ERR_NOT_SUPPORTED;
      } else {
        MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
        *(uint32*)data = __mtlk_df_user_litepath_is_registered(df_user);
        *length = 1;
        return MTLK_ERR_OK;
      }
  } else {
    return MTLK_ERR_NOT_HANDLED;
  }
}

static int
_mtlk_df_user_litepath_int_set_cfg (mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params)
{
  MTLK_ASSERT(df_user != NULL);

  if (PRM_ID_DCDP_API_LITEPATH == subcmd) {
      uint32 value;
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
      value = *(uint32*)data;
      if (!__mtlk_df_user_litepath_is_available(df_user)) {
        ELOG_V("Litepath not available");
        return MTLK_ERR_NOT_SUPPORTED;
      } else if (!value && __mtlk_df_user_litepath_is_registered(df_user)) {
        _mtlk_df_user_litepath_unregister(df_user);
        return MTLK_ERR_OK;
      } else if (value && !__mtlk_df_user_litepath_is_registered(df_user)) {
        return _mtlk_df_user_litepath_register(df_user);
      } else {
        return MTLK_ERR_OK;
      }
  } else {
    return MTLK_ERR_NOT_HANDLED;
  }
}
#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

#if MTLK_USE_PUMA6_UDMA
static int
_mtlk_df_user_udma_int_get_cfg(mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params, uint16* length)
{
  MTLK_ASSERT(df_user != NULL);

  switch (subcmd) {
    case PRM_ID_UDMA_API:
      if (!__mtlk_df_user_udma_is_available(df_user)) {
        return MTLK_ERR_NOT_SUPPORTED;
      } else {
        MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
        *(uint32*)data = __mtlk_df_user_udma_is_registered(df_user);
        *length = 1;
        return MTLK_ERR_OK;
      }
    case PRM_ID_UDMA_API_EXT:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == TEXT_ITEM);
      return _mtlk_df_user_udma_get_registered_ext (df_user, data, length, MTLK_IW_PRIV_LENGTH(params));
    case PRM_ID_UDMA_VLAN_ID:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
      *(uint32*)data = (uint32)_mtlk_df_user_udma_get_vlan_id(df_user);
      *length = 1;
      return MTLK_ERR_OK;
    case PRM_ID_UDMA_VLAN_ID_EXT:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == TEXT_ITEM);
      return _mtlk_df_user_udma_get_vlan_id_ext (df_user, data, length, MTLK_IW_PRIV_LENGTH(params));
    default:
      return MTLK_ERR_NOT_HANDLED;
  }
}

static int
_mtlk_df_user_udma_int_set_cfg(mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params)
{
  MTLK_ASSERT(df_user != NULL);

  switch (subcmd) {
    case PRM_ID_UDMA_API:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
      return _mtlk_df_user_udma_set_registered(df_user, !!(*(uint32*)data));
    case PRM_ID_UDMA_API_EXT:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == TEXT_ITEM);
      return _mtlk_df_user_udma_set_registered_ext(df_user, data);
    case PRM_ID_UDMA_VLAN_ID:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
      return _mtlk_df_user_udma_set_vlan_id(df_user, *(uint32*)data);
    case PRM_ID_UDMA_VLAN_ID_EXT:
      MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == TEXT_ITEM);
      return _mtlk_df_user_udma_set_vlan_id_ext(df_user, data);
    default:
      return MTLK_ERR_NOT_HANDLED;
  }
}
#endif /* MTLK_USE_PUMA6_UDMA */

/* Exported by mtlkroot */
extern int mtlk_genl_family_id;

static void
_mtlk_df_user_intvec_by_11b_antsel (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_11b_antsel_t *antsel)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 3, *intvec_length);

  intvec[0] = antsel->txAnt;
  intvec[1] = antsel->rxAnt;
  intvec[2] = antsel->rate;
}

static void
_mtlk_df_user_get_intvec_by_auto_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_coc_auto_cfg_t *auto_params)
{
  wave_wordcpy(intvec, max_length, auto_params->values, MTLK_COC_AUTO_MAX_CFG_PARAMS);

  *intvec_length = MTLK_COC_AUTO_MAX_CFG_PARAMS;
}

static void
_mtlk_df_user_get_intvec_by_coc_power_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length,
                                              const mtlk_coc_power_cfg_t *power_params)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_COC_PW_MAX_CFG_PARAMS, *intvec_length);

  intvec[0] = power_params->is_auto_mode;
  intvec[1] = power_params->antenna_params.num_tx_antennas;
  intvec[2] = power_params->antenna_params.num_rx_antennas;

  intvec[3] = power_params->ant_mask_params.mask[0];
  intvec[4] = power_params->ant_mask_params.mask[1];
  intvec[5] = power_params->ant_mask_params.mask[2];
  intvec[6] = power_params->ant_mask_params.mask[3];
  intvec[7] = power_params->bw_params.coc_BW;
  intvec[8] = power_params->bw_params.coc_BW;
  ILOG3_D("mask 0x%08X", power_params->ant_mask_params.word);

  MTLK_STATIC_ASSERT(9 == MTLK_COC_PW_MAX_CFG_PARAMS);
  *intvec_length = MTLK_COC_PW_MAX_CFG_PARAMS;
}

static void
_mtlk_df_user_intvec_by_coex (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_coex_data_t *coex_cfg)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 4, *intvec_length);

  intvec[0] =  coex_cfg->coex_mode;
  intvec[1] =  coex_cfg->activeWifiWindowSize;
  intvec[2] =  coex_cfg->inactiveWifiWindowSize;
  intvec[3] =  coex_cfg->cts2self_active;
}

static void
_mtlk_df_user_intvec_by_recovery (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const wave_rcvry_cfg_t *recovery)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 5, *intvec_length);

  intvec[0] =  recovery->rcvry_mode;
  intvec[1] =  recovery->rcvry_fast_cnt;
  intvec[2] =  recovery->rcvry_full_cnt;
  intvec[3] =  recovery->fw_dump;
  intvec[4] =  recovery->rcvry_timer_value;
}

static void
_mtlk_df_user_intvec_by_recovery_stats (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_wssa_drv_recovery_stats_t *stats)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 4, *intvec_length);

  intvec[0] = stats->FastRcvryProcessed;
  intvec[1] = stats->FullRcvryProcessed;
  intvec[2] = stats->FastRcvryFailed;
  intvec[3] = stats->FullRcvryFailed;
}

static mtlk_error_t
_mtlk_df_user_get_coc_power_ant_mask_params_by_intvec (const uint32 *intvec, size_t nof_masks,
                                                       mtlk_coc_ant_mask_cfg_t *ant_mask_params)
{
  uint32 value, mask;
  size_t i;

  for (i = 0; i < MIN(ARRAY_SIZE(ant_mask_params->mask), nof_masks); i++) {
    if (i < nof_masks) {
      value = intvec[i];
      if (value <= MTLK_COC_ANT_MASK_VALUE_MAX) {
        mask = value;
      } else if ((value == MTLK_COC_ANT_MASK_VALUE_UNDEF) || (value == MAX_UINT32)) {
        mask = MTLK_COC_ANT_MASK_VALUE_UNDEF; /* for both 0xFF (255) and 0xFFFFFFFF (-1) */
      } else {
        ELOG_DD("Invalid AntMask[%u] value (0x%X)", i, value);
        return MTLK_ERR_PARAMS;
      }
    } else {
      mask = MTLK_COC_ANT_MASK_VALUE_UNDEF;
    }
    ant_mask_params->mask[i] = mask;
    ILOG3_DD("[%u] mask 0x%02X", i, ant_mask_params->mask[i]);
  }

  return MTLK_ERR_OK;
}

static mtlk_error_t
_mtlk_df_user_get_coc_power_params_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_coc_power_cfg_t *power_params)
{
  uint32 mode, num_rx_ant, num_tx_ant;
  mtlk_error_t res = MTLK_ERR_PARAMS;

  memset(power_params, 0, sizeof(*power_params));

  mode = intvec[0]; /* 0 - manual, 1 - auto */
  if ((mode != 0) && (mode != 1)) {
    ELOG_D("Incorrect vector configuration: <enable mode %u>", mode);
  }
  else if(((mode == 0) && (intvec_length < MTLK_COC_PW_ANT_CFG_PARAMS))) {
    ELOG_DD("Incorrect vector configuration: <enable mode %u> and length %u", mode, intvec_length);
  }
  else if(mode) { /* auto */
    res = MTLK_ERR_OK;
  }
  else { /* manual */
    num_tx_ant = intvec[1];
    num_rx_ant = intvec[2];
    if ((num_tx_ant == 0) || (num_rx_ant == 0)) {
      ELOG_V("Incorrect vector configuration: CoC manual mode and zero antennas");
    } else {
      power_params->antenna_params.num_tx_antennas = num_tx_ant;
      power_params->antenna_params.num_rx_antennas = num_rx_ant;

      res = MTLK_ERR_OK;
    }
  }

  power_params->is_auto_mode = mode;

  if (MTLK_ERR_OK != res)
      return res;

  if (intvec_length > MTLK_COC_PW_ANT_CFG_PARAMS) { /* optional */
    power_params->nof_ant_masks = intvec_length - MTLK_COC_PW_ANT_CFG_PARAMS;
    res = _mtlk_df_user_get_coc_power_ant_mask_params_by_intvec(
              intvec + MTLK_COC_PW_ANT_CFG_PARAMS,
              power_params->nof_ant_masks, &power_params->ant_mask_params);
    power_params->bw_params.coc_BW =  intvec[7];
    power_params->bw_params.coc_omn_IE =  intvec[8];
  }

  return res;
}

static int
_mtlk_df_user_get_coc_auto_params_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_coc_auto_cfg_t *auto_params)
{
  int res = MTLK_ERR_PARAMS;

  memset(auto_params, 0, sizeof(mtlk_coc_auto_cfg_t));

  /* Check num of params only. All the values will be validated later */
  if ((intvec_length < MTLK_COC_AUTO_MIN_CFG_PARAMS) ||
      (intvec_length > MTLK_COC_AUTO_MAX_CFG_PARAMS)) {
    ELOG_DDD("Incorrect vector length %u, expected %u...%u",
             intvec_length, MTLK_COC_AUTO_MIN_CFG_PARAMS, MTLK_COC_AUTO_MAX_CFG_PARAMS);
  } else {
    wave_wordcpy(auto_params->values, ARRAY_SIZE(auto_params->values), intvec, intvec_length);
    auto_params->params_count = intvec_length;
    res = MTLK_ERR_OK;
  }

  return res;
}

#define MTLK_DF_USER_RTS_MODE_PARAM_COUNT 2

static int
_mtlk_df_user_get_rts_mode_params_by_intvec(const uint32 *intvec, uint16 intvec_length, mtlk_core_rts_mode_t *rts_mode)
{
  if (intvec_length != MTLK_DF_USER_RTS_MODE_PARAM_COUNT) {
    ELOG_DD("Incorrect vector length (%u), must be %u", intvec_length, MTLK_DF_USER_RTS_MODE_PARAM_COUNT);
    return MTLK_ERR_PARAMS;
  }

  rts_mode->dynamic_bw = intvec[0];
  rts_mode->static_bw  = intvec[1];

  return MTLK_ERR_OK;
}

static mtlk_error_t
_df_user_get_txop_params_by_intvec (const uint32 *data_in, uint16 data_in_len, UMI_SET_TXOP_CONFIG *data_out)
{
  uint32 param0, param1, param2, param3;

  if (data_in_len != WAVE_TXOP_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%u). %u arguments are required.",
            data_in_len, WAVE_TXOP_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  param0 = data_in[0];
  if ((param0 > WAVE_TXOP_CFG_STA_ID_MAX) &&
      (param0 != WAVE_TXOP_CFG_ALL_STA_ID)) {
    ELOG_DDD("Invalid 1st input argument (%u). Allowed values are 0...%u and %u.", param0,
             WAVE_TXOP_CFG_STA_ID_MAX, WAVE_TXOP_CFG_ALL_STA_ID);
    return MTLK_ERR_VALUE;
  }
  param1 = data_in[1];
  if (param1 > WAVE_TXOP_CFG_MODE_MAX) {
    ELOG_DD("Invalid 2nd input argument (%u). Allowed values are 0...%u.", param1, WAVE_TXOP_CFG_MODE_MAX);
    return MTLK_ERR_VALUE;
  }
  param2 = data_in[2];
  if ((param2 < WAVE_TXOP_CFG_TXOP_DURATION_MIN) ||
      (param2 > WAVE_TXOP_CFG_TXOP_DURATION_MAX)) {
    ELOG_DDD("Invalid 3rd input argument (%u). Allowed values are %u...%u.", param2,
             WAVE_TXOP_CFG_TXOP_DURATION_MIN, WAVE_TXOP_CFG_TXOP_DURATION_MAX);
    return MTLK_ERR_VALUE;
  }
  param3 = data_in[3];
  if (param3 > WAVE_TXOP_CFG_MAX_STAS_NUM_MAX) {
    ELOG_DD("Invalid 4th input argument (%u). Allowed values are 0...%u.", param3, WAVE_TXOP_CFG_MAX_STAS_NUM_MAX);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->staId = (uint16)param0;
  data_out->mode = (uint8)param1;
  data_out->txopDuration = (uint16)param2;
  data_out->maxNumOfStas = (uint16)param3;
  return MTLK_ERR_OK;
}

#define MTLK_DF_USER_UNSOLICITED_PARAM_COUNT 2

static int
_mtlk_df_user_get_unsolicited_cfg_params_by_intvec (const uint32 *intvec, uint16 intvec_length, wave_unsolicited_frame_cfg_t *unsolicited_cfg)
{
  if (MTLK_DF_USER_UNSOLICITED_PARAM_COUNT != intvec_length) {
    ELOG_D("Incorrect vector length (%u), must be 2", intvec_length);
    return MTLK_ERR_PARAMS;
  }

  if (intvec[0] > WAVE_UNSOLICITED_FRAME_CFG_MODE_FILS_DISC) {
    ELOG_DDD("Incorrect mode value %d, must be %d..%d",
             intvec[0], WAVE_UNSOLICITED_FRAME_CFG_MODE_DISABLE, WAVE_UNSOLICITED_FRAME_CFG_MODE_FILS_DISC);
    return MTLK_ERR_VALUE;
  }

  if (intvec[1] > MAX_UINT8) {
    ELOG_DD("Incorrect duration value %d, must be 0..%d", intvec[1], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }

  unsolicited_cfg->mode = intvec[0];
  unsolicited_cfg->duration = intvec[1];

  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_get_aggr_cfg_params_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_core_aggr_cfg_t *aggr_cfg)
{
  /* Inputs validated in serializer */
  if ((2 <= intvec_length) && (intvec_length <= 3)) {
    aggr_cfg->amsdu_mode = intvec[0];
    aggr_cfg->ba_mode    = intvec[1];

    /* windowSize is optional */
    if (intvec_length > 2) {
      aggr_cfg->windowSize = intvec[2];
    } else {
      aggr_cfg->windowSize = MTLK_AGGR_CFG_WINDOW_SIZE_DFLT;
    }

    return MTLK_ERR_OK;
  } else {
    ELOG_D("Incorrect vector length (%u), must be 2 or 3", intvec_length);
    return MTLK_ERR_PARAMS;
  }
}

static int
_mtlk_df_user_get_amsdu_num_params_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_amsdu_num_cfg_t *amsdu_num_cfg)
{
  if ((intvec_length < 1) || (intvec_length > 3)) {
    ELOG_D("Incorrect vector length (%u), must be 1..3", intvec_length);
    return MTLK_ERR_PARAMS;
  }

  MTLK_STATIC_ASSERT((AMSDU_VNUM_DFLT >= VHT_MSDU_IN_AMSDU_MIN) && (AMSDU_VNUM_DFLT <= VHT_MSDU_IN_AMSDU_MAX));
  MTLK_STATIC_ASSERT((AMSDU_HENUM_DFLT >= HE_MSDU_IN_AMSDU_MIN) && (AMSDU_HENUM_DFLT <= HE_MSDU_IN_AMSDU_MAX));

  /* Inputs validated in serializer */
  amsdu_num_cfg->amsdu_num = intvec[0];
  amsdu_num_cfg->amsdu_vnum = AMSDU_VNUM_DFLT;
  amsdu_num_cfg->amsdu_henum = AMSDU_HENUM_DFLT;

  if (intvec_length > 1)
    amsdu_num_cfg->amsdu_vnum = intvec[1];

  if (intvec_length > 2)
    amsdu_num_cfg->amsdu_henum = intvec[2];

  return MTLK_ERR_OK;
}

#define MTLK_DF_USER_PIE_PARAM_COUNT 41

static int
_mtlk_df_user_get_pie_params_by_intvec(const uint32 *intvec, uint16 intvec_length, wave_pie_cfg_t *pie_cfg)
{
  if (intvec_length != MTLK_DF_USER_PIE_PARAM_COUNT) {
    ELOG_DD("Incorrect vector length (%u), must be %u", intvec_length, MTLK_DF_USER_PIE_PARAM_COUNT);
    return MTLK_ERR_PARAMS;
  }

  pie_cfg->drop_thresh                   = intvec[0];
  pie_cfg->max_burst                     = intvec[1];
  pie_cfg->minQueueSizeBytes             = intvec[2];
  pie_cfg->weigth                        = intvec[3];
  pie_cfg->pie_convert_p[0].drop_thresh  = intvec[4];
  pie_cfg->pie_convert_p[0].denominator  = intvec[5];
  pie_cfg->pie_convert_p[1].drop_thresh  = intvec[6];
  pie_cfg->pie_convert_p[1].denominator  = intvec[7];
  pie_cfg->pie_convert_p[2].drop_thresh  = intvec[8];
  pie_cfg->pie_convert_p[2].denominator  = intvec[9];
  pie_cfg->pie_convert_p[3].drop_thresh  = intvec[10];
  pie_cfg->pie_convert_p[3].denominator  = intvec[11];
  pie_cfg->pie_convert_p[4].drop_thresh  = intvec[12];
  pie_cfg->pie_convert_p[4].denominator  = intvec[13];
  pie_cfg->pie_convert_p[5].drop_thresh  = intvec[14];
  pie_cfg->pie_convert_p[5].denominator  = intvec[15];
  pie_cfg->pie_convert_p[6].drop_thresh  = intvec[16];
  pie_cfg->pie_convert_p[6].denominator  = intvec[17];
  pie_cfg->pie_convert_p[7].drop_thresh  = intvec[18];
  pie_cfg->pie_convert_p[7].denominator  = intvec[19];
  pie_cfg->pie_convert_p[8].drop_thresh  = intvec[20];
  pie_cfg->pie_convert_p[8].denominator  = intvec[21];
  pie_cfg->pie_convert_p[9].drop_thresh  = intvec[22];
  pie_cfg->pie_convert_p[9].denominator  = intvec[23];
  pie_cfg->pie_convert_p[10].drop_thresh = intvec[24];
  pie_cfg->pie_convert_p[10].denominator = intvec[25];
  pie_cfg->decay_val                     = intvec[26];
  pie_cfg->min_drop_prob                 = intvec[27];
  pie_cfg->max_drop_prob                 = intvec[28];
  pie_cfg->alpha                         = intvec[29];
  pie_cfg->beta                          = intvec[30];
  pie_cfg->dq_thresh[0]                  = intvec[31];
  pie_cfg->dq_thresh[1]                  = intvec[32];
  pie_cfg->dq_thresh[2]                  = intvec[33];
  pie_cfg->dq_thresh[3]                  = intvec[34];
  pie_cfg->target_latency[0]             = intvec[35];
  pie_cfg->target_latency[1]             = intvec[36];
  pie_cfg->target_latency[2]             = intvec[37];
  pie_cfg->target_latency[3]             = intvec[38];
  pie_cfg->t_update                      = intvec[39];
  pie_cfg->user_mode                     = intvec[40];

  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_set_intvec_by_pie_params(uint32 *intvec, uint16 max_length, uint16 *intvec_length, const wave_pie_cfg_t *pie_cfg)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_DF_USER_PIE_PARAM_COUNT, *intvec_length);

  intvec[0]  = pie_cfg->drop_thresh;
  intvec[1]  = pie_cfg->max_burst;
  intvec[2]  = pie_cfg->minQueueSizeBytes;
  intvec[3]  = pie_cfg->weigth;
  intvec[4]  = pie_cfg->pie_convert_p[0].drop_thresh;
  intvec[5]  = pie_cfg->pie_convert_p[0].denominator;
  intvec[6]  = pie_cfg->pie_convert_p[1].drop_thresh;
  intvec[7]  = pie_cfg->pie_convert_p[1].denominator;
  intvec[8]  = pie_cfg->pie_convert_p[2].drop_thresh;
  intvec[9]  = pie_cfg->pie_convert_p[2].denominator;
  intvec[10] = pie_cfg->pie_convert_p[3].drop_thresh;
  intvec[11] = pie_cfg->pie_convert_p[3].denominator;
  intvec[12] = pie_cfg->pie_convert_p[4].drop_thresh;
  intvec[13] = pie_cfg->pie_convert_p[4].denominator;
  intvec[14] = pie_cfg->pie_convert_p[5].drop_thresh;
  intvec[15] = pie_cfg->pie_convert_p[5].denominator;
  intvec[16] = pie_cfg->pie_convert_p[6].drop_thresh;
  intvec[17] = pie_cfg->pie_convert_p[6].denominator;
  intvec[18] = pie_cfg->pie_convert_p[7].drop_thresh;
  intvec[19] = pie_cfg->pie_convert_p[7].denominator;
  intvec[20] = pie_cfg->pie_convert_p[8].drop_thresh;
  intvec[21] = pie_cfg->pie_convert_p[8].denominator;
  intvec[22] = pie_cfg->pie_convert_p[9].drop_thresh;
  intvec[23] = pie_cfg->pie_convert_p[9].denominator;
  intvec[24] = pie_cfg->pie_convert_p[10].drop_thresh;
  intvec[25] = pie_cfg->pie_convert_p[10].denominator;
  intvec[26] = pie_cfg->decay_val;
  intvec[27] = pie_cfg->min_drop_prob;
  intvec[28] = pie_cfg->max_drop_prob;
  intvec[29] = pie_cfg->alpha;
  intvec[30] = pie_cfg->beta;
  intvec[31] = pie_cfg->dq_thresh[0];
  intvec[32] = pie_cfg->dq_thresh[1];
  intvec[33] = pie_cfg->dq_thresh[2];
  intvec[34] = pie_cfg->dq_thresh[3];
  intvec[35] = pie_cfg->target_latency[0];
  intvec[36] = pie_cfg->target_latency[1];
  intvec[37] = pie_cfg->target_latency[2];
  intvec[38] = pie_cfg->target_latency[3];
  intvec[39] = pie_cfg->t_update;
  intvec[40] = pie_cfg->user_mode;
}

static void
_mtlk_df_user_set_intvec_by_channel_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_core_channel_def_t *ch_params)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 3, *intvec_length);

  intvec[0] = ch_params->channel;
  intvec[1] = ch_params->spectrum_mode;
  intvec[2] = ch_params->bonding;
}

static void
_mtlk_df_user_set_intvec_by_rts_mode_params(uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_core_rts_mode_t *rts_mode)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_DF_USER_RTS_MODE_PARAM_COUNT, *intvec_length);

  intvec[0] = rts_mode->dynamic_bw;
  intvec[1] = rts_mode->static_bw;
}

static void
_mtlk_df_user_get_intvec_by_txop_params (uint32 *data_out, uint16 max_length, uint16 *data_out_len, const UMI_SET_TXOP_CONFIG *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, WAVE_TXOP_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->staId;
  data_out[1] = (uint32)data_in->mode;
  data_out[2] = (uint32)data_in->txopDuration;
  data_out[3] = (uint32)data_in->maxNumOfStas;
}

static void
_mtlk_df_user_get_intvec_by_aggr_cfg_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_core_aggr_cfg_t *aggr_cfg)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 3, *intvec_length);

  intvec[0] = aggr_cfg->amsdu_mode;
  intvec[1] = aggr_cfg->ba_mode;
  intvec[2] = aggr_cfg->windowSize;
}

static void
_mtlk_df_user_get_intvec_by_amsdu_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_amsdu_num_cfg_t *amsdu_num_cfg)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 3, *intvec_length);

  intvec[0] = amsdu_num_cfg->amsdu_num;
  intvec[1] = amsdu_num_cfg->amsdu_vnum;
  intvec[2] = amsdu_num_cfg->amsdu_henum;
}

static int
_mtlk_df_user_get_cca_th_params_by_intvec (const int32 *intvec, uint16 intvec_length, mtlk_cca_th_cfg_t *cca_th_cfg)
{
    int i, value;

    if (intvec_length != MTLK_CCA_TH_PARAMS_LEN) {
        ELOG_DD("Incorrect vector length %d (%d expected)", intvec_length, MTLK_CCA_TH_PARAMS_LEN);
        return MTLK_ERR_PARAMS;
    }

    for (i = 0; i < MTLK_CCA_TH_PARAMS_LEN; i++) {
      value = intvec[i];
      if (value < (int8)MIN_INT8 || value > MAX_INT8) {
          ELOG_DD("Input value [%d](%d) is not fit to int8", i, value);
          return MTLK_ERR_PARAMS;
      }

      cca_th_cfg->cca_th_params.values[i] = value;
    }

    return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_intvec_by_cca_th_params(int32 *intvec, uint16 max_length, uint16 *intvec_length, mtlk_cca_th_cfg_t *cca_th_cfg)
{
    int i;
    WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_CCA_TH_PARAMS_LEN, *intvec_length);

    for (i = 0; i < MTLK_CCA_TH_PARAMS_LEN; i++) {
        intvec[i] = cca_th_cfg->cca_th_params.values[i];
    }
}

static int
_mtlk_df_user_get_cca_adapt_params_by_intvec(const int32 *intvec, uint16 intvec_length, mtlk_cca_adapt_cfg_t *cca_adapt_cfg)
{
  int i, value;

  if (intvec_length != MTLK_CCA_ADAPT_PARAMS_LEN) {
    ELOG_DD("Incorrect vector length %d (%d expected), use <initial interval> <iterative interval> <limit> <step up> <step down> <step down interval> <min unblocked time>",
                           intvec_length, MTLK_CCA_ADAPT_PARAMS_LEN);
    return MTLK_ERR_PARAMS;
  }

  for (i = 0; i < MTLK_CCA_ADAPT_PARAMS_LEN; i++) {
    value = intvec[i];
    if ((i != 2) && (value < 0)) {
      ELOG_DD("Input value [%d](%d) cannot be negative", i, value);
      return MTLK_ERR_PARAMS;
    }
  }

  cca_adapt_cfg->cca_adapt_params.initial   = intvec[0];
  cca_adapt_cfg->cca_adapt_params.iterative = intvec[1];
  cca_adapt_cfg->cca_adapt_params.limit     = intvec[2];
  cca_adapt_cfg->cca_adapt_params.step_up   = intvec[3];
  cca_adapt_cfg->cca_adapt_params.step_down = intvec[4];
  cca_adapt_cfg->cca_adapt_params.step_down_interval = intvec[5];
  cca_adapt_cfg->cca_adapt_params.min_unblocked_time = intvec[6];

  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_intvec_by_cca_adapt_params (int32 *intvec, uint16 max_length, uint16 *intvec_length, mtlk_cca_adapt_cfg_t *cca_adapt_cfg)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_CCA_ADAPT_PARAMS_LEN, *intvec_length);

  intvec[0] = cca_adapt_cfg->cca_adapt_params.initial;
  intvec[1] = cca_adapt_cfg->cca_adapt_params.iterative;
  intvec[2] = cca_adapt_cfg->cca_adapt_params.limit;
  intvec[3] = cca_adapt_cfg->cca_adapt_params.step_up;
  intvec[4] = cca_adapt_cfg->cca_adapt_params.step_down;
  intvec[5] = cca_adapt_cfg->cca_adapt_params.step_down_interval;
  intvec[6] = cca_adapt_cfg->cca_adapt_params.min_unblocked_time;
}

static int
_mtlk_df_user_get_11b_antsel_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_11b_antsel_t *antsel)
{
  int res = MTLK_ERR_PARAMS;

  memset(antsel, 0, sizeof(*antsel));

  if (intvec_length != 3) {
    ELOG_D("Incorrect vector length (%u)", intvec_length);
  }
  else { /* inputs validated in serializer */
    antsel->txAnt = intvec[0];
    antsel->rxAnt = intvec[1];
    antsel->rate  = intvec[2];

    res = MTLK_ERR_OK;
  }

  return res;
}

static int
_mtlk_df_user_get_coex_cfg_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_coex_data_t *coex)
{
  int res = MTLK_ERR_PARAMS;

  if (intvec_length != 4) {
    ELOG_D("Incorrect vector length (%u)", intvec_length);
  }
  else { /* inputs validated in serializer */
    coex->coex_mode                 = intvec[0];
    coex->activeWifiWindowSize      = (intvec[1] != MAX_UINT32) ? intvec[1] : MAX_UINT32;
    coex->inactiveWifiWindowSize    = (intvec[2] != MAX_UINT32) ? intvec[2] : MAX_UINT8;
    coex->cts2self_active           = intvec[3];

    res = MTLK_ERR_OK;
  }

  return res;
}

static int
_mtlk_df_user_get_recovery_by_intvec (const uint32 *intvec, uint16 intvec_length, wave_rcvry_cfg_t *recovery)
{
  int res = MTLK_ERR_PARAMS;

  if (intvec_length != 5) {
    ELOG_D("Incorrect vector length (%u)", intvec_length);
  }
  else { /* inputs validated in serializer */
    recovery->rcvry_mode        = intvec[0];
    recovery->rcvry_fast_cnt    = intvec[1];
    recovery->rcvry_full_cnt    = intvec[2];
    recovery->fw_dump           = intvec[3];
    recovery->rcvry_timer_value = intvec[4];

    res = MTLK_ERR_OK;
  }

  return res;
}

static int
_mtlk_df_user_get_rbm_by_intvec (const uint32 *intvec, uint32 intvec_length, uint8 *rbm)
{
  int res = MTLK_ERR_OK;

  /* Keep the iwpriv usage valid also with one argument (for backward compatibility) */
  if (1 == intvec_length) {
    /* No radar bit map specified, set it for all chandef */
    *rbm = MTLK_MAX_RBM;
  }
  else if (2 == intvec_length) {
    if (intvec[1] >= MTLK_MIN_RBM && intvec[1] <= MTLK_MAX_RBM) {
      *rbm = intvec[1];
    }
    else {
      ELOG_DD("Radar hit bit map must be in range from 0x%02x to 0x%02x",
              MTLK_MIN_RBM, MTLK_MAX_RBM);
      res = MTLK_ERR_PARAMS;
    }
  }
  else {
    ELOG_V("Error in params, usage: s11hEmulatRadar <unused_param> [radar bit map]");
    return MTLK_ERR_PARAMS;
  }

  return res;
}

static int
_mtlk_df_user_get_agg_rate_limit_by_intvec (uint32 *intvec, uint16 intvec_length, mtlk_agg_rate_limit_cfg_t *agg_cfg)
{
  if (intvec_length != 2) {
    ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
    return MTLK_ERR_PARAMS;
  }
  else {
    if ((intvec[0] != 0) && (intvec[0] != 1)) {
      ELOG_D("Incorrect mode %u, must be 0 or 1", intvec[0]);
      return MTLK_ERR_VALUE;
    }
    else if (intvec[1] > MAX_UINT16) {
      ELOG_DD("Incorrect max rate %u, must be 0..%u", intvec[1], MAX_UINT16);
      return MTLK_ERR_VALUE;
    }
    else
    {
      agg_cfg->agg_rate_limit.mode    = (uint8) intvec[0];
      agg_cfg->agg_rate_limit.maxRate = (uint16)intvec[1];
      return MTLK_ERR_OK;
    }
  }
}

static void
_mtlk_df_user_get_intvec_by_agg_rate_limit(uint32 *intvec, uint16 max_length, uint16 *intvec_length, mtlk_agg_rate_limit_cfg_t *agg_cfg)
{
    WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 2, *intvec_length);

    intvec[0] = agg_cfg->agg_rate_limit.mode;
    intvec[1] = agg_cfg->agg_rate_limit.maxRate;
}

#define MTLK_DF_USER_DYNAMIC_MC_PARAM_COUNT 2

static int
_mtlk_df_user_get_multicast_rate_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_multicast_rate_cfg_t *multicast_rate_cfg)
{
    if (intvec_length != MTLK_DF_USER_DYNAMIC_MC_PARAM_COUNT) {
      ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
      return MTLK_ERR_PARAMS;
    }

    multicast_rate_cfg->multicast_rate.mode    = intvec[0];
    multicast_rate_cfg->multicast_rate.maxRate = intvec[1];
    return MTLK_ERR_OK;
}

#define MTLK_DF_USER_RETRY_LIMIT_COUNT 2
#define MAX_RETRY_LIMIT 15
#define MIN_RETRY_LIMIT 0

static void
_mtlk_df_user_get_intvec_by_retry_limit(uint32 *intvec, uint16 max_length, uint16 *intvec_length, wave_retry_limit_cfg_t *retry_limit_cfg)
{
    WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 2, *intvec_length);

    intvec[0] = retry_limit_cfg->retry_limit.retry_limit_mgmt;
    intvec[1] = retry_limit_cfg->retry_limit.retry_limit_data;
}

static int
_mtlk_df_user_get_retry_limit_by_intvec (const uint32 *intvec, uint16 intvec_length, wave_retry_limit_cfg_t *retry_limit_cfg)
{
    if (intvec_length != MTLK_DF_USER_RETRY_LIMIT_COUNT) {
      ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
      return MTLK_ERR_PARAMS;
    }

    if (intvec[0] < MIN_RETRY_LIMIT || intvec[0] > MAX_RETRY_LIMIT) {
      ELOG_DDD("Error in retry limit mgmt : %d valid range %d ... %d",intvec[0],MIN_RETRY_LIMIT,MAX_RETRY_LIMIT);
      return MTLK_ERR_PARAMS;
    }

    if (intvec[1] < MIN_RETRY_LIMIT || intvec[1] > MAX_RETRY_LIMIT) {
      ELOG_DDD("Error in retry limit data : %d valid range %d ... %d",intvec[1],MIN_RETRY_LIMIT,MAX_RETRY_LIMIT);
      return MTLK_ERR_PARAMS;
    }

    retry_limit_cfg->retry_limit.retry_limit_mgmt = intvec[0];
    retry_limit_cfg->retry_limit.retry_limit_data = intvec[1];
    return MTLK_ERR_OK;
}

static int
_mtlk_df_user_get_mu_ofdma_bf_by_intvec(uint32 *intvec, uint16 intvec_length, mtlk_mu_ofdma_bf_cfg_t *mu_ofdma_bf_cfg)
{
    if (intvec_length != 2) {
      ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
      return MTLK_ERR_PARAMS;
    }
    else {

      if (intvec[0]!=0 && intvec[0]!=1 && intvec[0]!=2) {
          ELOG_D("Incorrect mode of MU Ofdma Beamforming %u : Valid Modes : 0 - Disable, 1 - Enable, 2 - Dynamic", intvec[0]);
          return MTLK_ERR_VALUE;
      }
      if  (!(intvec[1] >= MTLK_MIN_BFPERIOD && intvec[1] <= MTLK_MAX_BFPERIOD)) {
          ELOG_D("Incorrect MU Ofdma beamforming Period : %u : Valid values : Min. value - 100secs, Max Value - 1000secs", intvec[1]);
          return MTLK_ERR_VALUE;
      }
      else
      {
          mu_ofdma_bf_cfg->mu_ofdma_bf.mode     =  (uint8) intvec[0];
          mu_ofdma_bf_cfg->mu_ofdma_bf.bfPeriod = (uint32) intvec[1];
          return MTLK_ERR_OK;
      }
    }
}

static void
_mtlk_df_user_get_intvec_by_mu_ofdma_bf(uint32 *intvec, uint16 max_length, uint16 *intvec_length, mtlk_mu_ofdma_bf_cfg_t *mu_ofdma_bf_cfg)
{
    MTLK_ASSERT(max_length >= 2);

    intvec[0] = mu_ofdma_bf_cfg->mu_ofdma_bf.mode;
    intvec[1] = mu_ofdma_bf_cfg->mu_ofdma_bf.bfPeriod;
    *intvec_length = 2;
}

#define MTLK_DF_USER_WHM_CONFIG_COUNT (1)
#define WHM_ENABLE                    (0)
#define WHM_DRV_WARN                  (2)
#define WHM_FW_WARN                   (3)
#define WHM_PHY_WARN                  (4)

static int
_mtlk_df_user_get_whm_cfg_by_intvec (const uint32 *intvec, uint16 intvec_length, wave_whm_cfg_t *whm_cfg)
{
    if (intvec_length != MTLK_DF_USER_WHM_CONFIG_COUNT) {
      ELOG_D("Need exactly 1 arguments (length=%u)", intvec_length);
      return MTLK_ERR_PARAMS;
    }
    whm_cfg->whm_cfg.whm_enable      = MTLK_BIT_GET(intvec[0], WHM_ENABLE);
    whm_cfg->whm_cfg.whm_drv_warn    = MTLK_BIT_GET(intvec[0], WHM_DRV_WARN);
    whm_cfg->whm_cfg.whm_fw_warn     = MTLK_BIT_GET(intvec[0], WHM_FW_WARN);
    whm_cfg->whm_cfg.whm_phy_warn    = MTLK_BIT_GET(intvec[0], WHM_PHY_WARN);
    return MTLK_ERR_OK;
}

#define WHM_WARNING_ID_RES_HOSTAP_MIN 51
#define WHM_WARNING_ID_RES_HOSTAP_MAX 100
#define WHM_WARNING_ID_RES_IW_MIN  101
#define WHM_WARNING_ID_RES_IW_MAX  150

static int
_mtlk_df_user_get_whm_trigger_by_intvec (const uint32 *intvec, uint16 intvec_length, wave_whm_trigger_t *whm_trigger_cfg)
{
    if ((intvec_length < 1) || (intvec_length > 2)) {
      ELOG_D("Incorrect vector length (%u), must be 1..2", intvec_length);
      return MTLK_ERR_PARAMS;
    }

    if (intvec_length > 1) {
      if (intvec[1] == WHM_HOSTAPD_TRIGGER && ((intvec[0] >= WHM_WARNING_ID_RES_HOSTAP_MIN && intvec[0] <= WHM_WARNING_ID_RES_HOSTAP_MAX))) {
        whm_trigger_cfg->whm_warning_id= intvec[0];
        whm_trigger_cfg->whm_warning_layer = intvec[1];
      } else if (intvec[1] == WHM_IW_TRIGGER && ((intvec[0] >= WHM_WARNING_ID_RES_IW_MIN && intvec[0] <= WHM_WARNING_ID_RES_IW_MAX))) {
        whm_trigger_cfg->whm_warning_id= intvec[0];
        whm_trigger_cfg->whm_warning_layer = intvec[1];
      } else {
        ELOG_DDDDDDDD("Incorrect whm_trigger params Id(%d) Layer(%d) - hostapd layer (%d) warningID should be %d...%d and for iw layer (%d) warningID should be %d...%d", intvec[0], intvec[1], WHM_HOSTAPD_TRIGGER, WHM_WARNING_ID_RES_HOSTAP_MIN, WHM_WARNING_ID_RES_HOSTAP_MAX, WHM_IW_TRIGGER, WHM_WARNING_ID_RES_IW_MIN, WHM_WARNING_ID_RES_IW_MAX);
        return MTLK_ERR_PARAMS;
	  }
    }
    else {
      if (!(intvec[0] >= WHM_WARNING_ID_RES_IW_MIN && intvec[0] <= WHM_WARNING_ID_RES_IW_MAX)) {
        ELOG_DDD("Incorrect whm_trigger WarningId from iw (%d), must be %d ... %d",intvec[0], WHM_WARNING_ID_RES_IW_MIN, WHM_WARNING_ID_RES_IW_MAX);
        return MTLK_ERR_PARAMS;
      }
      whm_trigger_cfg->whm_warning_id = intvec[0];
      whm_trigger_cfg->whm_warning_layer = WHM_IW_TRIGGER;
    }
    return MTLK_ERR_OK;
}

static int
_mtlk_df_user_fill_rx_th_cfg (mtlk_rx_th_cfg_t *cfg, int32 data)
{
    if ((data < -128) || (data > 127)) {
        ELOG_D("Incorrect RX threshold %d", data);
        return MTLK_ERR_VALUE;
    }

    cfg->rx_threshold = (int8)data;
    return MTLK_ERR_OK;
}

static int
_mtlk_df_user_get_duty_cycle_cfg_by_intvec (uint32 *intvec, uint16 intvec_length, mtlk_rx_duty_cycle_cfg_t *cfg)
{
  if (intvec_length != 2) {
    ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
    return MTLK_ERR_PARAMS;
  }
  else
  {
    if (intvec[0] > 32767) {
      ELOG_D("Incorrect onTime %u", intvec[0]);
      return MTLK_ERR_VALUE;
    }
    if (intvec[1] > 32767) {
      ELOG_D("Incorrect offTime %u", intvec[1]);
      return MTLK_ERR_VALUE;
    }

    cfg->duty_cycle.onTime  = intvec[0];
    cfg->duty_cycle.offTime = intvec[1];
    return MTLK_ERR_OK;
  }
}

static void
_mtlk_df_user_get_intvec_by_duty_cycle_cfg(uint32 *intvec, uint16 max_length, uint16 *intvec_length, mtlk_rx_duty_cycle_cfg_t *cfg)
{
    WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 2, *intvec_length);

    intvec[0] = cfg->duty_cycle.onTime;
    intvec[1] = cfg->duty_cycle.offTime;
}

static int
_mtlk_df_user_get_mcast_range_by_text (mtlk_mcast_range_cfg_t *range_cfg, char *str)
{
  /* Parse command according to syntax:
   * <action>[,netaddress[,netmask]]
   * where:
   *    action: 0 -> cleanup table (no address/mask required)
   *    action: 1 -> add new range to table
   *    action: 2 -> delete range from table
   * if netmask is not specified, the value 255.255.255.255 is assumed.
   * */
  char buf[30];
  char *next_token = str;
  int res = MTLK_ERR_PARAMS;
  const char *bufend;

  ILOG4_S("parse str=%s", str);
  if (!str[0]) {
    goto end;
  }

  /* set type of address */
  memset(&range_cfg->range, 0, sizeof(range_cfg->range));
  range_cfg->range.netmask.type = MTLK_IPv4;

  /* Parse action code */
  {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    range_cfg->range.action = mtlk_osal_str_atol(buf);
    if (range_cfg->range.action >= MTLK_MCAST_ACTION_LAST) {
      ELOG_D("Invalid action: %d", range_cfg->range.action);
      goto end;
    }
    ILOG2_D("action: %u", range_cfg->range.action);
  }

  /* Parse net address */
  if (next_token) {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    if (in4_pton(buf, -1, (u8*)&range_cfg->range.netmask.addr.ip4_addr, '\0' ,&bufend) == 0) {
      ELOG_S("Invalid net address %s", buf);
      goto end;
    }
    ILOG2_D("netaddr: %B", htonl(range_cfg->range.netmask.addr.ip4_addr.s_addr));
  } else {
    if (MTLK_MCAST_ACTION_CLEANUP == range_cfg->range.action)
      res = MTLK_ERR_OK; /* "cleanup" action doesn't require addresses */
    goto end;
  }

  /* Parse net mask */
  if (next_token) {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    if (in4_pton(buf, -1, (u8*)&range_cfg->range.netmask.mask.ip4_addr, '\0' ,&bufend) == 0) {
      ELOG_S("Invalid net mask %s", buf);
      goto end;
    }
    ILOG2_D("netmask: %B", htonl(range_cfg->range.netmask.mask.ip4_addr.s_addr));
  } else {
    /* assign mask 255.255.255.255 */
    range_cfg->range.netmask.mask.ip4_addr.s_addr = 0xFFFFFFFF;
    ILOG2_V("Exact address specified");
  }
  res = MTLK_ERR_OK;

end:
  return res;
}

static int
_mtlk_df_user_get_mcast_range_ipv6_by_text (mtlk_mcast_range_cfg_t *range_cfg, char *str)
{
  char buf[100];
  char *next_token = str;
  int res = MTLK_ERR_PARAMS;
  const char *bufend;

  ILOG4_S("parse str=%s", str);
  if (!str[0]) {
    goto end;
  }

  /* set type of address */
  memset(&range_cfg->range, 0, sizeof(range_cfg->range));
  range_cfg->range.netmask.type = MTLK_IPv6;

  /* Parse action code */
  {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    range_cfg->range.action = mtlk_osal_str_atol(buf);
    if (range_cfg->range.action >= MTLK_MCAST_ACTION_LAST) {
      ELOG_D("Invalid action: %d", range_cfg->range.action);
      goto end;
    }
    ILOG2_D("action: %u", range_cfg->range.action);
  }

  /* Parse net address */
  if (next_token) {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    if (in6_pton(buf, -1, (u8*)&range_cfg->range.netmask.addr.ip6_addr, '\0' ,&bufend) == 0) {
      ELOG_S("Invalid net address %s", buf);
      goto end;
    }
    ILOG2_K("netaddr: %K", range_cfg->range.netmask.addr.ip6_addr.s6_addr);
  } else {
    if (MTLK_MCAST_ACTION_CLEANUP == range_cfg->range.action)
      res = MTLK_ERR_OK; /* "cleanup" action doesn't require addresses */
    goto end;
  }

  /* Parse net mask */
  if (next_token) {
    next_token = mtlk_get_token(next_token, buf, sizeof(buf), ',');
    if (in6_pton(buf, -1, (u8*)&range_cfg->range.netmask.mask.ip6_addr, '\0' ,&bufend) == 0) {
      ELOG_S("Invalid net mask %s", buf);
      goto end;
    }
    ILOG2_K("netmask: %K", range_cfg->range.netmask.mask.ip6_addr.s6_addr);
  } else {
    /* assign mask FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF */
    memset(&range_cfg->range.netmask.mask.ip6_addr, 0xFF, sizeof(range_cfg->range.netmask.mask.ip6_addr));
    ILOG2_V("Exact address specified");
  }
  res = MTLK_ERR_OK;

end:
  return res;
}

static void
_mtlk_df_user_get_text_by_mcast_range (char *buffer, mtlk_mcast_range_vect_cfg_t *mcast_range_vect_cfg, size_t max_len, uint16 *length)
{
  mtlk_clpb_t       *clpb_range_vect;
  mtlk_ip_netmask_t *netmask;
  uint32             addr_size;
  int                out_cnt = 0;
  size_t             used_len = 0;

  MTLK_ASSERT(buffer);
  MTLK_ASSERT(mcast_range_vect_cfg);
  clpb_range_vect = mcast_range_vect_cfg->range_vect;

  if (clpb_range_vect) {
    while(used_len < max_len)
    {
      netmask = (mtlk_ip_netmask_t*)mtlk_clpb_enum_get_next(clpb_range_vect, &addr_size);
      if (NULL == netmask) break;
      MTLK_ASSERT(addr_size == sizeof(*netmask));

      switch (netmask->type) {
        case MTLK_IPv4:
          used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\n%B/%B",
              htonl(netmask->addr.ip4_addr.s_addr), htonl(netmask->mask.ip4_addr.s_addr));
          break;

        case MTLK_IPv6:
          used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\n%K/%K",
              netmask->addr.ip6_addr.s6_addr, netmask->mask.ip6_addr.s6_addr);
          break;

        default:
          used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\nInvalid entry");
          break;
      }

      out_cnt++;
    }
  }

  if (0 == out_cnt) {
    used_len = mtlk_snprintf(buffer, max_len, " No any ranges");
  }

  if (used_len >= max_len) {
    used_len = max_len-1;
  }

  used_len += mtlk_snprintf(buffer+used_len, max_len-used_len, "\n");
  *length = used_len;

  if (clpb_range_vect)
    mtlk_clpb_delete(clpb_range_vect);
  mcast_range_vect_cfg->range_vect = NULL;
  return;
}

static int
_mtlk_df_user_fill_tx_power_lim_cfg (mtlk_tx_power_lim_cfg_t *cfg, uint32 data)
{
  if ((data != 0) && (data!=1) && (data != 3) && (data != 6) && (data != 9)) {
    ELOG_D("Incorrect TX power limit value %u", data);
    return MTLK_ERR_VALUE;
  }

  cfg->powerLimitOffset = (uint8)data;
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_fill_ht_protection_cfg (mtlk_df_user_t *df_user, mtlk_ht_protection_cfg_t *cfg, uint32 data)
{
  if ((data == PROTECTION_MODE_NO_PROTECTION)       ||
      (data == PROTECTION_MODE_NO_FORCE_PROTECTION) ||
      (data == PROTECTION_MODE_FORCE_RTS_CTS)       ||
      (data == PROTECTION_MODE_CTS_TO_SELF_ENABLE)  ||
      (data == PROTECTION_MODE_FORCE_CTS_TO_SELF))
  {
    cfg->use_cts_prot = data;
    return MTLK_ERR_OK;
  }
  else
  {
    ELOG_D("Incorrect HT protection method %u", data);
    return MTLK_ERR_VALUE;
  }
}

static int
_mtlk_df_user_get_freq_jump_mode_cfg_by_int (mtlk_freq_jump_mode_cfg_t *data_out, uint32 data_in)
{
  int res = MTLK_ERR_OK;
  if (data_in > 1) {
    ELOG_D("The command argument (%d) is invalid. Allowed values are 0 and 1.", data_in);
    res = MTLK_ERR_VALUE;
  } else {
    data_out->freq_jump_mode = data_in;
  }
  return res;
}

static int
_mtlk_df_user_get_restricted_ac_mode_cfg_by_intvec (uint32 *data_in, uint16 data_in_len,
                                                    UMI_SET_RESTRICTED_AC *data_out)
{
  if (data_in_len != MTLK_RESTRICTED_AC_MODE_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%d). %d arguments are required.",
            data_in_len, MTLK_RESTRICTED_AC_MODE_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  if (data_in[0] > 1) {
    ELOG_D("Invalid 1st input argument (%d). Allowed values are 0...1.", data_in[0]);
    return MTLK_ERR_VALUE;
  }
  if (data_in[1] > MTLK_RESTRICTED_AC_MODE_BITMAP_MASK) {
    ELOG_DD("Invalid 2nd input argument (%d). Allowed values are 0...%d", data_in[1],
            MTLK_RESTRICTED_AC_MODE_BITMAP_MASK);
    return MTLK_ERR_VALUE;
  }
  if (data_in[0] && !data_in[1]) {
    ELOG_D("Invalid 2nd input argument (%d). It must not be 0 while 1st argument is not 0.", data_in[1]);
    return MTLK_ERR_VALUE;
  }
  if (data_in[2] >= data_in[3]) {
    ELOG_D("Invalid 3rd input argument (%d). It must be less than 4th argument.", data_in[2]);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->restrictedAcModeEnable = (uint8)data_in[0];
  data_out->acRestrictedBitmap = (uint8)data_in[1];
  data_out->restrictedAcThreshEnter = (uint16)data_in[2];
  data_out->restrictedAcThreshExit = (uint16)data_in[3];
  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_intvec_by_restricted_ac_mode_cfg (uint32 *data_out, uint16 max_length, uint16 *data_out_len,
                                                    UMI_SET_RESTRICTED_AC *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_RESTRICTED_AC_MODE_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->restrictedAcModeEnable;
  data_out[1] = (uint32)data_in->acRestrictedBitmap;
  data_out[2] = (uint32)data_in->restrictedAcThreshEnter;
  data_out[3] = (uint32)data_in->restrictedAcThreshExit;
}

static int
_mtlk_df_user_get_pd_threshold_cfg_by_intvec (uint32 *data_in, uint16 data_in_len,
                                              UMI_SET_PD_THRESH *data_out)
{
  if (data_in_len != MTLK_PD_THRESH_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%d). %d arguments are required.",
            data_in_len, MTLK_PD_THRESH_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  if (data_in[0] > (QOS_PD_THRESHOLD_NUM_MODES - 1)) {
    ELOG_DD("Invalid 1st input argument (%d). Allowed values are 0...%d.", data_in[0],
            (QOS_PD_THRESHOLD_NUM_MODES - 1));
    return MTLK_ERR_VALUE;
  }
  if (data_in[1] > MAX_UINT16) {
    ELOG_DD("Invalid 2nd input argument (%d). Allowed values are 0...%d.", data_in[1], MAX_UINT16);
    return MTLK_ERR_VALUE;
  }
  if (data_in[2] > MAX_UINT8) {
    ELOG_DD("Invalid 3rd input argument (%d). Allowed values are 0...%d.", data_in[2], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->mode = (uint8)data_in[0];
  data_out->minPdDiff = (uint16)data_in[1];
  data_out->minPdAmount = (uint8)data_in[2];
  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_intvec_by_pd_threshold_cfg (uint32 *data_out, uint16 max_length, uint16 *data_out_len,
                                              UMI_SET_PD_THRESH *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, MTLK_PD_THRESH_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->mode;
  data_out[1] = (uint32)data_in->minPdDiff;
  data_out[2] = (uint32)data_in->minPdAmount;
}

static int
_df_user_get_dynamic_mu_type_cfg_by_intvec (uint32 *data_in, uint16 data_in_len,
                                            UMI_DYNAMIC_MU_TYPE *data_out)
{
  if (data_in_len != WAVE_DYNAMIC_MU_TYPE_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%d). %d arguments are required.",
            data_in_len, WAVE_DYNAMIC_MU_TYPE_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  if (data_in[0] > WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_MAX) {
    ELOG_DDD("Invalid 1st input argument (%d). Allowed values are %d...%d.", data_in[0],
             WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_MIN, WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_MAX);
    return MTLK_ERR_VALUE;
  }
  if (data_in[1] > WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_MAX) {
    ELOG_DDD("Invalid 2nd input argument (%d). Allowed values are %d...%d.", data_in[1],
             WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_MIN, WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_MAX);
    return MTLK_ERR_VALUE;
  }
  if (data_in[2] > MAX_UINT8) {
    ELOG_DD("Invalid 3rd input argument (%d). Allowed values are 0...%d.", data_in[2], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }
  if (data_in[3] > MAX_UINT8) {
    ELOG_DD("Invalid 4th input argument (%d). Allowed values are 0...%d.", data_in[3], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }
  if (data_in[4] > MAX_UINT8) {
    ELOG_DD("Invalid 5th input argument (%d). Allowed values are 0...%d.", data_in[4], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->dlMuType = (uint8)data_in[0];
  data_out->ulMuType = (uint8)data_in[1];
  data_out->minStationsInGroup = (uint8)data_in[2];
  data_out->maxStationsInGroup = (uint8)data_in[3];
  data_out->cdbConfig = (uint8)data_in[4];
  return MTLK_ERR_OK;
}

static void
_df_user_get_intvec_by_dynamic_mu_type_cfg (uint32 *data_out, uint16 max_length,
                                            uint16 *data_out_len, UMI_DYNAMIC_MU_TYPE *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, WAVE_DYNAMIC_MU_TYPE_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->dlMuType;
  data_out[1] = (uint32)data_in->ulMuType;
  data_out[2] = (uint32)data_in->minStationsInGroup;
  data_out[3] = (uint32)data_in->maxStationsInGroup;
  data_out[4] = (uint32)data_in->cdbConfig;
}


static int
_df_user_get_he_mu_fixed_parameters_cfg_by_intvec (uint32 *data_in, uint16 data_in_len,
                                                   UMI_HE_MU_FIXED_PARAMTERS *data_out)
{
  if (data_in_len != WAVE_HE_MU_FIXED_PARAMTERS_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%d). %d arguments are required.",
            data_in_len, WAVE_HE_MU_FIXED_PARAMTERS_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  if ((data_in[0] > WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_MAX) &&
      (data_in[0] != WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_DEFAULT)) {
    ELOG_DDDD("Invalid 1st input argument (%d). Allowed values are %d...%d and %d.", data_in[0],
              WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_MIN, WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_MAX,
              WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_DEFAULT);
    return MTLK_ERR_VALUE;
  }
  if ((data_in[1] > WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_MAX) &&
      (data_in[1] != WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_DEFAULT)) {
    ELOG_DDDD("Invalid 2nd input argument (%d). Allowed values are %d...%d and %d.", data_in[1],
              WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_MIN, WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_MAX,
              WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_DEFAULT);
    return MTLK_ERR_VALUE;
  }
  if ((data_in[2] > WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_MAX) &&
      (data_in[2] != WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_DEFAULT)) {
    ELOG_DDDD("Invalid 3rd input argument (%d). Allowed values are %d...%d and %d.", data_in[2],
              WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_MIN, WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_MAX,
              WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_DEFAULT);
    return MTLK_ERR_VALUE;
  }
  if (data_in[3] > MAX_UINT8) {
    ELOG_DD("Invalid 4th input argument (%d). Allowed values are 0...%d.", data_in[3], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->muSequence = (uint8)data_in[0];
  data_out->ltf_GI = (uint8)data_in[1];
  data_out->codingType = (uint8)data_in[2];
  data_out->heRate = (uint8)data_in[3];
  return MTLK_ERR_OK;
}

static void
_df_user_get_intvec_by_he_mu_fixed_parameters_cfg (uint32 *data_out, uint16 max_length,
                                                   uint16 *data_out_len, UMI_HE_MU_FIXED_PARAMTERS *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, WAVE_HE_MU_FIXED_PARAMTERS_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->muSequence;
  data_out[1] = (uint32)data_in->ltf_GI;
  data_out[2] = (uint32)data_in->codingType;
  data_out[3] = (uint32)data_in->heRate;
}


static int
_df_user_get_mu_duration_cfg_by_intvec (uint32 *data_in, uint16 data_in_len,
                                        UMI_HE_MU_DURATION *data_out)
{
  if (data_in_len != WAVE_HE_MU_DURATION_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%d). %d arguments are required.",
            data_in_len, WAVE_HE_MU_DURATION_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  if (data_in[0] > MAX_UINT16) {
    ELOG_DD("Invalid 1st input argument (%d). Allowed values are 0...%d.", data_in[0], MAX_UINT16);
    return MTLK_ERR_VALUE;
  }
  if (data_in[1] > MAX_UINT16) {
    ELOG_DD("Invalid 2nd input argument (%d). Allowed values are 0...%d.", data_in[1], MAX_UINT16);
    return MTLK_ERR_VALUE;
  }
  if (data_in[2] > MAX_UINT16) {
    ELOG_DD("Invalid 3rd input argument (%d). Allowed values are 0...%d.", data_in[2], MAX_UINT16);
    return MTLK_ERR_VALUE;
  }
  if (data_in[3] > MAX_UINT16) {
    ELOG_DD("Invalid 4th input argument (%d). Allowed values are 0...%d.", data_in[3], MAX_UINT16);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->PpduDuration = (uint16)data_in[0];
  data_out->TxopDuration = (uint16)data_in[1];
  data_out->TfLength = (uint16)data_in[2];
  data_out->NumberOfRepetitions = (uint8)data_in[3];
  return MTLK_ERR_OK;
}

static int
_df_user_get_preamble_punc_cfg_by_intvec (uint32 *data_in, uint16 data_in_len,
                                        UMI_PREAMBLE_PUNCT_CCA_OVERRIDE *data_out)
{
  if (data_in_len != WAVE_PREAMBLE_PUNCTURE_CFG_SIZE) {
    ELOG_DD("Invalid number of input arguments (%d). %d arguments are required.",
            data_in_len, WAVE_PREAMBLE_PUNCTURE_CFG_SIZE);
    return MTLK_ERR_PARAMS;
  }

  if (data_in[0] > 1) {
    ELOG_D("Invalid 1st input argument (%d). Allowed values are 0/1.", data_in[0]);
    return MTLK_ERR_VALUE;
  }
  if (data_in[1] > MAX_UINT8) {
    ELOG_DD("Invalid 2nd input argument (%d). Allowed values are 0...%d.", data_in[1], MAX_UINT8);
    return MTLK_ERR_VALUE;
  }
  if (data_in[2] > 1) {
    ELOG_D("Invalid 3rd input argument (%d). Allowed values are 0/1.", data_in[2]);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->ccaOverride = (uint8)data_in[0];
  data_out->ccaBitmap = (uint8)data_in[1];
  data_out->forceFreeBands = (uint8)data_in[2];
  return MTLK_ERR_OK;
}

static void
_df_user_get_intvec_by_he_mu_duration_cfg (uint32 *data_out, uint16 max_length,
                                           uint16 *data_out_len, UMI_HE_MU_DURATION *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, WAVE_HE_MU_DURATION_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->PpduDuration;
  data_out[1] = (uint32)data_in->TxopDuration;
  data_out[2] = (uint32)data_in->TfLength;
  data_out[3] = (uint32)data_in->NumberOfRepetitions;
}

static void
_df_user_get_intvec_by_ax_default_params (uint32 *data_out, uint16 max_length,
                                           uint16 *data_out_len, ax_default_params_t *data_in)
{
  twt_params_int_t *twtParams = &data_in->twt_params;
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, UMI_GET_AX_DEFAULT_SIZE, *data_out_len);

  data_out[0] = twtParams->individualWakeTimeLow;
  data_out[1] = twtParams->individualWakeTimeHigh;
  data_out[2] = twtParams->individualWakeInterval;
  data_out[3] = twtParams->individualMinWakeDuration;
  data_out[4] = twtParams->isImplicit;
  data_out[5] = twtParams->isAnnounced;
  data_out[6] = twtParams->isTriggerEnabled;
  data_out[7] = twtParams->agreementType;
  data_out[8] = twtParams->individualChannel;
  data_out[9] = twtParams->BroadcastTargetBeacon;
  data_out[10] = twtParams->BroadcastListenInterval;
}

static void
_df_user_get_intvec_by_etsi_ppdu_limits (uint32 *data_out, uint16 max_length,
                                           uint16 *data_out_len, UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, WAVE_ETSI_PPDU_DURATION_LIMITS_MIN_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->setEtsiPpduDuration;
}

static int
_df_user_get_etsi_ppdu_limits_by_intvec (uint32 *data_in, uint16 data_in_len,
                                         UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS *data_out)
{
  if (data_in_len != 1) {
    ELOG_D("Invalid number of input arguments (%d). Only 1 allowed", data_in_len);
    return MTLK_ERR_PARAMS;
  }

  if (data_in[0] > 1) {
    ELOG_D("Invalid 1st input argument (%d). Allowed values are 0..1", data_in[0]);
    return MTLK_ERR_VALUE;
  }

  memset(data_out, 0, sizeof(*data_out));
  data_out->setEtsiPpduDuration = (uint8)data_in[0];
  return MTLK_ERR_OK;
}

#define MTLK_ERP_CFG_SIZE           10
#define MLTK_ERP_MIN_INIT_WAIT_TIME 1                           /* 1 second */
#define MLTK_ERP_MAX_INIT_WAIT_TIME (MTLK_OSAL_SEC_IN_MIN * 10) /* 10 minutes */
#define MTLK_ERP_MIN_ON_OFF_TIME    10                          /* milliseconds */
#define MTLK_ERP_MAX_ON_OFF_TIME    MTLK_OSAL_MSEC_IN_SEC       /* 1 second */

static int
_mtlk_df_user_get_erp_cfg_by_intvec (uint32 *intvec, uint16 length, mtlk_coc_erp_cfg_t *erp_cfg)
{
  int res = MTLK_ERR_PARAMS;

  if (MTLK_ERP_CFG_SIZE != length) {
    ELOG_DD("Wrong parameter count given (%d), expected %d", length, MTLK_ERP_CFG_SIZE);
    return res;
  }

  erp_cfg->erp_enabled       = intvec[0];
  erp_cfg->initial_wait_time = intvec[1];
  erp_cfg->radio_on_time     = intvec[2];
  erp_cfg->radio_off_time    = intvec[3];
  erp_cfg->max_num_sta       = intvec[4];
  erp_cfg->rx_tp_max         = intvec[5];
  erp_cfg->tx_20_max_tp      = intvec[6];
  erp_cfg->tx_40_max_tp      = intvec[7];
  erp_cfg->tx_80_max_tp      = intvec[8];
  erp_cfg->tx_160_max_tp     = intvec[9];

  if (erp_cfg->initial_wait_time < MLTK_ERP_MIN_INIT_WAIT_TIME ||
      erp_cfg->initial_wait_time > MLTK_ERP_MAX_INIT_WAIT_TIME) {
    ELOG_DDD("Wrong Initial wait time given (%d), expected %d..%d",
            erp_cfg->initial_wait_time, MLTK_ERP_MIN_INIT_WAIT_TIME, MLTK_ERP_MAX_INIT_WAIT_TIME);
    return res;
  }

  if (erp_cfg->radio_off_time < MTLK_ERP_MIN_ON_OFF_TIME ||
      erp_cfg->radio_off_time > MTLK_ERP_MAX_ON_OFF_TIME) {
    ELOG_DDD("Wrong Radio OFF time given (%d), expected %d..%d",
            erp_cfg->radio_off_time, MTLK_ERP_MIN_ON_OFF_TIME, MTLK_ERP_MAX_ON_OFF_TIME);
    return res;
  }

  if (erp_cfg->radio_on_time < MTLK_ERP_MIN_ON_OFF_TIME ||
      erp_cfg->radio_on_time > MTLK_ERP_MAX_ON_OFF_TIME) {
    ELOG_DDD("Wrong Radio ON time given (%d), expected %d..%d",
            erp_cfg->radio_on_time, MTLK_ERP_MIN_ON_OFF_TIME, MTLK_ERP_MAX_ON_OFF_TIME);
    return res;
  }

  if (erp_cfg->erp_enabled != TRUE &&
      erp_cfg->erp_enabled != FALSE) {
    ELOG_DDD("Wrong ERP Enable value given (%d), expected %d or %d", erp_cfg->erp_enabled, FALSE, TRUE);
    return res;
  }

  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_erp_intvec_by_cfg (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_coc_erp_cfg_t *erp_cfg)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 10, *intvec_length);

  intvec[0] = erp_cfg->erp_enabled;
  intvec[1] = erp_cfg->initial_wait_time;
  intvec[2] = erp_cfg->radio_on_time;
  intvec[3] = erp_cfg->radio_off_time;
  intvec[4] = erp_cfg->max_num_sta;
  intvec[5] = erp_cfg->rx_tp_max;
  intvec[6] = erp_cfg->tx_20_max_tp;
  intvec[7] = erp_cfg->tx_40_max_tp;
  intvec[8] = erp_cfg->tx_80_max_tp;
  intvec[9] = erp_cfg->tx_160_max_tp;
}

static void
_mtlk_df_user_addr_vect_to_text (char *buf, mtlk_clpb_t *addr_vect_clpb, uint32 max_len, uint16 *length)
{
  uint32 used_len, out_cnt;
  uint32 addr_size;
  IEEE_ADDR *addr;

  ASSERT(buf != NULL);
  ASSERT(addr_vect_clpb != NULL);

  used_len = 0;
  out_cnt = 0;

  while(1)
  {
    addr = mtlk_clpb_enum_get_next(addr_vect_clpb, &addr_size);
    if (NULL == addr) break;

    MTLK_ASSERT(addr_size == sizeof(IEEE_ADDR));
    if (addr_size != sizeof(IEEE_ADDR))
      break;

    used_len += mtlk_snprintf(buf+used_len, max_len-used_len,
      "\n    " MAC_PRINTF_FMT,
      MAC_PRINTF_ARG(addr->au8Addr));

    out_cnt++;
  }

  if (out_cnt == 0)
  {
    used_len = mtlk_snprintf(buf, max_len, " No any address");
  }

  used_len += mtlk_snprintf(buf+used_len, max_len-used_len, "\n");

  *length = used_len;
  return;
}

/* Static Planner configuration */

typedef struct _mtlk_df_user_cfg_limit_t {
  uint32 min;
  uint32 max;
  char  *name;
} mtlk_df_user_cfg_limit_t;

BOOL __MTLK_IFUNC
mtlk_df_user_cfg_param_limits_valid (mtlk_df_user_cfg_limit_t *limits, const uint32 *params, int size)
{
    mtlk_df_user_cfg_limit_t *lim;
    uint32  value;
    BOOL    res = TRUE;
    int     i;

    for (i = 0, lim = limits; i < size; i++, lim++) {
      value = params[i];
      if ((value < lim->min) || (value > lim->max)) {
          ELOG_DSDDD("param[%2d] %s (%u) is not in range [%u ... %u]",
                    i, lim->name, value, lim->min, lim->max);
          res = FALSE;
      }
    }
    return res;
}

/* Configuration tables: Common and User */
#define _MTLK_STATIC_PLAN_COMMON_CFG_TABLE_ \
  _ENTRY_(  0,     1,  planOnOff                        )\
  _ENTRY_(  0, 32000,  maxTxopDuration                  )\
  _ENTRY_(  0,    15,  muSequenceType                   )\
  _ENTRY_(  0,     4,  startBwLimit                     )\
  _ENTRY_(  0,     7,  phaseFormat                      )\
  _ENTRY_(  1,     8,  numOfParticipatingStations       )\
  _ENTRY_(  0,     1,  muType                           )\
  _ENTRY_(  0,    15,  numberOfPhaseRepetitions         )\
  _ENTRY_(  0,  5400,  maximumPpduTransmissionTimeLimit )\
  _ENTRY_(  0,   255,  rfPower                          )\
  _ENTRY_(  0,     2,  dl_HeCp                          )\
  _ENTRY_(  0,     2,  dl_HeLtf                         )\
  _ENTRY_(  0,     2,  ul_HeCp                          )\
  _ENTRY_(  0,     2,  ul_HeLtf                         )\
  _ENTRY_(  0,     2,  tf_heGI_and_Ltf                  )\
  _ENTRY_(  0,  4095,  tf_Length                        )\
  _ENTRY_(  0,    59,  tf_psdu_rate                     )\
  _ENTRY_(  0, 65535,  rcr_heSigASpatialReuse           )\
  _ENTRY_(  0,     1,  rcr_Stbc                         )\
  _ENTRY_(  0,   255,  startingAid                      )\
  _ENTRY_(  0,     1,  multiplexingFlag                 )\
  _ENTRY_(  0,   255,  feedbackType                     )\

#define _MTLK_STATIC_PLAN_USER_CFG_TABLE_ \
  /* Per user phase  parameters */ \
  _ENTRY_(  0,   255,  uspStationIndexes                )\
  _ENTRY_(  0,    59,  dl_PsduRatePerUsp                )\
  _ENTRY_(  0,   511,  tid_alloc_bitmap                 )\
  _ENTRY_(  0,    59,  ul_PsduRatePerUsp                )\
  _ENTRY_(  0,     1,  dl_bfType                        )\
  _ENTRY_(  0,     7,  dl_subBandPerUsp                 )\
  _ENTRY_(  0,     9,  dl_startRuPerUsp                 )\
  _ENTRY_(  0,     6,  dl_ruSizePerUsp                  )\
  /* Trigger frame RCR per User */ \
  _ENTRY_(  0,     7,  tfStartingSS                     )\
  _ENTRY_(  0,     3,  tfMpduMuSpacingFactor            )\
  _ENTRY_(  0,     3,  tfPadding                        )\
  _ENTRY_(  0,   255,  targetRssi                       )\
  _ENTRY_(  0,     1,  ul_ldpc                          )\
  _ENTRY_(  0,    59,  ul_psduRate                      )\
  _ENTRY_(  0,     7,  ul_SubBand                       )\
  _ENTRY_(  0,    15,  ul_StartRU                       )\
  _ENTRY_(  0,     7,  ul_ruSize                        )\
  _ENTRY_(  0,   255,  SsAllocation                     )\
  _ENTRY_(  0,     1,  codingType_BCC_OR_LDPC           )\


/* Configuration tables: Common and User */
#define _ENTRY_(a, b, name)  { a, b, #name },

static mtlk_df_user_cfg_limit_t     _mtlk_static_plan_common_cfg_tab[] = {
  _MTLK_STATIC_PLAN_COMMON_CFG_TABLE_
};

static mtlk_df_user_cfg_limit_t     _mtlk_static_plan_user_cfg_tab[] = {
  _MTLK_STATIC_PLAN_USER_CFG_TABLE_
};
#undef _ENTRY_

static void
_mtlk_df_user_dump_limit_params (uint32 *params, mtlk_df_user_cfg_limit_t *cfg, int size)
{
  int i;
  for (i = 0; i < size; i++, cfg++, params++) {
    mtlk_osal_emergency_print("  [%2d] %32s: range [%u ... %5u] value 0x%04X (%u)",
                              i, cfg->name, cfg->min, cfg->max, *params, *params);
  }
}

static void
_mtlk_df_user_dump_static_plan_common_cfg (uint32 *params)
{
  mtlk_osal_emergency_print("--- Static Planner Common Config ---");
  _mtlk_df_user_dump_limit_params(params, _mtlk_static_plan_common_cfg_tab, ARRAY_SIZE(_mtlk_static_plan_common_cfg_tab));
}

static void
_mtlk_df_user_dump_static_plan_user_cfg (uint32 *params, int i)
{
  mtlk_osal_emergency_print("--- Static Planner User Config[%d] ---", i);
  _mtlk_df_user_dump_limit_params(params, _mtlk_static_plan_user_cfg_tab, ARRAY_SIZE(_mtlk_static_plan_user_cfg_tab));
}

#define _ENTRY_(a, b, name) { { \
        size_t szof = sizeof(cfg->name); \
        size_t offs = ((size_t)((void*)&cfg->name - (void*)cfg)); \
        MTLK_STATIC_ASSERT(0 == (offs & (szof - 1))); \
        cfg->name = \
            (4 == szof) ? HOST_TO_MAC32(*params) : \
            (2 == szof) ? HOST_TO_MAC16(*params) : \
            /* 1 */       (*params); \
        params++; } }

static int
_mtlk_df_user_fill_and_dump_static_plan_common_cfg (UMI_MU_PLAN_COMMON_CONFIGURATION *cfg, uint32 *params, int size)
{
    uint32 *values;
    BOOL    is_valid = TRUE;

    /* Filling the table for checking planOnOff */
    values = params;    /* Copy ptr to params because it will be modified by the filling */
    _MTLK_STATIC_PLAN_COMMON_CFG_TABLE_

    mtlk_osal_emergency_print("MuStaticPlan is %s", cfg->planOnOff ? "ON" : "Off");

    /* Dump and validation if plan is On */
    if (cfg->planOnOff) {
        _mtlk_df_user_dump_static_plan_common_cfg(values);
        is_valid = mtlk_df_user_cfg_param_limits_valid(_mtlk_static_plan_common_cfg_tab, values, size);
    } else { /* If plan is Off, reset numOfParticipatingStations */
        mtlk_osal_emergency_print("Apply numOfParticipatingStations=0 due to plan is Off");
        cfg->numOfParticipatingStations = 0;
    }

  return (is_valid ? MTLK_ERR_OK : MTLK_ERR_PARAMS);
}

static int
_mtlk_df_user_fill_static_plan_user_cfg (UMI_MU_PLAN_PER_USER_CONFIGURATION *cfg, uint32 *params, int size)
{
    /* Filling the table if input values are valid */
    if (mtlk_df_user_cfg_param_limits_valid(_mtlk_static_plan_user_cfg_tab, params, size)) {
        _MTLK_STATIC_PLAN_USER_CFG_TABLE_
        return MTLK_ERR_OK;
    } else {
        return MTLK_ERR_PARAMS;
    }
}

#undef _ENTRY_

static int
_mtlk_df_user_fill_static_plan_config_by_intvec (UMI_STATIC_PLAN_CONFIG *config, uint32 *data, unsigned length)
{
  UMI_MU_PLAN_COMMON_CONFIGURATION      *common_cfg;
  UMI_MU_PLAN_PER_USER_CONFIGURATION    *user_cfg;
  int   i, nof_users, size, remain;
  int   res;

  remain = length;

  memset(config, 0, sizeof(*config));

  /* Fill Common config */
  size = ARRAY_SIZE(_mtlk_static_plan_common_cfg_tab);
  if (remain < size) {
    ELOG_DD("Number of CommonConfig params (%d) too small, at least %d expected", remain, size);
    return MTLK_ERR_PARAMS;
  }

  common_cfg = &config->commonSection;
  res = _mtlk_df_user_fill_and_dump_static_plan_common_cfg(common_cfg, data, size);

  /* Don't parse/fill user configs if plan is Off */
  if (0 == common_cfg->planOnOff) {
    return MTLK_ERR_OK;
  }

  if (MTLK_ERR_OK != res) {
    ELOG_V("Invalid StaticPlanner CommonConfig");
    return res;
  }

  remain -= size;
  data   += size;

  /* Fill User config(s) */
  nof_users = common_cfg->numOfParticipatingStations;
  size = ARRAY_SIZE(_mtlk_static_plan_user_cfg_tab);
  if ((nof_users * size) != remain) {
    ELOG_DDD("Wrong number of UserConfig params: (%d * %d) != (%d)",
             nof_users, size, remain);
    return MTLK_ERR_PARAMS;
  }

  for (i = 0; i < nof_users; i++) {
    user_cfg = &config->perUserParameters[i];
    _mtlk_df_user_dump_static_plan_user_cfg(data, i);
    res = _mtlk_df_user_fill_static_plan_user_cfg(user_cfg, data, size);
    data += size;
    if (MTLK_ERR_OK != res) {
      ELOG_D("Invalid StaticPlanner UserConfig[%d]", i);
      return res;
    }
  }

  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_fill_nfrp_cfg_by_intvec (uint32 *intvec, uint16 intvec_length, UMI_NFRP_CONFIG *nfrp_cfg)
{
    if (intvec_length != 2) {
      ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
      return MTLK_ERR_PARAMS;
    }
    else {

      if ((intvec[0] != 0) && (intvec[0] != 1)) {
        ELOG_D("Incorrect setting %u, nfrpSupport: 0 - disabled, 1 - Enabled", intvec[0]);
        return MTLK_ERR_VALUE;
      }

      if(intvec[1] <= MAX_UINT8)
      {
        nfrp_cfg->nfrpSupport   = (uint8) intvec[0];
        nfrp_cfg->nfrpThreshold = (uint8) intvec[1];
        return MTLK_ERR_OK;
      } else {
        ELOG_DD("Incorrect setting %u, nfrpThreshold valid value is (0-%d):",
                intvec[1], MAX_UINT8);
        return MTLK_ERR_VALUE;
      }
    }
}


static void
_mtlk_df_user_get_intvec_by_pvt_sensor_params (int32 *intvec, uint16 max_length, uint16 *intvec_length, const iwpriv_pvt_t *pvt_params)
{
    WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 2, *intvec_length);

    intvec[0] = (int32)pvt_params->voltage;
    intvec[1] = (int32)pvt_params->temperature;
}

static void
_mtlk_df_user_get_text_by_phy_power_data (char *buffer, mtlk_phy_inband_power_data_t *power_data,
                                          size_t max_len, uint16 *length)
{
  size_t used_len = 0;
  int noise, sgain;
  unsigned i;

  MTLK_STATIC_ASSERT(ARRAY_SIZE(power_data->noise_estim) == ARRAY_SIZE(power_data->system_gain));

  /* Data per antenna are in 0.5 dB units */
  for (i = 0; i < MTLK_ARRAY_SIZE(power_data->noise_estim); i++) {
    noise = (power_data->noise_estim[i] >> 1); /* integer part */
    sgain = (power_data->system_gain[i] >> 1); /* integer part */

    used_len += mtlk_snprintf(buffer + used_len, max_len - used_len,
                  "\nAnt %u: Noise = %d [dBm], SystemGain = %d [dB]",
                  i, noise, sgain);

    if (used_len >= max_len) {
      used_len = max_len;
      break;
    }
  }

  *length = used_len;
}

static void
_mtlk_df_user_get_intvec_by_scan_params(uint32 *intvec, uint16 max_length, uint16 *intvec_length, const iwpriv_scan_params_t *scan_params)
{
  MTLK_STATIC_ASSERT(6 == NUM_IWPRIV_SCAN_PARAMS);
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, NUM_IWPRIV_SCAN_PARAMS, *intvec_length);

  intvec[0] = scan_params->passive_scan_time;
  intvec[1] = scan_params->active_scan_time;
  intvec[2] = scan_params->num_probe_reqs;
  intvec[3] = scan_params->probe_req_interval;
  intvec[4] = scan_params->passive_scan_valid_time;
  intvec[5] = scan_params->active_scan_valid_time;
}

static void
_mtlk_df_user_get_intvec_by_scan_params_bg(uint32 *intvec, uint16 max_length, uint16 *intvec_length, const iwpriv_scan_params_bg_t *scan_params_bg)
{
  MTLK_STATIC_ASSERT(10 == NUM_IWPRIV_SCAN_PARAMS_BG);
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, NUM_IWPRIV_SCAN_PARAMS_BG, *intvec_length);

  intvec[0] = scan_params_bg->passive_scan_time;
  intvec[1] = scan_params_bg->active_scan_time;
  intvec[2] = scan_params_bg->num_probe_reqs;
  intvec[3] = scan_params_bg->probe_req_interval;
  intvec[4] = scan_params_bg->num_chans_in_chunk;
  intvec[5] = scan_params_bg->chan_chunk_interval;
  intvec[6] = scan_params_bg->chan_chunk_interval_busy;
  intvec[7] = scan_params_bg->window_slice;
  intvec[8] = scan_params_bg->window_slice_overlap;
  intvec[9] = scan_params_bg->cts_to_self_duration;
}

static void
_df_user_get_intvec_by_preamble_punct_cca_ov_cfg (uint32 *data_out, uint16 max_length,
                                           uint16 *data_out_len, UMI_PREAMBLE_PUNCT_CCA_OVERRIDE  *data_in)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, WAVE_PREAMBLE_PUNCTURE_CFG_SIZE, *data_out_len);

  data_out[0] = (uint32)data_in->ccaOverride;
  data_out[1] = (uint32)data_in->ccaBitmap;
  data_out[2] = (uint32)data_in->forceFreeBands;
}

#define SCAN_PASSIVE_DWELL_MIN      20
#define SCAN_PASSIVE_DWELL_MAX      1000
#define SCAN_ACTIVE_DWELL_MIN       5
#define SCAN_ACTIVE_DWELL_MAX       1000
#define SCAN_NUM_PROBE_REQ_MIN      0
#define SCAN_NUM_PROBE_REQ_MAX      10
#define SCAN_PROBE_REQ_INT_MIN      1
#define SCAN_PROBE_REQ_INT_MAX      10
#define SCAN_CHANS_IN_CHUNK_MIN     1
#define SCAN_CHANS_IN_CHUNK_MAX     NUM_CHANS_MAX
#define SCAN_CHAN_CHUNK_INT_MIN     100
#define SCAN_CHAN_CHUNK_INT_MAX     MTLK_OSAL_MSEC_IN_MIN
#define SCAN_CTS_TO_SELF_MIN        0
#define SCAN_CTS_TO_SELF_MAX        32

static int _mtlk_df_user_validate_common_scan_params (uint32 passive_scan_time,
                                                      uint32 active_scan_time,
                                                      uint32 num_probe_reqs,
                                                      uint32 probe_req_interval)
{
  if ((passive_scan_time < SCAN_PASSIVE_DWELL_MIN) || (passive_scan_time > SCAN_PASSIVE_DWELL_MAX)) {
    ELOG_DDD("Incorrect passive dwell time %u, expected %u..%u",
             passive_scan_time, SCAN_PASSIVE_DWELL_MIN, SCAN_PASSIVE_DWELL_MAX);
    return MTLK_ERR_VALUE;
  }

  if ((active_scan_time < SCAN_ACTIVE_DWELL_MIN) || (active_scan_time > SCAN_ACTIVE_DWELL_MAX)) {
    ELOG_DDD("Incorrect active dwell time %u, expected %u..%u",
             active_scan_time, SCAN_ACTIVE_DWELL_MIN, SCAN_ACTIVE_DWELL_MAX);
    return MTLK_ERR_VALUE;
  }

  if ((num_probe_reqs < SCAN_NUM_PROBE_REQ_MIN) || (num_probe_reqs > SCAN_NUM_PROBE_REQ_MAX)) {
    ELOG_DDD("Incorrect number of probe req %u, expected %u..%u",
             num_probe_reqs, SCAN_NUM_PROBE_REQ_MIN, SCAN_NUM_PROBE_REQ_MAX);
    return MTLK_ERR_VALUE;
  }

  if ((probe_req_interval < SCAN_PROBE_REQ_INT_MIN) || (probe_req_interval > SCAN_PROBE_REQ_INT_MAX)) {
    ELOG_DDD("Incorrect probe req interval %u, expected %u..%u",
             probe_req_interval, SCAN_PROBE_REQ_INT_MIN, SCAN_PROBE_REQ_INT_MAX);
    return MTLK_ERR_VALUE;
  }

  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_get_scan_params_by_intvec (const uint32 *intvec, uint32 intvec_length, iwpriv_scan_params_t *scan_params)
{
  int res = MTLK_ERR_OK;

  if (intvec_length != NUM_IWPRIV_SCAN_PARAMS) {
    ELOG_DD("%i parameters given but %i are needed", intvec_length, NUM_IWPRIV_SCAN_PARAMS);
    return MTLK_ERR_PARAMS;
  }

  if (MTLK_ERR_OK != _mtlk_df_user_validate_common_scan_params(intvec[0], intvec[1], intvec[2], intvec[3]))
    return MTLK_ERR_VALUE;

  scan_params->passive_scan_time       = intvec[0];
  scan_params->active_scan_time        = intvec[1];
  scan_params->num_probe_reqs          = intvec[2];
  scan_params->probe_req_interval      = intvec[3];

  /* TODO: passive and active scan times are not compatible with 
   * our smart ACS, silently ignore values and set to 0 */
  scan_params->passive_scan_valid_time = 0;
  scan_params->active_scan_valid_time  = 0;

  return res;
}

static int _mtlk_df_user_validate_advertise_btwt_common_params (uint32 number_of_sps_to_add,
                                                                uint32 wake_duration_unit,
                                                                unsigned length)
{
  u8 expected_length;

  if (!IS_VALID_RANGE(number_of_sps_to_add, MIN_NUM_OF_BC_TWT_SP_TO_ADD, MAX_NUM_OF_BC_TWT_SP_TO_ADD)) {
    ELOG_DDD("Incorrect number of SPs %u, expected %u..%u",
             number_of_sps_to_add, MIN_NUM_OF_BC_TWT_SP_TO_ADD, MAX_NUM_OF_BC_TWT_SP_TO_ADD);
    return MTLK_ERR_VALUE;
  }

  expected_length = (number_of_sps_to_add*NUM_ADVERTISE_BTWT_SP_PARAMS) + NUM_ADVERTISE_BTWT_COMMON_PARAMS;
  if (length != expected_length) {
    ELOG_DDD("Incorrect length %u for %u schedule(s), should be %u", length, number_of_sps_to_add,
             expected_length);
    return MTLK_ERR_PARAMS;
  }

  if (!IS_VALID_RANGE(wake_duration_unit, MIN_WAKE_DURATION_UNIT_256US, MAX_WAKE_DURATION_UNIT_1024US)) {
    ELOG_DDD("Incorrect wake duration unit %u, expected %u..%u",
             wake_duration_unit, MIN_WAKE_DURATION_UNIT_256US, MAX_WAKE_DURATION_UNIT_1024US);
    return MTLK_ERR_VALUE;
  }
  return MTLK_ERR_OK;
}

static int _mtlk_df_user_validate_advertise_btwt_sp_params (uint32 broadcast_twt_id,
                                                            uint32 flow_type,
                                                            uint32 trigger_type,
                                                            uint32 twt_wake_duration,
                                                            uint32 twt_wake_interval_mantissa,
                                                            uint32 wake_interval_exp)
{
  if (!IS_VALID_RANGE(broadcast_twt_id, MIN_BROADCAST_TWT_ID, MAX_BROADCAST_TWT_ID)) {
    ELOG_DDD("Incorrect broadcast TWT ID %u, expected %u..%u",
             broadcast_twt_id, MIN_BROADCAST_TWT_ID, MAX_BROADCAST_TWT_ID);
    return MTLK_ERR_VALUE;
  }

  if (!IS_VALID_RANGE(flow_type, MIN_FLOW_TYPE_ANNOUNCED, MAX_FLOW_TYPE_UNANNOUNCED)) {
    ELOG_DDD("Incorrect flow type %u, expected %u..%u",
             flow_type, MIN_FLOW_TYPE_ANNOUNCED, MAX_FLOW_TYPE_UNANNOUNCED);
    return MTLK_ERR_VALUE;
  }

  if (!IS_VALID_RANGE(trigger_type, MIN_TRIGGER_UNTRIGGERED, MAX_TRIGGER_TRIGGERED)) {
    ELOG_DDD("Incorrect trigger type %u, expected %u..%u",
             trigger_type, MIN_TRIGGER_UNTRIGGERED, MAX_TRIGGER_TRIGGERED);
    return MTLK_ERR_VALUE;
  }

  if (!IS_VALID_RANGE(twt_wake_duration, MIN_WAKE_DURATION, MAX_WAKE_DURATION)) {
    ELOG_DDD("Incorrect TWT wake duration %u, expected %u..%u",
             twt_wake_duration, MIN_WAKE_DURATION, MAX_WAKE_DURATION);
    return MTLK_ERR_VALUE;
  }

  if (!IS_VALID_RANGE(twt_wake_interval_mantissa, MIN_WAKE_INTERVAL_MANTISSA, MAX_WAKE_INTERVAL_MANTISSA)) {
    ELOG_DDD("Incorrect TWT wake interval mantissa %u, expected %u..%u",
             twt_wake_interval_mantissa, MIN_WAKE_INTERVAL_MANTISSA, MAX_WAKE_INTERVAL_MANTISSA);
    return MTLK_ERR_VALUE;
  }

  if (!IS_VALID_RANGE(wake_interval_exp, MIN_WAKE_INTERVAL_EXP, MAX_WAKE_INTERVAL_EXP)) {
    ELOG_DDD("Incorrect wake interval exponent %u, expected %u..%u",
             wake_interval_exp, MIN_WAKE_INTERVAL_EXP, MAX_WAKE_INTERVAL_EXP);
    return MTLK_ERR_VALUE;
  }
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_fill_advertise_btwt_sp_config_by_intvec (UMI_ADVERTISE_BC_TWT_SP *adv_btwt_sp, const uint32 *intvec, unsigned length)
{
  u8 num_sp, idx;
  u16 wake_int_mantissa;
  u32 btwt_id_used = 0;
  u32 mask = 0;

  if (!IS_VALID_RANGE(length, MIN_NUM_ADVERTISE_BTWT_PARAMS, MAX_NUM_ADVERTISE_BTWT_PARAMS)) {
    ELOG_DDD("Incorrect length %u, expected %u..%u depending on number of schedules",
             length, MIN_NUM_ADVERTISE_BTWT_PARAMS, MAX_NUM_ADVERTISE_BTWT_PARAMS);
    return MTLK_ERR_PARAMS;
  }

  if (MTLK_ERR_OK != _mtlk_df_user_validate_advertise_btwt_common_params(intvec[0], intvec[1], length))
    return MTLK_ERR_VALUE;

  for (num_sp = 0; num_sp < intvec[0]; num_sp++) {
    idx = (ADVERTISE_BTWT_SP_PARAMS_OFFSET + (num_sp*NUM_ADVERTISE_BTWT_SP_PARAMS));
    if (MTLK_ERR_OK != _mtlk_df_user_validate_advertise_btwt_sp_params(intvec[idx], intvec[idx+1], intvec[idx+2], intvec[idx+3], intvec[idx+4], intvec[idx+5]))
      return MTLK_ERR_VALUE;
  }

  adv_btwt_sp->getSetOperation = API_SET_OPERATION;
  adv_btwt_sp->numberOfSpsToAdd = intvec[0];
  adv_btwt_sp->wakeDurationUnit = intvec[1];
  for (num_sp = 0; num_sp < intvec[0]; num_sp++) {
    idx = (ADVERTISE_BTWT_SP_PARAMS_OFFSET + (num_sp*NUM_ADVERTISE_BTWT_SP_PARAMS));

    /* check for any duplicate BTWT ID in the received command, and reject if found */
    mask = (1 << intvec[idx]);
    if (btwt_id_used & mask) {
      ELOG_D("Duplicate Bcast TWT ID %u received in the same command", intvec[idx]);
      return MTLK_ERR_PARAMS;
    } else {
      btwt_id_used |= mask;
    }

    adv_btwt_sp->bcTwtSpParams[num_sp].broadcastTwtId = intvec[idx];
    adv_btwt_sp->bcTwtSpParams[num_sp].flowType = intvec[idx+1];
    adv_btwt_sp->bcTwtSpParams[num_sp].trigger = intvec[idx+2];
    adv_btwt_sp->bcTwtSpParams[num_sp].twtWakeDuration = intvec[idx+3];
    wake_int_mantissa = intvec[idx+4];
    adv_btwt_sp->bcTwtSpParams[num_sp].twtWakeIntervalMantissa = HOST_TO_MAC16(wake_int_mantissa);
    adv_btwt_sp->bcTwtSpParams[num_sp].wakeIntervalExponent = intvec[idx+5];
  }
  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_intvec_by_advertised_btwt_sp (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const UMI_ADVERTISE_BC_TWT_SP *adv_btwt_sp)
{
  u8 num_sp, idx;
  u16 used_len = 0;
  intvec[0] = adv_btwt_sp->numberOfSpsToAdd;
  intvec[1] = adv_btwt_sp->wakeDurationUnit;
  for (num_sp = 0; num_sp < intvec[0]; num_sp++) {
    idx = (ADVERTISE_BTWT_SP_PARAMS_OFFSET + (num_sp*NUM_ADVERTISE_BTWT_SP_PARAMS));
    intvec[idx] = adv_btwt_sp->bcTwtSpParams[num_sp].broadcastTwtId;
    intvec[idx+1] = adv_btwt_sp->bcTwtSpParams[num_sp].flowType;
    intvec[idx+2] = adv_btwt_sp->bcTwtSpParams[num_sp].trigger;
    intvec[idx+3] = adv_btwt_sp->bcTwtSpParams[num_sp].twtWakeDuration;
    intvec[idx+4] = MAC_TO_HOST16(adv_btwt_sp->bcTwtSpParams[num_sp].twtWakeIntervalMantissa);
    intvec[idx+5] = adv_btwt_sp->bcTwtSpParams[num_sp].wakeIntervalExponent;
  }
  used_len = NUM_ADVERTISE_BTWT_COMMON_PARAMS + (adv_btwt_sp->numberOfSpsToAdd * NUM_ADVERTISE_BTWT_SP_PARAMS);
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, used_len, *intvec_length);
}

static int
_mtlk_df_user_fill_terminate_btwt_sp_config_by_intvec (UMI_TERMINATE_BC_TWT_SP *trm_btwt_sp, uint32 *intvec, unsigned length)
{
  if (length != NUM_TERMINATE_BTWT_PARAMS) {
    ELOG_DD("Incorrect length %u, should be %u",
             length, NUM_TERMINATE_BTWT_PARAMS);
    return MTLK_ERR_PARAMS;
  }

  if (!IS_VALID_RANGE(intvec[0], MIN_TWT_PERSISTENCE, MAX_TWT_PERSISTENCE)) {
    ELOG_DDD("Incorrect BTWT persistence %u, expected %u..%u",
             intvec[0], MIN_TWT_PERSISTENCE, MAX_TWT_PERSISTENCE);
    return MTLK_ERR_VALUE;
  }

  trm_btwt_sp->broadcastTwtPersistence = intvec[0];
  return MTLK_ERR_OK;
}

static int _mtlk_df_user_validate_twt_teardown_sp_params (uint32 agreement, uint32 id)
{

  if (!IS_VALID_RANGE(agreement, MIN_AGREEMENT_TYPE_INDIVIDUAL, MAX_AGREEMENT_TYPE_BROADCAST)) {
    ELOG_DDD("Incorrect agreement type %u, expected %u..%u",
             agreement, MIN_AGREEMENT_TYPE_INDIVIDUAL, MAX_AGREEMENT_TYPE_BROADCAST);
    return MTLK_ERR_VALUE;
  }

  if ((agreement == MIN_AGREEMENT_TYPE_INDIVIDUAL && !IS_VALID_RANGE(id, MIN_INDIVIDUAL_FLOW_ID, MAX_INDIVIDUAL_FLOW_ID)) ||
      (agreement == MAX_AGREEMENT_TYPE_BROADCAST && !IS_VALID_RANGE(id, MIN_BROADCAST_TWT_ID, MAX_BROADCAST_TWT_ID))) {
    ELOG_DDD("Incorrect TWT id %u, expected %u..%u",
             id, (agreement == MIN_AGREEMENT_TYPE_INDIVIDUAL) ? MIN_INDIVIDUAL_FLOW_ID : MIN_BROADCAST_TWT_ID,
             (agreement == MIN_AGREEMENT_TYPE_INDIVIDUAL) ? MAX_INDIVIDUAL_FLOW_ID : MAX_BROADCAST_TWT_ID);
    return MTLK_ERR_VALUE;
  }
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_fill_tx_twt_teardown_config_by_intvec (UMI_SEND_TWT_TEARDOWN_FRAME *tx_twt_teardown, uint32 *intvec, unsigned length)
{
  u8 all_twt;
  if (length != MIN_NUM_TX_TWT_TEARDOWN_PARAMS && length != MAX_NUM_TX_TWT_TEARDOWN_PARAMS) {
    ELOG_DDD("Incorrect length %u, should be either %u or %u",
            length, MIN_NUM_TX_TWT_TEARDOWN_PARAMS, MAX_NUM_TX_TWT_TEARDOWN_PARAMS);
    return MTLK_ERR_PARAMS;
  }

  if (!IS_VALID_RANGE(intvec[0], WAVE_MIN_STA_ID, WAVE_MAX_STA_ID)) {
    ELOG_DDD("Incorrect STA ID %u, expected %u..%u",
             intvec[0], WAVE_MIN_STA_ID, WAVE_MAX_STA_ID);
    return MTLK_ERR_VALUE;
  }

  if (!IS_VALID_RANGE(intvec[1], MIN_ALL_TWT, MAX_ALL_TWT)) {
    ELOG_DDD("Incorrect all TWT %u, expected %u..%u",
            intvec[1], MIN_ALL_TWT, MAX_ALL_TWT);
    return MTLK_ERR_VALUE;
  }

  all_twt = intvec[1];

  if (!all_twt && length == MIN_NUM_TX_TWT_TEARDOWN_PARAMS) {
    ELOG_DDD("Incorrect length %u for allTwt %u, expected %u",
             length, all_twt, MAX_NUM_TX_TWT_TEARDOWN_PARAMS);
    return MTLK_ERR_PARAMS;
  }

  if (!all_twt && (length == MAX_NUM_TX_TWT_TEARDOWN_PARAMS) &&
      MTLK_ERR_OK != _mtlk_df_user_validate_twt_teardown_sp_params(intvec[2], intvec[3]))
    return MTLK_ERR_VALUE;

  tx_twt_teardown->staId = intvec[0];
  tx_twt_teardown->alltwt = intvec[1];
  /* Allow user to configure min required parames in-case allTWT is enabled */
  if (all_twt) {
    tx_twt_teardown->agreementType = DEFAULT_AGREEMENT_TYPE;
    tx_twt_teardown->broadcastIdOrFlowId = DEFAULT_TWT_ID;
  } else {
    tx_twt_teardown->agreementType = intvec[2];
    tx_twt_teardown->broadcastIdOrFlowId = intvec[3];
  }
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_get_scan_params_bg_by_intvec (const uint32 *intvec, uint32 intvec_length, iwpriv_scan_params_bg_t *scan_params_bg)
{
  int res = MTLK_ERR_OK;

  if (intvec_length != NUM_IWPRIV_SCAN_PARAMS_BG) {
    ELOG_DD("%i parameters given but %i are needed", intvec_length, NUM_IWPRIV_SCAN_PARAMS_BG);
    return MTLK_ERR_PARAMS;
  }

  if (MTLK_ERR_OK != _mtlk_df_user_validate_common_scan_params(intvec[0], intvec[1], intvec[2], intvec[3]))
    return MTLK_ERR_VALUE;

  if ((intvec[4] < SCAN_CHANS_IN_CHUNK_MIN) || (intvec[4] > SCAN_CHANS_IN_CHUNK_MAX)) {
    ELOG_DDD("Incorrect number of channels in chunk %u, expected %u..%u",
             intvec[4], SCAN_CHANS_IN_CHUNK_MIN, SCAN_CHANS_IN_CHUNK_MAX);
    return MTLK_ERR_VALUE;
  }

  if ((intvec[5] < SCAN_CHAN_CHUNK_INT_MIN) || (intvec[5] > SCAN_CHAN_CHUNK_INT_MAX)) {
    ELOG_DDD("Incorrect BG scan break duration %u, expected %u..%u",
             intvec[5], SCAN_CHAN_CHUNK_INT_MIN, SCAN_CHAN_CHUNK_INT_MAX);
    return MTLK_ERR_VALUE;
  }

  if ((intvec[6] < SCAN_CHAN_CHUNK_INT_MIN) || (intvec[6] > SCAN_CHAN_CHUNK_INT_MAX)) {
    ELOG_DDD("Incorrect BG scan break busy duration %u, expected %u..%u",
             intvec[6], SCAN_CHAN_CHUNK_INT_MIN, SCAN_CHAN_CHUNK_INT_MAX);
    return MTLK_ERR_VALUE;
  }

  if (intvec[7] >= intvec[0]) {
    ELOG_D("Incorrect BG scan break window slice %u, expected window slice < passive scan",
             intvec[7]);
    return MTLK_ERR_VALUE;
  }

  if (intvec[8] >= intvec[7]) {
    ELOG_DDD("Incorrect BG scan break windows slice overlap %u, expected %u < %u",
             intvec[8], SCAN_CHAN_CHUNK_INT_MIN, SCAN_CHAN_CHUNK_INT_MAX);
    return MTLK_ERR_VALUE;
  }

  if(((intvec[2] * intvec[3] * intvec[4]) > intvec[1]) || ((intvec[2] * intvec[3] * intvec[4]) > intvec[7])) {
    ELOG_V("Incorrect BG scan number of probe requests, probe request interval and number of channels in chunk");
    return MTLK_ERR_VALUE;
  }

  if (intvec[9] > SCAN_CTS_TO_SELF_MAX) {
    ELOG_DDD("Incorrect BG scan CTS-to-self duration %u, expected %u..%u",
             intvec[9], SCAN_CTS_TO_SELF_MIN, SCAN_CTS_TO_SELF_MAX);
    return MTLK_ERR_VALUE;
  }

  scan_params_bg->passive_scan_time        = intvec[0];
  scan_params_bg->active_scan_time         = intvec[1];
  scan_params_bg->num_probe_reqs           = intvec[2];
  scan_params_bg->probe_req_interval       = intvec[3];
  scan_params_bg->num_chans_in_chunk       = intvec[4];
  scan_params_bg->chan_chunk_interval      = intvec[5];
  scan_params_bg->chan_chunk_interval_busy = intvec[6];
  scan_params_bg->window_slice             = intvec[7];
  scan_params_bg->window_slice_overlap     = intvec[8];
  scan_params_bg->cts_to_self_duration     = intvec[9];

  return res;
}

static int
_mtlk_df_user_fill_fixed_ltf_and_gi_by_intvec(uint32 *intvec, uint16 intvec_length, UMI_FIXED_LTF_AND_GI_REQ *fixed_ltf_and_gi)
{
    if (intvec_length != 2) {
      ELOG_D("Need exactly 2 arguments (length=%u)", intvec_length);
      return MTLK_ERR_PARAMS;
    }
    else {

      if ((intvec[0] != 0) && (intvec[0] != 1)) {
          ELOG_D("Incorrect setting %u, isAuto: 0 - fixed, 1 - Auto (default)", intvec[0]);
          return MTLK_ERR_VALUE;
      }

      if(intvec[1] < 6) /* TODO: Use constants here from UMI ? */
      {
          fixed_ltf_and_gi->isAuto = (uint8) intvec[0];
          fixed_ltf_and_gi->ltfAndGi = (uint8) intvec[1];
          return MTLK_ERR_OK;
      } else {
          ELOG_DDD("Incorrect setting %u, ltfAndGi valid value is (%d-%d):",
                                intvec[1], 0, 6 - 1); /*TODO: Use constants here from UMI ?*/
          return MTLK_ERR_VALUE;
      }
    }
}

static void
_mtlk_df_user_get_intvec_by_fixed_ltf_and_gi (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const UMI_FIXED_LTF_AND_GI_REQ *fixed_ltf_and_gi)
{
    WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 2, *intvec_length);

    intvec[0] = (uint32)fixed_ltf_and_gi->isAuto;
    intvec[1] = (uint32)fixed_ltf_and_gi->ltfAndGi;
}

/************************* DEBUG SET/GET FUNCTIONS *************************/
#ifdef CONFIG_WAVE_DEBUG

static int
_mtlk_df_user_set_counters_src_by_int (mtlk_wlan_counters_src_cfg_t *counters_src_out, uint32 data_in)
{
  int res = MTLK_ERR_OK;
  if (data_in > 2) {
    ELOG_D("The command argument (%d) is invalid. Allowed values are 0, 1 or 2.", data_in);
    res = MTLK_ERR_VALUE;
  } else {
    counters_src_out->src = data_in;
  }
  return res;
}

static int
_mtlk_df_user_fill_fixed_pwr_params (uint32 *intvec, uint16 intvec_length, FIXED_POWER *fixed_pwr_params)
{
  if (intvec_length != 4) {
    ELOG_D("Need exactly 4 arguments (length=%u)", intvec_length);
    return MTLK_ERR_PARAMS;
  }

  if (intvec[0] > MAX_UINT8 ||
      intvec[1] > MAX_UINT8 ||
      intvec[2] > MAX_UINT8 ||
      intvec[3] > MAX_UINT8) {
    ELOG_D("Wrong value/s given, should be in range 0..%d", MAX_UINT8);
    return MTLK_ERR_PARAMS;
  }

  fixed_pwr_params->vapId = intvec[0];
  fixed_pwr_params->stationId = intvec[1];
  fixed_pwr_params->powerVal = intvec[2];
  fixed_pwr_params->changeType = intvec[3];

  return MTLK_ERR_OK;
}

static void
_mtlk_df_user_get_intvec_by_fixed_pwr_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length, FIXED_POWER *fixed_pwr_params)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 4, *intvec_length);

  if (!MTLK_PARAM_DB_VALUE_IS_INVALID(fixed_pwr_params->vapId)) {
    intvec[0] = fixed_pwr_params->vapId;
    intvec[1] = fixed_pwr_params->stationId;
    intvec[2] = fixed_pwr_params->powerVal;
    intvec[3] = fixed_pwr_params->changeType;
  }
  else {
    ELOG_V("Fixed Power not Set, Getter invalid!");
    *intvec_length = 0;
  }
}

static
void _mtlk_df_user_get_intvec_by_tasklet_limits(uint32 *intvec, const uint16 max_length, uint16 *intvec_length, const struct tasklet_limits *tl)
{
  wave_memcpy(intvec, max_length * INT_ITEM_SIZE, tl, sizeof(*tl));
  *intvec_length = 5;
}

static
int _mtlk_df_user_get_tasklet_limits_by_intvec(const uint32 *intvec, uint16 intvec_length, struct tasklet_limits *tl)
{
  if (intvec_length != 5)
  {
    ELOG_D("Incorrect tasklet limits vector length %hu, need 5", intvec_length);
    return MTLK_ERR_PARAMS;
  }

  wave_memcpy(tl, sizeof(*tl), intvec, sizeof(*tl));
  return MTLK_ERR_OK;
}

static int
_mtlk_df_user_genl_int_get_cfg (mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params, uint16* length)
{
  if(PRM_ID_GENL_FAMILY_ID == subcmd) {
    MTLK_ASSERT(MTLK_IW_PRIV_TYPE(params) == INT_ITEM);
    *(uint32*)data = mtlk_genl_family_id;
    *length = 1;
    return MTLK_ERR_OK;
  } else {
    return MTLK_ERR_NOT_HANDLED;
  }
}

static int
_mtlk_df_user_print_ta_debug_info(char *buffer, size_t len, mtlk_ta_debug_info_cfg_t *debug_info)
{
  uint32 i;
  uint32 length=0;
  uint32 nof_crits;
  uint32 nof_stas;

  nof_crits = debug_info->nof_crit;
  length += mtlk_snprintf(buffer+length, len-length,   "\n");
  length += mtlk_snprintf(buffer+length, len-length,   "==============================================================================\n");
  length += mtlk_snprintf(buffer+length, len-length,   "                                   CRITERIA (%d)\n", nof_crits);
  length += mtlk_snprintf(buffer+length, len-length,   "------------------------------------------------------------------------------\n");
  length += mtlk_snprintf(buffer+length, len-length,   " ID | Signature |   FCN    | Callback | Context | Count | Period              \n");
  length += mtlk_snprintf(buffer+length, len-length,   "------------------------------------------------------------------------------\n");
  for (i=0; i < nof_crits; ++i) {
    mtlk_ta_crit_t *crit = &debug_info->crit[i];
    length += mtlk_snprintf(buffer+length, len-length, " %02u %10X  %10p %10p %10X %4u %8u \n",
                       crit->id, crit->signature, crit->fcn, crit->clb,
                            (uint32)crit->clb_ctx, crit->tmr_cnt, crit->tmr_period);
  }

  nof_stas = debug_info->nof_sta_wss;
  length += mtlk_snprintf(buffer+length, len-length,   "==============================================================================\n");
  length += mtlk_snprintf(buffer+length, len-length,   "                                   STA WSS (%d)\n", nof_stas);
  length += mtlk_snprintf(buffer+length, len-length,   "------------------------------------------------------------------------------\n");
  length += mtlk_snprintf(buffer+length, len-length,   "                 |       |          RX             |           TX             \n");
  length += mtlk_snprintf(buffer+length, len-length,   "       STA       | Valid |   Prev    |    Delta    |    Prev    |   Delta     \n");
  length += mtlk_snprintf(buffer+length, len-length,   "------------------------------------------------------------------------------\n");
  for (i=0; i < MIN(nof_stas, ARRAY_SIZE(debug_info->sta_wss)); ++i) {
    mtlk_ta_sta_wss_t *wss = &debug_info->sta_wss[i];
    mtlk_ta_wss_counter_t *rx = &wss->coc_rx_bytes;
    mtlk_ta_wss_counter_t *tx = &wss->coc_tx_bytes;
    length += mtlk_snprintf(buffer+length, len-length, MAC_PRINTF_FMT "  %c %10u    %10u   %10u   %10u\n",
                            MAC_PRINTF_ARG(wss->addr.au8Addr),
                            wss->coc_wss_valid?'Y':'N',
                            rx->prev, rx->delta,
                            tx->prev, tx->delta);
  }
  length += mtlk_snprintf(buffer+length, len-length,   "==============================================================================\n");

  return length;
}

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
static void
_mtlk_df_user_get_intvec_by_pcoc_params (uint32 *intvec, uint16 max_length, uint16 *intvec_length, const mtlk_pcoc_params_t *params)
{
  WAVE_CHECK_AND_SET_INTVEC_LENGTH(max_length, 4, *intvec_length);

  intvec[0] = params->interval_low2high;
  intvec[1] = params->interval_high2low;
  intvec[2] = params->limit_lower;
  intvec[3] = params->limit_upper;
}


static int
_mtlk_df_user_get_pcoc_params_by_intvec (const uint32 *intvec, uint16 intvec_length, mtlk_pcoc_params_t *params)
{
  int res = MTLK_ERR_PARAMS;

  memset(params, 0, sizeof(mtlk_pcoc_params_t));

  if (intvec_length != 4) {
    ELOG_D("Incorrect vector length. length(%u)", intvec_length);
  }
  else {
    params->interval_low2high = intvec[0];
    params->interval_high2low = intvec[1];
    params->limit_lower = intvec[2];
    params->limit_upper = intvec[3];

    res = MTLK_ERR_OK;
  }

  return res;
}
#endif /* CPTCFG_IWLWAV_PMCU_SUPPORT */

static int
_mtlk_df_user_fill_debug_nop(mtlk_11h_ap_cfg_t *dot11_cfg, uint32 data)
{
  if (data > 6000) {
    ELOG_V("Incorrect debug Non Occupancy Period. Should be in range from 0 to 6000 minutes (0 for default kernel NOP time)");
    return MTLK_ERR_VALUE;
  }

  dot11_cfg->debugNOP = data;
  return MTLK_ERR_OK;
}


static int
_mtlk_df_user_do_simple_cli (const uint32 *intvec, uint16 intvec_length, UMI_DBG_CLI_REQ *DbgCliReq)
{
  int res = MTLK_ERR_PARAMS;

  if ((1 > intvec_length) || (4 < intvec_length)) {
    ELOG_D("Incorrect vector length. length(%u)", intvec_length);
  } else {
    memset(DbgCliReq, 0, sizeof(UMI_DBG_CLI_REQ));
    DbgCliReq->numOfArgumets = intvec_length - 1;
    DbgCliReq->action = intvec[0];
    DbgCliReq->data1 =  intvec[1];
    DbgCliReq->data2 =  intvec[2];
    DbgCliReq->data3 =  intvec[3];
    res              = MTLK_ERR_OK;
  }

  return res;
}

static int
_mtlk_df_user_do_fw_debug (const uint32 *intvec, uint16 intvec_length, UMI_FW_DBG_REQ *FWDebugReq)
{
  uint32 value, max_val = UMI_DEBUG_EXCEPTION;
  int res = MTLK_ERR_PARAMS;

  if (1 != intvec_length) {
    ELOG_D("Incorrect vector length (%u), expected 1", intvec_length);
  } else if (max_val < (value = intvec[0])) {
    ELOG_DD("Value of debugType too big (%u > %u)", value, max_val);
  } else {
    memset(FWDebugReq, 0, sizeof(UMI_FW_DBG_REQ));
    FWDebugReq->debugType = (uint8)value;
    res = MTLK_ERR_OK;
  }

  return res;
}

static BOOL __INLINE
__is_valid_size (uint32 size)
{
  return (1 == size) || (2 == size) || (4 == size);
}

/* 0   1    2    3    4    etc.
   ID  size data size data etc.
   possible size: 1, 2 or 4 bytes */
static int
_mtlk_df_user_get_fw_cmd_by_intvec (uint32 *data_in, uint16 num_args, mtlk_debug_cmd_fw_t *debug_cmd)
{
  int i;
  uint32 param_size, total_len;
  uint8 *data_out_ptr = debug_cmd->data;

  if (num_args < 3) {
    ELOG_V("Wrong usage: Must be at least 3 parameters specified");
    return MTLK_ERR_PARAMS;
  }

  if (!(num_args % 2)) {
    ELOG_V("Wrong usage: Must be odd number of params");
    return MTLK_ERR_PARAMS;
  }

  total_len = 0;
  debug_cmd->len = 0;
  debug_cmd->cmd_id = data_in[0];

  for (i = 1; i < num_args; i+=2) {
    param_size = data_in[i];

    if (!__is_valid_size(param_size)) {
      ELOG_D("Wrong parameter size given %u, expected 1, 2 or 4", param_size);
      return MTLK_ERR_VALUE;
    }

    if ((total_len + param_size) > MAX_DBG_FW_MSG_SIZE) {
      ELOG_D("Too big message size, max expected %u", MAX_DBG_FW_MSG_SIZE);
      return MTLK_ERR_PARAMS;
    }

    switch (param_size) {
    case 1: {
      uint8 data = (uint8)data_in[i + 1];
      *(uint8 *)data_out_ptr = data;
      break;
    }
    case 2: {
      uint16 data = (uint16)data_in[i + 1];
      *(uint16 *)data_out_ptr = HOST_TO_MAC16(data);
      break;
    }
    case 4: {
      uint32 data = data_in[i + 1];
      *(uint32 *)data_out_ptr = HOST_TO_MAC32(data);
      break;
    }
    default:
      return MTLK_ERR_PARAMS;
    }

    total_len += param_size;
    data_out_ptr += param_size;
  }

  debug_cmd->len = total_len;

  return MTLK_ERR_OK;
}

#endif /* CONFIG_WAVE_DEBUG */

int __MTLK_IFUNC
_mtlk_df_user_iwpriv_get_param(mtlk_df_user_t* df_user, uint32 param_id, char* data, mtlk_iwpriv_params_t params, uint16* length)
{
  int res;
  uint16 max_length = MTLK_IW_PRIV_LENGTH(params);
  size_t max_size   = MTLK_IW_PRIV_SIZE(params);

  MTLK_ASSERT(length);
  ILOG2_DDSDD("param_id:%u, type:%u(%s), max_length:%u, max_size:%u",
      param_id, MTLK_IW_PRIV_TYPE(params), mtlk_iw_priv_item_type(params), max_length, max_size);

  /* Setup output length if maximal size is exactly one element */
  if (1 == max_length) *length = 1;

  _DF_USER_GET_PARAM_MAP_START(df_user, param_id, res)

    _DF_USER_GET_ON_PARAM(PRM_ID_11H_CHANNEL_AVAILABILITY_CHECK_TIME, WAVE_RADIO_REQ_GET_DOT11H_AP_CFG, FALSE, mtlk_11h_ap_cfg_t, dot11h_cfg)
      MTLK_CFG_GET_ITEM(dot11h_cfg, debugChannelAvailabilityCheckTime, *(int*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_CALIBRATION_ALGO_MASK, WAVE_RADIO_REQ_GET_CALIBRATION_MASK, FALSE, wave_radio_calibration_cfg_t, calibration_cfg)
      MTLK_CFG_GET_ITEM(calibration_cfg, calibr_algo_mask, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_ONLINE_CALIBRATION_ALGO_MASK, WAVE_RADIO_REQ_GET_CALIBRATION_MASK, FALSE, wave_radio_calibration_cfg_t, calibration_cfg)
      MTLK_CFG_GET_ITEM(calibration_cfg, online_calibr_algo_mask, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_MU_OPERATION, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, mu_operation)
      MTLK_CFG_GET_ITEM(master_cfg, mu_operation, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_PROBE_REQ_LIST, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, probe_req_list_enabled)
      MTLK_CFG_GET_ITEM(master_cfg, probe_req_list_enabled, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_BF_MODE, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, bf_mode)
      MTLK_CFG_GET_ITEM(master_cfg, bf_mode, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_ACS_UPDATE_TO, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, acs_update_timeout)
      MTLK_CFG_GET_ITEM(master_cfg, acs_update_timeout, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_RTS_MODE, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, rts_mode_params)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(master_cfg, rts_mode_params, _mtlk_df_user_set_intvec_by_rts_mode_params,
                                     ((uint32*)data, max_length, length, &master_cfg->rts_mode_params));

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_PIE_CFG, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, wave_pie_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(master_cfg, wave_pie_cfg, _mtlk_df_user_set_intvec_by_pie_params,
                                     ((uint32*)data, max_length, length, &master_cfg->wave_pie_cfg));

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_NICK_NAME, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, nickname)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(core_cfg, nickname, wave_memcpy, (data, max_size, core_cfg->nickname, IW_ESSID_MAX_SIZE));
      *length = mtlk_osal_strnlen(data, IW_ESSID_MAX_SIZE);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_ESSID, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, essid)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(core_cfg, essid, wave_memcpy, (data, max_size, core_cfg->essid, IW_ESSID_MAX_SIZE));
      *length = mtlk_osal_strnlen(data, IW_ESSID_MAX_SIZE);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_BSSID, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, bssid)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(core_cfg, bssid, _mtlk_df_user_fill_sockaddr, ((struct sockaddr*)data, &core_cfg->bssid));

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_ADMISSION_CAPACITY, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, admission_capacity)
      MTLK_CFG_GET_ITEM(core_cfg, admission_capacity, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_TEMPERATURE_SENSOR, WAVE_HW_REQ_GET_TEMPERATURE_SENSOR, FALSE, mtlk_temperature_sensor_t, temperature_cfg)
      MTLK_CFG_GET_ITEM(temperature_cfg, temperature, *(uint32*)data);
      *length = 1;

    /* Power-voltage-temperature (PVT) sensor */
    _DF_USER_GET_ON_PARAM(PRM_ID_PVT_SENSOR, WAVE_HW_REQ_GET_PVT_SENSOR, FALSE, wave_pvt_sensor_t, pvt_sensor)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(pvt_sensor, pvt_params, _mtlk_df_user_get_intvec_by_pvt_sensor_params,
                                     ((int32*)data, max_length, length, &pvt_sensor->pvt_params));

    _DF_USER_GET_ON_PARAM(PRM_ID_RADIO_MODE, WAVE_RADIO_REQ_GET_RADIO_MODE, FALSE, mtlk_radio_mode_cfg_t, radio_mode_cfg)
      MTLK_CFG_GET_ITEM(radio_mode_cfg, radio_mode, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_AGGR_CONFIG, WAVE_CORE_REQ_GET_AGGR_CONFIG, FALSE, mtlk_aggr_cfg_t, aggr_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(aggr_cfg, cfg, _mtlk_df_user_get_intvec_by_aggr_cfg_params,
                                    ((uint32*)data, max_length, length, &aggr_cfg->cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_AMSDU_NUM, WAVE_RADIO_REQ_GET_AMSDU_NUM, FALSE, mtlk_amsdu_num_cfg_t, amsdu_num_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(amsdu_num_cfg, amsdu_num, _mtlk_df_user_get_intvec_by_amsdu_params,
                                    ((uint32*)data, max_length, length, amsdu_num_cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_CCA_THRESHOLD, WAVE_RADIO_REQ_GET_CCA_THRESHOLD, FALSE, mtlk_cca_th_cfg_t, cca_th_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(cca_th_cfg, cca_th_params, _mtlk_df_user_get_intvec_by_cca_th_params,
                                    ((int32*)data, max_length, length, cca_th_cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_CCA_ADAPT, WAVE_RADIO_REQ_GET_CCA_ADAPTIVE, FALSE, mtlk_cca_adapt_cfg_t, cca_adapt_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(cca_adapt_cfg, cca_adapt_params, _mtlk_df_user_get_intvec_by_cca_adapt_params,
                                    ((int32*)data, max_length, length, cca_adapt_cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_RADAR_RSSI_TH, WAVE_RADIO_REQ_GET_RADAR_RSSI_TH, FALSE, mtlk_radar_rssi_th_cfg_t, radar_rssi_th_cfg)
      MTLK_CFG_GET_ITEM(radar_rssi_th_cfg, radar_rssi_th, *(int*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_EEPROM, WAVE_HW_REQ_GET_EEPROM_CFG, FALSE, mtlk_eeprom_cfg_t, eeprom_cfg)
    {
      int used_len = 0;
      *length = 0;
      MTLK_CFG_GET_ITEM_BY_FUNC(eeprom_cfg, eeprom_data, _mtlk_df_user_print_eeprom,
                                (&eeprom_cfg->eeprom_data, data, max_length), used_len);
      *length += used_len;
      MTLK_ASSERT(*length <= max_length);

      MTLK_CFG_GET_ITEM_BY_FUNC(eeprom_cfg, eeprom_raw_data, _mtlk_df_user_print_raw_eeprom_header,
                                (eeprom_cfg->eeprom_raw_data, eeprom_cfg->eeprom_data.hdr_size,
                                  data + *length, TEXT_SIZE - *length), used_len);
      *length += used_len;
      MTLK_ASSERT(*length <= max_length);
    }

    _DF_USER_GET_ON_PARAM(PRM_ID_TPC_LOOP_TYPE, WAVE_RADIO_REQ_GET_TPC_CFG, FALSE, mtlk_tpc_cfg_t, tpc_cfg)
      MTLK_CFG_GET_ITEM(tpc_cfg, loop_type, *(uint32*)data);

    /* CTS to self processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_CTS_TO_SELF_TO, WAVE_RADIO_REQ_GET_CTS_TO_SELF_TO, FALSE, mtlk_cts_to_self_to_cfg_t, mtlk_cts_to_self_to_cfg)
       MTLK_CFG_GET_ITEM(mtlk_cts_to_self_to_cfg, cts_to, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_TX_AMPDU_DENSITY, WAVE_RADIO_REQ_GET_TX_AMPDU_DENSITY, FALSE, mtlk_tx_ampdu_density_cfg_t, mtlk_tx_ampdu_density_cfg)
       MTLK_CFG_GET_ITEM(mtlk_tx_ampdu_density_cfg, ampdu_density, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_COC_POWER_MODE, WAVE_RADIO_REQ_GET_COC_CFG, FALSE, mtlk_coc_mode_cfg_t, coc_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(coc_cfg, power_params, _mtlk_df_user_get_intvec_by_coc_power_params,
                                     ((uint32*)data, max_length, length, &coc_cfg->power_params));

    _DF_USER_GET_ON_PARAM(PRM_ID_COC_AUTO_PARAMS, WAVE_RADIO_REQ_GET_COC_CFG, FALSE, mtlk_coc_mode_cfg_t, coc_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(coc_cfg, auto_params, _mtlk_df_user_get_intvec_by_auto_params,
                                     ((uint32*)data, max_length, length, &coc_cfg->auto_params));

    _DF_USER_GET_ON_PARAM(PRM_ID_ERP, WAVE_RADIO_REQ_GET_ERP_CFG, FALSE, mtlk_erp_mode_cfg_t, erp_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(erp_cfg, erp_cfg, _mtlk_df_user_get_erp_intvec_by_cfg,
                                     ((uint32*)data, max_length, length, &erp_cfg->erp_cfg));

    /* Interference Detection */
    _DF_USER_GET_ON_PARAM(PRM_ID_INTERFER_THRESH, WAVE_RADIO_REQ_GET_INTERFDET_CFG, TRUE, mtlk_interfdet_cfg_t, interfdet_cfg)
      MTLK_CFG_GET_ITEM(interfdet_cfg, threshold, *(int *)data);

    /* Card Capabilities */
    _DF_USER_GET_ON_PARAM(PRM_ID_AP_CAPABILITIES_MAX_STAs, WAVE_HW_REQ_GET_AP_CAPABILITIES, TRUE, mtlk_card_capabilities_t, card_capabilities)
      MTLK_CFG_GET_ITEM(card_capabilities, max_stas_supported, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_AP_CAPABILITIES_MAX_VAPs, WAVE_HW_REQ_GET_AP_CAPABILITIES, TRUE, mtlk_card_capabilities_t, card_capabilities)
      MTLK_CFG_GET_ITEM(card_capabilities, max_vaps_supported, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_FW_RECOVERY, WAVE_RADIO_REQ_GET_RECOVERY_CFG, FALSE, mtlk_rcvry_cfg_t, fw_rcvry_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(fw_rcvry_cfg, recovery_cfg, _mtlk_df_user_intvec_by_recovery,
                                     ((uint32*)data, max_length, length, &fw_rcvry_cfg->recovery_cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_RECOVERY_STATS, WAVE_RADIO_REQ_GET_RECOVERY_STATS, FALSE, mtlk_rcvry_stats_t, fw_rcvry_stats)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(fw_rcvry_stats, recovery_stats, _mtlk_df_user_intvec_by_recovery_stats,
                                     ((uint32*)data, max_length, length, &fw_rcvry_stats->recovery_stats));

    _DF_USER_GET_ON_PARAM(PRM_ID_11B_ANTENNA_SELECTION, WAVE_RADIO_REQ_GET_11B_CFG, FALSE, mtlk_11b_cfg_t, mtlk_11b_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(mtlk_11b_cfg, antsel, _mtlk_df_user_intvec_by_11b_antsel,
                                     ((uint32*)data, max_length, length, &mtlk_11b_cfg->antsel));

    _DF_USER_GET_ON_PARAM(PRM_ID_OUT_OF_SCAN_CACHING, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM(scan_and_calib_cfg, out_of_scan_cache, *(int*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_ALLOW_SCAN_DURING_CAC, WAVE_RADIO_REQ_GET_ALLOW_SCAN_DURING_CAC, FALSE, mtlk_scan_during_cac_cfg_t, scan_in_cac_cfg)
      MTLK_CFG_GET_ITEM(scan_in_cac_cfg, allow, *(int*)data);

    /* Aggregation-Rate Limit processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_AGG_RATE_LIMIT, WAVE_RADIO_REQ_GET_AGG_RATE_LIMIT, FALSE, mtlk_agg_rate_limit_cfg_t, agg_rate_limit_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(agg_rate_limit_cfg, agg_rate_limit, _mtlk_df_user_get_intvec_by_agg_rate_limit,
                                     ((uint32*)data, max_length, length, agg_rate_limit_cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_11H_RADAR_DETECTION, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM(scan_and_calib_cfg, radar_detect, *(uint32*)data);

    /* MU OFDMA Beamforming processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_MU_OFDMA_BF, WAVE_RADIO_REQ_GET_MU_OFDMA_BF, FALSE, mtlk_mu_ofdma_bf_cfg_t, mu_ofdma_bf_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(mu_ofdma_bf_cfg, mu_ofdma_bf, _mtlk_df_user_get_intvec_by_mu_ofdma_bf,
                                     ((uint32*)data, max_length, length, mu_ofdma_bf_cfg));

    /* Reception Duty Cycle settings iwpriv processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_RX_DUTY_CYCLE, WAVE_RADIO_REQ_GET_RX_DUTY_CYCLE, FALSE, mtlk_rx_duty_cycle_cfg_t, rx_duty_cycle_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(rx_duty_cycle_cfg, duty_cycle, _mtlk_df_user_get_intvec_by_duty_cycle_cfg,
                                     ((uint32*)data, max_length, length, rx_duty_cycle_cfg));

    /* Tx power upper limit iwpriv processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_TX_POWER_LIMIT_OFFSET, WAVE_RADIO_REQ_GET_TX_POWER_LIMIT_OFFSET, FALSE, mtlk_tx_power_lim_cfg_t, tx_power_lim_cfg)
      MTLK_CFG_GET_ITEM(tx_power_lim_cfg, powerLimitOffset, *(uint32*)data);

    /* Protection method */
    _DF_USER_GET_ON_PARAM(PRM_ID_PROTECTION_METHOD, WAVE_CORE_REQ_GET_HT_PROTECTION, FALSE, mtlk_ht_protection_cfg_t, protection_cfg)
      MTLK_CFG_GET_ITEM(protection_cfg, use_cts_prot, *(uint32*)data);

    /* QAMplus mode */
    _DF_USER_GET_ON_PARAM(PRM_ID_QAMPLUS_MODE, WAVE_RADIO_REQ_GET_QAMPLUS_MODE, FALSE, mtlk_qamplus_mode_cfg_t, qamplus_mode_cfg)
      MTLK_CFG_GET_ITEM(qamplus_mode_cfg, qamplus_mode, *(uint32*)data);

    /* MAX MPDU length processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_MAX_MPDU_LENGTH, WAVE_RADIO_REQ_GET_MAX_MPDU_LENGTH, FALSE, mtlk_max_mpdu_len_cfg_t, max_mpdu_len_cfg)
      MTLK_CFG_GET_ITEM(max_mpdu_len_cfg, max_mpdu_length, *(uint32*)data);

    /* Active Antenna mask */
    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_ACTIVE_ANT_MASK, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, active_ant_mask)
      MTLK_CFG_GET_ITEM(master_cfg, active_ant_mask, *(uint32*)data);

    /* TXOP configuration processing */
    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_TXOP_CONFIG, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, txop_params)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(master_cfg, txop_params, _mtlk_df_user_get_intvec_by_txop_params,
                                     ((uint32*)data, max_length, length, &master_cfg->txop_params));

    /* SSB Mode processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_SSB_MODE, WAVE_RADIO_REQ_GET_SSB_MODE, FALSE, mtlk_ssb_mode_cfg_t, ssb_mode_cfg)
    {
      if (max_length >= MTLK_SSB_MODE_CFG_SIZE) {
        MTLK_CFG_GET_ARRAY_ITEM(ssb_mode_cfg, params, (uint32*)data, MTLK_SSB_MODE_CFG_SIZE);
        *length = MTLK_SSB_MODE_CFG_SIZE;
      }
      else {
        *length = 0;
      }
    }

    /* 2.4 GHz coexistence */
    _DF_USER_GET_ON_PARAM(PRM_ID_COEX_CFG, WAVE_RADIO_REQ_GET_COEX_CFG, FALSE, mtlk_coex_cfg_t, coex_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(coex_cfg, coex_data, _mtlk_df_user_intvec_by_coex,
                                     ((uint32*)data, max_length, length, &coex_cfg->coex_data));

    /* Restricted AC Mode processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_RESTRICTED_AC_MODE, WAVE_RADIO_REQ_GET_RESTRICTED_AC_MODE, FALSE, mtlk_restricted_ac_mode_cfg_t, restricted_ac_mode_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(restricted_ac_mode_cfg, ac_mode_params, _mtlk_df_user_get_intvec_by_restricted_ac_mode_cfg,
                                     ((uint32*)data, max_length, length, &restricted_ac_mode_cfg->ac_mode_params));

    /* PD Threshold processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_PD_THRESHOLD, WAVE_RADIO_REQ_GET_RESTRICTED_AC_MODE, FALSE, mtlk_restricted_ac_mode_cfg_t, restricted_ac_mode_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(restricted_ac_mode_cfg, pd_thresh_params, _mtlk_df_user_get_intvec_by_pd_threshold_cfg,
                                     ((uint32*)data, max_length, length, &restricted_ac_mode_cfg->pd_thresh_params));

    /* Fast Drop */
    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_FAST_DROP, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg, fast_drop)
      MTLK_CFG_GET_ITEM(master_core_cfg, fast_drop, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_PEERAP_KEY_IDX, WAVE_CORE_REQ_GET_WDS_CFG, FALSE, mtlk_wds_cfg_t, wds_cfg)
      MTLK_CFG_GET_ITEM(wds_cfg, peer_ap_key_idx, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_PEERAP_LIST, WAVE_CORE_REQ_GET_WDS_PEERAP, FALSE, mtlk_wds_cfg_t, wds_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(wds_cfg, peer_ap_vect, _mtlk_df_user_addr_vect_to_text,
                                     (data, wds_cfg->peer_ap_vect, max_length, length));
      mtlk_clpb_delete(wds_cfg->peer_ap_vect);
      wds_cfg->peer_ap_vect = NULL;

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_BRIDGE_MODE, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, bridge_mode)
      MTLK_CFG_GET_ITEM(core_cfg, bridge_mode, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_DBG_SW_WD_ENABLE, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, dbg_sw_wd_enable)
      MTLK_CFG_GET_ITEM(core_cfg, dbg_sw_wd_enable, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_RELIABLE_MULTICAST, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, reliable_multicast)
      MTLK_CFG_GET_ITEM(core_cfg, reliable_multicast, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_AP_FORWARDING, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, ap_forwarding)
      MTLK_CFG_GET_ITEM(core_cfg, ap_forwarding, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_NETWORK_MODE, WAVE_CORE_REQ_GET_NETWORK_MODE, FALSE, wave_core_network_mode_cfg_t, network_mode_cfg)
      MTLK_CFG_GET_ITEM(network_mode_cfg, net_mode, *(uint32*)data);

    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_CHANNEL, WAVE_CORE_REQ_GET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, core_cfg, channel_def)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(core_cfg, channel_def, _mtlk_df_user_set_intvec_by_channel_params,
                                     ((uint32*)data, max_length, length, &core_cfg->channel_def));

    /* Set High Reception Threshold iwpriv processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_RX_THRESHOLD, WAVE_CORE_REQ_GET_RX_TH, FALSE, mtlk_rx_th_cfg_t, rx_th_cfg)
      MTLK_CFG_GET_ITEM(rx_th_cfg, rx_threshold, *(uint32*)data);

    /* Tx-Power processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_TX_POWER, WAVE_CORE_REQ_GET_TX_POWER, FALSE, mtlk_tx_power_cfg_t, tx_power_cfg)
      MTLK_CFG_GET_ITEM(tx_power_cfg, tx_power, *(uint32*)data);

    /* Four address list configuration processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_4ADDR_STA_LIST, WAVE_CORE_REQ_GET_FOUR_ADDR_STA_LIST, FALSE, mtlk_four_addr_cfg_t, four_addr_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(four_addr_cfg, sta_vect, _mtlk_df_user_addr_vect_to_text,
                                     (data, four_addr_cfg->sta_vect, max_length, length));
      mtlk_clpb_delete(four_addr_cfg->sta_vect);
      four_addr_cfg->sta_vect = NULL;

    /* Multicast handling */
    _DF_USER_GET_ON_PARAM(PRM_ID_MCAST_RANGE_SETUP, WAVE_CORE_REQ_GET_MCAST_RANGE_IPV4, FALSE, mtlk_mcast_range_vect_cfg_t, mcast_range_vect_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(mcast_range_vect_cfg, range_vect, _mtlk_df_user_get_text_by_mcast_range,
                                     ((char*)data, mcast_range_vect_cfg, max_length, length));

    _DF_USER_GET_ON_PARAM(PRM_ID_MCAST_RANGE_SETUP_IPV6, WAVE_CORE_REQ_GET_MCAST_RANGE_IPV6, FALSE, mtlk_mcast_range_vect_cfg_t, mcast_range_vect_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(mcast_range_vect_cfg, range_vect, _mtlk_df_user_get_text_by_mcast_range,
                                     ((char*)data, mcast_range_vect_cfg, max_length, length));

    _DF_USER_GET_ON_PARAM(PPM_ID_FORWARD_UNKNOWN_MCAST_FLAG, WAVE_CORE_REQ_GET_FWRD_UNKWN_MCAST_FLAG, FALSE, mtlk_forward_unknown_mcast_cfg_t, forward_unknown_mcast_cfg)
      MTLK_CFG_GET_ITEM(forward_unknown_mcast_cfg, flag, *(int*)data);

    /* RTS Protection Rate */
    _DF_USER_GET_ON_PARAM(PRM_ID_RTS_RATE, WAVE_RADIO_REQ_GET_RTS_RATE, FALSE, mtlk_wlan_rts_rate_cfg_t, mtlk_wlan_rts_rate_cfg)
      MTLK_CFG_GET_ITEM(mtlk_wlan_rts_rate_cfg, cutoff_point, *(uint32*)data);

    /* Stations Statistics */
    _DF_USER_GET_ON_PARAM(PRM_ID_STATIONS_STATS, WAVE_RADIO_REQ_GET_STATIONS_STATS, FALSE, mtlk_wlan_stations_stats_enabled_cfg_t, mtlk_wlan_stations_stats_enabled_cfg)
      MTLK_CFG_GET_ITEM(mtlk_wlan_stations_stats_enabled_cfg, enabled, *(uint32*)data);

    /* Dynamic MU Type processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_DYNAMIC_MU_TYPE, WAVE_HW_REQ_GET_DYNAMIC_MU_CFG, FALSE, wave_dynamic_mu_cfg_t, dinamic_mu_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(dinamic_mu_cfg, dynamic_mu_type_params, _df_user_get_intvec_by_dynamic_mu_type_cfg,
                                     ((uint32*)data, max_length, length, &dinamic_mu_cfg->dynamic_mu_type_params));

    /* HE MU Fixed Parameters processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_HE_MU_FIXED_PARAMTERS, WAVE_HW_REQ_GET_DYNAMIC_MU_CFG, FALSE, wave_dynamic_mu_cfg_t, dinamic_mu_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(dinamic_mu_cfg, he_mu_fixed_params, _df_user_get_intvec_by_he_mu_fixed_parameters_cfg,
                                     ((uint32*)data, max_length, length, &dinamic_mu_cfg->he_mu_fixed_params));

    /* HE MU Duration processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_HE_MU_DURATION, WAVE_HW_REQ_GET_DYNAMIC_MU_CFG, FALSE, wave_dynamic_mu_cfg_t, dinamic_mu_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(dinamic_mu_cfg, he_mu_duration_params, _df_user_get_intvec_by_he_mu_duration_cfg,
                                     ((uint32*)data, max_length, length, &dinamic_mu_cfg->he_mu_duration_params));

    /* ax default params */
    _DF_USER_GET_ON_PARAM(PRM_ID_AX_DEFAULT_PARAMS, WAVE_CORE_REQ_GET_AX_DEFAULT_PARAMS, FALSE, ax_default_params_t, ax_default_params)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(ax_default_params, twt_params, _df_user_get_intvec_by_ax_default_params,
                                     ((uint32*)data, max_length, length, ax_default_params));


    /* PHY in band power */
    _DF_USER_GET_ON_PARAM(PRM_ID_PHY_INBAND_POWER, WAVE_RADIO_REQ_GET_PHY_INBAND_POWER, FALSE, mtlk_phy_inband_power_cfg_t, phy_power_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(phy_power_cfg, power_data, _mtlk_df_user_get_text_by_phy_power_data,
                                     (data, &phy_power_cfg->power_data, max_length, length));

    /* ZWDFS Antenna Configuration */
    _DF_USER_GET_ON_PARAM(PRM_ID_ZWDFS_ANT_CONFIG, WAVE_HW_REQ_GET_ZWDFS_ANT_ENABLED, FALSE, wave_ui_mode_t, cfg)
      MTLK_CFG_GET_ITEM(cfg, mode, *(uint32*)data);

    /* ETSI PPDU Limits processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_ETSI_PPDU_LIMITS, WAVE_RADIO_REQ_GET_ETSI_PPDU_LIMITS, FALSE, wave_etsi_ppdu_duration_cfg_t, etsi_ppdu_duration)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(etsi_ppdu_duration, umi_set_etsi_ppdu_duration_limits_req_params, _df_user_get_intvec_by_etsi_ppdu_limits,
                                         ((uint32*)data, max_length, length, &etsi_ppdu_duration->umi_set_etsi_ppdu_duration_limits_req_params));

    _DF_USER_GET_ON_PARAM(PRM_ID_RTS_THRESHOLD, WAVE_RADIO_REQ_GET_RTS_THRESHOLD, FALSE, mtlk_wlan_rts_threshold_cfg_t, mtlk_wlan_rts_threshold_cfg)
       MTLK_CFG_GET_ITEM(mtlk_wlan_rts_threshold_cfg, threshold, *(uint32*)data);

    /* AP Retry Limit processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_AP_RETRY_LIMIT, WAVE_RADIO_REQ_GET_AP_RETRY_LIMIT, FALSE, wave_retry_limit_cfg_t, wave_retry_limit_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(wave_retry_limit_cfg, retry_limit, _mtlk_df_user_get_intvec_by_retry_limit,
                                     ((uint32*)data, max_length, length, wave_retry_limit_cfg));

    _DF_USER_GET_ON_PARAM(PRM_ID_SCAN_PARAMS, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(scan_and_calib_cfg, scan_params, _mtlk_df_user_get_intvec_by_scan_params,
                                     ((uint32*)data, max_length, length, &scan_and_calib_cfg->scan_params));

    _DF_USER_GET_ON_PARAM(PRM_ID_SCAN_PARAMS_BG, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(scan_and_calib_cfg, scan_params_bg, _mtlk_df_user_get_intvec_by_scan_params_bg,
                                     ((uint32*)data, max_length, length, &scan_and_calib_cfg->scan_params_bg));

    /* Preamble puncture cca override processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_PREAMBLE_PUNCTURE_CFG, WAVE_CORE_REQ_GET_PREAMBLE_PUNCTURE_CCA_OVERRIDE, FALSE, mtlk_preamble_punct_cca_ov_cfg_t, preamble_punct_cca_ov_cfg)
    MTLK_CFG_GET_ITEM_BY_FUNC_VOID(preamble_punct_cca_ov_cfg,preamble_punct_params, _df_user_get_intvec_by_preamble_punct_cca_ov_cfg,
                                ((uint32*)data, max_length, length, &preamble_punct_cca_ov_cfg->preamble_punct_params));

    /* Slow Probing Mask */
    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_PROBING_MASK, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg, slow_probing_mask)
      MTLK_CFG_GET_ITEM(master_core_cfg, slow_probing_mask, *(uint32*)data);

    /* scan and calib iwpriv processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_SCAN_MODIFS, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM(scan_and_calib_cfg, scan_modifs, *(uint32*)data);

    /* scan and calib iwpriv processing */
    _DF_USER_GET_ON_PARAM(PRM_ID_SCAN_PAUSE_BG_CACHE, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM(scan_and_calib_cfg, scan_pause_bg_cache, *(uint32*)data);

    /* Advertise Broadcast TWT */
    _DF_USER_GET_ON_PARAM(PRM_ID_ADVERTISE_BTWT_SCHEDULE, WAVE_CORE_REQ_GET_ADVERTISED_BTWT_SCHEDULE, FALSE, mtlk_advertise_btwt_sp_cfg_t, advertise_btwt_sp_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(advertise_btwt_sp_cfg, config, _mtlk_df_user_get_intvec_by_advertised_btwt_sp,
                                     ((uint32*)data, max_length, length, &advertise_btwt_sp_cfg->config));

    /* Fixed LTF and GI */
    _DF_USER_GET_ON_PARAM(PRM_ID_FIXED_LTF_AND_GI, WAVE_RADIO_REQ_GET_FIXED_LTF_AND_GI, FALSE, mtlk_fixed_ltf_and_gi_t, fixed_ltf_and_gi)
     MTLK_CFG_GET_ITEM_BY_FUNC_VOID(fixed_ltf_and_gi, fixed_ltf_and_gi_params, _mtlk_df_user_get_intvec_by_fixed_ltf_and_gi,
                                    ((uint32*)data, max_length, length, &fixed_ltf_and_gi->fixed_ltf_and_gi_params));

    /* get excessive retry limit */
    _DF_USER_GET_ON_PARAM(PRM_ID_AP_EXCE_RETRY_LIMIT, WAVE_CORE_REQ_GET_AP_EXCE_RETRY_LIMIT, FALSE, mtlk_exce_retry_limit_cfg_t, exce_retry_limit_cfg)
      MTLK_CFG_GET_ITEM(exce_retry_limit_cfg, exce_retry_limit, *(uint32*)data);

/************************* DEBUG GET APIs *************************/
#ifdef CONFIG_WAVE_DEBUG

    /* Get WAVE counters source */
    _DF_USER_GET_ON_PARAM(PRM_ID_SWITCH_COUNTERS_SRC, MTLK_HW_REQ_GET_COUNTERS_SRC, FALSE, mtlk_wlan_counters_src_cfg_t, wlan_counters_src_cfg)
      MTLK_CFG_GET_ITEM(wlan_counters_src_cfg, src, *(uint32*)data);

    /* Unconnected STA scan time */
    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_UNCONNECTED_STA_SCAN_TIME, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_cfg, unconnected_sta_scan_time)
      MTLK_CFG_GET_ITEM(master_cfg, unconnected_sta_scan_time, *(uint32*)data);

    /* Fixed TX management power */
    _DF_USER_GET_ON_PARAM_MEMBER(PRM_ID_FIXED_POWER, WAVE_RADIO_REQ_GET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg, fixed_pwr_params)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(master_core_cfg, fixed_pwr_params, _mtlk_df_user_get_intvec_by_fixed_pwr_params,
                                     ((uint32*)data, max_length, length, &master_core_cfg->fixed_pwr_params));

    _DF_USER_GET_ON_PARAM(PRM_ID_IRE_CTRL_B, WAVE_RADIO_REQ_GET_IRE_CTRL_B, FALSE, mtlk_ire_ctrl_cfg_t, ire_ctrl_cfg)
      MTLK_CFG_GET_ITEM(ire_ctrl_cfg, ire_ctrl_value, *(int*)data);

#ifdef CPTCFG_IWLWAV_SET_PM_QOS
    /* CPU DMA latency */
    _DF_USER_GET_ON_PARAM(PRM_ID_CPU_DMA_LATENCY, WAVE_RADIO_REQ_GET_CPU_DMA_LATENCY, FALSE, mtlk_pm_qos_cfg_t, pm_qos_cfg)
      MTLK_CFG_GET_ITEM(pm_qos_cfg, cpu_dma_latency, *(int32*)data);
#endif

    /* BfExplicitCapable */
    _DF_USER_GET_ON_PARAM(PRM_ID_BEAMFORM_EXPLICIT, WAVE_HW_REQ_GET_BF_EXPLICIT_CAP, FALSE, mtlk_bf_explicit_cap_cfg_t, bf_explicit_cap_cfg)
      MTLK_CFG_GET_ITEM(bf_explicit_cap_cfg, bf_explicit_cap, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_TASKLET_LIMITS, WAVE_HW_REQ_GET_TASKLET_LIMITS, FALSE, mtlk_tasklet_limits_cfg_t, tl_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(tl_cfg, tl, _mtlk_df_user_get_intvec_by_tasklet_limits,
                                     ((uint32 *) data, max_length, length, &tl_cfg->tl));

    _DF_USER_GET_ON_PARAM(PRM_ID_SCAN_EXP_TIME, WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_GET_ITEM(scan_and_calib_cfg, scan_expire_time, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_TA_DBG, WAVE_RADIO_REQ_GET_TA_CFG, FALSE, mtlk_ta_cfg_t, mtlk_ta_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC(mtlk_ta_cfg, debug_info, _mtlk_df_user_print_ta_debug_info,
                                (data, max_length, &mtlk_ta_cfg->debug_info), *length);

    _DF_USER_GET_ON_PARAM(PRM_ID_TA_TIMER_RESOLUTION, WAVE_RADIO_REQ_GET_TA_CFG, FALSE, mtlk_ta_cfg_t, mtlk_ta_cfg)
      MTLK_CFG_GET_ITEM(mtlk_ta_cfg, timer_resolution, *(uint32*)data);

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
    _DF_USER_GET_ON_PARAM(PRM_ID_PCOC_POWER_MODE, WAVE_RADIO_REQ_GET_PCOC_CFG, FALSE, mtlk_pcoc_mode_cfg_t, pcoc_cfg)
      MTLK_ASSERT(max_length >= 3);
      MTLK_CFG_GET_ITEM(pcoc_cfg, is_enabled,    ((uint32*)data)[0]);
      MTLK_CFG_GET_ITEM(pcoc_cfg, is_active,     ((uint32*)data)[1]);
      MTLK_CFG_GET_ITEM(pcoc_cfg, traffic_state, ((uint32*)data)[2]);
      *length = 3;

    _DF_USER_GET_ON_PARAM(PRM_ID_PCOC_AUTO_PARAMS, WAVE_RADIO_REQ_GET_PCOC_CFG, FALSE, mtlk_pcoc_mode_cfg_t, pcoc_cfg)
      MTLK_CFG_GET_ITEM_BY_FUNC_VOID(pcoc_cfg, params, _mtlk_df_user_get_intvec_by_pcoc_params,
                                     ((uint32*)data, max_length, length, &pcoc_cfg->params));
#endif

    _DF_USER_GET_ON_PARAM(PRM_ID_WDS_HOST_TIMEOUT, WAVE_CORE_REQ_GET_HSTDB_CFG, FALSE, mtlk_hstdb_cfg_t, hstdb_cfg)
      MTLK_CFG_GET_ITEM(hstdb_cfg, wds_host_timeout, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_MAC_WATCHDOG_TIMEOUT_MS, WAVE_RADIO_REQ_GET_MAC_WATCHDOG_CFG, FALSE, mtlk_mac_wdog_cfg_t, mac_wdog_cfg)
      MTLK_CFG_GET_ITEM(mac_wdog_cfg, mac_watchdog_timeout_ms, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_MAC_WATCHDOG_PERIOD_MS, WAVE_RADIO_REQ_GET_MAC_WATCHDOG_CFG, FALSE, mtlk_mac_wdog_cfg_t, mac_wdog_cfg)
      MTLK_CFG_GET_ITEM(mac_wdog_cfg, mac_watchdog_period_ms, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_11H_NOP, WAVE_RADIO_REQ_GET_DOT11H_AP_CFG, FALSE, mtlk_11h_ap_cfg_t, dot11h_cfg)
      MTLK_CFG_GET_ITEM(dot11h_cfg, debugNOP, *(uint32*)data);

    _DF_USER_GET_ON_PARAM(PRM_ID_11H_BEACON_COUNT, WAVE_RADIO_REQ_GET_DOT11H_AP_CFG, FALSE, mtlk_11h_ap_cfg_t, dot11h_cfg)
      MTLK_CFG_GET_ITEM(dot11h_cfg, debugChannelSwitchCount, *(int*)data);

#endif /* CONFIG_WAVE_DEBUG */

  _DF_USER_GET_PARAM_MAP_END()

  return res;
}

int __MTLK_IFUNC
_mtlk_df_user_iwpriv_set_param(mtlk_df_user_t* df_user, uint32 param_id, char* data, mtlk_iwpriv_params_t params)
{
  int res;
  uint16 length = MTLK_IW_PRIV_LENGTH(params);
  ILOG3_DDSD("param_id:%u, type:%u(%s), length:%u",
      param_id, MTLK_IW_PRIV_TYPE(params), mtlk_iw_priv_item_type(params), length);

  _DF_USER_SET_PARAM_MAP_START(df_user, param_id, res)

    _DF_USER_SET_ON_PARAM(PRM_ID_11H_CHANNEL_AVAILABILITY_CHECK_TIME, WAVE_RADIO_REQ_SET_DOT11H_AP_CFG, FALSE, mtlk_11h_ap_cfg_t, dot11h_cfg)
      MTLK_CFG_SET_ITEM(dot11h_cfg, debugChannelAvailabilityCheckTime, *(int*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_VAP_ADD, WAVE_RADIO_REQ_MBSS_ADD_VAP_IDX, FALSE, mtlk_mbss_cfg_t, mbss_cfg)
      MTLK_CFG_SET_ITEM(mbss_cfg, added_vap_index, _mtlk_df_user_get_core_slave_vap_index_by_iwpriv_param (*(uint32*)data));
      MTLK_CFG_SET_ITEM(mbss_cfg, role, MTLK_ROLE_AP);

    _DF_USER_SET_ON_PARAM(PRM_ID_VAP_DEL, WAVE_RADIO_REQ_MBSS_DEL_VAP_IDX, FALSE, mtlk_mbss_cfg_t, mbss_cfg)
      MTLK_CFG_SET_ITEM(mbss_cfg, deleted_vap_index, _mtlk_df_user_get_core_slave_vap_index_by_iwpriv_param (*(uint32*)data));

    _DF_USER_SET_ON_PARAM(PRM_ID_CALIBRATION_ALGO_MASK, WAVE_RADIO_REQ_SET_CALIBRATION_MASK, FALSE, wave_radio_calibration_cfg_t, calibration_cfg)
      MTLK_CFG_SET_ITEM(calibration_cfg, calibr_algo_mask, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_ONLINE_CALIBRATION_ALGO_MASK, WAVE_RADIO_REQ_SET_CALIBRATION_MASK, FALSE, wave_radio_calibration_cfg_t, calibration_cfg)
      MTLK_CFG_SET_ITEM(calibration_cfg, online_calibr_algo_mask, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_ADD_PEERAP, WAVE_CORE_REQ_SET_WDS_CFG, FALSE, mtlk_wds_cfg_t, wds_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(wds_cfg, peer_ap_addr_add, _mtlk_df_user_fill_ether_address,
                                (&wds_cfg->peer_ap_addr_add, (struct sockaddr*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_DEL_PEERAP, WAVE_CORE_REQ_SET_WDS_CFG, FALSE, mtlk_wds_cfg_t, wds_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(wds_cfg, peer_ap_addr_del, _mtlk_df_user_fill_ether_address,
                                (&wds_cfg->peer_ap_addr_del, (struct sockaddr*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_PEERAP_KEY_IDX, WAVE_CORE_REQ_SET_WDS_CFG, FALSE, mtlk_wds_cfg_t, wds_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(wds_cfg, peer_ap_key_idx, _mtlk_df_user_fill_peer_ap_key_idx,
                                (&wds_cfg->peer_ap_key_idx, *(uint32*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_BRIDGE_MODE, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, bridge_mode, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_DBG_SW_WD_ENABLE, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, dbg_sw_wd_enable, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_RELIABLE_MULTICAST, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, reliable_multicast, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_AP_FORWARDING, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, ap_forwarding, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_BSS_LOAD, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, bss_load, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_MESH_MODE, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, mesh_mode, *(int *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_SFTBLCK_DIS, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, softblock, *(uint32 *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_MBSSID_VAP, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, mbssid_vap, *(uint32 *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_VAPS_IN_GRP, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, vaps_in_grp, *(uint32 *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_MGMT_RATE, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, mgmt_rate, *(uint32 *)data);


    _DF_USER_SET_ON_PARAM(PRM_ID_MU_OPERATION, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, mu_operation, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_PROBE_REQ_LIST, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, probe_req_list_enabled, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_BF_MODE, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, bf_mode, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_ACS_UPDATE_TO, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, acs_update_timeout, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_RTS_MODE, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC_VOID(master_core_cfg, rts_mode_params, _mtlk_df_user_get_rts_mode_params_by_intvec,
                                     ((uint32*)data, length, &master_core_cfg->rts_mode_params));

    _DF_USER_SET_ON_PARAM(PRM_ID_PIE_CFG, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(master_core_cfg, wave_pie_cfg, _mtlk_df_user_get_pie_params_by_intvec,
                                ((uint32*)data, length, &master_core_cfg->wave_pie_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_DFS_DEBUG, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, dfs_debug, *(uint32 *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_NICK_NAME, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ARRAY_ITEM_BY_FUNC_VOID(pcore_cfg, nickname, wave_strncopy,
                                            (pcore_cfg->nickname, data, sizeof(pcore_cfg->nickname), length));


    _DF_USER_SET_ON_PARAM(PRM_ID_ADMISSION_CAPACITY, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, admission_capacity, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_HE_BEACON, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, he_beacon, *(uint32 *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_SB_TIMER_ACL, WAVE_CORE_REQ_SET_CORE_CFG, FALSE, mtlk_gen_core_cfg_t, pcore_cfg)
      MTLK_CFG_SET_ITEM(pcore_cfg, sb_timer_acl, *(wave_core_sb_timer_acl_t *)data); /*struct assignment */

    _DF_USER_SET_ON_PARAM(PRM_ID_TEMPERATURE_SENSOR, WAVE_HW_REQ_SET_TEMPERATURE_SENSOR, FALSE, mtlk_temperature_sensor_t, temperature_cfg)
      MTLK_CFG_SET_ITEM(temperature_cfg, calibrate_mask, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_RADIO_MODE, WAVE_RADIO_REQ_SET_RADIO_MODE, FALSE, mtlk_radio_mode_cfg_t, radio_mode_cfg)
      MTLK_CFG_SET_ITEM(radio_mode_cfg, radio_mode, !!(*(uint32*)data));

    _DF_USER_SET_ON_PARAM(PRM_ID_AGGR_CONFIG, WAVE_CORE_REQ_SET_AGGR_CONFIG, FALSE, mtlk_aggr_cfg_t, aggr_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(aggr_cfg, cfg, _mtlk_df_user_get_aggr_cfg_params_by_intvec,
                                ((uint32*)data, length, &aggr_cfg->cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_AMSDU_NUM, WAVE_RADIO_REQ_SET_AMSDU_NUM, FALSE, mtlk_amsdu_num_cfg_t, amsdu_num_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(amsdu_num_cfg, amsdu_num,
                                _mtlk_df_user_get_amsdu_num_params_by_intvec, ((uint32*)data, length, amsdu_num_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_CCA_THRESHOLD, WAVE_RADIO_REQ_SET_CCA_THRESHOLD, FALSE, mtlk_cca_th_cfg_t, cca_th_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(cca_th_cfg, cca_th_params,
                                _mtlk_df_user_get_cca_th_params_by_intvec, ((int32*)data, length, cca_th_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_CCA_ADAPT, WAVE_RADIO_REQ_SET_CCA_ADAPTIVE, FALSE, mtlk_cca_adapt_cfg_t, cca_adapt_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(cca_adapt_cfg, cca_adapt_params,
                                _mtlk_df_user_get_cca_adapt_params_by_intvec, ((int32*)data, length, cca_adapt_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_RADAR_RSSI_TH, WAVE_RADIO_REQ_SET_RADAR_RSSI_TH, FALSE,  mtlk_radar_rssi_th_cfg_t, radar_rssi_th_cfg)
      MTLK_CFG_SET_ITEM(radar_rssi_th_cfg, radar_rssi_th, *(int*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_TPC_LOOP_TYPE, WAVE_RADIO_REQ_SET_TPC_CFG, FALSE, mtlk_tpc_cfg_t, tpc_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(tpc_cfg, loop_type, _mtlk_df_user_fill_tpc_cfg,
                                (tpc_cfg, *(uint32*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_COC_POWER_MODE, WAVE_RADIO_REQ_SET_COC_CFG, FALSE, mtlk_coc_mode_cfg_t, coc_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(coc_cfg, power_params,
                                _mtlk_df_user_get_coc_power_params_by_intvec, ((uint32*)data, length, &coc_cfg->power_params), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_COC_AUTO_PARAMS, WAVE_RADIO_REQ_SET_COC_CFG, FALSE, mtlk_coc_mode_cfg_t, coc_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(coc_cfg, auto_params,
                                _mtlk_df_user_get_coc_auto_params_by_intvec, ((uint32*)data, length, &coc_cfg->auto_params), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_ERP, WAVE_RADIO_REQ_SET_ERP_CFG, FALSE, mtlk_erp_mode_cfg_t, erp_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(erp_cfg, erp_cfg,
                                _mtlk_df_user_get_erp_cfg_by_intvec, ((uint32*)data, length, &erp_cfg->erp_cfg), res);

    /* Interference Detection */
    _DF_USER_SET_ON_PARAM(PRM_ID_INTERFER_THRESH, WAVE_RADIO_REQ_SET_INTERFDET_CFG, FALSE, mtlk_interfdet_cfg_t, interfdet_cfg)
      MTLK_CFG_SET_ITEM(interfdet_cfg, threshold, *(int *)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_11B_ANTENNA_SELECTION, WAVE_RADIO_REQ_SET_11B_CFG, FALSE, mtlk_11b_cfg_t, mtlk_11b_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(mtlk_11b_cfg, antsel,
                                _mtlk_df_user_get_11b_antsel_by_intvec, ((uint32*)data, length, &mtlk_11b_cfg->antsel), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_FW_RECOVERY, WAVE_RADIO_REQ_SET_RECOVERY_CFG, FALSE, mtlk_rcvry_cfg_t, fw_rcvry_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(fw_rcvry_cfg, recovery_cfg,
                                _mtlk_df_user_get_recovery_by_intvec, ((uint32*)data, length, &fw_rcvry_cfg->recovery_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_OUT_OF_SCAN_CACHING, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM(scan_and_calib_cfg, out_of_scan_cache, *(int*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_ALLOW_SCAN_DURING_CAC, WAVE_RADIO_REQ_SET_ALLOW_SCAN_DURING_CAC, FALSE, mtlk_scan_during_cac_cfg_t, scan_in_cac_cfg)
      MTLK_CFG_SET_ITEM(scan_in_cac_cfg, allow, !!(*(int*)data)); /* 0 or 1 */

    _DF_USER_SET_ON_PARAM(PRM_ID_11H_EMULATE_RADAR_DETECTION, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(scan_and_calib_cfg, rbm,
                                _mtlk_df_user_get_rbm_by_intvec, ((uint32*)data, length, &scan_and_calib_cfg->rbm), res);

    /* Aggregation-Rate Limit processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_AGG_RATE_LIMIT, WAVE_RADIO_REQ_SET_AGG_RATE_LIMIT, FALSE, mtlk_agg_rate_limit_cfg_t, agg_rate_limit_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(agg_rate_limit_cfg, agg_rate_limit,
                                _mtlk_df_user_get_agg_rate_limit_by_intvec, ((uint32*)data, length, agg_rate_limit_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_11H_RADAR_DETECTION, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM(scan_and_calib_cfg, radar_detect, *(uint32*)data);

    /* Dynamic Multicast Rate Processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_DYNAMIC_MC_RATE, WAVE_CORE_REQ_SET_DYNAMIC_MC_RATE, FALSE, mtlk_multicast_rate_cfg_t, multicast_rate_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(multicast_rate_cfg, multicast_rate,
                                _mtlk_df_user_get_multicast_rate_by_intvec, ((uint32*)data, length, multicast_rate_cfg), res);

    /* MU OFDMA Beamforming processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_MU_OFDMA_BF, WAVE_RADIO_REQ_SET_MU_OFDMA_BF, FALSE, mtlk_mu_ofdma_bf_cfg_t, mu_ofdma_bf_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(mu_ofdma_bf_cfg, mu_ofdma_bf,
                                _mtlk_df_user_get_mu_ofdma_bf_by_intvec, ((uint32*)data, length, mu_ofdma_bf_cfg), res);

    /* Set High Reception Threshold iwpriv processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_RX_THRESHOLD, WAVE_CORE_REQ_SET_RX_TH, FALSE, mtlk_rx_th_cfg_t, rx_th_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(rx_th_cfg, rx_threshold,_mtlk_df_user_fill_rx_th_cfg,
                              (rx_th_cfg, *(int32*)data), res);

    /* Reception Duty Cycle settings iwpriv processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_RX_DUTY_CYCLE, WAVE_RADIO_REQ_SET_RX_DUTY_CYCLE, FALSE, mtlk_rx_duty_cycle_cfg_t, rx_duty_cycle_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(rx_duty_cycle_cfg, duty_cycle,
                              _mtlk_df_user_get_duty_cycle_cfg_by_intvec, ((uint32*)data, length, rx_duty_cycle_cfg), res);

    /* Tx power upper limit iwpriv processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_TX_POWER_LIMIT_OFFSET, WAVE_RADIO_REQ_SET_TX_POWER_LIMIT_OFFSET, FALSE, mtlk_tx_power_lim_cfg_t, tx_power_lim_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(tx_power_lim_cfg, powerLimitOffset, _mtlk_df_user_fill_tx_power_lim_cfg,
                               (tx_power_lim_cfg, *(uint32*)data), res);

    /* Protection method */
    _DF_USER_SET_ON_PARAM(PRM_ID_PROTECTION_METHOD, WAVE_CORE_REQ_SET_HT_PROTECTION, FALSE, mtlk_ht_protection_cfg_t, protection_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(protection_cfg, use_cts_prot, _mtlk_df_user_fill_ht_protection_cfg,
                               (df_user, protection_cfg, *(uint32*)data), res);

    /* QAMplus mode iwpriv processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_QAMPLUS_MODE, WAVE_RADIO_REQ_SET_QAMPLUS_MODE, FALSE, mtlk_qamplus_mode_cfg_t, qamplus_mode_cfg)
      MTLK_CFG_SET_ITEM(qamplus_mode_cfg, qamplus_mode, !!(*(uint32*)data));

    /* MAX MPDU length processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_MAX_MPDU_LENGTH, WAVE_RADIO_REQ_SET_MAX_MPDU_LENGTH, FALSE, mtlk_max_mpdu_len_cfg_t, max_mpdu_len_cfg)
      MTLK_CFG_SET_ITEM(max_mpdu_len_cfg, max_mpdu_length, *(uint32*)data);

    /* Four address list configuration processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_4ADDR_STA_ADD, WAVE_CORE_REQ_SET_FOUR_ADDR_CFG, FALSE, mtlk_four_addr_cfg_t, four_addr_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(four_addr_cfg, addr_add, _mtlk_df_user_fill_ether_address,
                                (&four_addr_cfg->addr_add, (struct sockaddr*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_4ADDR_STA_DEL, WAVE_CORE_REQ_SET_FOUR_ADDR_CFG, FALSE, mtlk_four_addr_cfg_t, four_addr_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(four_addr_cfg, addr_del, _mtlk_df_user_fill_ether_address,
                                (&four_addr_cfg->addr_del, (struct sockaddr*)data), res);

    /* Static Plan processing: Setting and Removing */
    _DF_USER_SET_ON_PARAM(PRM_ID_STATIC_PLAN_CONFIG, WAVE_RADIO_SET_STATIC_PLAN, FALSE, mtlk_static_plan_cfg_t, static_plan_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(static_plan_cfg, config, _mtlk_df_user_fill_static_plan_config_by_intvec,
                              (&static_plan_cfg->config, (uint32*)data, length), res);

    /* TXOP configuration processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_TXOP_CONFIG, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(master_core_cfg, txop_params, _df_user_get_txop_params_by_intvec,
        ((uint32*)data, length, &master_core_cfg->txop_params), res);

    /* Active Antenna mask */
    _DF_USER_SET_ON_PARAM(PRM_ID_ACTIVE_ANT_MASK, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, active_ant_mask, *(uint32*)data);

    /* SSB Mode processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_SSB_MODE, WAVE_RADIO_REQ_SET_SSB_MODE, FALSE, mtlk_ssb_mode_cfg_t, ssb_mode_cfg)
      MTLK_CFG_SET_ARRAY_ITEM(ssb_mode_cfg, params, (uint32*)data, length, res);

    /* Multicast handling */
    _DF_USER_SET_ON_PARAM(PRM_ID_MCAST_RANGE_SETUP, WAVE_CORE_REQ_SET_MCAST_RANGE, FALSE, mtlk_mcast_range_cfg_t, mcast_range_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(mcast_range_cfg, range, _mtlk_df_user_get_mcast_range_by_text,
                                (mcast_range_cfg, (char*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_MCAST_RANGE_SETUP_IPV6, WAVE_CORE_REQ_SET_MCAST_RANGE, FALSE, mtlk_mcast_range_cfg_t, mcast_range_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(mcast_range_cfg, range, _mtlk_df_user_get_mcast_range_ipv6_by_text,
                                (mcast_range_cfg, (char*)data), res);

    _DF_USER_SET_ON_PARAM(PPM_ID_FORWARD_UNKNOWN_MCAST_FLAG, WAVE_CORE_REQ_SET_FWRD_UNKWN_MCAST_FLAG, FALSE, mtlk_forward_unknown_mcast_cfg_t, forward_unknown_mcast_cfg)
      MTLK_CFG_SET_ITEM(forward_unknown_mcast_cfg, flag, *(int*)data);

    /* 2.4 GHz coexistance */
    _DF_USER_SET_ON_PARAM(PRM_ID_COEX_CFG, WAVE_RADIO_REQ_SET_COEX_CFG, FALSE, mtlk_coex_cfg_t, coex_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(coex_cfg, coex_data, _mtlk_df_user_get_coex_cfg_by_intvec,
                                 ((uint32*)data, length, &coex_cfg->coex_data), res);

    /* Frequency Jump Mode processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_FREQ_JUMP_MODE, WAVE_RADIO_REQ_SET_FREQ_JUMP_MODE, FALSE, mtlk_freq_jump_mode_cfg_t, freq_jump_mode_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(freq_jump_mode_cfg, freq_jump_mode, _mtlk_df_user_get_freq_jump_mode_cfg_by_int,
                                (freq_jump_mode_cfg, *(uint32*)data), res);

    /* Restricted AC Mode processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_RESTRICTED_AC_MODE, WAVE_RADIO_REQ_SET_RESTRICTED_AC_MODE, FALSE, mtlk_restricted_ac_mode_cfg_t, restricted_ac_mode_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(restricted_ac_mode_cfg, ac_mode_params, _mtlk_df_user_get_restricted_ac_mode_cfg_by_intvec,
                                ((uint32*)data, length, &restricted_ac_mode_cfg->ac_mode_params), res);

    /* PD Threshold processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_PD_THRESHOLD, WAVE_RADIO_REQ_SET_RESTRICTED_AC_MODE, FALSE, mtlk_restricted_ac_mode_cfg_t, restricted_ac_mode_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(restricted_ac_mode_cfg, pd_thresh_params, _mtlk_df_user_get_pd_threshold_cfg_by_intvec,
                                ((uint32*)data, length, &restricted_ac_mode_cfg->pd_thresh_params), res);

    /* Fast Drop */
    _DF_USER_SET_ON_PARAM(PRM_ID_FAST_DROP, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, fast_drop, *(uint32*)data);

    /* NFRP configuration set */
    _DF_USER_SET_ON_PARAM(PRM_ID_NFRP_CFG, WAVE_RADIO_REQ_SET_NFRP_CFG, FALSE, mtlk_nfrp_cfg_t, nfrp_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(nfrp_cfg, nfrp_cfg_params, _mtlk_df_user_fill_nfrp_cfg_by_intvec,
                                         ((uint32*)data, length, &nfrp_cfg->nfrp_cfg_params), res);


    /* RTS Protection Rate */
    _DF_USER_SET_ON_PARAM(PRM_ID_RTS_RATE, WAVE_RADIO_REQ_SET_RTS_RATE, FALSE, mtlk_wlan_rts_rate_cfg_t, mtlk_wlan_rts_rate_cfg)
      MTLK_CFG_SET_ITEM(mtlk_wlan_rts_rate_cfg, cutoff_point, *(uint32*)data);

    /* Stations Statistics */
    _DF_USER_SET_ON_PARAM(PRM_ID_STATIONS_STATS, WAVE_RADIO_REQ_SET_STATIONS_STATS, FALSE, mtlk_wlan_stations_stats_enabled_cfg_t, mtlk_wlan_stations_stats_enabled_cfg)
      MTLK_CFG_SET_ITEM(mtlk_wlan_stations_stats_enabled_cfg, enabled, !!(*(uint32*)data));

    /* Dynamic MU Type processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_DYNAMIC_MU_TYPE, WAVE_HW_REQ_SET_DYNAMIC_MU_CFG, FALSE, wave_dynamic_mu_cfg_t, dynamic_mu_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(dynamic_mu_cfg, dynamic_mu_type_params, _df_user_get_dynamic_mu_type_cfg_by_intvec,
                                ((uint32*)data, length, &dynamic_mu_cfg->dynamic_mu_type_params), res);

    /* HE MU Fixed Parameters processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_HE_MU_FIXED_PARAMTERS, WAVE_HW_REQ_SET_DYNAMIC_MU_CFG, FALSE, wave_dynamic_mu_cfg_t, dynamic_mu_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(dynamic_mu_cfg, he_mu_fixed_params, _df_user_get_he_mu_fixed_parameters_cfg_by_intvec,
                                ((uint32*)data, length, &dynamic_mu_cfg->he_mu_fixed_params), res);

    /* HE MU Duration processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_HE_MU_DURATION, WAVE_HW_REQ_SET_DYNAMIC_MU_CFG, FALSE, wave_dynamic_mu_cfg_t, dynamic_mu_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(dynamic_mu_cfg, he_mu_duration_params, _df_user_get_mu_duration_cfg_by_intvec,
                                ((uint32*)data, length, &dynamic_mu_cfg->he_mu_duration_params), res);

    /* ZWDFS Antenna Configuration */
    _DF_USER_SET_ON_PARAM(PRM_ID_ZWDFS_ANT_CONFIG, WAVE_HW_REQ_SET_ZWDFS_ANT_ENABLED, FALSE, wave_ui_mode_t, cfg)
      MTLK_CFG_SET_ITEM(cfg, mode, !!(*(uint32*)data)); /* 0 or 1 */

    /* ETSI PPDU Limits processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_ETSI_PPDU_LIMITS, WAVE_RADIO_REQ_SET_ETSI_PPDU_LIMITS, FALSE, wave_etsi_ppdu_duration_cfg_t, etsi_ppdu_duration)
      MTLK_CFG_SET_ITEM_BY_FUNC(etsi_ppdu_duration, umi_set_etsi_ppdu_duration_limits_req_params, _df_user_get_etsi_ppdu_limits_by_intvec,
                                ((uint32*)data, length, &etsi_ppdu_duration->umi_set_etsi_ppdu_duration_limits_req_params), res);

    /* AP retry configuration */
    _DF_USER_SET_ON_PARAM(PRM_ID_AP_RETRY_LIMIT, WAVE_RADIO_REQ_SET_AP_RETRY_LIMIT, FALSE, wave_retry_limit_cfg_t, wave_retry_limit_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(wave_retry_limit_cfg, retry_limit,
                                _mtlk_df_user_get_retry_limit_by_intvec, ((uint32*)data, length, wave_retry_limit_cfg), res);

     /* CTS to self value */
    _DF_USER_SET_ON_PARAM(PRM_ID_CTS_TO_SELF_TO, WAVE_RADIO_REQ_SET_CTS_TO_SELF_TO, FALSE, mtlk_cts_to_self_to_cfg_t, mtlk_cts_to_self_to_cfg)
      MTLK_CFG_SET_ITEM(mtlk_cts_to_self_to_cfg, cts_to, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_TX_AMPDU_DENSITY, WAVE_RADIO_REQ_SET_TX_AMPDU_DENSITY, FALSE, mtlk_tx_ampdu_density_cfg_t, mtlk_tx_ampdu_density_cfg)
      MTLK_CFG_SET_ITEM(mtlk_tx_ampdu_density_cfg, ampdu_density, *(uint32*)data);

    /* Unsolicited Frame Transmission Configuration */
    _DF_USER_SET_ON_PARAM(PRM_ID_UNSOLICITED_FRAME_TX, WAVE_CORE_REQ_SET_UNSOLICITED_FRAME_TX, FALSE, wave_unsolicited_cfg_t, unsolicited_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(unsolicited_cfg, cfg, _mtlk_df_user_get_unsolicited_cfg_params_by_intvec,
                                ((uint32*)data, length, &unsolicited_cfg->cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_SCAN_PARAMS, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(scan_and_calib_cfg, scan_params,
                                _mtlk_df_user_get_scan_params_by_intvec, ((uint32*)data, length, &scan_and_calib_cfg->scan_params), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_SCAN_PARAMS_BG, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(scan_and_calib_cfg, scan_params_bg,
                                _mtlk_df_user_get_scan_params_bg_by_intvec, ((uint32*)data, length, &scan_and_calib_cfg->scan_params_bg), res);

    /* Slow Probing Mask */
    _DF_USER_SET_ON_PARAM(PRM_ID_PROBING_MASK, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, slow_probing_mask, *(uint32*)data);

    /* Advertise Broadcast TWT */
    _DF_USER_SET_ON_PARAM(PRM_ID_ADVERTISE_BTWT_SCHEDULE, WAVE_CORE_REQ_ADVERTISE_BTWT_SCHEDULE, FALSE, mtlk_advertise_btwt_sp_cfg_t, advertise_btwt_sp_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(advertise_btwt_sp_cfg, config, _mtlk_df_user_fill_advertise_btwt_sp_config_by_intvec,
                              (&advertise_btwt_sp_cfg->config, (uint32*)data, length), res);

    /* Terminate Broadcast TWT */
    _DF_USER_SET_ON_PARAM(PRM_ID_TERMINATE_BTWT_SCHEDULE, WAVE_CORE_REQ_TERMINATE_BTWT_SCHEDULE, FALSE, mtlk_terminate_btwt_sp_cfg_t, terminate_btwt_sp_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(terminate_btwt_sp_cfg, config, _mtlk_df_user_fill_terminate_btwt_sp_config_by_intvec,
                              (&terminate_btwt_sp_cfg->config, (uint32*)data, length), res);

    /* excessive retry limit */
    _DF_USER_SET_ON_PARAM(PRM_ID_AP_EXCE_RETRY_LIMIT, WAVE_CORE_REQ_SET_AP_EXCE_RETRY_LIMIT, FALSE, mtlk_exce_retry_limit_cfg_t, exce_retry_limit_cfg)
      MTLK_CFG_SET_ITEM(exce_retry_limit_cfg, exce_retry_limit, *(uint32*)data);

    /* Transmit TWT Teardown */
    _DF_USER_SET_ON_PARAM(PRM_ID_TX_TWT_TEARDOWN, WAVE_CORE_REQ_TX_TWT_TEARDOWN, FALSE, mtlk_tx_twt_teardown_cfg_t, tx_twt_teardown_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(tx_twt_teardown_cfg, config, _mtlk_df_user_fill_tx_twt_teardown_config_by_intvec,
                               (&tx_twt_teardown_cfg->config, (uint32*)data, length), res);

    /* scan and calib iwpriv processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_SCAN_MODIFS, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM(scan_and_calib_cfg, scan_modifs, *(uint32*)data);

    /* WHM config processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_WHM_CONFIG, WAVE_RADIO_REQ_SET_WHM_CONFIG, FALSE, wave_whm_cfg_t, wave_whm_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(wave_whm_cfg, whm_cfg,
                                _mtlk_df_user_get_whm_cfg_by_intvec, ((uint32*)data, length, wave_whm_cfg), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_WHM_RESET, WAVE_RADIO_REQ_SET_WHM_RESET, FALSE, wave_whm_reset_t, wave_whm_reset)
      MTLK_CFG_SET_ITEM(wave_whm_reset, whm_reset, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_WHM_TRIGGER, WAVE_CORE_REQ_SET_WHM_TRIGGER, FALSE, wave_whm_trigger_t, wave_whm_trigger)
      MTLK_CFG_SET_ITEM_BY_FUNC(wave_whm_trigger, whm_warning_id,
                                _mtlk_df_user_get_whm_trigger_by_intvec, ((uint32*)data, length, wave_whm_trigger), res);

    /* scan and calib iwpriv processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_SCAN_PAUSE_BG_CACHE, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM(scan_and_calib_cfg, scan_pause_bg_cache, *(uint32*)data);

    /* Fixed LTF and GI */
    _DF_USER_SET_ON_PARAM(PRM_ID_FIXED_LTF_AND_GI, WAVE_RADIO_REQ_SET_FIXED_LTF_AND_GI, FALSE, mtlk_fixed_ltf_and_gi_t, fixed_ltf_and_gi)
      MTLK_CFG_SET_ITEM_BY_FUNC(fixed_ltf_and_gi, fixed_ltf_and_gi_params, _mtlk_df_user_fill_fixed_ltf_and_gi_by_intvec,
                                         ((uint32*)data, length, &fixed_ltf_and_gi->fixed_ltf_and_gi_params), res);

/************************* DEBUG SET APIs *************************/
#ifdef CONFIG_WAVE_DEBUG


    /* Select WAVE counters source */
    _DF_USER_SET_ON_PARAM(PRM_ID_SWITCH_COUNTERS_SRC, MTLK_HW_REQ_SET_COUNTERS_SRC, FALSE, mtlk_wlan_counters_src_cfg_t, wlan_counters_src_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(wlan_counters_src_cfg, src, _mtlk_df_user_set_counters_src_by_int,
                                (wlan_counters_src_cfg, *(uint32*)data), res);

    /* Unconnected STA scan time */
    _DF_USER_SET_ON_PARAM(PRM_ID_UNCONNECTED_STA_SCAN_TIME, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM(master_core_cfg, unconnected_sta_scan_time, *(uint32*)data);

    /* Fixed TX management power */
    _DF_USER_SET_ON_PARAM(PRM_ID_FIXED_POWER, WAVE_RADIO_REQ_SET_MASTER_CFG, FALSE, mtlk_master_core_cfg_t, master_core_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(master_core_cfg, fixed_pwr_params, _mtlk_df_user_fill_fixed_pwr_params,
                                ((uint32*)data, length, &master_core_cfg->fixed_pwr_params), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_IRE_CTRL_B, WAVE_RADIO_REQ_SET_IRE_CTRL_B, FALSE, mtlk_ire_ctrl_cfg_t, ire_ctrl_cfg)
      MTLK_CFG_SET_ITEM(ire_ctrl_cfg, ire_ctrl_value, *(int*)data);

#ifdef CPTCFG_IWLWAV_SET_PM_QOS
    /* CPU DMA latency */
    _DF_USER_SET_ON_PARAM(PRM_ID_CPU_DMA_LATENCY, WAVE_RADIO_REQ_SET_CPU_DMA_LATENCY, FALSE, mtlk_pm_qos_cfg_t, pm_qos_cfg)
      MTLK_CFG_SET_ITEM(pm_qos_cfg, cpu_dma_latency, *(int32*)data);
#endif

    _DF_USER_SET_ON_PARAM(PRM_ID_TASKLET_LIMITS, WAVE_HW_REQ_SET_TASKLET_LIMITS, FALSE, mtlk_tasklet_limits_cfg_t, tl_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(tl_cfg, tl, _mtlk_df_user_get_tasklet_limits_by_intvec, ((const uint32 *) data, length, &tl_cfg->tl), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_SCAN_EXP_TIME, WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG, FALSE, mtlk_scan_and_calib_cfg_t, scan_and_calib_cfg)
      MTLK_CFG_SET_ITEM(scan_and_calib_cfg, scan_expire_time, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_TA_TIMER_RESOLUTION, WAVE_RADIO_REQ_SET_TA_CFG, FALSE, mtlk_ta_cfg_t, mtlk_ta_cfg)
      MTLK_CFG_SET_ITEM(mtlk_ta_cfg, timer_resolution, *(uint32*)data);

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
    _DF_USER_SET_ON_PARAM(PRM_ID_PCOC_POWER_MODE, WAVE_RADIO_REQ_SET_PCOC_CFG, FALSE, mtlk_pcoc_mode_cfg_t, pcoc_cfg)
      MTLK_CFG_SET_ITEM(pcoc_cfg, is_enabled, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_PCOC_AUTO_PARAMS, WAVE_RADIO_REQ_SET_PCOC_CFG, FALSE, mtlk_pcoc_mode_cfg_t, pcoc_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(pcoc_cfg, params,
                                _mtlk_df_user_get_pcoc_params_by_intvec, ((uint32*)data, length, &pcoc_cfg->params), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_PCOC_PMCU_DEBUG, WAVE_RADIO_REQ_SET_PCOC_CFG, FALSE, mtlk_pcoc_mode_cfg_t, pcoc_cfg)
      MTLK_CFG_SET_ITEM(pcoc_cfg, pmcu_debug, *(uint32*)data);
#endif

    _DF_USER_SET_ON_PARAM(PRM_ID_WDS_HOST_TIMEOUT, WAVE_CORE_REQ_SET_HSTDB_CFG, FALSE, mtlk_hstdb_cfg_t, hstdb_cfg)
      MTLK_CFG_SET_ITEM(hstdb_cfg, wds_host_timeout, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_MAC_WATCHDOG_TIMEOUT_MS, WAVE_RADIO_REQ_SET_MAC_WATCHDOG_CFG, FALSE, mtlk_mac_wdog_cfg_t, mac_wdog_cfg)
      MTLK_CFG_SET_ITEM(mac_wdog_cfg, mac_watchdog_timeout_ms, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_MAC_WATCHDOG_PERIOD_MS, WAVE_RADIO_REQ_SET_MAC_WATCHDOG_CFG, FALSE, mtlk_mac_wdog_cfg_t, mac_wdog_cfg)
      MTLK_CFG_SET_ITEM(mac_wdog_cfg, mac_watchdog_period_ms, *(uint32*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_11H_NOP, WAVE_RADIO_REQ_SET_DOT11H_AP_CFG, FALSE, mtlk_11h_ap_cfg_t, dot11h_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(dot11h_cfg, debugNOP, _mtlk_df_user_fill_debug_nop,
                               (dot11h_cfg, *(uint32*)data), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_11H_BEACON_COUNT, WAVE_RADIO_REQ_SET_DOT11H_AP_CFG, FALSE, mtlk_11h_ap_cfg_t, dot11h_cfg)
      MTLK_CFG_SET_ITEM(dot11h_cfg, debugChannelSwitchCount, *(int*)data);

    _DF_USER_SET_ON_PARAM(PRM_ID_TEST_BUS, WAVE_HW_REQ_SET_TEST_BUS, FALSE, wave_ui_mode_t, cfg)
      MTLK_CFG_SET_ITEM(cfg, mode, !!(*(uint32*)data)); /* 0 or 1 */

    /* Fixed Rate processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_FIXED_RATE, WAVE_RADIO_REQ_SET_FIXED_RATE, FALSE, mtlk_fixed_rate_cfg_t, fixed_rate_cfg)
      MTLK_CFG_SET_ARRAY_ITEM(fixed_rate_cfg, params, (uint32*)data, length, res);

    _DF_USER_SET_ON_PARAM(PRM_ID_DBG_CLI, WAVE_RADIO_REQ_SET_DBG_CLI, FALSE, mtlk_dbg_cli_cfg_t, dbg_cli_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(dbg_cli_cfg, DbgCliReq,
                                _mtlk_df_user_do_simple_cli, ((uint32*)data, length, &dbg_cli_cfg->DbgCliReq), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_FW_DEBUG, WAVE_RADIO_REQ_SET_FW_DEBUG, FALSE, mtlk_fw_debug_cfg_t, fw_debug_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(fw_debug_cfg, FWDebugReq,
                                _mtlk_df_user_do_fw_debug, ((uint32*)data, length, &fw_debug_cfg->FWDebugReq), res);


    _DF_USER_SET_ON_PARAM(PRM_ID_FW_LOG_SEVERITY, WAVE_RADIO_REQ_SET_FW_LOG_SEVERITY, FALSE, mtlk_fw_log_severity_t, fw_log_severity)
      if (2 != (length)) {
        ELOG_D("Need exactly 2 arguments (length=%u)", length);
        res = MTLK_ERR_PARAMS;
      } else {
        MTLK_CFG_SET_ITEM(fw_log_severity, newLevel,  ((uint32*)data)[0]);
        MTLK_CFG_SET_ITEM(fw_log_severity, targetCPU, ((uint32*)data)[1]);
        /* ILOG0_DD("FUNCTION Values in core = %d, %d\n", ((uint32*)data)[0], ((uint32*)data)[1]); */
      }

    _DF_USER_SET_ON_PARAM(PRM_ID_MTLK_DBG_LVL, WAVE_RADIO_REQ_SET_MTLK_DEBUG, FALSE, mtlk_debug_log_level_t, mtlk_debug_level)
      if (3 != (length)) {
        ELOG_D("Need exactly 3 arguments (length=%u)", length);
        res = MTLK_ERR_PARAMS;
      } else {
        MTLK_CFG_SET_ITEM(mtlk_debug_level, oid,  ((uint32*)data)[0]);
        MTLK_CFG_SET_ITEM(mtlk_debug_level, level, ((int32*)data)[1]);
        MTLK_CFG_SET_ITEM(mtlk_debug_level, mode, ((uint32*)data)[2]);
      }

    /* Any message to FW for input validation */
    _DF_USER_SET_ON_PARAM(PRM_ID_DBG_CMD_FW, WAVE_RADIO_REQ_SET_DEBUG_CMD_FW, FALSE, mtlk_debug_cmd_fw_cfg_t, debug_cmd_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(debug_cmd_cfg, debug_cmd, _mtlk_df_user_get_fw_cmd_by_intvec,
      ((uint32*)data, length, &debug_cmd_cfg->debug_cmd), res);

#endif /* CONFIG_WAVE_DEBUG */

    /* Preamble puncture cca override processing */
    _DF_USER_SET_ON_PARAM(PRM_ID_PREAMBLE_PUNCTURE_CFG, WAVE_CORE_REQ_SET_PREAMBLE_PUNCTURE_CCA_OVERRIDE, FALSE, mtlk_preamble_punct_cca_ov_cfg_t,
                                                                                                                 preamble_punct_cca_ov_cfg)
      MTLK_CFG_SET_ITEM_BY_FUNC(preamble_punct_cca_ov_cfg,preamble_punct_params, _df_user_get_preamble_punc_cfg_by_intvec,
                                ((uint32*)data, length, &preamble_punct_cca_ov_cfg->preamble_punct_params), res);

    _DF_USER_SET_ON_PARAM(PRM_ID_EMULATE_INTERFERER, WAVE_RADIO_REQ_EMULATE_INTERFERER, FALSE, int, status)

    _DF_USER_SET_PARAM_MAP_END()

  return res;
}

static void
_mtlk_df_user_copy_linux_stats (struct rtnl_link_stats64 *linux_stats,
                          mtlk_core_general_stats_t *core_status)
{
  linux_stats->rx_packets     = core_status->rx_packets;
  linux_stats->tx_packets     = core_status->tx_packets;
  linux_stats->rx_bytes       = core_status->rx_bytes;
  linux_stats->tx_bytes       = core_status->tx_bytes;
  linux_stats->multicast      = core_status->rx_multicast_packets;

  /* TBD */
}

static int
_mtlk_df_user_get_dcdp_linux_stat (mtlk_df_user_t *df_user, struct rtnl_link_stats64 *linux_stats)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  /* return DC DP statistics only in fastpath mode */
  if (!(df_user->dcdp.mode & DC_DP_MODE_FASTPATH)) {
    return MTLK_ERR_NOT_HANDLED;
  }

  if (DC_DP_FAILURE == dc_dp_get_netif_stats(mtlk_df_user_get_ndev(df_user), &df_user->dcdp.subif_id, linux_stats, DC_DP_F_SUBIF_LOGICAL)) {
    ILOG1_S("%s: cannot get DC DP statistics", mtlk_df_user_get_name(df_user));
    return MTLK_ERR_UNKNOWN;
  }
  return MTLK_ERR_OK;
#else
  return MTLK_ERR_NOT_SUPPORTED;
#endif
}

/* Relevant only for AP role interfaces */
void __MTLK_IFUNC
mtlk_df_user_get_stats(mtlk_df_user_t *df_user, struct rtnl_link_stats64 *stats)
{
  wave_radio_t   *radio;
  unsigned src;

  /* Verify input pointer */
  MTLK_ASSERT(stats);
  if (!stats) return;

  if (!df_user) return;

  radio = wave_vap_radio_get(mtlk_df_get_vap_handle(df_user->df));
  MTLK_ASSERT(radio);
  if (!radio) return;

  src = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_WLAN_COUNTERS_SRC);
  switch (src) {
    case 0:
      /* Always return WLAN FW statistics */
      ILOG3_V("Read counters from WAVE FW");
      *stats = df_user->slow_ctx->linux_stats;
      break;

    case 1:
      /* Always return DC DP statistics */
      ILOG3_V("Read counters from DC DP");
      if (MTLK_ERR_OK != _mtlk_df_user_get_dcdp_linux_stat(df_user, stats)) {
        /* if DC DP statistics is not available, clear output structure */
        memset(stats, 0, sizeof(*stats));
      }
      break;

    default: /* 2 and others */
      /* Autodetect */
      ILOG3_V("Auto: read counters from DC DP");
      if (MTLK_ERR_OK != _mtlk_df_user_get_dcdp_linux_stat(df_user, stats)) {
        /* if DC DP statistics is not available, copy internal statistics (copy structure) */
        ILOG0_V("Auto: DC DP read failed. Read counters from WAVE FW");
        *stats = df_user->slow_ctx->linux_stats;
      }
  }
}

static void __MTLK_IFUNC
_mtlk_df_poll_stats_clb(mtlk_handle_t user_context,
                        int           processing_result,
                        mtlk_clpb_t  *pclpb)
{
  int res;
  mtlk_df_user_t* df_user = (mtlk_df_user_t*) user_context;
  struct _mtlk_hw_t *hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df_user->df));

  MTLK_ASSERT(NULL != hw);

  res = _mtlk_df_user_process_core_retval_void(processing_result, pclpb, WAVE_CORE_REQ_GET_STATUS, FALSE);

  if(MTLK_ERR_OK == res)
  {
    uint32 size;

    mtlk_core_general_stats_t* core_status =
      (mtlk_core_general_stats_t*) mtlk_clpb_enum_get_next(pclpb, &size);

    MTLK_ASSERT(NULL != core_status);
    MTLK_ASSERT(sizeof(*core_status) == size);

    if (NULL != core_status) {
      df_user->slow_ctx->core_general_stats = *core_status;
      _mtlk_df_user_copy_linux_stats(&df_user->slow_ctx->linux_stats, core_status);
    }
  }
  else
  {
    memset(&df_user->slow_ctx->linux_stats, 0,
           sizeof(df_user->slow_ctx->linux_stats));
  }

  if (!mtlk_osal_timer_is_cancelled(&df_user->slow_ctx->stat_timer)) {
    /* if timer has been already canceled, don't start it again,
       thus this callback can be performed from serializer later after timer stop */
    mtlk_osal_timer_set(&df_user->slow_ctx->stat_timer, wave_hw_mmb_get_stats_poll_period(hw));
  }
}

static uint32
_mtlk_df_poll_stats (mtlk_osal_timer_t *timer, mtlk_handle_t data)
{
  mtlk_df_user_t* df_user = (mtlk_df_user_t*) data;
  mtlk_hw_t *hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df_user->df));

  /* if error is occurred in _pci_probe then serializer is absent */
  if (!wave_rcvry_pci_probe_error_get(wave_hw_mmb_get_mmb_base(hw))) {
    _mtlk_df_user_invoke_core_async(df_user->df, WAVE_CORE_REQ_GET_STATUS,
                                    NULL, 0, _mtlk_df_poll_stats_clb, HANDLE_T(df_user));
  }
  return 0;
}

#define MAX_FW_RX_DATA_OFFSET 7
#define MTLK_MIN_MTU_SIZE     68

static int _mtlk_df_ui_create_card_dir(mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(NULL != df_user);

  df_user->slow_ctx->proc_df_node =
      mtlk_df_proc_node_create(mtlk_df_user_get_name(df_user), mtlk_dfg_get_drv_proc_node());

  if (NULL == df_user->slow_ctx->proc_df_node) {
    return MTLK_ERR_NO_MEM;
  }
  return MTLK_ERR_OK;
}

#ifdef MTLK_PER_RATE_STAT
mtlk_df_proc_fs_node_t * __MTLK_IFUNC
mtlk_df_user_get_per_stat_tx_proc_node (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(NULL != df_user);
  return df_user->slow_ctx->proc_per_rate_stats_tx_node;
}

mtlk_df_proc_fs_node_t * __MTLK_IFUNC
mtlk_df_user_get_per_stat_rx_proc_node (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(NULL != df_user);
  return df_user->slow_ctx->proc_per_rate_stats_rx_node;
}

mtlk_df_proc_fs_node_t * __MTLK_IFUNC
mtlk_df_user_get_per_stat_packet_error_rate_proc_node (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(NULL != df_user);
  return df_user->slow_ctx->proc_per_rate_stats_packet_error_rate_node;
}

static int _mtlk_df_ui_create_per_rate_stat_dir (mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(NULL != df_user);
  df_user->slow_ctx->proc_per_rate_stats_node = NULL;
  df_user->slow_ctx->proc_per_rate_stats_tx_node = NULL;
  df_user->slow_ctx->proc_per_rate_stats_rx_node = NULL;
  df_user->slow_ctx->proc_per_rate_stats_packet_error_rate_node = NULL;

  df_user->slow_ctx->proc_per_rate_stats_node = mtlk_df_proc_node_create("sta_per_rate_statistics", df_user->slow_ctx->proc_df_node);
  if (NULL == df_user->slow_ctx->proc_per_rate_stats_node) {
    goto ERROR_PROC_RET;
  }
  df_user->slow_ctx->proc_per_rate_stats_tx_node = mtlk_df_proc_node_create("tx", df_user->slow_ctx->proc_per_rate_stats_node);
  if (NULL == df_user->slow_ctx->proc_per_rate_stats_tx_node) {
    goto ERROR_PROC_RET;
  }
  df_user->slow_ctx->proc_per_rate_stats_rx_node = mtlk_df_proc_node_create("rx", df_user->slow_ctx->proc_per_rate_stats_node);
  if (NULL == df_user->slow_ctx->proc_per_rate_stats_rx_node) {
    goto ERROR_PROC_RET;
  }
  df_user->slow_ctx->proc_per_rate_stats_packet_error_rate_node = mtlk_df_proc_node_create("PER", df_user->slow_ctx->proc_per_rate_stats_node);
  if (NULL == df_user->slow_ctx->proc_per_rate_stats_packet_error_rate_node) {
    goto ERROR_PROC_RET;
  }
  return MTLK_ERR_OK;

ERROR_PROC_RET:
  if(df_user->slow_ctx->proc_per_rate_stats_rx_node) {
    mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_rx_node);
    df_user->slow_ctx->proc_per_rate_stats_rx_node = NULL;
  }
  if(df_user->slow_ctx->proc_per_rate_stats_tx_node) {
    mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_tx_node);
    df_user->slow_ctx->proc_per_rate_stats_tx_node = NULL;
  }
  if(df_user->slow_ctx->proc_per_rate_stats_node) {
    mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_node);
    df_user->slow_ctx->proc_per_rate_stats_node = NULL;
  }
  return MTLK_ERR_NO_MEM;
}
#endif

static void _mtlk_df_ui_delete_card_dir(mtlk_df_user_t* df_user)
{
  mtlk_df_proc_node_delete(df_user->slow_ctx->proc_df_node);
  df_user->slow_ctx->proc_df_node = NULL;
}

#ifdef CONFIG_WAVE_DEBUG
static int _mtlk_df_ui_create_debug_dir(mtlk_df_user_t* df_user)
{
  MTLK_ASSERT(NULL != df_user);

  df_user->slow_ctx->proc_df_debug_node =
      mtlk_df_proc_node_create("Debug", df_user->slow_ctx->proc_df_node);

  if (NULL == df_user->slow_ctx->proc_df_debug_node) {
    return MTLK_ERR_NO_MEM;
  }
  return MTLK_ERR_OK;
}

static void _mtlk_df_ui_delete_debug_dir(mtlk_df_user_t* df_user)
{
  mtlk_df_proc_node_delete(df_user->slow_ctx->proc_df_debug_node);
  df_user->slow_ctx->proc_df_debug_node = NULL;
}
#endif

#ifdef MTLK_PER_RATE_STAT
static void _mtlk_df_ui_delete_per_rate_stat_dir (mtlk_df_user_t* df_user)
{
  mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_packet_error_rate_node);
  df_user->slow_ctx->proc_per_rate_stats_packet_error_rate_node = NULL;
  mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_rx_node);
  df_user->slow_ctx->proc_per_rate_stats_rx_node = NULL;
  mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_tx_node);
  df_user->slow_ctx->proc_per_rate_stats_tx_node = NULL;
  mtlk_df_proc_node_delete(df_user->slow_ctx->proc_per_rate_stats_node);
  df_user->slow_ctx->proc_per_rate_stats_node = NULL;
}
#endif

MTLK_START_STEPS_LIST_BEGIN(df_user)
  MTLK_START_STEPS_LIST_ENTRY(df_user, STAT_POLL)
  MTLK_START_STEPS_LIST_ENTRY(df_user, DP_XFACE)
  MTLK_START_STEPS_LIST_ENTRY(df_user, DP_SUBXFACE)
  MTLK_START_STEPS_LIST_ENTRY(df_user, MC_GROUP_INIT)
MTLK_START_INNER_STEPS_BEGIN(df_user)
MTLK_START_STEPS_LIST_END(df_user);

void
mtlk_df_user_stop(mtlk_df_user_t *df_user)
{
  MTLK_STOP_BEGIN(df_user, MTLK_OBJ_PTR(df_user))
    MTLK_STOP_STEP(df_user, MC_GROUP_INIT, MTLK_OBJ_PTR(df_user),
                    mtlk_df_user_group_uninit, (df_user))
    MTLK_STOP_STEP(df_user, DP_SUBXFACE, MTLK_OBJ_PTR(df_user),
                   _mtlk_df_user_priv_subxface_stop, (df_user));
    MTLK_STOP_STEP(df_user, DP_XFACE, MTLK_OBJ_PTR(df_user),
                   _mtlk_df_user_priv_xface_stop, (df_user));
    MTLK_STOP_STEP(df_user, STAT_POLL, MTLK_OBJ_PTR(df_user),
                   mtlk_osal_timer_cancel_sync, (&df_user->slow_ctx->stat_timer));
  MTLK_STOP_END(df_user, MTLK_OBJ_PTR(df_user));
}

int
mtlk_df_user_start(mtlk_df_t *df, mtlk_df_user_t *df_user)
{
  /* From time we've allocated device name till time we register netdevice
   * we should hold rtnl lock to prohibit someone else from registering same netdevice name.
   * We can't use register_netdev, because we've splitted netdevice registration into 2 phases:
   * 1) allocate name
   * ... driver internal initialization
   * 2) register.
   * We need this because:
   * 1) initialization (registration of proc entries) requires knowledge of netdevice name
   * 2) we can't register netdevice before we have initialized the driver (we might crash on
   * request from the OS)
   *
   * NEW APPROACH: Now the Net Device name is allocated on DF UI initialization in assumption
   * that no one else will register the same device name.
   *  - DF infrastructure has been created already on DF UI initialization.
   *  - register_netdevice() API can used here from now
   *  - It is needed to take rtnl_lock manually for whole VAP creation
   *    This approach allows Core to register abilities correctly before Net Device registration.
   */

  mtlk_hw_t *hw = NULL;
  uint32 poll_period = _DF_STAT_POLL_PERIOD;

  MTLK_START_TRY(df_user, MTLK_OBJ_PTR(df_user))

    hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df));

    if (!wave_hw_mmb_card_is_asic(hw)) { /* non ASIC, i.e. FPGA/Emul */
      poll_period *= 1000;
    }

    if (wave_hw_mmb_get_stats_poll_period(hw) == 0 && !df_user->is_secondary_df) {
      wave_hw_mmb_set_stats_poll_period(df, poll_period);
    }
    MTLK_START_STEP_IF(!df_user->is_secondary_df, df_user, STAT_POLL, MTLK_OBJ_PTR(df_user),
                       mtlk_osal_timer_set, (&df_user->slow_ctx->stat_timer, poll_period));
    MTLK_START_STEP_VOID_IF((!df_user->is_secondary_df
                             && mtlk_vap_is_master(mtlk_df_get_vap_handle(df_user->df))),
                            df_user, DP_XFACE, MTLK_OBJ_PTR(df_user),
                            _mtlk_df_user_priv_xface_start, (df_user));
    MTLK_START_STEP(df_user, DP_SUBXFACE, MTLK_OBJ_PTR(df_user),
                     _mtlk_df_user_priv_subxface_start, (df_user));
    MTLK_START_STEP_VOID(df_user, MC_GROUP_INIT, MTLK_OBJ_PTR(df_user),
                      mtlk_df_user_group_init, (df_user));

  MTLK_START_FINALLY(df_user, MTLK_OBJ_PTR(df_user))
  MTLK_START_RETURN(df_user, MTLK_OBJ_PTR(df_user), mtlk_df_user_stop, (df_user))
}


int __MTLK_IFUNC
mtlk_df_ui_indicate_rx_data(mtlk_df_t *df, mtlk_nbuf_t *nbuf)
{
  int res;
  mtlk_df_user_t *df_user;

  MTLK_ASSERT(NULL != df);
  MTLK_ASSERT(NULL != nbuf);

  df_user = mtlk_df_get_user(df);

  mtlk_nbuf_stop_tracking(nbuf);

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
  mtlk_nbuf_priv_cleanup(mtlk_nbuf_priv(nbuf));
#endif

  /* set pointer to dev, nbuf->protocol for DirectConnect case will be set within DirectConnect RX call back */
  nbuf->dev = df_user->ndev;

#if MTLK_USE_DIRECTCONNECT_DP_API
  if (__mtlk_df_user_litepath_is_registered(df_user)) {
    /* set raw pointer for proper work if directpath is disabled */
    mtlk_df_nbuf_reset_mac_header(nbuf);

    /* send packets to the litepath */
    if (dc_dp_xmit(df_user->ndev, &df_user->dcdp.subif_id, &df_user->dcdp.subif_id, nbuf, nbuf->len, DC_DP_F_XMIT_LITEPATH) == DC_DP_SUCCESS) {
      ++df_user->dcdp.litepath_stats.rx_accepted;
      return MTLK_ERR_OK;
    }
    else
    {
      ++df_user->dcdp.litepath_stats.rx_rejected;
      /** \note: In LitePath mode we should not cleanup packet on sending failure.
       *    Also frame can not be transferred in to the IP stack.
       *    Otherwise packet will corrupt per CPU queue with NULL pointer access during dequeue.
       **/
      return MTLK_ERR_UNKNOWN;
    }
  }
#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

#if MTLK_USE_PUMA6_UDMA
  if (__mtlk_df_user_udma_is_active(df_user)) {
    /* set raw pointer for proper work if directpath is disabled */
    mtlk_df_nbuf_reset_mac_header(nbuf);

    if (0 != df_user->udma.vlan_id)
    {
        /* Insert 802.1Q VLAN tag */
        nbuf = vlan_put_tag(nbuf, HOST_TO_NET16(ETH_P_8021Q), df_user->udma.vlan_id & 0xFFF);
        if (!nbuf) {
          ++df_user->udma.stats.rx_tagging_error;
          return MTLK_ERR_UNKNOWN;
        }

#ifdef UDMA_DEBUG_PRINT
        {
            struct vlan_ethhdr *vlan_header = (struct vlan_ethhdr *)nbuf->data;
            ILOG1_DDDD("[US <--] added vlan tag: vlan_tag=0x%04X, eth_type=0x%04X, VID=0x%04X, len=%04u | ",
                    NET_TO_HOST16(vlan_header->h_vlan_proto),
                    NET_TO_HOST16(vlan_header->h_vlan_encapsulated_proto),
                    NET_TO_HOST16(vlan_header->h_vlan_TCI),
                    nbuf->len);
            ILOG1_YY("DST:%Y | SRC:%Y", vlan_header->h_dest, vlan_header->h_source);
        }
#endif
    }

    /* send packets using the udma */
    if (0 != udma_xmit_skb(df_user->udma.port_id, nbuf)) {
     /* Function udma_xmit_skb() adds a packet to the UDMA transmission queue.
      * When the queue has space, the packet is accepted by UDMA driver.
      * When the queue is full, the packet will be freed and its data discarded.
      * In both cases, either if the queue has space or not, the UDMA driver takes care of releasing the buffer. */
      ++df_user->udma.stats.rx_accepted;
      return MTLK_ERR_OK;
    }

    ++df_user->udma.stats.rx_rejected;
    return MTLK_ERR_UNKNOWN;
  }
#endif /* MTLK_USE_PUMA6_UDMA */

  ILOG5_V("Send to Linux");
  nbuf->protocol        = eth_type_trans(nbuf, nbuf->dev);
  res                   = netif_rx(nbuf);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
  df_user->ndev->last_rx = jiffies;
#endif
  if(NET_RX_SUCCESS != res) {
    ILOG2_D("netif_rx failed: %d", res);
    return MTLK_ERR_UNKNOWN;
  }

  return MTLK_ERR_OK;
}

BOOL __MTLK_IFUNC mtlk_df_ui_check_is_mc_group_member(mtlk_df_t *df,
                                                  const uint8* group_addr)
{
  mtlk_df_user_t *df_user;
  struct netdev_hw_addr *ha;

  MTLK_ASSERT(NULL != df);
  MTLK_ASSERT(NULL != group_addr);

  df_user = mtlk_df_get_user(df);

  /* check if we subscribed to all multicast */
  if (df_user->ndev->allmulti)
    return TRUE;

  netdev_for_each_mc_addr(ha, df_user->ndev) {
    if (!mtlk_osal_compare_eth_addresses(ha->addr, group_addr))
      return TRUE;
  }

  return FALSE;
}

void __MTLK_IFUNC
mtlk_df_ui_notify_mic_failure(mtlk_df_t *df,
                                const uint8 *src_mac,
                                mtlk_df_ui_mic_fail_type_t fail_type)
{
  enum nl80211_key_type key_type;
  int key_id = -1;  /* No key_id from FW, so we are assuming that key_id is missing */

  MTLK_ASSERT(NULL != df);
  MTLK_ASSERT(NULL != src_mac);

  switch (fail_type)
  {
  case MIC_FAIL_PAIRWISE:
    key_type = NL80211_KEYTYPE_PAIRWISE;
    break;

  case MIC_FAIL_GROUP:
    key_type = NL80211_KEYTYPE_GROUP;
    break;

  default:
    MTLK_ASSERT(!"Unknown MIC failure type");
    return;
  }

  cfg80211_michael_mic_failure(mtlk_df_get_user(df)->ndev, src_mac, key_type, key_id, NULL, GFP_KERNEL);
}

static const mtlk_guid_t IRBE_RMMOD = MTLK_IRB_GUID_RMMOD;

void __MTLK_IFUNC
mtlk_df_ui_notify_notify_rmmod (const char *wlanitrf)
{
  mtlk_irbd_notify_app(mtlk_irbd_get_root(), &IRBE_RMMOD, (char *) wlanitrf, IFNAMSIZ);
}

void __MTLK_IFUNC
mtlk_df_ui_notify_notify_fw_hang(mtlk_df_t *df, uint32 fw_cpu, uint32 sw_watchdog_data)
{
  mtlk_vap_handle_t vap_handle;

  MTLK_ASSERT(df != NULL);
  MTLK_ASSERT(fw_cpu < MAX_NUM_OF_FW_CORES);

  vap_handle = mtlk_df_get_vap_handle(df);
  wave_rcvry_mac_hang_evt_set(mtlk_vap_get_hw(vap_handle), fw_cpu);
  WLOG_DD("CID-%04x: FW CPU#%d hang detected", mtlk_vap_get_oid(vap_handle), fw_cpu);
}

void __MTLK_IFUNC
mtlk_df_on_rcvry_isol (mtlk_df_t *df)
{
  MTLK_ASSERT(df != NULL);

  /* Stop stat timer */
  mtlk_osal_timer_cancel_sync(&mtlk_df_get_user(df)->slow_ctx->stat_timer);

  /* Unlock waiting events */
  wave_rcvry_mac_hang_evt_set_all(mtlk_vap_get_hw(mtlk_df_get_vap_handle(df)));
}

void __MTLK_IFUNC
mtlk_df_on_rcvry_restore (mtlk_df_t *df)
{
  mtlk_hw_t *hw = NULL;

  MTLK_ASSERT(df != NULL);
  hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df));

  /* Clear waiting events */
  wave_rcvry_mac_hang_evt_reset_all(hw);

  /* Start stat timer */
  mtlk_osal_timer_set(&mtlk_df_get_user(df)->slow_ctx->stat_timer, wave_hw_mmb_get_stats_poll_period(hw));
}

/**************************************************************
 * Transactions with Core
 **************************************************************/

static int mtlk_df_ui_sta_list(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_NOT_SUPPORTED;
  mtlk_clpb_t *clpb = NULL;
  mtlk_stadb_stat_t *stadb_stat;
  uint32 size;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_ui_get_stadb_status_req_t get_stadb_status_req;


  get_stadb_status_req.get_hostdb = TRUE;
  get_stadb_status_req.use_cipher = FALSE;
  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_STADB_STATUS, &clpb,
                                  &get_stadb_status_req, sizeof(get_stadb_status_req));
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_STADB_STATUS, FALSE);
  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  mtlk_aux_seq_printf(s, "\n"
      "Driver Statistics\n"
      "\n"
      "------------------+-----+-----+----------------------\n"
      "MAC               | AID | VAP | AUTH | 4 ADDR | NSS |\n"
      "------------------+-----+-----+------|--------|------\n");


  /* enumerate sta entries */
  while(NULL != (stadb_stat = mtlk_clpb_enum_get_next(clpb, &size))) {
    if (sizeof(*stadb_stat) != size) {
      res = MTLK_ERR_UNKNOWN;
      goto delete_clpb;
    }

    if (STAT_ID_STADB == stadb_stat->type) {

      mtlk_aux_seq_printf(s, MAC_PRINTF_FMT " | %-3u | %-3u | %-3u  | %-3u    | %-3u |\n",
                          MAC_PRINTF_ARG(stadb_stat->u.general_stat.addr.au8Addr),
                          stadb_stat->u.general_stat.aid,
                          stadb_stat->u.general_stat.vap,
                          stadb_stat->u.general_stat.is_sta_auth,
                          stadb_stat->u.general_stat.is_four_addr,
                          stadb_stat->u.general_stat.nss);
    }
    else if (STAT_ID_HSTDB == stadb_stat->type) {
      if (mtlk_df_is_ap(df)) {
        mtlk_aux_seq_printf(s, "   STA's hosts:\n");
      }
      else {
        mtlk_aux_seq_printf(s, "   All hosts connected to this STA\n");
      }

      mtlk_aux_seq_printf(s, MAC_PRINTF_FMT "\n", MAC_PRINTF_ARG(stadb_stat->u.hstdb_stat.addr.au8Addr));
    } else {
      res = MTLK_ERR_UNKNOWN;
      goto delete_clpb;
    }
  }

  mtlk_aux_seq_printf(s, "------------------+-----+-----+------+--------+------\n");

delete_clpb:
  mtlk_clpb_delete(clpb);
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

/* Print Power value in fractional format: "  xx.xx" */
static void
_mtlk_df_print_power_in_dbm (mtlk_seq_entry_t *s, int power)
{
  int value;

  value = POWER_TO_MBM(power); /* 1 dBm = 100 mBm */
  mtlk_aux_seq_printf(s, "%4d.%02d", value / 100, value % 100);
}

/* Print line in format: "msg1 power msg2" */
static void
_mtlk_df_print_power_line (mtlk_seq_entry_t *s, char *msg1, char *msg2, int power)
{
  mtlk_aux_seq_printf(s, "%s", msg1);
  _mtlk_df_print_power_in_dbm(s, power);
  mtlk_aux_seq_printf(s, "%s\n", msg2);
}

static void
_mtlk_df_print_power_array (mtlk_seq_entry_t *s, char *msg, uint16 *pw_array, int size)
{
  int i;

  mtlk_aux_seq_printf(s, "%s", msg);
  for (i = 0; i < size; i++) {
    _mtlk_df_print_power_in_dbm(s, (int)pw_array[i]);
  }
  mtlk_aux_seq_printf(s,"\n");
}

static const char* _phy_mode_to_string (uint8 phy_mode)
{
  static const char* modes[] = { "802.11a/g", "802.11b", "802.11n", "802.11ac", "802.11ax" };

  if (phy_mode <  MTLK_ARRAY_SIZE(modes))
    return modes[phy_mode];

  return "unknown";
}

#define _DF_USER_MAX_CBW_SUPPORTED_     (CW_160)
#define _DF_USER_GET_CBW_VALUE_(cbw)    (MIN((cbw), _DF_USER_MAX_CBW_SUPPORTED_))
#define _DF_USER_GET_PW_SIZE_(cbw)      (1 + _DF_USER_GET_CBW_VALUE_(cbw))

static int mtlk_df_ui_tx_power(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  char *min_power_interpretation = "Ultimate EVM Power";
  char *max_power_interpretation = "Maximal Power     ";
  mtlk_tx_power_data_t  *tx_pw_data;
  mtlk_rate_pw_entry_t  *entry;
  unsigned  pw_size, pw_size_tmp;

  res = _mtlk_df_user_invoke_core(df, WAVE_RADIO_REQ_GET_TX_RATE_POWER, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_GET_TX_RATE_POWER, FALSE);

  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  /* 1. HW Tx power */
  if (NULL == (tx_pw_data = mtlk_clpb_enum_get_next(clpb, NULL))) {
    mtlk_aux_seq_printf(s, "Tx power data missing\n");
    goto finish;
  }

  if (0 == tx_pw_data->cur_chan) {
    mtlk_aux_seq_printf(s, "Channel number is not set\n");
    goto finish;
  }

  /* Actual size of PW arrays */
  pw_size = _DF_USER_GET_PW_SIZE_(tx_pw_data->cur_cbw);

  /* All power arrays have the same size. Check one of them */
  MTLK_ASSERT(pw_size <= ARRAY_SIZE(tx_pw_data->power_hw.pw_min_ant));

  mtlk_aux_seq_printf(s, "Current Country:        %s\n", tx_pw_data->cur_country_code.country);
  mtlk_aux_seq_printf(s, "Current Channel Number: %d\n", tx_pw_data->cur_chan);
  mtlk_aux_seq_printf(s, "Current Channel Width:  %dMHz\n", mtlkcw2cw(tx_pw_data->cur_cbw));
  mtlk_aux_seq_printf(s, "Maximal Channel Width:  %dMHz\n", mtlkcw2cw(tx_pw_data->max_cbw));

  mtlk_aux_seq_printf(s, "Maximal Antennas:   %5d\n", tx_pw_data->max_antennas);
  _mtlk_df_print_power_line(s, "Maximal Array Gain:  ", " dB", tx_pw_data->max_ant_gain);

  mtlk_aux_seq_printf(s, "Current Antennas:   %5d\n", tx_pw_data->cur_antennas);
  _mtlk_df_print_power_line(s, "Current Array Gain:  ", " dB\n", tx_pw_data->cur_ant_gain);

  _mtlk_df_print_power_line(s, "Total Tx power Regulatory limit:", " dBm", tx_pw_data->power_reg);
  if (tx_pw_data->power_psd) {
    _mtlk_df_print_power_line(s, "Total Tx power PSD limit:       ", " dBm", tx_pw_data->power_psd);
  } else {
    mtlk_aux_seq_printf(s, "Total Tx power PSD limit:         Not Applicable\n");
  }
  _mtlk_df_print_power_line(s, "Total Tx power User set offset: ", " dB",  -tx_pw_data->power_usr_offs);
  _mtlk_df_print_power_line(s, "Total Tx power Configured limit:", " dBm", tx_pw_data->power_cfg);

  mtlk_aux_seq_printf(s,"\nSingle Chain Tx Power Calibration Data:\n"
                        "Channel Width       20MHz  40MHz  80MHz 160MHz\n");
  _mtlk_df_print_power_array(s, min_power_interpretation, &tx_pw_data->power_hw.pw_min_ant[0], pw_size);
  _mtlk_df_print_power_array(s, max_power_interpretation, &tx_pw_data->power_hw.pw_max_ant[0], pw_size);

  mtlk_aux_seq_printf(s,"\nTotal Tx Power Calibration Data:\n"
                        "Channel Width       20MHz  40MHz  80MHz 160MHz\n");
  _mtlk_df_print_power_array(s, min_power_interpretation, &tx_pw_data->pw_min_brd[0], pw_size);
  _mtlk_df_print_power_array(s, max_power_interpretation, &tx_pw_data->pw_max_brd[0], pw_size);

  mtlk_aux_seq_printf(s,"\nOpen loop:          %s\n", tx_pw_data->open_loop ? "On " : "Off");

  /* 2. PSDB rate power list */
  mtlk_aux_seq_printf(s,"\nTx power targets per rate:\n"
                        " PhyMode   MCS NSS  20MHz  40MHz  80MHz 160MHz\n");

  while (NULL != (entry = mtlk_clpb_enum_get_next(clpb, NULL))) {
    unsigned i;
    int base_pw;
    BOOL is_11ag = (PHY_MODE_AG == entry->phy_mode);
    BOOL is_11b = (PHY_MODE_B == entry->phy_mode);
    BOOL is_11n = (PHY_MODE_N == entry->phy_mode);
    BOOL is_11ac = (PHY_MODE_AC == entry->phy_mode);
    BOOL is_11ax = (PHY_MODE_AX == entry->phy_mode);
    BOOL target_pw_based_on_UEVM = IS_QAM256_OR_HIGHER(entry->phy_mode, entry->mcs);
    uint16 pw_per_rate[ARRAY_SIZE(tx_pw_data->power_hw.pw_max_ant)];
    uint16 power_cfg = 0;

    /* Skip printing 11b lines for 5GHz (because not supported by 802.11b spec) */
    if ((MTLK_HW_BAND_5_2_GHZ == tx_pw_data->cur_band) && (is_11b)) continue;

    /* Skip printing 11ac lines for 2.4GHz (because not supported by 802.11ac spec) */
    if ((MTLK_HW_BAND_2_4_GHZ == tx_pw_data->cur_band) && (is_11ac)) continue;

    /* Skip printing non 11ax lines for 6GHz (because not supported by 802.11ax spec) */
    if ((MTLK_HW_BAND_6_GHZ == tx_pw_data->cur_band) && (!is_11ax)) continue;

    mtlk_aux_seq_printf(s, "%-9s %3d %3d ", _phy_mode_to_string(entry->phy_mode), entry->mcs, entry->nss);

    /*
     * Calculate power targets for all rates of the current CBW and below.
     * We cannot display values for BW above current one, since their channel/BW definitions are unknown/not-defined.
     */
    pw_size = _DF_USER_GET_PW_SIZE_(tx_pw_data->cur_cbw);

    /* pw_min_ant is an Ultimate EVM Power, not MIN power */
    i = _DF_USER_GET_CBW_VALUE_(tx_pw_data->cur_cbw);
    base_pw = target_pw_based_on_UEVM ? tx_pw_data->power_hw.pw_min_ant[i]
                                      : tx_pw_data->power_hw.pw_max_ant[i];

    for (i = 0; i < pw_size; i++) {
      /* Set the power_cfg according phy_mode */
      if (is_11b) {
        power_cfg = tx_pw_data->power_tpc_cfg.pw_limits[PSDB_PHY_CW_11B];
      }
      else if (is_11ax){
        power_cfg = tx_pw_data->power_tpc_cfg.pw_limits[PSDB_PHY_CW_AX_20 + i];
      }
      else if (is_11ac){
        power_cfg = tx_pw_data->power_tpc_cfg.pw_limits[PSDB_PHY_CW_OFDM_20 + i];
      }
      else if (is_11n){
        power_cfg = tx_pw_data->power_tpc_cfg.pw_limits[PSDB_PHY_CW_N_20 + i];
      } else {
        power_cfg = tx_pw_data->power_tpc_cfg.pw_limits[i + 1];
      }
      pw_per_rate[i] = hw_mmb_get_tx_power_target(power_cfg, (tx_pw_data->cur_ant_gain + entry->pw_offs + base_pw));

#ifdef MTLK_DEBUG
      /* debug traces */
      {
        char buf[64];
        mtlk_snprintf(buf, sizeof(buf), "Value: phy_mode:%-9s mcs:%3d nss:%3d",
          _phy_mode_to_string(entry->phy_mode),
          entry->mcs,
          entry->nss);
        ILOG1_S("%s", buf);
        ILOG1_DDDD("       phy_mode=%d pw_per_rate[%d]=%d UEVM_based=%d",
          entry->phy_mode,
          i,
          pw_per_rate[i],
          target_pw_based_on_UEVM);
        ILOG1_DDDD("       power_cfg=%d cur_ant_gain=%d pw_offs=%d base_pw=%d",
          power_cfg,
          tx_pw_data->cur_ant_gain,
          entry->pw_offs,
          base_pw);
      }
#endif
    }
    /* Calculate the number of columns to print according to the spec support. */
    if (is_11b) {
      /* In the 11b line, skip printing power values other than 20MHz (because not supported by 802.11b spec). */
      pw_size_tmp = 1;
    } else if ((is_11ag) || (is_11n)) {
      /* In the 11ag & 11n lines, skip printing power values other than 20MHz or 40MHz (because not supported by 802.11ag & 802.11n spec). */
      pw_size_tmp = MIN(2, pw_size);
    }
    else {
      pw_size_tmp = pw_size;
    }

    /* Print a line of power array for all supported phy modes. */
    _mtlk_df_print_power_array(s, "", &pw_per_rate[0], pw_size_tmp);
  }

finish:
  mtlk_clpb_delete(clpb);
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static void
_mtlk_df_ui_print_driver_stats(mtlk_seq_entry_t *s, mtlk_core_general_stats_t *general_stats)
{
  struct priv_stats *pstats;

  int i;
  static const char uint_fmt[] = "%10u %s\n";
  static const char luint_fmt[] = "%20llu %s\n";
  static const char uint_fmt_idx[] = "%10u %s %d\n";

  pstats = &general_stats->core_priv_stats;

  mtlk_aux_seq_printf(s, uint_fmt, pstats->num_disconnects, "Disconnections: disconnect/deactivate");

  /* uint64 counters */
  mtlk_aux_seq_printf(s, luint_fmt, pstats->tx_packets, "total packets sent");
  mtlk_aux_seq_printf(s, luint_fmt, pstats->rx_packets, "total packets received");
  mtlk_aux_seq_printf(s, luint_fmt, pstats->tx_bytes, "total bytes sent");
  mtlk_aux_seq_printf(s, luint_fmt, pstats->rx_bytes, "total bytes received");
  mtlk_aux_seq_printf(s, luint_fmt, pstats->rx_multicast_packets, "total multicast packets received");

  /* uint32 counters */
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->rx_dat_frames, "data frames received");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->rx_ctl_frames, "control frames received");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_man_frames_res_queue, "management frames in reserved queue");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_man_frames_sent, "management frames sent");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_man_frames_confirmed, "management frames confirmed");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->rx_man_frames, "management frames received");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->rx_man_frames_retry_dropped, "management frames dropped due to retries");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->tx_max_cons_drop, "TX maximum consecutive dropped packets");

  for (i = 0; i < NTS_TIDS_GEN6; i++) {
    mtlk_aux_seq_printf(s, uint_fmt_idx, pstats->ac_rx_counter[i], "MSDUs received, QoS priority", i);
  }

  for (i = 0; i < NTS_TIDS_GEN6; i++) {
    mtlk_aux_seq_printf(s, uint_fmt_idx, pstats->ac_tx_counter[i], "MSDUs transmitted, QoS priority", i);
  }

  for (i = 0; i < NTS_TIDS_GEN6; i++) {
    mtlk_aux_seq_printf(s, uint_fmt_idx, pstats->ac_dropped_counter[i], "MSDUs dropped, QoS priority", i);
  }

  for (i = 0; i < NTS_TIDS_GEN6; i++) {
    mtlk_aux_seq_printf(s, uint_fmt_idx, pstats->ac_used_counter[i], "MSDUs used, QoS priority", i);
  }

  mtlk_aux_seq_printf(s, uint_fmt, general_stats->fwd_rx_packets, "packets received that should be forwarded to one or more STAs");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->fwd_rx_bytes, "bytes received that should be forwarded to one or more STAs");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->fwd_tx_packets, "packets transmitted for forwarded data");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->fwd_tx_bytes, "bytes transmitted for forwarded data");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->fwd_dropped, "forwarding (transmission) failures");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->fwd_cannot_clone, "forwarding (transmission): packet's cloning failures");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->rmcast_dropped, "reliable multicast (transmission) failures");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->unicast_replayed_packets +
                                   general_stats->multicast_replayed_packets +
                                   general_stats->management_replayed_packets,
                                   "packets replayed");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->bars_cnt, "BAR frames received");

  mtlk_aux_seq_printf(s, "\nTX errors statistic\n\n");

  mtlk_aux_seq_printf(s, uint_fmt, pstats->txerr_drop_all_filter, "packets dropped due to 'DROP ALL' filter");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->txerr_undef_mc_index,  "packets dropped due to undefined MC index");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->txerr_swpath_overruns, "packets dropped due to msg buffer overrun (SW path)");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->txerr_swpath_hwsend,   "packets dropped due to HW send error (SW path)");
  mtlk_aux_seq_printf(s, uint_fmt, pstats->txerr_dc_xmit,         "packets dropped due to xmit error (DirectConnect)");

  mtlk_aux_seq_printf(s, "\nTXMM related statistic\n\n");

  mtlk_aux_seq_printf(s, uint_fmt, general_stats->txmm_sent, "FW MAN messages sent");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->txmm_cfmd, "FW MAN messages confirmed");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->txmm_peak, "FW MAN messages in peak");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->txdm_sent, "FW DBG messages sent");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->txdm_cfmd, "FW DBG messages confirmed");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->txdm_peak, "FW DBG messages in peak");

  mtlk_aux_seq_printf(s, "\nHW related statistic\n\n");

  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bist_check_passed, "BIST check passed");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->fw_logger_packets_processed, "FW logger packets processed");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->fw_logger_packets_dropped, "FW logger packets dropped");

  mtlk_aux_seq_printf(s, uint_fmt,  general_stats->tx_msdus_free, "TX MSDUs free");
  mtlk_aux_seq_printf(s, uint_fmt,  general_stats->tx_msdus_usage_peak, "TX MSDUs usage peak");

  mtlk_aux_seq_printf(s, uint_fmt, general_stats->dgaf_disabled_tx_pck_dropped, "DGAF disabled: TX DHCP packets converted to Unicast");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->dgaf_disabled_tx_pck_converted, "DGAF disabled: TX Broadcast packets dropped");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->radars_detected, "Radars detected");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->channel_load, "Channel load [%]");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->channel_util, "Channel utilization [%]");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->airtime, "Total Airtime [%]");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->airtime_efficiency, "Total Airtime Efficiency [bytes/sec]");

#ifdef MTLK_DEBUG
  mtlk_aux_seq_printf(s, uint_fmt,  general_stats->tx_max_cfm_in_tasklet,      "TX max cfm in tasklet");
  mtlk_aux_seq_printf(s, luint_fmt, general_stats->tx_max_time_int_to_tasklet, "TX max time from interrupt to tasklet ns");
  mtlk_aux_seq_printf(s, luint_fmt, general_stats->tx_max_time_int_to_pck,     "TX max time from interrupt to processed packet ns");
  mtlk_aux_seq_printf(s, uint_fmt,  general_stats->rx_max_pck_in_tasklet,      "RX max processed packets in tasklet");
  mtlk_aux_seq_printf(s, luint_fmt, general_stats->rx_max_time_int_to_tasklet, "RX max time from interrupt to tasklet ns");
  mtlk_aux_seq_printf(s, luint_fmt, general_stats->rx_max_time_int_to_pck,     "RX max time from interrupt to processed packet ns");
#endif

  mtlk_aux_seq_printf(s, "\nBSS mgmt. related statistic\n\n");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_bds_max_num,        "BSS mgmt bds queue max size");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_bds_free_num,       "BSS mgmt bds queue free entries");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_bds_usage_peak,     "BSS mgmt bds queue peak usage");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_bds_max_num_res,    "BSS mgmt bds queue max size (reserved queue)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_bds_free_num_res,   "BSS mgmt bds queue free entries (reserved queue)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_bds_usage_peak_res, "BSS mgmt bds queue peak usage (reserved queue)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_tx_que_full_cnt,    "BSS mgmt msgs dropped TX que full (per VAP)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_probe_resp_sent_cnt,      "Probe responses sent (per VAP)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_probe_resp_dropped_cnt,   "Probe responses dropped (per VAP)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->bss_mgmt_tx_que_full_cnt_per_radio,  "BSS mgmt msgs dropped TX que full (per Radio)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_probe_resp_sent_cnt_per_radio,    "Probe responses sent (per Radio)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->tx_probe_resp_dropped_cnt_per_radio, "Probe responses dropped (per Radio)");

#ifdef CPTCFG_IWLWAV_LEGACY_INT_RECOVERY_MON
  mtlk_aux_seq_printf(s, "\nInterrupt recovery statistics\n\n");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->isr_lost_suspect, "Lost interrupts suspects (per HW)");
  mtlk_aux_seq_printf(s, uint_fmt, general_stats->isr_recovered,    "Lost interrupts recovered (per HW)");
#endif
}

#if MTLK_USE_DIRECTCONNECT_DP_API
static void _mtlk_df_ui_print_litepath(mtlk_seq_entry_t *s, mtlk_df_user_t *df_user)
{
    mtlk_df_user_litepath_stats_t litepath_stats;
    static const char uint_fmt[] = "%10u %s\n";

   _mtlk_df_user_litepath_get_stats(df_user, &litepath_stats);

    mtlk_aux_seq_printf(s, "\nLitePath related statistic\n\n");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.tx_processed,           "TX frames processed from LitePath");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.tx_forward,             "TX frames forwarded to LitePath");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.tx_discarded_no_peers,  "TX frames discarded: no peers");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.tx_forward_fail,        "TX frames forward fail to LitePath");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.tx_sent_up,             "TX frames sent up from LitePath");
    mtlk_aux_seq_printf(s, uint_fmt, _mtlk_df_user_litepath_tx_dropped,     "TX frames dropped from LitePath");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.rx_accepted,            "RX frames accepted by LitePath");
    mtlk_aux_seq_printf(s, uint_fmt, litepath_stats.rx_rejected,            "RX frames rejected by LitePath");
}

static void _mtlk_df_ui_print_link_stats64 (mtlk_seq_entry_t *s, struct rtnl_link_stats64 *if_stats)
{
    static const char uint64_fmt[] = "%10llu %s\n";
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->rx_packets, "total packets received");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->tx_packets, "total packets transmitted");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->rx_bytes,   "total bytes received");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->tx_bytes,   "total bytes transmitted");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->rx_errors,  "bad packets received");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->tx_errors,  "packet transmit problems");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->rx_dropped, "total RX packets dropped");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->tx_dropped, "total TX packets dropped");
    mtlk_aux_seq_printf(s, uint64_fmt, if_stats->multicast,  "multicast packets received");
}

static void _mtlk_df_ui_print_dcdp_stats (mtlk_seq_entry_t *s, mtlk_df_user_t *df_user)
{
    struct rtnl_link_stats64 if_stats;
    mtlk_df_t *master_df;
    mtlk_df_user_t *df_user_master;

    master_df = mtlk_df_user_get_master_df(df_user);
    MTLK_CHECK_DF_NORES(master_df);
    df_user_master = mtlk_df_get_user(master_df);

    mtlk_aux_seq_printf(s, "\nDirectConnect related statistic (per HW)\n\n");
    if (DC_DP_SUCCESS == dc_dp_get_netif_stats(mtlk_df_user_get_ndev(df_user_master), &df_user_master->dcdp.subif_id, &if_stats, 0 /* per RADIO */)) {
        _mtlk_df_ui_print_link_stats64(s, &if_stats);
    } else {
        mtlk_aux_seq_printf(s, "- no statistic is available -\n");
    }

    mtlk_aux_seq_printf(s, "\nDirectConnect related statistic (per VAP)\n\n");
    if (DC_DP_SUCCESS == dc_dp_get_netif_stats(mtlk_df_user_get_ndev(df_user), &df_user->dcdp.subif_id, &if_stats, DC_DP_F_SUBIF_LOGICAL)) {
        _mtlk_df_ui_print_link_stats64(s, &if_stats);
    } else {
        mtlk_aux_seq_printf(s, "- no statistic is available -\n");
    }
}
#endif


#if MTLK_USE_PUMA6_UDMA
static void _mtlk_df_ui_print_udma(mtlk_seq_entry_t *s, mtlk_df_user_t *df_user)
{
    mtlk_df_user_udma_stats_t udma_stats;
    static const char uint_fmt[] = "%10u %s\n";

    _mtlk_df_user_udma_get_stats(df_user, &udma_stats);

    mtlk_aux_seq_printf(s, "\nUDMA related statistic\n\n");
    mtlk_aux_seq_printf(s, uint_fmt, udma_stats.tx_processed, "TX Frames processed from UDMA");
    mtlk_aux_seq_printf(s, uint_fmt, udma_stats.tx_rejected_no_vlan_tag, "TX Frames rejected (no VLAN tag)");
    mtlk_aux_seq_printf(s, uint_fmt, udma_stats.tx_rejected_no_vlan_registered, "TX Frames rejected (VLAN isn't registered)");
    mtlk_aux_seq_printf(s, uint_fmt, udma_stats.rx_accepted, "RX Frames accepted by UDMA");
    mtlk_aux_seq_printf(s, uint_fmt, udma_stats.rx_rejected, "RX Frames rejected by UDMA");
    mtlk_aux_seq_printf(s, uint_fmt, udma_stats.rx_tagging_error, "RX Frames VLAN tagging errors");
}
#endif /* MTLK_USE_PUMA6_UDMA */

static int mtlk_df_ui_debug_general(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_NOT_SUPPORTED;
  mtlk_clpb_t *clpb = NULL;
  mtlk_stadb_stat_t *stadb_stat;
  uint32 size;
  unsigned long total_rx_packets;
  unsigned long total_tx_packets;
  unsigned long total_rx_dropped;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  mtlk_core_ui_get_stadb_status_req_t get_stadb_status_req;
  unsigned long sta_cnt = 0;

  /* Get Core general information from DF buffer and don't call Core */

  get_stadb_status_req.get_hostdb = FALSE;
  get_stadb_status_req.use_cipher = FALSE;
  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_STADB_STATUS, &clpb,
                                  &get_stadb_status_req, sizeof(get_stadb_status_req));
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_STADB_STATUS, FALSE);
  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  mtlk_aux_seq_printf(s, "\n"
      "Driver Statistics\n"
      "\n"
      "------------------+-----+------------------+--------------------------------------\n"
      "MAC               | SID | Packets received | Packets sent     | Rx packets dropped\n"
      "------------------+ ----+------------------+--------------------------------------\n");

  total_rx_packets = 0;
  total_tx_packets = 0;
  total_rx_dropped = 0;

  /* enumerate sta entries */
  while(NULL != (stadb_stat = mtlk_clpb_enum_get_next(clpb, &size))) {
    if (sizeof(*stadb_stat) != size) {
      res = MTLK_ERR_UNKNOWN;
      goto delete_clpb;
    }

    if (STAT_ID_STADB == stadb_stat->type) {

      total_rx_packets += stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsReceived;
      total_tx_packets += stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsSent;
      total_rx_dropped += stadb_stat->u.general_stat.sta_rx_dropped;
      sta_cnt++;

      mtlk_aux_seq_printf(s, MAC_PRINTF_FMT " | %3u | %-16u | %-16u | %-16u\n",
                          MAC_PRINTF_ARG(stadb_stat->u.general_stat.addr.au8Addr),
                          stadb_stat->u.general_stat.sta_sid,
                          stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsReceived,
                          stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsSent,
                          stadb_stat->u.general_stat.sta_rx_dropped);
    } else {
      res = MTLK_ERR_UNKNOWN;
      goto delete_clpb;
    }
  }
  mtlk_aux_seq_printf(s, "Total Connected STA: %lu\n", sta_cnt);

  mtlk_aux_seq_printf(s,
      "------------------------+------------------+--------------------------------------\n"
      "Total                   | %-16lu | %-16lu | %lu\n"
      "------------------------+------------------+--------------------------------------\n"
      "Broadcast/non-reliable multicast           | %-16lu |\n"
      "------------------+------------------------+--------------------------------------\n"
      "\n",
      total_rx_packets, total_tx_packets, total_rx_dropped,
      (unsigned long)df_user->slow_ctx->core_general_stats.core_priv_stats.tx_bcast_nrmcast);

  _mtlk_df_ui_print_driver_stats(s, &df_user->slow_ctx->core_general_stats);

#if MTLK_USE_DIRECTCONNECT_DP_API
  _mtlk_df_ui_print_litepath(s, df_user);
  _mtlk_df_ui_print_dcdp_stats(s, df_user);
#endif

#if MTLK_USE_PUMA6_UDMA
  if (__mtlk_df_user_udma_is_available(df_user)) {
      _mtlk_df_ui_print_udma(s, df_user);
  }
#endif

delete_clpb:
  mtlk_clpb_delete(clpb);
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static void
_mtlk_df_ui_print_chandef_info(mtlk_seq_entry_t *s, const char *prefix, struct mtlk_chan_def *cd)
{
  unsigned low_chan_num, primary_chan_num, primary_chan_idx;

  mtlk_aux_seq_printf(s, "%swidth:           \t%4u\n", prefix, MHZS_PER_20MHZ << cd->width);
  mtlk_aux_seq_printf(s, "%scenter_freq1:    \t%4u\n", prefix, cd->center_freq1);
  mtlk_aux_seq_printf(s, "%scenter_freq2:    \t%4u\n", prefix, cd->center_freq2);
  mtlk_aux_seq_printf(s, "%schan.center_freq:\t%4u\n", prefix, cd->chan.center_freq);

  if ((cd->chan.band == MTLK_HW_BAND_6_GHZ) && (cd->width == CW_160)) {
      MTLK_ASSERT(cd->center_freq2 != 0);
      low_chan_num = freq2lowchannum(cd->center_freq2, cd->width);
  } else {
     low_chan_num = freq2lowchannum(cd->center_freq1, cd->width);
  }
  primary_chan_num = ieee80211_frequency_to_channel(cd->chan.center_freq);
  primary_chan_idx = (primary_chan_num - low_chan_num) / CHANNUMS_PER_20MHZ;

  mtlk_aux_seq_printf(s, "%schannum_from:    \t%4u\n", prefix, low_chan_num);
  mtlk_aux_seq_printf(s, "%schannum_to:      \t%4u\n", prefix, low_chan_num + (CHANNUMS_PER_20MHZ << cd->width) - CHANNUMS_PER_20MHZ);
  mtlk_aux_seq_printf(s, "%sprimary_channel: \t%4u\n", prefix, primary_chan_num);
  mtlk_aux_seq_printf(s, "%sprimary_chan_idx:\t%4u\n", prefix, primary_chan_idx);

  if (__mtlk_is_sb_dfs_switch(cd->sb_dfs.sb_dfs_bw)) {
    low_chan_num = freq2lowchannum(cd->sb_dfs.center_freq, cd->sb_dfs.width);

    mtlk_aux_seq_printf(s, "\n%sSub Band DFS active, chandef in FW:\n", prefix);
    mtlk_aux_seq_printf(s, "%s  center_freq1:  \t%4u\n", prefix, cd->sb_dfs.center_freq);
    mtlk_aux_seq_printf(s, "%s  width:         \t%4u\n", prefix, MHZS_PER_20MHZ << cd->sb_dfs.width);
    mtlk_aux_seq_printf(s, "%s  channum_from:  \t%4u\n", prefix, low_chan_num);
    mtlk_aux_seq_printf(s, "%s  channum_to:    \t%4u\n", prefix, low_chan_num + (CHANNUMS_PER_20MHZ << cd->sb_dfs.width) - CHANNUMS_PER_20MHZ);
  }
}

static int
_mtlk_df_ui_channel_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  struct mtlk_chan_def cd;

  /* Note: this info could have been changing while we copied it and
   * we won't necessarily catch it with the is_channel_certain() trick.
   */
  cd = *__wave_core_chandef_get(mcore);
  if (!is_channel_certain(&cd))
  {
    mtlk_aux_seq_printf(s, "Channel not certain: changing or restored to unset state\n");
    return 0; /* or -EAGAIN;? */
  }

  _mtlk_df_ui_print_chandef_info(s, "", &cd);
  return 0;
}

static int
_mtlk_df_ui_radio_cfg_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  wave_radio_t *radio = wave_vap_radio_get(mcore->vap_handle);
  mtlk_hw_t *hw = mtlk_vap_get_hw(mcore->vap_handle);

  char *band_cfg_to_string[] = { "none", "single band", "dual band 2x2", "dual band 3x1", "dual band 4x1" };
  char *band_to_string[] = { "5GHz", "2.4GHz", "6GHz", "none", "Dual_2.4_5.2", "Dual_2.4_6.0", "Dual_5.2_6.0", "none" };

  unsigned band = WAVE_RADIO_PDB_GET_UINT(radio, PARAM_DB_RADIO_FREQ_BAND_CUR);
  wave_hw_radio_band_cfg_t band_cfg = wave_hw_radio_band_cfg_get(hw);

  if (band >= MTLK_ARRAY_SIZE(band_to_string))
    return MTLK_ERR_UNKNOWN;

  if (band_cfg >= MTLK_ARRAY_SIZE(band_cfg_to_string))
    return MTLK_ERR_UNKNOWN;

  mtlk_aux_seq_printf(s, "band:           %s\n", band_to_string[band]);
  mtlk_aux_seq_printf(s, "band cfg:       %s\n", band_cfg_to_string[band_cfg]);
  mtlk_aux_seq_printf(s, "first in card:  %s\n", wave_radio_is_first(radio) ? "true" : "false");
  mtlk_aux_seq_printf(s, "zw-dfs ant:     %s\n", wave_radio_get_is_zwdfs_radio(radio) ? "true" : "false");

  return 0;
}

mtlk_error_t __MTLK_IFUNC
wave_hw_read_fw_trace_buffers (mtlk_hw_t *hw, mtlk_seq_entry_t *s);

static int
_wave_df_ui_fw_trace_read (mtlk_seq_entry_t *s, void *data)
{
  int        res;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_hw_t *hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df));

  res = wave_hw_read_fw_trace_buffers(hw, s);
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _mtlk_df_ui_hw_limits(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  mtlk_hw_limits_stat_entry_t *stat_entry;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);

  res = _mtlk_df_user_invoke_core(df, WAVE_RADIO_REQ_GET_HW_LIMITS, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_GET_HW_LIMITS, FALSE);

  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  mtlk_aux_seq_printf(s,"HW specific limits:\n"
                        "Domain Freq Spectrum PhyMode Limit\n");

  while(NULL != (stat_entry = mtlk_clpb_enum_get_next(clpb, NULL))) {
    mtlk_aux_seq_printf(s," 0x%02x %5d %5d %7d %7d\n",
                        stat_entry->reg_domain,
                        stat_entry->freq,
                        stat_entry->spectrum,
                        stat_entry->phy_mode,
                        stat_entry->tx_lim);
  }

  mtlk_clpb_delete(clpb);
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

#ifdef CONFIG_WAVE_DEBUG

static void
mtlk_df_debug_bcl_cleanup(mtlk_df_user_t *df_user);
static int
mtlk_df_debug_bcl_init(mtlk_df_user_t *df_user);

static int
mtlk_df_ui_reset_stats(mtlk_df_t* df, uint32 reset_radar_cnt)
{
  mtlk_clpb_t *clpb = NULL;
  int res;

  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_RESET_STATS, &clpb, &reset_radar_cnt, sizeof(reset_radar_cnt));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_CORE_REQ_RESET_STATS, TRUE);

  return res;
}

static unsigned
mtlk_cssidx2channum(unsigned chan, mtlk_hw_band_e band)
{
  return ((MTLK_HW_BAND_6_GHZ == band) ? cssidx6g2channum(chan) : cssidx2channum(chan));
}

static int
_mtlk_df_ui_calibration_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  struct scan_support *ss = mtlk_core_get_scan_support(mcore);
  unsigned i;
  mtlk_hw_band_e band = wave_radio_band_get(wave_vap_radio_get(mcore->vap_handle));
  int num_chans = ((MTLK_HW_BAND_6_GHZ == band) ? NUM_TOTAL_CHANS_6G : NUM_TOTAL_CHANS);
  struct channel_scan_support *loc_css = ((MTLK_HW_BAND_6_GHZ == band) ? ss->css_6g : ss->css);

  mtlk_aux_seq_printf(s, "param_algomask:  0x%08x\n", ss->param_algomask);
  mtlk_aux_seq_printf(s, "param_oalgomask: 0x%08x\n", ss->param_oalgomask);

  mtlk_aux_seq_printf(s, "\n# idx\tnum\tcalib_done_mask\tcalib_failed_mask\n");

  for (i = 1; i <= num_chans; i++)
    mtlk_aux_seq_printf(s, "%5u\t%3u\t           0x%02hhx\t            0x%02hhx\n", i, mtlk_cssidx2channum(i, band),
                        loc_css[i].calib_done_mask, loc_css[i].calib_failed_mask);

  return 0;
}

static int
_mtlk_df_ui_hdkconfig_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  UMI_HDK_CONFIG *chc = __wave_core_hdkconfig_get(mcore);

  /* Config data are in MAC format */
  mtlk_aux_seq_printf(s, "calibrationBufferBaseAddress: 0x%08x\n", MAC_TO_HOST32(chc->calibrationBufferBaseAddress));
  mtlk_aux_seq_printf(s, "numTxAnts:                    %hhu\n", chc->hdkConf.numTxAnts);
  mtlk_aux_seq_printf(s, "numRxAnts:                    %hhu\n", chc->hdkConf.numRxAnts);
  mtlk_aux_seq_printf(s, "EEPROMVersion:                0x%04X\n", MAC_TO_HOST16(chc->hdkConf.eepromInfo.u16EEPROMVersion));
  mtlk_aux_seq_printf(s, "NumberOfPoints5GHz:           %hhu\n", chc->hdkConf.eepromInfo.u8NumberOfPoints5GHz);
  mtlk_aux_seq_printf(s, "NumberOfPoints2GHz:           %hhu\n", chc->hdkConf.eepromInfo.u8NumberOfPoints2GHz);
  mtlk_aux_seq_printf(s, "band:                         %hhu (%s GHz)\n", chc->hdkConf.band, chc->hdkConf.band ? "2.4" : "5.2");
  mtlk_aux_seq_printf(s, "offlineCalMask:               0x%08x\n", MAC_TO_HOST32(chc->hdkConf.calibrationMasks.offlineCalMask));
  mtlk_aux_seq_printf(s, "onlineCalMask:                0x%08x\n", MAC_TO_HOST32(chc->hdkConf.calibrationMasks.onlineCalMask));

  return 0;
}

static int mtlk_aux_seq_hexdump (mtlk_seq_entry_t *s, const uint8 *data, size_t size)
{
  int line, i;

  for (line = 0; size; line++) {
    mtlk_aux_seq_printf(s, "%04x: ", line * 0x10);

    for (i = 0x10; i && size; size--, i--, data++) {
      mtlk_aux_seq_printf(s, i == 0x08 ? "  %02x" : " %02x", *data);
    }
    mtlk_aux_seq_printf(s, "\n");
    if (seq_has_overflowed(s)) {
      return MTLK_ERR_BUF_TOO_SMALL;
    }
  }

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC    mtlk_hw_get_hw_dump_info(mtlk_hw_t *hw, wave_hw_dump_info_t **hw_dump_info);

static int _mtlk_df_ui_hw_reg_dump (mtlk_seq_entry_t *s, void *data)
{
  int i, size;
  wave_hw_dump_info_t *info;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(df);
  mtlk_hw_t *hw = mtlk_vap_get_hw(vap_handle);

  if (!mtlk_core_is_hw_halted(mtlk_vap_get_core(vap_handle))) {
    mtlk_aux_seq_printf(s, "HW dumps are only available when FW/HW are in halted state!\n");
    return MTLK_ERR_OK;
  }

  size = mtlk_hw_get_hw_dump_info(hw, &info);

  for (i = 0; i < size; i++, info++) {
    mtlk_aux_seq_printf(s, "HEX dump of %s (base 0x%06X, size 0x%04X):\n", info->name, info->io_base, info->io_size);
    if (MTLK_ERR_BUF_TOO_SMALL == mtlk_aux_seq_hexdump(s, mtlk_hw_get_mmb_io_data(hw, info->io_base), info->io_size))
      return MTLK_ERR_OK; /* This function will be re-called with bigger buffer size */
    mtlk_aux_seq_printf(s, "\n");
  }

  return MTLK_ERR_OK;
}

static int
_mtlk_df_ui_debug_igmp_read(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_NOT_SUPPORTED;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_clpb_t *clpb = NULL;
  mtlk_mc_igmp_tbl_item_t   *igmp_tbl_item;
  mtlk_core_ui_mc_grid_action_t  *req;
  uint32 s_grp_addr, s_src_addr;
  unsigned grp_id;

  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_MC_IGMP_TBL, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_MC_IGMP_TBL, FALSE);
  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  while (NULL != (igmp_tbl_item = mtlk_clpb_enum_get_next(clpb, NULL))) {
    switch (igmp_tbl_item->type) {
    case MTLK_MC_IPV4_GRP_ADDR:
      s_grp_addr = htonl(((mtlk_mc_igmp_tbl_ipv4_item_t*)igmp_tbl_item)->addr.s_addr);
      grp_id = ((mtlk_mc_igmp_tbl_ipv4_item_t*)igmp_tbl_item)->grp_id;
      if (MCAST_GROUP_UNDEF == grp_id) {
        mtlk_aux_seq_printf(s, "IPv4 mcast group " MTLK_NIPQUAD_FMT "\n",
                MTLK_NIPQUAD( s_grp_addr ));
      } else {
        mtlk_aux_seq_printf(s, "IPv4 mcast group " MTLK_NIPQUAD_FMT " : %u\n",
                MTLK_NIPQUAD( s_grp_addr ), grp_id);
      }
      break;
    case MTLK_MC_IPV4_SRC_ADDR:
      s_grp_addr = htonl(((mtlk_mc_igmp_tbl_ipv4_item_t*)igmp_tbl_item)->addr.s_addr);
      grp_id = ((mtlk_mc_igmp_tbl_ipv4_item_t*)igmp_tbl_item)->grp_id;
      if (MCAST_GROUP_UNDEF == grp_id) {
        mtlk_aux_seq_printf(s, "  source: " MTLK_NIPQUAD_FMT "\n",
                MTLK_NIPQUAD( s_grp_addr ));
      } else {
        mtlk_aux_seq_printf(s, "  source: " MTLK_NIPQUAD_FMT " : %u\n",
                MTLK_NIPQUAD( s_grp_addr ), grp_id);
      }
      break;
    case MTLK_MC_IPV6_GRP_ADDR:
      grp_id = ((mtlk_mc_igmp_tbl_ipv6_item_t*)igmp_tbl_item)->grp_id;
      if (MCAST_GROUP_UNDEF == grp_id) {
        mtlk_aux_seq_printf(s, "IPv6 mcast group " MTLK_NIP6_FMT "\n",
                MTLK_NIP6( ((mtlk_mc_igmp_tbl_ipv6_item_t*)igmp_tbl_item)->addr ));
      } else {
        mtlk_aux_seq_printf(s, "IPv6 mcast group " MTLK_NIP6_FMT " : %u\n",
                MTLK_NIP6( ((mtlk_mc_igmp_tbl_ipv6_item_t*)igmp_tbl_item)->addr ), grp_id);
      }
      break;
    case MTLK_MC_IPV6_SRC_ADDR:
      grp_id = ((mtlk_mc_igmp_tbl_ipv6_item_t*)igmp_tbl_item)->grp_id;
      if (MCAST_GROUP_UNDEF == grp_id) {
        mtlk_aux_seq_printf(s, "  source: " MTLK_NIP6_FMT "\n",
                MTLK_NIP6( ((mtlk_mc_igmp_tbl_ipv6_item_t*)igmp_tbl_item)->addr ));
      } else {
        mtlk_aux_seq_printf(s, "  source: " MTLK_NIP6_FMT " : %u\n",
                MTLK_NIP6( ((mtlk_mc_igmp_tbl_ipv6_item_t*)igmp_tbl_item)->addr ), grp_id);
      }
      break;
    case MTLK_MC_MAC_ADDR:
        mtlk_aux_seq_printf(s, "sta: " MAC_PRINTF_FMT " vap_id:%u\n",
          MAC_PRINTF_ARG( ((mtlk_mc_igmp_tbl_mac_item_t*)igmp_tbl_item)->addr.au8Addr ),
            ((mtlk_mc_igmp_tbl_mac_item_t*)igmp_tbl_item)->vap_id);
        if (!mtlk_osal_is_zero_address(((mtlk_mc_igmp_tbl_mac_item_t*)igmp_tbl_item)->host_addr.au8Addr))
          mtlk_aux_seq_printf(s, "host:   " MAC_PRINTF_FMT "\n",
            MAC_PRINTF_ARG( ((mtlk_mc_igmp_tbl_mac_item_t*)igmp_tbl_item)->host_addr.au8Addr ));
      break;
    default:
      MTLK_ASSERT(FALSE);
    }
  }
  mtlk_clpb_delete(clpb);

  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_MC_HW_TBL, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_MC_HW_TBL, FALSE);
  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }
  mtlk_aux_seq_printf(s, "--- HW table of group Indexes --- \n");
  while (NULL != (req = mtlk_clpb_enum_get_next(clpb, NULL))) {
    switch (req->mc_addr.type) {
    case MTLK_IPv4:
      s_grp_addr = htonl(req->mc_addr.grp_ip.ip4_addr.s_addr);
      s_src_addr = htonl(req->mc_addr.src_ip.ip4_addr.s_addr);
      mtlk_aux_seq_printf(s, "group:%d IPv4 mcast: " MTLK_NIPQUAD_FMT " --> " MTLK_NIPQUAD_FMT "\n",
              req->grp_id, MTLK_NIPQUAD(s_src_addr), MTLK_NIPQUAD(s_grp_addr));
      break;
    case MTLK_IPv6:
      mtlk_aux_seq_printf(s, "group:%d IPv6 mcast group: " MTLK_NIP6_FMT " --> " MTLK_NIP6_FMT "\n",
              req->grp_id, MTLK_NIP6(req->mc_addr.src_ip.ip6_addr), MTLK_NIP6(req->mc_addr.grp_ip.ip6_addr));
      break;
    default:
      break;
    }
  }
  mtlk_clpb_delete(clpb);

err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _mtlk_df_ui_print_list(mtlk_seq_entry_t *s, mtlk_core_tx_req_id_t req_id)
{
  int res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);

  res = _mtlk_df_user_invoke_core(df, req_id, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, req_id, FALSE);

  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  while(NULL != (h = mtlk_clpb_enum_get_next(clpb, NULL))) {
    mtlk_aux_seq_printf(s, "%pM\n", h->key.au8Addr);
  }

  mtlk_clpb_delete(clpb);
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _mtlk_df_ui_widan_blacklist(mtlk_seq_entry_t *s, void *data)
{
  return _mtlk_df_ui_print_list(s, WAVE_CORE_REQ_GET_BLACKLIST_ENTRIES);
}

static int _mtlk_df_ui_multi_ap_blacklist(mtlk_seq_entry_t *s, void *data)
{
  return _mtlk_df_ui_print_list(s, WAVE_CORE_REQ_GET_MULTI_AP_BL_ENTRIES);
}

static int _mtlk_df_ui_wds_wpa_sta_list(mtlk_seq_entry_t *s, void *data)
{
  return _mtlk_df_ui_print_list(s, WAVE_CORE_REQ_GET_WDS_WPA_LIST_ENTRIES);
}

static void seq_file_hex_dump(mtlk_seq_entry_t *s, void *data, uint32 data_size)
{
  uint32 line, pos;

  for (line = 0; data_size; line++) {
    mtlk_aux_seq_printf(s, "%04x: ", line * 0x10);
    for (pos = 0x10; pos && data_size; data_size--, pos--, data++) {
      mtlk_aux_seq_printf(s, " %02x", *((unsigned char*) data));
    }
    mtlk_aux_seq_printf(s, "\n");
  }
}

static int _mtlk_df_qos_map_dump(mtlk_seq_entry_t *s, void *v)
{
  uint32 size;
  int                      res = MTLK_ERR_UNKNOWN;
  mtlk_df_t                *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_clpb_t              *clpb = NULL;
  mtlk_core_qos_map_info_t *qos_map_info;

  mtlk_aux_seq_printf(s, "\n    DSCP mapping table:\n\n");

  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_QOS_MAP_INFO, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_QOS_MAP_INFO, FALSE);
  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  qos_map_info = mtlk_clpb_enum_get_next(clpb, &size);

  if (qos_map_info && (sizeof(*qos_map_info) == size)) {
    seq_file_hex_dump(s, qos_map_info->dscp_table, sizeof(qos_map_info->dscp_table));
  }
  else {
    res = MTLK_ERR_UNKNOWN;
  }

  mtlk_clpb_delete(clpb);

err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int
_mtlk_df_ui_scanrequest_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  mtlk_scan_support_t *ss = mtlk_core_get_scan_support(mcore);
  struct mtlk_scan_request *req = &ss->req;
  int i;
  mtlk_hw_band_e band = wave_radio_band_get(wave_vap_radio_get(mcore->vap_handle));
  int numchans = ((MTLK_HW_BAND_6_GHZ == band) ? NUM_TOTAL_CHANS_6G : NUM_TOTAL_CHANS);
  int numssids = MIN(req->n_ssids, MAX_SCAN_SSIDS);

  numchans = MIN(req->n_channels, numchans);
  mtlk_aux_seq_printf(s, "n_channels:     %u\n", req->n_channels);
  mtlk_aux_seq_printf(s, "flags:          0x%08x\n", req->flags);
  mtlk_aux_seq_printf(s, "rates:          0x%08x 0x%08x\n", req->rates[0], req->rates[1]);
  mtlk_aux_seq_printf(s, "interval:       %u\n", req->interval);
  mtlk_aux_seq_printf(s, "min_rssi_thold: %i\n", req->min_rssi_thold);
  mtlk_aux_seq_printf(s, "n_ssids:        %u\n", req->n_ssids);
  mtlk_aux_seq_printf(s, "ie_len:         %zu\n", req->ie_len);

  mtlk_aux_seq_printf(s, "\n# idx\tfreq\tnum\t flags\n");

  for (i = 0; i < numchans; i++)
  {
    struct mtlk_channel *chan = req->channels + i;
    mtlk_aux_seq_printf(s, "%5u\t%4u\t%3u\t0x%06x\n", i, chan->center_freq,
                        ieee80211_frequency_to_channel(chan->center_freq), chan->flags);
  }

  mtlk_aux_seq_printf(s, "\n# ssid\n");

  for (i = 0; i < numssids; i++)
    mtlk_aux_seq_printf(s, "%s\n", req->ssids[i]);

  return 0;
}

static int
_mtlk_df_ui_scansupport_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  struct scan_support *ss = mtlk_core_get_scan_support(mcore);
  unsigned i;
  int num_chans = 0;
  struct channel_scan_support *loc_css = NULL;
  mtlk_hw_band_e band = (mtlk_hw_band_e) ss->last_chan_band;

  _mtlk_df_ui_print_chandef_info(s, "orig_chandef.", &ss->orig_chandef);

  mtlk_aux_seq_printf(s, "flags:                   0x%08x\n", ss->flags);
  mtlk_aux_seq_printf(s, "ScanModifierFlags:       0x%08x\n", ss->ScanModifierFlags);
  mtlk_aux_seq_printf(s, "passive_scan_time:       %u\n", ss->iwpriv_scan_params.passive_scan_time);
  mtlk_aux_seq_printf(s, "active_scan_time:        %u\n", ss->iwpriv_scan_params.active_scan_time);
  mtlk_aux_seq_printf(s, "num_probe_reqs:          %u\n", ss->iwpriv_scan_params.num_probe_reqs);
  mtlk_aux_seq_printf(s, "probe_req_interval:      %u\n", ss->iwpriv_scan_params.probe_req_interval);
  mtlk_aux_seq_printf(s, "passive_scan_valid_time: %u\n", ss->iwpriv_scan_params.passive_scan_valid_time);
  mtlk_aux_seq_printf(s, "active_scan_valid_time:  %u\n", ss->iwpriv_scan_params.active_scan_valid_time);
  mtlk_aux_seq_printf(s, "passive_scan_time_bg:    %u\n", ss->iwpriv_scan_params_bg.passive_scan_time);
  mtlk_aux_seq_printf(s, "active_scan_time_bg:     %u\n", ss->iwpriv_scan_params_bg.active_scan_time);
  mtlk_aux_seq_printf(s, "num_probe_reqs_bg:       %u\n", ss->iwpriv_scan_params_bg.num_probe_reqs);
  mtlk_aux_seq_printf(s, "probe_req_interval_bg:   %u\n", ss->iwpriv_scan_params_bg.probe_req_interval);
  mtlk_aux_seq_printf(s, "num_chans_in_chunk:      %u\n", ss->iwpriv_scan_params_bg.num_chans_in_chunk);
  mtlk_aux_seq_printf(s, "num_chunk_interval:      %u\n", ss->iwpriv_scan_params_bg.chan_chunk_interval);
  mtlk_aux_seq_printf(s, "num_chunk_interval_busy: %u\n", ss->iwpriv_scan_params_bg.chan_chunk_interval_busy);
  mtlk_aux_seq_printf(s, "window_slice:            %u\n", ss->iwpriv_scan_params_bg.window_slice);
  mtlk_aux_seq_printf(s, "window_slice_overlap:    %u\n", ss->iwpriv_scan_params_bg.window_slice_overlap);
  mtlk_aux_seq_printf(s, "PassiveValid:            %u\n", ss->PassiveValid);
  mtlk_aux_seq_printf(s, "ActiveValid:             %u\n", ss->ActiveValid);
  mtlk_aux_seq_printf(s, "PassiveScanWaitTime:     %u\n", ss->PassiveScanWaitTime);
  mtlk_aux_seq_printf(s, "ActiveScanWaitTime:      %u\n", ss->ActiveScanWaitTime);
  mtlk_aux_seq_printf(s, "NumProbeReqs:            %hu\n", ss->NumProbeReqs);
  mtlk_aux_seq_printf(s, "ProbeReqInterval:        %hu\n", ss->ProbeReqInterval);
  mtlk_aux_seq_printf(s, "scan_start_time:         %llu\n", ss->scan_start_time);
  mtlk_aux_seq_printf(s, "chan_in_chunk:           %i\n", ss->chan_in_chunk);
  mtlk_aux_seq_printf(s, "last_probe_num:          %i\n", ss->last_probe_num);
  mtlk_aux_seq_printf(s, "chan_flags:              0x%08x\n", ss->chan_flags);
  mtlk_aux_seq_printf(s, "last_chan_idx:           %i\n", ss->last_chan_idx);
  mtlk_aux_seq_printf(s, "last_chan_num:           %u\n", ss->last_chan_num);
  mtlk_aux_seq_printf(s, "last_chan_band:          %i\n", ss->last_chan_band);

  mtlk_aux_seq_printf(s, "\n# idx\tnum\tlast_passive_scan_time\tlast_active_scan_time\n");

  num_chans = ((MTLK_HW_BAND_6_GHZ == band) ? NUM_TOTAL_CHANS_6G : NUM_TOTAL_CHANS);
  loc_css = ((MTLK_HW_BAND_6_GHZ == band) ? ss->css_6g : ss->css);

  for (i = 1; i <= num_chans; i++)
    mtlk_aux_seq_printf(s, "%5u\t%3u\t%22llu\t%21llu\n", i, mtlk_cssidx2channum(i, band),
                        loc_css[i].last_passive_scan_time, loc_css[i].last_active_scan_time);

  return 0;
}

static int
_mtlk_df_ui_survey_read(mtlk_seq_entry_t *s, void *data)
{
  /* FIXME some protection against core removal is needed */
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  struct scan_support *ss = mtlk_core_get_scan_support(mcore);
  mtlk_hw_band_e band = wave_radio_band_get(wave_vap_radio_get(mcore->vap_handle));
  unsigned num_surveys = ((MTLK_HW_BAND_6_GHZ == band) ? MAX_NUM_SURVEYS_6G : MAX_NUM_SURVEYS);
  unsigned i;
  struct channel_survey_support *csys = ((MTLK_HW_BAND_6_GHZ == band) ? ss->csys_6g : ss->csys);

  num_surveys = MIN(ss->num_surveys, num_surveys);
  mtlk_aux_seq_printf(s, "num_surveys: %u\n", ss->num_surveys);
  mtlk_aux_seq_printf(s, "\n# idx\tfreq\tnum\tnoise\tload\n");

  for (i = 0; i < num_surveys; i++)
  {
    struct channel_survey_support *loc_csys = &csys[i];
    mtlk_aux_seq_printf(s, "%5u\t%4u\t%3u\t%5hhd\t%4hhu\n", i, loc_csys->channel->center_freq,
                        ieee80211_frequency_to_channel(loc_csys->channel->center_freq),
                        loc_csys->noise, loc_csys->load);
  }

  return 0;
}

int __MTLK_IFUNC
mtlk_hw_seq_write_logger_trace_buffers (mtlk_hw_t *hw, mtlk_seq_entry_t *s);

static int
_mtlk_df_ui_trace_buffer_read (mtlk_seq_entry_t *s, void *data)
{
  int        res;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_hw_t *hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df));

  res = mtlk_hw_seq_write_logger_trace_buffers(hw, s);
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int
_mtlk_df_ui_reset_stats_proc (struct file *file, const char __user *buffer,
                              unsigned long count, void *data)
{
  char *conf = NULL;
  uint32 reset_radar_cnt = FALSE;
  mtlk_df_t *df = mtlk_df_proc_entry_get_df(data);

  conf = kmalloc(count, GFP_KERNEL);

  if (!conf) {
    ELOG_D("Unable to allocate %lu bytes", count);
    return -EFAULT;
  }

  if (0 != (copy_from_user(conf, buffer, count))) {
    kfree(conf);
    return -EFAULT;
  }

  /* Reset Radar Counter */
  if (count == 4 && conf[0] == 'r' && conf[1] == 'r' && conf[2] == 'c')
    reset_radar_cnt = TRUE;

  mtlk_df_ui_reset_stats(df, reset_radar_cnt);

  kfree(conf);
  return count;
}

static int mtlk_df_ui_he_mu_dump(mtlk_seq_entry_t *s, void *data)
{
  mtlk_clpb_t *clpb = NULL;
  mtlk_stadb_stat_t *stadb_stat;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_ui_get_stadb_status_req_t get_stadb_status_req;
  wave_radio_t  *radio =  wave_vap_radio_get(mtlk_df_get_vap_handle(mtlk_df_proc_seq_entry_get_df(s)));
  uint32 i, j;
  uint32 res = MTLK_ERR_OK;
  uint32 size;
  uint16 sid;
  BOOL found;

  UMI_DBG_HE_MU_GROUP_STATS UmiDbgMuGroupStats[HE_MU_MAX_NUM_OF_GROUPS];
  mtlk_pdb_size_t UmiDbgMuGroupStatsSize = sizeof(UmiDbgMuGroupStats);

  /* Read out current MU group stats */
  res = WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_PLAN_MU_GROUP_STATS, &UmiDbgMuGroupStats[0], &UmiDbgMuGroupStatsSize);

  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  for(i = 0; i < HE_MU_MAX_NUM_OF_GROUPS; i++) {

    if(HE_MU_GROUP_SET == UmiDbgMuGroupStats[i].setReset) {
                mtlk_aux_seq_printf(s,"groupId:%u planType:%u vapId:%u setReset:%u\n",
                UmiDbgMuGroupStats[i].groupId, UmiDbgMuGroupStats[i].planType,
                UmiDbgMuGroupStats[i].vapId, UmiDbgMuGroupStats[i].setReset);

      for(j = 0; j < HE_MU_MAX_NUM_OF_USERS_PER_GROUP; j++) {
        sid = UmiDbgMuGroupStats[i].stationId[j];

        if(INVALID_SID_FOR_HE_GROUP == sid) {
          continue;
        }

        get_stadb_status_req.get_hostdb = FALSE;
        get_stadb_status_req.use_cipher = FALSE;

        res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_STADB_STATUS, &clpb,
                                     &get_stadb_status_req, sizeof(get_stadb_status_req));
        res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_STADB_STATUS, FALSE);

        if (MTLK_ERR_OK != res) {
          goto err_ret;
        }

        found = FALSE;

        /* Enumerate the STA DB to find the current SIDs MAC address */
        while(NULL != (stadb_stat = mtlk_clpb_enum_get_next(clpb, &size))) {
          if (sizeof(*stadb_stat) != size) {
            res = MTLK_ERR_UNKNOWN;
            goto delete_clpb;
          }

          if ((STAT_ID_STADB == stadb_stat->type) || (STAT_ID_HSTDB == stadb_stat->type)) {
            if(sid == stadb_stat->u.general_stat.sta_sid)
            {
              mtlk_aux_seq_printf(s, "\tstationId: %d - " MAC_PRINTF_FMT "\n", sid, MAC_PRINTF_ARG(stadb_stat->u.general_stat.addr.au8Addr));
              found = TRUE;
            }
          } else {
            res = MTLK_ERR_UNKNOWN;
            goto delete_clpb;
          }
        }

        if(!found) {
          mtlk_aux_seq_printf(s, "\tstationId: %d - (MAC ADDRESS NOT FOUND)\n", sid);
        }

      }
    }
  }

delete_clpb:
  if(NULL != clpb) {
      mtlk_clpb_delete(clpb);
  }
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int mtlk_df_ui_serializer_dump(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  mtlk_serializer_command_info_t *cmd_info;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);

  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_SERIALIZER_INFO, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_SERIALIZER_INFO, FALSE);

  if (MTLK_ERR_OK != res) {
    goto err_ret;
  }

  mtlk_aux_seq_printf(s,"Serialized Commands:\nCurr Prio GID FID LID\n");

  while(NULL != (cmd_info = mtlk_clpb_enum_get_next(clpb, NULL))) {
    mtlk_aux_seq_printf(s,"%4u %4u %3u %3u %3u\n",
      cmd_info->is_current,
      cmd_info->priority,
      mtlk_slid_get_gid(cmd_info->issuer_slid),
      mtlk_slid_get_fid(cmd_info->issuer_slid),
      mtlk_slid_get_lid(cmd_info->issuer_slid));
  }

  mtlk_clpb_delete(clpb);
err_ret:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int df_ui_wds_dbg(mtlk_seq_entry_t *s, void *v)
{
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_core_t *core = mtlk_vap_get_core(mtlk_df_get_vap_handle(df));

  if (BR_MODE_WDS != MTLK_CORE_HOT_PATH_PDB_GET_INT(core, CORE_DB_CORE_BRIDGE_MODE)) {
    mtlk_aux_seq_printf(s,"\nWDS is disabled\n");
  } else {
    mtlk_hash_enum_t              e;
    MTLK_HASH_ENTRY_T(ieee_addr) *h;
    mtlk_osal_timestamp_diff_t diff;
    mtlk_osal_msec_t msecs;
    mtlk_dlist_entry_t *entry;
    mtlk_dlist_entry_t *head;
    wds_peer_t *peer;
    wds_t *wds = &core->slow_ctx->wds_mng;
    hst_db *hstdb = &core->slow_ctx->hstdb;
    mtlk_osal_timestamp_t timestamp = mtlk_osal_timestamp();
    static const char *wds_state_str[] = {"Disconnected",
                                          "Connecting",
                                          "Connected",
                                          "Disconnecting",
                                          "Error"};

    mtlk_aux_seq_printf(s,"\nWDS Peers list\n");

    mtlk_osal_lock_acquire(&wds->peers_list_lock);

    mtlk_dlist_foreach(&wds->peers_list, entry, head)
    {
      peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);

      mtlk_aux_seq_printf(s, "\nAddr: " MAC_PRINTF_FMT "\n", MAC_PRINTF_ARG(peer->addr.au8Addr));
      mtlk_aux_seq_printf(s,"      SID                           : %u\n", mtlk_core_get_sid_by_addr(core, peer->addr.au8Addr));
      mtlk_aux_seq_printf(s,"      State                         : %s\n", wds_state_str[peer->state]);
      mtlk_aux_seq_printf(s,"      HT support                    : %d\n", (unsigned)peer->is_ht);
      mtlk_aux_seq_printf(s,"      VHT support                   : %d\n", (unsigned)peer->is_vht);
      mtlk_aux_seq_printf(s,"      HE support                    : %d\n", (unsigned)peer->is_he);
      msecs = peer->state == WDS_STATE_CONNECTED ?
              mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp_diff(timestamp, peer->age_ts)): 0;
      mtlk_aux_seq_printf(s,"      Connection age                : %u ms\n", msecs);
      msecs = mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp_diff(timestamp, peer->beacon_age_ts));
      mtlk_aux_seq_printf(s,"      Last beacon age               : %u ms\n", msecs);
      mtlk_aux_seq_printf(s,"      Number of reconnections       : %u\n", peer->numof_reconnect);
      mtlk_aux_seq_printf(s,"      Reconnections on beacon change: %u\n", peer->numof_reconnect_beacon);
      mtlk_aux_seq_printf(s,"      Beacons with wrong channel    : %u\n", peer->numof_beacons_wrong_chan);
      mtlk_aux_seq_printf(s,"      Beacons with wrong bonding    : %u\n", peer->numof_beacons_wrong_bond);
    }
    mtlk_osal_lock_release(&wds->peers_list_lock);

    /* Printout host DB entries with STA associated with it
       as well as STA's MAC address and time since last incoming
       packet from that host */
    mtlk_aux_seq_printf(s,"\nWDS hosts list\n");

    mtlk_osal_lock_acquire(&hstdb->lock);
    h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
    mtlk_aux_seq_printf(s," | Host              | STA mac           |  STA       | Age (ms)   \n");
    mtlk_aux_seq_printf(s," |===================|===================|============|============\n");
  /*mtlk_aux_seq_printf(s," | 11:22:33:44:55:66 | 11:22:33:44:55:66 | 0x12345678 | 123...     \n");*/
    while (h) {
      host_entry *hst  = MTLK_CONTAINER_OF(h, host_entry, hentry);
      const IEEE_ADDR *addr = _mtlk_hst_get_addr(hst);

      diff = mtlk_osal_timestamp_diff(timestamp, hst->timestamp);
      msecs = mtlk_osal_timestamp_to_ms(diff);

      mtlk_aux_seq_printf(s," | " MAC_PRINTF_FMT " | " MAC_PRINTF_FMT " | %u\n",
                          MAC_PRINTF_ARG(addr->au8Addr),
                          MAC_PRINTF_ARG(hst->sta_addr.au8Addr),
                          msecs);

      h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);
    }
    mtlk_osal_lock_release(&hstdb->lock);
  }
  return MTLK_ERR_OK;
}

#endif /* CONFIG_WAVE_DEBUG */

static int _mtlk_df_version_dump(mtlk_seq_entry_t *s, void *v)
{
  int res = MTLK_ERR_OK;
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_vap_handle_t vap = mtlk_df_get_vap_handle(df);
  char *buffer;

  buffer = mtlk_osal_mem_alloc(MTLK_HW_VERSION_PRINTOUT_LEN, MTLK_MEM_TAG_VERSION_PRINTOUT);

  if (!buffer)
  {
    ELOG_V("Cannot allocate memory for version printout");
    res = MTLK_ERR_NO_MEM;
    goto finish;
  }

  res = mtlk_hw_get_prop(mtlk_vap_get_hwapi(vap), MTLK_HW_VERSION_PRINTOUT, buffer, MTLK_HW_VERSION_PRINTOUT_LEN);

  if (res != MTLK_ERR_OK)
  {
    ELOG_V("Cannot perform version printout");
    goto free_buffer;
  }

  mtlk_aux_seq_printf(s, "%s", buffer);

free_buffer:
  mtlk_osal_mem_free(buffer);

finish:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _mtlk_df_peer_flow_status(mtlk_seq_entry_t *s, void *data)
{
    int res = MTLK_ERR_NOT_SUPPORTED;
    mtlk_clpb_t *clpb = NULL;
    mtlk_stadb_stat_t *stadb_stat;
    mtlk_wssa_drv_peer_stats_t *peer_stats;
    uint32 size;
    mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);
    mtlk_core_ui_get_stadb_status_req_t get_stadb_status_req;
    unsigned long sta_cnt = 0;
    int i, rate, int_rate, frac_rate;

    /* Get Core general information from DF buffer and don't call Core */

    get_stadb_status_req.get_hostdb = FALSE;
    get_stadb_status_req.use_cipher = FALSE;
    res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_GET_STADB_STATUS, &clpb,
        &get_stadb_status_req, sizeof(get_stadb_status_req));
    res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_STADB_STATUS, FALSE);
    if (MTLK_ERR_OK != res) {
        goto err_ret;
    }

    /* enumerate sta entries */
    while (NULL != (stadb_stat = mtlk_clpb_enum_get_next(clpb, &size))) {
        if (sizeof(*stadb_stat) != size) {
            res = MTLK_ERR_UNKNOWN;
            goto delete_clpb;
        }

        peer_stats = &stadb_stat->u.general_stat.peer_stats;

        mtlk_aux_seq_printf(s, "\t" MAC_PRINTF_FMT " : %s\n", MAC_PRINTF_ARG(stadb_stat->u.general_stat.addr.au8Addr), "STA MAC");
        mtlk_aux_seq_printf(s, "\t%17u : %s\n", stadb_stat->u.general_stat.sta_sid, "STA ID");
        mtlk_aux_seq_printf(s, "\t%17u : %s\n", peer_stats->tr181_stats.traffic_stats.PacketsSent,     "HW Number of packets transmitted");
        mtlk_aux_seq_printf(s, "\t%17u : %s\n", peer_stats->tr181_stats.traffic_stats.PacketsReceived, "HW Number of packets received");
        mtlk_aux_seq_printf(s, "\t%17u : %s\n", peer_stats->tr181_stats.traffic_stats.BytesSent,       "HW Number of bytes sent successfully");
        mtlk_aux_seq_printf(s, "\t%17u : %s\n", peer_stats->tr181_stats.traffic_stats.BytesReceived,   "HW Number of bytes received");

        rate = peer_stats->tr181_stats.LastDataDownlinkRate;
        int_rate = rate / 10;
        frac_rate = rate % 10;
        mtlk_aux_seq_printf(s, "\t%15d.%1u : %s\n", int_rate, frac_rate, "Last data transmit rate");

        rate = peer_stats->tr181_stats.LastDataUplinkRate;
        int_rate = rate / 10;
        frac_rate = rate % 10;
        mtlk_aux_seq_printf(s, "\t%15d.%1u : %s\n", int_rate, frac_rate, "Last data receive rate");

        mtlk_aux_seq_printf(s, "\t%17u : %s\n", peer_stats->AirtimeUsage, "Air Time Used by RX/TX to/from STA [%]");
        mtlk_aux_seq_printf(s, "\t%17u : %s\n", peer_stats->AirtimeEfficiency, "Efficiency of used air time [bytes/sec]");

        for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
            mtlk_aux_seq_printf(s, "\t%17d : %s [%i]\n", peer_stats->ShortTermRSSIAverage[i], "Short-term RSSI average per antenna [dBm]", i);
        }

        for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
            mtlk_aux_seq_printf(s, "\t%17d : %s [%i]\n", peer_stats->snr[i], "Signal to Noise ratio per antenna [dB]", i);
        }
/*
        for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
            mtlk_aux_seq_printf(s, "\t%17d : %s [%i]\n", peer_stats->LongTermRSSIAverage[i], "Long-term RSSI average per antenna", i);
        }
*/
        mtlk_aux_seq_printf(s, "\n");
        sta_cnt++;
    }

delete_clpb:
    mtlk_clpb_delete(clpb);
err_ret:
    return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int __wave_df_ui_mgmt_tx (struct file *file, const char __user *buffer,
  unsigned long count, void *data, BOOL is_mngmnt, BOOL debug)
{
  mtlk_df_t *df = mtlk_df_proc_entry_get_df(data);
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_df_get_vap_manager(df));
  mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(df);
  struct mtlk_chan_def cd;
  mtlk_clpb_t *clpb = NULL;
  char *input = NULL, *pkt = NULL;
  struct mgmt_tx_params mtp;
  uint64 cookie;
  int res;
  unsigned frame_ctrl, subtype;
  size_t len;

  MTLK_UNREFERENCED_PARAM(vap_handle); /* used for logger only - silent build support */

  if (count <= 1) /* only '\n' or empty string */
    return (int)count;
  /* Note: this info could have been changing while we copied it and
   * we won't necessarily catch it with the is_channel_certain() trick.
   */
  cd = *__wave_core_chandef_get(mcore);
  if (!is_channel_certain(&cd))
  {
    ELOG_D("CID-%04x: channel not certain: changing or restored to unset state",
      mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_NOT_READY;
    goto end;
  }

  input = kmalloc(count, GFP_KERNEL);
  if (!input) {
    ELOG_DD("CID-%04x: unable to allocate %lu bytes",
      mtlk_vap_get_oid(vap_handle), count);
    res = MTLK_ERR_NO_MEM;
    goto end;
  }
  pkt = kmalloc(count >> 1, GFP_KERNEL);
  if (!pkt) {
    ELOG_DD("CID-%04x: unable to allocate %lu bytes",
      mtlk_vap_get_oid(vap_handle), count >> 1);
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  if (0 != (copy_from_user(input, buffer, count))) {
    ELOG_D("CID-%04x: copy_from_user error", mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }
  /* if the last character is \n, replace by 0 */
  if (input[count - 1] == '\n')
    input[count - 1] = '\0';
  len = wave_remove_spaces(input, count);
  if (hex2bin(pkt, input, len >> 1)) {
    ELOG_D("CID-%04x: unable to convert an ASCII hexadecimal string to "
      "its binary representation", mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_VALUE;
    goto end;
  }

  memset(&mtp, 0, sizeof(mtp));
  mtp.buf = pkt;
  mtp.len = len >> 1;
  if (mtp.len < sizeof(frame_head_t)) {
    ELOG_D("CID-%04x: Management Frame length is wrong",
      mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_NOT_IN_USE;
    goto end;
  }
  if (!debug) {
    frame_ctrl = mtlk_wlan_pkt_get_frame_ctl(mtp.buf);
    subtype = (frame_ctrl & FRAME_SUBTYPE_MASK) >> FRAME_SUBTYPE_SHIFT;
    if (MAN_TYPE_PROBE_REQ != subtype) {
      ELOG_DD("CID-%04x: invalid frame type %d, only sending of probe requests "
        "is allowed", mtlk_vap_get_oid(vap_handle), subtype);
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }
  mtp.channum = ieee80211_frequency_to_channel(cd.chan.center_freq);
  mtp.cookie = &cookie;
  if (is_mngmnt)
    mtp.extra_processing = PROCESS_MANAGEMENT;
  else
    mtp.extra_processing = PROCESS_EAPOL;
  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_MGMT_TX, &clpb, &mtp, sizeof(mtp));
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_MGMT_TX, TRUE);
  if (res != MTLK_ERR_OK) {
    WLOG_DD("CID-%04x: sending management frame to firmware failed (res=%d)",
      mtlk_vap_get_oid(vap_handle), res);
  }

end:
  kfree(input);
  kfree(pkt);
  return (res == MTLK_ERR_OK) ? (int)count : _mtlk_df_mtlk_to_linux_error_code(res);
}

#ifdef CPTCFG_IWLWAV_PKT_INJECT
static void
_mtlk_df_ui_mgmt_fill_dflt_phy_info (mtlk_phy_info_t *phy_info)
{
  mtlk_hw_mmb_fill_phy_info_by_default(phy_info);
}

static int _mtlk_df_ui_mgmt_rx (struct file *file, const char __user *buffer,
                                 unsigned long count, void *data)
{
  mtlk_df_t *df = mtlk_df_proc_entry_get_df(data);
  mtlk_clpb_t *clpb = NULL;
  char *input, *pkt;
  mtlk_core_handle_rx_bss_t rx_bss;
  int res;

  if (count <= 1) /* only '\n' */
      return count;

  input = kmalloc(count, GFP_KERNEL);
  if (!input) {
    ELOG_DD("CID-%04x: unable to allocate %lu bytes",
      mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)), count);
    return count;
  }
  pkt = kmalloc(count >> 1, GFP_KERNEL);
  if (!pkt) {
    ELOG_DD("CID-%04x: unable to allocate %lu bytes",
      mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)), count >> 1);
    goto end;
  }

  if (0 != (copy_from_user(input, buffer, count))) {
    ELOG_D("CID-%04x: copy_from_user error",
      mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)));
    goto end;
  }
  /* the last character is \n, not needed, replace by 0 */
  input[count - 1] = '\0';
  if (hex2bin(pkt, input, count >> 1)) {
    ELOG_DS("CID-%04x: unable to convert an ascii hexadecimal string [%s] to "
      "its binary representation",
      mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)),
      input);
    goto end;
  }

  memset(&rx_bss, 0, sizeof(rx_bss));
  rx_bss.buf = pkt;
  rx_bss.size = count >> 1;

  _mtlk_df_ui_mgmt_fill_dflt_phy_info(&rx_bss.phy_info);

  res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_MGMT_RX, &clpb, &rx_bss, sizeof(rx_bss));
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_MGMT_RX, TRUE);
  if (res != MTLK_ERR_OK) {
    WLOG_DD("CID-%04x: sending management frame to hostapd failed (res=%d)",
      mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)), res);
  }

end:
  if (input)
    kfree(input);
  if (pkt)
    kfree(pkt);
  return count;
}

static int _mtlk_df_ui_mgmt_tx(struct file *file, const char __user *buffer,
                                 unsigned long count, void *data)
{

  return __wave_df_ui_mgmt_tx(file, buffer, count, data, TRUE, TRUE);
}

static int _mtlk_df_ui_data_tx(struct file *file, const char __user *buffer,
                                 unsigned long count, void *data)
{
  return __wave_df_ui_mgmt_tx(file, buffer, count, data, FALSE, TRUE);
}

#endif /* CPTCFG_IWLWAV_PKT_INJECT */

static int _wave_df_ui_mgmt_tx (struct file *file, const char __user *buffer,
  unsigned long count, void *data)
{
  return __wave_df_ui_mgmt_tx (file, buffer, count, data, TRUE, FALSE);
}

/**************************************************************/

#ifdef EEPROM_DATA_ACCESS

static int
_mtlk_df_ui_eeprom(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_NOT_SUPPORTED;
  uint32 req_data_length;
  mtlk_df_t *m_df = mtlk_df_user_get_master_df(
          mtlk_df_get_user(mtlk_df_proc_seq_entry_get_df(s)));
  mtlk_eeprom_cfg_t *eeprom_cfg;
  mtlk_handle_t hdata = MTLK_INVALID_HANDLE;

  MTLK_UNREFERENCED_PARAM(data);

  res = _mtlk_df_user_pull_core_data(m_df, WAVE_HW_REQ_GET_EEPROM_CFG, FALSE,
                                     (void**) &eeprom_cfg, &req_data_length,
                                     &hdata);

  if((MTLK_ERR_OK == res) && (MTLK_INVALID_HANDLE != hdata)) {
    MTLK_ASSERT(sizeof(mtlk_eeprom_cfg_t) == req_data_length);
    mtlk_aux_seq_write(s, eeprom_cfg->eeprom_raw_data,
                               sizeof(eeprom_cfg->eeprom_raw_data));
    _mtlk_df_user_free_core_data(m_df, hdata);
  }

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int
_mtlk_df_ui_eeprom_parsed(mtlk_seq_entry_t *s, void *data)
{
  int res = MTLK_ERR_NOT_SUPPORTED;
  uint32 req_data_length;
  mtlk_df_t *m_df = mtlk_df_user_get_master_df(
          mtlk_df_get_user(mtlk_df_proc_seq_entry_get_df(s)));
  mtlk_eeprom_cfg_t *eeprom_cfg;
  mtlk_handle_t hdata = MTLK_INVALID_HANDLE;

  uint16 length = 0;
  uint32 max_length = 1024;
  char *buffer;

  MTLK_UNREFERENCED_PARAM(data);

  res = _mtlk_df_user_pull_core_data(m_df, WAVE_HW_REQ_GET_EEPROM_CFG, FALSE,
                                     (void**) &eeprom_cfg, &req_data_length,
                                     &hdata);
  if(MTLK_ERR_OK != res)
    goto finish;

  MTLK_ASSERT(sizeof(mtlk_eeprom_cfg_t) == req_data_length);

  buffer = mtlk_osal_mem_alloc(max_length, MTLK_MEM_TAG_CORE_CFG);
  if (!buffer) {
    res = MTLK_ERR_NO_RESOURCES;
    goto finish;
  }

  length += _mtlk_df_user_print_eeprom(&eeprom_cfg->eeprom_data, buffer, max_length);
  if (length < max_length)
    length += _mtlk_df_user_print_raw_eeprom_header(eeprom_cfg->eeprom_raw_data,
                                        eeprom_cfg->eeprom_data.hdr_size,
                                        buffer + length, max_length - length);

  if (length < max_length) {
    buffer[length++] = '\n';
  }
  else {
    WLOG_DD("Writing only %u bytes of data out of total %u", max_length, length);
    length = max_length;
    buffer[length - 1] = '\n';
  }

  mtlk_aux_seq_write(s, buffer, length);

  mtlk_osal_mem_free(buffer);

finish:
  if(MTLK_INVALID_HANDLE != hdata)
    _mtlk_df_user_free_core_data(m_df, hdata);

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

#endif /* EEPROM_DATA_ACCESS */

/*------ PHY_RX_STATUS -------*/

#define PROC_PHY_RX_STATUS  "phy_rx_status"

static int
_mtlk_df_ui_phy_rx_status_show (mtlk_seq_entry_t *s, void *data)
{
  mtlk_df_t        *df = mtlk_df_proc_seq_entry_get_df(s);
  mtlk_clpb_t      *clpb = NULL;
  wave_bin_data_t   phy_rx_status;
  int               buf_size;
  int               res;

  buf_size = s->size - 1;   /* -1 to prevent buffer overflow */
  phy_rx_status.buff = s->buf;
  phy_rx_status.size = buf_size;
  MTLK_ASSERT(buf_size > 0);

  res = _mtlk_df_user_invoke_core(df, WAVE_RADIO_REQ_GET_PHY_RX_STATUS, &clpb,
                                  &phy_rx_status, sizeof(phy_rx_status));
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_GET_PHY_RX_STATUS, TRUE);

  if (MTLK_ERR_OK == res) {
      mtlk_aux_seq_write(s, phy_rx_status.buff, buf_size);
  }

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

int __MTLK_IFUNC
mtlk_df_ui_phy_rx_status_create (mtlk_df_user_t *df_user, unsigned size)
{
  /* only for master vap */
  if (mtlk_vap_is_master(mtlk_df_get_vap_handle(df_user->df))) {
    return mtlk_df_proc_node_add_seq_entry_size(PROC_PHY_RX_STATUS,
             df_user->slow_ctx->proc_df_node, df_user->df, _mtlk_df_ui_phy_rx_status_show, size);
  } else {
    return MTLK_ERR_OK;
  }
}

void __MTLK_IFUNC
mtlk_df_ui_phy_rx_status_delete (mtlk_df_user_t *df_user)
{
  /* only for master vap */
  if (mtlk_vap_is_master(mtlk_df_get_vap_handle(df_user->df))) {
    mtlk_df_proc_node_remove_entry(PROC_PHY_RX_STATUS, df_user->slow_ctx->proc_df_node);
  }
}

/*--- end of PHY_RX_STATUS ---*/

#ifdef CPTCFG_IWLWAV_DBG_DRV_UTEST

#include "cfg80211.h"

static int mtlk_df_ui_utest (mtlk_seq_entry_t *s, void *data)
{
  mtlk_df_t *df = mtlk_df_proc_seq_entry_get_df(s);

  MTLK_ASSERT(NULL != df);

  wv_cfg80211_utest(mtlk_df_user_get_ndev(mtlk_df_get_user(df)));

  mtlk_aux_seq_printf(s, "\nUTEST - DONE\n");

  return 0;
}

#endif /* CPTCFG_IWLWAV_DBG_DRV_UTEST */

/*---------------------------------------------------------------------------*/
/* Proc files table.
 * Supported file types: 1) seq file    2) write_only file
 * Other types (read_only and read_write) are not needed.
 */

#define _NODBG_     FALSE
#define _DEBUG_     TRUE

struct _proc_file_info {
    char    *name;
    BOOL    is_dbg;
    mtlk_df_proc_seq_entry_show_f show_func;
    mtlk_df_proc_entry_write_f    write_func;
};

static struct _proc_file_info _proc_files_list[] =
{ /* SEQ */
#ifdef EEPROM_DATA_ACCESS
  { "eeprom",           _NODBG_,  _mtlk_df_ui_eeprom              , NULL },
  { "eeprom_parsed",    _NODBG_,  _mtlk_df_ui_eeprom_parsed       , NULL },
#endif /* EEPROM_DATA_ACCESS */
  { "tx_power",         _NODBG_,  mtlk_df_ui_tx_power             , NULL },
  { "channel",          _NODBG_,  _mtlk_df_ui_channel_read        , NULL },
  { "version",          _NODBG_,  _mtlk_df_version_dump           , NULL },
  { "PeerFlowStatus",   _NODBG_,  _mtlk_df_peer_flow_status       , NULL },
  { "General",          _NODBG_,  mtlk_df_ui_debug_general        , NULL },
  { "sta_list",         _NODBG_,  mtlk_df_ui_sta_list             , NULL },

#ifdef CPTCFG_IWLWAV_DBG_DRV_UTEST
{ "utest",              _DEBUG_,  mtlk_df_ui_utest                , NULL },
#endif
#ifdef CPTCFG_IWLWAV_PKT_INJECT
  { "mgmt_rx",          _DEBUG_,  NULL,  _mtlk_df_ui_mgmt_rx    },
  { "mgmt_tx",          _DEBUG_,  NULL,  _mtlk_df_ui_mgmt_tx    },
  { "data_tx",          _DEBUG_,  NULL,  _mtlk_df_ui_data_tx    },
#endif

  { "mgmt_tx",          _NODBG_,  NULL, _wave_df_ui_mgmt_tx      },
  { "radio_cfg",        _NODBG_,  _mtlk_df_ui_radio_cfg_read      , NULL },
  { "fw_trace",         _NODBG_,  _wave_df_ui_fw_trace_read       , NULL },
  { "hw_limits",        _NODBG_,  _mtlk_df_ui_hw_limits           , NULL },

#ifdef CONFIG_WAVE_DEBUG
  { "calibration",        _DEBUG_, _mtlk_df_ui_calibration_read   , NULL },
  { "hdk_config",         _DEBUG_, _mtlk_df_ui_hdkconfig_read     , NULL },
  { "hw_reg_dump",        _DEBUG_, _mtlk_df_ui_hw_reg_dump        , NULL },
  { "igmp",               _DEBUG_, _mtlk_df_ui_debug_igmp_read    , NULL },
  { "widan_blacklist",    _DEBUG_, _mtlk_df_ui_widan_blacklist    , NULL },
  { "multi_ap_blacklist", _DEBUG_, _mtlk_df_ui_multi_ap_blacklist , NULL },
  { "wds_wpa_sta_list",   _DEBUG_, _mtlk_df_ui_wds_wpa_sta_list   , NULL },
  { "qos_map",            _DEBUG_, _mtlk_df_qos_map_dump          , NULL },
  { "scan_request",       _DEBUG_, _mtlk_df_ui_scanrequest_read   , NULL },
  { "scan_support",       _DEBUG_, _mtlk_df_ui_scansupport_read   , NULL },
  { "surveys",            _DEBUG_, _mtlk_df_ui_survey_read        , NULL },
  { "trace_buffer",       _DEBUG_, _mtlk_df_ui_trace_buffer_read  , NULL },
  { "he_mu_groups",       _DEBUG_,  mtlk_df_ui_he_mu_dump         , NULL },
  { "serializer_dump",    _DEBUG_,  mtlk_df_ui_serializer_dump    , NULL },
  { "wds_dbg",            _DEBUG_,  df_ui_wds_dbg                 , NULL },

  /* WriteOnly */
  { "ResetStats",     _DEBUG_, NULL, _mtlk_df_ui_reset_stats_proc },
#endif
};

#undef _NODBG_
#undef _DEBUG_

static void _mtlk_df_ui_pre_proc_init(mtlk_df_user_t* df_user)
{
  unsigned i;
  mtlk_df_t *df = mtlk_df_user_get_df(df_user);
  mtlk_hw_t *hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df));

  /* if mgmttx_en parameter is 0, don't create mgmt_tx proc FS entry */
  if (mgmttx_en[mtlk_hw_mmb_get_card_idx(hw)] == 0)
    for (i = 0; i < MTLK_ARRAY_SIZE(_proc_files_list); i++) {
      if (_proc_files_list[i].write_func == (mtlk_df_proc_entry_write_f)_wave_df_ui_mgmt_tx)
        _proc_files_list[i].write_func = NULL;
    }
}

static int
_mtlk_df_reg_file (mtlk_df_user_t* df_user, const struct _proc_file_info *info)
{
  mtlk_df_proc_fs_node_t *df_node;

#ifdef CONFIG_WAVE_DEBUG
  df_node = info->is_dbg ?
                df_user->slow_ctx->proc_df_debug_node :
                df_user->slow_ctx->proc_df_node;
#else
  df_node = df_user->slow_ctx->proc_df_node;
#endif

  if (info->show_func) {
    return mtlk_df_proc_node_add_seq_entry(info->name, df_node, df_user->df, info->show_func);
  } else if (info->write_func) {
    return mtlk_df_proc_node_add_wo_entry(info->name, df_node, df_user->df, info->write_func);
  }
  return MTLK_ERR_OK;
}

static void
_mtlk_df_unreg_file (mtlk_df_user_t* df_user, const struct _proc_file_info *info)
{
  mtlk_df_proc_fs_node_t *df_node;

#ifdef CONFIG_WAVE_DEBUG
  df_node = info->is_dbg ?
                df_user->slow_ctx->proc_df_debug_node :
                df_user->slow_ctx->proc_df_node;
#else
  df_node = df_user->slow_ctx->proc_df_node;
#endif

  if (info->show_func != NULL || info->write_func != NULL)
    mtlk_df_proc_node_remove_entry(info->name, df_node);
}

/*---------------------------------------------------------------------------*/

/* Driver support only 40 bit and 104 bit length WEP keys.
 * Also, according to IEEE standard packet transmission
 * with zero-filled WEP key is not supported.
 */
int __MTLK_IFUNC
mtlk_df_ui_validate_wep_key (const uint8 *key, size_t length) {
  size_t i;
  MTLK_ASSERT(key);

  /* Check key length */
  if ((length != WAVE_WEP_KEY_WEP1_LENGTH) && /* 40 bit */
      (length != WAVE_WEP_KEY_WEP2_LENGTH)) { /* 104 bit */
    return MTLK_ERR_PARAMS;
  }

  /* Validate key's value. All-zero key value is invalid. */
  for (i = 0; i < length; i++)
    if (key[i])
      return MTLK_ERR_OK;

  return MTLK_ERR_PARAMS;
}

typedef int (*mtlk_df_ui_cfg_getter_f)(mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params, uint16* length);
typedef int (*mtlk_df_ui_cfg_setter_f)(mtlk_df_user_t* df_user, uint32 subcmd, char* data, mtlk_iwpriv_params_t params);

static mtlk_df_ui_cfg_getter_f _mtlk_df_ui_cfg_getters_tbl[] =
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  _mtlk_df_user_litepath_int_get_cfg,
#endif
#if MTLK_USE_PUMA6_UDMA
  _mtlk_df_user_udma_int_get_cfg,
#endif
  _mtlk_df_user_datapath_text_get_cfg,
#ifdef CONFIG_WAVE_DEBUG
  _mtlk_df_user_genl_int_get_cfg,
#endif
  _mtlk_df_user_iwpriv_get_param,
  NULL
};

static mtlk_df_ui_cfg_setter_f _mtlk_df_ui_cfg_setters_tbl[] =
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  _mtlk_df_user_litepath_int_set_cfg,
#endif
#if MTLK_USE_PUMA6_UDMA
  _mtlk_df_user_udma_int_set_cfg,
#endif
  _mtlk_df_user_iwpriv_set_param,
  NULL,
};

int __MTLK_IFUNC
mtlk_df_ui_get_cfg (mtlk_df_user_t* df_user,
                    uint32 subcmd_id,
                    char* value,
                    mtlk_iwpriv_params_t params,
                    uint16* length)
{
  int result = MTLK_ERR_NOT_HANDLED;
  mtlk_df_ui_cfg_getter_f *cfg_handlers_tbl = _mtlk_df_ui_cfg_getters_tbl;
  ILOG1_D("Sub command #0x%04x being processed", subcmd_id);

  while ((NULL != *cfg_handlers_tbl) && (MTLK_ERR_NOT_HANDLED == result)) {
    result = (*cfg_handlers_tbl++)(df_user, subcmd_id, value, params, length);
  }

  MTLK_ASSERT(MTLK_ERR_NOT_HANDLED != result);
  ILOG1_D("Sub command #0x%04x processing done", subcmd_id);

  return result;
}

int __MTLK_IFUNC
mtlk_df_ui_set_cfg (mtlk_df_user_t* df_user,
                    uint32 subcmd_id,
                    char* value,
                    mtlk_iwpriv_params_t params)
{
  int result = MTLK_ERR_NOT_HANDLED;
  mtlk_df_ui_cfg_setter_f *cfg_handlers_tbl = _mtlk_df_ui_cfg_setters_tbl;

  ILOG1_D("Sub command #0x%04x being processed", subcmd_id);

  while ((NULL != *cfg_handlers_tbl) && (MTLK_ERR_NOT_HANDLED == result)) {
    result = (*cfg_handlers_tbl++)(df_user, subcmd_id, value, params);
  }

  MTLK_ASSERT(MTLK_ERR_NOT_HANDLED != result);
  ILOG1_D("Sub command #0x%04x processing done", subcmd_id);

  return result;
}

static void
_mtlk_df_user_cleanup(mtlk_df_user_t *df_user)
{
  int i;

  if (!df_user->is_secondary_df) {
    MTLK_CLEANUP_BEGIN(df_user, MTLK_OBJ_PTR(df_user))
      for (i = 0; MTLK_CLEANUP_ITERATONS_LEFT(MTLK_OBJ_PTR(df_user), PROC_INIT) > 0; i++)
      {
        MTLK_CLEANUP_STEP_LOOP(df_user, PROC_INIT, MTLK_OBJ_PTR(df_user),
                               _mtlk_df_unreg_file, (df_user, &_proc_files_list[i]));
      }
      MTLK_CLEANUP_STEP(df_user, PRE_PROC_INIT, MTLK_OBJ_PTR(df_user),
        MTLK_NOACTION, ());

#if MTLK_USE_PUMA6_UDMA
      MTLK_CLEANUP_STEP(df_user, UDMA_INIT, MTLK_OBJ_PTR(df_user),
                        MTLK_NOACTION, ());
#endif

      MTLK_CLEANUP_STEP(df_user, STAT_TIMER, MTLK_OBJ_PTR(df_user),
                        mtlk_osal_timer_cleanup, (&df_user->slow_ctx->stat_timer));
#ifdef MTLK_PER_RATE_STAT
      MTLK_CLEANUP_STEP(df_user, CREATE_PER_RATE_STAT_DIR, MTLK_OBJ_PTR(df_user),
                        _mtlk_df_ui_delete_per_rate_stat_dir, (df_user));
#endif
#ifdef CONFIG_WAVE_DEBUG
      MTLK_CLEANUP_STEP(df_user, CREATE_DEBUG_DIR, MTLK_OBJ_PTR(df_user),
                        _mtlk_df_ui_delete_debug_dir, (df_user));
#endif
      MTLK_CLEANUP_STEP(df_user, CREATE_CARD_DIR, MTLK_OBJ_PTR(df_user),
                        _mtlk_df_ui_delete_card_dir, (df_user));
#ifdef CONFIG_WAVE_DEBUG
      MTLK_CLEANUP_STEP(df_user, DEBUG_BCL, MTLK_OBJ_PTR(df_user),
                        mtlk_df_debug_bcl_cleanup, (df_user));
#endif
      MTLK_CLEANUP_STEP(df_user, ALLOC_SLOW_CTX, MTLK_OBJ_PTR(df_user),
                        mtlk_osal_mem_free, (df_user->slow_ctx));
    MTLK_CLEANUP_END(df_user, MTLK_OBJ_PTR(df_user));
  }
}

static int
_mtlk_df_user_init(mtlk_df_user_t *df_user,  const char *name)
{
  unsigned i;

  wave_strcopy(df_user->name, name, IFNAMSIZ);
  ILOG0_SD("netdev name:%s secondary:%d", df_user->name, (int) df_user->is_secondary_df);
  df_user->hs20_enabled = FALSE;

  if (!df_user->is_secondary_df) {
    MTLK_INIT_TRY(df_user, MTLK_OBJ_PTR(df_user))
      MTLK_INIT_STEP_EX(df_user, ALLOC_SLOW_CTX, MTLK_OBJ_PTR(df_user),
                           mtlk_osal_mem_alloc, (sizeof(*df_user->slow_ctx), MTLK_MEM_TAG_DF_SLOW),
                           df_user->slow_ctx, NULL != df_user->slow_ctx, MTLK_ERR_NO_MEM);

      memset(&df_user->slow_ctx->linux_stats, 0, sizeof(df_user->slow_ctx->linux_stats));
      memset(&df_user->slow_ctx->iw_stats, 0, sizeof(df_user->slow_ctx->iw_stats));
      memset(&df_user->slow_ctx->core_general_stats, 0, sizeof(df_user->slow_ctx->core_general_stats));

#ifdef CONFIG_WAVE_DEBUG
      MTLK_INIT_STEP(df_user, DEBUG_BCL, MTLK_OBJ_PTR(df_user),
                        mtlk_df_debug_bcl_init, (df_user));
#endif

      MTLK_INIT_STEP(df_user, CREATE_CARD_DIR, MTLK_OBJ_PTR(df_user),
                      _mtlk_df_ui_create_card_dir, (df_user));

#ifdef CONFIG_WAVE_DEBUG
      MTLK_INIT_STEP(df_user, CREATE_DEBUG_DIR, MTLK_OBJ_PTR(df_user),
                      _mtlk_df_ui_create_debug_dir, (df_user));
#endif

#ifdef MTLK_PER_RATE_STAT
      MTLK_INIT_STEP(df_user, CREATE_PER_RATE_STAT_DIR, MTLK_OBJ_PTR(df_user),
                      _mtlk_df_ui_create_per_rate_stat_dir, (df_user));
#endif

      MTLK_INIT_STEP(df_user, STAT_TIMER, MTLK_OBJ_PTR(df_user),
                      mtlk_osal_timer_init, (&df_user->slow_ctx->stat_timer, _mtlk_df_poll_stats, HANDLE_T(df_user)));

#if MTLK_USE_PUMA6_UDMA
      MTLK_INIT_STEP_VOID(df_user, UDMA_INIT, MTLK_OBJ_PTR(df_user),
                   _mtlk_df_user_udma_init, (df_user));
#endif

      MTLK_INIT_STEP_VOID(df_user, PRE_PROC_INIT, MTLK_OBJ_PTR(df_user),
                           _mtlk_df_ui_pre_proc_init, (df_user));

      for (i = 0; i < MTLK_ARRAY_SIZE(_proc_files_list); i++) {
        MTLK_INIT_STEP_LOOP(df_user, PROC_INIT, MTLK_OBJ_PTR(df_user),
                             _mtlk_df_reg_file, (df_user, &_proc_files_list[i]));
      }
    MTLK_INIT_FINALLY(df_user, MTLK_OBJ_PTR(df_user))
    MTLK_INIT_RETURN(df_user, MTLK_OBJ_PTR(df_user), _mtlk_df_user_cleanup, (df_user))
  } else
    return MTLK_ERR_OK;
}

mtlk_df_user_t*
mtlk_df_user_create (mtlk_df_t *df, const char *name, struct net_device *ndev_p)
{
  mtlk_df_user_t* df_user;

  /* Allocate df_user */
  df_user = mtlk_osal_mem_alloc(sizeof(mtlk_df_user_t), MTLK_MEM_TAG_DF);
  if (df_user == NULL){
    ELOG_V("Failed to allocate df_user");
    return NULL;
  }
  memset (df_user, 0, sizeof(mtlk_df_user_t));

  if(mtlk_vap_is_master_ap(mtlk_df_get_vap_handle(df)) && ndev_p == NULL) {
    struct ieee80211_hw *hw = wave_vap_ieee80211_hw_get(mtlk_df_get_vap_handle(df));
    ILOG0_V("Creating master netdev");
    ndev_p = ieee80211_if_alloc(hw, name, 0, NL80211_IFTYPE_AP, NULL);
    if (ndev_p == NULL) {
      ELOG_V("Failed to allocate net device for master df user");
      goto free_df_user;
    }
  } else if (ndev_p == NULL) {
    /* seconday df user */
    mtlk_df_user_p *df_user_pp;
    int rtnl_locked, res;

    df_user->is_secondary_df = TRUE;
    ndev_p = alloc_netdev(sizeof(mtlk_df_user_p), name, NET_NAME_UNKNOWN,
                          _mtlk_secondary_df_user_ndev_setup);
    if (ndev_p == NULL) {
      ELOG_V("Failed to allocate net device for secondary df user");
      goto free_df_user;
    }

    df_user_pp = (mtlk_df_user_p*)netdev_priv(ndev_p);
    *df_user_pp = df_user;

    rtnl_locked = rtnl_trylock();
    res = dev_alloc_name(ndev_p, name);
    if (res < 0) {
      if (rtnl_locked) rtnl_unlock();
      ELOG_V("failed to allocate net device name");
      goto free_ndev;
    }

    res = register_netdevice(ndev_p);
    if (rtnl_locked) rtnl_unlock();
    if (res < 0) {
      ELOG_V("failed to register net device");
      goto free_ndev;
    }
    df_user->ndev_registered = TRUE;
  }

  /* link DF User with DF */
  df_user->df = df;

  /* link DF User with Linux network device */
  df_user->ndev = ndev_p;

#ifdef CONFIG_WAVE_DEBUG
  if (ndev_p && !df_user->is_secondary_df) {
    /* Configure bcl iwpriv handlers */
    ndev_p->wireless_handlers = &wave_linux_iw_core_handler_bcl_only;
  }
#endif

  if (MTLK_ERR_OK != _mtlk_df_user_init(df_user, name)) {
    if (df_user->ndev_registered) {
      int rtnl_locked = rtnl_trylock();
      unregister_netdevice(ndev_p);
      if (rtnl_locked) rtnl_unlock();
    }
    goto free_df_user;
  }

  return df_user;

free_ndev:
  free_netdev(ndev_p);
free_df_user:
  mtlk_osal_mem_free(df_user);
  return NULL;
}

void
mtlk_df_user_delete(mtlk_df_user_t* df_user)
{
  _mtlk_df_user_cleanup(df_user);
  if (df_user->ndev_registered) {
    int rtnl_locked = rtnl_trylock();
    unregister_netdevice(df_user->ndev);
    if (rtnl_locked) rtnl_unlock();
  }
  mtlk_osal_mem_free (df_user);
}

int
mtlk_df_user_set_iftype(mtlk_df_user_t* df_user, enum nl80211_iftype iftype)
{
  bool is_ap = mtlk_vap_is_ap(mtlk_df_get_vap_handle(df_user->df));

  if (is_ap)
    df_user->wdev.iftype = iftype;

  return MTLK_ERR_OK;
}

#ifdef CONFIG_WAVE_DEBUG
/******************************************************************************************
 * BCL debugging interface implementation
 ******************************************************************************************/
/* TODO: add packing
 * Do not change this structure (synchronized with BCLSockServer) */
typedef struct _BCL_DRV_DATA_EX_REQUEST
{
    uint32         mode;
    uint32         category;
    uint32         index;
    uint32         datalen;
} BCL_DRV_DATA_EX_REQUEST;

/* BCL Driver message modes
 * Do not change these numbers (synchronized with BCLSockServer) */
typedef enum
{
    BclDrvModeCatInit  = 1,
    BclDrvModeCatFree  = 2,
    BclDrvModeNameGet  = 3,
    BclDrvModeValGet   = 4,
    BclDrvModeValSet   = 5
} BCL_DRV_DATA_EX_MODE;

/* BCL Driver message categories for DRV_* commands */
/* Do not change these values (synchronized with BBStudio) */
#define DRVCAT_DBG_API_RESET         1
#define DRVCAT_DBG_API_GENERAL_PKT   2
#define DRVCAT_DBG_API_GENERAL       3
/* #define DRVCAT_DBG_API_MAC_STATS     4 */ /* unsupported */
#define DRVCAT_DBG_API_RR_STATS      5

/* Subcommand indices for DRVCAT_DBG_API_RESET: */
/* Do not change these values (synchronized with BBStudio) */
#define IDX_DBG_API_RESET_ALL  1

/***** DRVCAT_DBG_API_RR_STATS categories definitions */
/**
 * The header string should be returned on get_text with 0 index value.
 **/
const char bcl_rr_counters_hdr[] = "MAC|ID|Too old|Duplicate|Queued|Overflows|Lost";
const int bcl_rr_counters_num_cnts = 5;

/***** DRVCAT_DBG_API_GENERAL_PKT categories definitions */
/**
 * The header string should be returned on get_text with 0 index value.
 * The footer string should be returned on get_text with last index value.
 **/
const char bcl_pkt_counters_hdr[] = "MAC|Packets received|Packets sent";
const int bcl_pkt_counters_num_cnts = 2;
const char bcl_pkt_counters_ftr[] = "Total";

/***** DRVCAT_DBG_API_GENERAL categories definitions */
#define MAX_STAT_NAME_LENGTH 256

#define FETCH_NAME 1
#define FETCH_VAL 2

/* General statistic data processing structures */
struct bcl_general_count_stat_params_t
{
  int num_stats;
};

struct bcl_general_fetch_stat_params_t
{
  int index_search;
  int index_cur;
  int what;
  char name[MAX_STAT_NAME_LENGTH];
  unsigned long val;
};

/***********************************************************
 * BCL function implementation
 ***********************************************************/
static int
mtlk_df_debug_bcl_category_free(mtlk_df_user_t *df_user, uint32 category);

void
mtlk_df_debug_bcl_cleanup(mtlk_df_user_t *df_user)
{
  mtlk_df_debug_bcl_category_free(df_user, DRVCAT_DBG_API_GENERAL);
  mtlk_df_debug_bcl_category_free(df_user, DRVCAT_DBG_API_GENERAL_PKT);
  mtlk_df_debug_bcl_category_free(df_user, DRVCAT_DBG_API_RR_STATS);
}

int
mtlk_df_debug_bcl_init(mtlk_df_user_t *df_user)
{
  df_user->slow_ctx->dbg_rr_addr = NULL;
  df_user->slow_ctx->dbg_rr_addr_num = 0;
  df_user->slow_ctx->dbg_rr_cnts = NULL;
  df_user->slow_ctx->dbg_rr_cnts_num = 0;

  df_user->slow_ctx->dbg_general_pkt_addr = NULL;
  df_user->slow_ctx->dbg_general_pkt_addr_num = 0;
  df_user->slow_ctx->dbg_general_pkt_cnts = NULL;
  df_user->slow_ctx->dbg_general_pkt_cnts_num = 0;
  return MTLK_ERR_OK;
}

static int
mtlk_df_debug_bcl_debug_rr_counters_init(mtlk_df_user_t *df_user, uint32 *pcnt)
{
  int                    res = MTLK_ERR_NOT_SUPPORTED;
  mtlk_clpb_t            *clpb = NULL;
  uint32                 size;
  mtlk_stadb_stat_t      *stadb_stat;
  mtlk_core_ui_get_stadb_status_req_t get_stadb_status_req;

  uint32                 num_sta_connected = 0;
  uint32                 addr_num_entries_max = 0;
  uint32                 cnt_num_entries_max = 0;
  uint32                 *dbg_rr_cnts = NULL;
  IEEE_ADDR              *dbg_rr_addr = NULL;

  uint32                 addr_cur_entry = 0;
  uint32                 cnts_cur_entry = 0;

  ILOG2_S("%s: Create rr_counters", mtlk_df_user_get_name(df_user));

  ASSERT(df_user->slow_ctx->dbg_rr_addr == NULL);
  ASSERT(df_user->slow_ctx->dbg_rr_cnts == NULL);

  *pcnt = 0;

  get_stadb_status_req.get_hostdb = FALSE;
  get_stadb_status_req.use_cipher = FALSE;
  res = _mtlk_df_user_invoke_core(df_user->df, WAVE_CORE_REQ_GET_STADB_STATUS,
                                  &clpb, &get_stadb_status_req, sizeof(get_stadb_status_req));
  res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_STADB_STATUS, FALSE);
  if (MTLK_ERR_OK != res) {
    goto finish;
  }

  num_sta_connected = mtlk_clpb_get_num_of_elements(clpb);
  if (0 == num_sta_connected) {
    goto delete_clpb;
  }

  addr_num_entries_max = num_sta_connected * NTS_TIDS;
  cnt_num_entries_max = num_sta_connected * NTS_TIDS * (bcl_rr_counters_num_cnts + 1/*+TID*/);

  dbg_rr_addr = mtlk_osal_mem_alloc(addr_num_entries_max * sizeof(IEEE_ADDR), MTLK_MEM_TAG_DEBUG_DATA);
  if (NULL == dbg_rr_addr) {
    ELOG_V("Out of memory");
    res = MTLK_ERR_NO_MEM;
    goto delete_clpb;
  }
  memset(dbg_rr_addr, 0, addr_num_entries_max * sizeof(IEEE_ADDR));

  dbg_rr_cnts = mtlk_osal_mem_alloc(cnt_num_entries_max * sizeof(uint32), MTLK_MEM_TAG_DEBUG_DATA);
  if (NULL == dbg_rr_cnts) {
    ELOG_V("Out of memory");
    res = MTLK_ERR_NO_MEM;
    goto delete_dbg_rr_addr;
  }
  memset(dbg_rr_cnts, 0, cnt_num_entries_max * sizeof(uint32));

  addr_cur_entry = 0;
  cnts_cur_entry = 0;

  while(NULL != (stadb_stat = mtlk_clpb_enum_get_next(clpb, &size))) {
    if ((sizeof(*stadb_stat) != size) || (STAT_ID_STADB != stadb_stat->type)) {
      res = MTLK_ERR_UNKNOWN;
      goto delete_dbg_rr_cnts;
    }
  }

  df_user->slow_ctx->dbg_rr_addr = dbg_rr_addr;
  df_user->slow_ctx->dbg_rr_addr_num = addr_cur_entry;
  df_user->slow_ctx->dbg_rr_cnts = dbg_rr_cnts;
  df_user->slow_ctx->dbg_rr_cnts_num = cnts_cur_entry;

  ILOG2_DDDDD("Created "
      "num_sta_connected (%d), "
      "addr_num (%d), cnts_num (%d), "
      "addr_num_entries_max (%d), cnt_num_entries_max (%d)",
      num_sta_connected,
      addr_cur_entry, cnts_cur_entry,
      addr_num_entries_max, cnt_num_entries_max);

  *pcnt = addr_cur_entry;

  if (0 == addr_cur_entry) {
    /* Not TIDs used */
    goto delete_dbg_rr_cnts;
  }

  goto delete_clpb;

delete_dbg_rr_cnts:
  mtlk_osal_mem_free(dbg_rr_cnts);
delete_dbg_rr_addr:
  mtlk_osal_mem_free(dbg_rr_addr);
delete_clpb:
  mtlk_clpb_delete(clpb);
finish:
  return res;
}

static int
mtlk_df_debug_bcl_debug_pkt_counters_init(mtlk_df_user_t *df_user, uint32 *pcnt)
{
  int                    res = MTLK_ERR_NOT_SUPPORTED;
  mtlk_clpb_t            *clpb = NULL;
  uint32                 size;

  uint32                 num_sta_connected;
  uint32                 addr_num_entries_max;
  uint32                 cnt_num_entries_max;
  uint32                 *dbg_general_pkt_cnts = NULL;
  IEEE_ADDR              *dbg_general_pkt_addr = NULL;
  uint32                 addr_cur_entry = 0;
  uint32                 cnts_cur_entry = 0;
  uint32                 total_rx_packets = 0;
  uint32                 total_tx_packets = 0;

  ILOG2_S("%s: Create pkt_counters", mtlk_df_user_get_name(df_user));

  ASSERT(df_user->slow_ctx->dbg_general_pkt_cnts == NULL);
  ASSERT(df_user->slow_ctx->dbg_general_pkt_addr == NULL);

  *pcnt = 0;

  if (mtlk_df_is_ap(df_user->df)) { /*AP*/
    mtlk_stadb_stat_t      *stadb_stat;
    mtlk_core_ui_get_stadb_status_req_t get_stadb_status_req;

    get_stadb_status_req.get_hostdb = FALSE;
    get_stadb_status_req.use_cipher = FALSE;
    res = _mtlk_df_user_invoke_core(df_user->df, WAVE_CORE_REQ_GET_STADB_STATUS,
                                    &clpb, &get_stadb_status_req, sizeof(get_stadb_status_req));
    res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_STADB_STATUS, FALSE);
    if (MTLK_ERR_OK != res) {
      goto finish;
    }

    num_sta_connected = mtlk_clpb_get_num_of_elements(clpb);
    if (0 == num_sta_connected) {
      goto finish;
    }

    addr_num_entries_max = num_sta_connected;
    cnt_num_entries_max = (num_sta_connected + 1 /*Total*/) * bcl_pkt_counters_num_cnts;

    dbg_general_pkt_addr = mtlk_osal_mem_alloc(addr_num_entries_max * sizeof(IEEE_ADDR), MTLK_MEM_TAG_DEBUG_DATA);
    if (NULL == dbg_general_pkt_addr) {
      ELOG_V("Out of memory");
      res = MTLK_ERR_NO_MEM;
      goto finish;
    }
    memset(dbg_general_pkt_addr, 0, addr_num_entries_max * sizeof(IEEE_ADDR));

    dbg_general_pkt_cnts = mtlk_osal_mem_alloc(cnt_num_entries_max * sizeof(uint32), MTLK_MEM_TAG_DEBUG_DATA);
    if (NULL == dbg_general_pkt_cnts) {
      ELOG_V("Out of memory");
      res = MTLK_ERR_NO_MEM;
      goto finish;
    }
    memset(dbg_general_pkt_cnts, 0, cnt_num_entries_max * sizeof(uint32));

    while(NULL != (stadb_stat = mtlk_clpb_enum_get_next(clpb, &size))) {
      if ((sizeof(*stadb_stat) != size) || (STAT_ID_STADB != stadb_stat->type)) {
        res = MTLK_ERR_UNKNOWN;
        goto finish;
      }

      ASSERT(addr_cur_entry < addr_num_entries_max);
      dbg_general_pkt_addr[addr_cur_entry++] = stadb_stat->u.general_stat.addr;

      total_rx_packets += stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsReceived;
      total_tx_packets += stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsSent;

      ASSERT(cnts_cur_entry < cnt_num_entries_max);
      dbg_general_pkt_cnts[cnts_cur_entry++] = stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsReceived;

      ASSERT(cnts_cur_entry < cnt_num_entries_max);
      dbg_general_pkt_cnts[cnts_cur_entry++] = stadb_stat->u.general_stat.peer_stats.tr181_stats.traffic_stats.PacketsSent;
    }
  } else { /*STA*/
    mtlk_core_general_stats_t *general_stats;

    /* Get Core general information from DF buffer and don't call Core */
    general_stats = &df_user->slow_ctx->core_general_stats;

    if (!core_cfg_net_state_is_connected(general_stats->net_state)) {
      res = MTLK_ERR_NOT_READY;
      goto finish;
    }

    num_sta_connected = 1;
    addr_num_entries_max = num_sta_connected;
    cnt_num_entries_max = (num_sta_connected + 1 /*Total*/) * bcl_pkt_counters_num_cnts;

    dbg_general_pkt_addr = mtlk_osal_mem_alloc(addr_num_entries_max * sizeof(IEEE_ADDR), MTLK_MEM_TAG_DEBUG_DATA);
    if (NULL == dbg_general_pkt_addr) {
      ELOG_V("Out of memory");
      res = MTLK_ERR_NO_MEM;
      goto finish;
    }
    memset(dbg_general_pkt_addr, 0, addr_num_entries_max * sizeof(IEEE_ADDR));

    dbg_general_pkt_cnts = mtlk_osal_mem_alloc(cnt_num_entries_max * sizeof(uint32), MTLK_MEM_TAG_DEBUG_DATA);
    if (NULL == dbg_general_pkt_cnts) {
      ELOG_V("Out of memory");
      res = MTLK_ERR_NO_MEM;
      goto finish;
    }
    memset(dbg_general_pkt_cnts, 0, cnt_num_entries_max * sizeof(uint32));

    dbg_general_pkt_addr[addr_cur_entry++] = general_stats->bssid;

    total_rx_packets = general_stats->core_priv_stats.sta_session_rx_packets;
    total_tx_packets = general_stats->core_priv_stats.sta_session_tx_packets;

    dbg_general_pkt_cnts[cnts_cur_entry++] = total_rx_packets;
    dbg_general_pkt_cnts[cnts_cur_entry++] = total_tx_packets;
  }

  ASSERT(cnts_cur_entry < cnt_num_entries_max);
  dbg_general_pkt_cnts[cnts_cur_entry++] = total_rx_packets;

  ASSERT(cnts_cur_entry < cnt_num_entries_max);
  dbg_general_pkt_cnts[cnts_cur_entry++] = total_tx_packets;

  df_user->slow_ctx->dbg_general_pkt_addr = dbg_general_pkt_addr;
  df_user->slow_ctx->dbg_general_pkt_addr_num = addr_cur_entry;
  df_user->slow_ctx->dbg_general_pkt_cnts = dbg_general_pkt_cnts;
  df_user->slow_ctx->dbg_general_pkt_cnts_num = cnts_cur_entry;

  ILOG2_DDDDD("Created "
      "num_sta_connected (%d), "
      "addr_num (%d), cnts_num (%d), "
      "addr_num_entries_max (%d), cnt_num_entries_max (%d)",
      num_sta_connected,
      addr_cur_entry, cnts_cur_entry,
      addr_num_entries_max, cnt_num_entries_max);

  *pcnt = addr_cur_entry + 1 /*Total*/;

finish:
  if (dbg_general_pkt_cnts) mtlk_osal_mem_free(dbg_general_pkt_cnts);
  if (dbg_general_pkt_addr) mtlk_osal_mem_free(dbg_general_pkt_addr);
  if (clpb) mtlk_clpb_delete(clpb);
  return res;
}

static int mtlk_df_debug_bcl_debug_general_iterate(mtlk_df_user_t *df_user,
    int (* fn)(void *params, unsigned long val, const char* str), void *params)
{
  int i;
  char buf[MAX_STAT_NAME_LENGTH];

  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.num_disconnects,
        "Disconnections: disconnect/deactivate"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.rx_dat_frames,
        "data frames received"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.rx_ctl_frames,
        "control frames received"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.tx_man_frames_res_queue,
        "management frames in reserved queue"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.tx_man_frames_sent,
        "management frames sent"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.tx_man_frames_confirmed,
        "management frames confirmed"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.rx_man_frames,
        "management frames received"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.tx_max_cons_drop,
        "TX maximum consecutive dropped packets"))
    return MTLK_ERR_PARAMS;

  for (i = 0; i < NTS_TIDS; i++) {
    mtlk_snprintf(buf, sizeof(buf), "MSDUs received, QoS priority %d", i);
    if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.ac_rx_counter[i], buf))
      return MTLK_ERR_PARAMS;
  }

  for (i = 0; i < NTS_TIDS; i++) {
    mtlk_snprintf(buf, sizeof(buf), "MSDUs transmitted, QoS priority %d", i);
    if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.ac_tx_counter[i], buf))
      return MTLK_ERR_PARAMS;
  }

  for (i = 0; i < NTS_TIDS; i++) {
    mtlk_snprintf(buf, sizeof(buf), "MSDUs dropped, QoS priority %d", i);
    if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.ac_dropped_counter[i], buf))
      return MTLK_ERR_PARAMS;
  }

  for (i = 0; i < NTS_TIDS; i++) {
    mtlk_snprintf(buf, sizeof(buf), "MSDUs used, QoS priority %d", i);
    if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.ac_used_counter[i], buf))
      return MTLK_ERR_PARAMS;
  }

  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.tx_msdus_free, "TX MSDUs free"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.tx_msdus_usage_peak, "TX MSDUs usage peak"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.dgaf_disabled_tx_pck_dropped,
      "DGAF disabled: TX DHCP packets converted to Unicast"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.dgaf_disabled_tx_pck_converted,
      "DGAF disabled: TX Broadcast packets dropped"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.radars_detected,
      "Radars detected"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.channel_load,
      "Channel Load [%]"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK != fn(params, df_user->slow_ctx->core_general_stats.fwd_rx_packets,
        "packets received that should be forwarded to one or more STAs"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.fwd_rx_bytes,
        "bytes received that should be forwarded to one or more STAs"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.fwd_tx_packets,
        "packets transmitted for forwarded data"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.fwd_tx_bytes,
        "bytes transmitted for forwarded data"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.fwd_dropped,
        "forwarding (transmission) failures"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.fwd_cannot_clone,
        "forwarding (transmission): packet's cloning failures"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.rmcast_dropped,
        "reliable multicast (transmission) failures"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.unicast_replayed_packets+
                               df_user->slow_ctx->core_general_stats.multicast_replayed_packets+
                               df_user->slow_ctx->core_general_stats.management_replayed_packets,
        "packets replayed"))
    return MTLK_ERR_PARAMS;
  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.core_priv_stats.bars_cnt,
        "BAR frames received"))
    return MTLK_ERR_PARAMS;

#if MTLK_USE_DIRECTCONNECT_DP_API
  {
    mtlk_df_user_litepath_stats_t litepath_stats;

    _mtlk_df_user_litepath_get_stats(df_user, &litepath_stats);
    if (MTLK_ERR_OK !=fn(params, litepath_stats.tx_processed,       "TX Frames processed from LitePath"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, litepath_stats.tx_sent_up,         "TX Frames sent up from LitePath"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, _mtlk_df_user_litepath_tx_dropped, "TX Frames dropped from LitePath"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, litepath_stats.rx_accepted,        "RX Frames accepted by LitePath"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, litepath_stats.rx_rejected,        "RX Frames rejected by LitePath"))
      return MTLK_ERR_PARAMS;
  }

  {
    struct rtnl_link_stats64 if_stats;
    if (DC_DP_SUCCESS == dc_dp_get_netif_stats(mtlk_df_user_get_ndev(df_user), &df_user->dcdp.subif_id, &if_stats, DC_DP_F_SUBIF_LOGICAL)) {
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.rx_packets, "DC: total packets received"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.tx_packets, "DC: total packets transmitted"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.rx_bytes,   "DC: total bytes received"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.tx_bytes,   "DC: total bytes transmitted"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.rx_errors,  "DC: bad packets received"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.tx_errors,  "DC: packet transmit problems"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.rx_dropped, "DC: total RX packets dropped"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.tx_dropped, "DC: total TX packets dropped"))
            return MTLK_ERR_PARAMS;
        if (MTLK_ERR_OK !=fn(params, (unsigned long)if_stats.multicast,  "DC: multicast packets received"))
            return MTLK_ERR_PARAMS;
    }
  }
#endif

#if MTLK_USE_PUMA6_UDMA
  if (__mtlk_df_user_udma_is_available(df_user))
  {
    mtlk_df_user_udma_stats_t udma_stats;

    _mtlk_df_user_udma_get_stats(df_user, &udma_stats);

    if (MTLK_ERR_OK !=fn(params, udma_stats.tx_processed, "TX Frames processed from UDMA"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, udma_stats.tx_rejected_no_vlan_tag, "TX Frames rejected (no VLAN tag)"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, udma_stats.tx_rejected_no_vlan_registered, "TX Frames rejected (VLAN isn't registered)"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, udma_stats.rx_accepted, "RX Frames accepted by UDMA"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, udma_stats.rx_rejected, "RX Frames rejected by UDMA"))
      return MTLK_ERR_PARAMS;
    if (MTLK_ERR_OK !=fn(params, udma_stats.rx_tagging_error, "RX Frames VLAN tagging errors"))
      return MTLK_ERR_PARAMS;
  }
#endif

  if (MTLK_ERR_OK !=fn(params, df_user->slow_ctx->core_general_stats.bist_check_passed, "BIST check passed"))
    return MTLK_ERR_PARAMS;

  return MTLK_ERR_OK;
}

static int mtlk_df_debug_bcl_debug_general_count_stat(void *params, unsigned long val, const char* str)
{
  struct bcl_general_count_stat_params_t *pcsp = (struct bcl_general_count_stat_params_t *) params;
  ++pcsp->num_stats;
  return MTLK_ERR_OK;
}

static int mtlk_df_debug_bcl_debug_general_fetch_stat(void *params, unsigned long val, const char *str)
{
  struct bcl_general_fetch_stat_params_t *pfsp = (struct bcl_general_fetch_stat_params_t*) params;
  int res = MTLK_ERR_OK;

  if (pfsp->index_cur == pfsp->index_search) {
    if (pfsp->what == FETCH_VAL)
      pfsp->val = val;

    else if (pfsp->what == FETCH_NAME) {
      int rslt = mtlk_snprintf(pfsp->name, MAX_STAT_NAME_LENGTH, "%s", str);
      if (rslt < 0 || rslt >= MAX_STAT_NAME_LENGTH)
        res = MTLK_ERR_PARAMS;

    } else {
      res = MTLK_ERR_PARAMS;
    }
  }
  ++pfsp->index_cur;
  return res;
}

static int
mtlk_df_debug_bcl_debug_general_init(mtlk_df_user_t *df_user, uint32 *pcnt)
{
  int res = MTLK_ERR_OK;
  struct bcl_general_count_stat_params_t  csp;

  ILOG2_S("%s: Create general", mtlk_df_user_get_name(df_user));

  /* Get Core general information from DF buffer and don't call Core */
  *pcnt = 0;

  csp.num_stats = 0;
  res = mtlk_df_debug_bcl_debug_general_iterate(
          df_user, &mtlk_df_debug_bcl_debug_general_count_stat, &csp);
  if (MTLK_ERR_OK != res) {
    ELOG_V("Error while iterating driver statistics");
    goto finish;
  }

  *pcnt = csp.num_stats;

finish:
  return res;
}

static int
mtlk_df_debug_bcl_category_init(mtlk_df_user_t *df_user, uint32 category, uint32 *cnt)
{
  int res = MTLK_ERR_PARAMS;

  mtlk_df_debug_bcl_category_free(df_user, category);

  switch (category)
  {
  case DRVCAT_DBG_API_GENERAL:
    res = mtlk_df_debug_bcl_debug_general_init(df_user, cnt);
    break;
  case DRVCAT_DBG_API_GENERAL_PKT:
    res = mtlk_df_debug_bcl_debug_pkt_counters_init(df_user, cnt);
    break;
  case DRVCAT_DBG_API_RR_STATS:
    res = mtlk_df_debug_bcl_debug_rr_counters_init(df_user, cnt);
    break;
  default:
    ELOG_D("Unsupported data category (%u) requested", category);
    break;
  }
  return res;
}

static int
mtlk_df_debug_bcl_debug_rr_counters_category_free(mtlk_df_user_t *df_user)
{
  ILOG2_S("%s: Free rr_counters", mtlk_df_user_get_name(df_user));

  if (NULL != df_user->slow_ctx->dbg_rr_addr) {
    mtlk_osal_mem_free(df_user->slow_ctx->dbg_rr_addr);
    df_user->slow_ctx->dbg_rr_addr = NULL;
  }
  df_user->slow_ctx->dbg_rr_addr_num = 0;

  if (NULL != df_user->slow_ctx->dbg_rr_cnts) {
    mtlk_osal_mem_free(df_user->slow_ctx->dbg_rr_cnts);
    df_user->slow_ctx->dbg_rr_cnts = NULL;
  }
  df_user->slow_ctx->dbg_rr_cnts_num = 0;
  return MTLK_ERR_OK;
}

static int
mtlk_df_debug_bcl_debug_pkt_counters_category_free(mtlk_df_user_t *df_user)
{
  ILOG2_S("%s: Free pkt_counters", mtlk_df_user_get_name(df_user));

  if (NULL != df_user->slow_ctx->dbg_general_pkt_addr) {
    mtlk_osal_mem_free(df_user->slow_ctx->dbg_general_pkt_addr);
    df_user->slow_ctx->dbg_general_pkt_addr = NULL;
  }
  df_user->slow_ctx->dbg_general_pkt_addr_num = 0;

  if (NULL != df_user->slow_ctx->dbg_general_pkt_cnts) {
    mtlk_osal_mem_free(df_user->slow_ctx->dbg_general_pkt_cnts);
    df_user->slow_ctx->dbg_general_pkt_cnts = NULL;
  }
  df_user->slow_ctx->dbg_general_pkt_cnts_num = 0;
  return MTLK_ERR_OK;
}

static int
mtlk_df_debug_bcl_debug_general_category_free(mtlk_df_user_t *df_user)
{
  ILOG2_S("%s: Free core_general_stats", mtlk_df_user_get_name(df_user));

  return MTLK_ERR_OK;
}

int
mtlk_df_debug_bcl_category_free(mtlk_df_user_t *df_user, uint32 category)
{
  int res = MTLK_ERR_PARAMS;

  switch (category)
  {
  case DRVCAT_DBG_API_GENERAL:
    res = mtlk_df_debug_bcl_debug_general_category_free(df_user);
    break;
  case DRVCAT_DBG_API_GENERAL_PKT:
    res = mtlk_df_debug_bcl_debug_pkt_counters_category_free(df_user);
    break;
  case DRVCAT_DBG_API_RR_STATS:
    res = mtlk_df_debug_bcl_debug_rr_counters_category_free(df_user);
    break;
  default:
    ELOG_D("Unsupported data category (%u) requested", category);
    break;
  }
  return res;
}

static int
mtlk_df_debug_bcl_debug_rr_counters_name_get(
    mtlk_df_user_t *df_user, uint32 index, char *pdata, uint32 datalen)
{
  int res = MTLK_ERR_OK;
  int rslt = 0;

  if (index >= (df_user->slow_ctx->dbg_rr_addr_num + 1)) { /* +1 for header */
    ELOG_D("Index out of bounds (index %u)", index);
    res = MTLK_ERR_PARAMS;

  } else if (0 == index) {
    rslt = wave_strcopy(pdata, bcl_rr_counters_hdr, datalen);

  } else if (NULL == df_user->slow_ctx->dbg_rr_addr) {
    res = MTLK_ERR_NOT_READY;

  } else {
    char mac_addr[MAC_ADDR_LENGTH*3];
    rslt = mtlk_snprintf(mac_addr, sizeof(mac_addr), "%Y",
                         df_user->slow_ctx->dbg_rr_addr[index - 1].au8Addr);
    if (rslt >= 0) {
      rslt = wave_strcopy(pdata, mac_addr, datalen);
    }
  }

  if (rslt <= 0) {
    WLOG_DD("Buffer size (%u) too small: string truncated (index %u)", datalen, index);
  }

  return res;
}

static int
mtlk_df_debug_bcl_debug_pkt_counters_name_get(
    mtlk_df_user_t *df_user, uint32 index, char *pdata, uint32 datalen)
{
  int res = MTLK_ERR_OK;
  int rslt = 0;

  if (index >= (df_user->slow_ctx->dbg_general_pkt_addr_num + 2)) { /* +1 for header +1 footer*/
    ELOG_D("Index out of bounds (index %u)", index);
    res = MTLK_ERR_PARAMS;

  } else if (0 == index) {
    rslt = wave_strcopy(pdata, bcl_pkt_counters_hdr, datalen);

  } else if ((df_user->slow_ctx->dbg_general_pkt_addr_num + 1) == index) {
    rslt = wave_strcopy(pdata, bcl_pkt_counters_ftr, datalen);

  } else if (NULL == df_user->slow_ctx->dbg_general_pkt_addr) {
    res = MTLK_ERR_NOT_READY;

  } else {
    char mac_addr[MAC_ADDR_LENGTH*3];
    rslt = mtlk_snprintf(mac_addr, sizeof(mac_addr), "%Y",
                         df_user->slow_ctx->dbg_general_pkt_addr[index - 1].au8Addr);
    if (rslt >= 0) {
      rslt = wave_strcopy(pdata, mac_addr, datalen);
    }
  }

  if (rslt <= 0) {
    WLOG_DD("Buffer size (%u) too small: string truncated (index %u)", datalen, index);
  }

  return res;
}

static int
mtlk_df_debug_bcl_debug_general_name_get(
    mtlk_df_user_t *df_user, uint32 index, char *pdata, uint32 datalen)
{
  int res = MTLK_ERR_OK;
  struct bcl_general_fetch_stat_params_t fsp;

  fsp.index_cur = 0;
  fsp.index_search = index;
  fsp.what = FETCH_NAME;

  res = mtlk_df_debug_bcl_debug_general_iterate(df_user, &mtlk_df_debug_bcl_debug_general_fetch_stat, &fsp);
  if (MTLK_ERR_OK != res) {
    ELOG_V("Error while iterating driver statistics");
  } else {
    if (!wave_strcopy(pdata, fsp.name, datalen)) {
      WLOG_DD("Buffer size (%u) too small: string truncated (index %u)", datalen, index);
    }
  }

  return res;
}

static int
mtlk_df_debug_bcl_name_get(mtlk_df_user_t *df_user, uint32 category,
    uint32 index, char *pdata, uint32 datalen)
{
  int res = MTLK_ERR_PARAMS;

  switch (category)
  {
  case DRVCAT_DBG_API_GENERAL:
    res = mtlk_df_debug_bcl_debug_general_name_get(df_user, index, pdata, datalen);
    break;
  case DRVCAT_DBG_API_GENERAL_PKT:
    res = mtlk_df_debug_bcl_debug_pkt_counters_name_get(df_user, index, pdata, datalen);
    break;
  case DRVCAT_DBG_API_RR_STATS:
    res = mtlk_df_debug_bcl_debug_rr_counters_name_get(df_user, index, pdata, datalen);
    break;
  default:
    ELOG_D("Unsupported data category (%u) requested", category);
    break;
  }
  return res;
}

static int
mtlk_df_debug_bcl_debug_rr_counters_val_get(mtlk_df_user_t *df_user, uint32 index, uint32 *pval)
{
  int res = MTLK_ERR_OK;

  if (NULL == df_user->slow_ctx->dbg_rr_cnts) {
    res = MTLK_ERR_NOT_READY;
  } else if (index >= df_user->slow_ctx->dbg_rr_cnts_num) {
    ELOG_D("Index out of bounds (index %u)", index);
    res = MTLK_ERR_PARAMS;
  } else {
    *pval = df_user->slow_ctx->dbg_rr_cnts[index];
  }

  return res;
}

static int
mtlk_df_debug_bcl_debug_pkt_counters_val_get(mtlk_df_user_t *df_user, uint32 index, uint32 *pval)
{
  int res = MTLK_ERR_OK;

  if (NULL == df_user->slow_ctx->dbg_general_pkt_cnts) {
    res = MTLK_ERR_NOT_READY;
  } else if (index >= df_user->slow_ctx->dbg_general_pkt_cnts_num) {
    ELOG_D("Index out of bounds (index %u)", index);
    res = MTLK_ERR_PARAMS;
  } else {
    *pval = df_user->slow_ctx->dbg_general_pkt_cnts[index];
  }

  return res;
}

static int
mtlk_df_debug_bcl_debug_general_val_get(mtlk_df_user_t *df_user, uint32 index, uint32 *pval)
{
  int res = MTLK_ERR_OK;
  struct bcl_general_fetch_stat_params_t fsp;

  fsp.index_cur = 0;
  fsp.index_search = index;
  fsp.what = FETCH_VAL;

  res = mtlk_df_debug_bcl_debug_general_iterate(df_user, &mtlk_df_debug_bcl_debug_general_fetch_stat, &fsp);
  if (MTLK_ERR_OK != res) {
    ELOG_V("Error while iterating driver statistics");
  } else {
    *pval = fsp.val;
  }

  return res;
}

static int
mtlk_df_debug_bcl_val_get(mtlk_df_user_t *df_user, uint32 category, uint32 index, uint32 *pval)
{
  int res = MTLK_ERR_PARAMS;

  switch (category)
  {
  case DRVCAT_DBG_API_GENERAL:
    res = mtlk_df_debug_bcl_debug_general_val_get(df_user, index, pval);
    break;
  case DRVCAT_DBG_API_GENERAL_PKT:
    res = mtlk_df_debug_bcl_debug_pkt_counters_val_get(df_user, index, pval);
    break;
  case DRVCAT_DBG_API_RR_STATS:
    res = mtlk_df_debug_bcl_debug_rr_counters_val_get(df_user, index, pval);
    break;
  default:
    ELOG_D("Unsupported data category (%u) requested", category);
    break;
  }
  return res;
}

static int
mtlk_df_debug_bcl_reset(mtlk_df_user_t *df_user, uint32 index, uint32 val)
{
  int res = MTLK_ERR_PARAMS;

  switch (index) {
  case IDX_DBG_API_RESET_ALL:
    res = mtlk_df_ui_reset_stats(df_user->df, FALSE);
#if MTLK_USE_DIRECTCONNECT_DP_API
    _mtlk_df_user_dcdp_zero_stats(df_user);
#endif
#if MTLK_USE_PUMA6_UDMA
    _mtlk_df_user_udma_zero_stats(df_user);
#endif
    break;
  default:
    ELOG_D("Index out of bounds (index %u)", index);
  }

  return res;
}

static int
mtlk_df_debug_bcl_val_put(mtlk_df_user_t *df_user, uint32 category, uint32 index, uint32 val)
{
  int res = MTLK_ERR_PARAMS;

  switch (category)
  {
  case DRVCAT_DBG_API_RESET:
    res = mtlk_df_debug_bcl_reset(df_user, index, val);
    break;
  default:
    ELOG_D("Unsupported data category (%u) requested", category);
    break;
  }

  return res;
}

static int __MTLK_IFUNC
mtlk_df_ui_linux_ioctl_bcl_drv_data_exchange (struct net_device *ndev,
            struct iw_request_info *info,
            union iwreq_data *wrqu, char *extra)
{
  mtlk_df_user_t            *df_user = mtlk_df_user_from_ndev(ndev);
  BCL_DRV_DATA_EX_REQUEST   preq;
  char                      *pdata = NULL;
  int                       res = MTLK_ERR_OK;
  uint32                    value;

  ILOG3_SSD("%s: Invoked from %s (%i)", ndev->name, current->comm, current->pid);
  MTLK_CHECK_DF_USER(df_user);

  if (mtlk_df_is_slave(df_user->df)) {
    res = MTLK_ERR_NOT_SUPPORTED;
    goto cleanup;
  }

  if (0 != (copy_from_user(&preq, wrqu->data.pointer, sizeof(preq)))) {
    res = MTLK_ERR_VALUE;
    goto cleanup;
  }

  switch (preq.mode) {
  case BclDrvModeCatInit:
    /* Make sure there's enough space to fit the counter: */
    if (sizeof(uint32) != preq.datalen) {
      res = MTLK_ERR_PARAMS;
    } else {
      /* Return category items counter to BCLSockServer: */
      res = mtlk_df_debug_bcl_category_init(df_user, preq.category, /* out */ &value);
      if (MTLK_ERR_OK == res) {
        if (0 != (copy_to_user(wrqu->data.pointer + sizeof(preq), &value, sizeof(uint32)))) {
          res = MTLK_ERR_VALUE;
        }
      }
    }
    break;
  case BclDrvModeCatFree:
    res = mtlk_df_debug_bcl_category_free(df_user, preq.category);
    break;
  case BclDrvModeNameGet:
    pdata = mtlk_osal_mem_alloc(preq.datalen * sizeof(char), MTLK_MEM_TAG_IOCTL);
    if (NULL == pdata) {
      res = MTLK_ERR_NO_MEM;
    } else {
      res = mtlk_df_debug_bcl_name_get(df_user, preq.category, preq.index, pdata, preq.datalen);
      if (MTLK_ERR_OK == res) {
        /* pdata is NUL-terminated as its filled via wave_strcopy */
        if (0 != (copy_to_user(wrqu->data.pointer + sizeof(preq),
                  pdata, (wave_strlen(pdata, preq.datalen) + 1) * sizeof(char) ))) {
          res = MTLK_ERR_VALUE;
        }
      }
      mtlk_osal_mem_free(pdata);
    }
    break;
  case BclDrvModeValGet:
    /* Make sure there's enough space to store the value: */
    if (sizeof(uint32) != preq.datalen) {
      res = MTLK_ERR_PARAMS;
    } else {
      /* Return the value to BCLSockServer: */
      res = mtlk_df_debug_bcl_val_get(df_user, preq.category, preq.index, /* out */ &value);
      if (MTLK_ERR_OK == res) {
        if (0 != (copy_to_user(wrqu->data.pointer + sizeof(preq), &value, sizeof(uint32)))) {
          res = MTLK_ERR_VALUE;
        }
      }
    }
    break;
  case BclDrvModeValSet:
    /* Make sure the value is present: */
    if (sizeof(uint32) != preq.datalen) {
      res = MTLK_ERR_PARAMS;
    } else {
    /* Process the value: */
      if (0 != (copy_from_user(&value, wrqu->data.pointer + sizeof(preq), sizeof(uint32)))) {
        res = MTLK_ERR_VALUE;
      } else {
        res = mtlk_df_debug_bcl_val_put(df_user, preq.category, preq.index, value);
      }
    }
    break;
  default:
    ELOG_D("Unknown data exchange mode (%u)", preq.mode);
    res = MTLK_ERR_PARAMS;
  }

cleanup:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _df_ui_bcl_mac_data_get (mtlk_hw_t *hw, UMI_BCL_REQUEST *preq)
{
  int res;
  BOOL f_bswap_data = TRUE;

  /* if Unit field value >= BCL_UNIT_MAX then we should not convert result data to host format */
  if (preq->Unit >= BCL_UNIT_MAX) {
    preq->Unit -= BCL_UNIT_MAX; /* restore original value */
    f_bswap_data = FALSE;
  }

  ILOG4_DDDD("Getting BCL over IO: unit(%d) address(0x%x) size(%u) (%x)",
             (int)preq->Unit, (unsigned int)preq->Address,
             (unsigned int)preq->Size, (unsigned int)preq->Data[0]);

  /* Convert message header to MAC format */
  mtlk_core_bswap_bcl_request(preq, TRUE);

  /* Send BCL request through IO */
  res = wave_hw_mmb_get_prop(hw, MTLK_HW_BCL_ON_EXCEPTION, preq, sizeof(*preq));
  if (MTLK_ERR_OK != res) {
    ELOG_D("CID-%02x: Can't get BCL", mtlk_hw_mmb_get_card_idx(hw));
    return res;
  }

  /* Convert message to host format */
  mtlk_core_bswap_bcl_request(preq, !f_bswap_data);
  /* Dump message content */
  mtlk_dump(3, preq, sizeof(*preq), "dump of outgoing UMI_BCL_REQUEST");

  return res;
}

static int __MTLK_IFUNC
mtlk_df_ui_iw_bcl_mac_data_get (struct net_device *ndev,
            struct iw_request_info *info,
            union iwreq_data *wrqu, char *extra)
{
  int res = 0;
  UMI_BCL_REQUEST req;
  UMI_BCL_REQUEST *req_result;
  mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(ndev);
  mtlk_clpb_t *clpb = NULL;
  mtlk_hw_t *hw;

  ILOG3_SSD("%s: Invoked from %s (%i)", ndev->name, current->comm, current->pid);
  MTLK_CHECK_DF_USER(df_user);

  if (0 != (copy_from_user(&req, wrqu->data.pointer, sizeof(req)))) {
    res = MTLK_ERR_PARAMS;
    goto finish;
  }

  /* if error is occurred in _pci_probe then serializer is absent */
  hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df_user->df));
  if (!wave_rcvry_pci_probe_error_get(wave_hw_mmb_get_mmb_base(hw))) {
    /* Send request to core */
    res = _mtlk_df_user_invoke_core(df_user->df, WAVE_RADIO_REQ_GET_BCL_MAC_DATA,
                                    &clpb, &req, sizeof(req));
    res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_GET_BCL_MAC_DATA, FALSE);
  }
  else {
    res = _df_ui_bcl_mac_data_get(hw, &req);
    if (MTLK_ERR_OK == res) {
      if (0 != (copy_to_user(wrqu->data.pointer, &req, sizeof(req))))
        res = MTLK_ERR_UNKNOWN;
    }
    goto finish;
  }

  if (MTLK_ERR_OK != res) {
    goto finish;
  }

  /* Handle result */
  req_result = mtlk_clpb_enum_get_next(clpb, NULL);

  if (NULL == req_result) {
    res = MTLK_ERR_UNKNOWN;
  } else if (0 != (copy_to_user(wrqu->data.pointer, req_result, sizeof(*req_result)))) {
    res = MTLK_ERR_UNKNOWN;
  }

  mtlk_clpb_delete(clpb);
finish:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _df_ui_bcl_mac_data_set (mtlk_hw_t *hw, UMI_BCL_REQUEST *preq)
{
  int res;

  ILOG2_DDDD("Setting BCL over IO: unit(%d) address(0x%x) size(%u) (%x)",
             (int)preq->Unit, (unsigned int)preq->Address,
             (unsigned int)preq->Size, (unsigned int)preq->Data[0]);

  /* Dump message content */
  mtlk_dump(3, preq, sizeof(*preq), "dump of incoming UMI_BCL_REQUEST");

  /* Send BCL request through IO */
  mtlk_core_bswap_bcl_request(preq, FALSE);

  res = wave_hw_mmb_set_prop(hw, MTLK_HW_BCL_ON_EXCEPTION, preq, sizeof(*preq));
  if (MTLK_ERR_OK != res)
    ELOG_D("CID-%02x: Can't set BCL", mtlk_hw_mmb_get_card_idx(hw));

  return res;
}

static int __MTLK_IFUNC
mtlk_df_ui_iw_bcl_mac_data_set (struct net_device *ndev,
            struct iw_request_info *info,
            union iwreq_data *wrqu, char *extra)
{
  int res = 0;
  UMI_BCL_REQUEST req;
  mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(ndev);
  mtlk_clpb_t *clpb = NULL;
  mtlk_hw_t *hw;

  ILOG3_SSD("%s: Invoked from %s (%i)", ndev->name, current->comm, current->pid);
  MTLK_CHECK_DF_USER(df_user);

  if (0 != (copy_from_user(&req, wrqu->data.pointer, sizeof(req)))) {
    res = MTLK_ERR_PARAMS;
    goto finish;
  }

  /* if error is occurred in _pci_probe then serializer is absent */
  hw = mtlk_vap_get_hw(mtlk_df_get_vap_handle(df_user->df));
  if (!wave_rcvry_pci_probe_error_get(wave_hw_mmb_get_mmb_base(hw))) {
    /* Send request to core */
    res = _mtlk_df_user_invoke_core(df_user->df, WAVE_RADIO_REQ_SET_BCL_MAC_DATA,
                                    &clpb, &req, sizeof(req));
    res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_SET_BCL_MAC_DATA, TRUE);
  }
  else
    res = _df_ui_bcl_mac_data_set(hw, &req);

finish:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

#endif

void __MTLK_IFUNC
mtlk_df_user_dcdp_remove_mac_addr(mtlk_df_t *df, const uint8 *mac_addr)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
    MTLK_ASSERT(NULL != df);
    MTLK_ASSERT(NULL != mac_addr);

    if (MTLK_ERR_OK != mtlk_df_ui_dp_disconnect_mac(df, mac_addr)) {
      ILOG1_DY("CID-%04x: [DC DP] - station %Y removal error",
              mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)), mac_addr);
    }
#endif
}

void __MTLK_IFUNC
mtlk_df_user_set_hs20_status(mtlk_df_t *df, BOOL hs20_enabled)
{
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  df_user->hs20_enabled = hs20_enabled;
  if (hs20_enabled){
    _mtlk_df_user_hs20_notify(df_user);
    df_user->hs20_notify_entry.data = (void*) df_user;
    mtlk_hs20_notify_register(_mtlk_df_user_hs20_notify, &df_user->hs20_notify_entry);
  }
  else {
    mtlk_hs20_notify_unregister(&df_user->hs20_notify_entry);
    _mtlk_df_update_litepath(df_user);
    _mtlk_df_update_netdev_cb(df_user);
  }
}

BOOL __MTLK_IFUNC
mtlk_df_user_get_hs20_status(mtlk_df_t *df)
{
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  return df_user->hs20_enabled;
}

/*--- HW RT-Logger ---*/
#ifdef CONFIG_WAVE_RTLOG_REMOTE
void rtlog_clb_func(mtlk_handle_t clb_ctx, rtlog_cfg_id_t req_id, void *data, uint32 size)
{
    int res;
    mtlk_df_t   *df = (mtlk_df_t *)(clb_ctx);
    mtlk_clpb_t *clpb = NULL;

    ILOG2_PDPD("ctx %p, req_id %d, data %p, size %d", clb_ctx, req_id, data, size);
    mtlk_dump(3, data, MIN(16, size), "clb_data");

    res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_SET_RTLOG_CFG, &clpb, data, size);
    res = _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_SET_RTLOG_CFG, TRUE);

    /* TBD */
    ILOG2_D("res %d", res);
//finish:
  //return _mtlk_df_mtlk_to_linux_error_code(res);
}

int __MTLK_IFUNC
mtlk_df_user_rtlog_register(uint32 wlan_if, mtlk_vap_handle_t vap_handle)
{
    mtlk_df_t *df = mtlk_vap_get_df(vap_handle);

    MTLK_ASSERT(NULL != df);

    rtlog_fw_clb_register(wlan_if, rtlog_clb_func, (mtlk_handle_t)df);

    return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_df_user_rtlog_unregister(uint32 wlan_if)
{
    rtlog_fw_clb_unregister(wlan_if);
}
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

#if MTLK_USE_DIRECTCONNECT_DP_API
/*! \brief  Multicast module callback handler
 *  \param[in] grp_id  Multicast group id.
 *  \param[in] dev  Registered net device.
 *  \param[in] mc_stream  Multicast stream information.
 *  \param[in] flags  :
 *      DC_DP_MC_F_ADD - Add a new mcast group membership to a DirectConnect interface.
 *      DC_DP_MC_F_UPD - Update an existing mcast group membership to a DirectConnect interface.
 *      DC_DP_MC_F_DEL - Delete an existing mcast group membership from a DirectConnect interface.
 *  \return none
 *  \note Group Identifier is allocated and managed by Multicast Subsystem.
 */
static void mtlk_df_ui_group_handler (uint32_t grp_id, struct net_device *ndev,
                                      void *mc_stream, uint32_t flags)
{
  mtlk_df_t                 *df;
  mtlk_df_user_t            *df_user       = mtlk_df_user_from_ndev(ndev);
  struct dc_dp_mcast_stream *mcast_stream  = mc_stream;
  struct dc_dp_ip_addr      *src_ip, *dst_ip;
  mtlk_core_ui_mc_update_sta_db_t req;
  size_t                     mac_list_size;

  if (__UNLIKELY(!df_user || !mcast_stream))
    return;
  df = mtlk_df_user_get_df(df_user);

  memset(&req, 0, sizeof(req));

  ILOG1_SDSD("MC_cb dev:%s flags:%d (%s) grp_id:%d", ndev->name,
    flags, (flags == DC_DP_MC_F_ADD) ? "add to group" : ((flags == DC_DP_MC_F_DEL) ? "del from group" : ((flags == DC_DP_MC_F_UPD) ? "update group": "unknown")),
    grp_id);
  ILOG1_DD("src_port: %d dst_port: %d", mcast_stream->src_port, mcast_stream->dst_port);

  src_ip = &mcast_stream->src_ip;
  dst_ip = &mcast_stream->dst_ip;

  switch (mcast_stream->dst_ip.ip_type) {
  case IPV4:
    ILOG1_SD("dev:%s src IPv4:%B", mcast_stream->mem_dev->name, src_ip->u.ip4_addr.s_addr);
    ILOG1_SD("dev:%s dst IPv4:%B", mcast_stream->mem_dev->name, dst_ip->u.ip4_addr.s_addr);
    req.mc_addr.type = MTLK_IPv4;
    req.mc_addr.src_ip.ip4_addr.s_addr = ntohl(src_ip->u.ip4_addr.s_addr);
    req.mc_addr.grp_ip.ip4_addr.s_addr = ntohl(dst_ip->u.ip4_addr.s_addr);
    break;
  case IPV6:
    ILOG1_SK("dev:%s src IPv6:%K", mcast_stream->mem_dev->name, src_ip->u.ip6_addr.s6_addr);
    ILOG1_SK("dev:%s dst IPv6:%K", mcast_stream->mem_dev->name, dst_ip->u.ip6_addr.s6_addr);
    req.mc_addr.type = MTLK_IPv6;
    req.mc_addr.src_ip.ip6_addr = src_ip->u.ip6_addr;
    req.mc_addr.grp_ip.ip6_addr = dst_ip->u.ip6_addr;
    break;
  default:
    ILOG1_V("Wrong ip type");
    return;
  }

  switch (flags) {
    case DC_DP_MC_F_ADD:
      req.action = MTLK_MC_STADB_ADD; break;
    case DC_DP_MC_F_DEL:
      req.action = MTLK_MC_STADB_DEL; break;
    case DC_DP_MC_F_UPD:
      req.action = MTLK_MC_STADB_UPD; break;
    default:
      /* no action */
      ILOG1_V("Wrong action");
      return;
  }

  mac_list_size = sizeof(IEEE_ADDR) * mcast_stream->num_joined_macs;
  req.grp_id = grp_id;
  req.macs_num = mcast_stream->num_joined_macs;

  if (mtlk_df_user_is_secondary(df_user))
    req.vap_id = mtlk_secondary_df_user_get_vap_idx(df_user);
  else
    req.vap_id = mtlk_vap_get_id(mtlk_df_get_vap_handle(df));

  MTLK_ASSERT(mac_list_size <= sizeof(mcast_stream->macaddr));
  if (__UNLIKELY(mac_list_size > sizeof(mcast_stream->macaddr))) {
    ELOG_DD("CID-%04x: Got wrong MAC address list size from DCDP (num_joined_macs=%u)",
                                 mac_list_size, mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)));
    return;
  }

  req.macs_list = mtlk_osal_mem_alloc(mac_list_size, MTLK_MEM_TAG_MCAST);
  ILOG1_D("Invoke MCAST helper request (macs_num=%u)", req.macs_num);
  MTLK_ASSERT(req.macs_list != NULL);
  if (__UNLIKELY(!req.macs_list)) {
    ELOG_D("CID-%04x: Can't allocate list of MACs", mtlk_vap_get_oid(mtlk_df_get_vap_handle(df)));
    return;
  }
  wave_memcpy(req.macs_list, mac_list_size, mcast_stream->macaddr, mac_list_size);

  _mtlk_df_user_invoke_core_async(df, MTLK_CORE_REQ_MCAST_HELPER_GROUP_ID_ACTION, &req, sizeof(req), NULL, 0);
}
#endif

void __MTLK_IFUNC
mtlk_df_user_group_init (mtlk_df_user_t* df_user)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  struct net_device *ndev = mtlk_df_user_get_ndev(df_user);
  MTLK_ASSERT(ndev != NULL);

  df_user->dcdp.mcast_registered = FALSE;

  if (!mtlk_vap_is_ap(vap_handle)) {
    ILOG1_S("%s: station mode doesn't need to register mcast module", df_user->name);
    return;
  }

  if (mtlk_mmb_fastpath_available(mtlk_vap_get_hw(vap_handle))) {
    uint32 flags = DC_DP_F_MC_REGISTER | DC_DP_F_MC_UPDATE_MAC_ADDR;

  #ifdef DC_DP_F_MC_FW_RESET
    /* If this flag defined, the multicast recovery feature is supported by DC DP */
    wave_rcvry_type_e rcvry_type = wave_rcvry_type_current_get(mtlk_vap_get_hw(vap_handle));
    if ((RCVRY_TYPE_FAST == rcvry_type) || (RCVRY_TYPE_FULL == rcvry_type)) {
      flags |= DC_DP_F_MC_FW_RESET;
      WLOG_S("%s: multicast recovery final phase", df_user->name);
    }
  #endif

    if (DC_DP_FAILURE == dc_dp_register_mcast_module(ndev, THIS_MODULE, mtlk_df_ui_group_handler, flags)) {
      WLOG_S("%s: can't register multicast callback. Internal manager will be used", df_user->name);
      return;
    }

    df_user->dcdp.mcast_registered = TRUE;
    ILOG0_S("%s: multicast callback registered", df_user->name);
    return;
  }
  else
#endif
  {
    ILOG0_S("%s: software path is used, dont't register multicast callback", df_user->name);
    return;
  }
}

void  __MTLK_IFUNC
mtlk_df_user_group_uninit (mtlk_df_user_t* df_user)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  struct net_device *ndev = mtlk_df_user_get_ndev(df_user);
  MTLK_ASSERT(ndev != NULL);

  if (df_user->dcdp.mcast_registered) {
    uint32 flags = DC_DP_F_MC_DEREGISTER;

    /* If this flag defined, the multicast recovery feature is supported by DC DP */
    mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
    wave_rcvry_type_e rcvry_type = wave_rcvry_type_current_get(mtlk_vap_get_hw(vap_handle));
    if ((RCVRY_TYPE_FAST == rcvry_type) || (RCVRY_TYPE_FULL == rcvry_type)) {
  #ifdef DC_DP_F_MC_FW_RESET
      flags |= DC_DP_F_MC_FW_RESET;
      WLOG_S("%s: multicast recovery initial phase", df_user->name);
  #else
      WLOG_S("%s: no multicast recovery support", df_user->name);
  #endif
    }

    df_user->dcdp.mcast_registered = FALSE;
    if (DC_DP_FAILURE == dc_dp_register_mcast_module(ndev, THIS_MODULE, mtlk_df_ui_group_handler, flags)) {
      ELOG_S("%s: multicast callback unregistration error", df_user->name);
    } else {
      ILOG0_S("%s: multicast callback unregistered", df_user->name);
    }
  }
  else
#endif
  {
    ILOG0_S("%s: no registered multicast callback", df_user->name);
  }
}

int __MTLK_IFUNC
mtlk_df_mcast_notify_sta_connected (mtlk_df_t *df)
{
#if MTLK_USE_DIRECTCONNECT_DP_API && defined(DC_DP_F_MC_NEW_STA)
  /* Notify mcast helper when new STA was connected. This event will force host to send IGMP query to all STAs */
  struct net_device *ndev = mtlk_df_user_get_ndev(mtlk_df_get_user(df));
  if (DC_DP_FAILURE == dc_dp_register_mcast_module(ndev, THIS_MODULE, NULL, DC_DP_F_MC_NEW_STA)) {
    WLOG_S("%s: can't notify mcast helper about new STA", ndev->name);
    return MTLK_ERR_UNKNOWN;
  }
  ILOG1_S("%s: mcast helper notified about new STA", ndev->name);
#endif
  return MTLK_ERR_OK;
}


/******************************************************************************
 * DirectConnect datapath support
 ******************************************************************************/
#if MTLK_USE_DIRECTCONNECT_DP_API

/**** DirectConnect DP defines ****/
#define TX_DATA_DP_PORT             MTLK_BFIELD_INFO(8, 4)  /*  4 bits starting bit8 */

BOOL __MTLK_IFUNC
mtlk_df_ui_mcast_is_registered (mtlk_df_t* df)
{
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  MTLK_ASSERT(df_user);
  return df_user->dcdp.mcast_registered;
}

int __MTLK_IFUNC
mtlk_df_ui_dp_disconnect_mac (mtlk_df_t *df, const uint8 *mac_addr)
{
  struct net_device *ndev;
  int res;

  ndev = mtlk_df_user_get_ndev(mtlk_df_get_user(df));
  res = dc_dp_disconn_if(ndev, NULL, (uint8 *)mac_addr, 0);
  if (__LIKELY(res == DC_DP_SUCCESS))
    return MTLK_ERR_OK;
  else
    return MTLK_ERR_UNKNOWN;
}

#ifdef WAVE_DCDP_DGAF_SUPPORTED
/* Update DC DP DGAF flag */
mtlk_error_t __MTLK_IFUNC
mtlk_df_ui_dp_dgaf_disable(mtlk_df_t *df, BOOL disabled)
{
  struct dc_dp_prop prop;
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  struct net_device *ndev = mtlk_df_user_get_ndev(df_user);

  memset(&prop, 0, sizeof(prop));
  prop.prop_id = DC_DP_PROP_ID_DGAF;
  prop.dgaf_disabled = !!disabled;
  ILOG2_SD("%s: DC DP dgaf_disabled: %u", ndev->name, (unsigned)disabled);
  if (DC_DP_FAILURE == dc_dp_set_property(ndev, &df_user->dcdp.subif_id, &prop)) {
    ELOG_S("%s: cannot change DC DP DGAF flag!", ndev->name);
    return MTLK_ERR_UNKNOWN;
  }
  return MTLK_ERR_OK;
}
#endif

/*
 * Note for below and fragmentation workaround:
 * As driver doesn't know the port id and ndev, it is agreed with DCDP
 * the driver uses ndev[0] and port[0] for the DCDP API.
 */
int __MTLK_IFUNC
mtlk_df_dcdp_handle_rx_ring_sw (mtlk_dcdp_dev_t *dp_dev)
{
  int res;
  MTLK_ASSERT(dp_dev);
  res = dc_dp_handle_ring_sw(THIS_MODULE, dp_dev->dp_port_id[0], dp_dev->dp_ndev[0], &dp_dev->dp_resources.rings.dev2soc, DC_DP_F_RX);
  if (__UNLIKELY(res == DC_DP_FAILURE))
    return MTLK_ERR_UNKNOWN;
  else
    return MTLK_ERR_OK; /* TODO: function will return HD's num */
}

int __MTLK_IFUNC
mtlk_df_dcdp_handle_txout_ring_sw (mtlk_dcdp_dev_t *dp_dev)
{
  int res;
  MTLK_ASSERT(dp_dev);
  res = dc_dp_handle_ring_sw(THIS_MODULE, dp_dev->dp_port_id[0], dp_dev->dp_ndev[0], &dp_dev->dp_resources.rings.soc2dev_ret, DC_DP_F_TX_COMPLETE);
  if (__UNLIKELY(res == DC_DP_FAILURE))
    return MTLK_ERR_UNKNOWN;
  else
    return MTLK_ERR_OK; /* TODO: function will return HD's num */
}

int __MTLK_IFUNC
mtlk_df_dcdp_handle_frag_wa_ring (mtlk_dcdp_dev_t *dp_dev)
{
  int res;
  MTLK_ASSERT(dp_dev);
  ILOG4_V("mtlk_df_dcdp_handle_frag_wa_ring() called");
  res = dc_dp_handle_ring_sw(THIS_MODULE, dp_dev->dp_port_id[0], dp_dev->dp_ndev[0], &dp_dev->dp_resources.rings.dev2soc_except, DC_DP_F_RX_FRAG_EXCEPT);
  if (__UNLIKELY(res == DC_DP_FAILURE))
    return MTLK_ERR_UNKNOWN;
  else
    return MTLK_ERR_OK; /* TODO: function will return HD's num */
}

static int __INLINE
_mtlk_df_ui_dp_xmit (struct net_device *ndev, mtlk_core_handle_tx_data_t *data, struct dp_subif *tx_subif)
{
  int res;
  int32 wmm_class;

  wmm_class = dc_dp_mark_pkt_devqos(tx_subif->port_id, ndev, data->nbuf);
  if (__UNLIKELY(wmm_class < 0)) {
    ELOG_V("Can't provide QoS AC/TID mapping for a given packet");
    return MTLK_ERR_UNKNOWN;
  }

  /* packet will handled and released by dc_dp_xmit() */
  mtlk_nbuf_stop_tracking(data->nbuf);

  /* if nbuf came from Linux, rx_ndev and rx_subif must be NULL */
  res = dc_dp_xmit(data->rx_if, data->rx_subif, tx_subif, data->nbuf, data->nbuf->len, 0);
  if (__LIKELY(res == DC_DP_SUCCESS)) {
    return MTLK_ERR_OK;
  } else {
    ILOG1_D("dc_dp_xmit error code (%d)", res);
    /* dc_dp_xmit() frees sk_buff regardless of returned error code.
     * So, in order to avoid double releasing of sk_buff function returns MTLK_ERR_DC_DP_XMIT;
     * this error code will be analyzed later in upper-layer functions */
    return MTLK_ERR_DC_DP_XMIT;
  }
}

int __MTLK_IFUNC
mtlk_df_ui_dp_prepare_and_xmit (mtlk_vap_handle_t vap_handle, mtlk_core_handle_tx_data_t *data, BOOL wds, uint16 sta_sid, int mc_index, uint32 nbuf_flags)
{
    int32 port_id = wave_vap_dcdp_port_get(vap_handle);
    uint8 vap_id  = data->vap_id;
    mtlk_nbuf_t *nbuf = data->nbuf;
    struct dp_subif tx_subif;
    struct dc_dp_subif dc_dp_subif;

    if (wds) nbuf_flags &= ~MTLK_NBUFF_BROADCAST;

    if (nbuf_flags & MTLK_NBUFF_SHORTCUT) {
      /* Allow Acceleration subsystem to learn about session when driver shortcuts
         forwarding without going to stack.
         Shortcut flag can be set only if packet forwarded from RX to TX, so data->rx_subif will be set */
      MTLK_ASSERT(data->rx_subif);
      if (__UNLIKELY(DC_DP_SUCCESS != dc_dp_add_session_shortcut_forward(data->rx_subif, nbuf, DC_DP_F_PREFORWARDING))) {
        ELOG_V("Cannot create shortcut session (preforwarding)");
      }
    }

    /* FIXME: DEBUG */
    ILOG2_DDDD("mc_index:%d, sta_sid:%d, wds:%d, nbuf_flags:%08X", mc_index, sta_sid, wds, nbuf_flags);

    {
      struct ethhdr *ether_header = (struct ethhdr *)nbuf->data;
      uint8 frame_type = (ntohs(ether_header->h_proto) >= ETH_P_802_3_MIN) ? FRAME_TYPE_ETHERNET : FRAME_TYPE_IPX_LLC_SNAP;

      memset(&tx_subif, 0, sizeof(tx_subif));
      memset(&dc_dp_subif, 0, sizeof(dc_dp_subif));
      dc_dp_subif.frame_type = frame_type;
      dc_dp_subif.vap_id = vap_id;
      dc_dp_subif.port_id = port_id;
      if (nbuf_flags & (MTLK_NBUFF_MULTICAST | MTLK_NBUFF_BROADCAST)) {
        dc_dp_subif.mc_index = mc_index;
        dc_dp_subif.mcf = 1;
        /* FIXME: reset sta_sid  because it is not fit to 8 bit */
        sta_sid = 0; /*This may not be required as sta_id in dp_subif is enhanced to uint16*/
      } else {
        dc_dp_subif.sta_id = sta_sid;
      }

      ILOG2_DDDDDD("port_id %d, FrameType %d : vap_id %d : mc_index %d : mcf %d : sta_id %d", port_id, dc_dp_subif.frame_type, dc_dp_subif.vap_id, dc_dp_subif.mc_index, dc_dp_subif.mcf, dc_dp_subif.sta_id);
    }

    /* API to SET subif params in DCDP */
    dc_dp_set_subif_param(&dc_dp_subif);
    ILOG3_D("DCDP subif : 0x%08x",dc_dp_subif.subif);
    tx_subif.subif = dc_dp_subif.subif;
    tx_subif.port_id = dc_dp_subif.port_id;

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    dc_dp_set_ep_pkt(nbuf, port_id, 0);
    dc_dp_set_subifid_pkt(port_id, nbuf, tx_subif.subif, 0);
    /* Update DW3.c flag: this flags means that driver already fills DW0..DW3 with actual values */
    dc_dp_set_mark_pkt(port_id, nbuf, 0);
#else
    dc_dp_set_ep_pkt(&nbuf->DW1, port_id, 0);
    dc_dp_set_subifid_pkt(&nbuf->DW0, tx_subif.subif, 0);
    ILOG3_DD("DW0=%08X, DW1=%08X", nbuf->DW0, nbuf->DW1);
    /* Update DW3.c flag: this flags means that driver already fills DW0..DW3 with actual values */
    dc_dp_set_mark_pkt(nbuf, 0);
#endif

#if MTLK_DCDP_SWPATH_SUPPORTED
    if ((MTLK_DC_DP_MODE_SWPATH == mtlk_hw_mmb_get_dp_mode(mtlk_vap_get_hw(vap_handle))) && (NULL != data->rx_subif))
      tx_subif.port_id = ((struct dp_subif*)data->rx_subif)->port_id;
    else
#endif
      tx_subif.port_id = port_id;

    if (nbuf_flags & MTLK_NBUFF_SHORTCUT) {
      if (__UNLIKELY(DC_DP_SUCCESS != dc_dp_add_session_shortcut_forward(&tx_subif, nbuf, DC_DP_F_POSTFORWARDING))) {
        ELOG_V("Cannot create shortcut session (postforwarding)");
      }
    }

    return _mtlk_df_ui_dp_xmit(mtlk_df_user_get_ndev(mtlk_df_get_user(mtlk_vap_get_secondary_df(vap_handle, vap_id))),
                               data, &tx_subif);
}
/**************************************************************************************************************************/
/*! \brief   Get Meta-SubInterface Integer Identifier (e.g. Station Id) callback. Mostly applicable to DC WLAN peripheral only.
 *  \param[in] netif  Network interface through whireturn packet to dst MAC address mac_addr will be transmitted
 *  \param[in] skb  Pointer to pointer to packet buffer, like sk_buff
 *  \param[in] subif_data  VCC information for DSL nas interface, must provide valid subif_data, otherwise set to NULL
 *  \param[in] mac_addr  MAC Address of Station
 *  \param[in,out] subif  subif->port_id as input and sub-interface including Meta Sub Interface Id (e.g. STA Id) returned
 *  \param[in] flags  Reserved for future use
 *  \return 0 if OK, -1 on ERROR
 *  \note : Optional function - Not needed in case Client driver does not have peripheral specific mapping of Meta Sub Interface in the sub-interfce field.
 */
/* FIXME should be serializible */

static int32_t
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
_mtlk_df_dcdp_get_subif_cb (struct net_device *netif, struct sk_buff *skb, void *subif_data, char *mac_addr, struct dp_subif *subif_id, uint32 flags)
#else
_mtlk_df_dcdp_get_subif_cb (int32_t port_id, struct net_device *netif, char *mac_addr, uint32_t *subif)
#endif
{
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
  uint32_t subif = 0;
  int32_t port_id = subif_id->port_id;

  /* not used in wlan driver */
  MTLK_UNUSED_VAR(skb);
  MTLK_UNUSED_VAR(subif_data);
  MTLK_UNUSED_VAR(flags);
#endif

  if (__UNLIKELY(NULL == netif)) {
    return DC_DP_FAILURE;
  } else {
    mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(netif);
    mtlk_vap_handle_t vap_handle;
    uint8 vap_id;

    if (__UNLIKELY(NULL == df_user))
      return DC_DP_FAILURE;

    vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));

    if (df_user->is_secondary_df)
      vap_id = mtlk_secondary_df_user_get_vap_idx(df_user);
    else
      vap_id = mtlk_vap_get_id(vap_handle);

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    if (subif_id && mtlk_core_dp_get_subif_cb(vap_handle, mac_addr, &subif, vap_id, port_id) == MTLK_ERR_OK) {
      subif_id->subif = subif;
      return DC_DP_SUCCESS;
    }
#else
    if (subif && mtlk_core_dp_get_subif_cb(vap_handle, mac_addr, subif, vap_id, port_id) == MTLK_ERR_OK) {
      return DC_DP_SUCCESS;
    }
#endif
    else {
      return DC_DP_FAILURE;
    }
  }
}
/*************************************************************************/
/*! \brief Stop Tx function callback for Tx flow control - Optional (NULL)
 * \param[in] dev  Pointer to Linux netdevice structure
 * \return Always return DC_DP_SUCCESS.
 * \note Not implemented
 */

static int32_t
_mtlk_df_dcdp_stop_tx_cb (struct net_device *dev)
{
  /* TODO: not yet implemented */
  return DC_DP_SUCCESS;
}

/*************************************************************************/
/*! \brief Restart Tx function callback for Tx flow control - Optional (NULL)
 * \param[in] dev  Pointer to Linux netdevice structure
 * \return Always return DC_DP_SUCCESS.
 * \note Not implemented
 */
static int32_t
_mtlk_df_dcdp_restart_tx_cb (struct net_device *dev)
{
  /* TODO: not yet */
  return DC_DP_SUCCESS;
}

#define TX_DEV_SPEC_INFO_CLASS    MTLK_BFIELD_INFO(24,4)  /* DW3, 4 bits starting bit 24 */
#define TX_DEV_SPEC_INFO_ETYPE    MTLK_BFIELD_INFO(14,2)  /* DW0, 2 bits starting bit 14 */
#define TX_DEV_SPEC_INFO_MCF      MTLK_BFIELD_INFO(13,1)  /* DW0, 1 bits starting bit 13 */
#define TX_DEV_SPEC_INFO_VAPID    MTLK_BFIELD_INFO(8,5)   /* DW0, 5 bits starting bit  8 */
#define TX_DEV_SPEC_INFO_MCIDX    MTLK_BFIELD_INFO(0,6)   /* DW0, 6 bits starting bit  0 */
#define TX_DEV_SPEC_INFO_STAID    MTLK_BFIELD_INFO(0,8)   /* DW0, 8 bits starting bit  0 */

/*************************************************************************/
/*! \brief  Get DC Peripheral specific Desc Information callback, which is
  invoked by DC library to the Peripheral driver. Peripheral driver indicates
  the field value and mask it wants to set in the desc/HD DWORDs
  \param[in] dev Pointer to Linux netdevice structure
  \param[in] port_id Port Number for which information is sought
  \param[in] skb Pointer to skbuff for which Peripheral specific info is
  requested/
  \param[out] desc_fields  Pointer to dc_dp_fields_value_dw structure
  \param[in] flags  : Reserved
  \return 0 if OK / -1 if error
  \remark The Acceleration layer can treat this info as 'black box' if required
 */
static int32_t
_mtlk_df_dcdp_get_dev_specific_desc_info_cb (
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,1,1)
    struct net_device *netif,
#endif
    int32_t port_id,
    struct sk_buff *skb,
    struct dc_dp_fields_value_dw *desc_fields,
    uint32_t flags)
{
  struct dc_dp_fields_dw *dp_fields;
  struct dc_dp_subif dc_dp_subif;
  uint32 subif_id, value;
  int32 wmm_class;
  struct net_device *dev;

  MTLK_ASSERT(desc_fields);
  MTLK_ASSERT(desc_fields->num_dw >= 2);
  MTLK_ASSERT(skb);
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,1,1)
    dev = netif;
#else
    dev = skb->dev;
#endif

  MTLK_ASSERT(dev);

  /* The dev passed as input param to this function must point to wlan netdevices.
   * The skb->dev may point to device which is not belong to wlan for example to VLAN detdev.
   */

  if (!skb || !desc_fields || (desc_fields->num_dw < 2))
    return DC_DP_FAILURE;

  ILOG3_V("Return device specific info to DC layer");

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
  if (0 == dc_dp_get_mark_pkt(port_id, skb, 0)) {
#else
  if (0 == dc_dp_get_mark_pkt(skb, 0)) {
#endif
    /* Update info */
    mtlk_core_dev_spec_info_t dev_spec_info;

    ILOG3_V("Adjust priority and update device specific info");
    if (MTLK_ERR_OK != mtlk_core_get_dev_spec_info(dev, skb, &dev_spec_info)) {
      return DC_DP_FAILURE;
    }

    if (dev_spec_info.mcf && (MCAST_GROUP_UNDEF == dev_spec_info.mc_index)) {
      /* Return error, if mc_index is undefined. In this case accelerated session will not be created */
      return DC_DP_FAILURE;
    }

    /* subif_id */
    memset(&dc_dp_subif, 0, sizeof(dc_dp_subif));
    if (dev_spec_info.mcf) {
      dc_dp_subif.mcf = 1;
      dc_dp_subif.vap_id = dev_spec_info.vap_id;
      dc_dp_subif.mc_index = dev_spec_info.mc_index;
      dc_dp_subif.frame_type = dev_spec_info.frame_type;
    } else {
      dc_dp_subif.mcf = 0;
      dc_dp_subif.vap_id = dev_spec_info.vap_id;
      dc_dp_subif.sta_id = dev_spec_info.sta_id;
      dc_dp_subif.frame_type = dev_spec_info.frame_type;
    }
    dc_dp_subif.port_id = port_id;

     /* API to SET subif params in DCDP */
     dc_dp_set_subif_param(&dc_dp_subif);
     ILOG1_D("DCDP subif : 0x%08x",dc_dp_subif.subif);

#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
    dc_dp_set_subifid_pkt(port_id, skb, dc_dp_subif.subif, 0);
    /* update flag */
    dc_dp_set_mark_pkt(port_id, skb, 0);
#else
    dc_dp_set_subifid_pkt(&skb->DW0, dc_dp_subif.subif, 0);
    /* update flag */
    dc_dp_set_mark_pkt(skb, 0);
#endif
  }

  desc_fields->num_dw = 2;
  dp_fields = desc_fields->dw;

  /* DW0 */
  subif_id = 0;
  /* TODO : This API, once  enhanced for genric platforms, should be used accordingly */
  /* TODO : BITFIELDS needs to replaced by generic SET/GET API implementation */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
  dc_dp_get_subifid_pkt(port_id, skb, &subif_id, 0);
#else
  dc_dp_get_subifid_pkt(&skb->DW0, &subif_id, 0);
#endif
#if 0 /* 32 VAP integration change */
  mcf = MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_MCF);
  if (mcf) {
    value = MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_MCF,   1, uint32) |
            MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_VAPID, MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_VAPID), uint32) |
            MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_MCIDX, MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_MCIDX), uint32) |
            MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_ETYPE, MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_FRAMETYPE), uint32);
  } else {
    value = MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_MCF,   0, uint32) |
            MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_VAPID, MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_VAPID), uint32) |
            MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_STAID, MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_STAID), uint32) |
            MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_ETYPE, MTLK_BFIELD_GET(subif_id, TX_DATA_INFO_FRAMETYPE), uint32);
  }
#endif
  dp_fields[0].dw = 0;
  dp_fields[0].desc_mask = 0x0000FFFF;
  dp_fields[0].desc_val = HOST_TO_MAC32(subif_id);

  /* DW3 */
  /* dev is unused param, for future use */
  wmm_class = dc_dp_mark_pkt_devqos(port_id, dev, skb);
  if (__UNLIKELY(wmm_class < 0)) {
    ELOG_V("Can't provide QoS AC/TID mapping for a given packet");
    return DC_DP_FAILURE;
  }

  value = MTLK_BFIELD_VALUE(TX_DEV_SPEC_INFO_CLASS, wmm_class, uint32);
  dp_fields[1].dw = 3;
  dp_fields[1].desc_mask = 0x0F000000;
  dp_fields[1].desc_val = value;

  return DC_DP_SUCCESS;
}

/******************************************************************************/
/*!  \brief Used as a data RX callback for DataPath driver
 *   \param[in] rx_if  Rx If netdevice pointer
 *   \param[in] tx_if  Tx If netdevice pointer - optional
 *   \param[in] subif  Tx/Rx SubIf pointer
 *   \param[in] skb  Pointer to pointer to packet buffer, like sk_buff
 *   \param[in] len  Length of the packet (optional as also present in skb->len)
 *   \param[in] flags:
 *              - DC_DP_F_RX_LITEPATH : received packet through litepath
 *   \return 0 if OK / -1 if error / > 0, if DC DP layer needs to do LitePath Rx
 *   \note The receive callback is MUST to register and is invoked by DirectConnect datapath driver to pass the packets to the peripheral driver
 *   \note If the Peripheral Driver does not pass the packet to stack, and does
 *   not free the Rx buffer, it needs to return > 0 to indicate to DC DP Library to
 *   send packet to accelerator - Valid for LitePath Rx case only.
 */
static int
_mtlk_df_dcdp_rx_cb (struct net_device *rxif, struct net_device *txif, struct dp_subif *rx_subif, struct sk_buff **pskb, int32_t len, uint32_t flags)
{
  int res;

  MTLK_ASSERT(pskb);
  MTLK_ASSERT(*pskb);

  if(__UNLIKELY(!pskb || !(*pskb))) {
    return DC_DP_FAILURE;
  }

  if (__UNLIKELY(len < (int)sizeof(struct ethhdr))) {
    ILOG1_V("Ethernet Frame length is wrong");
    dev_kfree_skb_any(*pskb);
    res = DC_DP_FAILURE;
    goto exit;
  }

  if (DC_DP_F_RX_LITEPATH == flags) {
    if (txif) {
      /* process as litepath TX callback */
      if (__UNLIKELY(!_mtlk_validate_wlan_netdev(txif))) {
        dev_kfree_skb_any(*pskb);
        res = DC_DP_FAILURE;
        goto exit;
      }
      ILOG3_S("Forward to litepath: TX. txif:%s", txif->name);
      res = _mtlk_df_user_litepath_tx(txif, *pskb, len);
    } else {
      MTLK_ASSERT(rxif);
      /* process as litepath callback, but forward skb to linux */
      if (__UNLIKELY(!_mtlk_validate_wlan_netdev(rxif))) {
        dev_kfree_skb_any(*pskb);
        res = DC_DP_FAILURE;
        goto exit;
      }
      ILOG3_S("Forward to litepath: Linux. rxif:%s", rxif->name);
      res = _mtlk_df_user_litepath_rx(rxif, *pskb, len);
    }
  }
  else
  {
    /* .. otherwise process as FastPath RX callback */
    MTLK_ASSERT(rxif);
    MTLK_ASSERT(rx_subif);
    if (__UNLIKELY(!_mtlk_validate_wlan_netdev(rxif) || !rx_subif)) {
      dev_kfree_skb_any(*pskb);
      res = DC_DP_FAILURE;
      goto exit;
    }
    res = mtlk_mmb_dcdp_rx(rxif, rx_subif, *pskb, len);
  }

exit:
/* res greater than 0 can be returned by _mtlk_df_user_litepath_tx */
  if (res <= 0) {
    /* sk_buff was consumed */
    *pskb = NULL;
  }
  return res;
}

/******************************************************************************/
/*!  \brief  Delete specified MAC from specified iface hostDB
 *   \param[in] netif  Network interface from which this MAC address is to be deleted
 *   \param[in] mac_addr  MAC Address of Station
 *   \param[in] flags  Reserved for future use

 *   \return 0 if OK, -1 on ERROR
 */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,0,2)
static int32_t
_mtlk_df_dcdp_notify_mac_cb (struct net_device *netif,
                             char *mac_addr,
                             uint32_t flags)
{
  mtlk_df_user_t *df_user;
  mtlk_vap_handle_t vap_handle;

  MTLK_ASSERT(netif);
  MTLK_ASSERT(mac_addr);

  if (__UNLIKELY(!netif || !mac_addr))
    return DC_DP_FAILURE;

  df_user = mtlk_df_user_from_ndev(netif);
  if (__UNLIKELY(NULL == df_user))
    return DC_DP_FAILURE;

  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));

  if (MTLK_ERR_OK != mtlk_hstdb_remove_host_by_addr(mtlk_core_get_hstdb(mtlk_vap_get_core(vap_handle)),
                                                    (const IEEE_ADDR *)mac_addr)) {
    ILOG1_Y("MAC address %Y not found in host DB", mac_addr);
    return DC_DP_FAILURE;
  }

  return DC_DP_SUCCESS;
}
#endif

struct dc_dp_cb dp_cb = {
      .rx_fn = _mtlk_df_dcdp_rx_cb,                             /*!< Rx function callback */
      .stop_fn = _mtlk_df_dcdp_stop_tx_cb,                      /*!< Stop Tx function callback for Tx flow control */
      .restart_fn = _mtlk_df_dcdp_restart_tx_cb,                /*!< Start Tx function callback for Tx flow control */
      .get_desc_info_fn = _mtlk_df_dcdp_get_dev_specific_desc_info_cb,  /*!< Get device/peripheral specific field info for desc/HD */
      .get_subif_fn = _mtlk_df_dcdp_get_subif_cb,               /*!< Get Station Id callback for MAC address */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,0,2)
      .notify_mac_fn = _mtlk_df_dcdp_notify_mac_cb,             /*!< Delete specified MAC from specified iface hostDB */
#endif
};


#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_INFO_DLEVEL)
static char const*
_mtlk_df_dcdp_mode_to_string (mtlk_hw_dcdp_mode_t mode)
{
    switch (mode) {
      case MTLK_DC_DP_MODE_FASTPATH:         return "FastPath";
      case MTLK_DC_DP_MODE_SWPATH:           return "Software only";
      case MTLK_DC_DP_MODE_LITEPATH:         return "Software+LitePath";
      case MTLK_DC_DP_MODE_LITEPATH_ONLY:    return "LitePath only (internal support of SW path)";
      default:                               return "Unknown DC DP connection type";
    }
}
#endif

static __INLINE BOOL
__mtlk_df_dcdp_is_fastpath_mode (struct dc_dp_dev *dp_devspec) {
    return (DC_DP_ACCEL_FULL_OFFLOAD == dp_devspec->dc_accel_used);
}

static void
_mtlk_df_dcdp_print_hwcmode (const char *msg, uint32 hw_cmode)
{
#if (IWLWAV_RTLOG_MAX_DLEVEL >= 2)
  char buf[128] = "";
  if (hw_cmode & DC_DP_F_DCCNTR_MODE_INCREMENTAL)
    strlcat(buf, "INCREMENTAL|", sizeof(buf));
  if (hw_cmode & DC_DP_F_DCCNTR_MODE_CUMULATIVE)
    strlcat(buf, "CUMULATIVE|", sizeof(buf));
  if (hw_cmode & DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN)
    strlcat(buf, "LITTLE_ENDIAN|", sizeof(buf));
  if (hw_cmode & DC_DP_F_DCCNTR_MODE_BIG_ENDIAN)
    strlcat(buf, "BIG_ENDIAN|", sizeof(buf));
  if (buf[0]) {
    size_t len = wave_strlen(buf, sizeof(buf)-1);
    if (len)
      buf[len - 1] = '\0';  /* truncate last '|' */
    else
      buf[0] = '\0'; /* shouldn't happen, but needed for KW */
  }
  ILOG2_SDS("%s: 0x%08X (%s)", msg, hw_cmode, buf);
#endif
}

static void
_mtlk_df_dcdp_print_hwcap (const char *msg, uint32 hw_cap)
{
#if (IWLWAV_RTLOG_MAX_DLEVEL >= 2)
  #if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
  char buf[384] = "";
  if (hw_cap & DC_DP_F_HOST_CAP_SG)
    strlcat(buf, "SG|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_IP_CSUM)
    strlcat(buf, "IP_CSUM|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_HW_CSUM)
    strlcat(buf, "HW_CSUM|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_IPV6_CSUM)
    strlcat(buf, "IPV6_CSUM|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_RXCSUM)
    strlcat(buf, "RXCSUM|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_TSO)
    strlcat(buf, "TSO|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_TSO6)
    strlcat(buf, "TSO6|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_LRO)
    strlcat(buf, "LRO|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_TX_FCS)
    strlcat(buf, "TX_FCS|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_RX_FCS)
    strlcat(buf, "RX_FCS|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_TX_WO_FCS)
    strlcat(buf, "TX_WO_FCS|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_RX_WO_FCS)
    strlcat(buf, "RX_WO_FCS|", sizeof(buf));

  if (hw_cap & DC_DP_F_HOST_CAP_TX_PMAC)
    strlcat(buf, "TX_PMAC|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_RX_PMAC)
    strlcat(buf, "RX_PMAC|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_RX_WO_PMAC)
    strlcat(buf, "RX_WO_PMAC|", sizeof(buf));

  if (hw_cap & DC_DP_F_HOST_CAP_HW_QOS)
    strlcat(buf, "HW_QOS|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_HW_QOS_WAN)
    strlcat(buf, "HW_QOS_WAN|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_RX_FRAG_HANDLING_RESTRICTED)
    strlcat(buf, "FRAG_EXCEPT|", sizeof(buf));
  if (hw_cap & DC_DP_F_HOST_CAP_DESC_2DW)
    strlcat(buf, "DESC_2DW|", sizeof(buf));
  #else
  char buf[128] = "";
  if (hw_cap & DC_DP_F_CAP_TX_FCS)
    strlcat(buf, "TX_FCS|", sizeof(buf));
  if (hw_cap & DC_DP_F_CAP_RX_FCS)
    strlcat(buf, "RX_FCS|", sizeof(buf));
  if (hw_cap & DC_DP_F_CAP_SG)
    strlcat(buf, "SG|", sizeof(buf));
  if (hw_cap & DC_DP_F_CAP_TSO)
    strlcat(buf, "TSO|", sizeof(buf));
  if (hw_cap & DC_DP_F_CAP_LRO)
    strlcat(buf, "LRO|", sizeof(buf));
  if (hw_cap & DC_DP_F_CAP_FRAG_EXCEPT)
    strlcat(buf, "FRAG_EXCEPT|", sizeof(buf));
  #endif /* DC_DP_API_VERSION_CODE */

  if (buf[0]) {
    size_t len = wave_strlen(buf, sizeof(buf)-1);
    if (len)
      buf[len - 1] = '\0';  /* truncate last '|' */
    else
      buf[0] = '\0'; /* shouldn't happen, but needed for KW */
  }
  ILOG2_SDS("%s: 0x%08X (%s)", msg, hw_cap, buf);
#endif
}

#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_INFO_DLEVEL)
static const char*
_mtlk_df_dcdp_fastpath_dcmode (enum dc_dp_mode_type dc_mode)
{
  switch (dc_mode) {
    case DC_DP_MODE_TYPE_0:
      return "MODE_0";
    case DC_DP_MODE_TYPE_1:
      return "MODE_1";
#ifdef WAVE_DCDP_LGM_FLM_SUPPORTED
    case DC_DP_MODE_TYPE_0_EXT:
      return "MODE_0_EXT";
    case DC_DP_MODE_TYPE_1_EXT:
      return "MODE_1_EXT";
#endif
    default:
      return "Unknown";
  }
}
#endif

static mtlk_error_t
_mtlk_df_dcdp_get_peripheral_config (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev)
{
  mtlk_error_t res = MTLK_ERR_OK;
  MTLK_ASSERT(dp_dev);
  MTLK_ASSERT(hw);

  memset(&dp_dev->dp_config, 0, sizeof(dp_dev->dp_config));

#ifndef WAVE_DCDP_LGM_FLM_SUPPORTED
  MTLK_UNREFERENCED_PARAM(hw);
  /* For GRX */
  dp_dev->dp_config.rxin_ring_size = 0x800;
  dp_dev->dp_config.rxout_ring_size = 0x800;
  dp_dev->dp_config.txin_ring_size = 0x800;
  dp_dev->dp_config.txout_ring_size = 0x800;
  dp_dev->dp_config.rx_bufs = 0x800;
  dp_dev->dp_config.tx_bufs = 0x2000;
#else
  {
    enum dc_dp_dev_type dev_type;

    if (wave_hw_radio_band_cfg_is_single(hw)) {
      dev_type = (mtlk_is_band_supported(HANDLE_T(hw), 0 ,MTLK_HW_BAND_2_4_GHZ)) ? DC_DP_DEV_WAV_600_24G : DC_DP_DEV_WAV_600_5G;
    } else {
      dev_type = DC_DP_DEV_WAV_600_CDB;
    }

    if (DC_DP_FAILURE == dc_dp_get_peripheral_config(dev_type, &dp_dev->dp_config)) {
      res = MTLK_ERR_UNKNOWN;
    }
  }
#endif
  TRACE_PARAM_NOTE("DC DP peripheral config data:");
  TRACE_PARAM_INT(dp_dev->dp_config.rxin_ring_size);
  TRACE_PARAM_INT(dp_dev->dp_config.rxout_ring_size);
  TRACE_PARAM_INT(dp_dev->dp_config.txin_ring_size);
  TRACE_PARAM_INT(dp_dev->dp_config.txout_ring_size);
  TRACE_PARAM_INT(dp_dev->dp_config.rx_bufs);
  TRACE_PARAM_INT(dp_dev->dp_config.tx_bufs);
  return res;
}

static mtlk_error_t
_mtlk_df_dcdp_get_host_capability (struct dc_dp_host_cap *dp_cap)
{
  MTLK_ASSERT(dp_cap);
  if (__UNLIKELY(dc_dp_get_host_capability (dp_cap, 0) == DC_DP_FAILURE)) {
    ELOG_V("Cannot get DC DP capabilities");
    memset(dp_cap, 0, sizeof(*dp_cap));
    return MTLK_ERR_UNKNOWN;
  }

  ILOG0_DDDD("DC DP library version: %u.%u.%u (0x%08X)",
      (uint8)(dp_cap->version >> 16), (uint8)(dp_cap->version >> 8), (uint8)(dp_cap->version), dp_cap->version);
#if defined(DC_DP_API_VERSION_CODE)
  ILOG0_DDDD("DC DP headers version: %u.%u.%u (0x%08X)",
      (uint8)(DC_DP_API_VERSION_CODE >> 16), (uint8)(DC_DP_API_VERSION_CODE >> 8), (uint8)(DC_DP_API_VERSION_CODE), DC_DP_API_VERSION_CODE);
#endif

  ILOG0_S("FastPath: %s", dp_cap->fastpath.support ? "supported" : "not supported");
  if (dp_cap->fastpath.support) {
    ILOG0_DS("supported HW DC mode: %u (%s)",
        dp_cap->fastpath.hw_dcmode, _mtlk_df_dcdp_fastpath_dcmode(dp_cap->fastpath.hw_dcmode));
    _mtlk_df_dcdp_print_hwcmode("supported HW DC counter mode (dev2soc)", dp_cap->fastpath.hw_cmode.dev2soc_write);
    _mtlk_df_dcdp_print_hwcmode("supported HW DC counter mode (soc2dev)", dp_cap->fastpath.hw_cmode.soc2dev_write);
    _mtlk_df_dcdp_print_hwcap("supported HW DC caps", dp_cap->fastpath.hw_cap);
  }

  ILOG0_S("LitePath: %s", dp_cap->litepath.support ? "supported" : "not supported");
  ILOG0_S("SWPath: %s", dp_cap->swpath.support ? "supported" : "not supported");
  if (dp_cap->swpath.support) {
    ILOG2_D("supported SW DC mode: %u", dp_cap->swpath.sw_dcmode);
    _mtlk_df_dcdp_print_hwcmode("supported SW DC counter mode (dev2soc)", dp_cap->swpath.sw_cmode.dev2soc_write);
    _mtlk_df_dcdp_print_hwcmode("supported SW DC counter mode (soc2dev)", dp_cap->swpath.sw_cmode.soc2dev_write);
    _mtlk_df_dcdp_print_hwcap("supported SW DC caps", dp_cap->swpath.sw_cap);
  }
  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_df_dcdp_datapath_dev_init (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev)
{
  uint8     dev_port;

  MTLK_ASSERT(hw);
  MTLK_ASSERT(dp_dev);

  memset(dp_dev, 0, sizeof(*dp_dev));
  dp_dev->dp_mode = MTLK_DC_DP_MODE_UNREGISTERED;
  dp_dev->dp_port_id[0]  = DC_DP_FAILURE;
  dp_dev->dp_port_id[1]  = DC_DP_FAILURE;

  /* We need 2 different ports per card for CDB support */
  dev_port = 2 * mtlk_hw_mmb_get_card_idx(hw);
  dp_dev->dp_dev_port[0] = dev_port + 0;
  dp_dev->dp_dev_port[1] = dev_port + 1;

  dp_dev->fw_datapath_mode = DATA_PATH_MODE_DC_NONE;
  if (MTLK_ERR_OK == _mtlk_df_dcdp_get_host_capability(&dp_dev->dp_cap)) {
    if (!mtlk_mmb_fastpath_available(hw))
      return;
    if (dp_dev->dp_cap.fastpath.support) {
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(4,0,0)
      dp_dev->dp_frag_wa_enable = !!(DC_DP_F_HOST_CAP_RX_FRAG_HANDLING_RESTRICTED & dp_dev->dp_cap.fastpath.hw_cap);
#else
      dp_dev->dp_frag_wa_enable = !!(DC_DP_F_CAP_FRAG_EXCEPT & dp_dev->dp_cap.fastpath.hw_cap);
#endif
      if (dp_dev->dp_frag_wa_enable) {
        ILOG0_V("Activate fragmentation W/A");
      }
    }

    /* Detect FW dc_mode */
    switch (dp_dev->dp_cap.fastpath.hw_dcmode) {
      /* GRX350/500: DC_DP_MODE_TYPE_0 -> FW DC_MODE_0 */
      case DC_DP_MODE_TYPE_0:
        dp_dev->fw_datapath_mode = DATA_PATH_MODE_DC_MODE_0; break;
      case DC_DP_MODE_TYPE_1:
        dp_dev->fw_datapath_mode = DATA_PATH_MODE_DC_MODE_1; break;
#ifdef WAVE_DCDP_LGM_FLM_SUPPORTED
      case DC_DP_MODE_TYPE_0_EXT:
        dp_dev->fw_datapath_mode = DATA_PATH_MODE_DC_MODE_3; break;
      case DC_DP_MODE_TYPE_1_EXT:
        dp_dev->fw_datapath_mode = DATA_PATH_MODE_DC_MODE_2; break;
#endif
    }
  }
}

void __MTLK_IFUNC
mtlk_df_dcdp_release_buflist(mtlk_dcdp_dev_t *dp_dev)
{
  MTLK_ASSERT(dp_dev);
  /* caller need to free buffer */
  if (dp_dev->dp_resources.buflist) {
    kfree(dp_dev->dp_resources.buflist);
    dp_dev->dp_resources.buflist = NULL;
  }
}

int __MTLK_IFUNC
mtlk_df_dcdp_datapath_dev_unregister (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev, BOOL is_recovery)
{
    int res = MTLK_ERR_OK;

    MTLK_ASSERT(dp_dev);

    /* In case of recovery AND LITEPATH_ONLY mode do not uninitialize DC DP device */
    if (is_recovery && (dp_dev->dp_mode == MTLK_DC_DP_MODE_LITEPATH_ONLY)) {
      return res;
    }

    /* De-register DirectConnect device */
    if (MTLK_DC_DP_MODE_UNREGISTERED != dp_dev->dp_mode) {
      /* remove all MAC table entries/routing sessions which uses specific netdevice */
      /*dc_dp_disconn_if(dp_dev->dp_ndev, NULL, NULL, 0);*/

      if (dp_dev->is_registered[1]) {
        MTLK_ASSERT(dp_dev->dp_ndev[1]);
        ILOG0_S("Deregistering network device %s from DirectConnect...", dp_dev->dp_ndev[1]->name);
        if (DC_DP_FAILURE == dc_dp_register_dev(THIS_MODULE, dp_dev->dp_port_id[1], dp_dev->dp_ndev[1], &dp_cb, NULL, &dp_dev->dp_devspec[1], DC_DP_F_DEREGISTER)) {
          ELOG_S("Can't deregister network device %s from DirectConnect", dp_dev->dp_ndev[1]->name);
          res = MTLK_ERR_UNKNOWN;
        }
        dp_dev->is_registered[1] = FALSE;
      }

      if (dp_dev->is_registered[0]) {
        MTLK_ASSERT(dp_dev->dp_ndev[0]);
        ILOG0_S("Deregistering network device %s from DirectConnect...", dp_dev->dp_ndev[0]->name);
        if (DC_DP_FAILURE == dc_dp_register_dev(THIS_MODULE, dp_dev->dp_port_id[0], dp_dev->dp_ndev[0], &dp_cb, NULL, &dp_dev->dp_devspec[0], DC_DP_F_DEREGISTER)) {
          ELOG_S("Can't deregister network device %s from DirectConnect", dp_dev->dp_ndev[0]->name);
          res = MTLK_ERR_UNKNOWN;
        }
        dp_dev->is_registered[0] = FALSE;
      }
      mtlk_df_dcdp_release_buflist(dp_dev);
      dp_dev->dp_mode = MTLK_DC_DP_MODE_UNREGISTERED;
    }

    /* De-register DirectConnect port */
    if (!is_recovery) {
      int i = WAVE_DCDP_MAX_PORTS - 1;
      do {
        if (DC_DP_FAILURE != dp_dev->dp_port_id[i]) {
          ILOG0_D("Deallocating DirectConnect port %d...", dp_dev->dp_port_id[i]);
          if (DC_DP_FAILURE == dc_dp_alloc_port(THIS_MODULE, dp_dev->dp_dev_port[i], dp_dev->dp_ndev[i], dp_dev->dp_port_id[i], DC_DP_F_DEREGISTER)) {
            ELOG_D("Can't deallocate DirectConnect port %d", dp_dev->dp_port_id[i]);
            res = MTLK_ERR_UNKNOWN;
          } else {
            ILOG0_D("DirectConnect port %d deallocated", dp_dev->dp_port_id[i]);
            dp_dev->dp_port_id[i] = DC_DP_FAILURE; /* successfully deallocated */
            dp_dev->dp_ndev[i] = NULL;
          }
        }
        if (dp_dev->dp_radio_ndev[i]) {
          mtlk_osal_mem_free(dp_dev->dp_radio_ndev[i]);
          dp_dev->dp_radio_ndev[i] = NULL;
        }
      } while (i--);
      dp_dev->dp_port_flags = 0;
    }

    return res;
}

static mtlk_ndev_t *litepath_dev_create (mtlk_ndev_t *netdev)
{
  mtlk_ndev_t *radio_ndev = mtlk_osal_mem_alloc(sizeof(mtlk_ndev_t), MTLK_MEM_TAG_DCDP);
  if (NULL == radio_ndev)
    return NULL;

  memset(radio_ndev, 0, sizeof(mtlk_ndev_t));
  wave_strcopy(radio_ndev->name, netdev->name, sizeof(radio_ndev->name));
  strlcat(radio_ndev->name, "_radio", sizeof(radio_ndev->name));
  return radio_ndev;
}

int __MTLK_IFUNC
mtlk_df_dcdp_datapath_dev_register (mtlk_hw_t *hw, mtlk_dcdp_dev_t *dp_dev, mtlk_dcdp_init_info_t *dp_init /*, BOOL is_recovery*/)
/* Register and initialize datapath device */
{
  int res = MTLK_ERR_OK;
  uint8                   dev_port;
  BOOL                    use_internal_sw_path = FALSE;
#ifdef MTLK_LGM_PLATFORM_FPGA
  BOOL                    is_cdb = FALSE;
#else
  BOOL                    is_cdb = wave_hw_is_cdb(hw);
#endif
  mtlk_ndev_t             *netdev[WAVE_DCDP_MAX_NDEVS] = { NULL, NULL };

  MTLK_ASSERT(hw);
  MTLK_ASSERT(dp_dev);
  MTLK_ASSERT(dp_init);

#ifdef MTLK_LGM_PLATFORM_FPGA
  ILOG0_V("MTLK_LGM_PLATFORM_FPGA: use single port/single band");
#endif

  netdev[0] = wave_hw_ndev_get(hw, 0);
  if (is_cdb) {
    netdev[1] = wave_hw_ndev_get(hw, 1);
  }


  if (MTLK_DC_DP_MODE_UNREGISTERED == dp_dev->dp_mode)
  {
    uint32 flags;

    /* We must have enough buffers at least for management */
    MTLK_ASSERT(dp_init->rd_pool_size > GEN5_MANG_RX_RING_SIZE);

    /* Register DC DP port only if it was not registered yet */
    TRACE_PARAM_INT(dp_dev->dp_dev_port[0]);
    TRACE_PARAM_INT(dp_dev->dp_dev_port[1]);
    TRACE_PARAM_INT(dp_dev->dp_port_id[0]);
    TRACE_PARAM_INT(dp_dev->dp_port_id[1]);

    if (DC_DP_FAILURE != dp_dev->dp_port_id[0]) {
      /* Port was already allocated, so we have to take existing flags */
      flags = dp_dev->dp_port_flags;
    }
    else {
      /* Request a datapath port depending from desired mode */
      flags = (mtlk_mmb_fastpath_available(hw)) ? DC_DP_F_FASTPATH : (DC_DP_F_LITEPATH | DC_DP_F_SWPATH);

      /* Create radio device for LitePath interface */
      if (DC_DP_F_LITEPATH & flags) {
        mtlk_ndev_t *radio_ndev;
        ILOG0_V("Create radio device for LitePath interface 0");
        radio_ndev = litepath_dev_create(netdev[0]);
        if (NULL == radio_ndev) {
          res = MTLK_ERR_NO_MEM;
          goto finish;
        }
        dp_dev->dp_ndev[0] = dp_dev->dp_radio_ndev[0] = radio_ndev;

        if(is_cdb) {
          ILOG0_V("Create radio device for LitePath interface 1");
          radio_ndev = litepath_dev_create(netdev[1]);
          if (NULL == radio_ndev) {
            res = MTLK_ERR_NO_MEM;
            goto finish;
          }
          dp_dev->dp_ndev[1] = dp_dev->dp_radio_ndev[1] = radio_ndev;
        }
      } else {
        if ((flags & DC_DP_F_FASTPATH) && (is_cdb)){
          flags |= DC_DP_F_MULTI_PORT;
        }
        dp_dev->dp_ndev[0] = netdev[0];
        dp_dev->dp_ndev[1] = netdev[1];
      }

      dev_port = dp_dev->dp_dev_port[0];
      ILOG0_D("Registering DirectConnect dev_port %u", dev_port);
      dp_dev->dp_port_id[0] = dc_dp_alloc_port(THIS_MODULE, dev_port, dp_dev->dp_ndev[0], 0, flags | DC_DP_F_FAST_WLAN);
      TRACE_PARAM_INT(dp_dev->dp_port_id[0]);
      if (DC_DP_FAILURE == dp_dev->dp_port_id[0]) {
        ELOG_V("Can't allocate DirectConnect port 0");
        res = MTLK_ERR_UNKNOWN;
        goto finish;
      }

/* For FLM we need to allocate 2 ports for SB chip since the vap bit for FLM is only 4 bits*/
#if 0
      if(!is_cdb) {
        flags |= DC_DP_F_MULTI_PORT;
        dev_port = dp_dev->dp_dev_port[1];
        ILOG0_D("FLM Registering DirectConnect dev_port %u", dev_port);
        dp_dev->dp_port_id[1] = dc_dp_alloc_port(THIS_MODULE, dev_port, dp_dev->dp_ndev[0], dp_dev->dp_port_id[0], flags | DC_DP_F_FAST_WLAN | DC_DP_F_SHARED_RES);
        if (DC_DP_FAILURE == dp_dev->dp_port_id[1]) {
          ELOG_V("FLM Can't allocate DirectConnect port 1");
          res = MTLK_ERR_UNKNOWN;
          goto finish;
        }
      }
#endif /* MTLK_FLM_PLATFORM */

      if(is_cdb) {
        if (DC_DP_F_FASTPATH & flags) {
          dev_port = dp_dev->dp_dev_port[1];
          ILOG0_D("Registering DirectConnect dev_port %u", dev_port);
          dp_dev->dp_port_id[1] = dc_dp_alloc_port(THIS_MODULE, dev_port, dp_dev->dp_ndev[1], dp_dev->dp_port_id[0], flags | DC_DP_F_SHARED_RES | 
                                                   DC_DP_F_FAST_WLAN);
        } else {
          dp_dev->dp_port_id[1] = dc_dp_alloc_port(THIS_MODULE, dev_port, dp_dev->dp_ndev[1], 0, flags | DC_DP_F_FAST_WLAN);
        }
        TRACE_PARAM_INT(dp_dev->dp_port_id[1]);
        if (DC_DP_FAILURE == dp_dev->dp_port_id[1]) {
          ELOG_V("Can't allocate DirectConnect port 1");
          res = MTLK_ERR_UNKNOWN;
          goto finish;
        }
      }

      dp_dev->dp_port_flags = flags;
    }



    /* Get peripheral configuration and adjust pool_size according to configured values */
    res = _mtlk_df_dcdp_get_peripheral_config(hw, dp_dev);
    if (MTLK_ERR_OK != res)
      goto finish;

    TRACE_PARAM_NOTE("Input params for registration:");

    /* prepare dp_resources structure:
       num_bufs_req     - [in]     Number of buffers to allocate (Directconnect Peripheral -> SoC direction)
       tx_num_bufs_req  - [in]     Number of buffers to allocate (SoC direction -> Directconnect Peripheral)
       rings            - [in/out] All the communication rings depending on DCMode0/DCMode1/SWModes
       num_umt          - [in]     Number of UMT units used - Could be HW or SW. Eg. VRX518 DSL Bonding will use 2 UMT.
       *umt;            - [in/out] array of number of UMT structures. Caller needs to allocate and free memory of umt pointer.
       *dev_priv_res    - [in]     Pointer to any dev specific resource structure
     */
    memset(&dp_dev->dp_resources, 0, sizeof(dp_dev->dp_resources));
    dp_dev->dp_resources.num_bufs_req = MIN(dp_dev->dp_config.rx_bufs, dp_init->rd_pool_size);
#ifdef WAVE_DCDP_NUM_OF_PDS_SUPPORTED
    dp_dev->dp_resources.tx_num_bufs_req = MIN(dp_dev->dp_config.tx_bufs, dp_init->num_of_pds);
#endif
    dp_dev->dp_resources.num_dccntr = MTLK_DCDP_DCCNTR_NUM; /* Number of DC counters. For WAVE500 it is 1 always */
    dp_dev->dp_resources.dccntr = dp_dev->dp_dccntr; /* assign static array */
    memset(dp_dev->dp_resources.dccntr, 0, MTLK_DCDP_DCCNTR_NUM * sizeof(struct dc_dp_dccntr));
    TRACE_PARAM_INT(dp_dev->dp_resources.num_bufs_req);
#ifdef WAVE_DCDP_NUM_OF_PDS_SUPPORTED
    TRACE_PARAM_INT(dp_dev->dp_resources.tx_num_bufs_req);
#endif
    TRACE_PARAM_INT(dp_dev->dp_resources.num_dccntr);

    /* prepare rings structures:
       size          - [in/out] Size of ring in terms of entries : TBD : Zero implies ring is unused
       flags         - [in]     DC_DP_F_PKTDESC_ENQ, DC_DP_F_PKTDESC_RET, DC_DP_F_RET_RING_SAME_AS_ENQ
     */
    dp_dev->dp_resources.rings.soc2dev.size       = MIN(dp_dev->dp_config.txin_ring_size, dp_init->tx_ring_size);
    dp_dev->dp_resources.rings.soc2dev.flags      = DC_DP_F_PKTDESC_ENQ;
    dp_dev->dp_resources.rings.soc2dev_ret.size   = MIN(dp_dev->dp_config.txout_ring_size, dp_init->tx_ring_size);
    dp_dev->dp_resources.rings.soc2dev_ret.flags  = DC_DP_F_PKTDESC_RET; /* | DC_DP_F_RET_RING_SAME_AS_ENQ;*/
    dp_dev->dp_resources.rings.dev2soc.size       = MIN(dp_dev->dp_config.rxout_ring_size, dp_init->rx_ring_size);
    dp_dev->dp_resources.rings.dev2soc.flags      = DC_DP_F_PKTDESC_ENQ;
    dp_dev->dp_resources.rings.dev2soc_ret.size   = MIN(dp_dev->dp_config.rxin_ring_size, dp_init->rx_ring_size);
    dp_dev->dp_resources.rings.dev2soc_ret.flags  = DC_DP_F_PKTDESC_ENQ; /* | DC_DP_F_RET_RING_SAME_AS_ENQ; */

    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev.flags);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev_ret.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev_ret.flags);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc.flags);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_ret.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_ret.flags);

    TRACE_PARAM_INT(dp_init->rd_pool_size);
    TRACE_PARAM_INT(dp_init->tx_ring_size);
    TRACE_PARAM_INT(dp_init->rx_ring_size);

    ILOG1_D("Requested # RX buffers: %u", dp_dev->dp_resources.num_bufs_req);
#ifdef WAVE_DCDP_NUM_OF_PDS_SUPPORTED
    ILOG1_D("Requested # TX buffers: %u", dp_dev->dp_resources.tx_num_bufs_req);
#endif
    ILOG1_D("Requested RXOUT ring size: %u", dp_dev->dp_resources.rings.dev2soc.size);
    ILOG1_D("Requested RXIN  ring size: %u", dp_dev->dp_resources.rings.dev2soc_ret.size);
    ILOG1_D("Requested TXIN  ring size: %u", dp_dev->dp_resources.rings.soc2dev.size);
    ILOG1_D("Requested TXOUT ring size: %u", dp_dev->dp_resources.rings.soc2dev_ret.size);

    /* prepare dccntr structures:
       soc_write_dccntr_mode  - [in] counter mode: Incremental or Cumulative; BIG-endian or LITTLE-endian
      *soc2dev_enq_phys_base  - [in] Physical Base address of DC counter write location (in Directconnect dev/peripheral memory). It MUST be provided by peripheral driver.
      *soc2dev_enq_base       - [in] Base address of DC counter write location (in Directconnect dev/peripheral memory)
       soc2dev_enq_dccntr_len - [in] DC counter(s) len (in bytes) to be written. Optionally, it may include Tx, and Rx return counters, where each counter is length of 4 bytes.
    */
    dp_dev->dp_resources.dccntr[0].soc_write_dccntr_mode      = DC_DP_F_DCCNTR_MODE_INCREMENTAL;
    dp_dev->dp_resources.dccntr[0].soc2dev_enq_dccntr_len     = MTLK_DCDP_DCCNTR_SIZE * MTLK_DCDP_DCCNTR_LENGTH;
    dp_dev->dp_resources.dccntr[0].soc2dev_ret_deq_dccntr_len = MTLK_DCDP_DCCNTR_SIZE * MTLK_DCDP_DCCNTR_LENGTH;
    dp_dev->dp_resources.dccntr[0].dev2soc_deq_dccntr_len     = MTLK_DCDP_DCCNTR_SIZE * MTLK_DCDP_DCCNTR_LENGTH;
    dp_dev->dp_resources.dccntr[0].dev2soc_ret_enq_dccntr_len = MTLK_DCDP_DCCNTR_SIZE * MTLK_DCDP_DCCNTR_LENGTH;

    if (MTLK_DC_DP_CNTR_MODE_BIG_ENDIAN == dp_init->cntr_mode) {
      ILOG1_V("Setup BIG-ENDIAN counters");
      dp_dev->dp_resources.dccntr[0].soc_write_dccntr_mode    |= DC_DP_F_DCCNTR_MODE_BIG_ENDIAN;
    }
    else {
      ILOG1_V("Setup LITTLE-ENDIAN counters");
      dp_dev->dp_resources.dccntr[0].soc_write_dccntr_mode    |= DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN;
    }

    dp_dev->dp_resources.dccntr[0].soc2dev_enq_phys_base     = dp_init->soc2dev_enq_phys_base;
    dp_dev->dp_resources.dccntr[0].soc2dev_enq_base          = dp_init->soc2dev_enq_base;
    dp_dev->dp_resources.dccntr[0].dev2soc_ret_enq_phys_base = dp_init->dev2soc_ret_enq_phys_base;
    dp_dev->dp_resources.dccntr[0].dev2soc_ret_enq_base      = dp_init->dev2soc_ret_enq_base;
    /* RX_OUT + TX_OUT */
    dp_dev->dp_resources.dccntr[0].soc2dev_ret_deq_phys_base = dp_init->soc2dev_ret_deq_phys_base;
    dp_dev->dp_resources.dccntr[0].soc2dev_ret_deq_base      = dp_init->soc2dev_ret_deq_base;
    dp_dev->dp_resources.dccntr[0].dev2soc_deq_phys_base     = dp_init->dev2soc_deq_phys_base;
    dp_dev->dp_resources.dccntr[0].dev2soc_deq_base          = dp_init->dev2soc_deq_base;

    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].soc_write_dccntr_mode);
    /* Wave TX_IN, filled */
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_enq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_enq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].soc2dev_enq_dccntr_len);
    /* Wave TX_OUT, filled */
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_ret_deq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_ret_deq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].soc2dev_ret_deq_dccntr_len);
    /* Wave RX_OUT, filled*/
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_deq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_deq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev2soc_deq_dccntr_len);
    /* Wave RX_IN, filled */
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_ret_enq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_ret_enq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev2soc_ret_enq_dccntr_len);

    if (dp_dev->dp_frag_wa_enable) {
      /* Address of fragmentation ring is provided by FW and passed to DCDP */
      dp_dev->dp_resources.rings.dev2soc_except.size      = dp_init->frag_ring_size;
      dp_dev->dp_resources.rings.dev2soc_except.phys_base = dp_init->dev2soc_frag_ring_phys_base;
      dp_dev->dp_resources.rings.dev2soc_except.base      = dp_init->dev2soc_frag_ring_base;
      dp_dev->dp_resources.rings.dev2soc_except.flags     = DC_DP_F_PKTDESC_ENQ;

      dp_dev->dp_resources.dccntr[0].dev2soc_except_deq_phys_base  = dp_init->dev2soc_frag_deq_phys_base;
      dp_dev->dp_resources.dccntr[0].dev2soc_except_deq_base       = dp_init->dev2soc_frag_deq_base;
      dp_dev->dp_resources.dccntr[0].dev2soc_except_deq_dccntr_len = MTLK_DCDP_DCCNTR_SIZE * MTLK_DCDP_DCCNTR_LENGTH;
      dp_dev->dp_resources.dccntr[0].dev2soc_except_enq_phys_base  = dp_init->dev2soc_frag_enq_phys_base;
      dp_dev->dp_resources.dccntr[0].dev2soc_except_enq_base       = dp_init->dev2soc_frag_enq_base;
      dp_dev->dp_resources.dccntr[0].dev2soc_except_enq_dccntr_len = MTLK_DCDP_DCCNTR_SIZE * MTLK_DCDP_DCCNTR_LENGTH;

      TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_except.size);
      TRACE_PARAM_PTR(dp_dev->dp_resources.rings.dev2soc_except.phys_base);
      TRACE_PARAM_PTR(dp_dev->dp_resources.rings.dev2soc_except.base);
      TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_except.flags);
      TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_except_deq_phys_base);
      TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_except_deq_base);
      TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev2soc_except_deq_dccntr_len);
      TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_except_enq_phys_base);
      TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_except_enq_base);
      TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev2soc_except_enq_dccntr_len);
    }

    /* prepare dp_devspec structure:
    dev_name      - [in] Netdevice Name string
    dev_ctx       - [in] Private device context
    */
    wave_strcopy(dp_dev->dp_devspec[0].dev_name, dp_dev->dp_ndev[0]->name, sizeof(dp_dev->dp_devspec[0].dev_name));

    dp_dev->dp_devspec[0].dev_ctx = NULL;
    TRACE_PARAM_STR(dp_dev->dp_devspec[0].dev_name);
    TRACE_PARAM_PTR(dp_dev->dp_devspec[0].dev_ctx);

    /* try to register network device */
    dp_dev->is_registered[0] = FALSE;
    ILOG0_S("Registering network device %s in DirectConnect...", dp_dev->dp_ndev[0]->name);
    if (DC_DP_SUCCESS != dc_dp_register_dev(THIS_MODULE, dp_dev->dp_port_id[0], dp_dev->dp_ndev[0], &dp_cb, &dp_dev->dp_resources, &dp_dev->dp_devspec[0], DC_DP_F_QOS)) {
      ELOG_S("Can't register device %s in DirectConnect!", dp_dev->dp_ndev[0]->name);

      if ((DC_DP_F_FASTPATH & flags) == 0) {
        /* if Fastpath wasn't requested from DirectConnect, wlan driver can continue with internal support of SW path */
        use_internal_sw_path = TRUE;
      }

      res = MTLK_ERR_UNKNOWN;
      goto finish;
    }
    dp_dev->is_registered[0] = TRUE;

    dp_dev->is_registered[1] = FALSE;
    if (is_cdb) {
      wave_strcopy(dp_dev->dp_devspec[1].dev_name, dp_dev->dp_ndev[1]->name, sizeof(dp_dev->dp_devspec[1].dev_name));

      dp_dev->dp_devspec[1].dev_ctx = NULL;
      TRACE_PARAM_STR(dp_dev->dp_devspec[1].dev_name);
      TRACE_PARAM_PTR(dp_dev->dp_devspec[1].dev_ctx);

      /* try to register network device */
      ILOG0_S("Registering network device %s in DirectConnect...", dp_dev->dp_ndev[1]->name);
      if (DC_DP_SUCCESS != dc_dp_register_dev(THIS_MODULE, dp_dev->dp_port_id[1], dp_dev->dp_ndev[1], &dp_cb, &dp_dev->dp_resources, &dp_dev->dp_devspec[1], (flags & DC_DP_F_FASTPATH ? DC_DP_F_QOS | DC_DP_F_SHARED_RES : DC_DP_F_QOS))) {
        ELOG_S("Can't register device %s in DirectConnect!", dp_dev->dp_ndev[1]->name);

        if ((DC_DP_F_FASTPATH & flags) == 0) {
          /* if Fastpath wasn't requested from DirectConnect, wlan driver can continue with internal support of SW path */
          use_internal_sw_path = TRUE;
        }

        res = MTLK_ERR_UNKNOWN;
        goto finish;
      }
      dp_dev->is_registered[1] = TRUE;
    }

    /* Device was registered, but mode still unknown */
    dp_dev->dp_mode = MTLK_DC_DP_MODE_UNKNOWN;

    if (dp_dev->dp_devspec[0].dc_tx_ring_used != dp_dev->dp_devspec[0].dc_rx_ring_used) {
      ELOG_V("TX and RX rings should have a same type");
      res = MTLK_ERR_UNKNOWN;
      goto finish;
    }

    /* Determine and check type of DC connection */
#if MTLK_DCDP_SWPATH_SUPPORTED
    if ((dp_dev->dp_devspec[0].dc_accel_used == DC_DP_ACCEL_NO_OFFLOAD) && (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_SW_MODE1))
      dp_dev->dp_mode = MTLK_DC_DP_MODE_SWPATH;
    else
#endif
    if ((dp_dev->dp_devspec[0].dc_accel_used == DC_DP_ACCEL_FULL_OFFLOAD) && (
              (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_HW_MODE0) ||
              (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_HW_MODE1) ||
              (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_HW_MODE0_EXT) ||
              (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_HW_MODE1_EXT)))
      dp_dev->dp_mode = MTLK_DC_DP_MODE_FASTPATH;
    else if ((dp_dev->dp_devspec[0].dc_accel_used == DC_DP_ACCEL_PARTIAL_OFFLOAD) && (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_NONE))
      dp_dev->dp_mode = MTLK_DC_DP_MODE_LITEPATH_ONLY;
    else if ((dp_dev->dp_devspec[0].dc_accel_used == DC_DP_ACCEL_PARTIAL_OFFLOAD) && (dp_dev->dp_devspec[0].dc_tx_ring_used == DC_DP_RING_SW_MODE1))
      dp_dev->dp_mode = MTLK_DC_DP_MODE_LITEPATH;

    ILOG0_DDS("DirectConnect accel type:%u, ring type:%u --> %s", dp_dev->dp_devspec[0].dc_accel_used, dp_dev->dp_devspec[0].dc_tx_ring_used,
            _mtlk_df_dcdp_mode_to_string(dp_dev->dp_mode));

    if (MTLK_DC_DP_MODE_UNKNOWN == dp_dev->dp_mode) {
      ELOG_V("Unknown/unsupported DC DP mode!");
      res = MTLK_ERR_UNKNOWN;
      goto finish;
    }

    if ((DC_DP_F_FASTPATH & flags) && !__mtlk_df_dcdp_is_fastpath_mode(&dp_dev->dp_devspec[0])) {
      /* Currently driver cannot to change fastpath mode to SW/LitePath mode on-the-fly (it requires FW reload) */
      ELOG_V("DC DP fastpath mode was requested!");
      res = MTLK_ERR_UNKNOWN;
      goto finish;
    }

    TRACE_PARAM_NOTE("Result of device registration:");
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev_write_dccntr_mode);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_deq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_deq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].soc2dev_deq_dccntr_len);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_ret_enq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].soc2dev_ret_enq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].soc2dev_ret_enq_dccntr_len);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_enq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_enq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev2soc_enq_dccntr_len);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_ret_deq_phys_base);
    TRACE_PARAM_PTR(dp_dev->dp_resources.dccntr[0].dev2soc_ret_deq_base);
    TRACE_PARAM_INT(dp_dev->dp_resources.dccntr[0].dev2soc_ret_deq_dccntr_len);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc.desc_dwsz);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_ret.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_ret.desc_dwsz);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev.desc_dwsz);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev_ret.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.soc2dev_ret.desc_dwsz);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_except.size);
    TRACE_PARAM_INT(dp_dev->dp_resources.rings.dev2soc_except.desc_dwsz);
    TRACE_PARAM_INT(dp_dev->dp_resources.num_bufpools);
    TRACE_PARAM_PTR(dp_dev->dp_resources.buflist);

    ILOG0_S("Network device %s registered in DirectConnect", dp_dev->dp_ndev[0]->name);
    if (is_cdb) {
      ILOG0_S("Network device %s registered in DirectConnect", dp_dev->dp_ndev[1]->name);
    }
  }

finish:
  /* Check for errocode and cleanup everything if error */
  if (MTLK_ERR_OK != res) {
    if ((NULL != dp_dev->dp_ndev[1]) && (DC_DP_FAILURE != dp_dev->dp_port_id[1])) {

      /* De-register DirectConnect device */
      if (dp_dev->is_registered[1]) {
        if (DC_DP_FAILURE == dc_dp_register_dev(THIS_MODULE, dp_dev->dp_port_id[1], dp_dev->dp_ndev[1], &dp_cb, NULL, &dp_dev->dp_devspec[1], DC_DP_F_DEREGISTER)) {
          ELOG_S("Can't deregister network device %s from DirectConnect", dp_dev->dp_ndev[1]->name);
        }
        dp_dev->is_registered[1] = FALSE;
      }

      /* De-register DirectConnect port */
      if (DC_DP_FAILURE == dc_dp_alloc_port(THIS_MODULE, dp_dev->dp_dev_port[1], dp_dev->dp_ndev[1], dp_dev->dp_port_id[1], DC_DP_F_DEREGISTER)) {
        ELOG_D("Can't deallocate DirectConnect port %d", dp_dev->dp_port_id[1]);
      }
      dp_dev->dp_port_id[1] = DC_DP_FAILURE;
      dp_dev->dp_ndev[1] = NULL;
    }

    if ((NULL != dp_dev->dp_ndev[0]) && (DC_DP_FAILURE != dp_dev->dp_port_id[0])) {

      /* De-register DirectConnect device */
      if (dp_dev->is_registered[0]) {
        if (DC_DP_FAILURE == dc_dp_register_dev(THIS_MODULE, dp_dev->dp_port_id[0], dp_dev->dp_ndev[0], &dp_cb, NULL, &dp_dev->dp_devspec[0], DC_DP_F_DEREGISTER)) {
          ELOG_S("Can't deregister network device %s from DirectConnect", dp_dev->dp_ndev[0]->name);
        }
        dp_dev->is_registered[0] = FALSE;
        /* make sure buflist is released after netdev 0 derigistration */
        mtlk_df_dcdp_release_buflist(dp_dev);
        dp_dev->dp_mode = MTLK_DC_DP_MODE_UNREGISTERED;
      }

      /* De-register DirectConnect port */
      if (DC_DP_FAILURE == dc_dp_alloc_port(THIS_MODULE, dp_dev->dp_dev_port[0], dp_dev->dp_ndev[0], dp_dev->dp_port_id[0], DC_DP_F_DEREGISTER)) {
        ELOG_D("Can't deallocate DirectConnect port %d", dp_dev->dp_port_id[0]);
      }
      dp_dev->dp_port_id[0] = DC_DP_FAILURE;
      dp_dev->dp_ndev[0] = NULL;
    }
    dp_dev->dp_port_flags = 0;
    dp_dev->dp_mode = MTLK_DC_DP_MODE_UNREGISTERED;

    /* release radio netdevice */
    if (dp_dev->dp_radio_ndev[1]) {
      mtlk_osal_mem_free(dp_dev->dp_radio_ndev[1]);
      dp_dev->dp_radio_ndev[1] = NULL;
    }
    if (dp_dev->dp_radio_ndev[0]) {
      mtlk_osal_mem_free(dp_dev->dp_radio_ndev[0]);
      dp_dev->dp_radio_ndev[0] = NULL;
    }

    if (use_internal_sw_path) {
      WLOG_S("%s: continue with internal slow path", netdev[0]->name);
      if (is_cdb) {
        WLOG_S("%s: continue with internal slow path", netdev[1]->name);
      }
      res = MTLK_ERR_OK;
    }
  }

  return res;
}

mtlk_nbuf_t * __MTLK_IFUNC
_mtlk_df_dcdp_nbuf_alloc (mtlk_df_t *df, uint32 size)
{
    mtlk_df_user_t  *df_user;
    mtlk_nbuf_t     *nbuf;

    MTLK_ASSERT(df);
    MTLK_ASSERT(size);

    df_user = mtlk_df_get_user(df);
    if (df_user->dcdp.mode == DC_DP_MODE_UNREGISTERED) {
        return NULL; /* To prevent an error message from dc_dp_alloc_skb */
    }
    nbuf = dc_dp_alloc_skb(size,  &df_user->dcdp.subif_id, 0 /* flags */);
    if (nbuf) {
        mtlk_nbuf_priv_init(mtlk_nbuf_priv(nbuf));
    }

    return nbuf;
}
#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

void mtlk_df_resume_stat_timers (mtlk_df_user_t *df_user)
{
    mtlk_vap_manager_t *vap_mgr;
    mtlk_vap_handle_t vap_handle;
    mtlk_df_t *master_df = mtlk_df_user_get_master_df(df_user);
    mtlk_df_user_t *df_master_user;
    unsigned vaps_count, vap_id;

    MTLK_CHECK_DF_NORES(master_df);
    df_master_user = mtlk_df_get_user(master_df);
    vap_mgr = mtlk_df_get_vap_manager(mtlk_df_user_get_df(df_master_user));
    vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);

    for (vap_id = 0; vap_id < vaps_count; vap_id++)
    {
        if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_id, &vap_handle) == MTLK_ERR_OK) {
            mtlk_df_user_t *tmp_df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
            ILOG2_D("Resume stat_timer for vap_id - %d\n", vap_id);
            mtlk_osal_timer_set(&tmp_df_user->slow_ctx->stat_timer, jiffies_to_msecs(tmp_df_user->slow_ctx->stat_timer.diff_time));
        }
    }

    return;
}

void mtlk_df_stop_stat_timers (mtlk_df_user_t *df_user)
{
    mtlk_vap_manager_t *vap_mgr;
    mtlk_vap_handle_t vap_handle;
    mtlk_df_t *master_df = mtlk_df_user_get_master_df(df_user);
    mtlk_df_user_t *df_master_user;
    unsigned vaps_count, vap_id;

    MTLK_CHECK_DF_NORES(master_df);
    df_master_user = mtlk_df_get_user(master_df);
    vap_mgr = mtlk_df_get_vap_manager(mtlk_df_user_get_df(df_master_user));
    vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);

    for (vap_id = 0; vap_id < vaps_count; vap_id++)
    {
        if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_id, &vap_handle) == MTLK_ERR_OK) {
          mtlk_df_user_t *tmp_df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
          mtlk_hw_t *hw = mtlk_vap_get_hw(vap_handle);
          if (!mtlk_osal_timer_is_cancelled(&tmp_df_user->slow_ctx->stat_timer)) {
            tmp_df_user->slow_ctx->stat_timer.diff_time = jiffies + msecs_to_jiffies(wave_hw_mmb_get_stats_poll_period(hw)) - tmp_df_user->slow_ctx->stat_timer.os_timer.expires;
            ILOG2_DD("Cancel stat_timer as Scan is running for vap_id - %d, diff_time - %d\n", vap_id, tmp_df_user->slow_ctx->stat_timer.diff_time);
            mtlk_osal_timer_cancel_sync(&tmp_df_user->slow_ctx->stat_timer);
          }
        }
    }

    return;
}

