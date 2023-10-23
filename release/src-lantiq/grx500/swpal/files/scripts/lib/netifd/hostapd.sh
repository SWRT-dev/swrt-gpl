. /lib/functions/network.sh
if [ -f /lib/netifd/debug_infrastructure.sh ]; then
	. /lib/netifd/debug_infrastructure.sh
fi
. /lib/functions.sh

wpa_supplicant_add_rate() {
	local var="$1"
	local val="$(($2 / 1000))"
	local sub="$((($2 / 100) % 10))"
	append $var "$val" ","
	[ $sub -gt 0 ] && append $var "."
}

hostapd_add_rate() {
	local var="$1"
	local val="$(($2 / 100))"
	append $var "$val" " "
}

hostapd_append_wep_key() {
	local var="$1"

	wep_keyidx=0
	set_default key 1
	case "$key" in
		[1234])
			for idx in 1 2 3 4; do
				local zidx
				zidx=$(($idx - 1))
				json_get_var ckey "key${idx}"
				[ -n "$ckey" ] && \
					append $var "wep_key${zidx}=$(prepare_key_wep "$ckey")" "$N$T"
			done
			wep_keyidx=$((key - 1))
		;;
		*)
			append $var "wep_key0=$(prepare_key_wep "$key")" "$N$T"
		;;
	esac
}

hostapd_append_wpa_key_mgmt() {
	local auth_type_l="$(echo $auth_type | tr 'a-z' 'A-Z')"

	case "$auth_type" in
		psk|eap)
			if [ "$mode" = "sta" ]; then
				append wpa_key_mgmt "WPA-$auth_type_l"
				[ "${ieee80211w:-0}" -gt 0 ] && append wpa_key_mgmt "WPA-${auth_type_l}-SHA256"
			else
				[ "${ieee80211w:-0}" -ne 2 ] && append wpa_key_mgmt "WPA-$auth_type_l"
				[ "${ieee80211w:-0}" -gt 1 ] && append wpa_key_mgmt "WPA-${auth_type_l}-SHA256"
			fi
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-${auth_type_l}"
		;;
		eap192)
			append wpa_key_mgmt "WPA-EAP-SUITE-B-192"
		;;
		eap-eap192)
			append wpa_key_mgmt "WPA-EAP-SUITE-B-192"
			append wpa_key_mgmt "WPA-EAP"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-EAP"
			if [ "$mode" = "sta" ]; then
				[ "${ieee80211w:-0}" -gt 0 ] && append wpa_key_mgmt "WPA-EAP-SHA256"
			else
				[ "${ieee80211w:-0}" -gt 1 ] && append wpa_key_mgmt "WPA-EAP-SHA256"
			fi
		;;
		sae)
			append wpa_key_mgmt "SAE"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-SAE"
		;;
		psk-sae)
			append wpa_key_mgmt "WPA-PSK"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-PSK"
			if [ "$mode" = "sta" ]; then
				[ "${ieee80211w:-0}" -gt 0 ] && append wpa_key_mgmt "WPA-PSK-SHA256"
			else
				[ "${ieee80211w:-0}" -gt 1 ] && append wpa_key_mgmt "WPA-PSK-SHA256"
			fi
			append wpa_key_mgmt "SAE"
			[ "${ieee80211r:-0}" -gt 0 ] && append wpa_key_mgmt "FT-SAE"
		;;
		owe)
			append wpa_key_mgmt "OWE"
		;;
	esac

	[ "$dpp_auth_enabled" -eq 1 ] && append wpa_key_mgmt "DPP"
	[ "$auth_osen" = "1" ] && append wpa_key_mgmt "OSEN"
}

hostapd_append_wmm_params() {
	local var="$1"

	[ -n "$wmm_ac_bk_cwmin" ] && append $var "wmm_ac_bk_cwmin=$wmm_ac_bk_cwmin" "$N"
	[ -n "$wmm_ac_bk_cwmax" ] && append $var "wmm_ac_bk_cwmax=$wmm_ac_bk_cwmax" "$N"
	[ -n "$wmm_ac_bk_aifs"  ] && append $var "wmm_ac_bk_aifs=$wmm_ac_bk_aifs" "$N"
	[ -n "$wmm_ac_bk_txop_limit" ] && append $var "wmm_ac_bk_txop_limit=$wmm_ac_bk_txop_limit" "$N"
	[ -n "$wmm_ac_bk_acm"  ] && append $var "wmm_ac_bk_acm=$wmm_ac_bk_acm" "$N"

	[ -n "$wmm_ac_be_cwmin" ] && append $var "wmm_ac_be_cwmin=$wmm_ac_be_cwmin" "$N"
	[ -n "$wmm_ac_be_cwmax" ] && append $var "wmm_ac_be_cwmax=$wmm_ac_be_cwmax" "$N"
	[ -n "$wmm_ac_be_aifs"  ] && append $var "wmm_ac_be_aifs=$wmm_ac_be_aifs" "$N"
	[ -n "$wmm_ac_be_txop_limit" ] && append $var "wmm_ac_be_txop_limit=$wmm_ac_be_txop_limit" "$N"
	[ -n "$wmm_ac_be_acm"  ] && append $var "wmm_ac_be_acm=$wmm_ac_be_acm" "$N"

	[ -n "$wmm_ac_vi_cwmin" ] && append $var "wmm_ac_vi_cwmin=$wmm_ac_vi_cwmin" "$N"
	[ -n "$wmm_ac_vi_cwmax" ] && append $var "wmm_ac_vi_cwmax=$wmm_ac_vi_cwmax" "$N"
	[ -n "$wmm_ac_vi_aifs"  ] && append $var "wmm_ac_vi_aifs=$wmm_ac_vi_aifs" "$N"
	[ -n "$wmm_ac_vi_txop_limit" ] && append $var "wmm_ac_vi_txop_limit=$wmm_ac_vi_txop_limit" "$N"
	[ -n "$wmm_ac_vi_acm"  ] && append $var "wmm_ac_vi_acm=$wmm_ac_vi_acm" "$N"

	[ -n "$wmm_ac_vo_cwmin" ] && append $var "wmm_ac_vo_cwmin=$wmm_ac_vo_cwmin" "$N"
	[ -n "$wmm_ac_vo_cwmax" ] && append $var "wmm_ac_vo_cwmax=$wmm_ac_vo_cwmax" "$N"
	[ -n "$wmm_ac_vo_aifs"  ] && append $var "wmm_ac_vo_aifs=$wmm_ac_vo_aifs" "$N"
	[ -n "$wmm_ac_vo_txop_limit" ] && append $var "wmm_ac_vo_txop_limit=$wmm_ac_vo_txop_limit" "$N"
	[ -n "$wmm_ac_vo_acm"  ] && append $var "wmm_ac_vo_acm=$wmm_ac_vo_acm" "$N"

	[ -n "$tx_queue_data0_cwmin" ] && append $var "tx_queue_data0_cwmin=$tx_queue_data0_cwmin" "$N"
	[ -n "$tx_queue_data0_cwmax" ] && append $var "tx_queue_data0_cwmax=$tx_queue_data0_cwmax" "$N"
	[ -n "$tx_queue_data0_aifs"  ] && append $var "tx_queue_data0_aifs=$tx_queue_data0_aifs" "$N"
	[ -n "$tx_queue_data0_burst" ] && append $var "tx_queue_data0_burst=$tx_queue_data0_burst" "$N"

	[ -n "$tx_queue_data1_cwmin" ] && append $var "tx_queue_data1_cwmin=$tx_queue_data1_cwmin" "$N"
	[ -n "$tx_queue_data1_cwmax" ] && append $var "tx_queue_data1_cwmax=$tx_queue_data1_cwmax" "$N"
	[ -n "$tx_queue_data1_aifs"  ] && append $var "tx_queue_data1_aifs=$tx_queue_data1_aifs" "$N"
	[ -n "$tx_queue_data1_burst" ] && append $var "tx_queue_data1_burst=$tx_queue_data1_burst" "$N"

	[ -n "$tx_queue_data2_cwmin" ] && append $var "tx_queue_data2_cwmin=$tx_queue_data2_cwmin" "$N"
	[ -n "$tx_queue_data2_cwmax" ] && append $var "tx_queue_data2_cwmax=$tx_queue_data2_cwmax" "$N"
	[ -n "$tx_queue_data2_aifs"  ] && append $var "tx_queue_data2_aifs=$tx_queue_data2_aifs" "$N"
	[ -n "$tx_queue_data2_burst" ] && append $var "tx_queue_data2_burst=$tx_queue_data2_burst" "$N"

	[ -n "$tx_queue_data3_cwmin" ] && append $var "tx_queue_data3_cwmin=$tx_queue_data3_cwmin" "$N"
	[ -n "$tx_queue_data3_cwmax" ] && append $var "tx_queue_data3_cwmax=$tx_queue_data3_cwmax" "$N"
	[ -n "$tx_queue_data3_aifs"  ] && append $var "tx_queue_data3_aifs=$tx_queue_data3_aifs" "$N"
	[ -n "$tx_queue_data3_burst" ] && append $var "tx_queue_data3_burst=$tx_queue_data3_burst" "$N"
}

hostapd_add_log_config() {
	config_add_boolean \
		log_80211 \
		log_8021x \
		log_radius \
		log_wpa \
		log_driver \
		log_iapp \
		log_mlme

	config_add_int log_level
}

hostapd_common_add_device_config() {
	config_add_array basic_rate
	config_add_array supported_rates

	config_add_string country
	config_add_string shared_hapd
	config_add_boolean country_ie doth
	config_add_string require_mode
	config_add_string dfs_ch_state_file
	config_add_boolean legacy_rates
	config_add_boolean tx_mcs_set_defined
	config_add_boolean ignore_supported_channels_errors
	config_add_boolean acs_use24overlapped
	config_add_int sub_band_dfs
	config_add_int sRadarRssiTh
	config_add_int ScanRssiTh
	config_add_int acs_scan_no_flush
	config_add_int sta_statistics
	config_add_int sConfigMRCoexActiveTime
	config_add_int sConfigMRCoex
	config_add_int sConfigMRCoexInactiveTime
	config_add_int sConfigMRCoexCts2SelfActive
	config_add_string sCoCPower
	config_add_string sCoCAutoCfg
	config_add_string sErpSet
	config_add_string sFWRecovery
	config_add_string sFixedRateCfg
	config_add_string sInterferDetThresh
	config_add_int acs_bg_scan_do_switch
	config_add_int acs_update_do_switch
	config_add_int num_antennas
	config_add_int owl
	config_add_int notify_action_frame
	config_add_int rts_threshold
	config_add_int sBfMode
	config_add_int sMaxMpduLen
	config_add_int sProbeReqCltMode
	config_add_boolean allow_scan_during_cac
	config_add_boolean sQAMplus
	config_add_boolean dynamic_country
	config_add_boolean dynamic_failsafe
	config_add_int multi_ap_profile
	config_add_int multi_ap_primary_vlanid
	config_add_int adv_proto_query_resp_len
	config_add_int rssi_reject_assoc_rssi


	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		debug_infrastructure_config_add_string debug_hostap_conf_
	fi

	hostapd_add_log_config
}

hostapd_prepare_device_config() {
	local config="$1"
	local driver="$2"

	local base="${config%%.conf}"
	local base_cfg=

	json_get_vars country country_ie beacon_int:100 doth require_mode legacy_rates \
					dfs_debug_chan externally_managed testbed_mode \
					sub_band_dfs sCoCPower sCoCAutoCfg sErpSet sFWRecovery sFixedRateCfg \
					sInterferDetThresh acs_bg_scan_do_switch acs_update_do_switch \
					sRadarRssiTh ScanRssiTh acs_scan_no_flush band num_antennas owl \
					sta_statistics notify_action_frame rts_threshold \
					allow_scan_during_cac dfs_ch_state_file \
					sConfigMRCoexActiveTime sQAMplus sBfMode sConfigMRCoex sConfigMRCoexCts2SelfActive \
					sConfigMRCoexInactiveTime sProbeReqCltMode acs_use24overlapped sMaxMpduLen \
					tx_mcs_set_defined ignore_supported_channels_errors dynamic_country dynamic_failsafe \
					multi_ap_profile multi_ap_primary_vlanid adv_proto_query_resp_len \
					rssi_reject_assoc_rssi

        json_get_var sPowerSelection txpower

	hostapd_set_log_options base_cfg

	set_default country_ie 1
	set_default doth 1
	set_default legacy_rates 1
	set_default testbed_mode 0
	set_default sPowerSelection "$txpower"

	case "$sPowerSelection" in
		"12") sPowerSelection=9 ;;
		"25") sPowerSelection=6 ;;
		"50") sPowerSelection=3 ;;
		"75") sPowerSelection=1 ;;
		"100") sPowerSelection=0 ;;
		*) sPowerSelection= ;;
	esac

	[ -n "$sCoCPower" ] && append base_cfg "sCoCPower=$sCoCPower" "$N"
	[ -n "$sCoCAutoCfg" ] && append base_cfg "sCoCAutoCfg=$sCoCAutoCfg" "$N"
	[ -n "$sErpSet" ] && append base_cfg "sErpSet=$sErpSet" "$N"
	[ -n "$sPowerSelection" ] && append base_cfg "sPowerSelection=$sPowerSelection" "$N"
	[ -n "$sFWRecovery" ] && append base_cfg "sFWRecovery=$sFWRecovery" "$N"
	[ -n "$sFixedRateCfg" ] && append base_cfg "sFixedRateCfg=$sFixedRateCfg" "$N"
	[ -n "$sInterferDetThresh" ] && append base_cfg "sInterferDetThresh=$sInterferDetThresh" "$N"
	[ -n "$acs_bg_scan_do_switch" ] && append base_cfg "acs_bg_scan_do_switch=$acs_bg_scan_do_switch" "$N"
	[ -n "$acs_update_do_switch" ] && append base_cfg "acs_update_do_switch=$acs_update_do_switch" "$N"
	[ -n "$sta_statistics" ] && append base_cfg "sStationsStat=$sta_statistics" "$N"
	[ -n "$owl" ] && append base_cfg "owl=$owl" "$N"
	[ -n "$notify_action_frame" ] && append base_cfg "notify_action_frame=$notify_action_frame" "$N"
	[ -n "$rts_threshold" ] && append base_cfg "rts_threshold=$rts_threshold" "$N"
	[ -n "$allow_scan_during_cac" ] && append base_cfg "allow_scan_during_cac=$allow_scan_during_cac" "$N"
	[ -n "$sBfMode" ] && append base_cfg "sBfMode=$sBfMode" "$N"
	[ -n "$sProbeReqCltMode" ] && append base_cfg "sProbeReqCltMode=$sProbeReqCltMode" "$N"
	[ -n "$acs_use24overlapped" ] && append base_cfg "acs_use24overlapped=$acs_use24overlapped" "$N"
	[ -n "$sMaxMpduLen" ] && append base_cfg "sMaxMpduLen=$sMaxMpduLen" "$N"
	[ -n "$dynamic_country" ] && append base_cfg "dynamic_country=$dynamic_country" "$N"
	[ -n "$dynamic_failsafe" ] && append base_cfg "dynamic_failsafe=$dynamic_failsafe" "$N"
	[ -n "$ScanRssiTh" ] && append base_cfg "ScanRssiTh=$ScanRssiTh" "$N"
	[ -n "$acs_scan_no_flush" ] && append base_cfg "acs_scan_no_flush=$acs_scan_no_flush" "$N"

	if [ -n "$sConfigMRCoex" ]; then
		append base_cfg "sConfigMRCoex=$sConfigMRCoex" "$N"
		[ -n "$sConfigMRCoexActiveTime" ] && append base_cfg "sConfigMRCoexActiveTime=$sConfigMRCoexActiveTime" "$N"
		[ -n "$sConfigMRCoexInactiveTime" ] && append base_cfg "sConfigMRCoexInactiveTime=$sConfigMRCoexInactiveTime" "$N"
		[ -n "$sConfigMRCoexCts2SelfActive" ] && append base_cfg "sConfigMRCoexCts2SelfActive=$sConfigMRCoexCts2SelfActive" "$N"
	fi

	[ -n "$dfs_ch_state_file" ] && append base_cfg "dfs_channels_state_file_location=$dfs_ch_state_file" "$N"

	[ -n "$tx_mcs_set_defined" ] && append base_cfg "tx_mcs_set_defined=$tx_mcs_set_defined" "$N"

	[ -n "$ignore_supported_channels_errors" ] && append base_cfg "ignore_supported_channels_errors=$ignore_supported_channels_errors" "$N"

	[ "$testbed_mode" -gt 0 ] && append base_cfg "testbed_mode=1" "$N"

	if [ "$hwmode" = "g" ] && [ -n "$ieee80211n" ]; then
		[ -n "$sQAMplus" ] && append base_cfg "sQAMplus=$sQAMplus" "$N"
	fi

	append base_cfg "atf_config_file=$hostapd_atf_conf_file" "$N"

	[ "$hwmode" = "b" ] && legacy_rates=1

	[ -n "$country" ] && {
		append base_cfg "country_code=$country" "$N"

		[ "$country_ie" -gt 0 ] && append base_cfg "ieee80211d=1" "$N"
		[ "$band" = "5GHz" -a "$doth" -gt 0 ] && {
			append base_cfg "ieee80211h=1" "$N"
			[ -n "$sub_band_dfs" ] && append base_cfg "sub_band_dfs=$sub_band_dfs" "$N"
			[ -n "$sRadarRssiTh" ] && append base_cfg "sRadarRssiTh=$sRadarRssiTh" "$N"
		}
	}

	local brlist= br
	json_get_values basic_rate_list basic_rate
	local rlist= r
	json_get_values rate_list supported_rates

	[ -n "$hwmode" ] && append base_cfg "hw_mode=$hwmode" "$N"
	[ "$legacy_rates" -eq 0 ] && set_default require_mode g

	[ "$hwmode" = "g" ] && { # TODO: Remove? not done in FAPI
		[ "$legacy_rates" -eq 0 ] && set_default rate_list "6000 9000 12000 18000 24000 36000 48000 54000"
		[ -n "$require_mode" ] && set_default basic_rate_list "6000 12000 24000"
	}

	case "$require_mode" in
		n) append base_cfg "require_ht=1" "$N";;
		ac) append base_cfg "require_vht=1" "$N";;
		ax) append base_cfg "require_he=1" "$N";;
	esac

	for r in $rate_list; do
		hostapd_add_rate rlist "$r"
	done

	for br in $basic_rate_list; do
		hostapd_add_rate brlist "$br"
	done

	[ -n "$rlist" ] && append base_cfg "supported_rates=$rlist" "$N"
	[ -n "$brlist" ] && append base_cfg "basic_rates=$brlist" "$N"
	append base_cfg "beacon_int=$beacon_int" "$N"

	[ -n "$dfs_debug_chan" ] && append base_cfg "dfs_debug_chan=$dfs_debug_chan" "$N"

	[ "$externally_managed" = "1" ] && append base_cfg "acs_scan_mode=1" "$N"
	[ -n "$he_beacon" ] && append base_cfg "he_beacon=$he_beacon" "$N"

	[ -n "$multi_ap_profile" ] && append base_cfg "multi_ap_profile=$multi_ap_profile" "$N"
	[ -n "$multi_ap_primary_vlanid" ] && append base_cfg "multi_ap_primary_vlanid=$multi_ap_primary_vlanid" "$N"
	[ -n "$adv_proto_query_resp_len" ] && append base_cfg "adv_proto_query_resp_len=$adv_proto_query_resp_len" "$N"
	[ -n "$rssi_reject_assoc_rssi" ] && append base_cfg "rssi_reject_assoc_rssi=$rssi_reject_assoc_rssi" "$N"
	cat > "$config" <<EOF
driver=$driver
$base_cfg
EOF
}

max_num_psk_file_entries_idx=29 # 30 entries
config_add_wpa_psk_config() {
	for i in $(seq 0 $max_num_psk_file_entries_idx); do
		config_add_array wpa_psk_key_$i
	done
}

hostapd_common_add_bss_config() {
	config_add_string colocated_6g_radio_info
	config_add_array 'colocated_6g_vap_info:list(int,macaddr,string,boolean,boolean,boolean,int)'
	config_add_string 'bssid:macaddr' 'ssid:string'
	config_add_boolean wmm uapsd hidden

	config_add_int maxassoc max_inactivity sae_pwe wds
	config_add_boolean disassoc_low_ack isolate short_preamble enable_hairpin

	config_add_int sBridgeMode

	config_add_string sAddPeerAP

	config_add_int \
		wep_rekey eap_reauth_period \
		wpa_group_rekey wpa_pair_rekey wpa_master_rekey
	config_add_boolean wpa_disable_eapol_key_retries

	config_add_boolean rsn_preauth auth_cache
	config_add_int ieee80211w
	config_add_boolean beacon_protection_enabled
	config_add_boolean disable_bigtk_rekey
	config_add_string group_mgmt_cipher
	config_add_int eapol_version

	config_add_string 'auth_server:host' 'server:host'
	config_add_string auth_secret
	config_add_int 'auth_port:port' 'port:port'

	config_add_string sec_auth_server
	config_add_string sec_auth_secret
	config_add_int sec_auth_port

	config_add_string acct_server
	config_add_string acct_secret
	config_add_int acct_port
	config_add_int acct_interim_interval

	config_add_string sec_acct_server
	config_add_string sec_acct_secret
	config_add_int sec_acct_port

	config_add_int eap_aaa_req_retries eap_aaa_req_timeout pmksa_life_time pmksa_interval \
		max_eap_failure auth_fail_blacklist_duration eap_req_id_retry_interval \
		failed_authentication_quiet_period

	config_add_string dae_client
	config_add_string dae_secret
	config_add_int dae_port

	config_add_string nasid
	config_add_string ownip
	config_add_string iapp_interface
	config_add_string eap_type ca_cert client_cert identity anonymous_identity auth priv_key priv_key_pwd

	config_add_int dynamic_vlan vlan_naming
	config_add_string vlan_tagged_interface vlan_bridge
	config_add_string vlan_file

	config_add_string 'key1:wepkey' 'key2:wepkey' 'key3:wepkey' 'key4:wepkey' 'password:wpakey'

	config_add_string wpa_psk_file
	config_add_wpa_psk_config

	config_add_string sae_key

	config_add_boolean wps_pushbutton wps_keypad wps_label ext_registrar wps_pbc_in_m1
	config_add_boolean wps_virtual_pushbutton wps_physical_pushbutton wps_virtual_display wps_physical_display
	config_add_int wps_ap_setup_locked wps_independent wps_state
	config_add_int wps_cred_processing
	config_add_string wps_device_type wps_device_name wps_manufacturer wps_pin
	config_add_string wps_uuid wps_pin_requests wps_os_version wps_rf_bands
	config_add_string wps_manufacturer_url wps_model_description upnp_bridge
	config_add_string wps_model_number wps_serial_number wps_skip_cred_build
	config_add_string wps_extra_cred wps_ap_settings wps_friendly_name
	config_add_string wps_model_url wps_upc wps_model_name

	config_add_boolean ieee80211r pmk_r1_push ft_over_ds
	config_add_int r0_key_lifetime reassociation_deadline
	config_add_string mobility_domain r1_key_holder
	config_add_array r0kh r1kh

	config_add_int ieee80211w_max_timeout ieee80211w_retry_timeout

	config_add_string macfilter 'macfile:file'
	config_add_array 'maclist:list(macaddr)'

	config_add_array bssid_blacklist
	config_add_array bssid_whitelist

	config_add_int mcast_rate
	config_add_array basic_rate
	config_add_array supported_rates

	config_add_boolean sae_require_mfp

	config_add_string 'owe_transition_bssid:macaddr' 'owe_transition_ssid:string'

	config_add_boolean hs20 disable_dgaf osen offload_gas
	config_add_int anqp_domain_id
	config_add_int hs20_deauth_req_timeout
	config_add_array hs20_oper_friendly_name
	config_add_array osu_provider
	config_add_array operator_icon
	config_add_array hs20_conn_capab
	config_add_string osu_ssid hs20_wan_metrics hs20_operating_class hs20_t_c_filename hs20_t_c_timestamp
	config_add_int hs20_release
	config_add_boolean enable_bss_load_ie manage_p2p allow_cross_connection tdls_prohibit

	config_add_string dpp_controller dpp_connector dpp_csign dpp_netaccesskey \
		dpp_netaccesskey_expired dpp_resp_wait_time dpp_gas_query_timeout_period

	config_add_int num_res_sta
	config_add_int proxy_arp
	config_add_int mbo
	config_add_int mbo_cell_aware
	config_add_int rrm_neighbor_report
	config_add_int bss_transition
	config_add_int interworking
	config_add_int access_network_type
	config_add_int gas_comeback_delay
	config_add_string authresp_elements
	config_add_string vendor_elements
	config_add_string assocresp_elements
	config_add_int num_vrt_backhauls
	config_add_int multi_ap
	config_add_int multi_ap_profile1_disallow
	config_add_int multi_ap_profile2_disallow
	config_add_string multi_ap_backhaul_ssid
	config_add_string multi_ap_backhaul_wpa_psk
	config_add_string multi_ap_backhaul_wpa_passphrase
	config_add_string ul_csa
	config_add_string mesh_mode
	config_add_string ctrl_interface_group
	config_add_string qos_map_set
	config_add_int s11nProtection
	config_add_string sAggrConfig
	config_add_string wav_bridge
	config_add_boolean sUdmaEnabled
	config_add_int sUdmaVlanId
	config_add_boolean vendor_vht
	config_add_boolean internet_available
	config_add_boolean asra
	config_add_boolean esr
	config_add_boolean uesa
	config_add_boolean rnr_auto_update
	config_add_int venue_type
	config_add_int venue_group
	config_add_string hessid
	config_add_int management_frames_rate
	config_add_int bss_beacon_int
	config_add_int sFwrdUnkwnMcast
	config_add_int mem_only_cred
	config_add_int dynamic_multicast_mode
	config_add_int dynamic_multicast_rate
	config_add_int roaming_anqp_ois_count
	config_add_string roaming_consortium_0
	config_add_string roaming_consortium_1
	config_add_string roaming_consortium_2


	config_add_boolean soft_block_acl_enable
	config_add_int soft_block_acl_wait_time \
		soft_block_acl_allow_time soft_block_acl_on_auth_req soft_block_acl_on_probe_req \
		event_cache_interval


	config_add_int \
		wmm_ac_bk_cwmin wmm_ac_bk_cwmax wmm_ac_bk_aifs wmm_ac_bk_txop_limit wmm_ac_bk_acm \
		wmm_ac_be_cwmin wmm_ac_be_cwmax wmm_ac_be_aifs wmm_ac_be_txop_limit wmm_ac_be_acm \
		wmm_ac_vi_cwmin wmm_ac_vi_cwmax wmm_ac_vi_aifs wmm_ac_vi_txop_limit wmm_ac_vi_acm \
		wmm_ac_vo_cwmin wmm_ac_vo_cwmax wmm_ac_vo_aifs wmm_ac_vo_txop_limit wmm_ac_vo_acm \
		tx_queue_data0_cwmin tx_queue_data0_cwmax tx_queue_data0_aifs \
		tx_queue_data1_cwmin tx_queue_data1_cwmax tx_queue_data1_aifs \
		tx_queue_data2_cwmin tx_queue_data2_cwmax tx_queue_data2_aifs \
		tx_queue_data3_cwmin tx_queue_data3_cwmax tx_queue_data3_aifs

	config_add_string \
		tx_queue_data0_burst tx_queue_data1_burst tx_queue_data2_burst tx_queue_data3_burst

	config_add_string start_after
	config_add_int start_after_delay
	config_add_int start_after_watchdog_time
	config_add_int pmf

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		debug_infrastructure_config_add_string debug_hostap_conf_
	fi
}

append_hs20_oper_friendly_name() {
	[ -n "$1" ] && append bss_conf "hs20_oper_friendly_name=$1" "$N"
}

append_osu_provider_service_desc() {
	append bss_conf "osu_service_desc=$1" "$N"
}

append_hs20_icon() {
	local width height lang type path
	config_get width "$1" width
	config_get height "$1" height
	config_get lang "$1" lang
	config_get type "$1" type
	config_get path "$1" path

	append bss_conf "hs20_icon=$width:$height:$lang:$type:$1:$path" "$N"
}

append_hs20_icons() {
	config_load wireless
	config_foreach append_hs20_icon hs20-icon
}

append_operator_icon() {
	[ -n "$1" ] && append bss_conf "operator_icon=$1" "$N"
}

append_osu_icon() {
	append bss_conf "osu_icon=$1" "$N"
}

append_osu_provider() {
	local cfgtype osu_server_uri osu_friendly_name osu_nai osu_nai2 osu_method_list

	config_load wireless
	config_get cfgtype "$1" TYPE
	[ "$cfgtype" != "osu-provider" ] && return

	append bss_conf "# provider $1" "$N"
	config_get osu_server_uri "$1" osu_server_uri
	config_get osu_nai "$1" osu_nai
	config_get osu_nai2 "$1" osu_nai2
	config_get osu_method_list "$1" osu_method

	append bss_conf "osu_server_uri=$osu_server_uri" "$N"
	append bss_conf "osu_nai=$osu_nai" "$N"
	append bss_conf "osu_nai2=$osu_nai2" "$N"
	append bss_conf "osu_method_list=$osu_method_list" "$N"

	config_list_foreach "$1" osu_service_desc append_osu_provider_service_desc
	config_list_foreach "$1" osu_icon append_osu_icon

	append bss_conf "$N"
}

append_hs20_conn_capab() {
	[ -n "$1" ] && append bss_conf "hs20_conn_capab=$1" "$N"
}

# List of arrays, each array contains 2 string entries: key_id and key
write_wpa_psk_file() {
	local ifname="$1"
	local psk_file="$2"
	local psk_conf=

	for i in $(seq 0 $max_num_psk_file_entries_idx); do
		j=0
		json_get_values wpa_psk_key_entry "wpa_psk_key_$i"

		if [ -z "$wpa_psk_key_entry" ]; then
			break
		fi

		for str in $wpa_psk_key_entry; do
			if [ $j -eq 0 ]; then
				append psk_conf "keyid=$str 00:00:00:00:00:00" "$N"
			elif [ $j -eq 1 ]; then
				append psk_conf "$str"
			else 
				logger -s "WPA PSK: Unsupported string $str in array at index $j"
			fi
			j=$((j+1))
		done
	done

	if [ -n "$psk_conf" ]; then
		if [ -z "$psk_file" ]; then
			psk_file="/var/run/hostapd-$ifname.wpa_psk"
			append bss_conf "wpa_psk_file=$psk_file" "$N"
		fi

		cat > "$psk_file" <<EOF
$psk_conf
EOF
	fi
}

hostapd_set_bss_options() {
	local var="$1"
	local phy="$2"
	local vif="$3"

	wireless_vif_parse_encryption

	local bss_conf
	local wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey wpa_key_mgmt
	local legacy_vendor_elements="dd050009860100"

	json_get_vars \
		wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey \
		wpa_disable_eapol_key_retries \
		maxassoc num_vrt_backhauls max_inactivity disassoc_low_ack isolate auth_cache \
		multi_ap multi_ap_profile1_disallow multi_ap_profile2_disallow \
		multi_ap_backhaul_ssid multi_ap_backhaul_wpa_psk multi_ap_backhaul_wpa_passphrase \
		wps_pushbutton wps_keypad wps_label ext_registrar wps_pbc_in_m1 wps_ap_setup_locked \
		wps_virtual_pushbutton wps_physical_pushbutton wps_virtual_display wps_physical_display \
		wps_independent wps_device_type wps_device_name wps_manufacturer wps_pin \
		macfilter ssid wmm uapsd hidden short_preamble rsn_preauth \
		iapp_interface eapol_version acct_server acct_secret acct_port \
		dynamic_vlan ieee80211w sec_acct_server sec_acct_secret sec_acct_port \
		acct_interim_interval wps_state wps_rf_bands wps_uuid qos_map_set \
		wmm_ac_bk_cwmin wmm_ac_bk_cwmax wmm_ac_bk_aifs wmm_ac_bk_txop_limit wmm_ac_bk_acm \
		wmm_ac_be_cwmin wmm_ac_be_cwmax wmm_ac_be_aifs wmm_ac_be_txop_limit wmm_ac_be_acm \
		wmm_ac_vi_cwmin wmm_ac_vi_cwmax wmm_ac_vi_aifs wmm_ac_vi_txop_limit wmm_ac_vi_acm \
		wmm_ac_vo_cwmin wmm_ac_vo_cwmax wmm_ac_vo_aifs wmm_ac_vo_txop_limit wmm_ac_vo_acm \
		tx_queue_data0_cwmin tx_queue_data0_cwmax tx_queue_data0_aifs tx_queue_data0_burst \
		tx_queue_data1_cwmin tx_queue_data1_cwmax tx_queue_data1_aifs tx_queue_data1_burst \
		tx_queue_data2_cwmin tx_queue_data2_cwmax tx_queue_data2_aifs tx_queue_data2_burst \
		tx_queue_data3_cwmin tx_queue_data3_cwmax tx_queue_data3_aifs tx_queue_data3_burst \
		mbo mbo_cell_aware rrm_neighbor_report num_res_sta mesh_mode ctrl_interface_group proxy_arp \
		authresp_elements vendor_elements assocresp_elements gas_comeback_delay \
		wps_pin_requests wps_os_version wps_cred_processing wps_manufacturer_url \
		wps_model_description interworking access_network_type bss_transition \
		s11nProtection sAggrConfig wav_bridge upnp_bridge \
		wps_model_number wps_serial_number wps_skip_cred_build wps_extra_cred \
		wps_ap_settings wps_friendly_name wps_model_url wps_upc wps_model_name \
		sUdmaEnabled sUdmaVlanId vendor_vht internet_available asra esr uesa \
		venue_type venue_group hessid sae_require_mfp management_frames_rate \
		sBridgeMode sAddPeerAP bss_beacon_int sFwrdUnkwnMcast \
		mem_only_cred dynamic_multicast_mode dynamic_multicast_rate sae_pwe \
		beacon_protection_enabled disable_bigtk_rekey group_mgmt_cipher \
		soft_block_acl_enable soft_block_acl_wait_time soft_block_acl_allow_time \
		soft_block_acl_on_auth_req soft_block_acl_on_probe_req event_cache_interval \
		colocated_6g_radio_info ul_csa rnr_auto_update \
		roaming_consortium_0 roaming_consortium_1 roaming_consortium_2 roaming_anqp_ois_count \
		start_after start_after_delay start_after_watchdog_time enable_hairpin

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		debug_infrastructure_json_get_vars debug_hostap_conf_
	fi

	set_default isolate 0
	set_default max_inactivity 0
	set_default proxy_arp 0
	set_default num_res_sta 0
	set_default gas_comeback_delay 0
	set_default short_preamble 1
	set_default disassoc_low_ack 1
	set_default hidden 0
	set_default wmm 1
	set_default uapsd 1
	set_default wpa_disable_eapol_key_retries 0
	set_default eapol_version 0
	set_default acct_port 1813
	set_default sec_acct_port 1813
	set_default mbo 1
	set_default rrm_neighbor_report 0
	set_default bss_transition 1
	set_default interworking 1
	set_default access_network_type 0
	set_default vendor_vht 1
	set_default mem_only_cred 0
	set_default enable_hairpin 0

	append bss_conf "ctrl_interface=/var/run/hostapd" "$N"
	[ -n "$ctrl_interface_group" ] && {
		append bss_conf "ctrl_interface_group=$ctrl_interface_group" "$N"
	}
	[ -n "$mesh_mode" ] && {

		# Hybrid mode is deprecated
		[ "$mesh_mode" = "hybrid" ] && {
			mesh_mode="ext_hybrid"
		}

		append bss_conf "mesh_mode=$mesh_mode" "$N"
		[ "$mesh_mode" = "bAP" ] && {
			maxassoc=1
			num_res_sta=0
		}

		[ "$mesh_mode" = "ext_hybrid" ] && {
			[ -n "$num_vrt_backhauls" ] && append bss_conf "num_vrt_bkh_netdevs=$num_vrt_backhauls" "$N"
		}

		[ "$mesh_mode" = "bAP" ] || [ "$mesh_mode" = "ext_hybrid" ] && {
			[ -n "$multi_ap" ] && append bss_conf "multi_ap=$multi_ap" "$N"
			[ -n "$multi_ap_profile1_disallow" ] && append bss_conf "multi_ap_profile1_disallow=$multi_ap_profile1_disallow" "$N"
			[ -n "$multi_ap_profile2_disallow" ] && append bss_conf "multi_ap_profile2_disallow=$multi_ap_profile2_disallow" "$N"
		}

		[ "$mesh_mode" = "fAP" ] && {
			[ -n "$multi_ap_backhaul_ssid" ] && append bss_conf "multi_ap_backhaul_ssid=\"$multi_ap_backhaul_ssid\"" "$N"
			[ -n "$multi_ap_backhaul_wpa_psk" ] && append bss_conf "multi_ap_backhaul_wpa_psk=$multi_ap_backhaul_wpa_psk" "$N"
			[ -n "$multi_ap_backhaul_wpa_passphrase" ] && append bss_conf "multi_ap_backhaul_wpa_passphrase=$multi_ap_backhaul_wpa_passphrase" "$N"
		}
	}
	[ -n "$ul_csa" ] && append bss_conf "ul_csa=$ul_csa" "$N"
	
	if [ "$isolate" -gt 0 ]; then
		append bss_conf "ap_isolate=$isolate" "$N"
		append bss_conf "enable_hairpin=$enable_hairpin" "$N"
	fi

	[ -n "$maxassoc" ] && append bss_conf "max_num_sta=$maxassoc" "$N"

	[ -n "$sBridgeMode" ] && append bss_conf "sBridgeMode=$sBridgeMode" "$N"

	[ -n "$sAddPeerAP" ] && append bss_conf "sAddPeerAP=$sAddPeerAP" "$N"

	if [ "$max_inactivity" -gt 0 ]; then
		append bss_conf "ap_max_inactivity=$max_inactivity" "$N"
	fi
	if [ "$proxy_arp" -gt 0 ]; then
		append bss_conf "proxy_arp=$proxy_arp" "$N"
	fi
	if [ "$num_res_sta" -gt 0 ]; then
		append bss_conf "num_res_sta=$num_res_sta" "$N"
	fi
	if [ "$gas_comeback_delay" -gt "0" ]; then
		append bss_conf "gas_comeback_delay=$gas_comeback_delay" "$N"
	fi
	if [ "$rrm_neighbor_report" -gt "0" ]; then
		append bss_conf "rrm_neighbor_report=$rrm_neighbor_report" "$N"
	fi
	if [ "$bss_transition" -gt "0" ]; then
		append bss_conf "bss_transition=$bss_transition" "$N"
	fi

	if [ "$mem_only_cred" -gt "0" ]; then
		append bss_conf "mem_only_cred=$mem_only_cred" "$N"
	fi

	[ -n "$bss_beacon_int" ] && append bss_conf "bss_beacon_int=$bss_beacon_int" "$N"
	[ -n "$sFwrdUnkwnMcast" ] && append bss_conf "sFwrdUnkwnMcast=$sFwrdUnkwnMcast" "$N"
	[ -n "$dynamic_multicast_mode" ] && append bss_conf "dynamic_multicast_mode=$dynamic_multicast_mode" "$N"
	[ -n "$dynamic_multicast_rate" ] && append bss_conf "dynamic_multicast_rate=$dynamic_multicast_rate" "$N"
	[ -n "$rnr_auto_update" ] && append bss_conf "rnr_auto_update=$rnr_auto_update" "$N"
	[ -n "$start_after" ] && append bss_conf "start_after=$start_after" "$N"
	[ -n "$start_after_delay" ] && append bss_conf "start_after_delay=$start_after_delay" "$N"
	[ -n "$start_after_watchdog_time" ] && append bss_conf "start_after_watchdog_time=$start_after_watchdog_time" "$N"

	if [ "$interworking" -gt 0 ]; then
		append bss_conf "interworking=$interworking" "$N"
		
		if [ "$access_network_type" -gt 0 ]; then
			append bss_conf "access_network_type=$access_network_type" "$N"
		fi

		if [ -n "$internet_available" ]; then
			append bss_conf "internet=$internet_available" "$N"
		fi
		if [ -n "$asra" ]; then
			append bss_conf "asra=$asra" "$N"
		fi
		if [ -n "$esr" ]; then
			append bss_conf "esr=$esr" "$N"
		fi
		if [ -n "$uesa" ]; then
			append bss_conf "uesa=$uesa" "$N"
		fi
		if [ -n "$venue_type" ]; then
			append bss_conf "venue_type=$venue_type" "$N"
		fi
		if [ -n "$venue_group" ]; then
			append bss_conf "venue_group=$venue_group" "$N"
		fi
		if [ -n "$hessid" ]; then
			append bss_conf "hessid=$hessid" "$N"
		fi
		if [ -n "$roaming_anqp_ois_count" ]; then
			append bss_conf "roaming_anqp_ois_count=$roaming_anqp_ois_count" "$N"
		fi
		if [ -n "$roaming_consortium_0" ]; then
			append bss_conf "roaming_consortium=$roaming_consortium_0" "$N"
		fi
		if [ -n "$roaming_consortium_1" ]; then
			append bss_conf "roaming_consortium=$roaming_consortium_1" "$N"
		fi
		if [ -n "$roaming_consortium_2" ]; then
			append bss_conf "roaming_consortium=$roaming_consortium_2" "$N"
		fi
	fi

	[ -n "$management_frames_rate"  ] && append bss_conf "management_frames_rate=$management_frames_rate" "$N"

	append bss_conf "disassoc_low_ack=$disassoc_low_ack" "$N"
	append bss_conf "preamble=$short_preamble" "$N"
	append bss_conf "wmm_enabled=$wmm" "$N"
	append bss_conf "ignore_broadcast_ssid=$hidden" "$N"
	 [ "$hidden" -gt 0 ] && {
		/usr/bin/logger -s "********* WPS warning! $ifname ignore_broadcast_ssid is set, WPS will be disabled  ***********"
	}
	append bss_conf "uapsd_advertisement_enabled=$uapsd" "$N"

	[ "$mbo" -gt 0 ] && {
		append bss_conf "mbo=$mbo" "$N"
		[ -n "$mbo_cell_aware" ] && append bss_conf "mbo_cell_aware=$mbo_cell_aware" "$N"
	}

	[ "$wmm" -gt 0 ] && {
		hostapd_append_wmm_params bss_conf
	}

	[ "$wpa" -gt 0 ] && {
		[ -n "$wpa_group_rekey"  ] && append bss_conf "wpa_group_rekey=$wpa_group_rekey" "$N"
		[ -n "$wpa_pair_rekey"   ] && append bss_conf "wpa_ptk_rekey=$wpa_pair_rekey"    "$N"
		[ -n "$wpa_master_rekey" ] && append bss_conf "wpa_gmk_rekey=$wpa_master_rekey"  "$N"
	}

	append bss_conf "vendor_elements=${legacy_vendor_elements}${vendor_elements}" "$N"
	[ -n "$authresp_elements" ] && append bss_conf "authresp_elements=$authresp_elements" "$N"
	[ -n "$assocresp_elements" ] && append bss_conf "assocresp_elements=$assocresp_elements" "$N"

	[ -n "$qos_map_set" ] && append bss_conf "qos_map_set=$qos_map_set" "$N"
	[ -n "$s11nProtection" ] && append bss_conf "s11nProtection=$s11nProtection" "$N"
	[ -n "$sAggrConfig" ] && append bss_conf "sAggrConfig=$sAggrConfig" "$N"
	[ -n "$sUdmaEnabled" ] && {
		append bss_conf "sUdmaEnabled=$sUdmaEnabled" "$N"
		[ -n "$sUdmaVlanId" ] && append bss_conf "sUdmaVlanId=$sUdmaVlanId" "$N"
	}

	# if soft_block_acl_enable isn't defined or set to false/0,
	# softblock parameters will not be written into .conf file
	[ -n "$soft_block_acl_enable" ] && {
		set_default soft_block_acl_enable 0
		append bss_conf "soft_block_acl_enable=$soft_block_acl_enable" "$N"

		# if soft_block_acl_enable defined and > 0, write parameters to conf
		[ "$soft_block_acl_enable" -gt 0 ] && {
			set_default soft_block_acl_wait_time 10000
			set_default soft_block_acl_allow_time 15000
			set_default soft_block_acl_on_auth_req 1
			set_default soft_block_acl_on_probe_req 1
			append bss_conf "soft_block_acl_wait_time=$soft_block_acl_wait_time" "$N"
			append bss_conf "soft_block_acl_allow_time=$soft_block_acl_allow_time" "$N"
			append bss_conf "soft_block_acl_on_auth_req=$soft_block_acl_on_auth_req" "$N"
			append bss_conf "soft_block_acl_on_probe_req=$soft_block_acl_on_probe_req" "$N"
		}
	}

	set_default event_cache_interval 1
	append bss_conf "event_cache_interval=$event_cache_interval" "$N"

	local hs20 disable_dgaf osen anqp_domain_id hs20_deauth_req_timeout \
		osu_ssid hs20_wan_metrics hs20_operating_class hs20_t_c_filename hs20_t_c_timestamp \
		hs20_release enable_bss_load_ie manage_p2p allow_cross_connection tdls_prohibit \
		offload_gas
	json_get_vars hs20 disable_dgaf osen anqp_domain_id hs20_deauth_req_timeout \
		osu_ssid hs20_wan_metrics hs20_operating_class hs20_t_c_filename hs20_t_c_timestamp \
		hs20_release enable_bss_load_ie manage_p2p allow_cross_connection tdls_prohibit \
		offload_gas

	set_default hs20 0
	set_default osen 0
	set_default anqp_domain_id 0
	set_default hs20_deauth_req_timeout 60
	set_default hs20_release 1
	if [ "$hs20" = "1" ]; then
		set_default offload_gas 1
		append bss_conf "hs20=1" "$N"
		append_hs20_icon
		append bss_conf "osen=$osen" "$N"
		append bss_conf "anqp_domain_id=$anqp_domain_id" "$N"
		append bss_conf "hs20_deauth_req_timeout=$hs20_deauth_req_timeout" "$N"
		append bss_conf "hs20_release=$hs20_release" "$N"
		[ -n "$disable_dgaf" ] && append bss_conf "disable_dgaf=$disable_dgaf" "$N"
		[ -n "$osu_ssid" ] && append bss_conf "osu_ssid=$osu_ssid" "$N"
		[ -n "$hs20_wan_metrics" ] && append bss_conf "hs20_wan_metrics=$hs20_wan_metrics" "$N"
		[ -n "$hs20_operating_class" ] && append bss_conf "hs20_operating_class=$hs20_operating_class" "$N"
		[ -n "$hs20_t_c_filename" ] && append bss_conf "hs20_t_c_filename=$hs20_t_c_filename" "$N"
		[ -n "$hs20_t_c_timestamp" ] && append bss_conf "hs20_t_c_timestamp=$hs20_t_c_timestamp" "$N"
		json_for_each_item append_hs20_conn_capab hs20_conn_capab
		json_for_each_item append_hs20_oper_friendly_name hs20_oper_friendly_name
		json_for_each_item append_osu_provider osu_provider
		json_for_each_item append_operator_icon operator_icon
	fi

	[ -n "$offload_gas" ] && append bss_conf "offload_gas=$offload_gas" "$N"
	[ -n "$enable_bss_load_ie" ] && append bss_conf "enable_bss_load_ie=$enable_bss_load_ie" "$N"
	[ -n "$tdls_prohibit" ] && append bss_conf "tdls_prohibit=$tdls_prohibit" "$N"
	[ -n "$manage_p2p" ] && {
		append bss_conf "manage_p2p=$manage_p2p" "$N"
		[ -n "$allow_cross_connection" ] && append bss_conf "allow_cross_connection=$allow_cross_connection" "$N"
	}

#	bss_md5sum=$(echo $bss_conf | md5sum | cut -d" " -f1)
#	append bss_conf "config_id=$bss_md5sum" "$N"

	if [ "$hwmode" = "g" ]; then
		[ -n "$ieee80211n" ] && append bss_conf "vendor_vht=$vendor_vht" "$N"
	fi

	[ -n "$acct_server" ] && {
		append bss_conf "acct_server_addr=$acct_server" "$N"
		append bss_conf "acct_server_port=$acct_port" "$N"
		[ -n "$acct_secret" ] && \
			append bss_conf "acct_server_shared_secret=$acct_secret" "$N"
	}

	[ -n "$sec_acct_server" ] && {
		append bss_conf "acct_server_addr=$sec_acct_server" "$N"
		append bss_conf "acct_server_port=$sec_acct_port" "$N"
		[ -n "$sec_acct_secret" ] && \
			append bss_conf "acct_server_shared_secret=$sec_acct_secret" "$N"
	}

	[ -n "$acct_server" -o -n "$sec_acct_server" ] && {
		[ -n "$acct_interim_interval" ] && \
		append bss_conf "radius_acct_interim_interval=$acct_interim_interval" "$N"
	}

	case "$auth_type" in
		sae|owe|eap192|eap-eap192)
			set_default ieee80211w 2
			set_default sae_require_mfp 1
		;;
		psk-sae)
			set_default ieee80211w 1
			set_default sae_require_mfp 1
		;;
	esac
	[ -n "$sae_require_mfp" ] && append bss_conf "sae_require_mfp=$sae_require_mfp" "$N"
	[ -n "$sae_pwe" ]  && append bss_conf "sae_pwe=$sae_pwe" "$N"

	# DPP configuration
	if [ "$dpp_auth_enabled" -eq 1 ]; then
		# For AP ieee80211w can be optional
		set_default ieee80211w 1
		set_default sae_require_mfp 1

		json_get_vars \
			dpp_controller dpp_connector dpp_csign dpp_netaccesskey

		set_default interworking 1
		[ -n "$dpp_controller" ] && append bss_conf "dpp_controller=$dpp_controller" "$N"
		[ -n "$dpp_connector" ] && append bss_conf "dpp_connector=$dpp_connector" "$N"
		[ -n "$dpp_csign" ] && append bss_conf "dpp_csign=$dpp_csign" "$N"
		[ -n "$dpp_netaccesskey" ] && append bss_conf "dpp_netaccesskey=$dpp_netaccesskey" "$N"
		if [ -n "$dpp_netaccesskey" ]; then
			json_get_vars dpp_netaccesskey_expiry
			[ -n "$dpp_netaccesskey_expiry" ] && append bss_conf "dpp_netaccesskey_expiry=$dpp_netaccesskey_expiry" "$N"
		fi

		# DPP timeouts
		json_get_vars dpp_resp_wait_time dpp_gas_query_timeout_period
		set_default dpp_resp_wait_time 5500
		set_default dpp_gas_query_timeout_period 10
		[ -n "$dpp_resp_wait_time" ] && append bss_conf "dpp_resp_wait_time=$dpp_resp_wait_time" "$N"
		[ -n "$dpp_gas_query_timeout_period" ] && append bss_conf "dpp_gas_query_timeout_period=$dpp_gas_query_timeout_period" "$N"
	fi

	local vlan_possible=""

	case "$auth_type" in
		none|owe)
			json_get_vars owe_transition_bssid owe_transition_ssid

			[ -n "$owe_transition_ssid" ] && append bss_conf "owe_transition_ssid=\"$owe_transition_ssid\"" "$N"
			[ -n "$owe_transition_bssid" ] && append bss_conf "owe_transition_bssid=$owe_transition_bssid" "$N"

			wps_possible=1
			# Here we make the assumption that if we're in open mode
			# with WPS enabled, we got to be in unconfigured state.
			wps_not_configured=1
			[ "$auth_type" = "none" ] &&
				/usr/bin/logger -s "********* WPS warning! $ifname security is not set, require explicit operation to create open network  ***********"
		;;
		psk)
			json_get_vars key wpa_psk_file
			if [ ${#key} -lt 8 ]; then
				if [ "$mem_only_cred" != "1" ]; then
					wireless_setup_vif_failed INVALID_WPA_PSK
					return 1
				fi
			elif [ ${#key} -eq 64 ]; then
				append bss_conf "wpa_psk=$key" "$N"
			else
				append bss_conf "wpa_passphrase=$key" "$N"
			fi
			[ -n "$wpa_psk_file" ] && {
				[ -e "$wpa_psk_file" ] || touch "$wpa_psk_file"
				append bss_conf "wpa_psk_file=$wpa_psk_file" "$N"
			}
			
			write_wpa_psk_file $ifname $wpa_psk_file

			[ "$eapol_version" -ge "1" -a "$eapol_version" -le "2" ] && append bss_conf "eapol_version=$eapol_version" "$N"

			wps_possible=1
		;;
		psk-sae)
			json_get_vars key sae_key
			if [ "$mem_only_cred" != "1" ]; then
				if [[ ${#key} -lt 8 || ${#key} -gt 63 ]]; then
					wireless_setup_vif_failed INVALID_WPA_PSK
					return 1
				fi
			fi

			if [[ "$key" != "" && "$sae_key" != "" ]]; then
				append bss_conf "wpa_passphrase=$key" "$N"
				append bss_conf "sae_password=$sae_key" "$N"
			elif [ "$key" != "" ]; then
				append bss_conf "wpa_passphrase=$key" "$N"
				append bss_conf "sae_password=$key" "$N"
			else
				if [ "$mem_only_cred" != "1" ]; then
					wireless_setup_vif_failed INVALID_WPA_PSK
					return 1
				fi
			fi

			wps_possible=1
		;;
		sae)
			json_get_vars key sae_key
			if [ "$sae_key" != "" ]; then
				append bss_conf "sae_password=$sae_key" "$N"
			elif [ "$key" != "" ]; then
				append bss_conf "sae_password=$key" "$N"
			else
				if [ "$mem_only_cred" != "1" ]; then
					wireless_setup_vif_failed INVALID_WPA_PSK
					return 1
				fi
			fi
		;;
		eap|eap192|eap-eap192)
			json_get_vars \
				auth_server auth_secret auth_port \
				sec_auth_server sec_auth_secret sec_auth_port \
				dae_client dae_secret dae_port \
				ownip \
				eap_reauth_period \
				venue_type venue_group hessid sae_require_mfp \
				eap_aaa_req_retries eap_aaa_req_timeout pmksa_life_time pmksa_interval \
				max_eap_failure auth_fail_blacklist_duration eap_req_id_retry_interval \
				failed_authentication_quiet_period

			# radius can provide VLAN ID for clients
			vlan_possible=1

			# legacy compatibility
			[ -n "$auth_server" ] || json_get_var auth_server server
			[ -n "$auth_port" ] || json_get_var auth_port port
			[ -n "$auth_secret" ] || json_get_var auth_secret key

			set_default auth_port 1812
			set_default sec_auth_port 1812
			set_default dae_port 3799

			append bss_conf "auth_server_addr=$auth_server" "$N"
			append bss_conf "auth_server_port=$auth_port" "$N"
			append bss_conf "auth_server_shared_secret=$auth_secret" "$N"

			[ -n "$eap_aaa_req_retries" ] && append bss_conf "eap_aaa_req_retries=$eap_aaa_req_retries" "$N"
			[ -n "$eap_aaa_req_timeout" ] && append bss_conf "eap_aaa_req_timeout=$eap_aaa_req_timeout" "$N"
			[ -n "$pmksa_life_time" ] && append bss_conf "pmksa_life_time=$pmksa_life_time" "$N"
			[ -n "$pmksa_interval" ] && append bss_conf "pmksa_interval=$pmksa_interval" "$N"
			[ -n "$max_eap_failure" ] && append bss_conf "max_eap_failure=$max_eap_failure" "$N"
			[ -n "$auth_fail_blacklist_duration" ] && append bss_conf "auth_fail_blacklist_duration=$auth_fail_blacklist_duration" "$N"
			[ -n "$eap_req_id_retry_interval" ] && append bss_conf "eap_req_id_retry_interval=$eap_req_id_retry_interval" "$N"
			[ -n "$failed_authentication_quiet_period" ] && append bss_conf "failed_authentication_quiet_period=$failed_authentication_quiet_period" "$N"

			[ -n "$sec_auth_server" ] && {
				append bss_conf "auth_server_addr=$sec_auth_server" "$N"
				append bss_conf "auth_server_port=$sec_auth_port" "$N"
				append bss_conf "auth_server_shared_secret=$sec_auth_secret" "$N"
			}

			[ -n "$eap_reauth_period" ] && append bss_conf "eap_reauth_period=$eap_reauth_period" "$N"

			[ -n "$dae_client" -a -n "$dae_secret" ] && {
				append bss_conf "radius_das_port=$dae_port" "$N"
				append bss_conf "radius_das_client=$dae_client $dae_secret" "$N"
			}

			[ -n "$ownip" ] && append bss_conf "own_ip_addr=$ownip" "$N"
			append bss_conf "eapol_key_index_workaround=1" "$N"
			append bss_conf "ieee8021x=1" "$N"

			[ "$eapol_version" -ge "1" -a "$eapol_version" -le "2" ] && append bss_conf "eapol_version=$eapol_version" "$N"
		;;
		wep)
			local wep_keyidx=0
			json_get_vars key
			hostapd_append_wep_key bss_conf
			append bss_conf "wep_default_key=$wep_keyidx" "$N"
			[ -n "$wep_rekey" ] && append bss_conf "wep_rekey_period=$wep_rekey" "$N"
			/usr/bin/logger -s "********* WPS warning! $ifname wep security is set, WPS will be disabled ***********"
		;;
	esac

	local auth_algs=$((($auth_mode_shared << 1) | $auth_mode_open))
	append bss_conf "auth_algs=${auth_algs:-1}" "$N"
	append bss_conf "wpa=$wpa" "$N"
	[ -n "$wpa_pairwise" ] && append bss_conf "wpa_pairwise=$wpa_pairwise" "$N"
	[ -n "$rsn_pairwise" ] && append bss_conf "rsn_pairwise=$rsn_pairwise" "$N"

	set_default wps_pushbutton 0
	set_default wps_keypad 0
	set_default wps_label 0
	set_default wps_pbc_in_m1 0
	set_default wps_virtual_pushbutton 0
	set_default wps_physical_pushbutton 0
	set_default wps_virtual_display 0
	set_default wps_physical_display 0

	config_methods=
	[ "$wps_pushbutton" -gt 0 ] && append config_methods push_button
	[ "$wps_label" -gt 0 ] && append config_methods label
	[ "$wps_keypad" -gt 0 ] && append config_methods keypad
	[ "$wps_virtual_pushbutton" -gt 0 ] && append config_methods virtual_push_button
	[ "$wps_physical_pushbutton" -gt 0 ] && append config_methods physical_push_button
	[ "$wps_virtual_display" -gt 0 ] && append config_methods virtual_display
	[ "$wps_physical_display" -gt 0 ] && append config_methods physical_display

	[ "$macfilter" = "allow" ] && wps_possible=

	[ -n "$wps_possible" -a -n "$config_methods" ] && {
		set_default ext_registrar 0
		set_default wps_device_type "6-0050F204-1"
		set_default wps_device_name "WLAN-ROUTER"
		set_default wps_manufacturer "Intel Corporation"
		set_default wps_manufacturer_url "http://www.intel.com"
		set_default wps_model_description "TR069 Gateway"
		set_default wps_os_version "01020300"
		set_default wps_cred_processing 1
		set_default wps_independent 1
		set_default wps_state 2

		if [ "$ext_registrar" -gt "0" ] && [ -n "$network_bridge" ]; then
			append bss_conf "upnp_iface=$network_bridge" "$N"
		elif [ -n "$upnp_bridge" ]; then
			append bss_conf "upnp_iface=$upnp_bridge" "$N"
		fi

		append bss_conf "eap_server=1" "$N"
		[ -n "$wps_pin" ] && append bss_conf "ap_pin=$wps_pin" "$N"
		[ -n "$wps_uuid" ] && append bss_conf "uuid=$wps_uuid" "$N"
		[ -n "$wps_pin_requests" ] && append bss_conf "wps_pin_requests=$wps_pin_requests" "$N"
		append bss_conf "wps_state=$wps_state" "$N"
		append bss_conf "device_type=$wps_device_type" "$N"
		append bss_conf "device_name=$wps_device_name" "$N"
		append bss_conf "manufacturer=$wps_manufacturer" "$N"
		append bss_conf "config_methods=$config_methods" "$N"
		append bss_conf "wps_independent=$wps_independent" "$N"
		append bss_conf "wps_cred_processing=$wps_cred_processing" "$N"
		[ -n "$wps_ap_setup_locked" ] && append bss_conf "ap_setup_locked=$wps_ap_setup_locked" "$N"
		[ "$wps_pbc_in_m1" -gt 0 ] && append bss_conf "pbc_in_m1=$wps_pbc_in_m1" "$N"

		append bss_conf "os_version=$wps_os_version" "$N"
		append bss_conf "manufacturer_url=$wps_manufacturer_url" "$N"
		append bss_conf "model_description=$wps_model_description" "$N"

		[ -n "$wps_rf_bands" ] && append bss_conf "wps_rf_bands=$wps_rf_bands" "$N"
		[ -n "$wps_model_name" ] && append bss_conf "model_name=$wps_model_name" "$N"
		[ -n "$wps_model_number" ] && append bss_conf "model_number=$wps_model_number" "$N"
		[ -n "$wps_serial_number" ] && append bss_conf "serial_number=$wps_serial_number" "$N"
		[ -n "$wps_skip_cred_build" ] && append bss_conf "skip_cred_build=$wps_skip_cred_build" "$N"
		[ -n "$wps_extra_cred" ] && append bss_conf "extra_cred=$wps_extra_cred" "$N"
		[ -n "$wps_ap_settings" ] && append bss_conf "ap_settings=$wps_ap_settings" "$N"
		[ -n "$wps_friendly_name" ] && append bss_conf "friendly_name=$wps_friendly_name" "$N"
		[ -n "$wps_model_url" ] && append bss_conf "model_url=$wps_model_url" "$N"
		[ -n "$wps_upc" ] && append bss_conf "upc=$wps_upc" "$N"
	}

	append bss_conf "ssid=$ssid" "$N"

	[ -n "$colocated_6g_radio_info" ] && append bss_conf "colocated_6g_radio_info=$colocated_6g_radio_info" "$N"
	json_get_values colocated_6g_vap_info_list colocated_6g_vap_info
	i=1
	num_coloc_vap_param=7 # <Radio_index> <BSSID> <SSID> <multibss_enable> 
				#<is_transmitted_bssid> <unsolicited_frame_support> <max_tx_power>

	if [ -n "$colocated_6g_vap_info_list" ]; then
		for j in $colocated_6g_vap_info_list #special handling as list data type
		do
			if [ -n "$val" ]; then
				val=$val" "$j
			else
				val=$j
			fi
			if [ $i -eq $num_coloc_vap_param ]; then
				append bss_conf "colocated_6g_vap_info=$val" "$N"
				i=1
				val=""
			else
				i=$((i+1))
			fi

		done
	fi

	if [ -n "$network_bridge" ]; then
		 append bss_conf "bridge=$network_bridge" "$N"
	else
		if [ -n "$wav_bridge" ]; then
			append bss_conf "bridge=$wav_bridge" "$N"
		fi
	fi
	[ -n "$iapp_interface" ] && {
		local ifname
		network_get_device ifname "$iapp_interface" || ifname="$iapp_interface"
		append bss_conf "iapp_interface=$ifname" "$N"
	}

	if [ "$wpa" -ge "1" ]; then
		json_get_vars nasid ieee80211r
		set_default ieee80211r 0
		[ -n "$nasid" ] && append bss_conf "nas_identifier=$nasid" "$N"

		if [ "$ieee80211r" -gt "0" ]; then
			json_get_vars mobility_domain r0_key_lifetime r1_key_holder \
			reassociation_deadline pmk_r1_push ft_over_ds
			json_get_values r0kh r0kh
			json_get_values r1kh r1kh

			set_default mobility_domain "4f57"
			set_default r0_key_lifetime 10000
			set_default r1_key_holder "00004f577274"
			set_default reassociation_deadline 1000
			set_default pmk_r1_push 0
			set_default ft_over_ds 1

			append bss_conf "mobility_domain=$mobility_domain" "$N"
			append bss_conf "r0_key_lifetime=$r0_key_lifetime" "$N"
			append bss_conf "r1_key_holder=$r1_key_holder" "$N"
			append bss_conf "reassociation_deadline=$reassociation_deadline" "$N"
			append bss_conf "pmk_r1_push=$pmk_r1_push" "$N"
			append bss_conf "ft_over_ds=$ft_over_ds" "$N"

			for kh in $r0kh; do
				append bss_conf "r0kh=${kh//,/ }" "$N"
			done
			for kh in $r1kh; do
				append bss_conf "r1kh=${kh//,/ }" "$N"
			done
		fi

		append bss_conf "wpa_disable_eapol_key_retries=$wpa_disable_eapol_key_retries" "$N"

		hostapd_append_wpa_key_mgmt
		[ -n "$wpa_key_mgmt" ] && append bss_conf "wpa_key_mgmt=$wpa_key_mgmt" "$N"
	fi

	if [ "$wpa" -ge "2" ]; then
		if [ -n "$network_bridge" -a "$rsn_preauth" = 1 ]; then
			set_default auth_cache 1
			append bss_conf "rsn_preauth=1" "$N"
			append bss_conf "rsn_preauth_interfaces=$network_bridge" "$N"
		else
			case "$auth_type" in
			sae|psk-sae|owe)
				set_default auth_cache 1
			;;
			*)
				set_default auth_cache 0
			;;
			esac
		fi

		append bss_conf "okc=$auth_cache" "$N"
		# Don't write parameter "disable_pmksa_caching" if dpp_auth_enabled==1
		[ "$auth_cache" = 0 -a "$dpp_auth_enabled" -ne 1 ] && append bss_conf "disable_pmksa_caching=1" "$N"

		# RSN -> allow management frame protection
		case "$ieee80211w" in
			[012])
				json_get_vars ieee80211w_max_timeout ieee80211w_retry_timeout beacon_protection_enabled disable_bigtk_rekey group_mgmt_cipher
				append bss_conf "ieee80211w=$ieee80211w" "$N"
				[ "$ieee80211w" -gt "0" ] && {
					[ -n "$ieee80211w_max_timeout" ] && \
						append bss_conf "assoc_sa_query_max_timeout=$ieee80211w_max_timeout" "$N"
					[ -n "$ieee80211w_retry_timeout" ] && \
						append bss_conf "assoc_sa_query_retry_timeout=$ieee80211w_retry_timeout" "$N"
					[ -n "$beacon_protection_enabled" ] && \
						append bss_conf "beacon_protection_enabled=$beacon_protection_enabled" "$N"
					[ -n "$disable_bigtk_rekey" ] && \
						append bss_conf "disable_bigtk_rekey=$disable_bigtk_rekey" "$N"
					[ -n "$group_mgmt_cipher" ] && \
						append bss_conf "group_mgmt_cipher=$group_mgmt_cipher" "$N"
				}
			;;
		esac
	fi

	_macfile="/var/run/hostapd-$ifname.maclist"
	case "$macfilter" in
		allow)
			append bss_conf "macaddr_acl=1" "$N"
			append bss_conf "accept_mac_file=$_macfile" "$N"
			# accept_mac_file can be used to set MAC to VLAN ID mapping
			vlan_possible=1
		;;
		deny)
			append bss_conf "macaddr_acl=0" "$N"
			append bss_conf "deny_mac_file=$_macfile" "$N"
		;;
		*)
			_macfile=""
		;;
	esac

	[ -n "$_macfile" ] && {
		json_get_vars macfile
		json_get_values maclist maclist

		rm -f "$_macfile"
		(
			for mac in $maclist; do
				echo "$mac"
			done
			[ -n "$macfile" -a -f "$macfile" ] && cat "$macfile"
		) > "$_macfile"
	}

	[ -n "$vlan_possible" -a -n "$dynamic_vlan" ] && {
		json_get_vars vlan_naming vlan_tagged_interface vlan_bridge vlan_file
		set_default vlan_naming 1
		append bss_conf "dynamic_vlan=$dynamic_vlan" "$N"
		append bss_conf "vlan_naming=$vlan_naming" "$N"
		[ -n "$vlan_bridge" ] && \
			append bss_conf "vlan_bridge=$vlan_bridge" "$N"
		[ -n "$vlan_tagged_interface" ] && \
			append bss_conf "vlan_tagged_interface=$vlan_tagged_interface" "$N"
		[ -n "$vlan_file" ] && {
			[ -e "$vlan_file" ] || touch "$vlan_file"
			append bss_conf "vlan_file=$vlan_file" "$N"
		}
	}

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		debug_infrastructure_append debug_hostap_conf_ bss_conf
	fi

	append "$var" "$bss_conf" "$N"
	return 0
}

hostapd_set_log_options() {
	local var="$1"

	local log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme
	json_get_vars log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme

	set_default log_level 2
	set_default log_80211  1
	set_default log_8021x  1
	set_default log_radius 1
	set_default log_wpa    1
	set_default log_driver 1
	set_default log_iapp   1
	set_default log_mlme   1

	local log_mask=$(( \
		($log_80211  << 0) | \
		($log_8021x  << 1) | \
		($log_radius << 2) | \
		($log_wpa    << 3) | \
		($log_driver << 4) | \
		($log_iapp   << 5) | \
		($log_mlme   << 6)   \
	))

	append "$var" "logger_syslog=$log_mask" "$N"
	append "$var" "logger_syslog_level=$log_level" "$N"
	append "$var" "logger_stdout=$log_mask" "$N"
	append "$var" "logger_stdout_level=$log_level" "$N"

	return 0
}

_wpa_supplicant_common() {
	local ifname="$1"

	_rpath="/var/run/wpa_supplicant"
	_config="${_rpath}-$ifname.conf"
	_pid_file="${_rpath}-$ifname.pid"
}

wpa_supplicant_teardown_interface() {
	_wpa_supplicant_common "$1"
	rm -rf "$_rpath/$1" "$_config" "$_pid_file"
}

wpa_supplicant_prepare_interface() {
	local ifname="$1"
	_w_driver="$2"

	_wpa_supplicant_common "$1"

	json_get_vars mode wds vendor_elems wps_manufacturer wps_device_name wps_device_type \
		wps_cred_processing wps_os_version wps_model_name wps_model_number wps_serial_number \
		wps_pushbutton wps_keypad wps_label wps_virtual_pushbutton wps_physical_pushbutton \
		wps_virtual_display wps_physical_display multi_ap_profile pmf

	set_default wps_pushbutton 0
	set_default wps_keypad 0
	set_default wps_label 0
	set_default wps_virtual_pushbutton 0
	set_default wps_physical_pushbutton 0
	set_default wps_virtual_display 0
	set_default wps_physical_display 0

	[ -n "$network_bridge" ] && {
		fail=
		case "$mode" in
			adhoc)
				fail=1
			;;
			sta)
				[ "$wds" = 1 ] || fail=1
			;;
		esac

		[ -n "$fail" ] && {
			wireless_setup_vif_failed BRIDGE_NOT_ALLOWED
			return 1
		}
	}

	local ap_scan=

	_w_mode="$mode"
	_w_modestr=

	[[ "$mode" = adhoc ]] && {
		ap_scan="ap_scan=2"

		_w_modestr="mode=1"
	}

	local country_str=
	[ -n "$country" ] && {
		country_str="country=$country"
	}

	local vendor_elems_str=
	[ -n "$vendor_elems" ] && {
		vendor_elems_str="vendor_elems=$vendor_elems"
	}

	local wds_str=
	[ -n "$wds" ] && {
		wds_str="wds=$wds"
	}

	local multi_ap_profile_str=
	[ -n "$multi_ap_profile" ] && {
		multi_ap_profile_str="multi_ap_profile=$multi_ap_profile"
	}

	local pmf_str=
	[ -n "$pmf" ] && {
		pmf_str="pmf=$pmf"
	}

	wpa_supplicant_teardown_interface "$ifname"
	cat > "$_config" <<EOF
$ap_scan
$country_str
$vendor_elems_str
$wds_str
$multi_ap_profile_str
$pmf_str
EOF

	config_methods=
	[ "$wps_pushbutton" -gt 0 ] && append config_methods push_button
	[ "$wps_label" -gt 0 ] && append config_methods label
	[ "$wps_keypad" -gt 0 ] && append config_methods keypad
	[ "$wps_virtual_pushbutton" -gt 0 ] && append config_methods virtual_push_button
	[ "$wps_physical_pushbutton" -gt 0 ] && append config_methods physical_push_button
	[ "$wps_virtual_display" -gt 0 ] && append config_methods virtual_display
	[ "$wps_physical_display" -gt 0 ] && append config_methods physical_display

	[ -n "$config_methods" ] && {
		set_default wps_manufacturer "Intel Corporation"
		set_default wps_device_name "WLAN-REPEATER"
		set_default wps_device_type "6-0050F204-1"
		set_default wps_cred_processing 1
		set_default wps_os_version "01020300"

		[ -n "$wps_manufacturer" ] && {
			echo "manufacturer=$wps_manufacturer" >> "$_config"
		}

		[ -n "$wps_device_name" ] && {
			echo "device_name=$wps_device_name" >> "$_config"
		}

		[ -n "$wps_device_type" ] && {
			echo "device_type=$wps_device_type" >> "$_config"
		}

		[ -n "$wps_cred_processing" ] && {
			echo "wps_cred_processing=$wps_cred_processing" >> "$_config"
		}

		[ -n "$wps_os_version" ] && {
			echo "os_version=$wps_os_version" >> "$_config"
		}

		[ -n "$wps_model_name" ] && {
			echo "model_name=$wps_model_name" >> "$_config"
		}

		[ -n "$wps_model_number" ] && {
			echo "model_number=$wps_model_number" >> "$_config"
		}

		[ -n "$wps_serial_number" ] && {
			echo "serial_number=$wps_serial_number" >> "$_config"
		}

		echo "config_methods=$config_methods" >> "$_config"
	}

	return 0
}

wpa_supplicant_add_network() {
	local ifname="$1"

	_wpa_supplicant_common "$1"
	wireless_vif_parse_encryption

	json_get_vars \
		ssid bssid key \
		basic_rate mcast_rate \
		ieee80211w ieee80211r

	set_default ieee80211r 0

	local key_mgmt='NONE'
	local enc_str=
	local network_data=
	local T="	"

	local scan_ssid="scan_ssid=1"
	local freq wpa_key_mgmt

	[[ "$_w_mode" = "adhoc" ]] && {
		append network_data "mode=1" "$N$T"
		[ -n "$channel" ] && {
			freq="$(get_freq "$phy" "$channel")"
			append network_data "fixed_freq=1" "$N$T"
			append network_data "frequency=$freq" "$N$T"
		}

		scan_ssid="scan_ssid=0"

		[ "$_w_driver" = "nl80211" ] ||	append wpa_key_mgmt "WPA-NONE"
	}

	[[ "$_w_mode" = "mesh" ]] && {
		json_get_vars mesh_id
		ssid="${mesh_id}"

		append network_data "mode=5" "$N$T"
		[ -n "$channel" ] && {
			freq="$(get_freq "$phy" "$channel")"
			append network_data "frequency=$freq" "$N$T"
		}
		append wpa_key_mgmt "SAE"
		scan_ssid=""
	}

	[ "$_w_mode" = "adhoc" -o "$_w_mode" = "mesh" ] && append network_data "$_w_modestr" "$N$T"

	[ "$dpp_auth_enabled" -eq 1 ] && hostapd_append_wpa_key_mgmt

	case "$auth_type" in
		none) ;;
		owe)
			hostapd_append_wpa_key_mgmt
			key_mgmt="$wpa_key_mgmt"
		;;
		wep)
			local wep_keyidx=0
			hostapd_append_wep_key network_data
			append network_data "wep_tx_keyidx=$wep_keyidx" "$N$T"
		;;
		psk|sae|psk-sae)
			local passphrase

			if [ "$_w_mode" != "mesh" ]; then
				hostapd_append_wpa_key_mgmt
			fi

			key_mgmt="$wpa_key_mgmt"

			if [ ${#key} -eq 64 ]; then
				passphrase="psk=${key}"
			else
				passphrase="psk=\"${key}\""
			fi
			append network_data "$passphrase" "$N$T"
		;;
		eap|eap192|eap-eap192)
			hostapd_append_wpa_key_mgmt
			key_mgmt="$wpa_key_mgmt"

			json_get_vars eap_type identity anonymous_identity ca_cert
			[ -n "$ca_cert" ] && append network_data "ca_cert=\"$ca_cert\"" "$N$T"
			[ -n "$identity" ] && append network_data "identity=\"$identity\"" "$N$T"
			[ -n "$anonymous_identity" ] && append network_data "anonymous_identity=\"$anonymous_identity\"" "$N$T"
			case "$eap_type" in
				tls)
					json_get_vars client_cert priv_key priv_key_pwd
					append network_data "client_cert=\"$client_cert\"" "$N$T"
					append network_data "private_key=\"$priv_key\"" "$N$T"
					append network_data "private_key_passwd=\"$priv_key_pwd\"" "$N$T"
				;;
				fast|peap|ttls)
					json_get_vars auth password ca_cert2 client_cert2 priv_key2 priv_key2_pwd
					set_default auth MSCHAPV2

					if [ "$auth" = "EAP-TLS" ]; then
						[ -n "$ca_cert2" ] &&
							append network_data "ca_cert2=\"$ca_cert2\"" "$N$T"
						append network_data "client_cert2=\"$client_cert2\"" "$N$T"
						append network_data "private_key2=\"$priv_key2\"" "$N$T"
						append network_data "private_key2_passwd=\"$priv_key2_pwd\"" "$N$T"
					else
						append network_data "password=\"$password\"" "$N$T"
					fi

					phase2proto="auth="
					case "$auth" in
						"auth"*)
							phase2proto=""
						;;
						"EAP-"*)
							auth="$(echo $auth | cut -b 5- )"
							[ "$eap_type" = "ttls" ] &&
								phase2proto="autheap="
						;;
					esac
					append network_data "phase2=\"$phase2proto$auth\"" "$N$T"
				;;
			esac
			append network_data "eap=$(echo $eap_type | tr 'a-z' 'A-Z')" "$N$T"
		;;
	esac

	[ "$mode" = mesh ] || {
		case "$wpa" in
			1)
				append network_data "proto=WPA" "$N$T"
			;;
			2)
				append network_data "proto=RSN" "$N$T"
			;;
		esac

		case "$ieee80211w" in
			[012])
				[ "$wpa" -ge 2 ] && append network_data "ieee80211w=$ieee80211w" "$N$T"
			;;
		esac
	}
	local beacon_int brates mrate
	[ -n "$bssid" ] && append network_data "bssid=$bssid" "$N$T"

	local bssid_blacklist bssid_whitelist
	json_get_values bssid_blacklist bssid_blacklist
	json_get_values bssid_whitelist bssid_whitelist

	[ -n "$bssid_blacklist" ] && append network_data "bssid_blacklist=$bssid_blacklist" "$N$T"
	[ -n "$bssid_whitelist" ] && append network_data "bssid_whitelist=$bssid_whitelist" "$N$T"

	[ -n "$basic_rate" ] && {
		local br rate_list=
		for br in $basic_rate; do
			wpa_supplicant_add_rate rate_list "$br"
		done
		[ -n "$rate_list" ] && append network_data "rates=$rate_list" "$N$T"
	}

	[ -n "$mcast_rate" ] && {
		local mc_rate=
		wpa_supplicant_add_rate mc_rate "$mcast_rate"
		append network_data "mcast_rate=$mc_rate" "$N$T"
	}

	local ht_str
	[[ "$_w_mode" = adhoc ]] || ibss_htmode=
	[ -n "$ibss_htmode" ] && append network_data "htmode=$ibss_htmode" "$N$T"

	[ -n "$ssid" ] && {
		cat >> "$_config" <<EOF
network={
	$scan_ssid
	ssid="$ssid"
	key_mgmt=$key_mgmt
	$network_data
}
EOF
	}
	return 0
}

wpa_supplicant_run() {
	local ifname="$1"; shift

	_wpa_supplicant_common "$ifname"

	[ -f "$_pid_file" ] && [ kill `cat "$_pid_file"` >/dev/null 2>&1 ]
	kill `ps -w | grep wpa_supplicant | grep ${ifname} | awk '{print $1;}' ` >/dev/null 2>&1

	/usr/sbin/wpa_supplicant -s -B \
		${network_bridge:+-b $network_bridge} \
		-P "$_pid_file" \
		-D ${_w_driver:-wext} \
		-i "$ifname" \
		-c "$_config" \
		-C "$_rpath" \
		"$@"

	ret="$?"

	if [ "$ret" != 0 ]; then
		wireless_setup_vif_failed WPA_SUPPLICANT_FAILED
		return $ret
	fi

	retry_count=0
	wpa_supplicant_pid=
	until [ $retry_count -ge 5 ]
	do
		wpa_supplicant_pid=`cat $_pid_file`
		if [ -n "$wpa_supplicant_pid" ]; then
			break;
		fi
		retry_count=$((retry_count+1))
		sleep 1
	done

	if [ -n "$wpa_supplicant_pid" ]; then
		wireless_add_process "$wpa_supplicant_pid" /usr/sbin/wpa_supplicant 1
	else
		wireless_setup_vif_failed WPA_SUPPLICANT_FAILED
		ret=1
	fi

	return $ret
}

hostapd_common_cleanup() {
	killall hostapd wpa_supplicant meshd-nl80211
}
