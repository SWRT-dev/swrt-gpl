# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += \
		 -I./../include \
		 -I./common/ \
		 -I./templete/ \
		 -I./defParm/ \
		 -fPIC -DWLAN_API_NL80211 -DLIBNL_2 -DWIN_AP_HOST -D_HOST_SIM_TESTING -D_FOR_HOST_TESTING

TARGET_LDFLAGS += -shared

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/libtlvutil/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/libtlvutil source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/libtlv1util.so $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/libtlv1util.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtlv1util.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/libtlvutil/*.c),)
	[ -d $(STAGEDIR)/usr/include/libtlvutil ] || install -d $(STAGEDIR)/usr/include/libtlvutil
	$(CP) $(PKG_BUILD_DIR)/common/tcmdHostInternal.h $(STAGEDIR)/usr/include/libtlvutil
	$(CP) $(PKG_BUILD_DIR)/*.h $(STAGEDIR)/usr/include/libtlvutil
	$(CP) $(PKG_BUILD_DIR)/libtlv1util.so $(STAGEDIR)/usr/lib
endif

