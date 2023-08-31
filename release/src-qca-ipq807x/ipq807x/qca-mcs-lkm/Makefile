include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)

QCA_MC_SNOOPING_MAKE_OPTS:= \
        CROSS_COMPILE=$(CROSS_COMPILE) \
        ARCH=$(ARCH) \
        KERNELPATH=$(LINUXDIR) \
        KBUILDPATH=$(LINUXDIR) \
        KERNELRELEASE=1 \
		CONFIG_SUPPORT_MLD=y \
        MDIR=$(PKG_BUILD_DIR) \
		STAGING_DIR=$(STAGING_DIR)

ifeq ($(CONFIG_PACKAGE_kmod-qca-ovsmgr),y)
QCA_MC_SNOOPING_MAKE_OPTS += CONFIG_SUPPORT_OVS=y
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-mcs/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-mcs source
	$(MAKE) -C $(LINUXDIR) M=$(PKG_BUILD_DIR) $(strip $(QCA_MC_SNOOPING_MAKE_OPTS))
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/qca-mcs.ko $(PKG_BUILD_DIR)
endif
	touch stamp-h1


install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/qca-mcs.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-mcs.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-mcs/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-mcs
	cp -rf $(PKG_BUILD_DIR)/mc_api.h $(STAGING_DIR)/usr/include/qca-mcs
	cp -rf $(PKG_BUILD_DIR)/mc_ecm.h $(STAGING_DIR)/usr/include/qca-mcs
endif
