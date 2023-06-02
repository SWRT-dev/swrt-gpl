include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source/src

TARGET_CFLAGS += -Wno-error=deprecated-declarations -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3 -I$(STAGEDIR)/usr/include/openssl -D_GNU_SOURCE
ifeq ($(or $(RTCONFIG_RALINK_EN7561),$(RTCONFIG_RALINK_EN7562),$(RTCONFIG_RALINK_MT7621)),y)
TARGET_CFLAGS += -DEASYMESH_VERSION=2
else
TARGET_CFLAGS += -DEASYMESH_VERSION=3
endif
TARGET_CFLAGS += -DCONTROLLER_SYNC_DYNAMIC_CNTLR_CONFIG -DEASYMESH_VENDOR_EXT_OUI=\\\"\\\\x9C\\\\xC9\\\\xCC\\\"
TARGET_CFLAGS += -DEASYMESH_VENDOR_EXT
ifeq ($(RTCONFIG_SWRTMESH),y)
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH
endif

all:
ifneq ($(wildcard source/src/Makefile),)
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)"
endif

install:
	[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	[ -d $(INSTALLDIR)/usr/lib/mapcontroller ] || install -d $(INSTALLDIR)/usr/lib/mapcontroller
	[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
ifneq ($(wildcard source/src/Makefile),)
	install -m 755 $(PKG_BUILD_DIR)/mapcontroller $(INSTALLDIR)/usr/sbin/
	install -m 755 $(PKG_BUILD_DIR)/plugins/steer/rcpi/rcpi.so $(INSTALLDIR)/usr/lib/mapcontroller/
else
	install -m 755 prebuilt/$(BUILD_NAME)/mapcontroller $(INSTALLDIR)/usr/sbin/
	install -m 755 prebuilt/$(BUILD_NAME)/rcpi.so $(INSTALLDIR)/usr/lib/mapcontroller/
endif
	$(STRIP) $(INSTALLDIR)/usr/lib/mapcontroller/rcpi.so
	$(STRIP) $(INSTALLDIR)/usr/sbin/mapcontroller

clean:
ifneq ($(wildcard source/src/Makefile),)
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f
endif


.PHONY: all install clean
