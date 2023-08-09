include $(TOP)/.config
TARGET_CC=$(CROSS_COMPILE)gcc
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source/src

TARGET_CFLAGS += -D_GNU_SOURCE -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3

ifneq ($(RTCONFIG_AMD64),y)
TARGET_CFLAGS += -DHAS_WIFI
endif
#TARGET_CFLAGS += -DCMDU_SA_IS_ALMAC
TARGET_CFLAGS += -DIEEE1905_CMDU_FRAGMENT_TLV_BOUNDARY -DWIFI_EASYMESH -DDYNAMIC_CNTLR_SYNC_CONFIG
ifeq ($(RTCONFIG_SWRTMESH),y)
TARGET_CFLAGS += -DRTCONFIG_SWRTMESH
endif
ifeq ($(or $(RTCONFIG_RALINK_EN7561),$(RTCONFIG_RALINK_EN7562),$(RTCONFIG_RALINK_MT7621)),y)
TARGET_CFLAGS += -DEASYMESH_VERSION=2
else
TARGET_CFLAGS += -DEASYMESH_VERSION=4
endif


all:
ifneq ($(wildcard source/src/Makefile),)
	$(MAKE) -C $(PKG_BUILD_DIR) TARGET_CFLAGS="$(TARGET_CFLAGS)" extmod_subdirs="extensions/map extensions/snoop extensions/topology"
endif

install:
	[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	[ -d $(INSTALLDIR)/usr/lib/ieee1905 ] || install -d $(INSTALLDIR)/usr/lib/ieee1905
	[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
ifneq ($(wildcard source/src/Makefile),)
	install -m 755 $(PKG_BUILD_DIR)/ieee1905d  $(INSTALLDIR)/usr/sbin/
	install -m 755 $(PKG_BUILD_DIR)/libieee1905.so $(INSTALLDIR)/usr/lib/
	install -m 755 $(PKG_BUILD_DIR)/libmidgen.so $(INSTALLDIR)/usr/lib/
	install -m 755 $(PKG_BUILD_DIR)/extensions/map/libmaputil.so $(INSTALLDIR)/usr/lib/
	install -m 755 $(PKG_BUILD_DIR)/extensions/map/map.so $(INSTALLDIR)/usr/lib/ieee1905/
	install -m 755 $(PKG_BUILD_DIR)/extensions/snoop/snoop.so $(INSTALLDIR)/usr/lib/ieee1905/
	install -m 755 $(PKG_BUILD_DIR)/extensions/topology/topology.so $(INSTALLDIR)/usr/lib/ieee1905/
else
	install -m 755 prebuilt/$(BUILD_NAME)/ieee1905d  $(INSTALLDIR)/usr/sbin/
	install -m 755 prebuilt/$(BUILD_NAME)/libieee1905.so $(INSTALLDIR)/usr/lib/
	install -m 755 prebuilt/$(BUILD_NAME)/libmidgen.so $(INSTALLDIR)/usr/lib/
	install -m 755 prebuilt/$(BUILD_NAME)/libmaputil.so $(INSTALLDIR)/usr/lib/
	install -m 755 prebuilt/$(BUILD_NAME)/map.so $(INSTALLDIR)/usr/lib/ieee1905/
	install -m 755 prebuilt/$(BUILD_NAME)/snoop.so $(INSTALLDIR)/usr/lib/ieee1905/
	install -m 755 prebuilt/$(BUILD_NAME)/topology.so $(INSTALLDIR)/usr/lib/ieee1905/
endif
	$(STRIP) $(INSTALLDIR)/usr/lib/*.so
	$(STRIP) $(INSTALLDIR)/usr/lib/ieee1905/*.so
	$(STRIP) $(INSTALLDIR)/usr/sbin/ieee1905d

clean:
ifneq ($(wildcard source/src/Makefile),)
	[ ! -f $(PKG_BUILD_DIR)/Makefile ] || $(MAKE) -C $(PKG_BUILD_DIR) clean
	find $(PKG_BUILD_DIR) -type f -name "*.o" -o -name "stage" | xargs rm -f
endif

stage:
	[ -d $(STAGEDIR)/usr/lib/ieee1905 ] || install -d $(STAGEDIR)/usr/lib/ieee1905
ifneq ($(wildcard source/src/Makefile),)
	install -m 644 $(PKG_BUILD_DIR)/{cmdu,cmdu_ackq,1905_tlvs,i1905_wsc,bufutil,timer_impl,i1905_extension}.h $(STAGEDIR)/usr/include
	install -m 644 $(PKG_BUILD_DIR)/extensions/map/{easymesh,map_module,cntlrsync}.h $(STAGEDIR)/usr/include
	install -m 755 $(PKG_BUILD_DIR)/libieee1905.so $(STAGEDIR)/usr/lib/
	install -m 755 $(PKG_BUILD_DIR)/libmidgen.so $(STAGEDIR)/usr/lib/
	install -m 755 $(PKG_BUILD_DIR)/extensions/map/libmaputil.so $(STAGEDIR)/usr/lib/
	install -m 755 $(PKG_BUILD_DIR)/extensions/map/map.so $(STAGEDIR)/usr/lib/ieee1905/
	install -m 755 $(PKG_BUILD_DIR)/extensions/snoop/snoop.so $(STAGEDIR)/usr/lib/ieee1905/
	install -m 755 $(PKG_BUILD_DIR)/extensions/topology/topology.so $(STAGEDIR)/usr/lib/ieee1905/
else
	install -m 644 prebuilt/$(BUILD_NAME)/{cmdu,cmdu_ackq,1905_tlvs,i1905_wsc,bufutil,timer_impl,i1905_extension}.h $(STAGEDIR)/usr/include
	install -m 644 prebuilt/$(BUILD_NAME)/extensions/map/{easymesh,map_module,cntlrsync}.h $(STAGEDIR)/usr/include
	install -m 755 prebuilt/$(BUILD_NAME)/libieee1905.so $(STAGEDIR)/usr/lib/
	install -m 755 prebuilt/$(BUILD_NAME)/libmidgen.so $(STAGEDIR)/usr/lib/
	install -m 755 prebuilt/$(BUILD_NAME)/libmaputil.so $(STAGEDIR)/usr/lib/
	install -m 755 prebuilt/$(BUILD_NAME)/map.so $(STAGEDIR)/usr/lib/ieee1905/
	install -m 755 prebuilt/$(BUILD_NAME)/snoop.so $(STAGEDIR)/usr/lib/ieee1905/
	install -m 755 prebuilt/$(BUILD_NAME)/topology.so $(STAGEDIR)/usr/lib/ieee1905/
endif

.PHONY: stage all install clean
