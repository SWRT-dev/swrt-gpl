# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

subtarget:=$(CONFIG_TARGET_BOARD)
INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/emesh-sp/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/emesh-sp source
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/emesh-sp.ko $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/emesh-sp.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/emesh-sp.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/emesh-sp/*.c),)
	cp -rf $(PKG_BUILD_DIR)/sp_api.h  $(STAGING_DIR)/usr/include/
endif
