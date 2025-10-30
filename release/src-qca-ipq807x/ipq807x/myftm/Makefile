# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -DCONFIG_FTM_WLAN -DDEBUG -DFTM_DEBUG -DCONFIG_FTM_WLAN_AUTOLOAD -DWIN_AP_HOST -DUSE_TLV2
TARGET_CFLAGS += -MMD -O2 -Wall -g
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/libathtestcmd
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/lib/libathtestcmd
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/lib/
TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -ltcmd -ltestcmd6174 -lz
TARGET_CSRCS := myftm_main.c myftm_wlan.c myftm_qtip.c
TARGET_LDFLAGS += -pie

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/myftm/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools/ath6kl-utils/myftm source
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) CSRCS="$(TARGET_CSRCS)" $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)
	cp prebuilt/$(BUILD_NAME)/myftm $(PKG_BUILD_DIR)
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/myftm $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/myftm


clean:

stage:

