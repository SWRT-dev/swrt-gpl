#!/bin/sh


SPACE_AVAL=$(df|grep -w "/jffs$" | awk '{print $2}')
SC_MOUNT=$(nvram get sc_mount)
IS_INSTALLED=$(nvram get sc_installed)
PLUGINS=$(find /jffs/softcenter/webs/ -name 'Module_*.asp' 2>/dev/null)
[ -f /jffs/softcenter/.sc_mounted ] && IS_MOUNTED="1" || IS_MOUNTED="0"
[ -f /jffs/softcenter/.sc_cifs ] && IS_CIFS="1" || IS_CIFS="0"

set_skin(){
	local UI_TYPE=ASUSWRT
	local SC_SKIN=$(nvram get sc_skin)
	local TS_FLAG=$(grep -o "2ED9C3" /www/css/difference.css 2>/dev/null|head -n1)
	local ROG_FLAG=$(cat /www/form_style.css|grep -A1 ".tab_NW:hover{"|grep "background"|sed 's/,//g'|grep -o "2071044")
	local TUF_FLAG=$(cat /www/form_style.css|grep -A1 ".tab_NW:hover{"|grep "background"|sed 's/,//g'|grep -o "D0982C")
	local WRT_FLAG=$(cat /www/form_style.css|grep -A1 ".tab_NW:hover{"|grep "background"|sed 's/,//g'|grep -o "4F5B5F")

	if [ -n "${TS_FLAG}" ];then
		UI_TYPE="TS"
	else
		if [ -n "${TUF_FLAG}" ];then
			UI_TYPE="TUF"
		fi
		if [ -n "${ROG_FLAG}" ];then
			UI_TYPE="ROG"
		fi
		if [ -n "${WRT_FLAG}" ];then
			UI_TYPE="ASUSWRT"
		fi
	fi
	if [ -z "${SC_SKIN}" -o "${SC_SKIN}" != "${UI_TYPE}" ];then
		nvram set sc_skin="${UI_TYPE}"
		nvram commit
	fi

	# compatibile
	if [ "${UI_TYPE}" == "ASUSWRT" ];then
		ln -sf /jffs/softcenter/res/softcenter_asus.css /jffs/softcenter/res/softcenter.css
	elif [ "${UI_TYPE}" == "ROG" ];then
		ln -sf /jffs/softcenter/res/softcenter_rog.css /jffs/softcenter/res/softcenter.css
	elif [ "${UI_TYPE}" == "TUF" ];then
		ln -sf /jffs/softcenter/res/softcenter_tuf.css /jffs/softcenter/res/softcenter.css
	elif [ "${UI_TYPE}" == "TS" ];then
		ln -sf /jffs/softcenter/res/softcenter_ts.css /jffs/softcenter/res/softcenter.css
	fi
}

mkdir -p /jffs/softcenter

if [ $SPACE_AVAL -gt 10240 -a "$SC_MOUNT" == "0" -a "$IS_MOUNTED" == "0" ];then
#jffs
	mkdir -p /jffs/softcenter/init.d
	mkdir -p /jffs/softcenter/bin
	mkdir -p /jffs/softcenter/etc
	mkdir -p /jffs/softcenter/scripts
	mkdir -p /jffs/softcenter/webs
	mkdir -p /jffs/softcenter/res
	mkdir -p /jffs/softcenter/ss
	mkdir -p /jffs/softcenter/lib
	mkdir -p /jffs/configs/dnsmasq.d
	mkdir -p /jffs/softcenter/configs
elif [ "$SC_MOUNT" == "2" ];then
#cifs
	url=$(nvram get sc_cifs_url)
	user=$(nvram get sc_cifs_user)
	pw=$(nvram get sc_cifs_pw)
	if [ "$user" == "" ];then
		opt="username=$user"
		if [ -n "$pw" ];then
			opt="${opt},password=$pw"
		fi
	else
		opt="username=guest"
	fi
	mount -t cifs "${url}" -o "${opt},rw,dir_mode=0777,file_mode=0777" /jffs/softcenter
	mkdir -p /jffs/softcenter/init.d
	mkdir -p /jffs/softcenter/bin
	mkdir -p /jffs/softcenter/etc
	mkdir -p /jffs/softcenter/scripts
	mkdir -p /jffs/softcenter/webs
	mkdir -p /jffs/softcenter/res
	mkdir -p /jffs/softcenter/ss
	mkdir -p /jffs/softcenter/lib
	mkdir -p /jffs/configs/dnsmasq.d
	mkdir -p /jffs/softcenter/configs
	touch /jffs/softcenter/.sc_cifs
elif [ "$SC_MOUNT" == "1" ];then
#usb
	mdisk=`nvram get sc_disk`
	usb_disk="/tmp/mnt/$mdisk"
	fat=$(mount |grep $usb_disk |grep fat)
	[ -n "$fat" ] && logger "Unsupport TFAT!" && exit 1
	if [ ! -d "$usb_disk" ]; then
		nvram set sc_mount="0"
		nvram commit
		logger "USB flash drive not detected!/没有找到可用的USB磁盘!" 
		exit 1
	else
		if [ "$PLUGINS" != "" ];then
			cp -rf /jffs/softcenter/bin $usb_disk
			cp -rf /jffs/softcenter/res $usb_disk
			cp -rf /jffs/softcenter/webs $usb_disk
			cp -rf /jffs/softcenter/scripts $usb_disk
			cp -rf /jffs/softcenter/lib $usb_disk
		else
			mkdir -p /jffs/softcenter
			mkdir -p $usb_disk/bin
			mkdir -p $usb_disk/res
			mkdir -p $usb_disk/webs
			mkdir -p $usb_disk/scripts
			mkdir -p $usb_disk/lib
		fi
		rm -rf /jffs/softcenter/bin /jffs/softcenter/res /jffs/softcenter/webs /jffs/softcenter/scripts /jffs/softcenter/lib
		mkdir -p /jffs/softcenter/etc
		mkdir -p /jffs/softcenter/init.d
		mkdir -p /jffs/softcenter/configs
		mkdir -p /jffs/softcenter/ss
		ln -sf $usb_disk/bin /jffs/softcenter/
		ln -sf $usb_disk/res /jffs/softcenter/
		ln -sf $usb_disk/webs /jffs/softcenter/
		ln -sf $usb_disk/scripts /jffs/softcenter/
		ln -sf $usb_disk/lib /jffs/softcenter/
		touch /jffs/softcenter/.sc_mounted
		cd $usb_disk && touch .sc_installed
	fi
elif [ "$SC_MOUNT" == "0" -a "$IS_MOUNTED" == "1" ];then
#uninstall
	mdisk=`nvram get sc_disk`
	usb_disk="/tmp/mnt/$mdisk"
	rm -rf $usb_disk/bin $usb_disk/res $usb_disk/webs $usb_disk/scripts $usb_disk/lib
	rm -rf /jffs/softcenter/*
	rm -rf /tmp/mnt/*/.sc_installed
	rm -rf /jffs/softcenter/.sc_mounted
	nvram set sc_installed=0
	exit 1
elif [ "$SC_MOUNT" == "0" -a "$IS_CIFS" == "1" ];then
#uninstall
	rm -rf /jffs/softcenter/*
	rm -rf /jffs/softcenter/.sc_cifs
	rm -rf /jffs/softcenter/.soft_ver
	umount /jffs/softcenter -l
	nvram set sc_installed=0
	exit 1
else
	logger "Not enough free space for JFFS!/当前jffs分区剩余空间不足!"
	logger "Exit!/退出安装!"
	exit 1
fi
cp -rf /rom/etc/softcenter/automount.sh /jffs/softcenter/
if [ "$PLUGINS" = "" ];then
	cp -rf /rom/etc/softcenter/scripts/* /jffs/softcenter/scripts/
	cp -rf /rom/etc/softcenter/res/* /jffs/softcenter/res/
	cp -rf /rom/etc/softcenter/webs/* /jffs/softcenter/webs/
	cp -rf /rom/etc/softcenter/bin/* /jffs/softcenter/bin/

	set_skin
fi
cd /jffs/softcenter/bin && ln -sf /usr/sbin/base64_encode base64_encode
cd /jffs/softcenter/bin && ln -sf /usr/sbin/base64_encode base64_decode
cd /jffs/softcenter/bin && ln -sf /usr/sbin/versioncmp versioncmp
cd /jffs/softcenter/bin && ln -sf /usr/sbin/resolveip resolveip
cd /jffs/softcenter/bin && ln -sf /usr/bin/jq jq
cd /jffs/softcenter/scripts && ln -sf ks_app_install.sh ks_app_remove.sh
chmod 755 /jffs/softcenter/scripts/*.sh
chmod 755 /jffs/softcenter/configs/*.sh
chmod 755 /jffs/softcenter/bin/*
chmod 755 /jffs/softcenter/init.d/*
chmod 755 /jffs/softcenter/automount.sh
echo 1.5.7 > /jffs/softcenter/.soft_ver
dbus set softcenter_api="1.5"
dbus set softcenter_version=`cat /jffs/softcenter/.soft_ver`
nvram set sc_installed=1
nvram commit

/jffs/softcenter/bin/sc_auth arch
/jffs/softcenter/bin/sc_auth tcode

mkdir -p /jffs/scripts

# creat profile file
if [ ! -f /jffs/configs/profile.add ]; then
cat > /jffs/configs/profile.add <<EOF
alias ls='ls -Fp --color=auto'
alias ll='ls -lFp --color=auto'
alias l='ls -AlFp --color=auto'
source /jffs/softcenter/scripts/base.sh

EOF
fi

