#ifndef __IEEE80211
#define __IEEE80211

/* 802.11n HT capability AMPDU settings (for ampdu_params_info) */
#define IEEE80211_HT_AMPDU_PARM_FACTOR          0x03
#define IEEE80211_HT_AMPDU_PARM_DENSITY         0x1C

#define IEEE80211_HT_CAP_SUP_WIDTH_20_40        0x0002
#define IEEE80211_HT_CAP_SGI_40                 0x0040
#define IEEE80211_HT_CAP_MAX_AMSDU              0x0800

#define IEEE80211_HT_MCS_MASK_LEN               10

/**
 * struct ieee80211_mcs_info - MCS information
 * @rx_mask: RX mask
 * @rx_highest: highest supported RX rate. If set represents
 *      the highest supported RX data rate in units of 1 Mbps.
 *      If this field is 0 this value should not be used to
 *      consider the highest RX data rate supported.
 * @tx_params: TX parameters
 */
struct ieee80211_mcs_info {
	__u8 rx_mask[IEEE80211_HT_MCS_MASK_LEN];
	__u16 rx_highest;
	__u8 tx_params;
	__u8 reserved[3];
} __attribute__ ((packed));


/**
 * struct ieee80211_ht_cap - HT capabilities
 *
 * This structure is the "HT capabilities element" as
 * described in 802.11n D5.0 7.3.2.57
 */
struct ieee80211_ht_cap {
	__u16 cap_info;
	__u8 ampdu_params_info;

	/* 16 bytes MCS information */
	struct ieee80211_mcs_info mcs;

	__u16 extended_ht_cap_info;
	__u32 tx_BF_cap_info;
	__u8 antenna_selection_info;
} __attribute__ ((packed));

struct ieee80211_vht_mcs_info {
	__u16 rx_vht_mcs;
	__u16 rx_highest;
	__u16 tx_vht_mcs;
	__u16 tx_highest;
} __attribute__ ((packed));

struct ieee80211_vht_cap {
	__u32 cap_info;
	struct ieee80211_vht_mcs_info mcs;
} __attribute__ ((packed));

/* Element ID Extension (EID 255) values */
#define WLAN_EID_EXT_HE_CAPABILITIES 35
#define WLAN_EID_EXT_HE_OPERATION 36

/* HE MAC Capabilities Information field defines */
#define HE_MAC_CAP_LEN 6
#define HE_MACCAP_CAP0_IDX 0
#define HE_MAC_CAP0_HTC_HE_SUPPORT ((uint8_t) BIT(0))
#define HE_MAC_CAP0_TWT_REQUESTER_SUPPORT ((uint8_t) BIT(1))
#define HE_MAC_CAP0_TWT_RESPONDER_SUPPORT ((uint8_t) BIT(2))
#define HE_MAC_CAP0_DYNAMIC_FRAGMENTATION_SUPPORT ((uint8_t) (BIT(3) | BIT(4)))
#define HE_MAC_CAP0_MAX_NUM_OF_FRAG_MSDU ((uint8_t) (BIT(5) | BIT(6) | BIT(7)))

#define HE_MACCAP_CAP1_IDX 1
#define HE_MAC_CAP1_MINIMUM_FRAGMENT_SIZE ((uint8_t) (BIT(0) | BIT(1)))
#define HE_MAC_CAP1_TRIGGER_FRAME_MAC_PAD_DUR ((uint8_t) (BIT(2) | BIT(3)))
#define HE_MAC_CAP1_MULTI_TID_AGGR_RX_SUPPORT ((uint8_t) (BIT(4) | BIT(5) | BIT(6)))
/* HE_MACCAP_HE_LINK_ADAPTION_SUPPORT  B15, B16 */
#define HE_MAC_CAP1_HE_LINK_ADAPTION_SUPPORT ((uint8_t) BIT(7))
#define HE_MAC_CAP1_HE_LINK_ADAPTION_SUPPORT_WIDTH 1

#define HE_MACCAP_CAP2_IDX 2
#define HE_MAC_CAP2_HE_LINK_ADAPTION_SUPPORT ((uint8_t) BIT(0))
#define HE_MAC_CAP2_ALL_ACK_SUPPORT ((uint8_t) BIT(1))
#define HE_MAC_CAP2_TRS_SUPPORT ((uint8_t) BIT(2))
#define HE_MAC_CAP2_BSR_SUPPORT ((uint8_t) BIT(3))
#define HE_MAC_CAP2_BROADCAST_TWT_SUPPORT ((uint8_t) BIT(4))
#define HE_MAC_CAP2_32BIT_BA_BITMAP_SUPPORT ((uint8_t) BIT(5))
#define HE_MAC_CAP2_MU_CASCADING_SUPPORT ((uint8_t) BIT(6))
#define HE_MAC_CAP2_ACK_ENABLED_AGGREGATION_SUPPORT ((uint8_t) BIT(7))

#define HE_MACCAP_CAP3_IDX 3
/* B24 - Reserved */
#define HE_MAC_CAP3_OM_CONTROL_SUPPORT ((uint8_t) BIT(1))
#define HE_MAC_CAP3_OFDMA_RA_SUPPORT ((uint8_t) BIT(2))
#define HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT ((uint8_t) (BIT(3) | BIT(4)))
#define HE_MAC_CAP3_AMSDU_FRGMENTATION_SUPPORT ((uint8_t) BIT(5))
#define HE_MAC_CAP3_FLEXIBLE_TWT_SCHEDULE_SUPPORT ((uint8_t) BIT(6))
#define HE_MAC_CAP3_RX_CONTROL_FRAME_TO_MULTIBSS ((uint8_t) BIT(7))

#define HE_MACCAP_CAP4_IDX 4
#define HE_MAC_CAP4_BSRP_BQRP_AMPDU_AGGREGATION ((uint8_t) BIT(0))
#define HE_MAC_CAP4_QTP_SUPPORT ((uint8_t) BIT(1))
#define HE_MAC_CAP4_BQR_SUPPORT ((uint8_t) BIT(2))
#define HE_MAC_CAP4_SRP_RESPONDER ((uint8_t) BIT(3))
#define HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT ((uint8_t) BIT(4))
#define HE_MAC_CAP4_OPS_SUPPORT ((uint8_t) BIT(5))
#define HE_MAC_CAP4_AMSDU_IN_ACKENABLED_AMPDU_SUPPORT ((uint8_t) BIT(6))
/* HE_MACCAP_MULTI_TID_AGGREGATION_TX_SUPPORT B39, B40, B41 */
#define HE_MAC_CAP4_MULTI_TID_AGGR_TX_SUPPORT ((uint8_t) (BIT(7)))
#define HE_MAC_CAP4_MULTI_TID_AGGR_TX_SUPPORT_WIDTH 1

#define HE_MACCAP_CAP5_IDX 5
#define HE_MAC_CAP5_MULTI_TID_AGGR_TX_SUPPORT ((uint8_t) (BIT(0) | BIT(1)))
#define HE_MAC_CAP5_HE_SUBCHANNEL_SELE_TRANS_SUP ((uint8_t) BIT(2))
#define HE_MAC_CAP5_UL_2X996TONE_RU_SUPPORT ((uint8_t) BIT(3))
#define HE_MAC_CAP5_OM_CONTROL_UL_MU_DATA_DIS_RX_SUP ((uint8_t) BIT(4))
#define HE_MAC_CAP5_HE_DYNAMIC_SM_POWER_SAVE ((uint8_t) BIT(5))
#define HE_MAC_CAP5_PUNCTURED_SOUNDING_SUPPORT ((uint8_t) BIT(6))
#define HE_MAC_CAP5_HT_AND_VHT_TRIGGER_FRAME_RX_SUPPORT ((uint8_t) BIT(7))




/* HE PHY Capabilities Information field defines */
#define HE_PHY_CAP_LEN 11
#define HE_PHYCAP_CAP0_IDX 0
/* B0 - Reserved */
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0 (uint8_t) BIT(1)
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1 (uint8_t) BIT(2)
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2 (uint8_t) BIT(3)
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B3 (uint8_t) BIT(4)
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B4 (uint8_t) BIT(5)
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B5 (uint8_t) BIT(6)
#define HE_PHY_CAP0_CHANNEL_WIDTH_SET_B6 (uint8_t) BIT(7)

#define HE_PHYCAP_CAP1_IDX 1
#define HE_PHY_CAP1_PUN_PREAM_RX ((uint8_t) (BIT(0) | BIT(1) | BIT(2) | BIT(3)))
#define HE_PHY_CAP1_DEVICE_CLASS ((uint8_t) BIT(4))
#define HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD ((uint8_t) BIT(5))
#define HE_PHY_CAP1_SU_PPDU_1XHE_LTF_0_8US_GI ((uint8_t) BIT(6))
/* HE_PHYCAP_MIDAMBLE_TXRX_MAX__NSTS B15 B16 */
#define HE_PHY_CAP1_MIDAMBLE_TXRX_MAX__NSTS_PART1 ((uint8_t) BIT(7))
#define HE_PHY_CAP1_MIDAMBLE_TXRX_MAX__NSTS_PART1_WIDTH 1

#define HE_PHYCAP_CAP2_IDX 2
#define HE_PHY_CAP2_MIDAMBLE_TXRX_MAX__NSTS_PART2 ((uint8_t) BIT(0))
#define HE_PHY_CAP2_NDP_4X_HE_LTF_AND_3_2US_GI ((uint8_t) BIT(1))
#define HE_PHY_CAP2_STBC_TX_LESS_OR_EQUAL_80MHz ((uint8_t) BIT(2))
#define HE_PHY_CAP2_STBC_RX_LESS_OR_EQUAL_80MHz ((uint8_t) BIT(3))
#define HE_PHY_CAP2_DOPPLER_TX ((uint8_t) BIT(4))
#define HE_PHY_CAP2_DOPPLER_RX ((uint8_t) BIT(5))
#define HE_PHY_CAP2_FULL_BANDWIDTH_UL_MU_MIMO ((uint8_t) BIT(6))
#define HE_PHY_CAP2_PARTIAL_BANDWIDTH_UL_MU_MIMO ((uint8_t) BIT(7))

#define HE_PHYCAP_CAP3_IDX 3
#define HE_PHY_CAP3_DCM_MAX_CONSTELLATION_TX ((uint8_t) (BIT(0) | BIT(1)))
#define HE_PHY_CAP3_DCM_MAX_NSS_TX ((uint8_t) BIT(2))
#define HE_PHY_CAP3_DCM_MAX_CONSTELLATION_RX ((uint8_t) (BIT(3) | BIT(4)))
#define HE_PHY_CAP3_DCM_MAX_NSS_RX ((uint8_t) BIT(5))
#define HE_PHY_CAP3_RX_PARTIAL_BW_SU_USING_HE_MUPPDU_FROM_NON_AP_STA ((uint8_t) BIT(6))
#define HE_PHY_CAP3_SU_BEAMFORMER ((uint8_t) BIT(7))

#define HE_PHYCAP_CAP4_IDX 4
#define HE_PHY_CAP4_SU_BEAMFORMEE ((uint8_t) BIT(0))
#define HE_PHY_CAP4_MU_BEAMFORMER ((uint8_t) BIT(1))
#define HE_PHY_CAP4_BF_STS_LESS_OR_EQ_80MHz ((uint8_t) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PHY_CAP4_BF_STS_GREATER_THAN_80MHz ((uint8_t) (BIT(5) | BIT(6) | BIT(7)))

#define HE_PHYCAP_CAP5_IDX 5
#define HE_PHY_CAP5_NUM_SOUND_DIM_LESS_80MHz ((uint8_t) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PHY_CAP5_NUM_SOUND_DIM_GREAT_80MHz ((uint8_t) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PHY_CAP5_NG_16_FOR_SU_FB_SUPPORT ((uint8_t) BIT(6))
#define HE_PHY_CAP5_NG_16_FOR_MU_FB_SUPPORT ((uint8_t) BIT(7))

#define HE_PHYCAP_CAP6_IDX 6
#define HE_PHY_CAP6_CODEBOOK_SIZE42_FOR_SU_SUPPORT ((uint8_t) BIT(0))
#define HE_PHY_CAP6_CODEBOOK_SIZE75_FOR_MU_SUPPORT ((uint8_t) BIT(1))
#define HE_PHY_CAP6_TRIGGERED_SU_BEAMFORMING_FEEDBACK ((uint8_t) BIT(2))
#define HE_PHY_CAP6_TRIGGERED_MU_BEAMFORMING_PARTIAL_BW_FEEDBACK ((uint8_t) BIT(3))
#define HE_PHY_CAP6_TRIGGERED_CQI_FEEDBACK ((uint8_t) BIT(4))
#define HE_PHY_CAP6_PARTIAL_BANDWIDTH_EXTENDED_RANGE ((uint8_t) BIT(5))
#define HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MU_MIMO ((uint8_t) BIT(6))
#define HE_PHY_CAP6_PPE_THRESHOLDS_PRESENT ((uint8_t) BIT(7))

#define HE_PHYCAP_CAP7_IDX 7
#define HE_PHY_CAP7_SRP_BASED_SR_SUPPORT ((uint8_t) BIT(0))
#define HE_PHY_CAP7_POWER_BOOST_FACTOR_SUPPORT ((uint8_t) BIT(1))
#define HE_PHY_CAP7_SU_PPDU_AND_HE_MU_WITH_4X_HE_LTF_0_8US_GI ((uint8_t) BIT(2))
#define HE_PHY_CAP7_MAX_NC ((uint8_t) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PHY_CAP7_STBC_TX_GREATER_THAN_80MHz ((uint8_t) BIT(6))
#define HE_PHY_CAP7_STBC_RX_GREATER_THAN_80MHz ((uint8_t) BIT(7))

#define HE_PHYCAP_CAP8_IDX 8
#define HE_PHY_CAP8_HE_ER_SU_PPDU_4X_HE_LTF_0_8_US_GI ((uint8_t) BIT(0))
#define HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_2_4_GHZ_BAND ((uint8_t) BIT(1))
#define HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU ((uint8_t) BIT(2))
#define HE_PHY_CAP8_80MHZ_IN_160MHZ_HE_PPDU ((uint8_t) BIT(3))
#define HE_PHY_CAP8_HE_ER_SU_PPDU_1X_HE_LTF_0_8_US_GI ((uint8_t) BIT(4))
#define HE_PHY_CAP8_MIDAMBLE_TX_RX_2X_AND_1X_HE_LTF ((uint8_t) BIT(5))
#define HE_PHY_CAP8_DCM_MAX_RU ((uint8_t) (BIT(6) | BIT(7)))

#define HE_PHYCAP_CAP9_IDX 9
#define HE_PHY_CAP9_LONGER_THAN_16_HE_SIGB_OFDM_SYMBOLS_SUPPORT ((uint8_t) BIT(0))
#define HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK ((uint8_t) BIT(1))
#define HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU_SUPPORT ((uint8_t) BIT(2))
#define HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU_SUPPORT ((uint8_t) BIT(3))
#define HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_PPDU_NON_COMP_SIGB ((uint8_t) BIT(4))
#define HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_PPDU_COMP_SIGB ((uint8_t) BIT(5))
#define HE_PHY_CAP9_NOMINAL_PACKET_PADDING ((uint8_t) (BIT(6) | BIT(7)))

#define HE_MCS_NSS_FOR_1SS ((uint8_t) (BIT(0) | BIT(1)))
#define HE_MCS_NSS_FOR_2SS ((uint8_t) (BIT(2) | BIT(3)))
#define HE_MCS_NSS_FOR_3SS ((uint8_t) (BIT(4) | BIT(5)))
#define HE_MCS_NSS_FOR_4SS ((uint8_t) (BIT(6) | BIT(7)))
#define HE_MCS_NSS_FOR_5SS ((uint8_t) (BIT(0) | BIT(1)))
#define HE_MCS_NSS_FOR_6SS ((uint8_t) (BIT(2) | BIT(3)))
#define HE_MCS_NSS_FOR_7SS ((uint8_t) (BIT(4) | BIT(5)))
#define HE_MCS_NSS_FOR_8SS ((uint8_t) (BIT(6) | BIT(7)))

/* HE PPE Thresholds field */
#define HE_PPE_NSTS ((uint8_t) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_RU_INDEX_BITMASK ((uint8_t) (BIT(3) | BIT(4) | BIT(5) | BIT(6)))

/*PPET PATTERN WILL REPEAT*/
#define HE_PPE_0_PPET16_PART1 ((uint8_t) (BIT(7)))
#define HE_PPE_0_PPET16_PART1_WIDTH 1
#define HE_PPE_0_PPET16_PART2 ((uint8_t) (BIT(0) | BIT(1)))
#define HE_PPE_0_PPET8 ((uint8_t) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PPE_1_PPET16 ((uint8_t) (BIT(5) | BIT(6) | BIT(7)))
#define HE_PPE_1_PPET8 ((uint8_t) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_2_PPET16 ((uint8_t) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PPE_2_PPET8_PART1 ((uint8_t) (BIT(6) | BIT(7)))
#define HE_PPE_2_PPET8_PART1_WIDTH 2
#define HE_PPE_2_PPET8_PART2 ((uint8_t) (BIT(0)))
#define HE_PPE_3_PPET16 ((uint8_t) (BIT(1) | BIT(2) | BIT(3)))
#define HE_PPE_3_PPET8 ((uint8_t) (BIT(4) | BIT(5) | BIT(6)))

enum ppecount_index {
	PPECOUNT0 = 0,
	PPECOUNT1 = 1,
	PPECOUNT2 = 2,
	PPECOUNT3 = 3,
};


/* HE Operation parameter defines */
#define HE_OPERATION_CAP0_DEFAULT_PE_DURATION ((uint8_t) (BIT(0) | BIT(1) | BIT(2)))
#define HE_OPERATION_CAP0_TWT_REQUIRED ((uint8_t) (BIT(3)))
#define HE_OPERATION_CAP0_TXOP_DUR_RTS_TH ((uint8_t) (BIT(4) | BIT(5) | BIT(6) | BIT(7)))
#define HE_OPERATION_CAP0_TXOP_DUR_RTS_TH_WIDTH 4

#define HE_OPERATION_CAP1_TXOP_DUR_RTS_TH ((uint8_t) (BIT(0) | BIT(1) | \
					BIT(2) | BIT(3) | BIT(4) | BIT(5)))
#define HE_OPERATION_CAP1_VHT_OPER_INFO_PRESENT ((uint8_t) BIT(6))
#define HE_OPERATION_CAP1_CO_HOSTED_BSS ((uint8_t) BIT(7))

#define HE_OPERATION_CAP2_ER_SU_DISABLE ((uint8_t) BIT(0)) /* B16 */
#define SIX_GHZ_OPERATION_INFO_PRESENT ((uint8_t) BIT(1)) /* B17 */
/* B18 ...B23 Reserved */

/* HE BSS Color Information field defines */
#define HE_OPERATION_BSS_COLOR ((uint8_t) (BIT(0) | BIT(1) | \
				BIT(2) | BIT(3) | BIT(4) | BIT(5)))
#define HE_OPERATION_PARTIAL_BSS_COLOR ((uint8_t) BIT(6))
#define HE_OPERATION_BSS_COLOR_DISABLED ((uint8_t) BIT(7))

#define SIX_GHZ_OPERATION_INFO_CHANNEL_WIDTH ((uint8_t) (BIT(0) | BIT(1)))

#endif /* __IEEE80211 */
