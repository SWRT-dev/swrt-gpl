# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

BUILD_VARIANT:=release
TARGET_CFLAGS += -DCONFIG_USE_MUSL
TARGET_CFLAGS += -DCONFIG_RPCD -DHAVE_C99=1
TARGET_CFLAGS += -DPACKAGE_ID=\\\"swpal_6x\\\" -DLOGGING_ID=\"swpal_6x\" \
				 -DLOG_LEVEL=6 -DLOG_TYPE=1
TARGET_LDFLAGS += -lsafec-3.3

ifeq ($(BUILD_VARIANT),debug)
TARGET_CFLAGS += -DCONFIG_WAVE_DEBUG

IFX_CFLAGS:=$(TARGET_CFLAGS)
IFX_LDFLAGS:=$(TARGET_LDFLAGS)

all:
	@echo "LDFLAGS: $(LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(IFX_CFLAGS)" LDFLAGS="$(IFX_LDFLAGS)" \
		IWLWAV_HOSTAP_DIR="$(PLATFORM_ROUTER_SRCBASE)/iwlwav-hostap/source" \
		IWLWAV_IW_DIR="$(PLATFORM_ROUTER_SRCBASE)/iwlwav-iw/source"
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/bin/ ] || install -d $(INSTALLDIR)/bin/
	@[ -d $(INSTALLDIR)/usr/lib/ ] || install -d $(INSTALLDIR)/usr/lib/
	@install -D $(PKG_BUILD_DIR)/libspal.so $(INSTALLDIR)/$(VENDOR_PATH)/usr/lib
ifeq ($(BUILD_VARIANT),debug)
	@install -D $(PKG_BUILD_DIR)/src/lite_nc/lite_nc $(INSTALLDIR)/bin/
endif
	$(STRIP) $(INSTALLDIR)/usr/sbin/*
	$(STRIP) $(INSTALLDIR)/usr/lib/*


clean:

stage:
	@[ -d $(STAGING_DIR)/usr/include/ ] || install -d $(STAGING_DIR)/usr/include/
	@[ -d $(STAGING_DIR)/usr/lib/ ] || install -d $(STAGING_DIR)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libspal.so $(STAGING_DIR)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/include/uci_wrapper.h $(STAGING_DIR)/usr/include/


