# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS+= \
	-I$(SDK_DIR_PLATFORM)/qca-nss-udp-st-drv/exports

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-udp-st/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-udp-st source
	$(TARGET_MAKE) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/nss-udp-st $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/nss-udp-st $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/nss-udp-st

clean:

stage:
	echo ok

