# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)

TARGET_CFLAGS += \
		     -I$(STAGING_DIR)/usr/include/ \
		     -I$(STAGING_DIR)/usr/include/libnl \
		     -I$(STAGING_DIR)/usr/include/libnl3 \
                     -D_GNU_SOURCE=1 -D__USE_GNU=1 -fpie -fPIC

TARGET_LDFLAGS += \
	             -lnl-3 -lnl-genl-3 -pie
TOOL_CFLAGS :=  -D__IPQ__=1 -DCONFIG_SUPPORT_LIBROXML=1 -DCONFIG_SUPPORT_VENCMDTABLE=1
all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/qca_nl80211_lib/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools source
	$(MAKE) -C $(PKG_BUILD_DIR)/qca_nl80211_lib CC="$(CC)" CFLAGS="$(CFLAGS) $(TARGET_CFLAGS)" LDFLAGS="$(LDFLAGS) $(TARGET_LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR)/vendorcmdtool CC="$(CC)" CFLAGS="$(CFLAGS) $(TARGET_CFLAGS) $(TOOL_CFLAGS)" LDFLAGS="$(LDFLAGS) $(TARGET_LDFLAGS) -lroxml"
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/qca_nl80211_lib
	mkdir -p $(PKG_BUILD_DIR)/vendorcmdtool
	cp prebuilt/libqca_nl80211_wrapper.so $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so
	cp prebuilt/cfg80211tool.1 $(PKG_BUILD_DIR)/vendorcmdtool/cfg80211tool.1
endif

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/qca_nl80211_lib/libqca_nl80211_wrapper.so $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/vendorcmdtool/cfg80211tool.1 $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/cfg80211tool.1
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
