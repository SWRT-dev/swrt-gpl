include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
subtarget:=$(CONFIG_TARGET_BOARD)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/emesh-sp/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/emesh-sp source
	$(MAKE) -C "$(LINUXDIR)" CROSS_COMPILE="$(TARGET_CROSS)" ARCH="$(ARCH)" M="$(PKG_BUILD_DIR)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/emesh-sp.ko $(PKG_BUILD_DIR)
endif

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/emesh-sp.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/emesh-sp.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/emesh-sp/*.c),)
	cp -rf $(PKG_BUILD_DIR)/sp_api.h  $(STAGING_DIR)/usr/include/
endif
