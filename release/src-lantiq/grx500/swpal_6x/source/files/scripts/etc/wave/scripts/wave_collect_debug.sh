#!/bin/sh
. /lib/wifi/platform_dependent.sh

#
# This script collects logs and other debug related files.
#	a script to send all wlan logs to a single file (for debugging).
#	warning: after running the script the preformance will be hindered
#   Collect logs, DB and FW dumps of all cards to a single file:
#   wlan_collect_debug.sh -a [optional: tftp server address to send the file to]
#   the script out pull all logs to /var/log/wlanLog
#

script_name=$0
optionFlag=$1
optionIP=$2



current_date=`date +%m_%d_%Y_%H_%M_%S`
function do_debug_assert(){
	wave500_count=`lspci -n | grep -c $LTQ_CODE_WAVE500`
	wave600_count=`lspci -n | grep -c -E "$LTQ_CODE_WAVE600|$LTQ_CODE_WAVE600D2"`

	if [ "$wave600_count" = "0" ] && [ "$wave500_count" = "0" ]; then
		echo "$script_name: No WAVE interfaces found" > /dev/console
		lspci_output=`lspci`
		echo "$script_name: lspci output:\n$lspci_output" > /dev/console
	fi

	card_idx=0
	while [ $card_idx -lt $wave600_count ]; do

		#get 'master' interface of the card
		line=`cat /proc/net/mtlk/topology | grep -Fn [hw$card_idx | awk -F ':' '{ print $1 }'`
		iface=`[ "X$line" != "X" ] && tail +$line /proc/net/mtlk/topology | grep -om1 "wlan[0-9]"`
		if [ "X$iface" == "X" ]; then
			echo "$script_name: interfaces for $iface not found in topology file!" > /dev/console
			card_idx=$((card_idx+1))
			continue
		fi

		blocking_loop_idx=6
		forcekill=""
		while [  -n "$(pgrep dump_handler)" ] && [ "$blocking_loop_idx" != "0" ]
		do
				[ "$blocking_loop_idx" = "1" ] &&  forcekill="-9"
				kill $forcekill $(pgrep -n dump_handler) #avoid race conditions with other running dump handlers
				blocking_loop_idx="$((blocking_loop_idx-1))"
		done

		`iw dev $iface iwlwav sDoDebugAssert 1`

		#wait for dumps to be available
		res=1
		count=0
		while [ $res -ne 0 ] && [ $count -lt 10 ]; do
			sleep 1
			`head -1 ${WAVE600_FW_DUMP_FILE_PREFIX}${card_idx}${WAVE600_FW_DUMP_FILE_SUFFIX} 2>/dev/null | grep $DUMP_HEADER_MAGIC > /dev/null`
			res=$?
			count=$((count+1))
		done

		cat ${WAVE600_FW_DUMP_FILE_PREFIX}${card_idx}${WAVE600_FW_DUMP_FILE_SUFFIX} > ${DUMP_IN_PATH}${card_idx}
		mkdir ${FW_DUMP_OUT_PATH}${card_idx}
		$DUMP_UTIL -i ${card_idx} -f ${FW_DUMP_OUT_PATH}${card_idx} -d ${DUMP_IN_PATH}${card_idx}

		#rm ${DUMP_IN_PATH}${card_idx}

		card_idx=$((card_idx+1))
	done

	card_idx=0
	while [ $card_idx -lt $wave500_count ]; do

		blocking_loop_idx=6
		forcekill=""
		while [  -n "$(pgrep dump_handler)" ] && [ "$blocking_loop_idx" != "0" ]
		do
				[ "$blocking_loop_idx" = "1" ] &&  forcekill="-9"
				kill $forcekill $(pgrep -n dump_handler) #avoid race conditions with other running dump handlers
				blocking_loop_idx="$((blocking_loop_idx-1))"
		done
		iface=wlan$((card_idx*2))
		`iw dev $iface iwlwav sDoDebugAssert 1`

		#wait for dumps to be available
		res=1
		count=0
		while [ $res -ne 0 ] && [ $count -lt 10 ]; do
			sleep 1
			`head -1 ${WAVE500_FW_DUMP_FILE_PREFIX}$((card_idx*2))${WAVE500_FW_DUMP_FILE_SUFFIX} 2>/dev/null | grep $DUMP_HEADER_MAGIC > /dev/null`
			res=$?
			count=$((count+1))
		done

		cat ${WAVE500_FW_DUMP_FILE_PREFIX}$((card_idx*2))${WAVE500_FW_DUMP_FILE_SUFFIX} > ${DUMP_IN_PATH}$((card_idx*2))
		mkdir ${FW_DUMP_OUT_PATH}$((card_idx*2))
		$DUMP_UTIL -i $((card_idx*2)) -f ${FW_DUMP_OUT_PATH}$((card_idx*2)) -d ${DUMP_IN_PATH}$((card_idx*2))

		#rm ${DUMP_IN_PATH}$((card_idx*2))

		card_idx=$((card_idx+1))
	done

}

function zip_db(){
	tar -C $DB_LOC -czf ${TMP_LOGS_PATH}/db_params_${current_date}.tar.gz $DB_LOC_NAME
}

function zip_confs(){
	cd $CONF_LOC
	tar -czf ${TMP_LOGS_PATH}/conf_files_${current_date}.tar.gz *conf
	cd -
}

function save_logs()
{
	# Save info from user and daemon logs to /var/log
	cat $IN_FILES | grep $MATCHES | sort -k4n -k1M -k2n -k3 > $WLAN_LOG_FILE_PATH

	# data path debug info
	[ -d /sys/kernel/debug/tmu ] && echo m -1 > /sys/kernel/debug/tmu/eqt # part of /var/log/
	[ -d /sys/kernel/debug/tmu ] && cat /sys/kernel/debug/tmu/lookup > ${TMP_LOGS_PATH}/kernel_dbg_tmu_lookup
	[ -d /sys/kernel/debug/dp ] && cat /sys/kernel/debug/dp/port > ${TMP_LOGS_PATH}/kernel_dbg_dp_port

	# Save /var/log files
	mkdir -p ${TMP_LOGS_PATH}/var_log
	cp -a ${VAR_LOGS_LOC}/* ${TMP_LOGS_PATH}/var_log

	# Copy the DB files
	cp -aL ${DB_LOC}/${DB_LOC_NAME} ${TMP_LOGS_PATH}

	# Copy the conf files
	mkdir -p ${TMP_LOGS_PATH}/confs
	cp -a ${CONF_LOC}/*.conf ${TMP_LOGS_PATH}/confs
	cp -a ${CONF_LOC}/*.txt ${TMP_LOGS_PATH}/confs

	# copy any FW dumps that were created
	[ "$(ls -l ${DUMP_LOC}/fw_dump*)" != "" ] && cp ${DUMP_LOC}/fw_dump* ${TMP_LOGS_PATH}

	# copy any whm dumps that were created
	[ "$(ls -l ${DUMP_LOC}/whm_dump*)" != "" ] && cp ${DUMP_LOC}/whm_dump* ${TMP_LOGS_PATH}

	# Copy nvram logs
	[ -n "$NVRAM_LOGS_LOC" ] && mkdir -p ${TMP_LOGS_PATH}/nvram_logs && cp -a ${NVRAM_LOGS_LOC}/* ${TMP_LOGS_PATH}/nvram_logs

	# Save dmesg output
	dmesg -c > ${TMP_LOGS_PATH}/dmesg
	cat ${TMP_LOGS_PATH}/dmesg | grep 'Call Trace\|Data bus error\|tainted\|MAC fatal error\|Broken pipe\|SIGSEGV\|message timeout\|Asser\|Error' > ${TMP_LOGS_PATH}/dmesg_main_erros

	# Save driver info
	cat /proc/net/mtlk/version > ${TMP_LOGS_PATH}/driver_version
	cat /proc/net/mtlk/topology > ${TMP_LOGS_PATH}/driver_topology
	[ -e /proc/net/mtlk/card0 ] && cat /proc/net/mtlk/card0/FW/recovery_stats > ${TMP_LOGS_PATH}/driver_card0_recovery_stats
	[ -e /proc/net/mtlk/card1 ] && cat /proc/net/mtlk/card1/FW/recovery_stats > ${TMP_LOGS_PATH}/driver_card1_recovery_stats
	[ -e /proc/net/mtlk/card2 ] && cat /proc/net/mtlk/card2/FW/recovery_stats > ${TMP_LOGS_PATH}/driver_card2_recovery_stats

	if [ -e /proc/net/mtlk/wlan0 ]; then
		cat /proc/net/mtlk/wlan0/radio_cfg > ${TMP_LOGS_PATH}/driver_wlan0_radio_cfg
		cat /proc/net/mtlk/wlan0/eeprom_parsed > ${TMP_LOGS_PATH}/driver_wlan0_eeprom_parsed
		cat /proc/net/mtlk/wlan0/sta_list > ${TMP_LOGS_PATH}/driver_wlan0_sta_list
	fi
	if [ -e /proc/net/mtlk/wlan2 ]; then
		cat /proc/net/mtlk/wlan2/radio_cfg > ${TMP_LOGS_PATH}/driver_wlan2_radio_cfg
		cat /proc/net/mtlk/wlan2/eeprom_parsed > ${TMP_LOGS_PATH}/driver_wlan2_eeprom_parsed
		cat /proc/net/mtlk/wlan2/sta_list > ${TMP_LOGS_PATH}/driver_wlan2_sta_list
	fi
	if [ -e /proc/net/mtlk/wlan4 ]; then
		cat /proc/net/mtlk/wlan4/radio_cfg > ${TMP_LOGS_PATH}/driver_wlan4_radio_cfg
		cat /proc/net/mtlk/wlan4/eeprom_parsed > ${TMP_LOGS_PATH}/driver_wlan4_eeprom_parsed
		cat /proc/net/mtlk/wlan4/sta_list > ${TMP_LOGS_PATH}/driver_wlan4_sta_list
	fi

	# Save journalctl output
	[ -n "$JOURNALCTRL_UTIL" ] && eval $JOURNALCTRL_UTIL > ${TMP_LOGS_PATH}/journalctl

	# Save wave_components
	cp $WAVE_COMPONENTS_FILE ${TMP_LOGS_PATH}

	# version
	[ -e /version ] && cat /version > ${TMP_LOGS_PATH}/puma_version
	[ -e /version.txt ] && cat /version.txt >> ${TMP_LOGS_PATH}/puma_version
	[ -e /usr/sbin/version.sh ] && /usr/sbin/version.sh > ${TMP_LOGS_PATH}/ugw_version

	# Read memory information
	echo "df -h" > ${TMP_LOGS_PATH}/memory_info
	df -h >> ${TMP_LOGS_PATH}/memory_info
	# Read top info
	top -n 1 > ${TMP_LOGS_PATH}/top_info
}

function save_extension_logs(){
	brctl show > ${TMP_LOGS_PATH}/bridge_info
	iw dev > ${TMP_LOGS_PATH}/iw_dev_info
	cat /proc/net/mtlk/version > ${TMP_LOGS_PATH}/mtlk_version
	ls /lib/firmware/ |grep cal_wlan > ${TMP_LOGS_PATH}/cal_wlan_info
}

function zip_full_logs(){
	cd $TMP_PATH
	. $WAVE_COMPONENTS_FILE
	rm -f full_log_*.tar.gz
	tar -czf full_logs_${wave_release_minor}_${current_date}.tar.gz $TMP_LOGS_PATH
	echo ""
	echo "###########################################################"
	echo "full_logs_${wave_release_minor}_${current_date}.tar.gz was created"
	echo "###########################################################"
	echo ""
	cd - > /dev/null
	sync
}

function zip_extension_logs(){
	cd $TMP_PATH
	. $WAVE_COMPONENTS_FILE
	rm -f full_log_*.tar.gz
	tar -czf extension_logs_${wave_release_minor}_${current_date}.tar.gz $TMP_LOGS_PATH
	echo ""
	echo "###########################################################"
	echo "full_logs_${wave_release_minor}_${current_date}.tar.gz was created"
	echo "###########################################################"
	echo ""
	cd - > /dev/null
	sync
}

if [ -z $optionFlag ]
then
	mkdir -p $TMP_LOGS_PATH
	rm -rf ${TMP_LOGS_PATH}/*
	save_logs
	echo ""
	echo "###########################################################"
	echo "The following logs were saved:"
	echo "# Logs of $param_1_file and $param_3_file have been copied to $WLAN_LOG_FILE_PATH"
	echo "# /var/log files saved in ${TMP_LOGS_PATH}/var_log folder"
	echo "# DB files saved in ${TMP_LOGS_PATH}/${DB_LOC_NAME}"
	echo "# Conf files (hostapd) saved in ${TMP_LOGS_PATH}/confs"
	echo "# if exist, ${NVRAM_LOGS_LOC} files saved in ${TMP_LOGS_PATH}/nvram_logs"
	echo "# Saved dmesg and journalctl(if exist) logs"
	echo "# Saved wave_components and CV version"
	echo "# Saved memory info in ${TMP_LOGS_PATH}/memory_info"
	echo "###########################################################"
	echo ""
	zip_full_logs
elif [ $optionFlag == "-d" ]
then	
	mkdir -p $TMP_LOGS_PATH
	rm -rf ${TMP_LOGS_PATH}/*
	save_logs
	echo ""
	echo "###########################################################"
	echo "The following logs were saved:"
	echo "# Logs of $param_1_file and $param_3_file have been copied to $WLAN_LOG_FILE_PATH"
	echo "# /var/log files saved in ${TMP_LOGS_PATH}/var_log folder"
	echo "# DB files saved in ${TMP_LOGS_PATH}/${DB_LOC_NAME}"
	echo "# Conf files (hostapd) saved in ${TMP_LOGS_PATH}/confs"
	echo "# if exist, ${NVRAM_LOGS_LOC} files saved in ${TMP_LOGS_PATH}/nvram_logs"
	echo "# Saved dmesg and journalctl(if exist) logs"
	echo "# Saved wave_components and CV version"
	echo "# Saved memory info in ${TMP_LOGS_PATH}/memory_info"
	echo "###########################################################"
	echo ""
	zip_full_logs
	if [[ $optionIP ]]; then
		echo "trying to send file using tftp to address ${optionIP}"
		cd ${TMP_PATH}
		tftp -pr full_logs_${wave_release_minor}_${current_date}.tar.gz $optionIP
		cd -
	fi
elif [ $optionFlag == "-e" ]
then	
	mkdir -p $TMP_LOGS_PATH
	rm -rf ${TMP_LOGS_PATH}/*
	save_extension_logs
	echo ""
	echo "###########################################################"
	echo "The following logs were saved:"
	echo "# bridge,mtlk version,peerflowstatus,channel,tx_power,cal_wlan,temperature,etc"
	echo "###########################################################"
	echo ""
	zip_extension_logs
	if [[ $optionIP ]]; then
		echo "trying to send file using tftp to address ${optionIP}"
		cd ${TMP_PATH}
		tftp -pr extension_logs_${wave_release_minor}_${current_date}.tar.gz $optionIP
		cd -
	fi	
elif [ $optionFlag == "-c" ]
then
	[ -n "$SYSLOG_NG_UTIL" ] && echo "THIS OPTION IS NOT SUPPORTED ON THIS PLATFROM" && exit
	[ -n "$SYSLOG_CONF" ] && cp $SYSLOG_CONF $DEBUG_SYSLOG_CONF
	echo "destination d_wlan { file(\"/var/log/wlan\" template(t_log_format) ); };" >> $DEBUG_SYSLOG_CONF
	echo "filter f_wlan { match('HALWLAN') or match('mtlk') or program('hostapd'); };" >> $DEBUG_SYSLOG_CONF
	echo "log { source(s_src); filter(f_wlan); destination(d_wlan); };" >> $DEBUG_SYSLOG_CONF

	echo ""
	echo "###########################################################"
	echo "from now on all logs of hostapd, mtlk and wlan will output to /var/log/wlan"
	echo "this will be reverted in the next reboot"
	echo "###########################################################"
	echo ""
	[ -n "$SYSLOG_NG_UTIL" ] && syslog-ng -f $DEBUG_SYSLOG_CONF
elif [ $optionFlag == "-a" ]; then
	mkdir -p $TMP_LOGS_PATH
	rm -rf ${TMP_LOGS_PATH}/*
	do_debug_assert
	zip_db
	zip_confs
	save_logs
	TAR_NAME=${TMP_PATH}/${MY_NAME_DebugLog}_${current_date}.tar.gz
	tar -C $TMP_PATH -czf $TAR_NAME $TMP_LOGS_PATH
	rm -rf ${TMP_LOGS_PATH}/*
	rm -rf ${DUMP_IN_PATH}*
	echo "logs, dumps ,db params were copied to ${TAR_NAME}"
	if [[ $optionIP ]]; then
		echo "trying to send file using tftp to address ${optionIP}"
		cd ${TMP_PATH}
		tftp -pr ${MY_NAME_DebugLog}_${current_date}.tar.gz $optionIP
		cd -
	fi
else
	echo "usage:"
	echo "collect logs to a single file: wavToolBox wdc"
	echo "collect logs, DB and FW dumps (trigger FW Assert) of all cards to a single file:  wavToolBox wcda [tftp server address to send to]"
	echo "    IMPORTANT: This options asserts the firmware of all cards!"
	echo "Only for RDKB:reconfig the syslog to save all future logs to a single file: wavToolBox wcdc"
fi
