export LINUXDIR := $(SRCBASE)/linux/linux-4.4

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -marm -march=armv7-a -msoft-float -mfloat-abi=soft -mtune=cortex-a7 -DMUSL_LIBC
endif

export DAKOTA_DTS :=ipq4019
export PLATFORM_ROUTER := ipq40xx
export BUILD := $(shell (gcc -dumpmachine))
export KERNEL_BINARY=$(LINUXDIR)/vmlinux
export MUSL32 :=y
export PLATFORM := arm-musl
export TOOLS := /opt/openwrt-gcc520_musl.arm
export CROSS_COMPILE := $(TOOLS)/bin/arm-openwrt-linux-muslgnueabi-
export READELF := $(TOOLS)/bin/arm-openwrt-linux-muslgnueabi-readelf
export CROSS_COMPILER := $(CROSS_COMPILE)
export CONFIGURE := ./configure --host=arm-linux --build=$(BUILD)
export HOSTCONFIG := linux-armv4
export ARCH := arm
export HOST := arm-linux
export KERNELCC := $(CROSS_COMPILE)gcc
export KERNELLD := $(CROSS_COMPILE)ld
LIBRT=$(wildcard $(TOOLS)/lib/librt-*.so)
ifneq ($(LIBRT),)
export RTVER := $(patsubst librt-%.so,%,$(shell basename $(wildcard $(TOOLS)/lib/librt-*.so)))
endif

# Kernel load address and entry address
export LOADADDR := 80208000
export ENTRYADDR := $(LOADADDR)

# OpenWRT's toolchain needs STAGING_DIR environment variable that points to top directory of toolchain.
export STAGING_DIR=$(TOOLS)

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_QCA -pipe -DDEBUG_NOISY -DDEBUG_RCTEST -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -march=armv7-a -D_GNU_SOURCE -D_BSD_SOURCE -DMUSL_LIBC -D__BIT_TYPES_DEFINED__

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
		echo "# RTCONFIG_SOC_IPQ60XX is not set" >>$(1); \
		sed -i "/RTCONFIG_GLOBAL_INI\>/d" $(1); \
		echo "# RTCONFIG_GLOBAL_INI is not set" >>$(1); \
		sed -i "/RTCONFIG_WIFI_QCN5024_QCN5054/d" $(1); \
		echo "# RTCONFIG_WIFI_QCN5024_QCN5054 is not set" >>$(1); \
		sed -i "/RTCONFIG_SPF11_QSDK/d" $(1); \
		echo "RTCONFIG_SPF11_QSDK=y" >>$(1); \
		sed -i "/RTCONFIG_SINGLE_HOSTAPD\>/d" $(1); \
		echo "RTCONFIG_SINGLE_HOSTAPD=y" >>$(1); \
		sed -i "/RTCONFIG_QCA_ARM/d" $(1); \
		echo "RTCONFIG_QCA_ARM=y" >>$(1); \
		sed -i "/RTCONFIG_32BYTES_ODMPID/d" $(1); \
		echo "RTCONFIG_32BYTES_ODMPID=y" >>$(1); \
		sed -i "/RTCONFIG_QCA_BIGRATE_WIFI\>/d" $(1); \
		echo "RTCONFIG_QCA_BIGRATE_WIFI=y" >>$(1); \
		if [ "$(IPQ40XX)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_IPQ40XX/d" $(1); \
			echo "RTCONFIG_SOC_IPQ40XX=y" >>$(1); \
		fi; \
		sed -i "/RTCONFIG_FITFDT/d" $(1); \
		echo "RTCONFIG_FITFDT=y" >>$(1); \
		if [ "$(QCA_VAP_LOCALMAC)" = "y" ]; then \
			sed -i "/RTCONFIG_QCA_VAP_LOCALMAC/d" $(1); \
			echo "RTCONFIG_QCA_VAP_LOCALMAC=y" >>$(1); \
		fi; \
		if [ "$(RTAC82U)" = "y" ] ; then \
			sed -i "/RTCONFIG_PCIE_QCA9984/d" $(1); \
			echo "RTCONFIG_PCIE_QCA9984=y" >>$(1); \
		fi; \
		if [ "$(RTAC95U)" = "y" ] ; then \
			sed -i "/RTCONFIG_PCIE_QCA9984/d" $(1); \
			echo "RTCONFIG_PCIE_QCA9984=y" >>$(1); \
			sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
			echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
		fi; \
		if [ "$(MAPAC2200)" = "y" ] ; then \
			sed -i "/RTCONFIG_HAS_5G_2/d" $(1); \
			echo "RTCONFIG_HAS_5G_2=y" >>$(1); \
			sed -i "/RTCONFIG_PCIE_QCA9888/d" $(1); \
			echo "RTCONFIG_PCIE_QCA9888=y" >>$(1); \
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
		if [ "$(RTCONFIG_EASYMESH)" = "y" ]; then \
			sed -i "/CONFIG_STAT\>/d" $(1); \
			echo "CONFIG_STAT=y" >>$(1); \
			echo "CONFIG_FEATURE_STAT_FORMAT=y" >>$(1); \
		fi; \
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
		if [ "$(RTAC82U)" = "y" ]; then \
			sed -i "/CONFIG_RTAC82U/d" $(1); \
			echo "CONFIG_RTAC82U=y" >>$(1); \
			sed -i "/CONFIG_NF_CONNTRACK_CHAIN_EVENTS/d" $(1); \
			echo "CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y" >>$(1); \
		fi; \
		if [ "$(MAPAC1300)" = "y" ]; then \
			sed -i "/CONFIG_MAPAC1300/d" $(1); \
			echo "CONFIG_MAPAC1300=y" >>$(1); \
		fi; \
		if [ "$(MAPAC2200)" = "y" ]; then \
			sed -i "/CONFIG_MAPAC2200\b/d" $(1); \
			echo "CONFIG_MAPAC2200=y" >>$(1); \
		fi; \
		if [ "$(VZWAC1300)" = "y" ]; then \
			sed -i "/CONFIG_VZWAC1300/d" $(1); \
			echo "CONFIG_VZWAC1300=y" >>$(1); \
		fi; \
		if [ "$(RTAC95U)" = "y" ]; then \
			sed -i "/CONFIG_RTAC95U/d" $(1); \
			echo "CONFIG_RTAC95U=y" >>$(1); \
		fi; \
		if [ "$(MAPAC1300)" = "y" ] || [ "$(MAPAC2200)" = "y" ] || [ "$(VZWAC1300)" = "y" ] || [ "$(RTAC95U)" = "y" ]; then \
			sed -i "/CONFIG_NF_CONNTRACK_CHAIN_EVENTS/d" $(1); \
			echo "CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y" >>$(1); \
		fi; \
		if [ "$(LP5523)" = "y" ] ; then \
			sed -i "/CONFIG_LEDS_LP55XX_COMMON/d" $(1); \
			echo "CONFIG_LEDS_LP55XX_COMMON=y" >>$(1); \
			sed -i "/CONFIG_LEDS_LP5523/d" $(1); \
			echo "CONFIG_LEDS_LP5523=y" >>$(1); \
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
			sed -i "/CONFIG_BT_HCIBTUSB/d" $(1); \
			if [ "$(RTAC95U)" = "y" ]; then \
				echo "# CONFIG_BT_HCIBTUSB is not set" >>$(1); \
			elif [ "$(BT_CONN)" = "y" ] || [ "$(BT_CONN)" = "USB" ] ; then \
				sed -i "/CONFIG_BT_HCIBTUSB/d" $(1); \
				echo "CONFIG_BT_HCIBTUSB=m" >>$(1); \
				sed -i "/CONFIG_BT_ATH3K/d" $(1); \
				echo "CONFIG_BT_ATH3K=m" >>$(1); \
			else \
				echo "CONFIG_BT_HCIBTUSB=m" >>$(1); \
			fi; \
			echo "# CONFIG_BT_HCIBTSDIO is not set" >>$(1); \
			sed -i "/CONFIG_BT_BNEP_PROTO_FILTER/d" $(1); \
			echo "CONFIG_BT_BNEP_PROTO_FILTER=y" >>$(1); \
			echo "# CONFIG_BT_HIDP is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIUART/d" $(1); \
			if [ "$(RTAC95U)" = "y" ]; then \
				echo "CONFIG_BT_HCIUART=y" >>$(1); \
			else \
				echo "# CONFIG_BT_HCIUART is not set" >>$(1); \
			fi; \
			echo "# CONFIG_BT_HCIUART_H4 is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIUART_BCSP/d" $(1); \
			echo "# CONFIG_BT_HCIUART_BCSP is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIUART_ATH3K/d" $(1); \
			echo "# CONFIG_BT_HCIUART_ATH3K is not set" >>$(1); \
			sed -i "/CONFIG_BT_ATH3K/d" $(1); \
			if [ "$(RTAC95U)" = "y" ]; then \
				echo "# CONFIG_BT_ATH3K is not set" >>$(1); \
			elif [ "$(BT_CONN)" = "UART" ] ; then \
				sed -i "/CONFIG_BT_HCIUART/d" $(1); \
				echo "CONFIG_BT_HCIUART=y" >>$(1); \
				sed -i "/CONFIG_BT_HCIUART_BCSP/d" $(1); \
				echo "CONFIG_BT_HCIUART_BCSP=y" >>$(1); \
				sed -i "/CONFIG_BT_HCIUART_ATH3K/d" $(1); \
				echo "CONFIG_BT_HCIUART_ATH3K=y" >>$(1); \
			else \
				echo "CONFIG_BT_ATH3K=m" >>$(1); \
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
			echo "# CONFIG_BT_WILINK is not set" >>$(1); \
			echo "# CONFIG_BTRFS_FS is not set" >>$(1); \
		fi; \
		if [ "$(IPQ40XX)" = "y" ]; then \
			if [ "$(HFS)" = "open" ]; then \
				sed -i "/HFSPLUS_FS_POSIX_ACL/d" $(1); \
				echo "# CONFIG_HFSPLUS_FS_POSIX_ACL is not set" >>$(1); \
			fi; \
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
			echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_ADDR=0x83ff0000" >>$(1); \
			echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_LEN=0x3000" >>$(1); \
		else \
			sed -i "/CONFIG_DUMP_PREV_OOPS_MSG/d" $(1); \
			echo "# CONFIG_DUMP_PREV_OOPS_MSG is not set" >>$(1); \
		fi; \
		if [ "$(IPV6SUPP)" = "y" ]; then \
			sed -i "/CONFIG_IPV6_MULTIPLE_TABLES is not set/d" $(1); \
			echo "# CONFIG_IPV6_MULTIPLE_TABLES is not set" >>$(1); \
		fi; \
	fi; \
	if [ "$(DUALWAN)" = "y" ]; then \
		sed -i "/CONFIG_ESSEDMA_DUALWAN/d" $(1); \
		echo "CONFIG_ESSEDMA_DUALWAN=y" >>$(1); \
	fi; \
	)
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)

