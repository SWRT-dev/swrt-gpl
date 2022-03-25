# Select toolchain for user-space program.
# If model name is not list here, gcc 4.6.3 + uClibc 0.9.33 is selected.
GCC520_UCLIBC_MODEL_LIST :=

# Select toolchain for kernel-space driver.
# If model name is not list here, toolchain for user-space program is selected.
# ########
# reference : KERNEL_GCC520_UCLIBC_MODEL_LIST := #$(addprefix _,$(addsuffix _,GT-AXY16000))
KERNEL_GCC520_UCLIBC_MODEL_LIST		:=
KERNEL_GCC520_MUSLLIBC32_MODEL_LIST	:=
KERNEL_GCC520_MUSLLIBC64_MODEL_LIST	:=

BUILD_NAME ?= $(shell echo $(MAKECMDGOALS) | tr a-z A-Z)
export _BUILD_NAME_ := $(addprefix _,$(addsuffix _,$(BUILD_NAME)))

ifeq ($(MUSL32),y)
GCC520_MUSLLIBC32_MODEL_LIST		+= $(_BUILD_NAME_)
KERNEL_GCC520_MUSLLIBC32_MODEL_LIST	+= $(_BUILD_NAME_)
else ifeq ($(MUSL64),y)
GCC520_MUSLLIBC64_MODEL_LIST		+= __SWRT-360V6 __PL-AX56_XP4
KERNEL_GCC520_MUSLLIBC64_MODEL_LIST	+= __SWRT-360V6 __PL-AX56_XP4
endif

# related path to platform specific software package
export PLATFORM_ROUTER := ipq60xx
export LINUXDIR := $(SRCBASE)/linux/linux-4.4.x

export BUILD := $(shell (gcc -dumpmachine))
export KERNEL_BINARY=$(LINUXDIR)/vmlinux

ifneq ($(findstring $(_BUILD_NAME_),$(GCC520_UCLIBC_MODEL_LIST)),)
export TOOLS := /opt/openwrt-gcc463.arm
export PLATFORM := arm-uclibc
export CROSS_COMPILE := $(TOOLS)/bin/arm-openwrt-linux-uclibcgnueabi-
export READELF := $(TOOLS)/bin/arm-openwrt-linux-uclibcgnueabi-readelf
else ifneq ($(findstring $(_BUILD_NAME_),$(GCC520_MUSLLIBC32_MODEL_LIST)),)
export TOOLS := /opt/openwrt-gcc520_musl.arm
export PLATFORM := arm-musl
export CROSS_COMPILE := $(TOOLS)/bin/arm-openwrt-linux-muslgnueabi-
export READELF := $(TOOLS)/bin/arm-openwrt-linux-muslgnueabi-readelf
else ifneq ($(findstring $(_BUILD_NAME_),$(GCC520_MUSLLIBC64_MODEL_LIST)),)
export TOOLS := /opt/openwrt-gcc520_musl.aarch64
export PLATFORM := arm-musl
export CROSS_COMPILE := $(TOOLS)/bin/aarch64-openwrt-linux-musl-
export READELF := $(TOOLS)/bin/aarch64-openwrt-linux-musl-readelf
else
export TOOLS := /opt/openwrt-gcc463.arm
export PLATFORM := arm-uclibc
export CROSS_COMPILE := $(TOOLS)/bin/arm-openwrt-linux-uclibcgnueabi-
export READELF := $(TOOLS)/bin/arm-openwrt-linux-uclibcgnueabi-readelf
endif

ifneq ($(findstring $(_BUILD_NAME_),$(GCC520_MUSLLIBC64_MODEL_LIST)),)
LIBRT=$(wildcard $(TOOLS)/lib/librt-*.so)
ifneq ($(LIBRT),)
export RTVER := $(patsubst librt-%.so,%,$(shell basename $(wildcard $(TOOLS)/lib/librt-*.so)))
endif
export CROSS_COMPILER := $(CROSS_COMPILE)
export CONFIGURE := ./configure --host=aarch64-linux-gnu --build=$(BUILD)
export HOSTCONFIG := linux-aarch64
# ARCH is used for linux kernel and some other else
export ARCH := arm64
export HOST := aarch64-linux

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -march=armv8-a --target=aarch64-arm-none-eabi -mcpu=cortex-a53
endif

else
LIBRT=$(wildcard $(TOOLS)/lib/librt-*.so)
ifneq ($(LIBRT),)
export RTVER := $(patsubst librt-%.so,%,$(shell basename $(wildcard $(TOOLS)/lib/librt-*.so)))
endif
export CROSS_COMPILER := $(CROSS_COMPILE)
export CONFIGURE := ./configure --host=arm-linux --build=$(BUILD)
export HOSTCONFIG := linux-armv4
export ARCH := $(firstword $(subst -, ,$(shell $(CROSS_COMPILE)gcc -dumpmachine)))
export HOST := arm-linux
endif

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -marm -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp
endif

ifneq ($(findstring $(_BUILD_NAME_),$(KERNEL_GCC520_UCLIBC_MODEL_LIST)),)
KERNELCC := /opt/openwrt-gcc520.arm/bin/arm-openwrt-linux-uclibcgnueabi-gcc
KERNELLD := /opt/openwrt-gcc520.arm/bin/arm-openwrt-linux-uclibcgnueabi-ld
else ifneq ($(findstring $(_BUILD_NAME_),$(KERNEL_GCC520_MUSLLIBC32_MODEL_LIST)),)
KERNELCC := /opt/openwrt-gcc520_musl.arm/bin/arm-openwrt-linux-muslgnueabi-gcc
KERNELLD := /opt/openwrt-gcc520_musl.arm/bin/arm-openwrt-linux-muslgnueabi-ld
else ifneq ($(findstring $(_BUILD_NAME_),$(KERNEL_GCC520_MUSLLIBC64_MODEL_LIST)),)
KERNELCC := /opt/openwrt-gcc520_musl.aarch64/bin/aarch64-openwrt-linux-musl-gcc
KERNELLD := /opt/openwrt-gcc520_musl.aarch64/bin/aarch64-openwrt-linux-musl-ld
endif

export KERNELCC ?= $(CROSS_COMPILE)gcc
export KERNELLD ?= $(CROSS_COMPILE)ld
export KARCH := $(firstword $(subst -, ,$(shell $(KERNELCC) -dumpmachine)))

# $(patsubst %-gcc,%-,$(KERNELCC))

# Kernel load address and entry address
export LOADADDR := 41208000
export ENTRYADDR := $(LOADADDR)

# OpenWRT's toolchain needs STAGING_DIR environment variable that points to top directory of toolchain.
export STAGING_DIR=$(TOOLS)

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_QCA -pipe -DDEBUG_NOISY -DDEBUG_RCTEST -mfpu=vfpv3-d16 -mfloat-abi=softfp

ifneq ($(findstring $(_BUILD_NAME_),$(GCC520_MUSLLIBC32_MODEL_LIST)),)
	EXTRA_CFLAGS+=-march=armv7-a -D_GNU_SOURCE -D_BSD_SOURCE
else ifneq ($(findstring $(_BUILD_NAME_),$(GCC520_MUSLLIBC64_MODEL_LIST)),)
	EXTRA_CFLAGS+=-D_GNU_SOURCE -D_BSD_SOURCE
else
	EXTRA_CFLAGS+=-march=armv7-a
endif

export CONFIG_LINUX26=y
export CONFIG_QCA=y
export TOOLCHAIN_TARGET_GCCVER := 5.2.0

EXTRA_CFLAGS += -DLINUX30
export CONFIG_LINUX30=y

define platformRouterOptions
	@( \
	if [ "$(QCA)" = "y" ]; then \
		sed -i "/RTCONFIG_QCA\>/d" $(1); \
		echo "RTCONFIG_QCA=y" >>$(1); \
		sed -i "/RTCONFIG_SOC_IPQ60XX/d" $(1); \
		echo "RTCONFIG_SOC_IPQ60XX=y" >>$(1); \
		sed -i "/RTCONFIG_GLOBAL_INI\>/d" $(1); \
		echo "RTCONFIG_GLOBAL_INI=y" >>$(1); \
		sed -i "/RTCONFIG_WIFI_QCN5024_QCN5054/d" $(1); \
		echo "RTCONFIG_WIFI_QCN5024_QCN5054=y" >>$(1); \
		sed -i "/RTCONFIG_SPF11_QSDK/d" $(1); \
		echo "RTCONFIG_SPF11_QSDK=y" >>$(1); \
		sed -i "/RTCONFIG_SINGLE_HOSTAPD\>/d" $(1); \
		echo "RTCONFIG_SINGLE_HOSTAPD=y" >>$(1); \
		sed -i "/RTCONFIG_QCA_ARM/d" $(1); \
		echo "RTCONFIG_QCA_ARM=y" >>$(1); \
		sed -i "/RTCONFIG_32BYTES_ODMPID/d" $(1); \
		echo "RTCONFIG_32BYTES_ODMPID=y" >>$(1); \
		sed -i "/RTCONFIG_FITFDT/d" $(1); \
		echo "RTCONFIG_FITFDT=y" >>$(1); \
		if [ "$(QCA_VAP_LOCALMAC)" = "y" ]; then \
			sed -i "/RTCONFIG_QCA_VAP_LOCALMAC/d" $(1); \
			echo "RTCONFIG_QCA_VAP_LOCALMAC=y" >>$(1); \
		fi; \
		sed -i "/RTCONFIG_VHT80_80/d" $(1); \
		echo "# RTCONFIG_VHT80_80 is not set" >>$(1); \
		sed -i "/RTCONFIG_QCA_BIGRATE_WIFI\>/d" $(1); \
		echo "RTCONFIG_QCA_BIGRATE_WIFI=y" >>$(1); \
		sed -i "/RTCONFIG_VHT160/d" $(1); \
		if [ "$(BW160M)" = "y" ]; then \
			echo "RTCONFIG_VHT160=y" >>$(1); \
			echo "RTCONFIG_BW160M=y" >>$(1); \
		else \
			echo "# RTCONFIG_VHT160 is not set" >>$(1); \
			echo "# RTCONFIG_BW160M is not set" >>$(1); \
		fi; \
		if [ "$(NSS_IPSEC)" = "y" ]; then \
			sed -i "/RTCONFIG_NSS_IPSEC/d" $(1); \
			echo "RTCONFIG_NSS_IPSEC=y" >>$(1); \
		fi; \
		if [ -n "$(wildcard $(TOOLS)/lib/ld-musl*)" ] ; then \
			sed -i "/RTCONFIG_MUSL_LIBC/d" $(1); \
			echo "RTCONFIG_MUSL_LIBC=y" >>$(1); \
		fi; \
		if [ "$(BUILD_NAME)" = "PL-AX56_XP4" ]; then \
			sed -i "/RTCONFIG_FIXED_BRIGHTNESS_RGBLED\>/d" $(1); \
			echo "RTCONFIG_FIXED_BRIGHTNESS_RGBLED=y" >>$(1); \
		fi; \
	fi; \
	)
endef

define platformBusyboxOptions
	@( \
	if [ "$(QCA)" = "y" ]; then \
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
	@( \
	if [ "$(QCA)" = "y" ]; then \
		sed -i "/CONFIG_$(subst RT4G,4G,$(MODEL))/d" $(1); \
		echo "CONFIG_$(subst RT4G,4G,$(MODEL))=y" >>$(1); \
		sed -i "/CONFIG_BRIDGE_NETFILTER/d" $(1); \
		echo "CONFIG_BRIDGE_NETFILTER=y" >>$(1); \
		sed -i "/CONFIG_NETFILTER_XT_TARGET_TPROXY/d" $(1); \
		echo "CONFIG_NETFILTER_XT_TARGET_TPROXY=m" >>$(1); \
		sed -i "/CONFIG_NF_CONNTRACK_CHAIN_EVENTS/d" $(1); \
		echo "CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y" >>$(1); \
		sed -i "/CONFIG_NETFILTER_XT_MATCH_PHYSDEV/d" $(1); \
		echo "CONFIG_NETFILTER_XT_MATCH_PHYSDEV=y" >>$(1); \
		if [ "$(CONFIG_LINUX30)" = "y" ]; then \
			if [ "$(BOOT_FLASH_TYPE)" = "SPI" ] ; then \
				sed -i "/CONFIG_MTD_MSM_NAND\>/d" $(1); \
				echo "# CONFIG_MTD_MSM_NAND is not set" >> $(1); \
				sed -i "/CONFIG_MTD_M25P80\>/d" $(1); \
				echo "CONFIG_MTD_M25P80=y" >> $(1); \
				sed -i "/CONFIG_SPI_QUP\>/d" $(1); \
				echo "CONFIG_SPI_QUP=y" >> $(1); \
			else \
				sed -i "/CONFIG_MTD_MSM_NAND\>/d" $(1); \
				echo "CONFIG_MTD_MSM_NAND=y" >> $(1); \
				sed -i "/CONFIG_MTD_M25P80\>/d" $(1); \
				echo "# CONFIG_MTD_M25P80 is not set" >> $(1); \
			fi; \
			sed -i "/CONFIG_BRIDGE_EBT_ARPNAT/d" $(1); \
			echo "# CONFIG_BRIDGE_EBT_ARPNAT is not set" >>$(1); \
			sed -i "/CONFIG_NF_CONNTRACK_EVENTS/d" $(1); \
			echo "CONFIG_NF_CONNTRACK_EVENTS=y" >>$(1); \
		fi; \
		if [ "$(BT_CONN)" != "" ] ; then \
			sed -i "/CONFIG_BT/d" $(1); \
			echo "CONFIG_BT=y" >>$(1); \
			sed -i "/CONFIG_BT_BREDR/d" $(1); \
			echo "CONFIG_BT_BREDR=y" >>$(1); \
			sed -i "/CONFIG_BT_RFCOMM/d" $(1); \
			echo "CONFIG_BT_RFCOMM=y" >>$(1); \
			sed -i "/CONFIG_BT_RFCOMM_TTY/d" $(1); \
			echo "CONFIG_BT_RFCOMM_TTY=y" >>$(1); \
			sed -i "/CONFIG_BT_BNEP/d" $(1); \
			echo "CONFIG_BT_BNEP=y" >>$(1); \
			sed -i "/CONFIG_BT_BNEP_MC_FILTER/d" $(1); \
			echo "CONFIG_BT_BNEP_MC_FILTER=y" >>$(1); \
			sed -i "/CONFIG_BT_HS/d" $(1); \
			echo "CONFIG_BT_HS=y" >>$(1); \
			sed -i "/CONFIG_BT_LE/d" $(1); \
			echo "CONFIG_BT_LE=y" >>$(1); \
			echo "# CONFIG_BT_SELFTEST is not set" >>$(1); \
			sed -i "/CONFIG_BT_DEBUGFS/d" $(1); \
			echo "CONFIG_BT_DEBUGFS=y" >>$(1); \
			echo "# CONFIG_BT_HCIBTUSB is not set" >>$(1); \
			if [ "$(BT_CONN)" = "y" ] || [ "$(BT_CONN)" = "USB" ] ; then \
				sed -i "/CONFIG_BT_HCIBTUSB/d" $(1); \
				echo "CONFIG_BT_HCIBTUSB=m" >>$(1); \
				sed -i "/CONFIG_BT_ATH3K/d" $(1); \
				echo "CONFIG_BT_ATH3K=m" >>$(1); \
			fi; \
			echo "# CONFIG_BT_HCIBTSDIO is not set" >>$(1); \
			sed -i "/CONFIG_BT_BNEP_PROTO_FILTER/d" $(1); \
			echo "CONFIG_BT_BNEP_PROTO_FILTER=y" >>$(1); \
			echo "# CONFIG_BT_HCIUART is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_H4 is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_BCSP is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_ATH3K is not set" >>$(1); \
			if [ "$(BT_CONN)" = "UART" ] ; then \
				sed -i "/CONFIG_BT_HCIUART/d" $(1); \
				echo "CONFIG_BT_HCIUART=y" >>$(1); \
				sed -i "/CONFIG_BT_HCIUART_BCSP/d" $(1); \
				echo "CONFIG_BT_HCIUART_BCSP=y" >>$(1); \
				sed -i "/CONFIG_BT_HCIUART_ATH3K/d" $(1); \
				echo "CONFIG_BT_HCIUART_ATH3K=y" >>$(1); \
			fi; \
			echo "# CONFIG_BT_HCIUART_LL is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_3WIRE is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_INTEL is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_BCM is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_QCA is not set" >>$(1); \
			echo "# CONFIG_BT_HCIBCM203X is not set" >>$(1); \
			echo "# CONFIG_BT_HCIBPA10X is not set" >>$(1); \
			echo "# CONFIG_BT_HCIBFUSB is not set" >>$(1); \
			echo "# CONFIG_BT_HCIVHCI is not set" >>$(1); \
			echo "# CONFIG_BT_MRVL is not set" >>$(1); \
			echo "# CONFIG_BTRFS_FS is not set" >>$(1); \
		fi; \
		if [ "$(BUILD_NAME)" = "PL-AX56_XP4" ]; then \
			sed -i "/CONFIG_IPQ_MEM_PROFILE/d" $(1); \
			echo "CONFIG_IPQ_MEM_PROFILE=512" >>$(1); \
		fi; \
	fi; \
	if [ "$(JFFS2)" = "y" ]; then \
		if [ "$(CONFIG_LINUX30)" = "y" ]; then \
			sed -i "/CONFIG_JFFS2_FS_WBUF_VERIFY/d" $(1); \
			echo "# CONFIG_JFFS2_FS_WBUF_VERIFY is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_CMODE_FAVOURLZO/d" $(1); \
			echo "# CONFIG_JFFS2_CMODE_FAVOURLZO is not set" >>$(1); \
		fi; \
	else \
		sed -i "/CONFIG_JFFS2_FS/d" $(1); \
		echo "# CONFIG_JFFS2_FS is not set" >>$(1); \
	fi; \
	if [ "$(FTRACE)" = "y" ]; then \
		sed -i "/CONFIG_FTRACE\>/d" $(1); \
		echo "CONFIG_FTRACE=y" >>$(1); \
		sed -i "/CONFIG_FUNCTION_TRACER\>/d" $(1); \
		echo "CONFIG_FUNCTION_TRACER=y" >>$(1); \
		sed -i "/CONFIG_FUNCTION_GRAPH_TRACER\>/d" $(1); \
		echo "CONFIG_FUNCTION_GRAPH_TRACER=y" >>$(1); \
		sed -i "/CONFIG_SCHED_TRACER\>/d" $(1); \
		echo "CONFIG_SCHED_TRACER=y" >>$(1); \
		sed -i "/CONFIG_FTRACE_SYSCALLS\>/d" $(1); \
		echo "CONFIG_FTRACE_SYSCALLS=y" >>$(1); \
		sed -i "/CONFIG_BRANCH_PROFILE_NONE\>/d" $(1); \
		echo "CONFIG_BRANCH_PROFILE_NONE=y" >>$(1); \
		sed -i "/CONFIG_DYNAMIC_FTRACE\>/d" $(1); \
		echo "CONFIG_DYNAMIC_FTRACE=y" >>$(1); \
		sed -i "/CONFIG_FUNCTION_PROFILER\>/d" $(1); \
		echo "CONFIG_FUNCTION_PROFILER=y" >>$(1); \
		sed -i "/CONFIG_TRACING_EVENTS_GPIO\>/d" $(1); \
		echo "CONFIG_TRACING_EVENTS_GPIO=y" >>$(1); \
	fi; \
	if [ "$(UBI)" = "y" ]; then \
		sed -i "/CONFIG_MTD_UBI\>/d" $(1); \
		echo "CONFIG_MTD_UBI=y" >>$(1); \
		sed -i "/CONFIG_MTD_UBI_WL_THRESHOLD/d" $(1); \
		echo "CONFIG_MTD_UBI_WL_THRESHOLD=4096" >>$(1); \
		sed -i "/CONFIG_MTD_UBI_BEB_RESERVE/d" $(1); \
		echo "CONFIG_MTD_UBI_BEB_RESERVE=1" >>$(1); \
		sed -i "/CONFIG_MTD_UBI_GLUEBI/d" $(1); \
		echo "CONFIG_MTD_UBI_GLUEBI=y" >>$(1); \
		sed -i "/CONFIG_FACTORY_CHECKSUM/d" $(1); \
		echo "CONFIG_FACTORY_CHECKSUM=y" >>$(1); \
		if [ "$(UBI_DEBUG)" = "y" ]; then \
			sed -i "/CONFIG_MTD_UBI_DEBUG/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG=y" >>$(1); \
			sed -i "/CONFIG_GCOV_KERNEL/d" $(1); \
			echo "# CONFIG_GCOV_KERNEL is not set" >>$(1); \
			sed -i "/CONFIG_L2TP_DEBUGFS/d" $(1); \
			echo "# CONFIG_L2TP_DEBUGFS is not set" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_MSG/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_MSG=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_PARANOID/d" $(1); \
			echo "# CONFIG_MTD_UBI_DEBUG_PARANOID is not set" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_DISABLE_BGT/d" $(1); \
			echo "# CONFIG_MTD_UBI_DEBUG_DISABLE_BGT is not set" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_EMULATE_BITFLIPS/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_EMULATE_BITFLIPS=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_EMULATE_WRITE_FAILURES/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_EMULATE_WRITE_FAILURES=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_EMULATE_ERASE_FAILURES/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_EMULATE_ERASE_FAILURES=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_MSG_BLD/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_MSG_BLD=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_MSG_EBA/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_MSG_EBA=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_MSG_WL/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_MSG_WL=y" >>$(1); \
			sed -i "/CONFIG_MTD_UBI_DEBUG_MSG_IO/d" $(1); \
			echo "CONFIG_MTD_UBI_DEBUG_MSG_IO=y" >>$(1); \
			sed -i "/CONFIG_JBD_DEBUG/d" $(1); \
			echo "# CONFIG_JBD_DEBUG is not set" >>$(1); \
			sed -i "/CONFIG_LKDTM/d" $(1); \
			echo "# CONFIG_LKDTM is not set" >>$(1); \
			sed -i "/CONFIG_DYNAMIC_DEBUG/d" $(1); \
			echo "CONFIG_DYNAMIC_DEBUG=y" >>$(1); \
			sed -i "/CONFIG_SPINLOCK_TEST/d" $(1); \
			echo "# CONFIG_SPINLOCK_TEST is not set" >>$(1); \
		else \
			sed -i "/CONFIG_MTD_UBI_DEBUG/d" $(1); \
			echo "# CONFIG_MTD_UBI_DEBUG is not set" >>$(1); \
		fi; \
		if [ "$(UBIFS)" = "y" ]; then \
			sed -i "/CONFIG_UBIFS_FS/d" $(1); \
			echo "CONFIG_UBIFS_FS=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_XATTR/d" $(1); \
			echo "# CONFIG_UBIFS_FS_XATTR is not set" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ADVANCED_COMPR/d" $(1); \
			echo "CONFIG_UBIFS_FS_ADVANCED_COMPR=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_LZO/d" $(1); \
			echo "CONFIG_UBIFS_FS_LZO=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ZLIB/d" $(1); \
			echo "CONFIG_UBIFS_FS_ZLIB=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_XZ/d" $(1); \
			echo "CONFIG_UBIFS_FS_XZ=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_DEBUG/d" $(1); \
			echo "# CONFIG_UBIFS_FS_DEBUG is not set" >>$(1); \
		else \
			sed -i "/CONFIG_UBIFS_FS/d" $(1); \
			echo "# CONFIG_UBIFS_FS is not set" >>$(1); \
		fi; \
		if [ "$(DUMP_OOPS_MSG)" = "y" ]; then \
			echo "CONFIG_DUMP_PREV_OOPS_MSG=y" >>$(1); \
			echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_ADDR=0x45300000" >>$(1); \
			echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_LEN=0x2000" >>$(1); \
		else \
			sed -i "/CONFIG_DUMP_PREV_OOPS_MSG/d" $(1); \
			echo "# CONFIG_DUMP_PREV_OOPS_MSG is not set" >>$(1); \
		fi; \
		if [ "$(IPV6SUPP)" = "y" ]; then \
			sed -i "/CONFIG_IPV6_MULTIPLE_TABLES is not set/d" $(1); \
			echo "# CONFIG_IPV6_MULTIPLE_TABLES is not set" >>$(1); \
		fi; \
	fi; \
	if [ "$(BUILD_NAME)" = "PL-AX56_XP4" ]; then \
		sed -i "/CONFIG_PLAX56XP4/d" $(1); \
		if [ "$(SWRT_NAME)" = "SWRT360V6" ]; then \
			echo "# CONFIG_PLAX56XP4 is not set" >>$(1); \
			echo "CONFIG_QIHOO360V6=y" >>$(1); \
			echo "# CONFIG_GLAX1800 is not set" >>$(1); \
		elif [ "$(SWRT_NAME)" = "GLAX1800" ]; then \
			echo "# CONFIG_PLAX56XP4 is not set" >>$(1); \
			echo "# CONFIG_QIHOO360V6 is not set" >>$(1); \
			echo "CONFIG_GLAX1800=y" >>$(1); \
		else \
			echo "CONFIG_PLAX56XP4=y" >>$(1); \
			echo "# CONFIG_QIHOO360V6 is not set" >>$(1); \
			echo "# CONFIG_GLAX1800 is not set" >>$(1); \
		fi; \
	fi; \
	)
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)

