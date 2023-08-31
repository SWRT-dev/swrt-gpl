include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
#TARGET_CROSS=$(shell basename $(CROSS_COMPILE))
TARGET_CROSS=$(CROSS_COMPILE)
LINUX_VERSION=$(LINUX_KERNEL)
subtarget:=$(CONFIG_TARGET_BOARD)

QCA_THERMALD_MAKE_OPTS:= \
        CROSS=$(TARGET_CROSS) \
        THERMALDIR=$(PKG_BUILD_DIR) \


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-thermald/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-thermald source
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCA_THERMALD_MAKE_OPTS)) LDFLAGS="$(TARGET_LDFLAGS)"
else
	mkdir -p $(PKG_BUILD_DIR)/install/usr/sbin
	cp prebuilt/thermald $(PKG_BUILD_DIR)/install/usr/sbin
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/etc/thermal ] || install -d $(INSTALLDIR)/etc/thermal
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -m 644 -D prebuilt/ipq-thermald-8064.conf $(INSTALLDIR)/etc/thermal
	@install -D $(PKG_BUILD_DIR)/install/usr/sbin/thermald $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/thermald

clean:

stage:
