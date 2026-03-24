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
 * Beacon manager implementation
 *
 */
#include "mtlkinc.h"
#include "mtlkbfield.h"
#include "core.h"
#include "core_config.h"
#include "mtlk_df_nbuf.h"
#include "cfg80211.h"
#include "mtlk_vap_manager.h"
#include "mtlk_param_db.h"
#include "hw_mmb.h"
#include "mtlk_df.h"
#include "wave_radio.h"
#include "frame.h"
#include "mac80211.h"
#include "mtlkaux.h"

#define MTLK_PACK_ON
#include "mtlkpack.h"
#include "ieee80211_crc32.h" /* for IEEE80211 32-bit CRC computation */
#include "mtlk_coreui.h"

#define LOG_LOCAL_GID   GID_BEACON_MAN
#define LOG_LOCAL_FID   0

#define HT_CAP_INFO_OFFSET      2
#define HT_CAP_INFO_LEN         2
#define SMPS_BIT_OFFSET         2
#define SMPS_BIT_LEN            2
#define RX_MCS_BITMASK_OFFSET   5
#define RX_MCS_BITMASK_LEN      10
#define VHT_MCS_NSS_OFFSET      6
#define VHT_MCS_NSS_LEN         8
#define IE_HE_MCS_NSS_OFFSET    20
#define HE_MCS_NSS_160MHZ_LEN   8
#define HE_MCS_NSS_80MHZ_LEN    4
/*Individual TWT support */
#define IE_HE_MAC_TWT_RESPONDER_OFFSET           (WLAN_EID_EXTENSION_ID_OFFSET + 1) /* Extension length=1 */
#define HE_MAC_TWT_RESPONDER_SUPPORT             2
#define HE_MAC_TWT_RESPONDER_SUPPORT_WIDTH       1
#define HE_MAC_TWT_RESPONDER_NOT_SUPPORTED       0
#define HE_MAC_TWT_RESPONDER_SUPPORTED           1
/*Broadcast TWT support */
#define IE_HE_MAC_BCAST_TWT_RESPONDER_OFFSET     (WLAN_EID_EXTENSION_ID_OFFSET + 3) /* HE MAC CAP BIT20 (OCTET 2) */
#define HE_MAC_BCAST_TWT_RESPONDER_SUPPORT       4 /* BIT #4 in the Octet */
#define HE_MAC_BCAST_TWT_RESPONDER_SUPPORT_WIDTH 1
#define HE_MAC_BCAST_TWT_RESPONDER_NOT_SUPPORTED 0
#define HE_MAC_BCAST_TWT_RESPONDER_SUPPORTED     1

#define IE_HE_PHY_CHANNEL_WIDTH_SET_OFFSET (WLAN_EID_EXTENSION_ID_OFFSET + 7) /* Extension length = 1, MAC length = 6 */
#define HE_PHY_CHANNEL_WIDTH_SET_B2         3
#define IE_HE_OPER_BSS_COLOR_OFFSET (WLAN_EID_EXTENSION_ID_OFFSET + 4) /* Extension length = 1, HE operation parameter = 3 */

#define IE_ID_OFFSET            0
#define IE_LENGTH_OFFSET        1
#define IE_PAYLOAD_OCTET1       2
#define IE_PAYLOAD_OCTET2       3
#define EXT_CAP_ARP_PROXY_MASK  0x10

#define OUI_MICROSOFT           0x0050f2
#define OUI_TYPE_WPA            0x01
#define OUI_TYPE_WMM            0x02
#define OUI_TYPE_WPS            0x04
#define OUI_WFA                 0x506f9a
#define OUI_TYPE_P2P            0x09
#define OUI_TYPE_WFD            0x0a
#define OUI_TYPE_HS20           0x10
#define OUI_TYPE_OSEN           0x12

#define HS20_DGAF_DISABLED_MASK 0x01

#define BMGR_NO_VAP             -1

#define MTLK_BEACON_MAN_RCVRY_WAIT_TIME 250 /* recovery: timeout for beacon template indication from FW in ms */

/* RNR IE constants: Refer to IEEE P802.11ax/D7.0? Sec 9.4.2.170 for more details */

#define RNR_6G_TBTT_INFO_COUNT_OFFSET         2
#define RNR_6G_TBTT_INFO_COUNT_SHIFT          4
#define RNR_6G_TBTT_INFO_LEN_OFFSET           3
#define RNR_6G_SHORT_SSID_LEN                 4

#define RNR_IE_EID_SIZE                       1
#define RNR_IE_LEN_SIZE                       1
#define RNR_IE_TBTT_INFO_HDR_SIZE             2
#define RNR_IE_OP_CLASS_SIZE                  1
#define RNR_IE_CHAN_NUM_SIZE                  1

#define TBTT_NEIGH_AP_OFFSET_SIZE             1
#define TBTT_BSSID_SIZE                       6
#define TBTT_SHORT_SSID_SIZE                  4
#define TBTT_BSS_PARAMS_SIZE                  1
#define TBTT_20MHZ_PSD_SIZE                   1

#define RNR_IE_MIN_SIZE                       (RNR_IE_EID_SIZE + RNR_IE_LEN_SIZE + \
                                               RNR_IE_TBTT_INFO_HDR_SIZE + RNR_IE_OP_CLASS_SIZE + \
                                               RNR_IE_CHAN_NUM_SIZE + TBTT_NEIGH_AP_OFFSET_SIZE)

#define TIME_STAMP_OFFSET_IN_PROBE_RESP       0
#define SIZEOF_FILS_DISC_CATEGORY             1
#define SIZEOF_FILS_DISC_ACTION               1
#define SIZEOF_FILS_DISC_FRAME_CTRL           2
#define TIME_STAMP_OFFSET_IN_FILS_DISC        (sizeof(frame_head_t) + SIZEOF_FILS_DISC_CATEGORY + \
                                               SIZEOF_FILS_DISC_ACTION + SIZEOF_FILS_DISC_FRAME_CTRL)
#define FILS_DISC_FRAME_LEN                   (sizeof(struct intel_vendor_fils_discovery_info) + \
                                               sizeof(frame_action_head_t) + sizeof(frame_head_t))

int wave_beacon_man_private_init(beacon_manager_priv_t *bmgr_priv)
{
  bmgr_priv->for_vap_idx = BMGR_NO_VAP;
  mtlk_osal_event_init(&bmgr_priv->rcvry_beacon_ind_event);
  return MTLK_ERR_OK;
}

int wave_beacon_man_init(beacon_manager_t *bmgr, mtlk_vap_handle_t vap_handle)
{
  memset(bmgr, 0, sizeof(beacon_manager_t));
  bmgr->drv_idx = 1; /* zero indexed buffer virtually belongs to the FW at init */
  return MTLK_ERR_OK;
}

void wave_beacon_man_private_cleanup(beacon_manager_priv_t *bmgr_priv, void *hw)
{
  MTLK_ASSERT(NULL != hw);

  mtlk_osal_event_cleanup(&bmgr_priv->rcvry_beacon_ind_event);

  if (bmgr_priv->tmpl.dma_addr) {
    mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(hw)),
                              bmgr_priv->tmpl.dma_addr, bmgr_priv->tmpl.alloced_size, MTLK_DATA_TO_DEVICE);
    bmgr_priv->tmpl.dma_addr = 0;
  }

  if (bmgr_priv->tmpl.ptr)
    mtlk_osal_mem_free(bmgr_priv->tmpl.ptr);
}

void wave_beacon_man_cleanup(beacon_manager_t *bmgr, mtlk_vap_handle_t vap_handle)
{
  int i;

  for (i = 0; i < MTLK_BEACON_MAN_RCVRY_TMPL_NUM; i++) {
    beacon_template_t *tmpl = bmgr->tmpl + i;
    if (tmpl->dma_addr)
      mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(vap_handle))),
                                tmpl->dma_addr, tmpl->alloced_size, MTLK_DATA_TO_DEVICE);
    if (tmpl->ptr)
      mtlk_osal_mem_free(tmpl->ptr);
  }
}

static int _wave_beacon_man_template_save(beacon_template_t *tmpl, mtlk_beacon_data_t *data)
{
  unsigned new_len = data->head_len + data->tail_len;

  if (new_len > tmpl->alloced_size) { /* need to reallocate */
    if (tmpl->ptr)
      mtlk_osal_mem_free(tmpl->ptr);

    tmpl->ptr = mtlk_osal_mem_dma_alloc(new_len, MTLK_MEM_TAG_BEACON_DATA);

    if (!tmpl->ptr) {
      ELOG_D("Malloc %u bytes for beacon data failed", new_len);
      tmpl->alloced_size = 0;
      return MTLK_ERR_NO_MEM;
    }

    tmpl->alloced_size = new_len;

    ILOG2_PD("allocated beacon data for beacon template at 0x%p, len %d", tmpl,
      new_len);
  }

  wave_memcpy(tmpl->ptr, new_len, data->head, data->head_len);
  wave_memcpy(tmpl->ptr + data->head_len, new_len - data->head_len, data->tail, data->tail_len);
  tmpl->head_len = data->head_len;
  tmpl->tail_len = data->tail_len;
  return MTLK_ERR_OK;
}

static BOOL
_wave_get_beacon_data_length(beacon_template_t *tmpl, int *part1_len, int *part2_len, int *part3_len, int *part4_len, int *part5_len, int *part6_len)
{
  BOOL result = TRUE;
  int ie_id;
  int ie_len;
  uint8 *pos = tmpl->ptr + tmpl->head_len;
  uint8 *last_pos = tmpl->ptr + tmpl->head_len + tmpl->tail_len;
  uint8 *ie;

  *part1_len = tmpl->head_len;
  *part2_len = 0;
  *part3_len = 0;
  *part4_len = 0;
  *part5_len = 0;
  *part6_len = 0;

  while (pos < last_pos) {
    ie = pos;
    ie_id = *pos;
    ie_len = *(pos+1) + 2;
    pos += ie_len;

    if (pos > last_pos)
    {
      ELOG_D("Wrong IE length %d", ie_len);
      result = FALSE;
      break;
    }

    switch (ie_id)
    {
    case IE_TIM:
      ELOG_V("TIM IE must not be present in beacon template");
      FALLTHROUGH;
      /* Drop down */
    case IE_COUNTRY:
    case IE_FH_PATTRN_PARAMS:
    case IE_FH_PATTRM_TABLE:
    case IE_PWR_CONSTRAINT:
      *part2_len += ie_len;
      break;
    case IE_CHANNEL_SW_ANN:
      ELOG_V("Channel Switch Announcement IE must not be present in beacon template");
      FALLTHROUGH;
      /* Drop down */
    case IE_QUIET:
    case IE_IBSS_DFS:
    case IE_TPC_REPORT:
    case IE_ERP_INFO:
    case IE_EXT_SUPP_RATES:
    case IE_RSN:
      *part3_len += ie_len;
      break;
    case IE_BSS_LOAD:
      ELOG_V("BSS Load IE must not be present in beacon template");
      FALLTHROUGH;
      /* Drop down */
    case IE_EDCA_PARAM_SET:
    case IE_HT_CAPABILITIES:
    case IE_QOS_CAPABILITY:
    case IE_AP_CHANNEL_REPORT:
    case IE_MOBILITY_DOMAIN:
    case IE_DSE_REGISTERED_LOCATION:
    case IE_SUPPORTED_OPER_CLASSES:
    case IE_EXT_CHANNEL_SW_ANN:
    case IE_HT_INFORMATION:
    case IE_BSS_AVERAGE_ACCESS:
    case IE_ANTENNA:
    case IE_MEASURE_PILOT_TRANSMISSION:
    case IE_BSS_AVAILABLE_ADMISSION_CAPACITY:
    case IE_BSS_AC_ACCESS_DELAY:
    case IE_TIME_ADV:
    case IE_RM_ENABLED_CAPABILITIES:
    case IE_MULTIPLE_BSSID:
    case IE_COEXISTENCE:
    case IE_OBSS_SCAN_PARAMETERS:
    case IE_FMS_DESCRIPTOR:
    case IE_QOS_TRAFFIC_CAPABILITY:
    case IE_INTERWORKING:
    case IE_ADV_PROTO:
    case IE_ROAMING_CONS:
    case IE_EMERGENCY_ALERT_ID:
    case IE_MESH_CONFIG:
    case IE_MESH_ID:
    case IE_MESH_CHANNEL_SW_PARAMETERS:
    case IE_MESH_AWAKE_WINDOW:
    case IE_BEACON_TIMING:
    case IE_MCCAOP_ADV:
    case IE_EXT_CAPABILITIES:
    case IE_MULTI_BAND:
    case IE_MCCAOP_ADV_OVERVIEW:
    case IE_QMF_POLICY:
    case IE_QLOAD_REPORT:
    case IE_HCCA_TXOP_UPDATE_COUNT:
    case IE_VHT_CAPABILITIES:
    case IE_VHT_OPERATION:
    case IE_TX_PWR_ENVELOPE:
      *part4_len += ie_len;
      break;
    case IE_CHANNEL_SW_WRAPPER:
      ELOG_V("Channel Switch Wrapper IE must not be present in beacon template");
      FALLTHROUGH;
      /* Drop down */
    case IE_REDUCED_NEIGH_REPORT:
    case IE_RSNXE:
      *part5_len += ie_len;
      break;
    case IE_TWT:
      ELOG_V("TWT IE must not be present in beacon template");
      *part6_len += ie_len;
      break;
    case IE_EXTENSION:
      if ((ie[WLAN_EID_EXTENSION_ID_OFFSET] == WLAN_EID_HE_CAP_EXT) ||
          (ie[WLAN_EID_EXTENSION_ID_OFFSET] == WLAN_EID_HE_OPER_EXT))
        *part5_len += ie_len;
      else
        *part6_len += ie_len;
      break;
    default:
      *part6_len += ie_len;
      break;
    }
  }

  if (result == TRUE) {
    if ((tmpl->head_len + tmpl->tail_len) != (*part1_len + *part2_len + *part3_len + *part4_len + *part5_len + *part6_len)) {
      ELOG_DDDDDDDD("Beacon template lengths mismatch: (%d + %d) is not equal (%d + %d + %d + %d + %d + %d)",
                   tmpl->head_len, tmpl->tail_len, *part1_len, *part2_len, *part3_len, *part4_len, *part5_len, *part6_len);
      result = FALSE;
    }
  }

  return result;
}

static int _wave_beacon_man_template_push(mtlk_core_t *core, beacon_template_t *tmpl)
{
  int               err = MTLK_ERR_OK;
  UMI_BEACON_SET    *psUmiBeacon;
  mtlk_txmm_data_t  *man_entry;
  mtlk_txmm_msg_t   man_msg;
  int               part1_len;
  int               part2_len;
  int               part3_len;
  int               part4_len;
  int               part5_len;
  int               part6_len;
  uint16            oid;
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_DP("CID-%04x: data 0x%p", oid, tmpl);

  if (NULL == tmpl->ptr) {
    ELOG_D("CID-%04x: tmpl->ptr is NULL", oid);
    return MTLK_ERR_PARAMS;
  }

  /* mtlk_dump(2, tmpl->ptr, tmpl->head_len + tmpl->tail_len, "dump of beacon template:"); */

  if (FALSE == _wave_get_beacon_data_length(tmpl, &part1_len, &part2_len, &part3_len, &part4_len, &part5_len, &part6_len)) {
    ELOG_D("CID-%04x: Failed to parse template parts", oid);
    return MTLK_ERR_PARAMS;
  }

  /* prepare for DMA transfer */
  if (0 != tmpl->dma_addr) { /* should always be zero, just additional check */
    ELOG_D("CID-%04x: dma_addr still mapped", oid);
    return MTLK_ERR_UNKNOWN;
  }

  tmpl->dma_addr = mtlk_osal_map_to_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                                              tmpl->ptr, tmpl->head_len + tmpl->tail_len, MTLK_DATA_TO_DEVICE);
  if (0 == tmpl->dma_addr) {
    ELOG_D("CID-%04x: Mem map for beacon data failed", oid);
    return MTLK_ERR_NO_MEM;
  }

  /* prepare msg to the FW */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &err);

  if (NULL == man_entry) {
    ELOG_DD("CID-%04x: UM_MAN_SET_BEACON_TEMPLATE_REQ init failed, err=%i", oid, err);
    /* DMA unmap */
    mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                              tmpl->dma_addr, tmpl->alloced_size, MTLK_DATA_TO_DEVICE);
    tmpl->dma_addr = 0;
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_BEACON_TEMPLATE_REQ;
  man_entry->payload_size = sizeof(UMI_BEACON_SET);
  psUmiBeacon = (UMI_BEACON_SET *) man_entry->payload;

  psUmiBeacon->u32hostAddress = HOST_TO_MAC32(tmpl->dma_addr);
  psUmiBeacon->u16part1Len    = HOST_TO_MAC16(part1_len);
  psUmiBeacon->u16part2Len    = HOST_TO_MAC16(part2_len);
  psUmiBeacon->u16part3Len    = HOST_TO_MAC16(part3_len);
  psUmiBeacon->u16part4Len    = HOST_TO_MAC16(part4_len);
  if (hw_get_btwt_support(HANDLE_T(hw))) {
    psUmiBeacon->u16part5Len  = HOST_TO_MAC16(part5_len);
    psUmiBeacon->u16part6Len  = HOST_TO_MAC16(part6_len);
  } else {
    psUmiBeacon->u16part5Len  = HOST_TO_MAC16(part5_len + part6_len);
    psUmiBeacon->u16part6Len  = 0;
  }
  psUmiBeacon->u8vapIndex     = mtlk_vap_get_id(core->vap_handle);
  psUmiBeacon->addBssLoadIe   = mtlk_df_user_get_hs20_status(mtlk_vap_get_df(core->vap_handle)) ? 1 : 0;
  psUmiBeacon->addBssLoadIe  |= MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_IS_BSS_LOAD_ENABLE) ? 1 : 0;
  ILOG1_D("Set Beacon: u16part6Len = %u",  MAC_TO_HOST16(psUmiBeacon->u16part6Len));
  mtlk_dump(2, psUmiBeacon, sizeof(UMI_BEACON_SET), "dump of UMI_BEACON_SET:");

  err = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (err != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: UM_MAN_BEACON_SET_REQ send failed, err=%i", oid, err);
  } else if (psUmiBeacon->u8Status != UMI_OK) {
    ELOG_DD("CID-%04x: UM_MAN_BEACON_SET_REQ failed, Status=%u", oid, psUmiBeacon->u8Status);
    err = MTLK_ERR_MAC;
  }
  mtlk_txmm_msg_cleanup(&man_msg);
  return err;
}


static __INLINE void
__wave_beacon_man_template_push_wrap(mtlk_core_t *core, beacon_template_t *tmpl,
                              beacon_manager_t *bmgr, beacon_manager_priv_t *bmgr_priv)
{
  if (MTLK_ERR_OK == _wave_beacon_man_template_push(core, tmpl)) {
    bmgr->updated = FALSE;
    bmgr->switch_pending = TRUE;
    bmgr_priv->fw_is_busy = TRUE;
  }
}

typedef struct _hs20_ie_t
{
  uint8 id;
  uint8 len;
  uint8 oui[3];
  uint8 type;
  uint8 hs_conf;
} __MTLK_PACKED hs20_ie_t;

static BOOL
_wave_core_hs20_get_dgaf_disabled(hs20_ie_t *hs20_ie, uint32 ie_len)
{
  /* we need to check on the size plus 2, 2 is id and len */
  MTLK_ASSERT(ie_len >= (sizeof(*hs20_ie) - sizeof(hs20_ie->id) - sizeof(hs20_ie->len)));
  return ((ie_len >= (sizeof(*hs20_ie) - sizeof(hs20_ie->id) - sizeof(hs20_ie->len))) && (!!(hs20_ie->hs_conf & HS20_DGAF_DISABLED_MASK)));
}

static int
_wave_probe_resp_template_gen_non_he (mtlk_core_t *core, uint8 *buf,
                                       size_t len)
{
  u8 *probe_resp_templ_non_he, *ies_data, *non_he_buf;
  ie_t *ie;
  size_t ies_len, ies_data_offs;
  size_t probe_resp_templ_non_he_len = 0;

  if (!buf || !len) {
    ELOG_V("Template buffer is empty");
    return MTLK_ERR_PARAMS;
  }
  /* Move frame buffer by frame head and fixed parameters */
  ies_data_offs = sizeof(frame_head_t) + sizeof(frame_beacon_head_t);
  ies_data = buf + ies_data_offs;
  ies_len = len - ies_data_offs;

  /* Allocate the same memory for non HE template as for normal template
     Truncate length based on the calculation from IEs added to the buffer
  */
  probe_resp_templ_non_he = mtlk_osal_mem_alloc(len, MTLK_MEM_TAG_BEACON_MNGR);
  if (!probe_resp_templ_non_he) {
    ELOG_D("Cannot allocate %hu bytes for a new Probe Response template", len);
    return MTLK_ERR_NO_MEM;
  }

  /* Copy frame header and fixed param into the non HE template */
  wave_memcpy(probe_resp_templ_non_he, len, buf, ies_data_offs);
  non_he_buf = probe_resp_templ_non_he + ies_data_offs;
  probe_resp_templ_non_he_len += ies_data_offs;

  ILOG4_DD("From Data: IEs Data Offs = %d IEs Data Length = %d",
         ies_data_offs, ies_len);

  /* Add IEs in the Probe Response template */
  while (ies_len > sizeof(ie_t))
  {
    size_t ie_len;
    ie = (ie_t *)ies_data;
    ie_len = ie->length + sizeof(ie_t);
    if (ie_len > ies_len) {
      ies_len = 0;
      break;
    }
    /* Copy all IEs upto VENDOR IE into non HE template */
    if (ie->id <= IE_VENDOR_SPECIFIC) {
      wave_memcpy(non_he_buf, ies_len, ie, ie_len);
      non_he_buf += ie_len;
      probe_resp_templ_non_he_len += ie_len;
    }
    ies_data += ie_len;
    ies_len -= ie_len;
  }

  /* Update parameters in core context that will be used in offload probe
     response Tx
  */
  core->slow_ctx->probe_resp_templ_non_he = probe_resp_templ_non_he;
  core->slow_ctx->probe_resp_templ_non_he_len = probe_resp_templ_non_he_len;

  ILOG4_D("Non HE Probe Templ Len = %d", probe_resp_templ_non_he_len);
  mtlk_dump(4, buf, len, "Probe Response Template");
  mtlk_dump(4, probe_resp_templ_non_he, probe_resp_templ_non_he_len, "Probe Response Template Non-HE");

  return MTLK_ERR_OK;
}

int
wave_beacon_template_get_beacon_data(struct sk_buff *beacon, struct sk_buff *probe_resp,
    struct cfg80211_beacon_data *beacon_data, struct ieee80211_mutable_offsets *offs)
{
  /************************Beacon Frame*********************************
   ** head                                                           ***
   *** V                                                             ***
   *** V                                                             ***
   *** | fc | duration | DA | SA | BSS ID | seq-ctl |                ***
   *** | 2  |    2     | 6  | 6  |    6   |    2    |                ***
   ***                                                               ***
   *** | timestamp | beacon_int | cap_info |  ssid  |                ***
   *** |     8     |      2     |     2    |   X    |                ***
   ***                        head+tim_off     tail                  ***
   ***                         (head end)       V                    ***
   ***                              V           V                    ***
   ***                              V<-tim_len->V                    ***
   *** | rates | DS | CF | IBSS set |    TIM    | country_info | ... ***
   *** |   X   | 2  | 8  |     4    |     X     |       X      | ... ***
   *********************************************************************/

  unsigned beacon_len, tail_offs;

  if (!beacon || !probe_resp || !beacon_data || !offs)
    return MTLK_ERR_PARAMS;

  if (skb_is_nonlinear(beacon) || skb_is_nonlinear(probe_resp)) {
    ELOG_V("Nonlinear skb of Beacon template and/or Probe response");
    return MTLK_ERR_PARAMS;
  }

  tail_offs = offs->tim_offset + offs->tim_length;
  beacon_len = skb_headlen(beacon);
  if (beacon_len < tail_offs) {
    ELOG_DD("Wrong beacon template len (%u < %u)", beacon_len, tail_offs);
    return MTLK_ERR_PARAMS;
  }

  beacon_data->head           = (u8*)beacon->data;
  beacon_data->head_len       = offs->tim_offset;
  beacon_data->tail           = beacon_data->head + tail_offs;
  beacon_data->tail_len       = beacon_len - tail_offs;
  beacon_data->probe_resp     = (u8*)probe_resp->data;
  beacon_data->probe_resp_len = skb_headlen(probe_resp);

  return MTLK_ERR_OK;
}

static bool
_wave_beacon_is_short_ssid_present_in_rnr_ie(u8 tbtt_info_len, u8 *short_ssid_offset)
{
    bool res = true;
    *short_ssid_offset = 0;

    switch(tbtt_info_len) {
      case (TBTT_NEIGH_AP_OFFSET_SIZE + TBTT_SHORT_SSID_SIZE):
      case (TBTT_NEIGH_AP_OFFSET_SIZE + TBTT_SHORT_SSID_SIZE +
            TBTT_BSS_PARAMS_SIZE):
        *short_ssid_offset = RNR_IE_MIN_SIZE;
        break;
      case (TBTT_NEIGH_AP_OFFSET_SIZE + TBTT_BSSID_SIZE +
            TBTT_SHORT_SSID_SIZE):
      case (TBTT_NEIGH_AP_OFFSET_SIZE + TBTT_BSSID_SIZE +
            TBTT_SHORT_SSID_SIZE + TBTT_BSS_PARAMS_SIZE):
      case (TBTT_NEIGH_AP_OFFSET_SIZE + TBTT_BSSID_SIZE +
            TBTT_SHORT_SSID_SIZE + TBTT_BSS_PARAMS_SIZE +
            TBTT_20MHZ_PSD_SIZE):
        *short_ssid_offset = (RNR_IE_MIN_SIZE + TBTT_BSSID_SIZE);
         break;
      default:
         ILOG1_D("No Short SSID found in the RNR IE, tbtt info len = %d", tbtt_info_len);
         res = false;
         break;
    }
    return res;
}

static void
_wave_beacon_template_parsing_ies(mtlk_core_t *core, mtlk_beacon_data_t *beacon_data)
{
  mtlk_pdb_t *param_db_core = mtlk_vap_get_param_db(core->vap_handle);

  /* 6GHz band support. Helps to retrieve the band of operation */
  wave_radio_t *radio;
  mtlk_hw_band_e band;
  u8 set_bss_flags = 0;
  const u8 *vht_cap_ie;

  mtlk_dump(2, beacon_data->head, beacon_data->head_len, "dump of beacon template head");
  mtlk_dump(2, beacon_data->tail, beacon_data->tail_len, "dump of beacon template tail");

  radio = wave_vap_radio_get(core->vap_handle);
  band = wave_radio_band_get(radio);

  /* HT Capabilities IE */
  {
    const u8 *ht_cap_ie = cfg80211_find_ie(IE_HT_CAPABILITIES, beacon_data->tail, beacon_data->tail_len);
    if (ht_cap_ie)
    {
      uint16 cap_info = 0;
      uint32 smps;

      MTLK_STATIC_ASSERT(sizeof(cap_info) == HT_CAP_INFO_LEN);

      wave_memcpy(&cap_info, sizeof(cap_info), ht_cap_ie + HT_CAP_INFO_OFFSET, HT_CAP_INFO_LEN);
      smps = MTLK_U_BITS_GET(WLAN_TO_HOST16(cap_info), SMPS_BIT_OFFSET, SMPS_BIT_LEN);
      set_bss_flags |= MTLK_BFIELD_VALUE(VAP_ADD_FLAGS_HT, 1, uint8);
      set_bss_flags |= MTLK_BFIELD_VALUE(VAP_ADD_FLAGS_SMPS, smps, uint8);

      wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_RX_MCS_BITMASK, ht_cap_ie + RX_MCS_BITMASK_OFFSET, RX_MCS_BITMASK_LEN);
    }
  }

  /* Extended Capabilities IE */
  {
    const u8 *ext_cap_ie = cfg80211_find_ie(IE_EXT_CAPABILITIES, beacon_data->tail, beacon_data->tail_len);
    if (ext_cap_ie)
    {
      int arp_proxy_enabled = 0;

      if (ext_cap_ie[IE_LENGTH_OFFSET] >= 2) {
        arp_proxy_enabled = ext_cap_ie[IE_PAYLOAD_OCTET2] & EXT_CAP_ARP_PROXY_MASK;
      }

      ILOG1_DS("CID-%04x: ARP Proxy %s", mtlk_vap_get_oid(core->vap_handle), arp_proxy_enabled ? "enabled" : "disabled");
      wave_pdb_set_int(param_db_core, PARAM_DB_CORE_ARP_PROXY, arp_proxy_enabled ? 1 : 0);
    }
  }

  /* VHT Capabilities IE */
  {
    vht_cap_ie = cfg80211_find_ie(IE_VHT_CAPABILITIES, beacon_data->tail, beacon_data->tail_len);
    if (vht_cap_ie)
    {
      set_bss_flags |= MTLK_BFIELD_VALUE(VAP_ADD_FLAGS_VHT, 1, uint8);
      wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_VHT_MCS_NSS, vht_cap_ie + VHT_MCS_NSS_OFFSET, VHT_MCS_NSS_LEN);
    }
  }

  /* HE Capabilities IE */
  {
    /* Find first occurrence of IE_EXTENSION out of three for each,
                                        capabilities, operation and EDCA elements. */
    const u8 *he_cap_ie = cfg80211_find_ie(IE_EXTENSION, beacon_data->tail, beacon_data->tail_len);
    const u8 *he_oper_ie;

    if (he_cap_ie)
    {
      if (WLAN_EID_HE_CAP_EXT == he_cap_ie[WLAN_EID_EXTENSION_ID_OFFSET]) {
        set_bss_flags |= MTLK_BFIELD_VALUE(VAP_ADD_FLAGS_HE, 1, uint8);
        /* Check the B2 of the Channel Width Set, size of NSS MCS depends on it */
        if(MTLK_U_BITS_GET(he_cap_ie[IE_HE_PHY_CHANNEL_WIDTH_SET_OFFSET], HE_PHY_CHANNEL_WIDTH_SET_B2, 1)) {
          wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_HE_MCS_NSS, he_cap_ie + IE_HE_MCS_NSS_OFFSET, HE_MCS_NSS_160MHZ_LEN);
          mtlk_dump(1, &he_cap_ie[IE_HE_MCS_NSS_OFFSET], HE_MCS_NSS_160MHZ_LEN, "dump of 160MHz HE_MCS_NSS:");
        } else {
          wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_HE_MCS_NSS, he_cap_ie + IE_HE_MCS_NSS_OFFSET, HE_MCS_NSS_80MHZ_LEN);
          mtlk_dump(1, &he_cap_ie[IE_HE_MCS_NSS_OFFSET], HE_MCS_NSS_80MHZ_LEN, "dump of HE_MCS_NSS:");
        }

        if(MTLK_U_BITS_GET(he_cap_ie[IE_HE_MAC_TWT_RESPONDER_OFFSET], HE_MAC_TWT_RESPONDER_SUPPORT, HE_MAC_TWT_RESPONDER_SUPPORT_WIDTH)) {
            wave_pdb_set_int(param_db_core, PARAM_DB_CORE_TWT_OPERATION_MODE, HE_MAC_TWT_RESPONDER_SUPPORTED);
            ILOG2_D("CID-%04x: PARAM_DB_CORE_TWT_OPERATION_MODE = 1", mtlk_vap_get_oid(core->vap_handle));
        }

	/* Broadcast TWT support stored in the PDB core */
        if(MTLK_U_BITS_GET(he_cap_ie[IE_HE_MAC_BCAST_TWT_RESPONDER_OFFSET], HE_MAC_BCAST_TWT_RESPONDER_SUPPORT, 
                           HE_MAC_BCAST_TWT_RESPONDER_SUPPORT_WIDTH)) {
            wave_pdb_set_int(param_db_core, PARAM_DB_CORE_BCAST_TWT_OPERATION_MODE, HE_MAC_BCAST_TWT_RESPONDER_SUPPORTED);
            ILOG2_D("CID-%04x: PARAM_DB_CORE_BCAST_TWT_OPERATION_MODE = 1", mtlk_vap_get_oid(core->vap_handle));
        }

      } else {
        ELOG_DDD("Extension ID %d Offset %d Expected %d", he_cap_ie[WLAN_EID_EXTENSION_ID_OFFSET], WLAN_EID_EXTENSION_ID_OFFSET, WLAN_EID_HE_CAP_EXT);
      }
      he_oper_ie = he_cap_ie +  he_cap_ie[IE_LENGTH_OFFSET] + WLAN_EID_EXTENSION_ID_OFFSET;
      if (he_oper_ie) {
        if (WLAN_EID_HE_OPER_EXT == he_oper_ie[WLAN_EID_EXTENSION_ID_OFFSET]) {
          struct intel_vendor_he_oper he_operation = {0};
          mtlk_pdb_size_t he_operation_len = sizeof(he_operation);
          wave_pdb_get_binary(param_db_core, PARAM_DB_CORE_HE_OPERATION, &he_operation, &he_operation_len);
          he_operation.he_bss_color_info = he_oper_ie[IE_HE_OPER_BSS_COLOR_OFFSET];
          wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_HE_OPERATION, &he_operation, sizeof(he_operation));
        } else {
          ELOG_DDD("Extension ID %d Offset %d Expected %d", he_oper_ie[WLAN_EID_EXTENSION_ID_OFFSET], WLAN_EID_EXTENSION_ID_OFFSET, WLAN_EID_HE_OPER_EXT);
        }
      }
    }
  }

  /* Vendor IEs */
  {
    const u8 *vendor_ie = cfg80211_find_vendor_ie(OUI_WFA, OUI_TYPE_HS20, beacon_data->tail, beacon_data->tail_len);
    if (vendor_ie)
    {
      ILOG1_V("HS20 enabled");
      mtlk_df_user_set_hs20_status(mtlk_vap_get_df(core->vap_handle), TRUE);
      core->dgaf_disabled = _wave_core_hs20_get_dgaf_disabled((hs20_ie_t *) vendor_ie, vendor_ie[1]);
      ILOG1_DS("CID-%04x: dgaf_disabled: %s", mtlk_vap_get_oid(core->vap_handle), core->dgaf_disabled ? "true" : "false");
    }

#ifdef WAVE_DCDP_DGAF_SUPPORTED
    if (core->dgaf_disabled != core->dgaf_disabled_update) {
      /* Update DC DP DGAF flag accordingly with core->dgaf_disabled */
      mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
      mtlk_df_ui_dp_dgaf_disable(df, core->dgaf_disabled);
      core->dgaf_disabled_update = core->dgaf_disabled;
    }
#endif
  }

  wave_pdb_set_int(param_db_core, PARAM_DB_CORE_SET_BSS_FLAGS, set_bss_flags);

  /* Update network mode by supported rates IEs */
  {
      bss_data_t     bss_data;
      ie_t          *ie;
      const u8      *ies_data;
      int            ies_len;

      memset(&bss_data, 0, sizeof(bss_data));

      /* Supported rates */
      ies_data = beacon_data->head + sizeof(frame_head_t) + sizeof(frame_beacon_head_t);
      ies_len = beacon_data->head_len - sizeof(frame_head_t) - sizeof(frame_beacon_head_t);
      MTLK_ASSERT(ies_len > 0);

      /* SSID parameter set*/
      ie = (ie_t *)cfg80211_find_ie(IE_SSID, ies_data, ies_len);
      if (ie)
        mtlk_core_cfg_set_ssid(core, GET_IE_DATA_PTR(ie), ie->length);

      ie = (ie_t *)cfg80211_find_ie(IE_SUPPORTED_RATES, ies_data, ies_len);
      if (ie)
          mtlk_frame_ie_extract_phy_rates(GET_IE_DATA_PTR(ie), ie->length, &bss_data);

      /* Extended Supported rates */
      ie = (ie_t *)cfg80211_find_ie(IE_EXT_SUPP_RATES, beacon_data->tail, beacon_data->tail_len);
      if (ie)
          mtlk_frame_ie_extract_phy_rates(GET_IE_DATA_PTR(ie), ie->length, &bss_data);

     if (band == MTLK_HW_BAND_6_GHZ) {
         mtlk_core_update_network_mode(core, MTLK_NETWORK_11AX_6G_ONLY);
     }   else {
         mtlk_core_update_network_mode(core,
             mtlk_frame_oper_rateset_to_net_mode(bss_data.operational_rate_set, band,
                 MTLK_BFIELD_GET(set_bss_flags, VAP_ADD_FLAGS_HT),
                 MTLK_BFIELD_GET(set_bss_flags, VAP_ADD_FLAGS_VHT),
                 MTLK_BFIELD_GET(set_bss_flags, VAP_ADD_FLAGS_HE)));
     }
  }

  /* Short SSID List extraction from 6G RNR IE and storing in the PDB core in the case of 2.4/5GHz interface */
  {
      if (band != MTLK_HW_BAND_6_GHZ) {
        mtlk_error_t res;
        u8 count, num_of_vaps = 0;
        u8 *short_ssid_list;
        mtlk_pdb_size_t list_size;
        u8 tbtt_info_len = 0;
        u8 first_short_ssid_offset = 0;

        /* Retrieve the RNR IE if present in the template */
        const u8 *rnr_ie = cfg80211_find_ie(IE_REDUCED_NEIGH_REPORT, beacon_data->tail, beacon_data->tail_len);
        wave_pdb_set_int(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT, 0); /* Reset to 0 by default */
        if (!rnr_ie) {
          ILOG1_DD("RNR IE id : %d not found in the Beacon template for band = %d",IE_REDUCED_NEIGH_REPORT, band);
          return;
         }

        tbtt_info_len = rnr_ie[RNR_6G_TBTT_INFO_LEN_OFFSET];
        if (!_wave_beacon_is_short_ssid_present_in_rnr_ie(tbtt_info_len, &first_short_ssid_offset))
          return;

      /* Retrieve the number of VAPs included in the RNR IE. (Count - 1) is reported in the IE */
      num_of_vaps = ((rnr_ie[RNR_6G_TBTT_INFO_COUNT_OFFSET] >> RNR_6G_TBTT_INFO_COUNT_SHIFT ) + 1);
      /* Allocate a local buffer to store the list of short SSIDs */
      list_size = (num_of_vaps * RNR_6G_SHORT_SSID_LEN);
      short_ssid_list = mtlk_osal_mem_alloc(list_size , MTLK_MEM_TAG_BEACON_MNGR);
      if (!short_ssid_list) {
        ELOG_DD("Cannot allocate %u bytes for copying the short SSID list for band = %d",list_size, band);
        return;
      }

      /* Copy the short SSIDs (in Little Endian format) into the local buffer.    */
      {
        const u8 *ptr_src = NULL;
        u8 *ptr_dst = NULL;
        u8 src_idx = first_short_ssid_offset;
        u8 dst_idx = 0;
        for (count = 0; count < num_of_vaps; count++) {
          ptr_src = &rnr_ie[src_idx];
          ptr_dst = &short_ssid_list[dst_idx];
          wave_memcpy(ptr_dst, RNR_6G_SHORT_SSID_LEN, ptr_src, RNR_6G_SHORT_SSID_LEN);
          src_idx += tbtt_info_len;
          dst_idx += RNR_6G_SHORT_SSID_LEN;
        }
      }

      /* Copy the local buffer containing the Short SSID list into the PDB */
      res = wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_LIST, short_ssid_list, list_size);

      if(res == MTLK_ERR_OK) {
        /* Save the Short SSID count to the PDB. Necessary later for retrieving the short SSID list from the PDB */
        wave_pdb_set_int(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT, num_of_vaps);
        ILOG1_DDDD("RNR IE id %d received for band = %d. TBTT Info Len = %d, Short SSID count = %d",rnr_ie[0], band, tbtt_info_len, num_of_vaps);
      } else {
        ELOG_D("Failed to set the Short SSID List in PDB for band = %d",band);
      }

      /* Release the short_ssid_list local buffer */
      mtlk_osal_mem_free(short_ssid_list);
     }
  }
}

static int _wave_beacon_man_push_ctrl(mtlk_core_t *core,
                                      beacon_manager_t      *bmgr,
                                      beacon_manager_priv_t *bmgr_priv,
                                      mtlk_beacon_data_t    *beacon_data,
                                      unsigned vap_idx)
{
  beacon_template_t *tmpl;
  int res;

  /* if both buffers belong to fw in current VAP */
  if (bmgr->switch_pending) {
    ILOG2_V("switch pending, saving tmpl to emergency buffer");
    res = _wave_beacon_man_template_save(&bmgr_priv->tmpl, beacon_data);
    if (MTLK_ERR_OK == res)
      bmgr_priv->for_vap_idx = vap_idx;
    return res;
  }

  /* driver owns free buffer */
  tmpl = &bmgr->tmpl[bmgr->drv_idx];
  res = _wave_beacon_man_template_save(tmpl, beacon_data);
  if (MTLK_ERR_OK != res)
    return res;

  bmgr->updated = TRUE;

  if (!bmgr_priv->fw_is_busy) {
    __wave_beacon_man_template_push_wrap(core, tmpl, bmgr, bmgr_priv);
  }

  return MTLK_ERR_OK;
}

/* Unsolicited broadcast frame transmission support */
void wave_unsolicited_frame_templ_unmap_free(mtlk_core_t *core, unsolicit_tx_frame_info_t *ptr, uint8 index)
{
  uint8 *tmp = ptr->templ[index];
  if (tmp) {
    MTLK_ASSERT(ptr->dma_addr_hist[index]);
    mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                              ptr->dma_addr_hist[index],
                              ptr->len[index], MTLK_DATA_TO_DEVICE);
    mtlk_osal_mem_free(tmp);
    ILOG1_DPDD("Unsol tx buffer %d released: addr %p, dma addr 0x%x, len = %d",
                index, tmp, ptr->dma_addr_hist[index], ptr->len[index]);
    ptr->templ[index] = NULL;
    ptr->len[index] = 0;
    ptr->dma_addr_hist[index] = 0;
  }
}

mtlk_error_t _wave_unsolicited_frame_template_push(mtlk_core_t *core, u8 const *data, uint16 len, uint16 interval,
                                                   uint8 frame_type, uint8 time_stamp_offset, bool enable)
{
  UMI_UNSOLICITED_SET       *psUmiUnsolicitedFrame;
  mtlk_txmm_data_t          *man_entry;
  mtlk_txmm_msg_t           man_msg;
  mtlk_error_t              err = MTLK_ERR_OK;
  frame_head_t              *tmp = NULL;
  uint16                    oid = mtlk_vap_get_oid(core->vap_handle);
  uint8                     *templ = NULL;
  uint32                    dma_addr = 0;
  uint8                     buf_index = 0;
  uint8                     free_index = 0;
  wave_radio_t              *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_pdb_t                *radio_param_db = wave_radio_param_db_get(radio);
  mtlk_error_t              pdb_ret;
  unsolicit_tx_frame_info_t frame_info = {0, {NULL, NULL}, {0, 0}, {0, 0}};
  unsolicit_tx_frame_info_t *p_frame_info = &frame_info;
  mtlk_pdb_size_t           size = sizeof(unsolicit_tx_frame_info_t);

  pdb_ret = wave_pdb_get_binary(radio_param_db, PARAM_DB_RADIO_UNSOLICIT_TX_FRAME_INFO, (void *)p_frame_info, &size);
  if (pdb_ret != MTLK_ERR_OK) {
    ELOG_D("Unable to retrieve unsolicited tx frame info from Param DB! res = %d", pdb_ret);
    return MTLK_ERR_UNKNOWN;
  }
  buf_index = p_frame_info->buf_index;
  if (data && len) {
    templ = mtlk_osal_mem_dma_alloc(len, MTLK_MEM_TAG_BEACON_DATA);
    if (!templ) {
      ELOG_D("Malloc %u bytes for unsolicited data failed", len);
      return MTLK_ERR_NO_MEM;
    }
    MTLK_ASSERT(buf_index < MAX_NUM_UNSOLICIT_TX_BUFS);
    if (p_frame_info->templ[buf_index] || p_frame_info->len[buf_index] || p_frame_info->dma_addr_hist[buf_index]) {
      ELOG_V("Unsolicited Tx frame buffer info incorrect! Unexpected error...");
      mtlk_osal_mem_free(templ);
      return MTLK_ERR_UNKNOWN;
    } else {
      MTLK_ASSERT(len > sizeof(frame_head_t));
      dma_addr = mtlk_osal_map_to_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                                            templ, len, MTLK_DATA_TO_DEVICE);
      if (!dma_addr) {
        mtlk_osal_mem_free(templ);
        ELOG_D("CID-%04x: Mem map for unsolicited tx frame failed", oid);
        return MTLK_ERR_NO_MEM;
      }
      p_frame_info->templ[buf_index] = templ;
      p_frame_info->len[buf_index] = len;
      p_frame_info->dma_addr_hist[buf_index] = dma_addr;
      ILOG1_DPDD("Unsol tx buffer %d allocated: addr %p, dma addr 0x%x, len %d", buf_index, templ, dma_addr, len);
      wave_memcpy(templ, len, data, len);
      tmp = (frame_head_t *)templ;
      memset(tmp->dst_addr.au8Addr, 0xFF, IEEE_ADDR_LEN);
    }
  }
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &err);
  if (NULL == man_entry) {
    ELOG_DD("CID-%04x: UM_MAN_SET_UNSOLICITED_REQ init failed, err=%i", oid, err);
    wave_unsolicited_frame_templ_unmap_free(core, p_frame_info, buf_index);
    return MTLK_ERR_NO_RESOURCES;
  }
  man_entry->id = UM_MAN_SET_UNSOLICITED_REQ;
  man_entry->payload_size = sizeof(UMI_UNSOLICITED_SET);
  psUmiUnsolicitedFrame = (UMI_UNSOLICITED_SET *)man_entry->payload;

  psUmiUnsolicitedFrame->hostBufferAddress = HOST_TO_MAC32(dma_addr);
  psUmiUnsolicitedFrame->hostBufferSize = HOST_TO_MAC16(len);
  psUmiUnsolicitedFrame->txIntervalInTU = HOST_TO_MAC16(interval);
  psUmiUnsolicitedFrame->filsTimestampOffset = time_stamp_offset;
  psUmiUnsolicitedFrame->isEnabled = enable;
  psUmiUnsolicitedFrame->frameType = frame_type;

  ILOG1_DDDDDD("UM_MAN_SET_UNSOLICITED_REQ: DMA addr: 0x%x, Size: %d, Offset: %d, Int: %d, Enable: %d, Frame: %d",
               psUmiUnsolicitedFrame->hostBufferAddress, psUmiUnsolicitedFrame->hostBufferSize,
               psUmiUnsolicitedFrame->filsTimestampOffset, psUmiUnsolicitedFrame->txIntervalInTU,
               psUmiUnsolicitedFrame->isEnabled, psUmiUnsolicitedFrame->frameType);
  err = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  oid = mtlk_vap_get_oid(core->vap_handle);
  if (err != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: UM_MAN_SET_UNSOLICITED_REQ send failed, err=%i", oid, err);
  } else if (psUmiUnsolicitedFrame->Status != UMI_OK) {
    ELOG_DD("CID-%04x: UM_MAN_SET_UNSOLICITED_REQ failed, Status=%u", oid, psUmiUnsolicitedFrame->Status);
    err = MTLK_ERR_MAC;
  }
  /* Unmap and free a buffer, update the buffer index and store in PDB Radio*/
  free_index = (err == MTLK_ERR_OK) ? GET_OTHER_UNSOLICIT_TX_BUF_INDEX(buf_index) : buf_index;
  wave_unsolicited_frame_templ_unmap_free(core, p_frame_info, free_index);
  p_frame_info->buf_index = free_index;
  pdb_ret = wave_pdb_set_binary(radio_param_db, PARAM_DB_RADIO_UNSOLICIT_TX_FRAME_INFO, (const void *)p_frame_info,
                                sizeof(unsolicit_tx_frame_info_t));
  mtlk_txmm_msg_cleanup(&man_msg);
  if (pdb_ret != MTLK_ERR_OK) {
      ELOG_D("Failed to store the unsolicit tx frame info in Param DB: res = %d", pdb_ret);
      return pdb_ret;
  }
  return err;
}

mtlk_error_t _wave_unsolicited_frame_template_push_wrap( mtlk_core_t *core)
{
  unsigned               len = 0;
  u8                     *buf = NULL;
  u8                     *probe_resp_templ_loc = NULL;
  uint8                  frame_type = PROBE_RES_FRAME;
  uint8                  time_stamp_offset = 0;
  uint16                 tx_interval = 0;
  bool                   enable = true;
  bool                   send_umi_msg = true;
  mtlk_error_t           res = MTLK_ERR_OK;
  uint8                  mode = WAVE_UNSOLICITED_FRAME_CFG_INVALID;
  uint8                  duration = WAVE_UNSOLICITED_FRAME_CFG_INVALID;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_hw_band_e         band = wave_radio_band_get(radio);
  mtlk_pdb_t             *radio_param_db = wave_radio_param_db_get(radio);
  u8                     fils_discovery_frame[FILS_DISC_FRAME_LEN];

  if (MTLK_HW_BAND_6_GHZ != band) {
    ILOG1_D("UM_MAN_SET_UNSOLICITED_REQ is not supported for band = %d", band);
    return res;
  }
  MTLK_ASSERT(NULL != radio_param_db);
  mode = wave_pdb_get_int(radio_param_db,
                          PARAM_DB_RADIO_UNSOLICIT_FRAME_CFG_MODE);
  duration = wave_pdb_get_int(radio_param_db,
                              PARAM_DB_RADIO_UNSOLICIT_FRAME_CFG_DURATION);
  if (mode == WAVE_UNSOLICITED_FRAME_CFG_MODE_PROBE_RESP) {
    time_stamp_offset = TIME_STAMP_OFFSET_IN_PROBE_RESP;
    len = wave_pdb_get_int(radio_param_db,
                           PARAM_DB_RADIO_UNSOLICIT_PROBE_RESP_TEMPL_LEN);
    if (len)
      probe_resp_templ_loc = mtlk_osal_mem_alloc(len, MTLK_MEM_TAG_BEACON_DATA);

    if (!probe_resp_templ_loc || !len) {
      ILOG0_V("No Probe Response template or buffer allocation failed!");
      send_umi_msg = false;
    } else {
      wave_pdb_get_binary(radio_param_db,
                          PARAM_DB_RADIO_UNSOLICIT_PROBE_RESP_TEMPL,
                          (void *)probe_resp_templ_loc, &len);
      buf = probe_resp_templ_loc;
      ILOG2_PD("Probe Response template: addr 0x%p, len  %d", buf, len);
    }
  } else if (mode == WAVE_UNSOLICITED_FRAME_CFG_MODE_FILS_DISC) {
    mtlk_pdb_size_t fils_discovery_frame_len = sizeof(fils_discovery_frame);
    frame_type = FILS_DISCOVERY_FRAME;
    time_stamp_offset = TIME_STAMP_OFFSET_IN_FILS_DISC;
    wave_pdb_get_binary(radio_param_db,
                        PARAM_DB_RADIO_FILS_DISCOVERY_FRAME,
                        (void *)&fils_discovery_frame[0],
                        &fils_discovery_frame_len);
    buf = fils_discovery_frame;
    len = fils_discovery_frame_len;
    mtlk_dump(1, buf, len, "FILS Discovery template before UM_MAN_SET_UNSOLICITED_REQ");
    if (!buf || (len < sizeof(fils_discovery_frame))) {
      ILOG1_V("No FILS Discovery template!");
      send_umi_msg = false;
    }
  } else if ((mode == WAVE_UNSOLICITED_FRAME_CFG_MODE_DISABLE)) {
    enable = false;
  }

  if (send_umi_msg) {
    tx_interval = duration;
     _wave_unsolicited_frame_template_push(core, (u8 const *)buf, (uint16)len, tx_interval,
                                           frame_type, time_stamp_offset, enable);
  } else {
    ILOG0_V("UM_MAN_SET_UNSOLICITED_REQ not sent to FW");
  }
  /* Free the local buffer */
  if (probe_resp_templ_loc)
    mtlk_osal_mem_free(probe_resp_templ_loc);
  return res;
}

int wave_beacon_man_set_unsolicited_frame_tx(mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_error_t            res   = MTLK_ERR_OK;
  mtlk_core_t            *core  = (mtlk_core_t*)hcore;
  wave_unsolicited_cfg_t *unsolicited_frame_cfg = NULL;
  uint32                 unsolicited_frame_cfg_size;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;
  uint8                  mode = WAVE_UNSOLICITED_FRAME_CFG_INVALID;
  uint8                  duration = WAVE_UNSOLICITED_FRAME_CFG_INVALID;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_hw_band_e         band = wave_radio_band_get(radio);
  mtlk_pdb_t             *param_db_radio = wave_radio_param_db_get(radio);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  unsolicited_frame_cfg = mtlk_clpb_enum_get_next(clpb, &unsolicited_frame_cfg_size);
  MTLK_CLPB_TRY(unsolicited_frame_cfg, unsolicited_frame_cfg_size)

    if (MTLK_HW_BAND_6_GHZ == band) {
      MTLK_ASSERT(NULL != param_db_radio);
      mode = unsolicited_frame_cfg->cfg.mode;
      duration = unsolicited_frame_cfg->cfg.duration;
      wave_pdb_set_int(param_db_radio,
                       PARAM_DB_RADIO_UNSOLICIT_FRAME_CFG_MODE, mode);
      wave_pdb_set_int(param_db_radio,
                       PARAM_DB_RADIO_UNSOLICIT_FRAME_CFG_DURATION, duration);
      ILOG1_DD("Unsolicited frame config received with mode = %d and duration = %d", mode, duration);
      _wave_unsolicited_frame_template_push_wrap(core);

    }  else {
      ILOG0_DD("UM_MAN_SET_UNSOLICITED_REQ is not supported for band = %d, VAP mode = %d",
	            band, MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_MBSSID_VAP));
    }

  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

/* Handling of unsolicited frame tx in the case of a recovery */
mtlk_error_t _wave_recovery_unsolicited_frame_template_push(mtlk_core_t *core)
{
   mtlk_error_t res = MTLK_ERR_OK;
   ILOG1_D("Calling unsolicited frame handling during recovery for band %d",
            wave_radio_band_get(wave_vap_radio_get(core->vap_handle)));
   res = _wave_unsolicited_frame_template_push_wrap(core);
   return res;
}

u8 *
_wave_reset_ssid_in_probe_response_templ(uint8 *buf, size_t len)
{
  u8 *probe_resp_templ_loc, *ies_data;
  ie_t *ie;
  size_t ies_len, ies_data_offs;

  if (!buf || !len) {
    ELOG_V("Template buffer is empty");
    return NULL;
  }

  probe_resp_templ_loc = mtlk_osal_mem_alloc(len, MTLK_MEM_TAG_BEACON_MNGR);
  if (!probe_resp_templ_loc) {
    ELOG_D("Cannot allocate %hu bytes for a new Probe Response template", len);
    return NULL;
  }

  /* Copy the full template */
  wave_memcpy(probe_resp_templ_loc, len, buf, len);

  /* Move frame buffer by frame head and fixed parameters */
  ies_data_offs = sizeof(frame_head_t) + sizeof(frame_beacon_head_t);
  ies_data = probe_resp_templ_loc + ies_data_offs;
  ies_len = len - ies_data_offs;

  /* Update SSID IE in the Probe Response template */
  while (ies_len > sizeof(ie_t))
  {
    size_t ie_len;
    ie = (ie_t *)ies_data;
    ie_len = ie->length + sizeof(ie_t);
    if (ie_len > ies_len) {
      ies_len = 0;
      break;
    }

    if (ie->id == IE_SSID) {
      memset((ies_data + sizeof(ie_t)), 0, ie->length);
      break;
    }
    ies_data += ie_len;
    ies_len -= ie_len;
  }
  return probe_resp_templ_loc;
}

/*
 * Entry point for setting beacon template.
 * It has to be called from master VAP serializer context.
 */
int wave_beacon_man_beacon_set(mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t           *mcore = HANDLE_T_PTR(mtlk_core_t, hcore); /* master core, we're running on it */
  mtlk_core_t           *core; /* core of the VAP the beacon is changed for */
  beacon_manager_priv_t *bmgr_priv;
  mtlk_clpb_t           *clpb = *(mtlk_clpb_t **) data;
  mtlk_beacon_data_t    *beacon_data;
  beacon_manager_t      *bmgr;
  mtlk_vap_handle_t     vap_handle;
  unsigned              beacon_data_size;
  int                   res = MTLK_ERR_OK;
  uint32                vap_idx;
  mtlk_pdb_t            *param_db_core;
  u8                    flags = 0;
  wave_radio_t          *radio;
  mtlk_hw_band_e        band;
  mtlk_pdb_t            *param_db_radio;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  beacon_data = mtlk_clpb_enum_get_next(clpb, &beacon_data_size);

  MTLK_CLPB_TRY(beacon_data, beacon_data_size)
  {
    vap_idx = beacon_data->vap_idx;
    bmgr_priv = beacon_data->bmgr_priv;
    ILOG2_D("VapID %u", vap_idx);

    /* find vap, referenced by vap index */
    res = mtlk_vap_manager_get_vap_handle(mtlk_vap_get_manager(mcore->vap_handle), vap_idx, &vap_handle);
    if (MTLK_ERR_OK != res) {
      ELOG_D("VapID %u doesn't exist", vap_idx);
      MTLK_CLPB_EXIT(res);
    }

    core = mtlk_vap_get_core(vap_handle);
    param_db_core = mtlk_vap_get_param_db(core->vap_handle);
    radio = wave_vap_radio_get(core->vap_handle);
    band = wave_radio_band_get(radio);
    param_db_radio = wave_radio_param_db_get(radio);

    if (beacon_data->probe_resp && beacon_data->probe_resp_len) /* if a new probe resp template is provided */
    {
      u8 *probe_resp_templ = mtlk_osal_mem_alloc(beacon_data->probe_resp_len, MTLK_MEM_TAG_BEACON_MNGR);

      if (!probe_resp_templ)
      {
        ELOG_D("Cannot allocate %hu bytes for a new Probe Response template", beacon_data->probe_resp_len);
        MTLK_CLPB_EXIT(MTLK_ERR_NO_MEM);
      }

      wave_memcpy(probe_resp_templ, beacon_data->probe_resp_len, beacon_data->probe_resp, beacon_data->probe_resp_len);

      /* Free Non-HE buffer in-case it was allocated separately, but do nothing
         if it was pointing to the same original template buffer to avoid
         double freeing.
      */
      if (core->slow_ctx->probe_resp_templ_non_he &&
          core->slow_ctx->probe_resp_templ_non_he != core->slow_ctx->probe_resp_templ) {
        mtlk_osal_mem_free(core->slow_ctx->probe_resp_templ_non_he);
        core->slow_ctx->probe_resp_templ_non_he = NULL;
      }

      if (core->slow_ctx->probe_resp_templ) {
        mtlk_osal_mem_free(core->slow_ctx->probe_resp_templ);
        core->slow_ctx->probe_resp_templ = NULL;
      }

      core->slow_ctx->probe_resp_templ = probe_resp_templ;
      core->slow_ctx->probe_resp_templ_len = beacon_data->probe_resp_len;
    }

    /* We need to fish some things out of the beacon and save until the set_bss time */
    _wave_beacon_template_parsing_ies(core, beacon_data);

    if (beacon_data->probe_resp && beacon_data->probe_resp_len) /* if a new probe resp template is provided */
    {
      /* Non-HE template not generated if we are not HE capable ourselves or
         if the compatibility feature is not enabled.

         Set the Non-HE template to the original template incase generation
         of Non-HE template fails for some reason
      */
      core->slow_ctx->probe_resp_templ_non_he = core->slow_ctx->probe_resp_templ;
      core->slow_ctx->probe_resp_templ_non_he_len = core->slow_ctx->probe_resp_templ_len;
      flags = wave_pdb_get_int(param_db_core, PARAM_DB_CORE_SET_BSS_FLAGS);
      if (MTLK_BFIELD_GET(flags, VAP_ADD_FLAGS_HE) &&
          wave_radio_get_11n_acax_compat(wave_vap_radio_get(core->vap_handle))) {
        res = _wave_probe_resp_template_gen_non_he(core, core->slow_ctx->probe_resp_templ,
                                                   core->slow_ctx->probe_resp_templ_len);
      }
    }

    {
      u8 mbssid_vap = MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_MBSSID_VAP);
      u32 hidden = MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_HIDDEN_SSID);
      if (mbssid_vap > WAVE_RADIO_OPERATION_MODE_MBSSID_TRANSMIT_VAP) {
        ILOG2_D("CID-%04x: Not pushing OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP", mtlk_vap_get_oid(mcore->vap_handle));
        MTLK_CLPB_EXIT(res);
      } else {
        uint32 probe_resp_templ_len = core->slow_ctx->probe_resp_templ_len;
        u8 *probe_resp_templ = core->slow_ctx->probe_resp_templ;
        if ((MTLK_HW_BAND_6_GHZ == band) && probe_resp_templ_len && probe_resp_templ) {
          ILOG1_DDD("Saving the 6GHz probe response template of length %d for mbssid vap %d in the PDB, hidden = %d",
                     probe_resp_templ_len, mbssid_vap, hidden);
          if (hidden)
            probe_resp_templ = _wave_reset_ssid_in_probe_response_templ(core->slow_ctx->probe_resp_templ, probe_resp_templ_len);
          if (probe_resp_templ) {
            wave_pdb_set_int(param_db_radio,
                             PARAM_DB_RADIO_UNSOLICIT_PROBE_RESP_TEMPL_LEN,
                             probe_resp_templ_len);
            wave_pdb_set_binary(param_db_radio,
                                PARAM_DB_RADIO_UNSOLICIT_PROBE_RESP_TEMPL,
                                probe_resp_templ,
                                probe_resp_templ_len);
            if (hidden)
              mtlk_osal_mem_free(probe_resp_templ);
          }
        }
      }

      ILOG2_DD("CID-%04x: Pushing VAP mode %d", mtlk_vap_get_oid(mcore->vap_handle), mbssid_vap);
    }

    /* head or tail can be NULL if not changed? */
    if ((NULL == beacon_data->head) || (NULL == beacon_data->tail)) {
      /* nothing to update, keep old template */
      ELOG_D("CID-%04x: No update for beacon template",
               mtlk_vap_get_oid(mcore->vap_handle));
      MTLK_CLPB_EXIT(res);
    }

    bmgr = &(core->slow_ctx->beacon_man_data);

    /* Save template for recovery */
    res = _wave_beacon_man_template_save(&bmgr->tmpl[MTLK_BEACON_MAN_RCVRY_TMPL_IDX], beacon_data);
    if (MTLK_ERR_OK != res)
      MTLK_CLPB_EXIT(res);

    res = _wave_beacon_man_push_ctrl(core, bmgr, bmgr_priv, beacon_data, vap_idx);
  }
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END;
}

/* indication processing always called from master VAP serializer context */
int wave_beacon_man_template_ind_handle(mtlk_handle_t hcore, const void *data, unsigned data_size)
{
  mtlk_core_t           *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  beacon_manager_priv_t *bmgr_priv;
  mtlk_vap_manager_t    *vap_mgr = mtlk_vap_get_manager(core->vap_handle);
  beacon_manager_t      *bmgr;
  beacon_template_t     *tmpl;
  mtlk_vap_handle_t     vap_handle;
  int                   res;
  uint32                vap_idx, i, max_vaps_count;

  MTLK_UNREFERENCED_PARAM(data_size);
  MTLK_ASSERT(NULL != data);

  /* get VAP index from indication message */
  vap_idx = ((UMI_BEACON_SET *)data)->u8vapIndex;
  ILOG2_D("vap_idx %u", vap_idx);

  /* get beacon manager private */
  bmgr_priv = wave_radio_beacon_man_private_get(wave_vap_radio_get(core->vap_handle));
  MTLK_ASSERT(NULL != bmgr_priv);

  /* mark fw as free, because we've got an indication */
  bmgr_priv->fw_is_busy = FALSE;

  /* find vap, referenced by vap index */
  res = mtlk_vap_manager_get_vap_handle(vap_mgr, vap_idx, &vap_handle);
  if (MTLK_ERR_OK != res) {
    /* It can be a late arriving message sent before the VAP has been removed */
    ILOG0_D("Cannot process MC_MAN_BEACON_TEMPLATE_WAS_SET_IND, VapID %u doesn't exist", vap_idx);
    return res;
  }

  /* beacon manager from VAP referenced by index */
  bmgr = &(mtlk_vap_get_core(vap_handle)->slow_ctx->beacon_man_data);

  /* swap buffer index */
  bmgr->drv_idx ^= 1;
  /* clear switch pending flag */
  bmgr->switch_pending = FALSE;

  tmpl = &bmgr->tmpl[bmgr->drv_idx];
  /* DMA unmap */
  if (tmpl->dma_addr) {
    mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(vap_handle))),
                              tmpl->dma_addr, tmpl->alloced_size, MTLK_DATA_TO_DEVICE);
    tmpl->dma_addr = 0;
  }

  /* if emergency buffer was used in radio module for current VAP */
  if (vap_idx == bmgr_priv->for_vap_idx) {
    /* put emergency template to the current one */
    beacon_template_t tswitch = *tmpl;
    *tmpl = bmgr_priv->tmpl;
    bmgr_priv->tmpl = tswitch;
    /* mark emergency buffer as free */
    bmgr_priv->for_vap_idx = BMGR_NO_VAP;
    /* template was updated */
    bmgr->updated = TRUE;
    ILOG2_V("emergency buffer --> vap ctx");
  }

  /* go over all vaps and find the next template to push */
  max_vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
  for( i= 0; i < max_vaps_count; i++) {
    /* start with next round-robin vap */
    if (++vap_idx >= max_vaps_count)
      vap_idx = 0;
    /* find vap, referenced by vap index */
    res = mtlk_vap_manager_get_vap_handle(vap_mgr, vap_idx, &vap_handle);
    if (MTLK_ERR_OK != res) {
      ILOG3_D("VapID %u doesn't exist, goto next", vap_idx);
      continue;
    }
    /* check if template was updated */
    core = mtlk_vap_get_core(vap_handle);
    bmgr = &(core->slow_ctx->beacon_man_data);
    if (bmgr->updated) {
      ILOG2_D("VapID %u to push template from", vap_idx);
      tmpl = &bmgr->tmpl[bmgr->drv_idx];
      __wave_beacon_man_template_push_wrap(core, tmpl, bmgr, bmgr_priv);
      break; /*exit loop */
    }
  } /* for */

  return MTLK_ERR_OK;
}

/* Helper function to retrieve the Broadcast TWT support */
BOOL
_wave_beacon_get_bcast_twt_support(mtlk_core_t *core)
{
  int btwt_support = wave_pdb_get_int(mtlk_vap_get_param_db(core->vap_handle), PARAM_DB_CORE_BCAST_TWT_OPERATION_MODE);
  return (btwt_support == HE_MAC_BCAST_TWT_RESPONDER_SUPPORTED);
}

static UMI_ADVERTISE_BC_TWT_SP *
_wave_beacon_get_btwt_config(mtlk_core_t *core)
{
  mtlk_error_t res = MTLK_ERR_OK;
  UMI_ADVERTISE_BC_TWT_SP *cmds = NULL;
  mtlk_pdb_t *param_db_core =  mtlk_vap_get_param_db(core->vap_handle);
  mtlk_pdb_size_t size = sizeof(*cmds);

  cmds = (UMI_ADVERTISE_BC_TWT_SP *)mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_BEACON_MNGR);
  if (!cmds) {
    ELOG_D("Cannot allocate %u bytes for retrieving the BTWT configuraiton from Param DB", size);
    return NULL;
  }
  res = wave_pdb_get_binary(param_db_core,
                            PARAM_DB_CORE_BCAST_TWT_CONFIG,
                            (void *)cmds, &size);
  if ((res == MTLK_ERR_OK) && 
      (size == (sizeof(*cmds))))
    return cmds;

  if (cmds)
    mtlk_osal_mem_free(cmds);
  
  return NULL;
}

static mtlk_error_t
_wave_beacon_btwt_msg_to_fw(mtlk_core_t *core, uint16 msg_id, void *msg, bool fw_recovery)
{
#define BCAST_TWT_ID_FOR_ALL_STAS 0
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  uint8 *fw_status = NULL;
  uint8 operation = API_SET_OPERATION;
  UMI_ADVERTISE_BC_TWT_SP *adv_req = NULL;
  UMI_TERMINATE_BC_TWT_SP *trm_req = NULL;
  mtlk_error_t res = MTLK_ERR_OK;
  BOOL bcast_twt_operation_mode = _wave_beacon_get_bcast_twt_support(core);

  if (!msg ||
      !((msg_id == UM_MAN_ADVERTISE_BC_TWT_SP_REQ) || (msg_id == UM_MAN_TERMINATE_BC_TWT_SP_REQ)) ||
      !bcast_twt_operation_mode) {
    ILOG0_DD("BTWT msg id 0x%04X not sent to FW, bcast_twt_oper_mode = %u", msg_id, bcast_twt_operation_mode);	
    return MTLK_ERR_PARAMS;
  }
  /* Do not send a Bcast Advertise msg with ID=0 to FW if one ore more STA(s) is/are already Connected on the VAP */
  /* TODO: To review the check for FW recovery */
  if (!fw_recovery && (msg_id == UM_MAN_ADVERTISE_BC_TWT_SP_REQ) && (mtlk_core_has_connections(core))) {
    uint8 num_sp = 0;
    adv_req = (UMI_ADVERTISE_BC_TWT_SP *)msg;
    for (num_sp=0; num_sp < adv_req->numberOfSpsToAdd; num_sp++) {
      if (adv_req->bcTwtSpParams[num_sp].broadcastTwtId == BCAST_TWT_ID_FOR_ALL_STAS) {
        ILOG0_V("Bcast TWT ID 0 Advertisement is prohibited when one or more Station(s) is/are connected!");
        return MTLK_ERR_PROHIB;
      }
    }
  }
  /* prepare msg for the FW */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res))) {
    ELOG_DDD("CID-%04x: BTWT message id 0x%04X init failed, err=%i",
             mtlk_vap_get_oid(core->vap_handle), msg_id, res);
    res = MTLK_ERR_NO_RESOURCES;
    goto end;
  }

  man_entry->id = msg_id;
  if (msg_id == UM_MAN_ADVERTISE_BC_TWT_SP_REQ) {
    adv_req = (UMI_ADVERTISE_BC_TWT_SP *)man_entry->payload;
    man_entry->payload_size = sizeof(*adv_req);
    *adv_req = *((UMI_ADVERTISE_BC_TWT_SP *)msg);
    operation = adv_req->getSetOperation;
    fw_status = &adv_req->status;
    mtlk_dump(2, adv_req, sizeof(*adv_req), "dump of UMI_ADVERTISE_BC_TWT_SP");
  } else if (msg_id == UM_MAN_TERMINATE_BC_TWT_SP_REQ) {
    trm_req = (UMI_TERMINATE_BC_TWT_SP *)man_entry->payload;
    man_entry->payload_size = sizeof(*trm_req);
    *trm_req = *((UMI_TERMINATE_BC_TWT_SP *)msg);
    fw_status = &trm_req->status;
    mtlk_dump(2, trm_req, sizeof(*trm_req), "dump of UMI_TERMINATE_BC_TWT_SP");
  }
  /* send message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  /* process result */
  if (res != MTLK_ERR_OK || UMI_OK != (*fw_status)) {
    ELOG_DDDD("CID-%04x: BTWT message id 0x%04X send failed, res=%i status=%hhu",
              mtlk_vap_get_oid(core->vap_handle), msg_id, res, *fw_status);
    if (UMI_OK != (*fw_status))
      res = MTLK_ERR_MAC;
  }
  if ((res == MTLK_ERR_OK) && adv_req && (operation == API_GET_OPERATION)) {
    /* GET Operation: Save the fetched buffer from FW to local buffer */
    adv_req->getSetOperation = API_GET_OPERATION; /*To ensure that there is no dependency on FW for this*/
    wave_memcpy(msg, sizeof(UMI_ADVERTISE_BC_TWT_SP), adv_req, sizeof(*adv_req));
  }
  if (NULL != man_entry)
    mtlk_txmm_msg_cleanup(&man_msg);
  end:
    return res;
#undef BCAST_TWT_ID_FOR_ALL_STAS
}

