export LINUXDIR := $(SRCBASE)/linux/linux-5.4.x

ifeq ($(EXTRACFLAGS),)
ifeq ($(MT798X),y)
export EXTRACFLAGS := -DBCMWPA2 -fno-delete-null-pointer-checks -march=armv8-a -mfpu=neon -mfloat-abi=softfp -mcpu=cortex-a53 -fsigned-char
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
EXTRA_CFLAGS += -Os -mcpu=cortex-a53 -march=armv8 -mfpu=neon -mfloat-abi=softfp -D_GNU_SOURCE -D_BSD_SOURCE -D__BIT_TYPES_DEFINED__ -DMUSL_LIBC -fsigned-char
endif

export CONFIG_LINUX26=y
export CONFIG_RALINK=y

#EXTRA_CFLAGS += -DLINUX30
export CONFIG_LINUX30=y
export CONFIG_SUPPORT_OPENWRT=y
define platformRouterOptions
	@( \
	sed -i "/RTCONFIG_RALINK\>/d" $(1); \
	echo "RTCONFIG_RALINK=y" >>$(1); \
	sed -i "/RTCONFIG_FITFDT/d" $(1); \
	echo "RTCONFIG_FITFDT=y" >>$(1); \
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
		sed -i "/RTCONFIG_QAM256_2G\>/d" $(1); \
		echo "RTCONFIG_QAM256_2G=y" >>$(1); \
		sed -i "/RTCONFIG_QAM1024_5G\>/d" $(1); \
		echo "RTCONFIG_QAM1024_5G=y" >>$(1); \
		if [ "$(MT7986_AX4200)" = "y" ]; then \
			sed -i "/CONFIG_MTK_WIFI_SKU_TYPE=/d" $(1); \
			echo "CONFIG_MTK_WIFI_SKU_TYPE=\"AX4200\"" >> $(1); \
		fi; \
		if [ "$(MTK_ADIE)" = "mt7976" ]; then \
			sed -i "/CONFIG_MTK_WIFI_ADIE_TYPE=/d" $(1); \
			echo "CONFIG_MTK_WIFI_ADIE_TYPE=\"mt7976\"" >> $(1); \
		elif [ "$(MTK_ADIE)" = "mt7975" ]; then \
			sed -i "/CONFIG_MTK_WIFI_ADIE_TYPE=/d" $(1); \
			echo "CONFIG_MTK_WIFI_ADIE_TYPE=\"mt7975\"" >> $(1); \
		fi; \
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
	if [ "$(BUILD_NAME)" = "TUF-AX6000" ]; then \
		sed -i "/RTCONFIG_PWMX2_GPIOX1_RGBLED\>/d" $(1); \
		echo "RTCONFIG_PWMX2_GPIOX1_RGBLED=y" >>$(1); \
	elif [ "$(BUILD_NAME)" = "RT-AX59U" ] ; then \
		sed -i "/RTCONFIG_PWM_RGBLED\>/d" $(1); \
		echo "RTCONFIG_PWM_RGBLED=y" >>$(1); \
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
	sed -i "/CONFIG_NVRAM_SIZE\>/d" $(1); \
	echo "CONFIG_NVRAM_SIZE=`printf 0x%x $(NVRAM_SIZE)`" >>$(1); \
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
			sed -i "/CONFIG_UBIFS_FS_AUTHENTICATION/d" $(1); \
			echo "# CONFIG_UBIFS_FS_AUTHENTICATION is not set" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ADVANCED_COMPR/d" $(1); \
			echo "CONFIG_UBIFS_FS_ADVANCED_COMPR=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_LZO/d" $(1); \
			echo "CONFIG_UBIFS_FS_LZO=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ZLIB/d" $(1); \
			echo "CONFIG_UBIFS_FS_ZLIB=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_XZ/d" $(1); \
			echo "CONFIG_UBIFS_FS_XZ=y" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_ZSTD/d" $(1); \
			echo "# CONFIG_UBIFS_FS_ZSTD is not set" >>$(1); \
			sed -i "/CONFIG_UBIFS_FS_DEBUG/d" $(1); \
			echo "# CONFIG_UBIFS_FS_DEBUG is not set" >>$(1); \
		else \
			sed -i "/CONFIG_UBIFS_FS/d" $(1); \
			echo "# CONFIG_UBIFS_FS is not set" >>$(1); \
		fi; \
	fi; \
	if [ "$(FIBOCOM_FG621)" = "y" ]; then \
		sed -i "/CONFIG_FIBOCOM_FG621/d" $(1); \
		echo "CONFIG_FIBOCOM_FG621=y" >>$(1); \
	fi; \
	if [ "$(MT7981)" = "y" ]; then \
		sed -i "/CONFIG_FIRST_IF_MT7986/d" $(1); \
		echo "# CONFIG_FIRST_IF_MT7986 is not set" >>$(1); \
		sed -i "/CONFIG_FIRST_IF_MT7981/d" $(1); \
		echo "CONFIG_FIRST_IF_MT7981=y" >>$(1); \
		sed -i "/CONFIG_RT_FIRST_CARD/d" $(1); \
		echo "CONFIG_RT_FIRST_CARD=7981" >>$(1); \
		sed -i "/CONFIG_CHIP_MT7986/d" $(1); \
		echo "# CONFIG_CHIP_MT7986 is not set" >>$(1); \
		echo "CONFIG_CHIP_MT7981=y" >>$(1); \
		sed -i "/CONFIG_WARP_CHIPSET/d" $(1); \
		echo "CONFIG_WARP_CHIPSET=\"mt7981\"" >>$(1); \
		sed -i "/CONFIG_COMMON_CLK_MT7986/d" $(1); \
		echo "# CONFIG_COMMON_CLK_MT7986 is not set" >>$(1); \
		echo "CONFIG_COMMON_CLK_MT7981=y" >>$(1); \
		sed -i "/CONFIG_PINCTRL_MT7986/d" $(1); \
		echo "# CONFIG_PINCTRL_MT7986 is not set" >>$(1); \
		echo "CONFIG_PINCTRL_MT7981=y" >>$(1); \
		echo "# CONFIG_AIROHA_EN8801S_PHY is not set" >>$(1); \
		echo "CONFIG_AIROHA_EN8801S_PHY=y" >>$(1); \
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
	elif [ "$(SWRT_FULLCONE)" = "y" ]; then \
		sed -i "/CONFIG_SWRT_FULLCONE/d" $(1); \
		echo "CONFIG_SWRT_FULLCONE=y" >>$(1); \
		echo "# CONFIG_SWRT_FULLCONEV2 is not set" >>$(1); \
	else \
		sed -i "/CONFIG_SWRT_FULLCONE/d" $(1); \
		echo "# CONFIG_SWRT_FULLCONE is not set" >>$(1); \
		echo "# CONFIG_SWRT_FULLCONEV2 is not set" >>$(1); \
	fi; \
	if [ "$(BUILD_NAME)" = "RM-AX6000" ]; then \
		sed -i "/CONFIG_LEDS_CLASS_MULTICOLOR\>/d" $(1); \
		echo "CONFIG_LEDS_CLASS_MULTICOLOR=y" >>$(1); \
		sed -i "/CONFIG_BLK_DEV_RAM\>/d" $(1); \
		echo "CONFIG_BLK_DEV_RAM=y" >>$(1); \
		echo "CONFIG_BLK_DEV_RAM_COUNT=1" >>$(1); \
		echo "CONFIG_BLK_DEV_RAM_SIZE=16384" >>$(1); \
		sed -i "/CONFIG_NMBM\>/d" $(1); \
		echo "CONFIG_NMBM=y" >>$(1); \
		echo "# CONFIG_NMBM_LOG_LEVEL_DEBUG is not set" >>$(1); \
		echo "CONFIG_NMBM_LOG_LEVEL_INFO=y" >>$(1); \
		echo "# CONFIG_NMBM_LOG_LEVEL_WARN is not set" >>$(1); \
		echo "# CONFIG_NMBM_LOG_LEVEL_ERR is not set" >>$(1); \
		echo "# CONFIG_NMBM_LOG_LEVEL_EMERG is not set" >>$(1); \
		echo "# CONFIG_NMBM_LOG_LEVEL_NONE is not set" >>$(1); \
		echo "CONFIG_NMBM_MTD=y" >>$(1); \
	fi; \
	if [ "$(BUILD_NAME)" = "SWRT360-T7" ]; then \
		sed -i "/CONFIG_MODEL_SWRT360T7/d" $(1); \
		echo "CONFIG_MODEL_SWRT360T7=y" >>$(1); \
		sed -i "/CONFIG_BLK_DEV_RAM\>/d" $(1); \
		echo "CONFIG_BLK_DEV_RAM=y" >>$(1); \
		echo "CONFIG_BLK_DEV_RAM_COUNT=1" >>$(1); \
		echo "CONFIG_BLK_DEV_RAM_SIZE=16384" >>$(1); \
	fi; \
	if [ "$(DUMP_OOPS_MSG)" = "y" ]; then \
		echo "CONFIG_DUMP_PREV_OOPS_MSG=y" >>$(1); \
		echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_ADDR=0x4C800000" >>$(1); \
		echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_LEN=0x2000" >>$(1); \
	else \
		sed -i "/CONFIG_DUMP_PREV_OOPS_MSG/d" $(1); \
		echo "# CONFIG_DUMP_PREV_OOPS_MSG is not set" >>$(1); \
	fi; \
	if [ "$(NFCM)" = "y" ]; then \
		sed -i "/CONFIG_MTK_HNAT_FORCE_CT_ACCOUNTING/d" $(1); \
		echo "CONFIG_MTK_HNAT_FORCE_CT_ACCOUNTING=y" >>$(1); \
	else \
		sed -i "/CONFIG_MTK_HNAT_FORCE_CT_ACCOUNTING/d" $(1); \
		echo "# CONFIG_MTK_HNAT_FORCE_CT_ACCOUNTING is not set" >>$(1); \
	fi; \
	if [ "$(WIREGUARD)" = "y" ]; then \
		echo "CONFIG_ARM64_CRYPTO=y" >>$(1); \
		echo "CONFIG_CRYPTO_SHA256_ARM64=y" >>$(1); \
		echo "CONFIG_CRYPTO_SHA512_ARM64=y" >>$(1); \
		echo "CONFIG_CRYPTO_SHA1_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_SHA2_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_SHA512_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_SHA3_ARM64=y" >>$(1); \
		echo "CONFIG_CRYPTO_SM3_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_SM4_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_GHASH_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_AES_ARM64=y" >>$(1); \
		echo "CONFIG_CRYPTO_AES_ARM64_CE=y" >>$(1); \
		echo "CONFIG_CRYPTO_AES_ARM64_CE_CCM=y" >>$(1); \
		echo "CONFIG_CRYPTO_AES_ARM64_CE_BLK=y" >>$(1); \
		echo "CONFIG_CRYPTO_AES_ARM64_NEON_BLK=y" >>$(1); \
		echo "CONFIG_CRYPTO_CHACHA20_NEON=y" >>$(1); \
		echo "CONFIG_CRYPTO_POLY1305_NEON=y" >>$(1); \
		echo "CONFIG_CRYPTO_NHPOLY1305_NEON=y" >>$(1); \
		echo "CONFIG_CRYPTO_AES_ARM64_BS=y" >>$(1); \
	fi; \
	if [ "$(TS_UI)" = "y" ]; then \
		sed -i "/CONFIG_OVERLAY_FS\>/d" $(1); \
		echo "CONFIG_OVERLAY_FS=y" >>$(1); \
		sed -i "/CONFIG_OVERLAY_FS_REDIRECT_DIR\>/d" $(1); \
		echo "# CONFIG_OVERLAY_FS_REDIRECT_DIR is not set" >>$(1); \
		sed -i "/CONFIG_OVERLAY_FS_REDIRECT_ALWAYS_FOLLOW\>/d" $(1); \
		echo "# CONFIG_OVERLAY_FS_REDIRECT_ALWAYS_FOLLOW is not set" >>$(1); \
		sed -i "/CONFIG_OVERLAY_FS_INDEX\>/d" $(1); \
		echo "# CONFIG_OVERLAY_FS_INDEX is not set" >>$(1); \
		sed -i "/CONFIG_OVERLAY_FS_XINO_AUTO\>/d" $(1); \
		echo "# CONFIG_OVERLAY_FS_XINO_AUTO is not set" >>$(1); \
		sed -i "/CONFIG_OVERLAY_FS_METACOPY\>/d" $(1); \
		echo "# CONFIG_OVERLAY_FS_METACOPY is not set" >>$(1); \
	fi; \
	if [ "$(SWRTMESH)" = "y" ]; then \
		sed -i "/CONFIG_CFG80211_SUPPORT/d" $(1); \
		echo "CONFIG_CFG80211_SUPPORT=y" >>$(1); \
		sed -i "/CONFIG_ENTERPRISE_AP_SUPPORT/d" $(1); \
		echo "CONFIG_ENTERPRISE_AP_SUPPORT=y" >>$(1); \
		echo "CONFIG_CUSTOMISED_HOSTAPD_SUPPORT=y" >>$(1); \
		echo "CONFIG_HOSTAPD_WPA3_SUPPORT=y" >>$(1); \
		echo "CONFIG_HOSTAPD_WPA3R3_SUPPORT=y" >>$(1); \
		echo "CONFIG_APCLI_STA_SUPPORT=y" >>$(1); \
		echo "CONFIG_WDS_STA_SUPPORT=y" >>$(1); \
		sed -i "/CONFIG_MBSS_AS_WDS_AP_SUPPORT/d" $(1); \
		echo "CONFIG_MBSS_AS_WDS_AP_SUPPORT=y" >>$(1); \
		echo "CONFIG_IWCOMMAND_CFG80211_SUPPORT=y" >>$(1); \
		echo "# CONFIG_DYNAMIC_VLAN_SUPPORT is not set" >>$(1); \
		echo "CONFIG_RADIUS_MAC_AUTH_SUPPORT=y" >>$(1); \
		echo "# CONFIG_VLAN_GTK_SUPPORT is not set" >>$(1); \
		echo "CONFIG_MTK_IAP_VENDOR1_FEATURE_SUPPORT=y" >>$(1); \
		sed -i "/CONFIG_WPA3_SUPPORT/d" $(1); \
		echo "# CONFIG_WPA3_SUPPORT is not set" >>$(1); \
		sed -i "/CONFIG_OWE_SUPPORT/d" $(1); \
		echo "# CONFIG_OWE_SUPPORT is not set" >>$(1); \
		sed -i "/CONFIG_MBO_SUPPORT/d" $(1); \
		echo "# CONFIG_MBO_SUPPORT is not set" >>$(1); \
		sed -i "/CONFIG_OCE_SUPPORT/d" $(1); \
		echo "# CONFIG_OCE_SUPPORT is not set" >>$(1); \
		sed -i "/CONFIG_QOS_R1_SUPPORT/d" $(1); \
		echo "# CONFIG_QOS_R1_SUPPORT is not set" >>$(1); \
		sed -i "/CONFIG_CFG80211\>/d" $(1); \
		echo "CONFIG_CFG80211=y" >>$(1); \
		echo "# CONFIG_MAC80211 is not set" >>$(1); \
		echo "# CONFIG_USB_NET_RNDIS_WLAN is not set" >>$(1); \
		echo "# CONFIG_PRISM2_USB is not set" >>$(1); \
		echo "CONFIG_NL80211_TESTMODE=y" >>$(1); \
		echo "# CONFIG_CFG80211_CERTIFICATION_ONUS is not set" >>$(1); \
		echo "CONFIG_CFG80211_CRDA_SUPPORT=y" >>$(1); \
		echo "# CONFIG_CFG80211_DEBUGFS is not set" >>$(1); \
		echo "CONFIG_CFG80211_DEFAULT_PS=y" >>$(1); \
		echo "# CONFIG_CFG80211_DEVELOPER_WARNINGS is not set" >>$(1); \
		echo "CONFIG_CFG80211_REQUIRE_SIGNED_REGDB=y" >>$(1); \
		echo "CONFIG_CFG80211_USE_KERNEL_REGDB_KEYS=y" >>$(1); \
		echo "# CONFIG_CFG80211_WEXT is not set" >>$(1); \
		echo "# CONFIG_VIRT_WIFI is not set" >>$(1); \
		echo "# CONFIG_RTL8723BS is not set" >>$(1); \
		echo "# CONFIG_R8188EU is not set" >>$(1); \
		echo "# CONFIG_WILC1000_SDIO is not set" >>$(1); \
		echo "# CONFIG_WILC1000_SPI is not set" >>$(1); \
		echo "# CONFIG_PKCS8_PRIVATE_KEY_PARSER is not set" >>$(1); \
		echo "# CONFIG_PKCS7_TEST_KEY is not set" >>$(1); \
		echo "# CONFIG_SYSTEM_EXTRA_CERTIFICATE is not set" >>$(1); \
		echo "# CONFIG_SECONDARY_TRUSTED_KEYRING is not set" >>$(1); \
		echo "# CONFIG_SIGNED_PE_FILE_VERIFICATION is not set" >>$(1); \
		echo "CONFIG_SYSTEM_TRUSTED_KEYS=\"\"" >>$(1); \
	fi; \
	)
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)

