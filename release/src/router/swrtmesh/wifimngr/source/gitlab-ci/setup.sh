#!/bin/bash

set -euxo pipefail

echo "preparation script"

pwd

cp -r ./test/files/etc/* /etc/
cp -r ./schemas/ubus/* /usr/share/rpcd/schemas

ls /etc/config/
ls /usr/share/rpcd/schemas/

