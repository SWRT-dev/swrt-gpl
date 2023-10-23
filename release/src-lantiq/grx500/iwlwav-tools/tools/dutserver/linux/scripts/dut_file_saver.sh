#!/bin/sh

#/******************************************************************************
#
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
#         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
#         Copyright 2009 - 2014 Lantiq Deutschland GmbH
#         Copyright 2007 - 2008 Infineon Technologies AG
#
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
#
#*******************************************************************************/

FIRMWARE_DIR=`grep FIRMWARE_DIR= /etc/hotplug/firmware.agent | sed 's/FIRMWARE_DIR=//'`

src_file=$1
dst_file=$FIRMWARE_DIR/$2

if test ! -f "$src_file"; then
  echo "error: file '$src_file' do not exists" >&2
  exit -1
fi

cp "$src_file" "$dst_file"
_ret=$?
if test $_ret -eq 0; then
  echo "file '$dst_file' saved."
else
  echo "error: failed to save file '$dst_file'" >&2
fi

exit $_ret
