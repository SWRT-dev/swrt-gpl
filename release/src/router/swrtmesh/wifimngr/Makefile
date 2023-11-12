include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -D_GNU_SOURCE -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH

all:
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)"

install:
	[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 $(PKG_BUILD_DIR)/wifimngr  $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/wifimngr

clean:
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f

.PHONY: all install clean