static mtlk_error_t
_wave_beacon_save_btwt_config(mtlk_core_t *core, UMI_ADVERTISE_BC_TWT_SP *req)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_pdb_t *param_db_core = mtlk_vap_get_param_db(core->vap_handle);
  memset(req, 0, sizeof(*req));
  req->getSetOperation = API_GET_OPERATION;
  res = _wave_beacon_btwt_msg_to_fw(core, UM_MAN_ADVERTISE_BC_TWT_SP_REQ, (void *)req, FALSE);
  if (res == MTLK_ERR_OK) {
    res = wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_BCAST_TWT_CONFIG,
                              (const void *)req, sizeof(*req));
    if (res != MTLK_ERR_OK)
      ILOG0_V("Failed to store the BTWT Advertise configuration in Param DB");
  } else {
    ILOG0_V("Failed to retrieve the BTWT configuration from FW!");
  }
  return res;
}

static mtlk_error_t
_wave_beacon_man_send_advertise_btwt_sp (mtlk_core_t *core, UMI_ADVERTISE_BC_TWT_SP *req)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);
  if (hw_get_btwt_support(HANDLE_T(hw))) {
    ILOG1_V("Send BTWT advertise configuration to FW");
    res = _wave_beacon_btwt_msg_to_fw(core, UM_MAN_ADVERTISE_BC_TWT_SP_REQ, (void *)req, FALSE);
    if (res == MTLK_ERR_OK) {
      if (req->getSetOperation != API_GET_OPERATION)
        /* SET operation: Save the req message in the PDB core for use during the recovery*/
        res = _wave_beacon_save_btwt_config(core, req);
    }
  } else {
    ILOG0_V("Broadcast TWT is NOT supported on this HW!");
    res = MTLK_ERR_NOT_SUPPORTED;
  }
  return res;
}

int wave_beacon_man_advertise_btwt_schedule (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_advertise_btwt_sp_cfg_t *advertise_btwt_sp_cfg = NULL;
  uint32 cfg_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* Fetch the BTWT advertise schedule configuration and send it to FW */
  advertise_btwt_sp_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(advertise_btwt_sp_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
       MTLK_CFG_CHECK_ITEM_AND_CALL(advertise_btwt_sp_cfg, config, _wave_beacon_man_send_advertise_btwt_sp,
                                    (core, &advertise_btwt_sp_cfg->config), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res); /* push result into clipboard */
  MTLK_CLPB_END
}

mtlk_error_t
_wave_beacon_reset_btwt_config(mtlk_core_t *core)
{
  mtlk_error_t res;
  const char btwt_config_null[] = "";
  mtlk_pdb_t *param_db_core = mtlk_vap_get_param_db(core->vap_handle);

  res = wave_pdb_set_binary(param_db_core,
                            PARAM_DB_CORE_BCAST_TWT_CONFIG,
                            btwt_config_null, (mtlk_pdb_size_t)sizeof(btwt_config_null));
  if (res != MTLK_ERR_OK)
    ILOG0_V("Failed to reset the BTWT configuration in Param DB");
  return res;
}

static mtlk_error_t
_wave_beacon_man_send_terminate_btwt_sp (mtlk_core_t *core, const UMI_TERMINATE_BC_TWT_SP *req)
{
  mtlk_error_t res;
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);

  if (hw_get_btwt_support(HANDLE_T(hw))) {
    ILOG0_V("Send BTWT terminate configuration to FW");
    res = _wave_beacon_btwt_msg_to_fw(core, UM_MAN_TERMINATE_BC_TWT_SP_REQ, (void *)req, FALSE);
    if (res == MTLK_ERR_OK)
      /*Reset the req message in the PDB core*/
      res = _wave_beacon_reset_btwt_config(core);
  } else {
    ILOG0_V("Broadcast TWT is NOT supported on this HW!");
    return MTLK_ERR_NOT_SUPPORTED;
  }
  return MTLK_ERR_OK;
}

int wave_beacon_man_terminate_btwt_schedule (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_terminate_btwt_sp_cfg_t *terminate_btwt_sp_cfg = NULL;
  uint32 cfg_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* Fetch the BTWT terminate schedule configuration and send it to FW */
  terminate_btwt_sp_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(terminate_btwt_sp_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
       MTLK_CFG_CHECK_ITEM_AND_CALL(terminate_btwt_sp_cfg, config, _wave_beacon_man_send_terminate_btwt_sp,
                                    (core, &terminate_btwt_sp_cfg->config), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res); /* push result into clipboard */
  MTLK_CLPB_END
}

/* Support for recovery */

int wave_beacon_man_rcvry_reset(mtlk_core_t *core)
{
  beacon_manager_t *bmgr;
  int i;

  MTLK_ASSERT(core);

  ILOG2_D("CID-%04x: Resetting beacon manager", mtlk_vap_get_oid(core->vap_handle));

  bmgr = &(core->slow_ctx->beacon_man_data);

  bmgr->drv_idx = 1;
  bmgr->switch_pending = bmgr->updated = 0;

  for (i = 0; i < MTLK_BEACON_MAN_RCVRY_TMPL_NUM; i++) {
    beacon_template_t *tmpl = bmgr->tmpl + i;
    if (tmpl->dma_addr) {
      mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                                tmpl->dma_addr, tmpl->alloced_size, MTLK_DATA_TO_DEVICE);
      tmpl->dma_addr = 0;
    }

    /* Don't clean up the third template which is for beacon restoration */
    if ((tmpl->ptr) && (i != MTLK_BEACON_MAN_RCVRY_TMPL_IDX)) {
      mtlk_osal_mem_free(tmpl->ptr);
      tmpl->alloced_size = tmpl->head_len = tmpl->tail_len = 0;
      tmpl->ptr = NULL;
    }
  }

  /* In master VAP context only */
  if (mtlk_vap_is_master(core->vap_handle)) {
    beacon_manager_priv_t *bmgr_priv;

    bmgr_priv = wave_radio_beacon_man_private_get(wave_vap_radio_get(core->vap_handle));
    MTLK_ASSERT(bmgr_priv);

    bmgr_priv->for_vap_idx = BMGR_NO_VAP;
    bmgr_priv->fw_is_busy = 0;

    if (bmgr_priv->tmpl.dma_addr)
      mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                                bmgr_priv->tmpl.dma_addr, bmgr_priv->tmpl.alloced_size, MTLK_DATA_TO_DEVICE);

    if (bmgr_priv->tmpl.ptr)
      mtlk_osal_mem_free(bmgr_priv->tmpl.ptr);

    memset(&bmgr_priv->tmpl, 0, sizeof(beacon_template_t));
  }

  return MTLK_ERR_OK;
}

/* called as a function, not as a task */
int wave_beacon_man_rcvry_beacon_set(mtlk_core_t *core)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t           *mcore;
  beacon_manager_priv_t *bmgr_priv;
  beacon_manager_t      *bmgr;
  beacon_template_t     *tmpl;

  MTLK_ASSERT(core);

  ILOG2_D("CID-%04x", mtlk_vap_get_oid(core->vap_handle));

  mcore     = mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle));
  bmgr      = &(core->slow_ctx->beacon_man_data);
  bmgr_priv = wave_radio_beacon_man_private_get(wave_vap_radio_get(core->vap_handle));
  tmpl      = &bmgr->tmpl[MTLK_BEACON_MAN_RCVRY_TMPL_IDX];

  MTLK_ASSERT(NULL != bmgr_priv);

  if (0 == tmpl->alloced_size) {
    return MTLK_ERR_OK;
  }

  mtlk_osal_event_reset(&bmgr_priv->rcvry_beacon_ind_event);
  /* push it directly, not through the wrapper */
  res = _wave_beacon_man_template_push(core, tmpl);

  if (MTLK_ERR_OK != res) {
    return res;
  }

  res = mtlk_osal_event_wait(&bmgr_priv->rcvry_beacon_ind_event, MTLK_BEACON_MAN_RCVRY_WAIT_TIME);

  if (MTLK_ERR_OK != res) {
    ELOG_D("CID-%04x: Timeout while waiting beacon_man_template_ind from FW", mtlk_vap_get_oid(core->vap_handle));
  }

  return res;
}

/* called as a function, not as a task */
void wave_beacon_man_rcvry_template_ind_handle(mtlk_core_t *core)
{
  beacon_manager_priv_t *bmgr_priv = wave_radio_beacon_man_private_get(wave_vap_radio_get(core->vap_handle));

  ILOG2_D("CID-%04x", mtlk_vap_get_oid(core->vap_handle));

  if (bmgr_priv) {
    mtlk_osal_event_set(&bmgr_priv->rcvry_beacon_ind_event);
  }

}

/* Handling of Broadcast TWT schedule in the case of a recovery */
mtlk_error_t _wave_beacon_recovery_btwt_config(mtlk_core_t *core)
{
  mtlk_error_t res = MTLK_ERR_OK;
  UMI_ADVERTISE_BC_TWT_SP *cmds = NULL;
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);

  if (hw_get_btwt_support(HANDLE_T(hw))) {
    ILOG0_V("Broadcast TWT schedule handling during recovery");
    /*Retrieve the Broadcast TWT config from PDB Core*/
    if ((cmds = _wave_beacon_get_btwt_config(core))) {
      cmds->getSetOperation = API_SET_OPERATION;
      res =  _wave_beacon_btwt_msg_to_fw(core, UM_MAN_ADVERTISE_BC_TWT_SP_REQ, (void *)cmds, TRUE);
      if (res == MTLK_ERR_OK)
        ILOG0_V("Bcast configuration sent successfully to FW during recovery"); 
      mtlk_osal_mem_free(cmds);
    } else {
      ILOG0_V("No Bcast configuration present in the PDB");
    }
  }
  return res;
}

static mtlk_error_t
_wave_beacon_man_receive_advertised_btwt_sp_cfg (mtlk_core_t *core, UMI_ADVERTISE_BC_TWT_SP *advertised_btwt_sp)
{
  if (!advertised_btwt_sp)
    return MTLK_ERR_PARAMS;

  /* Get operation */
  memset(advertised_btwt_sp, 0, sizeof(*advertised_btwt_sp));
  advertised_btwt_sp->getSetOperation = API_GET_OPERATION;
  return _wave_beacon_man_send_advertise_btwt_sp(core, advertised_btwt_sp);
}

int wave_beacon_man_get_advertised_btwt_schedule (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_advertise_btwt_sp_cfg_t advertise_btwt_sp_cfg = {0};
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* Fetch advertised BTWT configuration from the FW */
  MTLK_CFG_SET_ITEM_BY_FUNC(&advertise_btwt_sp_cfg, config, _wave_beacon_man_receive_advertised_btwt_sp_cfg,
                            (core, &advertise_btwt_sp_cfg.config), res);

  /* Push the result and fetched configuration to clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &advertise_btwt_sp_cfg, sizeof(advertise_btwt_sp_cfg));
}
