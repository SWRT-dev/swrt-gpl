# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

TARGET_CFLAGS += -D_GNU_SOURCE
TARGET_CFLAGS += -I$(TOP)/kernel_header/include -I$(STAGEDIR)/usr/include/ -I$(STAGEDIR)/usr/include/libnl-tiny -I./src

#TARGET_CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -Werror
TARGET_LDFLAGS += -L$(STAGEDIR)/usr/lib -lnl-tiny -lm

all:
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	touch stamp-h1

install:
	install -D $(PKG_BUILD_DIR)/switch $(INSTALLDIR)/usr/sbin/switch
	$(STRIP) $(INSTALLDIR)/usr/sbin/switch

clean:


