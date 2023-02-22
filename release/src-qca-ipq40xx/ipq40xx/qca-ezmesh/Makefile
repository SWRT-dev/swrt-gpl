# Convert swrt build environment variable to qca-ezmesh
include ../qsdk.config
TARGET_CROSS=$(CROSS_COMPILE)
TARGET_CC=$(CROSS_COMPILE)gcc
TARGET_CFLAGS=
TARGET_LDFLAGS=
KERNEL_MAKEOPTS=-C $(LINUXDIR)
LINUX_VERSION=$(LINUX_KERNEL)
PKG_BUILD_DIR=$(shell pwd)/source

QCAEZMESH_MAKE_OPTS:= \
	CROSS=$(CROSS_COMPILE) \
	GWLIB=$(STAGEDIR)/usr/lib \
	MATOP=$(PLATFORM_ROUTER_SRCBASE)/qca-ezmesh/source \
	GWINCLUDE=$(STAGEDIR)/usr/include/hyficommon \
	ATHDIR=$(STAGEDIR)/usr/include \
	HYFIDIR=$(STAGEDIR)/usr/include/hyfibr \
	HYFIMCDIR=$(STAGEDIR)/usr/include/hyfibr \
	HYFICMNDIR=$(STAGEDIR)/usr/include/hyficommon \
	HYFIBRLIBDIR=$(STAGEDIR)/usr/include/libhyfibr \
	QCASSDKDIR=$(STAGEDIR)/usr/include/qca-ssdk \
	WPA2INCLUDE=$(STAGEDIR)/usr/include/wpa2 \
	INSTALL_ROOT=$(PLATFORM_ROUTER_SRCBASE)/source/install \
	QCACFLAGS="-fpie -I$(STAGEDIR)/usr/include/ -I$(STAGEDIR)/usr/include/libxml2 $(if $(CONFIG_LINUX_5_4),-DCONFIG_LINUX_5_4) -DINCLUDE_BREAKPAD -g3" \
	QCALDFLAGS="-Wl,--gc-sections -pie -lbreakpad_qcawrapper" \
	KERNELVERSION=$(LINUX_KERNEL_VERSION) \
	STAGING_DIR=$(STAGEDIR) \
	ARCH="$(ARCH)" \
	HYBRID_ETH_SPEED=1000 \
	HYD_SUPPORT_VHT160=y \
	EZMESH_MODE_AGENT_ONLY=y EZMESH_MODE_CTRL_ONLY=y

ifeq ($(CONFIG_HYD_SUPPORT_CFG80211),y)
QCAEZMESH_MAKE_OPTS += LIBCFG80211_SUPPORT=y
endif
ifeq ($(CONFIG_PACKAGE_qca-vhyfid),y)
QCAEZMESH_MAKE_OPTS += GATEWAY_PROFILE_VHYFI=y
endif
ifeq ($(CONFIG_PACKAGE_qca-mad),y)
QCAEZMESH_MAKE_OPTS += HYD_SUPPORT_DESERVICE=y
endif
ifeq ($(RTCONFIG_QCA_PLC2),y)
QCAEZMESH_MAKE_OPTS += HYD_MODULE_PLC=y
endif

QCAEZMESH_SBIN := ezmesh ezmeshCtrl ezmeshAgent
QCAEZMESH_LIBS := libmapServiceCmn.so libmapConfigCmn.so libieee1905layer.so libstadb.so libezmeshcore.so
QCAEZMESH_LIBS += libtdService.so liblinkMetrics.so libdispatchService.so libdbgService.so libecmManager.so
QCAEZMESH_LIBS += libethManager.so libhyManager.so libwlanManager.so libwlanif.so libezmpluginManager.so
QCAEZMESH_LIBS += libezmmcfwdtblwlan5g.so libezmmcfwdtblwlan2g.so libezmmcfwdtbleswitch.so libmcManager.so
QCAEZMESH_LIBS += libmapServiceCtrl.so libmapServiceAgent.so libmapConfigCtrl.so libmapConfigAgent.so

all:
ifneq ($(wildcard qca-ezmesh/Makefile),)
	mkdir -p qca-ezmesh/install/lib
	mkdir -p qca-ezmesh/install/include
	mkdir -p qca-ezmesh/install/sbin
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCAEZMESH_MAKE_OPTS)) subdirs
endif

install:
	install -d $(INSTALLDIR)/usr/sbin
	install -d $(INSTALLDIR)/usr/lib
ifneq ($(wildcard qca-ezmesh/Makefile),)
	@for i in $(QCAEZMESH_LIBS); do install -D qca-ezmesh/install/lib/$$i $(INSTALLDIR)/usr/lib/$$i ; done
	@for i in $(QCAEZMESH_SBIN); do install -D qca-ezmesh/install/sbin/$$i $(INSTALLDIR)/usr/sbin/$$i ; done
else
	@for i in $(QCAEZMESH_LIBS); do install -D qca-ezmesh/prebuild/$(BUILD_NAME)/$$i $(INSTALLDIR)/usr/lib/$$i ; done
	@for i in $(QCAEZMESH_SBIN); do install -D qca-ezmesh/prebuild/$(BUILD_NAME)/$$i $(INSTALLDIR)/usr/sbin/$$i ; done
endif
	@for i in $(QCAEZMESH_LIBS); do $(STRIP) $(INSTALLDIR)/usr/lib/$$i ; done
	@for i in $(QCAEZMESH_SBIN); do $(STRIP) $(INSTALLDIR)/usr/sbin/$$i ; done

clean:
