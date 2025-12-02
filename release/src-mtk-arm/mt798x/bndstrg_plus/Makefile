# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/8021xd/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/8021xd source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/$(BUILD_NAME)/8021xd $(PKG_BUILD_DIR)/8021xd
endif
	touch stamp-h1

install:
	install -D $(PKG_BUILD_DIR)/8021xd $(INSTALLDIR)/usr/sbin/8021xd
	$(STRIP) $(INSTALLDIR)/usr/sbin/8021xd

clean:

