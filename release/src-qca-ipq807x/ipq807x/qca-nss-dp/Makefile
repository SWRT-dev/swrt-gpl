include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-ssdk \
	-I$(STAGING_DIR)/usr/include/qca-nss-ppe

ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
EXTRA_CFLAGS+= -DNSS_DP_MEM_PROFILE_LOW
else ifeq ($(CONFIG_LOWMEM_FLASH),y)
EXTRA_CFLAGS+= -DNSS_DP_MEM_PROFILE_LOW
else ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),512)
EXTRA_CFLAGS+= -DNSS_DP_MEM_PROFILE_MEDIUM
endif

ifeq ($(CONFIG_TARGET_ipq95xx_generic_QSDK_MinEnt), y)
EXTRA_CFLAGS+= -DNSS_DP_POINT_OFFLOAD
endif
ifneq ($(CONFIG_TARGET_ipq95xx)$(CONFIG_TARGET_ipq53xx),)
MAKE_OPTS+=dp-ppe-ds=y
endif
subtarget:=$(CONFIG_TARGET_BOARD)

NSS_DP_HAL_DIR:=$(PKG_BUILD_DIR)/hal
ifneq (, $(findstring _64, $(subtarget)))
hal_arch:=$(subst _64,,$(subtarget))
else ifneq (, $(findstring _32, $(subtarget)))
hal_arch:=$(subst _32,,$(subtarget))
else
hal_arch:=$(subtarget)
endif
INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-dp/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-dp source
	ln -sf $(NSS_DP_HAL_DIR)/soc_ops/$(hal_arch)/nss_$(hal_arch).h $(PKG_BUILD_DIR)/exports/nss_dp_arch.h
	cp $(SDK_DIR_PLATFORM)/qca-ssdk/Module.symvers $(PKG_BUILD_DIR)
	$(MAKE) -C "$(LINUXDIR)" $(strip $(MAKE_OPTS))	CROSS_COMPILE="$(CROSS_COMPILE)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC="$(hal_arch)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/qca-nss-dp.ko $(PKG_BUILD_DIR)
endif

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/qca-nss-dp.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-nss-dp.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-dp/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-nss-dp
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/qca-nss-dp/
endif
