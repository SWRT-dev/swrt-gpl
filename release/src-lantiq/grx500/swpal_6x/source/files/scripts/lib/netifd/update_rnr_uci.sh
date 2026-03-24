#usage
#/lib/netifd/update_rnr_uci.sh enable => enable RNR
#/lib/netifd/update_rnr_uci.sh disable => disable RNR
#/lib/netifd/update_rnr_uci.sh unsolicited_frame probe => enable 20TU_probe
#/lib/netifd/update_rnr_uci.sh unsolicited_frame fils => enable FILS
#/lib/netifd/update_rnr_uci.sh unsolicited_frame disable => disable unsolicited_frame

phy_info_for_6g()
{
	local phys=$(ls /sys/class/ieee80211/)

	for phy in $phys
	do
		`iw $phy info | grep "* 60.. MHz" > /dev/null`
		is_phy_6g=$?

		if [ $is_phy_6g = '0' ]; then
			echo $phy
			return
		fi
	done
	echo "0"
	return
}

ubus_call_set()
{
	var1=\"$1\"	
	var2=\"$2\"
	var3=\"$3\"
	if [ $# == 3 ]; then
		#ubus call uci set '{ "config" : "wireless" , "section" : '$var1', "values": { '$var2' : '"$var3"'}}'
		uci set wireless.$1.$2="$3"
	else
		echo "usage: ubus_set_call <section_name> <param_name> <param_val>"
	fi
}

ubus_call_delete()
{
	var1=\"$1\"	
	var2=\"$2\"
	var3=\"$3\"
	if [ $# == 3 ]; then
		#ubus call uci delete '{ "config" : "wireless" , "section" : '$var1', "type": '$var2', "option" : '$var3'}'
		uci delete wireless.$1.$3
	else
		echo "usage: ubus_delete_call <section_name> <param_type> <param_name>"
	fi
}

ubus_call_add_list()
{
	var1=\"$1\"	
	var2=\"$2\"
	var3=\"$3\"
	var4=\"$4\"
	if [ $# == 4 ]; then
		#ubus call uci add '{ "config" : "wireless" , "type" : '$var1', "name": '$var2', "values" : { '$var3' : ['"$var4"']}}'
		uci add_list wireless.$2.$3="$4"
	else
		echo "usage: ubus_add_list_call <type> <section_name> <param_name> <param_value>"
	fi
}

ubus_call_commit()
{
	uci commit wireless
	return
}

if [ "$1" == "unsolicited_frame" ]; then
	# Find 6GHz interface
	radio_interface=`uci show wireless | grep ".band" | grep 6GHz | cut -d. -f2`
	case "$2" in
	disable)
		ubus_call_set "$radio_interface" "unsolicited_frame_support" "0"
		;;
	probe)
		ubus_call_set "$radio_interface" "unsolicited_frame_support" "1"
		;;
	fils)
		ubus_call_set "$radio_interface" "unsolicited_frame_support" "2"
		;;
	esac
	ubus_call_commit
	exit
fi


# Find 2.4GHz interface
radio_interface=`uci show wireless | grep ".band" | grep 2.4GHz | cut -d. -f2`
default_radio_24g=`uci show wireless | grep "device='$radio_interface'"| cut -d. -f2`
for i in $radio_interface
do
	radio_24GHz=`echo $i | cut -d"." -f2`
	radio_24GHz_array=$radio_24GHz_array" "$radio_24GHz
done

# Find 5GHz interface
radio_interface=`uci show wireless | grep ".band" | grep 5GHz | cut -d. -f2`
default_radio_5g=`uci show wireless | grep "device='$radio_interface'"| cut -d. -f2`
for i in $radio_interface
do
	radio_5GHz=`echo $i | cut -d"." -f2`
	radio_5GHz_array=$radio_5GHz_array" "$radio_5GHz
done


# Find 6GHz interface
radio_6GHz_array=""
radio_interface=`uci show wireless | grep ".band" | grep 6GHz | cut -d. -f2`

default_radio=`uci show wireless | grep "device='$radio_interface'"| cut -d. -f1-2`

if [ "$1" = "he_beacon" ]; then
        if [ "$2" = "enable" ]; then
                ubus_call_set "$radio_interface" "he_beacon" "1"
        else
                ubus_call_delete "$radio_interface" "$string" "he_beacon"
        fi
        ubus_call_commit
        exit
fi

for i in $default_radio

do

        radio_6GHz=`uci show $i".ifname" | cut -d"=" -f2`

        radio_6GHz=`echo $radio_6GHz | awk '{print substr($0, 2, length($0) - 2)}'`

        radio_6GHz_array=$radio_6GHz_array" "$radio_6GHz

done

#if enable or disable rnr, first delete existing entry as it is list param
if [ $1 = "enable" -o $1 = "disable" ]; then
	rnr="DELETE_RNR $bssid"
	for j in $default_radio_24g
	do
		ubus_call_delete "$j" "string" "colocated_6g_radio_info"
		ubus_call_delete "$j" "string" "colocated_6g_vap_info"
	done
	for j in $default_radio_5g
	do
		ubus_call_delete "$j" "string" "colocated_6g_radio_info"
		ubus_call_delete "$j" "string" "colocated_6g_vap_info"
	done

fi

for i in $radio_6GHz_array

do

	if [ $i = "wlan4" ] || [ $i = "wlan5" ]; then
		continue
	fi

	echo "$i"

	if [ $1 = "enable" ]; then

		#prepare 6GHz rnr template
                bss=`uci show wireless |  grep $i | cut -d"." -f1-2`
                radio=`uci show wireless | grep 6GHz | cut -d"." -f1-2`

		bssid=`uci show $bss.macaddr | cut -d"=" -f2 | tr -d "'"`
                channel=`uci show $radio.channel | cut -d"=" -f2 | tr -d "'"`

                ssid=`uci show $bss.ssid | cut -d"=" -f2 | tr -d "'"`

                bw=`uci show $radio.htmode | cut -d"=" -f2 | tr -d "'VHT"`
		case $bw in

		20)

			op_class="131"

			;;

		40)

			op_class="132"

			;;

		80)

			op_class="133"

			;;

		160)

			op_class="134"

			;;

		80+80)

			op_class="135"

			;;

		*)
			echo "Not correct bw"
			exit
			;;

		esac
		
                multibss_enable=`uci show $radio | grep multibss_enable=1`
                [ -n "$multibss_enable" ] && multibss_enable=1
                [ -z "$multibss_enable" ] && multibss_enable=0

                is_transmitted_bssid=0
                [ "$i" = "wlan4.0" ]  && [ "$multibss_enable" = "1" ] && is_transmitted_bssid=1
                echo $i $multibss_enable $is_transmitted_bssid
                unsolicited_frame_support=0
                if [ "$i" = "wlan4.0" ]; then
                        unsolicited_frame_support_enable=`uci show $radio | tr -d "'" | grep unsolicited_frame_support=1`
                        [ -n "$unsolicited_frame_support_enable" ]  && unsolicited_frame_support=1
                fi

                if [ "$unsolicited_frame_support" == "1" ]; then
                    set_20_tu_probe_response=1
                else
                    set_20_tu_probe_response=0
                fi
		chan="\["$channel"\]"
		phy_6g=`phy_info_for_6g`
		if [ "$phy_6g" = "0" ]; then
			echo "No 6g interface found"
			exit
		fi
		max_tx_power=`iw $phy_6g info | grep $chan  | cut -d"(" -f2 | cut -d" " -f1 | cut -d"." -f1`
		rnr="UPDATE_RNR $bssid $op_class $channel $ssid $multibss_enable $is_transmitted_bssid $set_20_tu_probe_response $max_tx_power"
	
		for j in $default_radio_24g
		do
			ubus_call_set "$j" "colocated_6g_radio_info" "0 $op_class $channel"
			ubus_call_add_list "wifi-device" "$j" "colocated_6g_vap_info" "0 $bssid $ssid $multibss_enable $is_transmitted_bssid $set_20_tu_probe_response $max_tx_power"
		done
		for j in $default_radio_5g
		do
			ubus_call_set "$j" "colocated_6g_radio_info" "0 $op_class $channel"
			ubus_call_add_list "wifi-device" "$j" "colocated_6g_vap_info" "0 $bssid $ssid $multibss_enable $is_transmitted_bssid $set_20_tu_probe_response $max_tx_power"
		done
	fi

	ubus_call_commit

done
