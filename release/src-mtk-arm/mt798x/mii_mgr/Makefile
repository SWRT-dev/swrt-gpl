# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -Wall -Werror
TARGET_CFLAGS += -I./source


all:
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	touch stamp-h1

install:
	install -D $(PKG_BUILD_DIR)/mii_mgr $(INSTALLDIR)/usr/sbin/mii_mgr
	$(STRIP) $(INSTALLDIR)/usr/sbin/mii_mgr
	cd $(INSTALLDIR)/usr/sbin/ && ln -sf mii_mgr mii_mgr_cl45

clean:


