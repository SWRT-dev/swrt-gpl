TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source

all:
ifneq ($(wildcard source/Makefile),)
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)"
endif

install:
	[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
ifneq ($(wildcard source/Makefile),)
	install -m 755 $(PKG_BUILD_DIR)/swrtmesh $(INSTALLDIR)/usr/sbin/
	install -m 755 $(PKG_BUILD_DIR)/libswrtmesh.so $(INSTALLDIR)/usr/lib/
	$(STRIP) $(INSTALLDIR)/usr/sbin/swrtmesh
	$(STRIP) $(INSTALLDIR)/usr/lib/libswrtmesh.so
else
	install -m 755 prebuilt/$(BUILD_NAME)/swrtmesh $(INSTALLDIR)/usr/sbin/
	install -m 755 prebuilt/$(BUILD_NAME)/libswrtmesh.so $(INSTALLDIR)/usr/lib/
endif

clean:
ifneq ($(wildcard source/Makefile),)
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f
endif

stage:
ifneq ($(wildcard source/Makefile),)
	install -m 644 $(PKG_BUILD_DIR)/swrtmesh-utils.h $(STAGEDIR)/usr/include/
	install -m 644 $(PKG_BUILD_DIR)/libswrtmesh.so $(STAGEDIR)/usr/lib
else
	install -m 644 prebuilt/swrtmesh-utils.h $(STAGEDIR)/usr/include/
	install -m 644 prebuilt/$(BUILD_NAME)/libswrtmesh.so $(STAGEDIR)/usr/lib
endif

.PHONY: all install clean stage
