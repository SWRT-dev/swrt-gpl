include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)
EXTRA_CFLAGS += -I$(LINUXDIR)/net/openvswitch -I$(PKG_BUILD_DIR)/exports

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ovsmgr/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-ovsmgr source
	$(MAKE) -C "$(LINUXDIR)" $(strip $(MAKE_OPTS)) CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" SoC="$(subtarget)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/qca-ovsmgr.ko $(PKG_BUILD_DIR)
endif

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

