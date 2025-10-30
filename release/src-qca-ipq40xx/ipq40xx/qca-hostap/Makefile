# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CPPFLAGS += \
	-I$(PKG_BUILD_DIR)/src -I$(PKG_BUILD_DIR)/src/utils \
	-I$(PKG_BUILD_DIR)/src/drivers
TARGET_CFLAGS += -fpie -Wall #-Werror
TARGET_CFLAGS += -DSWRT_PATCH
TARGET_CPPFLAGS += -DSWRT_PATCH
TARGET_LDFLAGS += \
	-lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie -lssl -lcrypto

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/hostapd/*.c),)
	$(CP) hostapd-default.config $(PKG_BUILD_DIR)/hostapd/.config 
	$(CP) wpa_supplicant-default.config $(PKG_BUILD_DIR)/wpa_supplicant/.config
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR)/hostapd clean
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/hostapd $(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)" hostapd hostapd_cli
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR)/wpa_supplicant clean
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/wpa_supplicant $(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)" wpa_supplicant wpa_cli
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/hostapd
	mkdir -p $(PKG_BUILD_DIR)/wpa_supplicant
	$(CP) prebuilt/$(BUILD_NAME)/hostap* $(PKG_BUILD_DIR)/hostapd
	$(CP) prebuilt/$(BUILD_NAME)/wpa_* $(PKG_BUILD_DIR)/wpa_supplicant
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/bin ] || install -d $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/hostapd/hostapd $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/hostapd/hostapd_cli $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/wpa_supplicant/wpa_cli $(INSTALLDIR)/usr/bin
	$(STRIP) $(INSTALLDIR)/usr/bin/hostapd
	$(STRIP) $(INSTALLDIR)/usr/bin/hostapd_cli
	$(STRIP) $(INSTALLDIR)/usr/bin/wpa_supplicant
	$(STRIP) $(INSTALLDIR)/usr/bin/wpa_cli

clean:

stage:
ifneq ($(wildcard $(PKG_BUILD_DIR)/hostapd/*.c),)
	$(CP) $(PKG_BUILD_DIR)/src/common/wpa_ctrl.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/common/qca-vendor.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/common/ieee802_11_defs.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/includes.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/build_config.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/eloop.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/os.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/common.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/wpa_debug.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/utils/wpabuf.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/linux_ioctl.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/linux_wext.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/netlink.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/priv_netlink.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/nl80211_copy.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/common/wpa_ctrl.o $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/drivers/netlink.o $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/utils/eloop.o $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/utils/wpa_debug.o $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/utils/common.o $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/utils/os_unix.o $(STAGING_DIR)/usr/lib/
endif

