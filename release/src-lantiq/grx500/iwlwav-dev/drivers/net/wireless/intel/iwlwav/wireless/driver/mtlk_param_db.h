/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_PARAM_DB_H__
#define __MTLK_PARAM_DB_H__

#include "mtlkdfdefs.h"
#include "mtlk_card_types.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID  GID_PDB
#define LOG_LOCAL_FID  0

/** 
*\file mtlk_param_db.h 

*\brief Parameter database that should serve different modules in the driver

*\defgroup PARAM_DB Parameter database
*\{

   Parameter DB used as a container for driver's parameters. Default values are loaded during initialization and
   API is provided to access (set\get) the parameters. Currently three types of parameters supported - integers, strings and binary.

   There are two main usage scenarios for the param db:
     -# Fast parameter accessor functions
       - Handle to parameter should be retrieved using open function
       - Fast access to parameters, will locks over parameter only
     -# Regular accessor functions
       - There is additional DB lock when using regular functions.
      # Each module should define its own parameters and include the pointer to the array of mtlk_pdb_initial_value in mtlk_pdb_initial_values array in mtlk_param_db_def.c


*/

/* Maximal limit of the length of strings stored in the DB.
   The 4K is optimal size, but can be changed. */
#define PARAM_DB_MAX_STRING_LEN  (4095)

/* Param DB's module  ID */
typedef enum {
  PARAM_DB_MODULE_ID_FIRST = 0,
  PARAM_DB_MODULE_ID_RADIO,
  PARAM_DB_MODULE_ID_CORE,
  PARAM_DB_MODULE_ID_LAST
} param_db_module_id;

/* Core (per VAP) module parameters */
/* Value IDs */
typedef enum {
  PARAM_DB_CORE_ARP_PROXY = 0,   /*!< HS20  */
  PARAM_DB_CORE_BRIDGE_MODE,     /*!< Core Hot-Path (H-P) parameter  */
  PARAM_DB_CORE_AP_FORWARDING,   /*!< Core H-P parameter  */
  PARAM_DB_CORE_MAC_ADDR,        /*!< Core H-P parameter  */
  PARAM_DB_CORE_RELIABLE_MCAST,  /*!< Core H-P parameter - Enable/disable Reliable Multicast */
  PARAM_DB_CORE_BSSID,           /*!< Core H-P parameter - BSSID */

  PARAM_DB_CORE_STA_FORCE_SPECTRUM_MODE,   /*!< Core parameter  */
  PARAM_DB_CORE_NICK_NAME,      /*!< Core parameter - IW Nick name */
  PARAM_DB_CORE_ESSID,          /*!< Core parameter - ESSID */
  PARAM_DB_CORE_NET_MODE_CFG,   /*!< Core parameter - configured Network mode*/
  PARAM_DB_CORE_NET_MODE_CUR,   /*!< Core parameter - current Network mode*/

  PARAM_DB_CORE_HIDDEN_SSID,    /*!< use hidden_ssid from hostapd */
  PARAM_DB_CORE_BASIC_RATES_2,  /*!< basic rates from hostapd for 2.4 GHz*/
  PARAM_DB_CORE_BASIC_RATES_5,  /*!< basic rates from hostapd for 5 GHz*/
  PARAM_DB_CORE_RX_MCS_BITMASK, /*!< RX MCS bitmask from the beacon */
  PARAM_DB_CORE_VHT_MCS_NSS,    /*!< VHT_MCS_NSS from the beacon */
  PARAM_DB_CORE_HE_MCS_NSS,     /*!< HE_MCS_NSS from the beacon */
  PARAM_DB_CORE_HE_OPERATION,   /*!< HE_OPERATION from the beacon */
  PARAM_DB_CORE_SET_BSS_FLAGS,  /*!< additional flags to set at SET_BSS time */
  /* Note---these have to come in the same order as AC queue defines in enum UMI_AC */
  PARAM_DB_CORE_WMM_PARAMS_BE,  /*!< WMM parameters for AC BE from hostapd */
  PARAM_DB_CORE_WMM_PARAMS_BK,  /*!< WMM parameters for AC BK from hostapd */
  PARAM_DB_CORE_WMM_PARAMS_VI,  /*!< WMM parameters for AC VI from hostapd */
  PARAM_DB_CORE_WMM_PARAMS_VO,  /*!< WMM parameters for AC V0 from hostapd */
  PARAM_DB_CORE_IS_BSS_LOAD_ENABLE,       /*!< is bss load ie enable value */
  PARAM_DB_CORE_ADMISSION_CAPACITY,       /*!< available admiision capacity value for bss load ie*/

  PARAM_DB_CORE_STA_LIMIT_MIN,
  PARAM_DB_CORE_STA_LIMIT_MAX,

  /* RX Threshold */
  PARAM_DB_CORE_RX_THRESHOLD,             /*!< Mirror of FW's "Set High Reception Threshold": UMI_RX_TH.rxThValue */

  /* Aggregation configuration */
  PARAM_DB_CORE_AMSDU_MODE,               /*!< AMSDU mode */
  PARAM_DB_CORE_BA_MODE,                  /*!< BA mode */
  PARAM_DB_CORE_WINDOW_SIZE,              /*!< Dynamic window size */
  PARAM_DB_CORE_TWT_OPERATION_MODE,       /*!< TWT operation mode */
  PARAM_DB_CORE_IWPRIV_FORCED,            /*!< core parameter - settings, forced by iwpriv call */
  PARAM_DB_CORE_HT_PROTECTION,            /*!< Protection method */

  PARAM_DB_CORE_MESH_MODE,                /*!< Multi-AP mode for the current AP */
  PARAM_DB_CORE_DTIM_PERIOD,              /*!< DTIM_PERIOD */

  PARAM_DB_CORE_MBSSID_VAP,               /*!< Set MBSSID VAP */
  PARAM_DB_CORE_MBSSID_NUM_VAPS_IN_GROUP, /*!< Set MBSSID number of VAPs in group */

  PARAM_DB_CORE_SOFTBLOCK_DISABLE,        /*!< Set softblock disable */
  PARAM_DB_CORE_MGMT_FRAMES_RATE,         /*!< Management frames rate */

  PARAM_DB_CORE_BEACON_PERIOD,            /*!< MIB_BEACON_PERIOD */

  PARAM_DB_CORE_HE_NON_ADVERTISED,        /*!< Non advertised HE capabilities */

  PARAM_DB_CORE_CSA_DEAUTH_PARAMS,        /*!< CSA deauth multicast frames data */

  PARAM_DB_CORE_RNR_6G_SHORT_SSID_LIST,   /*!< 6GHz VAPs Short SSID List */

  PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT,  /*!< 6GHz VAPs Short SSID List size */

  PARAM_DB_CORE_HE_DEBUG_DATA,	          /*!< HE debug data */

  PARAM_DB_CORE_HE_BEACON,                /*!< Beacon in HE SU PPDU format */

  PARAM_DB_CORE_SHORT_SSID,               /*!< Computed Short SSID */        

  PARAM_DB_CORE_FWRD_UNKWN_MCAST_FLAG,    /*!< Core H-P parameter - unknown multicast filter mode */

  PARAM_DB_CORE_BCAST_TWT_OPERATION_MODE, /*!< Broadcast TWT operation mode */

  PARAM_DB_CORE_BCAST_TWT_CONFIG,         /*!< Broadcast TWT schedule(s) advertised */

  /* SoftBlock timer ACL control for probe requests */
  PARAM_DB_CORE_SB_TIMER_ACL_ON,          /*!< SoftBlock Timer ACL is ON */
  PARAM_DB_CORE_SB_TIMER_ACL_PROBE_DROP,  /*!< SoftBlock Timer ACL: drop all unicast probe requests */
  PARAM_DB_CORE_AP_RETRY_LIMIT_EXCE,      /*!< AP retry limit value excessive */

  PARAM_DB_CORE_LAST_VALUE_ID,            /*!< Last parameter ID */
} mtlk_pdb_id_t;/*!< \Enum of the parameters IDs. When adding parameter - extend this enum */

/* Radio (per radio) module parameters */
/* Value IDs */
typedef enum {
  PARAM_DB_RADIO_TEST_PARAM0 = PARAM_DB_CORE_LAST_VALUE_ID,  /*!< Test parameter  */
  PARAM_DB_RADIO_TEST_PARAM1,       /*!< Test parameter  */
  PARAM_DB_RADIO_TX_ANTENNAS,       /*!< Radio MIB parameter - transmission antenna list*/
  PARAM_DB_RADIO_RX_ANTENNAS,       /*!< Radio MIB parameter - reception antenna list*/
  PARAM_DB_RADIO_INTERFDET_THRESHOLD,                       /*!< notification threshold for firmware */
  PARAM_DB_RADIO_PROG_MODEL_SPECTRUM_MODE,                  /*!< radio parameter  */
  PARAM_DB_RADIO_SPECTRUM_MODE,                             /*!< radio parameter - spectrum mode */
  PARAM_DB_RADIO_BONDING_MODE,                              /*!< radio parameter - bonding mode */
  PARAM_DB_RADIO_CHANNEL_CFG,                               /*!< radio parameter configured channel */
  PARAM_DB_RADIO_CHANNEL_CUR,                               /*!< radio parameter current channel*/
  PARAM_DB_RADIO_CHANNEL_PREV,                              /*!< radio parameter previous channel*/
  PARAM_DB_RADIO_POWER_SELECTION,                           /*!< radio parameter  */
  /* probably move to per card or remove at all*/
  PARAM_DB_RADIO_MAC_SOFT_RESET_ENABLE,                     /*!< radio parameter - MAC reset control: automatically on MAC assert/exception */
  PARAM_DB_RADIO_MAC_WATCHDOG_TIMER_TIMEOUT_MS,             /*!< radio parameter - set timeout for MAC watchdog message sending */
  PARAM_DB_RADIO_MAC_WATCHDOG_TIMER_PERIOD_MS,              /*!< radio parameter - set MAC watchdog checking timer period */
  PARAM_DB_RADIO_FREQ_BAND_CFG,                             /*!< radio parameter - configured Frequency band*/
  PARAM_DB_RADIO_FREQ_BAND_CUR,                             /*!< radio parameter - current Frequency band*/
  PARAM_DB_RADIO_TX_POWER_REG_LIM,                          /*!< radio parameter - TX power regulatory limit */
  PARAM_DB_RADIO_TX_POWER_PSD,                              /*!< radio parameter - TX power limit according to PSD per current BW */
  PARAM_DB_RADIO_TX_POWER_CFG,                              /*!< radio parameter - TX power limit configured per current BW */
  PARAM_DB_RADIO_SHORT_CYCLIC_PREFIX,                       /*!< radio MIB parameter - use short cyclic prefix*/
  PARAM_DB_RADIO_POWER_INCREASE,                            /*!< radio MIB parameter - power increase vs duty circle*/
  PARAM_DB_RADIO_SM_ENABLE,                                 /*!< radio MIB parameter - channel announcement enabled*/
  PARAM_DB_RADIO_STBC,                                      /*!< MIB_USE_SPACE_TIME_BLOCK_CODE */
  PARAM_DB_RADIO_RTS_THRESH,                                /*!< MIB_RTS_THRESHOLD */
  PARAM_DB_RADIO_ADVANCED_CODING,                           /*!< MIB_ADVANCED_CODING_SUPPORTED */
  PARAM_DB_RADIO_OFDM_PROTECTION,                           /*!< MIB_OFDM_PROTECTION_METHOD */
  PARAM_DB_RADIO_OVERLAPPING_PROT,                          /*!<MIB_OVERLAPPING_PROTECTION_ENABLE */
  PARAM_DB_RADIO_MSDU_LIFETIME,                             /*!< MIB_TX_MSDU_LIFETIME */
  PARAM_DB_RADIO_POWER_LIMIT,                               /*!< UM_MAN_CHANGE_TX_POWER_LIMIT_REQ */
  PARAM_DB_RADIO_SHORT_PREAMBLE,                            /*!< use_short_preamble from hostapd */
  PARAM_DB_RADIO_SCAN_PARAMS,                               /*!< scan parameters (wait times, probe stuff, valid times */
  PARAM_DB_RADIO_SCAN_PARAMS_BG,                            /*!< scan parameters (wait times, probe stuff, chunk stuff */
  PARAM_DB_RADIO_SCAN_MODIFS,                               /*!< scan modifier flags */
  PARAM_DB_RADIO_SCAN_PAUSE_BG_CACHE,                       /*!< scan support bg scan pause */
  PARAM_DB_RADIO_11B_ANTSEL_RATE,
  PARAM_DB_RADIO_11B_ANTSEL_RXANT,
  PARAM_DB_RADIO_11B_ANTSEL_TXANT,
  PARAM_DB_RADIO_COUNTRY_CODE_ALPHA,                        /*!< country code as alpha */
  PARAM_DB_RADIO_CCA_THRESHOLD,                             /*!< CCA threshold */
  PARAM_DB_RADIO_CCA_ADAPT,                                 /*!< CCA adaptive intervals */
  PARAM_DB_RADIO_TX_POWER_LIMIT_OFFSET,                     /*!< Mirror of FW' "Set Power Limit": UMI_TX_POWER_LIMIT.powerLimitOffset */
  PARAM_DB_RADIO_QAMPLUS_MODE,                              /*!< QAMplus mode */
  PARAM_DB_RADIO_MODE_CURRENT,                              /*!< Current actual radio mode */
  PARAM_DB_RADIO_MODE_REQUESTED,                            /*!< Requested radio mode */
  PARAM_DB_RADIO_ACS_UPDATE_TO,                             /*!< AOCS update timeout */
  PARAM_DB_RADIO_MU_OPERATION,                              /*!< MU operation */
  PARAM_DB_RADIO_RADAR_RSSI_TH,                             /*!< Radar Detection RSSI threshold */
  PARAM_DB_RADIO_RTS_DYNAMIC_BW,                            /*!< Signaling of RTS frame with dynamic bw */
  PARAM_DB_RADIO_RTS_STATIC_BW,                             /*!< Signaling of RTS frame with static bw */
  PARAM_DB_RADIO_BF_MODE,                                   /*!< Beamforming Mode */
  PARAM_DB_RADIO_ACTIVE_ANT_MASK,                           /*!< Active Antenna Mask */
  PARAM_DB_RADIO_TXOP_CFG,                                  /*!< TXOP Parameters */
  PARAM_DB_RADIO_FREQ_JUMP_MODE,                            /*!< Frequency Jump Mode */
  PARAM_DB_RADIO_ONLINE_ACM,                                /*!< Radio parameter: online calibration algo mask*/
  PARAM_DB_RADIO_CALIBRATION_ALGO_MASK,                     /*!< Radio parameter: offline calibration algo mask*/
  PARAM_DB_RADIO_AGG_RATE_LIMIT_MODE,                       /*!< Mirror of FW's Aggregation-Rate Limit: UMI_AGG_RATE_LIMIT.mode */
  PARAM_DB_RADIO_AGG_RATE_LIMIT_MAXRATE,                    /*!< Mirror of FW's Aggregation-Rate Limit: UMI_AGG_RATE_LIMIT.maxRate */
  PARAM_DB_RADIO_DYNAMIC_MC_RATE_MODE,                      /*!< Mirror of FW's Multicast Rate : UM_MAN_DMR_CONFIG_REQ.mode */
  PARAM_DB_RADIO_DYNAMIC_MC_RATE_MAXRATE,                   /*!< Mirror of FW's Multicast Rate : UM_MAN_DMR_CONFIG_REQ.maxRate*/
  PARAM_DB_RADIO_COEX_MODE,                                 /*!< 2.4 GHz Coex (BT, BLE, ZigBee) mode */
  PARAM_DB_RADIO_TPC_RESERVED,                              /*!< Radio parameter - TPC Reserved */
  PARAM_DB_RADIO_COEX_ACTIVE_TIME,                          /*!< 2.4 GHz Coex period of time [mSec] that WIFI is allowed to transmit */
  PARAM_DB_RADIO_COEX_INACTIVE_TIME,                        /*!< 2.4 GHz Coex period of time [mSec] that WIFI is not allowed to transmit */
  PARAM_DB_RADIO_COEX_CTS2SELF_ACTIVE,                      /*!< 2.4 GHz Coex CTS2Self is activated - true (1), otherwise - false (0) */
  PARAM_DB_RADIO_TPC_LOOP_TYPE,                             /*!< Radio parameter - TPC loop type */
  PARAM_DB_RADIO_TPC_PW_LIMITS_PSD,                         /*!< Radio parameter - TPC power limits according to PSD for all BW */
  PARAM_DB_RADIO_TPC_PW_LIMITS_CFG,                         /*!< Radio parameter - TPC power limits configured for all BW */
  PARAM_DB_RADIO_DFS_RADAR_DETECTION,                       /*!< DFS params */
  PARAM_DB_RADIO_RX_DUTY_CYCLE_ON_TIME,                     /*!< Mirror of FW' "Set Reception Duty Cycle": UMI_RX_DUTY_CYCLE.onTime */
  PARAM_DB_RADIO_RX_DUTY_CYCLE_OFF_TIME,                    /*!< Mirror of FW' "Set Reception Duty Cycle": UMI_RX_DUTY_CYCLE.offTime */
  PARAM_DB_RADIO_AMSDU_NUM,                                 /*!< AMSDU num */
  PARAM_DB_RADIO_AMSDU_VNUM,                                /*!< AMSDU vnum */
  PARAM_DB_RADIO_AMSDU_HENUM,                               /*!< AMSDU henum */
  PARAM_DB_RADIO_MAX_MPDU_LENGTH,                           /*!< MAX MPDU length */
  PARAM_DB_RADIO_TF_PARAMS,                                 /*!< Trigger frame params for WCS AX Demo */
  PARAM_DB_RADIO_SSB_MODE,                                  /*!< SSB Mode */
  PARAM_DB_RADIO_SHORT_SLOT,                                /*!< use_short_slot_time from hostapd */
  PARAM_DB_RADIO_UNCONNECTED_STA_SCAN_TIME,                 /*!< Unconnected STA scan time */
  PARAM_DB_RADIO_RESTRICTED_AC_MODE,                        /*!< Restricted AC Mode */
  PARAM_DB_RADIO_PD_THRESHOLD,                              /*!< PD Threshold */
  PARAM_DB_RADIO_FAST_DROP,                                 /*!< Fast Drop */
  PARAM_DB_RADIO_WLAN_COUNTERS_SRC,                         /*!< WLAN counters source switch */
  PARAM_DB_RADIO_RTS_CUTOFF_POINT,                          /*!< RTS protection rate cuttoff point */
  PARAM_DB_RADIO_11N_ACAX_COMPAT,                           /*!< 11n, 11ac and 11ax compatibility */
  PARAM_DB_RADIO_DYNAMIC_MU_TYPE,                           /*!< Dynamic MU Type */
  PARAM_DB_RADIO_HE_MU_FIXED_PARAMTERS,                     /*!< HE MU Fixed Parameters */
  PARAM_DB_RADIO_HE_MU_DURATION,                            /*!< HE MU Duration */
  PARAM_DB_RADIO_STATIONS_STATS_ENABLED,                    /*!< Stations statistics enabled */
  PARAM_DB_RADIO_ZWDFS_ANT_ACTIVE,                          /*!< ZWDFS Antenna Enabled/Disable */
  PARAM_DB_RADIO_ETSI_PPDU_DURATION_LIMITS,                 /*!< ETSI PPDU Duration Limits */
  PARAM_DB_RADIO_RTS_THRESHOLD,                             /*!< RTS threshold */
  PARAM_DB_RADIO_AP_RETRY_LIMIT,                            /*!< AP retry limit value Mgmt */
  PARAM_DB_RADIO_AP_RETRY_LIMIT_DATA,                       /*!< AP retry limit value Data */
  PARAM_DB_RADIO_CTS_TO_SELF_TO,                            /*!< CTS to self timeout value */
  PARAM_DB_RADIO_TX_AMPDU_DENSITY,                          /*!< Tx AMPDU Density */
  PARAM_DB_RADIO_MU_OFDMA_BF_MODE,                          /*!< Mirror of FW's MU OFDMA BF UMI_MU_OFDMA_BF_CONFIG.mode */
  PARAM_DB_RADIO_MU_OFDMA_BF_PERIOD,                        /*!< Mirror of FW's MU OFDMA BF UMI_MU_OFDMA_BF_CONFIG.bfPeriod */
  PARAM_DB_RADIO_COLLECT_PROBE_REQ,                         /*!< Whether or not to collect probe requests for statistics */
  PARAM_DB_RADIO_PREAMBLE_PUNCTURE_CFG,
  PARAM_DB_RADIO_SLOW_PROBING_MASK,                         /*!< Slow Probing Mask */
  PARAM_DB_RADIO_FIXED_LTF_AND_GI,                          /*!< Fixed LTF and GI */
  PARAM_DB_RADIO_WHM_ENABLE,                                /*!< WHM Enable Main flag */
  PARAM_DB_RADIO_WHM_FW_WARN,                               /*!< WHM FW warn enable */
  PARAM_DB_RADIO_WHM_PHY_WARN,                              /*!< WHM PHY warn enable */

#ifdef CONFIG_WAVE_DEBUG
  PARAM_DB_RADIO_FIXED_PWR,                                 /*!< Fixed TX management power */
  PARAM_DB_RADIO_IRE_CTRL_B,                                /*!< IRE Control B */
  PARAM_DB_RADIO_TEST_BUS_MODE,                             /*!< Test Bus mode */
  PARAM_DB_RADIO_PLAN_MU_GROUP_STATS,                       /*!< HE MU Group manager statistics */
#endif
  PARAM_DB_RADIO_FILS_DISCOVERY_FRAME,                      /*!< FILS Discovery frame */
  PARAM_DB_RADIO_UNSOLICIT_FRAME_CFG_MODE,                  /*!< Unsolicited frame tx cfg mode */
  PARAM_DB_RADIO_UNSOLICIT_FRAME_CFG_DURATION,              /*!< Unsolicited frame tx cfg duration */
  PARAM_DB_RADIO_UNSOLICIT_PROBE_RESP_TEMPL,                /*!< Unsolicited Probe Response template */
  PARAM_DB_RADIO_UNSOLICIT_PROBE_RESP_TEMPL_LEN,            /*!< Unsolicited Probe Response template len*/
  PARAM_DB_RADIO_UNSOLICIT_TX_FRAME_INFO,                   /*!< Unsolicited frame tx info*/
  PARAM_DB_RADIO_LAST_VALUE_ID                              /*!< Last parameter ID */
} mtlk_pdb_radio_id_t;                                      /*!< \Enum of the parameters IDs. When adding parameter - extend this enum */

/* Possible value types */
#define PARAM_DB_TYPE_INT     0x01    /*!< Integer type */
/*It's recommended to initialize STRING parameters
 * using maximum possible string length value + 1 (for zero)*/
#define PARAM_DB_TYPE_STRING  0x02    /*!< String type */
#define PARAM_DB_TYPE_BINARY  0x04    /*!< Binary type */
#define PARAM_DB_TYPE_MAC     0x08    /*!< Binary type */

/* Value flags */
#define PARAM_DB_VALUE_FLAG_NO_FLAG       0x00    /*!< No flags defined, should be used to avoid the usage of magic numbers */
#define PARAM_DB_VALUE_FLAG_READONLY      0x01    /*!< Read only - calling setters will cause assertion */
#define PARAM_DB_VALUE_FLAG_UNINITIALIZED 0x02    /*!< Parameter was uninitialized - calling accessors will cause assertion */


/* Special flags. */

/* Set if PARAM_DB_CORE_HT_PROTECTION was changed by iwpriv (iwpriv has higher priority). */
#define PARAM_DB_CORE_IWPRIV_FORCED_HT_PROTECTION_FLAG  (0x01)

typedef uint32 mtlk_pdb_size_t;        /*!<  Parameter size type */

typedef const struct _mtlk_pdb_initial_value {
  uint32 id;                /*!< \private ID of the parameter */
  uint32 type;              /*!< \private Type of the parameter */
  uint32 flag;              /*!< \private Flags of the parameter */
  uint32 size;              /*!< \private Size of the parameter */
  const void * value;       /*!< \private Pointer to the memory holding parameters value */
}__MTLK_IDATA mtlk_pdb_initial_value; /*!<  Initial value - should be used only to define initial values of the parameters*/

extern const mtlk_pdb_initial_value *mtlk_pdb_initial_values[];

extern const mtlk_pdb_initial_value *wave_pdb_initial_values_pcieg6_wrx300[];
extern const mtlk_pdb_initial_value *wave_pdb_initial_values_pcieg6_wrx500[];

extern const mtlk_pdb_initial_value *wave_pdb_initial_values_radio[];

typedef struct _private_mtlk_pdb_value_t
{
  mtlk_pdb_size_t size; /*!< \private Size of the parameter */
  uint32 flags;         /*!< \private Flags of the parameter */
  uint32 type;          /*!< \private Type of the parameter */
  mtlk_osal_spinlock_t param_lock;   /*!< \private Access lock for the parameter */
  mtlk_pdb_t * parent;               /*!< \private Pointer to parent param db object*/

  union {
    void * value_ptr;               /*!< \private Placeholder for the pointer to string or binary values*/
    mtlk_atomic_t value_int;        /*!< \private Placeholder for the integer values*/
  } value;

}__MTLK_IDATA mtlk_pdb_value_t; /*!<  Parameter db value (parameter) */

typedef mtlk_pdb_value_t * mtlk_pdb_handle_t; /*!<  Parameter's handle*/


/*! Create param db object

    \return  mtlk_pdb_t*    Allocated param db object
*/
mtlk_pdb_t* __MTLK_IFUNC wave_pdb_create(param_db_module_id module_id, mtlk_card_type_t hw_type, mtlk_card_type_info_t hw_type_info);

/*! Deletes param db object with clean up

    \param  obj    Param db object to be deleted
*/

void __MTLK_IFUNC wave_pdb_delete (mtlk_pdb_t *obj);

/*! Retrieves the value of parameter of the integer type

    \param  obj     Param db object 
    \param  id      Parameter's ID

    \return value   parameter's value
*/
int __MTLK_IFUNC wave_pdb_get_int(mtlk_pdb_t* obj, mtlk_pdb_id_t id);

/*! Retrieves the value of parameter of the string type

    \param  obj     Param db object 
    \param  id      Parameter's ID
    \param  value   Pointer to variable that will receive parameter's value
    \param  size   Pointer to variable that will receive parameter's size, when calling the function should be set to the buffer size

    \return MTLK_ERR_OK   Parameter retrieved successfully
    \return MTLK_ERR_BUF_TOO_SMALL   Provided buffer is too small, size will be set to needed buffer size
*/
int __MTLK_IFUNC wave_pdb_get_string(mtlk_pdb_t* obj, mtlk_pdb_id_t id, char * value, mtlk_pdb_size_t * size);

/*! Retrieves the value of parameter of the binary type

    \param  obj     Param db object 
    \param  id      Parameter's ID
    \param  value   Pointer to variable that will receive parameter's value
    \param  size   Pointer to variable that will receive parameter's size, when calling the function should be set to the buffer size

    \return MTLK_ERR_OK   Parameter retrieved successfully
    \return MTLK_ERR_BUF_TOO_SMALL   Provided buffer is too small, size will be set to needed buffer size
*/
int __MTLK_IFUNC
wave_pdb_get_binary(mtlk_pdb_t* obj, mtlk_pdb_id_t id, void * buffer, mtlk_pdb_size_t * size);

/*! Retrieves the value of parameter of the MAC type

    \param  obj     Param db object
    \param  id      Parameter's ID
    \param  mac     Pointer to variable that will receive MAC's value
*/
void __MTLK_IFUNC
wave_pdb_get_mac(mtlk_pdb_t* obj, mtlk_pdb_id_t id, void * mac);

/*! Compares the value of parameter of the MAC type

    \param  obj     Param db object
    \param  id      Parameter's ID
    \param  mac     Pointer to variable for comparison that contains MAC's value

    \return 0       if parameter's buffer identical to supplied buffer
    \return !0      if parameter's buffer non-identical to supplied buffer
*/
unsigned __MTLK_IFUNC
mtlk_pdb_cmp_mac(mtlk_pdb_t* obj, mtlk_pdb_id_t id, void * mac);

/*! Sets the value of parameter of the integer type

    \param  obj     Param db object 
    \param  id      Parameter's ID
    \param  value   New parameter's value

*/
void __MTLK_IFUNC wave_pdb_set_int(mtlk_pdb_t* obj, mtlk_pdb_id_t id, uint32 value);

/*! Sets the value of parameter of the string type

    \param  obj     Param db object 
    \param  id      Parameter's ID
    \param  value   New parameter's value

    \return MTLK_ERR_OK   Parameter retrieved successfully
    \return MTLK_ERR_PARAMS   Zero size of the new value
    \return MTLK_ERR_NO_MEM   Memory allocation failed
*/
int __MTLK_IFUNC wave_pdb_set_string(mtlk_pdb_t* obj, mtlk_pdb_id_t id, const char * value);

/*! Sets the value of the parameter of the binary type

    \param  obj     Param db object 
    \param  id      Parameter's ID
    \param  value   New parameter's value

    \return MTLK_ERR_OK   Parameter retrieved successfully
    \return MTLK_ERR_PARAMS   Zero size of the new value
    \return MTLK_ERR_NO_MEM   Memory allocation failed
*/
int __MTLK_IFUNC
wave_pdb_set_binary(mtlk_pdb_t* obj, mtlk_pdb_id_t id, const void * buffer, mtlk_pdb_size_t size);

/*! Sets the value of the parameter of the MAC type

    \param  obj     Param db object
    \param  id      Parameter's ID
    \param  value   New MAC's value
*/
void __MTLK_IFUNC
wave_pdb_set_mac(mtlk_pdb_t* obj, mtlk_pdb_id_t id, const void *mac);

/* Fast access functions - work with mtlk_pdb_handle_t class*/

/*! Gets handle to parameter for fast access. All open handles should be closed using wave_pdb_close function

    \param  obj     Param db object 
    \param  id      Parameter's ID

    \return mtlk_pdb_handle_t   valid handle for the parameter
*/
mtlk_pdb_handle_t __MTLK_IFUNC wave_pdb_open(mtlk_pdb_t* obj, mtlk_pdb_id_t id);

/*! Parameter handle cleanup

\param  handle     Handle to parameter that was opened using wave_pdb_open

*/
void __MTLK_IFUNC wave_pdb_close(mtlk_pdb_handle_t handle);

/*! Tests parameter for set flags

    \param  handle     Handle to parameter that was opened using wave_pdb_open
    \param  flag     Flag(s) to test

    \return TRUE   Specified flag(s) are set.
    \return FALSE   Specified flag(s) are not set.
*/
static int __INLINE mtlk_pdb_is_param_flag_set(mtlk_pdb_handle_t handle, uint32 flag) {
  return !!(handle->flags & flag);
}

/*! Tests parameter to be of specific type

    \param  handle     Handle to parameter that was opened using wave_pdb_open

    \return TRUE   Parameter is of the specified type
    \return FALSE  Parameter is not of the specified type
*/
static int __INLINE mtlk_pdb_is_param_of_type(mtlk_pdb_handle_t handle, uint32 type) {
  return (handle->type == type);
}

/*! Fast get for integer parameters

    \param  handle  Handle to parameter that was opened using wave_pdb_open

    \return parameter's value
*/
static int __INLINE mtlk_pdb_fast_get_int(mtlk_pdb_handle_t handle) {
  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_INT));

  return mtlk_osal_atomic_get(&handle->value.value_int);
}

/*! Fast get for string parameters

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  value   Pointer to variable that will receive string value
    \param  size    Pointer to variable that
                    will receive current maximum string size (including zero character),
                    when calling the function should be set to the value buffer size (including zero character)

    \return MTLK_ERR_OK  Parameter retrieved successfully
    \return MTLK_ERR_BUF_TOO_SMALL   Provided buffer is too small,
                                     size will be set to needed buffer size (including zero character)
*/
static int __INLINE mtlk_pdb_fast_get_string(mtlk_pdb_handle_t handle, char * value, mtlk_pdb_size_t * size) {
  int result = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != value);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_STRING));

  mtlk_osal_lock_acquire(&handle->param_lock);

  /* size parameter is used just for checking here */
  if(*size < handle->size) {
    result = MTLK_ERR_BUF_TOO_SMALL;
    goto end;
  }

  /* Copy string into output buffer with size "*size" */
  wave_strcopy(value, handle->value.value_ptr, *size);

end:
  *size = handle->size; /* terminator is included in returned size */
  mtlk_osal_lock_release(&handle->param_lock);

  return result;
}

/*! Fast compare  for binary parameters

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  buffer   Pointer to the buffer that will be compared with parameter
    \param  size   Size of the buffer that need comparison.

    \return <0      if parameter's buffer less than supplied buffer
    \return 0      if parameter's buffer identical to supplied buffer
    \return >0      if parameter's buffer greater than supplied buffer
*/
static int __INLINE mtlk_pdb_fast_cmp_binary(mtlk_pdb_handle_t handle, const void * buffer, mtlk_pdb_size_t size) {
  int result;

  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != buffer);
  MTLK_ASSERT(0 != size);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_BINARY));

  mtlk_osal_lock_acquire(&handle->param_lock);
  if (size != handle->size) {
    result = (int)(handle->size - size);
  }
  else {
    result = memcmp(handle->value.value_ptr, buffer, handle->size);
  }
  mtlk_osal_lock_release(&handle->param_lock);

  return result;
}


/*! Fast get for binary parameters

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  buffer   Pointer to variable that will receive parameter's value
    \param  size   Pointer to variable that will receive parameter's size, when calling the function should be set to the buffer size

    \return MTLK_ERR_OK  Parameter retrieved successfully
    \return MTLK_ERR_BUF_TOO_SMALL   Provided buffer is too small, size will be set to needed buffer size
*/
static int __INLINE mtlk_pdb_fast_get_binary(mtlk_pdb_handle_t handle, void * buffer, mtlk_pdb_size_t * size) {
  int result = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != buffer);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_BINARY));

  mtlk_osal_lock_acquire(&handle->param_lock);

  if(*size < handle->size) {
    result = MTLK_ERR_BUF_TOO_SMALL;
    *size = handle->size;
    goto end;
  }

  wave_memcpy(buffer, *size, handle->value.value_ptr, handle->size);
  *size = handle->size;

end:
  mtlk_osal_lock_release(&handle->param_lock);

  return result;
}

/*! Fast sets the value of parameter of the integer type

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  value   New parameter's value

    \return MTLK_ERR_OK   Parameter retrieved successfully
*/
static void __INLINE mtlk_pdb_fast_set_int(mtlk_pdb_handle_t handle, uint32 value) {
  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_READONLY));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_INT));

  mtlk_osal_atomic_set(&handle->value.value_int, value);
}


/*! Fast sets the value of parameter of the string type

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  value   New parameter's value, NUL-terminated

    \return MTLK_ERR_OK   Parameter retrieved successfully
    \return MTLK_ERR_PARAMS   Zero size of the new value
    \return MTLK_ERR_NO_MEM   Memory allocation failed
*/
static int __INLINE mtlk_pdb_fast_set_string(mtlk_pdb_handle_t handle, const char * value) {
  int result = MTLK_ERR_OK;
  size_t size;

  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != value);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_READONLY));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_STRING));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_STRING));

  size = wave_strlen(value, PARAM_DB_MAX_STRING_LEN);
  if (PARAM_DB_MAX_STRING_LEN == size) {
     result = MTLK_ERR_PARAMS;
     goto end;
  }

  mtlk_osal_lock_acquire(&handle->param_lock);

  if (size > (handle->size - 1)) {    /* if current string buffer size is enough, do not reallocate memory */
    /* first try to allocate memory, only then release the existing buffer */
    char * temp_str_ptr = (char *)mtlk_osal_mem_alloc(size + 1, MTLK_MEM_TAG_PARAM_DB);

    if(!temp_str_ptr) {
      result = MTLK_ERR_NO_MEM;
      goto end;
    }

    if(handle->value.value_ptr) {
      mtlk_osal_mem_free(handle->value.value_ptr);
    }

    handle->value.value_ptr = temp_str_ptr;
    handle->size = size + 1; /* note that the saved size includes the terminator-byte */
  }

  /* Copy string including terminator-byte */
  wave_strcopy(handle->value.value_ptr, value, handle->size);


end:
  mtlk_osal_lock_release(&handle->param_lock);

  return result;
}

/*! Fast sets the value of parameter of the binary type

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  value   New parameter's value
    \param  size    Size of the new parameter's value

    \return MTLK_ERR_OK   Parameter retrieved successfully
    \return MTLK_ERR_PARAMS   Zero size of the new value
    \return MTLK_ERR_NO_MEM   Memory allocation failed
*/
static int __INLINE mtlk_pdb_fast_set_binary(mtlk_pdb_handle_t handle, const void * buffer, mtlk_pdb_size_t size) {
  int result = MTLK_ERR_OK;
  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != buffer);

  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_READONLY));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_BINARY));

  if(!size) {
    return MTLK_ERR_PARAMS;
  }

  mtlk_osal_lock_acquire(&handle->param_lock);

  if(size != handle->size) {    /* if same size, do not reallocate memory */
    /* first try to allocate memory, only then release the existing buffer */
    uint8 * temp_ptr =  (uint8 *)mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_PARAM_DB);

    if(!temp_ptr) {
      result = MTLK_ERR_NO_MEM;
      goto end;
    }

    if(handle->value.value_ptr) {
      mtlk_osal_mem_free(handle->value.value_ptr);
    }
    handle->value.value_ptr = temp_ptr;
  }

  handle->size = size;
  wave_memcpy(handle->value.value_ptr, size, buffer, size);

end:
  mtlk_osal_lock_release(&handle->param_lock);

  return result;
}

/*! Fast get for mac parameters

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  buffer   Pointer to variable that will receive parameter's value

*/
static void __INLINE mtlk_pdb_fast_get_mac(mtlk_pdb_handle_t handle, void * buffer) {
  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != buffer);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_MAC));
  MTLK_ASSERT(ETH_ALEN == handle->size);

  mtlk_osal_lock_acquire(&handle->param_lock);
  mtlk_osal_copy_eth_addresses(buffer, handle->value.value_ptr);
  mtlk_osal_lock_release(&handle->param_lock);
}


/*! Fast sets the value of parameter of the mac type
    As the size of mac is known we can optimize accessor function

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  value   New parameter's value
*/
static void __INLINE mtlk_pdb_fast_set_mac(mtlk_pdb_handle_t handle, const void * buffer) {
  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != buffer);

  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_READONLY));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_MAC));


  mtlk_osal_lock_acquire(&handle->param_lock);
  mtlk_osal_copy_eth_addresses(handle->value.value_ptr, buffer);
  mtlk_osal_lock_release(&handle->param_lock);
}

/*! Fast compare  for mac parameters

    \param  handle  Handle to parameter that was opened using wave_pdb_open
    \param  buffer   Pointer to the buffer that will be compared with parameter
    \param  size   Size of the buffer that need comparison.

    \return 0       if parameter's buffer identical to supplied buffer
    \return !0      if parameter's buffer non-identical to supplied buffer
*/
static unsigned __INLINE mtlk_pdb_fast_cmp_mac(mtlk_pdb_handle_t handle, const void * buffer) {
  unsigned result;

  MTLK_ASSERT(NULL != handle);
  MTLK_ASSERT(NULL != buffer);
  MTLK_ASSERT(!mtlk_pdb_is_param_flag_set(handle, PARAM_DB_VALUE_FLAG_UNINITIALIZED));
  MTLK_ASSERT(mtlk_pdb_is_param_of_type(handle, PARAM_DB_TYPE_MAC));

  mtlk_osal_lock_acquire(&handle->param_lock);
  MTLK_ASSERT(handle->size == ETH_ALEN);
  result = mtlk_osal_compare_eth_addresses(handle->value.value_ptr, buffer);
  mtlk_osal_lock_release(&handle->param_lock);

  return result;
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_PARAM_DB_H__ */
