#!/bin/sh
#
# $Id$
#
# This script is intended to build all stable components
# for all known platforms
#
# Copyright (c) 2006-2009 Metalink Broadband (Israel)
#

for file in `ls ./support/release/configs/*.config`; do
date
PLATFORM_NAME=`basename $file | sed -e 's/.config$//'`
echo ============ BUILDING $PLATFORM_NAME ============
rm -rf $PLATFORM_NAME || exit $?
./support/release/build-platform.sh $PLATFORM_NAME $PLATFORM_NAME all.stable || exit $?
make distcheck || exit $?
echo ============ DONE $PLATFORM_NAME ============
done
date
