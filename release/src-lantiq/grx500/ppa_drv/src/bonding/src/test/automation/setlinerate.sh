#!/bin/sh

function checkifup () {

	ifname=$1
	ifconfig_output=$(ifconfig $ifname 2>&1)
	
	echo $ifconfig_output | awk -v ifname=$ifname 'BEGIN {exit_status=1;} { if (match($0, ifname ": error")) exit_status=0;} /UP .* RUNNING/ {exit_status=2;} END {exit exit_status}' -
	exit_status=$?
	if [ $exit_status -eq 0 ]; then
		echo $ifname does not exist. Please check the system. Exiting...
		exit 1
	fi
	
	return $exit_status
}

function checkipaddr () {

	ifname=$1
	ifconfig_output=$(ifconfig $ifname 2>&1)
	
	echo $ifconfig_output | awk -v ifname=$ifname 'BEGIN {exit_status=1;} { if (match($0, ifname ": error")) exit_status=0;} /inet addr:[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}/ {exit_status=2;} END {exit exit_status}' -
	exit_status=$?
	if [ $exit_status -eq 0 ]; then
		echo $ifname does not exist. Please check the system. Exiting...
		exit 1
	fi

	return $exit_status
}

function getwanifname {

	for pattern in "^pppoe-wan" "^nas0_wan"
	do
		ifname=$(ifconfig | grep $pattern - | awk 'BEGIN {exit_status=1; ifname=""} /wan/ {ifname=$1; exit_status=0;} END {print ifname; exit exit_status;}' -)
		exit_status=$?
		if [ $exit_status -eq 0 ]; then
			checkifup $ifname 2>&1 > /dev/null
			if [ $? -eq 2 ]; then
				checkipaddr $ifname
				if [ $? -eq 2 ]; then
					echo "$ifname";
					return;
				fi
			fi
		fi
	done
	
	echo ""
}

if [ $# -ne 2 ]; then
	echo "Usage: $0 <0|1> [line rate in Kbps]"
	echo "Example: $0 1 600"
	exit
fi

linkid=$1
bitrate=$2

bitratehex=$(echo $bitrate | awk '{ input = $0; output = input/8; printf "0x%04x\n", output; }' -)
echo "bit rate hex of $bitrate for line-$linkid = $bitratehex"

# First reset the link's bit rate
#echo "Resetting Link-$linkid's line rate"
#dsl_cpe_pipe acs $linkid 2;
#sleep 3;
#while true; do
#state=$(dsl_cpe_pipe lsg -1 | awk -v linkid=$linkid '/nDevice=/ { if (match($2, "nDevice=" linkid)) print substr($3, 12);}' -)
#echo "State of Link-$linkid: $state"
#if [ $state = "0x801" ]; then break; fi;
#sleep 1;
#done

# Now start setting it
echo "Configuring Link-$linkid's line rate"
dsl_cpe_pipe acs $linkid 2;
sleep 1
count=0

while true; do

count=$((count+1))

linerate=$(dsl_cpe_pipe g997csg $linkid 0 0 | awk 'BEGIN {exit_status=1; linerate=-1;} /nReturn=0/ { exit_status=0; linerate=substr($5,16);} END { print linerate; exit exit_status}' -)
exit_status=$?
if [ $exit_status -eq 0 ]; then break; fi

echo "${count}) link-$linkid not yet in showtime. Shall check after 1 second"

if [ $count -gt 0 -a $((count % 5)) -eq 0 ]; then
echo "Reapplying 'dsl_cpe_pipe dms $linkid 0248 8 1 $bitratehex'"
dsl_cpe_pipe dms $linkid 0248 8 1 $bitratehex
fi

sleep 1

done

linerate=$(echo $linerate | awk '{print $0/1000}' -)
echo "line rate = $linerate kbps"
dsl_cpe_pipe g997csg -1 0 0

# Ensure that nas0 is up
checkifup nas0
if [ $? -ne 2 ]; then
	# nas0 is down
	ifconfig nas0 up
	checkifup nas0
	if [ $? -ne 2 ]; then
		echo HW Shaper could not be set because nas0 is down and I failed to bring it up.
		exit 1
	fi
fi

# Get name of WAN interface
for trial in 9 8 7 6 5 4 3 2 1 0
do
	wanifname=$(getwanifname)
	if [ "0$wanifname" != "0" ]; then
		break;
	elif [ $trial -eq 0 ]; then
		echo HW Shaper could not be set because no active WAN interface found.
		exit 1
	fi
	
	echo No active WAN interface found. Will wait $trial more times
	sleep 1
done

# Set HW Shaper to 4000mbps
qcfg --port -M shaper -i $wanifname -s cb --pir 4000
hwshaperrate=$(cat /sys/kernel/debug/tmu/queue1 | awk 'BEGIN {hwshaperrate=0;} /pir=/ {hwshaperrate=$5;} END {print hwshaperrate}' -)
if [ $hwshaperrate -eq 500000 ]; then
	echo HW Shaper rate for $wanifname successfully set to $hwshaperrate Bps
else
	echo HW Shaper rate for $wanifname unsuccessfully set to $hwshaperrate Bps
fi
