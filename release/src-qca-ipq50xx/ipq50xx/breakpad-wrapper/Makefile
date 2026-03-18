# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/breakpad -O3 -fPIC
TARGET_CXXFLAGS += $(TARGET_CFLAGS)
NEW_CXXFLAGS=$(filter-out -Wno-pointer-sign,$(TARGET_CXXFLAGS))
TARGET_CXXFLAGS=NEW_CXXFLAGS

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/breakpad-wrapper/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/breakpad-wrapper source
	$(TARGET_MAKE) all -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/$(BUILD_NAME)/libbreakpad_qcawrapper.so $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/libbreakpad_qcawrapper.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libbreakpad_qcawrapper.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/breakpad-wrapper/*.c),)
	cp -rf $(PKG_BUILD_DIR)/breakpad_qcawrapper.h $(STAGING_DIR)/usr/include
	cp -rf $(PKG_BUILD_DIR)/libbreakpad_qcawrapper.so $(STAGING_DIR)/usr/lib
endif
