include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)

TARGET_CPPFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	-I$(STAGING_DIR)/usr/include/qca-nss-macsec \
	-I$(PKG_BUILD_DIR)/src -I$(PKG_BUILD_DIR)/src/utils

TARGET_LDFLAGS += \
	-lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie

TARGET_CFLAGS += -fpie -Wall #-Werror
TARGET_LDFLAGS += -lfal -L$(STAGING_DIR)/usr/lib

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-hostap/hostapd/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-hostap source
	cp -f hostapd.config $(PKG_BUILD_DIR)/hostapd/.config 
	cp -f wpa_supplicant.config $(PKG_BUILD_DIR)/wpa_supplicant/.config
	$(MAKE) -C $(PKG_BUILD_DIR)/hostapd TARGET_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" $(TARGET_CONFIGURE_OPTS) LIBS="$(TARGET_LDFLAGS)" hostapd hostapd_cli
	$(MAKE) -C $(PKG_BUILD_DIR)/wpa_supplicant TARGET_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" $(TARGET_CONFIGURE_OPTS) LIBS="$(TARGET_LDFLAGS)" wpa_supplicant wpa_cli
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/hostapd
	mkdir -p $(PKG_BUILD_DIR)/wpa_supplicant
	cp -rf prebuilt/hostap* $(PKG_BUILD_DIR)/hostapd
	cp -rf prebuilt/wpa_* $(PKG_BUILD_DIR)/wpa_supplicant
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
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-hostap/hostapd/*.c),)
	cp -rf $(PKG_BUILD_DIR)/src/common/wpa_ctrl.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/common/qca-vendor.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/common/ieee802_11_defs.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/includes.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/build_config.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/eloop.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/os.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/common.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/wpa_debug.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/utils/wpabuf.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/drivers/linux_ioctl.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/drivers/linux_wext.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/drivers/netlink.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/drivers/priv_netlink.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/src/drivers/nl80211_copy.h $(STAGING_DIR)/usr/include/
	cp -rf $(PKG_BUILD_DIR)/build/hostapd/src/common/wpa_ctrl.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/hostapd/src/drivers/netlink.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/hostapd/src/utils/eloop.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/hostapd/src/utils/wpa_debug.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/hostapd/src/utils/common.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/hostapd/src/utils/os_unix.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/wpa_supplicant/src/common/wpa_ctrl.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/wpa_supplicant/src/drivers/netlink.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/eloop.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/wpa_debug.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/common.o $(STAGING_DIR)/usr/lib/
	cp -rf $(PKG_BUILD_DIR)/build/wpa_supplicant/src/utils/os_unix.o $(STAGING_DIR)/usr/lib/
endif
