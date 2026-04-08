#!/bin/sh

BASE_PATH=$1
DRIVER_PATH=$2
CFLAGS=$3
CROSS_COMPILE=$4

COMPRESSED_CONFIG=`cat $BASE_PATH/.config |
		   grep -v '^\#.*'         | \
		   grep -v '^$$'           | \
		   grep -v '_BACKPORTED_'  | \
		   grep -e 'IWLWAV'	     \
			-e 'CFG80211'	     \
			-e 'MAC80211'	   | \
                   sed -e's/^CONFIG_//'      \
                       -e's/=y//'            \
                       -e's/\"/\\\\\"/g' | \
		   awk '{printf $$0 " ";}'`

DRV_INFO_PATH="$DRIVER_PATH/wireless/driver/drv_info.c"
echo "" > $DRV_INFO_PATH
echo "const char *mtlk_drv_info[] = {" >> $DRV_INFO_PATH
echo "  \"CONFIG=$COMPRESSED_CONFIG\"," >> $DRV_INFO_PATH
echo "  \"CFLAGS="$(echo "$CFLAGS" | sed "s/\"/\'/g")"\"," >> $DRV_INFO_PATH
echo "  \"CROSS_COMPILE=$CROSS_COMPILE\"," >> $DRV_INFO_PATH
echo "  (const char *)0" >> $DRV_INFO_PATH
echo "};" >> $DRV_INFO_PATH
echo "" >> $DRV_INFO_PATH
