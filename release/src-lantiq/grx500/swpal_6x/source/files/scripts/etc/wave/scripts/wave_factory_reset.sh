#!/bin/sh

#
# This script performs wifi factory reset
#
# perform full factory - wave_factory_reset.sh
# perform factory for vap wlan0.0 - wave_factory_reset.sh vap wlan0.0
# perform factory for radio wlan0 - wave_factory_reset.sh radio wlan0
# use alternate db files for the factory - wave_factory_reset.sh -p <alternate directory name>
# perform factory reset and create user defined number of vaps - wave_factory_reset.sh -v <number of vaps per radio>
# reset only mac addresses wave_factory_reset.sh -m
# perform factory and choose which radio is set to which PCI slot - In the general database folder create configuration file name radio_map_file
#         map file example: radio0 PCI_SLOT01
#                           radio2 PCI_SLOT02
#
#
# Example phys_conf_file file:
# phy=phy0
# iface_index=0
# radio=2.4Ghz
#
# phy=phy1
# iface_index=2
# radio=5Ghz
# is_zwdfs=1
#
# phy=phy2
# iface_index=4
# radio=5Ghz
#
# Usage in server: wave_factory_reset.sh platform_dependent_build_time.sh
# You may change the platform_dependent_build_time.sh OS_NAME,DEFAULT_DB_PATH,UCI_DB_PATH to fit your needs.
# database dir must contain wireless_def_vap_db as well as wireless_def_radio_<24g/5g/6g> files and phys_conf_file
# Make sure common_utils.sh script is in the same folder as the wave_factory_reset.sh you are running
#

function main() {
	if [ -f /lib/wifi/platform_dependent.sh ]; then
		. /lib/wifi/platform_dependent.sh
		. "$SCRIPTS_PATH/common_utils.sh"
	else
		. "$1"
		local tmp=$(echo $0 | sed 's/\(.*\)\/.*/\1/')
		# assuming it is in the same path as called script
		. "$tmp/common_utils.sh"
	fi

	PROG=""

	MERGE_PARAM="meta-factory.merge.enable"

	update_mac_address_if_needed

	# Process optional arguments -p and/or -v
	while getopts "p:v:" OPTS; do
		if [ "$OPTS" = "p" ]; then
			if [ "${OPTARG//[0-9A-Za-z_\/-]/}" = "" ]; then
				if [ -d $DEFAULT_DB_PATH/"$OPTARG" ]; then
					PROG="$OPTARG"
				else
					print_logs "requested default DB folder "$OPTARG" does not exist"
				fi
			else
				print_logs "illegal default DB folder requested "$OPTARG". must use only \"0-9,a-z,A-Z,_,/,-\""
			fi
		elif [ "$OPTS" = "v" ]; then
			if [ "${OPTARG//[0-9]/}" = "" ] && [ "$OPTARG" -le 8 ]; then
				vapCount="$OPTARG"
				print_logs "requested factory reset in "$vapCount"+"$vapCount" mode"
			else
				print_logs "illegal default number of VAPs requested "$OPTARG". must use only \"0-8\""
			fi
		fi
	done

	meta_factory_init

	if [ "$PROG" = "" ]; then
		init_prog
	fi

	DEFAULT_DB_PATH=$DEFAULT_DB_PATH/$PROG
	print_logs "factory reset using default files from $DEFAULT_DB_PATH"

	DEFAULT_DB_STATION_VAP=$DEFAULT_DB_PATH/wireless_def_station_vap
	DEFAULT_DB_RADIO_6=$DEFAULT_DB_PATH/wireless_def_radio_6g
	DEFAULT_DB_RADIO_5=$DEFAULT_DB_PATH/wireless_def_radio_5g
	DEFAULT_DB_RADIO_24=$DEFAULT_DB_PATH/wireless_def_radio_24g
	DEFAULT_DB_VAP=$DEFAULT_DB_PATH/wireless_def_vap_db
	DEFAULT_DB_VAP_6G=$DEFAULT_DB_PATH/wireless_def_vap_db_6g
	DEFAULT_DB_VAP_5G=$DEFAULT_DB_PATH/wireless_def_vap_db_5g
	DEFAULT_DB_VAP_24G=$DEFAULT_DB_PATH/wireless_def_vap_db_24g
	DEFAULT_DB_VAP_SPECIFIC=$DEFAULT_DB_PATH/wireless_def_vap_
	DEFAULT_DB_GENERIC=$DEFAULT_DB_PATH/wireless_generic
	PHYS_CONF_FILE="$DEFAULT_DB_PATH/phys_conf_file"

	CURRENT_CONFIG=""

	DUMMY_VAP_OFSET=1000
}

function usage {
	print_logs "usage: $0 [init|merge|vap|radio|all_radios|help] <interface name>"
	print_logs "	init|merge 	run init flow - if no wireless db exists
			do full reset, else if merge is enabled ,do merge"
	print_logs "	vap <interface name> 		reset vap"
	print_logs "	radio <main interface name> 	reset radio"
	print_logs "	all_radios 			reset all radios"
	print_logs "	help				this log"
	print_logs "	anything else (or nothing)	full complete reset"
}

function create_station(){
	local rpc_idx=$1
	local sta_idx=$((rpc_idx+1))
	local vap_idx=$((10+sta_idx*16))

	if [ -f $DEFAULT_DB_STATION_VAP ]; then
		uci_set_helper "config wifi-iface 'default_radio$vap_idx'" "$tmp_wireless"
		uci_set_helper "		option device 'radio$rpc_idx'" "$tmp_wireless"
		uci_set_helper "		option ifname 'wlan$sta_idx'" "$tmp_wireless"

		set_mac_address "wlan$rpc_idx" "$tmp_wireless" "sta"

		use_template $rpc_idx $DEFAULT_DB_STATION_VAP 0 $tmp_wireless
	fi
}

# main function on regular flow

function full_reset(){

	print_logs "$0: Performing full factory reset..."

	if [ X$vapCount == "X" ]; then
		print_logs "using default number of VAPs"
		num_slave_vaps_24g=`$UCI -c $DEFAULT_DB_PATH/ get defaults.num_vaps.24g`
		num_slave_vaps_5g=`$UCI -c $DEFAULT_DB_PATH/ get defaults.num_vaps.5g`
		num_slave_vaps_6g=`$UCI -c $DEFAULT_DB_PATH/ get defaults.num_vaps.6g`
	else
		num_slave_vaps_24g="$vapCount"
		num_slave_vaps_5g="$vapCount"
		num_slave_vaps_6g="$vapCount"
		print_logs "overriding default number of VAPs"
	fi

	if [ ! -d $UCI_DB_PATH ]; then
		mkdir -p $UCI_DB_PATH
	fi

	# network file is required by UCI
	if [ ! -f $UCI_DB_PATH/network ]; then
		touch $UCI_DB_PATH/network
	fi

	# check if wave_trusted_store binary exists
	if [ -f /usr/bin/wave_trusted_store ]; then
		wave_trusted_store factory "$PROG"
	fi

	clean_uci_cache

	clean_uci_db

	prepare_vars
	local num_of_phys=`echo "$phys" | wc -l`

	iface_idx=0

	tmp_rpc_indexes_radio=$(mktemp /tmp/rpc_indexes_radio.XXXXXX)
	uci_set_helper "config wifi-info 'radio_rpc_indexes'" "$tmp_rpc_indexes_radio"
	tmp_rpc_indexes_radio_vap=$(mktemp /tmp/rpc_indexes_radio_vap.XXXXXX)
	uci_set_helper "config wifi-info 'radio_vap_rpc_indexes'" "$tmp_rpc_indexes_radio_vap"
	tmp_rpc_indexes_vap=$(mktemp /tmp/rpc_indexes_vap.XXXXXX)
	uci_set_helper "config wifi-info 'vap_rpc_indexes'" "$tmp_rpc_indexes_vap"

	# Fill Radio interfaces
	for phy in $phys; do
		get_iface_idx $phy
		iface="wlan$iface_idx"

		local phy_band=`get_band "$phy" "$iface"`

		radio_rpc_index=$(($iface_idx/2))

		remove_dfs_state_file "$iface_idx"

		uci_set_helper "config wifi-device 'radio$iface_idx'" "$tmp_wireless"
		uci_set_helper "        option rpc_index '$radio_rpc_index'" "$tmp_wireless"
		uci_set_helper "        option index$radio_rpc_index '$iface_idx'" "$tmp_rpc_indexes_radio"
		uci_set_helper "        option phy '$phy'" "$tmp_wireless"
		set_mac_address "$iface" "$tmp_wireless"

		# the radio configuration files must be named in one of the following formats:
		# <file name>
		# <file name>_<iface idx>
		# <file name>_<iface idx>_<HW type>_<HW revision>

		get_board
		if [ $phy_band = "5" ]
		then
			local num_slave_vaps=$num_slave_vaps_5g
			use_templates $iface_idx ${DEFAULT_DB_RADIO_5}_${iface_idx} $DEFAULT_DB_RADIO_5  $TMP_CONF_FILE
			local is_radio_zwdfs=`get_is_zwdfs ${iface_idx} $phy`
			if [ "$is_radio_zwdfs" -eq 1 ]; then
				num_slave_vaps=`$UCI -c $DEFAULT_DB_PATH/ get defaults.num_vaps.5g_zw_dfs`
				use_templates $iface_idx ${DEFAULT_DB_RADIO_5}_zw_dfs $TMP_CONF_FILE $TMP_CONF_FILE
			fi
			use_templates_tmp_file $iface_idx ${DEFAULT_DB_RADIO_5}_${iface_idx}_${board} $TMP_CONF_FILE $tmp_wireless
		elif [ $phy_band = "6" ]
		then
			local num_slave_vaps=$num_slave_vaps_6g
			use_templates $iface_idx ${DEFAULT_DB_RADIO_6}_${iface_idx} $DEFAULT_DB_RADIO_6  $TMP_CONF_FILE
			use_templates_tmp_file $iface_idx ${DEFAULT_DB_RADIO_6}_${iface_idx}_${board} $TMP_CONF_FILE $tmp_wireless
		else
			local num_slave_vaps=$num_slave_vaps_24g
			use_templates $iface_idx ${DEFAULT_DB_RADIO_24}_${iface_idx} $DEFAULT_DB_RADIO_24  $TMP_CONF_FILE
			use_templates_tmp_file $iface_idx ${DEFAULT_DB_RADIO_24}_${iface_idx}_${board} $TMP_CONF_FILE $tmp_wireless
		fi
		rm -f $TMP_CONF_FILE

		local first_vap=1

		# Fill VAP interfaces
		vap_idx=0
		while [ "$vap_idx" -le "$num_slave_vaps" ]; do

			if [ $first_vap -eq 1 ]; then
				uci_idx=$((DUMMY_VAP_OFSET+iface_idx))
				uci_set_helper "config wifi-iface 'default_radio$uci_idx'" "$tmp_wireless"
				uci_set_helper "        option rpc_index '$radio_rpc_index'" "$tmp_wireless"
				uci_set_helper "        option index$radio_rpc_index '$uci_idx'" "$tmp_rpc_indexes_radio_vap"
				minor=""
			elif [ $vap_idx -ge $num_slave_vaps ]; then
				break
			else
				uci_idx=$((10+iface_idx*16+vap_idx))
				rpc_idx=$((iface_idx/2+vap_idx*num_of_phys))
				uci_set_helper "config wifi-iface 'default_radio$uci_idx'" "$tmp_wireless"
				uci_set_helper "        option rpc_index '$rpc_idx'" "$tmp_wireless"
				uci_set_helper "        option index$rpc_idx '$uci_idx'" "$tmp_rpc_indexes_vap"
				minor=".$vap_idx"
			fi

			uci_set_helper "        option device 'radio$iface_idx'" "$tmp_wireless"
			uci_set_helper "        option ifname 'wlan$iface_idx$minor'" "$tmp_wireless"

			set_mac_address "wlan$iface_idx$minor" "$tmp_wireless"
			
			tmp_vap_conf_file=$(mktemp /tmp/vap_conf_file.XXXXXX)
			#generic vap db parameters
			use_templates $uci_idx $DEFAULT_DB_VAP_SPECIFIC$uci_idx $DEFAULT_DB_VAP $tmp_vap_conf_file
			
			#band specific vap db parameters
			if [ $phy_band = "6" ]
			then
				use_templates_tmp_file $uci_idx $DEFAULT_DB_VAP_6G $tmp_vap_conf_file $tmp_wireless
			elif [ $phy_band = "5" ]
			then
				use_templates_tmp_file $uci_idx $DEFAULT_DB_VAP_5G $tmp_vap_conf_file $tmp_wireless
			else
				use_templates_tmp_file $uci_idx $DEFAULT_DB_VAP_24G $tmp_vap_conf_file $tmp_wireless
			fi
			rm -f $tmp_vap_conf_file

			if [ $first_vap -eq 1 ]; then
				first_vap=0
			else
				vap_idx=$((vap_idx+1))
			fi

		done
		local station_supported=`is_station_supported $phy`
		if [ $station_supported -eq 1 ]; then
			create_station "$iface_idx"
		fi

		iface_idx=$((iface_idx+2))
	done

	use_templates 0 0 $tmp_rpc_indexes_radio $tmp_wireless
	use_templates 0 0 $tmp_rpc_indexes_radio_vap $tmp_wireless
	use_templates 0 0 $tmp_rpc_indexes_vap $tmp_wireless
	rm $tmp_rpc_indexes_radio
	rm $tmp_rpc_indexes_radio_vap
	rm $tmp_rpc_indexes_vap

	uci_set_helper "config wifi-iface 'generic'" "$tmp_wireless"
	use_templates 0 0 $DEFAULT_DB_GENERIC $tmp_wireless

	commit_changes

	print_logs "$0: Done..."
}

main "$@"

case $1 in
	radio)
		if [ "$#" -ne 2 ]; then
			usage
			exit 1
		fi
		reset_radio $2
		break
		;;
	all_radios)
		radios=`ifconfig -a | grep "wlan[0|2|4] " | awk '{ print $1 }'`
		for iface in $radios; do
			reset_radio $iface
		done
		break
		;;
	vap)
		if [ "$#" -ne 2 ]; then
			usage
			exit 1
		fi
		reset_vap $2
		break
		;;
	help)
		usage
		break
		;;
	init|merge)
		reset_on_init
		break
		;;
	*)
		full_reset
		;;
esac

