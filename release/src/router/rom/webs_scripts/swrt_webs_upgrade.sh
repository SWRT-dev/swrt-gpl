#!/bin/sh

IS_BCMHND=`nvram get rc_support|grep -i bcmhnd`

wget_options="-q -t 2 -T 30 --no-check-certificate"

dl_path_file="https://update.paldier.com"

fw_name=$(nvram get swrt_fw_name)
fw_md5=$(nvram get swrt_fw_md5)

echo "---- To download fw/rsa, Start ----" > /tmp/webs_upgrade.log
nvram set webs_state_upgrade=0 # INITIALIZING
nvram set auto_upgrade=1

if [ "$fw_name" == "" ] || [ "$fw_md5" == "" ];then
	echo "---- download fw failure ----" >> /tmp/webs_upgrade.log
	nvram set webs_state_error=1
	nvram commit
	return
fi

cfg_trigger=`echo $1`	# cfg_mnt skip
echo "---- cfg_trigger=${cfg_trigger} ----" >> /tmp/webs_upgrade.log

if [ "$cfg_trigger" != "1" ]; then # cfg_mnt skip these

	force_upgrade=`nvram get webs_state_dl`


#for small size fw to increase free
small_fw_update=`nvram show | grep rc_support | grep small_fw`

if [ "$small_fw_update" != "" ]; then
	echo "---- small_fw_update path ----" >> /tmp/webs_upgrade.log
	mkdir /tmp/mytmpfs
	mount -t tmpfs -o size=16M,nr_inodes=10k,mode=700 tmpfs /tmp/mytmpfs
	firmware_path="/tmp/mytmpfs/linux.trx"
	rc rc_service stop_upgrade
else
	firmware_path="/tmp/linux.trx"
fi

#kill old files
rm -f $firmware_path
	if [ "$force_upgrade" == "1" ]; then
		record="webs_state_dl_error"
		nvram set webs_state_dl_error=0
		nvram set webs_state_dl_error_day=
	else
		record="webs_state_error"
		nvram set webs_state_error=0
	fi

	webs_state_dl_error_count=`nvram get webs_state_dl_error_count`
	if [ -z "$webs_state_dl_error_count" ]; then
		webs_state_dl_error_count=0
	fi
	error_day=`date |awk '{print $1}'`

fi #cfg_trigger!=1


# get specific model
fw_check=`nvram get fw_check`
model=`nvram get productid`
if [ "$model" == "RT-AC68U" ] && [ "$fw_check" == "1" ]; then
	echo "---- TM model restore ----" >> /tmp/webs_upgrade.log
	fw_check
else

# current firmware information
productid=`nvram get productid`
if [ "$productid" == "BLUECAVE" ]; then
       rc rc_service stop_wrs_force
fi

forbeta=`nvram get swrt_beta`
if [ -z "$forbeta" ]; then
	forbeta="0"
fi

if [ -z "$IS_BCMHND" ]; then
	echo "---- ! IS_BCMHND ----" >> /tmp/webs_upgrade.log
	echo 3 > /proc/sys/vm/drop_caches
fi

if [ "$cfg_trigger" == "1" ]; then	# cfg_mnt need it
	nvram set cfg_fwstatus=6
fi #cfg_trigger==1

wget_result=0
wget_result2=0
if [ "$forbeta" != "1" ] && [ -n "$(echo $fw_name |grep beta)" ]; then
	echo "---- [LiveUpdate] it's beta firmware  ----" >> /tmp/webs_upgrade.log
	nvram set webs_state_error=1
	nvram set webs_state_upgrade=1
	nvram unset swrt_fw_name
	nvram unset swrt_fw_md5
	nvram commit
	return 1;
elif [ "$forbeta" == "1" ]; then
	echo "---- wget beta fw ${dl_path_file}/$fw_name ----" >> /tmp/webs_upgrade.log
	wget -t 2 -T 30 --no-check-certificate --output-file=/tmp/fwget_log ${dl_path_file}/$fw_name -O $firmware_path
	wget_result=$?
	echo "---- [LiveUpdate] wget fw, exit code: ${wget_result} ----" >> /tmp/webs_upgrade.log
else
	echo "---- wget fw Real ${dl_path_file}/$fw_name ----" >> /tmp/webs_upgrade.log
	wget -t 2 -T 30 --no-check-certificate --output-file=/tmp/fwget_log ${dl_path_file}/$fw_name -O $firmware_path
	wget_result=$?
	echo "---- [LiveUpdate] wget fw, exit code: ${wget_result} ----" >> /tmp/webs_upgrade.log
fi	

if [ "$wget_result" != "0" ]; then
	echo "---- download fw failure ----" >> /tmp/webs_upgrade.log
	rm -f $firmware_path
	nvram set $record=1	# fail to download the firmware
	if [ "$cfg_trigger" != "1" ]; then	# cfg_mnt skip
		if [ "$force_upgrade" == "1" ]; then
			webs_state_dl_error_count=$((webs_state_dl_error_count+1))
			nvram set webs_state_dl_error_count=$webs_state_dl_error_count
			nvram set webs_state_dl_error_day=$error_day
		fi
	fi # cfg_trigger!=1
else
	nvram set webs_state_upgrade=2	
	echo "---- mv trx OK ----" >> /tmp/webs_upgrade.log
	nvram set firmware_check=0
	nvram set rsasign_check=0
	firmware_check $firmware_path
	sleep 1

	firmware_md5=`md5sum /tmp/linux.trx | awk '{print $1}'`
	[ "$firmware_md5" == "$fw_md5" ] && nvram set rsasign_check=1

	firmware_check_ret=`nvram get firmware_check`
	rsasign_check_ret=`nvram get rsasign_check`

	if [ "$firmware_check_ret" == "1" ] && [ "$rsasign_check_ret" == "1" ]; then
		echo "---- fw check OK ----" >> /tmp/webs_upgrade.log
		/sbin/ejusb -1 0
		nvram set fwpath=2
		if [ "$cfg_trigger" != "1" ]; then	# cfg_mnt skip
			nvram set auto_upgrade=0
			nvram set webs_state_dl=0
			nvram commit
			rc rc_service restart_upgrade
		fi # cfg_trigger!=1
	else
		echo "---- fw check error, CRC: ${firmware_check_ret}  rsa: ${rsasign_check_ret} ----" >> /tmp/webs_upgrade.log
		rm -f $firmware_path
		nvram set $record=3	# wrong fw
		if [ "$cfg_trigger" != "1" ]; then	# cfg_mnt skip
			if [ "$force_upgrade" == "1" ]; then
				webs_state_dl_error_count=$((webs_state_dl_error_count+1))
				nvram set webs_state_dl_error_count=$webs_state_dl_error_count
				nvram set webs_state_dl_error_day=$error_day
			fi
			reboot
		fi # cfg_trigger!=1
	fi
fi


fi # RT-AC68U fw_check

nvram unset swrt_fw_name
nvram unset swrt_fw_md5
echo "---- To download fw/rsa, End ----" >> /tmp/webs_upgrade.log
nvram set webs_state_upgrade=1
nvram commit

