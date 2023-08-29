# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
TARGET_CROSS=$(CROSS_COMPILE)
TARGET_CC=$(CROSS_COMPILE)gcc

TARGET_CFLAGS += \
	-I$(LINUXDIR)/include/uapi/ \
	-I$(LINUXDIR)/include/

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-qmi-framework/qmi-framework.mk),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-qmi-framework source
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS=$(TARGET_CFLAGS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/qrtr
	mkdir -p $(PKG_BUILD_DIR)/lib
	cp -rf prebuilt/{qrtr-cfg,qrtr-ns,qrtr-lookup} $(PKG_BUILD_DIR)/qrtr
	cp -rf prebuilt/{*.so} $(PKG_BUILD_DIR)/qrtr $(PKG_BUILD_DIR)/lib
endif

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/qrtr/qrtr-cfg $(INSTALLDIR)/usr/sbin/
	install -m 755 -D $(PKG_BUILD_DIR)/qrtr/qrtr-ns $(INSTALLDIR)/usr/sbin/
	install -m 755 -D $(PKG_BUILD_DIR)/qrtr/qrtr-lookup $(INSTALLDIR)/usr/sbin/
	install -m 755 -D $(PKG_BUILD_DIR)/lib/libqmi_common_so.so $(1)/usr/lib/
	install -m 755 -D $(PKG_BUILD_DIR)/lib/libqmi_cci.so $(1)/usr/lib/
	install -m 755 -D $(PKG_BUILD_DIR)/lib/libqmi_qrtr_cci.so $(1)/usr/lib/
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qrtr-cfg
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qrtr-ns
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qrtr-lookup
	@$(STRIP) $(INSTALLDIR)/usr/lib/libqmi_common_so.so
	@$(STRIP) $(INSTALLDIR)/usr/lib/libqmi_cci.so
	@$(STRIP) $(INSTALLDIR)/usr/lib/libqmi_qrtr_cci.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-qmi-framework/qmi-framework.mk),)
	cp -rf $(PKG_BUILD_DIR)/inc/*.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/qcci/inc/*.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/common/inc/*.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/lib/*.so $(STAGING_DIR)/usr/lib/
endif
