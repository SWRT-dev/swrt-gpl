#!/bin/sh
ln -s /var/tmp/jffs2/images/ap_upper.bin /tmp/ap_upper.bin

test "x$1" = xstart && rmmod mtlk;
test "x$1" = xstart && insmod /var/tmp/jffs2/driver/mtlkd.ko dut=1;
test "x$1" = xstop && rmmod mtlk;
exit 0;