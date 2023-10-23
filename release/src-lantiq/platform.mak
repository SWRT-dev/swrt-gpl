export LINUXDIR := $(SRCBASE)/linux/linux-4.9.x

ifeq ($(EXTRACFLAGS),)
EXTRACFLAGS := -DCONFIG_LANTIQ -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -DLINUX30 -mips32r2 -mno-branch-likely -mtune=1004kc
endif
export KERNEL_BINARY=$(LINUXDIR)/vmlinux
export PLATFORM := mips-musl
export PLATFORM_ROUTER := grx500
export MODEL_EXT := _lantiq
export TOOLCHAIN_NAME=toolchain-mips_24kc+nomips16_gcc-8.3.0_musl
export TOOLS := $(SRCBASE)/tools/${TOOLCHAIN_NAME}
export STAGING_DIR := $(TOOLS)
export CROSS_COMPILE := $(STAGING_DIR)/bin/mips-openwrt-linux-musl-
export CROSS_COMPILER := $(CROSS_COMPILE)
export TOOLCHAIN_HOST_NAME=host
export READELF := $(STAGING_DIR)/mips-openwrt-linux-musl-readelf
export CONFIGURE := ./configure --host=mips-linux --build=$(BUILD)
export HOSTCONFIG := linux-mips
export ARCH := mips
export HOST := mips-linux
export KERNELCC := $(CROSS_COMPILE)gcc
export KERNELLD := $(CROSS_COMPILE)ld
export CC := $(CROSS_COMPILE)gcc
export RTVER := 0.9.30.1
export RTVER := 0.9.30.1

# Kernel load address and entry address
export LOADADDR := 41508000
export ENTRYADDR := $(LOADADDR)
export s_load_addr := 0xa0020000
export s_entry_addr := 0xa002df00
export image_header := MIPS LTQCPE Linux-4.9.x
export compression_type := lzma
export ENTRYADDR := $(LOADADDR)

export CONFIG_LINUX26=y
export CONFIG_LANTIQ=y

EXTRA_CFLAGS += -DLINUX30
EXTRA_BLUEZ_CFLAGS += -DRTCONFIG_LANTIQ -I$(SRCBASE)/include -I$(TOP)/shared -I$(TOP)/bluez-5.41/src/bleencrypt/include
EXTRA_BLUEZ_LDFLAGS += -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared
export CONFIG_LINUX30=y

# not for ALPINE EXTRA_CFLAGS := -DLINUX26 -DCONFIG_BCMWL5 -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -DTTEST
EXTRA_CFLAGS := -DLINUX26 -DCONFIG_LANTIQ -DCONFIG_INTEL -pipe -DDEBUG_NOISY -DDEBUG_RCTEST -mfpu=vfpv3-d16 -mfloat-abi=softfp
EXTRACFLAGS += -DLINUX26

define platformRouterOptions
	@( \
	if [ "$(INTEL)" = "y" ]; then \
	        sed -i "/RTCONFIG_INTEL\>/d" $(1); \
	        echo "RTCONFIG_INTEL=y" >>$(1); \
	fi; \
	)
endef

define platformBusyboxOptions
endef

define platformKernelConfig
endef
export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)
