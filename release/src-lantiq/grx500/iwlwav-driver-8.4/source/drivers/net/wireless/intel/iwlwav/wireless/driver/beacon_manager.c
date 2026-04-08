/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

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

#define LOG_LOCAL_GID   GID_BEACON_MAN
#define LOG_LOCAL_FID   0

#define HT_CAP_INFO_OFFSET      2
#define HT_CAP_INFO_LEN         2
#define SMPS_BIT_OFFSET         2
#define SMPS_BIT_LEN            2
#define SMPS_HTCAP_MODE_STATIC  0
#define SMPS_FW_MODE_STATIC     1
#define RX_MCS_BITMASK_OFFSET   5
#define RX_MCS_BITMASK_LEN      10
#define VHT_MCS_NSS_OFFSET      6
#define VHT_MCS_NSS_LEN         8
#define IE_HE_MCS_NSS_OFFSET    20
#define HE_MCS_NSS_160MHZ_LEN   8
#define HE_MCS_NSS_80MHZ_LEN    4
#define IE_HE_MAC_TWT_RESPONDER_OFFSET     (WLAN_EID_EXTENSION_ID_OFFSET + 1) /* Extension length=1 */
#define HE_MAC_TWT_RESPONDER_SUPPORT        2
#define HE_MAC_TWT_RESPONDER_SUPPORT_WIDTH  1
#define HE_MAC_TWT_RESPONDER_NOT_SUPPORTED  0
#define HE_MAC_TWT_RESPONDER_SUPPORTED      1
#define IE_HE_PHY_CHANNEL_WIDTH_SET_OFFSET (WLAN_EID_EXTENSION_ID_OFFSET + 7) /* Extension length = 1, MAC length = 6 */
#define HE_PHY_CHANNEL_WIDTH_SET_B2         3

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

/* TODO: Review if some of these need to be derived from the 6G RNR IE.        *
 * TBTT Info Length of 12 corresponds to the below TBTT info in RNR:           *
 * Neigh AP TBTT Offset, BSSID, Short SSID and BSS parameters subfields        *
 * Refer to IEEE P802.11ax/D4.3, August 2019 Sec 9.4.2.170 for more details    */

#define RNR_6G_TBTT_INFO_COUNT_OFFSET    2
#define RNR_6G_TBTT_INFO_COUNT_SHIFT     4
#define RNR_6G_TBTT_INFO_LEN_OFFSET      3
#define RNR_6G_TBTT_INFO_LEN_SUPPORTED  12
#define RNR_6G_SHORT_SSID_OFFSET_FIRST  13
#define RNR_6G_SHORT_SSID_OFFSET_NEXT   (RNR_6G_TBTT_INFO_LEN_SUPPORTED) 
#define RNR_6G_SHORT_SSID_LEN           4

#define TIME_STAMP_OFFSET_IN_PROBE_RESP    0
#define TIME_STAMP_OFFSET_IN_FILS_DISC     4

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
_wave_get_beacon_data_length(beacon_template_t *tmpl, int *part1_len, int *part2_len, int *part3_len, int *part4_len, int *part5_len)
{
  BOOL result = TRUE;
  int ie_id;
  int ie_len;
  uint8 *pos = tmpl->ptr + tmpl->head_len;
  uint8 *last_pos = tmpl->ptr + tmpl->head_len + tmpl->tail_len;

  *part1_len = tmpl->head_len;
  *part2_len = 0;
  *part3_len = 0;
  *part4_len = 0;
  *part5_len = 0;

  while (pos < last_pos) {
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
#if 0 /* VK: Commented code - info about sequence of IEs in beacon    */
      /* part 1 length already taken from beacon template head length */
    case IE_SSID:
    case IE_SUPPORTED_RATES:
    case IE_FH_PARAM_SET:
    case IE_DS_PARAM_SET:
    case IE_CF_PARAM_SET:
    case IE_IBSS:
      *part1_len += ie_len;
      break;
#endif
    case IE_TIM:
      ELOG_V("TIM IE must not be present in beacon template");
      /* Drop down */
    case IE_COUNTRY:
    case IE_FH_PATTRN_PARAMS:
    case IE_FH_PATTRM_TABLE:
    case IE_PWR_CONSTRAINT:
      *part2_len += ie_len;
      break;
    case IE_CHANNEL_SW_ANN:
      ELOG_V("Channel Switch Announcement IE must not be present in beacon template");
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
      /* Drop down */
    default:
      *part5_len += ie_len;
    }
  }

  if (result == TRUE) {
    if ((tmpl->head_len + tmpl->tail_len) != (*part1_len + *part2_len + *part3_len + *part4_len + *part5_len)) {
      ELOG_DDDDDDD("Beacon template lengths mismatch: (%d + %d) is not equal (%d + %d + %d + %d + %d)",
                   tmpl->head_len, tmpl->tail_len, *part1_len, *part2_len, *part3_len, *part4_len, *part5_len);
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
  uint16            oid;

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_DP("CID-%04x: data 0x%p", oid, tmpl);

  if (NULL == tmpl->ptr) {
    ELOG_D("CID-%04x: tmpl->ptr is NULL", oid);
    return MTLK_ERR_PARAMS;
  }

  /* mtlk_dump(2, tmpl->ptr, tmpl->head_len + tmpl->tail_len, "dump of beacon template:"); */

  if (FALSE == _wave_get_beacon_data_length(tmpl, &part1_len, &part2_len, &part3_len, &part4_len, &part5_len)) {
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
  psUmiBeacon->u16part5Len    = HOST_TO_MAC16(part5_len);
  psUmiBeacon->u8vapIndex     = mtlk_vap_get_id(core->vap_handle);
  psUmiBeacon->addBssLoadIe   = mtlk_df_user_get_hs20_status(mtlk_vap_get_df(core->vap_handle)) ? 1 : 0;
  psUmiBeacon->addBssLoadIe  |= MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_IS_BSS_LOAD_ENABLE) ? 1 : 0;

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
  MTLK_ASSERT(ie_len >= sizeof(*hs20_ie));
  return (ie_len >= sizeof(*hs20_ie)) && (!!(hs20_ie->hs_conf & HS20_DGAF_DISABLED_MASK));
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

  beacon_data->head = (u8*)beacon->data;
  beacon_data->head_len = offs->tim_offset;
  beacon_data->tail = beacon_data->head + beacon_data->head_len + offs->tim_length;
  beacon_data->tail_len = beacon->len - beacon_data->head_len - offs->tim_length;
  beacon_data->probe_resp = (u8*)probe_resp->data;
  beacon_data->probe_resp_len = probe_resp->len;

  return 0;
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
      set_bss_flags |= MTLK_BFIELD_VALUE(VAP_ADD_FLAGS_SMPS, (SMPS_HTCAP_MODE_STATIC == smps) ? SMPS_FW_MODE_STATIC : 0, uint8);

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

      } else {
        ELOG_DDD("Extension ID %d Offset %d Expected %d", he_cap_ie[WLAN_EID_EXTENSION_ID_OFFSET], WLAN_EID_EXTENSION_ID_OFFSET, WLAN_EID_HE_CAP_EXT);
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
    }
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
             mtlk_frame_oper_rateset_to_net_mode(bss_data.operational_rate_set,
                 MTLK_BFIELD_GET(set_bss_flags, VAP_ADD_FLAGS_HT),
                 MTLK_BFIELD_GET(set_bss_flags, VAP_ADD_FLAGS_VHT),
                 MTLK_BFIELD_GET(set_bss_flags, VAP_ADD_FLAGS_HE)));
     }
  }

  /* Short SSID List extraction from 6G RNR IE and storing in the PDB core */
  {
      mtlk_error_t res;
      u8 count, num_of_vaps;
      u8 *short_ssid_list;
      mtlk_pdb_size_t list_size;

      /* Retrieve the RNR IE if present in the template */
      const u8 *rnr_ie = cfg80211_find_ie(IE_REDUCED_NEIGH_REPORT, beacon_data->tail, beacon_data->tail_len);
      if (!rnr_ie) {
        wave_pdb_set_int(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT, 0);
        ILOG1_DD("RNR IE id : %d not found in the Beacon template for band = %d",IE_REDUCED_NEIGH_REPORT, band);
        return;
      }

      if (rnr_ie[RNR_6G_TBTT_INFO_LEN_OFFSET] != RNR_6G_TBTT_INFO_LEN_SUPPORTED) {
        ILOG1_DD("TBTT Information Length %u in RNR IE for band = %d. No Short SSID List present!",
                  rnr_ie[RNR_6G_TBTT_INFO_LEN_OFFSET], band);
        return;
      }

      /* Retrieve the number of VAPs included in the RNR IE. (Count - 1) is reported in the IE */
      num_of_vaps = (rnr_ie[RNR_6G_TBTT_INFO_COUNT_OFFSET] >> RNR_6G_TBTT_INFO_COUNT_SHIFT ) + 1;

      /* Allocate a local buffer to store the list of short SSIDs */
      list_size = (num_of_vaps * RNR_6G_SHORT_SSID_LEN);   
      short_ssid_list = mtlk_osal_mem_alloc(list_size , MTLK_MEM_TAG_BEACON_MNGR);
      if (!short_ssid_list) {
        ELOG_DD("Cannot allocate %u bytes for copying the short SSID list for band = %d",list_size, band);
        return;
      }

      /* Copy the short SSIDs into the local buffer.    *
       * Covert from Little Endian to Big Endian format */
      {
        const u8 *ptr_src = NULL;
        u8 *ptr_dst = NULL;
        u8 src_idx = RNR_6G_SHORT_SSID_OFFSET_FIRST;
        u8 dst_idx = 0;
        int i;
        
        for (count = 0; count < num_of_vaps; count++) {
          ptr_src = &rnr_ie[src_idx];
          ptr_dst = &short_ssid_list[dst_idx];
          for (i = (RNR_6G_SHORT_SSID_LEN - 1); i >= 0; i--)
            *(ptr_dst + i) = *ptr_src++;
          src_idx += RNR_6G_SHORT_SSID_OFFSET_NEXT;
          dst_idx += RNR_6G_SHORT_SSID_LEN;
        }
      }

      /* Copy the local buffer containing the Short SSID list into the PDB */
      res = wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_LIST, short_ssid_list, list_size);

      if(res == MTLK_ERR_OK) {
        /* Save the Short SSID count to the PDB. Necessary later for retrieving the short SSID list from the PDB */      
        wave_pdb_set_int(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT, num_of_vaps);
        ILOG1_DDD("RNR IE id %d received for band = %d. Short SSID count = %d",rnr_ie[0], band, num_of_vaps);
      } else {
        ELOG_D("Failed to set the Short SSID List in PDB for band = %d",band);
      }

      /* Release the short_ssid_list local buffer */
      mtlk_osal_mem_free(short_ssid_list);

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

mtlk_error_t _wave_unsolicited_frame_template_push(mtlk_core_t *core, void const *data, uint16 len, uint16 interval,
                                                   uint8 frame_type, uint16 time_stamp_offset, bool enable)
{
  UMI_UNSOLICITED_SET *psUmiUnsolicitedFrame;
  mtlk_txmm_data_t    *man_entry;
  mtlk_txmm_msg_t     man_msg;
  uint32              dma_addr = 0;
  mtlk_error_t        err = MTLK_ERR_OK;
  uint8               *unsolicit_frame_templ = NULL;
  frame_head_t        *tmp = NULL;
  uint16              oid = mtlk_vap_get_oid(core->vap_handle); 

  if (data && len) {
    unsolicit_frame_templ = mtlk_osal_mem_dma_alloc(len, MTLK_MEM_TAG_BEACON_DATA);
    if (!unsolicit_frame_templ) {
      ELOG_D("Malloc %u bytes for unsolicited data failed", len);
      return MTLK_ERR_NO_MEM;
    }
    wave_memcpy(unsolicit_frame_templ, len, data, len);
  
    tmp = (frame_head_t *)unsolicit_frame_templ;
    memset(tmp->dst_addr.au8Addr, 0xFF, IEEE_ADDR_LEN);
  }
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &err);
  if (NULL == man_entry) {
    ELOG_DD("CID-%04x: UM_MAN_SET_UNSOLICITED_REQ init failed, err=%i", oid, err);
    return MTLK_ERR_NO_RESOURCES;
  }
  if (unsolicit_frame_templ)
    dma_addr = mtlk_osal_map_to_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(mtlk_vap_get_hw(core->vap_handle))),
                                        unsolicit_frame_templ, len, MTLK_DATA_TO_DEVICE);

  man_entry->id = UM_MAN_SET_UNSOLICITED_REQ;
  man_entry->payload_size = sizeof(UMI_UNSOLICITED_SET);
  psUmiUnsolicitedFrame = (UMI_UNSOLICITED_SET *)man_entry->payload;

  psUmiUnsolicitedFrame->hostBufferAddress = HOST_TO_MAC32(dma_addr);
  psUmiUnsolicitedFrame->hostBufferSize = HOST_TO_MAC16(len);
  psUmiUnsolicitedFrame->filsTimestampOffset = HOST_TO_MAC16(time_stamp_offset);
  psUmiUnsolicitedFrame->txIntervalMsecs = HOST_TO_MAC16(interval);
  psUmiUnsolicitedFrame->isEnabled = enable;
  psUmiUnsolicitedFrame->frameType = frame_type;

  ILOG1_DDDDDD("UM_MAN_SET_UNSOLICITED_REQ: DMA addr: %x, Size: %x, Offset: %x, Int: %x, Enable: %x, Frame: %x",
              psUmiUnsolicitedFrame->hostBufferAddress, psUmiUnsolicitedFrame->hostBufferSize,
              psUmiUnsolicitedFrame->filsTimestampOffset,  psUmiUnsolicitedFrame->txIntervalMsecs,
              psUmiUnsolicitedFrame->isEnabled, psUmiUnsolicitedFrame->frameType); 
  err = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  oid = mtlk_vap_get_oid(core->vap_handle);
  if (err != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: UM_MAN_SET_UNSOLICITED_REQ send failed, err=%i", oid, err);
  } else if (psUmiUnsolicitedFrame->Status != UMI_OK) {
    ELOG_DD("CID-%04x: UM_MAN_SET_UNSOLICITED_REQ failed, Status=%u", oid, psUmiUnsolicitedFrame->Status);
    err = MTLK_ERR_MAC;
  }
  mtlk_txmm_msg_cleanup(&man_msg);
  return err;
}

int wave_beacon_man_set_unsolicited_frame_tx (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_error_t            res   = MTLK_ERR_OK;
  mtlk_core_t            *core  = (mtlk_core_t*)hcore;
  wave_unsolicited_cfg_t *unsolicited_frame_cfg = NULL;
  uint32                 unsolicited_frame_cfg_size;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;
  unsigned               len = 0;
  u8                     *buf = NULL;
  uint8                  frame_type = PROBE_RES_FRAME;
  uint16                 time_stamp_offset = 0;
  uint16                 tx_interval = 0;
  bool                   enable = true;
  bool                   send_umi_msg = true;
  mtlk_hw_band_e         band = wave_radio_band_get(wave_vap_radio_get(core->vap_handle));

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  unsolicited_frame_cfg = mtlk_clpb_enum_get_next(clpb, &unsolicited_frame_cfg_size);
  MTLK_CLPB_TRY(unsolicited_frame_cfg, unsolicited_frame_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()

    if (MTLK_HW_BAND_6_GHZ == band && mtlk_vap_get_id(core->vap_handle) == 0) {
      if (unsolicited_frame_cfg->cfg.mode == WAVE_UNSOLICITED_FRAME_CFG_MODE_PROBE_RESP) {
        time_stamp_offset = TIME_STAMP_OFFSET_IN_PROBE_RESP;
        buf = core->slow_ctx->probe_resp_templ;
        len = core->slow_ctx->probe_resp_templ_len;
        if (!buf || !len) {
          ILOG0_V("Missing Probe Response template!");
          send_umi_msg = false;
        }
      }
      /* TODO - Pass the FILS Discovery template correctly */
      else if (unsolicited_frame_cfg->cfg.mode == WAVE_UNSOLICITED_FRAME_CFG_MODE_FILS_DISC) {
        frame_type = FILS_DISCOVERY_FRAME;
        time_stamp_offset = TIME_STAMP_OFFSET_IN_FILS_DISC;
        buf = NULL; /* TODO */
        len = 0;   /* TODO */
        enable = false; /* TODO */
        send_umi_msg = false; /* TODO */
      }
      else if ((unsolicited_frame_cfg->cfg.mode == WAVE_UNSOLICITED_FRAME_CFG_MODE_DISABLE)) {
        enable = false;
      }
    
      if (send_umi_msg) {
        tx_interval = unsolicited_frame_cfg->cfg.duration;
        _wave_unsolicited_frame_template_push(core, (void const *)buf, (uint16)len, tx_interval,
                                              frame_type, time_stamp_offset, enable);
      } else {
        ILOG0_V("UM_MAN_SET_UNSOLICITED_REQ not sent to FW");
      }
    } else {
      ILOG0_DD("UM_MAN_SET_UNSOLICITED_REQ is not supported for band = %d vapid = %d", band, mtlk_vap_get_id(core->vap_handle));
    }
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
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

    if(MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_MBSSID_VAP) > WAVE_RADIO_OPERATION_MODE_MBSSID_TRANSMIT_VAP) {
      ILOG2_D("CID-%04x: Not pushing OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP", mtlk_vap_get_oid(mcore->vap_handle));
      MTLK_CLPB_EXIT(res);
    } else {
      ILOG2_D("CID-%04x: Pushing OPERATION_MODE_MBSSID_TRANSMIT_VAP", mtlk_vap_get_oid(mcore->vap_handle));
    }

    /* head or tail can be NULL if not changed? */
    if ((NULL == beacon_data->head) || (NULL == beacon_data->tail)) {
      /* nothing to update, keep old template */
      ELOG_DPP("CID-%04x: No update for beacon template: head = %p, tail = %p",
               mtlk_vap_get_oid(mcore->vap_handle), beacon_data->head, beacon_data->tail);
      MTLK_CLPB_EXIT(res);
    }

    bmgr = &(core->slow_ctx->beacon_man_data);

    /* Save template for recovery */
    res = _wave_beacon_man_template_save(&bmgr->tmpl[MTLK_BEACON_MAN_RCVRY_TMPL_IDX], beacon_data);
    if (MTLK_ERR_OK != res)
      MTLK_CLPB_EXIT(res);

    res = _wave_beacon_man_push_ctrl(core, bmgr, bmgr_priv, beacon_data, vap_idx);
#if ENABLE_UNSOLICIT_PROBE_RESP
    if (band == MTLK_HW_BAND_6_GHZ) {
      mtlk_osal_timer_set(&core->slow_ctx->unsolicited_probe_resp_timer,UNSOLICIT_PROBE_RESP_INTERVAL);
      ILOG0_V("setting timer for unsolicited probe resp from beacon set");
    }
#endif
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

