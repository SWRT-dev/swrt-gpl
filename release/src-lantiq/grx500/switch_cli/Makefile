# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

CONFIGURE_ARGS += \
	--enable-kernelincl="$(LINUXDIR)/include/net/switch_api/" \
	--enable-gswip --enable-printhelp

all:
	cd $(PKG_BUILD_DIR) && autoreconf -i -f && $(CONFIGURE) $(CONFIGURE_ARGS)
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	touch stamp-h1


install:
	@[ -d $(INSTALLDIR)/usr/bin ] || install -d $(INSTALLDIR)/usr/bin
	@install -D $(PKG_BUILD_DIR)/src/switch_cli $(INSTALLDIR)/usr/bin
	$(STRIP) $(INSTALLDIR)/usr/bin/switch_cli

clean:

stage:

