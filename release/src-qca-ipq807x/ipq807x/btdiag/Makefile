# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)
TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -pie

MAKE_FLAGS += \
	CFLAGS="$(CFLAGS) $(TARGET_CFLAGS) -ffunction-sections -fdata-sections" \
	LDFLAGS="$(TARGET_LDFLAGS) -Wl,--gc-sections"

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/btdiag/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/btdiag source
	$(MAKE) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/Btdiag $(PKG_BUILD_DIR)/Btdiag
endif

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	install -m 755 -D $(PKG_BUILD_DIR)/Btdiag $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/Btdiag

clean:

