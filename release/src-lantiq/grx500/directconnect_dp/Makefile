# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXTRA_CFLAGS += -fno-pic -mno-abicalls
TARGET_CFLAGS += -fno-pic -mno-abicalls
TARGET_CXXFLAGS += -fno-pic -mno-abicalls
TARGET_ASFLAGS_DEFAULT += -fno-pic -mno-abicalls
TARGET_CPPFLAGS += -fno-pic -mno-abicalls

EXTRA_CFLAGS += -DCONFIG_DIRECTCONNECT_DP_DBG
EXTRA_CFLAGS += -I$(PKG_BUILD_DIR)/include

ifeq ($(CONFIG_PACKAGE_kmod-ppa-drv),y)
	EXTRA_CFLAGS += -DCONFIG_DIRECTCONNECT_DP_LITEPATH
	PKG_EXTRA_KCONFIG += CONFIG_DIRECTCONNECT_DP_LITEPATH=y
endif

ifeq ($(CONFIG_TARGET_intel_mips_xrx500),y)
	EXTRA_CFLAGS += -DHAVE_DATAPATH_API
	PKG_EXTRA_KCONFIG += CONFIG_DIRECTCONNECT_DP_XRX500=y
endif

ifeq ($(CONFIG_TARGET_intel_x86_lgm),y)
	EXTRA_CFLAGS += -DHAVE_DATAPATH_EXT_API
	PKG_EXTRA_KCONFIG += CONFIG_DIRECTCONNECT_DP_LGM=y
endif

ifeq ($(CONFIG_TARGET_intel_mips_prx300),y)
	EXTRA_CFLAGS += -DHAVE_DATAPATH_EXT_API
	PKG_EXTRA_KCONFIG += CONFIG_DIRECTCONNECT_DP_PRX300=y
endif

KERNEL_MAKEOPTS += \
	$(PKG_EXTRA_KCONFIG) \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
	$(KERNEL_MAKE) $(PARALLEL_BUILD) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" M="$(PKG_BUILD_DIR)" modules
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/common/directconnect_datapath.ko $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/dc_mode/dc_mode-hw.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
	@[ -d $(STAGINGDIR)/include/net/ ] || install -d $(STAGEDIR)/include/net/
	$(CP) $(PKG_BUILD_DIR)/include/*.h $(STAGEDIR)/include/net/


