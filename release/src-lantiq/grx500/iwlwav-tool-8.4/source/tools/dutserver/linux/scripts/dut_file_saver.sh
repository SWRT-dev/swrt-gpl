#!/bin/sh

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
