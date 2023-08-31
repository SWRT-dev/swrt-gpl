include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)

PKG_BUILD_DEPENDS :=
MAKE_OPTS:=


EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-ssdk \
	-I$(STAGING_DIR)/usr/include/qca-ssdk/fal \
	-I$(STAGING_DIR)/usr/include/qca-ssdk/init \
	-I$(STAGING_DIR)/usr/include/nat46 \

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-ppe-bridge-mgr),)
MAKE_OPTS+=bridge-mgr=y
# enable OVS bridge if ovsmgr is enabled
ifneq ($(CONFIG_PACKAGE_kmod-qca-ovsmgr),)
MAKE_OPTS+=NSS_PPE_BRIDGE_MGR_OVS_ENABLE=y
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-ovsmgr
endif
# disable FDB learning for open profile
ifeq ($(CONFIG_TARGET_ipq95xx_generic_QSDK_Open)$(CONFIG_TARGET_ipq95xx_ipq95xx_32_QSDK_Open)$(CONFIG_TARGET_ipq53xx_generic_QSDK_Open)$(CONFIG_TARGET_ipq53xx_ipq53xx_32_QSDK_Open), y)
EXTRA_CFLAGS+=-DNSS_PPE_BRIDGE_MGR_FDB_DISABLE
endif

endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-ppe-vlan-mgr),)
MAKE_OPTS+=vlan-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-ppe-pppoe-mgr),)
MAKE_OPTS+=pppoe-mgr=y
endif

ifneq ($(CONFIG_PACKAGE_kmod-qca-nss-ppe-lag-mgr),)
MAKE_OPTS+=lag-mgr=y
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
KBUILD_EXTRA_SYMBOLS += $(SRCBASE)/$(PLATFORM_ROUTER)/nat46/source/nat46/modules/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(SDK_DIR_PLATFORM)/qca-ssdk/Module.symvers

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-ppe/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-ppe source
	$(MAKE) -C "$(LINUXDIR)" $(strip $(MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC="$(subtarget)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/drv
	mkdir -p $(PKG_BUILD_DIR)/drv/ppe_drv
	mkdir -p $(PKG_BUILD_DIR)/clients
	mkdir -p $(PKG_BUILD_DIR)/clients/bridge
	mkdir -p $(PKG_BUILD_DIR)/clients/vlan
	mkdir -p $(PKG_BUILD_DIR)/clients/pppoe
	mkdir -p $(PKG_BUILD_DIR)/clients/lag
	cp prebuilt/qca-nss-ppe.ko $(PKG_BUILD_DIR)/drv/ppe_drv
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/drv/ppe_drv/qca-nss-ppe.ko $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/clients/bridge/qca-nss-ppe-bridge-mgr.ko $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/clients/vlan/qca-nss-ppe-vlan.ko $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/clients/pppoe/qca-nss-ppe-pppoe-mgr.ko $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/clients/lag/qca-nss-ppe-lag.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-ppe/Makefile),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-ppe
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/qca-nss-ppe/
	cp -rf $(PKG_BUILD_DIR)/drv/exports/* $(STAGING_DIR)/usr/include/qca-nss-ppe/
endif

