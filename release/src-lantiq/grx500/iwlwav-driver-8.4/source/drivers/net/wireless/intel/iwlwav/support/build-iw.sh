#!/bin/sh

get_toolchain_path()
{
  echo $DEFAULT_TOOLCHAIN_PATH
}
srcdir=$PWD
source support/$1.env

TASK=$2
IW_TAR_DIR=tools/wifi_opensource
IW_TAR_NAME=`ls $IW_TAR_DIR | grep "iw-.*\.tar.*"`
IW_VERSION=`echo $IW_TAR_NAME | sed 's/iw-\(.*\)\.tar.*/\1/'`
IW_PATCH_DIR=tools/patches/iw_$IW_VERSION
PATCHES=`ls -v $IW_PATCH_DIR | grep "\.patch"`
BUILDS_DIR=builds/$1
IW_BUILD_DIR=$BUILDS_DIR/tools/wifi_opensource
IW_SRC_DIR=$IW_BUILD_DIR/iw-$IW_VERSION

if [ "$TASK" = "clean" ]
then
	if [ ! -d "$IW_SRC_DIR" ]
	then
		echo "Do make IW first"
		exit 1
	fi

	cd $IW_SRC_DIR
	make clean
	exit 0
fi

if [ "$TASK" = "diff" ]
then
	if [ ! -d "$IW_SRC_DIR" ]
	then
		echo "Do make IW first"
		exit 1
	fi

	cd $IW_SRC_DIR
	make clean
	cd $srcdir/$IW_BUILD_DIR
	DATE=`date +%Y_%m_%d-%H_%M_%S`
	DIFF_FILE=cr-iw-$DATE.diff
	diff -Npur iw-$IW_VERSION-orig iw-$IW_VERSION > $DIFF_FILE
	cp $DIFF_FILE $srcdir
	exit 0
fi

if [ "$TASK" = "make" ]
then
	if [ ! -d "$IW_BUILD_DIR" ]
	then
		mkdir $IW_BUILD_DIR
	fi

	if [ ! -d "$IW_BUILD_DIR/iw-$IW_VERSION" ]
	then
		tar xvf $IW_TAR_DIR/$IW_TAR_NAME -C $IW_BUILD_DIR

		cd $IW_SRC_DIR
		for file in $PATCHES
		do
			patch -p1 < $srcdir/$IW_PATCH_DIR/$file
			if [ "$?" != "0" ]
			then
				echo "Failed to patch IW !"
				exit 1
			fi
		done

		cd $srcdir
	fi

	cd $IW_SRC_DIR
	
	LIBNL=`echo $_BASE_TARGET_BUILD_PATH/libnl-3*`
	export BINDIR=$_BASE_TOOLCHAIN_PATH/bin
	export CC=`echo $BINDIR/*-openwrt-linux-uclibc-gcc`
	export LD=`echo $BINDIR/*-openwrt-linux-uclibc-ld`
	export STRIP=`echo $BINDIR/*-openwrt-linux-uclibc-strip`
	export CFLAGS+="-I$LIBNL/include"
	export LDFLAGS+="-L$_BASE_TARGET_PATH/usr/lib"
	export STAGING_DIR
	export PKG_CONFIG_PATH="$_BASE_TARGET_PATH/usr/lib/pkgconfig:$_BASE_TARGET_PATH/usr/share/pkgconfig"
	
	cp -r $srcdir/$IW_SRC_DIR $srcdir/$IW_SRC_DIR-orig

	make
fi
