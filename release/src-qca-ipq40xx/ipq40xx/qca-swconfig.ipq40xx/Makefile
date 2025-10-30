# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source
subtarget:=$(CONFIG_TARGET_BOARD)

all:
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	touch stamp-h1

clean:

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/swconfig $(INSTALLDIR)/usr/sbin/swconfig
	$(STRIP) $(INSTALLDIR)/usr/sbin/swconfig

stage:

