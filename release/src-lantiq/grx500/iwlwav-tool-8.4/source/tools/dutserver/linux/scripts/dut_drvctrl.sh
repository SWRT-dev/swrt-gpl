#!/bin/sh
# Example script for DUT driver usage

if true; then
  # wave300
  mopt="dut=1,1"
else
  # ar10
  mopt="dut=1,1 bb_cpu_ddr_mb_number=`uboot_env --get --name wlanm | cut -d M -f 0`"
fi

mpath="/lib/modules/`uname -r`/net/"
ipath="/root/mtlk/images/"
tpath="/tmp/"
helper_name="drvhlpr_dut"

case "$1" in
start)
	cp -s ${ipath}*.bin ${tpath}
	insmod ${mpath}mtlkroot.ko
	insmod ${mpath}mtlk.ko ${mopt}
	sleep 3
	echo "f_saver = /bin/dut_file_saver.sh" > ${tpath}${helper_name}.config
	ln -s -f `which drvhlpr` ${tpath}${helper_name}
	${tpath}${helper_name} --dut -p ${tpath}${helper_name}.config &
	;;
stop)
	rmmod mtlk
	rmmod mtlkroot

	killall ${helper_name}
	;;
esac

exit 0
