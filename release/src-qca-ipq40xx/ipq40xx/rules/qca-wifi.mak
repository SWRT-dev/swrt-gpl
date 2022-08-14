INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
QCAWIFI_KMOD=asf.ko ath_pktlog.ko mem_manager.ko qca_ol.ko qca_spectral.ko qdf.ko smart_antenna.ko umac.ko wifi_3_0.ko wifi_2_0.ko
QCAWIFI_LIBS=libwifistats.so libapstats.so libwlanfw_htt.so libqca_tools.so libtlvtemplate.so libtlvutil.so libtlvcmdrsp.so libtlvencoder.so libtlvparser.so
QCAWIFI_USR_SBIN=80211stats athstats athstatsclr apstats pktlogconf pktlogdump wifitool wlanconfig thermaltool wps_enhc exttool assocdenialnotify athkey
QCAWIFI_USR_SBIN+=qca_gensock radartool spectraltool athadhoc ssidsteering tx99tool dumpregs wifistats acsdbgtool qldtool cfr_test_app peerratestats athtestcmd

export CONFIG_WIFI_TARGET_WIFI_2_0:=y
export CONFIG_WIFI_TARGET_WIFI_3_0:=y
export FW_INCLUDE_PATH="$(SRCBASE)/$(PLATFORM_ROUTER)/dl/include"
export FW_COMMON_INCLUDE_PATH="$(SRCBASE)/$(PLATFORM_ROUTER)/dl/wlan/fwcommon/include"
export FW_INTERFACE_TOOLS_PATH="$(SRCBASE)/$(PLATFORM_ROUTER)/dl/wlan/fwcommon/fw_interface/tools/"
export FW_HDR_INCLUDE_PATH="$(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi/cmn_dev/fw_hdr"
export HALPHY_TOOLS_PATH="$(SRCBASE)/$(PLATFORM_ROUTER)/dl/wlan/halphy_tools"
export LINUX_VERSION="$(LINUX_KERNEL)"
export LINUX_DIR="$(LINUXDIR)"
export LINUX_SRC_DIR="$(LINUXDIR)"
export BUILD_VARIANT=""
export TARGET_CROSS="$(CROSS_COMPILE)"
export QCA_PLATFORM="ipq40xx"
export BIG_ENDIAN_HOST="0"
export TARGET_CFLAGS="$(CFLAGS)"
export TARGET_LDFLAGS="-lpthread -ldl"
export CONFIG_WIFI_MEM_DEBUG=""
export CONFIG_WLAN_REF_ID_TRACE=""
export CONFIG_WIFI_CE_TASKLET_DEBUG=""
export CONFIG_WIFI_LOG_UTILS="y"
export CONFIG_WIFI_VDEV_PEER_PROT_COUNT=""
export CONFIG_WIFI_VDEV_PEER_PROT_TESTING=""
export CONFIG_WIFI_EMULATION_WIFI_3_0=""
export CONFIG_WIFI_IPQ_MEM_PROFILE="0"
export CONFIG_TARGET_ipq_ipq807x_64_QSDK_Enterprise=""
export CONFIG_TARGET_ipq_ipq807x_QSDK_Enterprise=""
export CONFIG_TARGET_ipq_ipq806x=""
export CONFIG_TARGET_ipq_ipq40xx:=y
export CONFIG_64BIT:=n
export CONFIG_KERNEL_KASAN=""
export CONFIG_KERNEL_SKB_DEBUG=""
export CONFIG_KERNEL_SLUB_DEBUG=""
export CONFIG_WLAN_IOT_SIM_SUPPORT=""
export CONFIG_LOWMEM_FLASH=""
export CONFIG_DEBUG=""


qca-wifi-InstallDev:
ifneq ($(wildcard qca-wifi/Makefile),)
	install -d $(STAGEDIR)/usr/include
	install -d  $(STAGEDIR)/usr/include/qca-wifi
	$(MAKE) -C qca-wifi INSTALL_DEST="$(STAGEDIR)/usr/include/" install_headers
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap/src/drivers/nl80211_copy.h $(STAGEDIR)/usr/include
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi/component_dev/qca_mscs/inc/qca_mscs_if.h $(STAGEDIR)/usr/include/qca-wifi
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi/component_dev/qca_mesh_latency/inc/qca_mesh_latency_if.h $(STAGEDIR)/usr/include/qca-wifi
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-wifi/component_dev/tools/linux/cfg80211_ven_cmd.h $(STAGEDIR)/usr/include
endif

qca-wifi:
ifneq ($(wildcard qca-wifi/Makefile),)
	$(MAKE) -C qca-wifi src_prepare all_build
	qca-wifi-InstallDev
endif

qca-wifi-install:
ifneq ($(wildcard qca-wifi/Makefile),)
	install -D qca-wifi/asf/asf.ko $(INSTALLKMODDIR)/asf.ko
	install -D qca-wifi/lmac/ath_pktlog/ath_pktlog.ko $(INSTALLKMODDIR)/ath_pktlog.ko
	install -D qca-wifi/os/linux/mem/mem_manager.ko $(INSTALLKMODDIR)/mem_manager.ko
	install -D qca-wifi/qca_ol/qca_ol.ko $(INSTALLKMODDIR)/qca_ol.ko
	install -D qca-wifi/cmn_dev/spectral/qca_spectral.ko $(INSTALLKMODDIR)/qca_spectral.ko
	install -D qca-wifi/cmn_dev/qdf/qdf.ko $(INSTALLKMODDIR)/qdf.ko
	install -D qca-wifi/smartantenna/smart_antenna.ko $(INSTALLKMODDIR)/smart_antenna.ko
	install -D qca-wifi/umac/umac.ko $(INSTALLKMODDIR)/umac.ko
	install -D qca-wifi/qca_ol/wifi3.0/wifi_3_0.ko $(INSTALLKMODDIR)/wifi_3_0.ko
	install -D qca-wifi/qca_ol/wifi2.0/wifi_2_0.ko $(INSTALLKMODDIR)/wifi_2_0.ko
	install -D qca-wifi/os/linux/tools/80211stats $(INSTALLDIR)/qca-wifi/usr/sbin/80211stats
	install -D qca-wifi/os/linux/tools/athstats $(INSTALLDIR)/qca-wifi/usr/sbin/athstats
	install -D qca-wifi/os/linux/tools/athstatsclr $(INSTALLDIR)/qca-wifi/usr/sbin/athstatsclr
	install -D qca-wifi/os/linux/tools/apstats $(INSTALLDIR)/qca-wifi/usr/sbin/apstats
	install -D qca-wifi/os/linux/tools/pktlogconf $(INSTALLDIR)/qca-wifi/usr/sbin/pktlogconf
	install -D qca-wifi/os/linux/tools/pktlogdump $(INSTALLDIR)/qca-wifi/usr/sbin/pktlogdump
	install -D qca-wifi/os/linux/tools/wifitool $(INSTALLDIR)/qca-wifi/usr/sbin/wifitool
	install -D qca-wifi/os/linux/tools/wlanconfig $(INSTALLDIR)/qca-wifi/usr/sbin/wlanconfig
	install -D qca-wifi/os/linux/tools/thermaltool $(INSTALLDIR)/qca-wifi/usr/sbin/thermaltool
	install -D qca-wifi/os/linux/tools/wps_enhc $(INSTALLDIR)/qca-wifi/usr/sbin/wps_enhc
	install -D qca-wifi/os/linux/tools/exttool $(INSTALLDIR)/qca-wifi/usr/sbin/exttool
	install -D qca-wifi/os/linux/tools/assocdenialnotify $(INSTALLDIR)/qca-wifi/usr/sbin/assocdenialnotify
	install -D qca-wifi/os/linux/tools/athkey $(INSTALLDIR)/qca-wifi/usr/sbin/athkey
	install -D qca-wifi/os/linux/tools/qca_gensock $(INSTALLDIR)/qca-wifi/usr/sbin/qca_gensock
	install -D qca-wifi/os/linux/tools/radartool $(INSTALLDIR)/qca-wifi/usr/sbin/radartool
	install -D qca-wifi/os/linux/tools/spectraltool $(INSTALLDIR)/qca-wifi/usr/sbin/spectraltool
	install -D qca-wifi/os/linux/tools/athadhoc $(INSTALLDIR)/qca-wifi/usr/sbin/athadhoc
	install -D qca-wifi/os/linux/tools/ssidsteering $(INSTALLDIR)/qca-wifi/usr/sbin/ssidsteering
	install -D qca-wifi/os/linux/tools/tx99tool $(INSTALLDIR)/qca-wifi/usr/sbin/tx99tool
	install -D qca-wifi/os/linux/tools/dumpregs $(INSTALLDIR)/qca-wifi/usr/sbin/dumpregs
	install -D qca-wifi/os/linux/tools/wifistats $(INSTALLDIR)/qca-wifi/usr/sbin/wifistats
	install -D qca-wifi/os/linux/tools/acsdbgtool $(INSTALLDIR)/qca-wifi/usr/sbin/acsdbgtool
	install -D qca-wifi/os/linux/tools/qldtool $(INSTALLDIR)/qca-wifi/usr/sbin/qldtool
	install -D qca-wifi/component_dev/tools/linux/cfr_test_app $(INSTALLDIR)/qca-wifi/usr/sbin/cfr_test_app
	install -D qca-wifi/component_dev/tools/linux/peerratestats $(INSTALLDIR)/qca-wifi/usr/sbin/peerratestats
	install -D qca-wifi/halphy_tools/host/testcmd_tlv/Linux/athtestcmd $(INSTALLDIR)/qca-wifi/usr/sbin/athtestcmd
	install -D qca-wifi/halphy_tools/host/testcmd_tlv/Linux/libtlvparser.so $(INSTALLDIR)/qca-wifi/usr/lib/libtlvparser.so
	install -D qca-wifi/halphy_tools/host/testcmd_tlv/Linux/libtlvencoder.so $(INSTALLDIR)/qca-wifi/usr/lib/libtlvencoder.so
	install -D qca-wifi/halphy_tools/host/testcmd_tlv/Linux/libtlvcmdrsp.so $(INSTALLDIR)/qca-wifi/usr/lib/libtlvcmdrsp.so
	install -D qca-wifi/halphy_tools/host/testcmd_tlv/Linux/libtlvutil.so $(INSTALLDIR)/qca-wifi/usr/lib/libtlvutil.so
	install -D qca-wifi/halphy_tools/host/testcmd_tlv/Linux/libtlvtemplate.so $(INSTALLDIR)/qca-wifi/usr/lib/libtlvtemplate.so
	install -D qca-wifi/os/linux/tools/libqca_tools.so $(INSTALLDIR)/qca-wifi/usr/lib/libqca_tools.so
	install -D qca-wifi/os/linux/tools/libwlanfw_htt.so $(INSTALLDIR)/qca-wifi/usr/lib/libwlanfw_htt.so
	install -D qca-wifi/os/linux/tools/libapstats.so $(INSTALLDIR)/qca-wifi/usr/lib/libapstats.so
	install -D qca-wifi/os/linux/tools/libwifistats.so $(INSTALLDIR)/qca-wifi/usr/lib/libwifistats.so
else
	@for i in $(QCAWIFI_KMOD); do install -D qca-wifi/prebuild/$$i $(INSTALLKMODDIR) ; done
	@for i in $(QCAWIFI_LIBS); do install -D qca-wifi/prebuild/$$i $(INSTALLDIR)/usr/lib ; done
	@for i in $(QCAWIFI_USR_SBIN); do install -D qca-wifi/prebuild/$$i $(INSTALLDIR)/usr/sbin ; done
endif
	$(STRIP) $(INSTALLDIR)/usr/sbin/*
	$(STRIP) $(INSTALLDIR)/usr/lib/*
	$(STRIPX) $(INSTALLKMODDIR)/*.ko

qca-wifi-clean:
ifneq ($(wildcard qca-wifi/Makefile),)
	$(MAKE) -C qca-wifi clean
endif

