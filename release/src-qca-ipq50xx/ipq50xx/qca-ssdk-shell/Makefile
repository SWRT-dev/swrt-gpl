# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

QCASSDK_CONFIG_OPTS+= TOOL_PATH=/ \
                SYS_PATH=$(LINUXDIR) \
                TOOLPREFIX=$(TARGET_CROSS) \
                KVER=$(LINUX_VERSION) \
                ARCH=$(ARCH)

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-ssdk-shell/src/api/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-ssdk-shell source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCASSDK_CONFIG_OPTS)) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)/build
	mkdir -p $(PKG_BUILD_DIR)/build/bin
	cp prebuilt/$(BUILD_NAME)/ssdk_sh $(PKG_BUILD_DIR)/build/bin
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -D $(PKG_BUILD_DIR)/build/bin/ssdk_sh $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/ssdk_sh

clean:

stage:

