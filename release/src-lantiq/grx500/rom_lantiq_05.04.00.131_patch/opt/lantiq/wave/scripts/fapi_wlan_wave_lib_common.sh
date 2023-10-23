#!/tmp/wireless/lantiq/bin/sh

. /opt/lantiq/wave/scripts/fapi_wlan_wave_hostapd_debug_functions.sh

# Common paths and files names
export PATH=/tmp/wireless/lantiq/bin/:$PATH
export TEMP_DIR=/tmp/wlan_wave
## Support different paths for Puma and UGW
if [ -d /opt/lantiq ]
then
	export CONF_DIR=/opt/lantiq/wave/confs
	export ETC_PATH=/opt/lantiq/wave/scripts
	export BINDIR=/opt/lantiq/bin
	export SBINDIR=/opt/lantiq/sbin
	linux_ver=`uname -r`
	linux_ver=${linux_ver%%-*}
	export DRIVER_PATH=/opt/lantiq/lib/modules/${linux_ver}/net
	export IMAGES_PATH=/opt/lantiq/wave/images
	export FAPI_RPC=/opt/lantiq/wave/fapi_rpc_mode
else
	export CONF_DIR=/etc/wave/confs
	export ETC_PATH=/etc/wave/scripts
	export BINDIR=/usr/sbin
	export SBINDIR=/usr/sbin
	export DRIVER_PATH=/lib/modules/kernel/net
	export IMAGES_PATH=/etc/wave/bins/
	export FAPI_RPC=/nvram/etc/fapi_rpc_mode
fi
export GENL_FAMILY_ID_FILE=${CONF_DIR}/mtlk_genl_family_id_file
export RESTART_FLAG=fapi_wlan_wave_need_restart
export RESTART_DRVHLPR_FLAG=fapi_wlan_wave_restart_drvhlpr
export HOSTAPD_EVENTS_SCRIPT=${ETC_PATH}/fapi_wlan_wave_events_hostapd.sh
export SUPPLICANT_EVENTS_SCRIPT=${ETC_PATH}/fapi_wlan_wave_events_supplicant.sh
export INIT_FLAG=${TEMP_DIR}/fapi_wlan_wave_init_in_progress
export USE_CURRENT_CONF=${CONF_DIR}/fapi_wlan_wave_use_current_conf
export INIT_COMPLETE_RECOVERY_FLAG=${CONF_DIR}/fapi_wlan_wave_complete_recovery_in_progress
export RDKBOS_WIFI_UTIL=/etc/utopia/service.d/wifi_util.sh
export REG_DOMAIN_SET_FLAG=${CONF_DIR}/fapi_wlan_wave_reg_domain_changed

# Configuration files defines
export HOSTAPD_PHY_CONF_PREFIX=hostapd_phy
export HOSTAPD_VAP_CONF_PREFIX=hostapd_vap
export HOSTAPD_ATF_RADIO_CONF_PREFIX=hostapd_atf
export HOSTAPD_ATF_CONF_PREFIX=hostapd_vap_atf
export HOSTAPD_SSID_ATF_CONF_PREFIX=hostapd_ssid_atf
export HOSTAPD_ATF_GENERAL_CONF_PREFIX=hostapd_atf_general
export DRIVER_PRE_UP_CONF_PREFIX=drv_config_pre_up
export DRIVER_PRE_UP_CONFIGURED_FLAG=${TEMP_DIR}/fapi_wlan_wave_pre_up_configured
export DRIVER_POST_UP_CONF_PREFIX=drv_config_post_up
export DRIVER_SINGLE_CALL_CONFIG_FILE=drv_config_single_call
export OTF_CONFIG_FILE=fapi_wlan_wave_otf_config.conf
export SUPPLICANT_CONF_PREFIX=wpa_supplicant
export SUPPLICANT_CONFIGURATION_CONF_PREFIX=wpa_supplicant_configuration
export SUPPLICANT_PROFILE_CONF_PREFIX=supplicant_profile
export FAPI_WLAN_WAVE_RUNNNER=fapi_wlan_wave_runner.sh
export CRDA_FLAG=${TEMP_DIR}/crda_executed
export IN_CONF=${TEMP_DIR}/fapi_wlan_wave_in.conf
export OUT_CONF=${TEMP_DIR}/fapi_wlan_wave_out.conf
export RECOVERY_SCRIPT_PATH=${ETC_PATH}/fapi_wlan_wave_fw_recovery_notify
export RECOVERY_CONF=${CONF_DIR}/fapi_wlan_wave_recovery_conf
export RADIO_CONF=${CONF_DIR}/fapi_wlan_wave_radio_conf
export LOGGER_FW_CONF=${CONF_DIR}/fapi_wlan_wave_logger_fw_conf
export LOGGER_DRIVER_CONF=${CONF_DIR}/fapi_wlan_wave_logger_driver_conf
export LOGGER_COMMANDS_CONF=${CONF_DIR}/fapi_wlan_wave_logger_commands_conf
export LOGGER_CONFIGURED_FLAG=${TEMP_DIR}/fapi_wlan_wave_logger_configured
export RADIO_INIT_CONF=${CONF_DIR}/fapi_wlan_wave_init_radio_conf
export SSID_CONF=${CONF_DIR}/fapi_wlan_wave_ssid_conf
export ACCESSPOINT_CONF=${CONF_DIR}/fapi_wlan_wave_access_point_conf
export SECURITY_CONF=${CONF_DIR}/fapi_wlan_wave_security_conf
export WPS_CONF=${CONF_DIR}/fapi_wlan_wave_wps_conf
export WMM_BE_CONF=${CONF_DIR}/fapi_wlan_wave_wmm_be_conf
export WMM_BK_CONF=${CONF_DIR}/fapi_wlan_wave_wmm_bk_conf
export WMM_VI_CONF=${CONF_DIR}/fapi_wlan_wave_wmm_vi_conf
export WMM_VO_CONF=${CONF_DIR}/fapi_wlan_wave_wmm_vo_conf
export HS20_CONF=${CONF_DIR}/fapi_wlan_wave_hs20_conf
export HS20_CONF_ROAM=${CONF_DIR}/fapi_wlan_wave_hs20_roam_conf
export HS20_CONF_OP_FRIEND_NAME=${CONF_DIR}/fapi_wlan_wave_hs20_opfriend_name_conf
export HS20_CONF_VENUE_NAME=${CONF_DIR}/fapi_wlan_wave_hs20_venue_name_conf
export HS20_CONF_CAPAB_NAME=${CONF_DIR}/fapi_wlan_wave_hs20_capab_conf
export HS20_CONF_NAIREALM=${CONF_DIR}/fapi_wlan_wave_hs20_nairealm_conf
export HS20_CONF_WAN_METRICS=${CONF_DIR}/fapi_wlan_wave_hs20_wan_metrics_conf
export HS20_CONF_OSU_ICONS=${CONF_DIR}/fapi_wlan_wave_hs20_osu_icons_conf
export HS20_CONF_OSU_PROVIDE=${CONF_DIR}/fapi_wlan_wave_hs20_osu_provide_conf
export HS20_CONF_L2F_FW=${CONF_DIR}/fapi_wlan_wave_hs20_l2ffw_conf
export ENDPOINT_CONF=${CONF_DIR}/fapi_wlan_wave_endpoint_conf
export ENDPOINT_WPS_CONF=${CONF_DIR}/fapi_wlan_wave_endpoint_wps_conf
export ENDPOINT_SCAN_COMPLETED_EVENT=${CONF_DIR}/fapi_wlan_wave_endpoint_scan_completed
export PROFILE_CONF=${CONF_DIR}/fapi_wlan_wave_profile_conf
export STA_LIMITS_CONF=${CONF_DIR}/fapi_wlan_wave_sta_limits_conf
export PPA_STATUS_CONF=${CONF_DIR}/fapi_wlan_wave_ppa_status
export VAPS_LIST=${CONF_DIR}/fapi_wlan_wave_vaps
export DELETED_VAPS_LIST=${TEMP_DIR}/fapi_wlan_wave_deleted_vaps
export INTERFACES_INDEXES=${CONF_DIR}/fapi_wlan_wave_indexes
export CONF_IN_PROGRESS=${CONF_DIR}/fapi_wlan_wave_conf_in_progress
export INTERFACES_STATUS=${CONF_DIR}/fapi_wlan_wave_interfaces_status
export ENABLE_ONLINE_STATUS=${CONF_DIR}/fapi_wlan_wave_enable_online
export PUMA_NOTIFICATION_CONF=${CONF_DIR}/wlan_notification
export PROFILE_REFERENCE_FLAG=${CONF_DIR}/fapi_wlan_wave_profile_reference
export PROFILE_SECURITY_FLAG=${CONF_DIR}/fapi_wlan_wave_profile_security
export PROFILE_FLAG=${CONF_DIR}/fapi_wlan_wave_profile
export ALUMNUS_HS20_CONFIG_FILE=alumnus_hs20_config
export HS20_CONF_L2F_FW_RULES=${CONF_DIR}/l2f_rules
export CONNECT_FLAG=${CONF_DIR}/fapi_wlan_wave_connect
export REPEATER_FLAG=${CONF_DIR}/fapi_wlan_wave_repeater_mode
export ALIAS_ID_FILE=${TEMP_DIR}/fapi_wlan_profile_id
export REMOVE_RADIO_CONF=${TEMP_DIR}/fapi_wlan_wave_remove_radio_conf
export ENABLE_HOSTAPD=${CONF_DIR}/fapi_wlan_wave_enable_hostapd
export RADIO_UP_CONF=${CONF_DIR}/fapi_wlan_wave_radio_up
export DRVHLPR_UP_CONF=${CONF_DIR}/fapi_wlan_wave_drvhlpr_up
export ENDPOINT_UP_CONF=${CONF_DIR}/fapi_wlan_wave_endpoint_up
export WAIT_HOSTAPD_CONF=${CONF_DIR}/fapi_wlan_wave_wait_hostapd
export HS20_UP_CONF=${CONF_DIR}/fapi_wlan_wave_hs20_up
export WAVE_DELETE_CONFS_FLAG=${TEMP_DIR}/fapi_wlan_wave_delete_confs

# Scripts debug parameters
export debug_save_conf=1
export debug_save_runner=1

# Common parameters defines
export AP=0
export VAP=1
export STA=2

export FREQ_24G=0
export FREQ_5G=1
export FREQ_BOTH=2

export MODE_11BG=0
export MODE_11A=1
export MODE_11B=2
export MODE_11G=3
export MODE_11N=4
export MODE_11BGN=5
export MODE_11GN=6
export MODE_11AN=7
export MODE_11AC=8
export MODE_11NAC=9
export MODE_11ANAC=10

export CH_WIDTH_20=0
export CH_WIDTH_40=1
export CH_WIDTH_AUTO=2
export CH_WIDTH_80=3

export SECONDARY_CHANNEL_UPPER=0
export SECONDARY_CHANNEL_LOWER=1

export ACL_ACCEPT=0
export ACL_DENY=1
export ACL_DISABLED=2
export ACCEPT_ACL_FILE=hostapd.accept
export DENY_ACL_FILE=hostapd.deny

export DB_OPEN="None"
export DB_WEP64="WEP-64"
export DB_WEP128="WEP-128"
export DB_WPA_TKIP_PERSONAL="WPA-Personal"
export DB_WPA2_CCMP_PERSONAL="WPA2-Personal"
export DB_WPA2_CCMP_ONLY_PERSONAL="WPA2-CCMP-Only-Personal"
export DB_WPA_MIXED_PERSONAL="WPA-WPA2-Personal"
export DB_WPA_TKIP_ENTERPRISE="WPA-Enterprise"
export DB_WPA2_CCMP_ENTERPRISE="WPA2-Enterprise"
export DB_WPA_MIXED_ENTERPRISE="WPA-WPA2-Enterprise"

export HOSTAPD_WPA=1
export HOSTAPD_WPA2=2
export HOSTAPD_WPA_MIXED=3

export WEP_ASCII=0
export WEP_HEX=1

export PMF_DISABLED=0
export PMF_ENABLED=1
export PMF_REQUIRED=2

export WPS_DISABLED=0
export WPS_ENABLED_NOT_CONFIGURED=1
export WPS_ENABLED_CONFIGURED=2

export ENDPOINT_SCANNING="Scanning"
export ENDPOINT_SCAN_DONE="ScanDone"

export ENDPOINT_CONNECT="connect"
export ENDPOINT_DISCONNECT="disconnect"
export ENDPOINT_RECONNECT="reconnect"

export ENDPOINT_CONNECTION_TIMEOUT=30
export ENDPOINT_CONNECTION_TIMEOUT_INIT=60

export WDS_DISABLED="Disabled"
export WDS_ENABLED="Legacy"
export WDS_HYBRID="Hybrid"

# Logger parameters
export LOGGER_FW=0
export LOGGER_DRIVER=1
export LOGGER_CONFIGURATIONS=2
export LOGGER_HOSTAPD=3
export LOGGER_PROC=/proc/net/mtlk_log/rtlog
export LOGGER_LAN=0
export LOGGER_WAN=1

# HS2.0 parameters:
export HS20_MODE_DISABLED=0
export HS20_MODE_ENABLED=1
export HS20_MODE_OSEN=2
export PARP_CTRL_SCRIPT="${ETC_PATH}/wave_wifi_parp_ctrl.sh"
export DGAF_DISABLE_SCRIPT="${ETC_PATH}/wave_wifi_dgaf_disable.sh"
export WMDCTRL_SCRIPT="${ETC_PATH}/wmdctrl.sh"
export HAIRPIN_CONFIG_SCRIPT="${ETC_PATH}/wave_wifi_hairpin_config.sh"
export L2F_CTRL_SCRIPT="${ETC_PATH}/wave_wifi_l2f_ctrl.sh"
export HS20_COMMANDS_SAVE="/tmp/wave_wifi_hs20_save.sh"

# TR181 objects names
export TR181_VENDOR="X_LANTIQ_COM_Vendor"
export WIFI_OBJECT="Device.WiFi"
export RADIO_OBJECT="Device.WiFi.Radio"
export RADIO_VENDOR_OBJECT="${RADIO_OBJECT}.${TR181_VENDOR}"
export RADIO_VENDOR_DRIVER_LOGGER_OBJECT="${RADIO_VENDOR_OBJECT}.WaveDriverStream"
export RADIO_VENDOR_CONFIGURATION_LOGGER_OBJECT="${RADIO_VENDOR_OBJECT}.WaveConfigurationStream"
export RADIO_VENDOR_HOSTAPD_LOGGER_OBJECT="${RADIO_VENDOR_OBJECT}.WaveHostapdStream"
export RADIO_VENDOR_FW_LOGGER_OBJECT="${RADIO_VENDOR_OBJECT}.WaveFwStream"
export RADIO_STATS_OBJECT="Device.WiFi.Radio.Stats"
export NEIGHBORING_OBJECT="Device.WiFi.NeighboringWiFiDiagnostic"
export NEIGHBORING_RESULT_OBJECT="Device.WiFi.NeighboringWiFiDiagnostic.Result"
export SSID_OBJECT="Device.WiFi.SSID"
export SSID_STATS_OBJECT="Device.WiFi.SSID.Stats"
export ACCESSPOINT_OBJECT="Device.WiFi.AccessPoint"
export ACCESSPOINT_VENDOR_OBJECT="${ACCESSPOINT_OBJECT}.${TR181_VENDOR}"
export ACCESSPOINT_SECURITY_OBJECT="Device.WiFi.AccessPoint.Security"
export ACCESSPOINT_SECURITY_VENDOR_OBJECT="${ACCESSPOINT_VENDOR_OBJECT}.Security"
export ACCESSPOINT_WPS_OBJECT="Device.WiFi.AccessPoint.WPS"
#export ACCESSPOINT_WPS_VENDOR_OBJECT="${ACCESSPOINT_VENDOR_OBJECT}.WPS"
export RADIO_WPS_VENDOR_OBJECT="${RADIO_VENDOR_OBJECT}.WPS"
export ACCESSPOINT_HS20_VENDOR_OBJECT="${ACCESSPOINT_VENDOR_OBJECT}.HS20"
export ACCESSPOINT_HS20_L2FW_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.L2Firewall"
export ACCESSPOINT_HS20_VENUENAME_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.VenueName"
export ACCESSPOINT_HS20_ROAMING_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.RoamingConsortium"
export ACCESSPOINT_HS20_CONNECTIONCAPAB_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.ConnectionCapability"
export ACCESSPOINT_HS20_NAIREALM_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.NAIrealm"
export ACCESSPOINT_HS20_OPERFRIENDLYNAME_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.OperatorFriendlyName"
export ACCESSPOINT_HS20_OSUICONS_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.OSUicons"
export ACCESSPOINT_HS20_OSUPROVIDERS_OBJECT="${ACCESSPOINT_HS20_VENDOR_OBJECT}.OSUproviders"
export ACCESSPOINT_ASSOCIATED_DEVICES_OBJECT="Device.WiFi.AccessPoint.AssociatedDevice"
export ACCESSPOINT_ASSOCIATED_DEVICES_STATS_OBJECT="Device.WiFi.AccessPoint.AssociatedDevice.Stats"
export ACCESSPOINT_AC_OBJECT="Device.WiFi.AccessPoint.AC"
export ACCESSPOINT_AC_STATS_OBJECT="Device.WiFi.AccessPoint.AC.Stats"
export ENDPOINT_OBJECT="Device.WiFi.EndPoint"
export ENDPOINT_PROFILE_OBJECT="${ENDPOINT_OBJECT}.Profile"
export ENDPOINT_PROFILE_SECURITY_OBJECT="${ENDPOINT_PROFILE_OBJECT}.Security"
export DEVICE_INFO_OBJECT="Device.DeviceInfo"
export ENDPOINT_WPS_OBJECT="Device.WiFi.EndPoint.WPS"

export WPS_IN_PROGRESS_UNCONF_STATE=/tmp/wps_in_procees_in_unconfigured_state_flag
export WPS_IN_PROGRESS_EXTERNAL=/tmp/wps_in_procees_external_flag

# Print scripts logs.
# Possible log types: ERROR, WARNING, DEBUG, INFO or any other string.
# Output depends on the log level set in this file:
# 	0 = print only errors and any string that is not ERROR, WARNING, DEBUG or INFO.
#	1 = print errors, warnings and any string that is not ERROR, WARNING, DEBUG or INFO.
#	2 = print errors, warnings, debug and any string that is not ERROR, WARNING, DEBUG or INFO.
#	3 = print all outputs.
print2log()
{
	# Define local parameters
	local interface_index log_type msg log_level log_output

	interface_index=$1
	log_type=$2
	msg=$3

	[ ! "$RADIO_CONF_SOURCED" ] && local_db_source RADIO
	log_level=`db2fapi_convert regular WaveScriptsDebugLevel $interface_index`
	log_output=`db2fapi_convert regular WaveScriptsDebugOutput $interface_index`

	if [ -z "$log_level" ]
	then
		echo "WaveScriptsDebugLevel parameter is missing in DB, using value of 0" > /dev/console
		log_level=0
	fi

	if [ -z "$log_output" ]
	then
		echo "WaveScriptsDebugOutput parameter is missing in DB, printing to console" > /dev/console
		log_output="/dev/console"
	fi

	case $log_type in
	ERROR)
		[ $log_level -ge 0 ] && echo "$log_type $msg" > $log_output
	;;
	WARNING)
		[ $log_level -ge 1 ] && echo "$log_type $msg" > $log_output
	;;
	DEBUG)
		[ $log_level -ge 2 ] && echo "$log_type $msg" > $log_output
	;;
	INFO)
		[ $log_level -ge 3 ] && echo "$log_type $msg" > $log_output
	;;
	*)
		echo "$log_type $msg" > /dev/console
	;;
	esac
}

# Timestamp function for profiling.
# Results added to: /tmp/wlan_wave/wlanprofiling.log
timestamp()
{
	# Define local parameters
	local interface_index prefix profiling_debug seconds

	interface_index=$1
	prefix=$2

	[ ! "$RADIO_CONF_SOURCED" ] && local_db_source RADIO
	profiling_debug=`db2fapi_convert boolean WaveScriptsProfilingEnabled $interface_index`
	[ "$profiling_debug" = "0" ] && return

	seconds=`date +%s`
	echo ${prefix} ${seconds} >> /tmp/wlanprofiling.log
}

# Source fapi_wlan_wave_in.conf and set a flag indicating conf was sourced
in_conf_source()
{
	# Source fapi_wlan_wave_in.conf
	. ${IN_CONF}

	# Save flag indicating fapi_wlan_wave_in.conf is sourced.
	IN_CONF_SOURCED="1"
}

# Source a local DB file and set a flag indicating file was sourced
local_db_source()
{
	# Define local parameters
	local local_db_file local_db_file_name
	
	local_db_file=$1
	
	eval local_db_file_name=\${${local_db_file}_CONF}
	# Source fapi_wlan_wave_in.conf
	if [ -e $local_db_file_name ]
	then
		. ${local_db_file_name}
		# Save flag indicating the file is sourced.
		eval ${local_db_file}_CONF_SOURCED="1"
	fi
}

local_db_exist()
{
	# Define local parameters
	local local_db_file local_db_file_name
	
	local_db_file=$1
	
	eval local_db_file_name=\${${local_db_file}_CONF}
	# Source fapi_wlan_wave_in.conf
	if [ -e $local_db_file_name ]
	then
		echo 1
	else
		echo 0
	fi
}

# Write output parameters to fapi_wlan_wave_out.conf
# If a file name parameter is supplied, write to this file instead
update_conf_out()
{
	# Define local parameters
	local param value destination

	param=$1
	value="$2"
	destination=$3

	[ -z "$destination" ] && destination="${OUT_CONF}"
	echo "$param=$value" >> $destination
}

# Get the phy name in iw for the interface
find_phy_from_interface_name()
{
	# Define local parameters
	local interface_name phy_name
	
	interface_name=$1
	phy_name=`iw dev $interface_name info`
	phy_name=${phy_name##*wiphy }
	phy_name=phy${phy_name}
	echo $phy_name
}

# Get the index of an interface name in the local DB
# interfaces names are saved as wlanX_Y for wlanX.Y
find_index_from_interface_name()
{
	# Define local parameters
	local interface_name interface_index

	interface_name=$1
	interface_name=${interface_name/./_}
	. $INTERFACES_INDEXES
	eval interface_index=\${${interface_name}_index}
	echo "$interface_index"
}

# Add a new interface name to index-interfaces conf
# If the interface is already in the conf, return its index
set_index_for_interface()
{
	# Define local parameters
	local interface_name interface_index next_index

	interface_name=$1
	interface_name=${interface_name/./_}
	. $INTERFACES_INDEXES
	eval check_index=\${${interface_name}_index}
	if [ -n "$check_index" ]
	then
		interface_index="$check_index"
	else
		interface_index=$next_interface_index
		next_index=$((next_interface_index+1))
		echo "${interface_name}_index=$interface_index" >> $INTERFACES_INDEXES
		sed -i '/next_interface_index/d' $INTERFACES_INDEXES
		echo "next_interface_index=$next_index" >> $INTERFACES_INDEXES
	fi
	echo "$interface_index"
}

# Read requested DB value and driver max number of STAs value.
# If driver value is less than DB value, return the driver value for the DB to be updated.
driver_to_db_set_max_num_sta()
{
	# Define local parameters
	local radio_name interface_index value_to_set db_value driver_value

	radio_name=$1
	interface_index=$2

	value_to_set=""

	# Read value from DB
	db_value=`db2fapi_convert regular MaxAssociatedDevices $interface_index`
	# Read value from driver
	driver_value=`iwpriv $radio_name gAPCapsMaxSTAs`
	driver_value=${driver_value##w*:}
	driver_value=${driver_value%% *}
	[ "$driver_value" ] && [ "$db_value" ] && [ $driver_value -lt $db_value ] && value_to_set="$driver_value"

	echo $value_to_set
}

convert_num_ants()
{
	mask_tx_antennas=$1
	mask_tx_antennas="0x$mask_tx_antennas"
	num_ants=0
	loops="0 1 2 3"
	for i in $loops
	do
		let "shift=1<<$i"
		let "is_set=$mask_tx_antennas&$shift"
		if [ "$is_set" != "0" ]
		then
			num_ants=$((num_ants+1))
		fi
	done
	echo $num_ants
}

# Read the number of antennas supported by the driver
get_driver_num_of_antennas()
{
	# Define local parameters
	local interface_name phy_name driver_value

	interface_name=$1

	# Read value from driver
	# Get the phy name in iw for the interface
	phy_name=`find_phy_from_interface_name $interface_name`
	# Read iw info for the interface to a file and remove tabs and asterisks
	iw $phy_name info > ${CONF_DIR}/iw_info_${phy_name}
	sed -i -e 's/\t//g' -e 's/\* //' ${CONF_DIR}/iw_info_${phy_name}
	driver_value=`grep "Available Antennas" ${CONF_DIR}/iw_info_${phy_name}`
	driver_value=${driver_value##*TX 0x}
	driver_value=${driver_value:0:1}
	driver_value=`convert_num_ants $driver_value`
	rm -f ${CONF_DIR}/iw_info_${phy_name}

	echo $driver_value
}


# Find the index of a param in the fapi_wlan_wave_in.conf
# Gets 2 arguments:
# param_name: the name of the parameter to find its index
# param_value: the expected value for the searched parameter
map_param_index()
{
	# Define local parameters
	local param_name index found current_param current_value

	param_name=$1
	param_value=$2
	
	index=0
	found="no"

	tmp=${param_name}_${index}
	last_source=$tmp
	unset $last_source > /dev/null
	
	. ${IN_CONF}
	current_param=${param_name}_${index}
	eval current_value=\$$current_param
	current_value=$(printf "%b" "$current_value")
	if [ "$current_value" = "$param_value" ]; then
		found="yes"
	else
		index=$((index+1))
	fi

	while [ "$current_value" ] && [ "$found" = "no" ]; do
		current_param=${param_name}_${index}
		eval current_value=\$$current_param
		current_value=$(printf "%b" "$current_value")
		if [ "$current_value" = "$param_value" ]; then
			found="yes"
		else
			index=$((index+1))
		fi
	done

	# If parameter wasn't found, return empty value.
	[ "$found" = "no" ] && index=""

	echo $index
}

save_hw_init_out()
{
	# Define local parameters
	local interface_name interface_index param value hex_value

	interface_name=$1
	interface_index=$2
	param=$3
	value=$4

	hex_value=`ascii2hex $value`
	echo "${param}_${interface_index}=\"${hex_value}\"" >> ${RADIO_INIT_CONF}_${interface_name}
}

prepare_temp_local_db()
{
	# Define local parameters
	local tmp_local_db interface_name object_index interface_index tmp_in_conf grep_cmd current_index \
	param value

	tmp_local_db=$1
	interface_name=$2
	object_index=$3
	interface_index=$4
	tmp_in_conf=$5

	# Extract only the needed parameters and change the object index to the interface index
	grep "_${object_index}=" $tmp_in_conf > ${tmp_in_conf}_tmp
	mv -f ${tmp_in_conf}_tmp ${tmp_in_conf}
	sed -i 's/_'$object_index'/_'$interface_index'/' ${tmp_in_conf}
	# If init flag exists, only need to copy the in.conf with the correct interface index
	# If not on init, remove the modified params from the local DB and replace with new values
	if [ ! -e "$INIT_FLAG" ]
	then
		# Initiate the grep command to be used to remove params from the local DB
		grep_cmd="grep -wv \""
		# Go over the lines in the temp copy of fapi_wlan_wave_in.conf
		# Each param that belongs to the current interface:
		# 1. Is added to the grep command to remove from the local DB
		# 2. Is written to a temp file to be concatenated to the local DB
		while read line
		do
			# Check if the current line checked belongs to the current interface
			# If parameter name starts with X_LANTIQ_COM_Vendor, remove it
			current_index=${line#X_LANTIQ_COM_Vendor_}
			current_index=${current_index#*_}
			current_index=${current_index%%=*}
			[ $current_index -ne $interface_index ] && continue
			# Read the param name to remove from the local DB and add it to the grep command
			param=${line%%=*}
			value=${line##*=}
			grep_cmd=${grep_cmd}${param}\\\|
		done < $tmp_in_conf

		# Remove from tmp_local_db the needed parameters
		grep_cmd=${grep_cmd%\\\|}
		grep_cmd="${grep_cmd}\" $tmp_local_db > ${tmp_local_db}_tmp"
		eval $grep_cmd
		mv -f ${tmp_local_db}_tmp $tmp_local_db
	fi
	# Add the updated parameters to tmp_local_db
	cat $tmp_in_conf >> $tmp_local_db

	if [ -e "$INIT_FLAG" ] && [ -e "${RADIO_INIT_CONF}_${interface_name}" ] && [ "$local_db_name" = "$RADIO_CONF" ]
	then
		# On init sequence, need to save the parameters from hw_init to the radio conf file since it is updated in the DB only after init sequence is done
		# Remove the parameters saved in hw_init from the local DB and use hw_init values
		# Initiate the grep command to be used to remove params from the local DB
		grep_cmd="grep -wv \""
		while read line
		do
			param=${line%%=*}
			grep_cmd=${grep_cmd}${param}\\\|
		done < ${RADIO_INIT_CONF}_${interface_name}

		# Remove from tmp_local_db the needed parameters
		grep_cmd=${grep_cmd%\\\|}
		grep_cmd="${grep_cmd}\" $tmp_local_db > ${tmp_local_db}_tmp"
		eval $grep_cmd
		mv -f ${tmp_local_db}_tmp $tmp_local_db
		cat ${RADIO_INIT_CONF}_${interface_name} >> $tmp_local_db
		rm -f ${RADIO_INIT_CONF}_${interface_name}
	fi
}

save_db_params()
{
	# Define local parameters
	local caller_script interface_name object_index interface_index \
	local_db_name tmp_local_db tmp_in_conf

	caller_script=$1
	interface_name=$2
	object_index=$3
	interface_index=$4
	sub_object=$5

	local_db_name=""
	case "$caller_script" in
		"hw_init")
			# hw_init script generates the RADIO_CONF files for the radios detected and saves the radio vendor parameters in it
			cat $IN_CONF > $RADIO_CONF
			;;
		"radio_set")
			local_db_name="$RADIO_CONF"
			;;
		"logger_set_fw")
			local_db_name="$LOGGER_FW_CONF"
			;;
		"logger_set_driver")
			local_db_name="$LOGGER_DRIVER_CONF"
			;;
		"ssid_add"|"ssid_set")
			local_db_name="$SSID_CONF"
			;;
		"ap_set")
			local_db_name="$ACCESSPOINT_CONF"
			;;
		"security_set")
			local_db_name="$SECURITY_CONF"
			;;
		"wps_set")
			local_db_name="$WPS_CONF"
			;;
		"wmm_ap_set_be")
			local_db_name="$WMM_BE_CONF"
			;;
		"wmm_ap_set_bk")
			local_db_name="$WMM_BK_CONF"
			;;
		"wmm_ap_set_vi")
			local_db_name="$WMM_VI_CONF"
			;;
		"wmm_ap_set_vo")
			local_db_name="$WMM_VO_CONF"
			;;
		"endpoint_set")
			local_db_name="$ENDPOINT_CONF"
			;;
		"endpoint_wps_set")
			local_db_name="$ENDPOINT_WPS_CONF"
			;;
		"connect_to_ap")
			local_db_name="$PROFILE_CONF"
			;;
		"hotspot_set")
			local_db_name="$HS20_CONF"
			;;
		"hotspot_set_opfriend_name")
			local_db_name="$HS20_CONF_OP_FRIEND_NAME"_${sub_object}
			;;
		"hotspot_set_venue_name")
			local_db_name="$HS20_CONF_VENUE_NAME"_${sub_object}
			;;
		"hotspot_set_connect_capab")
			local_db_name="$HS20_CONF_CAPAB_NAME"_${sub_object}
			;;
		"hotspot_set_roam")
			local_db_name="$HS20_CONF_ROAM"_${sub_object}
			;;
		"hotspot_set_nairealm")
			local_db_name="$HS20_CONF_NAIREALM"_${sub_object}
			;;
		"hotspot_wan_metrics")
			local_db_name="$HS20_CONF_WAN_METRICS"_${sub_object}
			;;
		"hotspot_osu_icons")
			local_db_name="$HS20_CONF_OSU_ICONS"_${sub_object}
			;;
		"hotspot_osu_provide")
			local_db_name="$HS20_CONF_OSU_PROVIDE"_${sub_object}
			;;
		"hotspot_l2ffw")
			local_db_name="$HS20_CONF_L2F_FW"_${sub_object}
			;;
	esac
	if [ -n "$local_db_name" ]
	then
		[ ! -e "$local_db_name" ] && touch $local_db_name
		tmp_local_db=${local_db_name}_tmp
		tmp_in_conf=${IN_CONF}_tmp
		[ -e $local_db_name ] && cp $local_db_name $tmp_local_db
		cp $IN_CONF $tmp_in_conf
		prepare_temp_local_db $tmp_local_db $interface_name $object_index $interface_index $tmp_in_conf
		cp $tmp_local_db $local_db_name
		rm -f $tmp_local_db $tmp_in_conf
	fi
}

# save_db_params_local use otf created file (not IN_CONF).
save_db_params_local()
{
	# Define local parameters
	local in_file caller_script interface_name object_index interface_index \
	local_db_name tmp_local_db tmp_in_conf
	
	caller_script=$1
	interface_name=$2
	object_index=$3
	interface_index=$4
	sub_object=$5
	
	in_file=save_db_params_local_file
	
	local_db_name=""
	case "$caller_script" in
		"hw_init")
			# hw_init script generates the RADIO_CONF files for the radios detected and saves the radio vendor parameters in it
			cat $IN_CONF > $RADIO_CONF
			;;
		"radio_set")
			local_db_name="$RADIO_CONF"
			;;
		"logger_set_fw")
			local_db_name="$LOGGER_FW_CONF"
			;;
		"logger_set_driver")
			local_db_name="$LOGGER_DRIVER_CONF"
			;;
		"ssid_add"|"ssid_set")
			local_db_name="$SSID_CONF"
			;;
		"ap_set")
			local_db_name="$ACCESSPOINT_CONF"
			;;
		"security_set")
			local_db_name="$SECURITY_CONF"
			;;
		"wps_set")
			local_db_name="$WPS_CONF"
			;;
		"wmm_ap_set_be")
			local_db_name="$WMM_BE_CONF"
			;;
		"wmm_ap_set_bk")
			local_db_name="$WMM_BK_CONF"
			;;
		"wmm_ap_set_vi")
			local_db_name="$WMM_VI_CONF"
			;;
		"wmm_ap_set_vo")
			local_db_name="$WMM_VO_CONF"
			;;
		"endpoint_set")
			local_db_name="$ENDPOINT_CONF"
			;;
		"endpoint_wps_set")
			local_db_name="$ENDPOINT_WPS_CONF"
			;;
		"connect_to_ap")
			local_db_name="$PROFILE_CONF"
			;;
		"hotspot_set")
			local_db_name="$HS20_CONF"
			;;
		"hotspot_set_opfriend_name")
			local_db_name="$HS20_CONF_OP_FRIEND_NAME"_${sub_object}
			;;
		"hotspot_set_venue_name")
			local_db_name="$HS20_CONF_VENUE_NAME"_${sub_object}
			;;
		"hotspot_set_connect_capab")
			local_db_name="$HS20_CONF_CAPAB_NAME"_${sub_object}
			;;
		"hotspot_set_roam")
			local_db_name="$HS20_CONF_ROAM"_${sub_object}
			;;
		"hotspot_set_nairealm")
			local_db_name="$HS20_CONF_NAIREALM"_${sub_object}
			;;
		"hotspot_wan_metrics")
			local_db_name="$HS20_CONF_WAN_METRICS"_${sub_object}
			;;
		"hotspot_osu_icons")
			local_db_name="$HS20_CONF_OSU_ICONS"_${sub_object}
			;;
		"hotspot_osu_provide")
			local_db_name="$HS20_CONF_OSU_PROVIDE"_${sub_object}
			;;
		"hotspot_l2ffw")
			local_db_name="$HS20_CONF_L2F_FW"_${sub_object}
			;;
	esac
	if [ -n "$local_db_name" ]
	then
		[ ! -e "$local_db_name" ] && touch $local_db_name
		tmp_local_db=${local_db_name}_tmp
		tmp_in_conf=${TEMP_DIR}/${in_file}_tmp
		[ -e $local_db_name ] && cp $local_db_name $tmp_local_db
		cp ${TEMP_DIR}/$in_file $tmp_in_conf
		echo "save_db_params_local: dump ${TEMP_DIR}/$in_file" > /dev/console
		cat ${TEMP_DIR}/$in_file > /dev/console
		prepare_temp_local_db $tmp_local_db $interface_name $object_index $interface_index $tmp_in_conf
		cp $tmp_local_db $local_db_name
		rm -f $tmp_local_db $tmp_in_conf
	fi
	
	echo "save_db_params_local: dump /opt/lantiq/wave/confs/fapi_wlan_wave_profile_conf" > /dev/console
	cat /opt/lantiq/wave/confs/fapi_wlan_wave_profile_conf > /dev/console
}

# save_db_params_local_create_file index Object Param1 Val1 Param2 Val2 ...ParamN ValN
save_db_params_local_create_file()
{
	local num_params index object_name i j param value
	num_params=$#
	index=$1
	object_name=$2
	# Create/Overrun file:
	echo "Object_${index}=$object_name" > ${TEMP_DIR}/save_db_params_local_file
	shift 2
	i=$#
	j=0
	
	while [ $i -gt 0 ]
	do
		echo "Current parameters list: '$@'"
		param=$1
		shift
		value=$1
		shift
		echo "${param}_${index}=\"$value\"" >> ${TEMP_DIR}/save_db_params_local_file
		i=$#
		# safe loop:
		j=$((j+1))
		if [ $j -eq 50 ]
		then
			break;
		fi
	done
	
	echo "dump ${TEMP_DIR}/save_db_params_local_file:"
	cat ${TEMP_DIR}/save_db_params_local_file
}

# save_db_params_local_add_file index Param1 Val1 Param2 Val2 ...ParamN ValN
save_db_params_local_add_file()
{
	local num_params index i j param value
	
	num_params=$#
	index=$1
	# Create/Overrun file:
	touch ${TEMP_DIR}/save_db_params_local_file
	shift
	i=$#
	j=0
	
	while [ $i -gt 0 ]
	do
		echo "Current parameters list: '$@'"
		param=$1
		shift
		value=$1
		shift
		echo "${param}_${index}=\"$value\"" >> ${TEMP_DIR}/save_db_params_local_file
		i=$#
		# safe loop:
		j=$((j+1))
		if [ $j -eq 50 ]
		then
			break;
		fi
	done
	
	echo "dump ${TEMP_DIR}/save_db_params_local_file:"
	cat ${TEMP_DIR}/save_db_params_local_file
}

# Convert hex values from fapi_wlan_wave_in.conf to ascii values
db2fapi_regular_convert()
{
	# Define local parameters
	local param interface_index db_value ascii_value

	param=$1
	interface_index=$2

	eval db_value=\${${param}_${interface_index}}
	ascii_value=$(printf "%b" "$db_value")
	echo "$ascii_value"
}

# Convert hex values of boolean parameters from fapi_wlan_wave_in.conf to 1 (true) or 0 (false)
db2fapi_boolean_convert()
{
	# Define local parameters
	local param interface_index db_value ascii_db ascii_value

	param=$1
	interface_index=$2
	
	eval db_value=\${${param}_${interface_index}}
	ascii_db=$(printf "%b" "$db_value")
	# Convert to lower case
	ascii_value=`echo $ascii_db | tr '[A-Z]' '[a-z]'`
	if [ "$ascii_value" = "true" ]
	then
		ascii_value=1
	elif [ "$ascii_value" = "false" ]
	then
		ascii_value=0
	fi
	echo "$ascii_value"
}

# Read the value from the DB and don't convert to ascii
# Remove all \x
db2fapi_hex_convert()
{
	# Define local parameters
	local param interface_index db_value

	param=$1
	interface_index=$2

	eval db_value=\${${param}_${interface_index}}
	db_value=`echo $db_value | sed 's/\\\x//g'`
	echo "$db_value"
}

# Convert hex values from local DB to ascii values
# select the proper function to use according to the parameter type (regular/boolean)
db2fapi_convert()
{
	# Define local parameters
	local type param interface_index value
	
	type=$1
	param=$2
	interface_index=$3

	value=`db2fapi_${type}_convert "$param" "$interface_index"`
	echo "$value"
}

# Assistive function to convert from OperatingStandards to ieee80211n and ieee80211ac
db2fapi_convert_ieee80211()
{
	local operating_standards mode value

	operating_standards=$1
	mode=$2

	# Remove commas
	operating_standards=${operating_standards//,/}
	# Check if the requested mode is included in operating_standards
	value=0
	[ "$operating_standards" != "${operating_standards/$mode/}" ] && value=1

	echo "$value"
}

# Lock a file and perform copy operation while locked.
# TODO: find the application to lock files in 7.1 (syscfg_lock doesn't exist). Curremtly, copy withot lock.
lock_and_copy_conf_file()
{
	# Define local parameters
	local locking_file source_file destination_file

	locking_file=$1
	source_file=$2
	destination_file=$3

	# If the source file doesn't exist, create empty new destination file.
	if [ ! -e "$source_file" ]
	then
		touch $destination_file
	# Copy the file
	else
		cp $source_file $destination_file
	fi
}

# Check if the interface is in PPA.
# If the PPA is set to "remove" and the interface is in PPA, add commands to remove it.
# If the PPA is set to "add" and the interface is not in the PPA, add the commands to add it.
# The commands are written to the conf file sent as an argument.
set_ppa_commands()
{
	# Define local parameters
	local interface_name action conf_file in_lan_ppa in_wan_ppa interface_type

	interface_name=$1
	action=$2
	conf_file=$3

	# Source config.sh
	[ -e /etc/rc.d/config.sh ] && . /etc/rc.d/config.sh
	# Check if the interface is in PPA

	interface_type=`check_interface_type $interface_name`

	in_lan_ppa=`ppacmd getlan | grep "\<$interface_name with\>" -c`
	in_wan_ppa=0
	[ "$interface_type" = "$STA" ] && in_wan_ppa=`ppacmd getwan | grep "\<$interface_name with\>" -c`

	if [ "$action" = "remove" ]
	then
		if [ $in_lan_ppa -gt 0 ] || [ $in_wan_ppa -gt 0 ]
		then
			# Write commands to delete VAP from PPA to the conf file
			if [ "$CONFIG_IFX_CONFIG_CPU" != "GRX500" ] && [ "$CONFIG_IFX_CONFIG_CPU" != "GRX750" ]
			then
				echo "ppacmd getportid -i $interface_name > /dev/null" >> $conf_file
				echo "if [ \$? -eq 0 ]" >> $conf_file
				echo "then" >> $conf_file
				echo "nPortId=\`ppacmd getportid -i $interface_name | sed 's/The.* is //'\`" >> $conf_file
				echo "nPortId=\$((nPortId+4))" >> $conf_file
				echo "switch_cli IFX_ETHSW_PORT_CFG_SET nPortId=\$nPortId bLearningMAC_PortLock=0 > /dev/null 2>&1 || echo WARNING! switch CLI return error for IFX_ETHSW_PORT_CFG_SET" >> $conf_file
				echo "fi" >> $conf_file
			fi
			[ $in_lan_ppa -gt 0 ] && sed -i '/ppacmd add/d' $conf_file && echo "ppacmd dellan -i $interface_name" >> $conf_file
			[ $in_wan_ppa -gt 0 ] && sed -i '/ppacmd add/d' $conf_file && echo "ppacmd delwan -i $interface_name" >> $conf_file

		fi
	elif [ "$action" = "add" ] && [ $in_lan_ppa -eq 0 ] && [ $in_wan_ppa -eq 0 ]
	then
		# Write commands to add VAP to PPA to driver the conf file
		echo "ppacmd addlan -i $interface_name" >> $conf_file
		if [ "$CONFIG_IFX_CONFIG_CPU" != "GRX500" ] && [ "$CONFIG_IFX_CONFIG_CPU" != "GRX750" ]
		then
			echo "nPortId=\`ppacmd getportid -i $interface_name | sed 's/The.* is //'\`" >> $conf_file
			echo "nPortId=\$((nPortId+4))" >> $conf_file
			echo "switch_cli IFX_ETHSW_PORT_CFG_SET nPortId=\$nPortId bLearningMAC_PortLock=1 > /dev/null 2>&1 || echo WARNING! switch CLI return error for IFX_ETHSW_PORT_CFG_SET" >> $conf_file
		fi
	fi
}

# Check if need to write command to OTF file or mark the restart file.
check_and_write_to_otf_file()
{
	# Define local parameters
	local otf command interface_name

	otf=$1
	command=$2
	interface_name=$3
	
	if [ "$otf" = "otf" ]
	then
		echo "$command" >> ${CONF_DIR}/${OTF_CONFIG_FILE}
	else
		# Parameter is not OTF, write the radio name to the restart flag and set reconfigure flag for VAPs
		set_restart_and_reconfigure $interface_name ${interface_name%%.*}
	fi
}

# Write configuration to hostapd conf files.
set_hostapd_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	# If it is a comment, write comment to the conf file.
	[ "$command_type" = "comment" ] && echo "###$value" >> $conf_file_path && return
	# Write parameter to conf file.
	echo "$param_name=$value" >> $conf_file_path

	# check if atf command exist if not and command_type is atf then add one to OTF file.
	if [ "$command_type" = "atf" ]
	then
		if [ -e ${CONF_DIR}/${OTF_CONFIG_FILE} ]
		then
			atf_cli_exist=`check_atf_param_changed atf ${CONF_DIR}/${OTF_CONFIG_FILE}`
		else
			# file doesn't exist create one and put the command in it.
			atf_cli_exist="0"
		fi
		# ATF:adding hostapd_cli to the OTF conf file ending in the runner_up
		if [ "$atf_cli_exist" = "0" ]
		then
			echo "echo \"hostapd_cli for ATF activated\" > /dev/console" >> ${CONF_DIR}/${OTF_CONFIG_FILE}
			# hostapd_cli exist for the radios not for vaps.
			interface_name=${interface_name%%.*}
			check_and_write_to_otf_file $otf "/tmp/hostapd_cli_${interface_name} -i ${interface_name} update_atf_cfg"
			echo "ATF:OTF: hostapd_cli command to OTF file was added" >> /dev/console
		fi
	else
		# TODO: update the hostapd_cli command once hostapd OTF is available.
		check_and_write_to_otf_file $otf "#hostapd_cli $param_name $value" $interface_name
	fi
}

# Write wireless extensions API commands (iwconfig/iwpriv) to driver conf files.
set_drv_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	pre_up_prefix=${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}
	# Since driver configuration files are created per configuration, no check if line exists is needed
	# each line will be written at the end of the file.
	if [ "$command_type" = "proc" ]
	then
		echo "echo $value > $param_name" >> $conf_file_path
		check_and_write_to_otf_file $otf "echo $value > $param_name" $interface_name
	elif [ "$command_type" = "ppa" ]
	then
		# For PPA the value can be added to add the VAP to PPA or remove to delete VAP from PPA.
		# Call function to add the needed commands to the driver post_up conf file and OTF conf file.
		set_ppa_commands $interface_name $value $CONF_DIR/$OTF_CONFIG_FILE
		set_ppa_commands $interface_name $value $conf_file_path
	elif [ "$command_type" = "hs_cli" ] # TODO- change to support more hs_cli commands
	then
		echo "hs_cli AP_ISO -O $value -I $interface_name" >> $conf_file_path
		check_and_write_to_otf_file $otf "hs_cli $param_name -O $value -I $interface_name" $interface_name
	elif [ "$command_type" = "route" ]
	then
		echo "route $param_name $value " >> $conf_file_path
		check_and_write_to_otf_file $otf "route $param_name $value" $interface_name
	elif [ "$command_type" = "wpa_cli" ]
	then
		echo "wpa_cli -i${interface_name} $value" >> $conf_file_path
		check_and_write_to_otf_file $otf "wpa_cli -i${interface_name} $value" $interface_name
	else
		echo "$command_type $interface_name $param_name $value" >> $conf_file_path
		check_and_write_to_otf_file $otf "$command_type $interface_name $param_name $value" $interface_name
	fi
}

# Write configuration to drvhlpr conf file.
set_drvhlpr_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	# Write parameter to conf file.
	echo "$param_name = $value" >> $conf_file_path

	check_and_write_to_otf_file $otf "$param_name = $value" $interface_name
}

# Write configuration to the runner directly
set_runner_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	# each line will be written at the end of the file.
	if [ "$command_type" = "proc" ]
	then
		echo "echo $value > $param_name" >> $conf_file_path
	elif [ "$command_type" = "alumnus" ]
	then
		echo "$param_name $value" >> $conf_file_path
	else
		echo "$command_type $interface_name $param_name $value" >> $conf_file_path
	fi
}

# Write configuration to the alumnus file
set_alumnus_hs20_config_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	echo "$param_name $value" >> $conf_file_path
	check_and_write_to_otf_file $otf "$param_name $value" $interface_name
}

# Write configuration to wpa_supplicant conf file.
set_wpa_supplicant_configuration_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	# If it is a comment, write comment to the conf file.
	[ "$command_type" = "comment" ] && echo "###$value" >> $conf_file_path && return
	# Write parameter to conf file.
	echo "$param_name=$value" >> $conf_file_path

	check_and_write_to_otf_file $otf "#wpa_cli $param_name $value" $interface_name
}

# Write connection details to wpa_supplicant_configuration conf file.
set_supplicant_profile_param()
{
	# Define local parameters
	local conf_file_path command_type otf interface_name param_name value

	conf_file_path=$1
	command_type=$2
	otf=$3
	interface_name=$4
	param_name=$5
	value=$6

	# If the value is empty, nothing needs to be set.
	[ -z "$value" ] && return

	# If it is a text, write text to the conf file.
	[ "$param_name" = "text" ] && echo "$value" >> $conf_file_path && return
	# Write parameter to conf file.
	echo -ne "\t" >> $conf_file_path
	echo "$param_name=$value" >> $conf_file_path

	check_and_write_to_otf_file $otf "#wpa_cli $param_name $value" $interface_name
}

# Write a line in the needed configuration files.
# Parameters:
# conf_file - the configuration file type
#		hostapd_phy
#		hostapd_vap
#		wpa_supplicant_configuration
#		supplicant_profile
#		drv_config_pre_up
#		drv_config_post_up
#		drvhlpr
#		runner file name (according to the parameter WAVE_WLAN_RUNNNER)
# command_type -
# 		regular - use name=value or name = value format for the configuration file.
#		iwpriv - use iwpriv command in the driver configuration file.
# 		iwconfig - use iwconfig command in the driver configuration file.
#		proc - use proc command in the driver configuration file.
#		ppa - set the needed ppa commands in the driver configuration file.
#		hs_cli - use hs_cli command in the driver configuration file.
#		route - use route command in the driver configuration file.
#		comment - add new comment line to the hostapd configuration file.
#		alumnus - add execution line for alumnus script
#		text - add the text as-is
#		atf - handle the ATF commands.
#		wpa_cli - use wpa_cli command in the driver configuration file.
# otf - flag if the parameter is on-the-fly.
# pid - the pid used in the temporary configuration file name.
# interface_name - name of the interface.
# param_name - parameter name to add/modify
# value - value of the parameter to write.
set_conf_param()
{
	# Define local parameters
	local conf_file command_type otf pid interface_name param_name value set_conf_func_name conf_file_path

	conf_file=$1
	command_type=$2
	otf=$3
	pid=$4
	interface_name=$5
	param_name=$6
	# Using "echo" to clear leading spaces.
	value=`echo $7`

	# Get the prefix of name of the configuration before the underscore.
	set_conf_func_name="$conf_file"
	if [ "${conf_file%%_*}" = "hostapd" ] || [ "${conf_file%%_*}" = "drv" ]
	then
		set_conf_func_name=${conf_file%%_*}
	fi

	# Generate the full name of the configuration file to write to.
	conf_file_path=${CONF_DIR}/${conf_file}_${interface_name}_${pid}.conf

	# Set the parameters when the conf_file is the runner
	if [ "$conf_file" = "$WAVE_WLAN_RUNNNER" ]
	then
		set_conf_func_name="runner"
		conf_file_path=${CONF_DIR}/${WAVE_WLAN_RUNNNER}
	fi
	
	# Call the function for the current conf file.
	set_${set_conf_func_name}_param $conf_file_path $command_type $otf $interface_name "$param_name" "$value"
}

clear_wds_parameters()
{
	# Define local parameters
	local pid interface_name drv_post_up_conf_temp

	pid=$1
	interface_name=$2

	drv_post_up_conf_temp=${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}_${pid}.conf

	# Read current bridge mode
	[ -e "${ACCESSPOINT_CONF}_${interface_name}" ] && . ${ACCESSPOINT_CONF}_${interface_name}

	# If bridge mode is disabled, remove all WDS related commands from post up conf
	if [ "$sBridgeMode" = "0" ]
	then
		sed -i -e '/sPeerAPkeyIdx/d' -e '/ key /d' -e '/sAddPeerAP/d' $drv_post_up_conf_temp
	fi
}

set_wds_peer_ap_param()
{
	# Define local parameters
	local pid interface_name value driver_mac db_mac

	pid=$1
	interface_name=$2
	value=`echo $3`

	[ -e "${ACCESSPOINT_CONF}_${interface_name}" ] && . ${ACCESSPOINT_CONF}_${interface_name} > /dev/null

	# Handle remove peers
	for driver_mac in $wds_peers_list
	do
		if [ "$value" = "${value/$driver_mac/}" ]
		then
			set_conf_param $DRIVER_SINGLE_CALL_CONFIG_FILE iwpriv otf $pid $interface_name "sDelPeerAP" "$driver_mac"
			[ -e "${ACCESSPOINT_CONF}_${interface_name}" ] && sed -i '/^iwpriv '$interface_name' sAddPeerAP '$driver_mac'/d' ${ACCESSPOINT_CONF}_${interface_name}
		fi
	done

	# Handle add Peers, note that may add already exist Peer (not error)
	if [ "$value" == "" ] #use conf file if Peers not changed
	then
		value=$wds_peers_list
	fi
	
	for db_mac in $value
	do
#		if [ "$wds_peers_list" = "${wds_peers_list/$db_mac/}" ]
#		then
			set_conf_param drv_config_post_up iwpriv otf $pid $interface_name "sAddPeerAP" "$db_mac"
#		fi
	done	
}

# Calculate and update the MAC address of the interface.
update_mac_address()
{
	# Define local parameters
	local interface_name ap_type radio_name radio_index vap_index phy_offset board_mac vap_increment mac_address \
	board_mac1 board_mac23 board_mac46 suffix vap_mac4 vap_mac5 vap_mac6

	interface_name=$1
	ap_type=$2

	[ -e /etc/rc.d/config.sh ] && . /etc/rc.d/config.sh

	if [ "$ap_type" = "$STA" ]
	then
		radio_name=`get_radio_name_from_endpoint $interface_name`
		radio_index=`find_index_from_interface_name $radio_name`
		vap_index=1
	elif [ "$ap_type" = "$VAP" ]
	then
		# Find the radio index
		radio_name=${interface_name%%.*}
		radio_index=`find_index_from_interface_name $radio_name`
		vap_index=${interface_name##*.}
		vap_index=$((vap_index+2))
	else
		vap_index=0
	fi
	[ "$radio_name" = "wlan0" ] && phy_offset=0
	[ "$radio_name" = "wlan2" ] && phy_offset=4
	[ "$radio_name" = "wlan4" ] && phy_offset=8
	# If upgrade application doesn't exist, base on bridge MAC address
	which upgrade > /dev/null
	if [ $? -eq 0 ] && [ "$CONFIG_IFX_CONFIG_CPU" != "GRX750" ]
	then
		board_mac=`upgrade mac_get 0`
	else
		# Read the bridge name to which the radio belongs
		bridge_name=`read_bridge_from_db $radio_name $radio_index`
		board_mac=""
		if [ -n "$bridge_name" ]
		then
			[ -n "`ifconfig | grep -w ${bridge_name}`" ] && board_mac=`ifconfig ${bridge_name}`
			[ -z "${board_mac}" ] && [ ! -z "`ifconfig erouter0`" ] && board_mac=`ifconfig erouter0`
			board_mac=${board_mac##*HWaddr }
			board_mac=${board_mac%% *}
		fi
	fi
	
	# Divide the board MAC address to the first 3 bytes and the last 3 byte (which we are going to increment).
	board_mac1=0x`echo $board_mac | cut -c 1-2`
	board_mac23=`echo $board_mac | cut -c 4-8`
	board_mac46=0x`echo $board_mac | sed s/://g | cut -c 7-12`

	# Increment the last byte by the the proper incrementation according to the physical interface (wlan0/wlan2/wlan4)
	board_mac46=$((board_mac46+phy_offset))

	# If it is AP, verify MAC ends with 0 or 8 only.
	if [ "$ap_type" = "$AP" ]
	then
		suffix=$((board_mac46&7))
		if [ $suffix != 0 ]
		then
			print2log $radio_index ALERT "######################################################################################"
			print2log $radio_index ALERT "######### MAC of $radio_name is wrong. Must end with 0 or 8 ##"
			print2log $radio_index ALERT "######### Number of supported VAPs may be limited due to this error ##################"
			print2log $radio_index ALERT "######################################################################################"
		fi
	fi

	# For STA, use MAC of physical AP incremented by 1 (wlan1 increment wlan0 by 1).
	# For VAP, use MAC of physical AP incremented by the index of the interface name + 2 (wlan0.0 increment wlan0 by 2, wlan2.2 increment wlan2 by 2).
	board_mac46=$((board_mac46+$vap_index))
		
	# Generate the new MAC.
	vap_mac4=$((board_mac46/65536))
	board_mac46=$((board_mac46-vap_mac4*65536))
	vap_mac5=$((board_mac46/256))
	board_mac46=$((board_mac46-vap_mac5*256))
	vap_mac6=$board_mac46
	# If the 4th byte is greater than FF (255) set it to 00.
	[ $vap_mac4 -ge 256 ] && vap_mac4=0
		
	if [ "$interface_name" = "wlan0.0" ]
	then
		board_mac6=0x`echo $board_mac | sed s/://g | cut -c 11-12`
		vap_mac6=$((board_mac6+1))
	elif [ "$interface_name" = "wlan0.1" ]
	then
		board_mac6=0x`echo $board_mac | sed s/://g | cut -c 11-12`
		vap_mac6=$((board_mac6+2))
	elif [ "$interface_name" = "wlan0.2" ]
	then
		board_mac6=0x`echo $board_mac | sed s/://g | cut -c 11-12`
		vap_mac6=$((board_mac6+3))
	elif [ "$interface_name" = "wlan2.0" ]
	then
		board_mac6=0x`echo $board_mac | sed s/://g | cut -c 11-12`
		vap_mac6=$((board_mac6+5))
	elif [ "$interface_name" = "wlan2.1" ]
	then
		board_mac6=0x`echo $board_mac | sed s/://g | cut -c 11-12`
		vap_mac6=$((board_mac6+6))
	elif [ "$interface_name" = "wlan2.2" ]
	then
		board_mac6=0x`echo $board_mac | sed s/://g | cut -c 11-12`
		vap_mac6=$((board_mac6+7))
	fi

	mac_address=`printf '%02X:%s:%02X:%02X:%02X' $board_mac1 $board_mac23 $vap_mac4 $vap_mac5 $vap_mac6`
	print2log $radio_index DEBUG "New VAP MAC = $mac_address"
	echo "$mac_address"
}

# Write initial content to the temp conf files.
# First do safe copy of existing conf files. If hostapd/supplicant conf files are empty, write initial values to temp conf files.
write_initial_content()
{
	# Define local parameters
	local interface_name interface_type pid caller_script hostapd_phy_conf_temp hostapd_vap_conf_temp

	interface_name=$1
	interface_type=$2
	pid=$3
	caller_script=$4

	if [ "$interface_type" = "$AP" ]
	then
		# Copy existing hostapd_phy conf file to the temp conf file if the temp file doesn't exist.
		hostapd_phy_conf_temp=${CONF_DIR}/${HOSTAPD_PHY_CONF_PREFIX}_${interface_name}_${pid}.conf
		[ ! -e "$hostapd_phy_conf_temp" ] && lock_and_copy_conf_file ${CONF_DIR}/${HOSTAPD_PHY_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${HOSTAPD_PHY_CONF_PREFIX}_${interface_name}.conf $hostapd_phy_conf_temp

		# If the hostapd_phy conf file is empty, write initial values
		if [ ! -s $hostapd_phy_conf_temp ]
		then
			echo "################ Physical radio parameters ################" > $hostapd_phy_conf_temp
			set_conf_param hostapd_phy regular no_otf $pid $interface_name interface $interface_name
			set_conf_param hostapd_phy regular no_otf $pid $interface_name driver nl80211
			set_conf_param hostapd_phy regular no_otf $pid $interface_name logger_syslog_level 3
			set_conf_param hostapd_phy regular no_otf $pid $interface_name ctrl_interface /var/run/hostapd
			set_conf_param hostapd_phy regular no_otf $pid $interface_name ctrl_interface_group 0
			# ATF add atf_config_file param for each radio
			set_conf_param hostapd_phy regular no_otf $pid $interface_name "atf_config_file" ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_$interface_name.conf
		fi

		# Copy existing drvhlpr conf file to the temp conf file if the temp file doesn't exist.
		[ ! -e "${CONF_DIR}/drvhlpr_${interface_name}_${pid}.conf" ] && lock_and_copy_conf_file ${CONF_DIR}/drvhlpr_${interface_name}.conf ${CONF_DIR}/drvhlpr_${interface_name}.conf ${CONF_DIR}/drvhlpr_${interface_name}_${pid}.conf

		# If the drvhlpr conf file is empty, write initial values
		if [ ! -s ${CONF_DIR}/drvhlpr_${interface_name}_${pid}.conf ]
		then
			set_conf_param drvhlpr regular no_otf $pid $interface_name "Interface" "$interface_name"
			set_conf_param drvhlpr regular no_otf $pid $interface_name "arp_iface0" "eth0_1"
			set_conf_param drvhlpr regular no_otf $pid $interface_name "arp_iface1" "eth0_2"
			set_conf_param drvhlpr regular no_otf $pid $interface_name "recovery_script_path" "$RECOVERY_SCRIPT_PATH"
		fi
		#touch ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}_${pid}.conf
		[ ! -e "${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}_${pid}.conf" ] && lock_and_copy_conf_file ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}_${pid}.conf
	fi

	if [ "$interface_type" = "$AP" ] || [ "$interface_type" = "$VAP" ]
	then
		# Copy existing driver post-up conf file to the temp conf file if the temp file doesn't exist.
		[ ! -e "${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}_${pid}.conf" ] && lock_and_copy_conf_file ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}_${pid}.conf

		# Copy existing hostapd_vap conf file to the temp conf file if the temp file doesn't exist.
		hostapd_vap_conf_temp=${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}_${pid}.conf
		[ ! -e "$hostapd_vap_conf_temp" ] && lock_and_copy_conf_file ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}.conf $hostapd_vap_conf_temp

		# If the script calling is ssid_add, write initial values
		if [ "$caller_script" = "ssid_add" ]
		then
			echo "############## $interface_name VAP parameters #############" > $hostapd_vap_conf_temp
			[ "$interface_type" = "$VAP" ] && set_conf_param hostapd_vap regular no_otf $pid $interface_name bss $interface_name
			set_conf_param hostapd_vap regular no_otf $pid $interface_name vendor_elements "dd050009860100"
		fi

		[ "$caller_script" = "security" ] && [ ! -e "${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}_${pid}.conf" ] && lock_and_copy_conf_file ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${interface_name}_${pid}.conf

		touch ${CONF_DIR}/${DRIVER_SINGLE_CALL_CONFIG_FILE}_${interface_name}_${pid}.conf
	else
		# Copy existing driver post-up conf file to the temp conf file if the temp file doesn't exist.
		[ ! -e "${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}_${pid}.conf" ] && lock_and_copy_conf_file ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface_name}_${pid}.conf

		# Copy existing wpa_supplicant file to the temp conf file if the temp file doesn't exist.
		wpa_supplicant_configuration_conf_temp=${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface_name}_${pid}.conf
		[ ! -e "$wpa_supplicant_configuration_conf_temp" ] && lock_and_copy_conf_file ${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface_name}.conf $wpa_supplicant_configuration_conf_temp

		# Copy existing profile conf file to the temp conf file if the temp file doesn't exist.
		profile_conf_temp=${CONF_DIR}/${SUPPLICANT_PROFILE_CONF_PREFIX}_${interface_name}_${pid}.conf
		[ ! -e "$profile_conf_temp" ] && lock_and_copy_conf_file ${CONF_DIR}/${SUPPLICANT_PROFILE_CONF_PREFIX}_${interface_name}.conf ${CONF_DIR}/${SUPPLICANT_PROFILE_CONF_PREFIX}_${interface_name}.conf $profile_conf_temp
		# If the script calling is ssid_add, write initial values
		if [ "$caller_script" = "ssid_add" ]
		then
			echo "############## $interface_name supplicant parameters #############" > $wpa_supplicant_configuration_conf_temp
			set_conf_param wpa_supplicant_configuration regular otf $pid $interface_name ctrl_interface /var/run/wpa_supplicant
			set_conf_param wpa_supplicant_configuration regular otf $pid $interface_name update_config "1"
			# Filter for ASUS vsie
			set_conf_param wpa_supplicant_configuration regular otf $pid $interface_name driver_param "\"vendor_events_filter=f832e4\""
		fi
	fi
}

# Copy the temporary conf files instead of the existing files.
# ls command sends errors to /dev/null to avoid errors when "remove" already deleted all files.
update_conf_files()
{
	# Define local parameters
	local pid temp_files file orig_file

	pid=$1

	temp_files=`ls ${CONF_DIR}/*${pid}.conf 2>/dev/null`
	for file in $temp_files
	do
		orig_file=${file%%_${pid}*}.conf
		lock_and_copy_conf_file $orig_file $file $orig_file
	done
	
	# Delete all temporary files
	rm -f ${CONF_DIR}/*_${pid}*
}

# Find the VAP name to use.
# If name included in in.conf file, use it
# If not, find the first unused VAP index - looks for holes in existing allocation or adds at end
find_vap_name()
{
	radio_name=$1
	cur_vap_asuswrt=`cat /tmp/cur_vap_asuswrt.txt`
	if [ "$cur_vap_asuswrt" = "wlan0.0" ]
	then
		echo "wlan0.0"
	elif [ "$cur_vap_asuswrt" = "wlan0.1" ]
	then
		echo "wlan0.1"
	elif [ "$cur_vap_asuswrt" = "wlan0.2" ]
	then
		echo "wlan0.2"
	elif [ "$cur_vap_asuswrt" = "wlan2.0" ]
	then
		echo "wlan2.0"
	elif [ "$cur_vap_asuswrt" = "wlan2.1" ]
	then
		echo "wlan2.1"
	elif [ "$cur_vap_asuswrt" = "wlan2.2" ]
	then
		echo "wlan2.2"
	else
		echo "${radio_name}"
	fi
}

find_vap_name2()
{
	local radio_name name_db ascii_name_db new_index found_index interface current_index

	radio_name=$1

	# Check in.conf for interface name
	name_db=`grep ^Name_0 ${IN_CONF}`
	name_db=${name_db##*=}
	name_db=${name_db//\"/}
	ascii_name_db=$(printf "%b" "$name_db")
	[ -n "$ascii_name_db" ] && echo "$ascii_name_db" && return

	# Name wasn't found in in.conf, find name to use
	new_index=0
	num_interfaces=`wc -l  < ${VAPS_LIST}_${radio_name}`
	index_exist=0

	i=0
	while [ $i -lt $num_interfaces ]
	do
		while read interface
		do
			[ "$interface" = "$radio_name" ] && continue
			current_index=${interface##$radio_name.}
			if [ $i -eq $current_index ]
			then
				# Index exist, break internal loop
				index_exist=1
				break
			fi			
		done < ${VAPS_LIST}_${radio_name}
		
		if [ $index_exist -eq 1 ]
		then
			# Not found yet
			index_exist=0
		else
			# Save smallest missing index
			break
		fi
		i=$((i+1))
	done
	new_index=$i

	echo "${radio_name}.${new_index}"
}

# Find the STA name to use.
# STA name is radio name + 1 (radio wlan0 has wlan1 STA, etc.)
find_sta_name()
{
	local radio_name name_db ascii_name_db sta_suffix sta_name

	radio_name=$1

	sta_suffix=${radio_name##wlan}
	sta_suffix=$((sta_suffix+1))
	sta_name="wlan${sta_suffix}"
	echo "$sta_name"
}

# Read from in.conf the value of X_LANTIQ_COM_Vendor_SsidType
get_ssid_type()
{
	local ssid_type ascii_ssid_type

	ssid_type=`grep ^X_LANTIQ_COM_Vendor_SsidType_0 ${IN_CONF}`
	ssid_type=${ssid_type##*=}
	ssid_type=${ssid_type//\"/}
	ascii_ssid_type=$(printf "%b" "$ssid_type")
	echo "$ascii_ssid_type"
}

# There is 1 file indicating if a configuration was done on the current interface - if exists, no need to create temp conf files
# There is 1 file indicating if a configuration was done on a VAP related to the current radio - this file holds the pid to use for the current session.
# The radio file will be created if not yet created
# Calling script will source the radio file to get the pid to use
# EndPoint is treated as Radio
prepare_confs()
{
	local interface_name conf_in_progress_interface pid interface_type caller_script
	
	interface_name=$1
	conf_in_progress_interface=$2
	pid=$3
	interface_type=$4
	caller_script=$5

	# If no radio/STA related configuration was done yet, write pid in CONF_IN_PROGRESS file
	if [ ! -e "${CONF_IN_PROGRESS}_${conf_in_progress_interface}" ]
	then
		echo "pid=$pid" > ${CONF_IN_PROGRESS}_${conf_in_progress_interface}
	else
		. ${CONF_IN_PROGRESS}_${conf_in_progress_interface}
	fi

	# Prepare the temp configuration files for the configured radio/STA
	# Write initial content to the temp conf files
	write_initial_content $interface_name $interface_type $pid $caller_script
}

# Update the INTERFACES_STATUS file for the radio indicating if an interface is up or down
# If interface status is included in the in.conf file, sets the restart flag
update_enable_disable()
{
	local radio_name interface_name interface_index restart_interface interface_enabled enable_changed

	radio_name=$1
	interface_name=$2
	interface_index=$3
	restart_interface=$4

	interface_enabled=`db2fapi_convert boolean Enable $interface_index`
	[ -e "${INTERFACES_STATUS}_${radio_name}" ] && sed -i '/^'$interface_name'_enabled=.*/d' ${INTERFACES_STATUS}_${radio_name}
	echo "${interface_name}_enabled=${interface_enabled}" >> ${INTERFACES_STATUS}_${radio_name}

	# Check if Enable parameter is in in.conf (interface got enabled/disabled)
	enable_changed=`grep -c ^Enable ${IN_CONF}`
	if [ "$enable_changed" -gt 0 ]
	then
		set_restart_and_reconfigure $interface_name $restart_interface
	fi
}

# Update the ENABLE_ONLINE_STATUS file for the radio indicating if EnableOnline is set for an interface
# If interface status is included in the in.conf file, sets the restart flag
update_enable_online()
{
	local radio_name interface_name interface_index enable_online enable_online_changed

	radio_name=$1
	interface_name=$2
	interface_index=$3

	interface_no_dot=${interface/\./_}
	enable_online=`db2fapi_convert boolean EnableOnLine $interface_index`
	[ -e "${ENABLE_ONLINE_STATUS}_${radio_name}" ] && sed -i '/^'$interface_no_dot'_enableOnLine=.*/d' ${ENABLE_ONLINE_STATUS}_${radio_name}
	[ -n "$enable_online" ] && echo "${interface_no_dot}_enableOnLine=${enable_online}" >> ${ENABLE_ONLINE_STATUS}_${radio_name}

	# Check if EnableOnLine parameter is in in.conf (value was changed)
	enable_online_changed=`grep -c EnableOnLine ${IN_CONF}`
	if [ "$enable_online_changed" -gt 0 ]
	then
		set_restart_and_reconfigure $interface_name $radio_name
	fi
}

# clear the hostapd_vap conf file from security parameters that are not related to the current security mode
clear_security_parameters()
{
	local interface_name pid hostapd_vap_conf_temp ieee80211w

	interface_name=$1
	pid=$2

	hostapd_vap_conf_temp=${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}_${pid}.conf

	# Read current security mode from conf file
	[ -e "${SECURITY_CONF}_${interface_name}" ] && . ${SECURITY_CONF}_${interface_name}
	# Removed un-related parameters
	case "$security_mode" in
		"None")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^wpa=.*/d' -e '/^wpa_pairwise=.*/d' -e '/^wpa_key_mgmt=.*/d' -e '/^rsn_pairwise=.*/d' -e '/^ieee8021x=.*/d' -e '/^wpa_passphrase=.*/d' -e '/^wpa_group_rekey=.*/d' -e '/^wpa_gmk_rekey.*/d' -e '/^auth_server.*/d' -e '/^acct_server.*/d' -e '/^eap_reauth_period=.*/d' -e '/^ieee80211w=.*/d' -e '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		"WEP-64"|"WEP-128")
			sed -i -e '/^wpa=.*/d' -e '/^wpa_pairwise.*/d' -e '/^wpa_key_mgmt=.*/d' -e '/^rsn_pairwise=.*/d' -e '/^ieee8021x=.*/d' -e '/^wpa_passphrase=.*/d' -e '/^wpa_group_rekey=.*/d' -e '/^wpa_gmk_rekey.*/d' -e '/^auth_server.*/d' -e '/^acct_server.*/d' -e '/^eap_reauth_period=.*/d' -e '/^ieee80211w=.*/d' -e '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		"WPA-Personal")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^rsn_pairwise=.*/d' -e '/^ieee8021x=.*/d' -e '/^auth_server.*/d' -e '/^acct_server.*/d' -e '/^ieee80211w=.*/d' -e '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		"WPA2-Personal")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^rsn_pairwise=.*/d' -e '/^ieee8021x=.*/d' -e '/^auth_server.*/d' -e '/^acct_server.*/d' $hostapd_vap_conf_temp
			# If 802.11w is disabled, remove sa_query parameters
			ieee80211w=`grep ieee80211w $hostapd_vap_conf_temp`
			ieee80211w=${ieee80211w##*=}
			[ "$ieee80211w" != "1" ] && sed -i '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		"WPA-WPA2-Personal")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^ieee8021x=.*/d' -e '/^auth_server.*/d' -e '/^acct_server.*/d' -e '/^ieee80211w=.*/d' -e '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		"WPA-Enterprise")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^rsn_pairwise=.*/d' -e '/^ieee80211w=.*/d' -e '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		"WPA2-Enterprise")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^rsn_pairwise=.*/d' $hostapd_vap_conf_temp
			;;
		"WPA-WPA2-Enterprise")
			sed -i -e '/^wep_default_key=.*/d' -e '/^wep_key.*/d' -e '/^ieee80211w=.*/d' -e '/^assoc_sa_query.*/d' $hostapd_vap_conf_temp
			;;
		# TODO: add osen
		#$osen)
		#	;;
	esac
}

# clear the hostapd_vap conf file from list of parameters
clear_hostapd_params()
{
	local interface_name pid params hostapd_vap_conf_temp

	interface_name=$1
	pid=$2
	params=$3

	hostapd_vap_conf_temp=${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}_${pid}.conf
	echo "$params" > ${CONF_DIR}/params
	grep -f ${CONF_DIR}/params -v ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}_${pid}.conf > ${CONF_DIR}/hostapd_tmp
	mv ${CONF_DIR}/hostapd_tmp ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface_name}_${pid}.conf
	rm -f ${CONF_DIR}/params
}

# Check if an interface is VAP by searchig dot in the interface name.
check_is_vap()
{
	local interface_name dot_location

	interface_name=$1
	dot_location=`expr index $interface_name .`

	if [ $dot_location -gt 0 ]; then
		echo true
	else
		echo false
	fi
}

# Get the next available object index from the fapi_wlan_wave_in.conf
# The in.conf file is always written in order, so the last object index will be in the last line.
get_next_object_index()
{
	local last_line last_index next_index

	# Read the last line in the file
	last_line=`tail -n 1 ${IN_CONF}`
	last_index=${last_line%%=*}
	last_index=${last_index##*_}
	if [ -z "$last_index" ]
	then
		next_index=0
	else
		next_index=$((last_index+1))
	fi

	echo $next_index
}

# Check if the wlan interface is ready (ifconfig is working)
# If the interface is up, return 0, else return 1.
check_interface_is_ready()
{
	# Define local parameters
	local interface_name radio_name ifconfig_status

	interface_name=$1
	radio_name=${interface_name%%.*}

	# Check if the wlan interface exists.
	ifconfig_status=`ifconfig | grep -wc $interface_name`
	if [ $ifconfig_status -eq 0 ]
	then
		echo "$interface_name interface not ready." > /dev/console
		echo 1
		return
	fi
	echo 0
}

# Return the supported frequencies by an interface
# Function gets the iw output file to parse for the supported frequencies
get_supported_frequencies()
{
	# Define local parameters
	local iw_output iw_frequencies freq_24 freq_5 f f1 supported_frequencies
	
	iw_output=$1
	
	iw_frequencies=`grep "MHz" $iw_output | grep -v "short" | grep -v "total" | sed '/MHz/s/ MHz.*//'`
	freq_24=""
	freq_5=""
	for f in $iw_frequencies
	do
		f1=${f:0:1}
		[ "$f1" = "2" ] && freq_24="yes"
		[ "$f1" = "5" ] && freq_5="yes"
	done
	
	if [ "$freq_24" ]
	then
		if [ "$freq_5" ]
		then
			supported_frequencies="2.4GHz,5GHz"
		else
			supported_frequencies="2.4GHz"
		fi
	elif [ "$freq_5" ]
	then
		supported_frequencies="5GHz"
	fi
	
	echo "$supported_frequencies"
}

# Parse ifconfig result to extract the interfaces names
get_interfaces_from_ifconfig()
{
	local ifconfig_res ifconfig_radio_vaps cur_interface other_info

	ifconfig_res=$1
	ifconfig_radio_vaps=""

	# The ifconfig_res file has each interface found in a separate line.
	# The interface name is the first word in the line
	while read cur_interface other_info
	do
		ifconfig_radio_vaps="$ifconfig_radio_vaps $cur_interface"
	done < $ifconfig_res
	echo "$ifconfig_radio_vaps"
}

# Remove parameters from the a conf file by creating a grep string command and execute it to update the conf file.
# The created grep command removes all the parameters in the params_list from the conf file and writes the result to the conf file.
# Example of the final grep command: grep -wv "param_1\|param_2\|param_3" hostapd.conf > temp_hostapd.conf
remove_params_from_conf()
{
	# Define local parameters
	local params_list conf_file conf_file_type tmp_conf_file grep_cmd param

	params_list=$1
	conf_file=$2
	conf_file_type=$3

	tmp_conf_file=${conf_file}_tmp
	grep_cmd="grep -wv \""
	if [ "$conf_file_type" = "$HOSTAPD_VAP_CONF_PREFIX" ] || [ "$conf_file_type" = "$HOSTAPD_PHY_CONF_PREFIX" ]
	then
		for param in $params_list
		do
			grep_cmd=${grep_cmd}^${param}\\\|
		done
	else
		for param in $params_list
		do
			grep_cmd=${grep_cmd}${param}\\\|
		done
	fi

	grep_cmd=${grep_cmd%\\\|}
	grep_cmd="${grep_cmd}\" $conf_file > $tmp_conf_file"
	eval $grep_cmd
	mv -f $tmp_conf_file $conf_file
}

# return the list of parameters for a given hotspot object
get_hs20_params_list()
{
	# Define local parameters
	local object_name params_list

	object_name=$1

	case "$object_name" in
		"ACCESSPOINT_HS20_OPERFRIENDLYNAME_OBJECT")
			params_list="hs20_oper_friendly_name"
			;;
		"ACCESSPOINT_HS20_CONNECTIONCAPAB_OBJECT")
			params_list="hs20_conn_capab"
			;;
		"ACCESSPOINT_HS20_OSUICONS_OBJECT")
			params_list="hs20_icon"
			;;
		"ACCESSPOINT_HS20_OSUPROVIDERS_OBJECT")
			params_list="osu_server_uri
			osu_friendly_name
			osu_friendly_name2
			osu_nai
			osu_method_list
			osu_icon
			osu_icon2
			osu_service_desc
			osu_service_desc2"
			;;
		"ACCESSPOINT_HS20_ROAMING_OBJECT")
			params_list="roaming_consortium"
			;;
		"ACCESSPOINT_HS20_VENUENAME_OBJECT")
			params_list="venue_name"
			;;
		"ACCESSPOINT_HS20_NAIREALM_OBJECT")
			params_list="nai_realm"
			;;
	esac
	echo "$params_list"
}

# Clean hs20 parameters for a specific object
clean_hs20_params()
{
	# Define local parameters
	local object_name interface_name pid params_list conf_file

	object_name=$1
	interface_name=$2
	pid=$3

	params_list=`get_hs20_params_list $object_name`

	clear_hostapd_params $interface_name $pid "$params_list"
}

# Write the parameters for a requested hotspot object
write_hs20_params()
{
	# Define local parameters
	local object_name interface_name pid params_list object_index \
	param value conf_file

	object_name=$1
	interface_name=$2
	pid=$3
	object_index=$4

	params_list=`get_hs20_params_list $object_name`
	for param in $params_list
	do
		value=`get_conf_param $param $object_index $interface_name`
		if [ -n "$value" ]
		then
			set_conf_param hostapd_vap regular no_otf $pid $interface_name "$param" "$value"
		fi
	done

	# Remove suffix "2" from osu_friendly_name2, osu_icon2 and osu_service_desc2
	conf_file=${CONF_DIR}/hostapd_vap_${interface_name}_${pid}.conf
	sed -i -e 's/^osu_friendly_name2=/osu_friendly_name=/' -e 's/^osu_icon2=/osu_icon=/' -e 's/^osu_service_desc2=/osu_service_desc=/' $conf_file
}

# Some hotspot objects can have multiple values. For the requested object:
# Read the number of instances it has
# Remove all occurrences from the conf file
# Write all the values to the conf file
set_hotspot_objects()
{
	# Define local parameters
	local object_name interface_name pid hs20_index \
	num_objects i clean_done current_object

	object_name=$1
	interface_name=$2
	pid=$3
	hs20_index=$4

	num_objects=`get_next_object_index`
	i=0
	clean_done=""
	while [ $i -lt $num_objects ]
	do
		eval current_object=\${Object_${i}}
		current_object=$(printf "%b" "$current_object")
		if [ "$current_object" = "$object_name" ]
		then
			[ -z "$clean_done" ] && clean_hs20_params $object_name $interface_name $pid && clean_done="yes"
			write_hs20_params $object_name $interface_name $pid $i
		fi
		i=$((i+1))
	done
}

# Build the Wlan notification string for the proper platform
# Different platforms may required different notification commands
# Function get:
# The destination of the notification (wsd is web, servd is DB)
# The notification to send
# The configuration in the format of param1:value1 param2:value2... (param1 is always "Name", value1 is always the interface name)
# If conf_file flag="runner", write notification command to fapi_wlan_wave_runner.sh
# If conf_file flag=conf name, write notification command to this conf name.
# If conf_file flag is not set, create script to execute the notification and execute the script.
# GRX500 uses: ubus call
# Puma uses: fapi_wlan_cli
build_wlan_notification()
{
	local destination notification configuration conf_file interface_index platform_type fapi_rpc nid param_index \
	notify_type notify_command config param value notification_script object_name notification_conf \
	name_found i interface_name
	
	destination=$1
	notification=$2
	configuration="$3"
	conf_file=$4

	# Find the interface index of wlan0
	[ "$configuration" != "complete_recovery" ] && interface_index=`find_index_from_interface_name wlan0`

	# Currently, only GRX500 platforms are supported
	if [ -e /etc/rc.d/config.sh ]
	then
		. /etc/rc.d/config.sh
		platform_type="$CONFIG_IFX_CONFIG_CPU"
	else
		platform_type="Puma"
	fi
	
	
	fapi_rpc=false
	if [ -e $FAPI_RPC ]
	then
		fapi_rpc=true
	fi

	case "$fapi_rpc" in
		"false")
			# Create ubus command for SL to update DB with new values
			# Source ugw_notify_defs.sh
			. /etc/ugw_notify_defs.sh
			eval nid=\${${notification}}
			param_index=1
			
			# All destinations use notify type of "notify", wsd uses notify type of "notify.status"
			notify_type="notify"
			[ "$destination" = "wsd" ] && notify_type="notify.status"
			
			notify_command="ubus call $destination $notify_type '{\"nid\":$nid,\"type\":false"
			if [ "$configuration" != "NO_PARAMS" ]
			then
				for config in $configuration
				do
					param=${config%%:*}
					value=${config#$param:}
					[ "$param" = "RegulatoryDomain" ] && value="${value} "
					# Values that need to include spaces will have underscore instead, replace underscore to spaces
					# with the exceptions of Object names as parameters
					[ "$param" != "Object" ] && value=${value/_/ }
					notify_command="${notify_command},\"pn${param_index}\":\"${param}\",\"pv${param_index}\":\"${value}\""
					param_index=$((param_index+1))
				done
			fi
			notify_command="${notify_command}}' &"
			;;
		"true")
			# Web notifications are ignored in Puma
			[ "$destination" = "wsd" ] && return
			# Find the object to update according to the notification name
			if [ "$notification" = "NOTIFY_WIFI_UPDATE_PARAM" ]
			then
				object_name="" #InNotification
			else
			object_name=`find_object_from_notification $notification`
			fi
			# Create the configuration file and set the parameters in it
			# Since more than 1 notification can exist in the same execution, find a new name to use
			if [ -e $PUMA_NOTIFICATION_CONF ]
			then
				notification_conf=${PUMA_NOTIFICATION_CONF}
			else
				name_found="yes"
				i=0
				while [ "$name_found" ]
				do
					i=$((i+1))
					[ ! -e ${PUMA_NOTIFICATION_CONF}_${i} ] && name_found=""
				done
				notification_conf=${PUMA_NOTIFICATION_CONF}_${i}
			fi
			cat /dev/null > ${notification_conf}
			if [ -n "$object_name" ]
			then
				update_conf_out "Object_0" "$object_name" $notification_conf
			fi
			# Go over configuration input and update the configuration file
			for config in $configuration
			do
				param=${config%%:*}
				value=${config#$param:}
				[ "$param" = "Name" ] && interface_name="$value" && continue
				# Values that need to include spaces will have underscore instead, replace underscore to spaces
				[ "$param" != "Object" ] && [ "$param" = "Status" ] && value=${value/_/ }
				update_conf_out "${param}_0" "$value" $notification_conf
			done
			if [ "$object_name" = "$RADIO_WPS_VENDOR_OBJECT" ]
			then
				# Find the radio name. WPS object in Radio Vendor
				interface_name=${interface_name%%.*}
			fi
			notify_command="fapi_wlan_cli notify -n $interface_name -f ${notification_conf}"
			;;
	esac
	if [ -n "$conf_file" ] && [ "$conf_file" = "runner" ]
	then
		echo "$notify_command" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	elif [ -n "$conf_file" ]
	then
		echo "$notify_command" >> $conf_file
	else
		# If the notification is not executed in the runner, create a script to execute the notification
		notification_script="${CONF_DIR}/notification_script.sh"
		[ "$configuration" != "complete_recovery" ] && print2log $interface_index DEBUG "build_wlan_notification: sending notification: $notify_command"
		echo "$notify_command" > $notification_script
		chmod +x $notification_script
		$notification_script
		rm -f $notification_script
	fi
}

