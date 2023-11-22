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

# openssl-3.x
apt update > /dev/null 2>&1
apt install -y libssl-dev >/dev/null 2>&1
