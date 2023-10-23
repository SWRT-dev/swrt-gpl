#!/bin/sh

# rnr == Reduced Neighbor Report includes 6ghz bssid/operation_class/channel/ssid
# This script provides interface to start listerning on given interface and prepare rnr based on received events on interface
# input: 2.4GHz/5GHz/6GHz

found=0
supported_bands="2.4GHz 5GHz 6GHz"

for i in $supported_bands
do
        if [ -z $1 ]; then
                break
        fi
        if [ $1 = $i ]; then
                found=1
        fi
done

if [ $found -eq 0 ]; then
        echo "Usage: $0 2.4GHz/5GHz/6GHZ"
        exit
fi

while :;
do
        # Find 6GHz interface
        radio_interface=`uci show wireless | grep ".band" | grep 6GHz | cut -d. -f2`
        default_radio=`uci show wireless | grep "device='$radio_interface'"| cut -d. -f1-2`
        for i in $default_radio
        do
                radio_6GHz=`uci show $i".ifname" | cut -d"=" -f2`
                radio_6GHz=`echo $radio_6GHz | awk '{print substr($0, 2, length($0) - 2)}'`
                radio_6GHz_array=$radio_6GHz_array" "$radio_6GHz
        done
        if [ $1 = "6GHz" ]; then
                radio_6GHz=`echo $radio_6GHz | cut -d"." -f1` #listern to only master vap
                dwpal_cli -ihostap -v$radio_6GHz -lAP-ENABLED -lAP-DISABLED -lAP-CSA-FINISHED -lAP-20TU-PROBE-RESPONSE
        else
                radio_interface=`uci show wireless | grep ".band" | grep $1 | cut -d. -f2`
                default_radio=`uci show wireless | grep "device='$radio_interface'" | cut -d. -f1-2`
                for i in $default_radio
                do
                        radio=`uci show $i".ifname" | cut -d"=" -f2`
                        radio=`echo $radio | awk '{print substr($0, 2, length($0) - 2)}'`
                        radio=`echo $radio | cut -d"." -f1` #need only master vap, hostapd will propogate rnr to slave vaps
                done
                # For 2.4/5 only AP-ENABLED needs to be handled
                dwpal_cli -ihostap -v$radio -lAP-ENABLED 
        fi
        idx=0
        for i in $radio_6GHz_array
        do
                bssid=`iw dev $i info | grep addr | cut -d" " -f2`
                if [ -S "/var/run/hostapd/$i" ]; then
                        #prepare 6GHz rnr template
                        channel=`hostapd_cli -i$i status | grep "channel=" | grep -v secondary | cut -d"=" -f2`
                        ssid=`iw dev $i info | grep ssid | cut -d" " -f2`
                        #don't include dummy ssid in rnr			
                        if [ "$ssid" = "dummy_ssid_6G" ]; then
                            continue
                        fi
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
                        rnr="UPDATE_RNR $bssid $op_class $channel $ssid $multibss_enable $is_transmitted_bssid $set_20_tu_probe_response"
                else
                        rnr="DELETE_RNR $bssid"
                fi

                if [ $1 = "6GHz" ]; then
                        for i in `ls /var/run/hostapd | cut -d"." -f1 | grep -v $radio_6GHz`
                        do
                                echo "######### hostapd_cli -i$i $rnr ############# "
                                hostapd_cli -i$i $rnr
                        done
                else
                        echo "########### hostapd_cli -i$radio $rnr ############# "
                        hostapd_cli -i$radio $rnr
                fi
        done

done
