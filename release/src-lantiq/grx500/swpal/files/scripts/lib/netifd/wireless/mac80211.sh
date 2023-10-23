#!/bin/sh
. /lib/netifd/netifd-wireless.sh
. /lib/netifd/hostapd.sh
. /lib/wifi/platform_dependent.sh

init_wireless_driver "$@"

MP_CONFIG_INT="mesh_retry_timeout mesh_confirm_timeout mesh_holding_timeout mesh_max_peer_links
	       mesh_max_retries mesh_ttl mesh_element_ttl mesh_hwmp_max_preq_retries
	       mesh_path_refresh_time mesh_min_discovery_timeout mesh_hwmp_active_path_timeout
	       mesh_hwmp_preq_min_interval mesh_hwmp_net_diameter_traversal_time mesh_hwmp_rootmode
	       mesh_hwmp_rann_interval mesh_gate_announcements mesh_sync_offset_max_neighor
	       mesh_rssi_threshold mesh_hwmp_active_path_to_root_timeout mesh_hwmp_root_interval
	       mesh_hwmp_confirmation_interval mesh_awake_window mesh_plink_timeout"
MP_CONFIG_BOOL="mesh_auto_open_plinks mesh_fwding"
MP_CONFIG_STRING="mesh_power_mode"
WAVE_D2="0x980"
DEFAULT_SHARED_HAPD=hostapd

get_hwid_from_device()
{
        radio_interface=`uci show wireless | grep $1 | cut -d. -f2 | cut -d$'\n' -f1`
        default_radio=`uci show wireless | grep "device='$radio_interface'"| cut -d. -f1-2 | cut -d$'\n' -f1`
        ifname=`uci show $default_radio".ifname" | cut -d"=" -f2`
        ifname=`echo $ifname | awk '{print substr($0, 2, length($0) - 2)}'`
        ifname=`echo $ifname | cut -d"." -f1`
        hw_id=`cat /proc/net/mtlk/$ifname/eeprom_parsed | grep -e "HW ID" | cut -d":" -f2 | cut -d"," -f2`
        echo $hw_id
}

drv_mac80211_init_ax_config() {
	config_add_int sDynamicMuTypeDownLink
	config_add_int sDynamicMuTypeUpLink
	config_add_int sDynamicMuMinStationsInGroup
	config_add_int sDynamicMuMaxStationsInGroup
	config_add_int sDynamicMuCdbConfig
	config_add_int he_su_beamformer
	config_add_int he_su_beamformee
	config_add_int he_mu_beamformer
	config_add_int he_bss_color
	config_add_boolean he_bss_color_randomize
	config_add_int he_operation_bss_color_disabled
	config_add_int he_default_pe_duration
	config_add_int he_twt_required
	config_add_int he_rts_threshold
	config_add_int he_oper_chwidth
	config_add_int he_oper_centr_freq_seg0_idx
	config_add_int he_oper_centr_freq_seg1_idx
	config_add_int he_basic_mcs_nss_set
	config_add_int he_mu_edca_qos_info_param_count
	config_add_int he_mu_edca_qos_info_q_ack
	config_add_int he_mu_edca_qos_info_queue_request
	config_add_int he_mu_edca_qos_info_txop_request
	config_add_int he_mu_edca_ac_be_aifsn
	config_add_int he_mu_edca_ac_be_ecwmin
	config_add_int he_mu_edca_ac_be_ecwmax
	config_add_int he_mu_edca_ac_be_timer
	config_add_int he_mu_edca_ac_bk_aifsn
	config_add_int he_mu_edca_ac_bk_aci
	config_add_int he_mu_edca_ac_bk_ecwmin
	config_add_int he_mu_edca_ac_bk_ecwmax
	config_add_int he_mu_edca_ac_bk_timer
	config_add_int he_mu_edca_ac_vi_ecwmin
	config_add_int he_mu_edca_ac_vi_ecwmax
	config_add_int he_mu_edca_ac_vi_aifsn
	config_add_int he_mu_edca_ac_vi_aci
	config_add_int he_mu_edca_ac_vi_timer
	config_add_int he_mu_edca_ac_vo_aifsn
	config_add_int he_mu_edca_ac_vo_aci
	config_add_int he_mu_edca_ac_vo_ecwmin
	config_add_int he_mu_edca_ac_vo_ecwmax
	config_add_int he_mu_edca_ac_vo_timer
	config_add_int he_spr_sr_control
	config_add_int he_spr_non_srg_obss_pd_max_offset
	config_add_int he_spr_srg_obss_pd_min_offset
	config_add_int he_spr_srg_obss_pd_max_offset
	config_add_int multibss_enable
	config_add_int he_phy_preamble_puncturing_rx
	config_add_boolean he_operation_er_su_disable
}

drv_mac80211_init_device_config() {
	hostapd_common_add_device_config
	drv_mac80211_init_ax_config

	config_add_string path phy 'macaddr:macaddr'
	config_add_string hwmode band atf_config_file whm_config_file
	config_add_string acs_smart_info_file acs_history_file
	config_add_string \
		acs_penalty_factors \
		acs_to_degradation \
		acs_chan_cust_penalty \
		acs_grp_priorities_throughput \
		acs_grp_priorities_reach
	config_add_boolean \
		acs_policy \
		acs_bw_comparison
	config_add_int acs_switch_thresh
	config_add_int beacon_int chanbw frag rts dfs_debug_chan externally_managed testbed_mode
	config_add_int rxantenna txantenna txpower distance sFixedLtfGi
	config_add_string shared_hapd
	config_add_array ht_capab
	config_add_array channels acs_fallback_chan
	config_add_boolean \
		rxldpc \
		short_gi_80 \
		short_gi_160 \
		tx_stbc_2by1 \
		su_beamformer \
		su_beamformee \
		mu_beamformer \
		mu_beamformee \
		vht_txop_ps \
		htc_vht \
		rx_antenna_pattern \
		tx_antenna_pattern \
		he_beacon
	config_add_int vht_max_a_mpdu_len_exp vht_max_mpdu vht_link_adapt vht160 rx_stbc tx_stbc
	config_add_boolean \
		ldpc \
		greenfield \
		short_gi_20 \
		short_gi_40 \
		max_amsdu \
		dsss_cck_40 \
		disable_reconf
	config_add_boolean atf
	config_add_int atf_interval atf_free_time atf_debug vht_oper_chwidth
	config_add_int obss_interval obss_beacon_rssi_threshold ignore_40_mhz_intolerant
	config_add_boolean full_ch_master_control
	config_add_string ap_retry_limit
	config_add_string ap_retry_limit_data
	config_add_int unsolicited_frame_support unsolicited_frame_duration
	config_add_boolean mu_mimo_operation

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		config_add_string hostapd_log_level
		debug_infrastructure_config_add_string debug_iw_pre_up_
		debug_infrastructure_config_add_string debug_iw_post_up_
	fi
}

drv_mac80211_init_iface_config() {
	hostapd_common_add_bss_config

	config_add_string 'macaddr:macaddr' ifname

	config_add_boolean powersave
	config_add_int maxassoc
	config_add_int multi_ap_profile
	config_add_int wds
	config_add_int max_listen_int
	config_add_int dtim_period
	config_add_int start_disabled
	config_add_int atf_vap_grant
	config_add_array 'atf_sta_grants:list(macaddr,int)'
	config_add_string vendor_elems
	config_add_string start_after

	# mesh
	config_add_string mesh_id
	config_add_int $MP_CONFIG_INT
	config_add_boolean $MP_CONFIG_BOOL
	config_add_string $MP_CONFIG_STRING
}

mac80211_append_ax_parameters() {
	if [ "$ieee80211ax" = "1" ]; then

		json_get_vars \
			sDynamicMuTypeDownLink \
			sDynamicMuTypeUpLink \
			sDynamicMuMinStationsInGroup \
			sDynamicMuMaxStationsInGroup \
			sDynamicMuCdbConfig \
			he_su_beamformer \
			he_su_beamformee \
			he_mu_beamformer \
			he_bss_color \
			he_operation_bss_color_disabled \
			he_bss_color_randomize \
			he_default_pe_duration \
			he_twt_required \
			he_rts_threshold \
			he_oper_chwidth \
			he_oper_centr_freq_seg0_idx \
			he_oper_centr_freq_seg1_idx \
			he_basic_mcs_nss_set \
			he_mu_edca_qos_info_param_count \
			he_mu_edca_qos_info_q_ack \
			he_mu_edca_qos_info_queue_request \
			he_mu_edca_qos_info_txop_request \
			he_mu_edca_ac_be_aifsn \
			he_mu_edca_ac_be_ecwmin \
			he_mu_edca_ac_be_ecwmax \
			he_mu_edca_ac_be_timer \
			he_mu_edca_ac_bk_aifsn \
			he_mu_edca_ac_bk_aci \
			he_mu_edca_ac_bk_ecwmin \
			he_mu_edca_ac_bk_ecwmax \
			he_mu_edca_ac_bk_timer \
			he_mu_edca_ac_vi_ecwmin \
			he_mu_edca_ac_vi_ecwmax \
			he_mu_edca_ac_vi_aifsn \
			he_mu_edca_ac_vi_aci \
			he_mu_edca_ac_vi_timer \
			he_mu_edca_ac_vo_aifsn \
			he_mu_edca_ac_vo_aci \
			he_mu_edca_ac_vo_ecwmin \
			he_mu_edca_ac_vo_ecwmax \
			he_mu_edca_ac_vo_timer \
			he_spr_sr_control \
			he_spr_non_srg_obss_pd_max_offset \
			he_spr_srg_obss_pd_min_offset \
			he_spr_srg_obss_pd_max_offset \
			multibss_enable \
			he_phy_preamble_puncturing_rx \
			unsolicited_frame_support \
			unsolicited_frame_duration \
			he_operation_er_su_disable


		append base_cfg "ieee80211ax=$ieee80211ax" "$N"
		[ -n "$multibss_enable" ] && append base_cfg "multibss_enable=$multibss_enable" "$N"
		[ -n "$sDynamicMuTypeDownLink" ] && append base_cfg "sDynamicMuTypeDownLink=$sDynamicMuTypeDownLink" "$N"
		[ -n "$sDynamicMuTypeUpLink" ] && append base_cfg "sDynamicMuTypeUpLink=$sDynamicMuTypeUpLink" "$N"
		[ -n "$sDynamicMuMinStationsInGroup" ] && append base_cfg "sDynamicMuMinStationsInGroup=$sDynamicMuMinStationsInGroup" "$N"
		[ -n "$sDynamicMuMaxStationsInGroup" ] && append base_cfg "sDynamicMuMaxStationsInGroup=$sDynamicMuMaxStationsInGroup" "$N"
		[ -n "$sDynamicMuCdbConfig" ] && append base_cfg "sDynamicMuCdbConfig=$sDynamicMuCdbConfig" "$N"
		[ -n "$he_su_beamformer" ] && append base_cfg "he_su_beamformer=$he_su_beamformer" "$N"
		[ -n "$he_su_beamformee" ] && append base_cfg "he_su_beamformee=$he_su_beamformee" "$N"
		[ -n "$he_mu_beamformer" ] && append base_cfg "he_mu_beamformer=$he_mu_beamformer" "$N"
		if [ -n "$he_bss_color_randomize" ] && [ "$he_bss_color_randomize" = "1" ]; then
			local rand_bss_color=$(awk 'BEGIN { srand(); printf("%d\n",1+rand()*63) }')
			he_bss_color="$rand_bss_color"
		fi
		[ -n "$he_bss_color" ] && append base_cfg "he_bss_color=$he_bss_color" "$N"
		[ -n "$he_operation_bss_color_disabled" ] && append base_cfg "he_operation_bss_color_disabled=$he_operation_bss_color_disabled" "$N"
		[ -n "$he_default_pe_duration" ] && append base_cfg "he_default_pe_duration=$he_default_pe_duration" "$N"
		[ -n "$he_twt_required" ] && append base_cfg "he_twt_required=$he_twt_required" "$N"
		[ -n "$he_rts_threshold" ] && append base_cfg "he_rts_threshold=$he_rts_threshold" "$N"
		[ -n "$he_oper_chwidth" ] && append base_cfg "he_oper_chwidth=$he_oper_chwidth" "$N"
		[ -n "$he_oper_centr_freq_seg0_idx" ] && append base_cfg "he_oper_centr_freq_seg0_idx=$he_oper_centr_freq_seg0_idx" "$N"
		[ -n "$he_oper_centr_freq_seg1_idx" ] && append base_cfg "he_oper_centr_freq_seg1_idx=$he_oper_centr_freq_seg1_idx" "$N"
		[ -n "$he_basic_mcs_nss_set" ] && append base_cfg "he_basic_mcs_nss_set=$he_basic_mcs_nss_set" "$N"
		[ -n "$he_mu_edca_qos_info_param_count" ] && append base_cfg "he_mu_edca_qos_info_param_count=$he_mu_edca_qos_info_param_count" "$N"
		[ -n "$he_mu_edca_qos_info_q_ack" ] && append base_cfg "he_mu_edca_qos_info_q_ack=$he_mu_edca_qos_info_q_ack" "$N"
		[ -n "$he_mu_edca_qos_info_queue_request" ] && append base_cfg "he_mu_edca_qos_info_queue_request=$he_mu_edca_qos_info_queue_request" "$N"
		[ -n "$he_mu_edca_qos_info_txop_request" ] && append base_cfg "he_mu_edca_qos_info_txop_request=$he_mu_edca_qos_info_txop_request" "$N"
		[ -n "$he_mu_edca_ac_be_aifsn" ] && append base_cfg "he_mu_edca_ac_be_aifsn=$he_mu_edca_ac_be_aifsn" "$N"
		[ -n "$he_mu_edca_ac_be_ecwmin" ] && append base_cfg "he_mu_edca_ac_be_ecwmin=$he_mu_edca_ac_be_ecwmin" "$N"
		[ -n "$he_mu_edca_ac_be_ecwmax" ] && append base_cfg "he_mu_edca_ac_be_ecwmax=$he_mu_edca_ac_be_ecwmax" "$N"
		[ -n "$he_mu_edca_ac_be_timer" ] && append base_cfg "he_mu_edca_ac_be_timer=$he_mu_edca_ac_be_timer" "$N"
		[ -n "$he_mu_edca_ac_bk_aifsn" ] && append base_cfg "he_mu_edca_ac_bk_aifsn=$he_mu_edca_ac_bk_aifsn" "$N"
		[ -n "$he_mu_edca_ac_bk_aci" ] && append base_cfg "he_mu_edca_ac_bk_aci=$he_mu_edca_ac_bk_aci" "$N"
		[ -n "$he_mu_edca_ac_bk_ecwmin" ] && append base_cfg "he_mu_edca_ac_bk_ecwmin=$he_mu_edca_ac_bk_ecwmin" "$N"
		[ -n "$he_mu_edca_ac_bk_ecwmax" ] && append base_cfg "he_mu_edca_ac_bk_ecwmax=$he_mu_edca_ac_bk_ecwmax" "$N"
		[ -n "$he_mu_edca_ac_bk_timer" ] && append base_cfg "he_mu_edca_ac_bk_timer=$he_mu_edca_ac_bk_timer" "$N"
		[ -n "$he_mu_edca_ac_vi_ecwmin" ] && append base_cfg "he_mu_edca_ac_vi_ecwmin=$he_mu_edca_ac_vi_ecwmin" "$N"
		[ -n "$he_mu_edca_ac_vi_ecwmax" ] && append base_cfg "he_mu_edca_ac_vi_ecwmax=$he_mu_edca_ac_vi_ecwmax" "$N"
		[ -n "$he_mu_edca_ac_vi_aifsn" ] && append base_cfg "he_mu_edca_ac_vi_aifsn=$he_mu_edca_ac_vi_aifsn" "$N"
		[ -n "$he_mu_edca_ac_vi_aci" ] && append base_cfg "he_mu_edca_ac_vi_aci=$he_mu_edca_ac_vi_aci" "$N"
		[ -n "$he_mu_edca_ac_vi_timer" ] && append base_cfg "he_mu_edca_ac_vi_timer=$he_mu_edca_ac_vi_timer" "$N"
		[ -n "$he_mu_edca_ac_vo_aifsn" ] && append base_cfg "he_mu_edca_ac_vo_aifsn=$he_mu_edca_ac_vo_aifsn" "$N"
		[ -n "$he_mu_edca_ac_vo_aci" ] && append base_cfg "he_mu_edca_ac_vo_aci=$he_mu_edca_ac_vo_aci" "$N"
		[ -n "$he_mu_edca_ac_vo_ecwmin" ] && append base_cfg "he_mu_edca_ac_vo_ecwmin=$he_mu_edca_ac_vo_ecwmin" "$N"
		[ -n "$he_mu_edca_ac_vo_ecwmax" ] && append base_cfg "he_mu_edca_ac_vo_ecwmax=$he_mu_edca_ac_vo_ecwmax" "$N"
		[ -n "$he_mu_edca_ac_vo_timer" ] && append base_cfg "he_mu_edca_ac_vo_timer=$he_mu_edca_ac_vo_timer" "$N"
		[ -n "$he_spr_sr_control" ] && append base_cfg "he_spr_sr_control=$he_spr_sr_control" "$N"
		[ -n "$he_spr_non_srg_obss_pd_max_offset" ] && append base_cfg "he_spr_non_srg_obss_pd_max_offset=$he_spr_non_srg_obss_pd_max_offset" "$N"
		[ -n "$he_spr_srg_obss_pd_min_offset" ] && append base_cfg "he_spr_srg_obss_pd_min_offset=$he_spr_srg_obss_pd_min_offset" "$N"
		[ -n "$he_spr_srg_obss_pd_max_offset" ] && append base_cfg "he_spr_srg_obss_pd_max_offset=$he_spr_srg_obss_pd_max_offset" "$N"
		hw_id=`get_hwid_from_device $phy`
		if [ "$hw_id" = "$WAVE_D2" ]; then
			if [ "$band" = "5GHz" ] || [ "$band" = "6GHz" ]; then
				[ -n "$he_phy_preamble_puncturing_rx" ] && append base_cfg "he_phy_preamble_puncturing_rx=$he_phy_preamble_puncturing_rx" "$N"
			fi
		fi
		[ -n "$unsolicited_frame_support" ] && append base_cfg "unsolicited_frame_support=$unsolicited_frame_support" "$N"
		[ -n "$unsolicited_frame_duration" ] && append base_cfg "unsolicited_frame_duration=$unsolicited_frame_duration" "$N"
		[ -n "$he_operation_er_su_disable" ] && append base_cfg "he_operation_er_su_disable=$he_operation_er_su_disable" "$N"
	fi
}

mac80211_add_capabilities() {
	local __var="$1"; shift
	local __mask="$1"; shift
	local __out= oifs

	oifs="$IFS"
	IFS=:
	for capab in "$@"; do
		set -- $capab

		[ "$(($4))" -gt 0 ] || continue
		[ "$(($__mask & $2))" -eq "$((${3:-$2}))" ] || continue
		__out="$__out[$1]"
	done
	IFS="$oifs"

	export -n -- "$__var=$__out"
}

get_ht_capab() {
	local ht_capab=

	if [ "$auto_channel" -gt 0 -o "$channel" = "first_non_dfs" ]; then
		if [ "$htmode" != "VHT20" ] && [ "$htmode" != "HT20" ]; then
			ht_capab="[HT40+][HT40-]"
		fi
	else
		case "$htmode" in
			VHT20|HT20) ;;
			HT40+|VHT40+) ht_capab="[HT40+]" ;;
			HT40-|VHT40-) ht_capab="[HT40-]" ;;
			*) # 80 or above, add HT40+ if channel allows it
				ht_capab="[HT40+]"
				case "$channel" in
					8|9|10|11|12|13|40|48|56|64|104|112|120|128|136|144|153|161) ht_capab="[HT40-]" ;;
				esac
			;;
		esac
	fi

	echo "$ht_capab"
}

mac80211_hostapd_prepare_atf_config() {
	local config_file="$1"
	local atf_cfg=

	json_get_vars atf atf_interval atf_free_time atf_debug

	rm -f "$config_file"

	set_default atf_debug 0
	set_default atf 0
	set_default atf_interval 1000
	set_default atf_free_time 0
	append atf_cfg "debug=$atf_debug" "$N"
	append atf_cfg "distr_type=$atf" "$N"
	append atf_cfg "algo_type=1" "$N"
	append atf_cfg "vap_enabled=1" "$N"
	append atf_cfg "station_enabled=1" "$N"
	append atf_cfg "weighted_type=0" "$N"
	append atf_cfg "interval=$atf_interval" "$N"
	append atf_cfg "free_time=$atf_free_time" "$N"

	cat >> "$config_file" <<EOF
$atf_cfg

EOF
}

mac80211_hostapd_setup_base() {
	local phy="$1"

	json_select config
	json_get_vars band full_ch_master_control vht_oper_chwidth doth
	set_default full_ch_master_control 0
	json_get_vars acs_smart_info_file acs_history_file obss_beacon_rssi_threshold
	[ "$auto_channel" -gt 0 ] && {
		channel=acs_smart
	}
	json_get_values channel_list channels
	json_get_values acs_fallback_chan_list acs_fallback_chan

	json_get_values ht_capab

	ieee80211n=
	ieee80211ac=
	ieee80211ax=
	case "$hwmode" in
		ng|bgn|n|an) ieee80211n=1 ;;
		ac) ieee80211ac=1 ;;
		nac|anac)
			ieee80211n=1
			ieee80211ac=1
		;;
		ax) ieee80211ax=1 ;;
		bgnax)
			ieee80211n=1
			ieee80211ax=1
		;;
		anacax)
			ieee80211n=1
			ieee80211ac=1
			ieee80211ax=1
		;;
	esac

	case "$hwmode" in
		*g*) hwmode=g ;;
		*b*) hwmode=b ;;
		*a*) hwmode=a ;;
	esac

	ht_capab=`get_ht_capab`

	[ -n "$ieee80211n" ] && {
		append base_cfg "ieee80211n=1" "$N"

		json_get_vars \
			ldpc:1 \
			greenfield:0 \
			short_gi_20:1 \
			short_gi_40:1 \
			tx_stbc:1 \
			rx_stbc:3 \
			max_amsdu:1 \
			dsss_cck_40:1

		ht_cap_mask=0
		for cap in $(iw phy "$phy" info | grep 'Capabilities:' | cut -d: -f2); do
			ht_cap_mask="$(($ht_cap_mask | $cap))"
		done

		cap_rx_stbc=$((($ht_cap_mask >> 8) & 3))
		[ "$rx_stbc" -lt "$cap_rx_stbc" ] && cap_rx_stbc="$rx_stbc"
		ht_cap_mask="$(( ($ht_cap_mask & ~(0x300)) | ($cap_rx_stbc << 8) ))"

		mac80211_add_capabilities ht_capab_flags $ht_cap_mask \
			LDPC:0x1::$ldpc \
			GF:0x10::$greenfield \
			SHORT-GI-20:0x20::$short_gi_20 \
			SHORT-GI-40:0x40::$short_gi_40 \
			TX-STBC:0x80::$tx_stbc \
			RX-STBC1:0x300:0x100:1 \
			RX-STBC12:0x300:0x200:1 \
			RX-STBC123:0x300:0x300:1 \
			MAX-AMSDU-7935:0x800::$max_amsdu \
			DSSS_CCK-40:0x1000::$dsss_cck_40

		ht_capab="$ht_capab$ht_capab_flags"
		ht_capab_debug=`grep "ht_capab" $UCI_DB_PATH`
		[ -n "$ht_capab" ] && [ "$ht_capab_debug" = "" ] && append base_cfg "ht_capab=$ht_capab" "$N"
	}

	enable_vht=0
	he_phy_channel_width_set=1
	if [ "$band" = "2.4GHz" ]; then
		[ -n "$obss_interval" ] && append base_cfg "obss_interval=$obss_interval" "$N"
		[ -n "$ieee80211n" ] && enable_vht=1

		if [ "$htmode" = "auto" ]; then
			htmode="HT40"
		fi

		if [ "$channel" != "14" ]; then
			op_class=81
		else
			op_class=82
		fi
	fi

	[ -n "$ignore_40_mhz_intolerant" ] && append base_cfg "ignore_40_mhz_intolerant=$ignore_40_mhz_intolerant" "$N"

	if [ "$band" = "6GHz" ]; then
		#for 6GHz , ieee8021111ax/HE need to be enabled always
		ieee80211ax=1
		he_phy_channel_width_set=6
		case "$htmode" in
			VHT20|VHT40*)
				he_oper_chwidth=0
				if [ "$htmode" = VHT20 ]; then
					op_class=131
					he_oper_centr_freq_seg0_idx=$channel
				else
					op_class=132
					case "$channel" in
						1|5) he_oper_centr_freq_seg0_idx=3 ;;
						9|13) he_oper_centr_freq_seg0_idx=11 ;;
						17|21) he_oper_centr_freq_seg0_idx=19 ;;
						25|29) he_oper_centr_freq_seg0_idx=27 ;;
						33|37) he_oper_centr_freq_seg0_idx=35 ;;
						41|45) he_oper_centr_freq_seg0_idx=43 ;;
						49|53) he_oper_centr_freq_seg0_idx=51;;
						57|61) he_oper_centr_freq_seg0_idx=59 ;;
						65|69) he_oper_centr_freq_seg0_idx=67 ;;
						73|77) he_oper_centr_freq_seg0_idx=75 ;;
						81|85) he_oper_centr_freq_seg0_idx=83 ;;
						89|93) he_oper_centr_freq_seg0_idx=91 ;;
						97|101) he_oper_centr_freq_seg0_idx=99 ;;
						105|109) he_oper_centr_freq_seg0_idx=107 ;;
						113|117) he_oper_centr_freq_seg0_idx=115 ;;
						121|125) he_oper_centr_freq_seg0_idx=123 ;;
						129|133) he_oper_centr_freq_seg0_idx=131 ;;
						137|141) he_oper_centr_freq_seg0_idx=139 ;;
						145|149) he_oper_centr_freq_seg0_idx=147 ;;
						153|157) he_oper_centr_freq_seg0_idx=155 ;;
						161|165) he_oper_centr_freq_seg0_idx=163 ;;
						169|173) he_oper_centr_freq_seg0_idx=171 ;;
						177|181) he_oper_centr_freq_seg0_idx=179 ;;
						185|189) he_oper_centr_freq_seg0_idx=187 ;;
						193|197) he_oper_centr_freq_seg0_idx=195 ;;
						201|205) he_oper_centr_freq_seg0_idx=203 ;;
						209|213) he_oper_centr_freq_seg0_idx=211 ;;
						217|221) he_oper_centr_freq_seg0_idx=219 ;;
						225|229) he_oper_centr_freq_seg0_idx=227 ;;
					esac

				fi
			;;
			VHT80)
				op_class=133
				he_oper_chwidth=1
				case "$channel" in
					1|5|9|13) he_oper_centr_freq_seg0_idx=7 ;;
					17|21|25|29) he_oper_centr_freq_seg0_idx=23 ;;
					33|37|41|45) he_oper_centr_freq_seg0_idx=39 ;;
					49|53|57|61) he_oper_centr_freq_seg0_idx=55 ;;
					65|69|73|77) he_oper_centr_freq_seg0_idx=71 ;;
					81|85|89|93) he_oper_centr_freq_seg0_idx=87 ;;
					97|101|105|109) he_oper_centr_freq_seg0_idx=103 ;;
					113|117|121|125) he_oper_centr_freq_seg0_idx=119 ;;
					129|133|137|141) he_oper_centr_freq_seg0_idx=135 ;;
					145|149|153|157) he_oper_centr_freq_seg0_idx=151 ;;
					161|165|169|173) he_oper_centr_freq_seg0_idx=167 ;;
					177|181|185|189) he_oper_centr_freq_seg0_idx=183 ;;
					193|197|201|205) he_oper_centr_freq_seg0_idx=199 ;;
					209|213|217|221) he_oper_centr_freq_seg0_idx=215 ;;
				esac
			;;
                        VHT160)
				op_class=134
				he_oper_chwidth=2
				case "$channel" in
					1|5|9|13|17|21|25|29) he_oper_centr_freq_seg1_idx=15 ;;
					33|37|41|45|49|53|57|61) he_oper_centr_freq_seg1_idx=47 ;;
					65|69|73|77|81|85|89|93) he_oper_centr_freq_seg1_idx=79 ;;
					97|101|105|109|113|117|121|125) he_oper_centr_freq_seg1_idx=111 ;;
					129|133|137|141|145|149|153|157) he_oper_centr_freq_seg1_idx=143 ;;
					161|165|169|173|177|181|185|189) he_oper_centr_freq_seg1_idx=175 ;;
					193|197|201|205|209|213|217|221) he_oper_centr_freq_seg1_idx=207 ;;
				esac
				case "$channel" in
					1|5|9|13) he_oper_centr_freq_seg0_idx=7 ;;
					17|21|25|29) he_oper_centr_freq_seg0_idx=23 ;;
					33|37|41|45) he_oper_centr_freq_seg0_idx=39 ;;
					49|53|57|61) he_oper_centr_freq_seg0_idx=55 ;;
					65|69|73|77) he_oper_centr_freq_seg0_idx=71 ;;
					81|85|89|93) he_oper_centr_freq_seg0_idx=87 ;;
					97|101|105|109) he_oper_centr_freq_seg0_idx=103 ;;
					113|117|121|125) he_oper_centr_freq_seg0_idx=119 ;;
					129|133|137|141) he_oper_centr_freq_seg0_idx=135 ;;
					145|149|153|157) he_oper_centr_freq_seg0_idx=151 ;;
					161|165|169|173) he_oper_centr_freq_seg0_idx=167 ;;
					177|181|185|189) he_oper_centr_freq_seg0_idx=183 ;;
					193|197|201|205) he_oper_centr_freq_seg0_idx=199 ;;
					209|213|217|221) he_oper_centr_freq_seg0_idx=215 ;;
				esac

			;;
		esac
	fi

	# 802.11ac
	vht_oper_centr_freq_seg0_idx=0
	if [ "$band" = "5GHz" ]; then
		if [ "$band" = "5GHz" ]; then
			if [ $channel != "acs_smart" ] && [ $channel -gt 161 ]; then
				op_class=125
			else
				op_class=130
			fi
		fi
		if [ "$htmode" = "auto" ]; then
			htmode="VHT160"
			if [ "$doth" -eq 0 ]; then
				htmode="VHT80"
			fi
		fi
		he_phy_channel_width_set=6
		case "$htmode" in
			VHT20|VHT40*)
				vht_oper_chwidth=0
				if [ "$htmode" = "VHT40+" ] || [ "$htmode" = "VHT40" ] || [ "$htmode" = "VHT40-" ]; then
					[ "$htmode" = "VHT40+" ] && vht_oper_centr_freq_seg0_idx=$(($channel+2))
					[ "$htmode" = "VHT40-" ] && vht_oper_centr_freq_seg0_idx=$(($channel-2))
				fi

			;;
			VHT80)
				set_default vht_oper_chwidth 1
				#5GHz
					case "$channel" in
						36|40|44|48) vht_oper_centr_freq_seg0_idx=42 ;;
						52|56|60|64) vht_oper_centr_freq_seg0_idx=58 ;;
						100|104|108|112) vht_oper_centr_freq_seg0_idx=106 ;;
						116|120|124|128) vht_oper_centr_freq_seg0_idx=122 ;;
						132|136|140|144) vht_oper_centr_freq_seg0_idx=138 ;;
						149|153|157|161) vht_oper_centr_freq_seg0_idx=155 ;;
					esac
			;;
			VHT160)
				set_default vht_oper_chwidth 2
				#5GHz
					case "$channel" in
						36|40|44|48|52|56|60|64) vht_oper_centr_freq_seg0_idx=50 ;;
						100|104|108|112|116|120|124|128) vht_oper_centr_freq_seg0_idx=114 ;;
					esac
			;;
		esac
	fi
	set_default vht_oper_chwidth 0

	append base_cfg "op_class=$op_class" "$N"
	[ "$ieee80211ax" = "1" ] &&  append base_cfg "he_phy_channel_width_set=$he_phy_channel_width_set" "$N"

	if [ "$band" = "6GHz" ]; then
		[ -n "$he_oper_chwidth" ] && append base_cfg "he_oper_chwidth=$he_oper_chwidth" "$N"
		[ -n "$he_oper_centr_freq_seg0_idx" ] && append base_cfg "he_oper_centr_freq_seg0_idx=$he_oper_centr_freq_seg0_idx" "$N"
		[ -n "$he_oper_centr_freq_seg1_idx" ] && append base_cfg "he_oper_centr_freq_seg1_idx=$he_oper_centr_freq_seg1_idx" "$N"
	fi

	if [ -n "$ieee80211ac" ]; then
		if [ "$auto_channel" -gt 0 ]; then
			vht_oper_centr_freq_seg0_idx=0
			vht_oper_centr_freq_seg1_idx=0
		fi
		# for 5GHz
		if [ "$ieee80211ax" = "1" ]; then
			append base_cfg "he_oper_chwidth=$vht_oper_chwidth" "$N"
			append base_cfg "he_oper_centr_freq_seg0_idx=$vht_oper_centr_freq_seg0_idx" "$N"
		else
			append base_cfg "vht_oper_chwidth=$vht_oper_chwidth" "$N"
			append base_cfg "vht_oper_centr_freq_seg0_idx=$vht_oper_centr_freq_seg0_idx" "$N"
		fi

		append base_cfg "ieee80211ac=1" "$N"
		append base_cfg "opmode_notif=1" "$N"
		enable_vht=1
	fi

	if [ "$enable_vht" != "0" ]; then
		json_get_vars \
			rxldpc:1 \
			short_gi_80:1 \
			short_gi_160:1 \
			tx_stbc_2by1:1 \
			su_beamformer:1 \
			su_beamformee:1 \
			mu_beamformer:1 \
			mu_beamformee:1 \
			vht_txop_ps:1 \
			htc_vht:1 \
			rx_antenna_pattern:1 \
			tx_antenna_pattern:1 \
			vht_max_a_mpdu_len_exp:7 \
			vht_max_mpdu:11454 \
			rx_stbc:4 \
			vht_link_adapt:3 \
			vht160:2

		vht_cap=0
		for cap in $(iw phy "$phy" info | awk -F "[()]" '/VHT Capabilities/ { print $2 }'); do
			vht_cap="$(($vht_cap | $cap))"
		done

		cap_rx_stbc=$((($vht_cap >> 8) & 7))
		[ "$rx_stbc" -lt "$cap_rx_stbc" ] && cap_rx_stbc="$rx_stbc"
		vht_cap="$(( ($vht_cap & ~(0x700)) | ($cap_rx_stbc << 8) ))"


		mac80211_add_capabilities vht_capab $vht_cap \
			RXLDPC:0x10::$rxldpc \
			SHORT-GI-80:0x20::$short_gi_80 \
			SHORT-GI-160:0x40::$short_gi_160 \
			TX-STBC-2BY1:0x80::$((tx_stbc & tx_stbc_2by1)) \
			SU-BEAMFORMER:0x800::$su_beamformer \
			SU-BEAMFORMEE:0x1000::$su_beamformee \
			MU-BEAMFORMER:0x80000::$mu_beamformer \
			MU-BEAMFORMEE:0x100000::$mu_beamformee \
			VHT-TXOP-PS:0x200000::$vht_txop_ps \
			HTC-VHT:0x400000::$htc_vht \
			RX-ANTENNA-PATTERN:0x10000000::$rx_antenna_pattern \
			TX-ANTENNA-PATTERN:0x20000000::$tx_antenna_pattern \
			RX-STBC-1:0x700:0x100:1 \
			RX-STBC-12:0x700:0x200:1 \
			RX-STBC-123:0x700:0x300:1 \
			RX-STBC-1234:0x700:0x400:1 \

		# supported Channel widths
		vht160_hw=0
		[ "$(($vht_cap & 12))" -eq 4 -a 1 -le "$vht160" ] && \
			vht160_hw=1
		[ "$(($vht_cap & 12))" -eq 8 -a 2 -le "$vht160" ] && \
			vht160_hw=2
		[ "$vht160_hw" = 1 ] && vht_capab="$vht_capab[VHT160]"
		[ "$vht160_hw" = 2 ] && vht_capab="$vht_capab[VHT160-80PLUS80]"

		# maximum MPDU length
		vht_max_mpdu_hw=3895
		[ "$(($vht_cap & 3))" -ge 1 -a 7991 -le "$vht_max_mpdu" ] && \
			vht_max_mpdu_hw=7991
		[ "$(($vht_cap & 3))" -ge 2 -a 11454 -le "$vht_max_mpdu" ] && \
			vht_max_mpdu_hw=11454
		[ "$vht_max_mpdu_hw" != 3895 ] && \
			vht_capab="$vht_capab[MAX-MPDU-$vht_max_mpdu_hw]"

		# maximum A-MPDU length exponent
		vht_max_a_mpdu_len_exp_hw=0
		[ "$(($vht_cap & 58720256))" -ge 8388608 -a 1 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=1
		[ "$(($vht_cap & 58720256))" -ge 16777216 -a 2 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=2
		[ "$(($vht_cap & 58720256))" -ge 25165824 -a 3 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=3
		[ "$(($vht_cap & 58720256))" -ge 33554432 -a 4 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=4
		[ "$(($vht_cap & 58720256))" -ge 41943040 -a 5 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=5
		[ "$(($vht_cap & 58720256))" -ge 50331648 -a 6 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=6
		[ "$(($vht_cap & 58720256))" -ge 58720256 -a 7 -le "$vht_max_a_mpdu_len_exp" ] && \
			vht_max_a_mpdu_len_exp_hw=7
		vht_capab="$vht_capab[MAX-A-MPDU-LEN-EXP$vht_max_a_mpdu_len_exp_hw]"

		# whether or not the STA supports link adaptation using VHT variant
		vht_link_adapt_hw=0
		[ "$(($vht_cap & 201326592))" -ge 134217728 -a 2 -le "$vht_link_adapt" ] && \
			vht_link_adapt_hw=2
		[ "$(($vht_cap & 201326592))" -ge 201326592 -a 3 -le "$vht_link_adapt" ] && \
			vht_link_adapt_hw=3
		[ "$vht_link_adapt_hw" != 0 ] && \
			vht_capab="$vht_capab[VHT-LINK-ADAPT-$vht_link_adapt_hw]"

		vht_capab="$vht_capab[BF-ANTENNA-4]"

		num_antennas_in_hex=`iw phy "$phy" info | grep Configured | awk '{print $4}' | tr -d '0' | tr -d 'x'`
		case "$num_antennas_in_hex" in
			[f]) sounding_dimension=4 ;;
			[7bde]) sounding_dimension=3 ;;
			[3569ac]) sounding_dimension=2 ;;
			[1248]) sounding_dimension=1 ;;
		esac
		vht_capab="$vht_capab[SOUNDING-DIMENSION-$sounding_dimension]"
		vht_capab_debug=`grep "vht_capab" $UCI_DB_PATH`
		[ -n "$vht_capab" ]  && [ "$vht_capab_debug" = "" ] && append base_cfg "vht_capab=$vht_capab" "$N"
	fi

	json_get_vars sFixedLtfGi
	[ -n "$sFixedLtfGi" ] && append base_cfg "sFixedLtfGi=$sFixedLtfGi" "$N"

	mac80211_append_ax_parameters

	set_default acs_smart_info_file "/var/run/acs_smart_info_wlan${phy#phy}.txt"
	set_default acs_history_file "/var/run/acs_history_wlan${phy#phy}.txt"
	append base_cfg "acs_num_scans=1" "$N"
	append base_cfg "acs_smart_info_file=$acs_smart_info_file" "$N"
	append base_cfg "acs_history_file=$acs_history_file" "$N"

	json_get_vars \
		acs_penalty_factors acs_to_degradation acs_chan_cust_penalty \
		acs_grp_priorities_throughput acs_grp_priorities_reach \
		acs_policy acs_switch_thresh acs_bw_comparison

	[ -n "$acs_penalty_factors" ] && append base_cfg "acs_penalty_factors=$acs_penalty_factors" "$N"
	[ -n "$acs_to_degradation" ] && append base_cfg "acs_to_degradation=$acs_to_degradation" "$N"
	[ -n "$acs_chan_cust_penalty" ] && append base_cfg "acs_chan_cust_penalty=$acs_chan_cust_penalty" "$N"
	[ -n "$acs_grp_priorities_throughput" ] && append base_cfg "acs_grp_priorities_throughput=$acs_grp_priorities_throughput" "$N"
	[ -n "$acs_grp_priorities_reach" ] && append base_cfg "acs_grp_priorities_reach=$acs_grp_priorities_reach" "$N"
	[ -n "$acs_policy" ] && append base_cfg "acs_policy=$acs_policy" "$N"
	[ -n "$acs_switch_thresh" ] && append base_cfg "acs_switch_thresh=$acs_switch_thresh" "$N"
	[ -n "$acs_bw_comparison" ] && append base_cfg "acs_bw_comparison=$acs_bw_comparison" "$N"

	[ -n "$obss_beacon_rssi_threshold" ] && append base_cfg "obss_beacon_rssi_threshold=$obss_beacon_rssi_threshold" "$N"

	json_get_vars ap_retry_limit ap_retry_limit_data
	[ -n "$ap_retry_limit" ] && append base_cfg "ap_retry_limit=$ap_retry_limit" "$N"
	[ -n "$ap_retry_limit_data" ] && append base_cfg "ap_retry_limit_data=$ap_retry_limit_data" "$N"

	json_get_vars mu_mimo_operation
	[ -n "$mu_mimo_operation" ] && append base_cfg "mu_mimo_operation=$mu_mimo_operation" "$N"

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		debug_infrastructure_json_get_vars debug_hostap_conf_
		debug_infrastructure_append debug_hostap_conf_ base_cfg
	fi

	hostapd_prepare_device_config "$hostapd_conf_file" nl80211
	cat >> "$hostapd_conf_file" <<EOF
${channel:+channel=$channel}
${channel_list:+chanlist=$channel_list}
${acs_fallback_chan_list:+acs_fallback_chan=$acs_fallback_chan_list}
$base_cfg

EOF
	mac80211_hostapd_prepare_atf_config "$hostapd_atf_conf_file"
	json_select ..
}

mac80211_hostapd_setup_atf_bss() {
	local config_file="$1"
	local ifname="$2"
	local atf_cfg=
	local sta_mac
	local sta_precent

	has_content=0
	json_get_vars atf_vap_grant
	json_get_values atf_sta_grants atf_sta_grants

	append atf_cfg "[$ifname]" "$N"
	for atf_sta_grant in $atf_sta_grants; do
		sta_mac=`echo ${atf_sta_grant} | cut -d"," -f1`
		sta_precent=`echo ${atf_sta_grant} | cut -d"," -f2`
		sta_precent=`expr $sta_precent \* 100` # Driver expects it to be multiplied by 100
		append atf_cfg "sta=$sta_mac,$sta_precent" "$N"
		has_content=1
	done
	[ -n "$atf_vap_grant" ] && {
		atf_vap_grant=`expr $atf_vap_grant \* 100` # Driver expects it to be multiplied by 100
		append atf_cfg "vap_grant=$atf_vap_grant" "$N"
		has_content=1
	}

	if [ $has_content -eq 0 ]; then # Prevent having an empty section
		return
	fi

	cat >> "$config_file" <<EOF
$atf_cfg

EOF
}

mac80211_hostapd_setup_bss() {
	local phy="$1"
	local ifname="$2"
	local macaddr="$3"
	local type="$4"

	hostapd_cfg=
	append hostapd_cfg "$type=$ifname" "$N"

	hostapd_set_bss_options hostapd_cfg "$vif" || return 1
	json_get_vars wds dtim_period max_listen_int start_disabled start_after

	set_default wds 0
	set_default start_disabled 0

	[ "$wds" -gt 0 ] && append hostapd_cfg "wds_sta=1" "$N"

	[ "$type" = "bss" ] && [ "$staidx" -gt 0 ] && start_disabled=1 # Repeater mode & not Master VAP

	# delaying 2.4G Radio Start until after 5G Starts to Improve Same SSID situations Feature
	if [ -n "$start_after" ]; then
		local delayed_start_file="/tmp/${ifname}_start_after_${start_after}"
		if [ ! -e "${delayed_start_file}" ]; then
			start_disabled=1
		else
			local delayed_start_status=`cat ${delayed_start_file}`
			if [ "$delayed_start_status" -eq 0 ]; then
				start_disabled=1
				delayed_start_might_need_restart=1
			fi
		fi
	fi

	[ "$start_disabled" -eq 1 ] && append hostapd_cfg "start_disabled=1" "$N"

	cat >> /var/run/hostapd-$phy.conf <<EOF
$hostapd_cfg
bssid=$macaddr
${dtim_period:+dtim_period=$dtim_period}
${max_listen_int:+max_listen_interval=$max_listen_int}
EOF

	mac80211_hostapd_setup_atf_bss "$hostapd_atf_conf_file" "$ifname"
}

mac80211_get_addr() {
	local phy="$1"
	local idx="$(($2 + 1))"

	head -n $(($macidx + 1)) /sys/class/ieee80211/${phy}/addresses | tail -n1
}

mac80211_generate_mac() {
	local phy="$1"
	local id="${macidx:-0}"

	local ref="$(cat /sys/class/ieee80211/${phy}/macaddress)"
	local mask="$(cat /sys/class/ieee80211/${phy}/address_mask)"

	[ "$mask" = "00:00:00:00:00:00" ] && {
		mask="ff:ff:ff:ff:ff:ff";

		[ "$(wc -l < /sys/class/ieee80211/${phy}/addresses)" -gt 1 ] && {
			addr="$(mac80211_get_addr "$phy" "$id")"
			[ -n "$addr" ] && {
				echo "$addr"
				return
			}
		}
	}

	local oIFS="$IFS"; IFS=":"; set -- $mask; IFS="$oIFS"

	local mask1=$1
	local mask6=$6

	local oIFS="$IFS"; IFS=":"; set -- $ref; IFS="$oIFS"

	macidx=$(($id + 1))
	[ "$((0x$mask1))" -gt 0 ] && {
		b1="0x$1"
		[ "$id" -gt 0 ] && \
			b1=$(($b1 ^ ((($id - 1) << 2) | 0x2)))
		printf "%02x:%s:%s:%s:%s:%s" $b1 $2 $3 $4 $5 $6
		return
	}

	[ "$((0x$mask6))" -lt 255 ] && {
		printf "%s:%s:%s:%s:%s:%02x" $1 $2 $3 $4 $5 $(( 0x$6 ^ $id ))
		return
	}

	off2=$(( (0x$6 + $id) / 0x100 ))
	printf "%s:%s:%s:%s:%02x:%02x" \
		$1 $2 $3 $4 \
		$(( (0x$5 + $off2) % 0x100 )) \
		$(( (0x$6 + $id) % 0x100 ))
}

find_phy() {
	[ -n "$phy" -a -d /sys/class/ieee80211/$phy ] && return 0
	[ -n "$path" ] && {
		for phy in $(ls /sys/class/ieee80211 2>/dev/null); do
			case "$(readlink -f /sys/class/ieee80211/$phy/device)" in
				*$path) return 0;;
			esac
		done
	}
	[ -n "$macaddr" ] && {
		for phy in $(ls /sys/class/ieee80211 2>/dev/null); do
			grep -i -q "$macaddr" "/sys/class/ieee80211/${phy}/macaddress" && return 0
		done
	}
	return 1
}

mac80211_check_ap() {
	has_ap=1
}

mac80211_check_sta() {
	has_sta=1
}

mac80211_prepare_vif() {
	json_select config

	json_get_vars ifname mode ssid wds powersave macaddr

	[ -n "$ifname" ] || ifname="wlan${phy#phy}${if_idx:+-$if_idx}"
	if_idx=$((${if_idx:-0} + 1))

	set_default wds 0
	set_default powersave 0

	json_select ..

	[ -n "$macaddr" ] || {
		macaddr="$(mac80211_generate_mac $phy)"
		macidx="$(($macidx + 1))"
	}

	json_add_object data
	json_add_string ifname "$ifname"
	json_close_object
	json_select config

	# It is far easier to delete and create the desired interface
	case "$mode" in
		adhoc)
			iw phy "$phy" interface add "$ifname" type adhoc
		;;
		ap)
			# Hostapd will handle recreating the interface and
			# subsequent virtual APs belonging to the same PHY
			if [ -n "$hostapd_ctrl" ]; then
				type=bss
			else
				type=interface
			fi

			mac80211_hostapd_setup_bss "$phy" "$ifname" "$macaddr" "$type" || return

			[ -n "$hostapd_ctrl" ] || {
				iw phy "$phy" interface add "$ifname" type __ap
				hostapd_ctrl="${hostapd_ctrl:-/var/run/hostapd/$ifname}"
			}
		;;
		mesh)
			iw phy "$phy" interface add "$ifname" type mp
		;;
		monitor)
			iw phy "$phy" interface add "$ifname" type monitor
		;;
		sta)
			local wdsflag=
			staidx="$(($staidx + 1))"
			[ "$wds" -gt 0 ] && wdsflag="4addr on"
			[ -n "$hostapd_ctrl" ] || {
				local iface_num=${ifname#"wlan"}
				local master_num=$((iface_num-1))
				local master_iface="wlan$master_num"
				iw phy "$phy" interface add "$master_iface" type __ap
			}

			iw phy "$phy" interface add "$ifname" type managed $wdsflag
			[ "$powersave" -gt 0 ] && powersave="on" || powersave="off"
			iw "$ifname" set power_save "$powersave"
		;;
	esac

	case "$mode" in
		monitor|mesh)
			[ "$auto_channel" -gt 0 ] || iw dev "$ifname" set channel "$channel" $htmode
		;;
	esac

	if [ "$mode" != "ap" ]; then
		# ALL ap functionality will be passed to hostapd
		# All interfaces must have unique mac addresses
		# which can either be explicitly set in the device
		# section, or automatically generated
		ip link set dev "$ifname" address "$macaddr"
	fi

	json_select ..
}

