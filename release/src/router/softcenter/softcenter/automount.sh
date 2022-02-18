#!/bin/sh

mdisk=`nvram get sc_disk`
cd /tmp/mnt
if [ ! -d "$mdisk" ]; then
	list_disk=`ls /tmp/mnt`
	for cur_disk in $list_disk
	do
		if [ -f "$cur_disk/.sc_installed" ]; then
			ln -sf $cur_disk $mdisk
		fi
	done
fi

