include ../qsdk.config
# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include
ifeq ($(CONFIG_LINUX_5_4),y)
TARGET_CFLAGS += TARGET_USES_NO_FW_QMI_CLIENT:=true
endif

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss-daemon/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-cnss-daemon source
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS=$(TARGET_CFLAGS)
else
	mkdir -p $(PKG_BUILD_DIR)/lib
	cp prebuilt/cnssdaemon $(PKG_BUILD_DIR)/lib
	cp prebuilt/cnsscli $(PKG_BUILD_DIR)/lib
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/bin ] || install -d $(INSTALLDIR)/usr/bin
	@[ -d $(INSTALLDIR)/data/misc ] || install -d $(INSTALLDIR)/data/misc
	install -m 755 -D $(PKG_BUILD_DIR)/lib/cnssdaemon $(INSTALLDIR)/usr/bin
	install -m 755 -D $(PKG_BUILD_DIR)/lib/cnsscli $(INSTALLDIR)/usr/bin
	@$(STRIP) $(INSTALLDIR)/usr/bin/cnssdaemon
	@$(STRIP) $(INSTALLDIR)/usr/bin/cnsscli
	@ln -sf /tmp/wifi $(INSTALLDIR)/data/misc/wifi

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss-daemon/*.c),)
	cp -rf $(PKG_BUILD_DIR)/*.h $(STAGING_DIR)/usr/include/
endif
