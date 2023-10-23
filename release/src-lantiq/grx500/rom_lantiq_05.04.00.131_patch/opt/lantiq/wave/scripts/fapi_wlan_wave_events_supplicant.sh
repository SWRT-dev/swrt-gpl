#!/tmp/wireless/lantiq/bin/sh

export PATH=/tmp/wireless/lantiq/bin/:$PATH
script_name="$0"

[ ! "$LIB_COMMON_SOURCED" ] && . /tmp/fapi_wlan_wave_lib_common.sh

# input examples:
#   EndPoint connected:
#   /opt/lantiq/wave/scripts/fapi_wlan_wave_events_supplicant.sh wlan
interface_name=$1
name=$2
init=$3
type=$4
reg_domain=$5

#echo "$script_name: $*" > /dev/console
# Find the interface index and the radio index
interface_index=`find_index_from_interface_name $interface_name`
local_db_source SSID
ssid_type=`db2fapi_convert regular X_LANTIQ_COM_Vendor_SsidType $interface_index`
if [ "$ssid_type" = "EndPoint" ]
then
	radio_name=`get_radio_name_from_endpoint $interface_name`
else
	radio_name=${interface_name%%.*}
fi
radio_index=`find_index_from_interface_name $radio_name`

endpoint_connected()
{
	# Define local parameters
	local interface_name

	interface_name=$1

	# When an endpoint is connected:
	# 1. Update the BSSID of the remote AP in SSID object
	# 2. Update the status of the EndPoint to Connected

	# Read the BSSID using wpa_cli
	bssid=`wpa_cli -i${interface_name} status | grep ^bssid=`
	bssid=${bssid##bssid=}
	build_wlan_notification "servd" "NOTIFY_WIFI_UPDATE_PARAM" "Name:${interface_name} Object:${SSID_OBJECT} BSSID:${bssid}"
	build_wlan_notification "servd" "NOTIFY_WIFI_UPDATE_PARAM" "Name:${interface_name} Object:${ENDPOINT_OBJECT} X_LANTIQ_COM_Vendor_ConnectionStatus:Connected"
}

endpoint_disconnected()
{
	# Define local parameters
	local interface_name

	interface_name=$1

	# When an endpoint is disconnected:
	# 1. Remove the BSSID of the remote AP in SSID object (set value of NULL in the notification, SL converts it to empty string)
	# 2. Update the status of the EndPoint to Disconnected

	build_wlan_notification "servd" "NOTIFY_WIFI_UPDATE_PARAM" "Name:${interface_name} Object:${SSID_OBJECT} BSSID:NULL"
	build_wlan_notification "servd" "NOTIFY_WIFI_UPDATE_PARAM" "Name:${interface_name} Object:${ENDPOINT_OBJECT} X_LANTIQ_COM_Vendor_ConnectionStatus:Disconnected"
}

regulatory_domain_changed()
{
	# Define local parameters
	local interface_name country radio_name radio

	interface_name=$1
	country=$2

	country=${country#*=}
	if [ ! -e "${REG_DOMAIN_SET_FLAG}_${interface_name}" ]
	then
		# Notify about regulatory domain change only when EndPoint should be connected:
		# There is ProfileReference in the EndPoint and the profile status is "Active".
		local_db_source ENDPOINT
		local_db_source PROFILE

		profile_name=`db2fapi_convert regular ProfileReference $interface_index`
		[ -z "$profile_name" ] && exit
		profile_status=`db2fapi_convert regular Status $interface_index`
		[ "$profile_status" != "Active" ] && exit
		touch ${REG_DOMAIN_SET_FLAG}_${interface_name}
		echo "$script_name: Notifying APs about regulatory domain change to $country" > /dev/console

		radios_list=`ls ${INTERFACES_STATUS}*`
		for radio in $radios_list
		do
			radio=${radio#${INTERFACES_STATUS}_}
			build_wlan_notification "servd" "NOTIFY_WIFI_UPDATE_PARAM" "Name:${radio} Object:${RADIO_OBJECT} RegulatoryDomain:${country} "
		done
		echo "$script_name: Executing iw reg set $country" > /dev/console
		iw reg set $country > /dev/console
	fi
}

wps_success()
{
	# Define local parameters
	local interface_name

	interface_name=$1

	# Notify the web and the DB that status is "Success"
	build_wlan_notification "wsd" "NOTIFY_WPS_STATUS" "message:Success"
	#build_wlan_notification "servd" "NOTIFY_WIFI_WPS_STATUS" "Name:${interface_name} Status:Success"
	build_wlan_notification "servd" "NOTIFY_WIFI_UPDATE_PARAM" "Name:$interface_name Object:${ENDPOINT_WPS_OBJECT} X_LANTIQ_COM_Vendor_WPSStatus:Success"
}

scan_completed()
{
	# Define local parameters
	local interface_name

	interface_name=$1

	touch ${ENDPOINT_SCAN_COMPLETED_EVENT}_${interface_name}

	local LOCKFILE="/var/lock/scanned_bss.lock"

	#while [ -f $LOCKFILE ]
	#do
	#	sleep 1
	#done

	#touch $LOCKFILE

	( 
		flock -x 300 || exit 1
		#echo "In critical section" > /dev/console
		wpa_cli -i$interface_name scan_results simple > /tmp/scanned_bss
		#wpa_cli -i$interface_name scan_results simple > /dev/console
	) 300>$LOCKFILE

	#rm -f $LOCKFILE
}

escape_slashes()
{
    sed 's/\//\\\//g' 
}

change_line()
{
    local OLD_LINE_PATTERN=$1
    local NEW_LINE=$2
    local FILE=$3

    local NEW=$(echo "${NEW_LINE}" | escape_slashes)
    sed -i '/'"${OLD_LINE_PATTERN}"'/s/.*/'"${NEW}"'/' "${FILE}"
    #mv "${FILE}.bak" /tmp/
}

save_event()
{
	local TARGETFILE="/tmp/passive_scanned_bss"
	local LOCKFILE="/var/lock/passive_scanned_bss.lock"
	local bssid=$1
	local vsie=$2

	#while [ -f $LOCKFILE ]
	#do
	#	sleep 1
	#done

	#touch $LOCKFILE
	( 
		flock -x 200 || exit 1
		#echo "In critical section" > /dev/console
		if [ -f $TARGETFILE ]
		then
			if grep -q $bssid "$TARGETFILE"; then #chang the line
				change_line $bssid "bssid=$bssid vsie=$vsie" $TARGETFILE
			else  #append to end of target
				echo "bssid=$bssid vsie=$vsie" >> $TARGETFILE
		 	fi
		else
			echo "bssid=$bssid vsie=$vsie" > $TARGETFILE
		fi
	) 200>$LOCKFILE

	#rm -f $LOCKFILE
}

case $name in
	"CONNECTED")
		endpoint_connected $interface_name
	;;
	"DISCONNECTED")
		endpoint_disconnected $interface_name
	;;
	"CTRL-EVENT-REGDOM-CHANGE")
		[ -n "$reg_domain" ] && regulatory_domain_changed $interface_name $reg_domain
	;;
	"WPS-SUCCESS")
		wps_success $interface_name
	;;
	"CTRL-EVENT-SCAN-RESULTS")
		scan_completed $interface_name
	;;
	"CTRL-EVENT-BEACON" | "CTRL-EVENT-PROBE-RSP")
#		if [[ "{$4:0:12}" = "f832e4010102" ]]
#		then
#			echo "bssid=$3 vsie=$4" > /tmp/passive_scanned_bss
#		fi
#		echo "bssid=$3 vsie=$4" > /dev/console
		if [ "${4:0:12}" = "f832e4010102" ] || [ "${4:0:12}" = "f832e4010104" ]
		then
			save_event $3 $4
		fi
	;;
	*)
		echo "$script_name: $name"
	;;
esac
