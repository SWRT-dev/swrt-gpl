#!/bin/sh

# Test script for Wi-Fi Sigma Control API for APs
# Commands based on version 10.2.0
#
# On target board run:
# $ cd /tmp
# $ mknod sigma-pipe p
# $ nc -l -p 9000 < sigma-pipe | ./sigma-ap.sh > sigma-pipe &
# or as persistent
# while true; do nc -l -p 9000 < sigma-pipe | "./sigma-ap.sh" > ./sigma-pipe; done &
# On PC run:
# $ nc <board-ip> 8989
# or
# connect with telnet client in raw mode to <board-ip>:8989
# Then enter commands and send them with <ENTER>

# First digit - Major: Changes incompatible with previous version
# Second digit - Minor: Extension features
# Third digit - Fix: Fixes
# TODO: Store md5 checksum inside the script

source_flag=`cat "/proc/$PPID/cmdline" | grep -oE '[^/]+$' | sed 1d`

. /lib/wifi/platform_dependent.sh

# Read user input eg user_given_num_users_ofdma.. etc, prefix user_given, do not use _ap_ as variable having _ap_ will be unset after each capi call
USER_INPUT_FILE=/tmp/userinput
if [ -f $USER_INPUT_FILE ]; then
	. $USER_INPUT_FILE
fi
ORIG_IFS=$IFS
if [ "$OS_NAME" = "RDKB" ]; then
	WAVE_VERSION_FILE="/tmp/wave_components.ver"
	WIFI_RELOAD_CMD="systemctl restart systemd-netifd.service"
	MODEL=$OS_NAME
elif [ "$OS_NAME" = "UGW" ]; then
	WAVE_VERSION_FILE="/etc/wave_components.ver"
	WIFI_RELOAD_CMD="/etc/init.d/network restart"
	nof_interfaces=`iw phy | grep -c Wiphy`
    	atom_cpu=`version.sh | grep "CPU:" | grep -i atom`
	
	if [ "$nof_interfaces" = "3" ]; then
		hw_revision_wlan4=`cat /proc/net/mtlk/wlan4/eeprom_parsed | grep "HW revision" | awk '{print $4}'`
		hw_revision_wlan0=`cat /proc/net/mtlk/wlan0/eeprom_parsed | grep "HW revision" | awk '{print $4}'`
	elif [ "$nof_interfaces" = "2" ]; then
		hw_revision_wlan0=`cat /proc/net/mtlk/wlan0/eeprom_parsed | grep "HW revision" | awk '{print $4}'`
		hw_revision_wlan2=`cat /proc/net/mtlk/wlan2/eeprom_parsed | grep "HW revision" | awk '{print $4}'`
	fi

	if [ -n "$atom_cpu" ]; then
		[ "$nof_interfaces" = "3" ] && [ "$hw_revision_wlan4" = "0x4E" ] && MODEL="AX11000"
		[ "$nof_interfaces" = "3" ] && [ "$hw_revision_wlan4" = "0x55" ] && MODEL="AX11000" && cp /lib/netifd/lgm_radio_map /opt/intel/wave/radio_map_file
		[ "$nof_interfaces" = "2" ] && MODEL="AX11000"
	else
		[ "$nof_interfaces" = "3" ] && [ "$hw_revision_wlan4" = "0x4E"  -o "$hw_revision_wlan4" = "0x42" ] && MODEL="AX7800"
		[ "$nof_interfaces" = "2" ] && [ "$hw_revision_wlan0" = "0x41" ] && [ "$hw_revision_wlan2" = "0x42" ] && MODEL="AX6000"
		[ "$nof_interfaces" = "2" ] && [ "$hw_revision_wlan0" = "0x45" ] && [ "$hw_revision_wlan2" = "0x45" ] && MODEL="AX3000"
	fi
fi

error_print()
{
	echo "sigma ERROR: $*" > /dev/console
}

info_print()
{
	echo "sigma INFO: $*" > /dev/console
}

if [ "$MODEL" = "" ]; then
	MODEL="MXL-AP"
    error_print "check eeprom card values"
fi

WAVE_VERSION=`grep wave_release_minor $WAVE_VERSION_FILE`
WAVE_VERSION=${WAVE_VERSION##*=}
WAVE_VERSION=${WAVE_VERSION//\"}

CA_VERSION="Sigma-CAPI-10.12.0-${WAVE_VERSION}"

VENDOR='Maxlinear'
DEBUGGING=1 # 1=enabled 0=disabled
# TIMESTAMP="cat /proc/uptime"
TIMESTAMP=

FACTORY_CMD=$SCRIPTS_PATH/wave_factory_reset.sh
UCI_CMD=/lib/config/uci.sh
HOSTAPD_CLI_CMD=hostapd_cli
if [ "$MODEL" != "AX11000" ] && [ "$OS_NAME" = "UGW" ]; then
	HOSTAPD_CLI_CMD="sudo -u nwk -- $HOSTAPD_CLI_CMD"
fi

#UPDATE_RNR_CLI uses hostapd_cli and UPDATE_RNR_UCI uses uci
#Use user configuration accordingly
UPDATE_RNR_UCI=/lib/netifd/update_rnr_uci.sh
UPDATE_RNR_CLI=/lib/netifd/update_rnr.sh
if [ "$MODEL" != "AX11000" ] && [ "$OS_NAME" = "UGW" ]; then
	UPDATE_RNR_CLI="sudo -u nwk -- $UPDATE_RNR_CLI"
fi

dirname()
{
	full=$1
	file=`basename $full`
	path=${full%%$file}
	[ -z "$path" ] && path=./
	echo $path
}
thispath=`dirname $0`

debug_print()
{
	if [ "$DEBUGGING" = "1" ]; then
		echo "sigma DEBUG: $*" > /dev/console
	fi
}

# array like helper functions (ash)
values_get_count()
{
	local values=$1
	local delimiter=$2

	echo "$values" | awk -F"$delimiter" '{print NF}'
}

values_get_at()
{
	local values=$1
	local delimiter=$2
	local idx=$3

	let idx=idx+1
	echo "$values" | awk -F"$delimiter" '{print $'$idx'}'
}

values_replace_at()
{
	local values=$1
	local delimiter=$2
	local idx=$3
	local replace_with=$4

	local _out_values=""
	local _idx=0

	local _old_IFS=$IFS
	IFS=$delimiter
	for _val in $values; do
		if [ "$idx" -eq "$_idx" ] && [ -n "$replace_with" ]
		then
			_out_values="$_out_values$delimiter$replace_with"
		else
			_out_values="$_out_values$delimiter$_val"
		fi
		let _idx=_idx+1
	done
	IFS=$_old_IFS

	echo "$_out_values" | cut -c 2- # remove first '$delimiter'
}
is_6g_supported()
{
	local phys=$(ls /sys/class/ieee80211/)

	if [ "$ucc_program" != "he" ]; then
		echo "0"
		return
	fi

	for phy in $phys
	do
		`iw $phy info | grep "* 60.. MHz" > /dev/null`
		is_phy_6g=$?

		if [ $is_phy_6g = '0' ]; then
			echo "1"
			return
		fi
	done
	echo "0"
	return
}
kill_sigma_mbo_daemon()
{
	local old_ifs
	local killwatchdog=0
	local daemon_pid=`ps -w | grep sigma_mbo_daemon.sh | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
	old_ifs=$IFS
	IFS=' '
	while [ "$daemon_pid" != "" ]; do
		for p in $daemon_pid
		do
			#pidtoKill=$(echo $p | tr -d '\n')
			pidtoKill=$p
			kill "$pidtoKill"
		done
		daemon_pid=`ps -w | grep sigma_mbo_daemon.sh | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
		let killwatchdog=killwatchdog+1
	done
	IFS=$old_ifs

	info_print Sigma Kill Watchdog: $killwatchdog
}

kill_sigmaManagerDaemon()
{
	local old_ifs
	local killwatchdog=0
	local daemon_pid=`ps -w | grep sigmaManagerDaemon | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
	old_ifs=$IFS
	IFS=' '
	while [ "$daemon_pid" != "" ]; do
		for p in $daemon_pid
		do
			#pidtoKill=$(echo $p | tr -d '\n')
			pidtoKill=$p
			kill "$pidtoKill"
		done
		daemon_pid=`ps -w | grep sigmaManagerDaemon | grep -v grep | awk '{ print $1 }' | tr  '\n' ' '`
		let killwatchdog=killwatchdog+1
	done
	IFS=$old_ifs

	info_print Sigma Kill Watchdog: $killwatchdog
}

sortPlan()
{
	local static_plan_config subband_pos1 num_sta ru_pos1 subband_ru_list splanconfigRUsorted offset \
	subband_pos ru_pos i subband p ru subband_ru1 subband_ru2 subband_ru3 subband_ru4 sortedList \
	sorted_subband_ru1 sorted_subband_ru2 sorted_subband_ru3 sorted_subband_ru4 \
	old_ifs next_pos index skip_ru pos cur_subband_ru cur_subband cur_ru

	static_plan_config=$1
	subband_pos1=$2
	num_sta=$3
	subband_ru_list=""
	splanconfigRUsorted=""
	offset=19
	ru_pos1=$((subband_pos1+1))

	subband_pos=$subband_pos1
	ru_pos=$ru_pos1
	i=1
	while [ $i -le $num_sta ]
	do
		subband=`echo $static_plan_config | awk -v p="$subband_pos" '{print $p}'`
		ru=`echo $static_plan_config | awk -v p="$ru_pos" '{print $p}'`
		eval subband_ru${i}="${subband}${ru}"
		subband_pos=$((subband_pos+offset))
		ru_pos=$((ru_pos+offset))
		i=$((i+1))
	done

	subband_ru_list="$subband_ru1 $subband_ru2 $subband_ru3 $subband_ru4"
	sortedList=$(echo $subband_ru_list | tr " " "\n" | sort -n)
	sortedList=$(echo $sortedList | tr "\n" " ")

	eval sortedList_${subband_pos1}="$sortedList"

	i=1
	while [ $i -le $num_sta ]
	do
		eval sorted_subband_ru${i}=`echo $sortedList | awk -v p="$i" '{ print $p }'`
		i=$((i+1))
	done
	i=0
	old_ifs=$IFS
	IFS=' '
	next_pos=$subband_pos1
	index=1
	skip_ru=no
	for pos in $static_plan_config
	do
		i=$((i+1))
		if [ "$skip_ru" = "yes" ]
		then
			skip_ru=no
		elif [ $i -eq $next_pos ]
		then
			eval cur_subband_ru=\${sorted_subband_ru${index}}
			cur_subband=${cur_subband_ru:0:1}
			cur_ru=${cur_subband_ru:1:1}
			splanconfigRUsorted="$splanconfigRUsorted $cur_subband $cur_ru"
			skip_ru=yes
			next_pos=$((next_pos+offset))
			index=$((index+1))
		else		
			splanconfigRUsorted="$splanconfigRUsorted $pos"
		fi
	done
	splanconfigRUsorted=`echo $splanconfigRUsorted`
	IFS=$old_ifs
	echo $splanconfigRUsorted
}

##SP_DEBUG_INFRA
## function to send debug static plan from a known set of SP cmds.
## Before taking a cmd from the file - will check that the file is ready for reading,
## and not occupied by other proceess.
static_plan_send_debug_infra()
{
		local users=$1
		local forced_static_plan
		local blocking_loop_idx=5

		## checking the file is not occupied by another process, if yes wait..
		while [ ! -r "/tmp/${glob_ssid}_tmp" ] && [ "$blocking_loop_idx" != "0" ]
		do
			blocking_loop_idx=$((blocking_loop_idx--))
			sleep 1
		done
		[ "$blocking_loop_idx" = "0" ] && info_print "SP_DEBUG_INFRA:ERROR cannot read file for static plan cmd" && exit -1

		## take the top first line from the file.
		forced_static_plan=`sed -n '1p' "/tmp/${glob_ssid}_tmp"`
		## file is blocked by removing read permission untill cmd send
		chmod -r /tmp/${glob_ssid}_tmp
		##delete it after read for next cmd.
		sed -i '1d' "/tmp/${glob_ssid}_tmp"
		## remove any un-needed spaces
		forced_static_plan=${forced_static_plan//'  '/' '}
		## send the cmd
		if [ -n "$forced_static_plan" ]; then
			eval $forced_static_plan
			info_print "SP_DEBUG_INFRA:FORCED plan for $users users: $forced_static_plan"
		else
			info_print "SP_DEBUG_INFRA:!!!ERROR!!! cannot send empty static plan cmd"
			exit -1
		fi
		## allow reading for next cmd
		chmod +r /tmp/${glob_ssid}_tmp
}
static_plan_init_debug_infra()
{
	## debug infrastructure (SP_DEBUG_INFRA)
	## 1. Create a file in tmp dir with the same ssid that test is using e.g. HE-4.36.1_24G
	## Note: check the UCC log for getting the exact name.
	## 2. Copy all iw dev wlan iwlwav sMuStatPlanCfg ... cmds line after line ( SP off commands are not needed).
	if [ -e "/tmp/$glob_ssid" ]; then
		dos2unix "/tmp/$glob_ssid"
		##remove all empty lines
		sed -ir '/^\s*$/d' "/tmp/$glob_ssid"
		[ -e "/tmp/${glob_ssid}_tmp" ] && rm -f "/tmp/${glob_ssid}_tmp"
		## create a tmp working file
		cp "/tmp/$glob_ssid" "/tmp/${glob_ssid}_tmp"
		## remove wirte permission we need only read
		chmod -w /tmp/${glob_ssid}_tmp
		info_print "SP_DEBUG_INFRA:FORCED plans is activate"
	fi
}

# check if tr command is available
# lower/upper case conversion is faster with external tr
TR=`command -v tr`
if [ "$TR" = "" ]; then
	debug_print "tr not available"
	alias lower=lc_int
	alias upper=uc_int
else
	debug_print "tr available at $TR"
	alias lower=lc_ext
	alias upper=uc_ext
fi

##### Helper Functions #####

# default values for dut mode
nss_def_val_dl="2"	# Range: 1-4
mcs_def_val_dl="11"	# Range: 0-11
nss_def_val_ul="2"	# Range: 1-4
mcs_def_val_ul="11"	# Range: 0-11

# default values for testbed mode
nss_def_val_dl_testbed="2"	# Range: 1-4
mcs_def_val_dl_testbed="7"	# Range: 0-11
nss_def_val_ul_testbed="2"	# Range: 1-4
mcs_def_val_ul_testbed="7"	# Range: 0-11

convert_fixed_ltf_gi()
{
	is_auto=$1
	ltf_and_gi_value=$2

	case "$is_auto" in
		"Auto") is_auto_converted="1" ;;
		"Fixed") is_auto_converted="0" ;;
	esac

	case "$ltf_and_gi_value" in
		"HtVht0p4usCP") ltf_and_gi_value_converted="0" ;;
		"HtVht0p8usCP") ltf_and_gi_value_converted="1" ;;
		"He0p8usCP2xLTF") ltf_and_gi_value_converted="2" ;;
		"He1p6usCP2xLTF") ltf_and_gi_value_converted="3" ;;
		"He0p8usCP4xLTF") ltf_and_gi_value_converted="4" ;;
		"He3p2usCP4xLTF") ltf_and_gi_value_converted="5" ;;
	esac
}

convert_fixed_rate()
{
	band_width=$1
	phy_mode=$2

	case "$band_width" in
		"20MHz") bw_converted="0" ;;
		"40MHz") bw_converted="1" ;;
		"80MHz") bw_converted="2" ;;
		"160MHz") bw_converted="3" ;;
	esac
	
	case "$phy_mode" in
		"a")  phym_converted="0" ;;
		"b")  phym_converted="1" ;;
		"n")  phym_converted="2" ;;
		"ac") phym_converted="3" ;;
		"ax") phym_converted="4" ;;
		"ax_su_ext") phym_converted="5" ;;
		"*") ;; # not found
	esac
}


convert_Operation_ChWidth()
{
	band_width=$1

	case "$band_width" in
		"20MHz") vht_oper_chwidth_converted="0" ;;
		"40MHz") vht_oper_chwidth_converted="0" ;;
		"80MHz") vht_oper_chwidth_converted="1" ;;
		"160MHz") vht_oper_chwidth_converted="2" ;;
	esac	
}



get_index_from_db()
{
        param="$1"
        values=`$UCI_CMD get $param`
		values=`echo $values | sed 's/=/ /g'`
        x=${2/x}
        let x=x+1
        echo `values_get_at "$values" " " "$x"`
}

convert_bf_mode()
{
	# Define local parameters
	local bf_mode sBfMode
	upper "$1" bf_mode 
	
	case "$bf_mode" in
		"AUTO") sBfMode="0xff" ;;
		"EXPLICIT") sBfMode="0" ;;
		"IMPLICIT") sBfMode="1" ;;
		"STBC1X2") sBfMode="2" ;;
		"STBC2X4") sBfMode="3" ;;
		"DISABLED") sBfMode="4" ;;
	esac
	echo $sBfMode
}
check_all_interfaces()
{
	local total_vaps total_up_vaps
	if [ -z "$1" ]; then
		total_vaps=`eval uci show wireless | grep ifname | cut -d"=" -f2 | wc -w`
		total_up_vaps=`eval ifconfig | grep -c wlan`
	else
		total_vaps=`eval uci show wireless | grep ifname | grep $1 | cut -d"=" -f2 | wc -w`
		total_up_vaps=`eval ifconfig | grep $1 | grep -c wlan`
	fi
	if [ $total_vaps -eq $total_up_vaps ]
	then
		echo 0
	else
		echo 1
	fi
}

ap_config_transition_owe()
{
	local bss_owe bss_open
	# All OWE tests are reset to WPA3 program defaults and hence all the BSSes will have WPA3 encryption enabled by default
	# Safe to use the grep approach to derive the UCI path for OWE and OPEN network configurations as there will be
	# no other BSS configured with OWE or OPEN security configuration
	bss_owe=`eval uci show wireless | grep encryption | grep owe | awk -F"." '{print $1 "." $2}'`
	if [ -z "$bss_owe" ]; then
		return
	fi
	bss_open=`eval uci show wireless | grep encryption | grep none | awk -F"." '{print $1 "." $2}'`
	if [ -z "$bss_open" ]; then
		return
	fi
	ap_open_macaddr=`$UCI_CMD get $bss_open.macaddr`
	ap_open_ssid=`$UCI_CMD get $bss_open.ssid`
	#Set the SSID of OWE to dummy string "Owe"; eventually will be configured to hidden
	$UCI_CMD set $bss_owe.ssid=Owe
	$UCI_CMD set $bss_owe.owe_transition_ssid=$ap_open_ssid
	$UCI_CMD set $bss_owe.owe_transition_bssid=$ap_open_macaddr
	$UCI_CMD set $bss_owe.hidden=1
	ap_owe_macaddr=`$UCI_CMD get $bss_owe.macaddr`
	ap_owe_ssid=`$UCI_CMD get $bss_owe.ssid`
	$UCI_CMD set $bss_open.owe_transition_ssid=$ap_owe_ssid
	$UCI_CMD set $bss_open.owe_transition_bssid=$ap_owe_macaddr
	return
}

check_dfs_channel()
{
	radio=`uci show wireless | grep "band='5GHz'" | cut -d"." -f1-2`
	channel=`uci show $radio.channel | cut -d"=" -f2 | tr -d "'"`
	bw=`uci show $radio.htmode | cut -d"=" -f2 | tr -d "'"`

	if [ "$bw" = "VHT160" ] || [ "$bw" = "HT160" ] || { [ $channel -gt 48 ] && [ $channel -lt 149 ]; }; then
		debug_print "$channel is a dfs channel"
		echo 1
	else
		debug_print "$channel is not a dfs channel"
		echo 0
	fi
}

verify_interfaces_up()
{
	local timeout=0
	local max_timeout=90
	local is_dfs_channel=`check_dfs_channel`

	if [ "$is_dfs_channel" = "0" ]; then
		max_timeout=30
	fi

	while [ `check_all_interfaces $1` -eq 1 ] && [ $timeout -lt $max_timeout ]
	do
		sleep 1
		timeout=$((timeout+1))
	done

	if [ $timeout -lt $max_timeout ]
	then
		echo 0
	else
		echo 1
	fi
}

# Static planner - Defined indexes names and location 
#common SP
operation_mode_idx=0
txop_com_max_tx_op_duration_idx=1
sequence_type_idx=2
txop_com_start_bw_limit_idx=3 
dl_com_phases_format_idx=4
dl_com_num_of_participating_stations_idx=5
dl_com_mu_type_idx=6 
dl_com_number_of_phase_repetitions_idx=7
dl_com_maximum_ppdu_transmission_time_limit_idx=8
dl_com_rf_power_idx=9 
dl_com_he_cp_idx=10
dl_com_he_ltf_idx=11 
ul_com_he_cp_idx=12
ul_com_he_ltf_idx=13
rcr_com_tf_hegi_and_ltf_idx=14
rcr_com_tf_length_idx=15 
tf_com_psdu_rate_idx=16 
rcr_com_he_sig_a_spatial_reuse_idx=17 
rcr_com_stbc_idx=18 
nfrp_start_aid_idx=19 
nfrp_multiplexingFlag_idx=20
nfrp_feedbackType_idx=21

#Per_User SP - Defined indexes names and location 
dl_usr_usp_station_indexes_idx=0 
dl_usr_psdu_rate_per_usp_idx=1 
dl_usr_tid_alloc_bitmap_idx=2
dl_usr_ul_psdu_rate_per_usp_idx=3 
dl_usr_bf_type_idx=4 
dl_usr_sub_band_per_usp_idx=5 
dl_usr_start_ru_per_usp_idx=6 
dl_usr_ru_size_per_usp_idx=7
tf_usr_tf_starting_ss_idx=8 
tf_usr_tf_mpdu_mu_spacing_factor_idx=9 
tf_usr_tf_padding_idx=10 
rcr_tf_usr_target_rssi_idx=11 
rcr_tf_usr_ldpc_idx=12 
rcr_tf_usr_psdu_rate_idx=13 
rcr_tf_usr_sub_band_idx=14
rcr_tf_usr_start_ru_idx=15 
rcr_tf_usr_ru_size_idx=16 
rcr_tf_usr_ss_allocation_idx=17 
rcr_tf_usr_coding_type_bcc_or_lpdc_idx=18

send_running()
{
	IFS=,
	echo "status,RUNNING " `eval $TIMESTAMP`
	IFS=$ORIG_IFS
}

# First char for these function needs to be a "," to be able to also send replies
# without parameters
send_complete()
{
	IFS=,
	echo "status,COMPLETE$*" `eval $TIMESTAMP`
	IFS=$ORIG_IFS
}

send_error()
{
	IFS=,
	echo "status,ERROR$*" `eval $TIMESTAMP`
	IFS=$ORIG_IFS
}

send_invalid()
{
	IFS=,
	echo "status,INVALID$*" `eval $TIMESTAMP`
	IFS=$ORIG_IFS
}

#
# TODO: Check whether sed works faster when all the variables are changed at once
#		Maybe the file read only once in this case.
#

UPPERCHARS=ABCDEFGHIJKLMNOPQRSTUVWXYZ
LOWERCHARS=abcdefghijklmnopqrstuvwxyz

lc_int()
{
	# usage: lc "SOME STRING" "destination variable name"
	i=0
	OUTPUT=""
	while ([ $i -lt ${#1} ]) do
		CUR=${1:$i:1}
		case $UPPERCHARS in
			*$CUR*)
			CUR=${UPPERCHARS%$CUR*}
			OUTPUT="${OUTPUT}${LOWERCHARS:${#CUR}:1}"
		;;
		*)
			OUTPUT="${OUTPUT}$CUR"
		;;
		esac
		i=$((i+1))
	done
	debug_print "lower-${OUTPUT}"
	export ${2}="${OUTPUT}"
}

lc_ext()
{
	export ${2}=`echo $1 | tr '[A-Z]' '[a-z]'`
}

uc_int()
{
	# usage: uc "some string" -> "SOME STRING"
	i=0
	OUTPUT=""
	while ([ $i -lt ${#1} ]) do
		CUR=${1:$i:1}
		case $LOWERCHARS in
			*$CUR*)
				CUR=${LOWERCHARS%$CUR*}
				OUTPUT="${OUTPUT}${UPPERCHARS:${#CUR}:1}"
			;;
			*)
				OUTPUT="${OUTPUT}$CUR"
			;;
		esac
		i=$((i+1))
	done
	debug_print "upper-${OUTPUT}"
	export ${2}="${OUTPUT}"
}

uc_ext()
{
	export ${2}=`echo $1 | tr '[a-z]' '[A-Z]'`
}

Dynamic_set_get_helper_none_debug() 
{
		info_print "parameters=$@"
        interface_name="$1"
        param="$2"
        shift
        shift
        field="$1"
        shift
        values=`$UCI_CMD get $param`
		values=`echo $values | sed 's/=/ /g'`
		info_print "none values=$values"
        local num_values=`values_get_count "$values" " "`
        for x in $@; do
                local tmp_val=`values_get_at "$x" "=" "0"`
                local idx=${tmp_val/x}
                local value=`values_get_at "$x" "=" "1"`
                if [ "$idx" -lt "$num_values" ]; then
                        values=`values_replace_at "$values" " " "$idx" "$value"`
                fi
        done
		info_print "iw dev $interface_name iwlwav ${field} $values"
        eval iw dev $interface_name iwlwav ${field} $values
		info_print "none $UCI_CMD set ${param}=\"$values\""
		$UCI_CMD set ${param}="$values"
		eval iw $interface_name iwlwav $field ${values/${field}}
}

Dynamic_set_get_helper()
{
		info_print "parameters=$@"
		rf_flag="$1"
		shift
        interface_name="$1"
        param="$2"
        shift
        shift
        field="$1"
        shift
		values=`$UCI_CMD get $param`
		values=`echo $values | sed 's/=/ /g'`
		info_print "values=$UCI_CMD get $param"
		info_print "values=$values"
        local num_values=`values_get_count "$values" " "`
        for x in $@; do
                local tmp_val=`values_get_at "$x" "=" "0"`
                local idx=${tmp_val/x}
                local value=`values_get_at "$x" "=" "1"`
                let idx++
                if [ "$idx" -lt "$num_values" ]; then
                        values=`values_replace_at "$values" " " "$idx" "$value"`
                fi
        done
		info_print "$UCI_CMD set ${param}=\"$values\""
		$UCI_CMD set $param="$values"

		if [ "${rf_flag}" = "iw_on" ]; then
			iw $interface_name iwlwav $field ${values/${field}}
		fi
		if [ "${rf_flag}" = "iw_off" ]; then
			debug_print "Dynamic_set_get_helper iw_off not sending plan"
		fi
}

set_get_helper()
{
	param=`values_get_at "$1" "=" "0"`
	shift
	values=`$UCI_CMD get ${param}`
	values=`echo $values | sed 's/=/ /g'`
	local num_values=`values_get_count "$values" " "`
	for x in $@; do
		local tmp_val=`values_get_at "$x" "=" "0"`
		local idx=${tmp_val/x}
		local value=`values_get_at "$x" "=" "1"`
		let idx++
		if [ "$idx" -lt "$num_values" ]; then
			values=`values_replace_at "$values" " " "$idx" "$value"`
		fi
	done
	info_print "$UCI_CMD set ${param}=\"$values\""
	$UCI_CMD set ${param}="$values"
}

set_get_helper_non_debug()
{
	param="$1"
	shift
	values=`$UCI_CMD get $param`
	values=`echo $values | sed 's/=/ /g'`
	local num_values=`values_get_count "$values" " "`
	for x in $@; do
		local tmp_val=`values_get_at "$x" "=" "0"`
		local idx=${tmp_val/x}
		local value=`values_get_at "$x" "=" "1"`
		if [ "$idx" -lt "$num_values" ]; then
			values=`values_replace_at "$values" " " "$idx" "$value"`
		fi
	done
	info_print "$UCI_CMD set $param=\"$values\""
	$UCI_CMD set $param="$values"
} 

get_nss_mcs_val()
{
	local ap_nss_mcs_val ap_nss ap_mcs

	[ -z "$1" ] && echo "get_nss_mcs_val: ERROR: Missing ap_nss" && return
	[ -z "$2" ] && echo "get_nss_mcs_val: ERROR: Missing ap_mcs" && return

	ap_nss=$1
	ap_mcs=$2

	if [ "$ap_nss" = "1" ] && [ "$ap_mcs" = "7" ]; then
		ap_nss_mcs_val=65532
	elif [ "$ap_nss" = "1" ] && [ "$ap_mcs" = "9" ]; then
		ap_nss_mcs_val=65533
	elif [ "$ap_nss" = "1" ] && [ "$ap_mcs" = "11" ]; then
		ap_nss_mcs_val=65534
	elif [ "$ap_nss" = "2" ] && [ "$ap_mcs" = "7" ]; then
		ap_nss_mcs_val=65520
	elif [ "$ap_nss" = "2" ] && [ "$ap_mcs" = "9" ]; then
		ap_nss_mcs_val=65525
	elif [ "$ap_nss" = "2" ] && [ "$ap_mcs" = "11" ]; then
		ap_nss_mcs_val=65530
	elif [ "$ap_nss" = "3" ] && [ "$ap_mcs" = "7" ]; then
		ap_nss_mcs_val=65472
	elif [ "$ap_nss" = "3" ] && [ "$ap_mcs" = "9" ]; then
		ap_nss_mcs_val=65493
	elif [ "$ap_nss" = "3" ] && [ "$ap_mcs" = "11" ]; then
		ap_nss_mcs_val=65514
	elif [ "$ap_nss" = "4" ] && [ "$ap_mcs" = "7" ]; then
		ap_nss_mcs_val=65280
	elif [ "$ap_nss" = "4" ] && [ "$ap_mcs" = "9" ]; then
		ap_nss_mcs_val=65365
	elif [ "$ap_nss" = "4" ] && [ "$ap_mcs" = "11" ]; then
		ap_nss_mcs_val=65450
	else
		error_print "Unsupported value - ap_nss_cap:$1 ap_mcs_max_cap:$2"
		return
	fi

	info_print "$ap_nss_mcs_val"
	echo $ap_nss_mcs_val
}

get_test_case_name()
{
	local ssid_name tc_name

	[ -z "$1" ] && echo "get_test_case_name: ERROR: Missing ssid name" && echo 0
	ssid_name=$1

	tc_name=${ssid_name##*-}  #trim leading prefix
	tc_name=${tc_name%%_*}    #trim trailing postfix

	echo "$tc_name"
}

get_nof_sta_per_he_test_case()
{
	local ssid_name tc_name_1 
	ssid_name=$1

	tc_name_1=`get_test_case_name $ssid_name`

	case "$tc_name_1" in
			"4.58.1"|"4.68.1"|"5.60.1"|"5.61.1"|"5.73.1"|"5.74.1") nof_sta=1 ;;		
			"4.43.1"|"4.46.1"|"4.53.1"|"4.53.2"|"4.53.3"|"4.54.1"|"4.56.1"|"4.63.1"|"4.64.1"|"5.47.1"|"5.48.1"|"5.52.1"|"5.54.1"|"5.55.1"|"5.57.1"|"5.57.2"|"5.57.3"|"5.58.1") nof_sta=2 ;;
			"4.29.1"|"4.36.1"|"4.37.1"|"4.40.1"|"4.40.2"|"4.40.3"|"4.40.4"|"4.40.5"|"4.41.1"|"4.41.2"|"4.44.1"|"4.49.1"|"4.45.1"|"4.30.1"|"4.44.1"|"4.55.1"|"4.60.1"|"4.62.1"|"4.69.1"|"5.44.1"|"5.44.2"|"5.44.3"|"5.44.4"|"5.44.5"|"5.44.6"|"5.44.7"|"5.44.8"|"5.44.9"|"5.45.1"|"5.45.2"|"5.49.1"|"5.50.1"|"5.53.1") nof_sta=4 ;;
			*) nof_sta=0 ;; # not found
	esac
	if [ "$ap_program" = "HE" ]; then
                [ -z "$global_ap_num_users_ofdma" ] && [ -n "$user_given_num_users_ofdma" ] && nof_sta=$user_given_num_users_ofdma
                [ -n "$global_ap_num_users_ofdma" ] && nof_sta=$global_ap_num_users_ofdma
		debug_print "nof_sta:$nof_sta"
       fi
}

get_common_uci_path()
{
	# Get list of bands from DB
	ap_radio0_band=`$UCI_CMD get wireless.radio0.band 2>/dev/null`
	ap_radio2_band=`$UCI_CMD get wireless.radio2.band 2>/dev/null`
	ap_radio4_band=`$UCI_CMD get wireless.radio4.band 2>/dev/null`

	local ap_band i radio24 radio5 radio6 wlan_name radio
	for i in 0 2 4
	do
		# Map the band and the radio index
		eval ap_band=\${ap_radio${i}_band}
		if [ "$ap_band" = "2.4GHz" ]
		then
			ap_radio_24g_uci_path=wireless.radio${i}
			radio24=radio${i}
			ap_uci_24g_idx=10
			[ $i -gt 0 ] && ap_uci_24g_idx=42
		elif [ "$ap_band" = "5GHz" ]
		then
			ap_radio_5g_uci_path=wireless.radio${i}
			radio5=radio${i}
			ap_uci_5g_idx=42
			[ $i -eq 0 ] && ap_uci_5g_idx=10
		elif [ "$ap_band" = "6GHz" ]
		then
			ap_radio_6g_uci_path=wireless.radio${i}
			radio6=radio${i}
			ap_uci_6g_idx=74
		fi
	done

	for i in 24 5 6
	do
		eval radio=\${radio${i}}
		[ -z $radio ] && continue
		wlan_name=`uci show wireless | grep device=\'${radio}\' | head -1`
		wlan_name=${wlan_name%.*}
		wlan_name=`$UCI_CMD get ${wlan_name}.ifname`
		wlan_name=${wlan_name%%\.*}
		eval ap_wlan_${i}g_name=$wlan_name
	done
}

get_interface_details()
{
	debug_print get_interface_details $*

	# Not using default values and always find a way to detect the interface
	ap_channel=0
	ap_second_channel=0
	ap_interface=
	ap_channel_freq=0
	ap_wlan_tag=0
	ap_cur_wlan_tag=

	while [ "$1" != "" ]; do
		upper "$1" token
		shift
		case "$token" in
			CHANNEL)
				local channel_given="done"
				ap_tmp_ch=`echo $1 | grep ";"`
				if [ $? -eq 0 ]; then
					ap_channels=$1
					ap_channel=${ap_channels%%;*}
					ap_second_channel=${ap_channels##*;}
					CONFIGURE_BOTH_BANDS=1
				else
					ap_channel=$1
				fi
			;;
			INTERFACE)
				local interface_given="done"
				ap_interface=$1
			;;
			WLAN_TAG)
				local tag_given="done"
				ap_wlan_tag=$1
				ap_ucc_wlan_tag=$1
			;;
			CHNLFREQ)
				local channel_freq_given="done"
				ap_channel_freq=$1
			;;
			NONTXBSSINDEX)
				ap_non_tx_index=$1
			;;
		esac
		shift
	done

	if [ "$tag_given" = "done" ]
	then
		eval ap_cur_wlan_tag=\${CURRENT_WLAN_TAG${ap_ucc_wlan_tag}}
		ap_wlan_tag=$(($ap_wlan_tag - 1))
	fi

	if [ "$channel_given" = "" ] && [ "$interface_given" = "" ] && [ "$tag_given" = "" ] && [ "$channel_freq_given" = "" ] && [ "$CURRENT_RADIO_UCI_PATH" != "" ]
	then
		return
	fi

	if [ "$ap_cur_wlan_tag" = "6G" ] || [ "$ap_channel" -eq "33" ] || [ "$ap_channel" -eq "37" ] || [ "$ap_channel" -eq "53" ] || [ "$ap_interface" = "6G" ] || [ "$ap_channel_freq" -gt "5924" ]
	then
		ap_radio_uci_path=$ap_radio_6g_uci_path
		ap_wlan_name=$ap_wlan_6g_name
		if [ "$tag_given" = "done" ]
		then
			eval CURRENT_WLAN_TAG${ap_ucc_wlan_tag}=6G
			RADIO_6G_TAG=${ap_wlan_tag}
		fi
		[ "$BASE_TAG_6G" = "" ] && BASE_TAG_6G="$ap_wlan_tag"
		ap_wlan_tag=$((ap_wlan_tag-BASE_TAG_6G))
		ap_uci_6g_idx=$(($ap_uci_6g_idx + $ap_wlan_tag))
		ap_interface_uci_path=wireless.default_radio$ap_uci_6g_idx
	elif [ "$ap_cur_wlan_tag" = "5G" ] || [ "$ap_channel" -ge "36" ] || [ "$ap_interface" = "5G" ] || [ "$ap_interface" = "5.0" ]
	then
		ap_radio_uci_path=$ap_radio_5g_uci_path
		ap_wlan_name=$ap_wlan_5g_name
		if [ "$tag_given" = "done" ]
		then
			eval CURRENT_WLAN_TAG${ap_ucc_wlan_tag}=5G
			RADIO_5G_TAG=${ap_wlan_tag}
		fi
		[ "$BASE_TAG_5G" = "" ] && BASE_TAG_5G="$ap_wlan_tag"
		ap_wlan_tag=$((ap_wlan_tag-BASE_TAG_5G))
		ap_uci_5g_idx=$(($ap_uci_5g_idx + $ap_wlan_tag))
		ap_interface_uci_path=wireless.default_radio$ap_uci_5g_idx
	else
		ap_radio_uci_path=$ap_radio_24g_uci_path
		ap_wlan_name=$ap_wlan_24g_name
		if [ "$tag_given" = "done" ]
		then
			eval CURRENT_WLAN_TAG${ap_ucc_wlan_tag}=24G
		fi
		[ "$BASE_TAG_24G" = "" ] && BASE_TAG_24G="$ap_wlan_tag"
		ap_wlan_tag=$((ap_wlan_tag-BASE_TAG_24G))
		ap_uci_24g_idx=$(($ap_uci_24g_idx + $ap_wlan_tag))
		ap_interface_uci_path=wireless.default_radio$ap_uci_24g_idx
	fi

	CURRENT_RADIO_UCI_PATH="$ap_radio_uci_path"
	CURRENT_WLAN_NAME="$ap_wlan_name"
	CURRENT_IFACE_UCI_PATH="$ap_interface_uci_path"
	CURRENT_IFACE_IFNAME=`$UCI_CMD get $ap_interface_uci_path.ifname`
}

# Get the index in the db (default_radioX) of the last VAP for a radio.
get_last_vap_for_radio()
{
	local cur_radio="$1"

	local all_ifaces=`uci show wireless | grep device=\'$cur_radio\' | awk -F"." '{print $1 "." $2}' | awk -v RS=  '{$1=$1}1' | tr -d "'"`
	local all_iface_idxs=`echo $all_ifaces | sed -e 's/[^0-9 ]//g'`

	if [ "$cur_radio" = "radio0" ]; then
		local max_idx=10
	elif [ "$cur_radio" = "radio2" ]; then
		local max_idx=42
	elif [ "$cur_radio" = "radio4" ]; then
		local max_idx=74
	fi

	local old_IFS=$IFS
	IFS=$ORIG_IFS
	for curr_idx in $all_iface_idxs
	do
		if [ $curr_idx -lt 100 ] && [ $curr_idx -gt $max_idx ]
		then
			max_idx=$curr_idx
		fi
	done
	IFS=$old_IFS

	echo $max_idx
}

get_new_wlan_name_for_radio()
{
	local last_vap_idx="$1"
	local base_wlan=`$UCI_CMD get wireless.default_radio${last_vap_idx}.ifname`
	base_wlan=${base_wlan%%.*}

	for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
	do
		local cur_wlan=$base_wlan.$i
		local output=`uci show wireless | grep ifname=\'$cur_wlan\'`
		if [ "$output" = "" ]; then
			break
		fi

		if [ "$i" = "14" ]; then # 14 is the max vap allowed
			cur_wlan="ERROR"
		fi
	done

	echo $cur_wlan
}

add_interface()
{
	if [ "$1" != "" ]; then
		local cur_radio_path=wireless.$1
	else
		local cur_radio_path=$CURRENT_RADIO_UCI_PATH
	fi

	local cur_radio=`echo $cur_radio_path | awk -F"." '{print $2}'`
	local last_vap_idx=`get_last_vap_for_radio $cur_radio`
	local new_vap_idx=$((last_vap_idx+1))

	last_vap="wireless.default_radio$last_vap_idx"
	new_vap="wireless.default_radio$new_vap_idx"

	local last_vap_mac=`$UCI_CMD get $last_vap.macaddr`
	local last_vap_flex_mac_hexa=`echo $last_vap_mac | awk -F":" '{print $6}'`
	local last_vap_flex_mac="0x$last_vap_flex_mac_hexa"
	tc_name=`get_test_case_name $glob_ssid`
	#function called only for multibss enabled
	if [ "$ucc_program" = "he" -o "$tc_name" = "4.67.1" ]; then
		local new_vap_flex_mac=`printf '%x' $((last_vap_flex_mac+1))`
	else
		local new_vap_flex_mac=`printf '%x' $((last_vap_flex_mac+2))`
	fi
	local new_vap_flex_mac_len=${#new_vap_flex_mac}

	if [ $new_vap_flex_mac_len -eq 1 ]; then
		new_vap_flex_mac="0$new_vap_flex_mac"
	elif [ $new_vap_flex_mac_len -gt 2 ]; then
		new_vap_flex_mac=`echo -n $new_vap_flex_mac | tail -c 2`
	fi

	local new_vap_mac=`echo $last_vap_mac | awk -F":" '{print $1 ":" $2 ":" $3 ":" $4 ":" $5 ":"}'`
	new_vap_mac="$new_vap_mac$new_vap_flex_mac"

	local new_wlan=`get_new_wlan_name_for_radio $last_vap_idx`

	if [ "$new_wlan" = "ERROR" ]; then
		error_print "No place for new VAP"
		return
	fi

	$UCI_CMD set $new_vap='wifi-iface'
	$UCI_CMD set $new_vap.device=$cur_radio
	$UCI_CMD set $new_vap.ifname=$new_wlan
	$UCI_CMD set $new_vap.macaddr=$new_vap_mac
	$UCI_CMD set $new_vap.mode='ap'
	local tmp=`$FACTORY_CMD vap $new_wlan`
	sleep 1 # workaround , let vap factory uci configuration populate
	$UCI_CMD set $new_vap.mode='ap' # make sure atleast mode is set else vap will not come up
	if [ "$MODEL" != "AX11000" ] && [ "$OS_NAME" = "UGW" ]; then
		chown rpcd:rpcd $UCI_DB_PATH/wireless
		chmod +r $UCI_DB_PATH/wireless
	fi

	echo $new_vap | awk -F"." '{print $2}'
}

create_interface()
{
	debug_print create_interface $*

	ap_uci_vap_idx=$(echo $CURRENT_IFACE_UCI_PATH | sed -e 's/[^0-9 ]//g')
	ap_uci_base_idx=$((ap_uci_vap_idx-ap_wlan_tag))

	ap_base_mac=`$UCI_CMD get wireless.default_radio${ap_uci_base_idx}.macaddr`
	ap_flex_mac_hexa=$(echo $ap_base_mac | awk -F":" '{print $6}')
	ap_flex_mac="0x$ap_flex_mac_hexa"
	ap_new_flex_mac=$(printf '%x' $((ap_flex_mac + 2*ap_wlan_tag)))
	ap_new_flex_mac_len=${#ap_new_flex_mac}

	if [ $ap_new_flex_mac_len -eq 1 ]; then
		ap_new_flex_mac="0$ap_new_flex_mac"
	elif [ $ap_new_flex_mac_len -gt 2 ]; then
		ap_new_flex_mac=$(echo -n $ap_new_flex_mac | tail -c 2)
	fi

	ap_part_mac=$(echo $ap_base_mac | awk -F":" '{print $1 ":" $2 ":" $3 ":" $4 ":" $5 ":"}')
	ap_mew_mac="$ap_part_mac$ap_new_flex_mac"

	ap_radio_name=$(echo $CURRENT_RADIO_UCI_PATH | awk -F"." '{print $2}')

	$UCI_CMD set $CURRENT_IFACE_UCI_PATH='wifi-iface'
	$UCI_CMD set $CURRENT_IFACE_UCI_PATH.device=$ap_radio_name
	$UCI_CMD set $CURRENT_IFACE_UCI_PATH.ifname=$CURRENT_WLAN_NAME.$ap_wlan_tag
	$UCI_CMD set $CURRENT_IFACE_UCI_PATH.macaddr=$ap_mew_mac
	$UCI_CMD set $CURRENT_IFACE_UCI_PATH.mode='ap'
	$FACTORY_CMD vap $CURRENT_WLAN_NAME.$ap_wlan_tag
	if [ "$MODEL" != "AX11000" ] && [ "$OS_NAME" = "UGW" ]; then
		chown rpcd:rpcd $UCI_DB_PATH/wireless
		chmod +r $UCI_DB_PATH/wireless
	fi
}

get_uci_security_mode()
{
	upper "$1" ap_sigma_security_mode

	case "$ap_sigma_security_mode" in
		NONE)
			ap_uci_security_mode="none"
		;;
		WPA-PSK)
			ap_uci_security_mode="psk"
		;;
		WPA2-PSK)
			ap_uci_security_mode="psk2"
		;;
		WPA-ENT)
			ap_uci_security_mode="wpa"
		;;
		WPA2-ENT)
			ap_uci_security_mode="wpa2"
		;;
		WPA2-PSK-MIXED)
			ap_uci_security_mode="psk-mixed+tkip+ccmp"
		;;
		WPA2-MIXED)
			ap_uci_security_mode="wpa-mixed+tkip+ccmp"
		;;
		WPA2-PSK-SAE)
			ap_uci_security_mode="sae-mixed"
		;;
		SAE)
			ap_uci_security_mode="sae"
		;;
		OWE)
			ap_uci_security_mode="owe"
		;;
		SUITEB)
			ap_uci_security_mode="eap192"
		;;
		*)
			error_print "Unsupported security mode: $ap_sigma_security_mode"
			ap_uci_security_mode=""
		;;
	esac
}

get_uci_encryption()
{
	upper "$1" ap_sigma_encrypt

	case "$ap_sigma_encrypt" in
		WEP)
			ap_uci_encrypt="wep"
		;;
		*)
			error_print "Unsupported encryption: $ap_sigma_encrypt"
			ap_uci_encrypt=""
		;;
	esac
}

get_uci_pairwise_cipher()
{
	upper "$1" ap_pairwise_cipher

	ap_cipher=`echo $ap_pairwise_cipher | awk -F"-" '{print $2}'`
	ap_cipher_len=`echo $ap_pairwise_cipher | awk -F"-" '{print $3}'`

	if [ "$ap_cipher_len" = "256" ]; then
		ap_cipher="${ap_cipher}-${ap_cipher_len}"
	fi
}

get_uci_pmf()
{
	upper "$1" ap_sigma_pmf

	case "$ap_sigma_pmf" in
		REQUIRED)
			ap_uci_pmf=2
		;;
		OPTIONAL)
			ap_uci_pmf=1
		;;
		DISABLED)
			ap_uci_pmf=0
		;;
		*)
			error_print "Unsupported pmf: $ap_sigma_pmf"
			ap_uci_pmf=""
		;;
	esac
}

get_freq_from_chan()
{
	ap_freq=`iw $1 info | grep "\[$2\]" | awk '{print $2}'`
}

get_central_freq_vht()
{
	local ap_chan=$1
	local ap_width=$2
	local ap_phy=$3

	case $ap_width in
		20|40)
			get_freq_from_chan $ap_phy $ap_chan
			ap_centr_freq=$ap_freq
		;;
		40-)
			ap_centr_chan=$((ap_chan-2))
			get_freq_from_chan $ap_phy $ap_centr_chan
			ap_centr_freq=$ap_freq
		;;
		40+)
			ap_centr_chan=$((ap_chan+2))
			get_freq_from_chan $ap_phy $ap_centr_chan
			ap_centr_freq=$ap_freq
		;;
		80)
			case $ap_chan in
				36|40|44|48) ap_centr_freq=5210 ;;
				52|56|60|64) ap_centr_freq=5290 ;;
				100|104|108|112) ap_centr_freq=5530 ;;
				116|120|124|128) ap_centr_freq=5610 ;;
				132|136|140|144) ap_centr_freq=5690 ;;
				149|153|157|161) ap_centr_freq=5775 ;;
			esac
		;;
		160)
			case $ap_chan in
				36|40|44|48|52|56|60|64) ap_centr_freq=5250 ;;
				100|104|108|112|116|120|124|128) ap_centr_freq=5570 ;;
			esac
		;;
	esac
}

get_apqos_data_idx()
{
	case $1 in
		vo)
			ap_data_idx=0
		;;
		vi)
			ap_data_idx=1
		;;
		be)
			ap_data_idx=2
		;;
		bk)
			ap_data_idx=3
		;;
	esac
}

set_uci_offset()
{
	ap_offset_mode=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.htmode`
	ap_offset_mode_num=`echo $ap_offset_mode | sed -e 's/[^0-9 ]//g'`

	if [ "$ap_offset" != "" ] && [ $ap_offset_mode_num -eq 40 ]
	then
		ap_new_mode=`echo $ap_offset_mode | sed 's/[^a-zA-Z0-9]//g'`
		ap_new_mode="${ap_new_mode}${ap_offset}"
		$UCI_CMD set $CURRENT_RADIO_UCI_PATH.htmode=$ap_new_mode
	fi
}

config_neighbor()
{
	local band=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.band`
	if [ "$band" = "5GHz" ]; then
		local op_class=73
		local op_chan=36
	elif [ "$band" = "2.4GHz" ]; then
		local op_class=51
		local op_chan=01
	elif [ "$band" = "6GHz" ]; then
		local op_class=131
		local op_chan=37
	fi
	local hex_pref=`printf '%02x' $ap_nebor_pref`
	local nr=`echo $ap_nebor_bssid | sed 's/\://g'`
	nr="${nr}00000000${op_class}${op_chan}070301${hex_pref}"
	ap_cmd="$HOSTAPD_CLI_CMD -i$CURRENT_WLAN_NAME set_neighbor_per_vap $CURRENT_IFACE_IFNAME $ap_nebor_bssid ssid=\\\"MBO_NEIGHBOR\\\" nr=$nr"
	ap_tmp=`eval "$ap_cmd"`
}
merge_pref_non_pref_mbo_neighbors()
{
	#club pref and non_pref neighbors, once wnm received, non_pref will be removed
	CURRENT_NEIGHBORS=$CURRENT_NEIGHBORS" "$CURRENT_NON_PREF_NEIGHBORS
	echo $CURRENT_NEIGHBORS > /tmp/mbo_neighbors
 	rm /tmp/non_pref_mbo_neighbors
	CURRENT_NON_PREF_NEIGHBORS=""
}
add_neighbor_for_channel()
{
	local channel="$1"
	local new_curr_neighbors=""
	local non_pref_neighbors=""

	local old_IFS=$IFS
	IFS=$ORIG_IFS

	CURRENT_NEIGHBORS=`cat /tmp/mbo_neighbors`
	if [ -f "/tmp/non_pref_mbo_neighbors" ]; then
		CURRENT_NON_PREF_NEIGHBORS=`cat /tmp/non_pref_mbo_neighbors`
	fi
	for single_neighbor in $CURRENT_NON_PREF_NEIGHBORS
	do
		local curr_chan=`echo "$single_neighbor" | awk -F"," '{print $4}'`
		if [ "$curr_chan" == "$channel" ]; then
			new_curr_neighbors="$single_neighbor $new_curr_neighbors"
		else
			non_pref_neighbors="$single_neighbor $non_pref_neighbors"
		fi
	done

	CURRENT_NEIGHBORS=$new_curr_neighbors" "$CURRENT_NEIGHBORS
	CURRENT_NON_PREF_NEIGHBORS=$non_pref_neighbors
	echo $CURRENT_NEIGHBORS > /tmp/mbo_neighbors
	echo $CURRENT_NON_PREF_NEIGHBORS > /tmp/non_pref_mbo_neighbors

	IFS=$old_IFS
}

remove_neighbor_for_channel()
{
	local channel="$1"
	local new_curr_neighbors=""
	local non_pref_neighbors=""

	local old_IFS=$IFS
	IFS=$ORIG_IFS

	CURRENT_NEIGHBORS=`cat /tmp/mbo_neighbors`
	if [ -f "/tmp/non_pref_mbo_neighbors" ]; then
		CURRENT_NON_PREF_NEIGHBORS=`cat /tmp/non_pref_mbo_neighbors`
	fi
	for single_neighbor in $CURRENT_NEIGHBORS
	do
		local curr_chan=`echo "$single_neighbor" | awk -F"," '{print $4}'`
		if [ "$curr_chan" != "$channel" ]; then
			new_curr_neighbors="$single_neighbor $new_curr_neighbors"
		else
			non_pref_neighbors="$single_neighbor $non_pref_neighbors"
		fi
	done

	CURRENT_NEIGHBORS=$new_curr_neighbors
	CURRENT_NON_PREF_NEIGHBORS=$non_pref_neighbors" "$CURRENT_NON_PREF_NEIGHBORS
	echo $CURRENT_NEIGHBORS > /tmp/mbo_neighbors
	echo $CURRENT_NON_PREF_NEIGHBORS > /tmp/non_pref_mbo_neighbors

	IFS=$old_IFS
}

##### Command Functions #####

ap_ca_version()
{
	send_running
	send_complete ",version,$CA_VERSION"
}

ca_get_version()
{
	send_running
	send_complete ",version,$CA_VERSION"
}

uci_commit_wireless()
{
	$UCI_CMD commit wireless
	if [ "$MODEL" != "AX11000" ] && [ "$OS_NAME" = "UGW" ]; then
		chown rpcd:rpcd $UCI_DB_PATH/wireless
		chmod +r $UCI_DB_PATH/wireless
	fi
}

run_dwpal_cli_cmd()
{
	local i=0
	while [ "$i" -lt "20" ]; do
		if [ -n "$interface" ]; then
			vap_is_up=`ifconfig | grep -c $interface`
		else
			vap_is_up="1" #MBO Deamon doesn't set interface, no need to wait as it is in infinite loop
		fi
		if [ "$vap_is_up" -gt "0" ] ; then
			if [ "$OS_NAME" = "UGW" ]; then
				local _cmd="dwpal_cli"
				while [ "$1" != "" ]; do
					_cmd="$_cmd '$1'"
					shift
				done
				if [ "$MODEL" == "AX11000" ]; then
					ap_event_msg=`/bin/sh -c "export LD_LIBRARY_PATH=/opt/intel/lib; $_cmd"`
				else
					ap_event_msg=`sudo cap_provide "nwk:nwk,rpcd" "cap_net_admin,cap_net_bind_service" /bin/sh -c "export LD_LIBRARY_PATH=/opt/intel/lib; $_cmd"`
					ap_event_msg=`echo $ap_event_msg | sed -e "s/Userinput: nwk:nwk,rpcd //"`
				fi
			else
				ap_event_msg=`dwpal_cli "$@"`
			fi
		
			echo $ap_event_msg
			break 
		else 
			sleep 1
			i=$((i+1))
		fi
	done
	if [ "$i" -eq "10" ]; then
		error_print "Timeout- VAP is down"
		send_invalid ",errorCode,220"	
	fi
}

check_6g_tc()
{
	local ssid_6g="Wi-Fi_6E_"
	local is_6g_tc=`eval echo $glob_ssid | grep $ssid_6g`
	if [ "$ucc_program" = "he" -a -n "$is_6g_tc" ]; then
		echo "1"
	else
		echo "0"
	fi
}

ap_uci_commit_and_apply()
{
	ap_changes=`$UCI_CMD changes`
	local _cmd="dwpal_cli"

	if [ "$ucc_program" = "wpa3" ]; then
		ap_tmp=`ap_config_transition_owe`
	fi

	if [ `eval check_6g_tc` = "1" ]; then
		ap_tmp=`eval $UPDATE_RNR_UCI enable`
	fi

	uci_commit_wireless
	ap_tmp=`eval $WIFI_RELOAD_CMD`

	if [ "$MODEL" == "AX11000" ]; then
		[ -n "$ap_changes" ] && ap_tmp=`sh -c "export LD_LIBRARY_PATH=/opt/intel/lib; $_cmd -ihostap -mMain -vwlan0.0 -vwlan2.0 -dd -l"AP-ENABLED" -l"INTERFACE_RECONNECTED_OK""`
	elif [ "$OS_NAME" = "UGW" ]; then
		[ -n "$ap_changes" ] && ap_tmp=`sudo -u nwk sh -c "export LD_LIBRARY_PATH=/opt/intel/lib; $_cmd -ihostap -mMain -vwlan0.0 -vwlan2.0 -dd -l"AP-ENABLED" -l"INTERFACE_RECONNECTED_OK""`
	fi

	#WA in 6.1.1 to enable ICMP pings with frame size > 15000
	if [ "$OS_NAME" = "UGW" ]; then
		switch_cli GSW_QOS_METER_CFG_SET dev=1 nMeterId=33 nEbs=17000 nCbs=17000 > /dev/null 2>&1
	fi
}
arguments_file_initializer(){
	echo "export current_radio=$1" >> /tmp/sigma_hf_arguments
	echo "export glob_ssid=$2" >> /tmp/sigma_hf_arguments
}

ap_send_addba_req()
{
	send_running
	send_complete
}

refresh_static_plan()
{
	info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
	send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
	connected_stations=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station`
	if [ "$connected_stations" = "1" ] || [ "$connected_stations" = "2" ] || [ "$connected_stations" = "3" ] || [ "$connected_stations" = "4" ]; then
		send_plan_for_${connected_stations}_users ${CURRENT_WLAN_NAME} $connected_stations $CURRENT_RADIO_UCI_PATH $glob_ssid $CURRENT_WLAN_NAME.0
	fi
}

reload_driver_with_acceleration()
{
		rmmod mtlk;
		rmmod mtlkroot;
		rmmod mac80211;
		rmmod cfg80211;
		rmmod compat;
		if [ "$1" = "0" ]; then
			touch /tmp/driver_reload
			modprobe mtlk
		else
			modprobe mtlk fastpath=1,1,1 ahb_off=1 loggersid=255,255 dual_pci=1,1
			rm	/tmp/driver_reload
		fi
		ap_tmp=`systemctl restart systemd-netifd.service`
}

ap_set_wireless()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	ap_temp_ret=`$UCI_CMD get $CURRENT_IFACE_UCI_PATH 2>/dev/null`
	if [ "$ap_temp_ret" = ""  ]; then
		create_interface
	fi

	debug_print ap_radio_uci_path $CURRENT_RADIO_UCI_PATH
	debug_print ap_interface_uci_path $CURRENT_IFACE_UCI_PATH
	is_interface_known=`echo $@ | grep -i Interface`

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
		NAME)
			debug_print "set parameter ap_name=$1"
		;;
		INTERFACE)
			# skip since it was read in loop before
		;;
		WLAN_TAG)
			debug_print "set parameter ap_wlan_tag=$1"
		;;
		SSID)
			glob_ssid="$1"
			static_plan_init_debug_infra
			debug_print "set parameter ap_ssid=$1"
			
			[ -z "$ap_non_tx_bss_index" ] && $UCI_CMD set $CURRENT_IFACE_UCI_PATH.ssid=$1
			
			tc_name=`get_test_case_name $glob_ssid`
			if [ -z $global_num_non_tx_bss ]; then

				if [ "$tc_name" = "4.68.1" ] || [ "$tc_name" = "5.73.1" ]; then
					info_print "tc_name = $tc_name ==> set ap_ofdma='dl'"
					ap_ofdma="dl"
				fi
			else
				ap_ssid_non_tx_bss_index="$1"
				debug_print "ap_ssid_non_tx_bss_index:$ap_ssid_non_tx_bss_index"
			fi
			if [ $ap_second_channel -gt 0 ]; then
				$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.ssid=$1
				$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.ssid=$1
			else
				if [ "$change_ssid_one_time" != "1" ]; then
					if [ "$tc_name" = "4.67.1" ]; then
						change_ssid_one_time=1
					fi
					[ -z "$ap_non_tx_bss_index" ] && $UCI_CMD set $CURRENT_IFACE_UCI_PATH.ssid=$1
				fi
			fi
			if [ "$glob_ssid" = "4.2.28" ]; then
				$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.s11nProtection=2
				$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.s11nProtection=2
			fi
			if [ "$ucc_program" = "vht" ]; then
				if [ "$glob_ssid" = "VHT-4.2.5" ] || [ "$glob_ssid" = "VHT-4.2.5A" ] || [ "$glob_ssid" = "VHT-4.2.20" ] || [ "$glob_ssid" = "VHT-4.2.21" ] || [ "$glob_ssid" = "VHT-4.2.40" ]; then
					info_print "Disable Aggr"
					$UCI_CMD set $CURRENT_IFACE_UCI_PATH.sAggrConfig=0 1 64
				fi
			fi
			if [ "$glob_ssid" = "ioPL98=2bv" ] && [ $ap_channel -lt 36 ]; then
				$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.vendor_vht=0			
			fi
			if [ "$glob_ssid" = "4.2.12" ]; then
				$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.auth_cache=1
			fi
			if [ "$glob_ssid" = "WiFi1-4.2.6" -o "$glob_ssid" = "WiFi1-4.2.6E" ] && [ "$OS_NAME" = "RDKB" ]; then
				cli system/extswitch/setMacAging 0 1
			fi
			tc_name=`get_test_case_name $glob_ssid`
			if [ "$tc_name" = "4.44.1" ] && [ "$OS_NAME" = "RDKB" ]
			then
				cli sys/pp/setDefensiveMode 1
			fi
			#Workaround required for 11n-4.2.30_3SS AMSDU Rx test
			if [ "$glob_ssid" = "4.2.30" ] && [ "$OS_NAME" = "RDKB" ]
			then
				cli sys/pp/fragBpSupport 1
			fi
			if [ "$tc_name" = "4.14.1" ] && [ $ap_channel -lt 36 ]
			then
				iw dev $CURRENT_WLAN_NAME iwlwav sTxopConfig 0x1FF 0 32767 4
				iw dev $CURRENT_WLAN_NAME iwlwav s11nProtection 1
			fi
			if [ "$glob_ssid" = "HE-4.31.1_24G" ]; then
				if [ "$OS_NAME" = "UGW" ]; then #on LGM, beerocks is disabled else use root user instead sscripts
					ap_tmp=`sudo -u sscript -- /etc/init.d/prplmesh stop`
					sleep 3 # wait for process to stop
				elif [ "$OS_NAME" = "RDKB" ]; then
					ap_tmp=`systemctl stop systemd-prplmesh.target`
					sleep 3 # wait for process to stop
				fi
			fi
			if [ "$glob_ssid" = "NVCX@7" -a "$OS_NAME" = "RDKB" ]; then
				iptables -t mangle -D FORWARD -m state ! --state NEW -j DSCP --set-dscp 0x00
				iptables -t mangle -D FORWARD -m state --state NEW -j DSCP --set-dscp 0x14
				cli system/pp/setProtSesTimeout UDP 20
			fi
			if [ "$glob_ssid" = "4.2.3" ] && [ "$OS_NAME" = "RDKB" ]; then
				debug_print "reload driver with acceleration disable"
				reload_driver_with_acceleration 0
			elif [ -f "/tmp/driver_reload" ]; then
				reload_driver_with_acceleration 1
			fi

		;;
		CHANNEL)
			debug_print "set parameter ap_channel=$1"
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.channel=$ap_channel
			if [ "$ap_channel" -ge "50" -a "$ap_channel" -le "144" ]; then
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.doth=1
				iw dev $CURRENT_WLAN_NAME iwlwav s11hChCheckTime 1
				iw dev $CURRENT_WLAN_NAME iwlwav s11hRadarDetect 0
			fi
			if [ $ap_second_channel -gt 0 ]; then
				if [ "$ap_radio_5g_uci_path" = "$CURRENT_RADIO_UCI_PATH" ]; then
					$UCI_CMD set $ap_radio_24g_uci_path.channel=$ap_second_channel
				else
					$UCI_CMD set $ap_radio_5g_uci_path.channel=$ap_second_channel
				fi
			fi
		;;
		MODE)
			debug_print "set parameter ap_mode=$1"
			#TODO: currently there is no need to support this
			#$UCI_CMD set $ap_radio_uci_path.hwmode=$1
		;;
		WME)
			debug_print "set parameter ap_wme=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.wmm=$1
		;;
		WMMPS)
			debug_print "set parameter ap_wmmps=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.uapsd_advertisement_enabled=$1
		;;
		RTS)
			# param not supported
			debug_print "set parameter ap_rts=$1"
		;;
		FRGMNT)
			# param not supported
			debug_print "set parameter ap_frgmnt=$1"
		;;
		FRGMNTSUPPORT)
			# param not supported
			debug_print "set parameter ap_frgmntsupport=$1"
		;;
		PWRSAVE)
			# param not supported
			debug_print "set parameter ap_pwrsave=$1"
		;;
		BCNINT)
			debug_print "set parameter ap_bcnint=$1"
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.beacon_int=$1
		;;
		DTIM)
			debug_print "set parameter ap_dtim=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.dtim_period=$1
		;;
		SGI20)
			debug_print "set parameter ap_sgi20=$1"
			lower "$1" ap_enable_input
			if [ "$ap_enable_input" = "enable" ]; then
				ap_sgi_20=1
			else
				ap_sgi_20=0
			fi
			$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.short_gi_20=$ap_sgi_20
		;;
		SGI40)
			debug_print "set parameter ap_sgi40=$1"
			lower "$1" ap_enable_input
			if [ "$ap_enable_input" = "enable" ]; then
				ap_sgi_40=1
			else
				ap_sgi_40=0
			fi
			$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.short_gi_40=$ap_sgi_40
		;;
		SGI80)
			debug_print "set parameter ap_sgi80=$1"
			lower "$1" ap_enable_input
			if [ "$ap_enable_input" = "enable" ]; then
				ap_sgi_80=1
			else
				ap_sgi_80=0
			fi
			$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.short_gi_80=$ap_sgi_80
		;;
		STBC_TX)
			debug_print "set parameter ap_stbc_tx=$1"
			#TODO: Comma separated list of the number of spatial streams and the number of space time streams.
			#For example 1spatial stream and 2 space time streams = 1;2
			#$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.tx_stbc=$1
			local ap_tx_bf_val ap_sBfMode_local ap_debug_iw_post_up_idx
			if [ "$1" = "1"  -o "$1" = "1;2" ]; then
				ap_tx_bf_val="STBC1X2" 
			elif [ "$1" = "2" -o "$1" = "2;4" ]; then
				ap_tx_bf_val="STBC2X4"
			else
				debug_print "not correct STBC_TX value"
				send_invalid ",errorCode,2"
			fi
			ap_sBfMode_local=`convert_bf_mode $ap_tx_bf_val`	
			debug_print "converted ap_sBfMode_local=$ap_sBfMode_local"
			ap_debug_iw_post_up_idx=`uci show wireless | grep $CURRENT_RADIO_UCI_PATH | grep debug_iw_post_up | cut -d"=" -f 1 | cut -d"_" -f5 | sort -n | tail -1`
			if [ -z "$ap_debug_iw_post_up_idx" ]; then
				ap_debug_iw_post_up_idx=1
			else
				let "ap_debug_iw_post_up_idx=ap_debug_iw_post_up_idx+1"
			fi
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_iw_post_up_$ap_debug_iw_post_up_idx="sBfMode $ap_sBfMode_local"
		;;
		BW_SGNL)
			debug_print "set parameter ap_bw_sgnl=$1"
		;;
		DYN_BW_SGNL)
			debug_print "set parameter ap_dyn_bw_sgnl=$1"
			if [ "$1" = "enable" ]; then
					$UCI_CMD set $CURRENT_IFACE_UCI_PATH.debug_hostap_conf_1="sRTSmode=1 0"
					$UCI_CMD set $CURRENT_IFACE_UCI_PATH.s11nProtection=2
			elif [ "$1" = "disable" ]; then
					$UCI_CMD set $CURRENT_IFACE_UCI_PATH.debug_hostap_conf_1="sRTSmode=0 1"
					$UCI_CMD set $CURRENT_IFACE_UCI_PATH.s11nProtection=2
					iw dev wlan0 iwlwav sOnlineACM 0
					iw dev wlan2 iwlwav sOnlineACM 0
			fi
		;;
		STBC_RX)
			debug_print "set parameter ap_stbc_rx=$1"
			$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.rx_stbc=$1
		;;
		RADIO)
			debug_print "set parameter ap_radio=$1"
			if [ "$1" = "off" ]; then
				ap_radio_op="down"
			elif [ "$1" = "on" ]; then
				ap_radio_op="up"
			else
				send_invalid ",errorCode,2"
				return
			fi
			if [ -n "$is_interface_known" ]; then
				ap_radio_ifaces=`uci show wireless | grep $CURRENT_WLAN_NAME | awk -F"=" '{print $2}'`
			else
				ap_radio_ifaces=`uci show wireless | grep ifname | awk -F"=" '{print $2}'`
			fi
			for ap_radio_iface in $ap_radio_ifaces; do
				res=`eval ifconfig $ap_radio_iface $ap_radio_op`
			done
			#Verify if interfaces were up for the radio
			if [ "$ap_radio_op" = "up" ]; then
				local ap_radio_verify_up=0
				if [ -z "$is_interface_known" ]; then
					ap_radio_verify_up=`verify_interfaces_up`
				else
					ap_radio_verify_up=`verify_interfaces_up $CURRENT_WLAN_NAME`
				fi
				
				if [ $ap_radio_verify_up -eq 1 ]; then
					error_print "Interfaces are not up after radio ON"
					send_invalid ",errorCode,220"
					return
				fi
			fi
		;;
		P2PMGMTBIT)
			# do nothing
			debug_print "set parameter ap_p2pbit=$1"
		;;
		CHANNELUSAGE)
			# do nothing
			debug_print "set parameter ap_channelusage=$1"
		;;
		TDLSPROHIBIT)
			# do nothing
			debug_print "set parameter ap_tdls=$1"
		;;
		TDLSCHSWITCHPROHIBIT)
			# do nothing
			debug_print "set parameter ap_tdlschannel=$1"
		;;
		WIDTH)
			debug_print "set parameter ap_width=$1"
			ap_width=$1
			ap_curr_mode=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.htmode`
			ap_curr_prefix=`echo "${ap_curr_mode//[^[:alpha:]]/}"`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.htmode=$ap_curr_prefix$1

		;;
		OFFSET)
			debug_print "set parameter ap_offset=$1"
			lower "$1" ap_offset_input
			if [ "$ap_offset_input" = "below" ]; then
				ap_offset="-"
			elif [ "$ap_offset_input" = "above" ]; then
				ap_offset="+"
			fi
		;;
		COUNTRY) ## NB: Extension parameter
			debug_print "set parameter ap_country=$1"
		;;
		COUNTRYCODE)
			debug_print "set parameter ap_countrycode=$1"
			$UCI_CMD set wireless.radio0.country=$1
			$UCI_CMD set wireless.radio2.country=$1
		;;
		REG_DOMAIN) ## NB: Extension parameter
			# param not supported
			debug_print "set parameter ap_reg_domain=$1"
		;;
		CELLULAR_CAP_PREF)
			debug_print "set parameter ap_cellular_cap_pref=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.mbo_cell_aware=1
		;;
		GAS_CB_DELAY)
			debug_print "set parameter ap_gas_cb_delay=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.gas_comeback_delay=$1
		;;
		DOMAIN)
			debug_print "set parameter ap_domain=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.mobility_domain=$1
		;;
		FT_OA)
			debug_print "set parameter ap_ft_oa=$1"
			lower "$1" ap_enable_input
			if [ "$ap_enable_input" = "enable" ]; then
				$UCI_CMD set $CURRENT_IFACE_UCI_PATH.ieee80211r=1
				$UCI_CMD set $CURRENT_IFACE_UCI_PATH.ft_over_ds=0
				$UCI_CMD set $CURRENT_IFACE_UCI_PATH.nasid=CGR-APUT.intel.com
			else
				$UCI_CMD set $CURRENT_IFACE_UCI_PATH.ieee80211r=0
			fi
		;;
		FT_BSS_LIST)
			debug_print "set parameter ap_ft_bss_list=$1"
			ap_r0kh_r1kh_key=11002200330044005500660077008800
			ap_macaddr=`$UCI_CMD get $CURRENT_IFACE_UCI_PATH.macaddr`
			ap_mac_no_delimiter=`echo "$ap_macaddr" | tr -d :`
			ap_other_mac="$1"
			ap_other_mac_no_delimiter=`echo "$ap_other_mac" | tr -d :`
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.r1_key_holder="$ap_mac_no_delimiter"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.nasid="$ap_mac_no_delimiter"
			$UCI_CMD add_list $CURRENT_IFACE_UCI_PATH.r0kh="$ap_other_mac,$ap_other_mac_no_delimiter,$ap_r0kh_r1kh_key"
			$UCI_CMD add_list $CURRENT_IFACE_UCI_PATH.r1kh="$ap_other_mac,$ap_other_mac,$ap_r0kh_r1kh_key"
			$UCI_CMD add_list $CURRENT_IFACE_UCI_PATH.r0kh="$ap_macaddr,$ap_mac_no_delimiter,$ap_r0kh_r1kh_key"
			$UCI_CMD add_list $CURRENT_IFACE_UCI_PATH.r1kh="$ap_macaddr,$ap_macaddr,$ap_r0kh_r1kh_key"
			#$UCI_CMD set $CURRENT_IFACE_UCI_PATH.iapp_interface="brlan0" TODO: do we need it?
			#$UCI_CMD set $CURRENT_IFACE_UCI_PATH.debug_hostap_conf="ft_psk_generate_local=1" TODO: do we need it?
		;;
		PROGRAM)
			debug_print "set parameter ap_program=$1"
			ap_program=$1
		;;
		PROG)
			debug_print "set parameter ap_prog=$1"
		;;
		OCESUPPORT)
			debug_print "set parameter ap_oce_support=$1"
			lower "$1" ap_oce_support
		;;
		FILSDSCV)
			debug_print "set parameter ap_fils_dscv=$1"
			lower "$1" ap_fils_dscv
			if [ "$ap_fils_dscv" = "enable" ]; then
				ap_tmp=`eval $UPDATE_RNR_UCI unsolicited_frame fils`
			else
				if [ `uci get $ap_radio_6g_uci_path.unsolicited_frame_support` = 2 ]; then
					ap_tmp=`eval $UPDATE_RNR_UCI unsolicited_frame disable`
				fi
			fi
		;;
		UNSOLICITEDPROBERESP)
			debug_print "set parameter ap_unsolicitedproberesp=$1"
			lower "$1" ap_unsolicitedproberesp
			if [ "$ap_unsolicitedproberesp" = "enable" ]; then
				ap_tmp=`eval $UPDATE_RNR_UCI unsolicited_frame probe`
			else
				if [ `uci get $ap_radio_6g_uci_path.unsolicited_frame_support` = 1 ]; then
					ap_tmp=`eval $UPDATE_RNR_UCI unsolicited_frame disable`
				fi
			fi
		;;
		CADENCE_UNSOLICITEDPROBERESP)
			debug_print "set parameter ap_cadence_unsolicitedproberesp=$1"
			lower "$1" ap_cadence_unsolicitedproberesp
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.unsolicited_frame_duration=$ap_cadence_unsolicitedproberesp
		;;
		ACTIVEIND_UNSOLICITEDPROBERESP)
			debug_print "set parameter ap_activeind_unsolicitedproberesp=$1"
			lower "$1" ap_activeind_unsolicitedproberesp
			ap_tmp=`eval $UPDATE_RNR_UCI enable`
		;;
		FILSDSCVINTERVAL)
			debug_print "set parameter ap_fils_dscv_interval=$1"
		;;
		BROADCASTSSID)
			debug_print "set parameter ap_broadcast_ssid=$1"
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.ignore_broadcast_ssid=$1
		;;
		FILSHLP)
			debug_print "set parameter ap_filshlp=$1"
			lower "$1" ap_filshlp
		;;
		NAIREALM)
			debug_print "set parameter ap_nairealm=$1"
		;;
		RNR)
			debug_print "set parameter ap_rnr=$1"
			lower "$1" ap_rnr
			if [ "$ap_rnr" = "enable" ]; then
				ap_tmp=`eval $UPDATE_RNR_UCI enable`
			elif [ "$ap_rnr" = "disable" ]; then
				ap_tmp=`eval $UPDATE_RNR_UCI disable`
			fi
		;;
		DEAUTHDISASSOCTX)
			debug_print "set parameter ap_deauth_disassoc_tx=$1"
		;;
		BLESTACOUNT)
			debug_print "set parameter ap_ble_sta_count=$1"
		;;
		BLECHANNELUTIL)
			debug_print "set parameter ap_ble_channel_util=$1"
		;;
		BLEAVAILADMINCAP)
			debug_print "set parameter ap_ble_avail_admin_cap=$1"
		;;
		AIRTIMEFRACT)
			debug_print "set parameter ap_air_time_fract=$1"
		;;
		DATAPPDUDURATION)
			debug_print "set parameter ap_data_ppdu_duration=$1"
		;;
		DHCPSERVIPADDR)
			debug_print "set parameter ap_dhcp_serv_ip_addr=$1"
		;;
		NSS_MCS_CAP)
			debug_print "set parameter ap_nss_mcs_cap=$1"
			ap_nss_mcs_cap=$1
		;;
		FILSCAP)
			debug_print "set parameter ap_filscap=$1"
			lower "$1" ap_filscap
		;;
		BAWINSIZE)
			debug_print "set parameter ap_oce_ba_win_size=$1"
		;;
		DATAFORMAT)
			debug_print "set parameter ap_oce_data_format=$1"
		;;
		ESP_IE)
			debug_print "set parameter ap_oce_esp_ie=$1"
		;;
		AMPDU)
			debug_print "set parameter ap_ampdu=$1"
			lower "$1" ap_ampdu
		;;
		AMSDU)
			debug_print "set parameter ap_amsdu=$1"
			lower "$1" ap_amsdu
		;;
		MCS_FIXEDRATE)
			debug_print "set parameter global_mcs_fixedrate=$1"
		;;
		SPATIAL_RX_STREAM)
			debug_print "set parameter ap_spatial_rx_stream=$1"
			ap_spatial_rx_stream=$1

		;;
		SPATIAL_TX_STREAM)
			debug_print "set parameter ap_spatial_tx_stream=$1"
			ap_spatial_tx_stream=$1
		;;
		BCC)
			debug_print "set parameter ap_bcc=$1"
			lower $1 ap_bcc
		;;
		LDPC)
			debug_print "set parameter ap_ldpc=$1"
			lower $1 ap_ldpc
		;;
		NOACK)
			# param not supported
			debug_print "set parameter ap_no_ack=$1"
			if [ "$ap_no_ack" != "" ]; then
				error_print "Unsupported value - ap_no_ack:$ap_no_ack"
				send_error ",errorCode,120"
				return 
			fi

		;;
		OFDMA)
			debug_print "set parameter ap_ofdma=$1"
			lower "$1" ap_ofdma
			if [ "$ap_ofdma" = "dl-20and80" ]; then
				ap_ofdma="dl"
			fi

		;;
		PPDUTXTYPE)
			debug_print "set parameter ap_ppdutxtype=$1"
			ap_ppdutxtype=$1
			if [ "$ap_ppdutxtype" = "SU" ]; then
				ap_tmp=`eval $UPDATE_RNR_UCI he_beacon enable`
			fi
		;;
		SPECTRUMMGT)
			# do nothing
			debug_print "set parameter ap_specrummgt=$1"
		;;
		NUMUSERSOFDMA)
			debug_print "set parameter global_ap_num_users_ofdma=$1"
			lower "$1" global_ap_num_users_ofdma
		;;
		TXBF)
			debug_print "set parameter ap_tx_bf=$1"
			lower "$1" ap_tx_bf
		;;
		NUMSOUNDDIM)
			debug_print "set parameter ap_num_sound_dim=$1"
			lower "$1" ap_num_sound_dim
			if [ "$ap_num_sound_dim" != "" ]; then
				debug_print "ap_num_sound_dim:$ap_num_sound_dim"
				if [ "$ap_num_sound_dim" -gt 4 ]; then
					error_print "Unsupported value - ap_num_sound_dim:$ap_num_sound_dim"
					send_error ",errorCode,185"
					return
				fi
			fi
		;;
		VHT_CAP_160_CW)
			# do nothing
			debug_print "set parameter ap_vht_cap_160_bw=$1"
		;;
		VHT_EXTNSS)
			debug_print "set parameter ap_vht_extnss=$1"
			lower "$1" ap_vht_extnss			
		;;
		MU_EDCA)
			debug_print "set parameter ap_mu_edca=$1"
			lower "$1" ap_mu_edca
		;;
		ACKTYPE)
			# do nothing
			debug_print "set parameter ap_acktype=$1"
		;;
		MU_TXBF)
			debug_print "set parameter global_mu_txbf=$1"
			lower "$1" global_mu_txbf
			tc_name=`get_test_case_name $glob_ssid`
			if [ "$ucc_program" = "vht" -a "$tc_name" = "4.2.56" ]; then
				ap_debug_iw_post_up_idx=`uci show wireless | grep $CURRENT_RADIO_UCI_PATH | grep debug_iw_post_up | cut -d"=" -f 1 | cut -d"_" -f5 | sort -n | tail -1`
				if [ -z "$ap_debug_iw_post_up_idx" ]; then
					ap_debug_iw_post_up_idx=1
				else
					let "ap_debug_iw_post_up_idx=ap_debug_iw_post_up_idx+1"
				fi
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_iw_post_up_$ap_debug_iw_post_up_idx="sMuOperation 1"
				/lib/netifd/sigma_mbo_daemon.sh &  #To catch AP-STA-CONNECT event
			fi	
		;;
		TRIG_MAC_PADDING_DUR)
			# do nothing default is 16usec
			debug_print "set parameter ap_trig_mac_padding_dur=$1"
		;;
		BA_PARAM_AMSDU_SUPPORT)
			# same implementation as AMSDU
			debug_print "set parameter ap_ba_param_amsdu_support=$1"
			lower "$1" ap_ba_param_amsdu_support
			ap_amsdu="$ap_ba_param_amsdu_support"
		;;
		ADDBAREQ_BUFSIZE)
			debug_print "set parameter ap_addbareq_bufsize=$1"
			lower "$1" ap_addbareq_bufsize
		;;
		ADDBARESP_BUFSIZE)
			debug_print "set parameter ap_addbaresp_bufsize=$1"
			lower "$1" ap_addbaresp_bufsize
		;;
		ADDBA_REJECT)
			debug_print "set parameter ap_addba_reject=$1"
			ap_addba_reject=$1
		;;
		GREENFIELD)
			debug_print "set parameter ap_greenfield=$1"
			lower "$1" ap_enable_input
			if [ "$ap_enable_input" = "enable" ]; then
				ap_greenfield=1
			else
				ap_greenfield=0
			fi

			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.greenfield=$ap_greenfield
		;;
		BA_RECV_STATUS)
			# do nothing
			debug_print "set parameter ap_ba_recv_status=$1"
		;;
		OMCONTROL)
			debug_print "set parameter ap_omcontrol=$1"
			lower "$1" ap_omcontrol
		;;
		MIMO)
			debug_print "set parameter global_ap_mimo=$1"
			lower "$1" global_ap_mimo
			if [ "$global_ap_mimo" = "ul" ]; then
				ap_debug_iw_post_up_idx=`uci show wireless | grep $CURRENT_RADIO_UCI_PATH | grep debug_iw_post_up | cut -d"=" -f 1 | cut -d"_" -f5 | sort -n | tail -1`
				if [ -z "$ap_debug_iw_post_up_idx" ]; then
					ap_debug_iw_post_up_idx=1
				else
					ap_debug_iw_post_up_idx=$((ap_debug_iw_post_up_idx+1))
				fi
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_iw_post_up_$ap_debug_iw_post_up_idx="sDoSimpleCLI 3 13 2000 0"
			fi
		;;
		TWT_RESPSUPPORT)
			debug_print "set parameter ap_twt_respsupport=$1"
			lower "$1" ap_twt_respsupport
		;;
		SRCTRL_SRVALUE15ALLOWED)
			debug_print "set parameter ap_srctrl_srvalue15allowed=$1"
			lower "$1" ap_srctrl_srvalue15allowed
		;;
		MINMPDUSTARTSPACING)
			debug_print "set parameter ap_min_mpdu_start_spacing=$1"
			ap_min_mpdu_start_spacing=$1
		;;
		MBSSID)
			debug_print "set parameter global_ap_mbssid=$1"
			lower "$1" global_ap_mbssid
			if [ "$global_ap_mbssid" = "enable" -a `eval check_6g_tc` = "1" ]; then
				ap_tmp=`eval $UPDATE_RNR_UCI enable`
			fi
		;;
		NUMNONTXBSS)
			([ $1 -lt 1 ] || [ $1 -gt 7 ]) && error_print "NumNonTxBSS invalid value '$1'" && send_invalid ",errorCode,95" && return
			global_num_non_tx_bss=$1
			debug_print "global_num_non_tx_bss:$global_num_non_tx_bss"
		;;
		NONTXBSSINDEX)
			debug_print "set parameter ap_non_tx_bss_index=$1"
			([ $1 -lt 1 ] || [ $1 -gt 8 ]) && error_print "NonTxBSSIndex invalid value '$1'" && send_invalid ",errorCode,96" && return
			ap_non_tx_bss_index=$1
		
		;;
		HE_TXOPDURRTSTHR)
		lower "$1" ap_he_txop_dur_rts_thr_conf
			if [ "$ap_he_txop_dur_rts_thr_conf" = "enable" ]; then
				ap_he_txop_dur_rts_thr_conf=10
			elif [ "$ap_he_txop_dur_rts_thr_conf" = "disable" ]; then
				ap_he_txop_dur_rts_thr_conf=1023
			else
				ap_he_txop_dur_rts_thr_conf=""
			fi
			
			# We support here "Enable" & "Disable" only (tests # 4.66.1, 5.71.1)
			# 1. "Enable" - any value between 0..1022
			# 2. "Disable" - 1023

		;;
		MCS_32|MCS32)
			# do nothing
			debug_print "set parameter ap_mcs_32=$1"
		;;
		CHNLFREQ)
			debug_print "set parameter ap_channel_freq=$1"
			lower "$1" ap_channel_freq
		;;
		BAND6GONLY)
			debug_print "set parameter ap_band6gonly=$1"
			lower "$1" ap_band6gonly
			iw wlan0 iwlwav sEnableRadio 0
			iw wlan2 iwlwav sEnableRadio 0
		;;
		EXPBCNLENGTH)
			debug_print "ignore EXPBCNLENGTH"
			;;
		RSNXE)
		;;
		BSSMEMSELECT)
		;;
		ERSUDISABLE)
			debug_print "set parameter ap_ERSUdisable=$1"
			lower "$1" ap_ersudisable
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.he_operation_er_su_disable=$ap_ersudisable
		;;
		TWTINFOFRAMERX)
			# do nothing
			debug_print "set parameter ap_twtinfoframerx=$1"
		;;
		FULLBW_ULMUMIMO)
			debug_print "set parameter ap_fullbw_ulmumimo=$1"
			local ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_full_bandwidth_ul_mu_mimo' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			local val=0
			if [ -z "$ap_get_debug_hostap_conf_or_Post_Up_idx" ]; then
				ap_get_debug_hostap_conf_or_Post_Up_idx=`uci show wireless | grep debug_hostap | grep $CURRENT_RADIO_UCI_PATH | cut -d"_" -f4 | cut -d"=" -f1 | sort -n | tail -1`
				ap_get_debug_hostap_conf_or_Post_Up_idx=$((ap_get_debug_hostap_conf_or_Post_Up_idx+1))
			fi
			if [ "$1" = "enable" ]; then
				val=1
			elif [ "$1" = "disable" ]; then
				val=0
			fi
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_full_bandwidth_ul_mu_mimo=$val"
		;;
		OMCTRL_ULMUDATADISABLERX)
			debug_print "set parameter ap_omctrl_ulmudatadisablerx=$1"
			local ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mac_om_control_ul_mu_data_disable_rx_support' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			local val=0
			if [ -z "$ap_get_debug_hostap_conf_or_Post_Up_idx" ]; then
				ap_get_debug_hostap_conf_or_Post_Up_idx=`uci show wireless | grep debug_hostap | grep $CURRENT_RADIO_UCI_PATH | cut -d"_" -f4 | cut -d"=" -f1 | sort -n | tail -1`
				ap_get_debug_hostap_conf_or_Post_Up_idx=$((ap_get_debug_hostap_conf_or_Post_Up_idx+1))
			fi
			if [ "$1" = "enable" ]; then
				val=1
			elif [ "$1" = "disable" ]; then
				val=0
			fi
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mac_om_control_ul_mu_data_disable_rx_support=$val"
		;;
		HE_SMPS)
			debug_print "set parameter ap_he_smps=$1" #TODO: currently no processing of this param
			lower "$1" ap_he_smps
			if [ "$ap_he_smps" = "enable" ]; then
				debug_print "enable:HE_SMPS"
			elif [ "$ap_he_smps" = "disable" ]; then
				debug_print "disable:HE_SMPS"
			fi
		;;
		BROADCASTTWT)
			debug_print "set parameter global_ap_btwt=$1 ap_twt_respsupport=$1"
			if [ "$1" = "enable" ]; then
				global_ap_btwt=1
			elif [ "$1" = "disable" ]; then
				global_ap_btwt=0
			fi
			ap_twt_respsupport=$1
		;;
		PREAMBLEPUNCTTX)
			debug_print "set parameter global_ap_preamble_puncture=$1"
			lower "$1" global_ap_preamble_puncture
		;;
		*)
			error_print "while loop error $1"
			send_invalid ",errorCode,2"
			return
		;;
		esac
		shift
	done

	if [ "$ap_program" = "HE" ]; then
		if [ "$ap_width" != "" ]; then
			if [ "$ap_width" = "20" ]; then
				width_val="20MHz"
			elif [ "$ap_width" = "40" ]; then
				width_val="40MHz"
			elif [ "$ap_width" = "80" ]; then
				width_val="80MHz"
			elif [ "$ap_width" = "160" ]; then
				width_val="160MHz"
			else
				debug_print "ap_width:$ap_width"
				width_val="Auto"
			fi
		fi

		if [ "$width_val" != "" ]; then
			convert_fixed_rate ${width_val}
			set_get_helper_non_debug $CURRENT_RADIO_UCI_PATH.sFixedRateCfg x2=$bw_converted

			# Only in 160 MHz / 5GHz, change channel check time to 1 sec to save configuration time.
			# and disable radar detection to ensure no radar is detected during test and BW stays at 160MHz.
			tc_name=`get_test_case_name $glob_ssid`
			if [ "$width_val" = "160MHz" ] || [ "$tc_name" = "4.60.1" ]; then
				ap_band=`eval $UCI_CMD show $CURRENT_RADIO_UCI_PATH.band | cut -d"=" -f2 | tr -d "'"`
				ap_channel=`eval $UCI_CMD show $CURRENT_RADIO_UCI_PATH.channel | cut -d"=" -f2 | tr -d "'"`
				if [ "$ap_channel" -ge "36" ] && [ "$ap_band" = "5GHz" ]; then
					$UCI_CMD set ${CURRENT_RADIO_UCI_PATH}.doth=1
					iw dev $CURRENT_WLAN_NAME iwlwav s11hChCheckTime 1
					iw dev $CURRENT_WLAN_NAME iwlwav s11hRadarDetect 0
				fi
			fi
			
			# if static plan is on, turn it off, since we are going to change band (SMD will turn it back on).
			info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
			send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH

			[ "$width_val" = "20MHz" ] && ap_txop_com_start_bw_limit=0
			[ "$width_val" = "40MHz" ] && ap_txop_com_start_bw_limit=1
			[ "$width_val" = "80MHz" ] && ap_txop_com_start_bw_limit=2
			[ "$width_val" = "160MHz" ] && ap_txop_com_start_bw_limit=3
			
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x${txop_com_start_bw_limit_idx}=${ap_txop_com_start_bw_limit}
			
		fi
		
		## WA WLANRTSYS-13765 for TC 4.31.1 and 4.52.1_5G - set fixed rate with 2NSS.
		tc_name=`get_test_case_name $glob_ssid`
		if [ "$tc_name" = "4.31.1" ] ; then
			convert_fixed_rate ${ap_txbandwidth} "ax"
			set_get_helper_non_debug $CURRENT_RADIO_UCI_PATH.sFixedRateCfg x1=0 x2=${bw_converted} x3=4 x4=2 x5=11 x6=2
		elif [ "$tc_name" = "4.52.1" ] && [ $ap_channel -ge "36" ]; then
			onvert_fixed_rate ${ap_txbandwidth} "ax"
			set_get_helper_non_debug $CURRENT_RADIO_UCI_PATH.sFixedRateCfg x1=0 x2=${bw_converted} x3=4 x4=2 x5=11 x6=2
		fi



		if [ "$ap_ofdma" != "" ]; then
			# JIRA WLANRTSYS-9736: in case of the below tests (4.68.1 & 5.73.1), setting of OFDMA to DL, even if the test was setting it as UL
			tc_name=`get_test_case_name $glob_ssid`
			if [ "$ap_ofdma" != "dl" ] && ([ "$tc_name" = "4.68.1" ] || [ "$tc_name" = "5.73.1" ]); then
				info_print "tc_name = $tc_name (ap_ofdma = $ap_ofdma) ==> overwrite ap_ofdma, set it to 'dl'"
				ap_ofdma="dl"
			fi

			if [ "$ap_ofdma" = "dl" ] || [ "$ap_ofdma" = "ul" ]; then
				get_nof_sta_per_he_test_case $glob_ssid
				ap_num_users_ofdma=$nof_sta
				arguments_file_initializer $CURRENT_RADIO_UCI_PATH $glob_ssid
				kill_sigmaManagerDaemon
				/lib/netifd/sigmaManagerDaemon.sh $CURRENT_WLAN_NAME.0 $ap_num_users_ofdma &

				if [ "$ap_ofdma" = "dl" ]; then
					glob_ofdma_phase_format=0
				elif [ "$ap_ofdma" = "ul" ]; then
					glob_ofdma_phase_format=1 		
					ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x2=5 x7=1
				fi

				ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
				set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x4=${glob_ofdma_phase_format}
				if [ "$ap_num_users_ofdma" != "" ]; then
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x5=${ap_num_users_ofdma}
				else
					if [ "$glob_ssid" != "" ]; then
						# check if num_of_users can be obtained from the predefined list (by test plan)
						get_nof_sta_per_he_test_case $glob_ssid
						ap_num_users_ofdma=$nof_sta
						[ "$ap_num_users_ofdma" != "0" ] && set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x5=${ap_num_users_ofdma}
					fi
				fi
			else
				error_print "Unsupported value - ap_ofdma:$ap_ofdma"
				send_error ",errorCode,150"
				return
			fi

			# set fixed rate in OFDMA MU only
			info_print "sDoSimpleCLI 70 1"
			uci set $CURRENT_RADIO_UCI_PATH.debug_iw_post_up_2="sDoSimpleCLI 70 1"
		fi

		# Set coding (LDPC/BCC) for DL
		if [ "$ap_ldpc" != "" ] || [ "$ap_bcc" != "" ]; then
			debug_print "ap_ldpc:$ap_ldpc ap_bcc:$ap_bcc"
			if [ "$ap_ldpc" = "enable" ] && [ "$ap_bcc" = "enable" ]; then
				error_print "Unsupported value - ap_ldpc:$ap_ldpc ap_bcc:$ap_bcc"
				send_error ",errorCode,125"
				return
			elif [ "$ap_ldpc" = "disable" ] && [ "$ap_bcc" = "disable" ]; then
				error_print "Unsupported value - ap_ldpc:$ap_ldpc ap_bcc:$ap_bcc"
				send_error ",errorCode,130"
				return
			fi

			if [ "$ap_ldpc" = "disable" ]; then ap_bcc="enable"; fi
			if [ "$ap_bcc" = "disable" ]; then ap_ldpc="enable"; fi

			if [ "$ap_ldpc" = "enable" ]; then
				# set for SU, only if not OFDMA MU TC
				if [ "$glob_ofdma_phase_format" = "" ]; then
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.ldpc=1
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.rxldpc=1
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_ldpc_coding_in_payload' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_ldpc_coding_in_payload=1"
				fi
				
				# set for MU
				for usr_index in 1 2 3 4
				do
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlannUser$ap_usr_index"  x18=1
				done
			elif [ "$ap_bcc" = "enable" ]; then
				# set for SU, only if not OFDMA MU TC
				if [ "$glob_ofdma_phase_format" = "" ]; then 
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.ldpc=0
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.rxldpc=0
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_ldpc_coding_in_payload' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_ldpc_coding_in_payload=0"
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'enable_he_debug_mode' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="enable_he_debug_mode=1"
				fi			
				# set for MU
				for usr_index in 1 2 3 4
				do
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlannUser$ap_usr_index" x18=0 
				done
			else
				error_print "Unsupported value - ap_ldpc:$ap_ldpc ap_bcc:$ap_bcc"
				send_error ",errorCode,135"
				return
			fi
		fi
		if [ "$global_mcs_fixedrate" != "" ]; then
			debug_print "global_mcs_fixedrate:$global_mcs_fixedrate"

			# set for SU, only if not OFDMA MU TC
			if [ "$glob_ofdma_phase_format" = "" ]; then #related to ap_ofdma
				set_get_helper_non_debug $CURRENT_RADIO_UCI_PATH.sFixedRateCfg x1=0 x5=4 x7=${global_mcs_fixedrate} x8=5	
			fi			
				
			# set for MU
			for ap_usr_index in 1 2 3 4
			do
				# get MU DL NSS value from FAPI
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				sp_nss_mcs=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x1`
				let ap_nss="$sp_nss_mcs/16+1"

				# calculate the OFDMA MU NSS-MCS value (NSS: bits 5-4, MCS: bits 3-0)
				let ap_ofdma_mu_nss_mcs_val="($ap_nss-1)*16+$global_mcs_fixedrate"

				# set MU DL NSS MCS value
				set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlannUser$ap_usr_index" x1=${ap_ofdma_mu_nss_mcs_val}

				# get MU UL NSS value
				sp_nss_mcs=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x3`
				let ap_nss="$sp_nss_mcs%16"

				# calculate the OFDMA MU NSS-MCS value (NSS: bits 5-4, MCS: bits 3-0)
				let ap_ofdma_mu_nss_mcs_val="($ap_nss-1)*16+$global_mcs_fixedrate"

				# set MU UL NSS MCS value
				set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlannUser$ap_usr_index" x3=${ap_ofdma_mu_nss_mcs_val}
			done
		fi

		if [ "$ap_nss_mcs_cap" != "" ]; then
			debug_print "ap_nss_mcs_cap:$ap_nss_mcs_cap"
			ap_nss_cap=${ap_nss_mcs_cap%%;*}
			ap_mcs_cap=${ap_nss_mcs_cap##*;}

			ap_mcs_min_cap=${ap_mcs_cap%%-*}
			ap_mcs_max_cap=${ap_mcs_cap##*-}

			ap_rx_mcs_max_cap=$ap_mcs_max_cap
			ap_tx_mcs_max_cap=$ap_mcs_max_cap

			# calculate the OFDMA MU NSS-MCS value (NSS: bits 5-4, MCS: bits 3-0)
			let ap_ofdma_mu_nss_mcs_val="($ap_nss_cap-1)*16+$ap_mcs_max_cap"

			# set for MU
			for ap_usr_index in 1 2 3 4
			do
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				if [ "$global_ap_mimo" != "ul" ]; then	
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlannUser$ap_usr_index" x$dl_usr_psdu_rate_per_usp_idx=${ap_ofdma_mu_nss_mcs_val} x$dl_usr_ul_psdu_rate_per_usp_idx=${ap_ofdma_mu_nss_mcs_val}
				fi
			done

			# set the nss mcs capabilities
			ap_nss_mcs_val=`get_nss_mcs_val $ap_nss_cap $ap_mcs_max_cap`
			if [ "$ap_nss_mcs_val" = "" ]; then
				error_print "Unsupported value - ap_nss_cap:$ap_nss_cap ap_mcs_max_cap:$ap_mcs_max_cap"
				send_error ",errorCode,137"
				return
			fi
			
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz=${ap_nss_mcs_val}"
			## JIRA WLANRTSYS-11028: part0-Rx part1-Tx
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'vht_mcs_set_part0' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="vht_mcs_set_part0=${ap_nss_mcs_val}"
			global_nss_opt_ul=${ap_nss_cap}

			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz=${ap_nss_mcs_val}"
			# JIRA WLANRTSYS-11028: part0-Rx part1-Tx
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'vht_mcs_set_part1' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="vht_mcs_set_part1=${ap_nss_mcs_val}"
			global_nss_opt_dl=${ap_nss_cap}

			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_rx_he_mcs_map_160_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_rx_he_mcs_map_160_mhz=${ap_nss_mcs_val}"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_tx_he_mcs_map_160_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_tx_he_mcs_map_160_mhz=${ap_nss_mcs_val}"
		fi

		if [ "$ap_spatial_rx_stream" != "" ]; then
			debug_print "ap_spatial_rx_stream:$ap_spatial_rx_stream"
			debug_print "ap_program:$ap_program"

			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			mu_type=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x6`

			if [ "$global_ap_mimo" != "" ] || [ "$mu_type" = "1" ]; then
				[ "$ap_spatial_rx_stream" = "2SS" ] && ap_spatial_rx_stream="1SS"
				[ "$ap_spatial_rx_stream" = "4SS" ] && ap_spatial_rx_stream="2SS"
			fi

			spatial_rx_stream_number=${ap_spatial_rx_stream%%S*}

			# check that the NSS # is 1, 2, 3 or 4
			case $spatial_rx_stream_number in
			1|2|3|4) ;;
			*)
				error_print "Unsupported value - ap_spatial_rx_stream:$ap_spatial_rx_stream"
				send_error ",errorCode,140"
				return
			;;
			esac

			if [ -z "$ucc_type" ] || [ "$ucc_type" = "testbed" ]; then
				if [ -n "$ap_rx_mcs_max_cap" ]; then
					# JIRA WLANRTSYS-9372: When SPATIAL_RX_STREAM or SPATIAL_TX_STREAM are set, use ap_mcs_max_cap; otherwise, use the default value
					ap_local_mcs=$ap_rx_mcs_max_cap
					ap_rx_mcs_max_cap=""
				else
					ap_local_mcs=$mcs_def_val_ul_testbed
				fi
			elif [ "$ucc_type" = "dut" ]; then
				ap_local_mcs=$mcs_def_val_ul
			fi

			ap_spatial_rx_stream_val=`get_nss_mcs_val $spatial_rx_stream_number $ap_local_mcs`
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz=${ap_spatial_rx_stream_val}"
			# JIRA WLANRTSYS-11028: part0-Rx part1-Tx
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'vht_mcs_set_part0' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="vht_mcs_set_part0=${ap_spatial_rx_stream_val}"
			global_nss_opt_ul=${spatial_rx_stream_number}
		fi

		if [ "$ap_spatial_tx_stream" != "" ]; then
			debug_print "ap_spatial_tx_stream:$ap_spatial_tx_stream"
			debug_print "ap_program:$ap_program"

			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			mu_type=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x6`

			if [ "$global_ap_mimo" != "" ] || [ "$mu_type" = "1" ]; then
				[ "$ap_spatial_tx_stream" = "2SS" ] && ap_spatial_tx_stream="1SS"
				[ "$ap_spatial_tx_stream" = "4SS" ] && ap_spatial_tx_stream="2SS"
			fi

			spatial_tx_stream_number=${ap_spatial_tx_stream%%S*}

			# check that the NSS # is 1, 2, 3 or 4
			case $spatial_tx_stream_number in
			1|2|3|4) ;;
			*)
				error_print "Unsupported value - ap_spatial_tx_stream:$ap_spatial_tx_stream"
				send_error ",errorCode,145"
				return
			;;
			esac

			if [ -z "$ucc_type" ] || [ "$ucc_type" = "testbed" ]; then
				if [ -n "$ap_tx_mcs_max_cap" ]; then
					# JIRA WLANRTSYS-9372: When SPATIAL_RX_STREAM or SPATIAL_TX_STREAM are set, use ap_mcs_max_cap; otherwise, use the default value
					ap_local_mcs=$ap_tx_mcs_max_cap
					ap_tx_mcs_max_cap=""
				else
					ap_local_mcs=$mcs_def_val_dl_testbed
				fi
			elif [ "$ucc_type" = "dut" ]; then
				ap_local_mcs=$mcs_def_val_dl
			fi

			ap_spatial_tx_stream_val=`get_nss_mcs_val $spatial_tx_stream_number $ap_local_mcs`
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz=${ap_spatial_tx_stream_val}"
			# JIRA WLANRTSYS-11028: part0-Rx part1-Tx
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'vht_mcs_set_part1' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="vht_mcs_set_part1=${ap_spatial_tx_stream_val}"
			global_nss_opt_dl=${spatial_tx_stream_number}

			# set for MU
			for usr_index in 1 2 3 4
			do
				# get MU DL MCS value
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'${usr_index} $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				current_ap_mcs=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x1`
				let current_ap_mcs="$current_ap_mcs%16"
				
				# calculate the OFDMA MU NSS-MCS value (NSS: bits 5-4, MCS: bits 3-0)
				let ap_ofdma_mu_nss_mcs_val="($spatial_tx_stream_number-1)*16+$current_ap_mcs"
		
				# set MU DL NSS MCS value
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x1=${ap_ofdma_mu_nss_mcs_val}
				# TBD: do we need to set the UL value here?
#				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x3=${ap_ofdma_mu_nss_mcs_val}
		
			done

		fi

		if [ "$ap_ampdu" != "" ]; then
			if [ "$ap_ampdu" = "enable" ]; then
				ap_ampdu_val="1"
				ap_ampdu_exp_val="2"
			elif [ "$ap_ampdu" = "disable" ]; then
				ap_ampdu_val="0"
				ap_ampdu_exp_val="0"
			else
				error_print "Unsupported value - ap_ampdu"
				send_error ",errorCode,165"
				return
			fi
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mac_a_msdu_in_ack_enabled_a_mpdu_support' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mac_a_msdu_in_ack_enabled_a_mpdu_support=$ap_ampdu_val"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mac_maximum_a_mpdu_length_exponent' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mac_maximum_a_mpdu_length_exponent=$ap_ampdu_exp_val"
			# JIRA WLANRTSYS-9583 [Silicon limitation A1] ap_set_wireless AMPDU,Disable -> Disable BA Agreements in FW using sAggrConfig  
			set_get_helper_non_debug $CURRENT_IFACE_UCI_PATH.sAggrConfig x1=$ap_ampdu_val 	
		fi 
				
		if [ "$ap_amsdu" != "" ]; then
			if [ "$ap_amsdu" = "enable" ]; then
				ap_amsdu_val="1"
			elif [ "$ap_amsdu" = "disable" ]; then
				ap_amsdu_val="0"
			else
				error_print "Unsupported value - ap_amsdu:$ap_amsdu"
				send_error ",errorCode,170"
				return
			fi

			set_get_helper $CURRENT_IFACE_UCI_PATH.sAggrConfig x0=$ap_amsdu_val
			## WLANRTSYS-11027 [Silicon limitation A1] upon AMSDU enable Set sMaxMpduLen to 11000 
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMaxMpduLen' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`	
			[ "$ap_amsdu_val" = "1" ] && $UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMaxMpduLen=11000"
		fi

		if [ "$ap_addba_reject" != "" ]; then
			debug_print "ap_addba_reject:$ap_addba_reject"
			if [ "$ap_addba_reject" = "enable" ]; then
				ap_addba_reject_val="0" # reject=enable means 0 (disable BA agreement) 
			elif [ "$ap_addba_reject" = "disable" ]; then
				ap_addba_reject_val="1" # reject=disable means 1 (enable BA agreement)
			else
				error_print "Unsupported value - ap_addba_reject:$ap_addba_reject"
				send_error ",errorCode,175"
				return
			fi
			set_get_helper_non_debug $CURRENT_IFACE_UCI_PATH.sAggrConfig x2=${ap_addba_reject_val}
		fi

		if [ "$ap_addbareq_bufsize" != "" ]; then
			if [ "$ap_addbareq_bufsize" = "gt64" ]; then
				ap_addbareq_bufsize="256"
			elif [ "$ap_addbareq_bufsize" = "le64" ]; then
				ap_addbareq_bufsize="64"
			else
				error_print "Unsupported value - ap_addba_reject:$ap_addba_reject"
				send_error ",errorCode,176"
				return
			fi

			[ "$ap_addbareq_bufsize" != "0" ] && set_get_helper_non_debug $CURRENT_IFACE_UCI_PATH.sAggrConfig x2=${ap_addbareq_bufsize}
		fi

		## JIRA WLANRTSYS-10849
		if [ "$ap_addbaresp_bufsize" != "" ]; then
			if [ "$ap_addbaresp_bufsize" = "gt64" ]; then
				ap_addbaresp_bufsize="256"
			elif [ "$ap_addbaresp_bufsize" = "le64" ]; then
				ap_addbaresp_bufsize="64"
			else
				error_print "Unsupported value - ap_addbaresp_bufsize:$ap_addbaresp_bufsize"
				send_error ",errorCode,178"
				return
			fi

			[ "$ap_addbaresp_bufsize" != "0" ] && set_get_helper_non_debug $CURRENT_IFACE_UCI_PATH.sAggrConfig x2=${ap_addbaresp_bufsize} 
		fi

		if [ "$ap_tx_bf" != "" ]; then
			debug_print "ap_tx_bf:$ap_tx_bf"
			if [ "$ap_tx_bf" = "enable" ]; then
				ap_tx_bf_val="EXPLICIT"
				debug_print "ap_program=$ap_program"
				tc_name=`get_test_case_name $glob_ssid`
				debug_print "tc_name=$tc_name"
				debug_print "width_val=$width_val"
				if [ "$tc_name" = "4.35.1" ] && [ "$width_val" = "80MHz" ]; then
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_hostap_conf="he_phy_channel_width_set=2"
				fi
				# WLANRTSYS-10947
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_hostap_conf="he_phy_su_beamformer_capable=1"
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_hostap_conf="he_phy_mu_beamformer_capable=1"
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_hostap_conf="he_phy_su_beamformee_capable=1"
				# set the maximum but will not be set more the hw antennas.
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_max_nc' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_max_nc=4"
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.debug_hostap_conf="he_phy_triggered_su_beamforming_feedback=1"
			elif [ "$ap_tx_bf" = "disable" ]; then
				ap_tx_bf_val="Disabled"
			else
				error_print "Unsupported value - ap_tx_bf:$ap_tx_bf"
				send_error ",errorCode,180"
				return
			fi
			sBfMode_local=`convert_bf_mode $ap_tx_bf_val`			
			debug_print "converted sBfMode_local=$sBfMode_local"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sBfMode' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx=\"sBfMode $sBfMode_local\"
		fi

		if [ "$ap_num_sound_dim" != "" ]; then
			debug_print "ap_num_sound_dim:$ap_num_sound_dim"
			if [ "ap_num_sound_dim" -gt 4 ]; then
				error_print "Unsupported value - ap_num_sound_dim:$ap_num_sound_dim"
				send_error ",errorCode,185"
				return
			fi
		fi

		if [ "$ap_omcontrol" != "" ]; then
			debug_print "ap_omcontrol:$ap_omcontrol"
			if [ "$ap_omcontrol" = "enable" ]; then
				ap_omcontrol="true"
			elif [ "$ap_omcontrol" = "disable" ]; then
				ap_omcontrol="false"
			else
				error_print "Unsupported value - ap_omcontrol:$ap_omcontrol"
				send_error ",errorCode,195"
				return
			fi
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mac_om_control_support' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mac_om_control_support=${ap_omcontrol}"
		fi

		if [ "$global_ap_mimo" != "" ]; then
			if [ "$global_ap_mimo" = "dl" ] || [ "$global_ap_mimo" = "ul" ]; then
				get_nof_sta_per_he_test_case $glob_ssid
				ap_num_users_ofdma=$nof_sta
				arguments_file_initializer $CURRENT_RADIO_UCI_PATH $glob_ssid
				kill_sigmaManagerDaemon
				/lib/netifd/sigmaManagerDaemon.sh $CURRENT_WLAN_NAME.0 $ap_num_users_ofdma &

				if [ "$global_ap_mimo" = "dl" ]; then
					glob_ofdma_phase_format=0
					## Common PART
					ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x2=1 x6=1 x15=406
					# WLANRTSYS-9638: 'WaveSPDlUsrPsduRatePerUsp' will NOT be set at all
				elif [ "$global_ap_mimo" = "ul" ]; then
					glob_ofdma_phase_format=1
					## Common PART
					## Phase repetitions to be set as we do for UL OFDMA in order to efficiently utilize the TXOP
					## Default CP=1 (1.6us), LTF=0 (1x), GI=0 (1xLTF + 1.6 GI)
					ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x2=5 x6=1 x7=1 x12=1 x13=0 x14=0
				fi
				ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
				set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x4=${glob_ofdma_phase_format}
				## ap_num_users_ofdma in case UCC will send num of users if not user will be set according to the test
				if [ "$ap_num_users_ofdma" != "" ]; then
					set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x5=${ap_num_users_ofdma}
				else
					if [ "$glob_ssid" != "" ]; then 
						# check if num_of_users can be obtained from the predefined list (by test plan)
						get_nof_sta_per_he_test_case $glob_ssid
						ap_num_users_ofdma=$nof_sta
						[ "$ap_num_users_ofdma" != "0" ] && set_get_helper $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x5=${ap_num_users_ofdma}
					fi
				fi
			else
				error_print "Unsupported value - global_ap_mimo:$global_ap_mimo"
				send_error ",errorCode,197"
				return
			fi
			# set fixed rate in OFDMA MU only
			info_print "sDoSimpleCLI 70 1"
			uci set $CURRENT_RADIO_UCI_PATH.debug_iw_post_up_2="sDoSimpleCLI 70 1"
		fi

		if [ "$global_mu_txbf" != "" ] && [ "$global_mu_txbf" = "enable" ]; then
			# WLANRTSYS-10947
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_su_beamformer_capable' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_su_beamformer_capable=1"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_mu_beamformer_capable' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_mu_beamformer_capable=1"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_su_beamformee_capable' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_su_beamformee_capable=1"
			# set the maximum but will not be set more the hw antennas.
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_max_nc' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_max_nc=4"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_phy_triggered_su_beamforming_feedback' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_phy_triggered_su_beamforming_feedback=1"
			
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			set_get_helper  $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x$dl_com_mu_type_idx=1
			if [ "$ucc_type" = "testbed" ]; then
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'enable_he_debug_mode' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="enable_he_debug_mode=0"
			fi
			
			get_nof_sta_per_he_test_case $glob_ssid
			ap_num_users_ofdma=$nof_sta
			arguments_file_initializer $CURRENT_RADIO_UCI_PATH $glob_ssid
			kill_sigmaManagerDaemon
			[ "$ap_program" = "HE" ] && /lib/netifd/sigmaManagerDaemon.sh $CURRENT_WLAN_NAME.0 $ap_num_users_ofdma &
		fi

		if [ "$ap_mu_edca" != "" ]; then
			## JIRA WLANRTSYS-10947: WaveHeMuEdcaIePresent
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mu_edca_ie_present' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			[ "$ap_mu_edca" = "override" ] && $UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mu_edca_ie_present=1"
		fi

		if [ "$ap_twt_respsupport" != "" ]; then
			debug_print "ap_twt_respsupport:$ap_twt_respsupport"
			if [ "$ap_twt_respsupport" = "enable" ]; then
				ap_twt_respsupport="1"
				info_print "dl_com_number_of_phase_repetitions = 2"
				ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
				set_get_helper  $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sMuStaticPlann_common" x$dl_com_number_of_phase_repetitions_idx=2
			elif [ "$ap_twt_respsupport" = "disable" ]; then
				ap_twt_respsupport="0"
			else
				error_print "Unsupported value - ap_twt_respsupport:$ap_twt_respsupport"
				send_error ",errorCode,200"
				return
			fi
			
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep  -w 'twt_responder_support' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="twt_responder_support=${ap_twt_respsupport}"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mac_twt_responder_support' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mac_twt_responder_support=${ap_twt_respsupport}"
		fi

		if [ -n "$ap_min_mpdu_start_spacing" ]; then
			debug_print "ap_min_mpdu_start_spacing:$ap_min_mpdu_start_spacing"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'ht_minimum_mpdu_start_spacing' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="ht_minimum_mpdu_start_spacing=${ap_min_mpdu_start_spacing}"
		fi
		
		# JIRA WLANRTSYS-9943
		if [ "$ap_vht_extnss" != "" ]; then
			if [ "$ap_vht_extnss" = "eq0" ]; then
				ap_width="40"  # "40MHz", will set he_op_vht_channel_width to '0'
			elif [ "$ap_vht_extnss" = "eq1" ]; then
				ap_width="80"  # "80MHz", will set he_op_vht_channel_width to '1'
			else
				error_print "Unsupported value - ap_vht_extnss:$ap_vht_extnss"
				send_error ",errorCode,116"
				return
			fi
		fi
		# Handle MBSSID feature - start
		if [ -n "$global_ap_mbssid" ]; then
			debug_print "global_ap_mbssid:$global_ap_mbssid"
			# In order to avoid issues with new VAPs MACs, setting MAC for first VAP in the format XX:XX:XX:XX:XX:X0
			local macaddr_suffix=${CURRENT_WLAN_NAME##wlan}
			macaddr_suffix=$((macaddr_suffix+1))
			macaddr_suffix=$((macaddr_suffix*10))
			base_addr=`uci show wireless | grep $CURRENT_RADIO_UCI_PATH.macaddr | cut -d"=" -f2 | cut -d":" -f1-5 | cut -d"'" -f2`
			$UCI_CMD set $CURRENT_IFACE_UCI_PATH.macaddr=$base_addr:${macaddr_suffix}
			if [ "$global_ap_mbssid" = "enable" ]; then
				if [ -n "$global_num_non_tx_bss" ]; then
					if [ $global_num_non_tx_bss -le 3 ]; then
						ap_mbssid_num_non_tx_bss=3  # numNonTxBss <= 3 [1,2,3] creating additional 3 VAPs (in addition to the main AP).
					else
						ap_mbssid_num_non_tx_bss=7  # numNonTxBss > 3 [4,5,6,7] creating additional 7 VAPs (in addition to the main AP).
					fi
				fi
				if [ -z "$global_is_vaps_created" ] && [ -n "$ap_non_tx_bss_index" ] && [ -n "$ap_mbssid_num_non_tx_bss" ]; then
					debug_print "ap_non_tx_bss_index:$ap_non_tx_bss_index"
					ap_non_tx_bss_index_count=$global_num_non_tx_bss
					local count=0
					while [ $count -lt $ap_non_tx_bss_index_count ]; do
						count=$((count+1))
						if [ $count -eq $ap_non_tx_bss_index ]; then
							[ -n "$ap_ssid_non_tx_bss_index" ] && eval mbss_vap_$count=`add_interface`
							tmp_current_o_vap="mbss_vap_$count"
							current_o_vap=`eval 'echo $'$tmp_current_o_vap`
							$UCI_CMD set wireless.$current_o_vap.ssid="$ap_ssid_non_tx_bss_index"
							if [ `uci get $CURRENT_IFACE_UCI_PATH.ieee80211w` = 1 ]; then
								$UCI_CMD set wireless.$current_o_vap.ieee80211w="1"
							fi
						else
							eval mbss_vap_$count=`add_interface`
							tmp_current_o_vap="mbss_vap_$count"
							current_o_vap=`eval 'echo $'$tmp_current_o_vap`
							$UCI_CMD set wireless.$current_o_vap.ssid="MBSSID_VAP_${count}"
							$UCI_CMD set wireless.$current_o_vap.mbo="0"
							if [ `uci get $CURRENT_IFACE_UCI_PATH.ieee80211w` = 1 ]; then
								$UCI_CMD set wireless.$current_o_vap.ieee80211w="1"
							fi
						fi	

					done
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'multibss_enable' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="multibss_enable=1"
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_operation_cohosted_bss' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_operation_cohosted_bss=0"

					# JIRA WLANRTSYS-9713: rais a flag; otherwise, when getting 'NONTXBSSINDEX' again, a new set of vaps will be created
					global_is_vaps_created=1
				fi
				if [ "$global_is_vaps_created" = "1" ] && [ -n "$ap_ssid_non_tx_bss_index" ] && [ -n "$ap_non_tx_bss_index" ] && [ "$ap_non_tx_bss_index" -gt "1" ]; then
					# handle indexes '2' and above; index '1' SSID was already set
					local ap_vap_idx=$ap_non_tx_bss_index
					tmp_current_o_vap="mbss_vap_$ap_vap_idx"
					current_o_vap=`eval 'echo $'$tmp_current_o_vap`
					$UCI_CMD set wireless.$current_o_vap.ssid="$ap_ssid_non_tx_bss_index"
				fi
				[ "$ap_non_tx_bss_index" = "$global_num_non_tx_bss" ] && global_ap_mbssid=""

			else
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'multibss_enable' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="multibss_enable=0"
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_operation_cohosted_bss' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_operation_cohosted_bss=1"
			fi
		fi
		# Handle MBSSID feature - end
		if [ "$ap_srctrl_srvalue15allowed" != "" ]; then
			debug_print "ap_srctrl_srvalue15allowed:$ap_srctrl_srvalue15allowed"

			if [ "$ap_srctrl_srvalue15allowed" = "enable" ] || [ "$ap_srctrl_srvalue15allowed" = "1" ]; then
				ap_srctrl_srvalue15allowed="true"
			elif [ "$ap_srctrl_srvalue15allowed" = "disable" ] || [ "$ap_srctrl_srvalue15allowed" = "0" ]; then
				ap_srctrl_srvalue15allowed="false"
			else
				error_print "Unsupported value - ap_srctrl_srvalue15allowed:$ap_srctrl_srvalue15allowed"
				send_error ",errorCode,203"
				return
			fi

			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sr_control_field_hesiga_spatial_reuse_value15_allowed' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="sr_control_field_hesiga_spatial_reuse_value15_allowed=${ap_srctrl_srvalue15allowed}"
		fi

		if [ "$ap_he_txop_dur_rts_thr_conf" != "" ]; then
			debug_print "ap_he_txop_dur_rts_thr_conf:$ap_he_txop_dur_rts_thr_conf"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_operation_txop_duration_rts_threshold' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`\
			$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_operation_txop_duration_rts_threshold=${ap_he_txop_dur_rts_thr_conf}"
		fi
		#Re-spawn SMD in-case Preamble Puncture was enabled as the previous SMD was spawned before Preamble Puncture config reached us
		if [ "$global_ap_preamble_puncture" = "enable" ]; then
			get_nof_sta_per_he_test_case $glob_ssid
			ap_num_users_ofdma=$nof_sta
			arguments_file_initializer $CURRENT_RADIO_UCI_PATH $glob_ssid
			kill_sigmaManagerDaemon
			/lib/netifd/sigmaManagerDaemon.sh $CURRENT_WLAN_NAME.0 $ap_num_users_ofdma &
		fi
	fi #HE

	send_complete
}

