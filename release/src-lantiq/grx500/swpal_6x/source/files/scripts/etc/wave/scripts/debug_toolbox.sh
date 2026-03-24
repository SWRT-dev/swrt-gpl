#!/bin/sh

. /lib/wifi/platform_dependent.sh

version="20210306T1600"
script_name="$0"
command=$1
pc_ip=$2
param1=$3
param2=$4
param3=$5
param4=$6
param5=$7

burn_cal_file()
{
	local no_restart tftp_path interfaces_list interface_name burn_both cal_status

	no_restart=$1
	[ -z "$pc_ip" ] && echo "The PC IP parameter is missing." && exit

	tftp_path=${param1%\/*}
	interfaces_list=${param1##*\/}
	if [ "$tftp_path" = "$interfaces_list" ]
	then
		tftp_path=""
	else
		tftp_path="$tftp_path/"
	fi

	[ ! -d ${CAL_FILES_PATH} ] && mkdir -p ${CAL_FILES_PATH}
	[ "${OS_NAME}" = "RDKB" ] && chattr -i ${CAL_FILES_PATH}
	cd ${CAL_FILES_PATH}
	cal_status=0
	interface_name=${interfaces_list%%,*}
	while [ -n "$interface_name" ]
	do
		if [ "$interface_name" = "all" ]
		then
			tftp -gr "${tftp_path}cal_wlan0.bin" -l cal_wlan0.bin $pc_ip
			cal_status=$(( $cal_status + `echo $?` ))
			tftp -gr "${tftp_path}cal_wlan2.bin" -l cal_wlan2.bin $pc_ip
			cal_status=$(( $cal_status + `echo $?` ))
			tftp -gr "${tftp_path}cal_wlan4.bin" -l cal_wlan4.bin $pc_ip
			cal_status=$(( $cal_status + `echo $?` ))
		else
			tftp -gr "${tftp_path}cal_${interface_name}.bin" -l cal_${interface_name}.bin $pc_ip
			cal_status=$(( $cal_status + `echo $?` ))
		fi
		interfaces_list=${interfaces_list#$interface_name}
		interfaces_list=${interfaces_list#,}
		interface_name=${interfaces_list%%,*}
	done
	[ "${OS_NAME}" = "RDKB" ] && chattr +i ${CAL_FILES_PATH}
	cd - > /dev/null
	
	${SCRIPTS_PATH}/flash_file_saver.sh
	ret=$?
	if [ $ret = 0 ]
	then
		echo "$script_name: calibration files saved to flash, rebooting..."
		reboot
	else
		echo "$script_name: ERROR - failed to save calibration files to flash." >&2
		exit -1
	fi
}

remove_flash_cal_files()
{
	local save_mode image status rm_cal_files_dir interfaces_list interface_name

	# Detect which flash saving method to use
	save_mode=""
	# Check for upgrade tool (UGW mode)
	image=`which upgrade`
	status=$?
	if [ $status -eq 0 ]
	then
		save_mode="UGW"
	else
		save_mode="Puma"
	fi

	# Prepare relevant files to copy
	rm_cal_files_dir="/tmp/remove_cal_files"
	rm -rf ${rm_cal_files_dir}
	mkdir ${rm_cal_files_dir}
	cd ${rm_cal_files_dir}
	cp ${CAL_FILES_PATH}/cal_*.bin .

	# Remove calibration files according to input interfaces list
	interfaces_list=$1
	interface_name=${interfaces_list%%,*}
	while [ -n "$interface_name" ]
	do
		if [ "$interface_name" = "all" ]
		then
			rm ./*
		else
			rm ./cal_${interface_name}.bin
		fi
		interfaces_list=${interfaces_list#$interface_name}
		interfaces_list=${interfaces_list#,}
		interface_name=${interfaces_list%%,*}
	done

	# Save remaining cal files back to flash directory
	if [ "$save_mode" = "Puma" ]
	then
		#remove write protection
		chattr -i ${CAL_FILES_PATH}
		rm ${CAL_FILES_PATH}/*
		cp ./* ${CAL_FILES_PATH}
		#restore write protection
		chattr +i ${CAL_FILES_PATH}
		sync
	else
		tar czf wlanconfig_cal_files.tar.gz ./cal_*.bin &>/dev/null
		upgrade wlanconfig_cal_files.tar.gz wlanconfig 0 0
	fi

	echo "$script_name: required calibration files removed from flash, rebooting..."
	reboot
}

wlan_status_info()
{
	iw dev
}

setting_overlay()
{

	local user_select trace

	echo -e "####### overlay setting ############" > /dev/console
	echo -e "## 0. overlay Disabled            ##" > /dev/console
	echo -e "## 1. overlay Enabled             ##" > /dev/console
	echo -e "####################################" > /dev/console
	echo -e "Enter selection:" > /dev/console
	echo -ne ">>" > /dev/console;read user_select

	case $user_select in
	"0")
		overlay.sh remove
		trace="Change overlay configuation Disabled..rebooting"
	;;
	"1")
		overlay.sh create
		trace="Change overlay configuation Enabled...rebooting"
	;;
	esac

	echo -e "$trace" > /dev/console
	/etc/wave/scripts/wave_factory_reset.sh
	sleep 2
	reboot
}

setting_work_mode()
{
	systemctl stop CcspPandMSsp
	systemctl stop CcspEthAgent
}

wlan_version()
{
	iw dev wlan0 iwlwav gEEPROM
	iw dev wlan2 iwlwav gEEPROM
	cat /proc/net/mtlk/version
	cat ${WAVE_COMPONENTS_PATH}/wave_components.ver
	uname -a
}

wlan_factory()
{
	echo -e "####### #######################################################" > /dev/console
	echo -e "####### factroy ( on RDKB overlay is not deleted ) ############" > /dev/console
	echo -e "####### reboot...please wait                       ############" > /dev/console
	echo -e "####### #######################################################" > /dev/console
	sleep 5
	if [ $OS_NAME = "RDKB" ] ;then
		systemctl start CcspEthAgent
		systemctl start CcspPandMSsp
		fapi_wlan_cli factoryReboot
	else
		ubus call csd factoryreset '{ "object":"Device.WiFi."}'
	fi
}

dut_get_cv()
{
	cat ${WAVE_COMPONENTS_PATH}/wave_components.ver
}

wav_certification()
{
	local br_ip="$pc_ip"

	if [ -z "$br_ip" ]
	then
		if [ "$OS_NAME" = "UGW" ]
		then
				br_ip=`uci get network.lan.ipaddr`
		else
				br_ip=`dmcli eRT setv Device.X_CISCO_COM_DeviceControl.LanManagementEntry.1.LanIPAddress`
				br_ip=${br_ip##*value: }
				br_ip=`echo $br_ip`
		fi
		[ "$br_ip" = "$DEFAULT_IP_ADDRESS" ] && br_ip="192.165.100.10"
	fi
	echo "Setting bridge IP to $br_ip" > /dev/console
	if [ "$OS_NAME" = "UGW" ]
	then
			ubus call uci set '{ "config" : "network" ,"section" : "lan", "values": {"ipaddr" : "'$br_ip'"}}'
			ubus call uci commit '{"config" : "network" }'
            echo "object:Device.IP.Interface.IPv4Address.: :MODIFY" > /tmp/cert_ip_change.txt
			echo "param:IPAddress: :$br_ip" >> /tmp/cert_ip_change.txt
			echo "param:SubnetMask: :255.255.0.0" >> /tmp/cert_ip_change.txt
			caltest -s /tmp/cert_ip_change.txt -c SERVD
	else
			bridgeIPChange $br_ip
	fi

	# Set certification flag to enabled
	touch "$CERTIFICATION_FILE_PATH"
	# Execute sigma-start.sh
	echo "Running sigma-start" > /dev/console
	/lib/netifd/sigma-start.sh
}

driver_debug()
{
	local enable=$param1
	local interface="radio0"

	if [ $enable = "1" ] ;then
		#echo 8 cdebug=1 > /proc/net/mtlk_log/debug
		uci set wireless.$interface.debug_iw_post_up_20="sMtlkLogLevel 8 1 1"
	elif  [ $enable = "0" ]; then
		#echo 8 cdebug=0 > /proc/net/mtlk_log/debug
		uci set wireless.$interface.debug_iw_post_up_20="sMtlkLogLevel 8 0 1"
	fi

	if [ $OS_NAME = "RDKB" ] ;then
		uci commit wireless
		[ $enable = "1" ] && echo "" > /var/log/daemon
	else
		ubus call uci commit '{ "config" : "wireless" }'
		[ $enable = "1" ] && echo "" > /var/log/messages
	fi
	ubus call network restart
	cat /proc/net/mtlk_log/debug | grep -B1 'all\|Default log levels'
}

hostapd_debug()
{
	local interface=$pc_ip
	local enable=$param1
	local status=`uci get wireless.$interface.hostapd_log_level 2>/dev/null`

	[ "$status" = "dd" ] && status=1 || status=0

	if [ $status != $enable ]; then

		if [ $enable = "1" ]; then
			uci set wireless.$interface.hostapd_log_level=dd
		else
			uci delete wireless.$interface.hostapd_log_level
		fi

		if [ $OS_NAME = "RDKB" ] ;then
			uci commit wireless
			[ $enable = "1" ] && echo "" > /var/log/daemon
		else
			ubus call uci commit '{ "config" : "wireless" }'
			[ $enable = "1" ] && echo "" > /var/log/messages
		fi
		echo -e "####### restart hostapd...please wait #######" > /dev/console
		sleep 5
		ubus call network restart
	else
		echo -e "####### hostapd debug level is already set #######" > /dev/console
	fi
	if [ $OS_NAME = "RDKB" ] ;then
		cat /nvram/etc/config/wireless | grep -w -m8 'dd\|radio0\|radio2\|radio4' | grep -v mac
	else
		cat /etc/config/wireless | grep -w -m8 'dd\|radio0\|radio2\|radio4' | grep -v mac
	fi
}

wlan_fw_logger_600_B_HW_FIFO_table()
{
	echo -e "                    0           1         2           3                4             5"
	echo -e "1-7-HWCommonFIFO1   HostTXIn    QMRxLists TxLib       RxPP             Band0Activity QMTXMPDUBand1"
	echo -e "2-8-HWCommonFIFO2   HostTXOut   QMRxdata  TxLib       RxPP             Band1Activity QMTXMPDUBand0"
	echo -e "3-9-HWCommonFIFO3   HostRXIn    QMTxLists RXLib       TxAger           QMDMA         QMRXMPDUBand1"
	echo -e "4-10-HWCommonFIFO4  HostRxOut   QMTXData  RXLib       TxAger           Reserved      QMRXMPDUBand0"
	echo -e "5-11-HWBand0FIFO1   Reserved    Security  PreAgg      MUbitMapSelector BSRC          Reserved "
	echo -e "6-12-HWBand0FIFO2   Classifier  Reserved  OTFA        MULocker         Reserved      Reserved "
	echo -e "7-13-HWBand0FIFO3   RXHCDeagg   BFParser  TXHC        Locker           BAA           Reserved"
	echo -e "8-14-HWBand0FIFO4   Coordinator PSSetting TXPDAcc     TxSelBitmap      Delia         Reserved "
	echo -e "9-15-HWBand0FIFO5   AutoResp    Reserved  TFGen       Sequencer        PlanLocker    Reserved"
	echo -e "10-16-HWBand0FIFO6  BSRC        Alpha     FilterDelia TxH              Beacon        RXC"
	echo -e "11-17-HWBand1FIFO1  Reserved    Security  PreAgg      MUbitMapSelector BSRC          Reserved"
	echo -e "12-18-HWBand1FIFO2  Classifier  Reserved  OTFA        MULocker         Reserved      Reserved "
	echo -e "13-19-HWBand1FIFO3  RXHCDeagg   BFParser  TXHC        Locker           BAA           Reserved"
	echo -e "14-20-HWBand1FIFO4  Coordinator PSSetting TXPDAcc     TxSelBitmap      Delia         Reserved"
	echo -e "15-21-HWBand1FIFO5  AutoResp    Reserved  TFGen       Sequencer        PlanLocker    Reserved "
	echo -e "16-22-HWBand1FIFO6  BSRC        Alpha     FilterDelia TxH              Beacon        RXC"
	echo -e ">> 00-CONTINUE"
}

wlan_fw_logger_600_B_BIT_SET_table()
{
	echo -e "BIT  Module"
	echo -e "0  - UMACBand0"
	echo -e "1  - LMACBand0"
	echo -e "2  - HostIfRISC"
	echo -e "3  - RxHandlerBand0"
	echo -e "4  - TxSenderBand0"
	echo -e "5  - PHYGenRiscBand0"
	echo -e "6  - PHYHWBand0"
	echo -e "23 - LMACBand1"
	echo -e "24 - RxHandlerBand1"
	echo -e "25 - TxSenderBand1"
	echo -e "26 - PHYGenRiscBand1"
	echo -e "27 - PHYHWBand1"
	echo -e ">> 00 - CONTINUE"
}

wlan_fw_logger_600_D2_HW_FIFO_table()
{
	echo -e "                    0             1              2             3                4             5"
	echo -e "1-7-HWCommonFIFO1   HostTXIn      QMRxLists      TxLib         RxPP             Band0Activity QMTXMPDUBand1"
	echo -e "2-8-HWCommonFIFO2   HostTXOut     QMRxdata       TxLib         RxPP             Band1Activity QMTXMPDUBand0"
	echo -e "3-9-HWCommonFIFO3   HostRXIn      QMTxLists      RXLib         TxAger           QMDMA         QMRXMPDUBand1"
	echo -e "4-10-HWCommonFIFO4  HostRxOut     QMTXData       RXLib         TxAger           Reserved      QMRXMPDUBand0"
	echo -e "5-11-HWBand1FIFO1   BAAStatCNTR   Security       PreAgg        MUbitMapSelector BSRC          BestRU"
	echo -e "6-12-HWBand1FIFO2   Classifier    Reserved       OTFA          MULocker         Reserved      Reserved "
	echo -e "7-13-HWBand1FIFO3   RXHCDeagg     BFParser       TXHC          Locker           BAA           Reserved"
	echo -e "8-14-HWBand1FIFO4   Coordinator   PSSetting      TXPDAcc       TxSelBitmap      DeliaLegacy   Reserved"
	echo -e "9-15-HWBand1FIFO5   AutoResp      BAAStatCNTR    TFGen         Sequencer        PlanLocker    AlphaFiltersPHY"
	echo -e "10-16-HWBand1FIFO6  BSRC          AlphaFilterMAC DeliaAutofill TxH              Beacon        RXC"
	echo -e "11-17-HWBand0FIFO1  BAAStatCNTR   Security       PreAgg        MUbitMapSelector BSRC          BestRU "
	echo -e "12-18-HWBand0FIFO2  Classifier    Reserved       OTFA          MULocker         Reserved      Reserved "
	echo -e "13-19-HWBand0FIFO3  RXHCDeagg     BFParser       TXHC          Locker           BAA           Reserved"
	echo -e "14-20-HWBand0FIFO4  Coordinator   PSSetting      TXPDAcc       TxSelBitmap      DeliaLegacy   Reserved "
	echo -e "15-21-HWBand0FIFO5  AutoResp      BAAStatCNTR    TFGen         Sequencer        PlanLocker    AlphaFiltersPHY"
	echo -e "16-22-HWBand0FIFO6  BSRC          AlphaFilterMAC DeliaAutofill TxH              Beacon        BestRU"
	echo -e ">> 00-CONTINUE"
}

wlan_fw_logger_600_D2_BIT_SET_table()
{
	echo -e "BIT  Module"
	echo -e "0  - UMACBand0"
	echo -e "1  - LMACBand0"
	echo -e "2  - HostIfRISC"
	echo -e "3  - RxHandlerBand0"
	echo -e "4  - TxSenderBand0"
	echo -e "5  - PHYGenRiscBand0"
	echo -e "6  - PHYHWBand0"
	echo -e "23 - LMACBand1"
	echo -e "24 - RxHandlerBand1"
	echo -e "25 - TxSenderBand1"
	echo -e "26 - PHYGenRiscBand1"
	echo -e "27 - PHYHWBand1"
	echo -e ">> 00 - CONTINUE"
}


wlan_fw_logger()
{
	local host_pc_serverip="$pc_ip"
	local host_pc_ethaddr="$param1"
	local interface="$param2"
	local um_lm="$param3"
	local hw_modules="$param4"
	local bits_set="$param5"
	local cards_set="0x00"
	local temp="0x01"

	if [ "$pc_ip" = "" ]; then
	clear
	echo -ne "Enter platfrom type:700/D2/B>>" > /dev/console;read plat_type
	while [ 1 ]
	do
			rm -f /tmp/fw_logger > /dev/null 2>&1
			if [ "$plat_type" = "B" ]; then
				echo -e "PLAT TYPE 600-$plat_type"
				wlan_fw_logger_600_B_HW_FIFO_table >>/tmp/fw_logger
				wlan_fw_logger_600_B_HW_FIFO_table
			elif [ "$plat_type" = "D2" ] || [ "$plat_type" = "700" ]; then
				echo -e "PLAT TYPE $plat_type"
				wlan_fw_logger_600_D2_HW_FIFO_table >>/tmp/fw_logger
				wlan_fw_logger_600_D2_HW_FIFO_table
			else
				echo -e "Invalid input\n" > /dev/console
				exit 0
			fi
			echo -e "Enter selection valid raw:1-16 column:0-5 e.g. 6=2:HWBand0FIFO2 & OTFA:" > /dev/console
			echo -ne ">>" > /dev/console;read hw_modules_temp
			[ "$hw_modules_temp" = "00" ] && break
			hw_modules="$hw_modules,$hw_modules_temp"
			line_num=${hw_modules_temp%%=*}
			line_num=$((line_num+1))
			choosed_line=`cat /tmp/fw_logger  | sed -n "${line_num}p"`

			column_select=${hw_modules_temp##*=}
			column_select=$((column_select+2))
			select_summary=`echo $choosed_line | awk -v param="$column_select" '{print $1 " set "  $param}'`
			echo -e "Selected:${select_summary##*-}" >> /tmp/fw_logger_select_sum

			bit_select=${choosed_line%%-H*}
			bit_select=${bit_select##*-}
			while [ "$bit_select" != "0" ]
			do 
				temp=$((temp << 1))
				bit_select=$((bit_select-1))
			done
			cards_set=`printf '0x%X\n' $(( cards_set | temp ))`
			temp="0x01"
			clear
		done
		hw_modules=${hw_modules/,/}

		while [ 1 ]
		do
				echo -e "PLAT TYPE 600-$plat_type"
				[ "$plat_type" = "B" ] && wlan_fw_logger_600_B_BIT_SET_table || wlan_fw_logger_600_D2_BIT_SET_table
				echo -ne "Module BIT select valid:0-6 & 23-27 >>" > /dev/console;read bit_select
				[ "$bit_select" = "00" ] && break
				bit_select_sum="$bit_select_sum:$bit_select"
				temp="0x01"
				while [ "$bit_select" != "0" ]
				do
					temp=$((temp << 1))
					bit_select=$((bit_select-1))
				done
				cards_set=`printf '0x%X\n' $(( cards_set | temp ))`
				clear
		done

		echo -e "\nUser selection summary:\n"
		echo -e "hw_modules=$hw_modules"
		[ -e /tmp/fw_logger_select_sum ] && cat /tmp/fw_logger_select_sum
		rm -rf /tmp/fw_logger*
		echo -e "\nSelected bit modules:$bit_select_sum\n\nbit_set_summary=$cards_set"
		echo -ne "\npress any ket to continue...";read p
		clear

		echo -e "Enter host_pc_serverip:" > /dev/console
		echo -ne ">>" > /dev/console;read host_pc_serverip
		echo -e "Enter  host_pc_ethaddr:" > /dev/console
		echo -ne ">>" > /dev/console;read host_pc_ethaddr
		echo -e "Enter interface 0-wlan0 2-wlan2 4-wlan4:" > /dev/console
		echo -ne "Enter interface number>>" > /dev/console;read interface
		interface="wlan$interface"

	else

	[ -d /proc/net/mtlk/card0 ] && card0=1 || card0=0
	[ -d /proc/net/mtlk/card1 ] && card1=1 || card1=0
	[ -d /proc/net/mtlk/card2 ] && card2=1 || card2=0 
	[ "$card1" = "1" ] || [ "$card2" = "1" ] && cards_set=0x0000 || cards_set=0x800000
	cards_set=`printf '%X\n'  $(( cards_set | um_lm | bits_set))`
	cards_set="0x$cards_set"

	fi



	local ipaddr=`ifconfig $brlan | grep "inet addr"`
	ipaddr=${ipaddr##*addr:}
	ipaddr=${ipaddr%%' '*}

	local ethaddr="00:00:01:02:03:04"

	local hw_m=""
	if [ "$hw_modules" != "" ]; then
	[ "${hw_modules/16//}" = "$hw_modules" ] && hw_modules="$hw_modules,16=0"
	hw_modules=`echo $hw_modules | tr "," "\n" | sort -n | tr "\n" ","`
	local index
	local oifs="$IFS"
	IFS=,
		local index_arry=1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
		for module in $hw_modules; do
			for index in $index_arry; do
			  index_arry=`echo $index_arry | sed "s/^$index //"`
			  index_arry=${index_arry// /,}
			  if [ "$index" = ${module%%=*} ] ; then
				 hw_m="$hw_m ${module##*=}" && break
			  else
				 hw_m="$hw_m 0"
			  fi
			done
		done
	IFS="$oifs"
	else
		hw_m="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
	fi

	echo -e "ifconfig rtlog0 up\n " >/dev/console
	sleep 1
	ifconfig rtlog0 up
	echo -e "ifconfig rtlog0 hw ether $ethaddr\n " >/dev/console
	sleep 1
	ifconfig rtlog0 hw ether $ethaddr
	echo -e "brctl addif $brlan rtlog0\n " >/dev/console
	sleep 1
	[ "$(brctl show br-lan | grep rtlog0)" = "" ] && brctl addif $brlan rtlog0
	echo -e "LogHwModuleFifo $interface $hw_m\n " >/dev/console
	sleep 1
	echo LogHwModuleFifo $interface $hw_m > /proc/net/mtlk_log/rtlog
	echo -e "LogRemStream 0 $interface 0\n " >/dev/console
	sleep 1
	echo LogRemStream 0 $interface 0 > /proc/net/mtlk_log/rtlog
	echo -e "LogRemStream 1 $interface 0\n " >/dev/console
	sleep 1
	echo LogRemStream 1 $interface 0 > /proc/net/mtlk_log/rtlog

	echo -e "LogAddStream 0 $interface 0 $ipaddr 00:00:00:00:00:10 2008 $host_pc_serverip $host_pc_ethaddr 2009 0 1024 $cards_set \n " >/dev/console
	sleep 1
	echo LogAddStream 0 $interface 0 $ipaddr 00:00:00:00:00:10 2008 $host_pc_serverip $host_pc_ethaddr 2009 0 1024 $cards_set >/proc/net/mtlk_log/rtlog

	echo -e "\n#########################################################" >/dev/console
	echo -e "  if no Errors:Open WireShark IP:$host_pc_serverip" >/dev/console
	echo -e "#########################################################" >/dev/console

}

wlan_statistics()
{
	local interface="$pc_ip"
	local display_interval="$param1"
	local sta sta_mac index sta_count

	[ "$display_interval" = "" ] && display_interval=2

	 line_num=$(grep -n -m1 MAC /proc/net/mtlk/${interface}/sta_list)
	 line_num=${line_num%%:*}

	## STA list starts 2 raws below
	index=$((line_num+2))
	sta_count=0
	clear
	while [ 1 ]
	do
		sta=`sed -n ${index}p  /proc/net/mtlk/${interface}/sta_list`
		sta_mac=${sta%% *}

		## if equal we reached the end of the file - go to file start
		if [ "${sta_mac//:/}" != "$sta_mac" ]; then
			index=$((index+1))
		else
			index=$((line_num+2))
			if [ "$sta_count" != 0 ]; then
				sta_count=0
				continue
			else ## no STA are connected exit
				echo -e "\n########################### NO STAs CONNECTED ######################\n" >/dev/console
				exit 0
			fi

		fi
		sta_count=$((sta_count+1))
		echo -e "\n########################### STA STS[$sta_count]START :[ $sta_mac ]######################" >/dev/console
		dwpal_cli $interface PeerHostIfQos $sta_mac
		dwpal_cli $interface PeerHostIf $sta_mac
		dwpal_cli $interface PeerUlBsrcTidStats $sta_mac
		dwpal_cli $interface PeerBaaStats $sta_mac
		dwpal_cli $interface PlanManagerStats $sta_mac
		dwpal_cli $interface TwtStats $sta_mac
		dwpal_cli $interface PerClientStats $sta_mac
		dwpal_cli $interface PeerPhyRxStatus $sta_mac
		dwpal_cli $interface PeerInfo $sta_mac
		echo -e "\n########################### STA STS[$sta_count]END :[ $sta_mac ]######################" >/dev/console

		sleep $((display_interval-1))
		clear
	done

}

case $command in
	dut_get_cv)
		dut_get_cv
	;;
	burn_cal)
		burn_cal_file
	;;
	remove_cal)
		remove_flash_cal_files $2
	;;
	"wlan_status"|"ws")
		wlan_status_info
	;;
	"overlay"|"ov")
		setting_overlay
	;;
	"work_mode"|"wm")
		setting_work_mode
	;;
	"wlan_version"|"wv")
		wlan_version
	;;
	"wlan_factroy"|"wf")
		wlan_factory
	;;
	"wlan_collect_debug"|"wcd")
		${SCRIPTS_PATH}/wave_collect_debug.sh
	;;
	"wlan_collect_debug_default"|"wcdd")
		${SCRIPTS_PATH}/wave_collect_debug.sh -d $pc_ip
	;;
	"wlan_collect_debug_extension"|"wcde")
		${SCRIPTS_PATH}/wave_collect_debug.sh -e $pc_ip
	;;
	"wlan_collect_debug_assert"|"wcda")
		${SCRIPTS_PATH}/wave_collect_debug.sh -a $pc_ip
	;;
	"wlan_collect_debug_config"|"wcdc")
		${SCRIPTS_PATH}/wave_collect_debug.sh -c
	;;
	"wlan_certification"|"cert")
		wav_certification
	;;
	"wlan_hostapd_debug"|"whd")
		hostapd_debug
	;;
	"wlan_driver_debug"|"wdd")
		driver_debug
	;;
	"wlan_hostapd_driver_debug"|"whdd")
		param1=1
		hostapd_debug
		pc_ip=all
		driver_debug
	;;
	"wlan_fw_logger"|"wfl")
		wlan_fw_logger
	;;
	"wlan_statistics"|"wst")
		wlan_statistics
	;;
	*)

		[ "$command" = "" ] && command="help"
		echo -e "$script_name Version:$version: Unknown command $command\n \
		Usage: $script_name COMMAND [Argument 1] [Argument 2]\n" \
		"example: wavToolBox wcda 192.168.0.100 \n" \
		 "\n" \
		 "Commnads:\n" \
		 "burn_cal       Burn the calibration files\n" \
		 "  Arguments:\n" \
		 "  Argument 1:  Your PC IP\n" \
		 "  Argument 2:  The interface name or names to which calibration is burned: wlan0/wlan2/wlan4/all\n" \
		 "               Names can be specified in a comma-separated list: wlan0,wlan2\n" \
		 "               This argument can contain also the path in the tftp server before the interface name: /path/wlan\n" \
		 "               Example: $script_name burn_cal <PC IC> /private_folder/wlan0,wlan2,wlan4\n" \
		 "remove_cal                  Removes calibration files according to intrerfaces list\n" \
		 "  Arguments:\n" \
		 "  Argument 1:  The interface name or names to which calibration is removed: wlan0/wlan2/wlan4/all\n" \
		 "               Names can be specified in a comma-separated list, for example: wlan0,wlan2\n" \
		 "wlan_status                (ws) gives wlan interface main vaps (wlan0.0 and wlan2.0) status\n" \
		 "overlay                    (ov) setting overlay /bin /etc /lib access\n" \
		 "work_mode                  (wm) setting debug work mode - disable un-needed terminal traces \n" \
		 "wlan_version               (wv) getting wlan version info includes:eeprom,kernel_version,cv \n" \
		 "wlan_factory               (wf) complete clean-up ( overlay will not be deleted ) \n" \
		 "dut_get_cv                 returns wave_components.ver content \n" \
		 "wlan_collect_debug         (wcd) wlan collect debug info\n" \
		 "wlan_collect_debug_default (wcdd) <tftp ip> wlan collect default debug info and optional upload to tftp\n" \
		 "wlan_collect_debug_extension (wcde) <tftp ip> wlan collect extended debug info and optional upload to tftp\n" \
		 "wlan_collect_debug_assert  (wcda) <tftp ip> wlan collect debug info after triggring FW assert and optional upload to tftp\n" \
		 "wlan_collect_debug_config  (wcdc) Only for RDKB:reconfig the syslog to save all future logs to a single file\n" \
		 "wav_certification          (cert) switch to certification mode (change bridge IP and run sigma-start)\n" \
		 "  Arguments:\n" \
		 "      Argument 1: IP address to set (if no IP specified, setting IP to 192.165.100.10)\n" \
		 "                                                      \n" \
		 "wlan_hostapd_debug         (whd) set hostapd debug \n" \
		 "      Argument1: radio0 or radio2 or radio4\n" \
		 "      Argument2: 0 - disable 1 - enable\n" \
		  "      e.g. wavToolBox whd radio0 1 \n" \
		 "wlan_driver_debug          (wdd) set driver debug \n" \
		 "       Argument1: all\n" \
		 "       Argument2: 0 - disable 1 - enable\n" \
		 "      e.g. wavToolBox wdd all 1 \n" \
		 "wlan_hostapd_driver_debug  (whdd) set hostapd and driver debug\n"\
		 "       Argument1: radio0 or radio2 or radio4\n" \
		 "       Argument2:  1 - enable\n" \
		 "      e.g. wavToolBox whdd radio0 1 \n" \
		 "wlan_fw_logger              (wfl) enable FW logger (No Argumnet:interactive mode)\n" \
		 "      Arguments: <host PC serverip> <host PC ethaddr> <interface> <upper and lower MAC e.g: 0x3> \n" \
		 "      Optional:[hw_modules e.g HWBand0FIFO2=OTFA: 6=2 ] optional:[hw_modules bits e.g: 0x1000]\n" \
		 "wlan_statistics            (wst) display sta statistics per interface\n" \
		 "      Arguments: <interface> optional:[display interval in sec]\n"
	;;
esac
