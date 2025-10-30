# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

MAKEOPTS+= \
	$(TARGET_CONFIGURE_OPTS) \
	COPTS="$(TARGET_CFLAGS)" \
	WPA_PATH=$(STAGING_DIR)/usr/lib \
	QSDK_WRAPD=$(STAGING_DIR)/usr/include \
	LIBS="$(TARGET_LDFLAGS)"

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/*.c),)
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(strip $(MAKEOPTS)) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuild/$(BUILD_NAME)/wrapd $(PKG_BUILD_DIR)/wrapd
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/wrapd $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/wrapd

clean:

stage:

