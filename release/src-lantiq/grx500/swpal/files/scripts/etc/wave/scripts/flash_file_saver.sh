#!/bin/sh

. /lib/wifi/platform_dependent.sh

###########################
# Help & Input Parameters #
###########################
eeprom_tar=eeprom.tar.gz
script_name="$0"
src_file=$1
dst_file=${CAL_FILES_PATH}/$2

####################################################
# Copy source file to hotplug directory (optional) #
####################################################
copy_src_file()
{
	if [ -e "$src_file" ]
	then
		cp "$src_file" "$dst_file"
		ret=$?
		if [ $ret = 0 ]
		then
			echo "$script_name: file '$dst_file' saved."
		else
			echo "$script_name error: failed to save file '$dst_file'" >&2
			exit -1
		fi
	fi
}
###########################################
# Detect which flash saving method to use #
###########################################
# Set default value to Puma mode
burn_mode="Puma"

# Check for upgrade tool (UGW mode)
image=`which upgrade`
status=$?
if [ $status -eq 0 ]
then
	burn_mode="UGW" 
else
	# Check for haven_park
	haven_park=""
	[ -e /etc/config.sh ] && haven_park=`grep haven_park /etc/config.sh` 
	[ "$haven_park" != "" ] && burn_mode="haven_park"
fi
###############################################
# Write the new calibration file to the FLASH #
###############################################

if [ "$burn_mode" = "Puma" ]
then
	#Puma: save calibration files uncompressed to nvram/etc/wave_calibration folder
	[ ! -d ${CAL_FILES_PATH} ] && mkdir -p ${CAL_FILES_PATH}
	if [ -e "$src_file" ]
	then
		#remove write protection
		chattr -i ${CAL_FILES_PATH}
		copy_src_file
		#restore write protection
		chattr +i ${CAL_FILES_PATH}
		sync
	fi
else
	copy_src_file
	cd ${CAL_FILES_PATH}
	# Create tarball from calibration bins
	tar czf $eeprom_tar cal_*.bin
	if [ $? != 0 ]
	then
		echo "$script_name: failed to create $eeprom_tar" >&2
		exit -1
	fi
	
	if [ "$burn_mode" = "haven_park" ]
	then
		#haven_park:save calibration files compressed to /nvram/ folder
		cp ${CAL_FILES_PATH}/$eeprom_tar /nvram/
		sync
	else
		#UGW: Use upgrade tool to save calibration files compressed to flash partition
		upgrade ${CAL_FILES_PATH}/$eeprom_tar wlanconfig 0 0
		if [ $? != 0 ]
		then
			echo "$script_name: the partition wlanconfig doesn't exist and cannot be created" >&2
			exit -1
		fi
	fi
	cd - > /dev/null
fi

exit 0
