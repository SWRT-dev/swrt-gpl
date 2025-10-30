# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source
subtarget:=$(CONFIG_TARGET_BOARD)

QCASSDKSHELL_CONFIG_OPTS:= \
	TOOL_PATH=$(STAGING_DIR)/bin \
	SYS_PATH=$(LINUXDIR) \
	TOOLPREFIX=$(TARGET_CROSS) \
	KVER=$(LINUX_KERNEL) \
	ARCH=$(ARCH)

all:
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCASSDKSHELL_CONFIG_OPTS)) $(TARGET_CONFIGURE_OPTS)
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/build/bin/ssdk_sh $(INSTALLDIR)/usr/sbin/ssdk_sh
	$(STRIP) $(INSTALLDIR)/usr/sbin/ssdk_sh

clean:

stage:

