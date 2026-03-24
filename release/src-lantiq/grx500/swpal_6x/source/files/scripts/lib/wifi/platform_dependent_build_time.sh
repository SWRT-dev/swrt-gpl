#!/bin/sh

export OS_NAME="UGW"
export DEFAULT_DB_PATH="./files/db/etc/wave/db"
export UCI_DB_PATH="./output"

#functions

UCI=uci_get_parser

function update_mac_address_if_needed(){
     :
}

function init_prog(){
        TMP_CONF_FILE=$(mktemp /tmp/tmpConfFile.XXXXXX)
        if [ "$OS_NAME" = "UGW" ]; then
                PROG="ugw"
        else
                PROG="cgr"
        fi
}

function meta_factory_init(){
     :
}

function get_is_zwdfs(){
        get_value_from_phy_file $2 "is_zwdfs"
        if [ ! $get_value_from_phy_file_return = "-1" ]; then
	        echo "$get_value_from_phy_file_return"
	else
                echo "0"
        fi
}

function remove_dfs_state_file(){
     :
}

function is_station_supported(){
        local is_zwdfs=`get_is_zwdfs 0 $1`
        if [ $is_zwdfs -eq 0 ]; then
                echo "1"
        else
                echo "0"
        fi
}

function print_logs(){
        echo "$1"
}

function uci_set_helper(){
        echo "$1" >> "$2"
}

clean_uci_cache(){
     :
}

clean_uci_db(){
        cat /dev/null > $UCI_DB_PATH/wireless
}

prepare_vars(){
        get_phys_from_file
        phys=$get_phys_from_file_return
        tmp_wireless="$UCI_DB_PATH/wireless"
}

get_iface_idx(){
        get_value_from_phy_file $phy "iface_index"
        if [ ! $get_value_from_phy_file_return = "-1" ]; then
	        iface_idx=$get_value_from_phy_file_return
	fi
}

get_band(){
        get_value_from_phy_file $1 "radio"
        if [ $get_value_from_phy_file_return = "5Ghz" ]; then
                echo "0"
        else
                echo "1"
        fi
}

set_mac_address(){
     :
}

get_board(){
        get_value_from_phy_file $phy "board"
        board=$get_value_from_phy_file_return
}


function use_template(){
        use_template_single "$@"
}

use_templates(){
        set_conf_to_file "$@"
}

use_templates_tmp_file(){
        use_templates "$@"
}

update_mac_address_iface(){
     :
}

commit_changes(){
     :
}
