# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

MAKEOPTS+= \
	$(TARGET_CONFIGURE_OPTS) \
	WLAN_TOP=$(STAGING_DIR)/usr \
	COPTS="$(CFLAGS) -I. -I$(PKG_BUILD_DIR)/include" \
	QSDK_ACFG=1 MEM_PROFILE="$(CONFIG_KERNEL_IPQ_MEM_PROFILE)" \
	TOOLPREFIX=$(CROSS_COMPILE)

PBBINS=$(wildcard prebuilt/*)
QCA_GPL_BINS=$(filter-out %.conf,$(PBBINS))
QCA_GPL_CONF=$(filter %.conf,$(PBBINS))

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-acfg/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-acfg source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/src $(MAKEOPTS) $(TARGET_CONFIGURE_OPTS)
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/acfg_test_app $(MAKEOPTS) $(TARGET_CONFIGURE_OPTS) LIB_OUT=acfg
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/acfg_profile_app $(MAKEOPTS) $(TARGET_CONFIGURE_OPTS) PROFILE_LIB_OUT=acfg_profile
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/src
	mkdir -p $(PKG_BUILD_DIR)/acfg_test_app
	mkdir -p $(PKG_BUILD_DIR)/acfg_profile_app
	cp prebuilt/$(BUILD_NAME)/libacfg.so $(PKG_BUILD_DIR)/src/libacfg.so
	cp prebuilt/$(BUILD_NAME)/acfg_tool $(PKG_BUILD_DIR)/acfg_test_app/acfg_tool
	cp prebuilt/$(BUILD_NAME)/acfg_set_profile $(PKG_BUILD_DIR)/acfg_profile_app/acfg_set_profile
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/etc ] || install -d $(INSTALLDIR)/etc
	@install -m 755 -D $(PKG_BUILD_DIR)/acfg_test_app/acfg_tool $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/acfg_profile_app/acfg_set_profile $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/src/libacfg.so $(INSTALLDIR)/usr/lib
	install -m 644 -D prebuilt/acfg_common.conf $(INSTALLDIR)/etc
	@$(STRIP) $(INSTALLDIR)/usr/sbin/acfg_tool
	@$(STRIP) $(INSTALLDIR)/usr/sbin/acfg_set_profile
	@$(STRIP) $(INSTALLDIR)/usr/lib/libacfg.so

clean:

stage:
	@install -m 755 -D $(PKG_BUILD_DIR)/src/libacfg.so $(STAGEDIR)/usr/lib
