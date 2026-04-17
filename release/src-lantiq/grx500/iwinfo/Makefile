# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

FPIC=-fpic

IWINFO_BACKENDS := \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl),wl) \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl-mini),wl) \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl-mimo),wl) \
	$(if $(CONFIG_PACKAGE_kmod-cfg80211),nl80211) \
	$(if $(CONFIG_TARGET_intel_mips),nl80211)

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-D_GNU_SOURCE -DSWRT_PATCH

MAKE_FLAGS += \
	FPIC="$(FPIC)" \
	BACKENDS="$(IWINFO_BACKENDS)" \
	SOVERSION="$(IWINFO_ABI_VERSION)"

CFLAGS+="$(TARGET_CFLAGS)"
LDFLAGS+="$(TARGET_LDFLAGS)"

IWINFO_HDRS1 := $(wildcard $(PKG_BUILD_DIR)/include/iwinfo.h)
IWINFO_HDRS2 := $(wildcard $(PKG_BUILD_DIR)/include/iwinfo/*)

all:
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) $(MAKE_FLAGS)
	touch stamp-h1

install:
	@mkdir -p $(INSTALLDIR)/usr/bin
	install -m 755 $(PKG_BUILD_DIR)/iwinfo $(INSTALLDIR)/usr/bin
	$(STRIP) $(INSTALLDIR)/usr/bin/*
	@mkdir -p $(INSTALLDIR)/usr/lib
	install -m 755 $(PKG_BUILD_DIR)/libiwinfo.so.0 $(INSTALLDIR)/usr/lib
	$(STRIP) $(INSTALLDIR)/usr/lib/*
	cd $(INSTALLDIR)/usr/lib && ln -sf libiwinfo.so.0 libiwinfo.so
	@mkdir -p $(INSTALLDIR)/usr/share/libiwinfo
	rsync -avcH $(PKG_BUILD_DIR)/devices.txt $(INSTALLDIR)/usr/share/libiwinfo/
	#@mkdir -p $(INSTALLDIR)/usr/lib/lua
	#install -m 755 $(PKG_BUILD_DIR)/iwinfo.so $(INSTALLDIR)/usr/lib/lua
	#$(STRIP) $(INSTALLDIR)/usr/lib/lua/*

clean:
	find $(PKG_BUILD_DIR) -type f -name iwinfo \
		-o -name "*.so" \
		-o -name ".*.o.cmd" \
		-o -name ".*.ko.cmd" \
		-o -name "*.o" \
		-o -name "modules.order" \
		-o -name "*.ko" \
		-o -name ".tmp_versions" \
		-o -name "Module.symvers" \
		-o -name "*.mod.c" | xargs rm -fr

stage: $(IWINFO_HDRS1) $(IWINFO_HDRS2)
	@mkdir -p $(STAGEDIR)/usr/include/iwinfo
	rsync -avcH $(foreach l,$(IWINFO_HDRS1),$(l) )$(STAGEDIR)/usr/include/
	rsync -avcH $(foreach l,$(IWINFO_HDRS2),$(l) )$(STAGEDIR)/usr/include/iwinfo/
	@mkdir -p $(STAGEDIR)/usr/lib
	install -m 755 $(PKG_BUILD_DIR)/libiwinfo.so $(STAGEDIR)/usr/lib/libiwinfo.so
	#mkdir -p $(STAGEDIR)/usr/lib/lua
	#install -m 755 $(PKG_BUILD_DIR)/iwinfo.so $(STAGEDIR)/usr/lib/lua/iwinfo.so
	@touch $@
