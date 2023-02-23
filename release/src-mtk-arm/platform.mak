export LINUXDIR := $(SRCBASE)/linux/linux-5.4.x

ifeq ($(EXTRACFLAGS),)
ifeq ($(MT798X),y)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -march=armv8-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -mcpu=cortex-a53
else
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -mips32 -mtune=mips32
#export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -marm -march=armv7-a -msoft-float -mfloat-abi=soft -mtune=cortex-a7
#export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -marm -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp
endif
endif

export RTVER := 0.9.30.1
export ATEVER := 2.0.2
export OPENSSL11=y



export BUILD := $(shell (gcc -dumpmachine))
export KERNEL_BINARY=$(LINUXDIR)/vmlinux
ifeq ($(RT4GAC86U),y)
export EXTRACFLAGS += -DRT4GAC86U
export PLATFORM := arm-glibc
export PLATFORM_ROUTER := mt7622
export PLATFORM_ARCH := arm-glibc
export TOOLS := /opt/toolchain-aarch64_cortex-a53+neon-vfpv4_gcc-5.4.0_glibc-2.24
export CROSS_COMPILE := $(TOOLS)/bin/aarch64-openwrt-linux-gnu-
export CROSS_COMPILER := $(CROSS_COMPILE)
export READELF := $(TOOLS)/bin/aarch64-openwrt-linux-gnu-readelf
export KERNELCC := $(TOOLS)/bin/aarch64-openwrt-linux-gnu-gcc
export KERNELLD := $(TOOLS)/bin/aarch64-openwrt-linux-gnu-ld
export STAGING_DIR := $(TOOLS)
export ARCH := arm64
export HOST := arm-linux
export CONFIGURE := ./configure --host=arm-linux --build=$(BUILD)
export HOSTCONFIG := linux-aarch64
else ifeq ($(MT798X),y)
export MUSL64=y
export PLATFORM_ROUTER := mt798x
export PLATFORM := arm-musl
export PLATFORM_ARCH := arm-musl
export TOOLS := /opt/toolchain-aarch64_cortex-a53_gcc-8.4.0_musl
export CROSS_COMPILE := $(TOOLS)/bin/aarch64-openwrt-linux-
export CROSS_COMPILER := $(CROSS_COMPILE)
export READELF := $(TOOLS)/bin/aarch64-openwrt-linux-readelf
export KERNELCC := $(TOOLS)/bin/aarch64-openwrt-linux-gcc
export KERNELLD := $(TOOLS)/bin/aarch64-openwrt-linux-ld
export STAGING_DIR := $(TOOLS)
export ARCH := arm64
export HOST := aarch64-linux
export CONFIGURE := ./configure --host=aarch64-linux --build=$(BUILD)
export HOSTCONFIG := linux-aarch64
export ENTRYADDR := 48080000
export LOADADDR := $(ENTRYADDR)
else
export MUSL32=y
export PLATFORM_ROUTER := mt7629
export PLATFORM := arm-musl
export PLATFORM_ARCH := arm-musl
export TOOLS := /opt/lede-toolchain-mediatek-mt7629_gcc-5.4.0_musl-1.1.24_eabi.Linux-x86_64/toolchain-arm_cortex-a7_gcc-5.4.0_musl-1.1.24_eabi
export CROSS_COMPILE := $(TOOLS)/bin/arm-openwrt-linux-
export CROSS_COMPILER := $(CROSS_COMPILE)
export READELF := $(TOOLS)/bin/arm-openwrt-linux-readelf
export KERNELCC := /opt/lede-toolchain-mediatek-mt7629_gcc-5.4.0_musl-1.1.24_eabi.Linux-x86_64/toolchain-arm_cortex-a7_gcc-5.4.0_musl-1.1.24_eabi/bin/arm-openwrt-linux-gcc
export KERNELLD := /opt/lede-toolchain-mediatek-mt7629_gcc-5.4.0_musl-1.1.24_eabi.Linux-x86_64/toolchain-arm_cortex-a7_gcc-5.4.0_musl-1.1.24_eabi/bin/arm-openwrt-linux-ld
export STAGING_DIR := $(TOOLS)
export ARCH := arm
export HOST := arm-linux
export CONFIGURE := ./configure --host=arm-linux --build=$(BUILD)
export HOSTCONFIG := linux-armv4
endif

# for OpenWRT SDK
export DTS_DIR := $(LINUXDIR)/arch/$(ARCH)/boot/dts

#ifeq ($(RT4GAC86U),y)
EXTRA_CFLAGS := -DLINUX26 -DCONFIG_RALINK -pipe -DDEBUG_NOISY -DDEBUG_RCTEST
#else
#EXTRA_CFLAGS := -DLINUX26 -DCONFIG_RALINK -pipe -DDEBUG_NOISY -DDEBUG_RCTEST -mfpu=vfpv3-d16 -mfloat-abi=softfp
#endif
ifeq ($(RTACRH18),y)
EXTRA_CFLAGS += -march=armv7-a -D_GNU_SOURCE -DMUSL_LIBC -D_BSD_SOURCE -D__BIT_TYPES_DEFINED__
endif
ifeq ($(RT4GAC86U),y)
EXTRA_CFLAGS += -D_BSD_SOURCE -D__BIT_TYPES_DEFINED__
endif
ifeq ($(MT798X),y)
EXTRA_CFLAGS += -Os -mcpu=cortex-a53 -march=armv8 -mfpu=vfpv3-d16 -mfloat-abi=softfp -D_GNU_SOURCE -D_BSD_SOURCE -D__BIT_TYPES_DEFINED__ -DMUSL_LIBC
endif

export CONFIG_LINUX26=y
export CONFIG_RALINK=y

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
		if [ "$(RT3883)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_RT3883/d" $(1); \
			echo "RTCONFIG_RALINK_RT3883=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_RT3883/d" $(1); \
			echo "# RTCONFIG_RALINK_RT3883 is not set" >>$(1); \
		fi; \
		if [ "$(RT3052)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_RT3052/d" $(1); \
			echo "RTCONFIG_RALINK_RT3052=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_RT3052/d" $(1); \
			echo "# RTCONFIG_RALINK_RT3052 is not set" >>$(1); \
		fi; \
		if [ "$(MT7620)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_MT7620/d" $(1); \
			echo "RTCONFIG_RALINK_MT7620=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_MT7620/d" $(1); \
			echo "# RTCONFIG_RALINK_MT7620 is not set" >>$(1); \
		fi; \
		if [ "$(MT7621)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_MT7621/d" $(1); \
			echo "RTCONFIG_RALINK_MT7621=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_MT7621/d" $(1); \
			echo "# RTCONFIG_RALINK_MT7621 is not set" >>$(1); \
		fi; \
		if [ "$(MT7628)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_MT7628/d" $(1); \
			echo "RTCONFIG_RALINK_MT7628=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_MT7628/d" $(1); \
			echo "# RTCONFIG_RALINK_MT7628 is not set" >>$(1); \
		fi; \
		if [ "$(MT7629)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_MT7629/d" $(1); \
			echo "RTCONFIG_RALINK_MT7629=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_MT7629/d" $(1); \
			echo "# RTCONFIG_RALINK_MT7629 is not set" >>$(1); \
		fi; \
		if [ "$(MT7622)" = "y" ]; then \
			sed -i "/RTCONFIG_RALINK_MT7622/d" $(1); \
			echo "RTCONFIG_RALINK_MT7622=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_MT7622/d" $(1); \
			echo "# RTCONFIG_RALINK_MT7622 is not set" >>$(1); \
		fi; \
		if [ "$(MT798X)" = "y" ]; then \
			sed -i "/RTCONFIG_MT798X/d" $(1); \
			echo "RTCONFIG_MT798X=y" >>$(1); \
			sed -i "/RTCONFIG_SWITCH_MT7986_MT7531/d" $(1); \
			echo "RTCONFIG_SWITCH_MT7986_MT7531=y" >>$(1); \
			sed -i "/RTCONFIG_32BYTES_ODMPID/d" $(1); \
			echo "RTCONFIG_32BYTES_ODMPID=y" >>$(1); \
			sed -i "/RTCONFIG_FITFDT/d" $(1); \
			echo "RTCONFIG_FITFDT=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_RALINK_MT798X/d" $(1); \
			echo "# RTCONFIG_RALINK_MT798X is not set" >>$(1); \
		fi; \
		if [ "$(MT7986A)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_MT7986A/d" $(1); \
			echo "RTCONFIG_SOC_MT7986A=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_SOC_MT7986A/d" $(1); \
			echo "# RTCONFIG_SOC_MT7986A is not set" >>$(1); \
		fi; \
		if [ "$(MT7986B)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_MT7986B/d" $(1); \
			echo "RTCONFIG_SOC_MT7986B=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_SOC_MT7986B/d" $(1); \
			echo "# RTCONFIG_SOC_MT7986B is not set" >>$(1); \
		fi; \
		if [ "$(MT7986C)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_MT7986C/d" $(1); \
			echo "RTCONFIG_SOC_MT7986C=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_SOC_MT7986C/d" $(1); \
			echo "# RTCONFIG_SOC_MT7986C is not set" >>$(1); \
		fi; \
		if [ "$(MT7981)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_MT7981/d" $(1); \
			echo "RTCONFIG_SOC_MT7981=y" >>$(1); \
		else \
			sed -i "/RTCONFIG_SOC_MT7981/d" $(1); \
			echo "# RTCONFIG_SOC_MT7981 is not set" >>$(1); \
		fi; \
		if [ "$(BUILD_NAME)" = "RM-AX6000" ]; then \
			sed -i "/RTCONFIG_FIXED_BRIGHTNESS_RGBLED\>/d" $(1); \
			echo "RTCONFIG_FIXED_BRIGHTNESS_RGBLED=y" >>$(1); \
		fi; \
	fi; \
	)
endef

define platformBusyboxOptions
endef

FIRST_IF_POOL =		\
	"NONE"	\
	"RT2860"	\
	"RT3092"	\
	"RT5392"	\
	"RT5592"	\
	"RT3593"	\
	"MT7610E"	\
	"MT7612E"	\
	"MT7603E"	\
	"MT7602E"	\
	"MT7615E"

SECOND_IF_POOL = 	\
	"NONE"		\
	"RT3092"	\
	"RT5392"	\
	"RT5592"	\
	"RT3593"	\
	"RT3572"	\
	"RT5572"	\
	"MT7620"	\
	"MT7621"	\
	"MT7610U"	\
	"MT7610E"	\
	"RT8592"	\
	"MT7612U"	\
	"MT7612E"	\
	"MT7602E"	\
	"MT7603E"	\
	"MT7615E"


define platformKernelConfig
	@( \
	if [ "$(RALINK)" = "y" ]; then \
		if [ "$(RT3052)" = "y" ]; then \
			sed -i "/CONFIG_RALINK_RT3052_MP2/d" $(1); \
			echo "CONFIG_RALINK_RT3052_MP2=y" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3052/d" $(1); \
			echo "CONFIG_RALINK_RT3052=y" >>$(1); \
			sed -i "/CONFIG_MTD_PHYSMAP_START/d" $(1); \
			echo "CONFIG_MTD_PHYSMAP_START=0xBF000000" >>$(1); \
			sed -i "/CONFIG_RT3052_ASIC/d" $(1); \
			echo "CONFIG_RT3052_ASIC=y" >>$(1); \
			sed -i "/CONFIG_RT2880_DRAM_128M/d" $(1); \
			sed -i "/CONFIG_RT2880_DRAM_32M/d" $(1); \
			echo "CONFIG_RT2880_DRAM_32M=y" >>$(1); \
			sed -i "/CONFIG_RALINK_RAM_SIZE/d" $(1); \
			echo "CONFIG_RALINK_RAM_SIZE=32" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3883_MP/d" $(1); \
			echo "# CONFIG_RALINK_RT3883_MP is not set" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3883/d" $(1); \
			echo "# CONFIG_RALINK_RT3883 is not set" >>$(1); \
			sed -i "/CONFIG_RT3883_ASIC/d" $(1); \
			echo "# CONFIG_RT3883_ASIC is not set" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3883_MP/d" $(1); \
			echo "# CONFIG_RALINK_RT3883_MP is not set" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3883_MP/d" $(1); \
			echo "# CONFIG_RALINK_RT3883_MP is not set" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3662_2T2R/d" $(1); \
			echo "# CONFIG_RALINK_RT3662_2T2R is not set" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3052_2T2R/d" $(1); \
			echo "CONFIG_RALINK_RT3052_2T2R=y" >>$(1); \
			sed -i "/CONFIG_RALINK_RT3352/d" $(1); \
			echo "# CONFIG_RALINK_RT3352 is not set" >>$(1); \
			sed -i "/CONFIG_LAN_WAN_SUPPORT/d" $(1); \
			echo "CONFIG_LAN_WAN_SUPPORT=y" >>$(1); \
			sed -i "/CONFIG_RT_3052_ESW/d" $(1); \
			echo "CONFIG_RT_3052_ESW=y" >>$(1); \
		fi; \
		if [ "$(SPI_FAST_CLOCK)" = "y" ] ; then \
			sed -i "/CONFIG_MTD_SPI_FAST_CLOCK/d" $(1); \
			echo "CONFIG_MTD_SPI_FAST_CLOCK=y" >>$(1); \
		fi; \
		if [ "$(RTAC1200HP)" = "y" ] ; then \
			sed -i "/CONFIG_RAETH_HAS_PORT5/d" $(1); \
			echo "CONFIG_RAETH_HAS_PORT5=y" >>$(1); \
			sed -i "/CONFIG_P5_MAC_TO_PHY_MODE/d" $(1); \
			echo "CONFIG_P5_MAC_TO_PHY_MODE=y" >>$(1); \
			sed -i "/CONFIG_MAC_TO_GIGAPHY_MODE_ADDR/d" $(1); \
			echo "CONFIG_MAC_TO_GIGAPHY_MODE_ADDR=0x5" >>$(1); \
		fi; \
		if [ "$(EDCCA)" = "y" ] ; then \
			sed -i "/CONFIG_RT2860V2_AP_EDCCA_MONITOR/d" $(1); \
			echo "CONFIG_RT2860V2_AP_EDCCA_MONITOR=y" >>$(1); \
		fi; \
		if [ "$(MT7621)" = "y" ] ; then \
			sed -i "/CONFIG_RALINK_MT7620/d" $(1); \
			echo "# CONFIG_RALINK_MT7620 is not set" >>$(1); \
			sed -i "/CONFIG_MT7620_ASIC/d" $(1); \
			echo "# CONFIG_MT7620_ASIC is not set" >>$(1); \
			sed -i "/CONFIG_MT7620_BAUDRATE/d" $(1); \
			echo "# CONFIG_MT7620_BAUDRATE is not set" >>$(1); \
			sed -i "/CONFIG_RALINK_MT7621/d" $(1); \
			echo "CONFIG_RALINK_MT7621=y" >>$(1); \
			sed -i "/CONFIG_MT7621_ASIC/d" $(1); \
			echo "CONFIG_MT7621_ASIC=y" >>$(1); \
			sed -i "/CONFIG_MTK_MTD_NAND/d" $(1); \
			echo "# CONFIG_MTK_MTD_NAND is not set" >>$(1); \
			sed -i "/CONFIG_MIPS_MT_SMP/d" $(1); \
			echo "CONFIG_MIPS_MT_SMP=y" >>$(1); \
			sed -i "/CONFIG_MIPS_MT_SMTC/d" $(1); \
			echo "# CONFIG_MIPS_MT_SMTC is not set" >>$(1); \
			sed -i "/CONFIG_MIPS_VPE_LOADER/d" $(1); \
			echo "# CONFIG_MIPS_VPE_LOADER is not set" >>$(1); \
			sed -i "/CONFIG_SCHED_SMT/d" $(1); \
			echo "CONFIG_SCHED_SMT=y" >>$(1); \
			sed -i "/CONFIG_MIPS_MT_FPAFF/d" $(1); \
			echo "CONFIG_MIPS_MT_FPAFF=y" >>$(1); \
			sed -i "/CONFIG_MIPS_CMP/d" $(1); \
			echo "CONFIG_MIPS_CMP=y" >>$(1); \
			if [ "$(RT4GAX56)" != "y" ] ; then \
			sed -i "/CONFIG_HIGHMEM/d" $(1); \
			echo "# CONFIG_HIGHMEM is not set" >>$(1); \
			fi; \
			sed -i "/CONFIG_NR_CPUS/d" $(1); \
			echo "CONFIG_NR_CPUS=4" >>$(1); \
			sed -i "/CONFIG_PCIE_PORT0/d" $(1); \
			echo "CONFIG_PCIE_PORT0=y" >>$(1); \
			sed -i "/CONFIG_PCIE_PORT1/d" $(1); \
			echo "CONFIG_PCIE_PORT1=y" >>$(1); \
			sed -i "/CONFIG_RALINK_SPDIF/d" $(1); \
			echo "CONFIG_RALINK_SPDIF=m" >>$(1); \
			sed -i "/CONFIG_RCU_CPU_STALL_DETECTOR/d" $(1); \
			echo "CONFIG_RCU_CPU_STALL_DETECTOR=y" >>$(1); \
			sed -i "/CONFIG_CRYPTO_PCRYPT/d" $(1); \
			echo "# CONFIG_CRYPTO_PCRYPT is not set" >>$(1); \
			sed -i "/CONFIG_RAETH_HW_VLAN_RX/d" $(1); \
			echo "# CONFIG_RAETH_HW_VLAN_RX is not set" >>$(1); \
			sed -i "/CONFIG_RAETH_QDMA/d" $(1); \
			echo "CONFIG_RAETH_QDMA=y" >>$(1); \
			sed -i "/CONFIG_GE1_MII_FORCE_100/d" $(1); \
			echo "# CONFIG_GE1_MII_FORCE_100 is not set" >>$(1); \
			sed -i "/CONFIG_GE1_RGMII_FORCE_1000/d" $(1); \
			echo "CONFIG_GE1_RGMII_FORCE_1000=y" >>$(1); \
			sed -i "/CONFIG_RAETH_GMAC2/d" $(1); \
			echo "CONFIG_RAETH_GMAC2=y" >>$(1); \
			sed -i "/CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2/d" $(1); \
			echo "CONFIG_MAC_TO_GIGAPHY_MODE_ADDR2=4" >>$(1); \
			sed -i "/CONFIG_HW_IPSEC/d" $(1); \
			echo "# CONFIG_HW_IPSEC is not set" >>$(1); \
			sed -i "/CONFIG_GE1_MII_AN/d" $(1); \
			echo "# CONFIG_GE1_MII_AN is not set" >>$(1); \
			sed -i "/CONFIG_GE2_MII_AN/d" $(1); \
			echo "# CONFIG_GE2_MII_AN is not set" >>$(1); \
			sed -i "/CONFIG_GE1_RVMII_FORCE_100/d" $(1); \
			echo "# CONFIG_GE1_RVMII_FORCE_100 is not set" >>$(1); \
			sed -i "/CONFIG_GE2_RVMII_FORCE_100/d" $(1); \
			echo "# CONFIG_GE2_RVMII_FORCE_100 is not set" >>$(1); \
			sed -i "/CONFIG_GE1_RGMII_AN/d" $(1); \
			echo "# CONFIG_GE1_RGMII_AN is not set" >>$(1); \
			sed -i "/CONFIG_GE2_RGMII_AN/d" $(1); \
			echo "# CONFIG_GE2_RGMII_AN is not set" >>$(1); \
			sed -i "/CONFIG_GE1_RGMII_NONE/d" $(1); \
			echo "# CONFIG_GE1_RGMII_NONE is not set" >>$(1); \
			sed -i "/CONFIG_GE2_RGMII_FORCE_1000/d" $(1); \
			echo "# CONFIG_GE2_RGMII_FORCE_1000 is not set" >>$(1); \
			sed -i "/CONFIG_GE2_MII_FORCE_100/d" $(1); \
			echo "# CONFIG_GE2_MII_FORCE_100 is not set" >>$(1); \
			sed -i "/CONFIG_GE2_INTERNAL_GPHY/d" $(1); \
			echo "CONFIG_GE2_INTERNAL_GPHY=y" >>$(1); \
			sed -i "/CONFIG_RALINK_HWCRYPTO/d" $(1); \
			echo "# CONFIG_RALINK_HWCRYPTO is not set" >>$(1); \
			sed -i "/CONFIG_RA_HW_NAT_PREBIND/d" $(1); \
			echo "# CONFIG_RA_HW_NAT_PREBIND is not set" >>$(1); \
			sed -i "/CONFIG_PPE_MCAST/d" $(1); \
			echo "# CONFIG_PPE_MCAST is not set" >>$(1); \
			sed -i "/CONFIG_FIRST_IF_RT2860/d" $(1); \
			echo "# CONFIG_FIRST_IF_RT2860 is not set" >>$(1); \
			sed -i "/CONFIG_RT2860V2_AP/d" $(1); \
			echo "# CONFIG_RT2860V2_AP is not set" >>$(1); \
			sed -i "/CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION/d" $(1); \
			echo "# CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION is not set" >>$(1); \
			sed -i "/CONFIG_RT3XXX_EHCI/d" $(1); \
			echo "# CONFIG_RT3XXX_EHCI is not set" >>$(1); \
			sed -i "/CONFIG_USB_EHCI_ROOT_HUB_TT/d" $(1); \
			echo "# CONFIG_USB_EHCI_ROOT_HUB_TT is not set" >>$(1); \
			sed -i "/CONFIG_USB_EHCI_TT_NEWSCHED/d" $(1); \
			echo "# CONFIG_USB_EHCI_TT_NEWSCHED is not set" >>$(1); \
			sed -i "/CONFIG_RT3XXX_OHCI/d" $(1); \
			echo "# CONFIG_RT3XXX_OHCI is not set" >>$(1); \
			sed -i "/CONFIG_USB_OHCI_LITTLE_ENDIAN/d" $(1); \
			echo "# CONFIG_USB_OHCI_LITTLE_ENDIAN is not set" >>$(1); \
			sed -i "/CONFIG_USB_XHCI_HCD/d" $(1); \
			echo "CONFIG_USB_XHCI_HCD=y" >>$(1); \
			sed -i "/CONFIG_USB_MT7621_XHCI_HCD/d" $(1); \
			echo "CONFIG_USB_MT7621_XHCI_HCD=y" >>$(1); \
			sed -i "/CONFIG_PERIODIC_ENP/d" $(1); \
			echo "CONFIG_PERIODIC_ENP=y" >>$(1); \
			sed -i "/CONFIG_XHCI_DEV_NOTE/d" $(1); \
			echo "CONFIG_XHCI_DEV_NOTE=y" >>$(1); \
			sed -i "/CONFIG_USB_XHCI_HCD_DEBUGGING/d" $(1); \
			echo "CONFIG_USB_XHCI_HCD_DEBUGGING=y" >>$(1); \
			sed -i "/CONFIG_USB_EHCI_HCD/d" $(1); \
			echo "# CONFIG_USB_EHCI_HCD is not set" >>$(1); \
			sed -i "/CONFIG_USB_OHCI_HCD/d" $(1); \
			echo "# CONFIG_USB_OHCI_HCD is not set" >>$(1); \
			sed -i "/CONFIG_USB_OHCI_LITTLE_ENDIAN/d" $(1); \
			echo "# CONFIG_USB_OHCI_LITTLE_ENDIAN is not set" >>$(1); \
			for first_if in $(FIRST_IF_POOL) ; do \
				if [ "$(FIRST_IF)" = "$${first_if}" ] ; then \
					sed -i "/CONFIG_FIRST_IF_$${first_if}/d" $(1); \
					echo "CONFIG_FIRST_IF_$${first_if}=y" >> $(1); \
				else \
					sed -i "/CONFIG_FIRST_IF_$${first_if}/d" $(1); \
					echo "# CONFIG_FIRST_IF_$${first_if} is not set" >> $(1); \
				fi; \
			done; \
			if [ "$(NO_USBSTORAGE)" = "y" ]; then \
				sed -i "/CONFIG_PARTITION_ADVANCED/d" $(1); \
				echo "# CONFIG_PARTITION_ADVANCED is not set" >>$(1); \
				sed -i "/CONFIG_MSDOS_PARTITION/d" $(1); \
				echo "# CONFIG_MSDOS_PARTITION is not set" >>$(1); \
				sed -i "/CONFIG_EFI_PARTITION/d" $(1); \
				echo "# CONFIG_EFI_PARTITION is not set" >>$(1); \
				sed -i "/CONFIG_DEVTMPFS/d" $(1); \
				echo "# CONFIG_DEVTMPFS is not set" >>$(1); \
				sed -i "/CONFIG_DEVTMPFS_MOUNT/d" $(1); \
				echo "# CONFIG_DEVTMPFS_MOUNT is not set" >>$(1); \
				sed -i "/CONFIG_SCSI_MOD/d" $(1); \
				echo "# CONFIG_SCSI_MOD is not set" >>$(1); \
				sed -i "/CONFIG_SCSI/d" $(1); \
				echo "# CONFIG_SCSI is not set" >>$(1); \
				sed -i "/CONFIG_SCSI_DMA/d" $(1); \
				echo "# CONFIG_SCSI_DMA is not set" >>$(1); \
				sed -i "/CONFIG_SCSI_PROC_FS/d" $(1); \
				echo "# CONFIG_SCSI_PROC_FS is not set" >>$(1); \
				sed -i "/CONFIG_BLK_DEV_SD/d" $(1); \
				echo "# CONFIG_BLK_DEV_SD is not set" >>$(1); \
				sed -i "/CONFIG_BLK_DEV_SR/d" $(1); \
				echo "# CONFIG_BLK_DEV_SR is not set" >>$(1); \
				sed -i "/CONFIG_CHR_DEV_SG/d" $(1); \
				echo "# CONFIG_CHR_DEV_SG is not set" >>$(1); \
				sed -i "/CONFIG_SCSI_CONSTANTS/d" $(1); \
				echo "# CONFIG_SCSI_CONSTANTS is not set" >>$(1); \
				sed -i "/CONFIG_USB_ACM/d" $(1); \
				echo "# CONFIG_USB_ACM is not set" >>$(1); \
				sed -i "/CONFIG_USB_WDM/d" $(1); \
				echo "# CONFIG_USB_WDM is not set" >>$(1); \
				sed -i "/CONFIG_USB_TMC/d" $(1); \
				echo "# CONFIG_USB_TMC is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE/d" $(1); \
				echo "# CONFIG_USB_STORAGE is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_DATAFAB/d" $(1); \
				echo "# CONFIG_USB_STORAGE_DATAFAB is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_FREECOM/d" $(1); \
				echo "# CONFIG_USB_STORAGE_FREECOM is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_ISD200/d" $(1); \
				echo "# CONFIG_USB_STORAGE_ISD200 is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_USBAT/d" $(1); \
				echo "# CONFIG_USB_STORAGE_USBAT is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_SDDR09/d" $(1); \
				echo "# CONFIG_USB_STORAGE_SDDR09 is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_SDDR55/d" $(1); \
				echo "# CONFIG_USB_STORAGE_SDDR55 is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_JUMPSHOT/d" $(1); \
				echo "# CONFIG_USB_STORAGE_JUMPSHOT is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_ALAUDA/d" $(1); \
				echo "# CONFIG_USB_STORAGE_ALAUDA is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_KARMA/d" $(1); \
				echo "# CONFIG_USB_STORAGE_KARMA is not set" >>$(1); \
				sed -i "/CONFIG_USB_STORAGE_CYPRESS_ATACB/d" $(1); \
				echo "# CONFIG_USB_STORAGE_CYPRESS_ATACB is not set" >>$(1); \
				sed -i "/CONFIG_USB_UAS/d" $(1); \
				echo "# CONFIG_USB_UAS is not set" >>$(1); \
			fi; \
		fi; \
                if [ "$(MT7628)" = "y" ]; then \
                        sed -i "/CONFIG_RALINK_MT7628/d" $(1); \
                        echo "CONFIG_RALINK_MT7628=y" >>$(1); \
                        sed -i "/CONFIG_MT7628_ASIC/d" $(1); \
                        echo "CONFIG_MT7628_ASIC=y" >>$(1); \
                fi;     \
                if [ "$(MT7629)" = "y" ]; then \
                        sed -i "/CONFIG_RALINK_MT7629/d" $(1); \
                        echo "CONFIG_RALINK_MT7629=y" >>$(1); \
                        sed -i "/CONFIG_MT7629_ASIC/d" $(1); \
                        echo "CONFIG_MT7629_ASIC=y" >>$(1); \
                fi;     \
                if [ "$(MT7621)" = "y" ]; then \
                        sed -i "/CONFIG_RALINK_MT7621/d" $(1); \
                        echo "CONFIG_RALINK_MT7621=y" >>$(1); \
                        sed -i "/CONFIG_MT7621_ASIC/d" $(1); \
                        echo "CONFIG_MT7621_ASIC=y" >>$(1); \
                fi;     \
                if [ "$(MT7622)" = "y" ]; then \
                        sed -i "/CONFIG_RALINK_MT7622/d" $(1); \
                        echo "CONFIG_RALINK_MT7622=y" >>$(1); \
                        sed -i "/CONFIG_MT7622_ASIC/d" $(1); \
                        echo "CONFIG_MT7622_ASIC=y" >>$(1); \
                fi;     \
	fi; \
	for sec_if in $(SECOND_IF_POOL) ; do \
		sed -i "/CONFIG_SECOND_IF_$${sec_if}\>/d" $(1); \
		if [ "$(SECOND_IF)" = "$${sec_if}" ] ; then \
			echo "CONFIG_SECOND_IF_$${sec_if}=y" >> $(1); \
		else \
			echo "# CONFIG_SECOND_IF_$${sec_if} is not set" >> $(1); \
		fi; \
	done; \
	if [ "$(REPEATER)" = "y" ] ; then \
			sed -i "/CONFIG_RT2860V2_AP_APCLI/d" $(1); \
			echo "CONFIG_RT2860V2_AP_APCLI=y" >>$(1); \
			sed -i "/CONFIG_RT2860V2_AP_MAC_REPEATER/d" $(1); \
			echo "CONFIG_RT2860V2_AP_MAC_REPEATER=y" >>$(1); \
		if [ "$(FIRST_IF)" = "MT7615E" ] || [ "$(SECOND_IF)" = "MT7615E" ] ; then \
			sed -i "/CONFIG_APCLI_SUPPORT/d" $(1); \
			echo "CONFIG_APCLI_SUPPORT=y" >>$(1); \
			sed -i "/CONFIG_MAC_REPEATER_SUPPORT/d" $(1); \
			echo "CONFIG_MAC_REPEATER_SUPPORT=y" >>$(1); \
			sed -i "/CONFIG_APCLI_CERT_SUPPORT/d" $(1); \
			echo "CONFIG_APCLI_CERT_SUPPORT=y" >>$(1); \
		fi; \
	fi; \
	if [ "$(RA_SKU)" = "y" ] || [ "$(RA_SKU_IN_DRV)" = "y" ] ; then \
			sed -i "/CONFIG_RT2860V2_SINGLE_SKU/d" $(1); \
			echo "CONFIG_RT2860V2_SINGLE_SKU=y" >>$(1); \
		if [ "$(RA_SKU_IN_DRV)" = "y" ] ; then \
			sed -i "/CONFIG_SINGLE_SKU_IN_DRIVER/d" $(1); \
			echo "CONFIG_SINGLE_SKU_IN_DRIVER=y" >>$(1); \
		fi; \
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
			sed -i "/CONFIG_UBIFS_FS\>/d" $(1); \
			echo "CONFIG_UBIFS_FS=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_XATTR/d" $(1); \
			echo "# CONFIG_UBIFS_FS_XATTR is not set" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ADVANCED_COMPR/d" $(1); \
			echo "CONFIG_UBIFS_FS_ADVANCED_COMPR=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_LZO/d" $(1); \
			echo "CONFIG_UBIFS_FS_LZO=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ZLIB/d" $(1); \
			echo "CONFIG_UBIFS_FS_ZLIB=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_DEBUG/d" $(1); \
			echo "# CONFIG_UBIFS_FS_DEBUG is not set" >>$(1); \
			echo "# CONFIG_UBIFS_ATIME_SUPPORT is not set" >>$(1); \
			echo "# CONFIG_UBIFS_FS_ENCRYPTION is not set" >>$(1); \
			echo "# CONFIG_UBIFS_FS_SECURITY is not set" >>$(1); \
		else \
			sed -i "/CONFIG_UBIFS_FS/d" $(1); \
			echo "# CONFIG_UBIFS_FS is not set" >>$(1); \
		fi; \
	fi; \
	if [ "$(FIBOCOM_FG621)" = "y" ]; then \
		sed -i "/CONFIG_FIBOCOM_FG621/d" $(1); \
		echo "CONFIG_FIBOCOM_FG621=y" >>$(1); \
	fi; \
	if [ "$(RMAX6000)" = "y" ]; then \
		sed -i "/CONFIG_MODEL_RMAX6000/d" $(1); \
		echo "CONFIG_MODEL_RMAX6000=y" >>$(1); \
		sed -i "/CONFIG_CMDLINE_OVERRIDE/d" $(1); \
		echo "CONFIG_CMDLINE_OVERRIDE=y" >>$(1); \
	fi; \
	if [ "$(SWRT_FULLCONEV2)" = "y" ]; then \
		sed -i "/CONFIG_SWRT_FULLCONE/d" $(1); \
		echo "# CONFIG_SWRT_FULLCONE is not set" >>$(1); \
		echo "CONFIG_SWRT_FULLCONEV2=y" >>$(1); \
	else \
		sed -i "/CONFIG_SWRT_FULLCONE/d" $(1); \
		echo "CONFIG_SWRT_FULLCONE=y" >>$(1); \
		echo "# CONFIG_SWRT_FULLCONEV2 is not set" >>$(1); \
	fi; \
	if [ "$(BUILD_NAME)" = "RM-AX6000" ]; then \
		sed -i "/CONFIG_LEDS_CLASS_MULTICOLOR\>/d" $(1); \
		echo "CONFIG_LEDS_CLASS_MULTICOLOR=y" >>$(1); \
	fi; \
	)
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)

