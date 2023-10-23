#!/bin/sh

script_name="$0"

[ ! "$LIB_COMMON_SOURCED" ] && . /tmp/fapi_wlan_wave_lib_common.sh

export HOSTAPD_PIN_REQ=/var/run/hostapd.pin-req
export WPS_PIN_TEMP_FILE=${CONF_DIR}/wps_current_pin
export WPS_MAC_TEMP_FILE=${CONF_DIR}/wps_current_mac

# Read uuid from DB. If no value is set, generate new uuid
# Return the uuid.
read_uuid()
{
	# Define local parameters
	local interface_index uuid

	interface_index=$1

	uuid=`db2fapi_convert regular UUID $interface_index`
	if [ -z "$uuid" ]
	then
		which uuidgen > /dev/null
		if [ $? -eq 0 ]
		then
			uuid=`uuidgen`
		else
			uuid=`cat /proc/sys/kernel/random/uuid`
		fi
	fi

	echo "$uuid"
}

set_ap_configured()
{
	local interface_name wps_vendor_index interface_index radio_index wps_state

	interface_name=$1
	wps_vendor_index=$2
	interface_index=$3
	radio_index=$4

	print2log $radio_index DEBUG "$script_name: Start set_ap_configured"

	wps_state=`convert_wps_state $interface_index`

	# If it is a non-configured AP
	if [ "$wps_state" = "$WPS_ENABLED_NOT_CONFIGURED" ]
	then
		print2log $radio_index DEBUG "$script_name: set_ap_configured. AP is non-configured, setting to configured"
		update_conf_out "Object_${wps_vendor_index}" "$ACCESSPOINT_WPS_VENDOR_OBJECT"
		update_conf_out "ConfigState_${wps_vendor_index}" "Configured"
		# TBD: need to add ubus call that will re-trigger the WPS PBC with AP as configured. concider: how much time will it all take?
	else
		print2log $radio_index DEBUG "$script_name: set_ap_configured. AP is in configured mode"
	fi
	print2log $radio_index DEBUG "$script_name: End set_ap_configured"
}

wps_connect_via_pbc()
{
	local interface_name radio_name wps_vendor_index interface_index radio_index

	interface_name=$1
	radio_name=$2
	wps_vendor_index=$3

	# Find the interface index and the radio index
	interface_index=`find_index_from_interface_name $interface_name`
	radio_index=`find_index_from_interface_name $radio_name`

	print2log $radio_index WPS "FAPI_WLAN_VENDOR_WAVE: The button 'Start PBC' was activated for $interface_name"

	# send WPS-SESSION-START event to the WLAN events script
	( . $HOSTAPD_EVENTS_SCRIPT $radio_name WPS-SESSION-START $interface_name )
	# Set AP to configured mode.
	set_ap_configured $interface_name $wps_vendor_index $interface_index $radio_index
	# Start the PBC session
	/tmp/hostapd_cli_${radio_name} -i $interface_name wps_pbc
}

