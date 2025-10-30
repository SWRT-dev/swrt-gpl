# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source
subtarget:=$(CONFIG_TARGET_BOARD)

TARGET_CFLAGS += \
		 -I$(STAGING_DIR)/usr/include/ \
		 -I$(STAGING_DIR)/usr/include/libxml2 \
		 -I$(STAGING_DIR)/usr/include/libnl \
		 -I$(STAGING_DIR)/usr/lib/
		 -D_GNU_SOURCE=1 -D__USE_GNU=1 -D__WIN__=1
#include netlink-private
TARGET_CFLAGS += -I$(TOP_PLATFORM)/libnl/include

TARGET_LDFLAGS += \
		  -lnl-3 -lnl-genl-3 -lxml2

WIFISON_CFG_MAKEOPTS:= \
		TOOLPREFIX=$(TARGET_CROSS) \
		GWLIB=$(STAGING_DIR)/usr/include/ \
		GWINCLUDE=$(STAGING_DIR)/usr/include/hyficommon \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		EXTRA_LDFLAGS="$(TARGET_LDFLAGS)" \
		INSTALL_ROOT=$(PKG_BUILD_DIR)/install

#WIFISON_CFG_MAKEOPTS+=LIBCFG80211_SUPPORT=y
WIFISON_CFG_MAKEOPTS+=WIFISON_SUPPORT_QSDK=y
#WIFISON_CFG_MAKEOPTS+=EZMESH_MODULE_AFC=y
#WIFISON_CFG_MAKEOPTS+=ENABLE_SON_MEMORY_DEBUG=y
#WIFISON_CFG_MAKEOPTS+=SONMEMDEBUGINCLUDE=$(STAGING_DIR)/usr/include/son-mem-dbg
WIFISON_CFG_MAKEOPTS+=SWRT_PATCH=y

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/*.c),)
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib
	mkdir -p $(PKG_BUILD_DIR)/install/include
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR)/ $(strip $(WIFISON_CFG_MAKEOPTS)) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/install
	mkdir -p $(PKG_BUILD_DIR)/install/lib/
	cp prebuilt/$(BUILD_NAME)/libwifisoncfg.so $(PKG_BUILD_DIR)/install/lib/libwifisoncfg.so
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 -D $(PKG_BUILD_DIR)/install/lib/libwifisoncfg.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libwifisoncfg.so

clean:

stage:
ifneq ($(wildcard $(PKG_BUILD_DIR)/*.c),)
	$(CP) $(PKG_BUILD_DIR)/install/include/wlanif_cmn.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/install/include/sonlib_cmn.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/install/lib/libwifisoncfg.so $(STAGING_DIR)/usr/lib
endif

