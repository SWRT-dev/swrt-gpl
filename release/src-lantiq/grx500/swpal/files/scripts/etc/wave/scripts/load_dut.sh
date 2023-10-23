#!/bin/sh

. /lib/wifi/platform_dependent.sh

# TODO: replace start&stop drvhlpr with start&stop file_sever_daemon in the right places
###########
# drvhlpr #
###########
drvhlpr_app="${BINDIR}/drvhlpr"
drvhlpr_dut="drvhlpr_dut"
drvhlpr_dut_conf=/tmp/${drvhlpr_dut}.config

stop_drvhlpr()
{
	# kill drvhlpr
	killall $drvhlpr_dut 2>/dev/null
	killall drvhlpr_wlan0 2>/dev/null
	killall drvhlpr_wlan2 2>/dev/null
	drvhlpr_count=`ps | grep drvhlpr_wlan'[0-1]{1}' -c`
	while [ "$drvhlpr_count" -gt 0 ]; do echo wait_drvhlpr_close > /dev/console; sleep 1; drvhlpr_count=`ps | grep drvhlpr_wlan'[0-1]{1}' -c`; done
}
start_drvhlpr_dut_mode()
{
	# Start drvhlpr for dut
	cp -s $drvhlpr_app /tmp/$drvhlpr_dut
	echo "f_saver = ${SCRIPTS_PATH}/flash_file_saver.sh" > $drvhlpr_dut_conf
	/tmp/$drvhlpr_dut --dut -p $drvhlpr_dut_conf &
}
#############
# dutserver #
#############
stop_dutserver(){
	killall dutserver 2>/dev/null
	retries=0
	RETRIES_MAX=10
	while pgrep -x "${BINDIR}/dutserver" > /dev/null
	do
		if [ $retries -ge $RETRIES_MAX ]
		then
			echo "dutserver is still running after $retries seconds. Please try again..."
			exit 1
		fi
		sleep 1
		retries=$(expr $retries + 1)
	done
	sleep 1
}
start_dutserver(){
	mkdir -p /var/run/dutserver
	echo "wavToolBox_full_path = ${SCRIPTS_PATH}/debug_toolbox.sh" > /var/run/dutserver/platform.config
	${BINDIR}/dutserver &
}
##################
# dut_components #
##################
stop_dut_components(){
	stop_drvhlpr
	stop_dutserver
}
start_dut_components(){
	start_drvhlpr_dut_mode
	start_dutserver
}
##########################################################################################################################
# Get start/stop/restart flag
flag="start"
if [ $# -gt 0 ]
then
	flag="$1"
	if [ $flag != "start" ] && [ $flag != "stop" ] && [ $flag != "restart" ]
	then
		flag="start"
	fi
fi

## Execute dut components according to start/stop/restart flag
if [ $flag = "start" ]
then
	#if dutserver already running - exit
	if pgrep -x "${BINDIR}/dutserver" > /dev/null
	then
		exit 0
	else
		start_dut_components
	fi
elif [ "$flag" = "stop" ]
then
	stop_dut_components
elif [ "$flag" = "restart" ]
then
	stop_dut_components
	start_dut_components
fi

exit 0
