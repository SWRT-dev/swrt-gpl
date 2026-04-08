#!/bin/sh

#WARNING: configure script must be called by relative path, not absolute
#         in other case, makefiles will receive absolute pathes in top_srcdir and 
#         srcdir variables and won't operate properly.
#         This is a long-running bug between autoconf and some shells (at least bash)
#         See http://sourceware.org/ml/automake/2000-03/msg00151.html for additional info.

CONFIG_FILE=$1
WORK_MODE=$2

. $PWD/$CONFIG_FILE || exit 1

if [ x"$WORK_MODE" = x"get_bld_tree_cfg" ]; then
echo builds/$CONFIG_ENVIRONMENT_NAME/.config builds/$CONFIG_ENVIRONMENT_NAME/.config.h
exit 0;
fi

if [ x"$WORK_MODE" = x"get_bld_dir" ]; then
echo builds/$CONFIG_ENVIRONMENT_NAME
exit 0;
fi

if [ x"$WORK_MODE" = x"get_bld_name" ]; then
echo $CONFIG_ENVIRONMENT_NAME
exit 0;
fi

if [ x"$WORK_MODE" = x"complete_makefiles_am" ]; then

SRC_ROOT_DIR=$3
DST_ROOT_DIR=$4

for file in `cat $SRC_ROOT_DIR/configure.ac | grep Makefile | sed -e "s/AC_CONFIG_FILES(\[//" -e "s/Makefile/Makefile.am/"`
do
  mkdir -p `dirname $DST_ROOT_DIR/$file` && touch $DST_ROOT_DIR/$file || exit $?;
done 
exit 0;

fi

( cd `readlink -f $PWD` && \
  (test ! -d builds/$CONFIG_ENVIRONMENT_NAME || chmod -R +w builds/$CONFIG_ENVIRONMENT_NAME) && \
  rm -rf builds/$CONFIG_ENVIRONMENT_NAME && \
  mkdir -p builds/$CONFIG_ENVIRONMENT_NAME && \
  cd builds/$CONFIG_ENVIRONMENT_NAME && \
  cp -f ../../.config . && \
  awk -f ../../support/make_cfg_header.awk < .config > .config.h && \
  ../../configure --host $CONFIG_HOST_TYPE \
                  --build=`../../config.guess` \
                  --with-app-toolchain=$APP_TOOLCHAIN_DIR \
                  --with-kernel=$KERNEL_DIR \
                  --with-kernel-cross-compile=$KERNEL_CROSS_COMPILE \
                  --prefix=`readlink -f .`/binaries && \
  exit 0 ) || ( rm -f .config .config.h && exit 1 )

