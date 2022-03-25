export LINUXDIR := $(SRCBASE)/linux/linux-3.14.x

ifeq ($(EXTRACFLAGS),)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -marm -march=armv7-a -msoft-float -mfloat-abi=soft -mtune=cortex-a7
endif

export KERNEL_BINARY=$(LINUXDIR)/vmlinux
export PLATFORM := arm-uclibc
export CROSS_COMPILE := arm-openwrt-linux-uclibcgnueabi-
export CROSS_COMPILER := $(CROSS_COMPILE)
export READELF := arm-openwrt-linux-uclibcgnueabi-readelf
export CONFIGURE := ./configure --host=arm-linux --build=$(BUILD)
export HOSTCONFIG := linux-armv4
export ARCH := arm
export HOST := arm-linux
export KERNELCC := $(CROSS_COMPILE)gcc
export KERNELLD := $(CROSS_COMPILE)ld
export TOOLS := /opt/openwrt-gcc463.arm
export RTVER := 0.9.33.2

# Kernel load address and entry address
export LOADADDR := 80208000
export ENTRYADDR := $(LOADADDR)

# OpenWRT's toolchain needs STAGING_DIR environment variable that points to top directory of toolchain.
export STAGING_DIR=$(shell which arm-openwrt-linux-gcc|sed -e "s,/bin/arm-openwrt-linux-gcc,,")

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_QCA -pipe -DDEBUG_NOISY -DDEBUG_RCTEST -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp

export CONFIG_LINUX26=y
export CONFIG_QCA=y

EXTRA_CFLAGS += -DLINUX30
export CONFIG_LINUX30=y

ifneq ($(IPQ40XX),)
EXTRA_CFLAGS += -DLINUX3_14
export CONFIG_LINUX3_14=y
endif

SWITCH_CHIP_ID_POOL =

define platformRouterOptions
	@( \
	if [ "$(QCA)" = "y" ]; then \
		sed -i "/RTCONFIG_QCA\>/d" $(1); \
		echo "RTCONFIG_QCA=y" >>$(1); \
		sed -i "/RTCONFIG_QCA_ARM/d" $(1); \
		echo "RTCONFIG_QCA_ARM=y" >>$(1); \
		sed -i "/RTCONFIG_32BYTES_ODMPID/d" $(1); \
		echo "RTCONFIG_32BYTES_ODMPID=y" >>$(1); \
		sed -i "/RTCONFIG_QCA_BIGRATE_WIFI\>/d" $(1); \
		echo "RTCONFIG_QCA_BIGRATE_WIFI=y" >>$(1); \
		if [ "$(RT4GAC53U)" = "y" ]; then \
			sed -i "/RTCONFIG_LEDS_CLASS/d" $(1); \
			echo "RTCONFIG_LEDS_CLASS=y" >>$(1); \
			sed -i "/RTCONFIG_NO_WANPORT/d" $(1); \
			echo "RTCONFIG_NO_WANPORT=y" >>$(1); \
		fi; \
		if [ "$(IPQ40XX)" = "y" ]; then \
			sed -i "/RTCONFIG_SOC_IPQ40XX/d" $(1); \
			echo "RTCONFIG_SOC_IPQ40XX=y" >>$(1); \
			sed -i "/RTCONFIG_FITFDT/d" $(1); \
			echo "RTCONFIG_FITFDT=y" >>$(1); \
		fi; \
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
		if [ "$(WIFI_CHIP)" = "CASCADE" ] ; then \
			sed -i "/RTCONFIG_WIFI_QCA9994_QCA9994/d" $(1); \
			echo "RTCONFIG_WIFI_QCA9994_QCA9994=y" >>$(1); \
		fi; \
		for chip in $(SWITCH_CHIP_ID_POOL) ; do \
			sed -i "/RTCONFIG_SWITCH_$${chip}\>/d" $(1); \
			if [ "$(SWITCH_CHIP)" = "$${chip}" ] ; then \
				echo "RTCONFIG_SWITCH_$${chip}=y" >> $(1); \
			else \
				echo "# RTCONFIG_SWITCH_$${chip} is not set" >> $(1); \
			fi; \
		done; \
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
	fi; \
	)
endef

define platformKernelConfig
	@( \
	if [ "$(QCA)" = "y" ]; then \
		sed -i "/CONFIG_RTAC55U/d" $(1); \
                echo "# CONFIG_RTAC55U is not set" >>$(1); \
		if [ "$(CONFIG_LINUX30)" = "y" ]; then \
			if [ "$(BOOT_FLASH_TYPE)" = "SPI" ] ; then \
				sed -i "/CONFIG_MTD_MSM_NAND\>/d" $(1); \
				echo "# CONFIG_MTD_MSM_NAND is not set" >> $(1); \
				sed -i "/CONFIG_MTD_M25P80\>/d" $(1); \
				echo "CONFIG_MTD_M25P80=y" >> $(1); \
				sed -i "/CONFIG_SPI_QUP\>/d" $(1); \
				echo "CONFIG_SPI_QUP=y" >> $(1); \
				if [ "$(RTAC58U)" = "y" ] || [ "$(RT4GAC53U)" = "y" ] || [ "$(MAPAC1300)" = "y" ] || [ "$(VZWAC1300)" = "y" ]; then \
					sed -i "/CONFIG_MTD_SPINAND_MT29F/d" $(1); \
					echo "CONFIG_MTD_SPINAND_MT29F=y" >>$(1); \
					sed -i "/CONFIG_MTD_SPINAND_ONDIEECC/d" $(1); \
					echo "CONFIG_MTD_SPINAND_ONDIEECC=y" >>$(1); \
					sed -i "/CONFIG_MTD_SPINAND_GIGADEVICE/d" $(1); \
					echo "CONFIG_MTD_SPINAND_GIGADEVICE=y" >>$(1); \
				fi; \
			else \
				if [ "$(RTAC82U)" = "y" ] || [ "$(MAPAC2200)" = "y" ] || [ "$(RTAC95U)" = "y" ]; then \
					sed -i "/CONFIG_MTD_MSM_QPIC_NAND/d" $(1); \
					echo "CONFIG_MTD_MSM_QPIC_NAND=y" >>$(1); \
				else \
					sed -i "/CONFIG_MTD_MSM_NAND\>/d" $(1); \
					echo "CONFIG_MTD_MSM_NAND=y" >> $(1); \
				fi; \
				sed -i "/CONFIG_MTD_M25P80\>/d" $(1); \
				echo "# CONFIG_MTD_M25P80 is not set" >> $(1); \
			fi; \
			sed -i "/CONFIG_BRIDGE_EBT_ARPNAT/d" $(1); \
			echo "# CONFIG_BRIDGE_EBT_ARPNAT is not set" >>$(1); \
			sed -i "/CONFIG_NF_CONNTRACK_EVENTS/d" $(1); \
			echo "CONFIG_NF_CONNTRACK_EVENTS=y" >>$(1); \
			sed -i "/CONFIG_NF_CONNTRACK_CHAIN_EVENTS/d" $(1); \
			echo "# CONFIG_NF_CONNTRACK_CHAIN_EVENTS is not set" >>$(1); \
		fi; \
		if [ "$(RTAC58U)" = "y" ]; then \
			sed -i "/CONFIG_RTAC58U/d" $(1); \
			echo "CONFIG_RTAC58U=y" >>$(1); \
			sed -i "/CONFIG_NF_CONNTRACK_CHAIN_EVENTS/d" $(1); \
			echo "CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y" >>$(1); \
		fi; \
		if [ "$(RT4GAC53U)" = "y" ]; then \
			sed -i "/CONFIG_4GAC53U/d" $(1); \
			echo "CONFIG_4GAC53U=y" >>$(1); \
			sed -i "/CONFIG_NF_CONNTRACK_CHAIN_EVENTS/d" $(1); \
			echo "CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y" >>$(1); \
			sed -i "/CONFIG_SPI_GPIO/d" $(1); \
			echo "CONFIG_SPI_GPIO=y" >>$(1); \
			echo "# CONFIG_SPI_GPIO_OLD is not set" >>$(1); \
			sed -i "/CONFIG_GPIO_74X164/d" $(1); \
			echo "CONFIG_GPIO_74X164=y" >>$(1); \
			sed -i "/CONFIG_NEW_LEDS/d" $(1); \
			echo "CONFIG_NEW_LEDS=y" >>$(1); \
			sed -i "/CONFIG_LEDS_CLASS/d" $(1); \
			echo "CONFIG_LEDS_CLASS=y" >>$(1); \
			echo "# CONFIG_LEDS_LM3530 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LM3642 is not set" >>$(1); \
			sed -i "/CONFIG_LEDS_GPIO/d" $(1); \
			echo "CONFIG_LEDS_GPIO=y" >>$(1); \
			echo "# CONFIG_LEDS_LP3944 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP5521 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP5523 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP5562 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP8501 is not set" >>$(1); \
			echo "# CONFIG_LEDS_PCA955X is not set" >>$(1); \
			echo "# CONFIG_LEDS_PCA963X is not set" >>$(1); \
			echo "# CONFIG_LEDS_PCA9685 is not set" >>$(1); \
			echo "# CONFIG_LEDS_DAC124S085 is not set" >>$(1); \
			echo "# CONFIG_LEDS_REGULATOR is not set" >>$(1); \
			echo "# CONFIG_LEDS_BD2802 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LT3593 is not set" >>$(1); \
			echo "# CONFIG_LEDS_TCA6507 is not set" >>$(1); \
			echo "# CONFIG_LEDS_TLC591XX is not set" >>$(1); \
			echo "# CONFIG_LEDS_LM355x is not set" >>$(1); \
			echo "# CONFIG_LEDS_OT200 is not set" >>$(1); \
			echo "# CONFIG_LEDS_BLINKM is not set" >>$(1); \
			echo "# CONFIG_LEDS_IPQ40XX is not set" >>$(1); \
			echo "# CONFIG_LEDS_TRIGGERS is not set" >>$(1); \
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
			sed -i "/CONFIG_NEW_LEDS/d" $(1); \
			echo "CONFIG_NEW_LEDS=y" >>$(1); \
			sed -i "/CONFIG_LEDS_CLASS/d" $(1); \
			echo "CONFIG_LEDS_CLASS=y" >>$(1); \
			sed -i "/CONFIG_LEDS_LP55XX_COMMON/d" $(1); \
			echo "CONFIG_LEDS_LP55XX_COMMON=y" >>$(1); \
			sed -i "/CONFIG_LEDS_LP5523/d" $(1); \
			echo "CONFIG_LEDS_LP5523=y" >>$(1); \
			echo "# CONFIG_LEDS_LM3530 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LM3642 is not set" >>$(1); \
			echo "# CONFIG_LEDS_GPIO is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP3944 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP5521 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP5562 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LP8501 is not set" >>$(1); \
			echo "# CONFIG_LEDS_PCA955X is not set" >>$(1); \
			echo "# CONFIG_LEDS_PCA963X is not set" >>$(1); \
			echo "# CONFIG_LEDS_PCA9685 is not set" >>$(1); \
			echo "# CONFIG_LEDS_DAC124S085 is not set" >>$(1); \
			echo "# CONFIG_LEDS_REGULATOR is not set" >>$(1); \
			echo "# CONFIG_LEDS_BD2802 is not set" >>$(1); \
			echo "# CONFIG_LEDS_LT3593 is not set" >>$(1); \
			echo "# CONFIG_LEDS_TCA6507 is not set" >>$(1); \
			echo "# CONFIG_LEDS_TLC591XX is not set" >>$(1); \
			echo "# CONFIG_LEDS_LM355x is not set" >>$(1); \
			echo "# CONFIG_LEDS_OT200 is not set" >>$(1); \
			echo "# CONFIG_LEDS_BLINKM is not set" >>$(1); \
			echo "# CONFIG_LEDS_IPQ40XX is not set" >>$(1); \
			echo "# CONFIG_LEDS_TRIGGERS is not set" >>$(1); \
		fi; \
		if [ "$(BT_CONN)" = "y" ] ; then \
			sed -i "/CONFIG_BT/d" $(1); \
			echo "CONFIG_BT=y" >>$(1); \
			sed -i "/CONFIG_BT_HCIBTUSB/d" $(1); \
			if [ "$(RTAC95U)" = "y" ]; then \
				echo "# CONFIG_BT_HCIBTUSB is not set" >>$(1); \
			else \
				echo "CONFIG_BT_HCIBTUSB=m" >>$(1); \
			fi; \
			sed -i "/CONFIG_BT_RFCOMM/d" $(1); \
			echo "CONFIG_BT_RFCOMM=y" >>$(1); \
			sed -i "/CONFIG_BT_RFCOMM_TTY/d" $(1); \
			echo "CONFIG_BT_RFCOMM_TTY=y" >>$(1); \
			sed -i "/CONFIG_BT_BNEP/d" $(1); \
			echo "CONFIG_BT_BNEP=y" >>$(1); \
			sed -i "/CONFIG_BT_BNEP_MC_FILTER/d" $(1); \
			echo "CONFIG_BT_BNEP_MC_FILTER=y" >>$(1); \
			sed -i "/CONFIG_BT_BNEP_PROTO_FILTER/d" $(1); \
			echo "CONFIG_BT_BNEP_PROTO_FILTER=y" >>$(1); \
			sed -i "/CONFIG_BT_HCIUART/d" $(1); \
			if [ "$(RTAC95U)" = "y" ]; then \
				echo "CONFIG_BT_HCIUART=y" >>$(1); \
			else \
				echo "# CONFIG_BT_HCIUART is not set" >>$(1); \
			fi; \
			echo "# CONFIG_BT_HCIUART_H4 is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIUART_BCSP/d" $(1); \
			echo "# CONFIG_BT_HCIUART_BCSP is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIBCM203X/d" $(1); \
			echo "# CONFIG_BT_HCIBCM203X is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIBPA10X/d" $(1); \
			echo "# CONFIG_BT_HCIBPA10X is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIBFUSB/d" $(1); \
			echo "# CONFIG_BT_HCIBFUSB is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIUART_ATH3K/d" $(1); \
			echo "# CONFIG_BT_HCIUART_ATH3K is not set" >>$(1); \
			sed -i "/CONFIG_BT_ATH3K/d" $(1); \
			if [ "$(RTAC95U)" = "y" ]; then \
				echo "# CONFIG_BT_ATH3K is not set" >>$(1); \
			else \
				echo "CONFIG_BT_ATH3K=m" >>$(1); \
			fi; \
			echo "# CONFIG_BT_HCIUART_LL is not set" >>$(1); \
			echo "# CONFIG_BT_HCIUART_3WIRE is not set" >>$(1); \
			sed -i "/CONFIG_BT_HCIVHCI/d" $(1); \
			echo "# CONFIG_BT_HCIVHCI is not set" >>$(1); \
			echo "# CONFIG_BT_MRVL is not set" >>$(1); \
			echo "# CONFIG_BTRFS_FS is not set" >>$(1); \
		fi; \
		if [ "$(IPQ40XX)" = "y" ]; then \
			if [ "$(HFS)" = "open" ]; then \
				sed -i "/HFSPLUS_FS_POSIX_ACL/d" $(1); \
				echo "# CONFIG_HFSPLUS_FS_POSIX_ACL is not set" >>$(1); \
			fi; \
		fi; \
		if [ "$(RTAC88N)" = "y" ] ; then \
			sed -i "/CONFIG_MACH_IPQ806X_AP148\>/d" $(1); \
			echo "CONFIG_MACH_IPQ806X_AP148=y" >>$(1); \
			sed -i "/CONFIG_MACH_IPQ806X_AP161/d" $(1); \
			echo "CONFIG_MACH_IPQ806X_AP161=y" >>$(1); \
			sed -i "/CONFIG_QCA_AP148/d" $(1); \
			echo "CONFIG_QCA_AP148=y" >>$(1); \
			sed -i "/CONFIG_ASUS_RTAC88Q/d" $(1); \
			echo "# CONFIG_ASUS_RTAC88Q is not set" >>$(1); \
		fi; \
		if [ "$(RTAC88Q)" = "y" ] || [ "$(RTAC88S)" = "y" ] ; then \
			sed -i "/CONFIG_QCA_AP148/d" $(1); \
			echo "# CONFIG_QCA_AP148 is not set" >>$(1); \
			sed -i "/CONFIG_ASUS_RTAC88Q/d" $(1); \
			echo "CONFIG_ASUS_RTAC88Q=y" >>$(1); \
			sed -i "/CONFIG_SERIAL_MSM_HS_GSBI2/d" $(1); \
			echo "CONFIG_SERIAL_MSM_HS_GSBI2=y" >>$(1); \
		fi; \
		if [ "$(SWITCH_CHIP)" = "RTL8370M_PHY_QCA8033_X2" ] ; then \
			sed -i "/CONFIG_AR8033_PHY/d" $(1); \
			echo "CONFIG_AR8033_PHY=y" >>$(1); \
		fi; \
		for chip in $(SWITCH_CHIP_ID_POOL) ; do \
			sed -i "/CONFIG_SWITCH_$${chip}\>/d" $(1); \
			if [ "$(SWITCH_CHIP)" = "$${chip}" ] ; then \
				echo "CONFIG_SWITCH_$${chip}=y" >> $(1); \
			else \
				echo "# CONFIG_SWITCH_$${chip} is not set" >> $(1); \
			fi; \
		done; \
		if [ "$(WIFI_CHIP)" = "BEELINER" ] ; then \
			sed -i "/CONFIG_IPQ8064_ILQ_13_NSS/d" $(1); \
			echo "CONFIG_IPQ8064_ILQ_13_NSS=y" >>$(1); \
			sed -i "/CONFIG_ETHERNET/d" $(1); \
			echo "# CONFIG_ETHERNET is not set" >>$(1); \
			sed -i "/CONFIG_MDIO\>/d" $(1); \
			echo "# CONFIG_MDIO is not set" >>$(1); \
		fi; \
		if [ "$(WIFI_CHIP)" = "CASCADE" ] ; then \
			sed -i "/CONFIG_IPQ8064_ILQ_30_NSS/d" $(1); \
			echo "CONFIG_IPQ8064_ILQ_30_NSS=y" >>$(1); \
			sed -i "/CONFIG_ETHERNET/d" $(1); \
			echo "CONFIG_ETHERNET=y" >>$(1); \
			sed -i "/CONFIG_MDIO\>/d" $(1); \
			echo "CONFIG_MDIO=y" >>$(1); \
			sed -i "/CONFIG_IP_MROUTE_MULTIPLE_TABLES\>/d" $(1); \
			echo "CONFIG_IP_MROUTE_MULTIPLE_TABLES=y" >>$(1); \
			sed -i "/CONFIG_KEYS\>/d" $(1); \
			echo "CONFIG_KEYS=y" >>$(1); \
			sed -i "/CONFIG_ASYMMETRIC_KEY_TYPE\>/d" $(1); \
			echo "CONFIG_ASYMMETRIC_KEY_TYPE=y" >>$(1); \
			sed -i "/CONFIG_ASYMMETRIC_PUBLIC_KEY_SUBTYPE\>/d" $(1); \
			echo "CONFIG_ASYMMETRIC_PUBLIC_KEY_SUBTYPE=y" >>$(1); \
			sed -i "/CONFIG_PUBLIC_KEY_ALGO_RSA\>/d" $(1); \
			echo "CONFIG_PUBLIC_KEY_ALGO_RSA=y" >>$(1); \
			sed -i "/CONFIG_X509_CERTIFICATE_PARSER\>/d" $(1); \
			echo "CONFIG_X509_CERTIFICATE_PARSER=y" >>$(1); \
			sed -i "/CONFIG_FW_AUTH\>/d" $(1); \
			echo "CONFIG_FW_AUTH=y" >>$(1); \
		fi; \
	fi; \
	if [ "$(JFFS2)" = "y" ]; then \
		if [ "$(CONFIG_LINUX26)" = "y" ]; then \
			sed -i "/CONFIG_JFFS2_FS/d" $(1); \
			echo "CONFIG_JFFS2_FS=m" >>$(1); \
			sed -i "/CONFIG_JFFS2_FS_DEBUG/d" $(1); \
			echo "CONFIG_JFFS2_FS_DEBUG=0" >>$(1); \
			sed -i "/CONFIG_JFFS2_FS_WRITEBUFFER/d" $(1); \
			echo "CONFIG_JFFS2_FS_WRITEBUFFER=y" >>$(1); \
			sed -i "/CONFIG_JFFS2_SUMMARY/d" $(1); \
			echo "# CONFIG_JFFS2_SUMMARY is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_FS_XATTR/d" $(1); \
			echo "# CONFIG_JFFS2_FS_XATTR is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_COMPRESSION_OPTIONS/d" $(1); \
			echo "CONFIG_JFFS2_COMPRESSION_OPTIONS=y" >>$(1); \
			sed -i "/CONFIG_JFFS2_ZLIB/d" $(1); \
			echo "CONFIG_JFFS2_ZLIB=y" >>$(1); \
			sed -i "/CONFIG_JFFS2_LZO/d" $(1); \
			echo "# CONFIG_JFFS2_LZO is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_LZMA/d" $(1); \
			echo "# CONFIG_JFFS2_LZMA is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_RTIME/d" $(1); \
			echo "# CONFIG_JFFS2_RTIME is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_RUBIN/d" $(1); \
			echo "# CONFIG_JFFS2_RUBIN is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_CMODE_NONE/d" $(1); \
			echo "# CONFIG_JFFS2_CMODE_NONE is not set" >>$(1); \
			sed -i "/CONFIG_JFFS2_CMODE_PRIORITY/d" $(1); \
			echo "CONFIG_JFFS2_CMODE_PRIORITY=y" >>$(1); \
			sed -i "/CONFIG_JFFS2_CMODE_SIZE/d" $(1); \
			echo "# CONFIG_JFFS2_CMODE_SIZE is not set" >>$(1); \
		fi; \
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
		fi; \
	fi; \
	if [ "$(DUALWAN)" = "y" ]; then \
		sed -i "/CONFIG_ESSEDMA_DUALWAN/d" $(1); \
		echo "CONFIG_ESSEDMA_DUALWAN=y" >>$(1); \
	fi; \
	)
endef