# Start a WPS PIN session for the AP
wps_connect_via_pin()
{
	local interface_index interface_name radio_name wps_vendor_index enrollee_mac enrollee_uuid \
	radio_index enrollee_pin enrollee_type wps_pin_timeout line_timestamp line_uuid line_mac param4 \
	line res need_uuid

	interface_index=$1
	interface_name=$2
	radio_name=$3
	wps_vendor_index=$4
	enrollee_mac=$5
	enrollee_uuid=$6

	# Find the radio index
	radio_index=`find_index_from_interface_name $radio_name`

	# Read enrollee PIN
	enrollee_pin=`db2fapi_convert regular EndpointPIN $interface_index`
	[ -z "$enrollee_pin" ] && . $WPS_PIN_TEMP_FILE

	# If enrollee MAC is missing, check if authorized MAC is set
	[ -z "$enrollee_mac" ] && enrollee_mac=`db2fapi_convert regular AuthorizedMac $interface_index`
	# Make enrollee_mac lower case as appears in the pin requests file
	enrollee_mac=`echo "$enrollee_mac" | tr '[:upper:]' '[:lower:]'`

	enrollee_type=2
	# Using hard-coded timeout of 30 minutes
	wps_pin_timeout=1800

	print2log $radio_index WPS "FAPI_WLAN_VENDOR_WAVE: The button 'Connect' for PIN connection was activated for $interface_name with PIN=$enrollee_pin and MAC=$enrollee_mac"

	# If PIN wasn't received, show warning and exit
	[ -z "$enrollee_pin" ] && print2log $radio_index WARNING "$script_name: PIN connection started but no PIN number received" && exit

	# Save PIN and MAC of STA in temp files
	echo "enrollee_pin=$enrollee_pin" > $WPS_PIN_TEMP_FILE
	echo "enrollee_mac=$enrollee_mac" > $WPS_MAC_TEMP_FILE

	# send WPS-SESSION-START event to the WLAN events script
	( . $HOSTAPD_EVENTS_SCRIPT $radio_name WPS-SESSION-START $interface_name )
	# Set AP to configured mode.
	set_ap_configured $interface_name $wps_vendor_index $interface_index $radio_index

	# Check if web selection was to accept all incoming PIN connectios with correct PIN. In such case, no MAC and timeout are needed.
	if [ -z "$enrollee_mac"  ]
	then
		enrollee_uuid=any
		enrollee_mac=""
		wps_pin_timeout=""
	fi

	# If no uuid was received, need to read from pin_req file or generate new random
	if [ -z "$enrollee_uuid" ]
	then
		if [ -e $HOSTAPD_PIN_REQ ]
		then
			# Get the MAC from the file and compare with requesting MAC
			# Get the MACs and uuids from the file
			while read line_timestamp line_uuid line_mac param4
			do
				if [ "$enrollee_mac" = "$line_mac" ]
				then
					enrollee_uuid=$line_uuid
					break
				fi
			done < $HOSTAPD_PIN_REQ
		fi
		# If file list doesn't exist or MAC wasn't found in list, generate random uuid
		[ -z "$enrollee_uuid" ] && enrollee_uuid=`uuidgen`
	fi
	enrollee_pin=`echo $enrollee_pin | sed -e 's/-//g' -e 's/ //g'`
	/tmp/hostapd_cli_${radio_name} -i$interface_name wps_pin $enrollee_uuid $enrollee_pin $wps_pin_timeout $enrollee_mac
}

# Generate a new PIN number for the AP and return new value to the fapi_wlan_wave_out.conf
wps_generate_pin()
{
	local interface_name radio_name wps_vendor_index new_pin

	interface_name=$1
	radio_name=$2
	wps_vendor_index=$3

	new_pin=`/tmp/hostapd_cli_${radio_name} -i$interface_name wps_ap_pin random`

	# Write the new AP PIN to out.conf
	update_conf_out "PIN_${wps_vendor_index}" "$new_pin"
}

# Cancel currently running WPS session
cancel_wps()
{
	# Define local parameters
	local interface_name radio_name

	interface_name=$1
	radio_name=$2

	/tmp/hostapd_cli_${radio_name} -i$interface_name wps_cancel
}

# Read updated values in hostapd.conf after external registrar configured it.
# The updated values changed by the external registrar are saved in the hostapd conf file.
# These values are in a section starting with the comment "WPS configuration - START" and ending with the comment "WPS configuration - END".
# Read the updated parameters in the new section to a file and source this file.
read_new_hostapd_values()
{
	# Define local parameters
	local interface_name radio_name section_start_text section_end_text section_start hostapd_new_values line

	interface_name=$1
	radio_name=$2

	section_start_text="# WPS configuration - START"
	section_end_text="# WPS configuration - END"
	section_start=""
	hostapd_new_values=${CONF_DIR}/hostapd_new_values_${interface_name}

	# Go over the updated hostapd.conf file and find the needed parameters.
	while read line
	do
		# If found ending comment, exit the loop.
		[ "$line" = "$section_end_text" ] && break
		if [ "$section_start" ]
		then
			# Write the parameter to the temp file with the prefix "hostapd_"
			echo "hostapd_$line" >> $hostapd_new_values
		fi
		# If found starting comment, set the flag.
		[ "$line" = "$section_start_text" ] && section_start=1
	done < ${CONF_DIR}/hostapd_${radio_name}.conf

	# Source new hostapd values file and delete it.
	[ -e "$hostapd_new_values" ] && . $hostapd_new_values
	rm -f $hostapd_new_values
}

