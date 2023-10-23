
###############################################################################
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
# 
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
# 
###############################################################################

ARCH=mips
CROSS_COMPILE=mips-openwrt-linux-musl-

TOPDIR=../../../../../../openwrt
LINUX_DIR=$(TOPDIR)/../source/linux

export STAGING_DIR:=../../openwrt/staging_dir/toolchain-mips_24kc+nomips16_gcc-8.3.0_musl
export PATH:=$(STAGING_DIR)/bin:$(PATH)

