include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source/src

TARGET_CFLAGS += -Wno-error=deprecated-declarations -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3 -I$(STAGEDIR)/usr/include/openssl -D_GNU_SOURCE
ifeq ($(or $(RTCONFIG_RALINK_EN7561),$(RTCONFIG_RALINK_EN7562),$(RTCONFIG_RALINK_MT7621)),y)
TARGET_CFLAGS += -DEASYMESH_VERSION=2
else
TARGET_CFLAGS += -DEASYMESH_VERSION=4
endif
TARGET_CFLAGS += -DAGENT_SYNC_DYNAMIC_CNTLR_CONFIG -DEASYMESH_R2_CERT
TARGET_CFLAGS += -DEASYMESH_VENDOR_EXT -DEASYMESH_VENDOR_EXT_OUI=0xF832E4
TARGET_CFLAGS += -DAGENT_ISLAND_PREVENTION
TARGET_CFLAGS += -DUNAUTHORIZED_STA_IN_ASSOCLIST
TARGET_CFLAGS += -DCHANSWITCH_SKIP_DFS_UNAVAILABLE
TARGET_CFLAGS += -DLOCAL_ACS_SERVICE
TARGET_CFLAGS += -DSTRICT_OPER_CHANNEL_REPORT
#TARGET_CFLAGS += -DAGENT_DYNAMIC_CNTLR
TARGET_CFLAGS += -DHAS_UBUS -DEXTENSION_ALLOWED -DOPER_CHAN_CHANGE_RELAY_MCAST
ifeq ($(RTCONFIG_SWRTMESH),y)
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH
endif

all:
ifneq ($(wildcard source/src/Makefile),)
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)"
endif

install:
	[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
ifneq ($(wildcard source/src/Makefile),)
	install -m 755 $(PKG_BUILD_DIR)/mapagent $(INSTALLDIR)/usr/sbin/
	install -m 755 $(PKG_BUILD_DIR)/dynbh/dynbhd $(INSTALLDIR)/usr/sbin/
#	install -m 755 $(PKG_BUILD_DIR)/dynbh/api $(INSTALLDIR)/usr/sbin/
else
	install -m 755 prebuilt/$(BUILD_NAME)/mapagent $(INSTALLDIR)/usr/sbin/
	install -m 755 prebuilt/$(BUILD_NAME)/dynbhd $(INSTALLDIR)/usr/sbin/
#	install -m 755 prebuilt/$(BUILD_NAME)/api $(INSTALLDIR)/usr/sbin/
endif
	$(STRIP) $(INSTALLDIR)/usr/sbin/mapagent
	$(STRIP) $(INSTALLDIR)/usr/sbin/dynbhd

clean:
ifneq ($(wildcard source/src/Makefile),)
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f
endif


.PHONY: all install clean
