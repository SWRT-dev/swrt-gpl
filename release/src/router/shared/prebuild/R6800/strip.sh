#!/bin/sh

for file in $(find ./ -name '*.o') ;
do
echo "strip $file"
/opt/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.24/bin/mipsel-openwrt-linux-musl-strip  --strip-unneeded $file
done
