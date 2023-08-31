include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
TARGET_CC=$(CROSS_COMPILE)gcc
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
LINUX_VERSION=$(LINUX_KERNEL)

QCASSDK_CONFIG_OPTS+= TOOL_PATH=/ \
                SYS_PATH=$(LINUXDIR) \
                TOOLPREFIX=$(TARGET_CROSS) \
                KVER=$(LINUX_VERSION) \
                CFLAGS="$(CFLAGS)" \
                LDFLAGS="$(LDFLAGS)" \
                ARCH=$(ARCH)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ssdk-shell/src/api/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-ssdk-shell source
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCASSDK_CONFIG_OPTS))
else
	mkdir -p $(PKG_BUILD_DIR)/build
	mkdir -p $(PKG_BUILD_DIR)/build/bin
	cp prebuilt/ssdk_sh $(PKG_BUILD_DIR)/build/bin
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/build/bin/ssdk_sh $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/ssdk_sh

clean:
