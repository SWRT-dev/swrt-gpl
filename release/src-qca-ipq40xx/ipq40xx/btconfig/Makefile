include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
TARGET_CC=$(CROSS_COMPILE)gcc
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
LINUX_VERSION=$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/*.c),)
	$(MAKE) -C $(PKG_BUILD_DIR)
else
	mkdir -p $(PKG_BUILD_DIR)/
	cp prebuilt/$(BUILD_NAME)/btconfig $(PKG_BUILD_DIR)/btconfig
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/bin ] || install -d $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/btconfig $(INSTALLDIR)/usr/bin
	@$(STRIP) $(INSTALLDIR)/usr/bin/btconfig

clean:

stage:

