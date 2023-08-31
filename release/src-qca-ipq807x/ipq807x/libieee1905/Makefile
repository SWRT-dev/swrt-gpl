include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
LINUX_VERSION=$(LINUX_KERNEL)
subtarget:=$(CONFIG_TARGET_BOARD)


TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-D_GNU_SOURCE -fpic

QCAIEEE1905_MAKEOPTS:= \
        TOOLPREFIX=$(TARGET_CROSS) \
        GWINCLUDE=$(STAGING_DIR)/usr/include/hyficommon/ \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS) $(TARGET_CFLAGS)" \
		EXTRA_LDFLAGS="$(LDFLAGS)" \
        INSTALL_ROOT=$(PKG_BUILD_DIR)/install

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/libieee1905/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/libieee1905 source
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib
	mkdir -p $(PKG_BUILD_DIR)/install/include
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCAIEEE1905_MAKEOPTS))
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib/
	cp prebuilt/libieee1905.so $(PKG_BUILD_DIR)/install/lib/libieee1905.so
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/install/lib/libieee1905.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libieee1905.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/libieee1905/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/ieee1905
	cp -rf $(PKG_BUILD_DIR)/install/include/ieee1905_defs.h $(STAGING_DIR)/usr/include/ieee1905
	cp -rf $(PKG_BUILD_DIR)/install/include/ieee1905.h $(STAGING_DIR)/usr/include/ieee1905
	cp -rf $(PKG_BUILD_DIR)/install/include/ieee1905_vendor.h $(STAGING_DIR)/usr/include/ieee1905
	cp -rf $(PKG_BUILD_DIR)/install/include/ieee1905_vendor_consts.h $(STAGING_DIR)/usr/include/ieee1905
	cp -rf $(PKG_BUILD_DIR)/install/include/mid.h $(STAGING_DIR)/usr/include/ieee1905
	cp -rf $(PKG_BUILD_DIR)/install/lib/libieee1905.so $(STAGING_DIR)/usr/lib
endif