# Check if an interface is Wave500B
# Wave500B PCIe cards are identified as 1bef:0810 in lspci output.
check_wave500b()
{
	# Define local parameters
	local interface_name
	local i

	interface_name=$1
	i=0
	# Read information of physical Wlan interface from wlan_discover output
	. ${TEMP_DIR}/fapi_wlan_wave_discover.txt

	# If no PCIe detected, return "no"
	[ "$PCI_LTQ_COUNT" = "0" ] && echo "no" && return
	# If AHB detected, PCIe name can start from wlan2
	if [ "$AHB_WLAN_COUNT" = "1" ]
	then
		i=2
		[ "$AHB_DISABLED" = "1" ] && i=0
	fi

	# Go over the lspci output saved by wlan_discover.sh
	while read line
	do
		line=${line##*1bef:}
		[ "$line" != "${line/0810/}" ] && wave500b_interface="wlan${i}"
		[ "$wave500b_interface" = "$interface_name" ] && echo "yes" && return
		[ "${line:0:2}" = "08" ] && i=$((i+2))
	done < /tmp/lspci.txt
	echo "no"
}

# retrun the chip id for interface
# taken from : lspci output
check_wave_chip_id()
{
	local interface_name i chipid_detected
	interface_name=$1

	i=0
	# in case no chipid detected for interface_name retrun 0
	chipid_detected="0"

	# check if we have internal wlan on wlan0 then return 07E0
	. ${TEMP_DIR}/fapi_wlan_wave_discover.txt
	if [ "$AHB_WLAN_COUNT" = "1" ];	then
		if [ -z "$AHB_DISABLED" ] || [ "$AHB_DISABLED" = "0" ]; then
			i=2
			[ "$interface_name" = "wlan0" ] && echo "07E0" && return
		fi
	fi

	while read line
	do

		device_class_id="${line##*Class }"
		device_class_id="${device_class_id:0:4}"

		# Only handle Wireless controller cass id = 0d80
		if [ "$device_class_id" = "0d80" ]; then
			if [ "$interface_name" = "wlan${i}" ]; then
				chipid_detected="${line##*:}"
				echo "$chipid_detected"
				return
			fi
			i=$((i+2))
		fi

	done < /tmp/lspci.txt
	echo "Not Detected"
}

# Create new file with current parameters values and source this file.
# Check if the original conf file exists.
# The parameters in this file will be in the format of: current_<parameter name>="<value>" (e.g: current_hw_mode="g")
# conf_file_prefix can be drv_config_post_up, hostapd_phy, hostapd_vap or drvhlpr.
# The sed command for hostapd and drvhlpr files has 5 steps:
# Step 1: Remove all comment lines from conf file (starting with #)
# Step 2: Replace all " = " with "=" (parameters in drvhlpr conf file have the format of parameter = value)
# Step 3: Replace all = with ="
# Step 4: Add " to the end of line (to create the format of parameter="value")
# Step 5: Add the string "current_" to the beginning of the line (to create the final format of current_<parameter name>="<value>")

# The sed command for drv_config_post_up has 4 steps:
# Step 1: Remove all iwpriv and iwconfig with interface name.
# Step 2: Replace the first space with ="
# Step 3: Add " to the end of line (to create the format of parameter="value")
# Step 4: Add the string "current_" to the beginning of the line (to create the final format of current_<parameter name>="<value>")
read_current_values()
{
	# Define local parameters
	local interface_name conf_file_prefix current_values_file cur_ssid hex_ssid cur_osu_ssid hex_osu_ssid

	conf_file_prefix=$1
	interface_name=$2
	current_values_file=$3

	if [ -e ${CONF_DIR}/${conf_file_prefix}_${interface_name}.conf ]
	then
		case "$conf_file_prefix" in
		${HOSTAPD_VAP_CONF_PREFIX}|${HOSTAPD_PHY_CONF_PREFIX}|drvhlpr)
			sed -e '/#/d' -e 's/ = /=/' -e 's/=/=\"/' -e 's/$/\"/' -e 's/^/current_/' ${CONF_DIR}/${conf_file_prefix}_${interface_name}.conf > $current_values_file
			# Handle special charcters in ssid and osu_ssid, represent value in hex
			# Read current_ssid from conf file
			cur_ssid=`grep "current_ssid" $current_values_file`
			if [ ! -z "$cur_ssid" ]
			then
				# Remove current_ssid=" prefix and " suffix
				cur_ssid=${cur_ssid##current_ssid=\"}
				cur_ssid=${cur_ssid%\"}
				# Convert to hex value and update the current_values_file
				hex_ssid=`ascii2hex $cur_ssid`
				sed -i '/current_ssid=/d' $current_values_file && echo "current_ssid=\"$hex_ssid\"" >> $current_values_file
			fi

			cur_osu_ssid=`grep "current_osu_ssid" $current_values_file`
			if [ ! -z "$cur_osu_ssid" ]
			then
				# Remove current_osu_ssid=" prefix and " suffix
				cur_osu_ssid=${cur_osu_ssid##current_osu_ssid=\"}
				cur_osu_ssid=${cur_osu_ssid%\"}
				# Convert to hex value and update the current_values_file
				hex_osu_ssid=`ascii2hex $cur_osu_ssid`
				sed -i '/current_osu_ssid=/d' $current_values_file && echo "current_osu_ssid=\"$hex_osu_ssid\"" >> $current_values_file
			fi
		;;
		$DRIVER_POST_UP_CONF_PREFIX|$DRIVER_PRE_UP_CONF_PREFIX)
			sed -e 's/iwpriv '$interface_name' //' -e 's/iwconfig '$interface_name' //' -e '/LogHwModuleFifo/d' -e '/LogRemStream/d' -e '/rdebug/d' -e 's/ > \/proc.*\/*//' -e '/cdebug/ s/echo /driver_debug /' -e 's/ /=\"/' -e 's/$/\"/' -e 's/^/current_/' ${CONF_DIR}/${conf_file_prefix}_${interface_name}.conf > $current_values_file
		;;
		$SUPPLICANT_CONFIGURATION_CONF_PREFIX)
			sed -e '/#/d' -e 's/=/=\"/' -e 's/$/\"/' -e 's/^/current_/' ${CONF_DIR}/${conf_file_prefix}_${interface_name}.conf > $current_values_file
		;;
		esac

		. $current_values_file
		rm $current_values_file
	fi
}

# Go over the requested driver parameters and see if new value is different than current value.
# If change was found, mark the parameter as changed.
# Write to the new values file the parameter and the new value, if the new value is null, write the current value.
# Replace the current driver values file with the new values file.
update_driver_params_file()
{
	# Define local parameters
	local params_list driver_new_values_file driver_current_values_file current_value new_value

	params_list=$1
	driver_new_values_file=$2
	driver_current_values_file=$3

	for parameter in $params_list
	do
		eval current_value=\${current_$parameter}
		eval new_value=\$$parameter
		new_value=`echo $new_value`
		eval ${parameter}_changed=""

		# If current value and new value are empty, nothing needs to be done
		[ -z "$current_value" ] && [ -z "$new_value" ] && continue
		# If current value and new value are not the same, mark parameter as changed.
		if [ "$new_value" != "$current_value" ]
		then
			eval ${parameter}_changed=yes
		fi
		# Write all the values to the new values file.
		# If new value is null, write the current value, else write the new value.
		if [ -z "$new_value" ]
		then
			echo "current_$parameter=\"$current_value\"" >> $driver_new_values_file
		else
			echo "current_$parameter=\"$new_value\"" >> $driver_new_values_file
		fi
	done

	# Replace current values file with the new values file
	mv $driver_new_values_file $driver_current_values_file
}

# Go over the list of requested parameters and see if new value is different than current value.
# If change was found, return "yes", if no change found, return empty value.
check_param_changed()
{
	# Define local parameters
	local params_list params_changed current_value new_value

	params_list=$1

	params_changed=""
	for parameter in $params_list
	do
		eval current_value=\${current_$parameter}
		eval new_value=\$$parameter
		new_value=`echo $new_value`
		if [ "$parameter" = "ssid" ]
		then
			hex_value=`ascii2hex $new_value`
			[ "$hex_value" != "$current_value" ] && params_changed="yes" && break
		else
			[ "$new_value" != "$current_value" ] && params_changed="yes" && break
		fi
	done
	echo "$params_changed"
}

check_rkh_param_changed()
{
	# Define local parameters
	local param ret_val res

	ret_val=0
	param=$1
	res=`grep $param ${IN_CONF}`
	[ $? -eq 0 ] && ret_val=1
	echo "$ret_val"
}

check_atf_param_changed()
{
	# Define local parameters
	local param ret_val res

	ret_val=0
	param=$1
	fileToCheck=$2
	res=`grep $param $fileToCheck`
	[ $? -eq 0 ] && ret_val=1
	echo "$ret_val"
}

# Check if the WDS list (wds or 4 addresses) was changed (MACs were added or removed).
check_wds_list_changed()
{
	# Define local parameters
	local driver_list db_list list_changed driver_mac \
	driver_list_length num_mac_driver db_list_length num_macs_db

	driver_list=$1
	db_list=$2
	list_changed=""

	# Go over MACs in driver and see if each MAC appears in DB list. If not, a change was found, return "yes"
	for driver_mac in $driver_list
	do
		[ "$db_list" = "${db_list/$driver_mac/}" ] && list_changed="yes" && break
	done

	# If a change was not found yet, check if the number of MACs in driver and DB is different, if so, a change was found, return yes.
	if [ -z "$list_changed" ]
	then
		# Calculate number of peers in driver by measuring the length and divide by 17 (length of a MAC address).
		driver_list_length=${#driver_list}
		num_mac_driver=$((driver_list_length/17))
		# Calculate number of peers in DB by measuring the length and divide by 17 (length of a MAC address).
		db_list_length=${#db_list}
		num_macs_db=$((db_list_length/17))
		[ $num_mac_driver -ne $num_macs_db ] && list_changed="yes"
	fi

	echo "$list_changed"
}

# Converts ascii to hex
ascii2hex()
{
	# Define local parameters
	local ascii_x ascii_len i ascii_char

	ascii_x=$1
	ascii_len=${#ascii_x}
	i=0
	while [ $i -lt $ascii_len ]
	do
		ascii_char=${ascii_x:$i:1}
		printf '\\x%02x' "'$ascii_char" | sed 's/00/20/'
		i=$((i+1))
	done
}

find_object_from_notification()
{
	# Define local parameters
	local notification object_name

	notification=$1
	object_name=""
	case "$notification" in
		"NOTIFY_WIFI_RADIO_STATUS_CHANGE")
			object_name="$RADIO_OBJECT"
			;;
		"NOTIFY_WIFI_SSID_STATUS_CHANGE")
			object_name="$SSID_OBJECT"
			;;
		"NOTIFY_WIFI_WPS_STATUS")
			object_name="$RADIO_WPS_VENDOR_OBJECT"
			;;
		"NOTIFY_WIFI_DEVICE_ASSOCIATED")
			object_name="$ACCESSPOINT_ASSOCIATED_DEVICES_OBJECT"
			;;
	esac

	echo "$object_name"
}

extract_stats_from_mtdump()
{
	# Define local parameters
	local mtdump_output post_stat_text pre_stat_text

	mtdump_output=$1
	post_stat_text=$2
	pre_stat_text=$3

	stat_value=${mtdump_output%%$post_stat_text*}
	stat_value=${stat_value##*$pre_stat_text}
	stat_value=`echo $stat_value`
	echo "$stat_value"
}

read_bridge_from_db()
{
	# Define local parameters
	local interface_name interface_index bridge_name

	interface_name=$1
	interface_index=$2

	if [ "$interface_name" = "rtlog0" ]
	then
		local_db_source RADIO
		bridge_name=`db2fapi_convert regular WaveLoggerBridgeName $interface_index`
	else
		local_db_source SSID
		bridge_name=`db2fapi_convert regular X_LANTIQ_COM_Vendor_BridgeName $interface_index`
	fi

	echo "$bridge_name"
}

check_interface_type()
{
	# Define local parameters
	local interface_name interface_type

	interface_name=$1

	interface_type=`iw $interface_name info | grep type | awk '{print $2}'`
	interface_type=`echo $interface_type`
	if [ "$interface_type" = "managed" ]
	then
		interface_type="$STA"
	else
		interface_type="$AP"
	fi

	echo "$interface_type"
}

get_radio_name_from_endpoint()
{
	# Define local parameters
	local interface_name radio_index radio_name

	interface_name=$1

	radio_index=${interface_name#wlan}
	radio_index=$((radio_index-1))
	radio_name="wlan${radio_index}"
	echo "$radio_name"
}

# Set the flags indicating that all information was received to make connection
set_connect_flags()
{
	# Define local parameters
	local interface_name connect_to_ap profile_reference \
	profile_security_received profile_received

	interface_name=$1

	connect_to_ap="false"

	# Check if ProfileReference exists in in.conf and not empty
	profile_reference=`grep ^ProfileReference ${IN_CONF}`
	profile_reference=${profile_reference##*=}
	[ -n "$profile_reference" ] && touch ${PROFILE_REFERENCE_FLAG}_${interface_name}

	# Check if Profile object received
	profile_security_received=`grep -c $ENDPOINT_PROFILE_SECURITY_OBJECT ${IN_CONF}`
	profile_received=`grep -c $ENDPOINT_PROFILE_OBJECT ${IN_CONF}`
	if [ $profile_security_received -gt 0 ]
	then
		touch ${PROFILE_SECURITY_FLAG}_${interface_name}
		[ $profile_received -gt 1 ] && touch ${PROFILE_FLAG}_${interface_name}
	elif [ $profile_received -gt 0 ]
	then
		# Check if call is to disconnect
		disconnect=`grep "Status" ${IN_CONF}`
		disconnect=${disconnect#Status*=}
		disconnect=${disconnect//\"/}
		disconnect_ascii=$(printf "%b" "$disconnect")
		if [ "$disconnect_ascii" = "Disabled" ]
		then
			connect_to_ap="disconnect"
			rm -f ${CONNECT_FLAG}_${interface_name}
			echo $connect_to_ap
			return
		else
			touch ${PROFILE_FLAG}_${interface_name}
		fi
	fi

	if [ -e ${PROFILE_REFERENCE_FLAG}_${interface_name} ] && [ -e ${PROFILE_SECURITY_FLAG}_${interface_name} ] && [ -e ${PROFILE_FLAG}_${interface_name} ]
	then
		connect_to_ap="true"
	elif [ -e ${PROFILE_REFERENCE_FLAG}_${interface_name} ]
	then
		connect_to_ap="partial"
	fi
	echo "$connect_to_ap"
}

# Get the sub-object index from IN_CONF file.
# Example, for the below object it returns vector: '6 9'
# 	Object_6=Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.HS20.OSUicons
# 	Object_9=Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.HS20.OSUicons
get_param_index_dynamic()
{
	param_name=$1
	param_num_of_instances=$2
	sub_object_string=$3
	param_indexes=
	param_indexes_tmp=
	
	while read -r line; do
		obj_line_row=`echo "$line" | grep "$sub_object_string"`
		if [ -n "$obj_line_row" ]
		then
			#get index:
			param_indexes_tmp=`echo $obj_line_row | sed -e 's/Object_\(.*\)=.*/\1/'`
			param_indexes="$param_indexes $param_indexes_tmp"
		fi
	done < ${IN_CONF}
	
	#echo "param_indexes=$param_indexes" > /dev/console
	echo $param_indexes
}

# Prepare HS2.0 sub-objects conf file
# Command format:
# test-hs20_script <interface_name> <number_sub_objects> <sub_object_name> <conf_name>
# Exampls:
# test-hs20_script wlan0 2 RoamingConsortium hotspot_set_roam
# test-hs20_script wlan0 1 NAIrealm hotspot_set_nairealm
create_sub_instance_in_files()
{
	# Define local parameters
	local interface_name number_of_entries sub_object conf_name sub_object_string_in
	
	interface_name=$1
	number_of_entries=$2
	sub_object=$3
	conf_name=$4
	sub_object_string_in=HS20.${sub_object}

	# Find the interface index and the radio index
	interface_index=`find_index_from_interface_name $interface_name`
	#echo "number_of_entries=$number_of_entries" > /dev/console
	
	# Get the sub object indexes:
	#echo "Get the sub object indexes" > /dev/console
	index_list=`get_param_index_dynamic $sub_object number_of_entries $sub_object_string_in`
	#echo "index_list=$index_list" > /dev/console
	
	#echo "Map the objects indexes to the received objects in the in.conf file" > /dev/console
	for i in $index_list
		do
		#echo "call map_param_index_dynamic with $i" > /dev/console
		# Map the objects indexes to the received objects in the in.conf file
		eval sub_obj_index_${i}=$i
		#i=$((i+1))		
		#echo "i=$i" > /dev/console
	done
	
	# Save the input configuration parameters to a local DB used by Wave FAPI
	#echo "Save the input configuration parameters to a local DB used by Wave FAPI" > /dev/console
	i=0
	j=0
	for i in $index_list
	do
		current_param=sub_obj_index_${i}
		#echo "current_param=$current_param" > /dev/console
		eval hs20_sub_obj_index=\$$current_param
		#echo "hs20_sub_obj_index=$hs20_sub_obj_index" > /dev/console
		save_db_params $conf_name $interface_name $hs20_sub_obj_index $interface_index $j
		#i=$((i+1))
		j=$((j+1))
	done
}

check_endpoint_connected()
{
	# Define local parameters
	local radio_name endpoint_name connection_status

	radio_name=$1

	endpoint_index=${radio_name#wlan}
	endpoint_index=$((endpoint_index+1))
	endpoint_name="wlan${endpoint_index}"

	# Check if REPEATER_FLAG exists
	if [ -e ${REPEATER_FLAG}_${endpoint_name} ]
	then
		echo 1
	else
		# Check EndPoint status using wpa_cli
		connection_status=`wpa_cli -i${endpoint_name} status 2>/dev/null | grep "wpa_state=COMPLETED" -c`
		if [ $connection_status -gt 0 ]
		then
			echo 1
		else
			echo 0
		fi
	fi
}

# merge atf conf files into: hostapd_vap_atf.conf which is the final one configuration file.
merge_atf_conf_file()
{
	local interface_name interface_index atf_ena vap_ena i

	interface_name=$1
	atf_ena=$2
	vap_ena=$3

	rm -f ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf
	cat ${CONF_DIR}/${HOSTAPD_ATF_GENERAL_CONF_PREFIX}_${interface_name}.conf >> ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf 2>/dev/null

	# merge VAPs only if vap_enabled=1 ,if not do not merge the VAPs into the atf conf file.
	# Driver will report error in the case where vap_enabled=0 and VAPs data exist in the atf conf file.
	if [ "$atf_ena" = "1" ] && [ "$vap_ena" = "1" ]
	then
		cat ${CONF_DIR}/${HOSTAPD_ATF_CONF_PREFIX}_${interface_name}.conf >> ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf 2>/dev/null
		cat ${CONF_DIR}/${HOSTAPD_SSID_ATF_CONF_PREFIX}_${interface_name}.conf >> ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf 2>/dev/null
		i=0
		while [ -e ${CONF_DIR}/${HOSTAPD_ATF_CONF_PREFIX}_${interface_name}.${i}.conf ]
		do
			cat ${CONF_DIR}/${HOSTAPD_ATF_CONF_PREFIX}_${interface_name}.${i}.conf >> ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf 2>/dev/null
			cat ${CONF_DIR}/${HOSTAPD_SSID_ATF_CONF_PREFIX}_${interface_name}.${i}.conf >> ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf 2>/dev/null
			i=$((i+1))
		done
	fi

	# replace space with comma to align to the Driver format.
	sed -i 's/\ /,/g' ${CONF_DIR}/${HOSTAPD_ATF_RADIO_CONF_PREFIX}_${interface_name}.conf
	echo "ATF:merge_atf_conf_file - done " >> /dev/console
}

# Update wds_wpa_sta_file used when WDS mode is Hybrid
# Read MACs list from DB and add to the file
update_wds_wpa_sta_file()
{
	# Define local parameters
	local interface_name interface_index pid wds_wpa_sta_file_name \
	macs_list mac

	interface_name=$1
	interface_index=$2
	pid=$3

	wds_wpa_sta_file_name=${CONF_DIR}/wds_hybrid_${interface_name}_${pid}.conf
	cat /dev/null > $wds_wpa_sta_file_name

	macs_list=`db2fapi_convert regular WaveWDSPeers $interface_index`
	macs_list="${macs_list//,/ }"
	for mac in $macs_list
	do
		echo "$mac" >> $wds_wpa_sta_file_name
	done
}

read_wps_for_vap_name_from_hostapd()
{
	# Define local parameters
	local enable_log interface_name radio_name hostapd_conf_file_file vap_found wps_of_vap_found \
	section_start start_vap 
	enable_log=0
	interface_name=$1
	radio_name=$2
	hostapd_conf_file_file=${CONF_DIR}/hostapd_${radio_name}.conf
	vap_found=0
	wps_of_vap_found=0
	
	[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: interface_name=$interface_name, radio_name=$radio_name, hostapd_conf_file_file=$hostapd_conf_file_file" > /dev/console
	
	section_start=""
	start_vap=" VAP parameters #"
	
	while read line
	do
		#[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: line=$line" > /dev/console
		start_vap=`echo $line | grep ' VAP parameters #' -c`
		if [ "$vap_found" = "0" ] && [ "$start_vap" = "1" ]
		then
			[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: found VAP Start, get interface name" > /dev/console
			tmp1=${line################ }
			vap_name=`echo $tmp1 | awk '{print $1}'`
			[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: vap_name=$vap_name" > /dev/console
			# Overrun with last name
			if [ "$interface_name" = "$vap_name" ]
			then
				vap_found=1
				[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: vap_found=1" > /dev/console
			fi
		fi
		if [ "$vap_found" = "1" ]
		then
			[ "$wps_of_vap_found" = "0" ] && wps_of_vap_found=`echo $line | grep "___WPS_parameters___" -c`
			#[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: wps_of_vap_found=$wps_of_vap_found" > /dev/console
	
			if [ "$wps_of_vap_found" = "1" ]
			then
				# Look for wps_state
				[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: wps_of_vap_found: line=$line" > /dev/console
				line1=`echo $line | awk -F "=" '{print $1}'`
				[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: wps_of_vap_found: line1=$line1" > /dev/console
				if [ "$line1" = "wps_state" ]
				then
					[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: :-) :-) :-) " > /dev/console
					echo $line
					return
				fi
			fi
		fi
	done < $hostapd_conf_file_file
	
	[ "$enable_log" = "1" ] && echo "read_wps_for_vap_name_from_hostapd: Warning - interface name ($interface_name) not found in $hostapd_conf_file_file file (AddVap operation?)" > /dev/console
}


read_vap_name_of_wps_from_hostapd_common()
{
	# Define local parameters
	local interface_name radio_name section_start_text section_end_text section_start hostapd_get_name line enable_log
	
	# For logs set enable_log=1
	enable_log=0
	[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: $*" > /dev/console

	interface_name=$1
	radio_name=$2
	wps_state=$3
	source_file=$4
	hostapd_conf_file_file=

	[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: interface_name=$interface_name, radio_name=$radio_name, wps_state=$wps_state, source_file=$source_file" > /dev/console

	hostapd_conf_file_file=${CONF_DIR}/hostapd_${interface_name}.conf

	[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: hostapd_conf_file_file=$hostapd_conf_file_file" > /dev/console
	
	[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: wps_state=$wps_state" > /dev/console

	# Options: unConfigured and Configured hostapd files
	# #___WPS_parameters___#
	# # WPS configuration - START
	if [ "$wps_state" = "wps_state=1" ]
	then
		[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: look for name of Unconfigured VAP" > /dev/console
		section_start_text="#___WPS_parameters___#"
	else
		[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: look for name of Configured VAP" > /dev/console
		section_start_text="# WPS configuration - START"
#		[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: look for name of Unconfigured VAP" > /dev/console
#		section_start_text_2="#___WPS_parameters___#"
	fi

	section_start=""
	hostapd_get_name=${CONF_DIR}/hostapd_get_name_${interface_name}
	start_vap=" VAP parameters #"

	rm -f $hostapd_get_name
	
	while read line
	do
		[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: line=$line" > /dev/console
		start_vap=`echo $line | grep ' VAP parameters #' -c`
		if [ "$start_vap" = "1" ]
		then
			[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: found VAP Start, get interface name" > /dev/console
			tmp1=${line################ }
			vap_name=`echo $tmp1 | awk '{print $1}'`
			[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: vap_name=$vap_name" > /dev/console
			# Overrun with last name
			echo "hostapd_vap_name=$vap_name" > $hostapd_get_name
		fi
		# If this section has WPS enable, return the VAP name
		if [ "$section_start" = "1" ]
		then
			[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: section_start=1, compare line ($line) to $wps_state" > /dev/console
			if [ "$line" = "$wps_state" ]
			then
				[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: wps supported interface name already saved ($vap_name), can return" > /dev/console
				# Source new hostapd values file and delete it.
				[ -e "$hostapd_get_name" ] && [ "$source_file" = "1" ] && echo "source  hostapd_get_name" > /dev/console && . $hostapd_get_name
				[ -e "$hostapd_get_name" ] && [ "$source_file" = "0" ] && echo "return $vap_name" > /dev/console && echo $vap_name
				return
			fi
		fi
		# If found starting comment, set the flag.

		wps_of_vap_found=`echo $line | grep "$section_start_text" -c`
		[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: wps_of_vap_found=$wps_of_vap_found" > /dev/console

		if [ "$wps_of_vap_found" = "1" ]
		#if [ "$line" = "$section_start_text" ]
		then
			[ "$enable_log" = "1" ] && echo "read_vap_name_of_wps_from_hostapd_common: set section_start ($line)" > /dev/console
			section_start=1
		fi
	done < $hostapd_conf_file_file

	# Source new hostapd values file and delete it.
	[ -e "$hostapd_get_name" ] && . $hostapd_get_name
	# For debug savefile:
	[ -e "$hostapd_get_name" ] && cp $hostapd_get_name ${hostapd_get_name}_${interface_name}
}


handle_extra_actions()
{
	# Define local parameters
	local interface_name radio_name hostapd_files_list wps_state_1 wps_state_2 \
	radio_name_hostapd hostapd_vap_name wps_active
	interface_name=$1
	radio_name=${interface_name%%.*}
	
	rm -f /tmp/wps_success_*
	# How many interfaces support WPS:
	hostapd_files_list=`ls ${CONF_DIR}/ | grep -w hostapd_wlan[0-9].conf`
	print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: hostapd_files_list=$hostapd_files_list"
	# Loop over hostapd files.
	# Get radio name,
	# get interface name of active WPS,
	# stop WPS session for all interfaces execpt current interface.
	for file in $hostapd_files_list
	do
		print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: In loop: file=$file"
		wps_state_1=`cat /${CONF_DIR}/$file | grep ^"wps_state=1"`
		wps_state_2=`cat ${CONF_DIR}/$file | grep ^"wps_state=2"`
		print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: wps_state_1=$wps_state_1, wps_state_2=$wps_state_2"
		radio_name_hostapd=`echo "$file" | sed -e 's/^.*_//' -e 's/.conf//'`
		#interface_name_hostapd=`read_vap_name_of_wps_from_hostapd_common $interface_name_tmp $radio_name $wps_state`
		hostapd_vap_name=`read_vap_name_of_wps_from_hostapd_common $radio_name_hostapd $radio_name_hostapd "wps_state=1" 0`
		print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: hostapd_vap_name=$hostapd_vap_name"
		if [ -z "$hostapd_vap_name" ]
		then
			hostapd_vap_name=`read_vap_name_of_wps_from_hostapd_common $radio_name_hostapd $radio_name_hostapd "wps_state=2" 0`
			print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: hostapd_vap_name=$hostapd_vap_name"			
			#if [ -z "$hostapd_vap_name" ]
			#then
			#	echo "handle_extra_actions: dump $file #############" > /dev/console
			#	cat /${CONF_DIR}/$file > /dev/console
			#	echo "handle_extra_actions: dump END #############" > /dev/console
			#fi
		fi
		
		print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: wps_state_1=$wps_state_1, wps_state_2=$wps_state_2, radio_name_hostapd=$radio_name_hostapd"
		if [ -n "$wps_state_1" ] || [ -n "$wps_state_2" ]
		then
			# WPS is supported on device: If WPS in progres need to stop
			print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: WPS is supported on device: If WPS in progres need to stop"
			#wps_active=`/tmp/hostapd_cli_${radio_name} -i $radio_name wps_get_status $interface_name | grep "Status: Active" -c`
			#/tmp/hostapd_cli_${radio_name_hostapd} -i $radio_name_hostapd wps_get_status $hostapd_vap_name > /dev/console
			wps_active=`/tmp/hostapd_cli_${radio_name_hostapd} -i $radio_name_hostapd wps_get_status $hostapd_vap_name | grep "Status: Active" -c`
			print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: wps_active=$wps_active, radio_name_hostapd=$radio_name_hostapd, radio_name=$radio_name"
			if [ "$wps_active" = "1" ] && [ "$radio_name_hostapd" != "$radio_name" ]
			then
				# Stop WPS session
				print2log $radio_index DEBUG_WPS "handle_extra_actions.sh: Stop WPS session hostapd_vap_name=$hostapd_vap_name"
				/tmp/hostapd_cli_${radio_name_hostapd} -i $radio_name_hostapd wps_cancel $hostapd_vap_name
			fi
		fi
	done
}

# This function prepares the logger commands to be executed in the runner
up_prepare_logger_commands()
{
	# Define local parameters
	local rtlog_bridge bridge_mac

	rtlog_bridge=$1

	# Set MAC address to the logger interface and add it to the bridge. The MAC used is the bridge MAC
	bridge_mac=""
	which uboot_env > /dev/null
	if [ $? -eq 0 ]
	then
		bridge_mac=`uboot_env --get --name ethaddr`
	else
		[ -n "`ifconfig | grep ${rtlog_bridge}`" ] && bridge_mac=`ifconfig ${rtlog_bridge} | awk '/HWaddr/ {print $5}'`
	fi
	[ -z "${bridge_mac}" ] && bridge_mac="00:00:00:00:00:10"
	echo "### Wlan logger commands ###" >> $LOGGER_COMMANDS_CONF
	echo "ifconfig rtlog0 hw ether $bridge_mac" >> $LOGGER_COMMANDS_CONF
	echo "brctl addif $rtlog_bridge rtlog0" >> $LOGGER_COMMANDS_CONF
	echo "ifconfig rtlog0 up" >> $LOGGER_COMMANDS_CONF
	# Write switch_cli command to add the logger to the switch MAC table
	which switch_cli > /dev/null
	if [ $? -eq 0 ]
	then
		[ -e /etc/rc.d/config.sh ] && . /etc/rc.d/config.sh
		if [ "$CONFIG_IFX_CONFIG_CPU" != "GRX750" ]
		then
			echo "switch_cli dev=1 GSW_MAC_TABLE_ENTRY_ADD nFId=0 nPortId=9 nSubIfId=128 bStaticEntry=1 nMAC=00:00:00:00:00:10" >> $LOGGER_COMMANDS_CONF
		fi
	fi
}

remove_upnp_iface()
{
	# Define local parameters
	local radio_name endpoint_name current_vap

	radio_name=$1
	endpoint_name=$2

	while read current_vap
	do
		[ "$current_vap" = "$endpoint_name" ] && continue
		[ -e ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${current_vap}.conf ] && sed -i '/^upnp_iface/d' ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${current_vap}.conf
	done < ${VAPS_LIST}_${radio_name}
}

ssid_delete_from_confs()
{
	# Define local parameters
	local interface_name interface_index radio_name pid\
	interface_no_dot confs_list conf

	interface_name=$1
	interface_index=$2
	radio_name=$3
	pid=$4

	# delete the conf files of the requested interface
	rm -rf ${CONF_DIR}/*${interface_name}.* ${CONF_DIR}/*${interface_name}_${pid}.conf
	# Remove VAP from VAPs list
	sed -i '/\<'$interface_name'\>/d' ${VAPS_LIST}_${radio_name}
	# Remove VAP from indexes list
	interface_no_dot=${interface_name/\./_}
	sed -i '/'$interface_no_dot'_index/d' $INTERFACES_INDEXES
	# Remove VAP from status list
	sed -i '/'$interface_name'_enabled/d' ${INTERFACES_STATUS}_${radio_name}
	# Clear VAP parameters from internal DBs
	confs_list=`ls ${CONF_DIR}/*_conf`
	for conf in $confs_list
	do
		sed -i '/_'$interface_index'=/d' $conf
	done
}

remove_index_from_local_db()
{
	# Define local parameters
	local local_db_file interface_index local_db_file_name

	local_db_file=$1
	interface_index=$2

	eval local_db_file_name=\${${local_db_file}_CONF}
	sed -i '/_'$interface_index'=/d' $local_db_file_name
}

# This function will update the status of interfaces to be set in the out.conf
# created after fapi_wlan_wave_up is called
update_interfaces_status()
{
	# Define local parameters
	local interface_name interface_type radio_name all_interfaces current_interface status_to_set \
	current_interface_no_dot ifconfig_status object_index current_status current_vap current_vap_no_dot

	interface_name=$1
	interface_type=$2
	radio_name=$3

	# Execute ifconfig command to get the specific interface/radio related VAPs/all wlan interfaces
	# For all interfaces in ifconfig output save parameter <interface>_is_up=1
	ifconfig | grep $interface_name | awk '{print $1}' > ${TEMP_DIR}/ifconfig_status
	sed -i -e 's/\./_/' -e 's/$/_is_up=1/' ${TEMP_DIR}/ifconfig_status
	. ${TEMP_DIR}/ifconfig_status
	rm -f ${TEMP_DIR}/ifconfig_status

	# For all interfaces in ifconfig -a output set status=up if is_up=1, else set down
	touch ${CONF_DIR}/notification_status
	all_interfaces=`ifconfig -a | grep $interface_name | awk '{print $1}'`
	for current_interface in $all_interfaces
	do
		status_to_set="Down"
		current_interface_no_dot=${current_interface/\./_}
		eval ifconfig_status=\${${current_interface_no_dot}_is_up}
		[ -n "$ifconfig_status" ] && [ "$ifconfig_status" = "1" ] && status_to_set="Up"
		echo "${current_interface_no_dot}_status=${status_to_set}" >> ${CONF_DIR}/notification_status
	done

	# Update the output file with the interfaces status
	object_index=0
	. ${CONF_DIR}/notification_status

	if [ "$interface_type" = "$STA" ] || [ "$interface_type" = "All" ]
	then
		eval current_status=\${${interface_name}_status}
		[ -z "$current_status" ] && continue
		update_conf_out "Object_${object_index}" "${SSID_OBJECT}"
		update_conf_out "Name_${object_index}" "$interface_name"
		update_conf_out "Status_${object_index}" "$current_status"
		object_index=$((object_index+1))
		# Send notification used by other SLs
		build_wlan_notification "servd" "NOTIFY_WIFI_SSID_STATUS_CHANGE" "Name:${interface_name} Status:${current_status}"
	fi
	if [ "$interface_type" != "$STA" ] || [ "$interface_type" = "All" ]
	then
		current_vap=""
		if [ -e ${VAPS_LIST}_${radio_name} ]
		then
			while read current_vap
			do
				current_vap_no_dot=${current_vap/\./_}
				eval current_status=\${${current_vap_no_dot}_status}
				[ -z "$current_status" ] && continue
				if [ "$current_vap" = "$radio_name" ]
				then
					# Check for Radio if the radio TX is disabled
					transmit_status=`iwpriv $radio_name gEnableRadio`
					transmit_status=${transmit_status##*:}
					transmit_status=`echo $transmit_status`
					[ "$transmit_status" = "0" ] && current_status="Down"
					update_conf_out "Object_${object_index}" "${RADIO_OBJECT}"
					update_conf_out "Name_${object_index}" "$current_vap"
					update_conf_out "Status_${object_index}" "$current_status"
					object_index=$((object_index+1))
					# Send notification used by other SLs
					build_wlan_notification "servd" "NOTIFY_WIFI_RADIO_STATUS_CHANGE" "Name:${current_vap} Status:${current_status}"
				fi
				[ "$transmit_status" = "0" ] && current_status="Down"
				update_conf_out "Object_${object_index}" "${SSID_OBJECT}"
				update_conf_out "Name_${object_index}" "$current_vap"
				update_conf_out "Status_${object_index}" "$current_status"
				object_index=$((object_index+1))
				# Send notification used by other SLs
				build_wlan_notification "servd" "NOTIFY_WIFI_SSID_STATUS_CHANGE" "Name:${current_vap} Status:${current_status}"
			done < ${VAPS_LIST}_${radio_name}
		fi
	fi
	rm -f ${CONF_DIR}/notification_status
}

# Go over the list of interfaces in the status of the interfaces (enabled/disabled) conf file.
# For disabled interfaces: rename conf files and remove related commands from OTF conf
# For enabled interfaces, check if enable online is true
# If enable online is true, check enable router flag of the VAP
# If enable router is false, rename conf files and remove related commands from OTF conf
# Create temporary script with commands to rename the conf file to original name after configuration is done
# If interface is the Radio interface, don't allow to disable it
up_check_and_handle_enable_disable()
{
	# Define local parameters
	local radio_name interface_type endpoint_name accesspoint_disabled endpoint_disabled line \
	rename_confs status interface interface_no_dot rename_confs enable_online

	radio_name=$1
	interface_type=$2
	endpoint_name=$3

	[ -e ${ENABLE_ONLINE_STATUS}_${radio_name} ] && . ${ENABLE_ONLINE_STATUS}_${radio_name}
	accesspoint_disabled=0
	endpoint_disabled=0
	while read line
	do
		rename_confs=""
		status=${line##*=}
		[ ! -e ${ENABLE_ONLINE_STATUS}_${radio_name} ] && [ "$status" = "1" ] && continue
		interface=${line%%_enabled*}
		if [ "$interface_type" = "$STA" ]
		then
			[ "$interface" != "$endpoint_name" ] && continue
		else
			[ "$interface" = "$endpoint_name" ] && continue
		fi
		interface_no_dot=${interface/\./_}
		if [ -e ${CONF_DIR}/${interface}_down ] && [ "$interface" != "$radio_name" ]
		then
			rename_confs="yes"
		else
			if [ "$status" = "1" ]
			then
				eval enable_online=\${${interface_no_dot}_enableOnLine}
				# If online enable is not true, no changes are needed
				[ -z $enable_online ] || [ "$enable_online" = "0" ] && continue
				# If router enable flag is set, no changes are needed
				[ -e ${CONF_DIR}/${interface}_EnableRouter ] && continue
				rename_confs="yes"
			else
				rename_confs="yes"
			fi
		fi

		# rename conf files of the disabled VAP or VAP that doesn't need to be started
		if [ "$rename_confs" = "yes" ]
		then
			if [ "$interface_type" != "$STA" ]
			then
				# If the interface to disable is the Radio interface, hostapd will not be started at all and no AP related commands will be set
				[ "$interface" = "$radio_name" ] && accesspoint_disabled=1 && continue
				mv ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/dont_use_${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf
				echo "mv ${CONF_DIR}/dont_use_${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf" >> ${CONF_DIR}/temp_rename.sh
				mv ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/dont_use_${HOSTAPD_VAP_CONF_PREFIX}_${interface}.conf
				echo "mv ${CONF_DIR}/dont_use_${HOSTAPD_VAP_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${interface}.conf" >> ${CONF_DIR}/temp_rename.sh
				[ -e ${CONF_DIR}/${DRIVER_SINGLE_CALL_CONFIG_FILE}_${interface} ] && mv ${CONF_DIR}/${DRIVER_SINGLE_CALL_CONFIG_FILE}_${interface} ${CONF_DIR}/dont_use_${DRIVER_SINGLE_CALL_CONFIG_FILE}_${interface}
				[ -e ${CONF_DIR}/${ALUMNUS_HS20_CONFIG_FILE}_${interface} ] && mv ${CONF_DIR}/${ALUMNUS_HS20_CONFIG_FILE}_${interface} ${CONF_DIR}/dont_use_${ALUMNUS_HS20_CONFIG_FILE}_${interface}
				[ -e ${CONF_DIR}/${OTF_CONFIG_FILE} ] && sed -i '/\<'$interface'\>/d' ${CONF_DIR}/${OTF_CONFIG_FILE}
			fi
			# If the interface is Endpoint, only handle the supplicant conf file
			if [ "$interface" = "$endpoint_name" ]
			then
				endpoint_disabled=1
				if [ -e ${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface}.conf ]
				then
					mv ${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/dont_use_${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface}.conf
					echo "mv ${CONF_DIR}/dont_use_${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${interface}.conf" >> ${CONF_DIR}/temp_rename.sh
				fi
				if [ -e ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf ]
				then
					mv ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/dont_use_${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf
					echo "mv ${CONF_DIR}/dont_use_${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf" >> ${CONF_DIR}/temp_rename.sh
				fi
			fi
		fi
	done < ${INTERFACES_STATUS}_${radio_name}
	echo "$accesspoint_disabled $endpoint_disabled"
}

# Add the drv_config_pre_up commands to the runner
up_add_pre_up_to_runner()
{
	# Define local parameters
	local radio_name

	radio_name=$1

	echo "### AccessPoint pre-up commands ###" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	cat ${CONF_DIR}/${DRIVER_PRE_UP_CONF_PREFIX}_${radio_name}.conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}

	# Request to always change adaptive sensitivity threshold to this value:
	echo "iwpriv $radio_name sSetRxTH -82" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	# Change the default maximum size of nl received buffer in the kernel
	echo "echo 262144 > /proc/sys/net/core/rmem_max" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
}

up_merge_supplicant_conf()
{
	# Define local parameters
	local endpoint_name

	endpoint_name=$1

	# Add the supplicant configuration to the supplicant conf file
	cat ${CONF_DIR}/${SUPPLICANT_CONFIGURATION_CONF_PREFIX}_${endpoint_name}.conf > ${CONF_DIR}/${SUPPLICANT_CONF_PREFIX}_${endpoint_name}.conf
	# Add the network block to the supplicant conf file
	cat ${CONF_DIR}/${SUPPLICANT_PROFILE_CONF_PREFIX}_${endpoint_name}.conf >> ${CONF_DIR}/${SUPPLICANT_CONF_PREFIX}_${endpoint_name}.conf
}

# Add the commands to start the endpoint to a conf file copied to the runner
# Commands include:
# Start supplicant
# Verify supplicant is up
# Start wpa_cli listener for events
# Add network related commands on init only
# Add post-up commands
# On init (only called on first init), save the conf file to be used after reboot
up_add_start_endpoint_to_runner()
{
	# Define local parameters
	local endpoint_name endpoint_mac radio_mac script_name endpoint_bridge wds acceleration_enabled

	endpoint_name=$1
	endpoint_mac=$2
	radio_mac=$3
	script_name=$4
	endpoint_bridge=$5
	wds=$6
	acceleration_enabled=$7

	start_supplicant_conf=${TEMP_DIR}/start_supplicant_conf_${endpoint_name}

	cat /dev/null > $start_supplicant_conf
	echo -ne "\n" >> $start_supplicant_conf
	echo "### Bring up the Endpoint ###" >> $start_supplicant_conf

	echo "cp -s ${BINDIR}/wpa_supplicant /tmp/wpa_supplicant_${endpoint_name}" >> $start_supplicant_conf

	echo "ip link set dev $endpoint_name address $endpoint_mac" >> $start_supplicant_conf
	echo "ip link set dev $radio_name address $radio_mac" >> $start_supplicant_conf

	# Try to read driver debug level from configuration file
	hostapd_debug_config_get $endpoint_name
	if [ $enable == 1 -a -n "$type" -a -n "$level" ]; then
		if [ "$type" == "syslog" ]; then
			echo "echo \"$script_name: Start wpa_supplicant_${endpoint_name} in ${level} debug mode to /var/log/wpa_supplicant_${endpoint_name}\" > /dev/console" >> $start_supplicant_conf
			echo "/tmp/wpa_supplicant_${endpoint_name} ${level} -Dnl80211 -i${endpoint_name} -c${CONF_DIR}/wpa_supplicant_${endpoint_name}.conf -e/tmp/wpa_supplicant_ent_${endpoint_name} 2>&1 | logger -t wpa_supplicant_${endpoint_name} &" >> $start_supplicant_conf
		else
			path=${CONF_DIR}
			[[ "$type" == "usb" ]] && hostapd_debug_get_usb_path path
			echo "echo \"$script_name: Start wpa_supplicant_${endpoint_name} in ${level} debug mode to ${path}/wpa_supplicant_${endpoint_name}\" > /dev/console" >> $start_supplicant_conf
			echo "/tmp/wpa_supplicant_${endpoint_name} ${level} -Dnl80211 -i${endpoint_name} -c${CONF_DIR}/wpa_supplicant_${endpoint_name}.conf -e/tmp/wpa_supplicant_ent_${endpoint_name} > ${path}/wpa_supplicant_${endpoint_name}.log &" >> $start_supplicant_conf
		fi
		echo "sleep 4" >> $start_supplicant_conf
	else
		echo "echo \"$script_name: Start wpa_supplicant_${endpoint_name}\" > /dev/console" >> $start_supplicant_conf
		echo "/tmp/wpa_supplicant_${endpoint_name} -Dnl80211 -i${endpoint_name} -c${CONF_DIR}/wpa_supplicant_${endpoint_name}.conf -e/tmp/wpa_supplicant_ent_${endpoint_name} -B" >> $start_supplicant_conf
	fi

	# Verify that supplicant is up. May need to wait if AP performs scan at this time
	echo "supplicant_running=\`is_process_running wpa_supplicant_${endpoint_name}\`" >> $start_supplicant_conf
	echo "up_timeout=0" >> $start_supplicant_conf
	echo "while [ \$supplicant_running -eq 0 ] && [ \$up_timeout -lt 90 ]" >> $start_supplicant_conf
	echo "do" >> $start_supplicant_conf
	echo "	sleep 1" >> $start_supplicant_conf
	echo "	echo \"$script_name: Start wpa_supplicant_${endpoint_name}\" > /dev/console" >> $start_supplicant_conf
	echo "	/tmp/wpa_supplicant_${endpoint_name} -Dnl80211 -i${endpoint_name} -c${CONF_DIR}/wpa_supplicant_${endpoint_name}.conf -e/tmp/wpa_supplicant_ent_${endpoint_name} -B" >> $start_supplicant_conf
	echo "	supplicant_running=\`is_process_running wpa_supplicant_${endpoint_name}\`" >> $start_supplicant_conf
	echo "	up_timeout=\$((up_timeout+1))" >> $start_supplicant_conf
	echo "done" >> $start_supplicant_conf
	echo "[ "\$up_timeout" -eq 90 ] && echo $script_name ERROR: WPA SUPPLICANT FAILED to load within 90 seconds > /dev/console && exit 1" >> $start_supplicant_conf

	# Wait for the interface to be up
	echo "up_timeout=0" >> $start_supplicant_conf
	echo "while [ \`wpa_cli -i${endpoint_name} status 2>/dev/null | grep wpa_state -c\` -eq 0 ] && [ \$up_timeout -lt 30 ]" >> $start_supplicant_conf
	echo "do sleep 1; up_timeout=\$((up_timeout+1)); done" >> $start_supplicant_conf
	# Start wpa_cli to listen to events.
	echo "cp -s ${BINDIR}/wpa_cli /tmp/wpa_cli_${endpoint_name}" >> $start_supplicant_conf
	echo "/tmp/wpa_cli_${endpoint_name} -i${endpoint_name} -a${SUPPLICANT_EVENTS_SCRIPT} -B" >> $start_supplicant_conf


	# Add the EndPoint to the bridge, unless bridge name is None
	# ASUS_EXT. If the device is amesh re node, the STA interfaces are be added by bh_ctrl
	re_mode=`nvram get re_mode`
	if [ "$re_mode" != "1" ]
	then
		[ "$endpoint_bridge" != "None" ] && echo "brctl addif ${endpoint_bridge} ${endpoint_name}" >> $start_supplicant_conf
	fi
	# Execute network related commands during init flow when WDS is disabled
	if [ $wds -eq 0 ]
	then
		# ASUS_EXT. If the device is amesh re node, the STA interfaces are be added by bh_ctrl
		if [ "$re_mode" != "1" ]
		then
			echo "if [ -e /proc/l2nat/dev ]" >> $start_supplicant_conf
			echo "then" >> $start_supplicant_conf
			echo "	[ \`cat /proc/l2nat/dev | grep $endpoint_name -c\` -eq 0 ] && echo add $endpoint_name > /proc/l2nat/dev" >> $start_supplicant_conf
			echo "else" >> $start_supplicant_conf
			echo "	echo \"/proc/l2nat/dev was not found. L2NAT is not insmoded?\" > /dev/console" >> $start_supplicant_conf
			echo "fi" >> $start_supplicant_conf
		fi
		echo "if [ -e /proc/ppa/api/bridged_flow_learning ]" >> $start_supplicant_conf
		echo "then" >> $start_supplicant_conf
		echo "	[ \`cat /proc/ppa/api/bridged_flow_learning | grep Enabled -c\` -eq 0 ] && echo enable > /proc/ppa/api/bridged_flow_learning" >> $start_supplicant_conf
		echo "fi" >> $start_supplicant_conf
		echo "ppacmd setbr -f 0" >> $start_supplicant_conf
	fi

	# Copy post-up driver commands to the runner (otf commands).
	echo -ne "\n" >> $start_supplicant_conf
	echo "### EndPoint post-up commands ###" >> $start_supplicant_conf
	# For WISP, remove any ppa commands from post_up and add EndPoint to ppa wan
	if [ -e ${SBINDIR}/ppacmd ] && [ "$acceleration_enabled" = "1" ] && [ "$endpoint_bridge" = "None" ]
	then
		sed -i '/ppacmd/d' ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${endpoint_name}.conf
		echo "ppacmd addwan -i ${endpoint_name}" >> ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${endpoint_name}.conf
	fi
	cat ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${endpoint_name}.conf >> $start_supplicant_conf

	# Copy the commands to another conf to be used on the next reboot
	cp $start_supplicant_conf ${ENDPOINT_UP_CONF}_${endpoint_name}.conf
	cat $start_supplicant_conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
}

# Add the commands to reconfigure the endpoint to the runner
# Commnds include:
# add to bridge (if needed)
# Reconfigure supplicant
# Add post-up commands
up_reconfigure_endpoint()
{
	# Define local parameters
	local endpoint_name endpoint_bridge

	endpoint_name=$1
	endpoint_bridge=$2

	# ASUS_EXT. If the device is amesh re node, the STA interfaces are be added by bh_ctrl
	re_mode=`nvram get re_mode`

	# If Endpoint is not in the bridge, add it to the bridge, unless bridge name is None
	if [ "$endpoint_bridge" != "None" ]
	then
		# ASUS_EXT. If the device is amesh re node, the STA interfaces are be added by bh_ctrl
		if [ "$re_mode" != "1" ]
		then
			echo "if [ \`brctl show | grep $endpoint_name -c\` -eq 0 ]" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
			echo "then" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
			echo "	brctl addif ${endpoint_bridge} ${endpoint_name}" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
			echo "fi" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
		fi
	fi
	echo "wpa_cli -i${endpoint_name} reconfigure" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	echo "### EndPoint post-up commands ###" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	cat ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${endpoint_name}.conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	rm ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${endpoint_name}.conf
}

up_merge_hostapd_confs()
{
	# Define local parameters
	local radio_name atf_ena vap_ena

	radio_name=$1
	atf_ena=$2
	vap_ena=$3

	# Merge hostapd configuration files
	cat ${CONF_DIR}/${HOSTAPD_PHY_CONF_PREFIX}_${radio_name}.conf ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${radio_name}.conf > ${CONF_DIR}/hostapd_${radio_name}.conf
	cat ${CONF_DIR}/${HOSTAPD_VAP_CONF_PREFIX}_${radio_name}.*.conf >> ${CONF_DIR}/hostapd_${radio_name}.conf 2>/dev/null

	## Merge hostapd atf files if ATF enabled
	# enable or disable are is the Driver scope reading the type from the conf file.
	merge_atf_conf_file $radio_name $atf_ena $vap_ena
}

# Add the commands to start the AccessPoint to a conf file copied to the runner
# Commnds include:
# Reconfigure hostapd if only VAPs were modified/added/removed
# Start hostapd if radio was modified
# Start wpa_cli listener for events if hostapd was started
up_add_start_ap_to_runner()
{
	# Define local parameters
	local radio_name script_name enable_hostapd_logs restart_radio start_hostapd_conf \
	del_interface interface reconfigure_add_del_interface reconfigure_vap

	radio_name=$1
	script_name=$2
	enable_hostapd_logs=$3
	restart_radio=$4

	start_hostapd_conf=${TEMP_DIR}/start_hostapd_conf_${radio_name}

	cat /dev/null > $start_hostapd_conf
	echo -ne "\n" >> $start_hostapd_conf
	# If restart_radio is set, start hostapd
	# If restart_radio is not set, reconfigure the VAPs
	if [ "$restart_radio" != "yes" ]
	then
		# Check if VAPs were added/deleted
		if [ "$reconfigure_add_del" = "yes" ]
		then
			echo "### Reconfigure hostapd for add/del VAPs ###" >> $start_hostapd_conf
			# Go over the list of deleted VAPs and remove them from ppa
			while read del_interface
			do
				echo "ppacmd dellan -i ${del_interface}" >> $start_hostapd_conf
			done < ${DELETED_VAPS_LIST}_${radio_name}
			rm -f ${DELETED_VAPS_LIST}_${radio_name}
			echo "/tmp/hostapd_cli_${radio_name} -i${radio_name} reconf" >> $start_hostapd_conf
		fi
		# Go over list of VAPs and check if any of the VAPs needs to be reconfigured
		while read interface
		do
			interface_no_dot=${interface/\./_}
			# No reconf for radio interfaces
			[ "$interface" = "$radio_name" ] && continue
			# Don't call reconf for new/deleted VAPs (reconf already called)
			eval reconfigure_add_del_interface=\${reconfigure_add_del_${interface_no_dot}}
			[ "$reconfigure_add_del_interface" = "yes" ] && continue
			eval reconfigure_vap=\${reconfigure_${interface_no_dot}}
			if [ "$reconfigure_vap" = "yes" ]
			then
				echo "### Reconfigure hostapd for $interface ###" >> $start_hostapd_conf
				# VAP is brought down and up by hostapd, need to remove from PPA before that
				echo "ppacmd dellan -i ${interface}" >> $start_hostapd_conf
				echo "/tmp/hostapd_cli_${radio_name} -i${radio_name} reconf $interface" >> $start_hostapd_conf
			fi
		done < ${VAPS_LIST}_${radio_name}
	else
		echo "### Start hostapd ###" >> $start_hostapd_conf
		echo "cp -s ${BINDIR}/hostapd /tmp/hostapd_${radio_name}" >> $start_hostapd_conf

		# Try to read driver debug level from configuration file
		hostapd_debug_config_get $radio_name
		if [ $enable == 1 -a -n "$type" -a -n "$level" ]; then
			if [ "$type" == "syslog" ]; then
				echo "echo \"$script_name: Start hostapd_${radio_name} in ${level} debug mode to syslog (/var/log/hostapd_${radio_name})\" > /dev/console" >> $start_hostapd_conf
				echo "/tmp/hostapd_${radio_name} ${level} ${CONF_DIR}/hostapd_${radio_name}.conf -e /tmp/hostapd_ent_${radio_name} 2>&1 | logger -t hostapd_${radio_name} &" >> $start_hostapd_conf
			else
				path=${CONF_DIR}
				[[ "$type" == "usb" ]] && hostapd_debug_get_usb_path path
				echo "echo \"$script_name: Start hostapd_${radio_name} in ${level} debug mode to ${path}/hostapd_${radio_name})\" > /dev/console" >> $start_hostapd_conf
				echo "/tmp/hostapd_${radio_name} ${level} ${CONF_DIR}/hostapd_${radio_name}.conf -e /tmp/hostapd_ent_${radio_name} > ${path}/hostapd_${radio_name}.log &" >> $start_hostapd_conf
			fi
			echo "sleep 4" >> $start_hostapd_conf
		else
			if [ -n "$enable_hostapd_logs" ] && [ $enable_hostapd_logs -gt 0 ]
			then
				echo "echo \"$script_name: Start hostapd_${radio_name} in debug mode\" > /dev/console" >> $start_hostapd_conf
				echo "/tmp/hostapd_${radio_name} -ddt ${CONF_DIR}/hostapd_${radio_name}.conf -e /tmp/hostapd_ent_${radio_name} > ${CONF_DIR}/hostapd_${radio_name}.log &" >> $start_hostapd_conf
				echo "sleep 4" >> $start_hostapd_conf
			else
				echo "echo \"$script_name: Start hostapd_${radio_name}\" > /dev/console" >> $start_hostapd_conf
				echo "/tmp/hostapd_${radio_name} ${CONF_DIR}/hostapd_${radio_name}.conf -e /tmp/hostapd_ent_${radio_name} -B" >> $start_hostapd_conf
				echo "sleep 1" >> $start_hostapd_conf
			fi
		fi

		# Start hostapd_cli to listen to events
		echo "cp -s ${BINDIR}/hostapd_cli /tmp/hostapd_cli_${radio_name}" >> $start_hostapd_conf
		echo "/tmp/hostapd_cli_${radio_name} -i${radio_name} -a${HOSTAPD_EVENTS_SCRIPT} -B" >> $start_hostapd_conf

		# Copy the commands to another conf to be used on the next reboot
		cp $start_hostapd_conf ${RADIO_UP_CONF}_${radio_name}.conf
	fi
	cat $start_hostapd_conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
}

# Add the commands to start the drvhlpr to the runner
up_add_start_drvhlpr_to_runner()
{
	# Define local parameters
	local radio_name start_drvhlpr_conf

	radio_name=$1

	start_drvhlpr_conf=${TEMP_DIR}/start_drvhlpr_conf_${radio_name}
	cat /dev/null > $start_drvhlpr_conf

	echo -ne "\n" >> $start_drvhlpr_conf
	echo "### Start drvhlpr ###" >> $start_drvhlpr_conf
	echo "cp -s ${BINDIR}/drvhlpr /tmp/drvhlpr_${radio_name}" >> $start_drvhlpr_conf

	# Restart drvhlpr only if its conf file was changed or if it isn't running
	if [ -e "${CONF_DIR}/${RESTART_DRVHLPR_FLAG}_${radio_name}" ]
	then
		echo "killall drvhlpr_${radio_name} 2>/dev/null" >> $start_drvhlpr_conf
		echo "drvhlpr_running=\`is_process_running drvhlpr_$radio_name\`" >> $start_drvhlpr_conf
		echo "while [ "\$drvhlpr_running" -eq 1 ]; do sleep 1; drvhlpr_running=\`is_process_running drvhlpr_${radio_name}\`; done" >> $start_drvhlpr_conf
		# calling script to start the drvhlpr and wait on its return value for complete recovery.
		echo "(. $ETC_PATH/fapi_wlan_wave_drvhlpr.sh ${radio_name} &)" >> $start_drvhlpr_conf
	else
		echo "drvhlpr_running=\`is_process_running drvhlpr_$radio_name\`" >> $start_drvhlpr_conf
		echo "[ \$drvhlpr_running -eq 0 ] && (. $ETC_PATH/fapi_wlan_wave_drvhlpr.sh ${radio_name} &)" >> $start_drvhlpr_conf
	fi

	# On first init, copy the commands to another conf to be used on the next reboot
	[ -e $INIT_FLAG ] && cp $start_drvhlpr_conf ${DRVHLPR_UP_CONF}_${radio_name}.conf
	cat $start_drvhlpr_conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
}

# Add the commands to wait for hostapd to be up
up_add_wait_for_hostapd_to_runner()
{
	# Define local parameters
	local radio_name wait_for_hostapd_conf

	radio_name=$1

	wait_for_hostapd_conf=${TEMP_DIR}/wait_for_hostapd_conf_${radio_name}

	cat /dev/null > $wait_for_hostapd_conf
	# Wait until hostapd_cli state is "ENABLED" or "ACS_DONE"
	echo -ne "\n" >> $wait_for_hostapd_conf
	echo "### Wait for hostapd_cli state to be ENABLED or ACS_DONE ###" >> $wait_for_hostapd_conf
	echo "loop=0" >> $wait_for_hostapd_conf
	echo "echo -n \"Waiting for hostapd radio interface to be ready.\"" >> $wait_for_hostapd_conf
	echo "while [ \$loop -lt 100 ]" >> $wait_for_hostapd_conf
	echo "do" >> $wait_for_hostapd_conf
	echo "	[ \$((loop%10)) -eq 0 ] && echo -ne \"\n Waiting for ${radio_name} to be ready\"" >> $wait_for_hostapd_conf
	echo "	[ \$loop -eq 99 ] && echo \"hostapd_cli state didn't change to ENABLED or ACS_DONE for ${radio_name}'!, hostapd may not be up!\"" >> $wait_for_hostapd_conf
	echo "	found_enabled=\`hostapd_cli -i${radio_name} status 2>/dev/null | grep state=ENABLED -c\`" >> $wait_for_hostapd_conf
	echo "	found_acs_done=\`hostapd_cli -i${radio_name} status 2>/dev/null | grep state=ACS_DONE -c\`" >> $wait_for_hostapd_conf
	echo "	found=\$((found_enabled+found_acs_done))" >> $wait_for_hostapd_conf
	echo "	if [ \$found -eq 1 ]" >> $wait_for_hostapd_conf
	echo "	then" >> $wait_for_hostapd_conf
	echo "		loop=100" >> $wait_for_hostapd_conf
	echo "	else" >> $wait_for_hostapd_conf
	echo "		sleep 1" >> $wait_for_hostapd_conf
	echo "		loop=\$((loop+1))" >> $wait_for_hostapd_conf
	echo "		hostapd_running=\`is_process_running hostapd_${radio_name}\`" >> $wait_for_hostapd_conf
	echo "		[ \$hostapd_running -eq 0 ] && echo \"$script_name: ERROR hostapd_${radio_name}\" > /dev/console && exit 1" >> $wait_for_hostapd_conf
	echo "	fi" >> $wait_for_hostapd_conf
	echo "done" >> $wait_for_hostapd_conf

	# On first init, copy the commands to another conf to be used on the next reboot
	[ -e $INIT_FLAG ] && cp $wait_for_hostapd_conf ${WAIT_HOSTAPD_CONF}_${radio_name}.conf
	cat $wait_for_hostapd_conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
}

# Add the drv_config_post_up commands to the runner
up_add_ap_post_up_to_runner()
{
	# Define local parameters
	local radio_name accesspoint_disabled restart_radio vaps_post_up post_up \
	interface interface_no_dot reconfigure_vap

	radio_name=$1
	accesspoint_disabled=$2
	restart_radio=$3

	echo -ne "\n" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	echo "### AccessPoint post-up commands ###" >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	if [ "$accesspoint_disabled" = "0" ] && [ "$restart_radio" = "yes" ]
	then
		vaps_post_up=`ls ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${radio_name}*`
		for post_up in $vaps_post_up
		do
			cat $post_up >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
		done
	elif [ "$accesspoint_disabled" = "0" ] && [ "$restart_radio" != "yes" ]
	then
		while read interface
		do
			interface_no_dot=${interface/\./_}
			[ "$interface" = "$radio_name" ] && continue
			# Add post_up commands modified/added VAPs
			eval reconfigure_vap=\${reconfigure_${interface_no_dot}}
			if [ "$reconfigure_vap" = "yes" ]
			then
				cat ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${interface}.conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
			fi
		done < ${VAPS_LIST}_${radio_name}
	else
		# If the radio AP is disabled, execute post-up commands of the radio VAP without ppacmd
		sed '/ppacmd/d' ${CONF_DIR}/${DRIVER_POST_UP_CONF_PREFIX}_${radio_name}.conf >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	fi
}

# Add the single execute commands to the runner
up_add_single_execute_to_runner()
{
	# Define local parameters
	local radio_name vaps_single_execute single_execute

	radio_name=$1

	vaps_single_execute=`ls ${CONF_DIR}/${DRIVER_SINGLE_CALL_CONFIG_FILE}_${radio_name}* 2>/dev/null`
	for single_execute in $vaps_single_execute
	do
		cat $single_execute >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
		rm -f $single_execute
	done
}

# Add HS20 related commands to the runner.
# In Init, up file called after all configuration is ready.
# In set HS up is called for the interface.
# Need to support both:
# -Init: ignore HS20_COMMANDS_SAVE
# -HS20 set: remove interface related config from HS20_COMMANDS_SAVE
up_add_hs20_commands_to_runner()
{
	# Define local parameters
	local radio_name script_name vap_name vaps_alumnus_execute alumnus_execute_debug matched_file \
	alumnus_execute delete_alumnus_files count_match_lines count_commands_save num_commands

	radio_name=$1
	script_name=$2
	vap_name=$3

	delete_alumnus_files=0

	rm -f ${CONF_DIR}/${ALUMNUS_HS20_CONFIG_FILE}_${radio_name}_debug
	vaps_alumnus_execute=`ls ${CONF_DIR}/${ALUMNUS_HS20_CONFIG_FILE}_${radio_name}* 2>/dev/null`
	alumnus_execute_debug=0
	matched_file=0
	for alumnus_execute in $vaps_alumnus_execute
	do
		delete_alumnus_files=1
		cat /dev/null > /tmp/match_lines
		if [ -e ${HS20_COMMANDS_SAVE}_$radio_name ]
		then
			grep -f $alumnus_execute ${HS20_COMMANDS_SAVE}_$radio_name >/dev/null 2>&1 > /tmp/match_lines
			count_commands_save=`wc -l < ${HS20_COMMANDS_SAVE}_$radio_name`
		else
			count_commands_save=0
		fi
		count_match_lines=`wc -l < /tmp/match_lines`
		rm /tmp/match_lines
		if [ "$count_match_lines" = "$count_commands_save" ]
		then
			echo "$script_name: Set matched_file" > /dev/console
			matched_file=1
		fi
		cat $alumnus_execute >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
		if [ "$alumnus_execute_debug" = "1" ]
		then
			cat $alumnus_execute >> ${CONF_DIR}/${ALUMNUS_HS20_CONFIG_FILE}_${radio_name}_debug
			echo "fapi_wlan_wave_up: ####################################### dump $alumnus_execute:" > /dev/console
			cat $alumnus_execute > /dev/console
			echo "fapi_wlan_wave_up: ####################################### dump END" > /dev/console
		fi
		[ -e "$INIT_FLAG" ] && cat $alumnus_execute >> ${HS20_UP_CONF}_${radio_name}
		rm -f $alumnus_execute
	done
	#echo "$script_name: vap_name=$vap_name, radio_name=$radio_name" > /dev/console

	[ -e ${HS20_COMMANDS_SAVE}_${radio_name} ] && cp ${HS20_COMMANDS_SAVE}_${radio_name} ${HS20_COMMANDS_SAVE}_${radio_name}_tmp
	if [ -n "$vaps_alumnus_execute" ]
	then
		# Remove interface commands from file:
		#echo "$script_name: dump ${HS20_COMMANDS_SAVE}_${radio_name}_tmp before:" > /dev/console
		cat ${HS20_COMMANDS_SAVE}_${radio_name}_tmp > /dev/console
		sed -i '/'${vap_name}'$/d' ${HS20_COMMANDS_SAVE}_${radio_name}_tmp
		sed -i '/'${vap_name}' /d' ${HS20_COMMANDS_SAVE}_${radio_name}_tmp
		#echo "$script_name: dump ${HS20_COMMANDS_SAVE}_${radio_name}_tmp after:" > /dev/console
		cat ${HS20_COMMANDS_SAVE}_${radio_name}_tmp > /dev/console
		num_commands=`cat ${HS20_COMMANDS_SAVE}_${radio_name}_tmp | grep hs_cli -c`
		if [ "$num_commands" = "0" ]
		then
			#echo "$script_name: No commands in ${HS20_COMMANDS_SAVE}_${radio_name}_tmp file, remove it" > /dev/console
			rm ${HS20_COMMANDS_SAVE}_${radio_name}_tmp
		fi
	fi
	#echo "$script_name: INIT_FLAG=${INIT_FLAG}" > /dev/console
	if [ ! -e ${INIT_FLAG} ] && [ -e ${HS20_COMMANDS_SAVE}_${radio_name}_tmp ] && [ "$matched_file" != "1" ]
	then
		#echo "$script_name: Add ${HS20_COMMANDS_SAVE}_${radio_name}_tmp file to runner" > /dev/console
		cat ${HS20_COMMANDS_SAVE}_${radio_name}_tmp >> ${CONF_DIR}/${FAPI_WLAN_WAVE_RUNNNER}
	fi
	[ -e "$INIT_FLAG" ] && cat ${HS20_COMMANDS_SAVE}_${radio_name}_tmp>> ${HS20_UP_CONF}_${radio_name}
	echo "$delete_alumnus_files"
}

# Rename disabled interfaces conf files to origianl name and remove needed files
up_clean_and_restore()
{
	# Define local parameters
	local radio_name interface_name

	radio_name=$1
	interface_name=$2

	# Rename disabled interfaces conf files to origianl name
	if [ -e "${CONF_DIR}/temp_rename.sh" ]
	then
		chmod +x ${CONF_DIR}/temp_rename.sh
		${CONF_DIR}/temp_rename.sh
		rm -f ${CONF_DIR}/temp_rename.sh
	fi

	# Delete the OTF file and all "dont_use" files
	rm -f ${CONF_DIR}/${OTF_CONFIG_FILE} ${CONF_DIR}/dont_use_*

	# Delete flags
	rm -f ${CONF_IN_PROGRESS}_${radio_name}* ${ENABLE_HOSTAPD}_${radio_name} ${VAPS_LIST}_*_tmp
}

is_process_running()
{
	local process_name

	process_name=$1
	pgrep -f $process_name > /dev/null

	if [ $? -eq 0 ]
	then
		echo "1"
	else
		echo "0"
	fi
}

set_restart_and_reconfigure()
{
	local interface_name radio_name interface_no_dot

	interface_name=$1
	radio_name=$2

	# If interface is VAP and not radio, set reconfigure flag
	if [ "$radio_name" != "$interface_name" ]
	then
		interface_no_dot=${interface_name/\./_}
		echo "reconfigure_${interface_no_dot}=yes" >> ${CONF_DIR}/${RESTART_FLAG}_${radio_name}
	else
		echo "restart_${radio_name}=yes" >> ${CONF_DIR}/${RESTART_FLAG}_${radio_name}
	fi
}
LIB_COMMON_SOURCED="1"
