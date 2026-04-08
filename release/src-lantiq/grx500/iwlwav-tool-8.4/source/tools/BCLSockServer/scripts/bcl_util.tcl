#!/bin/tclsh

# Source the web_config file that contains platform-dependent paths
if { [file exists /mnt/jffs2/web/web_config.tcl] } { 
	source /mnt/jffs2/web/web_config.tcl
} elseif { [file exists /root/mtlk/web/web_config.tcl] } { 
	source /root/mtlk/web/web_config.tcl
} elseif { [file exists  ../web/web_config.tcl] } {    
        source ../web/web_config.tcl 
} else {
	# TODO: Error handling!
	puts "Missing web configuration file! This is needed for setting paths."
}


proc file_load {filename} {
	set contents ""
	catch {set contents [exec cat $filename]}
	set contents [split $contents "\n"]
	return $contents	
}

proc read_param_db {dbFile} {
	# handler is a list of two parts: 
	# 1. list of keys
	# 2. list of ConfFiles respectively
	# this function does not preform a "good" reading if the DB.
	# it only search for "Name" key followed by "ConfFile" key, and set the conf file as value for that name
	set keys {}
	set values {}
	set lastKey ""
	#puts " reading $dbFile"
	if {[file readable $dbFile] ==  1} {
		set contents [file_load $dbFile]
		foreach line $contents {
			if {[regexp {([^=]*)=(.*)} $line all key value] == 1 } {
				set key [string trim $key]
				set value [string trim $value]
				if {[string compare $key "Name"] == 0} { 
					set lastKey $value 
				} elseif {[string compare $key "ConfFile"] == 0} {
					
					lappend keys $lastKey
					lappend values $value
				}
			}			
		}

	} else {
		return 0
	}
	#puts "done"
	return [list $keys $values]

}

proc read_ini {iniFile} {
	# handler is a list of two parts: 
	# 1. list of keys
	# 2. list of values respectively
	set keys {}
	set values {}
	set contents [file_load $iniFile]
	
	#go over the line in fill sections
	foreach line $contents {
		# check if this is a key
		if {[regexp {([^=]*)=(.*)} $line all key value] == 1 } {
			set key [string trim $key]
			set value [string trim $value]
			lappend keys $key
			lappend values $value
		}
		# this is nor key nor section - igonre it
	}
	return [list $keys $values]
}

proc save_db {sFile handler} {
	#puts "------>save_db $sFile"
	set keys [lindex $handler 0]
	set values [lindex $handler 1]	
	set len [llength [lindex $handler 0]]
	set output_file [open $sFile "w" ]
	for {set i 0} {$i < $len} {incr i} {
		puts $output_file "[lindex $keys $i] = [lindex $values $i]"
		#puts "[lindex $keys $i]=[lindex $values $i]"
	}
	puts $output_file ""
	
	close $output_file
}

proc db_setVal {handler key value} {
	#section found, search the keys
	set keys [lindex $handler 0]
	set values [lindex $handler 1]
	set index [lsearch $keys $key]
	if {$index != -1} {
		# we found the index , replace the value		
		set values [lreplace $values $index $index $value]
		return [list $keys $values]
	}
	# only if $add is 1, add this parameter if not found
	
	lappend keys $key
	lappend values $value
	return [list $keys $values]
	
}


proc do_config_import {in_file wlan_conf sys_conf db reboot} {
	
	global g_web_web_app_folder
	#puts "Reading wlan and sys files"
	set wlan_db [read_ini $wlan_conf]
	set sys_db  [read_ini $sys_conf]
	set web_db [read_param_db $db]
	set import_lines [file_load $in_file]
	set params [lindex $web_db 0]
	set confs [lindex $web_db 1]
	foreach line $import_lines {
		if {[regexp {([^=]*)=(.*)} $line all key value] == 1 } {
			set key [string trim $key]
			set value [string trim $value]			
			if {[string length $value]} {
				set index [lsearch $params $key]
				if {$index != -1} {
					set conf [lindex $confs $index]
					if {$conf == "wlan"} {
						set wlan_db [db_setVal $wlan_db $key $value]
					} elseif {$conf == "sys"} {
						set sys_db  [db_setVal $sys_db  $key $value]
					}
				}
			}
		}
	}
	
	#save databases 
	save_db $wlan_conf $wlan_db
	save_db $sys_conf $sys_db
	
	# execute the init_security script to apply security options.
	set last_path [pwd]
	cd $g_web_web_app_folder
	# noReactivateSecurity because there is a reboot after
	catch {[exec ./update_config_files.tcl noReactivateSecurity]}
	cd $last_path
	
	if {$reboot==1} {
		exec reboot &
	}
}

