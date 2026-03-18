# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

subtarget:=$(CONFIG_TARGET_BOARD)

# v1.0 is for Akronite
# v2.0 is for Hawkeye/Cypress/Maple
ifneq (, $(findstring $(subtarget), "ipq95xx" "ipq95xx_32" "ipq807x" "ipq807x_64" "ipq60xx" "ipq60xx_64" "ipq50xx" "ipq50xx_64"))
NSS_CRYPTO_DIR:=v2.0
else
NSS_CRYPTO_DIR:=v1.0
endif

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(STAGING_DIR)/usr/include/qca-nss-crypto \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv \
	-I$(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/include \
	-I$(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/src

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-nss-drv/Module.symvers
export KBUILD_EXTRA_SYMBOLS


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-crypto/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-crypto source
	$(KERNEL_MAKE) $(PARALLEL_BUILD) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M="$(PKG_BUILD_DIR)" \
		NSS_CRYPTO_DIR=$(NSS_CRYPTO_DIR) SoC="$(subtarget)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)
	mkdir -p $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/src
	mkdir -p $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/tool
	cp prebuilt/$(BUILD_NAME)/qca-nss-crypto.ko $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/src
	cp prebuilt/$(BUILD_NAME)/qca-nss-crypto-tool.ko $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/tool
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/src/qca-nss-crypto.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/tool/qca-nss-crypto-tool.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-crypto.ko
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-crypto-tool.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-crypto/Makefile),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-crypto
	cp -rf $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/include/* $(STAGING_DIR)/usr/include/qca-nss-crypto
endif
