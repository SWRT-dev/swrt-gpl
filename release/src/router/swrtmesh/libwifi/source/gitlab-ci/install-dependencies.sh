#!/bin/bash

echo "install dependencies"

home=$(pwd)

function exec_cmd()
{
	echo "executing $@"
	$@ >/dev/null 2>&1
	local ret=$?

	if [ "${ret}" -ne 0 ]; then
		echo "Failed to execute $@ ret (${ret})"
		exit 1
	fi
}

# libeasy.so
rm -fr libeasy
mkdir -p /usr/include/easy
exec_cmd git clone -b devel https://dev.iopsys.eu/hal/libeasy.git
cd libeasy
exec_cmd make CFLAGS+="-I/usr/include/libnl3"
exec_cmd cp -a libeasy*.so* /usr/lib
exec_cmd cp -a easy.h event.h utils.h if_utils.h debug.h hlist.h /usr/include/easy/
