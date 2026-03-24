# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -DHAVE_C99=1 -DCONFIG_UBOOT_CONFIG_ENV_SIZE=0x4000 -DSWRT_PATCH \
	-DARCH_MIPS -DCONFIG_GCC_VERSION=\\\"8.3.0\\\" -DCONFIG_LIBC_VERSION=\\\"1.1.21\\\"

all:
ifneq ($(wildcard $(PKG_BUILD_DIR)/*.c),)
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) SWRT_PATCH=y
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/$(BUILD_NAME)/* $(PKG_BUILD_DIR)/
endif
	touch stamp-h1


install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -m 755 $(PKG_BUILD_DIR)/read_img $(INSTALLDIR)/usr/sbin/
	@install -m 755 $(PKG_BUILD_DIR)/uboot_env $(INSTALLDIR)/usr/sbin/
	@install -m 755 $(PKG_BUILD_DIR)/upgrade $(INSTALLDIR)/usr/sbin/
	@install -m 755 vol_mgmt $(INSTALLDIR)/usr/sbin/
	$(STRIP) $(INSTALLDIR)/usr/sbin/read_img
	$(STRIP) $(INSTALLDIR)/usr/sbin/uboot_env
	$(STRIP) $(INSTALLDIR)/usr/sbin/upgrade

clean:

stage:

