# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/bndstrg_plus/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/bndstrg_plus source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/$(BUILD_NAME)/bndstrg2 $(PKG_BUILD_DIR)/bndstrg2
endif
	touch stamp-h1

install:
	install -D $(PKG_BUILD_DIR)/bndstrg2 $(INSTALLDIR)/usr/sbin/bndstrg2
	$(STRIP) $(INSTALLDIR)/usr/sbin/bndstrg2

clean:

