include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
LINUX_VERSION=$(LINUX_KERNEL)
subtarget:=$(CONFIG_TARGET_BOARD)

QCA_NSS_MACSEC_CONFIG_OPTS+= TOOL_PATH=/ \
                SYS_PATH=$(LINUXDIR) \
                TOOLPREFIX=$(TARGET_CROSS) \
                KVER=$(LINUX_VERSION) \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
                ARCH=$(ARCH)

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-macsec/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-macsec source
	$(MAKE) -C "$(LINUXDIR)" CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" SoC="$(subtarget)" modules
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCA_NSS_MACSEC_CONFIG_OPTS)) -f Makefile.shell
else
	mkdir -p $(PKG_BUILD_DIR)
	cp -rf prebuilt/* $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/qca-nss-macsec.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/libfal.so $(INSTALLDIR)/usr/lib
	@install -D $(PKG_BUILD_DIR)/macsec_shell $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/macsec_shell
	@$(STRIP) $(INSTALLDIR)/usr/lib/libfal.so
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-macsec.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-macsec/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-macsec
	cp -rf $(PKG_BUILD_DIR)/include/nss_macsec_types.h $(STAGING_DIR)/usr/include/qca-nss-macsec
	cp -rf $(PKG_BUILD_DIR)/include/nss_macsec_secy.h $(STAGING_DIR)/usr/include/qca-nss-macsec
	cp -rf $(PKG_BUILD_DIR)/include/nss_macsec_secy_rx.h $(STAGING_DIR)/usr/include/qca-nss-macsec
	cp -rf $(PKG_BUILD_DIR)/include/nss_macsec_secy_tx.h $(STAGING_DIR)/usr/include/qca-nss-macsec
	cp -rf $(PKG_BUILD_DIR)/libfal.so $(STAGING_DIR)/usr/lib
endif
