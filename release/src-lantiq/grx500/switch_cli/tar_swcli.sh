#!/bin/bash
PKG_NAME="switch_cli_ugw8"
PKG_VERSION=$1
PKG_SOURCE=$PKG_NAME-$PKG_VERSION.tar.gz
USERNAME=$(whoami)

echo $PKG_SOURCE
echo $USERNAME
cd gsw_cli_dev/
make distclean
rm -rf Makefile.in aclocal.m4 autom4te.cache compile configure depcomp install-sh missing switch_cli_config.h.in
cd ../
tar -czvf $PKG_SOURCE gsw_cli_dev/
scp $PKG_SOURCE $USERNAME@trinity.vr.intel.com:/home/wlnsw/dist
