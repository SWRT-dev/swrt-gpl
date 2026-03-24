# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

PKG_EXTRA_CFLAGS += -DPPA_SS_VERSION='\"ugw_8.5.2.10\"'
ifneq ($(CONFIG_PACKAGE_kmod-ppa-drv-accel),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PPA_ACCEL
  MAKE_OPTS += CONFIG_PPA_ACCEL=m
endif

ifeq ($(CONFIG_TARGET_intel_x86_lgm),y)
PKG_EXTRA_CFLAGS += -DWMM_QOS_CONFIG=1
endif
ifeq ($(CONFIG_TARGET_intel_mips),y)
ifneq ($(CONFIG_PACKAGE_kmod-ppa-drv-grx500),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PPA_ACCEL -DCONFIG_PPA_TMU_HAL
  MAKE_OPTS += CONFIG_PPA_ACCEL=m CONFIG_PPA_TMU_HAL=m
endif
ifneq ($(CONFIG_PACKAGE_kmod-ppa-drv-grx500-mpe),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PPA_MPE_HAL -DMIB_MODE_ENABLE
  MAKE_OPTS += CONFIG_PPA_MPE_HAL=m
endif
ifneq ($(CONFIG_PACKAGE_kmod-ppa-drv-prx300-mpe),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PPA_MPE_HAL
  MAKE_OPTS += CONFIG_PPA_MPE_HAL=m
endif
ifneq ($(CONFIG_PACKAGE_kmod-litepath-hwacc),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PPA_LITEPATH_HWACC
  MAKE_OPTS += CONFIG_PPA_LITEPATH_HWACC=m
endif
ifneq ($(CONFIG_PACKAGE_kmod-mac-violation-mirror),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PPA_MAC_VIOLATION_MIRROR
  MAKE_OPTS += CONFIG_PPA_MAC_VIOLATION_MIRROR=m
endif
ifneq ($(CONFIG_PACKAGE_kmod-qos-mgr-drv),)
ifneq ($(CONFIG_TARGET_intel_mips_prx300),)
  PKG_EXTRA_CFLAGS += -DCONFIG_QOS_MGR_TC_SUPPORT
  MAKE_OPTS += CONFIG_QOS_MGR_TC_SUPPORT=m
endif
endif
ifneq ($(CONFIG_PACKAGE_kmod-atm_bond_drv),)
  MAKE_OPTS += CONFIG_PPA_ATM_BONDING=m
endif
ifneq ($(CONFIG_PACKAGE_atm_bond_cli),)
  PKG_EXTRA_CFLAGS += -I$(PKG_BUILD_DIR)/src/bonding/include/common
  PKG_EXTRA_CFLAGS += -DENABLE_IOCTL_DEBUG
endif
ifneq ($(CONFIG_PACKAGE_kmod-ppa-drv-prx300-gw),)
  PKG_EXTRA_CFLAGS += -DCONFIG_PRX300_HGU
  MAKE_OPTS += CONFIG_PRX300_HGU=m
endif
ifeq ($(CONFIG_TARGET_intel_mips_xrx500),y)
  PKG_EXTRA_CFLAGS += -DPLATFORM_XRX500
  export PLATFORM_XRX500=1
endif
PKG_EXTRA_CFLAGS += -Wno-uninitialized -Wno-switch-unreachable
endif

TARGET_CFLAGS += $(PKG_EXTRA_CFLAGS) -DCONFIG_PPA \
	-I$(LINUX_DIR)/include/ -I$(TOP)/kernel_header/include

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
	$(KERNEL_MAKE) $(PARALLEL_BUILD) $(MAKE_OPTS) EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" M="$(PKG_BUILD_DIR)/src" modules
	$(TARGET_MAKE) $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR)/tools/ppacmd all
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/lib/firmware ] || install -d $(INSTALLDIR)/lib/firmware
	@install -m 755 $(PKG_BUILD_DIR)/tools/ppacmd/ppacmd $(INSTALLDIR)/usr/sbin/
	@install -D $(PKG_BUILD_DIR)/src/ppa_api/ppa_api.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/ppa_api/ppa_api_sw_accel_mod.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/ppa_api/ppa_drv_stack_al.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/platform/xrx500/ltq_pae_hal.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/platform/xrx500/ltq_tmu_hal_drv.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/platform/xrx500/ltq_mpe_hal_drv.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/platform/xrx500/mpe_fw_be.img $(INSTALLDIR)/lib/firmware/
	@install -D $(PKG_BUILD_DIR)/src/mac_violation_mirror/mac_violation_mirror.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/platform/xrx500/hw_tcp_litepath.ko $(INSTALLKMODDIR)
	@$(STRIP) $(INSTALLDIR)/usr/sbin/ppacmd
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:


