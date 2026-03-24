/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_HWHAL_H_
#define _MTLK_HWHAL_H_

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_HWHAL
#define LOG_LOCAL_FID   1

/**********************************************************************
 * Common definitions. Used by HW and Core (logic module)
***********************************************************************/
typedef struct _mtlk_hw_msg_t                 mtlk_hw_msg_t;
typedef struct _mtlk_persistent_device_data_t mtlk_persistent_device_data_t;
/**********************************************************************/

/**********************************************************************
 * Core (logic module) => HW :
 *  - must be implemented by HW layer
 *  - called by core (logic module)
***********************************************************************/

#define MTLK_HW_VERSION_PRINTOUT_LEN 4096

typedef enum _mtlk_hw_prop_e
{/* prop_id */
  MTLK_HW_PROP_STATE,             /* buffer: GET: mtlk_hw_state_e*,              SET - mtlk_hw_state_e*                              */
  MTLK_HW_FREE_TX_MSGS,           /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_TX_MSGS_USED_PEAK,      /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_PROGMODEL,              /* buffer: GET: mtlk_core_firmware_file_t*,    SET - const mtlk_core_firmware_file_t*              */
  MTLK_HW_DUMP,                   /* buffer: GET: mtlk_hw_dump_t*,               SET - not supported                                 */
  MTLK_HW_BIST,                   /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_RESET,                  /* buffer: GET: not supported,                 SET - void                                          */
#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
  MTLK_HW_FW_LOG_BUFFER,          /* buffer: GET: not supported,                 SET - const mtlk_core_fw_log_buffer_t*              */
  MTLK_HW_LOG,                    /* buffer: GET: not suppotted,                 SET - const mtlk_log_event_t*                       */
  MTLK_HW_FW_LOGGER_IS_EX,        /* buffer: GET: uint8*,                        SET - not supported                                 */
  MTLK_HW_LOG_BUFFERS_CNT,        /* buffer: GET: uint32*,                       SET - not supported                                 */
#endif
  MTLK_HW_FW_BUFFERS_PROCESSED,   /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_FW_BUFFERS_DROPPED,     /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_RADARS_DETECTED,        /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_IRBD,                   /* buffer: GET: mtlk_irbd_t**,                 SET - not supported                                 */
  MTLK_HW_WSS,                    /* buffer: GET: mtlk_wss_t**,                  SET - not supported                                 */
  MTLK_HW_PROP_EEPROM_DATA,       /* buffer: GET: mtlk_eeprom_data_t**,          SET - not supported                                 */
  MTLK_HW_PROP_CCR,               /* buffer: GET: mtlk_ccr_t**,                  SET - not supported                                 */
  MTLK_HW_PROP_CARD_TYPE,         /* buffer: GET: mtlk_card_type_t*,             SET - not supported                                 */
  MTLK_HW_PROP_CARD_TYPE_INFO,    /* buffer: GET: mtlk_card_type_info_t*,        SET - not supported                                 */
  MTLK_HW_PROP_CHIP_REVISION,     /* buffer: GET: uint8*,                        SET - not supported                                 */
  MTLK_HW_PROP_CHIP_NAME,         /* buffer: GET: char const**,                  SET - not supported                                 */
  MTLK_HW_EEPROM_NAME,            /* buffer: GET: char const*,                   SET - not supported                                 */
  MTLK_HW_SIGNED_CAL_NAME,        /* buffer: GET: char const*,                   SET - not supported                                 */
  MTLK_HW_PROP_TXMM_BASE,         /* buffer: GET: mtlk_txmm_base_t*,             SET - not supported                                 */
  MTLK_HW_PROP_TXDM_BASE,         /* buffer: GET: mtlk_txmm_base_t*,             SET - not supported                                 */
#ifdef CPTCFG_IWLWAV_LEGACY_INT_RECOVERY_MON
  MTLK_HW_ISR_LOST_SUSPECT,       /* buffer: GET: uint32*,                       SET - not supported                                 */
  MTLK_HW_ISR_LOST_RECOVERED,     /* buffer: GET: uint32*,                       SET - not supported                                 */
#endif

#ifdef MTLK_DEBUG
  MTLK_HW_FW_CFM_IN_TASKLET,         /* buffer: GET: uint32*,                    SET - not supported                                 */
  MTLK_HW_FW_TX_TIME_INT_TO_TASKLET, /* buffer: GET: uint64*,                    SET - not supported                                 */
  MTLK_HW_FW_TX_TIME_INT_TO_CFM,     /* buffer: GET: uint64*,                    SET - not supported                                 */
  MTLK_HW_FW_DATA_IN_TASKLET,        /* buffer: GET: uint32*,                    SET - not supported                                 */
  MTLK_HW_FW_RX_TIME_INT_TO_TASKLET, /* buffer: GET: uint64*,                    SET - not supported                                 */
  MTLK_HW_FW_RX_TIME_INT_TO_PCK,     /* buffer: GET: uint64*,                    SET - not supported                                 */
#endif

  MTLK_HW_CALIB_BUF_DMA_ADDR,        /* buffer: GET: uint8*,                     SET - not supported                                 */
  MTLK_HW_VERSION_PRINTOUT,          /* buffer: GET: char*,                      SET - not supported                                 */
  MTLK_HW_MC_GROUP_ID,               /* buffer: GET: uint32,                     SET - uint32                                        */

#ifdef CONFIG_WAVE_DEBUG
  MTLK_HW_BCL_ON_EXCEPTION,          /* buffer: GET: UMI_BCL_REQUEST*,           SET - UMI_BCL_REQUEST*                              */
  MTLK_HW_DATA_TXOUT_LIM,            /* buffer: GET: unsigned*,                  SET - unsigned *                                    */
  MTLK_HW_DATA_RX_LIM,               /* buffer: GET: unsigned*,                  SET - unsigned *                                    */
  MTLK_HW_BSS_RX_LIM,                /* buffer: GET: unsigned*,                  SET - unsigned *                                    */
  MTLK_HW_BSS_CFM_LIM,               /* buffer: GET: unsigned*,                  SET - unsigned *                                    */
  MTLK_HW_LEGACY_LIM,                /* buffer: GET: unsigned*,                  SET - unsigned *                                    */
  MTLK_HW_DBG_ASSERT_FW,             /* buffer: GET: not supported,              SET - uint32* (LMIPS or UMIPS)                      */
  MTLK_HW_DBG_ASSERT_ALL_MACS,       /* buffer: GET: not supported,              SET - void - all MACs reset                         */

#endif

  MTLK_HW_BSS_MGMT_MAX_MSGS,            /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_FREE_MSGS,           /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_MSGS_USED_PEAK,      /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_MAX_RES_MSGS,        /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_FREE_RES_MSGS,       /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_MSGS_RES_USED_PEAK,  /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_PROBE_RESP_SENT,     /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_PROBE_RESP_DROPPED,  /* buffer: GET: uint32*,                 SET - not supported                                 */
  MTLK_HW_BSS_MGMT_TX_QUE_FULL,         /* buffer: GET: uint32*,                 SET - not supported                                 */
#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
  MTLK_HW_PROP_PMCU_FREQ,               /* buffer: GET: BOOL*, Is Current CPU Freq High, SET - No parameters, only change to High Frequency  */
#endif /* CPTCFG_IWLWAV_PMCU_SUPPORT */

  MTLK_HW_PROP_LAST
} mtlk_hw_prop_e;

typedef struct _mtlk_hw_dump_t
{
  uint32 addr;
  uint32 size;
  void*  buffer;
} __MTLK_IDATA mtlk_hw_dump_t;

typedef struct
{
  uint32             index;      /* index in mirror array */
  mtlk_dlist_entry_t list_entry; /* for mirror elements list */
} mtlk_hw_mirror_hdr_t;

typedef struct
{
  mtlk_hw_mirror_hdr_t hdr;        /* Header */
  mtlk_nbuf_t         *nbuf;       /* Network buffer   */
  uint32               dma_addr;   /* DMA mapped address */
  uint32               size;       /* Data buffer size */
  uint8                tid;        /* Packets priority */
  mtlk_osal_timestamp_t ts;        /* Timestamp (used for TX monitoring) */
  uint8                vap_id;
  uint8                radio_id;
  uint16               sid;             /* Station SID number                               */
  /* send TX data */
  uint8                wds;             /* WDS packet flag                                  */
  uint8                frame_type;      /* Frame type, 0b00 - Ethernet II, !EAPOL ### 0b11 - EAPOL ### 0b10 - IPX, LLC/SNAP */
  uint8                mc_index;        /* multicast group, or 0 if not multicast */
  uint8                mcf;
  /* release TX data */
  UMI_STATUS           status;          /* Is Tx failed?                            */
  uint16               nof_retries;     /* Number of retries made by FW             */
} mtlk_hw_data_req_mirror_t;

