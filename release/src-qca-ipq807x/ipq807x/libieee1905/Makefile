# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)


TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-D_GNU_SOURCE -fpic

QCAIEEE1905_MAKEOPTS:= \
        TOOLPREFIX=$(TARGET_CROSS) \
        GWINCLUDE=$(STAGING_DIR)/usr/include/hyficommon/ \
		EXTRA_CFLAGS ="$(EXTRA_CFLAGS) $(TARGET_CFLAGS)" \
		EXTRA_LDFLAGS ="$(LDFLAGS)" \
        INSTALL_ROOT=$(PKG_BUILD_DIR)/install

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/libieee1905/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/libieee1905 source
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCAIEEE1905_MAKEOPTS))
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/libieee1905.so $(PKG_BUILD_DIR)/libieee1905.so
endif

install: all
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/libieee1905.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libieee1905.so

clean:

stage:
