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
 *  Originaly written by Artem Migaev
 *
 */

#ifndef _FRAME_H_
#define _FRAME_H_

#include "rsn.h"
#include "stadb.h"
#include "mtlk_coreui.h"

#define  MTLK_IDEFS_ON
#define  MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"
#include "ieee80211defs.h"
#include "mhi_ieee_address.h"
#include <linux/ieee80211.h>

#define BASIC_RATESET_FLAG      0x80
#define SUPPORTED_BITRATE_MASK  0x7F

#define BASIC_RATE_SET_11A_6MBPS                 0x00000001
#define BASIC_RATE_SET_11A_9MBPS                 0x00000002
#define BASIC_RATE_SET_11A_12MBPS                0x00000004
#define BASIC_RATE_SET_11A_18MBPS                0x00000008
#define BASIC_RATE_SET_11A_24MBPS                0x00000010
#define BASIC_RATE_SET_11A_36MBPS                0x00000020
#define BASIC_RATE_SET_11A_48MBPS                0x00000040
#define BASIC_RATE_SET_11A_54MBPS                0x00000080
#define BASIC_RATE_SET_11B_1MBPS_LONG            0x00000800
#define BASIC_RATE_SET_11B_2MBPS_SHORT_LONG      0x00001100
#define BASIC_RATE_SET_11B_5DOT5MBPS_SHORT_LONG  0x00002200
#define BASIC_RATE_SET_11B_11MBPS_SHORT_LONG     0x00004400

#define BASIC_RATE_SET_11B_RATES                (BASIC_RATE_SET_11B_1MBPS_LONG |\
                                                 BASIC_RATE_SET_11B_2MBPS_SHORT_LONG |\
                                                 BASIC_RATE_SET_11B_5DOT5MBPS_SHORT_LONG |\
                                                 BASIC_RATE_SET_11B_11MBPS_SHORT_LONG)

#define BASIC_RATE_SET_OFDM_MANDATORY_RATES     (BASIC_RATE_SET_11A_6MBPS  |\
                                                 BASIC_RATE_SET_11A_12MBPS |\
                                                 BASIC_RATE_SET_11A_24MBPS)

#define BASIC_RATE_SET_OFDM_MANDATORY_RATES_MASK    0x00000015
#define BASIC_RATE_SET_CCK_MANDATORY_RATES          0x00007800
#define BASIC_RATE_SET_CCK_MANDATORY_RATES_MASK     0x00007800
#define BASIC_RATE_SET_11N_RATE_MSK                 0xFFFF8000
#define BASIC_RATE_SET_2DOT4_OPERATIONAL_RATE       0x00007F00
#define BSS_DATA_2DOT4_CHANNEL_MIN                  1
#define BSS_DATA_2DOT4_CHANNEL_MAX                  14

/*
 * Reason Codes
*/
#define FRAME_REASON_UNSPECIFIED (1)
#define FRAME_REASON_DISASSOC_DUE_TO_INACTIVITY (4)

/*
 * Frame packed headers
 */
#define FRAME_SUBTYPE_SIZE                      16
#define FRAME_SUBTYPE_MASK                      0x00F0
#define FRAME_SUBTYPE_SHIFT                     4
#define MAX_INFO_ELEMENTS_LENGTH                512
#define COUNTRY_IE_LENGTH                       2
#define COEXISTENCE_IE_LENGTH                   1
#define MIN_INTOLERANT_CHANNELS_IE_LENGTH       2
#define INTOLERANT_CHANNELS_LIST_OFFSET         1
#define OBSS_SCAN_PARAMETERS_LENGTH             14

#define CE_INFORMATION_REQUEST_BIT_MASK     0x1
#define CE_INTOLERANT_BIT_MASK              0x2
#define CE_20MHZ_WIDTH_REQUEST_BIT_MASK     0x4
#define CE_SCAN_EXEMPTION_REQUEST_BIT_MASK  0x8
#define CE_SCAN_EXEMPTION_GRANT_BIT_MASK    0x10

/* todo: remove this define after WLAN_EID_OPMODE_NOTIF added to kernel */
#define WLAN_EID_OPMODE_NOTIF               (enum ieee80211_eid)199

typedef struct {
  uint16    frame_control;
  uint16    duration;
  IEEE_ADDR dst_addr;
  IEEE_ADDR src_addr;
  IEEE_ADDR bssid;
  uint16    seq_control;
} __MTLK_IDATA frame_head_t;

typedef struct {
  uint64 beacon_timestamp;
  uint16 beacon_interval;
  uint16 capability;
} __MTLK_IDATA frame_beacon_head_t;

typedef struct {
  uint8  timestamp[8];
  uint16 beacon_int;
  uint16 capab_info;
  uint8  variable[0];
} __MTLK_IDATA frame_probe_resp_head_t;

typedef struct {
  uint8 id; // actually this is ie_id type! but enum's size is int...
  uint8 length;
} __MTLK_IDATA ie_t;

typedef struct {
  uint8  id;
  uint8  length;
  uint16 station_cnt;
  uint8  channel_utilization;
  uint16 admission_capacity;
} __MTLK_IDATA ie_bss_load_t;

#define GET_IE_DATA_PTR(ie_ptr) ((uint8*)ie_ptr + sizeof(*ie_ptr))

typedef struct {
  uint16 info;
  uint8 ampdu_params;
  uint8 supported_mcs[16];
  uint16 ht_ext_capabilities;
  uint32 txbf_capabilities;
  uint8 asel_capabilities;
} __MTLK_IDATA htcap_ie_t;

typedef struct {
  uint16    fc;                 /* Frame Control            */
  uint16    dur;                /* Duration                 */
  IEEE_ADDR ra;                 /* RA - Receiver Address    */
  IEEE_ADDR ta;                 /* TA - Transmitter Address */
  uint16    ctl;                /* BAR Control              */
  uint16    ssn;                /* Start Sequence Number    */
} __MTLK_IDATA frame_bar_t;

#define ACTION_FRAME_CATEGORY_SPECTRUM_MANAGEMENT          0
#define ACTION_FRAME_CATEGORY_QOS                          1
#define ACTION_FRAME_CATEGORY_DLS                          2
#define ACTION_FRAME_CATEGORY_BLOCK_ACK                    3
#define ACTION_FRAME_CATEGORY_PUBLIC                       4
#define ACTION_FRAME_CATEGORY_RADIO_MEASUREMENT            5
#define ACTION_FRAME_CATEGORY_FAST_BSS_TRANSITION          6
#define ACTION_FRAME_CATEGORY_SA_QUERY                     8
#define ACTION_FRAME_CATEGORY_PROTECTED_DUAL_PUBLIC_ACTION 9
#define ACTION_FRAME_CATEGORY_WNM                          10
#define ACTION_FRAME_CATEGORY_MESH                         13
#define ACTION_FRAME_CATEGORY_MULTIHOP                     14
#define ACTION_FRAME_CATEGORY_VENDOR_SPECIFIC_PROTECTED    126
#define ACTION_FRAME_CATEGORY_VENDOR_SPECIFIC              127

#define ACTION_FRAME_ACTION_ADDBA_REQUEST       0
#define ACTION_FRAME_ACTION_ADDBA_RESPONSE      1
#define ACTION_FRAME_ACTION_DELBA               2

#define ACTION_FRAME_ACTION_PUBLIC_COEXISTENCE  0

typedef struct {
  uint8      category;       /* CategoryCode         */
  uint8      action;         /* ActionCode           */
} __MTLK_IDATA frame_action_head_t; /* Action frame header */

#include "mtlkbfield.h"

#define ADDBA_PARAM_SET_POLICY    MTLK_BFIELD_INFO(1, 1)
#define ADDBA_PARAM_SET_TID       MTLK_BFIELD_INFO(2, 4)
#define ADDBA_PARAM_SET_SIZE      MTLK_BFIELD_INFO(6, 10)

#define ADDBA_SSN_SSN             MTLK_BFIELD_INFO(4, 12)

#define DELBA_PARAM_SET_INITIATOR MTLK_BFIELD_INFO(11, 1)
#define DELBA_PARAM_SET_TID       MTLK_BFIELD_INFO(12, 4)

typedef struct {
  uint8  dlgt;       /* DialogToken          */
  uint16 param_set;  /* BlockAckParameterSet */
  uint16 timeout;    /* BlockAckTimeOut      */
  uint16 ssn;        /* BlockAckStartSn      */
} __MTLK_IDATA frame_ba_addba_req_t;

typedef struct {
  uint8  dlgt;       /* DialogToken          */
  uint16 scode;      /* StatusCode           */
  uint16 param_set;  /* BlockAckParameterSet */
  uint16 timeout;    /* BlockAckTimeOut      */
} __MTLK_IDATA frame_ba_addba_res_t;

typedef struct {
  uint16 param_set;
  uint16 reason;
} __MTLK_IDATA frame_ba_delba_t;

/* HT capabilities info fields */
#define HTCAP_SUP_CHNL_WIDTH_SET 0x0002
#define HTCAP_40MHZ_INTOLERANT   0x4000

/* ERP info field */
#define ERP_INFO_NON_ERP_PRESENT BIT(0)

struct country_constr_t
{
  uint8 first_ch;
  uint8 num_ch;
  int8 max_power;
} __MTLK_IDATA;

struct country_ie_t
{
  uint8 id;
  uint8 length;
  uint8 country[3];
  // if first_ch < 201, channel info
  // else - reg class info, followed by channel info...
  struct country_constr_t constr[1];
} __MTLK_IDATA;

struct power_constr_ie_t
{
  uint8 id;
  uint8 length;
  uint8 power;
} __MTLK_IDATA;

typedef enum {
  MAN_TYPE_ASSOC_REQ,
  MAN_TYPE_ASSOC_RES,
  MAN_TYPE_REASSOC_REQ,
  MAN_TYPE_REASSOC_RES,
  MAN_TYPE_PROBE_REQ,
  MAN_TYPE_PROBE_RES,
  /* 6 - 7 reserved*/
  MAN_TYPE_BEACON = 8,
  MAN_TYPE_ATIM,
  MAN_TYPE_DISASSOC,
  MAN_TYPE_AUTH,
  MAN_TYPE_DEAUTH,
  MAN_TYPE_ACTION
  /* 14 - 15 reserved*/
} man_frame_t;

/* Definitions as per IEEE 802.11 - 2016 std - chapter 9.4.2 for frame validation */
#define IE_EXT_SUPP_RATES_MIN_LEN 1
#define IE_EXT_SUPP_RATES_MAX_LEN 255
#define IE_SUPP_RATES_MIN_LEN 1
#define IE_SUPP_RATES_MAX_LEN 8
#define IE_HT_INFORMATION_LEN 22
#define IE_RSN_MIN_LEN 2
#define IE_DS_PARAM_SET_LEN 1
#define IE_HT_CAPABILITIES_LEN 26
#define IE_VHT_CAPABILITIES_LEN 12
#define IE_PWR_CONSTRAINT_LEN 1
#define IE_COUNTRY_MIN_LEN 3
#define IE_TIM_MIN_LEN 4
#define IE_TIM_MAX_LEN 254
#define IE_ERP_INFO_LEN 1
#define IE_INTERWORKING_MIN_LEN 1
#define IE_INTERWORKING_MAX_LEN 9

/* Definitions as per IEEE P802.11ax/D4.0, February 2019 */
#define IE_HE_CAPABILITIES_MIN_LEN 22

typedef enum {
  IE_SSID,
  IE_SUPPORTED_RATES,
  IE_FH_PARAM_SET,
  IE_DS_PARAM_SET,
  IE_CF_PARAM_SET,
  IE_TIM,
  IE_IBSS,
  IE_COUNTRY,
  IE_FH_PATTRN_PARAMS,
  IE_FH_PATTRM_TABLE,
  IE_REQUEST,
  IE_BSS_LOAD,
  IE_EDCA_PARAM_SET,
  IE_TSPEC,
  IE_TCLAS,
  IE_SCHEDULE,
  IE_CHALLENGE_TXT,
  /* 17...31 are not used */
  IE_PWR_CONSTRAINT = 32,
  IE_PWR_CAPABILITY,
  IE_TPC_REQUEST,
  IE_TPC_REPORT,
  IE_SUPP_CHANNELS,
  IE_CHANNEL_SW_ANN,
  IE_MEASURE_REQUEST,
  IE_MEASURE_REPOR,
  IE_QUIET,
  IE_IBSS_DFS,
  IE_ERP_INFO,
  IE_TS_DELAY,
  IE_TCLAS_PROCESSING,
  IE_HT_CAPABILITIES,
  IE_QOS_CAPABILITY,
  /* 47 unused */
  IE_RSN = 48,
  /* 49 unused */
  IE_EXT_SUPP_RATES = 50,
  IE_AP_CHANNEL_REPORT,
  IE_MOBILITY_DOMAIN = 54,
  IE_DSE_REGISTERED_LOCATION = 58,
  IE_SUPPORTED_OPER_CLASSES,
  IE_EXT_CHANNEL_SW_ANN,
  IE_HT_INFORMATION,
  IE_BSS_AVERAGE_ACCESS = 63,
  IE_ANTENNA,
  IE_MEASURE_PILOT_TRANSMISSION = 66,
  IE_BSS_AVAILABLE_ADMISSION_CAPACITY,
  IE_BSS_AC_ACCESS_DELAY,
  IE_TIME_ADV,
  IE_RM_ENABLED_CAPABILITIES,
  IE_MULTIPLE_BSSID,
  IE_COEXISTENCE,
  IE_INTOLERANT_CHANNELS_REPORT,
  IE_OBSS_SCAN_PARAMETERS,
  IE_SSID_LIST = 84,
  IE_FMS_DESCRIPTOR = 86,
  IE_QOS_TRAFFIC_CAPABILITY = 89,
  IE_INTERWORKING = 107,
  IE_ADV_PROTO,
  IE_QOS_MAP = 110,
  IE_ROAMING_CONS,
  IE_EMERGENCY_ALERT_ID,
  IE_MESH_CONFIG,
  IE_MESH_ID,
  IE_MESH_CHANNEL_SW_PARAMETERS = 118,
  IE_MESH_AWAKE_WINDOW,
  IE_BEACON_TIMING,
  IE_MCCAOP_ADV = 123,
  IE_EXT_CAPABILITIES = 127,
  /* 128...129, 133...136, 149...150, 155...156 are not used */
  IE_MULTI_BAND = 158,
  /* 173 unused */
  IE_MCCAOP_ADV_OVERVIEW = 174,
  /* 176, 178...180 are not used */
  IE_QMF_POLICY = 181,
  IE_QLOAD_REPORT = 186,
  IE_HCCA_TXOP_UPDATE_COUNT,
  IE_VHT_CAPABILITIES = 191,
  IE_VHT_OPERATION,
  IE_TX_PWR_ENVELOPE = 195,
  IE_CHANNEL_SW_WRAPPER,
  IE_REDUCED_NEIGH_REPORT = 201,
  /* 203, 207...215 are not used */
  IE_TWT = 216,
  IE_VENDOR_SPECIFIC = 221,
  /* 222...243 are not used */
  IE_RSNXE = 244,
  /* 245...254 are not used */
  IE_EXTENSION = 255
} ie_id_t;

#define IE_EXT_SHORT_SSID_LIST 58

#define BSS_ESSID_MAX_SIZE 32

typedef struct {
  // Platform dependant stuff
  uint64    beacon_timestamp;
  mtlk_osal_timestamp_t received_timestamp;
  // Calculated
  IEEE_ADDR bssid;
  uint16    bss_type;
  uint16    channel;
  uint8     secondary_channel_offset;

  int8      max_rssi;
  uint8     all_rssi[NUMBER_OF_RX_ANTENNAS];
  int8      noise; /* dBm */
  // From IFs
  uint16    capability;
  uint16    beacon_interval;
  // From IEs
  uint8     info_elements[MAX_INFO_ELEMENTS_LENGTH];
  uint16    ie_used_length;
  uint16    wps_offs;
  uint16    interworking_offs;
  uint16    vsie_offs;
  uint8     *ht_cap_info;
  uint8     *vht_cap_info;
  uint8     *he_cap_info;
  uint8     *he_6ghz_cap_info;
  unsigned  ht_cap_info_len;
  unsigned  vht_cap_info_len;
  unsigned  he_cap_info_len;
  unsigned  he_6ghz_cap_info_len;
  uint8     is_ht;
  uint8     is_vht;
  uint8     is_he;
  uint32    basic_rate_set;
  uint32    operational_rate_set;
  uint8     spectrum;
  uint8     upper_lower;
  int8      power;
  BOOL      forty_mhz_intolerant;
  uint8     dtim_period;
  uint8     erp_info;
} bss_data_t;


#define MGMT_FRAME_BUF_COUNT 16 /* must be power of 2 */

typedef struct {
  frame_head_t   buffer[MGMT_FRAME_BUF_COUNT];
  unsigned       count;
  unsigned       idx;
} mgmt_frame_filter_t;

int mgmt_frame_filter_init(mgmt_frame_filter_t *frame_filter);
void mgmt_frame_filter_cleanup(mgmt_frame_filter_t *frame_filter);
BOOL mgmt_frame_filter_allows (mtlk_core_t *nic, mgmt_frame_filter_t *frame_filter, uint8 *frame, unsigned len, BOOL *is_broadcast);

#define RSN_IE_SIZE(bss) (((bss).rsn_offs == 0xffff)? 0:(bss).info_elements[(bss).rsn_offs+1])
#define RSN_IE(bss) (&(bss).info_elements[(bss).rsn_offs])
#define WPS_IE_SIZE(bss) (((bss).wps_offs == 0xffff)? 0:(bss).info_elements[(bss).wps_offs+1])
#define WPS_IE(bss) (&(bss).info_elements[(bss).wps_offs])
#define WPS_IE_FOUND(bss) ((bss).wps_offs != 0xffff)
#define INTERWORKING_IE_SIZE(bss) (((bss).interworking_offs == 0xffff) ?  \
            0 : (bss).info_elements[(bss).interworking_offs + 1])
#define INTERWORKING_IE(bss) (&(bss).info_elements[(bss).interworking_offs])
#define INTERWORKING_IE_FOUND(bss) ((bss).interworking_offs != 0xffff)
#define VSIE_FOUND(bss) ((bss).vsie_offs != 0xffff)
#define HE_IE_FOUND(bss) ((bss).is_he != 0)

#define BSS_WEP_MASK ((uint16)0x0010)
#define BSS_IS_WEP_ENABLED(bss) (((bss)->capability & BSS_WEP_MASK) ? TRUE: FALSE)

/* BAR Control */
#define IEEE80211_BAR_TID       0xf000
#define IEEE80211_BAR_TID_S     12

typedef struct {
  uint8     rssi;      // Global RSSI in dBm
  uint8     noise;     // Global Noise in dBm
  uint16    reserved;
} __MTLK_IDATA rcv_status_t;

#define MTLK_MGMT_FRAME_DECRYPTED    0x1
#define MTLK_MGMT_FRAME_IV_STRIPPED  0x2
#define MTLK_MGMT_FRAME_MIC_STRIPPED 0x4

