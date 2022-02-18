#!/bin/sh

if [ "$(nvram get entware_mount)" == 1 ];then
	mdisk=`nvram get entware_disk`
	usb_disk="/tmp/mnt/$mdisk"
	fat=$(mount |grep $usb_disk |grep tfat)
	fat1=$(mount |grep $usb_disk |grep vfat)
	[ -n "$fat" -o -n "$fat1" ] && logger "Unsupport TFAT!" && exit 1
	if [ ! -e "$usb_disk" ]; then
		nvram set entware_mount="0"
		nvram commit
		logger "USB flash drive not detected!/没有找到可用的USB磁盘!" 
		exit 1
	else
		mkdir -p $usb_disk/opt
		entwareopt="bin etc lib sbin share tmp usr include doc var home root"
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
	rm -f /jffs/opt
fi

