-include $(SRCBASE)/router/.config

export LINUXDIR := $(SRCBASE)/linux/linux-4.4.x
export BUILD := $(shell (gcc -dumpmachine))

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -DBCMARM -fno-delete-null-pointer-checks -marm -DRTCONFIG_MUSL_LIBC -DLINUX26 -DCONFIG_BCMWL5 -DMUSL_LIBC -mno-unaligned-access
endif

export KERNEL_BINARY=$(LINUXDIR)/arch/arm/boot/zImage
export MUSL32 :=y
export MUSL_LIBC :=y
export TOOLS := $(SRCBASE)/../../toolchains/toolchain-arm_cortex-a9_gcc-8.4.0_musl_eabi
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
export PLATFORM_ROUTER := bcm470x

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_BCMWL5 -DDEBUG_NOISY -DDEBUG_RCTEST -D_GNU_SOURCE -D_BSD_SOURCE -pipe -DTTEST -mcpu=cortex-a9 -mtune=cortex-a9  -msoft-float -mfloat-abi=soft -fno-caller-saves -fno-plt -fcommon -DRTCONFIG_MUSL_LIBC -DMUSL_LIBC -D__BIT_TYPES_DEFINED__ -mno-unaligned-access

export CONFIG_LINUX26=y
export CONFIG_LINUX30=y
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
	if [ "$(SBRAC3200P)" = "y" ] ; then \
		sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
		echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
	fi; \
	)
endef

define platformBusyboxOptions
	@( \
	if [ "$(CONFIG_BCMWL5)" = "y" ]; then \
		sed -i "/CONFIG_FEATURE_TOP_SMP_CPU/d" $(1); \
		echo "CONFIG_FEATURE_TOP_SMP_CPU=y" >>$(1); \
		sed -i "/CONFIG_FEATURE_TOP_DECIMALS/d" $(1); \
		echo "CONFIG_FEATURE_TOP_DECIMALS=y" >>$(1); \
		sed -i "/CONFIG_FEATURE_TOP_SMP_PROCESS/d" $(1); \
		echo "CONFIG_FEATURE_TOP_SMP_PROCESS=y" >>$(1); \
		sed -i "/CONFIG_FEATURE_TOPMEM/d" $(1); \
		echo "CONFIG_FEATURE_TOPMEM=y" >>$(1); \
		sed -i "/CONFIG_FEATURE_SHOW_THREADS/d" $(1); \
		echo "CONFIG_FEATURE_SHOW_THREADS=y" >>$(1); \
		sed -i "/CONFIG_DEVMEM/d" $(1); \
		echo "CONFIG_DEVMEM=y" >>$(1); \
		sed -i "/CONFIG_TFTP\>/d" $(1); \
		echo "CONFIG_TFTP=y" >>$(1); \
		sed -i "/CONFIG_FEATURE_TFTP_GET/d" $(1); \
		echo "CONFIG_FEATURE_TFTP_GET=y" >>$(1); \
		sed -i "/CONFIG_FEATURE_TFTP_PUT/d" $(1); \
		echo "CONFIG_FEATURE_TFTP_PUT=y" >>$(1); \
		sed -i "/CONFIG_SEQ\>/d" $(1); \
		echo "CONFIG_SEQ=y" >>$(1); \
	fi; \
	)
endef

define platformKernelConfig
	if [ "$(DEBUG)" = "y" ] ; then \
		sed -i "/CONFIG_BUG/d" $(1); \
		echo "CONFIG_BUG=y" >>$(1); \
		sed -i "/CONFIG_DEVKMEM/d" $(1); \
		echo "CONFIG_DEVKMEM=y" >>$(1); \
		sed -i "/CONFIG_PRINTK_TIME/d" $(1); \
		echo "CONFIG_PRINTK_TIME=y" >>$(1); \
		echo "# CONFIG_DEBUG_INFO is not set" >>$(1); \
		sed -i "/CONFIG_ENABLE_WARN_DEPRECATED/d" $(1); \
		echo "CONFIG_ENABLE_WARN_DEPRECATED=y" >>$(1); \
		sed -i "/CONFIG_STRIP_ASM_SYMS/d" $(1); \
		echo "# CONFIG_STRIP_ASM_SYMS is not set" >>$(1); \
		sed -i "/CONFIG_DEBUG_KERNEL/d" $(1); \
		echo "CONFIG_DEBUG_KERNEL=y" >>$(1); \
		sed -i "/CONFIG_STACKTRACE/d" $(1); \
		echo "CONFIG_STACKTRACE=y" >>$(1); \
		sed -i "/CONFIG_DEBUG_RT_MUTEXES/d" $(1); \
		echo "CONFIG_DEBUG_RT_MUTEXES=y" >>$(1); \
		sed -i "/CONFIG_DEBUG_SPINLOCK/d" $(1); \
		echo "CONFIG_DEBUG_SPINLOCK=y" >>$(1); \
		sed -i "/CONFIG_DEBUG_MUTEXES/d" $(1); \
		echo "CONFIG_DEBUG_MUTEXES=y" >>$(1); \
		sed -i "/CONFIG_DEBUG_WW_MUTEX_SLOWPATH/d" $(1); \
		echo "CONFIG_DEBUG_WW_MUTEX_SLOWPATH=y" >>$(1); \
		sed -i "/CONFIG_DEBUG_LOCK_ALLOC/d" $(1); \
		echo "CONFIG_DEBUG_LOCK_ALLOC=y" >>$(1); \
		sed -i "/CONFIG_PROVE_LOCKING/d" $(1); \
		echo "CONFIG_PROVE_LOCKING=y" >>$(1); \
		sed -i "/CONFIG_LOCK_STAT/d" $(1); \
		echo "CONFIG_LOCK_STAT=y" >>$(1); \
		echo "CONFIG_DEBUG_LOCKDEP=y" >>$(1); \
		echo "CONFIG_DEBUG_BUGVERBOSE=y" >>$(1); \
		echo "# CONFIG_PROVE_RCU_REPEATEDLY is not set" >>$(1); \
	fi; \
	if [ "$(R7000P)" = "y" ] || [ "$(SBRAC3200P)" = "y" ] || [ "$(RTAC68U)" = "y" ] ; then \
		sed -i "/CONFIG_DRAM_SIZE/d" $(1); \
		echo "CONFIG_DRAM_SIZE=0x10000000" >>$(1); \
	elif [ "$(RTAC88U)" = "y" ] || [ "$(RTAC3100)" = "y" ] || [ "$(RTAC5300)" = "y" ] ; then \
		sed -i "/CONFIG_DRAM_SIZE/d" $(1); \
		echo "CONFIG_DRAM_SIZE=0x20000000" >>$(1); \
	fi; \
	if [ "$(WIREGUARD)" = "y" ]; then \
		echo "# CONFIG_CRYPTO_SHA1_ARM_NEON is not set" >>$(1); \
		echo "# CONFIG_CRYPTO_SHA1_ARM_CE is not set" >>$(1); \
		echo "# CONFIG_CRYPTO_SHA2_ARM_CE is not set" >>$(1); \
		echo "# CONFIG_CRYPTO_AES_ARM_BS is not set" >>$(1); \
		echo "# CONFIG_CRYPTO_AES_ARM_CE is not set" >>$(1); \
		echo "# CONFIG_CRYPTO_GHASH_ARM_CE is not set" >>$(1); \
	fi;
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)
