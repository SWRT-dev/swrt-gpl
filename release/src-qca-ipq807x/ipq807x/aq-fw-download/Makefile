# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)

TARGET_CPPFLAGS := -D_GNU_SOURCE -I$(LINUXDIR)/include -I$(LINUXDIR)/arch/arm64/include -Isrc

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/aq-fw-download/src/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/aq-fw-download/src source
	$(MAKE) -C (PKG_BUILD_DIR) CFLAGS="$(TARGET_CPPFLAGS) $(CFLAGS)" LDFLAGS="$(LDFLAGS)"
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/aq-fw-download $(PKG_BUILD_DIR)/aq-fw-download
endif

install:
ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), RT-AX89U RAX120))
	@[ -d $(INSTALLDIR)/sbin ] || install -d $(INSTALLDIR)/sbin
	@[ -d $(INSTALLDIR)/lib/firmware ] || install -d $(INSTALLDIR)/lib/firmware
	install -m 755 -D $(PKG_BUILD_DIR)/aq-fw-download $(INSTALLDIR)/sbin
ifeq ($(BUILD_NAME),RT-AX89U)
	@install -m 755 -D prebuilt/RT-AX89U_aqr107.cld $(INSTALLDIR)/lib/firmware
	@install -m 755 -D prebuilt/RT-AX89U_aqr113.cld $(INSTALLDIR)/lib/firmware
else ifeq ($(BUILD_NAME),RAX120)
	@install -m 755 -D prebuilt/RAX120_aqr111.cld $(INSTALLDIR)/lib/firmware
endif
	@$(STRIP) $(INSTALLDIR)/sbin/aq-fw-download
endif

clean:
