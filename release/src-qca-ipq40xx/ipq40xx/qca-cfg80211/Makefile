# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += \
		-I$(STAGING_DIR)/usr/include/libnl \
		-I$(STAGING_DIR)/usr/include/libnl3 \
		-I$(TOP)/libnl/include -I$(PKG_BUILD_DIR)/qca_nl80211_lib \
		-I$(PKG_BUILD_DIR)/vendorcmdtool \
		-I$(SDK_DIR_PLATFORM)/qca-wifi/include \
		-I$(SDK_DIR_PLATFORM)/qca-wifi/component_dev/tools/linux \
		-D_GNU_SOURCE=1 -D__USE_GNU=1 -fpie -fPIC

TARGET_LDFLAGS += -lnl-3 -lnl-genl-3 -pie
TARGET_CFLAGS+= -D__IPQ__=1
#TARGET_CFLAGS+= -DCONFIG_SUPPORT_LIBROXML=1 -I$(SDK_DIR_PLATFORM)/libroxml/source/src
#TARGET_CFLAGS+= -DCONFIG_SUPPORT_VENCMDTABLE=1
#TARGET_LDFLAGS+= -L$(SDK_DIR_PLATFORM)/libroxml/source/.libs -lroxml

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/qca_nl80211_lib/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/qca_nl80211_lib $(TARGET_CONFIGURE_OPTS)
#	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/vendorcmdtool $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/qca_nl80211_lib
	mkdir -p $(PKG_BUILD_DIR)/vendorcmdtool
	cp prebuild/$(BUILD_NAME)/libqca_nl80211_wrapper.so $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so
#	cp prebuild/$(BUILD_NAME)/cfg80211tool.1 $(PKG_BUILD_DIR)/vendorcmdtool/cfg80211tool.1
endif
	touch stamp-h1

install:
#	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so $(INSTALLDIR)/usr/lib
#	@install -m 755 -D $(PKG_BUILD_DIR)/vendorcmdtool/cfg80211tool.1 $(INSTALLDIR)/usr/sbin
#	@$(STRIP) $(INSTALLDIR)/usr/sbin/cfg80211tool.1
	@$(STRIP) $(INSTALLDIR)/usr/lib/libqca_nl80211_wrapper.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/qca_nl80211_lib/*.c),)
	[ -d $(STAGEDIR)/usr/include/qca-cfg80211 ] || install -d $(STAGEDIR)/usr/include/qca-cfg80211
	cp -f $(PKG_BUILD_DIR)/qca_nl80211_lib/cfg80211_nlwrapper_api.h $(STAGEDIR)/usr/include/cfg80211_nlwrapper_api.h
	cp -f $(PKG_BUILD_DIR)/qca_nl80211_lib/cfg80211_nlwrapper_pvt.h $(STAGEDIR)/usr/include/cfg80211_nlwrapper_pvt.h
	cp -f $(PKG_BUILD_DIR)/qca_nl80211_lib/qca-vendor.h $(STAGEDIR)/usr/include/qca-cfg80211/qca-vendor.h
	cp -f $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so $(STAGEDIR)/usr/lib/
endif