# Save the settings after WPS external registrar session from the hostapd conf file to the database.
# Notify SL that an update of the DB after external registrar is needed using ubus call with the parameters to update
wps_external_done()
{
	local interface_name interface_index radio_name radio_index nid \
	param_index ubus_call db_security_mode wep_key_length wpa_type key_management

	interface_name=$1

	# Find the interface index and the radio index
	interface_index=`find_index_from_interface_name $interface_name`
	radio_name=${interface_name%%.*}
	radio_index=`find_index_from_interface_name $radio_name`

	print2log $radio_index DEBUG "$script_name: Start wps_external_done"

	# Check hostapd configuration file exists.
	[ ! -e ${CONF_DIR}/hostapd_${radio_name}.conf ] && print2log $radio_index ERROR "$script_name: Aborted. hostapd_${radio_name}.conf file doesn't exist" && exit 1

	# Read the updated parameters from hostapd conf file
	read_new_hostapd_values $interface_name $radio_name

	# Convert security hostapd values to DB values
	db_security_mode=""
	
	# If wpa is 0, security is wep or open.
	if [ "$hostapd_wpa" = "0" ]
	then
		db_security_mode="None"
		# Check if WEP by checking if wep_default_key was set.
		if [ "$hostapd_wep_default_key" ]
		then
			db_security_mode="WEP-64"
			# Check if key is 128
			wep_key_length=`echo $hostapd_wep_key | wc -L`
			if [ $wep_key_length = 13 ] || [ $wep_key_length = 26 ]
			then
				db_security_mode="WEP-128"
			fi
		fi
	else # Security is WPA 
		if [ "$hostapd_wpa_pairwise" ] && [ "$hostapd_wpa_pairwise" = "CCMP" ]
		then
			wpa_type="WPA2"
		else
			wpa_type="WPA"
			[ "$hostapd_rsn_pairwise" ] && [ "$hostapd_rsn_pairwise" = "CCMP" ] && wpa_type="WPA-WPA2"
		fi
		# Check wpa_key_mgmt to see if in WPA-personal or WPA-enterparise security mode.
		key_management="Personal"
		[ "$hostapd_wpa_key_mgmt" ] && [ "$hostapd_wpa_key_mgmt" = "WPA-EAP" ] && key_management="Enterprise"
		db_security_mode="${wpa_type}-${key_management}"
	fi
	
	# Create ubus command for SL to update DB with new values
	# Source ugw_notify_defs.sh
	. /etc/ugw_notify_defs.sh
	nid=$NOTIFY_WIFI_WPS_NEW_AP_SETTINGS
	param_index=1

	ubus_call=${CONF_DIR}/ubus_call.sh

	ubus_command="ubus call servd notify '{\"nid\":$nid,\"type\":false,\"pn${param_index}\":\"ObjectName\",\"pv${param_index}\":\"$SSID_OBJECT\","
	param_index=$((param_index+1))
	ubus_command="${ubus_command}\"pn${param_index}\":\"SSID\",\"pv${param_index}\":\"$hostapd_ssid\","
	param_index=$((param_index+1))
	ubus_command="${ubus_command}\"pn${param_index}\":\"Name\",\"pv${param_index}\":\"$interface_name\","
	param_index=$((param_index+1))
	ubus_command="${ubus_command}\"pn${param_index}\":\"ObjectName\",\"pv${param_index}\":\"$ACCESSPOINT_SECURITY_OBJECT\","
	param_index=$((param_index+1))
	ubus_command="${ubus_command}\"pn${param_index}\":\"ModeEnabled\",\"pv${param_index}\":\"$db_security_mode\","
	param_index=$((param_index+1))
	# If security is WPA/WPA2/mixed, set paddphrase
	if [ ! -z "$hostapd_wpa_passphrase" ]
	then
		ubus_command="${ubus_command}\"pn${param_index}\":\"KeyPassphrase\",\"pv${param_index}\":\"$hostapd_wpa_passphrase\","
		param_index=$((param_index+1))
	fi
	# If security is WEP, set WEP key
	if [ ! -z "$hostapd_wep_key" ]
	then
		ubus_command="${ubus_command}\"pn${param_index}\":\"WEPKey\",\"pv${param_index}\":\"$hostapd_wep_key\","
		param_index=$((param_index+1))
	fi
	ubus_command="${ubus_command}\"pn${param_index}\":\"ObjectName\",\"pv${param_index}\":\"$ACCESSPOINT_WPS_VENDOR_OBJECT\","
	param_index=$((param_index+1))
	ubus_command="${ubus_command}\"pn${param_index}\":\"ConfigState\",\"pv${param_index}\":\"Configured\"}'"

	# Make the script calling the ubus executable and execute it
	echo "$ubus_command" > $ubus_call
	chmod +x $ubus_call
	$ubus_call
	rm -f $ubus_call

	print2log $radio_index DEBUG "fapi_wlan_wave_wps.sh: End wps_external_done"
}
LIB_WPS_SOURCED="1"
