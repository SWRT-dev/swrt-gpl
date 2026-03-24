#!/bin/sh

listenPort=9000
. /lib/wifi/platform_dependent.sh
. /lib/netifd/sigma-ap.sh

# If certification flag doesn't exist, don't start the script
if [ ! -e "$CERTIFICATION_FILE_PATH" ]
then
	echo "$0 : Certification flag was not found, please execute /opt/intel/wave/scripts/debug_toolbox.sh cert [IP address]" > /dev/console
	return
fi

HOSTAPD_CLI_CMD=hostapd_cli
if [ "${MODEL:0:3}" != "LGM" ] && [ "$OS_NAME" = "UGW" ]; then
	HOSTAPD_CLI_CMD="sudo -u nwk -- $HOSTAPD_CLI_CMD"
fi

procName=$0

# clean unused process
ownPid=$$
sigmaStartPid=`ps | grep "$0" | grep -v "grep" | grep -v "$ownPid" | awk '{ print $1 }'`
kill $sigmaStartPid

kill_sigma_ap()
{
	killwatchdog=0
	daemon_pid=`ps | grep sigma-ap.sh | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
	while [ "$daemon_pid" != "" ]; do
		for p in $daemon_pid
		do
			pidtoKill=$p
			kill "$pidtoKill"
		done
		daemon_pid=`ps | grep sigma-ap.sh | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
		let killwatchdog=killwatchdog+1
	done
}
kill_sigma_mbo_daemon
kill_sigmaManagerDaemon
kill_sigma_ap

if [ -z `ls $NC_COMMAND` ]; then
	NC_COMMAND=`cat /tmp/lite_nc_location`
	if [ -z `ls "$NC_COMMAND"` ]; then
		NC_COMMAND=`find / -name lite_nc | tail -n1`
		if [ -z "$NC_COMMAND" ]; then
			echo "Notice - lite_nc not found, I am using busybox"
			if [ ! -x "$BUSYBOX_BIN_PATH/busybox" ]; then
				echo "Error - busybox has no x permissions"
				exit 1
			fi
			if [ -z `"$BUSYBOX_BIN_PATH/busybox" nc 2>&1 | grep Listen` ]; then
				echo "Error - busybox nc has no listen option"
				exit 1
			fi
			NC_COMMAND="$BUSYBOX_BIN_PATH/busybox nc -l -p $listenPort"
		else
			chmod +x "$NC_COMMAND"
			echo "$NC_COMMAND" > /tmp/lite_nc_location
		fi
	else
		chmod +x "$NC_COMMAND"
	fi
else
	chmod +x "$NC_COMMAND"
fi

ncPid=`ps | grep "lite_nc" | grep -v "grep" | awk '{ print $1 }'`
if [ "$ncPid" = "" ]; then
	ncPid=`ps | grep "busybox nc -l -p $listenPort" | grep -v "grep" | awk '{ print $1 }'`
fi
for cur_pid in $ncPid
do
	kill "$cur_pid"
done

if [ "$OS_NAME" = "RDKB" ]; then
	iptables -t mangle -D FORWARD -m state ! --state NEW -j DSCP --set-dscp 0x00
	iptables -t mangle -D FORWARD -m state --state NEW -j DSCP --set-dscp 0x14
elif [ "$OS_NAME" = "UGW" ]; then
	PROTO=tcp                # The PROTO can be ‘tcp’ or ‘udp’
	DPORT=9000
	ubus call firewalld notify_firewall_change '{ "rule1" : "iptables -I zone_wan_input -p '$PROTO' --dport '$DPORT' -j ACCEPT" }'
	ubus call firewalld notify_firewall_change '{ "rule2" : "iptables -I zone_lan_input -p '$PROTO' --dport '$DPORT' -j ACCEPT" }'
fi

dirname() {
	full=$1
	file=`basename $full`
	path=${full%%$file}
	[ -z "$path" ] && path=./
	echo $path
}
thispath=`dirname $0`

if [ "$OS_NAME" = "UGW" ]; then
	sigma_path="/tmp/"
else
	sigma_path="/var/"
fi

cp $thispath/sigma-ap.sh $sigma_path
cd $sigma_path
ap_tmp=`dmesg -n7`
[ ! -e sigma-pipe ] && mknod sigma-pipe p

while [ `$HOSTAPD_CLI_CMD status | sed -n 's/state=//p'` != "ENABLED" ]; do echo sigma-start.sh: Waiting for interface up; sleep 3; done
while true; do $NC_COMMAND < sigma-pipe  | "./sigma-ap.sh" > ./sigma-pipe; done &
