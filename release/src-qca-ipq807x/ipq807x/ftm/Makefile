# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -DCONFIG_FTM_WLAN -DDEBUG -DFTM_DEBUG -DWIN_AP_HOST
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/qca-diag
TARGET_CFLAGS += -MMD -O2 -Wall -g
TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -ldiag -lnl-3 -lnl-genl-3 -lrt -ltcmd
TARGET_CSRCS := ftm_main.c ftm_wlan.c ftm_write_to_flash.c
TARGET_LDFLAGS += -pie

all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/common-tools/ftm/*.c),)
	ln -sf $(SDK_DIR_PLATFORM)/common-tools source
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR)/ftm CSRCS="$(TARGET_CSRCS)" $(TARGET_CONFIGURE_OPTS)
else
	mkdir -p $(PKG_BUILD_DIR)/
	mkdir -p $(PKG_BUILD_DIR)/ftm
	$(CP) prebuilt/$(BUILD_NAME)/ftm $(PKG_BUILD_DIR)/ftm
endif
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@install -m 755 -D $(PKG_BUILD_DIR)/ftm/ftm $(INSTALLDIR)/usr/sbin
	@$(STRIP) $(INSTALLDIR)/usr/sbin/ftm

clean:

stage:

