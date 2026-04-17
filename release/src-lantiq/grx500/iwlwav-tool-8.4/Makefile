# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -DHAVE_C99=1 -I$(STAGING_DIR)/usr/include/libnl3
TARGET_LDFLAGS += -lnl-genl-3 -lnl-3 -pthread -lpthread -lsafec-3.3
TARGET_CFLAGS += -fstack-protector-strong -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security -Wall -fPIC -Wl,-strip-debug
TARGET_LDFLAGS += -Wl,-z,noexecstack -Wl,-z,now -Wl,-z,relro



BUILD_VARIANT:=release
ifeq ($(DEBUG),y)
TARGET_CFLAGS += -DCONFIG_WAVE_DEBUG
BUILD_VARIANT:=debug
endif


all:
	@echo "LDFLAGS: $(LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE) $(TARGET_CONFIGURE_OPTS) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/opt/intel/bin/ ] || install -d $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/dutserver/linux/dutserver $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/dump_handler/dump_handler $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/mttools/drvhlpr/drvhlpr $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/mttools/mtlk_cli/mtlk_cli $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/mttools/mtlk_cli_dbg_srv/mtlk_cli_dbg_server $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/rtlogger/logserver/linux/logserver $(INSTALLDIR)/opt/intel/bin/
	@install -D $(PKG_BUILD_DIR)/tools/BCLSockServer/BCLSockServer $(INSTALLDIR)/opt/intel/bin/
	$(STRIP) $(INSTALLDIR)/opt/intel/bin/*
	#echo -e "iwlwav_tools_hash=\"c28b7725a5a83f5632ea11d532a41fba9c50520d\"\niwlwav_tools_tag=\"c28b7725a5a83f5632ea11d532a41fba9c50520d\"" > $(1)/etc/iwlwav_tools.ver


clean:

stage:


