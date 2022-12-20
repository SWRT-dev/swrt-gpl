export LINUXDIR := $(SRCBASE)/linux/linux-5.4.x

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -mips32 -mtune=mips32 -msoft-float -DBB_SOC
endif

export RTVER := 0.9.30.1
export ATEVER := 2.0.2
export OPENSSL11=y


export BUILD := $(shell (gcc -dumpmachine))
export KERNEL_BINARY=$(LINUXDIR)/vmlinux
export MUSL32=y
export PLATFORM_ROUTER := en7561
export PLATFORM := mipsel-musl
export PLATFORM_ARCH := mipsel-musl
export TOOLS :=/opt/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.24
export CROSS_COMPILE := $(TOOLS)/bin/mipsel-openwrt-linux-musl-
export CROSS_COMPILER := $(CROSS_COMPILE)
export READELF := $(TOOLS)/bin/mipsel-openwrt-linux-musl-readelf
export CONFIGURE := ./configure --host=mipsel-linux --build=$(BUILD)
export HOSTCONFIG := linux-mipsel
export ARCH := mips
export HOST := mipsel-linux
export KERNELCC := $(TOOLS)/bin/mipsel-openwrt-linux-musl-gcc
export KERNELLD := $(TOOLS)/bin/mipsel-openwrt-linux-musl-ld
export STAGING_DIR := $(TOOLS)

# for OpenWRT SDK
export DTS_DIR := $(LINUXDIR)/arch/$(ARCH)/boot/dts
ifneq ($(BUILD_NAME),)
export PROFILE :=$(BUILD_NAME)
include $(SRCBASE)/$(PLATFORM_ROUTER)/Project/$(PROFILE)/$(PROFILE).profile
include $(SRCBASE)/$(PLATFORM_ROUTER)/mak/rule.mak
include $(SRCBASE)/$(PLATFORM_ROUTER)/mak/dir.mak
endif

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_RALINK -pipe -DDEBUG_NOISY -DDEBUG_RCTEST -DBB_SOC

export CONFIG_LINUX26=y
export CONFIG_RALINK=y
export BB_SOC=y

#EXTRA_CFLAGS += -DLINUX30
export CONFIG_LINUX30=y
export CONFIG_SUPPORT_OPENWRT=y
define platformRouterOptions
	@( \
	if [ "$(RALINK)" = "y" ]; then \
		sed -i "/RTCONFIG_RALINK\>/d" $(1); \
		echo "RTCONFIG_RALINK=y" >>$(1); \
		sed -i "/CONFIG_RA_HW_NAT_IPV6/d" $(1); \
		echo "# CONFIG_RA_HW_NAT_IPV6 is not set" >>$(1); \
		if [ "$(PMF)" = "y" ]; then \
			sed -i "/RTCONFIG_MFP/d" $(1); \
			echo "RTCONFIG_MFP=y" >>$(1); \
		fi; \
		if [ "$(EN7561)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_EN7561/d" $(1); \
			echo "RTCONFIG_RALINK_EN7561=y" >>$(1); \
		fi; \
		if [ "$(FIRST_IF)" = "MT7915D" ]; then \
			sed -i "/RTCONFIG_WLMODULE_MT7915D_AP/d" $(1); \
			echo "RTCONFIG_WLMODULE_MT7915D_AP=y" >> $(1); \
		fi; \
		if [ "$(FIRST_IF)" = "MT7615E" ]; then \
			sed -i "/RTCONFIG_WLMODULE_MT7615E_AP/d" $(1); \
			echo "RTCONFIG_WLMODULE_MT7615E_AP=y" >> $(1); \
		fi; \
	fi; \
	)
endef

define platformBusyboxOptions
endef

define platformKernelConfig
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)

