include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source/src

TARGET_CFLAGS += -Wno-error=deprecated-declarations -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3 -I$(TOP)/libnetfilter_conntrack-1.0.7/include -D_GNU_SOURCE
TARGET_CFLAGS += -DEASYMESH_VENDOR_EXT_OUI=\\\"\\\\xF8\\\\x32\\\\xE4\\\"
ifeq ($(RTCONFIG_SWRTMESH),y)
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH
endif

all:
ifneq ($(wildcard source/src/Makefile),)
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)"
endif

install:
	[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
ifneq ($(wildcard source/src/Makefile),)
	install -m 755 $(PKG_BUILD_DIR)/topologyd $(INSTALLDIR)/usr/sbin/
else
	install -m 755 prebuilt/$(BUILD_NAME)/topologyd $(INSTALLDIR)/usr/sbin/
endif
	$(STRIP) $(INSTALLDIR)/usr/sbin/topologyd

clean:
ifneq ($(wildcard source/src/Makefile),)
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f
endif


.PHONY: all install clean