typedef enum _mtlk_hw_state_e  /* don't reorder elements - the order is important */
{
  MTLK_HW_STATE_HALTED,        /* HW/FW is in Initialization state */
  MTLK_HW_STATE_INITIATING,
  MTLK_HW_STATE_WAITING_READY, /* HW/FW is in Steady state */
  MTLK_HW_STATE_READY,
  MTLK_HW_STATE_ERROR,         /* HW/FW is in Error state */
  MTLK_HW_STATE_EXCEPTION,
  MTLK_HW_STATE_APPFATAL,
  MTLK_HW_STATE_UNLOADING,
  MTLK_HW_STATE_MAC_ASSERTED,
  MTLK_HW_STATE_LAST
} mtlk_hw_state_e;

typedef mtlk_hw_msg_t* (__MTLK_IFUNC *mtlk_hw_get_msg_to_send_f)(mtlk_hw_t *hw, mtlk_vap_handle_t vap_handle, uint32 *nof_free_tx_msgs);
typedef int            (__MTLK_IFUNC *mtlk_hw_send_data_f)(mtlk_hw_t *hw, mtlk_vap_handle_t vap_handle, mtlk_hw_data_req_mirror_t *data);
typedef int            (__MTLK_IFUNC *mtlk_hw_release_msg_to_send_f)(mtlk_hw_t *hw, mtlk_hw_msg_t *msg);
typedef int            (__MTLK_IFUNC *mtlk_hw_set_prop_f)(mtlk_hw_t *hw, mtlk_hw_prop_e prop_id, void *buffer, uint32 size);
typedef int            (__MTLK_IFUNC *mtlk_hw_get_prop_f)(mtlk_hw_t *hw, mtlk_hw_prop_e prop_id, void *buffer, uint32 size);
typedef int            (__MTLK_IFUNC *mtlk_hw_load_file_f)(mtlk_hw_t *hw, const char *name, mtlk_df_fw_file_buf_t *fb);
typedef void           (__MTLK_IFUNC *mtlk_hw_unload_file_f)(mtlk_hw_t *hw, mtlk_df_fw_file_buf_t *fb);
typedef int            (__MTLK_IFUNC *mtlk_hw_get_info_f)(mtlk_hw_t *card, char *hw_info, uint32 size);

typedef struct _mtlk_hw_vft_t
{
  mtlk_hw_get_msg_to_send_f     get_msg_to_send;
  mtlk_hw_send_data_f           send_data;
  mtlk_hw_release_msg_to_send_f release_msg_to_send;
  mtlk_hw_set_prop_f            set_prop;
  mtlk_hw_get_prop_f            get_prop;
  mtlk_hw_load_file_f           load_file;
  mtlk_hw_unload_file_f         unload_file;
  mtlk_hw_get_info_f            get_info;
} __MTLK_IDATA mtlk_hw_vft_t;