mac80211_setup_supplicant() {
	wpa_supplicant_prepare_interface "$ifname" nl80211 || return 1
	wpa_supplicant_add_network "$ifname"
	[ "$full_ch_master_control" -gt 0 ] && full_ch_master="-F"
	wpa_supplicant_run "$ifname" ${hostapd_ctrl:+-H $hostapd_ctrl $full_ch_master}
}

mac80211_setup_adhoc_htmode() {
	case "$htmode" in
		VHT20|HT20) ibss_htmode=HT20;;
		HT40*|VHT40|VHT160)
			case "$hwmode" in
				a)
					case "$(( ($channel / 4) % 2 ))" in
						1) ibss_htmode="HT40+" ;;
						0) ibss_htmode="HT40-";;
					esac
				;;
				*)
					case "$htmode" in
						HT40+) ibss_htmode="HT40+";;
						HT40-) ibss_htmode="HT40-";;
						*)
							if [ "$channel" -lt 7 ]; then
								ibss_htmode="HT40+"
							else
								ibss_htmode="HT40-"
							fi
						;;
					esac
				;;
			esac
			[ "$auto_channel" -gt 0 ] && ibss_htmode="HT40+"
		;;
		VHT80)
			ibss_htmode="80MHZ"
		;;
		NONE|NOHT)
			ibss_htmode="NOHT"
		;;
		*) ibss_htmode="" ;;
	esac

}

mac80211_setup_adhoc() {
	json_get_vars bssid ssid key mcast_rate

	keyspec=
	[ "$auth_type" = "wep" ] && {
		set_default key 1
		case "$key" in
			[1234])
				local idx
				for idx in 1 2 3 4; do
					json_get_var ikey "key$idx"

					[ -n "$ikey" ] && {
						ikey="$(($idx - 1)):$(prepare_key_wep "$ikey")"
						[ $idx -eq $key ] && ikey="d:$ikey"
						append keyspec "$ikey"
					}
				done
			;;
			*)
				append keyspec "d:0:$(prepare_key_wep "$key")"
			;;
		esac
	}

	brstr=
	for br in $basic_rate_list; do
		wpa_supplicant_add_rate brstr "$br"
	done

	mcval=
	[ -n "$mcast_rate" ] && wpa_supplicant_add_rate mcval "$mcast_rate"

	iw dev "$ifname" ibss join "$ssid" $freq $ibss_htmode fixed-freq $bssid \
		beacon-interval $beacon_int \
		${brstr:+basic-rates $brstr} \
		${mcval:+mcast-rate $mcval} \
		${keyspec:+keys $keyspec}
}

mac80211_setup_vif() {
	local name="$1"
	local failed

	json_select data
	json_get_vars ifname
	json_select ..

	json_select config
	json_get_vars mode

	echo $ifname | egrep -q "^wlan[0-9]{1,2}\.[0-9]{1,2}$"
	if [ $? == "0" ]; then
		# ap interface is slave VAP, and is controlled by hostapd.
		json_select ..
		return
	fi

	ip link set dev "$ifname" up || {
		wireless_setup_vif_failed IFUP_ERROR
		json_select ..
		return
	}

	case "$mode" in
		mesh)
			# authsae or wpa_supplicant
			json_get_vars key
			if [ -n "$key" ]; then
				if [ -e "/lib/wifi/authsae.sh" ]; then
					. /lib/wifi/authsae.sh
					authsae_start_interface || failed=1
				else
					wireless_vif_parse_encryption
					mac80211_setup_supplicant || failed=1
				fi
			else
				json_get_vars mesh_id mcast_rate

				mcval=
				[ -n "$mcast_rate" ] && wpa_supplicant_add_rate mcval "$mcast_rate"

				case "$htmode" in
					VHT20|HT20) mesh_htmode=HT20;;
					HT40*|VHT40)
						case "$hwmode" in
							a)
								case "$(( ($channel / 4) % 2 ))" in
									1) mesh_htmode="HT40+" ;;
									0) mesh_htmode="HT40-";;
								esac
							;;
							*)
								case "$htmode" in
									HT40+) mesh_htmode="HT40+";;
									HT40-) mesh_htmode="HT40-";;
									*)
										if [ "$channel" -lt 7 ]; then
											mesh_htmode="HT40+"
										else
											mesh_htmode="HT40-"
										fi
									;;
								esac
							;;
						esac
					;;
					VHT80)
						mesh_htmode="80Mhz"
					;;
					VHT160)
						mesh_htmode="160Mhz"
					;;
					*) mesh_htmode="NOHT" ;;
				esac

				freq="$(get_freq "$phy" "$channel")"
				iw dev "$ifname" mesh join "$mesh_id" freq $freq $mesh_htmode \
					${mcval:+mcast-rate $mcval} \
					beacon-interval $beacon_int
			fi

			for var in $MP_CONFIG_INT $MP_CONFIG_BOOL $MP_CONFIG_STRING; do
				json_get_var mp_val "$var"
				[ -n "$mp_val" ] && iw dev "$ifname" set mesh_param "$var" "$mp_val"
			done
		;;
		adhoc)
			wireless_vif_parse_encryption
			mac80211_setup_adhoc_htmode
			if [ "$wpa" -gt 0 -o "$auto_channel" -gt 0 ]; then
				mac80211_setup_supplicant || failed=1
			else
				mac80211_setup_adhoc
			fi
		;;
		sta)
			mac80211_setup_supplicant || failed=1
		;;
	esac

	json_select ..
	[ -n "$failed" ] || wireless_add_vif "$name" "$ifname"
}

get_freq() {
	local phy="$1"
	local chan="$2"
	iw "$phy" info | grep -E -m1 "(\* ${chan:-....} MHz${chan:+|\\[$chan\\]})" | grep MHz | awk '{print $2}'
}



get_main_iface_backup_way() {
	local phy="$1"
	local wdev
	for wdev in $(list_phy_interfaces "$phy"); do
		local wdev_phy=$(iw $wdev info | grep wiphy | tr -dc '0-9')
		local phy_idx="${phy:3:1}"
		if [ "$wdev_phy" -ne "$phy_idx" ]; then
			continue;
		fi
		local is_main=$(echo "$wdev" | tr -dc '0-9' | wc -c)
		if [ "$is_main" = "1" ]; then
			local is_ap=`iw $wdev info | grep "type AP"`
			if [ -n "$is_ap" ]; then
				echo "$wdev"
				return
			fi
		fi
	done
}

get_station_iface() {
	local phy="$1"
	local wdev
	for wdev in $(list_phy_interfaces "$phy"); do
		local wdev_phy=$(iw $wdev info | grep wiphy | tr -dc '0-9')
		local phy_idx="${phy:3:1}"
		if [ "$wdev_phy" -ne "$phy_idx" ]; then
			continue;
		fi
		local is_sta=`iw $wdev info | grep "type managed"`
		if [ -n "$is_sta" ]; then
			echo "$wdev"
			return
		fi
	done
}

get_main_iface() {
	local phy="$1"
	local wdev=`cat /proc/net/mtlk/$phy/master_vap_name 2>/dev/null`
	if [ -z "$wdev" ]; then
		wdev=`get_main_iface_backup_way "$phy"`
	fi
	echo "$wdev"
}

mac80211_interface_cleanup() {
	local phy="$1"
	local main_iface pid
	local shared_hapd="$2"

	main_iface=`get_main_iface $phy`

	if [ "$main_iface" = "unregistered" ]; then
		logger -s "no registered iface for $phy yet, no need for cleanup"
		return
	fi

	if [ -n "$shared_hapd" ]; then
		use_flock ${shared_hapd} 223
		pid=`_is_shared_hapd_already_running "$shared_hapd"`
		ret="$?"

		if [ "$ret" -eq 0 -a -n "$pid" ]; then
			local iface_exist=`/usr/sbin/hostapd_cli -iglobal-${shared_hapd} raw INTERFACES | grep $main_iface`
			if [ -n "$iface_exist" ]; then
				logger -s "$main_iface is still running in mac80211_interface_cleanup, removing it"
				do_detach
			fi
		fi
		flock -u 223
	else
		pid=`cat /var/run/wifi-$phy.pid 2>/dev/null`
		local proc_name=`cat /proc/${pid}/comm`
		if [ "$proc_name" = "hostapd" ]; then
			logger -s "$main_iface is still running in mac80211_interface_cleanup, killing it"
			kill $pid
		fi
	fi
	local station_iface=`get_station_iface $phy`
	if [ -n "$station_iface" ]; then
		[ -f "/var/run/wpa_supplicant-${station_iface}.pid" ] && [ kill `cat "/var/run/wpa_supplicant-${station_iface}.pid"` >/dev/null 2>&1 ]
		iw dev "$station_iface" del >/dev/null 2>&1
	fi
}

drv_mac80211_cleanup() {
	hostapd_common_cleanup
}

drv_mac80211_setup_phy() {
	local phys=$(ls /sys/class/ieee80211/)
	local path_5g=`uci show | grep "5GHz" | awk -F"." '{print $1 "." $2}' | awk -v RS=  '{$1=$1}1'`
	local path_6g=`uci show | grep "6GHz" | awk -F"." '{print $1 "." $2}' | awk -v RS=  '{$1=$1}1'`
	local path_24g=`uci show | grep "2.4GHz" | awk -F"." '{print $1 "." $2}' | awk -v RS=  '{$1=$1}1'`
	local idx_5g=1
	local idx_6g=1
	local idx_24g=1

	for phy in $phys
	do
		`iw $phy info | grep "* 58.. MHz" > /dev/null`
		is_phy_5g=$?
		`iw $phy info | grep "* 60.. MHz" > /dev/null`
		is_phy_6g=$?

		if [ $is_phy_5g = '0' ]; then
			local path=`echo "$path_5g" | awk -v idx_5g="$idx_5g" '{print $idx_5g}'`
			idx_5g=$((idx_5g+1))
		elif [ $is_phy_6g = '0' ]; then
			local path=`echo "$path_6g" | awk -v idx_6g="$idx_6g" '{print $idx_6g}'`
			idx_6g=$((idx_6g+1))
		else
			local path=`echo "$path_24g" | awk -v idx_24g="$idx_24g" '{print $idx_24g}'`
			idx_24g=$((idx_24g+1))
		fi

		uci set $path.phy=$phy
	done

	uci commit wireless
}

check_repeater_mode() {
	auto_channel=0
	channel=first_non_dfs
	htmode="auto"

	ignore_40_mhz_intolerant=1
	obss_interval=0
}

_is_shared_hapd_already_running() {
	local shared_hapd="$1"
	local shared_pid=`cat /var/run/wifi-global-${shared_hapd}.pid 2>/dev/null`

	[ -n "$shared_pid" ] || return 0

	local proc_name=`cat /proc/${shared_pid}/comm 2>/dev/null`
	[ "$proc_name" = "hostapd" ] || return 0

	echo "$shared_pid"
}

start_delayed_iface_start_script() {
	echo "0" > "${delayed_start_file}"
	/lib/netifd/iface_start_after_band.sh "$1" "$2" &
	local delayed_script_pid=$!
	local delayed_script_name="$(readlink -f /proc/${delayed_script_pid}/exe)"
	wireless_add_process "${delayed_script_pid}" "${delayed_script_name}"
}

start_delayed_start_script_if_needed() {
	json_select config

	json_get_vars ifname start_after

	[ -n "$ifname" ] || ifname="wlan${phy#phy}${if_idx:+-$if_idx}"
	if_idx=$((${if_idx:-0} + 1))

	json_select ..

	if [ -n "$start_after" ]; then
		local delayed_start_file="/tmp/${ifname}_start_after_${start_after}"
		if [ ! -e "${delayed_start_file}" ]; then
			touch "${delayed_start_file}"
			start_delayed_iface_start_script "$ifname" "$start_after"
		elif [ "${delayed_start_might_need_restart}" -eq 1 ]; then
			local delayed_start_status=`cat ${delayed_start_file}`
			if [ "$is_setup" -eq 1 -o "$delayed_start_status" -eq 1 ]; then
				start_delayed_iface_start_script "$ifname" "$start_after"
			fi
		fi
	fi
}

use_flock() {
	local lock_file="/tmp/lock_file_$1"
	local file_num=$2
	local lock_attempts_left=30
	which flock > /dev/null 2>&1
	local which_ret=$?
	if [ $which_ret -eq 0 ]; then
		eval "exec $file_num>$lock_file"
		flock -n $file_num
		local flock_ret=$?

		while [[ $flock_ret -ne 0 && $lock_attempts_left -gt 0 ]]; do
			sleep 1
			flock -n $file_num
			flock_ret=$?
			lock_attempts_left=$((lock_attempts_left-1))
		done

		if [ $lock_attempts_left -le 0 ]; then
			echo "flock $lock_file failed"
			exit 1
		fi
	else
		/usr/bin/logger -t HOSTAPD_CONF -p 3 "flock isn't found..."
		/usr/bin/logger -t HOSTAPD_CONF -p 3 "...Big probability of race condition"
	fi
}

ubus_cmd() {
	local cmd="$1"
	local dev="$2"

	json_init
	[ -n "$dev" ] && json_add_string device "$dev"
	ubus call network.wireless "$cmd" "$(json_dump)"
}

setup_reconf() {
	json_select config
	json_get_vars \
		phy macaddr path \
		country chanbw distance \
		txpower \
		rxantenna txantenna \
		frag rts beacon_int:100 htmode atf_config_file \
		obss_interval ignore_40_mhz_intolerant he_beacon \
		shared_hapd multibss_enable disable_reconf band

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		json_get_vars hostapd_log_level
		debug_infrastructure_json_get_vars debug_iw_pre_up_
		debug_infrastructure_json_get_vars debug_iw_post_up_
	fi

	json_get_values basic_rate_list basic_rate
	json_select ..

	local action="$1"; shift

	find_phy || {
		echo "Could not find PHY for device '$1'"
		wireless_set_retry 0
		return 1
	}

	local wdev=`get_main_iface $phy`

	set_default shared_hapd "$DEFAULT_SHARED_HAPD"

	[ "$multibss_enable" = "1" ] && set_default disable_reconf 1

	[ "$band" = "6GHz" ] && set_default disable_reconf 1

	if [ "$disable_reconf" = "1" -a "$action" = "reconf" ]; then
		# restart radio since multibss does not support reconf
		has_sta=
		for_each_interface "sta" mac80211_check_sta
		if [ -z "$shared_hapd" -o -n "$has_sta" ]; then
			# must be done through netifd in case we are a repeater
			ubus_cmd down $1
			ubus_cmd up $1
			return
		else
			use_flock $shared_hapd 223
			/usr/sbin/hostapd_cli -i global-${shared_hapd} raw REMOVE ${wdev}
			action=setup
			flock -u 223
		fi
	fi

	[ "$action" == "setup" ] && is_setup=1

	wireless_set_data phy="$phy"

	if [ "$action" = "setup" ]; then
		mac80211_interface_cleanup "$phy" "$shared_hapd"
	fi

	# convert channel to frequency
	[ "$auto_channel" -gt 0 ] || freq="$(get_freq "$phy" "$channel")"

	[ -n "$country" ] && {
		iw reg get | grep -q "^country $country:" || {
			iw reg set "$country"
			sleep 1
		}
	}

	use_flock $phy 222

	set_default whm_config_file "/var/run/whm.conf"
	append base_cfg "whm_config_file=$whm_config_file" "$N"
	set_default atf_config_file "/var/run/hostapd-$phy-atf.conf"
	hostapd_conf_file="/var/run/hostapd-$phy.conf"
	hostapd_atf_conf_file="$atf_config_file"

	no_ap=1
	macidx=0
	staidx=0

	[ -n "$chanbw" ] && {
		for file in /sys/kernel/debug/ieee80211/$phy/ath9k/chanbw /sys/kernel/debug/ieee80211/$phy/ath5k/bwmode; do
			[ -f "$file" ] && echo "$chanbw" > "$file"
		done
	}

	set_default rxantenna all
	set_default txantenna all
	set_default distance 0

	iw phy "$phy" set antenna $txantenna $rxantenna >/dev/null 2>&1
	iw phy "$phy" set distance "$distance"

	[ -n "$frag" ] && iw phy "$phy" set frag "${frag%%.*}"
	[ -n "$rts" ] && iw phy "$phy" set rts "${rts%%.*}"

	has_ap=
	hostapd_ctrl=
	for_each_interface "ap" mac80211_check_ap

	rm -f "$hostapd_conf_file"

	for_each_interface "sta" check_repeater_mode

	[ -n "$has_ap" ] && mac80211_hostapd_setup_base "$phy"

	if [ "$action" = "setup" ]; then
		for_each_interface "sta adhoc mesh monitor" mac80211_prepare_vif
	fi

	for_each_interface "ap" mac80211_prepare_vif

	if [ "$wdev" = "unregistered" ]; then
		wdev=`get_main_iface $phy`
	fi

	local whm_ifdown_id=102 #holds ifdown warning id.

	case "$action" in
		reconf)
			local reconf_vaps_list="$2"
			local reconf_radio="$wdev"

			old_ifs=$IFS
			IFS=' '
			for reconf_vap in $reconf_vaps_list; do
				[ -n "$hostapd_ctrl" ] && {
					if [ "$reconf_vap" = "$reconf_radio" ]; then
						/usr/sbin/hostapd_cli -i${reconf_radio} reconf
					else
						/usr/sbin/hostapd_cli -i${reconf_radio} reconf ${reconf_vap}
					fi

					ret="$?"
					[ "$ret" != 0 ] && {
						echo "[MAC]: reconf failed"
						trigger_whm_warn ${reconf_radio} $whm_ifdown_id "HOSTAPD_RECONF_FAILED"
						wireless_setup_failed HOSTAPD_RECONF_FAILED
						return
					}
				}
			done
			IFS=$old_ifs
		;;
		setup)
			[ -n "$hostapd_ctrl" ] && {
				if [ -f /lib/netifd/debug_infrastructure.sh ]; then
					debug_infrastructure_execute_iw_command debug_iw_pre_up_ $wdev
				fi

				local start_new=1
				local global_ctrl_iface=""
				local pid_file_name="wifi-$phy.pid"
				local is_shared=0
				if [ -n "$shared_hapd" ]; then
					global_ctrl_iface=" -g /var/run/hostapd/global-$shared_hapd"
					pid_file_name="wifi-global-$shared_hapd.pid"
					is_shared=1
					use_flock $shared_hapd 223

					hostapd_pid=`_is_shared_hapd_already_running "$shared_hapd"`
					ret="$?"

					if [ "$ret" -eq 0 -a -n "$hostapd_pid" ]; then
						/usr/sbin/hostapd_cli -i global-${shared_hapd} raw ADD ${wdev} config=${hostapd_conf_file}
						ret="$?"
						if [ "$ret" -eq 0 ]; then
							/usr/sbin/hostapd_cli -i${wdev} enable
							ret="$?"
							if [ "$ret" -ne 0 ]; then
								trigger_whm_warn ${wdev} $whm_ifdown_id "SHARED_HOSTAPD_JOIN_FAILED - if enable failed"
								/usr/sbin/hostapd_cli -i global-${shared_hapd} raw REMOVE ${wdev}
								[ "$?" -ne 0 ] && {
									kill "$hostapd_pid"
								}
							fi
						else
							trigger_whm_warn ${wdev} $whm_ifdown_id "SHARED_HOSTAPD_JOIN_FAILED - if add failed"
						fi
					fi

					[ "$ret" != 0 ] && {
						echo "[MAC]: setup failed"
						wireless_setup_failed SHARED_HOSTAPD_JOIN_FAILED
						return
					}

					[ -n "$hostapd_pid" ] && start_new=0
				fi

				if [ "$start_new" -eq 1 ]; then
					/usr/sbin/hostapd -s$hostapd_log_level $global_ctrl_iface -P /var/run/$pid_file_name -B $hostapd_conf_file
					ret="$?"

					[ "$ret" != 0 ] && {
						trigger_whm_warn ${wdev} $whm_ifdown_id "HOSTAPD_START_FAILED"
						echo "[MAC]: setup failed"
						wireless_setup_failed HOSTAPD_START_FAILED
						return
					}

					retry_count=0
					hostapd_pid=
					until [ $retry_count -ge 5 ]
					do
						hostapd_pid=`cat /var/run/$pid_file_name`
						if [ -n "$hostapd_pid" ]; then
							break;
						fi
						retry_count=$((retry_count+1))
						sleep 1
					done
				fi

				[ ! -n "$hostapd_pid" ] && {
					trigger_whm_warn ${wdev} $whm_ifdown_id "HOSTAPD_START_FAILED"
					wireless_setup_failed HOSTAPD_START_FAILED
					return
				}

				wireless_add_process "$hostapd_pid" "/usr/sbin/hostapd" 1 "$is_shared"

				if [ -f /lib/netifd/debug_infrastructure.sh ]; then
					debug_infrastructure_execute_iw_command debug_iw_post_up_ $wdev
				fi
			}
		;;
		*)
			echo "Unknown action: \"$action\". Doing nothng"
		;;
	esac

	# delaying 2.4G Radio Start until after 5G Starts to Improve Same SSID situations Feature
	# Next line commented out due to conflict with the latest implementation based on hostapd
	# for_each_interface "ap" start_delayed_start_script_if_needed

	if [ -n "$shared_hapd" ]; then
		flock -u 223
	fi
	flock -u 222

	for_each_interface "ap" mac80211_setup_vif

	if [ "$action" = "setup" ]; then
		for_each_interface "sta adhoc mesh monitor" mac80211_setup_vif
		wireless_set_up
	fi
}

drv_mac80211_setup() {
	setup_reconf "setup" "$@"
}

drv_mac80211_reconf() {
	setup_reconf "reconf" "$@"
}

do_detach() {
	local cmd="/usr/sbin/hostapd_cli -iglobal-${shared_hapd} raw REMOVE ${main_iface}"
	$cmd

	local ifaces_left=`/usr/sbin/hostapd_cli -iglobal-${shared_hapd} raw INTERFACES`
	if [ "$ifaces_left" = "" ]; then
		kill "$pid"
	fi
}

drv_mac80211_detach() {

	json_select config
	json_get_vars \
		phy shared_hapd

	set_default shared_hapd "$DEFAULT_SHARED_HAPD"

	find_phy || {
		return
	}

	use_flock ${shared_hapd} 223

	local pid=`cat /var/run/wifi-global-${shared_hapd}.pid`
	if [ "$pid" -ne "$3" ]; then
		logger -s "pid check of detach $1 failed"
		logger -s "/var/run/wifi-global-${shared_hapd}.pid=$pid | pid received=$3"
		logger -s "will continue anyway"
	else
		local proc_name=`cat /proc/$pid/comm`
		if [ "$proc_name" -ne "hostapd" ]; then
			logger -s "proc_name check in detach $1 failed"
			logger -s "/proc/$pid/comm=$proc_name | expected hostapd"
			logger -s "will continue anyway"
		fi
	fi

	local main_iface=`get_main_iface $phy`

	do_detach

	flock -u 223
}

list_phy_interfaces() {
	local phy="$1"
	if [ -d "/sys/class/ieee80211/${phy}/device/net" ]; then
		ls "/sys/class/ieee80211/${phy}/device/net" 2>/dev/null;
	else
		ls "/sys/class/ieee80211/${phy}/device" 2>/dev/null | grep net: | sed -e 's,net:,,g'
	fi
}

trigger_whm_warn() {
	local iface=$1;
	local warn_id=$2
	local warn_str=$3

	#Trigger WHM Warning
	echo "Trigger WHM - iface: $iface id: $warn_id $warn_str"
	/usr/bin/timeout 5 iw dev $iface iwlwav sWhmTrigger $warn_id
}

drv_mac80211_teardown() {
	wireless_process_kill_all

	json_select data
	json_get_vars phy shared_hapd macaddr path
	set_default shared_hapd "$DEFAULT_SHARED_HAPD"
	json_select ..

	find_phy || {
		return
	}

	mac80211_interface_cleanup "$phy" "$shared_hapd"
}

add_driver mac80211
