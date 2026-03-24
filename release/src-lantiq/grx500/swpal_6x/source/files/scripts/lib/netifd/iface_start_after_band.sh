#!/bin/sh

restore_iface() {
	local delayed_start_file="/tmp/${ifname}_start_after_${start_after}"

	hostapd_cli -i "$iface" SET start_disabled 0
	echo 1 > "$delayed_start_file"
}

iface="$1"
start_after_miface="$2"

miface_hostapd_up_checks=10
while [ "$miface_hostapd_up_checks" -ge 0 ]; do
	status=`hostapd_cli -i ${start_after_miface} STATUS`
	if [ -n "$status" ]; then
		break
	fi

	if [ "$miface_hostapd_up_checks" -eq 0 ]; then
		restore_iface
		exit 1
	fi

	sleep 1
	miface_hostapd_up_checks=$((miface_hostapd_up_checks-1))
done

is_enabled=`echo "$status" | grep "state=ENABLED"`
if [ -n "$is_enabled" ]; then
	sleep 5
	restore_iface
	exit 0
fi

miface_hostapd_dfs_checks=15
while [ "$miface_hostapd_dfs_checks" -ge 0 ]; do
	status=`hostapd_cli -i ${start_after_miface} STATUS`
	is_enabled=`echo "$status" | grep "state=ENABLED"`
	if [ -n "$is_enabled" ]; then
		sleep 5
		restore_iface
		exit 0
	fi

	is_dfs=`echo "$status" | grep "state=DFS"`
	if [ -n "$is_dfs" ]; then
		cac_time=`echo "$status" | grep "cac_time_seconds="`
		cac_time=${cac_time#*=}
		if [ "$cac_time" -gt 60 ]; then
			restore_iface
			exit 0
		fi

		break
	fi

	if [ "$miface_hostapd_dfs_checks" -eq 0 ]; then
		restore_iface
		exit 1
	fi

	sleep 1
	miface_hostapd_dfs_checks=$((miface_hostapd_dfs_checks-1))
done

cac_wait=62
while [ "$cac_wait" -gt 0 ]; do
	status=`hostapd_cli -i ${start_after_miface} STATUS`
	is_enabled=`echo "$status" | grep "state=ENABLED"`
	if [ -n "$is_enabled" ]; then
		break
	fi

	sleep 1
	cac_wait=$((cac_wait-1))
done

sleep 5
restore_iface
exit 0
