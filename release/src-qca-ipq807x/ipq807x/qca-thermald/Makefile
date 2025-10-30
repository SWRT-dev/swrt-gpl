# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

subtarget:=$(CONFIG_TARGET_BOARD)

QCA_THERMALD_MAKE_OPTS:= \
        CROSS=$(TARGET_CROSS) \
        THERMALDIR=$(PKG_BUILD_DIR) \


all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-thermald/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-thermald source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCA_THERMALD_MAKE_OPTS)) $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)/install/usr/sbin
	cp prebuilt/$(BUILD_NAME)/thermald $(PKG_BUILD_DIR)/install/usr/sbin
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
