include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)

QCAHYFI_BRIDGE_MAKE_OPTS:= \
        CROSS_COMPILE=$(CROSS_COMPILE) \
        ARCH=$(ARCH) \
        HYBRID_MC_MLD=1 \
        KERNELPATH=$(LINUXDIR) \
        KBUILDPATH=$(LINUXDIR) \
        KERNELRELEASE=1 \
        MDIR=$(PKG_BUILD_DIR) \
        STAGING_DIR=$(STAGING_DIR)

ifeq ($(CONFIG_PACKAGE_qca-plc-serv),y)
        QCAHYFI_BRIDGE_MAKE_OPTS += PLC_SUPPORT_NF=1
endif

ifeq ($(CONFIG_HYFI_DISABLE_SSDK_SUPPORT),y)
    QCAHYFI_BRIDGE_MAKE_OPTS+= DISABLE_SSDK_SUPPORT=1
endif

ifeq ($(CONFIG_HYD_SUPPORT_MULTICAST),y)
    QCAHYFI_BRIDGE_MAKE_OPTS+= HYFI_MULTICAST_SUPPORT=1
endif

ifneq ($(CONFIG_PACKAGE_kmod-emesh-sp),)
    QCAHYFI_BRIDGE_MAKE_OPTS += HYFI_BRIDGE_EMESH_ENABLE=1
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-hyfi-bridge/hyfi-netfilter/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-hyfi-bridge source
	cp $(SDK_DIR_PLATFORM)/emesh-sp/Module.symvers $(PKG_BUILD_DIR)
	cat $(SDK_DIR_PLATFORM)/qca-ssdk/Module.symvers >> $(PKG_BUILD_DIR)/Module.symvers
	$(MAKE) -C $(LINUXDIR) M=$(PKG_BUILD_DIR) $(strip $(QCAHYFI_BRIDGE_MAKE_OPTS))
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/hyfi-bridging.ko $(PKG_BUILD_DIR)
endif

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/hyfi-bridging.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/hyfi-bridging.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-hyfi-bridge/hyfi-netfilter/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/hyfibr
	cp -rf $(PKG_BUILD_DIR)/hyfi-multicast/mc_api.h $(STAGING_DIR)/usr/include/hyfibr
	cp -rf $(PKG_BUILD_DIR)/hyfi-netfilter/hyfi_ecm.h  $(STAGING_DIR)/usr/include/hyfibr
	cp -rf $(PKG_BUILD_DIR)/hyfi-netfilter/hyfi_api.h  $(STAGING_DIR)/usr/include/hyfibr
	cp -rf $(PKG_BUILD_DIR)/hyfi-netfilter/hyfi_hash.h  $(STAGING_DIR)/usr/include/hyfibr
endif
