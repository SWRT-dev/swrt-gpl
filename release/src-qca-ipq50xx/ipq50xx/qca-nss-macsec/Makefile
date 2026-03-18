# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

subtarget:=$(CONFIG_TARGET_BOARD)

QCA_NSS_MACSEC_CONFIG_OPTS+= TOOL_PATH=/ \
                SYS_PATH=$(LINUXDIR) \
                TOOLPREFIX=$(TARGET_CROSS) \
                KVER=$(LINUX_VERSION) \
                ARCH=$(ARCH)

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-macsec/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-macsec source
	$(KERNEL_MAKE) $(PARALLEL_BUILD) M="$(PKG_BUILD_DIR)" SoC="$(subtarget)" modules
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) \
		$(strip $(QCA_NSS_MACSEC_CONFIG_OPTS)) -f Makefile.shell
else
	mkdir -p $(PKG_BUILD_DIR)
	cp -rf prebuilt/$(BUILD_NAME)/* $(PKG_BUILD_DIR)
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
