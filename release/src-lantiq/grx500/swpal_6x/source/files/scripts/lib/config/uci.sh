#!/bin/sh
# Shell script compatibility wrappers for /sbin/uci
#
# Copyright (C) 2008-2010  OpenWrt.org
# Copyright (C) 2008  Felix Fietkau <nbd@nbd.name>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
CONFIG_APPEND=
UCI_APP="/sbin/uci"

if [ -f /lib/netifd/debug_infrastructure.sh ]; then
	. /lib/netifd/debug_infrastructure.sh
else
	number_of_debug_configs=0
fi

uci_load() {
	local PACKAGE="$1"
	local DATA
	local RET
	local VAR

	_C=0
	if [ -z "$CONFIG_APPEND" ]; then
		for VAR in $CONFIG_LIST_STATE; do
			export ${NO_EXPORT:+-n} CONFIG_${VAR}=
			export ${NO_EXPORT:+-n} CONFIG_${VAR}_LENGTH=
		done
		export ${NO_EXPORT:+-n} CONFIG_LIST_STATE=
		export ${NO_EXPORT:+-n} CONFIG_SECTIONS=
		export ${NO_EXPORT:+-n} CONFIG_NUM_SECTIONS=0
		export ${NO_EXPORT:+-n} CONFIG_SECTION=
	fi

	DATA="$($UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} ${LOAD_STATE:+-P /var/state} -S -n export "$PACKAGE" 2>/dev/null)"
	RET="$?"
	[ "$RET" != 0 -o -z "$DATA" ] || eval "$DATA"
	unset DATA

	${CONFIG_SECTION:+config_cb}
	return "$RET"
}

uci_set_default() {
	local PACKAGE="$1"
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} -q show "$PACKAGE" > /dev/null && return 0
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} import "$PACKAGE"
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit "$PACKAGE"
}

uci_revert_state() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"

	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} -P /var/state revert "$PACKAGE${CONFIG:+.$CONFIG}${OPTION:+.$OPTION}"
}

uci_set_state() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local VALUE="$4"

	[ "$#" = 4 ] || return 0
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} -P /var/state set "$PACKAGE.$CONFIG${OPTION:+.$OPTION}=$VALUE"
}

uci_toggle_state() {
	uci_revert_state "$1" "$2" "$3"
	uci_set_state "$1" "$2" "$3" "$4"
}

get_common_free_debug_conf_idx()
{
	local uci_path=$1.$2
	local debug_conf_val="val"
	debug_conf_idx=0

	while [ "$debug_conf_val" != "" ]; do
		debug_conf_idx=$(($debug_conf_idx + 1))
		debug_conf_val=`$UCI_APP get $uci_path.${3}_$debug_conf_idx 2> /dev/null`
	done
}

uci_set() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local VALUE="$4"
    local is_substr_equal=`echo $4 | grep "="`

	if [[ "$OPTION" = "debug_iw_pre_up" || "$OPTION" = "debug_iw_post_up" || \
		  "$OPTION" = "debug_hostap_conf" ]]; then
		get_common_free_debug_conf_idx $PACKAGE $CONFIG $OPTION
		if [ $debug_conf_idx -gt  $number_of_debug_configs ]; then
			echo "Error: maximum number of $OPTION is $number_of_debug_configs"
			return
		fi
		OPTION=${OPTION}_${debug_conf_idx}

		if [ "$5" != "" ] && [ -z "$is_substr_equal" ]; then
			VALUE="$VALUE=$5"
			shift
		fi
	elif [[ "$OPTION" = debug_iw_pre_up_* || "$OPTION" = debug_iw_post_up_* || \
			"$OPTION" = debug_hostap_conf_* ]]; then
		if [ "$5" != "" ] && [ -z "$is_substr_equal" ]; then
			VALUE="$VALUE=$5"
			shift
		fi
	fi

	while [ "$5" != "" ]; do
		VALUE="$VALUE $5"
		shift
	done

	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set "$PACKAGE.$CONFIG.$OPTION=$VALUE"
}

uci_get_state() {
	uci_get "$1" "$2" "$3" "$4" "/var/state"
}

uci_get() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local DEFAULT="$4"
	local STATE="$5"

	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} ${STATE:+-P $STATE} -q get "$PACKAGE${CONFIG:+.$CONFIG}${OPTION:+.$OPTION}"
	RET="$?"
	[ "$RET" -ne 0 ] && [ -n "$DEFAULT" ] && echo "$DEFAULT"
	return "$RET"
}

uci_add() {
	local PACKAGE="$1"
	local TYPE="$2"
	local CONFIG="$3"

	if [ -z "$CONFIG" ]; then
		export ${NO_EXPORT:+-n} CONFIG_SECTION="$($UCI_APP add "$PACKAGE" "$TYPE")"
	else
		$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set "$PACKAGE.$CONFIG=$TYPE"
		export ${NO_EXPORT:+-n} CONFIG_SECTION="$CONFIG"
	fi
}

uci_rename() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local VALUE="$3"

	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} rename "$PACKAGE.$CONFIG=$VALUE"
}

uci_remove() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	if [ "${PACKAGE}" == "wireless" ]; then
		if [ -z ${OPTION} ]; then # >> delete the whole interface
			$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} del "$PACKAGE.$CONFIG"
		else # >> delete a field/value
			$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} del "$PACKAGE.$CONFIG.$OPTION"
		fi
	else
		$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} del "$PACKAGE.$CONFIG${$OPTION:+.$OPTION}"
	fi
}

uci_commit() {
	local PACKAGE="$1"
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit "$PACKAGE"
}

uci_add_vap() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	local VALUE="$4"
	if [ -z ${VALUE} ]; then
		$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set "$PACKAGE.$CONFIG=$OPTION"
	else
		$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set "$PACKAGE.$CONFIG.$OPTION=$VALUE"
	fi
}

uci_add_list() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	shift;shift;shift
	local VALUE="${@}"
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} add_list "$PACKAGE.$CONFIG.$OPTION=$VALUE"
}

uci_del_list() {
	local PACKAGE="$1"
	local CONFIG="$2"
	local OPTION="$3"
	shift;shift;shift
	local VALUE="${@}"
	$UCI_APP ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} del_list "$PACKAGE.$CONFIG.$OPTION=$VALUE"
}

function wireless_apply() {
	uci_commit "wireless"
	#dynamic
	echo ""
	echo "******************************************"
	echo "* In error situation run: killall netifd *"
	echo "******************************************"
	echo ""
	eval 'wifi up radio0'
	#in error situation run killall netifd
	eval 'wifi up radio2'
	#in error situation run killall netifd
}

function main_func() {
	if [ "$1" = "set" ]; then
		input_tmp=`echo "$2" | awk '$1=$1' FS="." OFS=" "`
		#The IFS changed only for this line (tested in bash)
		dbFile=`echo "$input_tmp" | awk '{print $1}'`
		input_tmp_num=`echo "$input_tmp" | awk '{print NF}'`
		if [ "$input_tmp_num" = "2" ]; then                                   #uci set wireless.default_radio99='wifi-iface'
			input_tmp_2=`echo "$input_tmp" | awk '{print $2}' | awk '$1=$1' FS="=" OFS=" "`
			entity=`echo $input_tmp_2 | awk '{print $1}'`
			input_tmp_3=`echo "$2" | awk '$1=$1' FS="=" OFS=" "`
			iface=`echo $input_tmp_3 | awk '{print $2}'`
			uci_add_vap ${dbFile} ${entity} ${iface}
		else                    #uci set wireless.default_radio99.device=radio2 / uci set wireless.default_radio42.maxassoc=38
			entity=`echo "$input_tmp" | awk '{print $2}'`
			input_tmp_2=`echo "$input_tmp" | awk '{print $3}' | awk '$1=$1' FS="=" OFS=" "`
			field=`echo "$input_tmp_2" | awk '{print $1}'`
			value=${2#*=}
			while [ "$3" != "" ]; do
				value="$value $3"
				shift
			done
			if [ "${field}" = "device" ]; then
				uci_add_vap ${dbFile} ${entity} ${field} ${value}
			else
				uci_set ${dbFile} ${entity} ${field} ${value}
			fi
		fi
	elif [[ "$1" = "delete" || "$1" = "del" ]]; then
		input_tmp=`echo "$2" | awk '$1=$1' FS="." OFS=" "`
		dbFile=`echo "$input_tmp" | awk '{print $1}'`
		entity=`echo "$input_tmp" | awk '{print $2}'`
		input_tmp_num=`echo "$input_tmp" | awk '{print NF}'`
		if [ "$input_tmp_num" = "2" ]; then
			uci_remove ${dbFile} ${entity}
		elif [ "$input_tmp_num" = "3" ]; then
			field=`echo "$input_tmp" | awk '{print $3}'`
			uci_remove ${dbFile} ${entity} ${field}
		else
			input_tmp_2=`echo "$input_tmp" | awk '{print $3}' | awk '$1=$1' FS="=" OFS=" "`
			field=`echo "$input_tmp_2" | awk '{print $1}'`
			value=`echo "$input_tmp_2" | awk '{print $2}'`
			uci_remove ${dbFile} ${entity} ${field}
		fi

	elif [[ "$1" = "add_list" || "$1" = "del_list" ]]; then
		input_tmp=`echo "$2" | awk '$1=$1' FS="." OFS=" "`
		dbFile=`echo "$input_tmp" | awk '{print $1}'`
		entity=`echo "$input_tmp" | awk '{print $2}'`
		input_tmp_2=`echo "$input_tmp" | awk '{print $3}' | awk '$1=$1' FS="=" OFS=" "`
		field=`echo "$input_tmp_2" | awk '{print $1}'`
		input_tmp_3=`echo "$2" | awk '$1=$1' FS="=" OFS=" "`
		value=`echo $input_tmp_3 | awk '{$1=""}1'`
		uci_${1} ${dbFile} ${entity} ${field} ${value}
	elif [ "${1}" = "commit" ]; then
		uci_commit ${2}
	elif [ "${1}" = "apply" ]; then
		wireless_apply
		exit
	elif [ "${1}" = "revert" ]; then
		$UCI_APP revert wireless
	else
		uci_ret=`eval $UCI_APP $@`
		echo $uci_ret
	fi
}

if [ "/lib/config/uci.sh" = "$0" ]; then
	input=`echo "${@/#-*/}" | awk -v RS=  '{$1=$1}1'`
	main_func ${input}
fi
