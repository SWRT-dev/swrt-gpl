# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

subtarget:=$(CONFIG_TARGET_BOARD)
EXTRA_CFLAGS += -I$(LINUXDIR)/net/openvswitch -I$(PKG_BUILD_DIR)/exports

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ovsmgr/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-ovsmgr source
	$(KERNEL_MAKE) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC="$(subtarget)" M="$(PKG_BUILD_DIR)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/qca-ovsmgr.ko $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/qca-ovsmgr.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/qca-ovsmgr.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ovsmgr/*.c),)
	mkdir -p $(STAGING_DIR)/usr/include/qca-ovsmgr
	cp -rf $(PKG_BUILD_DIR)/exports/ovsmgr.h $(STAGING_DIR)/usr/include/qca-ovsmgr/
endif

