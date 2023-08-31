include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
subtarget:=$(CONFIG_TARGET_BOARD)

QCAMCS_MAKE_OPTS+= \
        CROSS=$(TARGET_CROSS) \
        GWLIB=$(STAGING_DIR)/usr/lib \
        MCSTOP=$(PKG_BUILD_DIR) \
        GWINCLUDE=$(STAGING_DIR)/usr/include/hyficommon \
        ATHDIR=$(STAGING_DIR)/usr/include \
        KMCDIR=$(STAGING_DIR)/usr/include/qca-mcs \
        QCASSDKDIR=$(STAGING_DIR)/usr/include/qca-ssdk \
        INSTALL_ROOT=$(PKG_BUILD_DIR)/install \
        STAGING_DIR=$(STAGING_DIR) \
		TARGET_CFLAGS="$(CFLAGS)" \
		TARGET_LDFLAGS="$(LDFLAGS) -lnl-3 -lnl-genl-3"


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-mcs-apps/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-mcs-apps source
	mkdir -p $(PKG_BUILD_DIR)/install/sbin
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCAMCS_MAKE_OPTS)) subdirs
	cp -rf prebuilt/mcst $(PKG_BUILD_DIR)/install/sbin
else
	mkdir -p $(PKG_BUILD_DIR)/install/sbin
	cp -rf prebuilt/* $(PKG_BUILD_DIR)/install/sbin
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/install/sbin/mcsd $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/install/sbin/mcsctl $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/install/sbin/mcst $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/mcsd
	@$(STRIP) $(INSTALLDIR)/usr/sbin/mcsctl

clean:

stage:

