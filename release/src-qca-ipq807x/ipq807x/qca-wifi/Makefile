include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)

DRIVER_PATH:=(SDK_DIR_PLATFORM)/qca-wifi
# legacy and hawkeye header files.
FW_INCLUDE_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/include
FW_COMMON_INCLUDE_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/wlan/fwcommon/include
FW_INTERFACE_TOOLS_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/wlan/fwcommon/fw_interface/tools/
FW_HDR_INCLUDE_PATH:=$(PKG_BUILD_DIR)/cmn_dev/fw_hdr
HALPHY_TOOLS_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/header/wlan/halphy_tools

ifeq ($(CONFIG_TARGET_ipq_ipq807x_QSDK_Deluxe),y)
export COPY_6290_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq_ipq60xx)$(CONFIG_TARGET_ipq_ipq60xx_64)$(CONFIG_TARGET_ipq60xx),y)
export ENABLE_6018_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq_ipq50xx)$(CONFIG_TARGET_ipq_ipq50xx_64)$(CONFIG_TARGET_ipq50xx),y)
export ENABLE_QCA5018_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq95xx),y)
export ENABLE_QCA9574_HEADER:=1
endif

ifeq ($(CONFIG_TARGET_ipq53xx),y)
export ENABLE_QCA5332_HEADER:=1
endif

ifeq ($(CONFIG_WHUNT),y)
export CONFIG_WHUNT=1
export QCA_SINGLE_WIFI_3_0=1
ifeq ($(CONFIG_LINUX_5_4),y)
export AHB_FW_SIM=0
export PCIE_FW_SIM=1
else
export AHB_FW_SIM=1
export PCIE_FW_SIM=0
endif
endif

export DRIVER_PATH
-include $(DRIVER_PATH)/config.mk
ifeq ($(CONFIG_LINUX_5_4),y)
export QCA_WIFI_KERNEL_VERSION_5_4=1
endif
ifneq ($(CONFIG_TARGET_ipq)$(CONFIG_TARGET_ipq806x),)
	QCA_PLATFORM="ipq806x"
else
	QCA_PLATFORM="$(CONFIG_TARGET_ARCH_PACKAGES)"
endif

QCAWLAN_MODULE_NAME:= $(notdir $(basename $(strip $(QCAWLAN_MODULE_LIST))))

ifeq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), unified-profile-nonss unified-profile custc-profile))
ifeq ($(CONFIG_TARGET_ipq95xx)$(CONFIG_TARGET_ipq53xx),y)
 QCA_PPE_VP_ENABLED:=1
 QCA_PPE_DS_ENABLED:=1
 export QCA_PPE_VP_ENABLED
 export QCA_PPE_DS_ENABLED
endif
endif

ifneq ($(CONFIG_TARGET_ipq95xx),y)
ifneq ($(CONFIG_TARGET_ipq53xx),y)
ifneq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), unified-profile-nonss lowmem-profile-nonss))
ifeq ($(CONFIG_TARGET_BOARD),$(filter $(CONFIG_TARGET_BOARD),ipq806x ipq807x ipq807x_64 generic ipq807x_32 ipq60xx ipq60xx_64 ipq60xx_32 ipq50xx ipq50xx_64 ipq50xx_32))
 QCA_NSS_ENABLED:=1
 export QCA_NSS_ENABLED
endif
endif
endif
endif

ifeq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), wkk-profile))
 QCA_NSS_ENABLED:=0
 export QCA_NSS_ENABLED
 QCA_PPE_VP_ENABLED:=1
 export QCA_PPE_VP_ENABLED
endif

export CONFIG_CNSS2_MODULE="$(CONFIG_CNSS2_MODULE)"
export CONFIG_WIFI_TARGET_WIFI_2_0="$(CONFIG_WIFI_TARGET_WIFI_2_0)"
export CONFIG_WIFI_TARGET_WIFI_3_0="$(CONFIG_WIFI_TARGET_WIFI_3_0)"
export FW_INCLUDE_PATH="$(FW_INCLUDE_PATH)"
export FW_COMMON_INCLUDE_PATH="$(FW_COMMON_INCLUDE_PATH)"
export FW_INTERFACE_TOOLS_PATH="$(FW_INTERFACE_TOOLS_PATH)"
export FW_HDR_INCLUDE_PATH="$(FW_HDR_INCLUDE_PATH)"
export HALPHY_TOOLS_PATH="$(HALPHY_TOOLS_PATH)"
export MAKE="$(MAKE)"
export LINUX_VERSION="$(LINUX_VERSION)"
export LINUX_DIR="$(LINUXDIR)"
export LINUX_SRC_DIR="$(LINUXDIR)"
export ARCH="$(ARCH)"
export BUILD_VARIANT="unified-profile"
export TARGET_CROSS="$(CROSS_COMPILE)"
export QCA_PLATFORM="$(QCA_PLATFORM)"
export BIG_ENDIAN_HOST="$(BIG_ENDIAN_HOST)"
export TARGET_CFLAGS="$(TARGET_CFLAGS)"
export TARGET_LDFLAGS="$(TARGET_LDFLAGS) -lpthread -ldl"
export CONFIG_WIFI_MEM_DEBUG="$(CONFIG_WIFI_MEM_DEBUG)"
export CONFIG_WLAN_REF_ID_TRACE="$(CONFIG_WLAN_REF_ID_TRACE)"
export CONFIG_WIFI_CE_TASKLET_DEBUG="$(CONFIG_WIFI_CE_TASKLET_DEBUG)"
export CONFIG_WIFI_LOG_UTILS="$(CONFIG_WIFI_LOG_UTILS)"
export CONFIG_WIFI_VDEV_PEER_PROT_COUNT="$(CONFIG_WIFI_VDEV_PEER_PROT_COUNT)"
export CONFIG_WIFI_VDEV_PEER_PROT_TESTING="$(CONFIG_WIFI_VDEV_PEER_PROT_TESTING)"
export CONFIG_WIFI_EMULATION_WIFI_3_0="$(CONFIG_WIFI_EMULATION_WIFI_3_0)"
export CONFIG_WIFI_IPQ_MEM_PROFILE="$(CONFIG_KERNEL_IPQ_MEM_PROFILE)"
export CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise="$(CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise)"
export CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise="$(CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise)"
export CONFIG_TARGET_ipq_ipq806x="$(CONFIG_TARGET_ipq_ipq806x)"
export CONFIG_TARGET_ipq_ipq40xx="$(CONFIG_TARGET_ipq_ipq40xx)"
export CONFIG_TARGET_ipq807x="$(CONFIG_TARGET_ipq807x)"
export CONFIG_TARGET_ipq95xx="$(CONFIG_TARGET_ipq95xx)"
export CONFIG_TARGET_ipq53xx="$(CONFIG_TARGET_ipq53xx)"
export CONFIG_TARGET_ipq60xx="$(CONFIG_TARGET_ipq60xx)"
export CONFIG_64BIT="$(CONFIG_64BIT)"
export CONFIG_KERNEL_KASAN="$(CONFIG_KERNEL_KASAN)"
export CONFIG_KERNEL_SKB_DEBUG="$(CONFIG_KERNEL_SKB_DEBUG)";
export CONFIG_KERNEL_SLUB_DEBUG="$(CONFIG_KERNEL_SLUB_DEBUG)"
export CONFIG_WLAN_IOT_SIM_SUPPORT="$(CONFIG_WLAN_IOT_SIM_SUPPORT)"
export CONFIG_LOWMEM_FLASH="$(CONFIG_LOWMEM_FLASH)"
export CONFIG_DEBUG="$(CONFIG_DEBUG)"
export CONFIG_WIFI_MONITOR_SUPPORT="$(CONFIG_WIFI_MONITOR_SUPPORT)"
export CONFIG_WIFI_11BE_D3_0_SUPPORT="$(CONFIG_WIFI_11BE_D3_0_SUPPORT)"

QCAWIFI_KMOD := asf.ko ath_hal.ko ath_spectral.ko qca_da.ko smart_antenna.ko ath_dev.ko ath_pktlog.ko hst_tx99.ko qca_ol.ko tm.ko
QCAWIFI_KMOD += ath_dfs.ko ath_rate_atheros.ko mem_manager.ko qdf.ko umac.ko
QCAWIFI_LIBS := libwlanstats.so libtlvtemplate.so libtlvutil.so libtlvcmdrsp.so libtlvencoder.so libtlvparser.so
QCAWIFI_USR_SBIN := 80211stats athstats athstatsclr apstats pktlogconf pktlogdump wifitool wlanconfig thermaltool wps_enhc exttool assocdenialnotify athkey pstats
QCAWIFI_USR_SBIN += radartool spectraltool athadhoc ssidsteering tx99tool athtestcmd wnm-app
#QCAWIFI_USR_SBIN += dumpregs reg 

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-wifi/wow/*.c),)
	ln -sf (SDK_DIR_PLATFORM)/qca-wifi source
	$(CP) $(LINUXDIR)/include/uapi/linux/nl80211.h $(STAGING_DIR)/usr/include/nl80211_copy.h
	$(CP) -f $(DRIVER_PATH)/component_dev/tools/linux/cfg80211_ven_cmd.h $(STAGING_DIR)/usr/include
ifeq ($(CONFIG_CNSS2_MODULE),y)
	$(CP) -f $(STAGING_DIR)/usr/include/qca-cnss/cnss2.h $(STAGING_DIR)/usr/include
endif
	$(MAKE) -C $(PKG_BUILD_DIR) src_prepare all_build
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libqca_tools.so $(STAGING_DIR)/usr/lib/
ifeq ($(BUILD_VARIANT),$(filter $(BUILD_VARIANT), unified-profile unified-profile-nonss))
	[ ! -f $(PKG_BUILD_DIR)/os/linux/tools/libstats_lib.so ] || $(CP) $(PKG_BUILD_DIR)/os/linux/tools/libstats_lib.so $(STAGING_DIR)/usr/lib/
endif
ifneq ($(CONFIG_LOWMEM_FLASH),y)
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_htt.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libapstats.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwifistats.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_wmi.so $(STAGING_DIR)/usr/lib/
else
ifeq ($(CONFIG_DEBUG),y)
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_htt.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libapstats.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwifistats.so $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/os/linux/tools/libwlanfw_wmi.so $(STAGING_DIR)/usr/lib/
endif
endif
else

endif

install:
	@[ -d $(INSTALLDIR)/ini ] || install -d $(INSTALLDIR)/ini
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/lib/wifi ] || install -d $(INSTALLDIR)/lib/wifi
	@[ -d $(INSTALLDIR)/data/vendor ] || install -d $(INSTALLDIR)/data/vendor
	@[ -d $(INSTALLDIR)/sbin ] || install -d $(INSTALLDIR)/sbin
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-wifi/wow/*.c),)
	@install -m 755 -D $(PKG_BUILD_DIR)/cfg80211_configs/cfg80211tool $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/cfg80211_configs/qcacommands_ol_radio.xml $(INSTALLDIR)/lib/wifi
	@install -m 755 -D $(PKG_BUILD_DIR)/cfg80211_configs/qcacommands_vap.xml $(INSTALLDIR)/lib/wifi
	@install -m 755 -D $(PKG_BUILD_DIR)/umac/son/cfg80211_configs/qcacommands_mesh.xml $(INSTALLDIR)/lib/wifi
	ln -s /usr/sbin/cfg80211tool $(1)/usr/sbin/cfg80211tool_mesh
	$(MAKE) -C $(PKG_BUILD_DIR) QCAWLAN_TOOL_LIST="$(QCAWLAN_TOOL_LIST)" INSTALL_BIN_DEST="$(INSTALLDIR)/usr/sbin" INSTALL_LIB_DEST="$(INSTALLDIR)/usr/lib" tools_installonly
else
	@install $(foreach f,$(QCAWIFI_KMOD),$(f)) $(INSTALLKMODDIR)
	@install $(foreach f,$(QCAWIFI_USR_SBIN),$(f)) $(INSTALLDIR)/usr/sbin
	@install $(foreach f,$(QCAWIFI_LIBS),$(f)) $(INSTALLDIR)/usr/lib
	@install $(foreach f,$(QCAWIFI_EXEC),$(f)) $(INSTALLDIR)/sbin
	@install $(foreach f,$(QCAWIFI_XML),$(f)) $(INSTALLDIR)/lib/wifi
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko
	@$(STRIP) $(INSTALLDIR)/usr/lib/*
	@$(STRIP) $(INSTALLDIR)/usr/sbin/*
	@$(STRIP) $(foreach f,$(filter-out %/cfg80211tool,$(QCAWIFI_EXEC)),$(f))
endif
	@ln -sf /tmp/wifi $(INSTALLDIR)/data/vendor/wifi
	@cd prebuilt/ini && tar cvf - . | tar xvf - -C $(INSTALLDIR)/ini

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-wifi/wow/*.c),)
	[ -d $(STAGEDIR)/usr/include/qca-wifi ] || install -d $(STAGEDIR)/usr/include/qca-wifi
	$(MAKE) -C $(PKG_BUILD_DIR) INSTALL_DEST="$(STAGING_DIR)/usr/include/" install_headers
	$(CP) $(LINUXDIR)/include/uapi/linux/nl80211.h $(STAGING_DIR)/usr/include/nl80211_copy.h
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_mscs/inc/qca_mscs_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_mesh_latency/inc/qca_mesh_latency_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_sawf/qca_sawf_if.h $(STAGING_DIR)/usr/include/qca-wifi/qca_sawf_if.h
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_scs/inc/qca_scs_if.h $(STAGING_DIR)/usr/include/qca-wifi
	$(CP) -f $(DRIVER_PATH)/component_dev/qca_fse/qca_fse_if.h $(STAGING_DIR)/usr/include/qca-wifi
endif
