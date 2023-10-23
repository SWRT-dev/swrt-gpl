#!/bin/sh

export OS_NAME="UGW"
export BINDIR="/opt/intel/bin"
export SCRIPTS_PATH="/opt/intel/wave/scripts"
export ETC_CONFIG_WIRELESS_PATH="/etc/config/wireless"
export DEV_CREAT_PATH="/dev"
export DEFAULT_DB_PATH="/opt/intel/wave/db"
export UCI_DB_PATH="/etc/config"
export BUSYBOX_BIN_PATH="/bin"
export NC_COMMAND="/bin/lite_nc"
export WAVE_COMPONENTS_PATH="/etc"
export CAL_FILES_PATH="/tmp/wlanconfig"
export CERTIFICATION_FILE_PATH="/opt/intel/wave/certification_enabled"
export DEFAULT_IP_ADDRESS="192.168.1.1"
export RADIO_MAP_FILE="/opt/intel/wave/radio_map_file"

## for wlan debug collect
export WLAN_LOG_FILE_NAME=wlanLog
export SYSLOG_CONF=""
export DEBUG_SYSLOG_CONF=""
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

export DUMP_LOC=/opt/intel/wave
export DB_LOC=/etc
export DB_LOC_NAME=config
export CONF_LOC=/var/run
export NVRAM_LOGS_LOC=""
export VAR_LOGS_LOC=/var/log

export DUMP_UTIL=/opt/intel/bin/dump_handler
export JOURNALCTRL_UTIL=""
export SYSLOG_NG_UTIL=""

export LTQ_CODE_WAVE500="1bef:08"
export LTQ_CODE_WAVE600="8086:09"
export LTQ_CODE_WAVE600D2="8086:0d5a"
export WAVE_COMPONENTS_FILE="/etc/wave_components.ver"

## logs for debug
export module1=""
export module1_LogFile=""
export module2=""
export module2_LogFile=""
export module3="hostapd"
export module3_LogFile="/var/log/messages"
export module4=""
export module4_LogFile=""

export IN_FILES=$(echo "$module1_LogFile $module2_LogFile $module3_LogFile $module4_LogFile" | xargs -n1 | sort -u | xargs)
export MATCHES="$module1\|$module2\|$module3\|$module4"

export brlan="br-lan"

#functions

UCI=uci_parser

function uci_set_helper(){
        if [ "$2" = "$tmp_wireless" ]; then
               uci_set_helper_ubus "$@"
        else
             echo "$1" >> "$2"
        fi
}

function use_template(){
        use_template_single "$@"
}

function print_logs(){
        echo "$1" >/dev/console
}

function update_mac_address_if_needed(){
        do_update_mac_address "$@"
}

function init_prog(){
        if [ "$USER" != "root" ]; then
                print_logs "can't use seamless factory on non-root user"
                print_logs "using full factory"
                PROG="ugw"
                if [ -f $UCI_DB_PATH/meta-factory ]; then
                        $UCI set "$MERGE_PARAM"=0
                        print_logs "disabling seamless factory"
                fi
        else
                check_meta_factory
                TMP_CONF_FILE=$(mktemp /tmp/tmpConfFile.XXXXXX)
                if [ "$PROG" = "" ]; then
                        PROG=`$UCI get meta-factory.merge.prog`
                fi
        fi
}

function meta_factory_init(){
        :
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

clean_uci_cache(){
        ubus call uci revert '{ "config" : "wireless"}' > /dev/null 2>&1
}

clean_uci_db(){
        local wireless=`cat "$UCI_DB_PATH/wireless"`
        local old_ifs;

        old_ifs=$IFS
        IFS=$'\n'
        for line in $wireless; do
                if [ ""`echo "$line" | cut -f 1 -d " "` = "config" ]; then
                        current_iface=`echo "$line" | cut -f 3 -d " " | tr -d "'"`
                        ubus call uci delete '{ "config" : "wireless", "section" : "'$current_iface'" }'
                fi
        done
        IFS=$old_ifs
}

prepare_vars(){
        phys=`ls /sys/class/ieee80211/`
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
        use_template $1 $3 1
        use_template $1 $2 0
}

use_templates_tmp_file(){
        use_template $1 $2 0
}

commit_changes(){
        $UCI commit wireless
}

add_changes_commit(){
        commit_changes
}

copy_meta_factory(){
        cp $DEFAULT_DB_PATH/meta-factory $UCI_DB_PATH/
        chown rpcd:rpcd $UCI_DB_PATH/meta-factory
        chmod 640 $UCI_DB_PATH/meta-factory
}
