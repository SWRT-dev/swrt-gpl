include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source
ifeq ($(RTCONFIG_SWRTMESH),y)
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH
endif

all:
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS=$(TARGET_CFLAGS)

install:
	[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	cp -rf $(PKG_BUILD_DIR)/libeasy.so* $(INSTALLDIR)/usr/lib/
	$(STRIP) $(INSTALLDIR)/usr/lib/libeasy.so*

clean:
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f

stage:
	mkdir -p $(STAGEDIR)/usr/include/easy
	install -m 644 $(PKG_BUILD_DIR)/{easy,event,utils,if_utils,debug,hlist}.h $(STAGEDIR)/usr/include/easy
	cp -rf $(PKG_BUILD_DIR)/libeasy.so* $(STAGEDIR)/usr/lib

.PHONY: stage all install clean