typedef struct mtlk_mgmt_frame_data
{
  mtlk_phy_info_t     *phy_info;
  struct mtlk_channel *chan;
  int                  max_rssi;
  uint8                pmf_flags;
  BOOL                 notify_hapd_supplicant;
  BOOL                 probe_req_wps_ie;
  BOOL                 probe_req_interworking_ie;
  BOOL                 probe_req_vsie;
  BOOL                 probe_req_he_ie;
} mtlk_mgmt_frame_data_t;

int mtlk_process_man_frame (mtlk_handle_t context,
                            bool is_sta,
                            bool is_sta_mfp,
                            uint8 *fbuf,
                            int32 *buflen,
                            mtlk_mgmt_frame_data_t *fd);

int mtlk_process_ctl_frame (mtlk_handle_t context,
                            uint8 *fbuf,
                            int32 flen);

/* Move the following prototype to the corresponding cross-platform file
   when it available */
int mtlk_handle_bar(mtlk_handle_t context,
                    uint8 *ta,
                    uint8 tid,
                    uint16 ssn);

#define MTLK_LANTIQ_PROPRIETRY_LDPC      MTLK_BFIELD_INFO(0, 1)
#define RSNE_CAPAB_PBAC BIT(12)

#define RSNE_VERSION_LEN               2
#define RSNE_GROUP_DATA_CSUITE_LEN     4
#define RSNE_PAIRWISE_CSUITE_COUNT_LEN 2
#define RSNE_PAIRWISE_CSUITE_LEN       4
#define RSNE_AKM_SUITE_COUNT_LEN       2
#define RSNE_AKM_SUITE_LEN             4
#define RSNE_CAPAB_LEN                 2

#define WLAN_EID_VENDOR_SPECIFIC_OUI_OFFSET             0
#define WLAN_EID_VENDOR_SPECIFIC_OUI_TYPE_OFFSET        3

/* Intel 4-addr mode */
#define WLAN_EID_INTEL_VENDOR_ATTRIBUTES_OFFSET  4
#define WLAN_EID_INTEL_VENDOR_TYPE_4ADDR_MODE    0x30
#define WLAN_EID_INTEL_VENDOR_4ADDR_MODE_STA     0x01
#define WLAN_EID_INTEL_VENDOR_3ADDR_MODE_STA     0x02

/* Broadcom 4-addr mode */
#define WLAN_EID_BRCM_VENDOR_VERSION_2           2
#define WLAN_EID_BRCM_VENDOR_FLAGS1_OFFSET       6
#define WLAN_EID_BRCM_VENDOR_DWDS_CAPABLE        0x80

#define WLAN_OUI_TYPE_WFA_MULTI_AP               0x1B

#define WLAN_EID_HE_CAP_EXT                             35
#define WLAN_EID_HE_OPER_EXT                            36
#define WLAN_EID_HE_6GHZ_CAP_EXT                        59
#define WLAN_EID_EXTENSION_ID_OFFSET                    2
#define WLAN_ELEM_ID_EXTENSION_LEN                      1

typedef struct {
  const uint16 *rsne_capab;
  const uint8  *bss_coex_20_40;
  const uint8  *vendor_metalink;
  const uint8  *vendor_lantiq;
  const uint8  *vendor_w101;
  const uint8  *vendor_intel;
  const uint8  *vendor_brcm;
  const uint8  *opmode_notif;
  const uint8  *vendor_wfa;
} wv_ie80211_elems;

int __MTLK_IFUNC mtlk_mgmt_parse_elems(uint8 const *start,
                                       uint32 len,
                                       wv_ie80211_elems *elems);

int     mtlk_frame_ie_extract_phy_rates(uint8 *ie_data, int32 length, bss_data_t *bss_data);
uint32  mtlk_frame_oper_rateset_to_net_mode(uint32 oper_rateset, mtlk_hw_band_e band, BOOL is_ht, BOOL is_vht, BOOL is_he);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* _FRAME_H_ */

