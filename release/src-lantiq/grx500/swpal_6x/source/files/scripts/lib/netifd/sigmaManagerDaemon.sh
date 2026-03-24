#!/bin/sh
. /lib/netifd/sigma-ap.sh

# Wait until AP gets an ICMP requset (or there is a timeout)
# and then wait for the pings to stop
wait_for_ping_UGW(){
	local counter=0
	local pkts=0

	ap_tmp=`eval switch_cli dev=1 GSW_PCE_RULE_WRITE pattern.nIndex=30 pattern.bEnable=1 pattern.bPortIdEnable=1 pattern.nPortId=0 pattern.bPortId_Exclude=1 pattern.bProtocolEnable=1 pattern.nProtocol=1 pattern.nProtocolMask=0 action.ePortMapAction=4 action.nForwardPortMap=0x1`

	ap_tmp=`eval ebtables -I FORWARD  1 -p IPv4 --ip-proto icmp -j CONTINUE`

	while [ $pkts -eq 0 ] && [ $counter -lt 20 ] ; do
		pkts=`ebtables -L FORWARD --Lc | grep icmp | awk '{ print $10 }'`
		sleep 1
		counter=$((counter+1))
	done
	info_print "Finished waiting for pings to start"

	if [ $counter -ne 10 ] ; then
		info_print "Pings started, waiting for pings to finish"
		counter=0
		pkts=10
		while [ $pkts -ne 0 ] && [ $counter -lt 20 ] ; do
			ap_tmp=`eval ebtables -Z FORWARD`
			sleep 2
			pkts=`ebtables -L FORWARD --Lc | grep icmp | awk '{ print $10 }'`
			counter=$((counter+2))
		done
	else
		info_print "Timeout waiting for pings"
	fi
	ap_tmp=`eval ebtables -D FORWARD  1`
}

wait_for_ping_RDKB(){

	local counter=0
	dmesg -c > /dev/null

	ap_tmp=`eval iptables -I FORWARD  1 -p icmp --icmp-type echo-request -j GWMETA --dis-pp`
	ap_tmp=`eval iptables -I FORWARD  1 -p icmp --icmp-type echo-request -j LOG --log-prefix "IPTables-PING: " --log-level 7`

	while ! `dmesg|grep "IPTables-PING" > /dev/null` ; do

		counter=$((counter+1))
		if [ $counter -gt 10 ]; then
			info_print "Timeout wating for ping"
			ap_tmp=`eval iptables -D FORWARD 1`
			ap_tmp=`eval iptables -D FORWARD 1`
			return
		fi
		sleep 1
	done
	info_print "Pings started"

	# Wait until there are at least 2 seconds without getting ICMP requests
	counter=0
	while dmesg -c > /dev/null && sleep 2 && ! `dmesg|grep "IPTables-PING" > /dev/null`; do
	counter=$((counter+1))
	if [ $counter -gt 10 ]; then
		info_print "Timeout wating for pings to stop"
		ap_tmp=`eval iptables -D FORWARD 1`
		ap_tmp=`eval iptables -D FORWARD 1`
		return
	fi
	done
	info_print "Pings stopped"
	ap_tmp=`eval iptables -D FORWARD 1`
	ap_tmp=`eval iptables -D FORWARD 1`
}

debug_print "--------------------------------------------------- DAEMON STARTED ---------------------------------------------------"
ap_ofdma=ap_ofdma_$ap_ofdma
interface=$1
sta_number=$2
current_num_of_connected_stations=`run_dwpal_cli_cmd $interface peerlist | grep "peer(s) connected" | awk '{print $1}'`
num_of_connected_sta=$current_num_of_connected_stations
if [ "$num_of_connected_sta" = "" ]; then
	num_of_connected_sta=0
