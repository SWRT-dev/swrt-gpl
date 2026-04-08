#!/bin/sh

update_config_file_if_needed()
{
  cd $1
	if [ "`cat $HAPD_CONF_FILE | grep $CC_CONFIG_DONE`" != "$CC_CONFIG_DONE" ]
	then
		echo $LIBNL_INC_CMD >> $HAPD_CONF_FILE
		echo $CFLAGS_CMD >> $HAPD_CONF_FILE
		echo $LDFLAGS_CMD >> $HAPD_CONF_FILE
		echo $IFX_LDFLAGS_CMD >> $HAPD_CONF_FILE
		echo $CC_CMD >> $HAPD_CONF_FILE
		echo $LD_CMD >> $HAPD_CONF_FILE
		echo $STRIP_CMD >> $HAPD_CONF_FILE
		echo $CC_CONFIG_DONE >> $HAPD_CONF_FILE
		
		if [ "$#" = "2" ]
		then
			cp -r $srcdir/$HAPD_SRC_DIR $srcdir/$HAPD_SRC_DIR-orig
		fi
	fi
}

hostapd_clean()
{
	cd $srcdir
	if [ ! -d "$HAPD_SRC_DIR" ]
	then
		echo "Do make hostap first"
		exit 1
	fi

	cd $HAPD_SRC_DIR/hostapd
	make clean
}

supplicant_clean()
{
	cd $srcdir
	if [ ! -d "$HAPD_SRC_DIR" ]
	then
		echo "Do make hostap first"
		exit 1
	fi

	cd $HAPD_SRC_DIR/wpa_supplicant
	make clean
}

get_toolchain_path()
{
  echo $DEFAULT_TOOLCHAIN_PATH
}

get_kernel_cross_compile()
{
  echo $DEFAULT_KERNEL_CROSS_COMPILE
}

srcdir=$PWD
source support/$1.env
export STAGING_DIR

TASK=$2
HAPD_TAR_DIR=tools/wifi_opensource
HAPD_TAR_NAME=`ls $HAPD_TAR_DIR | grep "hostapd-.*\.tar.*"`
HAPD_VERSION=`echo $HAPD_TAR_NAME | sed 's/hostapd-\(.*\)\.tar.*/\1/'`
HAPD_PATCH_DIR=tools/patches/hostapd_$HAPD_VERSION
PATCHES=`ls -v $HAPD_PATCH_DIR | grep "\.patch"`
BUILDS_DIR=builds/$1
HAPD_BUILD_DIR=$BUILDS_DIR/tools/wifi_opensource
HAPD_SRC_DIR=$HAPD_BUILD_DIR/hostapd-$HAPD_VERSION
HAPD_CONF_FILE=.config
CC_CONFIG_DONE="#MTLK_HAPD_CC_CONFIG_DONE"

if [ "$TASK" = "hostapd_clean" ]
then
	hostapd_clean
	exit 0
fi

if [ "$TASK" = "supplicant_clean" ]
then
	supplicant_clean
	exit 0
fi

if [ "$TASK" = "hostap_clean" ]
then
	hostapd_clean
	supplicant_clean
	exit 0
fi

if [ "$TASK" = "diff" ]
then
	if [ ! -d "$HAPD_SRC_DIR" ]
	then
		echo "Do make hostapd first"
		exit 1
	fi

	hostapd_clean > /dev/null
	supplicant_clean > /dev/null

	cd $srcdir/$HAPD_BUILD_DIR
	DATE=`date +%Y_%m_%d-%H_%M_%S`
	DIFF_FILE=cr-hostapd-$DATE.diff
	diff -Npur -X $srcdir/support/hostap-diff-exclude hostapd-$HAPD_VERSION-orig hostapd-$HAPD_VERSION > $DIFF_FILE
	cp $DIFF_FILE $srcdir
	exit 0
fi

if [ "$TASK" = "hostap" ] || [ "$TASK" = "hostapd" ] || [ "$TASK" = "supplicant" ]
then
	if [ ! -d "$HAPD_BUILD_DIR" ]
	then
		mkdir $HAPD_BUILD_DIR
	fi

	if [ ! -d "$HAPD_BUILD_DIR/hostapd-$HAPD_VERSION" ]
	then
		tar xvjf $HAPD_TAR_DIR/$HAPD_TAR_NAME -C $HAPD_BUILD_DIR

		cd $HAPD_SRC_DIR
		for file in $PATCHES
		do
			echo "--- Applying patch ${file} ---"
			patch -p1 < $srcdir/$HAPD_PATCH_DIR/$file
			if [ "$?" != "0" ]
			then
				echo "Hostapd patch applying failed!"
				exit 1
			fi
		done

		cd $srcdir
	fi
	
	if [ -z $YOCTO_TAG ] 
	then
		LD="${_BASE_TOOLCHAIN_PATH}/bin/*-linux-ld"
		STRIP="${_BASE_TOOLCHAIN_PATH}/bin/*-linux-strip"
		LIBNL="$_BASE_TARGET_BUILD_PATH/libnl-3*"
	else
		LD="${_BASE_TARGET_CROSS}ld"
		STRIP="${_BASE_TARGET_CROSS}strip"
		LIBNL="$DEFAULT_TOOLCHAIN_PATH/../work/core2-*-linux/libnl/3*/libnl-3*"
	fi

	LIBNL_INC_CMD="LIBNL_INC := $LIBNL/include"
	CFLAGS_CMD="CFLAGS += $CPPFLAGS"
	LDFLAGS_CMD="LDFLAGS += $LDFLAGS"
	IFX_LDFLAGS_CMD="IFX_LDFLAGS := \$(LDFLAGS)"
	CC_CMD="CC := $CC"
	LD_CMD="LD := $LD"
	STRIP_CMD="STRIP := $STRIP"
	
	update_config_file_if_needed $srcdir/$HAPD_SRC_DIR/hostapd
	update_config_file_if_needed $srcdir/$HAPD_SRC_DIR/wpa_supplicant 1

	if [ "$TASK" = "hostap" ] || [ "$TASK" = "hostapd" ]
	then
		cd $srcdir/$HAPD_SRC_DIR/hostapd
		make
		if [ "$?" != "0" ]
		then
			exit 1
		fi	
	fi

	if [ "$TASK" = "hostap" ] || [ "$TASK" = "supplicant" ]
	then
		cd $srcdir/$HAPD_SRC_DIR/wpa_supplicant
		make
	fi
fi
