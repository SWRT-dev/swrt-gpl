/*! \file  iwlwav.c
 *  \brief Intel vendor NL80211 command implementation as separate iw commands
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include "nl80211.h"
#include "iw.h"

typedef unsigned long long int u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#include "vendor_cmds_copy.h"

#if defined YOCTO
#include <puma_safe_libc.h>
#else
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#define SET_ADDR_LEN 8
#define TEXT_ARRAY_SIZE 256
#define IW_ENCODING_TOKEN_MAX 64

SECTION(iwlwav);

struct print_data
{
	uint8_t num_of_params;
	char print_msg[32];
};

/***************************** SET FUNCTIONS *****************************/
/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command with integer data for sending
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  argc               number of arguments
 *  \param[in]  argv               pointer to arguments, all must be integers
 *  \param[in]  min_num_of_params  minimum number of allowed arguments
 *  \param[in]  max_num_of_params  maximum number of allowed arguments
 *  \param[in]  subcmd             Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *
 *  \return     0 on success, non 0 on error
 */
static int set_int(struct nl_msg *msg,
		   int argc, char **argv,
		   int min_num_of_params,
		   int max_num_of_params,
		   enum ltq_nl80211_vendor_subcmds subcmd)
{
	int *data;
	int array_size = sizeof(int) * argc;
	int i;

	if (!msg)
		return -EFAULT;

	if (argc < min_num_of_params || argc > max_num_of_params)
		return -EINVAL;

	data = (int *) malloc(array_size);
	if (!data)
		return -ENOMEM;

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, subcmd);

	for(i = 0; i < argc; i++)
		if (sscanf(argv[i], "%i", &data[i]) != 1)
			goto nla_put_failure;
	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, array_size, (char *) data);

	free(data);
	return 0;

nla_put_failure:
	free(data);
	return -ENOBUFS;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command with text data for sending
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  argc               number of arguments
 *  \param[in]  argv               pointer to arguments
 *  \param[in]  min_num_of_params  minimum number of allowed arguments
 *  \param[in]  max_num_of_params  maximum number of allowed arguments
 *  \param[in]  subcmd             Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *
 *  \return     0 on success, non 0 on error
 */
static int set_text(struct nl_msg *msg,
		    int argc, char **argv,
		    int min_num_of_params,
		    int max_num_of_params,
		    enum ltq_nl80211_vendor_subcmds subcmd)
{
	char *data;

	if (!msg)
		return -EFAULT;

	if (argc < min_num_of_params || argc > max_num_of_params)
		return -EINVAL;

	data = (char *) malloc(TEXT_ARRAY_SIZE);
	if (!data)
		return -ENOMEM;

	strncpy_s(data, TEXT_ARRAY_SIZE, argv[0], strnlen_s(argv[0], TEXT_ARRAY_SIZE));

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, subcmd);

	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, TEXT_ARRAY_SIZE, data);

	free(data);
	return 0;

nla_put_failure:
	free(data);
	return -ENOBUFS;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command with MAC address for sending
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  argc               number of arguments, must be 1
 *  \param[in]  argv               pointer to arguments, must be in MAC address format
 *  \param[in]  subcmd             Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *
 *  \return     0 on success, non 0 on error
 */
static int set_addr(struct nl_msg *msg,
		    int argc, char **argv,
		    enum ltq_nl80211_vendor_subcmds subcmd)
{
	unsigned int data[SET_ADDR_LEN];
	int i, count;
	uint8_t data8[SET_ADDR_LEN];
	uint16_t sa_family = 1;

	if (!msg)
		return -EFAULT;

	if (argc != 1)
		return -EINVAL;

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, subcmd);

	memcpy_s(data8, sizeof(data8), &sa_family, sizeof(sa_family));

	count = sscanf(argv[0], "%2x:%2x:%2x:%2x:%2x:%2x",
		       &data[2], &data[3], &data[4], &data[5], &data[6], &data[7]);

	if (count != 6)
		return -EINVAL;

	for(i = 2; i < SET_ADDR_LEN; i++)
		data8[i] = (uint8_t) data[i];

	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, SET_ADDR_LEN, (char *) data8);

	return 0;

nla_put_failure:
	return -ENOBUFS;
}

static int handle_iwlwav_set_mtlk_log_level(struct nl80211_state *state,
						 struct nl_msg *msg,
						 int argc, char **argv,
						 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 3, LTQ_NL80211_VENDOR_SUBCMD_SET_MTLK_LOG_LEVEL);
}
COMMAND(iwlwav, sMtlkLogLevel, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mtlk_log_level, "");

static int handle_iwlwav_set_11h_radar_detection(struct nl80211_state *state,
						 struct nl_msg *msg,
						 int argc, char **argv,
						 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_11H_RADAR_DETECT);
}
COMMAND(iwlwav, s11hRadarDetect, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_11h_radar_detection, "");

static int handle_iwlwav_set_11h_ch_check_time(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_11H_CH_CHECK_TIME);
}
COMMAND(iwlwav, s11hChCheckTime, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_11h_ch_check_time, "");

static int handle_iwlwav_set_11h_emulat_radar(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_11H_EMULAT_RADAR);
}
COMMAND(iwlwav, s11hEmulatRadar, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_11h_emulat_radar, "");

static int handle_iwlwav_emulat_interferer(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 0, 0, LTQ_NL80211_VENDOR_SUBCMD_EMULATE_INTERFERER);
}
COMMAND(iwlwav, emulateInterferer, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_emulat_interferer, "");

static int handle_iwlwav_set_add_peer_ap(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_addr(msg, argc, argv, LTQ_NL80211_VENDOR_SUBCMD_SET_ADD_PEERAP);
}
COMMAND(iwlwav, sAddPeerAP, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_add_peer_ap, "");

static int handle_iwlwav_set_del_peer_ap(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_addr(msg, argc, argv, LTQ_NL80211_VENDOR_SUBCMD_SET_DEL_PEERAP);
}
COMMAND(iwlwav, sDelPeerAP, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_del_peer_ap, "");

static int handle_iwlwav_set_peer_ap_key_idx(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_PEERAP_KEY_IDX);
}
COMMAND(iwlwav, sPeerAPkeyIdx, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_peer_ap_key_idx, "");

static int handle_iwlwav_set_bridge_mode(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_BRIDGE_MODE);

}
COMMAND(iwlwav, sBridgeMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_bridge_mode, "");

static int handle_iwlwav_set_reliable_mcast(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_RELIABLE_MULTICAST);

}
COMMAND(iwlwav, sReliableMcast, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_reliable_mcast, "");

static int handle_iwlwav_set_ap_forwarding(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_AP_FORWARDING);

}
COMMAND(iwlwav, sAPforwarding, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_ap_forwarding, "");

static int handle_iwlwav_set_lt_path_enabled(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_DCDP_API_LITEPATH);
}
COMMAND(iwlwav, sLtPathEnabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_lt_path_enabled, "");

static int handle_iwlwav_set_ipx_ppa_enabled(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_DCDP_API_LITEPATH_COMP);
}
COMMAND(iwlwav, sIpxPpaEnabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_ipx_ppa_enabled, "");

static int handle_iwlwav_set_udma_enabled(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_API);
}
COMMAND(iwlwav, sUdmaEnabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_udma_enabled, "");

static int handle_iwlwav_set_udma_enabled_ext(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_API_EXT);
}
COMMAND(iwlwav, sUdmaEnabledExt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_udma_enabled_ext, "");

static int handle_iwlwav_set_udma_vlan_id(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_VLAN_ID);
}
COMMAND(iwlwav, sUdmaVlanId, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_udma_vlan_id, "");

static int handle_iwlwav_set_udma_vlan_id_ext(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_VLAN_ID_EXT);
}
COMMAND(iwlwav, sUdmaVlanIdExt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_udma_vlan_id_ext, "");

static int handle_iwlwav_set_coc_power(struct nl80211_state *state,
				       struct nl_msg *msg,
				       int argc, char **argv,
				       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 9, LTQ_NL80211_VENDOR_SUBCMD_SET_COC_POWER_MODE);
}
COMMAND(iwlwav, sCoCPower, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_coc_power, "");

static int handle_iwlwav_set_coc_auto_cfg(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 10, 17, LTQ_NL80211_VENDOR_SUBCMD_SET_COC_AUTO_PARAMS);
}
COMMAND(iwlwav, sCoCAutoCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_coc_auto_cfg, "");

static int handle_iwlwav_set_tcp_loop_type(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_PRM_ID_TPC_LOOP_TYPE);
}
COMMAND(iwlwav, sTpcLoopType, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_tcp_loop_type, "");

static int handle_iwlwav_set_inter_det_thresh(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_INTERFER_THRESH);
}
COMMAND(iwlwav, sInterfDetThresh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_inter_det_thresh, "");

static int handle_iwlwav_set_11b_ant_selection(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 3, 3, LTQ_NL80211_VENDOR_SUBCMD_SET_11B_ANTENNA_SELECTION);

}
COMMAND(iwlwav, s11bAntSelection, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_11b_ant_selection, "");

static int handle_iwlwav_set_fw_recovery(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 5, 5, LTQ_NL80211_VENDOR_SUBCMD_SET_FW_RECOVERY);
}
COMMAND(iwlwav, sFWRecovery, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fw_recovery, "");

static int handle_iwlwav_set_out_of_scan_caching(struct nl80211_state *state,
						 struct nl_msg *msg,
						 int argc, char **argv,
						 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_OUT_OF_SCAN_CACHING);
}
COMMAND(iwlwav, sOOScanCaching, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_out_of_scan_caching, "");

static int handle_iwlwav_set_allow_scan_during_cac(struct nl80211_state *state,
						   struct nl_msg *msg,
						   int argc, char **argv,
						   enum id_input id)
{
	/* Setting this value from iw is not supported. Value is set on init by
	 * hostapd and can't be changed on the fly */
	return -EBUSY;
}
COMMAND(iwlwav, sAllowScanInCac, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_allow_scan_during_cac, "");

static int handle_iwlwav_set_enable_radio(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_RADIO_MODE);
}
COMMAND(iwlwav, sEnableRadio, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_enable_radio, "");

static int handle_iwlwav_set_aggr_config(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 2, 3, LTQ_NL80211_VENDOR_SUBCMD_SET_AGGR_CONFIG);

}
COMMAND(iwlwav, sAggrConfig, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_aggr_config, "");

static int handle_iwlwav_set_num_msdu_in_amsdu(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 3, LTQ_NL80211_VENDOR_SUBCMD_SET_AMSDU_NUM);
}
COMMAND(iwlwav, sNumMsduInAmsdu, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_num_msdu_in_amsdu, "");

static int handle_iwlwav_set_agg_rate_limit(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_AGG_RATE_LIMIT);
}
COMMAND(iwlwav, sAggRateLimit, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_agg_rate_limit, "");

static int handle_iwlwav_set_mu_ofdma_bf(struct nl80211_state *state,
				struct nl_msg *msg,
				int argc, char **argv,
				enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_MU_OFDMA_BF);
}
COMMAND(iwlwav, sMuOfdmaBf, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mu_ofdma_bf, "");

static int handle_iwlwav_set_avail_adm_cap(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_ADMISSION_CAPACITY);
}
COMMAND(iwlwav, sAvailAdmCap, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_avail_adm_cap, "");

static int handle_iwlwav_set_rx_th(struct nl80211_state *state,
				   struct nl_msg *msg,
				   int argc, char **argv,
				   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_RX_THRESHOLD);
}
COMMAND(iwlwav, sSetRxTH, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_rx_th, "");

static int handle_iwlwav_set_rx_duty_cyc(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_RX_DUTY_CYCLE);
}
COMMAND(iwlwav, sRxDutyCyc, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_rx_duty_cyc, "");

static int handle_iwlwav_set_power_selection(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_TX_POWER_LIMIT_OFFSET);
}
COMMAND(iwlwav, sPowerSelection, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_power_selection, "");

static int handle_iwlwav_set_11n_protection(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_PROTECTION_METHOD);
}
COMMAND(iwlwav, s11nProtection, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_11n_protection, "");

static int handle_iwlwav_set_calib_on_demand(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_TEMPERATURE_SENSOR);
}
COMMAND(iwlwav, sCalibOnDemand, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_calib_on_demand, "");

static int handle_iwlwav_set_qam_plus(struct nl80211_state *state,
				      struct nl_msg *msg,
				      int argc, char **argv,
				      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_QAMPLUS_MODE);
}
COMMAND(iwlwav, sQAMplus, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_qam_plus, "");

static int handle_iwlwav_set_acs_update_to(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_ACS_UPDATE_TO);
}
COMMAND(iwlwav, sAcsUpdateTo, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_acs_update_to, "");

static int handle_iwlwav_set_mu_operation(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_MU_OPERATION);

}
COMMAND(iwlwav, sMuOperation, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mu_operation, "");

static int handle_iwlwav_set_cca_th(struct nl80211_state *state,
				    struct nl_msg *msg,
				    int argc, char **argv,
				    enum id_input id)
{
	return set_int(msg, argc, argv, 5, 5, LTQ_NL80211_VENDOR_SUBCMD_SET_CCA_THRESHOLD);
}
COMMAND(iwlwav, sCcaTh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_cca_th, "");

static int handle_iwlwav_set_cca_adapt(struct nl80211_state *state,
				       struct nl_msg *msg,
				       int argc, char **argv,
				       enum id_input id)
{
	return set_int(msg, argc, argv, 7, 7, LTQ_NL80211_VENDOR_SUBCMD_SET_CCA_ADAPT);
}
COMMAND(iwlwav, sCcaAdapt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_cca_adapt, "");

static int handle_iwlwav_set_radar_rssi_th(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_RADAR_RSSI_TH);
}
COMMAND(iwlwav, sRadarRssiTh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_radar_rssi_th, "");

static int handle_iwlwav_set_rts_mode(struct nl80211_state *state,
				      struct nl_msg *msg,
				      int argc, char **argv,
				      enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_RTS_MODE);
}
COMMAND(iwlwav, sRTSmode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_rts_mode, "");

static int handle_iwlwav_set_max_mpdu_len(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_MAX_MPDU_LENGTH);
}
COMMAND(iwlwav, sMaxMpduLen, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_max_mpdu_len, "");

static int handle_iwlwav_set_bf_mode(struct nl80211_state *state,
				     struct nl_msg *msg,
				     int argc, char **argv,
				     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_BF_MODE);
}
COMMAND(iwlwav, sBfMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_bf_mode, "");

static int handle_iwlwav_set_probe_req_list_mode(struct nl80211_state *state,
						 struct nl_msg *msg,
						 int argc, char **argv,
						 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_CLT_PROBE_REQS_MODE);
}
COMMAND(iwlwav, sProbeReqCltMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_probe_req_list_mode, "");

static int handle_iwlwav_set_active_ant_mask(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_ACTIVE_ANT_MASK);
}
COMMAND(iwlwav, sActiveAntMask, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_active_ant_mask, "");

static int handle_iwlwav_set_add_four_addr_sta(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_addr(msg, argc, argv, LTQ_NL80211_VENDOR_SUBCMD_SET_4ADDR_STA_ADD);
}
COMMAND(iwlwav, sAddFourAddrSta, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_add_four_addr_sta, "");

static int handle_iwlwav_set_del_four_addr_sta(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_addr(msg, argc, argv, LTQ_NL80211_VENDOR_SUBCMD_SET_4ADDR_STA_DEL);
}
COMMAND(iwlwav, sDelFourAddrSta, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_del_four_addr_sta, "");

static int handle_iwlwav_set_txop_config(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_TXOP_CONFIG);
}
COMMAND(iwlwav, sTxopConfig, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_txop_config, "");

static int handle_iwlwav_set_ssb_mode(struct nl80211_state *state,
				      struct nl_msg *msg,
				      int argc, char **argv,
				      enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_SSB_MODE);
}
COMMAND(iwlwav, sSsbMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_ssb_mode, "");

static int handle_iwlwav_set_mcast_range(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_text(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_MCAST_RANGE_SETUP);
}
COMMAND(iwlwav, sMcastRange, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mcast_range, "");

static int handle_iwlwav_set_mcast_range6(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_text(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_MCAST_RANGE_SETUP_IPV6);
}
COMMAND(iwlwav, sMcastRange6, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mcast_range6, "");

static int handle_iwlwav_set_unknown_mcast_filter_mode(struct nl80211_state *state,
						       struct nl_msg *msg,
						       int argc, char **argv,
						       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_FORWARD_UNKNOWN_MCAST_FLAG);
}
COMMAND(iwlwav, sFwrdUnkwnMcast, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_unknown_mcast_filter_mode, "");

static int handle_iwlwav_set_online_acm(struct nl80211_state *state,
					struct nl_msg *msg,
					int argc, char **argv,
					enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_ONLINE_CALIBRATION_ALGO_MASK);
}
COMMAND(iwlwav, sOnlineACM, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_online_acm, "");

static int handle_iwlwav_set_algo_calibr_mask(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_CALIBRATION_ALGO_MASK);
}
COMMAND(iwlwav, sAlgoCalibrMask, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_algo_calibr_mask, "");

static int handle_iwlwav_set_freq_jump_mode(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_FREQ_JUMP_MODE);
}
COMMAND(iwlwav, sFreqJumpMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_freq_jump_mode, "");

static int handle_iwlwav_set_whm_reset(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_WHM_RESET);
}
COMMAND(iwlwav, sWhmReset, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_whm_reset, "");

static int handle_iwlwav_set_whm_trigger(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_WHM_TRIGGER);
}
COMMAND(iwlwav, sWhmTrigger, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_whm_trigger, "");

static int handle_iwlwav_set_restrict_ac_mode(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_RESTRICTED_AC_MODE);
}
COMMAND(iwlwav, sRestrictAcMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_restrict_ac_mode, "");

static int handle_iwlwav_set_pd_thresh(struct nl80211_state *state,
				       struct nl_msg *msg,
				       int argc, char **argv,
				       enum id_input id)
{
	return set_int(msg, argc, argv, 3, 3, LTQ_NL80211_VENDOR_SUBCMD_SET_PD_THRESHOLD);
}
COMMAND(iwlwav, sPdThresh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_pd_thresh, "");

static int handle_iwlwav_set_fast_drop(struct nl80211_state *state,
				       struct nl_msg *msg,
				       int argc, char **argv,
				       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_FAST_DROP);
}
COMMAND(iwlwav, sFastDrop, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fast_drop, "");

static int handle_iwlwav_set_erp_set(struct nl80211_state *state,
				     struct nl_msg *msg,
				     int argc, char **argv,
				     enum id_input id)
{
	return set_int(msg, argc, argv, 10, 10, LTQ_NL80211_VENDOR_SUBCMD_SET_ERP);
}
COMMAND(iwlwav, sErpSet, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_erp_set, "");

static int handle_iwlwav_set_mu_stat_plan_cfg(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 22, 174, LTQ_NL80211_VENDOR_SUBCMD_SET_STATIC_PLAN_CONFIG);
}
COMMAND(iwlwav, sMuStatPlanCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mu_stat_plan_cfg, "");

static int handle_iwlwav_set_pie_cfg(struct nl80211_state *state,
				     struct nl_msg *msg,
				     int argc, char **argv,
				     enum id_input id)
{
	return set_int(msg, argc, argv, 41, 41, LTQ_NL80211_VENDOR_SUBCMD_SET_PIE_CFG);
}
COMMAND(iwlwav, sPIEcfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_pie_cfg, "");

static int handle_iwlwav_set_nfrp_cfg(struct nl80211_state *state,
				struct nl_msg *msg,
				int argc, char **argv,
				enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_NFRP_CFG);
}
COMMAND(iwlwav, sNfrpCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_nfrp_cfg, "");

static int handle_iwlwav_set_cca_preamble_puncture_cfg(struct nl80211_state *state,
					struct nl_msg *msg,
					int argc, char **argv,
					enum id_input id)
{
	return set_int(msg, argc, argv, 3, 3, LTQ_NL80211_VENDOR_SUBCMD_SET_CCA_PREAMBLE_PUNCTURE_CFG);
}
COMMAND(iwlwav, sPreamPunCcaOvr, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_cca_preamble_puncture_cfg, "");

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command with WEP ecryption data for sending
 *
 *  \param[in]  state              pointer struct nl80211_state, unused param
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  argc               number of arguments, must be 2
 *  \param[in]  argv               pointer to arguments
 *  \param[in]  id                 from 'enum id_input', unused param
 *
 *  \return     0 on success, non 0 on error
 */
static int handle_iwlwav_set_wds_wep_enc_cfg(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	unsigned char data[IW_ENCODING_TOKEN_MAX + sizeof(int)];
	int key_id, key_len;

	if (!msg)
		return -EFAULT;

	if (argc != 2)
		return -EINVAL;

	key_id = atoi(argv[0]);
	memcpy_s(data, sizeof(data), &key_id, sizeof(key_id));
	key_len = hex2bin2((char*)(&data[sizeof(int)]), IW_ENCODING_TOKEN_MAX,
			   argv[1], strnlen_s(argv[1], (IW_ENCODING_TOKEN_MAX * 2)));
	if (key_len <= 0)
		return -EINVAL;

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, LTQ_NL80211_VENDOR_SUBCMD_SET_WDS_WEP_ENC_CFG);
	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, key_len + sizeof(int), data);

	return 0;

nla_put_failure:
	return -ENOBUFS;
}
COMMAND(iwlwav, sWdsWepEncCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_wds_wep_enc_cfg, "");

static int handle_iwlwav_set_rts_rate(struct nl80211_state *state,
				      struct nl_msg *msg,
				      int argc, char **argv,
				      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_RTS_RATE);
}
COMMAND(iwlwav, sRtsRate, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_rts_rate, "");

static int handle_iwlwav_set_stations_statistics(struct nl80211_state *state,
						 struct nl_msg *msg,
						 int argc, char **argv,
						 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_STATIONS_STATISTICS);
}
COMMAND(iwlwav, sStationsStat, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_stations_statistics, "");

static int handle_iwlwav_set_stats_poll_period(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_STATS_POLL_PERIOD);
}
COMMAND(iwlwav, sStatsPollPeriod, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_stats_poll_period, "");

static int handle_iwlwav_set_dynamic_mu_type(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 5, 5, LTQ_NL80211_VENDOR_SUBCMD_SET_DYNAMIC_MU_TYPE);
}
COMMAND(iwlwav, sDynamicMu, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_dynamic_mu_type, "");

static int handle_iwlwav_set_he_mu_fixed_parameters(struct nl80211_state *state,
						    struct nl_msg *msg,
						    int argc, char **argv,
						    enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_HE_MU_FIXED_PARAMETERS);
}
COMMAND(iwlwav, sMuFixedCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_he_mu_fixed_parameters, "");

static int handle_iwlwav_set_he_mu_duration(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_HE_MU_DURATION);
}
COMMAND(iwlwav, sMuDurationCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_he_mu_duration, "");

static int handle_iwlwav_set_etsi_ppdu_limits(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
    return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_ETSI_PPDU_LIMITS);
}
COMMAND(iwlwav, sETSILimitation, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_etsi_ppdu_limits, "");

static int handle_iwlwav_set_tx_retry_limit(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_AP_RETRY_LIMIT);
}
COMMAND(iwlwav, sTxRetryLimit, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_tx_retry_limit, "");


static int handle_iwlwav_set_tx_exce_retry_limit(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_AP_EXCE_RETRY_LIMIT);
}
COMMAND(iwlwav, sTxExceRetryLimit, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_tx_exce_retry_limit, "");

static int handle_iwlwav_set_cts_to_self_to(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_CTS_TO_SELF_TO);
}
COMMAND(iwlwav, sCtsToSelfTo, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_cts_to_self_to, "");

static int handle_iwlwav_set_tx_ampdu_density(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_TX_AMPDU_DENSITY);
}
COMMAND(iwlwav, sTxAmpduDensity, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_tx_ampdu_density, "");

static int handle_iwlwav_set_slow_probing_mask(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_PROBING_MASK);
}
COMMAND(iwlwav, sSlowProbingMask, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_slow_probing_mask, "");

static int handle_iwlwav_set_scan_modif_flags(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_MODIFS);
}
COMMAND(iwlwav, sScanModifFlags, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_scan_modif_flags, "");

static int handle_iwlwav_set_scan_pause_bg_cache(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PAUSE_BG_CACHE);
}
COMMAND(iwlwav, sScanPauseBGCache, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_scan_pause_bg_cache, "");

static int handle_iwlwav_set_config_mrcoex(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_COEX_CFG);
}
COMMAND(iwlwav, sConfigMRCoex, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_config_mrcoex, "");

static int handle_iwlwav_set_fixed_ltf_gi(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_LTF_AND_GI);
}
COMMAND(iwlwav, sFixedLtfGi, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fixed_ltf_gi, "");


/***************************** DEBUG SET COMMANDS ************************/
#ifdef CONFIG_WAVE_DEBUG

static int handle_iwlwav_set_counters_src(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_COUNTERS_SRC);
}
COMMAND(iwlwav, sCountersSrc, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_counters_src, "");

static int handle_iwlwav_set_unconn_time(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_UNCONNECTED_STA_SCAN_TIME);
}
COMMAND(iwlwav, sUnconnTime, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_unconn_time, "");

static int handle_iwlwav_set_fixed_power(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_POWER);
}
COMMAND(iwlwav, sFixedPower, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fixed_power, "");

static int handle_iwlwav_set_ctrl_bfilter_bank(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_IRE_CTRL_B);
}
COMMAND(iwlwav, sCtrlBFilterBank, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_ctrl_bfilter_bank, "");

static int handle_iwlwav_set_cpu_dma_latency(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_CPU_DMA_LATENCY);
}
COMMAND(iwlwav, sCpuDmaLatency, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_cpu_dma_latency, "");

static int handle_iwlwav_set_tasklet_limits(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 5, 5, LTQ_NL80211_VENDOR_SUBCMD_SET_TASKLET_LIMITS);
}
COMMAND(iwlwav, sTaskletLimits, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_tasklet_limits, "");

static int handle_intel_vendor_set_scan_cal_exp_time(struct nl80211_state *state,
						     struct nl_msg *msg,
						     int argc, char **argv,
						     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_EXP_TIME);
}
COMMAND(iwlwav, sScanExpTime, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_intel_vendor_set_scan_cal_exp_time, "");

static int handle_iwlwav_set_scan_params(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 6, 6, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PARAMS);
}
COMMAND(iwlwav, sScanParams, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_scan_params, "");

static int handle_iwlwav_set_scan_params_bg(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 10, 10, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PARAMS_BG);
}
COMMAND(iwlwav, sScanParamsBG, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_scan_params_bg, "");

static int handle_iwlwav_set_ta_timer_res(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_TA_TIMER_RESOLUTION);
}
COMMAND(iwlwav, sTATimerRes, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_ta_timer_res, "");

static int handle_iwlwav_set_pcoc_auto_cfg(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 4, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_PCOC_AUTO_PARAMS);
}
COMMAND(iwlwav, sPCoCAutoCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_pcoc_auto_cfg, "");

static int handle_iwlwav_set_pcoc_power(struct nl80211_state *state,
					struct nl_msg *msg,
					int argc, char **argv,
					enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_PCOC_POWER_MODE);
}
COMMAND(iwlwav, sPCoCPower, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_pcoc_power, "");

static int handle_iwlwav_set_wds_host_to(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_WDS_HOST_TIMEOUT);
}
COMMAND(iwlwav, sWDSHostTO, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_wds_host_to, "");

static int handle_iwlwav_set_mac_wd_period_ms(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_MAC_WATCHDOG_PERIOD_MS);
}
COMMAND(iwlwav, sMACWdPeriodMs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mac_wd_period_ms, "");

static int handle_iwlwav_set_mac_wd_timeout_ms(struct nl80211_state *state,
					       struct nl_msg *msg,
					       int argc, char **argv,
					       enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_MAC_WATCHDOG_TIMEOUT_MS);
}
COMMAND(iwlwav, sMACWdTimeoutMs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_mac_wd_timeout_ms, "");

static int handle_iwlwav_set_non_occupated_period(struct nl80211_state *state,
						  struct nl_msg *msg,
						  int argc, char **argv,
						  enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_NON_OCCUPATED_PRD);
}
COMMAND(iwlwav, sNonOccupatePrd, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_non_occupated_period, "");

static int handle_iwlwav_set_11h_beacon_count(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_11H_BEACON_COUNT);
}
COMMAND(iwlwav, s11hBeaconCount, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_11h_beacon_count, "");

static int handle_iwlwav_set_enable_test_bus(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_ENABLE_TEST_BUS);
}
COMMAND(iwlwav, sEnableTestBus, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_enable_test_bus, "");

static int handle_iwlwav_set_fixed_rate_cfg(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return set_int(msg, argc, argv, 11, 11, LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_RATE);
}
COMMAND(iwlwav, sFixedRateCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fixed_rate_cfg, "");

static int handle_iwlwav_set_do_debug_assert(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 1, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_DBG_ASSERT);
}
COMMAND(iwlwav, sDoDebugAssert, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_do_debug_assert, "");

static int handle_iwlwav_set_do_fw_debug(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 1, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_FW_DEBUG);
}
COMMAND(iwlwav, sDoFwDebug, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_do_fw_debug, "");

static int handle_iwlwav_set_do_simple_cli(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 4, LTQ_NL80211_VENDOR_SUBCMD_SET_DBG_CLI);
}
COMMAND(iwlwav, sDoSimpleCLI, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_do_simple_cli, "");

static int handle_iwlwav_set_pmcu_debug(struct nl80211_state *state,
					struct nl_msg *msg,
					int argc, char **argv,
					enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_SET_PCOC_PMCU_DEBUG);
}
COMMAND(iwlwav, sPMCUDebug, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_pmcu_debug, "");

static int handle_iwlwav_set_fw_log_severity(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_FW_LOG_SEVERITY);
}
COMMAND(iwlwav, sFwLogSeverity, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fw_log_severity, "");

static int handle_iwlwav_set_fw_debug_cmd(struct nl80211_state *state,
					  struct nl_msg *msg,
					  int argc, char **argv,
					  enum id_input id)
{
  return set_int(msg, argc, argv, 3, 1000, LTQ_NL80211_VENDOR_SUBCMD_SET_DBG_CMD_FW);
}
COMMAND(iwlwav, sDebugCmdFw, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_fw_debug_cmd, "");

#endif

static int handle_iwlwav_set_start_cca_msr(struct nl80211_state *state,
				     struct nl_msg *msg,
				     int argc, char **argv,
				     enum id_input id)
{
	return set_int(msg, argc, argv, 2, 2, LTQ_NL80211_VENDOR_SUBCMD_SET_START_CCA_MSR_OFF_CHAN);
}
COMMAND(iwlwav, sStartCcaMsr, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_set_start_cca_msr, "");

static int handle_iwlwav_advertise_btwt_schedule(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 8, 50, LTQ_NL80211_VENDOR_SUBCMD_ADVERTISE_BTWT_SCHEDULE);
}
COMMAND(iwlwav, sAdvertiseBcTwtSp, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_advertise_btwt_schedule, "");

static int handle_iwlwav_terminate_btwt_schedule(struct nl80211_state *state,
					   struct nl_msg *msg,
					   int argc, char **argv,
					   enum id_input id)
{
	return set_int(msg, argc, argv, 1, 1, LTQ_NL80211_VENDOR_SUBCMD_TERMINATE_BTWT_SCHEDULE);
}
COMMAND(iwlwav, sTerminateBcTwtSp, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_terminate_btwt_schedule, "");

static int handle_iwlwav_tx_twt_teardown(struct nl80211_state *state,
					 struct nl_msg *msg,
					 int argc, char **argv,
					 enum id_input id)
{
	return set_int(msg, argc, argv, 2, 4, LTQ_NL80211_VENDOR_SUBCMD_TX_TWT_TEARDOWN);
}
COMMAND(iwlwav, sTxTwtTeardown, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_tx_twt_teardown, "");

/***************************** GET FUNCTIONS *****************************/

/******************************************************************************/
/*! \brief      Print probe requests list
 *
 *  \param[in]  msg                pointer to NL message data, must not be NULL
 *  \param[in]  arg                pointer to 'struct print_data' data, must not be NULL
 *
 *  \return     NL_OK on success, non NL_SKIP on error
 */
static int print_probe_req_list(struct nl_msg *msg, void *arg)
{
	struct nlattr *attr;
	struct genlmsghdr *gnlh;
	int len;
	int i;
	struct print_data *print_d;
	struct intel_vendor_probe_req_info *arr;

	if (!msg || !arg)
		return NL_SKIP;

	gnlh = nlmsg_data(nlmsg_hdr(msg));
	print_d = (struct print_data *) arg;
	attr = nla_find(genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0),
			NL80211_ATTR_VENDOR_DATA);
	if (!attr) {
		fprintf(stderr, "vendor data attribute missing!\n");
		return NL_SKIP;
	}

	len = (nla_len(attr)/sizeof(struct intel_vendor_probe_req_info));
	arr = (struct intel_vendor_probe_req_info *) nla_data(attr);
	printf("%s [%d]:\n", print_d->print_msg, len);
	for (i=0; i < len; i++) {
		u8 *addr = arr[i].addr;
		printf("mac addr = %02x:%02x:%02x:%02x:%02x:%02x ",
		        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
		printf("age = %d\n", arr[i].age);
	}

	printf("\n");

	return NL_OK;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command to retrieve data from \a sub_cmd
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  sub_cmd            Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *  \param[in]  num_of_params      expected number of params (bytes) to be received
 *  \param[in]  print_msg          Text to be printed prior to printing data
 *
 *  \return     0 on success, non 0 on error
 */
static int sub_cmd_print_probe_req_list(struct nl_msg *msg,
					uint16_t sub_cmd,
					uint8_t num_of_params,
					char* print_msg)
{
	static struct print_data print_d;

	if (!msg) {
		fprintf(stderr, "no msg on sub_cmd_print_probe_req_list!\n");
		return -EFAULT;
	}

	print_d.num_of_params = num_of_params;
	strncpy_s(print_d.print_msg, sizeof(print_d.print_msg),
		  print_msg, strnlen_s(print_msg, sizeof(print_d.print_msg)));

	register_handler(print_probe_req_list, (void *)&print_d);

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, sub_cmd);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}

/******************************************************************************/
/*! \brief      Print he operation BSS color
 *
 *  \param[in]  msg                pointer to NL message data, must not be NULL
 *  \param[in]  arg                pointer to 'struct print_data' data, must not be NULL
 *
 *  \return     NL_OK on success, non NL_SKIP on error
 */
static int print_he_operation_bss_color(struct nl_msg *msg, void *arg)
{
	struct nlattr *attr;
	struct genlmsghdr *gnlh;
	struct intel_vendor_he_oper *data;
	struct print_data *print_d;

	if (!msg || !arg)
		return NL_SKIP;

	gnlh = nlmsg_data(nlmsg_hdr(msg));
	print_d = (struct print_data *) arg;
	attr = nla_find(genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0),
			NL80211_ATTR_VENDOR_DATA);
	if (!attr) {
		fprintf(stderr, "vendor data attribute missing!\n");
		return NL_SKIP;
	}

	if (nla_len(attr) != print_d->num_of_params)
		return NL_SKIP;

	data = (struct intel_vendor_he_oper *) nla_data(attr);
	printf("%s:", print_d->print_msg);
	printf("%d ", (data->he_bss_color_info & HE_OPERATION_BSS_COLOR));

	printf("\n");

	return NL_OK;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command to retrieve data from \a sub_cmd
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  sub_cmd            Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *  \param[in]  num_of_params      expected number of params (bytes) to be received
 *  \param[in]  print_msg          Text to be printed prior to printing data
 *
 *  \return     0 on success, non 0 on error
 */
static int sub_cmd_print_he_operation_bss_color(struct nl_msg *msg,
						uint16_t sub_cmd,
						uint8_t num_of_params,
						char* print_msg)
{
	static struct print_data print_d;

	if (!msg)
		return -EFAULT;

	print_d.num_of_params = num_of_params;
	strncpy_s(print_d.print_msg, sizeof(print_d.print_msg),
		  print_msg, strnlen_s(print_msg, sizeof(print_d.print_msg)));

	register_handler(print_he_operation_bss_color, (void *)&print_d);

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, sub_cmd);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}

/******************************************************************************/
/*! \brief      Print he operation BSS color disabled flag
 *
 *  \param[in]  msg                pointer to NL message data, must not be NULL
 *  \param[in]  arg                pointer to 'struct print_data' data, must not be NULL
 *
 *  \return     NL_OK on success, non NL_SKIP on error
 */
static int print_he_operation_bss_color_disabled_flag(struct nl_msg *msg,
						      void *arg)
{
	struct nlattr *attr;
	struct genlmsghdr *gnlh;
	struct intel_vendor_he_oper *data;
	struct print_data *print_d;

	if (!msg || !arg)
		return NL_SKIP;

	gnlh = nlmsg_data(nlmsg_hdr(msg));
	print_d = (struct print_data *) arg;
	attr = nla_find(genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0),
			NL80211_ATTR_VENDOR_DATA);
	if (!attr) {
		fprintf(stderr, "vendor data attribute missing!\n");
		return NL_SKIP;
	}

	if (nla_len(attr) != print_d->num_of_params)
		return NL_SKIP;

	data = (struct intel_vendor_he_oper *) nla_data(attr);
	printf("%s:", print_d->print_msg);
	printf("%d ", ((data->he_bss_color_info & HE_OPERATION_BSS_COLOR_DISABLED) ? 1 : 0));

	printf("\n");

	return NL_OK;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command to retrieve data from \a sub_cmd
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  sub_cmd            Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *  \param[in]  num_of_params      expected number of params (bytes) to be received
 *  \param[in]  print_msg          Text to be printed prior to printing data
 *
 *  \return     0 on success, non 0 on error
 */
static int sub_cmd_print_bss_color_disabled_flag(struct nl_msg *msg,
						 uint16_t sub_cmd,
						 uint8_t num_of_params,
						 char* print_msg)
{
	static struct print_data print_d;

	if (!msg)
		return -EFAULT;

	print_d.num_of_params = num_of_params;
	strncpy_s(print_d.print_msg, sizeof(print_d.print_msg),
		  print_msg, strnlen_s(print_msg, sizeof(print_d.print_msg)));

	register_handler(print_he_operation_bss_color_disabled_flag, (void *)&print_d);

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, sub_cmd);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}

static uint32_t get_uint32_value(uint8_t* data,
				 uint8_t num_of_params)
{
	return *(uint32_t *) (data + num_of_params * sizeof(int));
}

/******************************************************************************/
/*! \brief      Print NL80211 vendor command received integer value/values from driver
 *
 *  \param[in]  msg                pointer to NL message data, must not be NULL
 *  \param[in]  arg                pointer to 'struct print_data' data, must not be NULL
 *
 *  \return     NL_OK on success, non NL_SKIP on error
 */
static int print_vendor_int(struct nl_msg *msg,
			    void *arg)
{
	struct nlattr *attr;
	struct genlmsghdr *gnlh;
	uint8_t* data;
	int i;
	struct print_data *print_d;

	if (!msg || !arg)
		return NL_SKIP;

	gnlh = nlmsg_data(nlmsg_hdr(msg));
	print_d = (struct print_data *) arg;
	attr = nla_find(genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0),
			NL80211_ATTR_VENDOR_DATA);
	if (!attr) {
		fprintf(stderr, "vendor data attribute missing!\n");
		return NL_SKIP;
	}

	/*
	 * if num_of_params wasn't set, it means that the number of params
	 * can varied for this command, so get it explicitly from the received attr
	 */
	if (!print_d->num_of_params)
		print_d->num_of_params = (nla_len(attr)) / sizeof(int);

	data = (uint8_t *)nla_data(attr);
	printf("%s:", print_d->print_msg);
	for(i = 0; i < print_d->num_of_params; i++)
		printf("%d ", get_uint32_value(data, i));

	printf("\n");

	return NL_OK;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command to retrieve data from \a sub_cmd
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  sub_cmd            Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *  \param[in]  print_msg          Text to be printed prior to printing data
 *
 *  \return     0 on success, non 0 on error
 */
static int sub_cmd_print_int_function(struct nl_msg *msg,
				      uint16_t sub_cmd,
				      char* print_msg)
{
	static struct print_data print_d;

	if (!msg)
		return -EFAULT;

	strncpy_s(print_d.print_msg, sizeof(print_d.print_msg),
		  print_msg, strnlen_s(print_msg, sizeof(print_d.print_msg)));

	register_handler(print_vendor_int, (void *)&print_d);

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, sub_cmd);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}

/******************************************************************************/
/*! \brief      Print NL80211 vendor command received text from driver
 *
 *  \param[in]  msg                pointer to NL message data, must not be NULL
 *  \param[in]  arg                pointer to 'struct print_data' data, must not be NULL
 *
 *  \return     NL_OK on success, non NL_SKIP on error
 */
static int print_vendor_text(struct nl_msg *msg,
			     void *arg)
{
	struct nlattr *attr;
	struct genlmsghdr *gnlh;
	char *data;
	struct print_data *print_d;

	if (!msg || !arg)
		return NL_SKIP;

	gnlh = nlmsg_data(nlmsg_hdr(msg));
	print_d = (struct print_data *) arg;
	attr = nla_find(genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0),
			NL80211_ATTR_VENDOR_DATA);
	if (!attr) {
		fprintf(stderr, "vendor data attribute missing!\n");
		return NL_SKIP;
	}

	data = (char *) nla_data(attr);

	printf("%s:", print_d->print_msg);
	printf("%.*s", nla_len(attr), &data[0]);
	printf("\n");

	return NL_OK;
}

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command to retrieve data from \a sub_cmd
 *
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  sub_cmd            Intel vendor sub command, from 'enum ltq_nl80211_vendor_subcmds'
 *  \param[in]  print_msg          Text to be printed prior to printing data
 *
 *  \return     0 on success, non 0 on error
 */
static int sub_cmd_print_text_function(struct nl_msg *msg,
				       uint16_t sub_cmd,
				       char* print_msg)
{
	static struct print_data print_d;

	if (!msg)
		return -EFAULT;

	strncpy_s(print_d.print_msg, sizeof(print_d.print_msg),
		print_msg, strnlen_s(print_msg, sizeof(print_d.print_msg)));

	register_handler(print_vendor_text, (void *)&print_d);

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, sub_cmd);

	return 0;
nla_put_failure:
	return -ENOBUFS;
}

static int handle_iwlwav_get_11h_radar_detect(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_11H_RADAR_DETECT, "g11hRadarDetect");
}
COMMAND(iwlwav, g11hRadarDetect, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_11h_radar_detect, "");

static int handle_iwlwav_get_11h_ch_check_time(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_11H_CH_CHECK_TIME, "g11hChCheckTime");
}
COMMAND(iwlwav, g11hChCheckTime, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_11h_ch_check_time, "");

static int handle_iwlwav_get_peer_ap_key_idx(struct nl80211_state *state,
					     struct nl_msg *msg, int argc,
					     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PEERAP_KEY_IDX, "gPeerAPkeyIdx");
}
COMMAND(iwlwav, gPeerAPkeyIdx, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_peer_ap_key_idx, "");

static int handle_iwlwav_get_peer_aps(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PEERAP_LIST, "gPeerAPs");
}
COMMAND(iwlwav, gPeerAPs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_peer_aps, "");

static int handle_iwlwav_get_bridge_mode(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_BRIDGE_MODE, "gBridgeMode");
}
COMMAND(iwlwav, gBridgeMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_bridge_mode, "");

static int handle_iwlwav_get_reliable_mcast(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RELIABLE_MULTICAST, "gReliableMcast");
}
COMMAND(iwlwav, gReliableMcast, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_reliable_mcast, "");

static int handle_iwlwav_get_ap_forwarding(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AP_FORWARDING, "gAPforwarding");
}
COMMAND(iwlwav, gAPforwarding, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ap_forwarding, "");

static int handle_iwlwav_get_eeprom(struct nl80211_state *state,
				    struct nl_msg *msg, int argc,
				    char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_EEPROM, "gEEPROM");
}
COMMAND(iwlwav, gEEPROM, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_eeprom, "");

static int handle_iwlwav_datapath_mode(struct nl80211_state *state,
				       struct nl_msg *msg, int argc,
				       char **argv, enum id_input id)
{
    return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_DCDP_DATAPATH_MODE, "gDataPathMode");
}
COMMAND(iwlwav, gDataPathMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_datapath_mode, "");

static int handle_iwlwav_lt_path_enabled(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_DCDP_API_LITEPATH, "gLtPathEnabled");
}
COMMAND(iwlwav, gLtPathEnabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_lt_path_enabled, "");

static int handle_iwlwav_ipx_ppa_enabled(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_DCDP_API_LITEPATH_COMP, "gIpxPpaEnabled");
}
COMMAND(iwlwav, gIpxPpaEnabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_ipx_ppa_enabled, "");

static int handle_iwlwav_udma_enabled(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_API, "gUdmaEnabled");
}
COMMAND(iwlwav, gUdmaEnabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_udma_enabled, "");

static int handle_iwlwav_udma_enabled_ext(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_API_EXT, "gUdmaEnabledExt");
}
COMMAND(iwlwav, gUdmaEnabledExt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_udma_enabled_ext, "");

static int handle_iwlwav_udma_vlan_id(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_VLAN_ID, "gUdmaVlanId");
}
COMMAND(iwlwav, gUdmaVlanId, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_udma_vlan_id, "");


static int handle_iwlwav_udma_vlan_id_ext(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_VLAN_ID_EXT, "gUdmaVlanIdExt");
}
COMMAND(iwlwav, gUdmaVlanIdExt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_udma_vlan_id_ext, "");

static int handle_iwlwav_get_coc_power(struct nl80211_state *state,
				       struct nl_msg *msg, int argc,
				       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_COC_POWER_MODE, "gCoCPower");
}
COMMAND(iwlwav, gCoCPower, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_coc_power, "");

static int handle_iwlwav_get_coc_auto_cfg(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_COC_AUTO_PARAMS, "gCoCAutoCfg");
}
COMMAND(iwlwav, gCoCAutoCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_coc_auto_cfg, "");

static int handle_iwlwav_get_erp_cfg(struct nl80211_state *state,
				     struct nl_msg *msg, int argc,
				     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ERP_CFG, "gErpSet");
}
COMMAND(iwlwav, gErpSet, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_erp_cfg, "");

static int handle_iwlwav_get_tcp_loop_type(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PRM_ID_TPC_LOOP_TYPE, "gTpcLoopType");
}
COMMAND(iwlwav, gTpcLoopType, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_tcp_loop_type, "");

static int handle_iwlwav_get_inter_det_thresh(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_INTERFER_MODE, "gInterfDetThresh");
}
COMMAND(iwlwav, gInterfDetThresh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_inter_det_thresh, "");

static int handle_iwlwav_get_ap_caps_max_stas(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AP_CAPABILITIES_MAX_STAs, "gAPCapsMaxSTAs");
}
COMMAND(iwlwav, gAPCapsMaxSTAs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ap_caps_max_stas, "");

static int handle_iwlwav_get_ap_caps_max_vaps(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AP_CAPABILITIES_MAX_VAPs, "gAPCapsMaxVAPs");
}
COMMAND(iwlwav, gAPCapsMaxVAPs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ap_caps_max_vaps, "");

static int handle_iwlwav_get_11b_ant_selection(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_11B_ANTENNA_SELECTION, "g11bAntSelection");
}
COMMAND(iwlwav, g11bAntSelection, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_11b_ant_selection, "");

static int handle_iwlwav_get_fw_recovery(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_FW_RECOVERY, "gFWRecovery");
}
COMMAND(iwlwav, gFWRecovery, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_fw_recovery, "");

static int handle_iwlwav_get_fw_recovery_statistics(struct nl80211_state *state,
						    struct nl_msg *msg, int argc,
						    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RCVRY_STATS, "gFWRecoveryStat");
}
COMMAND(iwlwav, gFWRecoveryStat, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_fw_recovery_statistics, "");

static int handle_iwlwav_get_out_of_scan_caching(struct nl80211_state *state,
						 struct nl_msg *msg, int argc,
						 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_OUT_OF_SCAN_CACHING, "gOOScanCaching");
}
COMMAND(iwlwav, gOOScanCaching, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_out_of_scan_caching, "");

static int handle_iwlwav_get_allow_scan_during_cac(struct nl80211_state *state,
						   struct nl_msg *msg, int argc,
						   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ALLOW_SCAN_DURING_CAC, "gAllowScanInCac");
}
COMMAND(iwlwav, gAllowScanInCac, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_allow_scan_during_cac, "");

static int handle_iwlwav_get_enable_radio(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_MODE, "gEnableRadio");
}
COMMAND(iwlwav, gEnableRadio, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_enable_radio, "");

static int handle_iwlwav_get_aggr_config(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AGGR_CONFIG, "gAggrConfig");
}
COMMAND(iwlwav, gAggrConfig, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_aggr_config, "");

static int handle_iwlwav_get_num_msdu_in_amsdu(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AMSDU_NUM, "gNumMsduInAmsdu");
}
COMMAND(iwlwav, gNumMsduInAmsdu, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_num_msdu_in_amsdu, "");

static int handle_iwlwav_get_agg_rate_limit(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AGG_RATE_LIMIT, "gAggRateLimit");
}
COMMAND(iwlwav, gAggRateLimit, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_agg_rate_limit, "");

static int handle_iwlwav_get_mu_ofdma_bf(struct nl80211_state *state,
			      struct nl_msg *msg, int argc,
			      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MU_OFDMA_BF, "gMuOfdmaBf");
}
COMMAND(iwlwav, gMuOfdmaBf, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_mu_ofdma_bf, "");

static int handle_iwlwav_get_avail_adm_cap(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ADMISSION_CAPACITY, "gAvailAdmCap");
}
COMMAND(iwlwav, gAvailAdmCap, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_avail_adm_cap, "");

static int handle_iwlwav_get_set_rx_th(struct nl80211_state *state,
				       struct nl_msg *msg, int argc,
				       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RX_THRESHOLD, "gSetRxTH");
}
COMMAND(iwlwav, gSetRxTH, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_set_rx_th, "");

static int handle_iwlwav_get_rx_duty_cyc(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RX_DUTY_CYCLE, "gRxDutyCyc");
}
COMMAND(iwlwav, gRxDutyCyc, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_rx_duty_cyc, "");

static int handle_iwlwav_get_power_selection(struct nl80211_state *state,
					     struct nl_msg *msg, int argc,
					     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TX_POWER_LIMIT_OFFSET, "gPowerSelection");
}
COMMAND(iwlwav, gPowerSelection, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_power_selection, "");

static int handle_iwlwav_get_11n_protection(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PROTECTION_METHOD, "g11nProtection");
}
COMMAND(iwlwav, g11nProtection, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_11n_protection, "");

static int handle_iwlwav_get_temperature(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TEMPERATURE_SENSOR, "gTemperature");
}
COMMAND(iwlwav, gTemperature, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_temperature, "");

static int handle_iwlwav_get_qam_plus(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_QAMPLUS_MODE, "gQAMplus");
}
COMMAND(iwlwav, gQAMplus, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_qam_plus, "");

static int handle_iwlwav_get_acs_update_to(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ACS_UPDATE_TO, "gAcsUpdateTo");
}
COMMAND(iwlwav, gAcsUpdateTo, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_acs_update_to, "");

static int handle_iwlwav_get_mu_operation(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MU_OPERATION, "gMuOperation");
}
COMMAND(iwlwav, gMuOperation, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_mu_operation, "");

static int handle_iwlwav_get_cac_th(struct nl80211_state *state,
				    struct nl_msg *msg, int argc,
				    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_THRESHOLD, "gCcaTh");
}
COMMAND(iwlwav, gCcaTh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cac_th, "");

static int handle_iwlwav_get_cca_adapt(struct nl80211_state *state,
				       struct nl_msg *msg, int argc,
				       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_ADAPT, "gCcaAdapt");
}
COMMAND(iwlwav, gCcaAdapt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cca_adapt, "");

static int handle_iwlwav_get_radar_rssi_th(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RADAR_RSSI_TH, "gRadarRssiTh");
}
COMMAND(iwlwav, gRadarRssiTh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_radar_rssi_th, "");

static int handle_iwlwav_get_rts_mode(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RTS_MODE, "gRTSmode");
}
COMMAND(iwlwav, gRTSmode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_rts_mode, "");

static int handle_iwlwav_get_max_mpdu_len(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MAX_MPDU_LENGTH, "gMaxMpduLen");
}
COMMAND(iwlwav, gMaxMpduLen, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_max_mpdu_len, "");

static int handle_iwlwav_get_bf_mode(struct nl80211_state *state,
				     struct nl_msg *msg, int argc,
				     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_BF_MODE, "gBfMode");
}
COMMAND(iwlwav, gBfMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_bf_mode, "");

static int handle_iwlwav_get_probe_req_list_mode(struct nl80211_state *state,
						 struct nl_msg *msg, int argc,
						 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CLT_PROBE_REQS_MODE, "gProbeReqCltMode");
}
COMMAND(iwlwav, gProbeReqCltMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_probe_req_list_mode, "");

static int handle_iwlwav_get_active_ant_mask(struct nl80211_state *state,
					     struct nl_msg *msg, int argc,
					     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ACTIVE_ANT_MASK, "gActiveAntMask");
}
COMMAND(iwlwav, gActiveAntMask, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_active_ant_mask, "");

static int handle_iwlwav_get_four_addr_stats(struct nl80211_state *state,
					     struct nl_msg *msg, int argc,
					     char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_4ADDR_STA_LIST, "gFourAddrStas");
}
COMMAND(iwlwav, gFourAddrStas, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_four_addr_stats, "");

static int handle_iwlwav_get_txop_config(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TXOP_CONFIG, "gTxopConfig");
}
COMMAND(iwlwav, gTxopConfig, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_txop_config, "");

static int handle_iwlwav_get_ssb_mode(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_SSB_MODE, "gSsbMode");
}
COMMAND(iwlwav, gSsbMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ssb_mode, "");

static int handle_iwlwav_get_mcast_range(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MCAST_RANGE_SETUP, "gMcastRange");
}
COMMAND(iwlwav, gMcastRange, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_mcast_range, "");

static int handle_iwlwav_get_mcast_range6(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MCAST_RANGE_SETUP_IPV6, "gMcastRange6");
}
COMMAND(iwlwav, gMcastRange6, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_mcast_range6, "");

static int handle_iwlwav_get_unknown_mcast_filter_mode(struct nl80211_state *state,
						       struct nl_msg *msg, int argc,
						       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_FORWARD_UNKNOWN_MCAST_FLAG, "gFwrdUnkwnMcast");
}
COMMAND(iwlwav, gFwrdUnkwnMcast, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_unknown_mcast_filter_mode, "");

static int handle_iwlwav_get_online_acm(struct nl80211_state *state,
					struct nl_msg *msg, int argc,
					char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ONLINE_CALIBRATION_ALGO_MASK, "gOnlineACM");
}
COMMAND(iwlwav, gOnlineACM, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_online_acm, "");

static int handle_iwlwav_get_algo_calibr_mask(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CALIBRATION_ALGO_MASK, "gAlgoCalibrMask");
}
COMMAND(iwlwav, gAlgoCalibrMask, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_algo_calibr_mask, "");

static int handle_iwlwav_get_restrict_ac_mode(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RESTRICTED_AC_MODE, "gRestrictAcMode");
}
COMMAND(iwlwav, gRestrictAcMode, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_restrict_ac_mode, "");

static int handle_iwlwav_get_pd_thresh(struct nl80211_state *state,
				       struct nl_msg *msg, int argc,
				       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PD_THRESHOLD, "gPdThresh");
}
COMMAND(iwlwav, gPdThresh, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_pd_thresh, "");

static int handle_iwlwav_get_fast_drop(struct nl80211_state *state,
				       struct nl_msg *msg, int argc,
				       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_FAST_DROP, "gFastDrop");
}
COMMAND(iwlwav, gFastDrop, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_fast_drop, "");

static int handle_iwlwav_get_pvt_sensor(struct nl80211_state *state,
					struct nl_msg *msg, int argc,
					char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PVT_SENSOR, "gPVT");
}
COMMAND(iwlwav, gPVT, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_pvt_sensor, "");

static int handle_iwlwav_get_rts_rate(struct nl80211_state *state,
				      struct nl_msg *msg, int argc,
				      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RTS_RATE, "gRtsRate");
}
COMMAND(iwlwav, gRtsRate, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_rts_rate, "");

static int handle_iwlwav_get_pie_cfg(struct nl80211_state *state,
				     struct nl_msg *msg, int argc,
				     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PIE_CFG, "gPIEcfg");
}
COMMAND(iwlwav, gPIEcfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_pie_cfg, "");

static int handle_iwlwav_get_stations_statistics(struct nl80211_state *state,
						 struct nl_msg *msg, int argc,
						 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_STATIONS_STATISTICS, "gStationsStat");
}
COMMAND(iwlwav, gStationsStat, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_stations_statistics, "");

static int handle_iwlwav_get_rts_threshold(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RTS_THRESHOLD, "gRtsThreshold");
}
COMMAND(iwlwav, gRtsThreshold, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_rts_threshold, "");

static int handle_iwlwav_get_stats_poll_period(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_STATS_POLL_PERIOD, "gStatsPollPeriod");
}
COMMAND(iwlwav, gStatsPollPeriod, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_stats_poll_period, "");

static int handle_iwlwav_get_dynamic_mu_type(struct nl80211_state *state,
					     struct nl_msg *msg,
					     int argc, char **argv,
					     enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_DYNAMIC_MU_TYPE, "gDynamicMu");
}
COMMAND(iwlwav, gDynamicMu, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_dynamic_mu_type, "");

static int handle_iwlwav_get_he_mu_fixed_parameters(struct nl80211_state *state,
						    struct nl_msg *msg,
						    int argc, char **argv,
						    enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_HE_MU_FIXED_PARAMETERS, "gMuFixedCfg");
}
COMMAND(iwlwav, gMuFixedCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_he_mu_fixed_parameters, "");

static int handle_iwlwav_get_he_mu_duration(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_HE_MU_DURATION, "gMuDurationCfg");
}
COMMAND(iwlwav, gMuDurationCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_he_mu_duration, "");

static int handle_iwlwav_get_in_band_power(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PHY_INBAND_POWER, "gIBpowerPerAnt");
}
COMMAND(iwlwav, gIBpowerPerAnt, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_in_band_power, "");

static int handle_iwlwav_get_etsi_ppdu_limits(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
    return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ETSI_PPDU_LIMITS, "gETSILimitation");
}
COMMAND(iwlwav, gETSILimitation, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_etsi_ppdu_limits, "");

static int handle_iwlwav_get_he_operation_bss_color_disabled(struct nl80211_state *state,
							     struct nl_msg *msg, int argc,
							     char **argv, enum id_input id)
{
	return sub_cmd_print_bss_color_disabled_flag(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_HE_OPERATION,
						     sizeof(struct intel_vendor_he_oper), "gHEOperationBssColorDisabled");
}
COMMAND(iwlwav, gHEOperationBssColorDisabled, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_he_operation_bss_color_disabled, "");

static int handle_iwlwav_get_he_operation_bss_color(struct nl80211_state *state,
						    struct nl_msg *msg, int argc,
						    char **argv, enum id_input id)
{
	return sub_cmd_print_he_operation_bss_color(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_HE_OPERATION,
						    sizeof(struct intel_vendor_he_oper), "gHEOperationBssColor");
}
COMMAND(iwlwav, gHEOperationBssColor, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_he_operation_bss_color, "");

static int handle_iwlwav_get_probe_req_list(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_probe_req_list(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_LAST_PROBE_REQS, 0, "gProbeReqList");
}
COMMAND(iwlwav, gProbeReqList, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_probe_req_list, "");

static int handle_iwlwav_get_cca_preamble_puncture_cfg(struct nl80211_state *state,
					struct nl_msg *msg, int argc,
					char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_PREAMBLE_PUNCTURE_CFG, "gPreamPunCcaOvr");
}
COMMAND(iwlwav, gPreamPunCcaOvr, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cca_preamble_puncture_cfg, "");

/******************************************************************************/
/*! \brief      Prepare Intel vendor netlink command to retrieve data from LTQ_NL80211_VENDOR_SUBCMD_GET_TWT_PARAMETERS
 *
 *  \param[in]  state              pointer struct nl80211_state, unused param
 *  \param[out] msg                pointer to NL message data to be filled, must not be NULL
 *  \param[in]  argc               number of arguments, must be 1 (MAC address)
 *  \param[in]  argv               pointer to argument
 *  \param[in]  id                 from 'enum id_input', unused param
 *
 *  \return     0 on success, non 0 on error
 */
static int handle_iwlwav_get_twt_parameters(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	int count, stat = 0;
	uint8_t data8[6];
	unsigned int data[sizeof(data8)];
	unsigned int i;

	if (!msg)
		return -EFAULT;

	if (argc != 1)
		return -EINVAL;

	count = sscanf(argv[0], "%2x:%2x:%2x:%2x:%2x:%2x", &data[0], &data[1], &data[2], &data[3], &data[4], &data[5]);
	
	if (count != 6)
		return -EINVAL;
	
	for(i = 0; i < sizeof(data8); i++)
		data8[i] = (uint8_t) data[i];

	stat = sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TWT_PARAMETERS, "gTwtParams");
	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, sizeof(data8), (char *) data8);

	return stat;

nla_put_failure:
	return -ENOBUFS;
}
COMMAND(iwlwav, gTwtParams, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_twt_parameters, "");

static int handle_iwlwav_get_ax_default_params(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AX_DEFAULT_PARAMS, "gAxDefaultParams");
}
COMMAND(iwlwav, gAxDefaultParams, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ax_default_params, "");

static int handle_iwlwav_get_tx_retry_limit(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AP_RETRY_LIMIT, "gTxRetryLimit");
}
COMMAND(iwlwav, gTxRetryLimit, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_tx_retry_limit, "");

static int handle_iwlwav_get_tx_exce_retry_limit(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_AP_EXCE_RETRY_LIMIT, "gTxExceRetryLimit");
}
COMMAND(iwlwav, gTxExceRetryLimit, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_tx_exce_retry_limit, "");

static int handle_iwlwav_get_cts_to_self_to(struct nl80211_state *state,
					    struct nl_msg *msg,
					    int argc, char **argv,
					    enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CTS_TO_SELF_TO, "gCtsToSelfTo");
}
COMMAND(iwlwav, gCtsToSelfTo, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cts_to_self_to, "");

static int handle_iwlwav_get_tx_ampdu_density(struct nl80211_state *state,
					      struct nl_msg *msg,
					      int argc, char **argv,
					      enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TX_AMPDU_DENSITY, "gTxAmpduDensity");
}
COMMAND(iwlwav, gTxAmpduDensity, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_tx_ampdu_density, "");

static int handle_iwlwav_get_cca_msr(struct nl80211_state *state,
				     struct nl_msg *msg, int argc,
				     char **argv, enum id_input id)
{
	int count, stat = 0;
	unsigned int data;

	if (!msg)
		return -EFAULT;

	if (argc != 1)
		return -EINVAL;

	count = sscanf(argv[0], "%u", &data);

	if (count != 1)
		return -EINVAL;

	stat = sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_MSR_OFF_CHAN, "gGetCcaMsr");
	NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, sizeof(data), (char *) &data);

	return stat;

nla_put_failure:
	return -ENOBUFS;
}
COMMAND(iwlwav, gGetCcaMsr, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cca_msr, "");

static int handle_iwlwav_get_cca_stats(struct nl80211_state *state,
				     struct nl_msg *msg, int argc,
				     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_STATS_CURRENT_CHAN, "gGetCcaStats");
}
COMMAND(iwlwav, gGetCcaStats, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cca_stats, "");

static int handle_iwlwav_get_radio_usage_stats(struct nl80211_state *state,
				     struct nl_msg *msg, int argc,
				     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_USAGE_STATS, "gRadioUsageStats");
}
COMMAND(iwlwav, gRadioUsageStats, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_radio_usage_stats, "");

static int handle_iwlwav_get_slow_probing_mask(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PROBING_MASK, "gSlowProbingMask");
}
COMMAND(iwlwav, gSlowProbingMask, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_slow_probing_mask, "");

static int handle_iwlwav_get_scan_modif_flags(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_MODIFS, "gScanModifFlags");
}
COMMAND(iwlwav, gScanModifFlags, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_scan_modif_flags, "");

static int handle_iwlwav_get_scan_pause_bg_cache(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PAUSE_BG_CACHE, "gScanPauseBGCache");
}
COMMAND(iwlwav, gScanPauseBGCache, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_scan_pause_bg_cache, "");

static int handle_iwlwav_get_advertised_btwt_schedule(struct nl80211_state *state,
						      struct nl_msg *msg, int argc,
						      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_ADVERTISED_BTWT_SCHEDULE, "gAdvertisedBcTwtSp");
}
COMMAND(iwlwav, gAdvertisedBcTwtSp, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_advertised_btwt_schedule, "");

static int handle_iwlwav_get_config_mrcoex(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_COEX_CFG, "gConfigMRCoex");
}
COMMAND(iwlwav, gConfigMRCoex, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_config_mrcoex, "");

static int handle_iwlwav_get_fixed_ltf_gi(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_FIXED_LTF_AND_GI, "gFixedLtfGi");
}
COMMAND(iwlwav, gFixedLtfGi, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_fixed_ltf_gi, "");


/***************************** DEBUG GET COMMANDS ****************************/
#ifdef CONFIG_WAVE_DEBUG


static int handle_iwlwav_get_counters_src(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_COUNTERS_SRC, "gCountersSrc");
}
COMMAND(iwlwav, gCountersSrc, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_counters_src, "");

static int handle_iwlwav_get_unconn_time(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_UNCONNECTED_STA_SCAN_TIME, "gUnconnTime");
}
COMMAND(iwlwav, gUnconnTime, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_unconn_time, "");

static int handle_iwlwav_get_fixed_power(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_FIXED_POWER, "gFixedPower");
}
COMMAND(iwlwav, gFixedPower, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_fixed_power, "");

static int handle_iwlwav_get_ctrl_bfilter_bank(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_IRE_CTRL_B, "gCtrlBFilterBank");
}
COMMAND(iwlwav, gCtrlBFilterBank, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ctrl_bfilter_bank, "");

static int handle_iwlwav_get_cpu_dma_latency(struct nl80211_state *state,
					     struct nl_msg *msg, int argc,
					     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_CPU_DMA_LATENCY, "gCpuDmaLatency");
}
COMMAND(iwlwav, gCpuDmaLatency, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_cpu_dma_latency, "");

static int handle_iwlwav_get_bf_explicit_cap(struct nl80211_state *state,
					     struct nl_msg *msg, int argc,
					     char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_BEAMFORM_EXPLICIT, "gBfExplicitCap");
}
COMMAND(iwlwav, gBfExplicitCap, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_bf_explicit_cap, "");

static int handle_iwlwav_get_tasklet_limits(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TASKLET_LIMITS, "gTaskletLimits");
}
COMMAND(iwlwav, gTaskletLimits, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_tasklet_limits, "");

static int handle_iwlwav_get_genl_family_id(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_GENL_FAMILY_ID, "gGenlFamilyId");
}
COMMAND(iwlwav, gGenlFamilyId, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_genl_family_id, "");

static int handle_iwlwav_get_scan_exp_time(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_EXP_TIME, "gScanExpTime");
}
COMMAND(iwlwav, gScanExpTime, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_scan_exp_time, "");

static int handle_iwlwav_get_scan_params(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS, "gScanParams");
}
COMMAND(iwlwav, gScanParams, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_scan_params, "");

static int handle_iwlwav_get_scan_params_bg(struct nl80211_state *state,
					    struct nl_msg *msg, int argc,
					    char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG, "gScanParamsBG");
}
COMMAND(iwlwav, gScanParamsBG, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_scan_params_bg, "");

static int handle_iwlwav_get_ta_dbg(struct nl80211_state *state,
				    struct nl_msg *msg, int argc,
				    char **argv, enum id_input id)
{
	return sub_cmd_print_text_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TA_DBG, "gTADbg");
}
COMMAND(iwlwav, gTADbg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ta_dbg, "");

static int handle_iwlwav_get_ta_timer_res(struct nl80211_state *state,
					  struct nl_msg *msg, int argc,
					  char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_TA_TIMER_RESOLUTION, "gTATimerRes");
}
COMMAND(iwlwav, gTATimerRes, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_ta_timer_res, "");

static int handle_iwlwav_get_pcoc_auto_cfg(struct nl80211_state *state,
					   struct nl_msg *msg, int argc,
					   char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PCOC_AUTO_PARAMS, "gPCoCAutoCfg");
}
COMMAND(iwlwav, gPCoCAutoCfg, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_pcoc_auto_cfg, "");

static int handle_iwlwav_get_pcoc_power(struct nl80211_state *state,
					struct nl_msg *msg, int argc,
					char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_PCOC_POWER_MODE, "gPCoCPower");
}
COMMAND(iwlwav, gPCoCPower, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_pcoc_power, "");

static int handle_iwlwav_get_wds_host_to(struct nl80211_state *state,
					 struct nl_msg *msg, int argc,
					 char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_WDS_HOST_TIMEOUT, "gWDSHostTO");
}
COMMAND(iwlwav, gWDSHostTO, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_wds_host_to, "");

static int handle_iwlwav_get_mac_wd_period_ms(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MAC_WATCHDOG_PERIOD_MS, "gMACWdPeriodMs");
}
COMMAND(iwlwav, gMACWdPeriodMs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_mac_wd_period_ms, "");

static int handle_iwlwav_get_mac_wd_timeout_ms(struct nl80211_state *state,
					       struct nl_msg *msg, int argc,
					       char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_MAC_WATCHDOG_TIMEOUT_MS, "gMACWdTimeoutMs");
}
COMMAND(iwlwav, gMACWdTimeoutMs, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_mac_wd_timeout_ms, "");

static int handle_iwlwav_get_non_occupate_prd(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_NON_OCCUPATED_PRD, "gNonOccupatePrd");
}
COMMAND(iwlwav, gNonOccupatePrd, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_non_occupate_prd, "");

static int handle_iwlwav_get_11h_beacon_count(struct nl80211_state *state,
					      struct nl_msg *msg, int argc,
					      char **argv, enum id_input id)
{
	return sub_cmd_print_int_function(msg, LTQ_NL80211_VENDOR_SUBCMD_GET_11H_BEACON_COUNT, "g11hBeaconCount");
}
COMMAND(iwlwav, g11hBeaconCount, "", NL80211_CMD_VENDOR, 0, CIB_NETDEV, handle_iwlwav_get_11h_beacon_count, "");

#endif

/***************************** HELP FUNCTION *****************************/

