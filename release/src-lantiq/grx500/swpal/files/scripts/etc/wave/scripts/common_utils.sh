#!/bin/sh

# meta_factory functions

function check_meta_factory() {
	if [ ! -f $UCI_DB_PATH/meta-factory ]; then
		copy_meta_factory
	else
		local enable=`$UCI -c $DEFAULT_DB_PATH/ get meta-factory.merge.enable`
		$UCI set meta-factory.merge.enable=$enable
		$UCI commit meta-factory
	fi

	if [ -f /lib/netifd/debug_infrastructure.sh ]; then
		local force=`$UCI get meta-factory.merge.force 2>/dev/null`
		if [ "$force" != "" ]; then
			MERGE_PARAM="meta-factory.merge.force"
		fi
	fi

	local merge=`$UCI get $MERGE_PARAM 2>/dev/null`
	if [ ! -d $UCI_DB_PATH/src_templates ]; then
		mkdir $UCI_DB_PATH/src_templates
	fi

	local templates_count=`ls -1A "$UCI_DB_PATH/src_templates" | wc -l`
	if [ $templates_count -eq 0 ]; then
		local prog=`$UCI get meta-factory.merge.prog`
		local path="$DEFAULT_DB_PATH/$prog"
		# not UGW SDL complient
		cp $path/* $UCI_DB_PATH/src_templates
	fi
}

function do_meta_factory_init(){
        check_meta_factory
        TMP_CONF_FILE=$(mktemp /tmp/tmpConfFile.XXXXXX)
}

function reset_on_init(){
	if [ -e "/nvram/wifi_reset_indicator" ]; then
		full_reset "$@"
		rm /nvram/wifi_reset_indicator > /dev/null 2>&1
		return
	fi

	if [ ! -s $UCI_DB_PATH/wireless ]; then
		full_reset "$@"
		return
	fi

	if [ ! -f $UCI_DB_PATH/meta-factory ]; then
		return
	fi

	local merge_stat=`$UCI get $MERGE_PARAM 2>/dev/null`

	local prev_checksum=`$UCI get meta-factory.merge.checksum 2>/dev/null`
	local curr_checksum=`cat $DEFAULT_DB_PATH/* | md5sum | awk '{print $1}'`
	if [ "$prev_checksum" != "$curr_checksum" ]; then
		if [ "$merge_stat" = "1" ]
		then
			if [ -f $DEFAULT_DB_PATH/user_preference ]; then
				partial_merge_reset "$@"
			else
				complete_merge_reset "$@"
			fi
		fi
		update_templates
	fi

	if [ "$merge_stat" = "1" ]
	then
		if [ -f $DEFAULT_DB_PATH/obligatory_settings ]; then
			obligatory_override
		fi

		$UCI commit wireless
	fi
}

# MAC address functions

# Check the MAC address of the interface.
function check_mac_address(){
	local board_mac_check ret_val

	board_mac_check=$1

	ret_val=1

	board_mac_check=${board_mac_check//:/""}
	if [ "${board_mac_check//[0-9A-Fa-f]/}" != "" ]; then
		print_logs "$0: wrong MAC Address format!"
		ret_val=0
	fi

	echo $ret_val
}

update_all_mac_address(){
	local radio_names=`$UCI show wireless | sed 's/[a-z0-9A-Z-]*\.//' | sed 's/\..*//' | sed 's/\=.*//' | sort -u`
	local interface
	local macaddr
	local is_ap
	local main_index
	local sta_index
	old_ifs=$IFS
	IFS=$'\n'
	for radio_name in $radio_names; do
		interface=`$UCI get wireless.$radio_name.ifname 2>/dev/null`
		if [ -z "$interface" ]; then
			is_ap="1"
			P_radios=`$UCI show wireless | grep ifname | grep -v -E "wlan.\." |  sed 's/[a-z0-9A-Z-]*\.//' | sed 's/\..*//'`
			for P_radio in $P_radios; do
				if [ ""`$UCI get wireless.$P_radio.mode` = "ap" -a ""`$UCI get wireless.$P_radio.device` = "$radio_name" ]; then
					interface=`$UCI get wireless.$P_radio.ifname`
				fi
			done
		else
			`$UCI show wireless.$radio_name.mode | grep sta > /dev/null `
			is_ap=$?
		fi
		if [ "$is_ap" = "0" ]; then
			sta_index=`echo $interface | sed 's/[a-zA-Z-]*//'`
			let main_index=$sta_index-1
			macaddr=`update_mac_address wlan$main_index sta`
		else
			macaddr=`update_mac_address $interface`
		fi
		$UCI set wireless.$radio_name.macaddr=$macaddr
	done
	IFS=$old_ifs
	$UCI commit wireless
}

