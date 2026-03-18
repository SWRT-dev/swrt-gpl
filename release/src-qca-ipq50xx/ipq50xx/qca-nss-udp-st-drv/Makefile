# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

EXTRA_CFLAGS+= \
	-I$(SDK_DIR_PLATFORM)/qca-nss-udp-st-drv/exports

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-udp-st-drv/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-nss-udp-st-drv source
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/nss-udp-st.ko $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	install -D $(PKG_BUILD_DIR)/nss-udp-st.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-nss-udp-st-drv/Makefile),)
	mkdir -p $(STAGING_DIR)/usr/include/nss-udp-st-drv
	cp -rf $(PKG_BUILD_DIR)/exports/* $(STAGING_DIR)/usr/include/nss-udp-st-drv/
endif