static int handle_iwlwav_help(struct nl80211_state *state,
			      struct nl_msg *msg, int argc,
			      char **argv, enum id_input id)
{
	printf("\tdev <devname> iwlwav s11hRadarDetect <radar detection>\n");
	printf("\t\tSet 11h radar detection.\n\n");

	printf("\tdev <devname> iwlwav s11hChCheckTime <channel check time>\n");
	printf("\t\tSet 11h channel availability check time.\n\n");

	printf("\tdev <devname> iwlwav s11hEmulatRadar <emualte radar>\n");
	printf("\t\tSet emulate radar detection.\n\n");

	printf("\tdev <devname> iwlwav emulateInterferer\n");
	printf("\t\tSet emulate Interferer detection.\n\n");

	printf("\tdev <devname> iwlwav sAddPeerAP <peer ap>\n");
	printf("\t\tSet add peer ap.\n\n");

	printf("\tdev <devname> iwlwav sDelPeerAP <peer ap>\n");
	printf("\t\tSet delete peer ap.\n\n");

	printf("\tdev <devname> iwlwav sPeerAPkeyIdx <peer ap key index>\n");
	printf("\t\tSet peer ap key index.\n\n");

	printf("\tdev <devname> iwlwav sBridgeModen <bridge mode>\n");
	printf("\t\tSet bridge mode.\n\n");

	printf("\tdev <devname> iwlwav sReliableMcast <reliable multicast>\n");
	printf("\t\tSet reliable multicast.\n\n");

	printf("\tdev <devname> iwlwav sAPforwarding <AP forwarding>\n");
	printf("\t\tSet AP forwarding.\n\n");

	printf("\tdev <devname> iwlwav sLtPathEnabled <enabled>\n");
	printf("\t\tSet lite path enabled.\n\n");

	printf("\tdev <devname> iwlwav sIpxPpaEnabled <enabled>\n");
	printf("\t\tSet ipx ppa enabled.\n\n");

	printf("\tdev <devname> iwlwav sUdmaEnabled <enabled>\n");
	printf("\t\tSet udma enabled.\n\n");

	printf("\tdev <devname> iwlwav sUdmaEnabledExt <enabled>\n");
	printf("\t\tSet udma ext enabled.\n\n");

	printf("\tdev <devname> iwlwav sUdmaVlanId <enabled>\n");
	printf("\t\tSet udma vlan id.\n\n");

	printf("\tdev <devname> iwlwav sUdmaVlanIdExt <enabled>\n");
	printf("\t\tSet udma vlan id ext.\n\n");

	printf("\tdev <devname> iwlwav sCoCPower <enable mode> <tx num> <rx num>\n");
	printf("\t\tSet COC power mode.\n\n");

	printf("\tdev <devname> iwlwav sCoCAutoCfg <interval_1x1> <interval_2x2> <interval_3x3> < interval_4x4> <high_limit_1x1> <low_limit_2x2> <high_limit_2x2> <low_limit_3x3> <high_limit_3x3> <low_limit_4x4>\n");
	printf("\t\tSet COC auto parameters.\n\n");

	printf("\tdev <devname> iwlwav sTpcLoopType <loop type>\n");
	printf("\t\tSet TPC loop type.\n\n");

	printf("\tdev <devname> iwlwav sInterfDetThresh <notification_threshold> \n");
	printf("\t\tSet interferer detection threshold\n\n");

	printf("\tdev <devname> iwlwav s11bAntSelection <txAnt> <rxAnt> <Rate>\n");
	printf("\t\tSet 11b antenna selection.\n\n");

	printf("\tdev <devname> iwlwav sFWRecovery <recovery mode> <number of Fast recovery> <number of full recovery> <dump evacuation on fault> <time period for consecutive recovery>\n");
	printf("\t\tSet FW recovery.\n\n");

	printf("\tdev <devname> iwlwav sEnableRadio <enable radio>\n");
	printf("\t\tSet radio mode.\n\n");

	printf("\tdev <devname> iwlwav sAggrConfig <amsdu_mode> <ba_mode> [<window_size>]\n");
	printf("\t\tSet aggr config.\n\n");

	printf("\tdev <devname> iwlwav sNumMsduInAmsdu <amsdu_num> <amsdu_vnum>\n");
	printf("\t\tSet amsdu number.\n\n");

	printf("\tdev <devname> iwlwav sAggRateLimit <mode> <maxRate>\n");
	printf("\t\tSet aggr rate limit.\n\n");

	printf("\tdev <devname> iwlwav sMuOfdmaBf <mode> <bfPeriod>\n");
	printf("\t\tSet mu ofdma beamforming.\n\n");

	printf("\tdev <devname> iwlwav sAvailAdmCap <admission capacity>\n");
	printf("\t\tSet admission capacity.\n\n");

	printf("\tdev <devname> iwlwav sSetRxTH <rx threshold>\n");
	printf("\t\tSet rx threshold.\n\n");

	printf("\tdev <devname> iwlwav sRxDutyCyc <onTime> <offTime>\n");
	printf("\t\tSet rx duty cycle.\n\n");

	printf("\tdev <devname> iwlwav sPowerSelection <power selection>\n");
	printf("\t\tSet tx power limit offset.\n\n");

	printf("\tdev <devname> iwlwav s11nProtection <protection>\n");
	printf("\t\tSet s11 protection method.\n\n");

	printf("\tdev <devname> iwlwav sCalibOnDemand <calib on demand>\n");
	printf("\t\tSet calib on demand.\n\n");

	printf("\tdev <devname> iwlwav sQAMplus <QAMplus mode>\n");
	printf("\t\tSet QAMplus mode.\n\n");

	printf("\tdev <devname> iwlwav sAcsUpdateTo <update to>\n\n");
	printf("\t\tSet acs update to.\n\n");

	printf("\tdev <devname> iwlwav sMuOperation <mu operation>\n");
	printf("\t\tSet mu operation.\n\n");

	printf("\tdev <devname> iwlwav sCcaTh <primary> <secondary> <midPktPrimary> <midPktSecondary20> <midPktSecondary40>\n");
	printf("\t\tSet CCA threshold.\n\n");

	printf("\tdev <devname> iwlwav sCcaAdapt <initial interval> <iterative interval> <limit> <step up> <step down> <step down interval> <min unblocked time>\n");
	printf("\t\tSet CCA adapt.\n\n");

	printf("\tdev <devname> iwlwav sRadarRssiTh <radar rssi>\n");
	printf("\t\tSet radar rssi threashold.\n\n");

	printf("\tdev <devname> iwlwav sRTSmode <dynamic_bw> <static_bw>\n");
	printf("\t\tSet RTS mode.\n\n");

	printf("\tdev <devname> iwlwav sMaxMpduLen <max length>\n");
	printf("\t\tSet max mpdu length.\n\n");

	printf("\tdev <devname> iwlwav sBfMode <bf mode>\n");
	printf("\t\tSet bf mode.\n\n");

	printf("\tdev <devname> iwlwav sProbeReqCltMode <enable/disable probe request collection>\n");
	printf("\t\tSet bf mode.\n\n");

	printf("\tdev <devname> iwlwav sActiveAntMask <active ant mask>\n");
	printf("\t\tSet active ant mask.\n\n");

	printf("\tdev <devname> iwlwav sFourAddrMode <4addr mode>\n");
	printf("\t\tSet 4addr mode.\n\n");

	printf("\tdev <devname> iwlwav sAddFourAddrSta <mac address>\n");
	printf("\t\tSet add 4addr STA.\n\n");

	printf("\tdev <devname> iwlwav sDelFourAddrSta <mac address>\n");
	printf("\t\tSet delete 4addr STA.\n\n");

	printf("\tdev <devname> iwlwav sTxopConfig <STA ID> <Mode> <Duration> <Max number of STAs>\n");
	printf("\t\tSet txop config.\n\n");

	printf("\tdev <devname> iwlwav sSsbMode <value_1> <value_2>\n");
	printf("\t\tSet SSB mode.\n\n");

	printf("\tdev <devname> iwlwav sMcastRange <operation>,<mcast range>\n");
	printf("\t\tSet mcast range.\n\n");
	printf("\t\t<operation> = 0 delete all.\n\n");
	printf("\t\t<operation> = 1 add.\n\n");
	printf("\t\t<operation> = 2 delete.\n\n");

	printf("\tdev <devname> iwlwav sMcastRange6 <operation>,<mcast range6>\n");
	printf("\t\tSet mcast range.\n\n");
	printf("\t\t<operation> = 0 delete all.\n\n");
	printf("\t\t<operation> = 1 add.\n\n");
	printf("\t\t<operation> = 2 delete.\n\n");

	printf("\tdev <devname> iwlwav sOnlineACM <online acm>\n");
	printf("\t\tSet online acm.\n\n");

	printf("\tdev <devname> iwlwav sAlgoCalibrMask <calibr mask>\n");
	printf("\t\tSet algo calibration mask.\n\n");

	printf("\tdev <devname> iwlwav sFreqJumpMode <frequency jump mode>\n");
	printf("\t\tSet frequency jump mode.\n\n");

	printf("\tdev <devname> iwlwav sWhmReset <Reset WHM>\n");
	printf("\t\tSet WHM Reset.\n\n");

	printf("\tdev <devname> iwlwav sWhmTrigger <WHM WarningId 101...150> <WHM WarningLayer>\n");
	printf("\t\tSet WHM Trigger.\n\n");

	printf("\tdev <devname> iwlwav sRestrictAcMode <restrictedAcModeEnable> <acRestrictedBitmap> <restrictedAcThreshEnter> <restrictedAcThreshExit>\n");
	printf("\t\tSet restricted ac mode.\n\n");

	printf("\tdev <devname> iwlwav sPdThresh <mode> <minPdDiff> <minPdAmount>\n");
	printf("\t\tSet pd threshold.\n\n");

	printf("\tdev <devname> iwlwav sFastDrop <fast drop>\n");
	printf("\t\tSet fast drop.\n\n");

	printf("\tdev <devname> iwlwav sMuStatPlanCfg <19 to 170 params> <...>\n");
	printf("\t\tSet mu stat plan cfg.\n\n");

	printf("\tdev <devname> iwlwav sWdsWepEncCfg <key_id> <wds wep key>\n");
	printf("\t\tSet wds wep key.\n\n");

	printf("\tdev <devname> iwlwav sRtsRate <0..2>\n");
	printf("\t\tSet RTS protection rate.\n\n");

	printf("\tdev <devname> iwlwav sPIEcfg <param_1> <..> <param_n>\n");
	printf("\t\tSet PIE configuration.\n\n");

	printf("\tdev <devname> iwlwav sStationsStat <0..1>\n");
	printf("\t\tSet stations statistics (enable/disable).\n\n");

	printf("\tdev <devname> iwlwav sTxExceRetryLimit <threshold[0..255]>\n");
	printf("\t\tSet excessive retry limit.\n\n");

	printf("\tdev <devname> iwlwav sStatsPollPeriod<1..300>\n");
	printf("\t\tSet statistics auto polling period.\n\n");
	printf("\tdev <devname> iwlwav sDynamicMu <dl mu type> <ul mu type> <min stas in group> <max stas in group> <cdb cfg>\n");
	printf("\t\tSet Dynamic MU type.\n\n");
	printf("\tdev <devname> iwlwav sMuFixedCfg <mu sequence> <ltf gi> <coding type> <he rate>\n");
	printf("\t\tSet HE MU Fixed parameters.\n\n");
	printf("\tdev <devname> iwlwav sMuDurationCfg <ppdu duration> <txop duration> <tf length> <num of repetitions>\n");
	printf("\t\tSet HE MU Duration.\n\n");
	printf("\tdev <devname> iwlwav sETSILimitation <enable/disable>\n");
	printf("\t\tSet ETSI PPDU Limitation.\n\n");
	printf("\tdev <devname> iwlwav sTxRetryLimit <mgmt[0...15]> <data[0...15]>\n");
	printf("\t\tSet Tx Retry Limit.\n\n");
	printf("\tdev <devname> iwlwav sCtsToSelfTo <1..32ms>\n");
	printf("\t\tSet Cts to self timeout.\n\n");

	printf("\tdev <devname> iwlwav sNfrpCfg\n\n");
	printf("\t\tSet Nfrp configuration.\n\n");

	printf("\tdev <devname> iwlwav sPreamPunCcaOvr\n\n");
	printf("\t\tSet Preamble puncture cca override.\n\n");

	printf("\tdev <devname> iwlwav sSlowProbingMask <probing mask>\n");
	printf("\t\tSet slow probing mask.\n\n");

	printf("\tdev <devname> iwlwav sScanModifFlags <modifs flags>\n");
	printf("\t\tSet scan modifs.\n\n");

	printf("\tdev <devname> iwlwav sScanPauseBGCache <flag>\n");
	printf("\t\tSet scan pause bg cache.\n\n");

	printf("\tdev <devname> iwlwav sAdvertiseBcTwtSp <numberOfSpsToAdd> <wakeDurationUnit> [<broadcastTwtId> <flowType> <triggerType> <twtWakeDuration> <twtWakeIntervalMantissa> <wakeIntervalExponent>]\n");
	printf("\t\tAdvertise broadcast TWT schedule(s) upto a maximum of 8 schedules per request. The fields starting from broadcastTwtId and upto wakeIntervalExponent are per schedule.\n\n");

	printf("\tdev <devname> iwlwav sTerminateBcTwtSP <broadcastTwtPersistence>\n");
	printf("\t\tTerminate all broadcast TWT schedules after a fixed number of TBTTs indicated by the persistence field.\n\n");

	printf("\tdev <devname> iwlwav sTxTwtTeardown <staId> <allTWT> <agreementType> <twtId>\n");
	printf("\t\tTransmit a TWT teardown frame to an associated station with the configured parameters.\n\n");

	printf("\tdev <devname> iwlwav sConfigMRCoex <coex mode> <active time> <inactive time> <cts2self active>\n");
	printf("\t\tSet coex enable.\n\n");

	printf("\tdev <devname> iwlwav sFixedLtfGi <0-fixed, 1-auto> <value for fixed>\n");
	printf("\t\tSet fixed lt fgi.\n\n");

/*************************** DEBUG SET COMMANDS ****************************/
#ifdef CONFIG_WAVE_DEBUG
	printf("\tdev <devname> iwlwav sCountersSrc <counter src>\n");
	printf("\t\tSet switch counter src.\n\n");

	printf("\tdev <devname> iwlwav sUnconnTime <unconnected scan time>\n");
	printf("\t\tSet unconnected STA scan time.\n\n");

	printf("\tdev <devname> iwlwav sFixedPower <vapId> <stationId> <powerVal> <changeType>\n");
	printf("\t\tSet fixed power.\n\n");

	printf("\tdev <devname> iwlwav sCtrlBFilterBank <filter bank>\n");
	printf("\t\tSet control b filter bank.\n\n");

	printf("\tdev <devname> iwlwav sCpuDmaLatency <latency>\n");
	printf("\t\tSet control CPU DMA Latency.\n\n");

	printf("\tdev <devname> iwlwav sTaskletLimits <data_txout_lim> <data_rx_lim> <bss_rx_lim> <bss_cfm_lim> <legacy_lim>\n");
	printf("\t\tSet tasklet limits.\n\n");

	printf("\tdev <devname> iwlwav sScanExpTime <exp time>\n");
	printf("\t\tSet scan exp time.\n\n");

	printf("\tdev <devname> iwlwav sScanParams <passiveScanTime> <activeScanTime> <numProbeReqs> <probeReqInterval> <passiveScanValidTime> <activeScanValidTime>\n");
	printf("\t\tSet scan parameters.\n\n");

	printf("\tdev <devname> iwlwav sScanParamsBG <passiveScanTimeBG> <activeScanTimeBG> <numProbeReqsBG> <probeReqIntervalBG> "
	       "<numChansInChunkBG> <chanChunkIntervalBG> <window_slice> <window_slice_overlap> <cts_to_self_duration>\n");
	printf("\t\tSet scan parameters background.\n\n");

	printf("\tdev <devname> iwlwav sTATimerRes <timer resolution>\n");
	printf("\t\tSet TA timer resolution.\n\n");

	printf("\tdev <devname> iwlwav sPCoCAutoCfg <interval_low2high> <interval_high2low> <limit_lower> <limit_upper active_polling_timeout>\n");
	printf("\t\tSet PCOC auto parameters.\n\n");

	printf("\tdev <devname> iwlwav sPCoCPower <enable mode>\n");
	printf("\t\tSet PCOC power mode.\n\n");

	printf("\tdev <devname> iwlwav sWDSHostTO <host timeput>\n");
	printf("\t\tSet WDS host timeout.\n\n");

	printf("\tdev <devname> iwlwav sMACWdPeriodMs <period ms>\n");
	printf("\t\tSet mac watchdog period ms.\n\n");

	printf("\tdev <devname> iwlwav sMACWdTimeoutMs <timeout ms>\n");
	printf("\t\tSet mac watchdog timeout ms.\n\n");

	printf("\tdev <devname> iwlwav sNonOccupatePrd <non occupated period>\n");
	printf("\t\tSet non occupated period.\n\n");

	printf("\tdev <devname> iwlwav s11hBeaconCount <beacon count>\n");
	printf("\t\tSet 11h beacon count.\n\n");

	printf("\tdev <devname> iwlwav sEnableTestBus <enable/disable>\n");
	printf("\t\tSet enable test bus.\n\n");

	printf("\tdev <devname> iwlwav sFixedRateCfg <stationIndex> <isAutoRate> <bw> <phyMode> <nss> <mcs> <scp> <dcm> <heExtPartialBwData> <heExtPartialBwMng> <changeType>\n");
	printf("\t\tSet fixed rate.\n\n");

	printf("\tdev <devname> iwlwav sDoDebugAssert <assert type> [optional type]\n");
	printf("\t\tAssert FW.\n\n");

	printf("\tdev <devname> iwlwav sDoFwDebug <value>\n");
	printf("\t\tSet FW debug.\n\n");

	printf("\tdev <devname> iwlwav sDoSimpleCLI <value>\n");
	printf("\t\tSet debug CLI.\n\n");

	printf("\tdev <devname> iwlwav sPMCUDebug <debug>\n");
	printf("\t\tSet pmcu debug.\n\n");

	printf("\tdev <devname> iwlwav sFwLogSeverity <newLevel> <targetCPU>\n");
	printf("\t\tSet fw log severity.\n\n");

	printf("\tdev <devname> iwlwav sDebugCmdFw <CMD ID> <param1 size> <param1 value> [param2 size] [param2 value] ...\n");
	printf("\t\tManually compose any message to FW.\n\n");
#endif

	printf("\tdev <devname> iwlwav sStartCcaMsr <channel> <dwellTimeMs>\n");
	printf("\t\tStarts a CCA (Clear Channel Assessment) scan for a given interface and the given channel using a specific dwell time.\n\n");

	printf("\tdev <devname> iwlwav g11hRadarDetect\n\n");
	printf("\tdev <devname> iwlwav g11hChCheckTime\n\n");
	printf("\tdev <devname> iwlwav gPeerAPkeyIdx\n\n");
	printf("\tdev <devname> iwlwav gPeerAPs\n\n");
	printf("\tdev <devname> iwlwav gBridgeMode\n\n");
	printf("\tdev <devname> iwlwav gReliableMcast\n\n");
	printf("\tdev <devname> iwlwav gAPforwarding\n\n");
	printf("\tdev <devname> iwlwav gEEPROM\n\n");

	printf("\tdev <devname> iwlwav gDataPathMode\n");
	printf("\t\tGet actual datapath mode for a specified device\n\n");

	printf("\tdev <devname> iwlwav gLtPathEnabled\n\n");
	printf("\tdev <devname> iwlwav gIpxPpaEnabled\n\n");
	printf("\tdev <devname> iwlwav gUdmaEnabled\n\n");
	printf("\tdev <devname> iwlwav gUdmaEnabledExt\n\n");
	printf("\tdev <devname> iwlwav gUdmaVlanId\n\n");
	printf("\tdev <devname> iwlwav gUdmaVlanIdExt\n\n");
	printf("\tdev <devname> iwlwav gCoCPower\n\n");
	printf("\tdev <devname> iwlwav gCoCAutoCfg\n\n");
	printf("\tdev <devname> iwlwav gTpcLoopType\n\n");
	printf("\tdev <devname> iwlwav gInterfDetThresh\n\n");
	printf("\tdev <devname> iwlwav gAPCapsMaxSTAs\n\n");
	printf("\tdev <devname> iwlwav gAPCapsMaxVAPs\n\n");
	printf("\tdev <devname> iwlwav g11bAntSelection\n\n");
	printf("\tdev <devname> iwlwav gFWRecovery\n\n");
	printf("\tdev <devname> iwlwav gFWRecoveryStat\n\n");
	printf("\tdev <devname> iwlwav gEnableRadio\n\n");
	printf("\tdev <devname> iwlwav gAggrConfig\n\n");
	printf("\tdev <devname> iwlwav gNumMsduInAmsdu\n\n");
	printf("\tdev <devname> iwlwav gAggRateLimit\n\n");
	printf("\tdev <devname> iwlwav gMuOfdmaBf\n\n");
	printf("\tdev <devname> iwlwav gAvailAdmCap\n\n");
	printf("\tdev <devname> iwlwav gSetRxTH\n\n");
	printf("\tdev <devname> iwlwav gRxDutyCyc\n\n");
	printf("\tdev <devname> iwlwav gPowerSelection\n\n");
	printf("\tdev <devname> iwlwav g11nProtection\n\n");
	printf("\tdev <devname> iwlwav gTemperature\n\n");
	printf("\tdev <devname> iwlwav gQAMplus\n\n");
	printf("\tdev <devname> iwlwav gAcsUpdateTo\n\n");
	printf("\tdev <devname> iwlwav gMuOperation\n\n");
	printf("\tdev <devname> iwlwav gCcaTh\n\n");
	printf("\tdev <devname> iwlwav gCcaAdapt\n\n");
	printf("\tdev <devname> iwlwav gRadarRssiTh\n\n");
	printf("\tdev <devname> iwlwav gRTSmode\n\n");
	printf("\tdev <devname> iwlwav gMaxMpduLen\n\n");
	printf("\tdev <devname> iwlwav gBfMode\n\n");
	printf("\tdev <devname> iwlwav gProbeReqCltMode\n\n");
	printf("\tdev <devname> iwlwav gActiveAntMask\n\n");
	printf("\tdev <devname> iwlwav gFourAddrMode\n\n");
	printf("\tdev <devname> iwlwav gFourAddrStas\n\n");
	printf("\tdev <devname> iwlwav gTxopConfig\n\n");
	printf("\tdev <devname> iwlwav gSsbMode\n\n");
	printf("\tdev <devname> iwlwav gMcastRange\n\n");
	printf("\tdev <devname> iwlwav gMcastRange6\n\n");
	printf("\tdev <devname> iwlwav gOnlineACM\n\n");
	printf("\tdev <devname> iwlwav gAlgoCalibrMask\n\n");
	printf("\tdev <devname> iwlwav gRestrictAcMode\n\n");
	printf("\tdev <devname> iwlwav gPdThresh\n\n");
	printf("\tdev <devname> iwlwav gFastDrop\n\n");
	printf("\tdev <devname> iwlwav gPVT\n\n");
	printf("\tdev <devname> iwlwav gRtsRate\n\n");
	printf("\tdev <devname> iwlwav gPIEcfg\n\n");
	printf("\tdev <devname> iwlwav gStationsStat\n\n");
	printf("\tdev <devname> iwlwav gStatsPollPeriod\n\n");
	printf("\tdev <devname> iwlwav gDynamicMu\n\n");
	printf("\tdev <devname> iwlwav gMuFixedCfg\n\n");
	printf("\tdev <devname> iwlwav gMuDurationCfg\n\n");
	printf("\tdev <devname> iwlwav gIBpowerPerAnt\n\n");
	printf("\tdev <devname> iwlwav gHEOperationBssColorDisabled\n\n");
	printf("\tdev <devname> iwlwav gHEOperationBssColor\n\n");
	printf("\tdev <devname> iwlwav gTwtParams <station mac address>\n\n");
	printf("\tdev <devname> iwlwav gAxDefaultParams\n\n");
	printf("\tdev <devname> iwlwav gETSILimitation\n\n");
	printf("\tdev <devname> iwlwav gRtsThreshold\n\n");
	printf("\tdev <devname> iwlwav gTxRetryLimit\n\n");
	printf("\tdev <devname> iwlwav gCtsToSelfTo\n\n");
	printf("\tdev <devname> iwlwav gGetCcaMsr <channel>\n\n");
	printf("\tdev <devname> iwlwav gGetCcaStats\n\n");
	printf("\tdev <devname> iwlwav sNfrpCfg\n\n");
	printf("\tdev <devname> iwlwav sPreamPunCcaOvr\n\n");
	printf("\tdev <devname> iwlwav gPreamPunCcaOvr\n\n");
	printf("\tdev <devname> iwlwav gRadioUsageStats\n\n");
	printf("\tdev <devname> iwlwav gSlowProbingMask\n\n");
	printf("\tdev <devname> iwlwav gScanModifFlags\n\n");
	printf("\tdev <devname> iwlwav gScanPauseBGCache\n\n");
	printf("\tdev <devname> iwlwav gAdvertiseBcTwtSp\n\n");
	printf("\tdev <devname> iwlwav gConfigMRCoex\n\n");
	printf("\tdev <devname> iwlwav gFixedLtfGi\n\n");
	printf("\tdev <devname> iwlwav gTxExceRetryLimit\n\n");

/*************************** DEBUG GET COMMANDS ****************************/
#ifdef CONFIG_WAVE_DEBUG
	printf("\tdev <devname> iwlwav sMtlkLogLevel <oid:0-31> <level:0-2> <mode:1-3>\n \
			\t\t set mtlk debug level oid: e.g. 8(mtlk) 6(tools) level: e.g. 1(level one) mode:e.g. (1)cdebug\n\n");
	printf("\tdev <devname> iwlwav gCountersSrc\n\n");
	printf("\tdev <devname> iwlwav gUnconnTime\n\n");
	printf("\tdev <devname> iwlwav gFixedPower\n\n");
	printf("\tdev <devname> iwlwav gCtrlBFilterBank\n\n");
	printf("\tdev <devname> iwlwav gCpuDmaLatency\n\n");
	printf("\tdev <devname> iwlwav gBfExplicitCap\n\n");
	printf("\tdev <devname> iwlwav gTaskletLimits\n\n");
	printf("\tdev <devname> iwlwav gGenlFamilyId\n\n");
	printf("\tdev <devname> iwlwav gScanExpTime\n\n");
	printf("\tdev <devname> iwlwav gScanParams\n\n");
	printf("\tdev <devname> iwlwav gScanParamsBG\n\n");
	printf("\tdev <devname> iwlwav gTADbg\n\n");
	printf("\tdev <devname> iwlwav gTATimerRes\n\n");
	printf("\tdev <devname> iwlwav gPCoCAutoCfg\n\n");
	printf("\tdev <devname> iwlwav gPCoCPower\n\n");\
	printf("\tdev <devname> iwlwav gWDSHostTO\n\n");
	printf("\tdev <devname> iwlwav gCdbCfg\n\n");
	printf("\tdev <devname> iwlwav gMACWdPeriodMs\n\n");
	printf("\tdev <devname> iwlwav gMACWdTimeoutMs\n\n");
	printf("\tdev <devname> iwlwav gNonOccupatePrd\n\n");
	printf("\tdev <devname> iwlwav g11hBeaconCount\n\n");
#endif

	return 0;
}
COMMAND(iwlwav, help, "", 0, 0, CIB_NETDEV, handle_iwlwav_help, "");