ap_set_11n_wireless()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_IFACE_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			WLAN_TAG)
				# skip as it is determined in get_interface_details
			;;
			AMPDU)
				debug_print "set parameter ap_ampdu=$1"
			;;
			SPATIAL_RX_STREAM)
				# do nothing
				debug_print "set parameter ap_spatial_rx_stream=$1"
			;;
			SPATIAL_TX_STREAM)
				# do nothing
				debug_print "set parameter ap_spatial_tx_stream=$1"
			;;
			SGI20)
				debug_print "set parameter ap_sgi20=$1"
				lower "$1" ap_enable_input
				if [ "$ap_enable_input" = "enable" ]; then
					ap_sgi_20=1
				else
					ap_sgi_20=0
				fi
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.short_gi_20=$ap_sgi_20
			;;
			WIDTH)
				debug_print "set parameter ap_width=$1"
				ap_curr_mode=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.htmode`
				ap_curr_prefix=`echo "${ap_curr_mode//[^[:alpha:]]/}"`
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.htmode=$ap_curr_prefix$1
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,7"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_11h()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_RADIO_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			WLAN_TAG)
				# skip as it is determined in get_interface_details
			;;
			DFS_CHAN)
				debug_print "set parameter ap_dfs_chan=$1"
				$UCI_CMD set ${CURRENT_RADIO_UCI_PATH}.channel=$1
			;;
			DFS_MODE)
				debug_print "set parameter ap_dfs_mode=$1"
				lower "$1" ap_enable_input
				if [ "$ap_enable_input" = "enable" ]; then
					ap_doth=1
				else
					ap_doth=0
				fi
				$UCI_CMD set ${CURRENT_RADIO_UCI_PATH}.doth=$ap_doth
			;;
			REGULATORY_MODE)
				# param not supported
				debug_print "set parameter ap_regulatory_mode=$1"
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,7"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_11d()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_RADIO_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			WLAN_TAG)
				# skip as it is determined in get_interface_details
			;;
			COUNTRYCODE)
				debug_print "set parameter ap_country_code=$1"
				$UCI_CMD set wireless.radio0.country=$1
				$UCI_CMD set wireless.radio2.country=$1
			;;
			REGULATORY_MODE)
				# param not supported
				debug_print "set parameter ap_regulatory_mode=$1"
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,7"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_security()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_IFACE_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			WLAN_TAG)
				# skip as it is determined in get_interface_details
			;;
			KEYMGNT)
				debug_print "set parameter ap_keymgnt=$1"
				ap_keymgnt=$1
				get_uci_security_mode "$1"
				if [ "$ap_uci_security_mode" = "" ]; then
					send_invalid ",errorCode,2"
					return
				else
					if [ "$CONFIGURE_BOTH_BANDS" != "" ] && [ $CONFIGURE_BOTH_BANDS -gt 0 ]
					then
						$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.encryption=$ap_uci_security_mode
						$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.encryption=$ap_uci_security_mode
					else
						$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.encryption=$ap_uci_security_mode
					fi
				fi

				if [ "$ap_uci_security_mode" = "sae-mixed" ]; then
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.ieee80211w=1
				elif [ "$ap_uci_security_mode" = "sae" ]; then
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.ieee80211w=2
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.auth_cache=1
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.interworking=1
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.mbo=0
					#for 6GHz certification if sae enable then sae_pwe=1 must
					ap_sae_band=`$UCI_CMD get ${CURRENT_RADIO_UCI_PATH}.band | cut -d"=" -f2`
					if [ "$ap_sae_band" = "6GHz"  -a "$ap_non_tx_bss_index" = "" ] ; then
						$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.mbo=1
						$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.sae_pwe=1
					fi
				fi
				if [ "$ap_uci_security_mode" = "owe" ]; then
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.ieee80211w=2
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.mbo=0
				fi
			;;
			ENCRYPT)
				debug_print "set parameter ap_encrypt=$1"
				get_uci_encryption "$1"
				if [ "$ap_uci_encrypt" = "" ]; then
					send_invalid ",errorCode,2"
					return
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.encryption=$ap_uci_encrypt
				fi
			;;
			PSK)
				debug_print "set parameter ap_psk=$1"
				ap_psk=$1
				if [ "$CONFIGURE_BOTH_BANDS" != "" ] && [ $CONFIGURE_BOTH_BANDS -gt 0 ]
				then
					$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.key=$1
					$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.key=$1
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.key=$1
				fi
			;;
			WEPKEY)
				debug_print "set parameter ap_wepkey=$1"
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.key=$1
			;;
			SSID)
				debug_print "set parameter ap_ssid=$1"
			;;
			PMF)
				debug_print "set parameter ap_pmf=$1"
				get_uci_pmf "$1"
				if [ "$ap_uci_pmf" = "" ]; then
					send_invalid ",errorCode,2"
					return
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.ieee80211w=$ap_uci_pmf
				fi
			;;
			SHA256AD)
				# param not supported
				debug_print "set parameter ap_sha256ad=$1"
			;;
			AKMSUITETYPE)
				debug_print "set parameter ap_akm_suite_type=$1"
			;;
			PMKSACACHING)
				debug_print "set parameter ap_pmks_a_caching=$1"
				upper "$1" ap_pmksacaching
				if [ "$ap_pmksacaching" = "DISABLED" ]; then
					local ap_ac_enable=0
				elif [ "$ap_pmksacaching" = "ENABLED" ]; then
					local ap_ac_enable=1
				else
					send_invalid ",errorCode,2"
					return
				fi
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.auth_cache=$ap_ac_enable
			;;
			ANTICLOGGINGTHRESHOLD)
				debug_print "set parameter sae_anti_clogging_threshold=$1"
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.debug_hostap_conf="sae_anti_clogging_threshold=$1"
			;;
			PAIRWISECIPHER)
				debug_print "set parameter pairwise_cipher=$1"
				get_uci_pairwise_cipher $1
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.debug_hostap_conf="wpa_pairwise=$ap_cipher"
			;;
			GROUPCIPHER)
				# param not supported
				debug_print "set parameter group_cipher=$1"
			;;
			GROUPMGNTCIPHER)
				debug_print "set parameter group_mgnt_cipher=$1"
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.debug_hostap_conf="group_mgmt_cipher=$1"
			;;
			REFLECTION)
				# param not supported
				debug_print "set parameter reflection=$1"
			;;
			INVALIDSAEELEMENT)
				# param not supported
				debug_print "set parameter invalid_sae_element=$1"
			;;
			ECGROUPID)
				debug_print "set parameter ec_group_id=$1"
				ap_sae_groups=`echo $1 | sed 's/;/ /g'`
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.debug_hostap_conf="sae_groups=$ap_sae_groups"
			;;
			NONTXBSSINDEX)
				debug_print "ap_set_security: set ap_non_tx_bss_index=$1"
				ap_non_tx_bss_index=$1
			;;
			CHNLFREQ)
				debug_print "ignore CHNLFREQ"
			;;
			SAE_PWE)
				if [ "$1" = "looping" ]; then
	                                $UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.sae_pwe=0
				fi
			;;
			*)
				debug_print "while loop error $1"
				send_invalid ",errorCode,18"
				return
			;;
		esac
		shift
	done

	if [ -n "$ap_non_tx_bss_index" ]; then
		([ $ap_non_tx_bss_index -lt 1 ] || [ $ap_non_tx_bss_index -gt 8 ]) && error_print "NonTxBSSIndex invalid value '$1'" && send_invalid ",errorCode,96" && return
		vap_index=$((ap_non_tx_bss_index))
		if [ -n "$ap_keymgnt" ] && [ -n "$ap_psk" ]; then
			[ "$ap_keymgnt" = "wpa2-psk" ] && ap_keymgnt="WPA2-Personal"
			tmp_current_o_vap="mbss_vap_$vap_index"
			current_o_vap=`eval 'echo $'$tmp_current_o_vap`
			debug_print "------------------------ security: current_o_vap=$current_o_vap ------------------------"
			$UCI_CMD set wireless.$current_o_vap.encryption="$ap_uci_security_mode"
			$UCI_CMD set wireless.$current_o_vap.key="$ap_psk"
			#for 6GHz certification if sae enable then sae_pwe=1 must
			if [ "$ap_sae_band" = "6GHz" ] ; then
				$UCI_CMD set wireless.$current_o_vap.mbo=1
				$UCI_CMD set wireless.$current_o_vap.sae_pwe=1
			fi

			ap_non_tx_bss_index=""
			ap_keymgnt=""
			ap_psk=""
			ap_sae_band=""
		fi
	fi

	send_complete
}

start_wps_registration()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			WPSROLE)
				debug_print "set parameter ap_wps_role=$1"
				# no param for this in our DB.
				# we are already configured correctly by default no action here.
				# By default AP is Registrar and STA is Enrollee
			;;
			WPSCONFIGMETHOD)
				debug_print "set parameter ap_wps_config_method=$1"
				# ConfigMethodsEnabled
				# we are already configured correctly by default no action here.
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,20"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_wps_pbc()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,21"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_pmf()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			PMF)
				debug_print "set parameter ap_pmf_ena=$1"
				get_uci_pmf "$1"
				if [ "$ap_uci_pmf" = "" ]; then
					send_invalid ",errorCode,2"
					return
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.ieee80211w=$ap_uci_pmf
				fi
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,22"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_apqos()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_IFACE_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip as it is determined in get_interface_details
			;;
			CWMIN*)
				lower "${token#CWMIN_*}" ap_actype
				ap_acpref=${ap_actype#*cwmin_}
				get_apqos_data_idx $ap_acpref
				local ap_cwmin=$((1<<$1))
				ap_cwmin=$((ap_cwmin-1))
				debug_print "set parameter ap_cwmin=$ap_cwmin"
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.tx_queue_data${ap_data_idx}_cwmin=$ap_cwmin
			;;
			CWMAX*)
				lower "${token#CWMAX_*}" ap_actype
				ap_acpref=${ap_actype#*cwmax_}
				get_apqos_data_idx $ap_acpref
				local ap_cwmax=$((1<<$1))
				ap_cwmax=$((ap_cwmax-1))
				debug_print "set parameter ap_cwmax=$ap_cwmax"
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.tx_queue_data${ap_data_idx}_cwmax=$ap_cwmax
			;;
			AIFS*)
				lower "${token#AIFS_*}" ap_actype
				debug_print "set parameter ap_aifs=$1"
				ap_acpref=${ap_actype#*aifs_}
				get_apqos_data_idx $ap_acpref
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.tx_queue_data${ap_data_idx}_aifs=$1
			;;
			TXOP*)
				lower "${token#TXOP_*}" ap_actype
				debug_print "set parameter ap_txop=$1"
			;;
			ACM*)
				lower "${token#ACM_*}" ap_actype
				debug_print "set parameter ap_acm=$1"
				lower "$ap_acm" ap_acm_${ap_actype}
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,31"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_staqos()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_IFACE_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip as it is determined in get_interface_details
			;;
			CWMIN*)
				lower "${token#CWMIN_*}" ap_actype
				debug_print "set parameter ap_cwmin=$1"
				ap_acpref=${ap_actype#*cwmin_}
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.wmm_ac_${ap_acpref}_cwmin=$1
			;;
			CWMAX*)
				lower "${token#CWMAX_*}" ap_actype
				debug_print "set parameter ap_cwmax=$1"
				ap_acpref=${ap_actype#*cwmax_}
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.wmm_ac_${ap_acpref}_cwmax=$1
			;;
			AIFS*)
				lower "${token#AIFS_*}" ap_actype
				debug_print "set parameter ap_aifs=$1"
				ap_acpref=${ap_actype#*aifs_}
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.wmm_ac_${ap_acpref}_aifs=$1
			;;
			TXOP*)
				lower "${token#TXOP_*}" ap_actype
				debug_print "set parameter ap_txop=$1"
				ap_acpref=${ap_actype#*txop_}
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.wmm_ac_${ap_acpref}_txop_limit=$1
			;;
			ACM*)
				lower "${token#ACM_*}" ap_actype
				debug_print "set parameter ap_acm=$1"

				if [ "$1" = "off" ]; then
					ap_acm_enable=0
				elif [ "$1" = "on" ]; then
					ap_acm_enable=1
				else
					send_invalid ",errorCode,2"
					return
				fi

				ap_acpref=${ap_actype#*acm_}
				$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.wmm_ac_${ap_acpref}_acm=$ap_acm_enable
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,37"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_radius()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_IFACE_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			IPADDR)
				debug_print "set parameter ap_ipaddr=$1"
				tc_name=`get_test_case_name $glob_ssid`
				if [ "$tc_name" = "4.7.1" ]; then 
					CONFIGURE_BOTH_BANDS=1
				fi
				if [ "$CONFIGURE_BOTH_BANDS" != "" ] && [ $CONFIGURE_BOTH_BANDS -gt 0 ]
				then
					$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.server=$1
					$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.server=$1
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.server=$1
				fi
			;;
			PORT)
				debug_print "set parameter ap_port=$1"
				if [ "$CONFIGURE_BOTH_BANDS" != "" ] && [ $CONFIGURE_BOTH_BANDS -gt 0 ]
				then
					$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.port=$1
					$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.port=$1
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.port=$1
				fi
			;;
			PASSWORD)
				debug_print "set parameter ap_password=$1"
				if [ "$CONFIGURE_BOTH_BANDS" != "" ] && [ $CONFIGURE_BOTH_BANDS -gt 0 ]
				then
					$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.key=$1
					$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.key=$1
				else
					$UCI_CMD set ${CURRENT_IFACE_UCI_PATH}.key=$1
				fi
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,38"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_hs2()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERWORKING)
				debug_print "set parameter ap_interworking=$1"
			;;
			ACCS_NET_TYPE)
				debug_print "set parameter ap_accs_net_type=$1"
			;;
			INTERNET)
				debug_print "set parameter ap_internet=$1"
			;;
			VENUE_GRP)
				debug_print "set parameter ap_venue_grp=$1"
			;;
			VENUE_TYPE)
				debug_print "set parameter ap_venue_type=$1"
			;;
			VENUE_NAME)
				debug_print "set parameter ap_venue_name=$1"
			;;
			HESSID)
				debug_print "set parameter ap_hessid=$1"
			;;
			ROAMING_CONS)
				debug_print "set parameter ap_roaming_cons=$1"
			;;
			DGAF_DISABLE)
				debug_print "set parameter ap_dgaf_disabled=$1"
			;;
			ANQP)
				debug_print "set parameter ap_anqp=$1"
			;;
			NET_AUTH_TYPE)
				debug_print "set parameter ap_net_auth_type=$1"
			;;
			NAI_REALM_LIST)
				debug_print "set parameter ap_nai_realm_list=$1"
			;;
			DOMAIN_LIST)
				debug_print "set parameter ap_domain_list=$1"
			;;
			OPER_NAME)
				debug_print "set parameter ap_oper_name=$1"
			;;
			GAS_CB_DELAY)
				debug_print "set parameter ap_gas_cb_delay=$1"
			;;
			MIH)
				# param not supported
				debug_print "set parameter ap_mih=$1"
			;;
			L2_TRAFFIC_INSPECT)
				# not defined in Oliver's doc
				debug_print "set parameter ap_l2_traffic_inspect=$1"
			;;
			BCST_UNCST)
				# param not supported
				debug_print "set parameter ap_bcst_uncst=$1"
			;;
			PLMN_MCC)
				PLMN_MCC_VAL="$1"
				debug_print "set parameter ap_plmn_mcc=$1"
			;;
			PLMN_MNC)
				debug_print "set parameter ap_plmn_mnc=$1"
			;;
			PROXY_ARP)
				debug_print "set parameter ap_proxy_arp=$1"
			;;
			WAN_METRICS)
				debug_print "set parameter ap_wan_metrics=$1"
			;;
			CONN_CAP)
				case "$1" in
					1)
						debug_print "set parameter addConnectionCap 6:20:1"
						debug_print "set parameter addConnectionCap 6:80:1"
						debug_print "set parameter addConnectionCap 6:443:1"
						debug_print "set parameter addConnectionCap 17:244:1"
						debug_print "set parameter addConnectionCap 17:4500:1"
					;;
				esac
			;;
			IP_ADD_TYPE_AVAIL)
				case "$1" in
					1)
						debug_print "set parameter ipv4AddrType"
					;;
				esac
			;;
			ICMPv4_ECHO)
				debug_print "set parameter ap_icmpv4_echo=$1"
				#wlancli -c set_wlan_hs_l2_firewall_list -P addAction "$1" addProtocol 1
			;;
			OPER_CLASS)
				case "$1" in
					1)
						debug_print "set parameter operatingClass=51"
					;;
					2)
						debug_print "set parameter operatingClass=73"
					;;
					3)
						debug_print "set parameter operatingClass=5173"
					;;
				esac
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,39"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_set_rfeature()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	if [ "$CURRENT_RADIO_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	if [ "$CURRENT_IFACE_UCI_PATH" = "" ]; then
		debug_print "Can't obtain uci path"
	fi

	if [ "$CURRENT_WLAN_NAME" = "" ]; then
		debug_print "Can't obtain wlan name"
	fi

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "set parameter ap_name=$1"
			;;
			INTERFACE)
				# skip as it is determined in get_interface_details
			;;
			WLAN_TAG)
				# skip as it is determined in get_interface_details
			;;
			TYPE)
				debug_print "set parameter ap_type=$1"
				ap_type=$1
			;;
			BSS_TRANSITION)
				# do nothing
				debug_print "set parameter ap_bss_transition=$1"
			;;
			NSS_MCS_OPT)
				debug_print "set parameter ap_nss_mcs_opt=$1"
				ap_nss_mcs_opt=$1
			;;
			OPT_MD_NOTIF_IE)
				debug_print "set parameter ap_opt_md_notif_ie=$1"
			;;
			CHNUM_BAND)
				debug_print "set parameter ap_chnum_band=$1"

				ap_chnum_band=$1
				ap_phy=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.phy`
				ap_chan=${ap_chnum_band%%;*}
				ap_band=${ap_chnum_band##*;}

				get_freq_from_chan $ap_phy $ap_chan
				ap_new_freq=$ap_freq
				get_central_freq_vht $ap_chan $ap_band $ap_phy

				ap_cmd="$HOSTAPD_CLI_CMD -i$CURRENT_WLAN_NAME chan_switch 5 $ap_new_freq center_freq1=$ap_centr_freq bandwidth=$ap_band auto-ht"
				ap_tmp=`eval $ap_cmd`
			;;
			RTS_FORCE)
				debug_print "set parameter ap_rts_force=$1"
				local cur_rts_mode=`uci show wireless | grep sRTSmode | cut -d "=" -f3 | tr -d "'"`
				if [ -n "$cur_rts_mode" ]; then
					iw dev $CURRENT_WLAN_NAME iwlwav sRTSmode $cur_rts_mode
				fi
			;;
			BTM_DISASSOCIMNT)
				debug_print "set parameter ap_btmreq_disassoc_imnt=$1"
			;;
			BTMREQ_DISASSOC_IMNT)
				debug_print "set parameter ap_btmreq_disassoc_imnt=$1"
				BTM_DISASSOC_IMMITIENT=$1
				if [ "$BTM_REASSOC_DELAY" = "" ]; then
					BTM_REASSOC_DELAY=100
				fi
			;;
			BTMREQ_TERM_BIT)
				debug_print "set parameter ap_btmreq_term_bit=$1"
				BTM_BSS_TERM_BIT=$1
			;;
			BTM_BSSTERM)
				debug_print "set parameter ap_btm_bssterm=$1"
			;;
			BSS_TERM_DURATION)
				debug_print "set parameter ap_btm_bssterm=$1"
				BTM_BSS_TERM_DURATION=$1
			;;
			ASSOC_DISALLOW)
				debug_print "set parameter ap_assoc_disallow=$1"
				lower "$1" ap_enable_input
				if [ "$ap_enable_input" = "enable" ]; then
					ap_enable=1
				else
					ap_enable=0
				fi

				ap_macaddr=`$UCI_CMD get $CURRENT_IFACE_UCI_PATH.macaddr`
				ap_cmd="$HOSTAPD_CLI_CMD -i$CURRENT_WLAN_NAME mbo_bss_assoc_disallow $ap_macaddr $ap_enable"
				ap_tmp=`eval $ap_cmd`
			;;
			DISASSOC_TIMER)
				debug_print "set parameter ap_disassoc_timer=$1"
				ap_disassoc_timer=$1
			;;
			ASSOC_DELAY)
				debug_print "set parameter ap_assoc_delay=$1"
				BTM_REASSOC_DELAY=$1
			;;
			NEBOR_BSSID)
				debug_print "set parameter ap_nebor_bssid=$1"
				ap_nebor_bssid=$1
			;;
			NEBOR_OP_CLASS)
				debug_print "set parameter ap_nebor_op_class=$1"
				ap_nebor_op_class=$1
			;;
			NEBOR_OP_CH)
				debug_print "set parameter ap_nebor_op_ch=$1"
				ap_nebor_op_ch=$1
			;;
			NEBOR_PREF)
				debug_print "set parameter ap_nebor_priority=$1"
				ap_nebor_pref=$1
			;;
			BSS_TERM_TSF)
				debug_print "set parameter ap_bssTermTSF=$1"
				BTM_BSS_TERM_TSF=$1
			;;
			PROGRAM)
				debug_print "set parameter ap_program=$1"
				ap_program=$1
			;;
			DOWNLINKAVAILCAP)
				debug_print "set parameter ap_down_link_avail_cap=$1"
			;;
			UPLINKAVAILCAP)
				debug_print "set parameter ap_up_link_avail_cap=$1"
			;;
			RSSITHRESHOLD)
				debug_print "set parameter ap_rssi_threshold=$1"
			;;
			RETRYDELAY)
				debug_print "set parameter ap_retry_delay=$1"
			;;
			TXPOWER)
				debug_print "set parameter ap_tx_power=$1"
			;;
			TXBANDWIDTH)
				debug_print "set parameter ap_txbandwidth=$1"
				ap_txbandwidth=$1
				glob_ap_txbandwidth=$1
			;;
			LTF)
				# param not supported
				debug_print "set parameter ap_ltf=$1"
				ap_ltf=$1
			;;
			GI)
				# param not supported
				debug_print "set parameter ap_gi=$1"
				ap_gi=$1
			;;
			RUALLOCTONES)
				debug_print "set parameter ap_rualloctones=$1"
				ap_rualloctones=$1
				glob_ap_rualloctones=$1
			;;
			ACKPOLICY)
				ap_ack_policy=$1
			;;
			ACKPOLICY_MAC)
				debug_print "set parameter glob_ap_ack_policy_mac=$1"
				glob_ap_ack_policy_mac=$1
			;;
			TRIGGERTYPE)
				debug_print "set parameter ap_trigger_type=$1"
				ap_trigger_type=$1
			;;
			TRIGGER_TXBF)
				# do nothing
				debug_print "set parameter ap_trigger_tx_bf=$1"
				iw dev $CURRENT_WLAN_NAME iwlwav sBfMode 0
				# currently this command is passive. Should affect the OFDMA TF BF only.
			;;
			TRIGGERCODING)
				debug_print "set parameter glob_ap_trigger_coding=$1"
				glob_ap_trigger_coding=$1
			;;
			AID)
				debug_print "set parameter ap_sta_aid=$1"
				glob_ap_sta_aid=$1
			;;
			TRIG_COMINFO_BW)
				debug_print "set parameter ap_cominfo_bw=$1"
				ap_cominfo_bw=$1
			;;
			TRIG_COMINFO_GI-LTF)
				debug_print "set parameter ap_cominfo_gi_ltf=$1"
				ap_cominfo_gi_ltf=$1
			;;
			TRIG_USRINFO_SSALLOC_RA-RU)
				debug_print "set parameter ap_usrinfo_ss_alloc_ra_ru=$1"
				ap_usrinfo_ss_alloc_ra_ru=$1
			;;
			TRIG_USRINFO_RUALLOC)
				debug_print "set parameter glob_ap_usrinfo_ru_alloc=$1"
				glob_ap_usrinfo_ru_alloc=$1
			;;
			ACKTYPE)
				# do nothing
			;;
			PPDUTXTYPE)
				debug_print "set parameter ap_ppdutxtype=$1"
				ap_ppdutxtype=$1
			;;
			DISABLETRIGGERTYPE)
				debug_print "set parameter ap_disable_trigger_type=$1"
				ap_disable_trigger_type=$1
			;;
			UNSOLICITEDPROBERESP)
				debug_print "set parameter ap_unsolicitedproberesp=$1"
				lower "$1" ap_unsolicitedproberesp
				if [ "$ap_unsolicitedproberesp" = "enable" ]; then
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name set_unsolicited_frame_support 1`
				else
					if [ `eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name get_unsolicited_frame_support` = 1 ]; then
						ap_tmp=`eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name set_unsolicited_frame_support 0`
					fi
				fi
				ap_tmp=`eval $UPDATE_RNR_CLI enable`
			;;
			CADENCE_UNSOLICITEDPROBERESP)
				debug_print "set parameter ap_cadence_unsolicitedproberesp=$1"
				lower "$1" ap_cadence_unsolicitedproberesp
				ap_tmp=`eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name set_unsolicited_frame_duration $ap_cadence_unsolicitedproberesp`
			;;
			FILSDSCV)
				debug_print "set parameter ap_fils_dscv=$1"
				lower "$1" ap_fils_dscv
				if [ "$ap_fils_dscv" = "enable" ]; then
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name set_unsolicited_frame_support 2`
				else
					if [ `eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name get_unsolicited_frame_support` = 2 ]; then
						ap_tmp=`eval $HOSTAPD_CLI_CMD -i $ap_wlan_6g_name set_unsolicited_frame_support 0`
					fi
				fi
			;;
			TRIG_USRINFO_UL-MCS)
				debug_print "set parameter ap_trig_usrinfo_ul_mcs=$1"
				ap_trig_usrinfo_ul_mcs=$1
			;;
			TRIG_COMINFO_ULLENGTH)
				debug_print "set parameter ap_trig_cominfo_ullength=$1"
				ap_trig_cominfo_ullength=$1
			;;
			NAV_UPDATE)
				debug_print "set parameter ap_nav_update=$1"
				ap_nav_update=$1
				#Enable SDoSimpleCLI command to disable NAV update in RxC
				if [ "$ap_nav_update" = "disable" ]; then
					ap_tmp=`eval iw $CURRENT_WLAN_NAME iwlwav sDoSimpleCLI 4 0`
				else
					ap_tmp=`eval iw $CURRENT_WLAN_NAME iwlwav sDoSimpleCLI 4 1`
				fi
			;;
			TRIG_INTERVAL)
				debug_print "set parameter ap_trig_interval=$1"
				ap_trig_interval=$1
			;;
			TRIG_COMINFO_CSREQUIRED)
				# do nothing
				# for test # 5.62.1: in the FW 'commonTfInfoPtr->tfCsRequired' = 1 by default
				if [ "$1" != "1" ]; then
					error_print "Trig_ComInfo_CSRequired = '$1' ; only '1' is supported"
					send_invalid ",errorCode,390"
					return
				fi
			;;
			STA_WMMPE_TXOP*)
				lower "${token#STA_WMMPE_TXOP_*}" ap_actype
				debug_print "set parameter ap_txop=$1"
				ap_acpref=${ap_actype#*txop_}
				ap_tmp=`eval $HOSTAPD_CLI_CMD -i$CURRENT_WLAN_NAME.0 set wmm_ac_${ap_acpref}_txop_limit $1`
			;;
			BTWT_ID)
				debug_print "set parameter ap_bwt_id=$1"
				ap_btwt_id=$1
			;;
			TWT_TRIGGER)
				debug_print "set parameter ap_btwt_trigger=$1"
				if [ "$1" = "enable" ]; then
					ap_btwt_trigger=1
				elif [ "$1" = "disable" ]; then
					ap_btwt_trigger=0
				fi
			;;
			FLOWTYPE)
				debug_print "set parameter ap_btwt_flowtype=$1"
				ap_btwt_flowtype=$1
			;;
			BTWT_RECOMMENDATION)
				debug_print "set parameter ap_btwt_recommendation=$1"
				ap_btwt_recommendation=$1
			;;
			WAKEINTERVALEXP)
				debug_print "set parameter ap_btwt_wakeintervalexp=$1"
				ap_btwt_wakeintervalexp=$1
			;;
			WAKEINTERVALMANTISSA)
				debug_print "set parameter ap_btwt_wakeintervalmantissa=$1"
				ap_btwt_wakeintervalmantissa=$1
			;;
			NOMINALMINWAKEDUR)
				debug_print "set parameter ap_btwt_nominalminwakedur=$1"
				ap_btwt_nominalminwakedur=$1
			;;
			BTWT_PERSISTENCE)
				debug_print "set parameter ap_btwt_persistence=$1"
				ap_btwt_persistence=$1
			;;
			RXMAC)
				debug_print "set parameter ap_rxmac=$1"
				ap_rxmac=$1
			;;
			TEARDOWNALLTWT)
				debug_print "set parameter ap_teardownalltwt=$1"
				ap_teardownalltwt=$1
			;;
			TWTELEMENT)
				debug_print "set parameter ap_twtelement=$1"
				ap_twtelement=$1
			;;
			PREAMBLEPUNCTMODE)
				debug_print "set parameter global_ap_preamblepunctmode=$1"
				lower "$1" global_ap_preamblepunctmode
			;;
			PUNCTCHANNEL)
				debug_print "set parameter global_ap_preamblepunctchannel=$1"
				lower "$1" global_ap_preamblepunctchannel
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,40"
				return
			;;
		esac
		shift
	done

	if [ "$ucc_program" = "mbo" -o "$ucc_program" = "he" ]
	then
		config_neighbor

		if [ "${ap_nebor_bssid}" != "" ] && [ "${ap_nebor_op_class}" != "" ] && [ "${ap_nebor_op_ch}" != "" ] && [ "${ap_nebor_pref}" != "" ]
		then
			local old_IFS=$IFS
			IFS=""
			CURRENT_NEIGHBORS=`cat /tmp/mbo_neighbors`
			CURRENT_NEIGHBORS="neighbor=${ap_nebor_bssid},0,${ap_nebor_op_class},${ap_nebor_op_ch},9,${ap_nebor_pref} $CURRENT_NEIGHBORS"
			echo $CURRENT_NEIGHBORS > /tmp/mbo_neighbors
			IFS=$old_IFS
		fi
	fi

	if [ "$global_ap_btwt" = "1" ]; then
		if [ "$ap_btwt_id" != "" ]; then
			debug_print "Advertise BTWT schedule id:$ap_btwt_id flowtype:$ap_btwt_flowtype trigger:$ap_btwt_trigger wakedur:$ap_btwt_nominalminwakedur wakeintmant:$ap_btwt_wakeintervalmantissa wakeintexp: $ap_btwt_wakeintervalexp"
			ap_tmp=`eval iw dev $CURRENT_WLAN_NAME.0 iwlwav sAdvertiseBcTwtSp 1 0 $ap_btwt_id $ap_btwt_flowtype $ap_btwt_trigger $ap_btwt_nominalminwakedur $ap_btwt_wakeintervalmantissa $ap_btwt_wakeintervalexp`
		fi
	fi

	if [ "$ap_teardownalltwt" = "1" ]; then
		if [ "$ap_rxmac" != "" ]; then
			debug_print "Teardown all TWT schedules for station:$ap_rxmac"
			ap_aid=`cat /proc/net/mtlk/$CURRENT_WLAN_NAME.0/sta_list | grep "0" | grep -iF "$ap_rxmac" | awk '{print $3}'`
			if [ "$ap_aid" = "" ]; then
				error_print ""MAC not found: ap_rxmac:$ap_rxmac""
				send_error ",errorCode,204"
				return
			fi
			ap_sid=$((ap_aid-1))
			ap_tmp=`eval iw dev $CURRENT_WLAN_NAME.0 iwlwav sTxTwtTeardown $ap_sid 1`
		fi
	fi

	if [ "$ap_twtelement" = "exclude" ]; then
		debug_print "Exclude BTWT schedules from the beacon"
		ap_tmp=`eval iw dev $CURRENT_WLAN_NAME.0 iwlwav sTerminateBcTwtSp 1`
		# turn static plan off
		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
	fi

	if [ "$ap_nss_mcs_opt" != "" ]; then
		debug_print "ap_nss_mcs_opt:$ap_nss_mcs_opt"
		ap_nss_opt=${ap_nss_mcs_opt%%;*}
		ap_mcs_opt=${ap_nss_mcs_opt##*;}

		if [ "$ap_nss_opt" = "def" ]; then
			# change the NSS to the default value
			ap_nss_opt="$nss_def_val_dl"
			[ "$glob_ofdma_phase_format" = "1" ] && ap_nss_opt="$nss_def_val_ul"
			if [ "$ucc_type" = "testbed" ]; then
				ap_nss_opt="$nss_def_val_dl_testbed"
				[ "$glob_ofdma_phase_format" = "1" ] && ap_nss_opt="$nss_def_val_ul_testbed"
			fi
		fi

		if [ "$ap_mcs_opt" = "def" ]; then
			# change the MCS to the default value
			ap_mcs_opt="$mcs_def_val_dl"
			[ "$glob_ofdma_phase_format" = "1" ] && ap_mcs_opt="$mcs_def_val_ul"
			if [ "$ucc_type" = "testbed" ]; then
				ap_mcs_opt="$mcs_def_val_dl_testbed"
				[ "$glob_ofdma_phase_format" = "1" ] && ap_mcs_opt="$mcs_def_val_ul_testbed"
			fi
		fi

		# TODO: Handle for SU, when MU is not set

		# JIRA WLANRTSYS-9813: check whether the previously set value of nss is lower than the current one; if so, set the one we just got (the higher one)
		if [ -n "$ap_nss_opt" ] && [ -n "$global_nss_opt_ul" ] && [ -n "$global_nss_opt_dl" ]; then
			if [ $ap_nss_opt -gt $global_nss_opt_ul ] || [ $ap_nss_opt -gt $global_nss_opt_dl ]; then
				local nss_mcs_val=`get_nss_mcs_val $ap_nss_opt $ap_mcs_opt`

				if [ $ap_nss_opt -gt $global_nss_opt_ul ]; then
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz=${nss_mcs_val}"
					# JIRA WLANRTSYS-11028: part0-Rx part1-Tx
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'vht_mcs_set_part0' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="vht_mcs_set_part0=${nss_mcs_val}"
					global_nss_opt_ul=${ap_nss_opt}
				fi

				if [ $ap_nss_opt -gt $global_nss_opt_dl ]; then
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz=${nss_mcs_val}"
					# JIRA WLANRTSYS-11028: part0-Rx part1-Tx
					ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'vht_mcs_set_part1' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
					$UCI_CMD set $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx="vht_mcs_set_part1=${nss_mcs_val}"
					global_nss_opt_dl=${ap_nss_opt}
				fi
			fi
		fi

		# calculate the OFDMA MU NSS-MCS value (NSS: bits 5-4, MCS: bits 3-0)
		let ap_ofdma_mu_nss_mcs_val="($ap_nss_opt-1)*16+$ap_mcs_opt"

		# set for MU
		for usr_index in 1 2 3 4
		do
			# get MU DL MCS value
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'${usr_index} $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			current_ap_mcs=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x1`
			let current_ap_mcs="$current_ap_mcs%16"

			# set MU DL NSS MCS value
			if [ "$glob_ofdma_phase_format" = "0" ]; then
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x1=${ap_ofdma_mu_nss_mcs_val}
			elif [ "$glob_ofdma_phase_format" = "1" ]; then
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x3=${ap_ofdma_mu_nss_mcs_val}
			fi
		done
		ap_uci_commit_and_apply
		if [ "$OS_NAME" = "UGW" ]; then
			check_and_update_dc_registration
		fi
	fi

		# for test HE-4.43.1
	if [ "$ap_disable_trigger_type" != "" ] && [ "$ap_disable_trigger_type" = "0" ]; then
		# turn static plan off
		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
	fi

	# WLANRTSYS-11513 TC 5.61.1
	if [ "$ap_ppdutxtype" != "" ]; then
		if [ "$ap_ppdutxtype" = "HE-SU" ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=0
		elif [ "$ap_ppdutxtype" = "legacy" ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2700
		else
			error_print "!!! PPDUTXTYPE wrong value !!!"
		fi
		refresh_static_plan

	fi

	if [ "$ap_txbandwidth" != "" ]; then
		debug_print "ap_txbandwidth:$ap_txbandwidth"

		# set for SU, only if not OFDMA MU TC
		if [ "$glob_ofdma_phase_format" = "" ]; then
			debug_print "if [ \"$glob_ofdma_phase_format\" = \"\" ]; then"
			# JIRA WLANRTSYS-9189: remove the call to 'is_test_case_permitted_to_set_channel' - always set the channel
			convert_Operation_ChWidth ${ap_txbandwidth}MHz
			current_ht_mode=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.htmode`
			if [ "$current_ht_mode" != "VHT${ap_txbandwidth}" ]; then
				debug_print "vht_oper_chwidth_converted= $vht_oper_chwidth_converted"
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.htmode=VHT${ap_txbandwidth}
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.vht_oper_chwidth=$vht_oper_chwidth_converted
				convert_fixed_rate ${ap_txbandwidth}MHz "ax"
				Dynamic_set_get_helper_none_debug $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.sFixedRateCfg sFixedRateCfg x1=0 x2=${bw_converted} x3=$phym_converted x6=5 #band_width
				ap_uci_commit_and_apply # commit and apply 
				sleep 20
			fi	
		else
			# JIRA WLANRTSYS-9189: remove the call to 'is_test_case_permitted_to_set_channel' - always set the channel
			convert_Operation_ChWidth ${ap_txbandwidth}MHz
			current_ht_mode=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.htmode`
			if [ "$current_ht_mode" != "VHT${ap_txbandwidth}" ]; then
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.htmode=VHT${ap_txbandwidth}
				$UCI_CMD set $CURRENT_RADIO_UCI_PATH.vht_oper_chwidth=$vht_oper_chwidth_converted
				ap_uci_commit_and_apply
			fi
		fi
	
		# set for MU
		[ "$ap_txbandwidth" = "20" ] && ap_txbandwidth=0
		[ "$ap_txbandwidth" = "40" ] && ap_txbandwidth=1
		[ "$ap_txbandwidth" = "80" ] && ap_txbandwidth=2
		[ "$ap_txbandwidth" = "160" ] && ap_txbandwidth=3
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		info_print "Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${txop_com_start_bw_limit_idx}=${ap_txbandwidth}"
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${txop_com_start_bw_limit_idx}=${ap_txbandwidth}
		ap_num_participating_users=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station` 
		info_print "ap_num_participating_users=$ap_num_participating_users"

		local dl_sub_band1 dl_start_ru1 dl_ru_size1
		local dl_sub_band2 dl_start_ru2 dl_ru_size2
		local dl_sub_band3 dl_start_ru3 dl_ru_size3
		local dl_sub_band4 dl_start_ru4 dl_ru_size4

		# update 4 user plan according to BW - W/A to be align to WFA UCC.
		# 0-20MHz, 1-40MHz, 2-80MHz, 3-160MHz
		case "$ap_txbandwidth" in
			"0")
				if [ $ap_num_participating_users -gt 2 ]; then
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=1
					#USER2
					dl_sub_band2=0;dl_start_ru2=2;dl_ru_size2=1
					#USER3
					dl_sub_band3=0;dl_start_ru3=5;dl_ru_size3=1
					#USER4
					dl_sub_band4=0;dl_start_ru4=7;dl_ru_size4=1
				else
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2
					#USER2
					dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2
				fi
			;;
			"1")
				if [ $ap_num_participating_users -gt 2 ]; then
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2
					#USER2
					dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2
					#USER3
					dl_sub_band3=1;dl_start_ru3=0;dl_ru_size3=2
					#USER4
					dl_sub_band4=1;dl_start_ru4=5;dl_ru_size4=2
				else
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3
					#USER2
					dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3
				fi
			;;
			"2")
				if [ $ap_num_participating_users -gt 2 ]; then
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3
					#USER2
					dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3
					#USER3
					dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=3
					#USER4
					dl_sub_band4=3;dl_start_ru4=0;dl_ru_size4=3
				else
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4;
					#USER2
					dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4;
				fi
			;;
			"3")
				if [ $ap_num_participating_users -gt 2 ]; then
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4
					#USER2
					dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4
					#USER3
					dl_sub_band3=4;dl_start_ru3=0;dl_ru_size3=4
					#USER4
					dl_sub_band4=6;dl_start_ru4=0;dl_ru_size4=4
				else
					#USER1
					dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=5
					#USER2
					dl_sub_band2=4;dl_start_ru2=0;dl_ru_size2=5
				fi
			;;
		esac
		
		## WLANRTSYS-12035
		if [ $dl_ru_size1 -lt 2 ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2300
		else
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2700
		fi

	
		# update per-user params in DB
		for usr_index in 1 2 3 4
		do
			local tmp_param tmp_val
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			tmp_param="dl_sub_band$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${usr_index} x${dl_usr_sub_band_per_usp_idx}=${tmp_val}
	
			tmp_param="dl_start_ru$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${usr_index} x${dl_usr_start_ru_per_usp_idx}=${tmp_val}
			tmp_param="dl_ru_size$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${usr_index} x${dl_usr_ru_size_per_usp_idx}=${tmp_val}
		done

		if [ "$glob_ofdma_phase_format" != "" ]; then
			refresh_static_plan
		fi
	fi

	if [ "$ap_rualloctones" != "" ]; then
		debug_print "ap_rualloctones:$ap_rualloctones"

		# replace all ':' with " "
		tmp_ap_rualloctones=${ap_rualloctones//:/ }
		ap_rualloctones=${tmp_ap_rualloctones%% *}
		# ap_rualloctones implicitly holds the number of users.

		local user_index user_list index user_value start_bw_limit
		local dl_sub_band1 dl_start_ru1 dl_ru_size1
		local dl_sub_band2 dl_start_ru2 dl_ru_size2
		local dl_sub_band3 dl_start_ru3 dl_ru_size3
		local dl_sub_band4 dl_start_ru4 dl_ru_size4

		if [ "$ap_txbandwidth" != "" ]; then
			info_print "ap_txbandwidth :$ap_txbandwidth"
			# if exist, get the bw from previous parameter in this command
			start_bw_limit=$ap_txbandwidth
		else
			# else, get the bw from the SP
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			start_bw_limit=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$txop_com_start_bw_limit_idx` #x3
		fi

		user_index=0
		for user_value in $tmp_ap_rualloctones
		do
			let user_index=$user_index+1

			### BW=160MHz ###
			if [ "$start_bw_limit" = "3" ]; then
				if [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2
						;;
						"2") #USER2
							dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=2
						;;
						"3") #USER3
							dl_sub_band3=4;dl_start_ru3=0;dl_ru_size3=2
						;;
						"4") #USER4
							dl_sub_band4=6;dl_start_ru4=0;dl_ru_size4=2
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3
						;;
						"2") #USER2
							dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=3
						;;
						"3") #USER3
							dl_sub_band3=4;dl_start_ru3=0;dl_ru_size3=3
						;;
						"4") #USER4
							dl_sub_band4=6;dl_start_ru4=0;dl_ru_size4=3
						;;
					esac
				elif [ "$user_value" = "484" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4
						;;
						"2") #USER2
							dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4
						;;
						"3") #USER3
							dl_sub_band3=4;dl_start_ru3=0;dl_ru_size3=4
						;;
						"4") #USER4
							dl_sub_band4=6;dl_start_ru4=0;dl_ru_size4=4
						;;
					esac
				elif [ "$user_value" = "996" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=5
						;;
						"2") #USER2
							dl_sub_band2=4;dl_start_ru2=0;dl_ru_size2=5
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,640"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,645"
							return
						;;
					esac
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,650"
					return
				fi

			### BW=80MHz ###
			elif [ "$start_bw_limit" = "2" ]; then
				if [ "$user_value" = "26" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=0
						;;
						"2") #USER2
							dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=0
						;;
						"3") #USER3
							dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=0
						;;
						"4") #USER4
							dl_sub_band4=3;dl_start_ru4=0;dl_ru_size4=0
						;;
					esac
				elif [ "$user_value" = "52" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=1
						;;
						"2") #USER2
							dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=1
						;;
						"3") #USER3
							dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=1
						;;
						"4") #USER4
							dl_sub_band4=3;dl_start_ru4=0;dl_ru_size4=1
						;;
					esac
				elif [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2
						;;
						"2") #USER2
							dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=2
						;;
						"3") #USER3
							dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=2
						;;
						"4") #USER4
							dl_sub_band4=3;dl_start_ru4=0;dl_ru_size4=2
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3
						;;
						"2") #USER2
							dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3
						;;
						"3") #USER3
							dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=3
						;;
						"4") #USER4
							dl_sub_band4=3;dl_start_ru4=0;dl_ru_size4=3
						;;
					esac
				elif [ "$user_value" = "484" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4
						;;
						"2") #USER2
							dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,440"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,441"
							return
						;;
					esac
				elif [ "$user_value" = "996" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=5
						;;
						"2") #USER2
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,442"
							return
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,443"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,444"
							return
						;;
					esac
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,445"
					return
				fi

			### BW=40MHz ###
			elif [ "$start_bw_limit" = "1" ]; then
				if [ "$user_value" = "26" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=0
						;;
						"2") #USER2
							dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=0
						;;
						"3") #USER3
							dl_sub_band3=1;dl_start_ru3=0;dl_ru_size3=0
						;;
						"4") #USER4
							dl_sub_band4=1;dl_start_ru4=5;dl_ru_size4=0
						;;
					esac
				elif [ "$user_value" = "52" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=1
						;;
						"2") #USER2
							dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=1
						;;
						"3") #USER3
							dl_sub_band3=1;dl_start_ru3=0;dl_ru_size3=1
						;;
						"4") #USER4
							dl_sub_band4=1;dl_start_ru4=5;dl_ru_size4=1
						;;
					esac
				elif [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2
						;;
						"2") #USER2
							dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2
						;;
						"3") #USER3
							dl_sub_band3=1;dl_start_ru3=0;dl_ru_size3=2
						;;
						"4") #USER4
							dl_sub_band4=1;dl_start_ru4=5;dl_ru_size4=2
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3
						;;
						"2") #USER2
							dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,446"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,447"
							return
						;;
					esac
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,448"
					return
				fi

			### BW=20MHz ###
			elif [ "$start_bw_limit" = "0" ]; then
				if [ "$user_value" = "26" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=0
						;;
						"2") #USER2
							dl_sub_band2=0;dl_start_ru2=2;dl_ru_size2=0
						;;
						"3") #USER3
							dl_sub_band3=0;dl_start_ru3=5;dl_ru_size3=0
						;;
						"4") #USER4
							dl_sub_band4=0;dl_start_ru4=7;dl_ru_size4=0
						;;
					esac
				elif [ "$user_value" = "52" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=1
						;;
						"2") #USER2
							dl_sub_band2=0;dl_start_ru2=2;dl_ru_size2=1
						;;
						"3") #USER3
							dl_sub_band3=0;dl_start_ru3=5;dl_ru_size3=1
						;;
						"4") #USER4
							dl_sub_band4=0;dl_start_ru4=7;dl_ru_size4=1
						;;
					esac
				elif [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2
						;;
						"2") #USER2
							dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2
						;;
						"3") #USER3
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,450"
							return
						;;
						"4") #USER4
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,451"
							return
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3
						;;
						"2") #USER2
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,452"
							return
						;;
						"3") #USER3
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,453"
							return
						;;
						"4") #USER4
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,454"
							return
						;;
					esac
				elif [ "$user_value" = "484" ]; then
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,455"
					return
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,456"
					return
				fi
			else
				error_print "Unsupported value - start_bw_limit:$start_bw_limit"
				send_invalid ",errorCode,457"
				return
			fi
		done

		# user_index contains the number of users. set it to DB to be used by static plan.
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_num_of_participating_stations_idx}=${user_index}
	
		## WLANRTSYS-12035
		if [ $dl_ru_size1 -lt 2 ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2300
		else
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2700
		fi
	
	
		# update per-user params in DB, per number of users
		#for index in $user_index
		for index in 1 2 3 4 
		do
			local tmp_param tmp_val
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			
			tmp_param="dl_sub_band${index}";eval tmp_val=\$$tmp_param		
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${dl_usr_sub_band_per_usp_idx}=${tmp_val}
			
			tmp_param="dl_start_ru${index}";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${dl_usr_start_ru_per_usp_idx}=${tmp_val}
			
			tmp_param="dl_ru_size${index}";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${dl_usr_ru_size_per_usp_idx}=${tmp_val}
		done

		# dynamically update STA index in DB
		ap_aid_list=`cat /proc/net/mtlk/$ap_wlan_name/sta_list | awk '{print $3}' | tr  "\n" ","`
		ap_aid_list=${ap_aid_list##*AID}
		ap_aid_list="${ap_aid_list##,,}"
		ap_aid_list="${ap_aid_list%%,,}"

		index=0
		debug_print "ap_aid_list:$ap_aid_list"
		# assure that aid list is not empty (i.e. it contains one ',' when no sta connected)
		if [ "$ap_aid_list" != "," ]; then
			for ap_aid_index in $ap_aid_list
			do
				let index=index+1
				[ $ap_aid_index -gt 0 ] && let ap_sta_index=$ap_aid_index-1
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${dl_usr_usp_station_indexes_idx}=${ap_sta_index}
			done
		fi
		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
		connected_stations=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station`
		if [ "$connected_stations" != "0" ]; then
			send_plan_for_${connected_stations}_users ${CURRENT_WLAN_NAME} $connected_stations $CURRENT_RADIO_UCI_PATH $glob_ssid $CURRENT_WLAN_NAME.0
		else
			#Required for 5.34.5 Step 4, where the NSS is modified from 1 to 2 and RU Allocation is done prior to stations associating to the AP
			kill_sigmaManagerDaemon
			send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
			/lib/netifd/sigmaManagerDaemon.sh $CURRENT_WLAN_NAME.0 $user_index &
		fi
	fi

	if [ "$ap_ltf" != "" ] || [ "$ap_gi" != "" ]; then
		debug_print "ap_ltf:$ap_ltf ap_gi:$ap_gi"		
        tc_name=`get_test_case_name $glob_ssid`
		if [ "$tc_name" = "4.30.1" ]; then
			kill_sigmaManagerDaemon
		fi
		if [ "$ap_ltf" = "6.4" ] && [ "$ap_gi" = "0.8" ]; then
			ap_su_ltf_gi="He0p8usCP2xLTF"
			ap_mu_dl_com_he_cp=0
			ap_mu_dl_com_he_ltf=1
			if [ "$glob_ofdma_phase_format" = "1" ]; then
				# this LTF and GI combination is not supported in MU UL
				error_print "Unsupported value - glob_ofdma_phase_format:$glob_ofdma_phase_format ap_ltf:$ap_ltf ap_gi:$ap_gi"
				send_invalid ",errorCode,420"
				return
			fi
		elif [ "$ap_gi" = "1.6" ]; then
			# JIRA WLANRTSYS-9350: in this case, handle not getting "ap_ltf" as if it has the value of "6.4"
			if [ "$ap_ltf" = "" ] || [ "$ap_ltf" = "6.4" ]; then
				ap_su_ltf_gi="He1p6usCP2xLTF"
				ap_mu_dl_com_he_cp=1
				ap_mu_dl_com_he_ltf=1
				ap_mu_ul_com_he_cp=1
				ap_mu_ul_com_he_ltf=1
				ap_mu_ul_com_he_tf_cp_and_ltf=1
				ap_mu_tf_len=3094
			fi
		elif [ "$ap_ltf" = "12.8" ] && [ "$ap_gi" = "3.2" ]; then
			ap_su_ltf_gi="He3p2usCP4xLTF"
			ap_mu_dl_com_he_cp=2
			ap_mu_dl_com_he_ltf=2
			ap_mu_ul_com_he_cp=2
			ap_mu_ul_com_he_ltf=2
			ap_mu_ul_com_he_tf_cp_and_ltf=2
			ap_mu_tf_len=2914
		else
			# all other LTF and GI combinations are not required by WFA
			error_print "Unsupported value - ap_ltf:$ap_ltf ap_gi:$ap_gi"
			send_invalid ",errorCode,430"
			return
		fi

		debug_print "ap_su_ltf_gi:$ap_su_ltf_gi ap_mu_dl_com_he_cp:$ap_mu_dl_com_he_cp ap_mu_dl_com_he_ltf:$ap_mu_dl_com_he_ltf"
		debug_print "ap_mu_ul_com_he_cp:$ap_mu_ul_com_he_cp ap_mu_ul_com_he_ltf:$ap_mu_ul_com_he_ltf ap_mu_ul_com_he_tf_cp_and_ltf:$ap_mu_ul_com_he_tf_cp_and_ltf ap_mu_tf_len:$ap_mu_tf_len"

		# set for SU, only if not in OFDMA MU SP			
		if [ "$glob_ofdma_phase_format" = "" ]; then
			convert_fixed_ltf_gi Fixed ${ap_su_ltf_gi} 
			info_print "$CURRENT_WLAN_NAME iwlwav sFixedLtfGi ${is_auto_converted} ${ltf_and_gi_value_converted}"
			iw dev $CURRENT_WLAN_NAME iwlwav sFixedLtfGi ${is_auto_converted} ${ltf_and_gi_value_converted}
		else
		# set for MU
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			[ "$ap_mu_dl_com_he_cp" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$dl_com_he_cp_idx=${ap_mu_dl_com_he_cp}
			[ "$ap_mu_dl_com_he_ltf" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$dl_com_he_ltf_idx=${ap_mu_dl_com_he_ltf}
			[ "$ap_mu_ul_com_he_cp" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$ul_com_he_cp_idx=${ap_mu_ul_com_he_cp}
			[ "$ap_mu_ul_com_he_ltf" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$ul_com_he_ltf_idx=${ap_mu_ul_com_he_ltf}
			[ "$ap_mu_ul_com_he_tf_cp_and_ltf" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$rcr_com_tf_hegi_and_ltf_idx=${ap_mu_ul_com_he_tf_cp_and_ltf}
			[ "$ap_mu_tf_len" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common $rcr_com_tf_length_idx=${ap_mu_tf_len}
			
			refresh_static_plan
		fi
	fi

	local is_activate_sigmaManagerDaemon=0

	if [ "$ap_trigger_type" != "" ]; then
		info_print "iw dev $CURRENT_WLAN_NAME iwlwav sDoSimpleCLI 70 1"
		iw dev $CURRENT_WLAN_NAME iwlwav sDoSimpleCLI 70 1
		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		## kill the daemon if active - we need it only with ap_ack_policy empty.
		kill_sigmaManagerDaemon
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
		# JIRA WLANRTSYS-9307: in case "TRIGGERTYPE" was set, activate the SMD
		is_activate_sigmaManagerDaemon=1
	fi

	if [ "$ap_ack_policy" = "" ] && [ "$ap_trigger_type" != "" ]; then
		info_print "CURRENT_WLAN_NAME:$CURRENT_WLAN_NAME ap_trigger_type:$ap_trigger_type"
		case "$ap_trigger_type" in
		0)
			# BASIC - do nothing. We passed without configuring this.
		;;
		1)
			# BF_RPT_POLL (MU-BRP)
			ap_sequence_type=1  #HE_MU_SEQ_VHT_LIKE
			# Enable MIMO and set the phase format to DL to have DL traffic work. Using sounding phase format (=2) will not enable DL traffic phase as this is an unsupported config in FW.
			# MIMO will ensure to enable sounding implicitly.
			glob_ofdma_phase_format=0
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_phases_format_idx}=$glob_ofdma_phase_format
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_mu_type_idx}=1

			for ap_usr_index in 1 2
			do
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'${ap_usr_index} $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				if [ "$global_ap_mimo" != "ul" ]; then
					Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_usr_index} x${dl_usr_ul_psdu_rate_per_usp_idx}=4
				fi
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_usr_index} x${rcr_tf_usr_psdu_rate_idx}=4
			done
		;;
		3)
			# MU-RTS
			ap_sequence_type=4  #HE_MU_SEQ_VHT_LIKE_PROTECTION
		;;
		4)
			# BUFFER_STATUS_RPT (BSRP)
			if [ -n "$glob_ofdma_phase_format" ] && [ $glob_ofdma_phase_format -eq 0 ]; then
				ap_sequence_type=10 #HE_MU_BSRP_DL
			else
				ap_sequence_type=6  #HE_MU_BSRP
			fi
			if [ "$global_ap_mimo" = "dl" ]; then
				ap_tf_len=406
			else
				ap_tf_len=106
			fi
			# (was 109) UL_LEN = 109 (together with PE_dis=1) causes T_PE to exceed 16usec, which is not according to standard
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${rcr_com_tf_length_idx}=$ap_tf_len
			ap_mu_tf_len=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$rcr_com_tf_length_idx`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_number_of_phase_repetitions_idx}=0
		;;
		*)
			error_print "Unsupported value - ap_ack_policy empty and ap_trigger_type:$ap_trigger_type"
			send_invalid ",errorCode,461"
			return
		;;
		esac

	fi

	if [ "$ap_ack_policy" != "" ]; then
		debug_print "ap_ack_policy:$ap_ack_policy, ap_trigger_type:$ap_trigger_type"

		case "$ap_ack_policy" in
		0)
			# Ack Policy set to Normal Ack (internal name: immediate Ack)
			# we use the glob_ap_ack_policy_mac to set the requested user as primary.

			# Ack Policy MAC address handling
			if [ "$glob_ap_ack_policy_mac" != "" ]; then
				ap_aid=`cat /proc/net/mtlk/$CURRENT_WLAN_NAME.0/sta_list | grep "0" | grep -iF "$glob_ap_ack_policy_mac" | awk '{print $3}'`

				[ "$ap_aid" = "" ] && error_print ""MAC not found: glob_ap_ack_policy_mac:$glob_ap_ack_policy_mac""
			fi
			info_print "CURRENT_WLAN_NAME:$CURRENT_WLAN_NAME ap_ack_policy:$ap_ack_policy glob_ap_ack_policy_mac:$glob_ap_ack_policy_mac"
			ap_sequence_type=3  #HE_MU_SEQ_VHT_LIKE_IMM_ACK
		;;
		1)
			# Ack Policy set to No Ack
			# nothing to do. not supported yet.
			info_print "CURRENT_WLAN_NAME:$CURRENT_WLAN_NAME ap_ack_policy:$ap_ack_policy"
		;;
		2)
			# Ack Policy set to Implicit Ack (internal name: immediate Ack on Aggr., VHT-like)
			# nothing to do. not supported yet.
			info_print "CURRENT_WLAN_NAME:$CURRENT_WLAN_NAME ap_ack_policy:$ap_ack_policy"
		;;
		3)
			# Ack Policy set to Block Ack (internal name: sequential BAR, VHT-like)
			# this is the default value, but we anyway return it to default.
			info_print "CURRENT_WLAN_NAME:$CURRENT_WLAN_NAME ap_ack_policy:$ap_ack_policy"
			ap_sequence_type=1  #HE_MU_SEQ_VHT_LIKE

			# MU-BAR scenario TC 5.51 & 4.45
			if [ "$ap_trigger_type" = "2" ]; then
				ap_sequence_type=0  #HE_MU_SEQ_MU_BAR
				ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${rcr_com_tf_length_idx}=310
				ap_mu_tf_len=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$rcr_com_tf_length_idx`
			fi
		;;
		4)
			# Ack Policy set to Unicast TF Basic
			if [ "$ap_trigger_type" = "0" ]; then
				ap_sequence_type=2  #HE_MU_SEQ_DL_BASIC_TF
			fi
		;;
		*)
			error_print "Unsupported value - ap_ack_policy:$ap_ack_policy"
			send_invalid ",errorCode,460"
			return
		;;
		esac
	fi

	# update the phase format, only if needed
	if [ "$ap_sequence_type" != "" ]; then
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${sequence_type_idx}=${ap_sequence_type}

		ap_interface_index=${ap_wlan_name/wlan/}

		# update the primary sta, only if needed
		if [ "$ap_aid" != "" ]; then
			# convert aid=1,2,3,... to  sta_id=0,1,2,...
			let ap_sta_id=$ap_aid-1

			# find the user id of the requested primary sta
			for ap_user_id in 1 2 3 4
			do
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_user_id $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				ap_sta_index=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$dl_usr_usp_station_indexes_idx`
				
				if [ "$ap_sta_index" = "$ap_sta_id" ]; then
					glob_ap_user_id_prim=$ap_user_id
					break
				fi 
			done

			# switch the OFDMA users, so the primary sta id will be at user 1 (first user). not needed if it is already in user 1.
			if [ "$glob_ap_user_id_prim" != "1" ]; then
				# 1. load the sta id of user1
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser1' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				ap_orig_sta_index=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$dl_usr_usp_station_indexes_idx`

				# 2. store the primary sta id in user 1
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser1 x${dl_usr_usp_station_indexes_idx}=${ap_sta_id}
				# 3. store the original sta id from user 1 in the found user
				ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'${glob_ap_user_id_prim} $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${glob_ap_user_id_prim} x${dl_usr_usp_station_indexes_idx}=${ap_orig_sta_index}
			fi
		fi

		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
		connected_stations=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station`

		# JIRA WLANRTSYS-9307: in case the SMD needed to be activated, make sure the plan won't be set
		if [ "$is_activate_sigmaManagerDaemon" = "1" ]; then
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=0
		fi

		send_plan_for_${connected_stations}_users ${CURRENT_WLAN_NAME} $connected_stations $CURRENT_RADIO_UCI_PATH $glob_ssid $CURRENT_WLAN_NAME.0
	fi
	# for test HE-5.64.1 & 5.61.1
	if [ "$ap_trig_interval" != "" ]; then
		# iw wlanX sDoSimpleCLI 3 13 <Trigger Interval> <Fixed Interval>
		# Trigger Interval field is in units of usecs
		# Fixed Interval is set to 1 to force no repetitions
		ap_trig_interval=$((ap_trig_interval * 1000 ))
		iw $CURRENT_WLAN_NAME iwlwav sDoSimpleCLI 3 13 $ap_trig_interval 1

		if [ "$ap_trig_interval" != "10000" ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_number_of_phase_repetitions_idx}=0
		fi
		refresh_static_plan
	fi

	if [ "$ap_trig_cominfo_ullength" != "" ]
	then
		# for test HE-5.61.1
		if [ "$ap_trig_cominfo_ullength" = "601" ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${rcr_com_tf_length_idx}=610
		# for test HE-5.64.1
		elif [ "$ap_trig_cominfo_ullength" = "2251" ]; then
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${rcr_com_tf_length_idx}=2254
		else
			# regular case, not '601' nor '2251'
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${rcr_com_tf_length_idx}=$ap_trig_cominfo_ullength
		fi
		ap_mu_tf_len=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$rcr_com_tf_length_idx`
		refresh_static_plan
	fi
	
	# for tests HE-5.59.2 & HE-5.61.1
	if [ "$ap_trig_usrinfo_ul_mcs" != "" ]; then
		for usr_index in 1 2 3 4
		do
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			ap_mcs_nss=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x3`
			ap_nss=$(((ap_mcs_nss)/16+1))
			ap_trig_nss_mcs_val=$(((ap_trig_ul_nss*16)+ap_trig_usrinfo_ul_mcs))
			if [ "$global_ap_mimo" != "ul" ]; then
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ul_psdu_rate_per_usp_idx=${ap_trig_mu_nss_mcs_val}
			fi
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_psdu_rate_per_usp_idx==${ap_trig_mu_nss_mcs_val}
		done
		refresh_static_plan

	fi

	#Preamble Puncturing
	if [ "$global_ap_preamblepunctmode" != "" ] && [ "$global_ap_preamblepunctchannel" != "" ]; then
		local dl_sub_band1 ul_sub_band1
		local dl_sub_band2 ul_sub_band2
		local dl_sub_band3 ul_sub_band3
		local ap_preamblepunc_sequence_type=1 #HE_MU_SEQ_VHT_LIKE
		if [ "$global_ap_preamblepunctmode" = "4" ] && ([ "$global_ap_preamblepunctchannel" = "40" ] || [ "$global_ap_preamblepunctchannel" = "33" ]); then
			debug_print "Set preamble puncture override for Mode 4"
			if [ "$global_ap_preamblepunctchannel" = "40" ]; then
				ap_tmp=`eval iw dev $CURRENT_WLAN_NAME iwlwav sPreamPunCcaOvr 1 2 1`
				dl_sub_band1=0;ul_sub_band1=0
				dl_sub_band2=2;ul_sub_band2=2
				dl_sub_band3=3;ul_sub_band3=3
			else
				#Puncture map TCRs and DL/UL sub-band are in the order of frequency
				#Channels:33-37(Beaconing)-41-45 in 6GHz would mean DL sub-band for Ch37 is 1 and not 0
				ap_tmp=`eval iw dev $CURRENT_WLAN_NAME iwlwav sPreamPunCcaOvr 1 1 1`
				dl_sub_band1=1;ul_sub_band1=1
				dl_sub_band2=2;ul_sub_band2=2
				dl_sub_band3=3;ul_sub_band3=3
			fi
		elif [ "$global_ap_preamblepunctmode" = "5" ] && ([ "$global_ap_preamblepunctchannel" = "44" ] || [ "$global_ap_preamblepunctchannel" = "41" ]); then
			debug_print "Set preamble puncture override for Mode 5"
			ap_tmp=`eval iw dev $CURRENT_WLAN_NAME iwlwav sPreamPunCcaOvr 1 4 1`
			dl_sub_band1=0;ul_sub_band1=0
			dl_sub_band2=1;ul_sub_band2=1
			dl_sub_band3=3;ul_sub_band3=3
		elif [ "$global_ap_preamblepunctmode" = "5" ] && ([ "$global_ap_preamblepunctchannel" = "48" ] || [ "$global_ap_preamblepunctchannel" = "45" ]); then
			debug_print "Set preamble puncture override for Mode 5"
			ap_tmp=`eval iw dev $CURRENT_WLAN_NAME iwlwav sPreamPunCcaOvr 1 8 1`
			dl_sub_band1=0;ul_sub_band1=0
			dl_sub_band2=1;ul_sub_band2=1
			dl_sub_band3=2;ul_sub_band3=2
		fi
		for usr_index in 1 2 3
		do
			local tmp_param tmp_val
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			tmp_param="dl_sub_band$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${usr_index} x${dl_usr_sub_band_per_usp_idx}=${tmp_val}
			tmp_param="ul_sub_band$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${usr_index} x${rcr_tf_usr_sub_band_idx}=${tmp_val}
		done
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${sequence_type_idx}=${ap_preamblepunc_sequence_type}
		refresh_static_plan
	fi

	# JIRA WLANRTSYS-9307: in case "TRIGGERTYPE" was set, activate the SMD ap_trigger_type
	if [ "$is_activate_sigmaManagerDaemon" = "1" ]; then
		get_nof_sta_per_he_test_case $glob_ssid
		ap_num_users_ofdma=$nof_sta
		arguments_file_initializer $CURRENT_RADIO_UCI_PATH $glob_ssid
		kill_sigmaManagerDaemon
		if [ "$ap_num_users_ofdma" = "0" ]; then
			ap_num_users_ofdma=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station`
		fi
		/lib/netifd/sigmaManagerDaemon.sh $CURRENT_WLAN_NAME.0 $ap_num_users_ofdma &
		is_activate_sigmaManagerDaemon=0
	fi
	
	# Set trigger coding (LDPC/BCC) for UL
	if [ "$glob_ap_trigger_coding" != "" ]; then
		debug_print "glob_ap_trigger_coding:$glob_ap_trigger_coding"
		
		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH
		info_print "ap_bcc_ldpc_int: BCC or LDPC"
		if [ "$glob_ap_trigger_coding" = "BCC" ]; then
			ap_bcc_ldpc_int=0
		elif [ "$glob_ap_trigger_coding" = "LDPC" ]; then
			ap_bcc_ldpc_int=1
		fi
		
		for usr_index in 1 2 3 4
		do
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'${usr_index} $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			Dynamic_set_get_helper iw_off_helper $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${usr_index} x$rcr_tf_usr_ldpc_idx=$ap_bcc_ldpc_int
		done
		
		connected_stations=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station`
		if [ "$connected_stations" = "1" ] || [ "$connected_stations" = "2" ] || [ "$connected_stations" = "4" ]; then
			send_plan_for_${connected_stations}_users ${CURRENT_WLAN_NAME} $connected_stations $CURRENT_RADIO_UCI_PATH $glob_ssid $CURRENT_WLAN_NAME.0
		fi

	fi

	if [ "$ap_cominfo_bw" != "" ]; then
		debug_print "ap_cominfo_bw:$ap_cominfo_bw"
		info_print "Plan OFF send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH"
		send_plan_off ${CURRENT_WLAN_NAME/.0} $CURRENT_RADIO_UCI_PATH

		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${txop_com_start_bw_limit_idx}=${ap_cominfo_bw}

		connected_stations=`iw dev $CURRENT_WLAN_NAME.0 station dump | grep -c Station`
		if [ "$connected_stations" = "1" ] || [ "$connected_stations" = "2" ] || [ "$connected_stations" = "4" ]; then
			send_plan_for_${connected_stations}_users ${CURRENT_WLAN_NAME} $connected_stations $CURRENT_RADIO_UCI_PATH $glob_ssid $CURRENT_WLAN_NAME.0
		fi
	fi
	
	if [ "$ap_usrinfo_ss_alloc_ra_ru" != "" ]; then
		local ap_sta_aid=$glob_ap_sta_aid
		debug_print "ap_sta_aid:$ap_sta_aid ap_usrinfo_ss_alloc_ra_ru:$ap_usrinfo_ss_alloc_ra_ru"
		local AID_SS_FILE="/tmp/sigma-aid-ss-conf"
		local AID_SS_FILE_SORTED="/tmp/sigma-aid-ss-conf-sort"
		[ -e $AID_SS_FILE ] && rm $AID_SS_FILE && rm $AID_SS_FILE_SORTED
		local i=0
		while [ $i -lt 4 ]
		do
			echo ${ap_usrinfo_ss_alloc_ra_ru:i:1},${ap_sta_aid:i:1} >> $AID_SS_FILE
			i=$((i+1))
		done 
		
		sort -r $AID_SS_FILE > $AID_SS_FILE_SORTED
		local line param index sta_aid_val
		# update all users according to the AID_SS_FILE_SORTED

		index=0
		while read -r line || [[ -n "$line" ]]
		do
			# 3 params per line: ss_alloc_ra_ru,sta_aid,ru_alloc
			let index=index+1
			info_print "line=$line"
			ap_usrinfo_ss_alloc_ra_ru=${line%%,*}
			line="${line//$ap_usrinfo_ss_alloc_ra_ru,/""}"
			# update the DB with ss_alloc_ra_ru
			# get MU UL MCS value
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			ap_mcs=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x3`
			let ap_mcs="$ap_mcs%16"
			# calculate the OFDMA MU NSS-MCS value (NSS: bits 6-4, MCS: bits 3-0)
			let ap_ofdma_mu_nss_mcs_val="($ap_usrinfo_ss_alloc_ra_ru)*16+$ap_mcs"
			# set MU UL NSS MCS value
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'${index} $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			if [ "$global_ap_mimo" != "ul" ]; then
				Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${dl_usr_ul_psdu_rate_per_usp_idx}=$ap_ofdma_mu_nss_mcs_val
			fi
			ap_sta_aid=${line}
			# update the DB with AID
			[ "$ap_sta_aid" -gt "0" ] && let sta_aid_val=$ap_sta_aid-1
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${dl_usr_usp_station_indexes_idx}=${sta_aid_val}

		done < $AID_SS_FILE_SORTED

		refresh_static_plan
	fi
	
	if [ "$ap_cominfo_gi_ltf" != "" ]; then
		debug_print "ap_cominfo_gi_ltf:$ap_cominfo_gi_ltf"
		if [ "$ap_cominfo_gi_ltf" = "1" ]; then
			ap_mu_ul_com_he_cp=1
			ap_mu_ul_com_he_ltf=1
			ap_mu_ul_com_he_tf_cp_and_ltf=1
			ap_mu_tf_len=`sp_set_plan_tf_length $CURRENT_WLAN_NAME.0 3094`
			if [ "$global_ap_mimo" = "ul" ]; then
				ap_mu_tf_len=`sp_set_plan_tf_length $CURRENT_WLAN_NAME.0 208`
				ap_num_rep=3
			fi
		elif [ "$ap_cominfo_gi_ltf" = "2" ]; then
			ap_mu_ul_com_he_cp=2
			ap_mu_ul_com_he_ltf=2
			ap_mu_ul_com_he_tf_cp_and_ltf=2
			ap_mu_tf_len=2914
			if [ "$global_ap_mimo" = "ul" ]; then
				ap_mu_tf_len=`sp_set_plan_tf_length $CURRENT_WLAN_NAME.0 202`
				ap_num_rep=3
			fi
		elif [ "$ap_cominfo_gi_ltf" = "0" ]; then
			ap_mu_ul_com_he_cp=1
			ap_mu_ul_com_he_ltf=0
			ap_mu_ul_com_he_tf_cp_and_ltf=0
			#ap_mu_tf_len=? TBD
		else
			# all other LTF and GI combinations are not required by WFA
			error_print "Unsupported value - ap_ltf:$ap_ltf ap_gi:$ap_gi"
			send_invalid ",errorCode,482"
			return
		fi
		debug_print "ap_mu_ul_com_he_cp:$ap_mu_ul_com_he_cp ap_mu_ul_com_he_ltf:$ap_mu_ul_com_he_ltf ap_mu_ul_com_he_tf_cp_and_ltf:$ap_mu_ul_com_he_tf_cp_and_ltf ap_mu_tf_len:$ap_mu_tf_len"
		[ "$ap_num_rep" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_number_of_phase_repetitions_idx}=${ap_num_rep}
		[ "$ap_mu_ul_com_he_cp" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$ul_com_he_cp_idx=${ap_mu_ul_com_he_cp}
		[ "$ap_mu_ul_com_he_ltf" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$ul_com_he_ltf_idx=${ap_mu_ul_com_he_ltf}
		[ "$ap_mu_ul_com_he_tf_cp_and_ltf" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$rcr_com_tf_hegi_and_ltf_idx=${ap_mu_ul_com_he_tf_cp_and_ltf}
		[ "$ap_mu_tf_len" != "" ] && Dynamic_set_get_helper "iw_off" $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common $rcr_com_tf_length_idx=${ap_mu_tf_len}

		refresh_static_plan
	fi
	
		# handle RU allocation for UL
	if [ "$glob_ap_usrinfo_ru_alloc" != "" ]; then
		debug_print "glob_ap_usrinfo_ru_alloc:$glob_ap_usrinfo_ru_alloc"
		# replace all ':' with " "
		tmp_ap_usrinfo_ru_alloc=${glob_ap_usrinfo_ru_alloc//:/ }

		local user_index user_list index user_value start_bw_limit
		local ul_sub_band1 ul_start_ru1 ul_ru_size1
		local ul_sub_band2 ul_start_ru2 ul_ru_size2
		local ul_sub_band3 ul_start_ru3 ul_ru_size3
		local ul_sub_band4 ul_start_ru4 ul_ru_size4

		if [ "$ap_cominfo_bw" != "" ]; then
			# if exist, get the bw from previous parameter in this command
			start_bw_limit=$ap_cominfo_bw
		else
			# else, get the bw from the SP
			ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
			start_bw_limit=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$txop_com_start_bw_limit_idx` #x3
		fi
		user_index=0
		for user_value in $tmp_ap_usrinfo_ru_alloc
		do
			let user_index=user_index+1

			### BW=160MHz ###
			if [ "$start_bw_limit" = "3" ]; then
				info_print "RU allocation for 160MHz UL: currently values are same as DL need to be updated"
				if [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
						;;
						"2") #USER2
							ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=2
						;;
						"3") #USER3
							ul_sub_band3=4;ul_start_ru3=0;ul_ru_size3=2
						;;
						"4") #USER4
							ul_sub_band4=6;ul_start_ru4=0;ul_ru_size4=2
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
						;;
						"2") #USER2
							ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=3
						;;
						"3") #USER3
							ul_sub_band3=4;ul_start_ru3=0;ul_ru_size3=3
						;;
						"4") #USER4
							ul_sub_band4=6;ul_start_ru4=0;ul_ru_size4=3
						;;
					esac
				elif [ "$user_value" = "484" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4
						;;
						"2") #USER2
							ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=4
						;;
						"3") #USER3 - not supported
							ul_sub_band3=4;ul_start_ru3=0;ul_ru_size3=4
						;;
						"4") #USER4 - not supported
							ul_sub_band4=6;ul_start_ru4=0;ul_ru_size4=4
						;;
					esac
				elif [ "$user_value" = "996" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=5
						;;
						"2") #USER2
							ul_sub_band2=4;ul_start_ru2=0;ul_ru_size2=5
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,655"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,660"
							return
						;;
					esac
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,665"
					return
				fi

			### BW=80MHz ###
			elif [ "$start_bw_limit" = "2" ]; then
				if [ "$user_value" = "26" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=0
						;;
						"2") #USER2
							ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=0
						;;
						"3") #USER3
							ul_sub_band3=2;ul_start_ru3=0;ul_ru_size3=0
						;;
						"4") #USER4
							ul_sub_band4=3;ul_start_ru4=0;ul_ru_size4=0
						;;
					esac
				elif [ "$user_value" = "52" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=1
						;;
						"2") #USER2
							ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=1
						;;
						"3") #USER3
							ul_sub_band3=2;ul_start_ru3=0;ul_ru_size3=1
						;;
						"4") #USER4
							ul_sub_band4=3;ul_start_ru4=0;ul_ru_size4=1
						;;
					esac
				elif [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
						;;
						"2") #USER2
							ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=2
						;;
						"3") #USER3
							ul_sub_band3=2;ul_start_ru3=0;ul_ru_size3=2
						;;
						"4") #USER4
							ul_sub_band4=3;ul_start_ru4=0;ul_ru_size4=2
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
						;;
						"2") #USER2
							ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=3
						;;
						"3") #USER3
							ul_sub_band3=2;ul_start_ru3=0;ul_ru_size3=3
						;;
						"4") #USER4
							ul_sub_band4=3;ul_start_ru4=0;ul_ru_size4=3
						;;
					esac
				elif [ "$user_value" = "484" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4
						;;
						"2") #USER2
							ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=4
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,481"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,482"
							return
						;;
					esac
				elif [ "$user_value" = "996" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=5
						;;
						"2") #USER2
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,483"
							return
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,484"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,485"
							return
						;;
					esac
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,486"
					return
				fi

			### BW=40MHz ###
			elif [ "$start_bw_limit" = "1" ]; then
				if [ "$user_value" = "26" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=0
						;;
						"2") #USER2
							ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=0
						;;
						"3") #USER3
							ul_sub_band3=1;ul_start_ru3=0;ul_ru_size3=0
						;;
						"4") #USER4
							ul_sub_band4=1;ul_start_ru4=5;ul_ru_size4=0
						;;
					esac
				elif [ "$user_value" = "52" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=1
						;;
						"2") #USER2
							ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=1
						;;
						"3") #USER3
							ul_sub_band3=1;ul_start_ru3=0;ul_ru_size3=1
						;;
						"4") #USER4
							ul_sub_band4=1;ul_start_ru4=5;ul_ru_size4=1
						;;
					esac
				elif [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
						;;
						"2") #USER2
							ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=2
						;;
						"3") #USER3
							ul_sub_band3=1;ul_start_ru3=0;ul_ru_size3=2
						;;
						"4") #USER4
							ul_sub_band4=1;ul_start_ru4=5;ul_ru_size4=2
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
						;;
						"2") #USER2
							ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=3
						;;
						"3") #USER3 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,487"
							return
						;;
						"4") #USER4 - not supported
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,488"
							return
						;;
					esac
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,489"
					return
				fi

			### BW=20MHz ###
			elif [ "$start_bw_limit" = "0" ]; then
				if [ "$user_value" = "26" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=0
						;;
						"2") #USER2
							ul_sub_band2=0;ul_start_ru2=2;ul_ru_size2=0
						;;
						"3") #USER3
							ul_sub_band3=0;ul_start_ru3=5;ul_ru_size3=0
						;;
						"4") #USER4
							ul_sub_band4=0;ul_start_ru4=7;ul_ru_size4=0
						;;
					esac
				elif [ "$user_value" = "52" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=1
						;;
						"2") #USER2
							ul_sub_band2=0;ul_start_ru2=2;ul_ru_size2=1
						;;
						"3") #USER3
							ul_sub_band3=0;ul_start_ru3=5;ul_ru_size3=1
						;;
						"4") #USER4
							ul_sub_band4=0;ul_start_ru4=7;ul_ru_size4=1
						;;
					esac
				elif [ "$user_value" = "106" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
						;;
						"2") #USER2
							ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=2
						;;
						"3") #USER3
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,490"
							return
						;;
						"4") #USER4
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,491"
							return
						;;
					esac
				elif [ "$user_value" = "242" ]; then
					case "$user_index" in
						"1") #USER1
							ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
						;;
						"2") #USER2
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,492"
							return
						;;
						"3") #USER3
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,493"
							return
						;;
						"4") #USER4
							error_print "cannot set user${user_index} with ru=${user_value}"
							send_invalid ",errorCode,494"
							return
						;;
					esac
				elif [ "$user_value" = "484" ]; then
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,495"
					return
				else
					error_print "cannot set user${user_index} with ru=${user_value}"
					send_invalid ",errorCode,496"
					return
				fi
			else
				error_print "Unsupported value - start_bw_limit:$start_bw_limit"
				send_invalid ",errorCode,497"
				return
			fi
		done

		# user_index contains the number of users. set it to DB to be used by static plan.
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_num_of_participating_stations_idx}=${user_index}

		# update per-user params in DB, per number of users
		#for index in $user_index
		for index in 1 2 3 4 
		do
			local tmp_param tmp_val
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			
			tmp_param="ul_sub_band${index}";eval tmp_val=\$$tmp_param		
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${rcr_tf_usr_sub_band_idx}=${tmp_val}
			
			tmp_param="ul_start_ru${index}";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${rcr_tf_usr_start_ru_idx}=${tmp_val}
			
			tmp_param="ul_ru_size${index}";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${index} x${rcr_tf_usr_ru_size_idx}=${tmp_val}
		done

		refresh_static_plan
	fi



	send_complete
}

dev_send_frame()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				ap_name=$1
			;;
			INTERFACE)
				# skip as it is determined in get_interface_details
			;;
			WLAN_TAG)
				# skip as it is determined in get_interface_details
			;;
			DEST_MAC)
				ap_dest_mac=$1
			;;
			PROGRAM)
				# do nothing
			;;
			FRAMENAME)
				ap_frame_name=$1
			;;
			CAND_LIST)
				ap_cand_list=$1
			;;
			BTMQUERY_REASON_CODE)
				ap_btmquery_reason_code=$1
			;;
			DISASSOC_TIMER)
				ap_disassoc_timer=$1
			;;
			MEAMODE)
				lower $1 ap_meamode
			;;
			REGCLASS)
				ap_regclass=$1
			;;
			CHANNEL)
				ap_channel=$1
			;;
			RANDINT)
				ap_randint=$1
			;;
			MEADUR)
				ap_meadur=$1
			;;
			SSID)
				ap_ssid=$1
			;;
			RPTCOND)
				ap_rptcond=$1
			;;
			RPTDET)
				ap_rpt_det=$1
			;;
			MEADURMAND)
				ap_meadurmand=$1
			;;
			APCHANRPT)
				ap_apchanrpt=$1
			;;
			REQINFO)
				ap_reqinfo=$1
			;;
			REQUEST_MODE)
			lower "$1" ap_request_mode
			if [ "$ap_request_mode" = "disassoc" ]; then
				BTM_DISASSOC_IMMITIENT=1
			fi
			;;
			LASTBEACONRPTINDICATION)
				ap_lastbeaconrptindication=$1
			;;
			BSSID)
				ap_bssid=$1
			;;
			INTERVAL)
				frame_duration=$1
			;;
			PROTECTED)
				if [ "$1" = "Unprotected" ]; then
					frame_protected=0
				elif [ "$1" = "CorrectKey" ]; then
					frame_protected=1
					frame_key=1
				elif [ "$1" = "IncorrectKey" ]; then
					frame_protected=1
					frame_key=0
				fi
			;;
			STATIONID)
				station_id=$1
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,500"
				return
			;;
		esac
		shift
	done

	case "$ap_frame_name" in
		BcnRptReq)
			ap_beacon_req_params=""

			# ap_dest_mac + num_of_repetitions + measurement_request_mode
			ap_beacon_req_params=$ap_beacon_req_params" $ap_dest_mac 0 0"

			if [ "$ap_regclass" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" $ap_regclass"
			else
				ap_curr_band=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.band`
				if [ "$ap_curr_band" = "5GHz" ]; then
					ap_beacon_req_params=$ap_beacon_req_params" 115"
				else
					ap_beacon_req_params=$ap_beacon_req_params" 81"
				fi
			fi

			if [ "$ap_channel" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" $ap_channel"
			else
				ap_beacon_req_params=$ap_beacon_req_params" 255"
			fi

			if [ "$ap_randint" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" $ap_randint"
			else
				ap_beacon_req_params=$ap_beacon_req_params" 0"
			fi

			if [ "$ap_meadur" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" $ap_meadur"
			else
				ap_beacon_req_params=$ap_beacon_req_params" 20"
			fi

			if [ "$ap_meamode" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" $ap_meamode"
			else
				ap_beacon_req_params=$ap_beacon_req_params" active"
			fi

			if [ "$ap_bssid" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" $ap_bssid"
			else
				ap_beacon_req_params=$ap_beacon_req_params" ff:ff:ff:ff:ff:ff"
			fi

			# the remaining parameters are optional
			if [ "$ap_ssid" != "" -a "$ap_ssid" != "ZeroLength" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" ssid='\"$ap_ssid\"'"
			fi

			if [ "$ap_rpt_det" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" rep_detail=$ap_rpt_det"
			fi

			if [ "$ap_rptcond" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" rep_cond=$ap_rptcond"
			fi

			if [ "$ap_lastbeaconrptindication" != "" ]; then
				ap_beacon_req_params=$ap_beacon_req_params" last_indication=$ap_lastbeaconrptindication"
			fi

			if [ "$ap_apchanrpt" != "" ]; then
				# replace all "_" with "," in received string
				ap_apchanrpt_param="${ap_apchanrpt//_/,}"
				ap_beacon_req_params=$ap_beacon_req_params" ap_ch_report=$ap_apchanrpt_param"
			fi

			if [ "$ap_reqinfo" != "" ]; then
				# replace all "_" with "," in received string
				ap_reqinfo_param="${ap_reqinfo//_/,}"
				ap_beacon_req_params=$ap_beacon_req_params" req_elements=$ap_reqinfo_param"
			fi

			ap_all_ifaces=`uci show wireless | grep ifname | awk -F"=" '{print $2}' | awk -v RS=  '{$1=$1}1' | tr -d "'"`
			IFS=$ORIG_IFS
			for ap_curr_iface in $ap_all_ifaces; do
				ap_res=`iw dev $ap_curr_iface station dump | grep $ap_dest_mac`
				if [ "$ap_res" != "" ]; then
					ap_send_iface=$ap_curr_iface
					break
				fi
			done

			if [ "$ap_send_iface" != "" ]; then
				CURRENT_IFACE_IFNAME=$ap_send_iface
			fi

			debug_print "$HOSTAPD_CLI_CMD -i$CURRENT_IFACE_IFNAME REQ_BEACON $ap_beacon_req_params"
			ap_tmp=`eval $HOSTAPD_CLI_CMD -i$CURRENT_IFACE_IFNAME REQ_BEACON "$ap_beacon_req_params"`
		;;
		BTMReq)
			debug_print BTMReq
			sleep 1 #WA if WNM notify received late
			ap_cmd="$HOSTAPD_CLI_CMD -i$CURRENT_IFACE_IFNAME BSS_TM_REQ $ap_dest_mac"

			if [ "$ap_cand_list" != "" ]; then
				ap_cmd="$ap_cmd pref=$ap_cand_list"
			fi

			CURRENT_NEIGHBORS=`cat /tmp/mbo_neighbors`
			if [ "$CURRENT_NEIGHBORS" != "" ]; then
				ap_cmd="$ap_cmd $CURRENT_NEIGHBORS"
			elif [ "$ap_cand_list" != "" ]; then
				ap_self_mac=`$UCI_CMD get $CURRENT_IFACE_UCI_PATH.macaddr`
				ap_band=`$UCI_CMD get $CURRENT_RADIO_UCI_PATH.band`
				if [ "$ap_band" = "5GHz" ]; then
					ap_op_class=73
					ap_op_chan=36
				elif [ "$ap_band" = "2.4GHz" ]; then
					ap_op_class=51
					ap_op_chan=1
				elif [ "$ap_band" = "6GHz" ]; then
					ap_op_class=131
					ap_op_chan=37
				fi
				ap_cmd="$ap_cmd neighbor=${ap_self_mac},0,${ap_op_class},${ap_op_chan},9,0"
			fi

			if [ "${BTM_DISASSOC_IMMITIENT}" != "" ]; then
				ap_cmd="$ap_cmd disassoc_imminent=${BTM_DISASSOC_IMMITIENT}"
			fi

			if [ "$ap_disassoc_timer" != "" ]; then
				ap_cmd="$ap_cmd disassoc_timer=$ap_disassoc_timer"
			elif [ "$BTM_DISASSOC_IMMITIENT" = "1" ]; then
				ap_cmd="$ap_cmd disassoc_timer=5"
			fi

			if [ "$BTM_BSS_TERM_BIT" = "1" ]; then
				ap_cmd="$ap_cmd disassoc_timer=$BTM_BSS_TERM_TSF bss_term=${BTM_BSS_TERM_TSF},${BTM_BSS_TERM_DURATION}"
			fi

			if [ "$BTM_REASSOC_DELAY" = "" ]; then
				BTM_REASSOC_DELAY=0
			fi

			ap_cell=`$UCI_CMD get ${CURRENT_IFACE_UCI_PATH}.mbo_cell_aware`
			if [ "$ap_cell" = "1" ]; then
				ap_mbo_cell="1"
			else
				ap_mbo_cell="-1"
			fi

			ap_cmd="$ap_cmd mbo=4:${BTM_REASSOC_DELAY}:${ap_mbo_cell}"
			debug_print "$ap_cmd"

			ap_tmp=`eval "$ap_cmd"`

			if [ "$BTM_BSS_TERM_DURATION" != "" ] && [ "$BTM_BSS_TERM_TSF" != "" ]
			then
				ap_filename="/lib/netifd/terminate_radio_after.sh"
				debug_print "$ap_filename $CURRENT_WLAN_NAME $BTM_BSS_TERM_TSF $BTM_BSS_TERM_DURATION"
				$ap_filename $CURRENT_WLAN_NAME $BTM_BSS_TERM_TSF $BTM_BSS_TERM_DURATION &
			fi

			BTM_REASSOC_DELAY=""

			ap_tmp_chan=`$UCI_CMD get ${CURRENT_RADIO_UCI_PATH}.channel`
			remove_neighbor_for_channel $ap_tmp_chan

		;;
		disassoc)
			debug_print $HOSTAPD_CLI_CMD -i$CURRENT_IFACE_IFNAME DISASSOCIATE $CURRENT_IFACE_IFNAME $ap_dest_mac
			ap_tmp=`eval $HOSTAPD_CLI_CMD -i$CURRENT_IFACE_IFNAME DISASSOCIATE $CURRENT_IFACE_IFNAME $ap_dest_mac`
		;;
		MsntPilot)
			local current_frame
			current_frame=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 get_unsolicited_frame_support`
			ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME set_unsolicited_frame_duration $frame_duration`
			if [ "$current_frame" = "0" ]; then
				ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME set_unsolicited_frame_support 2`
			fi
		;;
		deauth)
			if [ "$frame_protected" = "0" ]; then #unprotected deauth
				ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 set_unprotected_deauth 1`
				ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 DEAUTHENTICATE $ap_wlan_6g_name.0 $station_id test=0`
				ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 set_unprotected_deauth 0`
			else
				if [ "$frame_key" = "1" ]; then #protected with correct key deauth
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 deauthenticate $ap_wlan_6g_name.0 $station_id`
				else #protected with incorrect key deauth
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 set_pn_reset 0`
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 set_incorrect_pn $station_id`
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 deauthenticate $ap_wlan_6g_name.0 $station_id test=0`
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 set_correct_pn $station_id`
					ap_tmp=`eval $HOSTAPD_CLI_CMD -i $CURRENT_WLAN_NAME.0 set_pn_reset 1`
				fi
			fi
		;;
		*)
			error_print "not supported frame_name $ap_frame_name"
			send_invalid ",errorCode,500"
			return
		;;
	esac

	send_complete
}

ap_reboot()
{
	send_running
	reboot
	send_complete
}

check_and_update_dc_registration()
{
	local total_vaps=`cat /proc/ppa/api/netif | grep phys_netif_name | grep -c wlan`
	local dc_enabled=`cat /proc/ppa/api/netif | grep -c NETIF_DIRECTCONNECT`
	local vaps=`cat /proc/ppa/api/netif | grep phys_netif_name | grep  wlan | cut -d = -f2`
	if [ $total_vaps -ne $dc_enabled ]; then
		for vap in $vaps
		do
			local ap_tmp=`eval ppacmd dellan -i $vap`
			ap_tmp=`eval ppacmd addlan -i $vap`
		done
		debug_print "Applied WA for ARP issue"
	fi
}

refresh_unsolicited_frame_support()
{
	interfaces=`ls /var/run/hostapd | cut -d"." -f1 | grep -v global-hostapd`
	interfaces=`echo $interfaces | awk '{for (i=1;i<=NF;i++) if (!a[$i]++) printf("%s%s",$i,FS)}{printf(" ")}'`
	for i in $interfaces
	do
	cf1=`eval $HOSTAPD_CLI_CMD -i$i radio | grep Cf1`
	cf1=`echo $cf1 | cut -d"=" -f2`
	if [ $cf1 -gt 5900 ]; then #center freq for 6Ghz must be above 59**
		interface_6g=$i
		break
	fi
	done
	cur_frame=`eval $HOSTAPD_CLI_CMD -i $interface_6g get_unsolicited_frame_support`
	ap_tmp=`eval $HOSTAPD_CLI_CMD -i $interface_6g set_unsolicited_frame_support $cur_frame`
}

ap_config_commit()
 {
	send_running

	#In case RADIO off received earlier and interfaces were removed from ifconfig, add it back
	ap_radio_ifaces=`uci show wireless | grep ifname | awk -F"=" '{print $2}'`
	for ap_radio_iface in $ap_radio_ifaces; do
		res=`eval ifconfig $ap_radio_iface up`
	done

	ap_uci_commit_and_apply
	local verify_up=`verify_interfaces_up`
	if [ $verify_up -eq 1 ]
	then
		error_print "Interfaces are not up after commit"
		send_invalid ",errorCode,220"
		return
	fi
	if [ "$OS_NAME" = "UGW" ]; then
		check_and_update_dc_registration
	fi
	if [ "1" = `is_6g_supported` ]; then
		refresh_unsolicited_frame_support
	fi	
	send_complete
}
ap_common_reset_default()
{
	$FACTORY_CMD -p $1
    #CGR interface doesnt restart, forceful restart
    if [ "$OS_NAME" = "RDKB" ]; then
    	ap_tmp=`systemctl restart systemd-netifd.service`
		sleep 15
    fi
	debug_print "clearing all global_ variables"
	local variables=`set | grep "^global_" | cut -d= -f1 | xargs echo `

	for var in $variables; do
		#debug_print "clearing globals $var"
		unset ${var}
	done

	if [ "$MODEL" != "AX11000" ] && [ "$OS_NAME" = "UGW" ]; then
		chown rpcd:rpcd $UCI_DB_PATH/wireless
		chmod +r $UCI_DB_PATH/wireless
	fi

	$UCI_CMD set wireless.default_radio$ap_uci_24g_idx.ssid="$1_ssid_24g"
	$UCI_CMD set wireless.default_radio$ap_uci_5g_idx.ssid="$1_ssid_5g"
	$UCI_CMD set wireless.default_radio$ap_uci_6g_idx.ssid="$1_ssid_6g"
	if [ "$ucc_program" = "he" ]; then
		randomize_macs
	fi

	BASE_TAG_6G=""
	BASE_TAG_5G=""
	BASE_TAG_24G=""
}

ap_mbo_reset_default_var()
{
	CURRENT_NEIGHBORS=""
	CURRENT_NON_PREF_NEIGHBORS=""
	BTM_DISASSOC_IMMITIENT=""
	BTM_REASSOC_DELAY=""
	BTM_BSS_TERM_BIT=""
	BTM_BSS_TERM_DURATION=""

	cat /dev/null > /tmp/mbo_neighbors
	cat /dev/null > /tmp/non_pref_mbo_neighbors
	kill_sigma_mbo_daemon
	/lib/netifd/sigma_mbo_daemon.sh &

}
ap_mbo_reset_default()
{
	ap_common_reset_default mbo
	ap_mbo_reset_default_var
}

ap_he_reset_default()
{
	# HE includes 6G
	ap_common_reset_default he
    ap_mbo_reset_default_var
}


ap_he_testbed_reset_default()
{
	# HE includes 6G
	ap_common_reset_default he_testbed
    ap_mbo_reset_default_var
}

ap_11n_reset_default()
{
	ap_common_reset_default 11n
}

ap_vht_reset_default()
{
	ap_common_reset_default vht

	CONFIGURE_BOTH_BANDS=0
}

ap_wpa2_reset_default()
{
	ap_common_reset_default wpa2
}

ap_wpa3_reset_default()
{
	ap_common_reset_default wpa3
}

ap_pmf_reset_default()
{
	ap_common_reset_default pmf
}

ap_reset_default()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	ucc_program=""
	ucc_type=""

	kill_sigmaManagerDaemon

	while [ "$1" != "" ]; do
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				ap_name=$1
			;;
			PROGRAM)
				lower "$1" ucc_program
			;;
			TYPE)
				lower "$1" ucc_type
			;;
			*)
			;;
		esac
		shift
	done

	info_print "##### PROGRAM:$ucc_program TYPE:$ucc_type AP_NAME:$ap_name #####"

	if [ "$ucc_type" != "" ] && [ "$ucc_type" != "dut" ] && [ "$ucc_type" != "testbed" ]; then
		error_print "Unsupported ucc type - $ucc_type"
		send_invalid ",errorCode,610"
		return
	fi

	if [ "$ucc_program" = "mbo" ]; then
		ap_mbo_reset_default
	elif [ "$ucc_program" = "he" ]; then
		if [ "$ucc_type" = "dut" ]; then
			ap_he_reset_default
		else
			ap_he_testbed_reset_default
		fi
	elif [ "$ucc_program" = "11n" ]; then
		ap_11n_reset_default
	elif [ "$ucc_program" = "vht" ]; then
		ap_vht_reset_default
	elif [ "$ucc_program" = "wpa2" ]; then
		ap_wpa2_reset_default
	elif [ "$ucc_program" = "wpa3" ]; then
		ap_wpa3_reset_default
	elif [ "$ucc_program" = "pmf" ]; then
		ap_pmf_reset_default
	else
		error_print "Unsupported ucc program - $ucc_program"
		send_invalid ",errorCode,610"
		return
	fi

	debug_print "Clearing CURRENT_WLAN_TAG variables"
	unset CURRENT_WLAN_TAG1
	unset CURRENT_WLAN_TAG2
	local verify_up=`verify_interfaces_up`
	if [ $verify_up -eq 1 ]
	then
		error_print "Interfaces are not up after reset_default"
		#Won't harm if we continue
	fi

	#make sure re-enable tx
	iw wlan0 iwlwav sEnableRadio 1
	iw wlan2 iwlwav sEnableRadio 1
	if [ "1" = `is_6g_supported` ]; then
		iw wlan4 iwlwav sEnableRadio 1
	fi

	send_complete
}

ap_get_info()
{
	send_running
	#answer=need to see what info need to be returned
	send_complete "$answer"
}

ap_deauth_sta()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	get_common_uci_path
	get_interface_details $@

	while [ "$1" != "" ]; do
		# for upper case only
		upper "$1" token
		shift
		debug_print "while loop $1 - token:$token"
		case "$token" in
			NAME)
				debug_print "deauthenticate ap_name=$1"
			;;
			INTERFACE)
				# skip since it was read in loop before
			;;
			STA_MAC_ADDRESS)
				debug_print "deauthenticate ap_sta_mac_address=$1"
			;;
			MINORCODE)
				debug_print "deauthenticate ap_minorcode=$1"
			;;
			*)
				error_print "while loop error $1"
				send_invalid ",errorCode,44"
				return
			;;
		esac
		shift
	done

	send_complete
}

ap_get_mac_address()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	ap_non_tx_index=0
	get_common_uci_path
	get_interface_details $@

	ap_mac_index="${CURRENT_IFACE_UCI_PATH/wireless.default_radio}"
	ap_mac_index=$((ap_mac_index+ap_non_tx_index))

	ap_mac=`$UCI_CMD get wireless.default_radio$ap_mac_index.macaddr`
	send_complete ",mac,$ap_mac"
}

ap_get_parameter()
{
        ap_name="Maxlinear"
        IFS=$ORIG_IFS
        send_running
        get_common_uci_path
        while [ "$1" != "" ]; do
                # for upper case only
                upper "$1" token
                shift
                debug_print "while loop $1 - token:$token"
                case "$token" in
                NAME)
                        debug_print "set parameter ap_name=$1"
                ;;
                STA_MAC_ADDRESS)
                        local sta_mac_addr=$1
                        debug_print "set parameter sta_mac_addr=$1"
                        lower "$1" sta_mac_addr
                ;;
                PARAMETER)
                        local param=$1
                        debug_print "set parameter param=$1"
                        lower "$1" param
                        if [ "$param" = "pmk" ]; then
                                pmk=`eval $HOSTAPD_CLI_CMD -i$CURRENT_WLAN_NAME.0 pmk | grep $sta_mac_addr | cut -d" " -f3`
                                debug_print $pmk
                        fi
                ;;
                esac
                shift
        done
        if [ -n "$pmk" ]; then
                send_complete ",pmk,$pmk"
        else
                send_error ",errorCode,120"
        fi
}
dev_configure_ie()
{
        ap_name="Maxlinear"
        IFS=$ORIG_IFS
        send_running
        get_common_uci_path
        while [ "$1" != "" ]; do
                # for upper case only
                upper "$1" token
                shift
                debug_print "while loop $1 - token:$token"
                case "$token" in
                INTERFACE)
                        debug_print "interface:$1"
                ;;
                IE_NAME)
                        debug_print "IE_name:$1"
                ;;
                CONTENTS)
                        debug_print "content:$1"
                        #get debug max index, then create next debug_hostapd_conf_index
                        wpa_idx=`uci show wireless | grep debug_hostap_conf | cut -d"=" -f 1 | cut -d"_" -f4 | tail -1`
                        let "wpa_idx=wpa_idx+1"
                        eval uci set wireless.default_radio$ap_uci_6g_idx.debug_hostap_conf_$wpa_idx='wpa_hex_buf=$1'

                ;;
                esac
                shift
        done
        send_complete
}
device_get_info()
{
	ap_name="Maxlinear"
	IFS=$ORIG_IFS
	send_running

	# configure following values: vendor, model name, FW version
	send_complete ",vendor,$VENDOR,model,$MODEL,version,$WAVE_VERSION"
}

##### Parser #####

parse_command()
{
	#echo parsing,`eval $TIMESTAMP`
	lower "$1" cmd
	shift

	debug_print "running command: >>>$cmd<<<"
	$cmd "$@"
	local res=$?
	debug_print "result: $res"
	if [ $res != "0" ]; then
		send_invalid ",errorCode,46"
		error_print "Unknown command: >>>$cmd<<<"
		error_print "Supported commands:"
		error_print "ap_ca_version, ca_get_version, ap_set_wireless, ap_set_11n_wireless, ap_set_security"
		error_print "ap_set_pmf, ap_set_statqos, ap_set_radius, ap_set_hs2, ap_reboot, ap_config_commit,"
		error_print "ap_reset_default, ap_get_info, ap_deauth_sta, ap_get_mac_address, ap_set_rfeature"
		error_print "ap_send_addba_req, dev_send_frame, device_get_info ap_get_parameter"
	fi
	cmd=""
	return
}

############################################################## Planner and its functions ##############################################################
# Info is saved to a file and the file is sourced to be used.
sp_check_ldpc_support()
{
	local interface_name HOSTAPD_CLI_ALL_STA_FILE LDPC_SUPPORT_FILE \
	current_aid line ldpc

	interface_name=$1

	# Check for each STA if it supports LDPC or not and save to a file
	HOSTAPD_CLI_ALL_STA_FILE="/tmp/sigma-hostapd-cli-all-sta-conf"
	LDPC_SUPPORT_FILE="/tmp/sigma-ldpc-support-conf"
	rm -f $HOSTAPD_CLI_ALL_STA_FILE $LDPC_SUPPORT_FILE
	eval $HOSTAPD_CLI_CMD -i${interface_name} all_sta $interface_name > $HOSTAPD_CLI_ALL_STA_FILE
	while [ $? -ne 0 ]; do
		sleep 3
		eval $HOSTAPD_CLI_CMD -i${interface_name} all_sta $interface_name > $HOSTAPD_CLI_ALL_STA_FILE
	done 
	current_aid=0
	while read -r line || [[ -n "$line" ]]
	do
		# Find the aid of the current block
		if [ "${line##aid=}" != "$line" ]; then
			current_aid=${line##aid=}
			# The LDPC support bit appears in he_phy line
		elif [ "${line##he_phy=}" != "$line" ]; then
			ldpc=`echo $line | awk '{print $2}'`
			# bit#6 0x20 means STA supports LDPC
			ldpc="0x$ldpc"
			if [ "$((ldpc & 0x20))" != "0" ]; then
				ldpc=1
			else
				ldpc=0
			fi
			echo "ldpc_${current_aid}=${ldpc}" >> $LDPC_SUPPORT_FILE
		fi
	done < $HOSTAPD_CLI_ALL_STA_FILE

	# Save ldpc support information to be set
	[ -e $LDPC_SUPPORT_FILE ] && source $LDPC_SUPPORT_FILE
	[ "$DEBUGGING" = "1" ] && [ -e $LDPC_SUPPORT_FILE ] && cat $LDPC_SUPPORT_FILE > /dev/console
}

get_max_nss()
{
	local interface_name MAX_NSS_FILE max_nss nss

	interface_name=$1

	MAX_NSS_FILE="/tmp/sigma-max-nss-file"
	rm -f $MAX_NSS_FILE
	max_nss=-1

	cat /proc/net/mtlk/$interface_name/sta_list | awk '{print $11}' > $MAX_NSS_FILE
	sed -i 's/|//' $MAX_NSS_FILE
	while read -r line || [[ -n "$line" ]]
	do
		[ -z "$line" ] && continue
		[ $line -gt $max_nss ] && max_nss=$line
	done < $MAX_NSS_FILE

	echo "$max_nss"
}

sp_set_plan_tf_length()
{
	local interface_name ap_mu_tf_len current_tf_length max_nss

	interface_name=$1
	ap_mu_tf_len=$2

	if [ -z $ap_mu_tf_len ]; then
		ap_mu_tf_len=2914
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		current_tf_length=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$rcr_com_tf_length_idx`
#		current_tf_length=`get_plan_tf_length $interface_name` #WaveSPRcrComTfLength ==> rcr_com_tf_length
	else
		current_tf_length=$ap_mu_tf_len
	fi

	if [ $current_tf_length -eq 3094 ]; then
		max_nss=`get_max_nss $interface_name`
		case $max_nss in
			1) ap_mu_tf_len=3076 ;;
			2) ap_mu_tf_len=3082 ;;
		esac
	fi
	echo "$ap_mu_tf_len"
}

# get the radio's band
get_radio_band()
{
	local interface_name

	[ -z "$1" ] && error_print "get_sta_highest_bw: ERROR: Missing ifname" && echo -1 && return

	interface_name=$1

	tmp_channel=`cat /proc/net/mtlk/$interface_name/channel | grep primary_channel | awk '{print $2}'`
	if [ $tmp_channel -eq 37 ] || [ $tmp_channel -eq 33 ] || [ $tmp_channel -eq 53 ]; then
		echo "6GHz"
	elif [ $tmp_channel -ge 36 ]; then
		echo "5GHz"
	else
		echo "2.4GHz"

	fi
}

# get the highest (last) station's BW
get_sta_highest_bw()
{
	local interface_name ap_client_mac sta_bw list_of_sta_bw

	[ -z "$1" ] && error_print "get_sta_highest_bw: ERROR: Missing ifname and sta mac address" && echo -1 && return
	[ -z "$2" ] && error_print "get_sta_highest_bw: ERROR: Missing sta mac address" && echo -1 && return

	interface_name=$1
	ap_client_mac=$2

	list_of_sta_bw=`eval $HOSTAPD_CLI_CMD -i${interface_name} get_he_phy_channel_width_set $ap_client_mac`
	# "he_phy_chanwidth_set=20MHz,40MHz,80MHz,160MHz" or only part of these rates will the answer.
	# in case of a non-HE station, the response will be: "CTRL: GET_HE_PHY_CHANNEL_WIDTH_SET - STA doesn't have HE elements in assoc req"

	if [ "${list_of_sta_bw/,}" != "${list_of_sta_bw}" ]; then
		sta_bw=${list_of_sta_bw##*,}
	else
		sta_bw=${list_of_sta_bw##*=}
	fi

	echo $sta_bw
}

# get the indication whether to lower ru-size or not - WLANRTSYS-9745
is_set_low_ru_size_get()
{
	local interface_name ru_size aid_index band ap_client_mac sta_aid sta_bw field

	[ -z "$1" ] && error_print "is_set_low_ru_size_get: ERROR: Missing ifname, ru_size and aid_index" && echo -1 && return
	[ -z "$2" ] && error_print "is_set_low_ru_size_get: ERROR: Missing ru_size and aid_index" && echo -1 && return
	[ -z "$3" ] && error_print "is_set_low_ru_size_get: ERROR: Missing aid_index" && echo -1 && return

	interface_name=$1
	ru_size=$2
	aid_index=$3

	if [ "$ru_size" != "3" ]; then
		echo "0"
		return
	fi

	band=`get_radio_band $interface_name`

	while read -r line || [[ -n "$line" ]]
	do
		# display $line or do something with $line
		field=`echo $line | awk '{print $1}' | tr  "\n"`
		ap_client_mac=`echo $field | grep ":"`
		if [ -n "$ap_client_mac" ]; then

		local IFS_orig=$IFS
		IFS="'|'"
		sta_aid=${line//"'"/}
		sta_aid=`echo $sta_aid | awk '{print $2}'`
		IFS=$ORIG_IFS

			if [ "$sta_aid" != "" ] && [ $sta_aid -gt 0 ]; then
				if [ "$sta_aid" = "$aid_index" ]; then
					sta_bw=`get_sta_highest_bw $interface_name $ap_client_mac`
					if [ "$sta_bw" = "20MHz" ]; then
						# set the nss with the highest value; this way it will be the first one after sorting all stations
						echo "'$ap_client_mac' is a 20MHz station" > /dev/console

						sta_he_caps=`$HOSTAPD_CLI_CMD -i $interface_name get_sta_he_caps $ap_client_mac`

						if [ "$band" = "2.4GHz" ]; then
							if [ "${sta_he_caps//B4/}" = "$sta_he_caps" ]; then  # 'B4' is NOT present
								echo "1"
								return
							else
								# 'B4' is present - check if he_phy_20_mhz_in_160_slash_80_plus_80_mhz_he_ppdu != 1
								sta_he_caps=${sta_he_caps##*he_phy_20_mhz_in_40_mhz_he_ppdu_in_24_ghz_band=}
								sta_he_caps=${sta_he_caps%%he*}
								sta_he_caps=`echo $sta_he_caps`
								if [ "$sta_he_caps" != "1" ]; then
									echo "1"
									return
								fi
							fi
						elif [ "$band" = "5GHz" ]; then
							if [ "${sta_he_caps//B5/}" = "$sta_he_caps" ]; then  # 'B5' is NOT present
								echo "1"
								return
							else
								# 'B5' is present - check if he_phy_20_mhz_in_160_slash_80_plus_80_mhz_he_ppdu != 1
								sta_he_caps=${sta_he_caps##*he_phy_20_mhz_in_160_slash_80_plus_80_mhz_he_ppdu=}
								sta_he_caps=`echo $sta_he_caps`
								if [ "$sta_he_caps" != "1" ]; then
									echo "0"
									return
								fi
							fi
						fi
					fi
				fi
			fi
		fi
	done < /proc/net/mtlk/$interface_name/sta_list

	echo "0"
}

# get the sorted station AID index according to nss descending order (high to low)
get_sta_aid_idx_sorted_list()
{
	local interface_name field sta_bw
	local ap_client_mac aid_index sta_nss sta_index_sorted_list
	local SMD_AID_SS_FILE="/tmp/sigma-smd-aid-ss-conf"
	local SMD_AID_SS_FILE_SORTED="/tmp/sigma-smd-aid-ss-conf-sorted"

	interface_name=$1
	[ -z "$1" ] && error_print "get_sta_aid_idx_sorted_list: ERROR: Missing interface_name" && echo -1 && return

	rm -f $SMD_AID_SS_FILE $SMD_AID_SS_FILE_SORTED

	while read -r line || [[ -n "$line" ]]
	do
		# display $line or do something with $line
		field=`echo $line | awk '{print $1}' | tr  "\n"`
		ap_client_mac=`echo $field | grep ":"`
		if [ -n "$ap_client_mac" ]; then
			aid_index=`echo $line | cut -d'|' -f 2`
			aid_index="$(echo -e "${aid_index}" | tr -d '[:space:]')"
			sta_nss=`echo $line | cut -d'|' -f 6`

			# remove all blanks (before and after the digits)
			sta_nss="$(echo -e "${sta_nss}" | tr -d '[:space:]')"

			# add 100 just for the sort to work fine
			sta_nss=$((sta_nss+100))

			# here check the station's maximum band width, and mark it to be the 1st one at the sorted list 

			sta_bw=`get_sta_highest_bw $interface_name $ap_client_mac`

			if [ "$sta_bw" = "20MHz" ]; then
				# set the nss with the highest value; this way it will be the first one after sorting all stations
				sta_nss=$((sta_nss+100))
				echo "'$ap_client_mac' is a 20MHz station" > /dev/console
			fi

			echo "$sta_nss,$aid_index,$ap_client_mac" >> $SMD_AID_SS_FILE
		fi
	done < /proc/net/mtlk/$interface_name/sta_list

	sort -r $SMD_AID_SS_FILE > $SMD_AID_SS_FILE_SORTED

	## update all users according to the AID_SS_FILE_SORTED higher to lower ss.
	sta_index_sorted_list=""
	while read -r line || [[ -n "$line" ]]
	do
		## 2 params in line : nss,aid_index
		aid_index=`echo "${line}" | cut -d',' -f 2`
		if [ -z "$sta_index_sorted_list" ]; then
			sta_index_sorted_list="$aid_index"
		else
			sta_index_sorted_list="$sta_index_sorted_list,$aid_index"
		fi
	done <  $SMD_AID_SS_FILE_SORTED

	echo "$sta_index_sorted_list"
}

aid_idx_out_of_list_get()
{
	local count=0 aid_idx

	[ -z "$1" ] && error_print "aid_idx_out_of_list_get: ERROR: Missing station sorted AID list" && echo -1 && return
	[ -z "$2" ] && error_print "aid_idx_out_of_list_get: ERROR: Missing number of parameter to get" && echo -1 && return

	sta_index_sorted_list="$1,"  # add , at the end for the parsing
	field_location=$2

	while [ $count -lt $field_location ]; do
		aid_idx=${sta_index_sorted_list%%,*}
		sta_index_sorted_list="${sta_index_sorted_list/$aid_idx,/""}"

		count=$((count+1))
	done

	echo "$aid_idx"
}

send_plan_for_1_users(){

	local interface_name start_bw_limit aid_list aid_index sta_index index usr_index \
	dl_sub_band1 dl_start_ru1 dl_ru_size1 ul_sub_band1 ul_start_ru1 ul_ru_size1 \
	dl_sub_band2 dl_start_ru2 dl_ru_size2 ul_sub_band2 ul_start_ru2 ul_ru_size2 \
	mu_type sp_enable_value tc_name CURRENT_RADIO_UCI_PATH CURRENT_WLAN_NAME glob_ssid ldpc_tmp_1
	
	CURRENT_RADIO_UCI_PATH=$3
	CURRENT_WLAN_NAME=$1
	glob_ssid=$4
	debug_print "send_plan_for_1_users ---> get_index_from_db CURRENT_RADIO_UCI_PATH=$CURRENT_RADIO_UCI_PATH"
	
	interface_name=$1
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	start_bw_limit=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$txop_com_start_bw_limit_idx` #x3
	mu_type=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$dl_com_mu_type_idx`

	info_print "SMD: send_plan_for_1_user on $interface_name" 
	
	# update common part (WaveHeMuOperationEnable is on by default)
	tc_name=`get_test_case_name $glob_ssid`
	if [ "$tc_name" = "5.64.1" ]; then
		sp_enable_value=0
	else
		sp_enable_value=1
	fi
	#set operation_mode and dl_com_num_of_participating_stations into sMuStaticPlann_common
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=$sp_enable_value x$dl_com_num_of_participating_stations_idx=1
	
	# update 1 user plan according to BW.
	# 0-20MHz, 1-40MHz, 2-80MHz, 3-160MHz
	case "$start_bw_limit" in
		"0")
			#USER1
			dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
		;;
		"1")
			#USER1
			dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4
		;;
		"2")
			#USER1
			dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=5;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=5
		;;
		"3")
			#USER1
			dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=6;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=6
		;;
	esac
	
	# update 1 user params in DB
	usr_index=1
	local tmp_param tmp_val
	tmp_param="dl_sub_band$usr_index";eval tmp_val=\$$tmp_param
	ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`	
	[ -n "$tmp_val" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_sub_band_per_usp_idx=$tmp_val
	tmp_param="dl_start_ru$usr_index";eval tmp_val=\$$tmp_param
	[ -n "$tmp_val" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_start_ru_per_usp_idx=$tmp_val
	tmp_param="dl_ru_size$usr_index";eval tmp_val=\$$tmp_param
	[ -n "$tmp_val" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ru_size_per_usp_idx=$tmp_val
	tmp_param="ul_sub_band$usr_index";eval tmp_val=\$$tmp_param
	[ -n "$tmp_val" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_sub_band_idx=$tmp_val
	tmp_param="ul_start_ru$usr_index";eval tmp_val=\$$tmp_param	
	[ -n "$tmp_val" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_start_ru_idx=$tmp_val
	tmp_param="ul_ru_size$usr_index";eval tmp_val=\$$tmp_param
	[ -n "$tmp_val" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_ru_size_idx=$tmp_val
	
	## update ldpc according to STA ##
	sp_check_ldpc_support $5

	local static_plan_config=""
	sortFile="/tmp/sigma-sort-sta-file"
	sortedFile="/tmp/sigma-sorted-sta-file"
	rm -f $sortFile $sortedFile

	aid_index=`$HOSTAPD_CLI_CMD -i$5 all_sta $5 | grep aid=`
	aid_index=${aid_index##*=}
	# Incase multibss is enabled on the interface (for e.g. 6GHz), AID offset needs to be accounted before deriving SID
	local band=`uci show $CURRENT_RADIO_UCI_PATH | grep band | cut -d "=" -f2 | tr -d "'"`
	if [ "$band" = "6GHz" ]; then
		[ $aid_index -gt 64 ] && aid_index=$((aid_index-64))
	fi
	ldpc_tmp_1="ldpc_${aid_index}"
	ldpc_support=`eval 'echo $'$ldpc_tmp_1`

	[ "$ldpc_support" != "" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_ldpc_idx=$ldpc_support
	
	[ $aid_index -gt 0 ] && let sta_index=$aid_index-1
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_usp_station_indexes_idx=$sta_index
	$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $sortFile
		
	sort -k2 -n $sortFile > $sortedFile

	while read -r line; do
		static_plan_config="$(echo $line | sed "s/sMuStaticPlannUser[0-9]//g") ${static_plan_config}"
	done < $sortedFile

	# Change the length according to maximum NSS value of the connected STAs.
	[ "$mu_type" = "0" ] && ap_mu_tf_len=`sp_set_plan_tf_length $interface_name $ap_mu_tf_len`
	
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	dl_com_number_of_phase_repetitions=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x7`
	# if dl_com_number_of_phase_repetitions is configured -> twt_respsupport is enabled and ap_mu_tf_len="1486
	if [ "$dl_com_number_of_phase_repetitions" = "2" ]; then
		ap_mu_tf_len="1486"
	fi
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$rcr_com_tf_length_idx=$ap_mu_tf_len

	uci_commit_wireless
	static_plan_config="$($UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' | sed 's/sMuStaticPlann_common//g' | sed 's/false/0/g') ${static_plan_config}"
	debug_print "CREATE COMMAND: Args $static_plan_config"
	eval iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config
	info_print "plan for 1 user: iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config"

}

# send plan for 2 users.
send_plan_for_2_users()
{
	local interface_name start_bw_limit aid_list aid_index sta_index usr_index \
	dl_sub_band1 dl_start_ru1 dl_ru_size1 ul_sub_band1 ul_start_ru1 ul_ru_size1 \
	dl_sub_band2 dl_start_ru2 dl_ru_size2 ul_sub_band2 ul_start_ru2 ul_ru_size2 \
	mu_type sp_enable_value aid_idx_sorted_list tmp_param tmp_val ap_aid_index \
	is_set_low_ru_size CURRENT_RADIO_UCI_PATH CURRENT_WLAN_NAME glob_ssid ldpc_tmp_1 \
	splanconfigRUsorted

	CURRENT_RADIO_UCI_PATH=$3
	CURRENT_WLAN_NAME=$1
	glob_ssid=$4
	debug_print "send_plan_for_2_users ---> get_index_from_db CURRENT_RADIO_UCI_PATH=$CURRENT_RADIO_UCI_PATH"

	interface_name=$1
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	start_bw_limit=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$txop_com_start_bw_limit_idx` #x3
	mu_type=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$dl_com_mu_type_idx`

	info_print "SMD: send_plan_for_2_user on $interface_name" #info_print

	# update common part (WaveHeMuOperationEnable is on by default)
	tc_name=`get_test_case_name $glob_ssid`
	if [ "$tc_name" = "5.64.1" ]; then
		sp_enable_value=0
	else
		sp_enable_value=1
	fi
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=$sp_enable_value x$dl_com_num_of_participating_stations_idx=2

	# update 2 user plan according to BW.
	# 0-20MHz, 1-40MHz, 2-80MHz, 3-160MHz
	case "$start_bw_limit" in
		"0")
			if [ "$mu_type" = "0" ]; then
				## OFDMA ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
				#USER2
				dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2;ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=2
			else
				## MIMO ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2;ul_psdu_rate_per_usp1=4;spr_cr_tf_usr_psdu_rate1=4
				#USER2
				dl_sub_band2=0;dl_start_ru2=0;dl_ru_size2=3;ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=2;ul_psdu_rate_per_usp2=4;spr_cr_tf_usr_psdu_rate2=4
			fi
		;;
		"1")
			if [ "$mu_type" = "0" ]; then
				## OFDMA ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
				#USER2
				dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3;ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=3
			else
				## MIMO ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3;ul_psdu_rate_per_usp1=4;spr_cr_tf_usr_psdu_rate1=4
				#USER2
				dl_sub_band2=0;dl_start_ru2=0;dl_ru_size2=4;ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=3;ul_psdu_rate_per_usp2=4;spr_cr_tf_usr_psdu_rate2=4
			fi
		;;
		"2")
			if [ "$mu_type" = "0" ]; then
				## OFDMA ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4
				#USER2
				dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4;ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=4
			else
				## MIMO ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=5;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4;ul_psdu_rate_per_usp1=4;spr_cr_tf_usr_psdu_rate1=4
				#USER2
				dl_sub_band2=0;dl_start_ru2=0;dl_ru_size2=5;ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=4;ul_psdu_rate_per_usp2=4;spr_cr_tf_usr_psdu_rate2=4
			fi
		;;
		"3")
			if [ "$mu_type" = "0" ]; then
				## OFDMA ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=5;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=5
				#USER2
				dl_sub_band2=4;dl_start_ru2=0;dl_ru_size2=5;ul_sub_band2=4;ul_start_ru2=0;ul_ru_size2=5
			else
				## MIMO ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=6;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=5;ul_psdu_rate_per_usp1=4;spr_cr_tf_usr_psdu_rate1=4
				#USER2
				dl_sub_band2=0;dl_start_ru2=0;dl_ru_size2=6;ul_sub_band2=4;ul_start_ru2=0;ul_ru_size2=5;ul_psdu_rate_per_usp2=4;spr_cr_tf_usr_psdu_rate2=4
			fi
		;;
	esac

	is_set_low_ru_size="0"

	for ap_aid_index in 1 2
	do
		tmp_param="dl_ru_size$ap_aid_index"
		eval tmp_val=\${$tmp_param}

		# WLANRTSYS-9745: check if lower value is needed only if it is still not needed
		if [ $is_set_low_ru_size = "0" ]; then
			is_set_low_ru_size=`is_set_low_ru_size_get $5 $tmp_val $ap_aid_index`
		fi
	done

	# update per-user params in DB
	for usr_index in 1 2
	do
		ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`	
		tmp_param="dl_sub_band$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_sub_band_per_usp_idx=$tmp_val

		tmp_param="dl_start_ru$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_start_ru_per_usp_idx=$tmp_val

		tmp_param="ul_sub_band$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_sub_band_idx=$tmp_val

		tmp_param="ul_start_ru$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_start_ru_idx=$tmp_val

		tmp_param="ul_ru_size$usr_index";eval tmp_val=\$$tmp_param
		[ -z "$glob_ap_usrinfo_ru_alloc" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_ru_size_idx=$tmp_val

		if [ "$global_ap_mimo" != "ul" ]; then
			tmp_param="ul_psdu_rate_per_usp$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ul_psdu_rate_per_usp_idx=$tmp_val
		elif [ "$global_ap_mimo" = "ul" ] && [ "$ucc_type" = "testbed" ]; then
			# Hard-code MCS to 5 for STAUT UL MIMO data traffic phase. This is a system limitation until the feature will be stabilized for higher performance
			debug_print "Setting UL MIMO MCS to 5"
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ul_psdu_rate_per_usp_idx=5
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_target_rssi_idx=60
		fi

		tmp_param="spr_cr_tf_usr_psdu_rate$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_psdu_rate_idx=$tmp_val

		if [ $is_set_low_ru_size = "1" ]; then
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ru_size_per_usp_idx=2
		else
			debug_print "ru_size_static-plan_for2"
			tmp_param="dl_ru_size$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ru_size_per_usp_idx=$tmp_val
		fi
	done

	sp_check_ldpc_support $5
	# dynamically update STA index in DB according to: 20 MHz STA first, then NSS (higher to lower NSS)
	aid_idx_sorted_list=`get_sta_aid_idx_sorted_list $5`
	local static_plan_config=""
	sortFile="/tmp/sigma-sort-sta-file"
	sortedFile="/tmp/sigma-sorted-sta-file"
	rm -f $sortFile $sortedFile
	for ap_aid_index in 1 2
	do
		debug_print "glob_ap_ack_policy_mac=$glob_ap_ack_policy_mac ap_sta_id=$ap_sta_id"
		aid_index=`aid_idx_out_of_list_get "${aid_idx_sorted_list}" "${ap_aid_index}"`
		aid_index=`echo ${aid_index//[!0-9]/}`
		[ $aid_index -gt 0 ] && sta_index=$((aid_index-1))
		# switch the OFDMA users, so the primary sta id will be at user 1 (first user). not needed if it is already in user 1.
		if [ "$glob_ap_ack_policy_mac" != "" ] && [ $ap_sta_id -gt 0 ]; then
			#store the primary sta id in user 1
			debug_print "store the primary sta id in user 1"
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser1' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser1 x${dl_usr_usp_station_indexes_idx}=${ap_sta_id}
			$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $sortFile

			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser2' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
			$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $sortFile
			break
		else
			ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_aid_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u` 
			[ -z "$glob_ap_sta_aid" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$dl_usr_usp_station_indexes_idx=$sta_index
			$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $sortFile
		fi
		ldpc_tmp_1="ldpc_${aid_index}"
		ldpc_support=`eval 'echo $'$ldpc_tmp_1`
		[ "$ldpc_support" != "" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$rcr_tf_usr_ldpc_idx=$ldpc_support
		
		if [ "$glob_ap_trigger_coding" = "BCC" ]; then
			ap_bcc_ldpc_int=0
		elif [ "$glob_ap_trigger_coding" = "LDPC" ]; then
			ap_bcc_ldpc_int=1
		fi
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$rcr_tf_usr_ldpc_idx=$ap_bcc_ldpc_int
	done
	debug_print "remove sMuStaticPlannUser from sortPlan"
	sed -i 's/sMuStaticPlannUser[0-9]//g' $sortFile
	if [ "$glob_ap_ack_policy_mac" != "" ] && [ $ap_sta_id -gt 0 ]; then
		sort -nr $sortFile > $sortedFile
	else
		sort -n $sortFile > $sortedFile
	fi
	
	while read -r line; do
		debug_print "static_plan_config"
		static_plan_config="${static_plan_config} ${line}"
	done < $sortedFile
	
	# Change the length according to maximum NSS value of the connected STAs.
	[ "$mu_type" = "0" ] && ap_mu_tf_len=`sp_set_plan_tf_length $5 $ap_mu_tf_len`

	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	# TWT test
	band=`get_radio_band $interface_name`
	if [ "$tc_name" = "4.56.1" ]; then
		## WLANRTSYS-18156 set dl_com_number_of_phase_repetitions=3 and from PF8 Sigma: rcr_com_tf_length=1486
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${rcr_com_tf_length_idx}=1486
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_number_of_phase_repetitions_idx}=3
		ap_mu_tf_len=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$rcr_com_tf_length_idx`
	fi

	uci_commit_wireless
	static_plan_config="$($UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' | sed 's/sMuStaticPlann_common//g' | sed 's/false/0/g') ${static_plan_config}"
	debug_print "CREATE COMMAND: Args $static_plan_config"
	
	info_print "plan for 2 user Ru not sorted: iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config"
	splanconfigRUsorted=`sortPlan "$static_plan_config" 28 2`
	splanconfigDlUlRUsorted=`sortPlan "$splanconfigRUsorted" 37 2`

	if [ "$sortedList_28" != "$sortedList_37" ]
	then
		error_print "DL_RU and UP_RU not equal $sortedList_28"
		error_print "DL_RU and UP_RU not equal $sortedList_37"
		exit -1
	fi

	if [ -e "/tmp/${glob_ssid}_tmp" ]; then
		static_plan_send_debug_infra 2
	else
		eval iw dev $interface_name iwlwav sMuStatPlanCfg $splanconfigDlUlRUsorted
	fi

	info_print "plan for 2 user: iw dev $interface_name iwlwav sMuStatPlanCfg $splanconfigDlUlRUsorted"
}

# send plan for 3 users.
send_plan_for_3_users()
{
	local interface_name start_bw_limit aid_list aid_index sta_index usr_index \
	dl_sub_band1 dl_start_ru1 dl_ru_size1 ul_sub_band1 ul_start_ru1 ul_ru_size1 \
	dl_sub_band2 dl_start_ru2 dl_ru_size2 ul_sub_band2 ul_start_ru2 ul_ru_size2 \
	dl_sub_band3 dl_start_ru3 dl_ru_size3 ul_sub_band3 ul_start_ru3 ul_ru_size3 \
	mu_type tmp_param tmp_val aid_idx_sorted_list \
	ap_aid_index is_set_low_ru_size CURRENT_RADIO_UCI_PATH CURRENT_WLAN_NAME glob_ssid ldpc_tmp_1 \
	splanconfigRUsorted


	CURRENT_RADIO_UCI_PATH=$3
	CURRENT_WLAN_NAME=$1
	glob_ssid=$4
	debug_print "send_plan_for_3_users ---> get_index_from_db CURRENT_RADIO_UCI_PATH=$CURRENT_RADIO_UCI_PATH"

	interface_name=$1
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	start_bw_limit=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$txop_com_start_bw_limit_idx` #x3
	mu_type=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$dl_com_mu_type_idx`

	info_print "SMD: send_plan_for_3_user on $interface_name" #info_print

	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=1 x$dl_com_num_of_participating_stations_idx=3

	# update 4 user plan according to BW.
	# 0-20MHz, 1-40MHz, 2-80MHz, 3-160MHz
	case "$start_bw_limit" in
		"0")
			if [ "$mu_type" = "0" ]; then
				## OFDMA ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=1;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=1
				#USER2
				dl_sub_band2=0;dl_start_ru2=2;dl_ru_size2=1;ul_sub_band2=0;ul_start_ru2=2;ul_ru_size2=1
				#USER3
				dl_sub_band3=0;dl_start_ru3=5;dl_ru_size3=1;ul_sub_band3=0;ul_start_ru3=5;ul_ru_size3=1
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_3_users: MIMO not supported"
			fi
		;;
		"1")
			if [ "$mu_type" = "0" ]; then
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
				#USER2
				dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2;ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=2
				#USER3
				dl_sub_band3=1;dl_start_ru3=0;dl_ru_size3=2;ul_sub_band3=1;ul_start_ru3=0;ul_ru_size3=2
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_3_users: MIMO not supported"
			fi
		;;
		"2")
			if [ "$mu_type" = "0" ]; then
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
				#USER2
				dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3;ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=3
				#USER3
				dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=3;ul_sub_band3=2;ul_start_ru3=0;ul_ru_size3=3
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_3_users: MIMO not supported"
			fi
		;;
		"3")
			if [ "$mu_type" = "0" ]; then
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4
				#USER2
				dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4;ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=4
				#USER3
				dl_sub_band3=4;dl_start_ru3=0;dl_ru_size3=4;ul_sub_band3=4;ul_start_ru3=0;ul_ru_size3=4
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_3_users: MIMO not supported"
			fi
		;;
	esac

	is_set_low_ru_size="0"

	for ap_aid_index in 1 2 3
	do
		tmp_param="dl_ru_size$ap_aid_index"
		eval tmp_val=\${$tmp_param}

		# WLANRTSYS-9745: check if lower value is needed only if it is still not needed
		if [ $is_set_low_ru_size = "0" ]; then
			is_set_low_ru_size=`is_set_low_ru_size_get $5 $tmp_val $ap_aid_index`
		fi
	done

	## WLANRTSYS-12035
	if [ $dl_ru_size1 -lt 2 ]; then
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2300
	else
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2700
	fi


	# update per-user params in DB
	aid_idx_sorted_list=`get_sta_aid_idx_sorted_list $5`
	for usr_index in 1 2 3
	do
		ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
		#Do not update DL/UL sub-band in-case puncture configuration was done prior to final call to configure plan
		if ["$global_ap_preamblepunctmode" = ""] && ["$global_ap_preamblepunctchannel" = ""]; then
			tmp_param="dl_sub_band$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_sub_band_per_usp_idx=$tmp_val
			tmp_param="ul_sub_band$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_sub_band_idx=$tmp_val
		fi
		tmp_param="dl_start_ru$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_start_ru_per_usp_idx=$tmp_val
		tmp_param="ul_start_ru$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_start_ru_idx=$tmp_val
		debug_print "glob_ap_usrinfo_ru_alloc_SP=$glob_ap_usrinfo_ru_alloc"
		tmp_param="ul_ru_size$usr_index";eval tmp_val=\$$tmp_param
		[ -z "$glob_ap_usrinfo_ru_alloc" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_ru_size_idx=$tmp_val

		debug_print "ap_rualloctones_plan2=$ap_rualloctones ap_txbandwidth=$ap_txbandwidth"
		if [ $is_set_low_ru_size = "1" ]; then
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index  x$dl_usr_ru_size_per_usp_idx=2
		elif [ "$glob_ap_rualloctones" = "" ] || [ "$glob_ap_txbandwidth" = "" ]; then
			tmp_param="dl_ru_size$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ru_size_per_usp_idx=$tmp_val
		fi
	done

	sp_check_ldpc_support $5
	local static_plan_config=""
	# dynamically update STA index in DB according to: 20 MHz STA first, then NSS (higher to lower NSS)
	aid_idx_sorted_list=`get_sta_aid_idx_sorted_list $5`
	sortFile="/tmp/sigma-sort-sta-file"
	sortedFile="/tmp/sigma-sorted-sta-file"
	rm -f $sortFile $sortedFile
	for ap_aid_index in 1 2 3
	do
		aid_index=`aid_idx_out_of_list_get "${aid_idx_sorted_list}" "${ap_aid_index}"`
		aid_index=`echo ${aid_index//[!0-9]/}`
		[ $aid_index -gt 0 ] && sta_index=$((aid_index-1))
		ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_aid_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
		[ -z "$glob_ap_sta_aid" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$dl_usr_usp_station_indexes_idx=$sta_index
		$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $sortFile
		ldpc_tmp_1="ldpc_${aid_index}"
		ldpc_support=`eval 'echo $'$ldpc_tmp_1`
		[ "$ldpc_support" != "" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$rcr_tf_usr_ldpc_idx=$ap_bcc_ldpc_int

		if [ "$glob_ap_trigger_coding" = "BCC" ]; then
			ap_bcc_ldpc_int=0
		elif [ "$glob_ap_trigger_coding" = "LDPC" ]; then
			ap_bcc_ldpc_int=1
		fi
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$rcr_tf_usr_ldpc_idx=$ap_bcc_ldpc_int

	done

	sort -k2 -n $sortFile > $sortedFile
	while read -r line; do
		static_plan_config="$(echo $line | sed "s/sMuStaticPlannUser[0-9]//g") ${static_plan_config}"
	done < $sortedFile
	rm -f $sortFile $sortedFile

	uci_commit_wireless
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	static_plan_config="$($UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' | sed 's/sMuStaticPlann_common//g' | sed 's/false/0/g') ${static_plan_config}"
	debug_print "CREATE COMMAND: Args $static_plan_config"

	info_print "plan for 3 user Ru not sorted: iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config"
	splanconfigRUsorted=`sortPlan "$static_plan_config" 28 3`
	splanconfigDlUlRUsorted=`sortPlan "$splanconfigRUsorted" 37 3`

	if [ "$sortedList_28" != "$sortedList_37" ]
	then
		error_print "DL_RU and UP_RU not equal $sortedList_28"
		error_print "DL_RU and UP_RU not equal $sortedList_37"
		exit -1
	fi

	if [ -e "/tmp/${glob_ssid}_tmp" ]; then
		static_plan_send_debug_infra 3
	else
		eval iw dev $interface_name iwlwav sMuStatPlanCfg $splanconfigDlUlRUsorted
	fi
	info_print "plan for 3 user: iw dev $interface_name iwlwav sMuStatPlanCfg $splanconfigDlUlRUsorted"
}

send_plan_for_4_users()
{
	local interface_name start_bw_limit aid_list aid_index sta_index usr_index \
	dl_sub_band1 dl_start_ru1 dl_ru_size1 ul_sub_band1 ul_start_ru1 ul_ru_size1 \
	dl_sub_band2 dl_start_ru2 dl_ru_size2 ul_sub_band2 ul_start_ru2 ul_ru_size2 \
	dl_sub_band3 dl_start_ru3 dl_ru_size3 ul_sub_band3 ul_start_ru3 ul_ru_size3 \
	dl_sub_band4 dl_start_ru4 dl_ru_size4 ul_sub_band4 ul_start_ru4 ul_ru_size4 \
	mu_type sp_enable_value tc_name tmp_param tmp_val aid_idx_sorted_list \
	ap_aid_index is_set_low_ru_size CURRENT_RADIO_UCI_PATH CURRENT_WLAN_NAME glob_ssid ldpc_tmp_1 \
	usr cnt splanconfigRUsorted


	CURRENT_RADIO_UCI_PATH=$3
	CURRENT_WLAN_NAME=$1
	glob_ssid=$4
	debug_print "send_plan_for_4_users ---> get_index_from_db CURRENT_RADIO_UCI_PATH=$CURRENT_RADIO_UCI_PATH"
	
	interface_name=$1
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	start_bw_limit=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$txop_com_start_bw_limit_idx` #x3
	mu_type=`get_index_from_db $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx x$dl_com_mu_type_idx`

	info_print "SMD: send_plan_for_4_user on $interface_name" #info_print

	# update common part (WaveHeMuOperationEnable is on by default)
	tc_name=`get_test_case_name $glob_ssid`
	if [ "$tc_name" = "5.64.1" ]; then
		sp_enable_value=0
	else
		sp_enable_value=1
	fi
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=$sp_enable_value x$dl_com_num_of_participating_stations_idx=4
	
	# update 4 user plan according to BW.
	# 0-20MHz, 1-40MHz, 2-80MHz, 3-160MHz
	case "$start_bw_limit" in
		"0")
			if [ "$mu_type" = "0" ]; then
				## OFDMA ##
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=1;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=1
				#USER2
				dl_sub_band2=0;dl_start_ru2=2;dl_ru_size2=1;ul_sub_band2=0;ul_start_ru2=2;ul_ru_size2=1
				#USER3
				dl_sub_band3=0;dl_start_ru3=5;dl_ru_size3=1;ul_sub_band3=0;ul_start_ru3=5;ul_ru_size3=1
				#USER4
				dl_sub_band4=0;dl_start_ru4=7;dl_ru_size4=1;ul_sub_band4=0;ul_start_ru4=7;ul_ru_size4=1
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_4_users: MIMO not supported"
			fi
		;;
		"1")
			if [ "$mu_type" = "0" ]; then
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=2;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=2
				#USER2
				dl_sub_band2=0;dl_start_ru2=5;dl_ru_size2=2;ul_sub_band2=0;ul_start_ru2=5;ul_ru_size2=2
				#USER3
				dl_sub_band3=1;dl_start_ru3=0;dl_ru_size3=2;ul_sub_band3=1;ul_start_ru3=0;ul_ru_size3=2
				#USER4
				dl_sub_band4=1;dl_start_ru4=5;dl_ru_size4=2;ul_sub_band4=1;ul_start_ru4=5;ul_ru_size4=2
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_4_users: MIMO not supported"
			fi
		;;
		"2")
			if [ "$mu_type" = "0" ]; then
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=3;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=3
				#USER2
				dl_sub_band2=1;dl_start_ru2=0;dl_ru_size2=3;ul_sub_band2=1;ul_start_ru2=0;ul_ru_size2=3
				#USER3
				dl_sub_band3=2;dl_start_ru3=0;dl_ru_size3=3;ul_sub_band3=2;ul_start_ru3=0;ul_ru_size3=3
				#USER4
				dl_sub_band4=3;dl_start_ru4=0;dl_ru_size4=3;ul_sub_band4=3;ul_start_ru4=0;ul_ru_size4=3
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_4_users: MIMO not supported"
			fi
		;;
		"3")
			if [ "$mu_type" = "0" ]; then
				#USER1
				dl_sub_band1=0;dl_start_ru1=0;dl_ru_size1=4;ul_sub_band1=0;ul_start_ru1=0;ul_ru_size1=4
				#USER2
				dl_sub_band2=2;dl_start_ru2=0;dl_ru_size2=4;ul_sub_band2=2;ul_start_ru2=0;ul_ru_size2=4
				#USER3
				dl_sub_band3=4;dl_start_ru3=0;dl_ru_size3=4;ul_sub_band3=4;ul_start_ru3=0;ul_ru_size3=4
				#USER4
				dl_sub_band4=6;dl_start_ru4=0;dl_ru_size4=4;ul_sub_band4=6;ul_start_ru4=0;ul_ru_size4=4
			else
				## MIMO ##
				info_print "SMD:start_bw_limit=$start_bw_limit:send_plan_for_4_users: MIMO not supported"
			fi
		;;
	esac

	is_set_low_ru_size="0"

	for ap_aid_index in 1 2 3 4
	do
		tmp_param="dl_ru_size$ap_aid_index"
		eval tmp_val=\${$tmp_param}

		# WLANRTSYS-9745: check if lower value is needed only if it is still not needed
		if [ $is_set_low_ru_size = "0" ]; then
			is_set_low_ru_size=`is_set_low_ru_size_get $5 $tmp_val $ap_aid_index`
		fi
	done
	
	## WLANRTSYS-12035
	if [ $dl_ru_size1 -lt 2 ]; then
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2300
	else
		ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x${dl_com_maximum_ppdu_transmission_time_limit_idx}=2700
	fi


	# update per-user params in DB
	aid_idx_sorted_list=`get_sta_aid_idx_sorted_list $5`
	for usr_index in 1 2 3 4
	do
		ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$usr_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
		tmp_param="dl_sub_band$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_sub_band_per_usp_idx=$tmp_val
		tmp_param="dl_start_ru$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_start_ru_per_usp_idx=$tmp_val
		tmp_param="ul_sub_band$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_sub_band_idx=$tmp_val
		tmp_param="ul_start_ru$usr_index";eval tmp_val=\$$tmp_param
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_start_ru_idx=$tmp_val
		debug_print "glob_ap_usrinfo_ru_alloc_SP=$glob_ap_usrinfo_ru_alloc"
		tmp_param="ul_ru_size$usr_index";eval tmp_val=\$$tmp_param
		[ -z "$glob_ap_usrinfo_ru_alloc" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$rcr_tf_usr_ru_size_idx=$tmp_val
		
		debug_print "ap_rualloctones_plan2=$ap_rualloctones ap_txbandwidth=$ap_txbandwidth"
		if [ $is_set_low_ru_size = "1" ]; then
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index  x$dl_usr_ru_size_per_usp_idx=2
		elif [ "$glob_ap_rualloctones" = "" ] || [ "$glob_ap_txbandwidth" = "" ]; then
			tmp_param="dl_ru_size$usr_index";eval tmp_val=\$$tmp_param
			Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser$usr_index x$dl_usr_ru_size_per_usp_idx=$tmp_val
		fi
	done

	sp_check_ldpc_support $5
	local static_plan_config=""
	# dynamically update STA index in DB according to: 20 MHz STA first, then NSS (higher to lower NSS)
	aid_idx_sorted_list=`get_sta_aid_idx_sorted_list $5`
	sortFile="/tmp/sigma-sort-sta-file"
	sortedFile="/tmp/sigma-sorted-sta-file"
	rm -f $sortFile $sortedFile
	for ap_aid_index in 1 2 3 4
	do
		aid_index=`aid_idx_out_of_list_get "${aid_idx_sorted_list}" "${ap_aid_index}"`
		aid_index=`echo ${aid_index//[!0-9]/}`
		[ $aid_index -gt 0 ] && sta_index=$((aid_index-1))
		ap_get_debug_hostap_conf_or_Post_Up_idx=`grep 'sMuStaticPlannUser'$ap_aid_index $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
		[ -z "$glob_ap_sta_aid" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$dl_usr_usp_station_indexes_idx=$sta_index
		$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $sortFile
		ldpc_tmp_1="ldpc_${aid_index}"
		ldpc_support=`eval 'echo $'$ldpc_tmp_1`
		[ "$ldpc_support" != "" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$rcr_tf_usr_ldpc_idx=$ap_bcc_ldpc_int
		
		if [ "$glob_ap_trigger_coding" = "BCC" ]; then
			ap_bcc_ldpc_int=0
		elif [ "$glob_ap_trigger_coding" = "LDPC" ]; then
			ap_bcc_ldpc_int=1
		fi
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlannUser${ap_aid_index} x$rcr_tf_usr_ldpc_idx=$ap_bcc_ldpc_int

	done
	
	tc_name=`get_test_case_name $glob_ssid`
	if [ "$tc_name" = "4.69.1" ]; then
	usersFile="/tmp/sigma-usrs-file"
	usr=""
	cnt=0
	while read -r line_1; do
		usr="$(echo "${line_1}" | cut -d' ' -f1)"
		ap_get_debug_hostap_conf_or_Post_Up_idx=`grep $usr $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`	
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx ${usr} x$dl_usr_sub_band_per_usp_idx=$cnt
		Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx ${usr} x$rcr_tf_usr_sub_band_idx=$cnt
		cnt=$((cnt+1))
		$UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' >> $usersFile
	done < $sortFile

	while read -r line; do
		static_plan_config="${static_plan_config} $(echo $line | sed "s/sMuStaticPlannUser[0-9]//g")"
	done < $usersFile

	rm -f $sortFile $sortedFile $usersFile
	else
		sort -k2 -n $sortFile > $sortedFile
		while read -r line; do
			static_plan_config="$(echo $line | sed "s/sMuStaticPlannUser[0-9]//g") ${static_plan_config}"
		done < $sortedFile
		rm -f $sortFile $sortedFile
	fi
	# Change the length according to maximum NSS value of the connected STAs.
	[ "$mu_type" = "0" ] && ap_mu_tf_len=`sp_set_plan_tf_length $5 $ap_mu_tf_len`
	tc_name=`get_test_case_name $glob_ssid`
	if [ "$tc_name" = "4.45.1" ]; then
		ap_mu_tf_len="310"
	fi


	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	[ -n "$ap_mu_tf_len" ] && Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$rcr_com_tf_length_idx=$ap_mu_tf_len

	uci_commit_wireless
	static_plan_config="$($UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' | sed 's/sMuStaticPlann_common//g' | sed 's/false/0/g') ${static_plan_config}"
	debug_print "CREATE COMMAND: Args $static_plan_config"
	
	info_print "plan for 4 user Ru not sorted: iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config"
	splanconfigRUsorted=`sortPlan "$static_plan_config" 28 4`
	splanconfigDlUlRUsorted=`sortPlan "$splanconfigRUsorted" 37 4`

	if [ "$sortedList_28" != "$sortedList_37" ]
	then
		error_print "DL_RU and UP_RU not equal $sortedList_28"
		error_print "DL_RU and UP_RU not equal $sortedList_37"
		exit -1
	fi

	if [ -e "/tmp/${glob_ssid}_tmp" ]; then
		static_plan_send_debug_infra 4
	else
		eval iw dev $interface_name iwlwav sMuStatPlanCfg $splanconfigDlUlRUsorted
	fi
	info_print "plan for 4 user: iw dev $interface_name iwlwav sMuStatPlanCfg $splanconfigDlUlRUsorted"
}

send_plan_off()
{
	#local interface_name sp_enable static_plan_config CURRENT_WLAN_NAME CURRENT_RADIO_UCI_PATH
	local interface_name=$1
	local CURRENT_WLAN_NAME=$1
	local CURRENT_RADIO_UCI_PATH=$2
	ap_get_debug_hostap_conf_or_Post_Up_idx=`get_sMuStaticPlann_common_idx_from_db $CURRENT_RADIO_UCI_PATH`
	info_print "send_plan_off $interface_name"
	Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=0
	info_print "Dynamic_set_get_helper iw_off $CURRENT_WLAN_NAME $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx sMuStaticPlann_common x$operation_mode_idx=0"
	static_plan_config="$($UCI_CMD get $CURRENT_RADIO_UCI_PATH.$ap_get_debug_hostap_conf_or_Post_Up_idx | sed 's/=/ /g' | sed 's/sMuStaticPlann_common//g' | sed 's/false/0/g')"
	static_plan_config=`echo $static_plan_config`
	info_print "static_plan_config=$static_plan_config"
	if [ -n "$static_plan_config" ]
	then
		eval iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config
		debug_print "iw plan off : iw dev $interface_name iwlwav sMuStatPlanCfg $static_plan_config"
	fi
}

# usage: handler_func <status: plan off/on> <interface> <number-of-users>
handler_func(){
	debug_print "handler_func $@"
	. /tmp/sigma_hf_arguments
	local plan_type current_wlan station_number
	plan_type=$1
	current_wlan=$2
	station_number=$3
	glob_ofdma=$4
	#Skip sending 1-user plan for MIMO
	if ([ "$global_ap_mimo" == "dl" ] || [ "$global_ap_mimo" == "ul" ] || [ "$global_mu_txbf" == "enable" ]) && [ "$station_number" = "1" ]; then
		plan_type="plan_off"
	fi
	#Skip sending 3-user plan if Preamble Puncturing was not enabled
	if [ "$global_ap_preamble_puncture" != "enable" ] && [ "$station_number" = "3" ]; then
		plan_type="plan_off"
	fi
	ap_get_debug_hostap_conf_or_Post_Up_idx=`grep  -w 'twt_responder_support' $ETC_CONFIG_WIRELESS_PATH | awk '{print $2}' | sort -u`
	glob_twt_respsupport=`$UCI_CMD get $current_radio.$ap_get_debug_hostap_conf_or_Post_Up_idx`
	glob_twt_respsupport="${glob_twt_respsupport#*=}"
	case "$plan_type" in
		"plan_off")
			send_plan_off ${current_wlan/.0} $current_radio
		;;
		"plan_on")
			send_plan_for_${3}_users ${current_wlan/.0} $station_number $current_radio $glob_ssid $current_wlan
		;;
		*)
			error_print "Error: wrong plan type." #error_print
		;;
	esac

}

mbo_handler() {
	debug_print "mbo_handler $@"
	local ap_sta_connected=`echo $@ | grep AP-STA-CONNECTED`
	local rrm_beacon_rep=`echo $@ | grep RRM-BEACON-REP-RECEIVED`
	local wnm_notification=`echo $@ | grep AP-STA-WNM-NOTIF`

	if [ "$ucc_program" = "vht" ]; then
		if [ "$ap_sta_connected" != "" ]; then
			local radio=`uci show wireless | grep 5GHz | cut -d"." -f2`
			local default_radio=`uci show wireless | grep $radio | grep default_radio | head -1 | cut -d"." -f1-2`
			local interface=`uci show $default_radio.ifname | cut -d"=" -f2 | tr -d "'"`
			local num_sta=`run_dwpal_cli_cmd $interface.0 peerlist | grep "peer(s) connected" | awk '{print $1}'`
			if [ "2" = "$num_sta" ]; then
				ap_tmp=`iw dev $interface iwlwav sDoSimpleCLI 50 0x00010000 0x01ff01ff 0x00000101`
			fi
		fi
		#mbo_handler should be called in mbo TC only, for vht this is workaround to run 4.2.56 TC 
		return 
	fi
	if [ "$ap_sta_connected" != "" ]; then
		debug_print "ap_sta_connected received"
		local non_pref_chan=`echo $ap_sta_connected | sed -n 's/.*non_pref_chan=//p' | awk '{print $1}'`
		local channel=`echo $non_pref_chan | cut -d':' -f2`
		if [ "$channel" != "" ]; then
			remove_neighbor_for_channel $channel
		fi
	elif [ "$rrm_beacon_rep" != "" ]; then
		debug_print "rrm_beacon_rep received"
		local op_class=`echo $rrm_beacon_rep | sed -n 's/.*op_class=//p' | awk '{print $1}'`
		local channel=`echo $rrm_beacon_rep | sed -n 's/.*channel=//p' | awk '{print $1}'`
		local bssid=`echo $rrm_beacon_rep | sed -n 's/.*bssid=//p' | awk '{print $1}'`
		is_channel_non_pref=""
		for single_neighbor in $CURRENT_NON_PREF_NEIGHBORS
		do
			curr_chan=`echo "$single_neighbor" | awk -F"," '{print $4}'`
			if [ "$curr_chan" = "$channel" ]; then
				is_channel_non_pref="1"
			fi
		done
		if [ "$bssid" != "" -a "$op_class" != "" -a "$channel" != "" -a "$channel" != "44" -a $is_channel_non_pref = "" ]; then
			CURRENT_NEIGHBORS=`cat /tmp/mbo_neighbors`
			CURRENT_NEIGHBORS="neighbor=${bssid},0,${op_class},${channel},9,253 $CURRENT_NEIGHBORS"
			echo $CURRENT_NEIGHBORS > /tmp/mbo_neighbors
		fi
	elif [ "$wnm_notification" != "" ]; then
		debug_print "wnm_notification received"
		merge_pref_non_pref_mbo_neighbors
		local non_pref_chan=`echo $wnm_notification | grep -oE "non_pref_chan=[0-9]*:[0-9]*:[0-9]*:[0-9]*"`
		while [ "$non_pref_chan" != "" ]; do
			non_pref_chan=`echo $non_pref_chan | awk '{print $1}'`
			local channel=`echo $non_pref_chan | cut -d':' -f2`
			local pref=`echo $non_pref_chan | cut -d':' -f3`
			if [ "$channel" != ""  ]; then
				if [ "$pref" == "0" ]; then
					remove_neighbor_for_channel $channel
				else
					add_neighbor_for_channel $channel
				fi
			fi
			wnm_notification=`echo $wnm_notification | sed -n "s/$non_pref_chan//p"`
			non_pref_chan=`echo $wnm_notification | grep -oE "non_pref_chan=[0-9]*:[0-9]*:[0-9]*:[0-9]*"`
		done
	fi
}

get_sMuStaticPlann_common_idx_from_db()
{
	local CURRENT_RADIO_UCI_PATH=$1
	local db_idx=`uci show wireless | grep ${CURRENT_RADIO_UCI_PATH}.*sMuStaticPlann_common`
	db_idx=${db_idx##$CURRENT_RADIO_UCI_PATH.}
	echo ${db_idx%%=*}
}

randomize_macs()
{
	local radio
	local radio_list="radio0 radio2"
	if [ `is_6g_supported` = "1" ]; then
		radio_list="radio0 radio2 radio4"
	fi
	for radio in $radio_list
	do
		randd=`hexdump -n2 -e'/1 ":%02X"' /dev/urandom | cut -c2-`
		while [ ${#randd} -ne 5 ]
		do
			randd=`hexdump -n2 -e'/1 ":%02X"' /dev/urandom | cut -c2-`
		done
		rand_mac="00:09:86:$randd:00"
		rand_mac_vap="00:09:86:$randd:01"
		radio_name=`uci show wireless | grep device=\'${radio}\' | head -1`
		radio_name=${radio_name%.*}
		radio_vap_name=`uci show wireless | grep device=\'${radio}\' | tail -1`
		radio_vap_name=${radio_vap_name%.*}
		$UCI_CMD set wireless.${radio}.macaddr=$rand_mac
		$UCI_CMD set ${radio_name}.macaddr=$rand_mac
		$UCI_CMD set ${radio_vap_name}.macaddr=$rand_mac_vap
	done
}
############################################################## End of planner ##############################################################

if [ "$source_flag" = "sigma-start.sh" ]; then

	info_print "Sigma-AP Agent version $CA_VERSION is running ..."

	debug_print "HW_MODEL:$MODEL"

	#important, set field separator properly
	IFS=$ORIG_IFS

	let ap_line_count=0

	while read line; do
	debug_print "read: >>>$line<<<"
	let ap_line_count+=1
	# remove special characters except comma, underscore, exclamation mark
	#tline=`echo $line | tr -dc '[:alnum:],_!\n'`
	#debug_print "tline: >>>$tline<<<"
	# For Windows hosts we need to remove carriage returns from the line
	tline=`echo $line | tr -d '\r'`
	# Delete trailing spaces
	tline=`echo $tline | sed 's/ *$//g'`
	debug_print "tline: >>>$tline<<<"
	IFS=,
	parse_command $tline
	IFS=$ORIG_IFS

	debug_print "lines parsed: $ap_line_count"
	debug_print "clearing all temp ap_ variables"

	variables=`set | grep "^ap_" | cut -d= -f1 | xargs echo `
	for var in $variables; do
		#debug_print "clearing $var"
		unset ${var}
	done

	unset variables
	unset token
	unset line
	unset tline
	done
	if [ "$tc_name" = "4.44.1" ] && [ "$OS_NAME" = "RDKB" ]
	then
		cli sys/pp/setDefensiveMode 0
	fi
	#if [ "$tc_name" = "4.2.30" ] && [ "$OS_NAME" = "RDKB" ]
	#then
		#cli sys/pp/fragBpSupport 0
	#fi
fi
