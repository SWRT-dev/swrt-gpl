#!/bin/sh

export OS_NAME="RDKB"
export BINDIR="/usr/sbin"
export SCRIPTS_PATH="/etc/wave/scripts"
export ETC_CONFIG_WIRELESS_PATH="/nvram/etc/config/wireless"
export DEV_CREAT_PATH="/dev"
export DEFAULT_DB_PATH="/etc/wave/db"
export RADIO_MAP_FILE="/etc/wave/db/radio_map_file"
export UCI_DB_PATH="/nvram/etc/config"
export BUSYBOX_BIN_PATH="/nvram"
export NC_COMMAND="/lib/netifd/lite_nc"
export WAVE_COMPONENTS_PATH="/etc/wave/bins"
export CAL_FILES_PATH="/nvram/etc/wave_calibration"
export CERTIFICATION_FILE_PATH="/nvram/etc/wave/certification_enabled"
export DEFAULT_IP_ADDRESS="10.0.0.1"

## for wlan debug collect
export WLAN_LOG_FILE_NAME=wlanLog
export SYSLOG_CONF=/etc/syslog-ng/syslog-ng.conf
export DEBUG_SYSLOG_CONF=/nvram/syslog-ng-debug.conf
export DUMP_HEADER_MAGIC="INTL"
export TMP_PATH=/tmp
export MY_NAME_DebugLog="logs_dump"
export TMP_LOGS_PATH=${TMP_PATH}/debug_logs
export WLAN_LOG_FILE_PATH=/var/log/${WLAN_LOG_FILE_NAME}
export WAVE600_FW_DUMP_FILE_PREFIX="/proc/net/mtlk/card"
export WAVE600_FW_DUMP_FILE_SUFFIX="/FW/fw_dump"
export WAVE500_FW_DUMP_FILE_PREFIX="/proc/net/mtlk/wlan"
export WAVE500_FW_DUMP_FILE_SUFFIX="/fw_dump"
export FW_DUMP_OUT_PATH=${TMP_LOGS_PATH}/fw_tmp_dump
export DUMP_IN_PATH=/tmp/fw_tmp_dump

export DB_LOC=/nvram/etc
export DUMP_LOC=${DB_LOC}
export DB_LOC_NAME=config
export CONF_LOC=/var/run
export NVRAM_LOGS_LOC=/nvram/logs
export VAR_LOGS_LOC=/var/log

export DUMP_UTIL=dump_handler
export JOURNALCTRL_UTIL=/bin/journalctl

export LTQ_CODE_WAVE500="1bef:08"
export LTQ_CODE_WAVE600="8086:09"
export LTQ_CODE_WAVE600D2="8086:0d5a"
export WAVE_COMPONENTS_FILE="/etc/wave/bins/wave_components.ver"

## logs for debug
export module1="HALWLAN"
export module1_LogFile="/var/log/user"
export module2="mtlk"
export module2_LogFile="/var/log/user"
export module3="hostapd"
export module3_LogFile="/var/log/daemon"
export module4="DWPAL"
export module4_LogFile="/var/log/user"

export IN_FILES=$(echo "$module1_LogFile $module2_LogFile $module3_LogFile $module4_LogFile" | xargs -n1 | sort -u | xargs)
export MATCHES="$module1\|$module2\|$module3\|$module4"

export brlan="brlan0"

#functions

UCI=`which uci`

function update_mac_address_if_needed(){
        do_update_mac_address "$@"
}

function init_prog(){
        check_meta_factory
        TMP_CONF_FILE=$(mktemp /tmp/tmpConfFile.XXXXXX)
        if [ "$PROG" = "" ]; then
                PROG=`$UCI get meta-factory.merge.prog`
        fi
}

function meta_factory_init(){
        do_meta_factory_init "$@"
}

function get_is_zwdfs(){
        get_is_zwdfs_device "$@"
}

function remove_dfs_state_file(){
        remove_dfs_state_file_device "$@"
}

function is_station_supported(){
        is_station_supported_device "$@"
}

function print_logs(){
        echo "$1" >/dev/console
}

function uci_set_helper(){
        echo "$1" >> "$2"
}

clean_uci_cache(){
        uci revert wireless > /dev/null 2>&1
}

clean_uci_db(){
        cat /dev/null > $UCI_DB_PATH/wireless
        rm -f $UCI_DB_PATH/.wireless.uci* 2> /dev/null
}

prepare_vars(){
        phys=`ls /sys/class/ieee80211/`
        tmp_wireless=$(mktemp /tmp/wireless.XXXXXX)
        validate_radio_map_file
}

get_iface_idx(){
        get_iface_idx_on_device "$@"
}

get_band(){
        get_band_on_device "$@"
}

set_mac_address(){
        set_mac_address_on_device "$@"
}

get_board(){
        get_board_on_device "$@"
}

use_templates(){
        set_conf_to_file "$@"
}

use_templates_tmp_file(){
        use_templates "$@" 1
}

use_template(){
        cat $2 >> $4
}

commit_changes(){
        mv $tmp_wireless $UCI_DB_PATH/wireless
        $UCI set meta-factory.merge.prog=$PROG 2>/dev/null
        update_templates
}

add_changes_commit(){
	cat $1 >> $UCI_DB_PATH/wireless
}

copy_meta_factory(){
        cp $DEFAULT_DB_PATH/meta-factory $UCI_DB_PATH/
}
