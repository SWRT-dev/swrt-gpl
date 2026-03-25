# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

CONFIGURE_ARGS += \
	--enable-kernelbuild="$(LINUX_DIR)" ARCH=$(ARCH)

ifeq ($(DEBUG),y)
CONFIGURE_ARGS += --enable-debug
endif

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)


all:
	cd $(PKG_BUILD_DIR) && $(CONFIGURE) $(CONFIGURE_ARGS)
	make -C $(PKG_BUILD_DIR) lint
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/src/ltq_temp.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:


