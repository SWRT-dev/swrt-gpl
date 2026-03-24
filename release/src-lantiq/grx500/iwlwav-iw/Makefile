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
TARGET_LDFLAGS += -lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie -lssl -lcrypto-lrt -lsafec-3.3
TARGET_LDFLAGS += -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro
IFX_CFLAGS:=$(TARGET_CFLAGS)
IFX_LDFLAGS=$(TARGET_LDFLAGS)

all:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) \
		 IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)"
	touch stamp-h1


install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/iw $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/iw
	#echo -e "iwlwav_iw_hash=\"9198b42e874de8de23d9c46d44b4a9398febf72b\"\niwlwav_iw_tag=\"9198b42e874de8de23d9c46d44b4a9398febf72b\"" > $(INSTALLDIR)/etc/iwlwav_iw.ver

clean:

stage:

