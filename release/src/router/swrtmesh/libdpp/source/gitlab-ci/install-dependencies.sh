#!/bin/bash

set -euxo pipefail

echo "install dependencies"

pwd

# libeasy
cd /opt/dev
rm -fr libeasy
git clone -b devel https://dev.iopsys.eu/hal/libeasy.git
cd libeasy
make CFLAGS+="-I/usr/include/libnl3"
mkdir -p /usr/include/easy
cp easy.h event.h utils.h if_utils.h debug.h hlist.h bufutil.h cryptutil.h ecc_cryptutil.h /usr/include/easy
cp -a libeasy*.so* /usr/lib

# libwifi
cd /opt/dev
rm -fr libwifi
git clone -b devel https://dev.iopsys.eu/hal/libwifi.git
cd libwifi
make HAS_WIFI=1 WIFI_TYPE=TEST
cp wifidefs.h wifiutils.h wifiops.h wifi.h /usr/include
cp -a libwifi*.so* /usr/lib
sudo ldconfig
