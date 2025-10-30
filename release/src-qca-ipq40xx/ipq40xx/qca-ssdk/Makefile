# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXTRA_CFLAGS+= -DSWRT_PATCH
KERNEL_MAKEOPTS+= EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

QCASSDK_CONFIG_OPTS:= \
	PTP_FEATURE=disable \
	HNAT_FEATURE=enable \
	TOOL_PATH=$(TOOLCHAIN_DIR)/bin \
	SYS_PATH=$(LINUXDIR) \
	TOOLPREFIX=$(TARGET_CROSS) \
	KVER=$(LINUX_VERSION) \
	ARCH=$(ARCH) \
	TARGET_SUFFIX=$(CONFIG_TARGET_SUFFIX) \
	GCC_VERSION=$(TOOLCHAIN_TARGET_GCCVER) \
	MINI_SSDK=disable \
	OS_VER=4_4 \
	HK_CHIP=enable #RFS_FEATURE=enable
QCASSDK_CONFIG_OPTS+=SWRT_PATCH=y
QCASSDK_CONFIG_OPTS+=CFLAGS=-I$(STAGING_DIR)/usr/include

all:
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(strip $(QCASSDK_CONFIG_OPTS)) $(TARGET_CONFIGURE_OPTS)
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
	$(CP) $(PKG_BUILD_DIR)/include/api/sw_ioctl.h $(STAGING_DIR)/usr/include/qca-ssdk/api
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_vsi.h ]; then \
	$(CP) $(PKG_BUILD_DIR)/include/ref/ref_vsi.h $(STAGING_DIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_fdb.h ]; then \
	$(CP) $(PKG_BUILD_DIR)/include/ref/ref_fdb.h $(STAGING_DIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/ref/ref_port_ctrl.h ]; then \
	$(CP) $(PKG_BUILD_DIR)/include/ref/ref_port_ctrl.h $(STAGING_DIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/init/ssdk_init.h ]; then \
	$(CP) $(PKG_BUILD_DIR)/include/init/ssdk_init.h $(STAGING_DIR)/usr/include/qca-ssdk/init/; \
	fi
	if [ -f $(PKG_BUILD_DIR)/include/init/ssdk_netlink.h ]; then \
	$(CP) $(PKG_BUILD_DIR)/include/init/ssdk_netlink.h $(STAGING_DIR)/usr/include/qca-ssdk/init/; \
	fi
	$(CP) $(PKG_BUILD_DIR)/include/fal $(STAGING_DIR)/usr/include/qca-ssdk
	$(CP) $(PKG_BUILD_DIR)/include/common/*.h $(STAGING_DIR)/usr/include/qca-ssdk
	$(CP) $(PKG_BUILD_DIR)/include/sal/os/linux/*.h $(STAGING_DIR)/usr/include/qca-ssdk
	$(CP) $(PKG_BUILD_DIR)/include/sal/os/*.h $(STAGING_DIR)/usr/include/qca-ssdk
endif

