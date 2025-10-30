# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS+= -Wno-attributes

QCASSDK_CONFIG_OPTS+= TOOL_PATH=$(TOOLS)/bin/ \
	SYS_PATH=$(LINUXDIR) \
	TOOLPREFIX=$(TARGET_CROSS) \
	KVER=$(LINUX_VERSION) \
	ARCH=$(ARCH) \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
	TARGET_CPPFLAGS="$(TARGET_CPPFLAGS)"

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-diag/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-diag source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/ $(QCASSDK_CONFIG_OPTS) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/lib
	mkdir -p $(PKG_BUILD_DIR)/sbin
	$(CP) prebuilt/$(BUILD_NAME)/libdiag.so $(PKG_BUILD_DIR)/lib
	$(CP) prebuilt/$(BUILD_NAME)/diag_socket_app $(PKG_BUILD_DIR)/sbin
	$(CP) prebuilt/$(BUILD_NAME)/diag_stress_app $(PKG_BUILD_DIR)/sbin
	$(CP) prebuilt/$(BUILD_NAME)/registerReboot $(PKG_BUILD_DIR)/sbin
	$(CP) prebuilt/$(BUILD_NAME)/qld_server $(PKG_BUILD_DIR)/sbin
	$(CP) prebuilt/$(BUILD_NAME)/diag_mdlog $(PKG_BUILD_DIR)/sbin
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/lib/libdiag.so $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/bin/diag_socket_app $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/bin/diag_stress_app $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/bin/registerReboot $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/bin/qld_server $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/bin/diag_mdlog $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/lib/libdiag.so
	@$(STRIP) $(INSTALLDIR)/usr/sbin/diag_socket_app
	@$(STRIP) $(INSTALLDIR)/usr/sbin/diag_stress_app
	@$(STRIP) $(INSTALLDIR)/usr/sbin/registerReboot
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qld_server
	@$(STRIP) $(INSTALLDIR)/usr/sbin/diag_mdlog

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-diag/*.c),)
	[ -d $(STAGEDIR)/usr/include/qca-diag ] || install -d $(STAGEDIR)/usr/include/qca-diag
	$(CP) $(PKG_BUILD_DIR)/include/* $(STAGING_DIR)/usr/include/qca-diag/
	$(CP) $(PKG_BUILD_DIR)/lib/libdiag.so $(STAGING_DIR)/usr/lib
endif

