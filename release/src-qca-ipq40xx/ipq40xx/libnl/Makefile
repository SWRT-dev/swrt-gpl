include ../qsdk.config

# Convert asuswrt build environment variable to libnl
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source
LIBNL_HDRS:=$(wildcard $(PKG_BUILD_DIR)/include/*)

TARGET_CFLAGS += -ffunction-sections $(FPIC)
TARGET_LDFLAGS += -pie

all: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/Makefile: $(PKG_BUILD_DIR)/configure
	$(MAKE) $(PKG_BUILD_DIR)-configure

$(PKG_BUILD_DIR)/configure:
	( cd $(PKG_BUILD_DIR) ; ./autogen.sh )

$(PKG_BUILD_DIR)-configure:
	( cd $(PKG_BUILD_DIR) ; $(CONFIGURE) --prefix=/usr --bindir=/usr/sbin --libdir=/usr/lib --sysconfdir=/etc )

install: stage
	[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	install -m 755 $(STAGEDIR)/usr/lib/libnl-route-3.so.* $(INSTALLDIR)/usr/lib/
	install -m 755 $(STAGEDIR)/usr/lib/libnl-genl-3.so.* $(INSTALLDIR)/usr/lib/
	install -m 755 $(STAGEDIR)/usr/lib/libnl-nf-3.so.* $(INSTALLDIR)/usr/lib/
	install -m 755 $(STAGEDIR)/usr/lib/libnl-3.so.* $(INSTALLDIR)/usr/lib/

clean:
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f

stage: $(LIBNL_HDRS)
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(STAGEDIR)" install
	install -d $(STAGEDIR)/usr/include/libnl
	cd $(PKG_BUILD_DIR)/include && rsync -avcH --delete . $(STAGEDIR)/usr/include/libnl/
	@touch $@
