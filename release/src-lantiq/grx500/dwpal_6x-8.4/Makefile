# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

BUILD_VARIANT:=release
ifeq ($(DEBUG),y)
TARGET_CFLAGS += -DCONFIG_WAVE_DEBUG
BUILD_VARIANT:=debug
endif



TARGET_CFLAGS += -I$(PLATFORM_ROUTER_SRCBASE)/iwlwav-iw-8.4/source/
TARGET_CFLAGS += -I$(PLATFORM_ROUTER_SRCBASE)/iwlwav-hostap-8.4/source/src/common/
TARGET_CFLAGS += -I$(PLATFORM_ROUTER_SRCBASE)/iwlwav-hostap-8.4/source/src/utils/ -DHAVE_C99=1
TARGET_CFLAGS += -fstack-protector-strong -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security
TARGET_CFLAGS += -Werror=format-security -Wall -fPIC -Wl,-strip-debug -DSWRT_PATCH
TARGET_LDFLAGS += -lnl-3 -lsafec-3.3
TARGET_LDFLAGS += -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro
ifeq ($(BUILD_VARIANT),debug)
TARGET_CFLAGS += -DCONFIG_WAVE_DEBUG
endif

IFX_CFLAGS:=$(TARGET_CFLAGS)
IFX_LDFLAGS:=$(TARGET_LDFLAGS)

all:
	@echo "LDFLAGS: $(LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(IFX_CFLAGS)" LDFLAGS="$(IFX_LDFLAGS)" \
		IWLWAV_HOSTAP_DIR="$(PLATFORM_ROUTER_SRCBASE)/iwlwav-hostap-8.4/source" \
		IWLWAV_IW_DIR="$(PLATFORM_ROUTER_SRCBASE)/iwlwav-iw-8.4/source"
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin/ ] || install -d $(INSTALLDIR)/usr/sbin/
	@[ -d $(INSTALLDIR)/usr/lib/ ] || install -d $(INSTALLDIR)/usr/lib/
	@install -D $(PKG_BUILD_DIR)/dwpal_cli $(INSTALLDIR)/usr/sbin/
	@install -D $(PKG_BUILD_DIR)/libdwpal.so $(INSTALLDIR)/usr/lib/
	$(STRIP) $(INSTALLDIR)/usr/sbin/*
	$(STRIP) $(INSTALLDIR)/usr/lib/*


clean:

stage:
	@install -D $(PKG_BUILD_DIR)/libdwpal.so $(STAGEDIR)/usr/lib
	@install -D $(PKG_BUILD_DIR)/include/*.h $(STAGEDIR)/usr/include/