fi
flag=0
counter=0
connected_users=0
if [ $sta_number -gt 0 ]; then
	pids=`ps | grep sigmaManagerDaemon_static | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
        for i in $pids; do
                echo "killing existing $i" >> /dev/console
                kill -9 $i
        done
	/lib/netifd/sigmaManagerDaemon_static.sh $* &
	exit
fi
while [ $counter -le "120" ]
do
	if [ "$num_of_connected_sta" = "" ]; then
		num_of_connected_sta=0
	fi
	if [ $flag -eq 0 ]; then
		debug_print "waiting stations to connect"
		debug_print "current connected stations = $num_of_connected_sta"
	fi
	connected_sta=`run_dwpal_cli_cmd $interface peerlist`
	no_sta_connected=`echo $connected_sta | grep -i "no"` #sometimes dwpal sends out junk buffer along with no peerlist
	if [ -n "$no_sta_connected" ]; then
		connected_sta=0
	fi
	## take only the number of peers connected.
	num_of_connected_sta=${connected_sta%%' '*}
	num_of_connected_sta=${num_of_connected_sta//' '/}
	num_of_connected_sta=`echo $num_of_connected_sta`
	[ "$num_of_connected_sta" = "No" ] && num_of_connected_sta=0
	## leave only MACs
	connected_sta=${connected_sta%%' ('*}
	connected_sta=${connected_sta##*': '}
	if [ "$connected_users" -lt "$num_of_connected_sta" ]; then
		if [ "$num_of_connected_sta" = "1" ] || [ "$num_of_connected_sta" = "2" ] || [ "$num_of_connected_sta" = "3" ] || [ "$num_of_connected_sta" = "4" ]; then
			reached_connected_sta_macs=${connected_sta//[$'\n']}
			reached_connected_sta_macs=${reached_connected_sta_macs##*connected:}
			debug_print "connected_sta=$connected_sta"
			debug_print "num_of_connected_sta=$num_of_connected_sta"
			handler_func "plan_off" ${interface} &
			if [ "$OS_NAME" = "RDKB" ]; then
				wait_for_ping_RDKB
			elif [ "$OS_NAME" = "UGW" ]; then
				wait_for_ping_UGW
			fi
			handler_func "plan_on"  ${interface} ${num_of_connected_sta} ${ap_ofdma} & #plan_on

			connected_users=${num_of_connected_sta}
			counter=0
		elif [ "$num_of_connected_sta" -gt "4" ]; then
			debug_print "Error: Too many connected stations"
			continue #No test has more than 4 stations, we hit this case due to wrong dwpal output, try again
		fi
	fi
	counter=$((counter+1))
	sleep 1
	let flag+=1
	if [ $flag -eq 5 ]; then
		flag=0
	fi
done

all_connected_macs=""; known_disconnected_sta_Mac="no"
while :;
do
	if [ "$known_disconnected_sta_Mac" = "no" ]; then
		ret=`run_dwpal_cli_cmd -ihostap -mMain -v$interface -dd -l"AP-STA-DISCONNECTED"`
		current_connected_sta=`run_dwpal_cli_cmd $interface peerlist`

		## leave only MACs
		current_connected_sta=${current_connected_sta%%' ('*}
		current_connected_sta=${current_connected_sta##*': '}

		current_connected_sta=${current_connected_sta//[$'\n']}
		current_connected_sta=${current_connected_sta##*connected:}
		disconnected_sta_MAC=${reached_connected_sta_macs/$current_connected_sta}
		debug_print "disconnected_sta_MAC=$disconnected_sta_MAC"
		handler_func "plan_off" ${interface} &
	fi
	sleep 5
	current_connected_sta=`run_dwpal_cli_cmd $interface peerlist`
	## take only the number of peers connected.
	num_of_connected_sta=${current_connected_sta%%' '*}
	num_of_connected_sta=${num_of_connected_sta//' '/}
	## leave only MACs
	current_connected_sta=${current_connected_sta%%' ('*}
	current_connected_sta=${current_connected_sta##*': '}
	current_connected_sta=`echo $current_connected_sta`
	debug_print "current_connected_sta=$current_connected_sta"
	debug_print "----------------"
	debug_print ""
	if [ "$num_of_connected_sta" = "$sta_number" ]; then #connected_sta == current_connected_sta
		known_disconnected_sta_Mac="no"
		handler_func "plan_on"  ${interface} ${sta_number} ${ap_ofdma} &
		continue
	else
		debug_print "disconnected station for more than 5 seconds. No other station has been connected in this time"
		debug_print "number of current connected station $num_of_connected_sta"
#		plan already off.
		known_disconnected_sta_Mac="yes"
	fi
done
