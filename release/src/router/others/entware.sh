#!/bin/sh

if [ "$(nvram get entware_mount)" == 1 ];then
	mdisk=`nvram get entware_disk`
	usb_disk="/tmp/mnt/$mdisk"
	ext=$(mount |grep $usb_disk |grep ext)
	tntfs=$(mount |grep $usb_disk |grep tntfs)
	if [ ! -e "$usb_disk" ]; then
		nvram set entware_mount="0"
		nvram commit
		logger "No usable USB disk found!/没有找到可用的USB磁盘!" 
		exit 1
	elif [ ! -n "$ext" ] && [ ! -n "$tntfs" ]; then
		nvram set entware_mount="0"
		nvram commit
		logger "No supported disk type found!/没有找到可用的USB磁盘格式!" 
		exit 1
	else
		mkdir -p $usb_disk/opt
		entwareopt="bin etc lib sbin share tmp usr include doc var home root libexec"
		for f in $entwareopt
		do
			mkdir -p /jffs/opt
			mkdir -p $usb_disk/opt/$f
			echo "ln -sf $usb_disk/opt/$f /jffs/opt/$f"
			ln -sf $usb_disk/opt/$f /jffs/opt/$f
		done
		ln -sf /jffs/opt /tmp/opt
		if [ "$(nvram get productid)" == "BLUECAVE" ]; then
			mount --bind /tmp/opt /opt
			ln -sf /tmp/wireless/lantiq /opt/lantiq
		fi
	fi
else
	rm -rf /jffs/opt
fi

