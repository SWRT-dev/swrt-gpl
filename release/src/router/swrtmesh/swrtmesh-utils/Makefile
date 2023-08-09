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
	[ -d $(INSTALLDIR)/lib/wifi ] || install -d $(INSTALLDIR)/lib/wifi
ifneq ($(wildcard source/Makefile),)
	install -m 755 $(PKG_BUILD_DIR)/multiap $(INSTALLDIR)/usr/sbin/
	install -m 755 $(PKG_BUILD_DIR)/libswrtmesh.so $(INSTALLDIR)/usr/lib/
else
	install -m 755 prebuilt/$(BUILD_NAME)/multiap $(INSTALLDIR)/usr/sbin/
	install -m 755 prebuilt/$(BUILD_NAME)/libswrtmesh.so $(INSTALLDIR)/usr/lib/
endif
	$(STRIP) $(INSTALLDIR)/usr/sbin/multiap
	$(STRIP) $(INSTALLDIR)/usr/lib/libswrtmesh.so
	cd $(INSTALLDIR)/lib/wifi && ln -sf /usr/sbin/multiap multiap

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
