# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -DHAVE_C99=1
LDFLAGS += -lnl-genl-3 -lnl-3 -pthread -lpthread -lsafec-3.3
TARGET_CFLAGS += -fstack-protector-strong -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security -Wall -fPIC -Wl,-strip-debug
LDFLAGS += -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro




BUILD_VARIANT:=release
ifeq ($(DEBUG),y)
TARGET_CFLAGS += -DCONFIG_WAVE_DEBUG
BUILD_VARIANT:=debug
endif


all:
	@echo "LDFLAGS: $(LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR) CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(LDFLAGS)" $(BUILD_VARIANT)
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/opt/intel/bin/ ] || install -d $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/dutserver/linux/dutserver $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/dump_handler/dump_handler $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/whm_handler/whm_handler $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/drvhlpr/drvhlpr $(INSTALLDIR)/opt/intel/bin/
ifeq ($(BUILD_VARIANT),debug)
	@install -D $(PKG_BUILD_DIR)/tools/rtlogger/logserver/linux/logserver $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/BCLSockServer/BCLSockServer $(INSTALLDIR)/opt/intel/bin/
endif
	$(STRIP) $(INSTALLDIR)/opt/intel/bin/*
	#echo -e "iwlwav_tools_hash=\"68e5c9477eab18cc2711394156cd5eb3474d22b6\"\niwlwav_tools_tag=\"68e5c9477eab18cc2711394156cd5eb3474d22b6\"" > $(1)/etc/iwlwav_tools.ver


clean:

stage:


