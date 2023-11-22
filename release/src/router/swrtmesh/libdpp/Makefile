include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source/lib

TARGET_CFLAGS += -D_GNU_SOURCE -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH

all:
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)"

install:
	[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 $(PKG_BUILD_DIR)/libdpp.so $(INSTALLDIR)/usr/lib/
	$(STRIP) $(INSTALLDIR)/usr/lib/libdpp.so

clean:
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f

stage:
	install -m 644 $(PKG_BUILD_DIR)/{dpp_api,dpputils}.h $(STAGEDIR)/usr/include
	cp -rf $(PKG_BUILD_DIR)/libdpp.so $(STAGEDIR)/usr/lib

.PHONY: stage all install clean
