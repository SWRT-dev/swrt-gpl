# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source
subtarget:=$(CONFIG_TARGET_BOARD)

TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -pie

WIFSONEXTLIB_MAKEOPTS:= \
	TOOLPREFIX=$(TARGET_CROSS) \
	GWINCLUDE=$(STAGING_DIR)/usr/include/qca/ \
	QCACFLAGS="$(TARGET_CFLAGS)"\
	QCALDFLAGS="-Wl,--gc-sections $(TARGET_LDFLAGS)"\
	EXTRA_CFLAGS=$(STAGING_DIR)/usr/include/libnl \
	EXTRA_CFLAGS=$(STAGING_DIR)/usr/include/libnl3 \
	EXTRA_LDFLAGS="-lnl-3 -lnl-genl-3"\
	INSTALL_ROOT=$(PKG_BUILD_DIR)/install

all:

ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-wifison-ext-lib/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-wifison-ext-lib source
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib/
	mkdir -p $(PKG_BUILD_DIR)/install/include/
	mkdir -p $(PKG_BUILD_DIR)/install/sbin
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR)/ $(strip $(WIFSONEXTLIB_MAKEOPTS)) $(TARGET_CONFIGURE_OPTS)
	$(CP) $(PKG_BUILD_DIR)/libqca_wifison_ext.so  $(STAGING_DIR)/usr/lib/
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR)/samplecode $(strip $(WIFSONEXTLIB_MAKEOPTS)) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib/
	mkdir -p $(PKG_BUILD_DIR)/install/include/
	mkdir -p $(PKG_BUILD_DIR)/install/sbin
	$(CP) prebuild/$(BUILD_NAME)/libqca_wifison_ext.so $(PKG_BUILD_DIR)/install/lib/libqca_wifison_ext.so
	$(CP) prebuild/$(BUILD_NAME)/qca_event_sample $(PKG_BUILD_DIR)/install/sbin/qca_event_sample
	$(CP) prebuild/$(BUILD_NAME)/qca_listen_port $(PKG_BUILD_DIR)/install/sbin/qca_listen_port
	$(CP) prebuild/$(BUILD_NAME)/qca_genlevent_sample $(PKG_BUILD_DIR)/install/sbin/qca_genlevent_sample
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/install/lib/libqca_wifison_ext.so $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/install/sbin/qca_event_sample $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/install/sbin/qca_listen_port $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/install/sbin/qca_genlevent_sample $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/lib/libqca_wifison_ext.so
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qca_event_sample
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qca_listen_port
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qca_genlevent_sample

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-wifison-ext-lib/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-wifison-ext-lib
	$(CP) $(PKG_BUILD_DIR)/install/include/wifison_event.h $(STAGING_DIR)/usr/include/qca-wifison-ext-lib
	$(CP) $(PKG_BUILD_DIR)/install/lib/libqca_wifison_ext.so  $(STAGING_DIR)/usr/lib/
endif

