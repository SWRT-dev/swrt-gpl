# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

#TARGET_CFLAGS += -DSWRT_PATCH
#TARGET_CFLAGS += -DINCLUDE_BREAKPAD -O3 -DSWRT_PATCH
#TARGET_LDFLAGS += -lbreakpad_qcawrapper

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss-daemon/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/qca-cnss-daemon source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) TARGET_USES_NO_FW_QMI_CLIENT=true
else
	mkdir -p $(PKG_BUILD_DIR)/lib
	cp prebuilt/$(BUILD_NAME)/cnssdaemon $(PKG_BUILD_DIR)/lib
	cp prebuilt/$(BUILD_NAME)/cnsscli $(PKG_BUILD_DIR)/lib
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/bin ] || install -d $(INSTALLDIR)/usr/bin
	install -m 755 -D $(PKG_BUILD_DIR)/lib/cnssdaemon $(INSTALLDIR)/usr/bin
	install -m 755 -D $(PKG_BUILD_DIR)/lib/cnsscli $(INSTALLDIR)/usr/bin
	@$(STRIP) $(INSTALLDIR)/usr/bin/cnssdaemon
	@$(STRIP) $(INSTALLDIR)/usr/bin/cnsscli

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/qca-cnss-daemon/*.c),)
	cp -rf $(PKG_BUILD_DIR)/*.h $(STAGING_DIR)/usr/include/
endif
