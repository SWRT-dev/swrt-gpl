# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source
subtarget:=$(CONFIG_TARGET_BOARD)

TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -pie

QCAWHCLBD_MAKE_OPTS:= \
		CROSS=$(TARGET_CROSS) \
		GWLIB=$(STAGING_DIR)/usr/lib \
		LBTOP=$(PKG_BUILD_DIR) \
		GWINCLUDE=$(STAGING_DIR)/usr/include/hyficommon \
		IEEE1905DIR=$(STAGING_DIR)/usr/include/ieee1905 \
		ATHDIR=$(STAGING_DIR)/usr/include \
		INSTALL_ROOT=$(PKG_BUILD_DIR)/install \
		QCACFLAGS="$(TARGET_CFLAGS)"\
		QCALDFLAGS="-Wl,--gc-sections $(TARGET_LDFLAGS)"\
		KERNELVERSION=$(LINUX_VERSION) \
		STAGING_DIR=$(STAGING_DIR) \
		ARCH="$(ARCH)"

QCAWHCLBD_MAKE_OPTS+=SONEVENTINCLUDE=$(STAGING_DIR)/usr/include/qca-wifison-ext-lib
QCAWHCLBD_MAKE_OPTS+=LBD_MODULE_SONEVENT=y
QCAWHCLBD_MAKE_OPTS+=LBD_SUPPORT_VHT160=y
QCAWHCLBD_MAKE_OPTS+=LBD_SUPPORT_CFG80211=y
QCAWHCLBD_MAKE_OPTS+=LBD_SUPPORT_QSDK=y
#QCAWHCLBD_MAKE_OPTS += CALLTRACE_SUPPORT=y
#QCAWHCLBD_MAKE_OPTS+= ENABLE_LBD_SUPPORT_11BE=y
#QCAWHCLBD_MAKE_OPTS+=SONMEMDEBUGINCLUDE=$(STAGING_DIR)/usr/include/son-mem-dbg
#QCAWHCLBD_MAKE_OPTS+=ENABLE_SON_MEMORY_DEBUG=y
QCAWHCLBD_MAKE_OPTS+=SWRT_PATCH=y



all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-whc-lbd/src/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-whc-lbd source
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib
	mkdir -p $(PKG_BUILD_DIR)/install/include
	mkdir -p $(PKG_BUILD_DIR)/install/sbin
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCAWHCLBD_MAKE_OPTS)) subdirs $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib/
	mkdir -p $(PKG_BUILD_DIR)/install/sbin
	cp prebuilt/$(BUILD_NAME)/liblbcmnlibs.so $(PKG_BUILD_DIR)/install/lib/liblbcmnlibs.so
	cp prebuilt/$(BUILD_NAME)/lbd $(PKG_BUILD_DIR)/install/sbin/lbd
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/install/lib/liblbcmnlibs.so $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/install/sbin/lbd $(INSTALLDIR)/usr/sbin
	install -m 755 -D lbt $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/lib/liblbcmnlibs.so
	@$(STRIP) $(INSTALLDIR)/usr/sbin/lbd

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-whc-lbd/src/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/bandmon.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/bandmonCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/diaglog.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/estimator.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/estimatorCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/estimatorRCPIToPhyRate.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/estimatorInterferenceDetectionCurve.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/estimatorPollutionAccumulator.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/lb_assert.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/lb_common.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/lb_module.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/lb_profileSections.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/lbd_assert.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/lbd_types.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/persist.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/stadbEntry.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/stadb.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/stadbCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/stamon.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/stamonCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/steeralg.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/steeralgCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/steerexec.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/steerexecImplCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanif.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanifBSteerControl.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanifBSteerControlCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanifBSteerEvents.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanifLinkEvents.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanifLinkEventsCmn.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/include/soneventService.h $(STAGING_DIR)/usr/include/whc-wlb
	$(CP) $(PKG_BUILD_DIR)/install/lib/liblbcmnlibs.so $(STAGING_DIR)/usr/lib
endif

