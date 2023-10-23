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

ln -s /var/tmp/jffs2/images/ap_upper.bin /tmp/ap_upper.bin

test "x$1" = xstart && rmmod mtlk;
test "x$1" = xstart && insmod /var/tmp/jffs2/driver/mtlkd.ko dut=1;
test "x$1" = xstop && rmmod mtlk;
exit 0;