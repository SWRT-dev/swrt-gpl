# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/ -I./include/

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/athtestcmd/lithium/tools/testcmd/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/athtestcmd source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR)/lithium/tools/testcmd -f makefile.linux TOOLPREFIX="$(TARGET_CROSS)" \
		LIBS="$(TARGET_LDFLAGS)" WLAN_TOP=$(PKG_BUILD_DIR)/lithium $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/lithium
	mkdir -p $(PKG_BUILD_DIR)/lithium/tools
	mkdir -p $(PKG_BUILD_DIR)/lithium/tools/testcmd
	mkdir -p $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux
	cp prebuilt/$(BUILD_NAME)/qcatestcmd $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux/
	cp -rf prebuilt/$(BUILD_NAME)/*.so $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux/qcatestcmd $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux/libtlvparser2.so $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux/libtlvencoder2.so $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/lithium/tools/testcmd/Linux/libtlvcmdrsp2.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/sbin/qcatestcmd
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtlvparser2.so
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtlvencoder2.so
	@$(STRIP) $(INSTALLDIR)/usr/lib/libtlvcmdrsp2.so

clean:

stage:

