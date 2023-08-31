include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
LINUX_VERSION=$(LINUX_KERNEL)

MAKEOPTS+= \
           $(TARGET_CONFIGURE_OPTS) \
           COPTS="$(CFLAGS)" \
           WPA_PATH=$(STAGING_DIR)/usr/lib \
           QSDK_WRAPD=$(STAGING_DIR)/usr/include \
           LIBS="$(LDFLAGS)"

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-wrapd/*.c),)
	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKEOPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp -rf prebuilt/* $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/wrapd $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/wrapd

clean:

stage:
