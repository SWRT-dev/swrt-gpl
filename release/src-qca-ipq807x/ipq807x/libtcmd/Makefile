# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += \
		 -I$(STAGING_DIR)/usr/include/libnl3 \
		 -I$(STAGING_DIR)/include \
		 -fPIC -DWLAN_API_NL80211 -DLIBNL_2 -DWIN_AP_HOST

TARGET_LDFLAGS += -lnl-3 -lnl-genl-3 -shared


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libtcmd/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libtcmd source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/libtcmd.so $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/libtcmd.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtcmd.so


clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libtcmd/*.c),)
	$(CP) $(PKG_BUILD_DIR)/*.h $(STAGEDIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libtcmd.so  $(STAGEDIR)/usr/lib/
endif

