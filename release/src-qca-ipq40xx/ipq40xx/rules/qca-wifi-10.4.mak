QCAWIFI_KMOD := asf.ko ath_hal.ko ath_spectral.ko qca_da.ko smart_antenna.ko ath_dev.ko ath_pktlog.ko hst_tx99.ko qca_ol.ko tm.ko
QCAWIFI_KMOD += ath_dfs.ko ath_rate_atheros.ko mem_manager.ko qdf.ko umac.ko
QCAWIFI_LIBS := libwlanstats.so libtlvtemplate.so libtlvutil.so libtlvcmdrsp.so libtlvencoder.so libtlvparser.so
QCAWIFI_USR_SBIN := 80211stats athstats athstatsclr apstats pktlogconf pktlogdump wifitool wlanconfig thermaltool wps_enhc exttool assocdenialnotify athkey pstats
QCAWIFI_USR_SBIN += radartool spectraltool athadhoc ssidsteering tx99tool athtestcmd wnm-app
#QCAWIFI_USR_SBIN += dumpregs reg 

export PKG_BUILD_DIR:=$(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi-10.4
export DRIVER_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi-10.4
export FW_INCLUDE_PATH:=$(STAGEDIR)/usr/src/qca-wifi/include
export FW_COMMON_INCLUDE_PATH:=$(STAGEDIR)/usr/src/qca-wifi/wlan/fwcommon/include
export FW_INTERFACE_TOOLS_PATH:=$(STAGEDIR)/usr/src/qca-wifi/wlan/fwcommon/fw_interface/tools/
export FW_HDR_INCLUDE_PATH:=$(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi-10.4/cmn_dev/fw_hdr
export HALPHY_TOOLS_PATH:=$(STAGEDIR)/usr/src/qca-wifi/wlan/halphy_tools
export LINUX_VERSION:=$(LINUX_KERNEL)
export LINUX_DIR:=$(LINUXDIR)
export LINUX_SRC_DIR:=$(LINUXDIR)
export BUILD_VARIANT:=unified-perf
export TARGET_CROSS:=$(CROSS_COMPILE)
export TARGET_CFLAGS:=$(EXTRA_CFLAGS) -I$(STAGEDIR)/usr/include/ -I$(TOP)/kernel_header/include/ -DRTCONFIG_MUSL_LIBC
export TARGET_LDFLAGS:= -lpthread -ldl -L$(STAGEDIR)/usr/lib/
export QCA_NSS_WIFI_OFFLOAD_SUPPORT:=0
export CONFIG_WIFI_IPQ_MEM_PROFILE:=n
export CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise:=n
export CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise:=n
export BUILD_X86:=n
export LIMIT_RXBUF_LEN_4K:=0
export ATH_GEN_RANDOMNESS:=0
export DUMP_FW_RAM:=0
export ATH_SUPPORT_FW_RAM_DUMP_FOR_MIPS:=0
export QCA_TGT_FW_IRAM_DUMP_ENABLE:=0
export CONFIG_WIFI_TARGET_WIFI_2_0:=y
export CONFIG_WIFI_TARGET_WIFI_3_0:=y
export CONFIG_WIFI_MEM_DEBUG:=n
export CONFIG_WIFI_EMULATION_WIFI_3_0:=n
export CONFIG_WIFI_LOG_UTILS:=n
export CONFIG_PLD_STUB:=n
#export AH_DEBUG:=1
#export ATH_DEBUG:=1
#export ADF_OS_DEBUG:=1
export ENDIAN:=AH_LITTLE_ENDIAN
export BIG_ENDIAN_HOST:=0
export QCA_PLATFORM=ipq806x
-include $(DRIVER_PATH)/config.mk

qca-wifi-10.4-stage:
ifneq ($(wildcard qca-wifi-10.4/config.mk),)
	install -d $(STAGEDIR)/usr/include
	install -d  $(STAGEDIR)/usr/include/qca-wifi
	$(MAKE) -C qca-wifi-10.4 INSTALL_DEST="$(STAGEDIR)/usr/include/" install_headers
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/nl80211_copy.h $(STAGEDIR)/usr/include
endif

qca-wifi-10.4: qca-wifi-fw-10.4
ifneq ($(wildcard qca-wifi-10.4/config.mk),)
	[ -f qca-wifi-10.4/stamp-h1 ] || $(MAKE) -C qca-wifi-10.4 src_prepare all_build
	$(MAKE) qca-wifi-10.4-stage
	touch qca-wifi-10.4/stamp-h1
endif

qca-wifi-10.4-install:
ifneq ($(wildcard qca-wifi-10.4/config.mk),)
	install -d $(INSTALLDIR)/qca-wifi-10.4/lib/modules/$(LINUX_KERNEL)
	install -d $(INSTALLDIR)/qca-wifi-10.4/usr/sbin
	install -d $(INSTALLDIR)/qca-wifi-10.4/usr/lib
	$(MAKE) -C qca-wifi-10.4 QCAWLAN_TOOL_LIST="$(QCAWLAN_TOOL_LIST)" INSTALL_ROOT_DRV="$(INSTALLDIR)/qca-wifi-10.4/lib/modules/$(LINUX_KERNEL)" driver_installonly
	$(MAKE) -C qca-wifi-10.4 QCAWLAN_MODULE_LIST="$(QCAWLAN_MODULE_LIST)" INSTALL_BIN_DEST="$(INSTALLDIR)/qca-wifi-10.4/usr/sbin" INSTALL_LIB_DEST="$(INSTALLDIR)/qca-wifi-10.4/usr/lib" tools_installonly
	install -D qca-wifi-10.4/testmodule/tm.ko $(INSTALLDIR)/qca-wifi-10.4/lib/modules/$(LINUX_KERNEL)/tm.ko
	install -D qca-wifi-10.4/os/linux/tools/wnm-app $(INSTALLDIR)/qca-wifi-10.4/usr/sbin/wnm-app
else
	@for i in $(QCAWIFI_KMOD); do install -D qca-wifi-10.4/prebuild/$$i $(INSTALLDIR)/qca-wifi-10.4/lib/modules/$(LINUX_KERNEL)/$$i ; done
	@for i in $(QCAWIFI_LIBS); do install -D qca-wifi-10.4/prebuild/$$i $(INSTALLDIR)/qca-wifi-10.4/usr/lib/$$i ; done
	@for i in $(QCAWIFI_USR_SBIN); do install -D qca-wifi-10.4/prebuild/$$i $(INSTALLDIR)/qca-wifi-10.4/usr/sbin/$$i ; done
endif
	$(STRIP) $(INSTALLDIR)/qca-wifi-10.4/usr/sbin/*
	$(STRIP) $(INSTALLDIR)/qca-wifi-10.4/usr/lib/*
	$(STRIPX) $(INSTALLDIR)/qca-wifi-10.4/lib/modules/$(LINUX_KERNEL)/*.ko

qca-wifi-10.4-clean:
ifneq ($(wildcard qca-wifi-10.4/config.mk),)
	$(MAKE) -C qca-wifi-10.4 clean
	rm -f qca-wifi-10.4/stamp-h1
endif

