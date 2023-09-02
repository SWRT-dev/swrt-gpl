include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
subtarget:=$(CONFIG_TARGET_BOARD)

ifneq (, $(findstring $(subtarget), "ipq807x" "ipq807x_64" "ipq60xx" "ipq60xx_64" "ipq50xx" "ipq50xx_64""ipq95xx" "ipq95xx_32"))
  CFI_OCF_DIR:=ocf/v2.0
  CFI_CRYPTOAPI_DIR:=cryptoapi/v2.0
else
  CFI_CRYPTOAPI_DIR:=cryptoapi/v1.1
  CFI_OCF_DIR:=ocf/v1.0
  CFI_IPSEC_DIR:=ipsec/v1.0
endif

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(LINUXDIR)/crypto/ocf \
	-I$(STAGING_DIR)/usr/include/qca-nss-crypto \
	-I$(STAGING_DIR)/usr/include/crypto \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv

ifneq (, $(findstring $(subtarget), "ipq95xx" "ipq95xx_32" "ipq807x" "ipq807x_64" "ipq60xx" "ipq60xx_64" "ipq50xx" "ipq50xx_64"))
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-clients
endif

# Build individual packages if selected
ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-cfi-cryptoapi),)
MAKE_OPTS+=cryptoapi=y
endif

#ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-cfi-ocf),)
#MAKE_OPTS+=ocf=y
#endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-nss-drv/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-nss-crypto/Module.symvers

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-cfi/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-cfi source
	$(MAKE) -C "$(LINUXDIR)" $(strip $(MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" CC="$(TARGET_CC)" CFI_CRYPTOAPI_DIR=$(CFI_CRYPTOAPI_DIR) CFI_OCF_DIR=$(CFI_OCF_DIR) \
		CFI_IPSEC_DIR=$(CFI_IPSEC_DIR) SoC=$(subtarget) KBUILD_EXTRA_SYMBOLS=$(KBUILD_EXTRA_SYMBOLS) modules
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/$(CFI_CRYPTOAPI_DIR)
#	mkdir -p $(PKG_BUILD_DIR)/$(CFI_OCF_DIR)
ifdef CFI_IPSEC_DIR
	mkdir -p $(PKG_BUILD_DIR)/$(CFI_IPSEC_DIR)
	cp -rf prebuilt/qca-nss-ipsec.ko $(PKG_BUILD_DIR)/$(CFI_IPSEC_DIR)
endif
	cp -rf prebuilt/qca-nss-cfi-cryptoapi.ko $(PKG_BUILD_DIR)/$(CFI_CRYPTOAPI_DIR)
#	cp -rf prebuilt/qca-nss-cfi-ocf.ko $(PKG_BUILD_DIR)/$(CFI_OCF_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/$(CFI_CRYPTOAPI_DIR)/qca-nss-cfi-cryptoapi.ko $(INSTALLKMODDIR)
#	@install -D $(PKG_BUILD_DIR)/$(CFI_OCF_DIR)/qca-nss-cfi-ocf.ko $(INSTALLKMODDIR)
ifdef CFI_IPSEC_DIR
	@install -D $(PKG_BUILD_DIR)/$(CFI_IPSEC_DIR)/qca-nss-ipsec.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-ipsec.ko
endif
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-cfi-cryptoapi.ko
#	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-cfi-ocf.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-cfi/Makefile),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-cfi
	cp -rf $(PKG_BUILD_DIR)/$(CFI_CRYPTOAPI_DIR)/../exports/* $(STAGING_DIR)/usr/include/qca-nss-cfi
	cp -rf $(PKG_BUILD_DIR)/include/* $(STAGING_DIR)/usr/include/qca-nss-cfi
endif
