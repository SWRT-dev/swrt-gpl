# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -fpie
TARGET_LDFLAGS += -pie

TARGET_CPPFLAGS:= \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	$(TARGET_CPPFLAGS) \
	-DCONFIG_LIBNL20 \
	-D_GNU_SOURCE

MAKE_FLAGS += \
	NL1FOUND="" NL2FOUND=Y \
	NLLIBNAME="libnl-tiny" \
	LIBS="-lm -lnl-tiny" \
	V=1 IW_FULL=1

CFLAGS+="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -ffunction-sections -fdata-sections"
LDFLAGS+="$(TARGET_LDFLAGS) -Wl,--gc-sections"

all:
	echo "const char iw_version[] = \"5.9-8fab0c9e\";" > $(PKG_BUILD_DIR)/version.c
	echo "#!/bin/sh" > $(PKG_BUILD_DIR)/version.sh
	chmod +x $(PKG_BUILD_DIR)/version.sh
	$(TARGET_MAKE) $(PARALLEL_BUILD) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) $(MAKE_FLAGS)
	touch stamp-h1

install:
	install -D  $(PKG_BUILD_DIR)/iw $(INSTALLDIR)/usr/sbin/iw
	$(STRIP) $(INSTALLDIR)/usr/sbin/iw

clean:

