# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

subtarget:=$(CONFIG_TARGET_BOARD)
ifneq (, $(findstring _64, $(subtarget)))
	hal_arch:=$(subst _64,,$(subtarget))
else ifneq (, $(findstring _32, $(subtarget)))
	hal_arch:=$(subst _32,,$(subtarget))
else
	hal_arch:=$(subtarget)
endif
ifneq (, $(findstring $(hal_arch), "ipq50xx"))
QCASSDK_CONFIG_OPTS+= HNAT_FEATURE=enable
endif
ifneq (, $(findstring $(hal_arch), "ipq40xx"))
QCASSDK_CONFIG_OPTS+= HNAT_FEATURE=enable
endif
ifneq (, $(findstring $(hal_arch), "ipq95xx"))
	QCASSDK_CONFIG_OPTS+= CHIP_TYPE=APPE
else ifneq (, $(findstring $(hal_arch), "ipq807x"))
	QCASSDK_CONFIG_OPTS+= CHIP_TYPE=HPPE
else ifneq (, $(findstring $(hal_arch), "ipq60xx"))
	QCASSDK_CONFIG_OPTS+= CHIP_TYPE=CPPE
else ifneq (, $(findstring $(hal_arch), "ipq50xx"))
	QCASSDK_CONFIG_OPTS+= CHIP_TYPE=MP
else ifneq (, $(findstring $(hal_arch), "ipq53xx"))
	QCASSDK_CONFIG_OPTS+= CHIP_TYPE=MPPE
endif
ifneq (, $(findstring $(CONFIG_KERNEL_IPQ_MEM_PROFILE), 256 512)$(CONFIG_LOWMEM_FLASH))
	QCASSDK_CONFIG_OPTS+= MINI_SSDK=enable
	#Disable MINI_SSDK on MP 512 profile
	ifneq (, $(findstring $(hal_arch), "ipq50xx"))
		ifneq (, $(findstring $(CONFIG_KERNEL_IPQ_MEM_PROFILE), 512))
			QCASSDK_CONFIG_OPTS+= MINI_SSDK=disable
		endif
	endif
	ifeq ($(CONFIG_LOWMEM_FLASH),y)
		QCASSDK_CONFIG_OPTS+= LOWMEM_FLASH=enable
	endif
endif
QCASSDK_CONFIG_OPTS+= \
			      TARGET_NAME=$(CONFIG_ARCH)-openwrt-linux$(if $(CONFIG_TARGET_SUFFIX),-$(CONFIG_TARGET_SUFFIX))
QCASSDK_CONFIG_OPTS+= TOOL_PATH=/ \
                SYS_PATH=$(LINUXDIR) \
                TOOLPREFIX=$(TARGET_CROSS) \
                KVER=$(LINUX_VERSION) \
                ARCH=$(ARCH) \
		TARGET_SUFFIX=$(CONFIG_TARGET_SUFFIX) \
		GCC_VERSION=$(CONFIG_GCC_VERSION) \
		SoC=$(hal_arch) \

ifneq (, $(findstring $(hal_arch), "ipq53xx"))
QCASSDK_CONFIG_OPTS += LNX_MAKEOPTS='$(KERNEL_MAKEOPTS)' MODULE_TYPE=KSLIB modules
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ssdk/src/api/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-ssdk source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(strip $(QCASSDK_CONFIG_OPTS)) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)/build
	mkdir -p $(PKG_BUILD_DIR)/build/bin
	cp prebuilt/$(BUILD_NAME)/qca-ssdk.ko $(PKG_BUILD_DIR)/build/bin
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/build/bin/qca-ssdk.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-ssdk.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ssdk/src/api/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-ssdk
	mkdir -p $(STAGING_DIR)/usr/include/qca-ssdk/api
	mkdir -p $(STAGING_DIR)/usr/include/qca-ssdk/ref
	mkdir -p $(STAGING_DIR)/usr/include/qca-ssdk/fal
	mkdir -p $(STAGING_DIR)/usr/include/qca-ssdk/sal
	mkdir -p $(STAGING_DIR)/usr/include/qca-ssdk/init
	cp -rf $(PKG_BUILD_DIR)/include/api/sw_ioctl.h $(STAGING_DIR)/usr/include/qca-ssdk/api
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_vsi.h ]; then \
	cp -rf $(PKG_BUILD_DIR)/include/ref/ref_vsi.h $(STAGING_DIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_fdb.h ]; then \
	cp -rf $(PKG_BUILD_DIR)/include/ref/ref_fdb.h $(STAGING_DIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_port_ctrl.h ]; then \
	cp -rf $(PKG_BUILD_DIR)/include/ref/ref_port_ctrl.h $(STAGING_DIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/init/ssdk_init.h ]; then \
	cp -rf $(PKG_BUILD_DIR)/include/init/ssdk_init.h $(STAGING_DIR)/usr/include/qca-ssdk/init/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/init/ssdk_netlink.h ]; then \
	cp -rf $(PKG_BUILD_DIR)/include/init/ssdk_netlink.h $(STAGING_DIR)/usr/include/qca-ssdk/init/; \
	fi
	cp -rf $(PKG_BUILD_DIR)/include/fal $(STAGING_DIR)/usr/include/qca-ssdk
	cp -rf $(PKG_BUILD_DIR)/include/common/*.h $(STAGING_DIR)/usr/include/qca-ssdk
	cp -rf $(PKG_BUILD_DIR)/include/sal/os/linux/*.h $(STAGING_DIR)/usr/include/qca-ssdk
	cp -rf $(PKG_BUILD_DIR)/include/sal/os/*.h $(STAGING_DIR)/usr/include/qca-ssdk
endif
