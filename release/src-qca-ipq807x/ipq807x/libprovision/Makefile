# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += \
	-DDEBUG \
	-DBTD_DEBUG \
	-Wall \
	-Werror \
	-DCONFIG_FTM_AFC

TARGET_LDFLAGS += -ljson-c

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/libprovision/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/libprovision source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/ $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) prebuilt/libprovision.so $(PKG_BUILD_DIR)/
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/libprovision.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libprovision.so

clean:

stage:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/libprovision/*.c),)
	$(CP) $(PKG_BUILD_DIR)/libprovision.so $(STAGEDIR)/usr/lib
endif

