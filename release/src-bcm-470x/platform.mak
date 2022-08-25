-include $(SRCBASE)/router/.config

export LINUXDIR := $(SRCBASE)/linux/linux-4.4
export BUILD := $(shell (gcc -dumpmachine))

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -DBCMARM -fno-delete-null-pointer-checks -marm -DRTCONFIG_MUSL_LIBC -DLINUX26 -DCONFIG_BCMWL5
endif

export KERNEL_BINARY=$(LINUXDIR)/arch/arm/boot/zImage
export MUSL32 :=y
export MUSL_LIBC :=y
export TOOLS := $(SRCBASE)/../../toolchains/toolchain-arm_cortex-a9_gcc-8.2.0_musl_eabi
export PLATFORM_ARCH := arm-musl
export CROSS_COMPILE := $(TOOLS)/bin/arm-openwrt-linux-
export CROSS_COMPILER := $(CROSS_COMPILE)
export CONFIGURE := ./configure --host=arm-linux --build=$(BUILD)
export HOSTCONFIG := linux-armv4
export BCMEX := _arm
export EXTRA_FLAG := -lgcc_s
export ARCH := arm
export HOST := arm-linux
export BCMSUB := brcmarm
export KERNELCC := $(CROSS_COMPILE)gcc
export KERNELLD := $(CROSS_COMPILE)ld
LIBRT=$(wildcard $(TOOLS)/lib/librt-*.so)
ifneq ($(LIBRT),)
export RTVER := $(patsubst librt-%.so,%,$(shell basename $(wildcard $(TOOLS)/lib/librt-*.so)))
endif
export STAGING_DIR=$(TOOLS)
ifeq ($(PLATFORM),)
export PLATFORM := $(PLATFORM_ARCH)
endif

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_BCMWL5 -DDEBUG_NOISY -DDEBUG_RCTEST -D_GNU_SOURCE -D_BSD_SOURCE -pipe -DTTEST -mcpu=cortex-a9 -mtune=cortex-a9  -msoft-float -mfloat-abi=soft -fno-caller-saves -fno-plt -fcommon -DRTCONFIG_MUSL_LIBC

export CONFIG_LINUX26=y
export CONFIG_BCMWL5=y
export OPENSSL11=y


define platformRouterOptions
	@( \
	if [ "$(RTAC5300)" = "y" ] ; then \
		sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
		echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
	fi; \
	if [ "$(RTAC3200)" = "y" ] ; then \
		sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
		echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
	fi; \
	if [ "$(R8500)" = "y" ] ; then \
		sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
		echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
	fi; \
	if [ "$(R8000)" = "y" ] ; then \
		sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
		echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
	fi; \
	)
endef

define platformBusyboxOptions
endef

define platformKernelConfig
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)