# Calculate and update the MAC address of the interface.
update_mac_address()
{

	# Define local parameters
	local interface_name radio_name radio_index vap_index phy_offset board_mac mac_address \
	board_mac1 board_mac23 board_mac46 vap_mac4 vap_mac5 vap_mac6

	interface_name=$1
	sta_flag=$2

	#for station use master interface (for example for wlan1 radio_name should be wlan0).
	radio_name=${interface_name%%.*}
	[ "$radio_name" = "wlan0" ] && phy_offset=16
	[ "$radio_name" = "wlan2" ] && phy_offset=33
	[ "$radio_name" = "wlan4" ] && phy_offset=50

	if [ "$radio_name" == "$interface_name" ] ; then
		if [ "$sta_flag" == "sta" ] ; then
			vap_index=1
		else
			# master VAP
			vap_index=0
		fi
	else
		# slave VAPs
		vap_index=${interface_name##*.}
		vap_index=$((vap_index+2))
	fi

	rdk_error=0
	if [ "$OS_NAME" = "RDKB" ]; then
		mac_address=`itstore_mac_db $((${radio_name##wlan}/2)) ${vap_index}`
		if [ "$(check_mac_address $mac_address)" != "1" ]; then
			print_logs "$0: ERROR: Retrival of default MAC Address from ITstore Failed !"
			print_logs "$0: Check the ITstore Production settings and get the correct Mac address"
			rdk_error=1
		fi
	fi

	if [ "$OS_NAME" = "UGW" -o "$rdk_error" = "1" ]; then
		if [ -e "/nvram/etc/wave/wav_base_mac" ]; then
			source /nvram/etc/wave/wav_base_mac
			board_mac=${board_mac##*HWaddr }
			board_mac=${board_mac%% *}
		elif [ "$rdk_error" = "1" ]; then
			board_mac="00:50:F1:80:00:00"
		else
			board_mac=`ifconfig eth0_1`
			board_mac=${board_mac##*HWaddr }
			board_mac=${board_mac%% *}
		fi

		if [ "$board_mac" == "00:50:F1:64:D7:FE" ] || [ "$board_mac" == "00:50:F1:80:00:00" ]
		then
			print_logs "$0:  USING DEAFULT MAC, MAC should be different than 00:50:F1:64:D7:FE and 00:50:F1:80:00:00 ##"
		fi

		# Divide the board MAC address to the first 3 bytes and the last 3 byte (which we are going to increment).
		board_mac1=0x`echo $board_mac | cut -c 1-2`
		board_mac23=`echo $board_mac | cut -c 4-8`
		board_mac46=0x`echo $board_mac | sed s/://g | cut -c 7-12`

		# Increment the last byte by the the proper incrementation according to the physical interface (wlan0/wlan2/wlan4)
		board_mac46=$((board_mac46+phy_offset))

		# For STA, use MAC of physical AP incremented by 1 (wlan1 increment wlan0 by 1).
		# For VAP, use MAC of physical AP incremented by the index of the interface name + 2 (wlan0.0 increment wlan0 by 2, wlan2.2 increment wlan2 by 2).
		board_mac46=$((board_mac46+$vap_index))

		# Generate the new MAC.
		vap_mac4=$((board_mac46/65536))
		board_mac46=$((board_mac46-vap_mac4*65536))
		vap_mac5=$((board_mac46/256))
		board_mac46=$((board_mac46-vap_mac5*256))
		vap_mac6=$board_mac46
		# If the 4th byte is greater than FF (255) set it to 00.
		[ $vap_mac4 -ge 256 ] && vap_mac4=0

		mac_address=`printf '%02X:%s:%02X:%02X:%02X' $board_mac1 $board_mac23 $vap_mac4 $vap_mac5 $vap_mac6`
	fi
	echo "$mac_address"
}

function do_update_mac_address(){
        if [ -n "`cat "$ETC_CONFIG_WIRELESS_PATH" 2>/dev/null | grep wifi-device 2>/dev/null`" ]; then
                if [ "$1" = "-m" ] || [ -z "`$UCI show wireless | grep macaddr`" ]; then
                        update_all_mac_address
                        exit 0
                fi
        fi
}

# Normal templates functions

function set_conf_to_file(){
	local old_ifs
	rpc_idx=$1
	specific_conf_file=$2
	template_conf_file=$3
	output_conf_file=$4
	do_not_add_suffix=$5

	if [ -f $specific_conf_file ]; then
		local tmp_conf_file=$(mktemp /tmp/Local_tmp_conf_file.XXXXXX)
		if [ -z $do_not_add_suffix ]; then
			cat $template_conf_file | sed "s,option ssid '[^']*,&_$rpc_idx,g" > $tmp_conf_file
		else
			cat $template_conf_file > $tmp_conf_file
		fi

		arr=`cat $specific_conf_file | grep "option" | awk '{ print $2 }' | uniq`
		old_ifs=$IFS
		IFS=$'\n'
		for item in $arr
		do
			sed -i "/$item/d" $tmp_conf_file
		done
		IFS="$old_ifs"

		cat $specific_conf_file >> $tmp_conf_file
		cat $tmp_conf_file >> $output_conf_file
		rm $tmp_conf_file
	else
		# save all the rest of the defaults + add suffix to ssid
		if [ -z $do_not_add_suffix ]; then
			cat $template_conf_file | sed "s,option ssid '[^']*,&_$rpc_idx,g" >> $output_conf_file
		else
			cat $template_conf_file >> $output_conf_file
		fi
	fi
}

function use_template_single(){
	local tmp;
	local old_ifs;
	rpc_idx=$1
	template_conf_file=$2
	add_suffix=$3

	if [ ! -f $template_conf_file ]; then
		return;
	fi

	if [ -n add_suffix ]; then
		tmp=`cat $template_conf_file | sed "s,option ssid '[^']*,&_$rpc_idx,g"`
	else
		tmp=`cat $template_conf_file`
	fi

	old_ifs=$IFS
	IFS=$'\n'
	for line in $tmp; do
		uci_set_helper "$line" "$tmp_wireless"
	done
	IFS=$old_ifs
}

# radio_map_file functions

radio_map_file_content=
get_index_from_map_file_return_value=
function get_index_from_map_file(){
	local phy_name=$1
	local pciIndex=""
	if [ "$OS_NAME" = "UGW" ]; then
		pciIndex=$(grep PCI_SLOT_NAME /sys/class/ieee80211/${phy_name}/device/uevent | cut -d"=" -f2 | awk -F":" '{print $1}') #For LGM
	else
		pciIndex=$(grep PCI_SLOT_NAME /sys/class/ieee80211/${phy_name}/device/uevent | awk -F":" '{print $2}') #For Xb7
	fi
	local map_line=$(echo "$radio_map_file_content" | grep -nm1 PCI_SLOT${pciIndex})
	local radio_name=$(echo "$map_line" | awk '{print $1}' | awk -F":" '{print $2}')
	local line_number=$(echo "$map_line" | awk '{print $1}' | awk -F":" '{print $1}')

	# delete the used line
	radio_map_file_content=$(echo "$radio_map_file_content" | sed "${line_number}d")

	get_index_from_map_file_return_value=${radio_name//[a-z]/}
}

function validate_radio_map_file(){
	if [ ! -f "$RADIO_MAP_FILE" ]; then
		# no map file supplied
		return
	fi

	radio_map_file_content=$(cat $RADIO_MAP_FILE)
	local radio_index
	local phys=`ls /sys/class/ieee80211/`
	for phy in $phys; do
		get_index_from_map_file $phy
		radio_index=$get_index_from_map_file_return_value
		if [ -z "$radio_index" ]; then
			print_logs "RADIO_MAP_FILE is invalid. not using it"
			radio_map_file_content=
			return
		fi
	done

	print_logs "using radio map configuration file"
	radio_map_file_content=$(cat $RADIO_MAP_FILE)
}

# phys_conf_file functions

get_phys_from_file_return=
function get_phys_from_file(){
	if [ -z "$PHYS_CONF_FILE" ] || [ ! -f "$PHYS_CONF_FILE" ]; then
		print_logs "ERROR:no phys_conf_file supplied, provide one in the db folder (at $PHYS_CONF_FILE)"
		exit 1;
	fi
	get_phys_from_file_return=`cat "$PHYS_CONF_FILE" | grep phy | sed 's/...=//'`
}

get_value_from_phy_file_return="-1"
function get_value_from_phy_file(){
	local tmp=`cat $PHYS_CONF_FILE `
	get_value_from_phy_file_return="-1"
	local ind="0";
	IFS=$'\n'
	for line in $tmp; do
		if [ "$ind" = "1" -a `echo $line | sed 's/\=.*//'` = "$2" ]; then
			get_value_from_phy_file_return=`echo ${line#*=}`
			IFS=$old_ifs
			return
		fi
		if [ "$ind" = "1" -a `echo $line | sed 's/\=.*//'` = "phy" ]; then
			# passed our phy, this is another one
			IFS=$old_ifs
			return;
		fi
		if [ "$line" = "phy=$1" ]; then
			ind="1";
		fi
	done
	IFS=$old_ifs
}

# factory merge functions

function update_templates(){
	local merge=`$UCI get $MERGE_PARAM`
	# not UGW SDL complient
	rm -rf $UCI_DB_PATH/src_templates 2>/dev/null
	mkdir  $UCI_DB_PATH/src_templates
	cp $DEFAULT_DB_PATH/* $UCI_DB_PATH/src_templates

	local new_checksum=`cat $UCI_DB_PATH/src_templates/* | md5sum | awk '{print $1}'`
	$UCI set meta-factory.merge.checksum=$new_checksum
	$UCI commit meta-factory
}

function check_preference(){
	local file_name=$1
	local section_name=$2
	local param_name=$3
	local section_type=$4
	local pref=""

	if [ "$param_name" = "" ]; then
		pref=`$UCI -c $DEFAULT_DB_PATH/ get ${file_name}.entire_section.${section_name} 2>/dev/null`
	else
		pref=`$UCI -c $DEFAULT_DB_PATH/ get $file_name.$section_name.$param_name 2>/dev/null`
		if [ "$pref" = "" ]; then
			if [ "$section_type" = "wifi-device" ]; then
				pref=`$UCI -c $DEFAULT_DB_PATH/ get ${file_name}.radioall.${param_name} 2>/dev/null`
			elif [ "$section_type" = "wifi-iface" ]; then
				pref=`$UCI -c $DEFAULT_DB_PATH/ get ${file_name}.default_radioall.${param_name} 2>/dev/null`
			fi
		fi
	fi

	if [ "$pref" = "1" ]; then
		echo 1
	else
		echo 0
	fi
}

function save_entire_section(){
	local file_name=$1
	local section_name=$2

	while read line; do
		local curr_type=`echo $line | awk '{print $1}'`
		if [ "$curr_type" = "config" ]; then
			local cur_section_name=`echo $line | awk '{print $3}' | sed "s/'//g"`
			local cur_section_type=`echo $line | awk '{print $2}'`
		elif [[ "$curr_type" = "option" || "$curr_type" = "list" ]]; then
			local param=`echo $line | awk '{print $2}'`
			local value=`echo $line | awk '{print $3}' | sed "s/'//g"`
		else
			continue
		fi

		if [ "$cur_section_name" != "$section_name" ]; then
			continue
		fi

		if [ "$curr_type" = "config" ]; then
			echo "set wireless.$cur_section_name=$cur_section_type" >> $file_name
		elif [ "$curr_type" = "option" ]; then
			echo "set wireless.$cur_section_name.$param=$value" >> $file_name
		elif [ "$curr_type" = "list" ]; then
			echo "add_list wireless.$cur_section_name.$param=$value" >> $file_name
		fi
	done < $UCI_DB_PATH/wireless
}

function partial_merge_reset(){
	local tmp_partial=`mktemp /tmp/partial.XXXXXX`
	local section_name=""
	local section_type=""
	local skip_section=0

	while read line; do
		local curr_type=`echo $line | awk '{print $1}'`
		if [ "$curr_type" = "config" ]; then
			section_name=`echo $line | awk '{print $3}' | sed "s/'//g"`
			section_type=`echo $line | awk '{print $2}'`

			local section_pref=`check_preference "user_preference" "$section_name"`
			if [ "$section_pref" = "1" ]; then
				save_entire_section "$tmp_partial" "$section_name"
				skip_section="1"
			else
				skip_section="0"
			fi

			continue

		elif [ "$skip_section" -ne "1" ] && [ "$curr_type" = "option" -o "$curr_type" = "list" ]; then
			local param=`echo $line | awk '{print $2}'`
			local value=`echo $line | awk '{print $3}' | sed "s/'//g"`
		else
			continue
		fi

		local param_pref=`check_preference "user_preference" "$section_name" "$param" "$section_type"`
		if [ "$param_pref" != "1" ]; then
			continue
		fi

		local idx=`echo $section_name | sed 's/[^0-9]*//g'`
		if [ -z "$idx" ]; then
			continue
		fi
		if [ $idx -ge $DUMMY_VAP_OFSET ]; then
			continue
		fi

		if [ "$curr_type" = "option" ]; then
			echo "set wireless.$section_name.$param=$value" >> $tmp_partial
		elif [ "$curr_type" = "list" ]; then
			echo "add_list wireless.$section_name.$param=$value" >> $tmp_partial
		fi
	done < $UCI_DB_PATH/wireless

	full_reset

	while read line; do
		local curr_type=`echo $line | awk '{print $1}'`
		if [ "$curr_type" = "config" ]; then
			section_name=`echo $line | awk '{print $3}' | sed "s/'//g"`
			section_type=`echo $line | awk '{print $2}'`

			local section_pref=`check_preference "user_preference" "$section_name"`
			if [ "$section_pref" = "1" ]; then
				$UCI delete wireless.$section_name
				skip_section="1"
			else
				skip_section="0"
			fi

			continue

		elif [ "$skip_section" -ne "1" ] && [ "$curr_type" = "option" -o "$curr_type" = "list" ]; then
			local param=`echo $line | awk '{print $2}'`
			local value=`echo $line | awk '{print $3}' | sed "s/'//g"`
		else
			continue
		fi

		local param_pref=`check_preference "user_preference" "$section_name" "$param" "$section_type"`
		if [ "$param_pref" != "1" ]; then
			continue
		fi

		local idx=`echo $section_name | sed 's/[^0-9]*//g'`
		if [ -z "$idx" ]; then
			continue
		fi
		if [ $idx -ge $DUMMY_VAP_OFSET ]; then
			continue
		fi

		if [ "$curr_type" = "option" ]; then
			$UCI delete wireless.$section_name.$param
		elif [ "$curr_type" = "list" ]; then
			$UCI del_list wireless.$section_name.$param=$value
		fi
	done < $UCI_DB_PATH/wireless

	while read line; do
		$UCI $line
	done < $tmp_partial

	rm $tmp_partial
}

function reduce_num_of_vaps_to(){
	local num=$1
	local count=0
	local radio=$2
	local all_vaps=`$UCI show wireless | grep wifi-iface | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`

	for vap in $all_vaps
	do
		local idx=`echo $vap | sed 's/[^0-9]*//g'`
		if [ -z "$idx" ]; then
			continue
		fi
		if [ $idx -ge $DUMMY_VAP_OFSET ]; then
			continue
		fi

		local curr_mode=`$UCI get wireless.$vap.mode`
		if [ "$curr_mode" = "sta" ]; then
			continue
		fi

		local curr_radio=`$UCI get wireless.$vap.device`
		if [ "$curr_radio" != "$radio" ]; then
			continue
		fi

		count=$((count+1))

		if [ $count -gt $num ]; then
			local rpc_idx=`$UCI get wireless.$vap.rpc_index`
			$UCI del wireless.$vap
			$UCI del wireless.vap_rpc_indexes.index$rpc_idx
		fi
	done
}

function increase_num_of_vaps_by(){
	local num=$1
	local count=0
	local radio=$2
	local last_vap=""
	local tmp_wireless=$(mktemp /tmp/wireless.XXXXXX)
	local all_vaps=`$UCI show wireless | grep wifi-iface | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`

	for vap in $all_vaps
	do
		local idx=`echo $vap | sed 's/[^0-9]*//g'`
		if [ -z "$idx" ]; then
			continue
		fi
		if [ $idx -ge $DUMMY_VAP_OFSET ]; then
			continue
		fi

		local curr_radio=`$UCI get wireless.$vap.device`
		local curr_mode=`$UCI get wireless.$vap.mode`
		if [[ "$curr_radio" = "$radio" && "$curr_mode" = "ap" ]]; then
			local last_vap_idx=`echo $vap | sed 's/[^0-9]*//g'`
		fi
	done

	uci_idx=$last_vap_idx

	local last_ifname=`$UCI get wireless.default_radio${uci_idx}.ifname`

	while [ $count -lt $num ]
	do
		local new_ifname=`echo $last_ifname | awk -F"." '{print $1}'`
		local ifname_sfx=`echo $last_ifname | awk -F"." '{print $2}'`
		local iface_index=`echo $new_ifname | sed 's/[^0-9]*//g'`
		ifname_sfx=$((ifname_sfx+1))
		new_ifname="${new_ifname}.${ifname_sfx}"
		last_ifname=$new_ifname

		local num_of_phys=`ls /sys/class/ieee80211/ | wc -l`

		uci_idx=$((uci_idx+1))

		local rpc_idx=$((iface_index/2+ifname_sfx*num_of_phys))
		$UCI set wireless.vap_rpc_indexes.index$rpc_idx=$uci_idx

		uci_set_helper "config wifi-iface 'default_radio$uci_idx'" $tmp_wireless
		uci_set_helper "    option rpc_index '$rpc_idx'" $tmp_wireless
		uci_set_helper "    option device '$radio'" $tmp_wireless
		uci_set_helper "    option ifname '$new_ifname'" $tmp_wireless
		new_mac=`update_mac_address $new_ifname`
		uci_set_helper "    option macaddr '$new_mac'" $tmp_wireless

		tmp_vap_conf_file=$(mktemp /tmp/vap_conf_file.XXXXXX)
		#generic vap db parameters
		use_templates $uci_idx $DEFAULT_DB_VAP_SPECIFIC$uci_idx $DEFAULT_DB_VAP $tmp_vap_conf_file

		local curr_band=`$UCI get wireless.$radio.band`

		#band specific vap db parameters
		if [ $curr_band = "6GHz" ]
		then
			use_templates_tmp_file $uci_idx $DEFAULT_DB_VAP_6G $tmp_vap_conf_file $tmp_wireless
		elif [ "$curr_band" = "5GHz" ]; then
			use_templates_tmp_file $uci_idx $DEFAULT_DB_VAP_5G $tmp_vap_conf_file $tmp_wireless
		else
			use_templates_tmp_file $uci_idx $DEFAULT_DB_VAP_24G $tmp_vap_conf_file $tmp_wireless
		fi
		rm -f $tmp_vap_conf_file

		count=$((count+1))
	done

	add_changes_commit "$tmp_wireless"
}

function update_num_of_vaps(){
	local radio=$1
	local old_num=$2
	local new_num=$3
	local count=0
	local all_vaps""

	if [ "$old_num" = "$new_num" ]; then
		# no change -> do nothing
		return
	fi

	all_vaps=`$UCI show wireless | grep wifi-iface | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`
	for vap in $all_vaps
	do
		local idx=`echo $vap | sed 's/[^0-9]*//g'`
		if [ -z "$idx" ]; then
			continue
		fi
		if [ $idx -ge $DUMMY_VAP_OFSET ]; then
			continue
		fi

		local curr_mode=`$UCI get wireless.$vap.mode 2>/dev/null`
		if [ "$curr_mode" = "sta" ]; then
			continue
		fi

		local curr_radio=`$UCI get wireless.$vap.device 2>/dev/null`
		if [ "$curr_radio" = "$radio" ]; then
			count=$((count+1))
		fi
	done

	if [ $count -eq $old_num ]; then
		if [ $count -gt $new_num ]; then
			reduce_num_of_vaps_to $new_num $radio
		else
			local add_vaps=$((new_num-count))
			increase_num_of_vaps_by $add_vaps $radio
		fi
	fi
}

function template_diff(){
	local old_template=$1
	local new_template=$2
	local main_template=$3
	local output=""

	if [[ ! -f $old_template && ! -f $new_template ]]; then
		#do nothing
		print_logs "No templates provided"
	elif [ ! -f $old_template ]; then
		while read line; do
			local cur_type=`echo $line | awk '{print $1}'`
			local cur_param=`echo $line | awk '{print $2}'`
			local cur_value=`echo $line | awk '{print $3}'`

			if [ "$cur_type" = "list" ]; then
				output="$output add_list"
			elif [ "$cur_type" = "option" ]; then
				output="$output set"
			else
				print_logs "warning: invalid line in $new_template"
				continue
			fi

			output="$output $cur_param=$cur_value"
		done < $new_template
	elif [ ! -f $new_template ]; then
		while read line; do
			local cur_type=`echo $line | awk '{print $1}'`
			local cur_param=`echo $line | awk '{print $2}'`
			local cur_value=""

			if [ "$cur_type" = "list" ]; then
				output="$output del_list"
				cur_value=`echo $line | awk '{print $3}'`
			elif [ "$cur_type" = "option" ]; then
				if [ -f "$main_template" ]; then
					local tmp=`cat $main_template | grep "$cur_type $cur_param"`
					if [ ! "$tmp" = "" ]; then
						cur_value=`echo $tmp | awk '{print $3}'`
						output="$output set $cur_param=$cur_value"
						continue
					fi
				fi
				output="$output del"
			else
				print_logs "warning: invalid line in $old_template"
				continue
			fi

			output="$output $cur_param"
			if [ "$cur_value" != "" ]; then
				output="$output=$cur_value"
			fi
		done < $old_template
	else
		while read line; do
			local cur_type=`echo $line | awk '{print $1}'`
			local cur_param=`echo $line | awk '{print $2}'`

			if [ "$cur_type" = "list" ]; then
				local cur_value=`echo $line | awk '{print $3}'`
				local tmp=`cat $new_template | grep "$cur_type $cur_param $cur_value"`
				if [ "$tmp" = "" ]; then
					output="$output del_list $cur_param=$cur_value"
				fi
			elif [ "$cur_type" = "option" ]; then
				local tmp=`cat $new_template | grep "$cur_type $cur_param "`
				if [ "$tmp" = "" ]; then
					if [ -f "$main_template" ]; then
						local tmp=`cat $main_template | grep "$cur_type $cur_param"`
						if [ ! "$tmp" = "" ]; then
							cur_value=`echo $tmp | awk '{print $3}'`
							output="$output set $cur_param=$cur_value"
							continue
						fi
					fi
					output="$output del $cur_param"
				fi
			else
				print_logs "warning: invalid line in $old_template"
				continue
			fi
		done < $old_template

		while read line; do
			local cur_type=`echo $line | awk '{print $1}'`
			local cur_param=`echo $line | awk '{print $2}'`
			local cur_value=`echo $line | awk '{print $3}'`

			local tmp=`cat $old_template | grep "$cur_type $cur_param $cur_value"`
			if [ "$tmp" = "" ]; then
				if [ "$cur_type" = "list" ]; then
					output="$output add_list"
				elif [ "$cur_type" = "option" ]; then
					output="$output set"
				else
					print_logs "warning: invalid line in $new_template"
					continue
				fi

				output="$output $cur_param=$cur_value"
			fi
		done < $new_template
	fi

	echo $output
}

function apply_diff(){
	local old_tmpl=$1
	local section=$2
	local prev_param=""
	shift
	shift

	while [ "$1" != "" ]
	do
		local option=$1
		shift
		local change=`echo $1 | sed "s/'//g"`
		shift

		local param=`echo $change | awk -F"=" '{print $1}'`

		local orig_val=`cat $old_tmpl | grep " $param " | awk '{print $3}' | awk -v RS=  '{$1=$1}1' | sed "s/'//g"  2>/dev/null`
		if [ "$param" = "$prev_param" ]; then
			# not the first element of the list, no need to check
			$UCI $option wireless.${section}.${change}
		else
			local user_val=`$UCI get wireless.$section.$param 2>/dev/null`
			if [ "$orig_val" = "$user_val" ]; then
				$UCI $option wireless.${section}.${change}
				prev_param=$param # save in case it's a list element
			fi
		fi
	done
}

# TODO: add support for 6g in merge functions
function complete_merge_reset_radio(){
	local radio=$1

	local band=`$UCI get wireless.${radio}.band | sed 's/[^0-9]*//g' 2>/dev/null`
	local radio_diff=`template_diff "$UCI_DB_PATH/src_templates/wireless_def_radio_${band}g" "$DEFAULT_DB_PATH/wireless_def_radio_${band}g"`
	apply_diff "$UCI_DB_PATH/src_templates/wireless_def_radio_${band}g" "$radio" $radio_diff

	if [ $band = '5' ]; then
		local iface_idx=`echo "$radio" | sed 's/[^0-9]*//g'`

		`cat /proc/net/mtlk/wlan${iface_idx}/radio_cfg | grep "zw-dfs ant:" | grep "true" > /dev/null`
		if [ $? -eq 0 ]; then
			local main_template="$DEFAULT_DB_PATH/wireless_def_radio_${band}g"
			local old_tmpl="$UCI_DB_PATH/src_templates/wireless_def_radio_${band}g_zw_dfs"
			local new_tmpl="$DEFAULT_DB_PATH/wireless_def_radio_${band}g_zw_dfs"
			radio_diff=`template_diff "$old_tmpl" "$new_tmpl" "$main_template"`
			apply_diff "$old_tmpl" "$radio" $radio_diff
		fi
	fi
}

function complete_merge_reset_vap(){
	local vap=$1

	local idx=`echo $vap | sed 's/[^0-9]*//g'`
	if [ -z "$idx" ]; then
		return
	fi
	if [ $idx -ge $DUMMY_VAP_OFSET ]; then
		return
	fi

	local mode=`$UCI get wireless.${vap}.mode 2>/dev/null`
	local vap_device=`$UCI get wireless.${vap}.device 2>/dev/null`
	local vap_band=`$UCI get wireless.${vap_device}.band 2>/dev/null`
	local vap_db_band

	if [ "$vap_band" = "5GHz" ]; then
		vap_db_band=5g
	elif [ "$vap_band" = "6GHz" ]; then
		vap_db_band=6g
	else
		vap_db_band=24g
	fi

	if [ "$mode" = "sta" ]; then
		local old_tmpl="$UCI_DB_PATH/src_templates/wireless_def_station_vap"
	else
		local old_tmpl="$UCI_DB_PATH/src_templates/wireless_def_vap_db"
	fi

	if [ "$mode" = "sta" ]; then
		local new_tmpl="$DEFAULT_DB_PATH/wireless_def_station_vap"
	else
		local new_tmpl="$DEFAULT_DB_PATH/wireless_def_vap_db"
	fi

	local vap_diff=`template_diff "$old_tmpl" "$new_tmpl"`
	apply_diff "$old_tmpl" "$vap" $vap_diff

	local main_template="$new_tmpl"

	local by_band_old_template="$UCI_DB_PATH/src_templates/wireless_def_vap_db_${vap_db_band}"
	local by_band_new_template="$DEFAULT_DB_PATH/wireless_def_vap_db_${vap_db_band}"
	local by_idx_old_template="$UCI_DB_PATH/src_templates/wireless_def_vap_${idx}"
	local by_idx_new_template="$DEFAULT_DB_PATH/wireless_def_vap_${idx}"
	local tmp_template_file_old=$(mktemp /tmp/Local_tmp_template_file_old.XXXXXX)
	local tmp_template_file_new=$(mktemp /tmp/Local_tmp_template_file_new.XXXXXX)

	local sec_old_template="$by_idx_old_template"
	local sec_new_template="$by_idx_new_template"

	if [ "$mode" = "ap" ]; then
		if [ -f $by_band_old_template ]; then
			if [ -f $by_idx_old_template ]; then
				set_conf_to_file 0 "$by_idx_old_template" "$by_band_old_template" "$tmp_template_file_old" 0
				sec_old_template=$tmp_template_file_old
			else
				sec_old_template=$by_band_old_template
			fi
		fi
		if [ -f $by_band_new_template ]; then
			if [ -f $by_idx_new_template ]; then
				set_conf_to_file 0 "$by_idx_new_template" "$by_band_new_template" "$tmp_template_file_new" 0
				sec_new_template=$tmp_template_file_new
			else
				sec_new_template=$by_band_new_template
			fi
		fi
	fi

	if [[ -f $sec_old_template || -f $sec_new_template ]]; then
		local vap_diff=`template_diff "$sec_old_template" "$sec_new_template" "$main_template"`
		apply_diff "$sec_old_template" "$vap" $vap_diff
	fi
	rm $tmp_template_file_old
	rm $tmp_template_file_new
}

# TODO: add support for 6g in merge functions
function complete_merge_reset(){
	local section_type="$1"
	local section_name="$2"

	if [ ! -d $UCI_DB_PATH/src_templates ]; then
		return
	fi

	local all_radios=`$UCI show wireless | grep wifi-device | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`
	local all_vaps=`$UCI show wireless | grep wifi-iface | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`


	for radio in $all_radios
	do
		local band=`$UCI get wireless.${radio}.band | sed 's/[^0-9]*//g' 2>/dev/null`
		band="${band}g"
		if [ $band = '5g' ]; then
			local iface_idx=`echo "$radio" | sed 's/[^0-9]*//g'`
			`cat /proc/net/mtlk/wlan${iface_idx}/radio_cfg | grep "zw-dfs ant:" | grep "true" > /dev/null`
			if [ $? -eq 0 ]; then
				band=${band}_zw_dfs
			fi
		fi

		local pref_num_of_vaps=`$UCI -c $UCI_DB_PATH/src_templates/ get defaults.num_vaps.${band}`
		local new_num_of_vaps=`$UCI -c $DEFAULT_DB_PATH/ get defaults.num_vaps.${band}`
		update_num_of_vaps "$radio" "$pref_num_of_vaps" "$new_num_of_vaps"

		complete_merge_reset_radio "$radio"
	done

	for vap in $all_vaps
	do
		complete_merge_reset_vap "$vap"
	done
}

function override_radio(){
	local radio=$1
	local param=$2

	if [ ! -d $UCI_DB_PATH/src_templates ]; then
		return
	fi

	local band=`$UCI get wireless.$radio.band | sed 's/[^0-9]*//g' 2>/dev/null`
	local value=`cat $UCI_DB_PATH/src_templates/wireless_def_radio_${band}g | grep $param | awk '{print $3}' | sed "s/'//g" 2>/dev/null`
	$UCI set wireless.${radio}.${param}=${value}
}

function override_vap(){
	local vap=$1
	local param=$2

	if [ ! -d $UCI_DB_PATH/src_templates ]; then
		return
	fi

	local mode=`UCI get wireless.${vap}.mode 2>/dev/null`
	local idx=`echo $vap | sed 's/[^0-9]*//g'`
	local vap_device=`$UCI get wireless.${vap}.device 2>/dev/null`
	local vap_band=`$UCI get wireless.${vap_device}.band 2>/dev/null`
	local vap_db_band

	if [ "$vap_band" = "5GHz" ]; then
		vap_db_band=5g
	elif [ "$vap_band" = "6GHz" ]; then
		vap_db_band=6g
	else
		vap_db_band=24g
	fi

	if [ -f $UCI_DB_PATH/src_templates/wireless_def_vap_${idx} ]; then
		local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_${idx} | grep $param | awk '{print $3}' | sed "s/'//g"`
		if [ "$value" = "" ]; then
			if [ -f $UCI_DB_PATH/src_templates/wireless_def_vap_db_${vap_db_band} ]; then
				local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_db_${vap_db_band} | grep $param | awk '{print $3}' | sed "s/'//g"`
				if [ "$value" = "" ]; then
					local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_db | grep $param | awk '{print $3}' | sed "s/'//g"`
				fi
			else
				local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_db | grep $param | awk '{print $3}' | sed "s/'//g"`
			fi
		fi
	elif [ "$mode" = "sta" ]; then
		local value=`cat $UCI_DB_PATH/src_templates/wireless_def_station_vap | grep $param | awk '{print $3}' | sed "s/'//g"`
	else
		if [ -f $UCI_DB_PATH/src_templates/wireless_def_vap_db_${vap_db_band} ]; then
			local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_db_${vap_db_band} | grep $param | awk '{print $3}' | sed "s/'//g"`
			if [ "$value" = "" ]; then
				local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_db | grep $param | awk '{print $3}' | sed "s/'//g"`
			fi
		else
			local value=`cat $UCI_DB_PATH/src_templates/wireless_def_vap_db | grep $param | awk '{print $3}' | sed "s/'//g"`
		fi
	fi

	$UCI set wireless.${vap}.${param}=${value}
}

function obligatory_override(){
	local section_name=""
	local section_type=""

	local all_radios=`$UCI show wireless | grep wifi-device | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`
	local all_vaps=`$UCI show wireless | grep wifi-iface | awk -F"." '{print $2}' | awk -F"=" '{print $1}' 2>/dev/null`

	while read line; do
		local curr_type=`echo $line | awk '{print $1}'`
		if [ "$curr_type" = "config" ]; then
			section_name=`echo $line | awk '{print $3}' | sed "s/'//g"`
			section_type=`echo $line | awk '{print $2}'`
			continue
		elif [[ $skip_section != 1 && "$curr_type" = "option" || "$curr_type" = "list" ]]; then
			local param=`echo $line | awk '{print $2}'`
			local param_pref=`echo $line | awk '{print $3}' | sed "s/'//g"`
		else
			continue
		fi

		if [ "$param_pref" != "1" ]; then
			continue
		fi

		local idx=`echo $section_name | sed 's/[^0-9]*//g'`
		if [[ "$idx" != "" && $idx -ge $DUMMY_VAP_OFSET ]]; then
			continue
		fi

		if [ "$section_type" = "wifi-device" ]; then
			if [ "$section_name" = "radioall" ]; then
				for radio in $all_radios
				do
					override_radio "$radio" "$param"
				done
			else
				override_radio "$section_name" "$param"
			fi
		elif [ "$section_type" = "wifi-iface" ]; then
			if [ "$section_name" = "default_radioall" ]; then
				for vap in $all_vaps
				do
					local idx=`echo $vap | sed 's/[^0-9]*//g'`
					if [ -z "$idx" ]; then
						continue
					fi
					if [ $idx -ge $DUMMY_VAP_OFSET ]; then
						continue
					fi

					override_vap "$vap" "$param"
				done
			else
				override_vap "$section_name" "$param"
			fi
		fi
    done < $DEFAULT_DB_PATH/obligatory_settings
}

# reset radio functions

function full_reset_radio(){
	local iface_name=$1
	local radio_idx=`echo $iface_name | sed "s/[^0-9]//g"`
	local phy_idx=`iw $iface_name info | grep wiphy | awk '{print $2}'`
	local phy="phy$phy_idx"
	local orig_band=`$UCI get wireless.radio$radio_idx.band | head -c1`

	if [ -z "$phy_idx" ]; then
		phy=`$UCI get wireless.radio$radio_idx.phy`
	fi

	# remove all configurable parameters, since there might be parameters which do not exist in the default db file.
	local extra_params=`$UCI show wireless.radio$radio_idx | grep "wireless.radio$radio_idx\." | grep -v "\.phy" \
	| grep -v "\.type" | grep -v "\.macaddr" | grep -v "\.rpc_index" | awk -F"=" '{print $1}'`

	for option in $extra_params
	do
		$UCI delete $option
	done

	local phy_band=`get_band "$phy" "$iface_name"`

	if [ -z "$phy_band" ]; then
		phy_band=$orig_band
	fi

	remove_dfs_state_file "$radio_idx"

	# the radio configuration files must be named in one of the following formats:
	# <file name>
	# <file name>_<radio idx>
	# <file name>_<radio idx>_<HW type>_<HW revision>
	local board=`iw dev $iface_name iwlwav gEEPROM | grep "HW type\|HW revision" | awk '{print $4}' | tr '\n' '_' | sed "s/.$//"`
	if [ $phy_band = "5" ]
	then
		set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_5}_${radio_idx} $DEFAULT_DB_RADIO_5  ${TMP_CONF_FILE}_
		`cat /proc/net/mtlk/${iface_name}/radio_cfg | grep "zw-dfs ant:" | grep "true" > /dev/null`
		if [ $? -eq 0 ]
		then
			set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_5}_zw_dfs ${TMP_CONF_FILE}_ ${TMP_CONF_FILE}_
		fi
		set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_5}_${radio_idx}_${board} ${TMP_CONF_FILE}_ $TMP_CONF_FILE
	elif [ $phy_band = "6" ]
	then
		set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_6}_${radio_idx} $DEFAULT_DB_RADIO_6  ${TMP_CONF_FILE}_
		set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_6}_${radio_idx}_${board} ${TMP_CONF_FILE}_ $TMP_CONF_FILE
	else
		set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_24}_${radio_idx} $DEFAULT_DB_RADIO_24  ${TMP_CONF_FILE}_
		set_conf_to_file $radio_idx ${DEFAULT_DB_RADIO_24}_${radio_idx}_${board} ${TMP_CONF_FILE}_ $TMP_CONF_FILE
	fi
	rm ${TMP_CONF_FILE}_
	local db_file=$TMP_CONF_FILE

	#set default params from template file
	while read line
	do
		local param=`echo $line | awk '{ print $2 }'`
		# read value from default db. remove extra \ and '
		local value=`echo $line | sed "s/[ ]*option $param //g" |  sed "s/[\\']//g"`

		$UCI set wireless.radio$radio_idx.$param="$value"
		local res=`echo $?`
		if [ $res -ne '0' ]; then
			print_logs "$0: Error setting $param..."
		fi
	done < $db_file
	rm -f $TMP_CONF_FILE
}

function reset_radio(){

	print_logs "$0: Performing radio reset for radio $1..."

	local iface=$1
	local radio_idx=`echo $iface | sed "s/[^0-9]//g"`
	local radio="radio$radio_idx"

	# check if wave_trusted_store binary exists
	if [ -f /usr/bin/wave_trusted_store ]; then
		local prog=`$UCI get meta-factory.merge.prog`
		wave_trusted_store factory "$prog" radio "$iface"
	fi

	full_reset_radio "$iface"

	$UCI commit wireless

	print_logs "$0: Done..."
}

# reset vap functions

function full_reset_vap(){
	local vap=$1
	local vap_idx=`echo $vap | sed "s/[^0-9]*//"`

	# remove all configurable parameters, since there might be parameters which do not exist in the default db file.
	local extra_params=`$UCI show wireless.$vap | grep "wireless.$vap\." | grep -v "\.device" \
	| grep -v "\.ifname" |  grep -v "\.macaddr" | grep -v "\.rpc_index" | awk -F"=" '{print $1}'`

	for option in $extra_params
	do
		$UCI delete $option
	done

	set_conf_to_file $vap_idx $DEFAULT_DB_VAP_SPECIFIC$vap_idx $DEFAULT_DB_VAP  $TMP_CONF_FILE

	#set default params from template file
	while read line
	do
		param=`echo $line | awk '{print $2}'`
		# read value from default db and add _<index> to SSID. remove extra \ and '
		value=`echo $line | sed "s/[ ]*option $param //g" |  sed "s/[\\']//g"`

		$UCI set wireless.$vap.$param="$value"
		res=`echo $?`
		if [ $res -ne '0' ]; then
			print_logs "$0: Error setting $param..."
		fi
	done < $TMP_CONF_FILE
	rm $TMP_CONF_FILE
}

function reset_vap(){

	print_logs "$0: Performing Vap reset for VAP $1 ..."

	local vap=`$UCI show wireless | grep ifname=\'$1\' | awk -F"." '{print $2}'`
	if [ X$vap = "X" ]; then
		print_logs "$0: Error, can't find VPA in UCI DB"
		exit 1
	fi

	# check if wave_trusted_store binary exists
	if [ -f /usr/bin/wave_trusted_store ]; then
		local prog=`$UCI get meta-factory.merge.prog`
		wave_trusted_store factory "$prog" vap $1
	fi

	full_reset_vap "$vap"

	$UCI commit wireless

	print_logs "$0: Done..."
}

# ubus related functions

function uci_set_helper_ubus(){
	local first;
	local second;
	local third;
	local set_to;
	local value;
	local input;
        # remove leading tabs and spaces, then parsing
        input=`echo "$1" | awk '{$1=$1};1' `
        type=`echo "$input" | cut -f 1 -d " "`
        set_to=`echo "$input" | cut -f 2 -d " "`
        value=${input#*$set_to }
        value=${value//\'/}
        if [ "$type" = "config" ]; then
                ubus call uci add '{ "config" : "wireless", "type" : "'$set_to'", "name" : "'$value'" }' > /dev/null 2>&1
                CURRENT_CONFIG="$value"
        else
                ubus call uci set '{ "config" : "wireless", "section" : "'$CURRENT_CONFIG'", "values" : { "'$set_to'" : "'$value'" } }'
        fi
}

function uci_parser() {
	local cmd=$1
	local argument=$2
	local section
	local set_to
	local value
	local cur_list
	local config=`echo $argument | sed 's/\..*//'`
	if [ "$config" = "-c" ]; then
		return
	fi
	if [ "$cmd" = "show" ]; then
		uci show "$argument"
	elif [ "$cmd" = "-c" ]; then
		uci "$@"
	elif [ "$cmd" = "commit" ]; then
		ubus call uci commit '{"config" : "'$config'" }'
	elif [ "$cmd" = "revert" ]; then
		ubus call uci revert '{"config" : "'$config'" }'
	elif [ "$cmd" = "get" ]; then
		uci get "$argument"
	elif [ "$cmd" = "changes" ]; then
		uci changes "$argument"
	elif [ "$cmd" = "delete" ] || [ "$cmd" = "del" ]; then
		section=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\..*//'`
		set_to=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | grep "\." | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\=.*//'`
		if [ -n "$set_to" ]; then
			ubus call uci delete '{ "config" : "'$config'", "section" : "'$section'", "option" : "'$set_to'" }'
		else
			ubus call uci delete '{ "config" : "'$config'", "section" : "'$section'" }'
		fi
	elif [ "$cmd" = "add_list" ]; then
		cur_list=`echo $argument | sed 's/\=.*//'`
		cur_list=`uci get $cur_list`
		cur_list="'${cur_list//[[:space:]]/"','"}'"
		cur_list=`echo $cur_list | tail -c +2 | head -c -2`
		section=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\..*//'`
		set_to=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\=.*//'`
		value=`echo ${argument#*=}`
		ubus call uci set "{ 'config' : '$config', 'section' : '$section', 'values' : { '$set_to' : [ '$cur_list', '$value' ] } }"
	elif [ "$cmd" = "del_list" ]; then
		cur_list=`echo $argument | sed 's/\=.*//'`
		cur_list=`uci get $cur_list`
		section=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\..*//'`
		set_to=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\=.*//'`
		value=`echo ${argument#*=}`
		local new_list=""
		for item in $cur_list; do
			if [ "$item" != "$value" ]; then
				new_list="$new_list $item"
			fi
		done
		new_list=`echo $new_list`
		new_list="'${new_list//[[:space:]]/"','"}'"
		new_list=`echo $new_list | tail -c +2 | head -c -2`
		ubus call uci set "{ 'config' : '$config', 'section' : '$section', 'values' : { '$set_to' : [ '$new_list' ] } }"
	elif [ "$cmd" = "set" ]; then
		section=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\..*//'`
		set_to=`echo $argument | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\=.*//'`
		value=`echo ${argument#*=}`
		ubus call uci set '{ "config" : "'$config'", "section" : "'$section'", "values" : { "'$set_to'" : "'$value'" } }'
	fi
}

# other functions

get_iface_idx_on_device(){
        if [ -n "$radio_map_file_content" ]; then
                get_index_from_map_file $1
                iface_idx=$get_index_from_map_file_return_value
        fi
}

get_band_on_device(){
	if [ `iw $1 info | grep -c "* 6... MHz"` -gt 0 ]
	then
		echo 6
	elif [ `iw $1 info | grep -c "* 5... MHz"` -gt 0 ]
	then
		echo 5
	elif [ `iw $1 info | grep -c "* 24.. MHz"` -gt 0 ]
	then
		echo 2
	fi
}

get_board_on_device(){
        board=`iw dev $iface iwlwav gEEPROM | grep "HW type\|HW revision" | awk '{print $4}' | tr '\n' '_' | sed "s/.$//"`
}

set_mac_address_on_device(){
        new_mac=`update_mac_address "$1" "$3"`
	uci_set_helper "        option macaddr '$new_mac'" "$2"
}

get_number_of_vaps(){
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
}

function uci_get_parser(){
	local cur_section=""
	local input
	local type
	local current
	local value
	local old_ifs=$IFS
	local filename=`echo $4 | sed 's/\..*//'`
	local section=`echo $4 | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\..*//'`
	local varname=`echo $4 | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/[_a-z0-9A-Z-]*\.//' | sed 's/\=.*//'`
	local tmp=`cat $2$filename`
	IFS=$'\n'
	for line in $tmp; do
		input=`echo "$line" | awk '{$1=$1};1' `
        	type=`echo "$input" | cut -f 1 -d " "`
		current=`echo "$input" | cut -f 2 -d " "`
		value=`echo "$input" | cut -f 3 -d " " | tr -d "'"`
		if [ "$type" = "config" ];then
			cur_section="$section"
		elif [ "$cur_section" = "$section" -a "$current" = "$varname" ]; then
			echo "$value"
			IFS="$old_ifs"
			return
		fi
	done
	IFS="$old_ifs"
}

function get_is_zwdfs_device(){
        `cat /proc/net/mtlk/wlan${1}/radio_cfg | grep "zw-dfs ant:" | grep "true" > /dev/null`
	if [ $? -eq 0 ]; then
		echo "1"
	else
		echo "0"
	fi
}

function is_station_supported_device(){
        `iw $1 info | grep -A1 "valid interface combinations" | grep "managed" > /dev/null`
	if [ $? -eq 0 ]; then
		echo "1"
	else
		echo "0"
	fi
}

function remove_dfs_state_file_device(){
	local location=`$UCI get wireless.radio$1.dfs_ch_state_file 2>/dev/null`
	if [ ! "$location" = ""  ]; then
		rm "$location"
	fi
}