struct _mtlk_hw_api_t
{
  const mtlk_hw_vft_t *vft;
  mtlk_hw_t           *hw;
};

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
/* Getting timestamp: FW TSF */
uint32 __MTLK_IFUNC mtlk_hw_get_timestamp(mtlk_vap_handle_t vap_handle);
/* Getting 2 timestamps: Logger and FW TSF */
void    __MTLK_IFUNC mtlk_hw_get_log_fw_timestamps(mtlk_vap_handle_t vap_handle, uint32 *t_log, uint32 *t_tsf);
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

static __INLINE mtlk_hw_msg_t *
mtlk_hw_get_msg_to_send (mtlk_hw_api_t *hw_api, mtlk_vap_handle_t vap_handle, uint32 *nof_free_tx_msgs)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->get_msg_to_send);
  MTLK_ASSERT(NULL != vap_handle);

  return hw_api->vft->get_msg_to_send(hw_api->hw, vap_handle, nof_free_tx_msgs);
}

static __INLINE int
mtlk_hw_send_data (mtlk_hw_api_t *hw_api, mtlk_vap_handle_t vap_handle, mtlk_hw_data_req_mirror_t *data)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->send_data);
  MTLK_ASSERT(NULL != vap_handle);
  MTLK_ASSERT(NULL != data);

  return hw_api->vft->send_data(hw_api->hw, vap_handle, data);
}

static __INLINE int
mtlk_hw_release_msg_to_send (mtlk_hw_api_t *hw_api, mtlk_hw_msg_t *msg)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->release_msg_to_send);
  MTLK_ASSERT(NULL != msg);

  return hw_api->vft->release_msg_to_send(hw_api->hw, msg);
}

static __INLINE int
mtlk_hw_set_prop (mtlk_hw_api_t *hw_api, mtlk_hw_prop_e prop_id, void *buffer, uint32 size)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->set_prop);

  if (__UNLIKELY(NULL == hw_api))
    return MTLK_ERR_UNKNOWN;

  return hw_api->vft->set_prop(hw_api->hw, prop_id, buffer, size);
}

static __INLINE int
mtlk_hw_get_prop (mtlk_hw_api_t *hw_api, mtlk_hw_prop_e prop_id, void *buffer, uint32 size)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->get_prop);
  MTLK_ASSERT(NULL != buffer);

  if (__UNLIKELY(NULL == hw_api))
    return MTLK_ERR_UNKNOWN;

  return hw_api->vft->get_prop(hw_api->hw, prop_id, buffer, size);
}

static __INLINE int
mtlk_hw_load_file (mtlk_hw_api_t *hw_api, const char *name, mtlk_df_fw_file_buf_t *fb)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->load_file);

  if (__UNLIKELY(NULL == hw_api))
    return MTLK_ERR_UNKNOWN;

  return hw_api->vft->load_file(hw_api->hw, name, fb);
}

static __INLINE void
mtlk_hw_unload_file (mtlk_hw_api_t *hw_api, mtlk_df_fw_file_buf_t *fb)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->hw);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->unload_file);

  if (__UNLIKELY(NULL == hw_api))
    return;

  hw_api->vft->unload_file(hw_api->hw, fb);
}

static __INLINE int
mtlk_hw_get_info (mtlk_hw_api_t *hw_api, mtlk_hw_t *card, char *hw_info, uint32 size)
{
  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != hw_api->vft);
  MTLK_ASSERT(NULL != hw_api->vft->get_info);
  MTLK_ASSERT(NULL != card);
  MTLK_ASSERT(NULL != hw_info);

  if (__UNLIKELY(NULL == hw_api))
    return MTLK_ERR_UNKNOWN;

  return hw_api->vft->get_info(card, hw_info, size);
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* _MTLK_HWHAL_H_ */
