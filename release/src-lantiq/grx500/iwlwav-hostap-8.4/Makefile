# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

CONFIG_SUFFIX:=release
TARGET_CFLAGS += -DSWRT_PATCH -DHAVE_C99=1 -fstack-protector-strong -O2 -D_FORTIFY_SOURCE=2
TARGET_CFLAGS += -Wformat -Wformat-security -Werror=format-security -Wall -Wtype-limits -fPIE -Wl,-strip-debug
TARGET_CPPFLAGS += -DSWRT_PATCH
ifeq ($(DEBUG),y)
TARGET_CFLAGS += -DCONFIG_WAVE_DEBUG #-DCONFIG_NO_HOSTAPD_LOGGER
CONFIG_SUFFIX:=debug
endif
TARGET_LDFLAGS += -lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -latomic -pie -lrt -lsafec-3.3
TARGET_LDFLAGS += -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro -pie
IFX_CFLAGS:=$(TARGET_CFLAGS)
IFX_LDFLAGS=$(TARGET_LDFLAGS)



all:
	$(MAKE) -C $(PKG_BUILD_DIR)/src clean
	$(CP) ./hostapd-$(CONFIG_SUFFIX).config $(PKG_BUILD_DIR)/hostapd/.config
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/hostapd $(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)" EXTRA_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" hostapd hostapd_cli
	$(MAKE) -C $(PKG_BUILD_DIR)/src clean
	$(CP) ./wpa_supplicant-$(CONFIG_SUFFIX).config $(PKG_BUILD_DIR)/wpa_supplicant/.config
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/wpa_supplicant $(TARGET_CONFIGURE_OPTS) \
		LIBS="$(TARGET_LDFLAGS)" EXTRA_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" \
		wpa_supplicant wpa_cli wpa_passphrase
	touch stamp-h1


install:
	@[ -d $(INSTALLDIR)/usr/bin ] || install -d $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/hostapd/hostapd $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/hostapd/hostapd_cli $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/wpa_supplicant/wpa_passphrase $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/wpa_supplicant/wpa_cli $(INSTALLDIR)/usr/bin
	$(STRIP) $(INSTALLDIR)/usr/bin/hostapd
	$(STRIP) $(INSTALLDIR)/usr/bin/hostapd_cli
	$(STRIP) $(INSTALLDIR)/usr/bin/wpa_supplicant
	$(STRIP) $(INSTALLDIR)/usr/bin/wpa_passphrase
	$(STRIP) $(INSTALLDIR)/usr/bin/wpa_cli
	#echo -e "iwlwav_hostap_hash=\"ca1c1ee61ce1046974b749c108fa8e073ee3ae37\"\niwlwav_hostap_tag=\"ca1c1ee61ce1046974b749c108fa8e073ee3ae37\"" > $(INSTALLDIR)/etc/iwlwav_hostap.ver

clean:

stage:
	$(CP) $(PKG_BUILD_DIR)/src/common/wpa_ctrl.h $(STAGING_DIR)/usr/include/

