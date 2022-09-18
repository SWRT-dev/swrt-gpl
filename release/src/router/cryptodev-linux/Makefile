#
# Since version 1.6 the asynchronous mode has been
# disabled by default. To re-enable it uncomment the
# corresponding CFLAG.
#
# Patched for BRCM/LANTIQ/QCA/MTK SDK
# Copyright 2021-2022, SWRTdev.
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND (SWRTdev) GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.

include $(TOP)/.config
PWD=$(shell pwd)
VERSION = 1.12

ifeq ($(or $(RTCONFIG_SOC_IPQ50XX),$(RTCONFIG_SOC_IPQ60XX),$(RTCONFIG_SOC_IPQ8064),$(RTCONFIG_SOC_IPQ8074)),y)
HWCHIP=QCANSS
else ifeq ($(RTCONFIG_SOC_IPQ40XX),y)
HWCHIP=QCA
else ifeq ($(RTCONFIG_RALINK_MT7621),y)
HWCHIP=MT7621
else ifeq ($(RTCONFIG_RALINK_MT7622),y)
HWCHIP=MT7622
else ifeq ($(RTCONFIG_LANTIQ),y)
HWCHIP=LANTIQ
else
ifneq (,$(wildcard $(LINUXDIR)/../bcmkernel/Makefile))
#bcmsdk5.04
BCMSDK_INC=-I$(LINUXDIR)/../bcmkernel/include
endif
ifeq ($(BRCM_CHIP),$(filter $(BRCM_CHIP),63178 47622 6756 6750))
HWCHIP=BCM675X
else ifeq ($(BRCM_CHIP),$(filter $(BRCM_CHIP),4908))
HWCHIP=BCM49XX
ifeq ($(HND_ROUTER_AX),y)
ARM64CC=/opt/toolchains/crosstools-aarch64-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/usr/bin/aarch64-buildroot-linux-gnu-
SYSROOT=/opt/toolchains/crosstools-aarch64-gcc-5.5-linux-4.1-glibc-2.26-binutils-2.28.1/usr/aarch64-buildroot-linux-gnu/sysroot/usr/include
else
ARM64CC=/opt/toolchains/crosstools-aarch64-gcc-5.3-linux-4.1-glibc-2.22-binutils-2.25/usr/bin/aarch64-buildroot-linux-gnu-
SYSROOT=/opt/toolchains/crosstools-aarch64-gcc-5.3-linux-4.1-glibc-2.22-binutils-2.25/usr/aarch64-buildroot-linux-gnu/sysroot/usr/include
endif
else ifeq ($(BRCM_CHIP),$(filter $(BRCM_CHIP),4912))
HWCHIP=BCM49XX
ARM64CC=/opt/toolchains/crosstools-aarch64-gcc-9.2-linux-4.19-glibc-2.30-binutils-2.32/bin/aarch64-buildroot-linux-gnu-
SYSROOT=/opt/toolchains/crosstools-aarch64-gcc-9.2-linux-4.19-glibc-2.30-binutils-2.32/aarch64-buildroot-linux-gnu/sysroot/usr/include
else
#bcm470x?
HWCHIP=UNSUPPORTED
endif
endif

ifneq ($(ARM64CC),)
all: version.h
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(ARM64CC)) ARCH=arm64 EXTRA_CFLAGS="-I$(TOP)/shared -I$(PWD)/src -I$(SYSROOT) -D$(HWCHIP) -DENABLE_ASYNC $(BCMSDK_INC)" SUBDIRS=$(PWD)/src modules

else
all: version.h
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="-I$(TOP)/shared -I$(PWD)/src -D$(HWCHIP) -DENABLE_ASYNC $(BCMSDK_INC)" SUBDIRS=$(PWD)/src modules

endif

version.h:
	@echo "#define VERSION \"$(VERSION)\"" > src/version.h

ifneq ($(ARM64CC),)
install:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(ARM64CC)) ARCH=arm64 EXTRA_CFLAGS="-I$(TOP)/shared -I$(PWD)/src -I$(SYSROOT) -D$(HWCHIP) -DENABLE_ASYNC" SUBDIRS=$(PWD)/src INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(ARM64CC)strip -x

else
install:
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(TOP)/shared SUBDIRS=$(PWD)/src INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
	@find $(INSTALLDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR) -name "*.ko" | xargs $(STRIPX)

endif

clean:
	$(RM) src/*.o src/*.ko src/*.mod.* src/version.h

