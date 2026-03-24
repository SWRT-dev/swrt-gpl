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
 * HAL interface
 *
 */

#ifndef __MTLKHAL_H__
#define __MTLKHAL_H__

#include "mhi_umi.h"
#include "mtlk_df_fw.h"
#include "hw_hal.h"
#include "bitrate.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/**********************************************************************
 * HW => Core (logic module):
 *  - must be implemented by core (logic module)
 *  - called HW layer
***********************************************************************/

/* MTLK_CORE_PROP_TXM_STUCK_DETECTED additional info */
typedef enum
{
  MTLK_TXM_MAN,
  MTLK_TXM_DBG,
  MTLK_TXM_BCL,
  MTLK_TXM_LAST
} mtlk_txm_obj_id_e;

typedef enum _mtlk_core_prop_e
{/* prop_id */
  MTLK_CORE_PROP_MAC_SW_RESET_ENABLED,  /* SET: unsupported,            GET: uint32*     */
  MTLK_CORE_PROP_MAC_STUCK_DETECTED,    /* SET: uint32 *fw_cpu_no,      GET: unsupported */
  MTLK_CORE_PROP_IS_DUT,                /* SET: unsupported,            GET: BOOL*       */
  MTLK_CORE_PROP_IS_MAC_FATAL_PENDING,  /* SET: unsupported,            GET: BOOL*       */
  MTLK_CORE_PROP_LAST
} mtlk_core_prop_e;

#define MAX_FIRMWARE_FILENAME_LEN 64

#define IRQS_TO_HANDLE 6

#define DATA_REQ_MIRROR_PTR(msg) ((mtlk_hw_data_req_mirror_t *)(msg))
#define HW_MSG_PTR(msg)          ((mtlk_hw_msg_t *)(msg))

/* HW chip revision */
typedef enum _mtlk_chip_rev_e {
  MTLK_CHIP_REV_NONE = 0,
  MTLK_CHIP_REV_WRX500,
  MTLK_CHIP_REV_WRX514,
  MTLK_CHIP_REV_WRX300,
  MTLK_CHIP_REV_CNT
} mtlk_chip_rev_e;

/* File type.
 * Here should be consecutive numbering!!!
 */
typedef enum _mtlk_file_type_e {
  MTLK_FILE_TYPE_HW,
  MTLK_FILE_TYPE_ANT_TX,
  MTLK_FILE_TYPE_ANT_RX,
} mtlk_file_type_e;

typedef struct _mtlk_core_firmware_file_t
{
  mtlk_file_type_e       ftype;
  char                   fname[MAX_FIRMWARE_FILENAME_LEN];
  mtlk_df_fw_file_buf_t  fcontents;
  BOOL                   update_version;
} __MTLK_IDATA mtlk_core_firmware_file_t;

#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
typedef struct _mtlk_core_fw_log_buffer_t
{
  uint32 addr;
  uint32 length;
} __MTLK_IDATA mtlk_core_fw_log_buffer_t;
#endif

typedef struct _mtlk_core_handle_rx_data_t
{
  mtlk_nbuf_t               *nbuf;     /* Network buffer        */
  uint32                     priority; /* QOS/TID               */
  uint32                     sid;      /* Station ID            */
} __MTLK_IDATA mtlk_core_handle_rx_data_t;

typedef struct _mtlk_core_handle_tx_data_t
{
  mtlk_nbuf_t               *nbuf;     /* Network buffer         */
  sta_entry                 *dst_sta;  /* Dest. station Entry    */
  mtlk_ndev_t               *rx_if;    /* RX interface           */
  void                      *rx_subif; /* pointer to DC DP subinterface data: struct dp_subif */
  uint8                      vap_id;   /* vap id for data path (can be secondary vap) */
} __MTLK_IDATA mtlk_core_handle_tx_data_t;

