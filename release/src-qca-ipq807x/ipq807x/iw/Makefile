# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)

TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -pie

TARGET_CPPFLAGS:= \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	$(TARGET_CPPFLAGS) \
	-DCONFIG_LIBNL20 \
	-D_GNU_SOURCE

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -ffunction-sections -fdata-sections" \
	LDFLAGS="$(TARGET_LDFLAGS) -Wl,--gc-sections" \
	NL1FOUND="" NL2FOUND=Y \
	NLLIBNAME="libnl-tiny" \
	LIBS="-lm -lnl-tiny" \
	V=1

all:
	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS)
	touch stamp-h1

install:
	install -d $(INSTALLDIR)/usr/sbin
	install -m 755 $(PKG_BUILD_DIR)/iw $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/*

clean:
	find $(PKG_BUILD_DIR) -type f -name iw \
		-o -name ".*.o.cmd" \
		-o -name ".*.ko.cmd" \
		-o -name "*.o" \
		-o -name "modules.order" \
		-o -name "*.ko" \
		-o -name ".tmp_versions" \
		-o -name "Module.symvers" \
		-o -name "*.mod.c" | xargs rm -fr
