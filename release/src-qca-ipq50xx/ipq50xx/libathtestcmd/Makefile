# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += \
		 -I./include \
		 -I$(STAGING_DIR)/usr/lib/ \
		 -I$(STAGING_DIR)/usr/include/libnl3 \
		 -I$(STAGING_DIR)/usr/include/libnl3/netlink \
		 -I$(STAGING_DIR)/usr/include/libtlv2 \
		 -I$(STAGING_DIR)/usr/include/libtlvutil \
		 -fPIC -DWLAN_API_NL80211 -DLIBNL_2 -DWIN_AP_HOST -D_HOST_SIM_TESTING -DLinux -DUSE_TLV2

TARGET_LDFLAGS += -shared
TARGET_LDFLAGS += -ltcmd -ltlv2 -ltlv1util

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/libtestcmd6174.so $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/libtestcmd6174.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtestcmd6174.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/*.c),)
	[ -d $(STAGEDIR)/usr/include/libathtestcmd ] || install -d $(STAGEDIR)/usr/include/libathtestcmd
	$(CP) $(PKG_BUILD_DIR)/testcmd6174.h $(STAGEDIR)/usr/include/libathtestcmd
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGEDIR)/usr/include/libathtestcmd
	$(CP) $(PKG_BUILD_DIR)/libtestcmd6174.so $(STAGEDIR)/usr/lib
endif