proc set_param {param_name param_val wlan_conf sys_conf db} {
	global g_web_web_app_folder
	#puts "Reading wlan and sys files"
	set web_db [read_param_db $db]
	set params [lindex $web_db 0]
	set confs [lindex $web_db 1]
	set line "$param_name = $param_val"
	set success 0
	if {[regexp {([^=]*)=(.*)} $line all key value] == 1 } {
		set key [string trim $key]
		set value [string trim $value]			
		if {[string length $value]} {
			set index [lsearch $params $key]
			if {$index != -1} {
				set conf [lindex $confs $index]
				if {$conf == "wlan"} {
					set wlan_db [read_ini $wlan_conf]
					set wlan_db [db_setVal $wlan_db $key $value]
					save_db $wlan_conf $wlan_db
					set success 1
				} elseif {$conf == "sys"} {
					set sys_db  [read_ini $sys_conf]
					set sys_db  [db_setVal $sys_db  $key $value]
					save_db $sys_conf $sys_db
					set success 1
				}
			}
		}
	}

	if {$success == 1} {
		# execute the init_security script to apply security options.
		set last_path [pwd]
		cd $g_web_web_app_folder
		# noReactivateSecurity because there is a reboot after
		catch {[exec ./update_config_files.tcl noReactivateSecurity]}
		cd $last_path
	} else {
		puts "ERROR"
	}
}


proc search_file {conf_file param} {
	set conf_file_lines [file_load $conf_file]
	foreach line $conf_file_lines {
		set line [string trim $line]
		#puts $line
		if {[regexp {([^=]*)=(.*)} $line all key value] == 1 } {
			set key [string trim $key]
			set value [string trim $value]
			if {[string compare $key $param]==0} {
				puts -nonewline $value
				return 1
			}
		}
	}
	return 0
}

#
#
# bcl_util.tcl - command line interface for the dongle configuration.
# This script is called from the BCLServer, or from the command line.
#
# available options :
# 1. bcl_util.tcl getparam param_name card_num
# 		Searches for the value of param_name from wireless card card_num and prints the result
#
# 2. bcl_util.tcl setparam param_name param_value card_num
#		Set the value of param_name to param_value and save in the configuration file of wireless interface card_num.
#
# 3. bcl_util.tcl loadconfig config_filename card_num 
#		Load configuration from config_filename into wireless interface card_num.
#

if {[info exists ::argv]} {
	set sys_conf_name		"$g_web_config_folder/sys.conf"
	set action [lindex $::argv 0]
	if {[string compare $action "getparam"]==0} {
		set param_name [lindex $::argv 1]
		set card_num [lindex $::argv 2]
		
		if {[llength $card_num]==0} {
			set card_num 0
		}		
		set wlan_conf_name "$g_web_config_folder/wlan${card_num}.conf"
		# if not found in wlan , search in sys
		if {[search_file $wlan_conf_name $param_name]== 0} {
			search_file $sys_conf_name $param_name
		}
	} elseif {[string compare $action "setparam"]==0} {
		set param_name [lindex $::argv 1]
		set param_val [lindex $::argv 2]
		set card_num [lindex $::argv 3]
		set db "$g_web_web_app_folder/mt_params.db"
		if {[llength $card_num]==0} {
			set card_num 0
		}		
		
		set wlan_conf_name "$g_web_config_folder/wlan${card_num}.conf"
		#puts "$param_name<br>$param_val<br>$card_num<br>$db<br>$wlan_conf_name<br>$sys_conf_name<br>"
		set_param "$param_name" "$param_val" "$wlan_conf_name" "$sys_conf_name" "$db"
	} elseif {[string compare $action "loadconfig"]==0} {
		set card_num [lindex $::argv 2]
		set in_config_file_name [lindex $::argv 1]
		if {[llength $card_num]==0} {
			set card_num 0
		}
		
		set wlan_conf_name "$g_web_config_folder/wlan${card_num}.conf"
		set db "$g_web_web_app_folder/mt_params.db"
		do_config_import $in_config_file_name $wlan_conf_name $sys_conf_name $db 0

	} else {
		puts -nonewline ""
	}
}
