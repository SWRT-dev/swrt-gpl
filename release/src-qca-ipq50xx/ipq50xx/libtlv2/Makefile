# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += \
		 -I./include \
		 -I./include/testUtil.h \
		 -fPIC -DWLAN_API_NL80211 -DLIBNL_2 -DWIN_AP_HOST -D_HOST_SIM_TESTING -D_FOR_HOST_TESTING \
                 -DLinux \
		 -I./cmdEncoder \
		 -I./cmdParser \
		 -I./common \
		 -I./tlvcmdrsp \
		 -I$(STAGING_DIR)/usr/include/libathtestcmd

TARGET_LDFLAGS += -shared

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/libtlv2/common/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/libtlv2 source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/ $(TARGET_CONFIGURE_OPTS) -f Makefile.win
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/libtlv2.so $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/libtlv2.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtlv2.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/libathtestcmd/libtlv2/common/*.c),)
	[ -d $(STAGEDIR)/usr/include/libtlv2 ] || install -d $(STAGEDIR)/usr/include/libtlv2
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGEDIR)/usr/include/libtlv2
	$(CP) $(PKG_BUILD_DIR)/libtlv2.so $(STAGEDIR)/usr/lib
endif