typedef struct _mtlk_phy_info_t
{
    mtlk_bitrate_info_t  bitrate_info;
    /* Values per Antenna */
    union {
        uint32 word_rssi;
        struct {
            int8 rssi[4];
        };
    };
    union {
        uint32 word_noise;
        struct {
            int8 noise[4];
        };
    };
    union {
        uint32 word_snr;
        struct {
            int8 snr[4];
        };
    };
    /* Maximal/estimation values */
    union {
      int8  max_rssi; /* Max RSSI in dBm */
      int8  sig_dbm;
    };
    int8    noise_estimation; /* in dBm */
    int8    snr_db;

    uint8   phy_mode;
    uint8   rate_idx;
} __MTLK_IDATA mtlk_phy_info_t;

typedef struct _mtlk_core_handle_rx_bss_t
{
  uint8                     *buf;      /* Data buffer */
  uint32                     size;     /* Data size   */
  mtlk_phy_info_t            phy_info; /* PHY metrics */
  BOOL                       make_assert; /* Assert fw in case of error */
} __MTLK_IDATA mtlk_core_handle_rx_bss_t;


typedef int    (__MTLK_IFUNC *mtlk_hal_core_start_f)(mtlk_vap_handle_t vap_handle);
typedef int    (__MTLK_IFUNC *mtlk_hal_core_handle_tx_data_f)(mtlk_core_t* nic, mtlk_core_handle_tx_data_t *data, uint32 nbuf_flags);
typedef int    (__MTLK_IFUNC *mtlk_hal_core_release_tx_data_f)(mtlk_vap_handle_t vap_handle, mtlk_hw_data_req_mirror_t *data_req);
typedef int    (__MTLK_IFUNC *mtlk_hal_core_handle_rx_data_f)(mtlk_vap_handle_t vap_handle, mtlk_core_handle_rx_data_t *data);
typedef int    (__MTLK_IFUNC *mtlk_hal_core_handle_rx_bss_f)(mtlk_vap_handle_t vap_handle, mtlk_core_handle_rx_bss_t *data);
typedef void   (__MTLK_IFUNC *mtlk_hal_core_handle_rx_ctrl_f)(mtlk_vap_handle_t vap_handle,
                                                              uint32            id,
                                                              void*             payload,
                                                              uint32            payload_buffer_size);
typedef int    (__MTLK_IFUNC *mtlk_hal_core_get_prop_f)(mtlk_vap_handle_t vap_handle, mtlk_core_prop_e prop_id, void* buffer, uint32 size);
typedef int    (__MTLK_IFUNC *mtlk_hal_core_set_prop_f)(mtlk_vap_handle_t vap_handle, mtlk_core_prop_e prop_id, void* buffer, uint32 size);
typedef void   (__MTLK_IFUNC *mtlk_hal_core_stop_f)(mtlk_vap_handle_t vap_handle);
typedef void   (__MTLK_IFUNC *mtlk_hal_core_prepare_stop_f)(mtlk_vap_handle_t vap_handle);

/* core specific function table
  NOTE: all functions should be initialized by core, no any NULL values
        accepted. In case of unsupported functionality the function with
        empty body required!
 */
typedef struct _mtlk_core_vft_t
{
  mtlk_hal_core_start_f           start;
  mtlk_hal_core_handle_tx_data_f  handle_tx_data;
  mtlk_hal_core_release_tx_data_f release_tx_data;
  mtlk_hal_core_handle_rx_data_f  handle_rx_data;
  mtlk_hal_core_handle_rx_bss_f   handle_rx_bss;
  mtlk_hal_core_handle_rx_ctrl_f  handle_rx_ctrl;   /* MTLK_ERR_PENDING for pending. In this be used to respond later.  */
  mtlk_hal_core_get_prop_f        get_prop;
  mtlk_hal_core_set_prop_f        set_prop;
  mtlk_hal_core_stop_f            stop;
  mtlk_hal_core_prepare_stop_f    prepare_stop;
} __MTLK_IDATA mtlk_core_vft_t;

typedef struct _mtlk_core_api_t
{
  mtlk_handle_t          core_handle;
  mtlk_core_vft_t const *vft;
} __MTLK_IDATA mtlk_core_api_t;

int __MTLK_IFUNC mtlk_core_api_init (mtlk_vap_handle_t vap_handle, mtlk_core_api_t *core_api, mtlk_df_t* df);
void __MTLK_IFUNC mtlk_core_api_cleanup (mtlk_core_api_t *core_api);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* !__MTLKHAL_H__ */
