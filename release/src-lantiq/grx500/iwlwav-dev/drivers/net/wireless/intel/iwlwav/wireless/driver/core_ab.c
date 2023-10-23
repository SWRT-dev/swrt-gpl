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
 * Core's abilities management functionality
 *
 * Written by: Grygorii Strashko
 *
 */

#include "mtlkinc.h"
#include "core_priv.h"
#include "core.h"
#include "mtlk_coreui.h"

#define LOG_LOCAL_GID   GID_CORE
#define LOG_LOCAL_FID   1

/*************************************************************************************************
 * Core abilities
 *************************************************************************************************/

static const mtlk_ability_id_t _core_general_abilities[] = {
  WAVE_CORE_REQ_ACTIVATE_OPEN,
  WAVE_CORE_REQ_SET_BEACON,
  WAVE_CORE_REQ_REQUEST_SID,
  WAVE_CORE_REQ_REMOVE_SID,
  WAVE_CORE_REQ_SYNC_DONE,
  WAVE_CORE_REQ_GET_STATION,
  WAVE_CORE_REQ_SET_WMM_PARAM,
  WAVE_CORE_REQ_MGMT_TX,
  WAVE_CORE_REQ_MGMT_RX,
  WAVE_CORE_REQ_DEACTIVATE,
  WAVE_CORE_REQ_GET_CORE_CFG,
  WAVE_CORE_REQ_SET_CORE_CFG,
  WAVE_CORE_REQ_SET_MAC_ADDR,
  WAVE_CORE_REQ_GET_MAC_ADDR,
  WAVE_CORE_REQ_CHANGE_BSS,
  MTLK_CORE_REQ_MCAST_HELPER_GROUP_ID_ACTION,
  WAVE_CORE_REQ_SET_QOS_MAP,
  WAVE_CORE_REQ_SET_AGGR_CONFIG,
  WAVE_CORE_REQ_GET_AGGR_CONFIG,
  WAVE_CORE_REQ_SET_BLACKLIST_ENTRY,
  WAVE_CORE_REQ_GET_STATION_MEASUREMENTS,
  WAVE_CORE_REQ_GET_VAP_MEASUREMENTS,
  WAVE_CORE_REQ_SET_FOUR_ADDR_CFG,
  WAVE_CORE_REQ_GET_FOUR_ADDR_STA_LIST,
  WAVE_CORE_REQ_SET_WDS_WPA_LIST_ENTRY,
  WAVE_CORE_REQ_CHECK_4ADDR_MODE,
  WAVE_CORE_REQ_GET_NETWORK_MODE,
  WAVE_CORE_REQ_SET_SOFTBLOCKLIST_ENTRY,
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_STATS,
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_RATE_INFO_RX_STATS,
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_RATE_INFO_TX_STATS,
  WAVE_CORE_REQ_GET_CHANNEL_STATS,
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_TID_STATS,
  WAVE_CORE_REQ_SET_STATS_POLL_PERIOD,
  WAVE_CORE_REQ_GET_STATS_POLL_PERIOD,
  WAVE_CORE_REQ_GET_PEER_LIST,
  WAVE_CORE_REQ_GET_PEER_FLOW_STATUS,
  WAVE_CORE_REQ_GET_PEER_CAPABILITIES,
  WAVE_CORE_REQ_GET_PEER_RATE_INFO,
  WAVE_CORE_REQ_GET_RECOVERY_STATS,
  WAVE_CORE_REQ_GET_HW_FLOW_STATUS,
  WAVE_CORE_REQ_GET_TR181_WLAN_STATS,
  WAVE_CORE_REQ_GET_TR181_HW_STATS,
  WAVE_CORE_REQ_GET_TR181_PEER_STATS,
  WAVE_CORE_REQ_GET_DEV_DIAG_RESULT2,
  WAVE_CORE_REQ_GET_DEV_DIAG_RESULT3,
  WAVE_CORE_REQ_GET_ASSOC_STATIONS_STATS,
  WAVE_CORE_REQ_SET_CHAN_SWITCH_DEAUTH_PARAMS,
  WAVE_CORE_REQ_GET_PRIORITY_GPIO_STATS,
  WAVE_CORE_REQ_SET_DYNAMIC_MC_RATE,
  WAVE_CORE_REQ_SET_FILS_DISCOVERY_FRAME,
  WAVE_CORE_REQ_SET_HE_OPERATION,
  WAVE_CORE_REQ_GET_HE_OPERATION,
  WAVE_CORE_REQ_SET_HE_NON_ADVERTISED,
  WAVE_CORE_REQ_GET_HE_NON_ADVERTISED,
  WAVE_CORE_REQ_SET_HE_DEBUG,
  WAVE_CORE_REQ_SET_PREAMBLE_PUNCTURE_CCA_OVERRIDE,
  WAVE_CORE_REQ_GET_PREAMBLE_PUNCTURE_CCA_OVERRIDE,
  WAVE_CORE_REQ_TX_TWT_TEARDOWN,
  WAVE_CORE_REQ_SET_WHM_TRIGGER,
  WAVE_CORE_REQ_GET_CONNECTION_ALIVE,
  WAVE_CORE_REQ_GET_TX_RX_WARN,
  WAVE_CORE_REQ_SET_AP_EXCE_RETRY_LIMIT,
  WAVE_CORE_REQ_GET_AP_EXCE_RETRY_LIMIT,

#ifdef CONFIG_WAVE_DEBUG
  WAVE_CORE_REQ_GET_BLACKLIST_ENTRIES,
  WAVE_CORE_REQ_GET_MULTI_AP_BL_ENTRIES,
  WAVE_CORE_REQ_GET_WDS_WPA_LIST_ENTRIES,
  WAVE_CORE_REQ_GET_QOS_MAP_INFO,
  WAVE_CORE_REQ_GET_SERIALIZER_INFO,
  WAVE_CORE_REQ_GET_PEER_HOST_IF_QOS,
  WAVE_CORE_REQ_GET_PEER_HOST_IF,
  WAVE_CORE_REQ_GET_PEER_RX_STATS,
  WAVE_CORE_REQ_GET_PEER_UL_BSRC_TID,
  WAVE_CORE_REQ_GET_PEER_BAA_STATS,
  WAVE_CORE_REQ_GET_LINK_ADAPTION_STATS,
  WAVE_CORE_REQ_GET_PLAN_MANAGER_STATS,
  WAVE_CORE_REQ_GET_TWT_STATS,
  WAVE_CORE_REQ_GET_PER_CLIENT_STATS,
  WAVE_CORE_REQ_GET_PEER_PHY_RX_STATUS,
  WAVE_CORE_REQ_GET_PEER_INFO,
  WAVE_CORE_REQ_GET_WLAN_HOST_IF_QOS,
  WAVE_CORE_REQ_GET_WLAN_HOST_IF,
  WAVE_CORE_REQ_GET_WLAN_RX_STATS,
  WAVE_CORE_REQ_GET_WLAN_BAA_STATS,
  WAVE_CORE_REQ_GET_RADIO_RX_STATS,
  WAVE_CORE_REQ_GET_RADIO_BAA_STATS,
  WAVE_CORE_REQ_GET_TSMAN_INIT_TID_GL,
  WAVE_CORE_REQ_GET_TSMAN_INIT_STA_GL,
  WAVE_CORE_REQ_GET_TSMAN_RCPT_TID_GL,
  WAVE_CORE_REQ_GET_TSMAN_RCPT_STA_GL,
  WAVE_CORE_REQ_GET_RADIO_LINK_ADAPT_STATS,
  WAVE_CORE_REQ_GET_MULTICAST_STATS,
  WAVE_CORE_REQ_GET_TR_MAN_STATS,
  WAVE_CORE_REQ_GET_GRP_MAN_STATS,
  WAVE_CORE_REQ_GET_GENERAL_STATS,
  WAVE_CORE_REQ_GET_CUR_CHANNEL_STATS,
  WAVE_CORE_REQ_GET_RADIO_PHY_RX_STATS,
  WAVE_CORE_REQ_GET_DYNAMIC_BW_STATS,
  WAVE_CORE_REQ_GET_LINK_ADAPT_MU_STATS,
#endif
};

static const mtlk_ability_id_t _core_ap_general_abilities[] = {
  WAVE_CORE_REQ_AP_CONNECT_STA,
  WAVE_CORE_REQ_AP_DISCONNECT_STA,
  WAVE_CORE_REQ_AP_DISCONNECT_ALL,
  WAVE_CORE_REQ_AUTHORIZING_STA,
  WAVE_CORE_REQ_SET_MCAST_RANGE,
  WAVE_CORE_REQ_GET_MCAST_RANGE_IPV4,
  WAVE_CORE_REQ_GET_MCAST_RANGE_IPV6,
  WAVE_CORE_REQ_SET_FWRD_UNKWN_MCAST_FLAG,
  WAVE_CORE_REQ_GET_FWRD_UNKWN_MCAST_FLAG,
  WAVE_CORE_REQ_GET_RX_TH,
  WAVE_CORE_REQ_SET_RX_TH,
  WAVE_CORE_REQ_GET_UNCONNECTED_STATION,
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  WAVE_CORE_REQ_SET_RTLOG_CFG,
#endif
  WAVE_CORE_REQ_SET_ATF_QUOTAS,
  WAVE_CORE_REQ_GET_TX_POWER,
  WAVE_CORE_REQ_SET_HT_PROTECTION,
  WAVE_CORE_REQ_GET_HT_PROTECTION,
  WAVE_CORE_REQ_GET_TWT_PARAMETERS,
  WAVE_CORE_REQ_GET_AX_DEFAULT_PARAMS,
  WAVE_CORE_REQ_SET_UNSOLICITED_FRAME_TX,
  WAVE_CORE_REQ_ADVERTISE_BTWT_SCHEDULE,
  WAVE_CORE_REQ_TERMINATE_BTWT_SCHEDULE,
  WAVE_CORE_REQ_GET_ADVERTISED_BTWT_SCHEDULE
};

static const mtlk_ability_id_t _core_status_abilities[] = {
  WAVE_CORE_REQ_GET_STATUS,
  WAVE_CORE_REQ_GET_RANGE_INFO,
  WAVE_CORE_REQ_CCA_STATS_GET,
#ifdef CONFIG_WAVE_DEBUG
  WAVE_CORE_REQ_RESET_STATS,
  WAVE_CORE_REQ_GET_MC_IGMP_TBL,
  WAVE_CORE_REQ_GET_MC_HW_TBL,
#endif
};

static const mtlk_ability_id_t _core_security_abilities[] = {
  WAVE_CORE_REQ_SET_WDS_WEP_ENC_CFG,
  WAVE_CORE_REQ_GET_ENCEXT_CFG,
  WAVE_CORE_REQ_SET_ENCEXT_CFG,
  WAVE_CORE_REQ_SET_DEFAULT_KEY_CFG
};

static const mtlk_ability_id_t _core_sta_connect_abilities[] = {
  WAVE_CORE_REQ_AP_CONNECT_STA,
  WAVE_CORE_REQ_AP_DISCONNECT_STA,
  WAVE_CORE_REQ_AP_DISCONNECT_ALL,
  WAVE_CORE_REQ_AUTHORIZING_STA
};

static const mtlk_ability_id_t _core_sta_general_abilities[] = {
  WAVE_CORE_REQ_SET_BEACON_INTERVAL,
  WAVE_CORE_REQ_SET_OPERATING_MODE
};

static const mtlk_ab_tbl_item_t wave_core_abilities_ap_init_tbl[]= {
    { _core_general_abilities,            ARRAY_SIZE(_core_general_abilities)},
    { _core_status_abilities,             ARRAY_SIZE(_core_status_abilities)},
    { _core_security_abilities,           ARRAY_SIZE(_core_security_abilities)},
    { _core_ap_general_abilities,         ARRAY_SIZE(_core_ap_general_abilities)},
    { NULL, 0},
};

static const mtlk_ab_tbl_item_t wave_core_abilities_sta_init_tbl[]= {
    { _core_general_abilities,            ARRAY_SIZE(_core_general_abilities)},
    { _core_status_abilities,             ARRAY_SIZE(_core_status_abilities)},
    { _core_security_abilities,           ARRAY_SIZE(_core_security_abilities)},
    { _core_sta_connect_abilities,        ARRAY_SIZE(_core_sta_connect_abilities)},
    { _core_sta_general_abilities,        ARRAY_SIZE(_core_sta_general_abilities)},
    { NULL, 0},
};

/*************************************************************************************************
 * Core's abilities management implementation
 *************************************************************************************************/
int
wave_core_abilities_register(mtlk_core_t *core)
{
  int res = MTLK_ERR_OK;
  const mtlk_ab_tbl_item_t* ab_tabl;
  mtlk_abmgr_t* abmgr = mtlk_vap_get_abmgr(core->vap_handle);
  const mtlk_ab_tbl_item_t* curr_item;

  if (mtlk_vap_is_ap(core->vap_handle)) {
    ab_tabl = wave_core_abilities_ap_init_tbl;
  } else {
    ab_tabl = wave_core_abilities_sta_init_tbl;
  }

  for(curr_item = ab_tabl; NULL != curr_item->abilities; ++curr_item) {
    res = mtlk_abmgr_register_ability_set(abmgr, curr_item->abilities, curr_item->num_elems);
    if (MTLK_ERR_OK != res) {
      const mtlk_ab_tbl_item_t* rollback_item;
      for(rollback_item = ab_tabl; rollback_item != curr_item; ++rollback_item) {
        mtlk_abmgr_disable_ability_set(abmgr,  rollback_item->abilities, rollback_item->num_elems);
        mtlk_abmgr_unregister_ability_set(abmgr,  rollback_item->abilities, rollback_item->num_elems);
      }
      break;
    }
    mtlk_abmgr_enable_ability_set(abmgr, curr_item->abilities, curr_item->num_elems);
  }

  return res;
}

void
wave_core_abilities_unregister(mtlk_core_t *core)
{
  const mtlk_ab_tbl_item_t* ab_tabl;
  mtlk_abmgr_t* abmgr = mtlk_vap_get_abmgr(core->vap_handle);

  if (mtlk_vap_is_ap(core->vap_handle)) {
    ab_tabl = wave_core_abilities_ap_init_tbl;
  } else {
    ab_tabl = wave_core_abilities_sta_init_tbl;
  }

  for (; NULL != ab_tabl->abilities; ++ab_tabl) {
    mtlk_abmgr_disable_ability_set(abmgr, ab_tabl->abilities, ab_tabl->num_elems);
    mtlk_abmgr_unregister_ability_set(abmgr, ab_tabl->abilities, ab_tabl->num_elems);
  }
}
