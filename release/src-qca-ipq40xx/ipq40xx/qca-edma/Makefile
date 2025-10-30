# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CONFIGURE_OPTS+= KERNEL_SRC=$(LINUXDIR)
EXTRA_CFLAGS+= -DSWRT_PATCH
EXTRA_CFLAGS+= -I$(PKG_BUILD_DIR)
KERNEL_MAKEOPTS+= EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/*.c),)
	$(KERNEL_MAKE) $(PARALLEL_BUILD) M="$(PKG_BUILD_DIR)" modules
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/*.ko $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@install -D $(PKG_BUILD_DIR)/essedma.ko $(INSTALLKMODDIR)
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:

