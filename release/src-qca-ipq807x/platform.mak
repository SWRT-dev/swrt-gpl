
SWITCH_CHIP_ID_POOL =					\
	"QCA8075"				\
	"QCA8075_PHY_AQR107"				\
	"QCA8075_PHY_AQR111"				\
	"QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033"

# related path to platform specific software package
export PLATFORM_ROUTER := ipq807x
#export LOADADDR := 42208000
export LOADADDR := 41080000
export ENTRYADDR := $(LOADADDR)
export LINUXDIR := $(SRCBASE)/linux/linux-5.4.x
export SDKDIR := $(SDK_DIR_PLATFORM)
export BUILD := $(shell (gcc -dumpmachine))
export KERNEL_BINARY=$(LINUXDIR)/vmlinux
export PLATFORM := arm-musl
export TOOLS := /opt/toolchain-aarch64_cortex-a53+neon-vfpv4_gcc-8.3.0_musl
export CROSS_COMPILE := $(TOOLS)/bin/aarch64-openwrt-linux-musl-
export READELF := $(TOOLS)/bin/aarch64-openwrt-linux-musl-readelf
#export RTVER := $(patsubst librt-%.so,%,$(shell basename $(wildcard $(TOOLS)/lib/librt-*.so)))
export CROSS_COMPILER := $(CROSS_COMPILE)
export CONFIGURE := ./configure --host=aarch64-openwrt-linux --build=$(BUILD)
export HOSTCONFIG := linux-aarch64
export ARCH := arm64
export HOST := aarch64-linux
export KERNELCC := $(TOOLS)/bin/aarch64-openwrt-linux-musl-gcc
export KERNELLD := $(TOOLS)/bin/aarch64-openwrt-linux-musl-ld
export KARCH := $(firstword $(subst -, ,$(shell $(KERNELCC) -dumpmachine)))
#export DTB := "qcom/$(DTB)"

EXTRACFLAGS += -DBCMWPA2 -fno-delete-null-pointer-checks -march=armv8-a -mcpu=cortex-a53+crypto
export EXTRACFLAGS

# OpenWRT's toolchain needs STAGING_DIR environment variable that points to top directory of toolchain.
export STAGING_DIR=$(TOOLS)

EXTRA_CFLAGS := -DLINUX26 -DCONFIG_QCA -pipe -DDEBUG_NOISY -DDEBUG_RCTEST #-mfpu=neon-vfpv4 -mfloat-abi=softfp
EXTRA_CFLAGS += -D_GNU_SOURCE -D_BSD_SOURCE

export CONFIG_LINUX26=y
export CONFIG_QCA=y

EXTRA_CFLAGS += -DLINUX30
export CONFIG_LINUX30=y

define platformRouterOptions
	@( \
	sed -i "/RTCONFIG_QCA\>/d" $(1); \
	echo "RTCONFIG_QCA=y" >>$(1); \
	sed -i "/RTCONFIG_SPF12_2_QSDK/d" $(1); \
	echo "RTCONFIG_SPF12_2_QSDK=y" >>$(1); \
	sed -i "/RTCONFIG_CFG80211\>/d" $(1); \
	echo "RTCONFIG_CFG80211=y" >>$(1); \
	sed -i "/RTCONFIG_GLOBAL_INI\>/d" $(1); \
	echo "RTCONFIG_GLOBAL_INI=y" >>$(1); \
	if [ "$(DUAL_TRX)" = "y" ] ; then \
		sed -i "/RTCONFIG_DUAL_TRX2/d" $(1); \
		echo "RTCONFIG_DUAL_TRX2=y" >>$(1); \
	else \
		sed -i "/RTCONFIG_DUAL_TRX2/d" $(1); \
		echo "# RTCONFIG_DUAL_TRX2 is not set" >>$(1); \
	fi; \
	sed -i "/RTCONFIG_SOC_IPQ8074/d" $(1); \
	echo "RTCONFIG_SOC_IPQ8074=y" >>$(1); \
	sed -i "/RTCONFIG_SINGLE_HOSTAPD\>/d" $(1); \
	echo "RTCONFIG_SINGLE_HOSTAPD=y" >>$(1); \
	sed -i "/RTCONFIG_MFP\>/d" $(1); \
	echo "RTCONFIG_MFP=y" >>$(1); \
	sed -i "/RTCONFIG_QCA_ARM/d" $(1); \
	echo "RTCONFIG_QCA_ARM=y" >>$(1); \
	sed -i "/RTCONFIG_32BYTES_ODMPID/d" $(1); \
	echo "RTCONFIG_32BYTES_ODMPID=y" >>$(1); \
	if [ "$(RTAX89U)" = y ] ; then \
		sed -i "/RTCONFIG_FITFDT/d" $(1); \
		echo "# RTCONFIG_FITFDT is not set" >>$(1); \
	else \
		sed -i "/RTCONFIG_FITFDT/d" $(1); \
		echo "RTCONFIG_FITFDT=y" >>$(1); \
	fi; \
	if [ "$(WIFI_CHIP)" = "QCN50X4" ] ; then \
		sed -i "/RTCONFIG_WIFI_QCN5024_QCN5054/d" $(1); \
		echo "RTCONFIG_WIFI_QCN5024_QCN5054=y" >>$(1); \
		sed -i "/RTCONFIG_VHT80_80/d" $(1); \
		echo "# RTCONFIG_VHT80_80 is not set" >>$(1); \
		sed -i "/RTCONFIG_QCA_BIGRATE_WIFI\>/d" $(1); \
		echo "RTCONFIG_QCA_BIGRATE_WIFI=y" >>$(1); \
	fi; \
	sed -i "/RTCONFIG_VHT160/d" $(1); \
	if [ "$(BW160M)" = "y" ]; then \
		echo "RTCONFIG_VHT160=y" >>$(1); \
		echo "RTCONFIG_BW160M=y" >>$(1); \
	else \
		echo "# RTCONFIG_VHT160 is not set" >>$(1); \
		echo "# RTCONFIG_BW160M is not set" >>$(1); \
	fi; \
	for chip in $(SWITCH_CHIP_ID_POOL) ; do \
		sed -i "/RTCONFIG_SWITCH_$${chip}\>/d" $(1); \
		if [ "$(SWITCH_CHIP)" = "$${chip}" ] ; then \
			echo "RTCONFIG_SWITCH_$${chip}=y" >> $(1); \
		else \
			echo "# RTCONFIG_SWITCH_$${chip} is not set" >> $(1); \
		fi; \
	done; \
	if [ "$(TEST_BDF)" = "y" ]; then \
		sed -i "/RTCONFIG_TEST_BOARDDATA_FILE/d" $(1); \
		echo "RTCONFIG_TEST_BOARDDATA_FILE=y" >>$(1); \
	fi; \
	if [ "$(NSS_IPSEC)" = "y" ]; then \
		sed -i "/RTCONFIG_NSS_IPSEC/d" $(1); \
		echo "RTCONFIG_NSS_IPSEC=y" >>$(1); \
	fi; \
	sed -i "/RTCONFIG_MUSL_LIBC/d" $(1); \
	echo "RTCONFIG_MUSL_LIBC=y" >>$(1); \
	echo "# RTCONFIG_MUSL_LIBC_1116_WAR is not set" >>$(1); \
	)
endef

define platformBusyboxOptions
	@( \
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
	)
endef

define platformKernelConfig
	@( \
	sed -i "/CONFIG_BRIDGE_NETFILTER/d" $(1); \
	echo "CONFIG_BRIDGE_NETFILTER=y" >>$(1); \
	sed -i "/CONFIG_NETFILTER_XT_TARGET_TPROXY/d" $(1); \
	echo "CONFIG_NETFILTER_XT_TARGET_TPROXY=m" >>$(1); \
	sed -i "/CONFIG_USB_LIBCOMPOSITE/d" $(1); \
	echo "CONFIG_USB_LIBCOMPOSITE=m" >>$(1); \
	sed -i "/CONFIG_USB_CONFIGFS\>/d" $(1); \
	echo "CONFIG_USB_CONFIGFS=m" >>$(1); \
	sed -i "/CONFIG_USB_CONFIGFS_F_DIAG/d" $(1); \
	echo "CONFIG_USB_CONFIGFS_F_DIAG=y" >>$(1); \
	sed -i "/CONFIG_USB_F_DIAG/d" $(1); \
	echo "CONFIG_USB_F_DIAG=m" >>$(1); \
	sed -i "/CONFIG_DIAG_CHAR/d" $(1); \
	echo "CONFIG_DIAG_CHAR=m" >>$(1); \
	sed -i "/CONFIG_NET_PTP_CLASSIFY/d" $(1); \
	echo "CONFIG_NET_PTP_CLASSIFY=y" >>$(1); \
	sed -i "/CONFIG_CFG80211\>/d" $(1); \
	echo "CONFIG_CFG80211=y" >>$(1); \
	sed -i "/CONFIG_CFG80211_DEVELOPER_WARNINGS/d" $(1); \
	echo "CONFIG_CFG80211_DEVELOPER_WARNINGS=y" >>$(1); \
	sed -i "/CONFIG_CFG80211_DEBUGFS/d" $(1); \
	echo "CONFIG_CFG80211_DEBUGFS=y" >>$(1); \
	sed -i "/CONFIG_CFG80211_DEBUGFS/d" $(1); \
	echo "CONFIG_CFG80211_DEBUGFS=y" >>$(1); \
	sed -i "/CONFIG_CFG80211_INTERNAL_REGDB/d" $(1); \
	echo "CONFIG_CFG80211_INTERNAL_REGDB=y" >>$(1); \
	sed -i "/CONFIG_CFG80211_CRDA_SUPPORT/d" $(1); \
	echo "# CONFIG_CFG80211_CRDA_SUPPORT is not set" >>$(1); \
	sed -i "/CONFIG_CFG80211_WEXT/d" $(1); \
	echo "CONFIG_CFG80211_WEXT=y" >>$(1); \
	sed -i "/CONFIG_PPS\>/d" $(1); \
	echo "CONFIG_PPS=m" >>$(1); \
	sed -i "/CONFIG_PTP_1588_CLOCK/d" $(1); \
	echo "CONFIG_PTP_1588_CLOCK=m" >>$(1); \
	sed -i "/CONFIG_PINCTRL_IPQ807x/d" $(1); \
	echo "CONFIG_PINCTRL_IPQ807x=y" >>$(1); \
	sed -i "/CONFIG_IPQ_ADSS_807x/d" $(1); \
	echo "CONFIG_IPQ_ADSS_807x=y" >>$(1); \
	sed -i "/CONFIG_IPQ_APSS_807x/d" $(1); \
	echo "CONFIG_IPQ_APSS_807x=y" >>$(1); \
	sed -i "/CONFIG_IPQ_GCC_807x/d" $(1); \
	echo "CONFIG_IPQ_GCC_807x=y" >>$(1); \
	sed -i "/CONFIG_PHY_QCA_PCIE_QMP/d" $(1); \
	echo "CONFIG_PHY_QCA_PCIE_QMP=y" >>$(1); \
	sed -i "/CONFIG_CRYPTO_CCM/d" $(1); \
	echo "CONFIG_CRYPTO_CCM=y" >>$(1); \
	sed -i "/CONFIG_CRYPTO_DEV_QCE\>/d" $(1); \
	echo "# CONFIG_CRYPTO_DEV_QCE is not set" >>$(1); \
	sed -i "/CONFIG_REGULATOR_CPR3\>/d" $(1); \
	echo "CONFIG_REGULATOR_CPR3=y" >>$(1); \
	sed -i "/CONFIG_REGULATOR_CPR3_NPU\>/d" $(1); \
	echo "CONFIG_REGULATOR_CPR3_NPU=y" >>$(1); \
	sed -i "/CONFIG_REGULATOR_CPR4_APSS\>/d" $(1); \
	echo "CONFIG_REGULATOR_CPR4_APSS=y" >>$(1); \
	sed -i "/CONFIG_VMSPLIT_3G>/d" $(1); \
	echo "# CONFIG_VMSPLIT_3G is not set" >>$(1); \
	sed -i "/CONFIG_VMSPLIT_2G\>/d" $(1); \
	echo "CONFIG_VMSPLIT_2G=y" >>$(1); \
	sed -i "/CONFIG_PRINTK_TIME\>/d" $(1); \
	echo "CONFIG_PRINTK_TIME=y" >>$(1); \
	sed -i "/CONFIG_DEBUG_INFO\>/d" $(1); \
	echo "CONFIG_DEBUG_INFO=y" >>$(1); \
	if [ "$(FANCTRL)" = "y" ] ; then \
		sed -i "/CONFIG_HWMON\>/d" $(1); \
		echo "CONFIG_HWMON=y" >>$(1); \
		sed -i "/CONFIG_SENSORS_GPIO_FAN\>/d" $(1); \
		echo "CONFIG_SENSORS_GPIO_FAN=y" >>$(1); \
		sed -i "/CONFIG_THERMAL_HWMON\>/d" $(1); \
		echo "CONFIG_THERMAL_HWMON=y" >>$(1); \
	fi; \
	sed -i "/CONFIG_ARCH_WANT_KMAP_ATOMIC_FLUSH\>/d" $(1); \
	echo "CONFIG_ARCH_WANT_KMAP_ATOMIC_FLUSH=y" >>$(1); \
	sed -i "/CONFIG_QCOM_DCC\>/d" $(1); \
	echo "CONFIG_QCOM_DCC=y" >>$(1); \
	sed -i "/CONFIG_KRAITCC\>/d" $(1); \
	echo "# CONFIG_KRAITCC is not set" >>$(1); \
	sed -i "/CONFIG_KRAIT_CLOCKS\>/d" $(1); \
	echo "# CONFIG_KRAIT_CLOCKS is not set" >>$(1); \
	sed -i "/CONFIG_KRAIT_L2_ACCESSORS\>/d" $(1); \
	echo "# CONFIG_KRAIT_L2_ACCESSORS is not set" >>$(1); \
	sed -i "/CONFIG_PCIEPORTBUS\>/d" $(1); \
	echo "# CONFIG_PCIEPORTBUS is not set" >>$(1); \
	sed -i "/CONFIG_PCIE_PME\>/d" $(1); \
	echo "# CONFIG_PCIE_PME is not set" >>$(1); \
	sed -i "/CONFIG_GENERIC_CPUFREQ_KRAIT\>/d" $(1); \
	echo "# CONFIG_GENERIC_CPUFREQ_KRAIT is not set" >>$(1); \
	sed -i "/CONFIG_REGMAP_ALLOW_WRITE_DEBUGFS\>/d" $(1); \
	echo "CONFIG_REGMAP_ALLOW_WRITE_DEBUGFS=y" >>$(1); \
	sed -i "/CONFIG_REGMAP_SPI\>/d" $(1); \
	echo "CONFIG_REGMAP_SPI=y" >>$(1); \
	sed -i "/CONFIG_SPS\>/d" $(1); \
	echo "CONFIG_SPS=y" >>$(1); \
	sed -i "/CONFIG_SPS_SUPPORT_NDP_BAM\>/d" $(1); \
	echo "CONFIG_SPS_SUPPORT_NDP_BAM=y" >>$(1); \
	sed -i "/CONFIG_BLK_DEV_NVME\>/d" $(1); \
	echo "CONFIG_BLK_DEV_NVME=y" >>$(1); \
	sed -i "/CONFIG_RELAY\>/d" $(1); \
	echo "CONFIG_RELAY=y" >>$(1); \
	sed -i "/CONFIG_MD\>/d" $(1); \
	echo "CONFIG_MD=y" >>$(1); \
	sed -i "/CONFIG_BLK_DEV_DM\>/d" $(1); \
	echo "CONFIG_BLK_DEV_DM=y" >>$(1); \
	sed -i "/CONFIG_DM_CRYPT\>/d" $(1); \
	echo "CONFIG_DM_CRYPT=m" >>$(1); \
	sed -i "/CONFIG_DM_REQ_CRYPT\>/d" $(1); \
	echo "CONFIG_DM_REQ_CRYPT=m" >>$(1); \
	sed -i "/CONFIG_DM_MIRROR\>/d" $(1); \
	echo "CONFIG_DM_MIRROR=m" >>$(1); \
	sed -i "/CONFIG_PTP_1588_CLOCK\>/d" $(1); \
	echo "CONFIG_PTP_1588_CLOCK=y" >>$(1); \
	sed -i "/CONFIG_REGULATOR_RPM_GLINK\>/d" $(1); \
	echo "CONFIG_REGULATOR_RPM_GLINK=y" >>$(1); \
	sed -i "/CONFIG_MMC_SDHCI_MSM_ICE\>/d" $(1); \
	echo "CONFIG_MMC_SDHCI_MSM_ICE=y" >>$(1); \
	sed -i "/CONFIG_CRYPTO_DEV_QCOM_ICE\>/d" $(1); \
	echo "CONFIG_CRYPTO_DEV_QCOM_ICE=y" >>$(1); \
	sed -i "/CONFIG_WANT_DEV_COREDUMP\>/d" $(1); \
	echo "CONFIG_WANT_DEV_COREDUMP=y" >>$(1); \
	sed -i "/CONFIG_USB_F_QDSS\>/d" $(1); \
	echo "CONFIG_USB_F_QDSS=m" >>$(1); \
	sed -i "/CONFIG_USB_CONFIGFS_F_QDSS\>/d" $(1); \
	echo "CONFIG_USB_CONFIGFS_F_QDSS=y" >>$(1); \
	sed -i "/CONFIG_USB_BAM\>/d" $(1); \
	echo "CONFIG_USB_BAM=y" >>$(1); \
	sed -i "/CONFIG_CNSS2_GENL\>/d" $(1); \
	echo "CONFIG_CNSS2_GENL=y" >>$(1); \
	sed -i "/CONFIG_QCA_MINIDUMP\>/d" $(1); \
	echo "CONFIG_QCA_MINIDUMP=y" >>$(1); \
	sed -i "/CONFIG_MAILBOX\>/d" $(1); \
	echo "CONFIG_MAILBOX=y" >>$(1); \
	sed -i "/CONFIG_ARM_KERNMEM_PERMS\>/d" $(1); \
	echo "# CONFIG_ARM_KERNMEM_PERMS is not set" >>$(1); \
	sed -i "/CONFIG_DEBUG_RODATA\>/d" $(1); \
	echo "# CONFIG_DEBUG_RODATA is not set" >>$(1); \
	sed -i "/CONFIG_RMNET_DATA\>/d" $(1); \
	echo "CONFIG_RMNET_DATA=y" >>$(1); \
	sed -i "/CONFIG_RMNET_DATA_DEBUG_PKT\>/d" $(1); \
	echo "CONFIG_RMNET_DATA_DEBUG_PKT=y" >>$(1); \
	sed -i "/CONFIG_MHI_BUS\>/d" $(1); \
	echo "CONFIG_MHI_BUS=y" >>$(1); \
	sed -i "/CONFIG_MHI_DEBUG\>/d" $(1); \
	echo "CONFIG_MHI_DEBUG=y" >>$(1); \
	sed -i "/CONFIG_MHI_QTI\>/d" $(1); \
	echo "CONFIG_MHI_QTI=y" >>$(1); \
	sed -i "/CONFIG_MHI_NETDEV\>/d" $(1); \
	echo "CONFIG_MHI_NETDEV=y" >>$(1); \
	sed -i "/CONFIG_MHI_UCI\>/d" $(1); \
	echo "CONFIG_MHI_UCI=y" >>$(1); \
	sed -i "/CONFIG_QCOM_QMI_HELPERS\>/d" $(1); \
	echo "CONFIG_QCOM_QMI_HELPERS=y" >>$(1); \
	sed -i "/CONFIG_DEBUG_SET_MODULE_RONX\>/d" $(1); \
	echo "# CONFIG_DEBUG_SET_MODULE_RONX is not set" >>$(1); \
	sed -i "/CONFIG_PHY_QCOM_DWC3\>/d" $(1); \
	echo "# CONFIG_PHY_QCOM_DWC3 is not set" >>$(1); \
	sed -i "/CONFIG_CRYPTO_MICHAEL_MIC\>/d" $(1); \
	echo "CONFIG_CRYPTO_MICHAEL_MIC=y" >>$(1); \
	sed -i "/CONFIG_NET_VENDOR_QUALCOMM\>/d" $(1); \
	echo "CONFIG_NET_VENDOR_QUALCOMM=y" >>$(1); \
	sed -i "/CONFIG_RMNET\>/d" $(1); \
	echo "CONFIG_RMNET=y" >>$(1); \
	sed -i "/CONFIG_NET_L3_MASTER_DEV\>/d" $(1); \
	echo "CONFIG_NET_L3_MASTER_DEV=y" >>$(1); \
	sed -i "/CONFIG_HAVE_GCC_PLUGINS\>/d" $(1); \
	echo "CONFIG_HAVE_GCC_PLUGINS=y" >>$(1); \
	sed -i "/CONFIG_IPC_ROUTER\>/d" $(1); \
	echo "# CONFIG_IPC_ROUTER is not set" >>$(1); \
	sed -i "/CONFIG_IPQ807X_REMOTEPROC\>/d" $(1); \
	echo "# CONFIG_IPQ807X_REMOTEPROC is not set" >>$(1); \
	sed -i "/CONFIG_IPQ_REMOTEPROC_ADSP\>/d" $(1); \
	echo "# CONFIG_IPQ_REMOTEPROC_ADSP is not set" >>$(1); \
	sed -i "/CONFIG_MSM_GLINK\>/d" $(1); \
	echo "# CONFIG_MSM_GLINK is not set" >>$(1); \
	sed -i "/CONFIG_MSM_GLINK_SMEM_NATIVE_XPRT\>/d" $(1); \
	echo "# CONFIG_MSM_GLINK_SMEM_NATIVE_XPRT is not set" >>$(1); \
	sed -i "/CONFIG_MSM_GLINK_PKT\>/d" $(1); \
	echo "# CONFIG_MSM_GLINK_PKT is not set" >>$(1); \
	sed -i "/CONFIG_MSM_IPC_ROUTER_GLINK_XPRT\>/d" $(1); \
	echo "# CONFIG_MSM_IPC_ROUTER_GLINK_XPRT is not set" >>$(1); \
	sed -i "/CONFIG_MSM_QMI_INTERFACE\>/d" $(1); \
	echo "# CONFIG_MSM_QMI_INTERFACE is not set" >>$(1); \
	sed -i "/CONFIG_MSM_TEST_QMI_CLIENT\>/d" $(1); \
	echo "# CONFIG_MSM_TEST_QMI_CLIENT is not set" >>$(1); \
	sed -i "/CONFIG_MSM_RPM_GLINK\>/d" $(1); \
	echo "# CONFIG_MSM_RPM_GLINK is not set" >>$(1); \
	sed -i "/CONFIG_IPQ_SUBSYSTEM_RAMDUMP\>/d" $(1); \
	echo "# CONFIG_IPQ_SUBSYSTEM_RAMDUMP is not set" >>$(1); \
	sed -i "/CONFIG_DIAG_OVER_QRTR\>/d" $(1); \
	echo "CONFIG_DIAG_OVER_QRTR=y" >>$(1); \
	sed -i "/CONFIG_DIAGFWD_BRIDGE_CODE\>/d" $(1); \
	echo "CONFIG_DIAGFWD_BRIDGE_CODE=y" >>$(1); \
	sed -i "/CONFIG_QCOM_APCS_IPC\>/d" $(1); \
	echo "CONFIG_QCOM_APCS_IPC=y" >>$(1); \
	sed -i "/CONFIG_RPMSG\>/d" $(1); \
	echo "CONFIG_RPMSG=y" >>$(1); \
	sed -i "/CONFIG_RPMSG_CHAR\>/d" $(1); \
	echo "CONFIG_RPMSG_CHAR=y" >>$(1); \
	sed -i "/CONFIG_RPMSG_QCOM_GLINK_NATIVE\>/d" $(1); \
	echo "CONFIG_RPMSG_QCOM_GLINK_NATIVE=y" >>$(1); \
	sed -i "/CONFIG_RPMSG_QCOM_GLINK_RPM\>/d" $(1); \
	echo "CONFIG_RPMSG_QCOM_GLINK_RPM=y" >>$(1); \
	sed -i "/CONFIG_RPMSG_QCOM_GLINK_SMEM\>/d" $(1); \
	echo "CONFIG_RPMSG_QCOM_GLINK_SMEM=y" >>$(1); \
	sed -i "/CONFIG_RPMSG_QCOM_SMD\>/d" $(1); \
	echo "CONFIG_RPMSG_QCOM_SMD=y" >>$(1); \
	sed -i "/CONFIG_MSM_RPM_RPMSG\>/d" $(1); \
	echo "CONFIG_MSM_RPM_RPMSG=y" >>$(1); \
	sed -i "/CONFIG_QRTR\>/d" $(1); \
	echo "CONFIG_QRTR=y" >>$(1); \
	sed -i "/CONFIG_QRTR_SMD\>/d" $(1); \
	echo "CONFIG_QRTR_SMD=y" >>$(1); \
	sed -i "/CONFIG_QRTR_MHI\>/d" $(1); \
	echo "CONFIG_QRTR_MHI=y" >>$(1); \
	sed -i "/CONFIG_QCOM_RPROC_COMMON\>/d" $(1); \
	echo "CONFIG_QCOM_RPROC_COMMON=y" >>$(1); \
	sed -i "/CONFIG_QCOM_Q6V5_COMMON\>/d" $(1); \
	echo "CONFIG_QCOM_Q6V5_COMMON=y" >>$(1); \
	sed -i "/CONFIG_QCOM_Q6V5_ADSP\>/d" $(1); \
	echo "CONFIG_QCOM_Q6V5_ADSP=y" >>$(1); \
	sed -i "/CONFIG_QCOM_Q6V5_WCSS\>/d" $(1); \
	echo "CONFIG_QCOM_Q6V5_WCSS=y" >>$(1); \
	sed -i "/CONFIG_QCOM_SYSMON\>/d" $(1); \
	echo "CONFIG_QCOM_SYSMON=y" >>$(1); \
	sed -i "/CONFIG_QCOM_GLINK_SSR\>/d" $(1); \
	echo "CONFIG_QCOM_GLINK_SSR=y" >>$(1); \
	sed -i "/CONFIG_QCOM_MDT_LOADER\>/d" $(1); \
	echo "CONFIG_QCOM_MDT_LOADER=y" >>$(1); \
	sed -i "/CONFIG_SAMPLES\>/d" $(1); \
	echo "CONFIG_SAMPLES=y" >>$(1); \
	sed -i "/CONFIG_SAMPLE_QMI_CLIENT\>/d" $(1); \
	echo "CONFIG_SAMPLE_QMI_CLIENT=m" >>$(1); \
	sed -i "/CONFIG_CORESIGHT_STREAM\>/d" $(1); \
	echo "CONFIG_CORESIGHT_STREAM=m" >>$(1); \
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
	if [ "$(RTAX89U)" = "y" ] ; then \
		sed -i "/CONFIG_MODEL_RTAX89U/d" $(1); \
		echo "CONFIG_MODEL_RTAX89U=y" >>$(1); \
		sed -i "/CONFIG_IPQ_MEM_PROFILE/d" $(1); \
		echo "CONFIG_IPQ_MEM_PROFILE=1024" >>$(1); \
	elif [ "$(RAX120)" = "y" ] ; then \
		sed -i "/CONFIG_MODEL_RAX120/d" $(1); \
		echo "CONFIG_MODEL_RAX120=y" >>$(1); \
		sed -i "/CONFIG_IPQ_MEM_PROFILE/d" $(1); \
		echo "CONFIG_IPQ_MEM_PROFILE=1024" >>$(1); \
	elif [ "$(XMAX3600)" = "y" ] ; then \
		sed -i "/CONFIG_MODEL_XMAX3600/d" $(1); \
		echo "CONFIG_MODEL_XMAX3600=y" >>$(1); \
	fi; \
	if [ "$(SWITCH_CHIP)" = "QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033" ] || \
	   [ "$(SWITCH_CHIP)" = "QCA8075_PHY_AQR107" ] || \
	   [ "$(SWITCH_CHIP)" = "QCA8075" ] || \
	   [ "$(SWITCH_CHIP)" = "QCA8075_PHY_AQR111" ] ; then \
		sed -i "/CONFIG_AQUANTIA_PHY/d" $(1); \
		echo "CONFIG_AQUANTIA_PHY=y" >>$(1); \
	fi; \
	for chip in $(SWITCH_CHIP_ID_POOL) ; do \
		sed -i "/CONFIG_SWITCH_$${chip}\>/d" $(1); \
		if [ "$(SWITCH_CHIP)" = "$${chip}" ] ; then \
			echo "CONFIG_SWITCH_$${chip}=y" >> $(1); \
		else \
			echo "# CONFIG_SWITCH_$${chip} is not set" >> $(1); \
		fi; \
	done; \
	if [ "$(M2_SSD)" = "y" ]; then \
		sed -i "/CONFIG_SATA_AHCI_PLATFORM/d" $(1); \
		echo "CONFIG_SATA_AHCI_PLATFORM=y" >>$(1); \
		sed -i "/CONFIG_SATA_AHCI_MSM/d" $(1); \
		echo "CONFIG_SATA_AHCI_MSM=y" >>$(1); \
		sed -i "/CONFIG_SATA_SNPS_PHY/d" $(1); \
		echo "CONFIG_SATA_SNPS_PHY=y" >>$(1); \
		sed -i "/CONFIG_ATA_SFF/d" $(1); \
		echo "CONFIG_ATA_SFF=y" >>$(1); \
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
		if [ "$(WIFI_CHIP)" = "QCN50X4" ]; then \
			sed -i "/CONFIG_FACTORY_NR_LEB/d" $(1); \
			echo "CONFIG_FACTORY_NR_LEB=3" >>$(1); \
		fi; \
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
			echo "# CONFIG_UBIFS_FS_ZSTD is not set" >>$(1); \
			echo "# CONFIG_UBIFS_FS_AUTHENTICATION is not set" >>$(1); \
		else \
			sed -i "/CONFIG_UBIFS_FS/d" $(1); \
			echo "# CONFIG_UBIFS_FS is not set" >>$(1); \
		fi; \
		if [ "$(DUMP_OOPS_MSG)" = "y" ]; then \
			echo "CONFIG_DUMP_PREV_OOPS_MSG=y" >>$(1); \
			echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_ADDR=0x45300000" >>$(1); \
			echo "CONFIG_DUMP_PREV_OOPS_MSG_BUF_LEN=0x2000" >>$(1); \
		fi; \
		if [ "$(IPV6SUPP)" = "y" ]; then \
			if [ "$(IPQ806X)" = "y" -o "$(IPQ807X)" = "y" ]; then \
				sed -i "/CONFIG_IPV6_MULTIPLE_TABLES is not set/d" $(1); \
				echo "# CONFIG_IPV6_MULTIPLE_TABLES is not set" >>$(1); \
			fi; \
		fi; \
	fi; \
	if [ "$(WIREGUARD)" = "y" ]; then \
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
	if [ "$(SWRT_FULLCONE)" = "y" ]; then \
		sed -i "/CONFIG_SWRT_FULLCONE/d" $(1); \
		echo "CONFIG_SWRT_FULLCONE=y" >>$(1); \
	else \
		sed -i "/CONFIG_SWRT_FULLCONE/d" $(1); \
		echo "# CONFIG_SWRT_FULLCONE is not set" >>$(1); \
	fi; \
	if [ "$(RAX120)" = y ] ; then \
		sed -i "/CONFIG_CMDLINE_OVERRIDE/d" $(1); \
		echo "CONFIG_CMDLINE_OVERRIDE=y" >>$(1); \
		sed -i "/CONFIG_BLK_DEV_RAM_SIZE/d" $(1); \
		echo "CONFIG_BLK_DEV_RAM_SIZE=65536" >>$(1); \
		sed -i "/CONFIG_OCF_CRYPTODEV/d" $(1); \
		echo "# CONFIG_OCF_CRYPTODEV is not set" >>$(1); \
		sed -i "/CONFIG_SPI_QUP/d" $(1); \
		echo "CONFIG_SPI_QUP=y" >>$(1); \
		sed -i "/CONFIG_SENSORS_G761/d" $(1); \
		echo "CONFIG_SENSORS_G761=y" >>$(1); \
	elif [ "$(XMAX3600)" = "y" ] ; then \
		sed -i "/CONFIG_CMDLINE_OVERRIDE/d" $(1); \
		echo "CONFIG_CMDLINE_OVERRIDE=y" >>$(1); \
		sed -i "/CONFIG_BLK_DEV_RAM_SIZE/d" $(1); \
		echo "CONFIG_BLK_DEV_RAM_SIZE=65536" >>$(1); \
		sed -i "/CONFIG_OCF_CRYPTODEV/d" $(1); \
		echo "# CONFIG_OCF_CRYPTODEV is not set" >>$(1); \
		sed -i "/CONFIG_SPI_QUP/d" $(1); \
		echo "CONFIG_SPI_QUP=y" >>$(1); \
	fi; \
	)
endef

# $1: $(TARGETDIR)
# $2: Top dir of kernel modules. $(TARGETDIR)/lib/modules/x.y.z
define platformGen_Target
endef

export PARALLEL_BUILD := -j$(shell grep -c '^processor' /proc/cpuinfo)
