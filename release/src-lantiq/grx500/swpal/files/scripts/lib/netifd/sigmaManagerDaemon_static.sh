#!/bin/sh
. /lib/netifd/sigma-ap.sh

debug_print "--------------------------------------------------- DAEMON STARTED ---------------------------------------------------"

interface=$1
sta_number=$2
current_num_of_connected_stations=`run_dwpal_cli_cmd $interface peerlist | grep "peer(s) connected" | awk '{print $1}'`
num_of_connected_sta=$current_num_of_connected_stations
if [ "$num_of_connected_sta" = "" ]; then
	num_of_connected_sta=0
fi
flag=0
while :;
do
	if [ "$num_of_connected_sta" = "" ]; then
		num_of_connected_sta=0
	fi
	if [ $flag -eq 0 ]; then
		debug_print "waiting stations to connect"
		debug_print "current connected stations = $num_of_connected_sta"
	fi
	connected_sta=`run_dwpal_cli_cmd $interface peerlist`
	## take only the number of peers connected.
	num_of_connected_sta=${connected_sta%%' '*}
	num_of_connected_sta=${num_of_connected_sta//' '/}
	num_of_connected_sta=`echo $num_of_connected_sta`
	[ "$num_of_connected_sta" = "No" ] && num_of_connected_sta=0
	## leave only MACs
	connected_sta=${connected_sta%%' ('*}
	connected_sta=${connected_sta##*': '}
	if [ "$num_of_connected_sta" = "$sta_number" ]; then
		reached_connected_sta_macs=${connected_sta//[$'\n']}
		reached_connected_sta_macs=${reached_connected_sta_macs##*connected:}
		debug_print "connected_sta=$connected_sta"
		debug_print "num_of_connected_sta=$num_of_connected_sta"
		handler_func "plan_on"  ${interface} ${sta_number} & #plan_on
		break
	elif [ "$num_of_connected_sta" -gt "$sta_number" ]; then
		debug_print "Error: Too many connected stations"
		continue #No test has more than 4 stations, we hit this case due to wrong dwpal output, try again
	fi
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
	num_of_connected_sta=`echo $num_of_connected_sta`
	## leave only MACs
	current_connected_sta=${current_connected_sta%%' ('*}
	current_connected_sta=${current_connected_sta##*': '}
	current_connected_sta=`echo $current_connected_sta`
	debug_print "current_connected_sta=$current_connected_sta"
	debug_print "----------------"
	debug_print ""
	if [ "$num_of_connected_sta" = "$sta_number" ]; then #connected_sta == current_connected_sta
		known_disconnected_sta_Mac="no"
		handler_func "plan_on"  ${interface} ${sta_number} &
		continue
	else
		debug_print "disconnected station for more than 5 seconds. No other station has been connected in this time"
		debug_print "number of current connected station $num_of_connected_sta"
#		plan already off.
		known_disconnected_sta_Mac="yes"
	fi
done
