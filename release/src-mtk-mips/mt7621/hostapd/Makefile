# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CPPFLAGS += \
	-I$(PKG_BUILD_DIR)/src -I$(PKG_BUILD_DIR)/src/utils

TARGET_CFLAGS += -fpie -Wall #-Werror
TARGET_CFLAGS += -DSWRT_PATCH
TARGET_CPPFLAGS += -DSWRT_PATCH

TARGET_LDFLAGS += \
	-lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie -lssl -lcrypto -latomic


TARGET_CONFIGURE_OPTS+= CONFIG_P2P=y CONFIG_INTERNAL_LIBTOMMATH=y 
TARGET_CONFIGURE_OPTS+= CONFIG_DRIVER_NL80211=y RTCONFIG_MT798X=$(RTCONFIG_MT798X)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/hostapd/hostapd/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/hostapd source
	$(CP) hostapd.config $(PKG_BUILD_DIR)/hostapd/.config 
	$(CP) wpa_supplicant.config $(PKG_BUILD_DIR)/wpa_supplicant/.config
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/hostapd $(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)" hostapd hostapd_cli
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

