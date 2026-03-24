# Find 6GHz interface

radio_interface=`uci show wireless | grep ".band" | grep 6GHz | cut -d. -f2`

default_radio=`uci show wireless | grep "device='$radio_interface'"| cut -d. -f1-2`

for i in $default_radio

do

	radio_6GHz=`uci show $i".ifname" | cut -d"=" -f2`

	radio_6GHz=`echo $radio_6GHz | awk '{print substr($0, 2, length($0) - 2)}'`

	radio_6GHz_array=$radio_6GHz_array" "$radio_6GHz

done

for i in $radio_6GHz_array

do

	if [ $i = "wlan4" -o $i = "wlan5" ]; then

		continue

	fi

	bssid=`iw dev $i info | grep addr | cut -d" " -f2`

	if [ $1 = "enable" ]; then

		#prepare 6GHz rnr template

		channel=`hostapd_cli -i$i status | grep "channel=" | grep -v "_channel" | cut -d"=" -f2`

		ssid=`iw dev $i info | grep ssid | cut -d" " -f2`

		bw=`hostapd_cli -i$i radio_info | grep OperatingChannelBandwidt | cut -d"=" -f2`

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

			op_class="131"

		esac

		multibss_enable=`hostapd_cli -i$i get_multibss_enable`

		is_transmitted_bssid=`hostapd_cli -i$i is_transmitted_bssid`

		unsolicited_frame_support=`hostapd_cli -i$i get_unsolicited_frame_support`
		if [ "$unsolicited_frame_support" == "1" ]; then
		    set_20_tu_probe_response=1
		else
		    set_20_tu_probe_response=0
		fi
		max_tx_power=`hostapd_cli -i$i get_max_tx_power`
		rnr="UPDATE_RNR $bssid $op_class $channel $ssid $multibss_enable $is_transmitted_bssid $set_20_tu_probe_response $max_tx_power"

	else

		rnr="DELETE_RNR $bssid"

	fi

	if [ -z $2 ]; then
		interfaces=`ls /var/run/hostapd | cut -d"." -f1 | grep -v wlan4 | grep -v global-hostapd`
                #remove duplicate
                interfaces=`echo $interfaces | awk '{for (i=1;i<=NF;i++) if (!a[$i]++) printf("%s%s",$i,FS)}{printf(" ")}'`
                for j in $interfaces
		do
			echo "######### hostapd_cli -i$j $rnr ############# "
			hostapd_cli -i$j $rnr
		done
	else
		echo "############ hostapd_cli -i$2 $rnr"
		hostapd_cli -i$2 $rnr
	fi
done
