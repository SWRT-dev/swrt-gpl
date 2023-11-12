include $(TOP)/common.mak
include $(SRCBASE)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source
TARGET_CFLAGS:= \
	-I$(STAGEDIR)/usr/include/libnl-tiny \
	-DCONFIG_LIBNL20 \
	-D_GNU_SOURCE \
	-flto -DIW_FULL

TARGET_LDFLAGS:= \
	-Wl,--gc-sections -flto -L$(STAGEDIR)/usr/lib

all:
	echo "const char iw_version[] = \"5.9-8fab0c9e\";" > $(PKG_BUILD_DIR)/version.c
	echo "#!/bin/sh" > $(PKG_BUILD_DIR)/version.sh
	chmod +x $(PKG_BUILD_DIR)/version.sh
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)" TARGET_LDFLAGS="$(TARGET_LDFLAGS)" IW_FULL=1 NL1FOUND="" NL2FOUND=Y NLLIBNAME="libnl-tiny" LIBS="-lm -lnl-tiny" V=1

install:
	install -D  $(PKG_BUILD_DIR)/iw $(INSTALLDIR)/usr/sbin/iw
	$(STRIP) $(INSTALLDIR)/usr/sbin/iw

clean:

