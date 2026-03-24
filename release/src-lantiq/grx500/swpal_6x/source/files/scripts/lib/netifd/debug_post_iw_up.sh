#!/bin/sh

# For certification: set IP address
# Check OS
os_name="RDKB"
default_ip="10.0.0.1"
certification_file="/etc/wave/certification_enabled"
which dmcli > /dev/null
if [ $? -eq 1 ]
then
        os_name="UGW"
        default_ip="192.168.1.1"
        certification_file="/opt/intel/wave/certification_enabled"
fi
if [ -e "$certification_file" ]
then
        if [ "$os_name" = "UGW" ]
        then
                br_ip=`uci get network.lan.ipaddr`
        else
                br_ip=`dmcli eRT setv Device.X_CISCO_COM_DeviceControl.LanManagementEntry.1.LanIPAddress`
                br_ip=${br_ip##*value: }
                br_ip=`echo $br_ip`
        fi
        if [ "$br_ip" = "$default_ip" ] || [ -z "$br_ip" ]
        then
                br_ip="192.165.100.10"
        fi
        change_ip=1
fi

debug_infrastructure=/lib/netifd/debug_infrastructure.sh
if [ ! -f $debug_infrastructure ]; then
        exit
fi
. $debug_infrastructure

uci_indexes=`uci show | grep ifname | sed 's/.ifname/ /g' | awk '{print $1}'`
phys=$(ls /sys/class/ieee80211/)
is_6g_supported="0"
for phy in $phys
do
	`iw $phy info | grep "* 60.. MHz" > /dev/null`
	is_phy_6g=$?

	if [ $is_phy_6g = '0' ]; then
		is_6g_supported="1"
		break
	fi
done

while :;
do
        vaps=`uci show wireless | grep ifname | cut -d"=" -f2 | grep -v "\." | tr '\r\n' ' ' | tr -d "'" | sed 's/w/-vw/g'`
        eventMsg=`dwpal_cli -ihostap $vaps -l"AP-ENABLED" -l"INTERFACE_RECONNECTED_OK"`
        sleep 1

        for uci_index in $uci_indexes ; do
                interface=`uci get ${uci_index}.ifname`
                for i in $(seq 1 $number_of_debug_configs); do
                        debug_config=`uci -q get ${uci_index}.debug_iw_post_up_$i`
                        if [ -n "$debug_config" ]; then
                                eval "iw $interface iwlwav $debug_config"
                        fi
                done
        done
	if [ "$change_ip" = "1" ]
	then
                if [ "$os_name" = "UGW" ]
                then
                        ubus call uci set '{ "config" : "network" ,"section" : "lan", "values": {"ipaddr" : "'$br_ip'"}}'
                        ubus call uci commit '{"config" : "network" }'
                else
                        bridgeIPChange $br_ip
                fi
		change_ip=0
	fi
done
