number_of_debug_configs=90

debug_infrastructure_config_add_string() {
	local config_name_prefix="$1"
	for i in $(seq 1 $number_of_debug_configs); do
		config_add_string $config_name_prefix$i
	done
}

debug_infrastructure_json_get_vars() {
	local config_name_prefix="$1"
	for i in $(seq 1 $number_of_debug_configs); do
		json_get_vars $config_name_prefix$i
	done
}

debug_infrastructure_append() {
        local config_name_prefix="$1"
        local config_type="$2"
        local param tmp_param tmp_array tmp_config_type

        for i in $(seq 1 $number_of_debug_configs); do
                eval debug_config=\"\${$config_name_prefix$i}\"
                debug_config_value=$debug_config
                if [ -n "$debug_config_value" ]; then
					conf_param="${debug_config_value%%=*}"
					## in case of capab params (ht,vht and he) we must remove the existing one.
					## looping on config_type(=base_cfg) and re-creating the hostapd cfg param
					## without the relevante capab param and then add the needed debug param.
					case "$conf_param" in
						ht_capab|vht_capab|he_capab)
							eval tmp_config_type=\"\${$config_type}\"
							tmp_array=""
							for param in $tmp_config_type
							do
								tmp_param="${param%%=*}"
								[ "$tmp_param" != "$conf_param" ] && append tmp_array "$param" "$N"
							done
							eval $config_type=\"\${tmp_array}\"
						;;
					 esac
					append $config_type "$debug_config_value" "$N"
                fi
        done
}

debug_infrastructure_execute_iw_command(){
	local config_name_prefix="$1"
	local wdev="$2"

	for i in $(seq 1 $number_of_debug_configs); do
		eval debug_config=\"\${$config_name_prefix$i}\"
		debug_config_value=`echo $debug_config | sed 's/=/ /g'` #dont expect = in iw command

		if [ -n "$debug_config_value" ]; then
			eval "iw $wdev iwlwav $debug_config_value"
		fi
	done
}
