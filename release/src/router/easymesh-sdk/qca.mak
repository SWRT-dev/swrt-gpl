PREBUILT_BIN=$(shell if [ ! -f "$(SRCBASE)/../sdk/qca/11.5/qca-ezmesh/Makefile" ]; then echo 1; else echo 0; fi)

TARGET_CFLAGS += -fpie -I$(STAGEDIR)/usr/include/ -I$(STAGEDIR)/usr/include/libxml2
#TARGET_CFLAGS += -DCONFIG_LINUX_5_4
TARGET_LDFLAGS += -pie

ifeq ($(RTCONFIG_QCA_PLC2),y)
export HYD_MODULE_PLC=y
endif

all:
ifneq ($(PREBUILT_BIN),1)
	rm -f qca-ezmesh
	$(MAKE) -C qca-ezmesh all CROSS=$(CROSS_COMPILE) GWLIB=$(STAGEDIR)/usr/lib MATOP=$(TOP)/router/easymesh-sdk/qca-ezmesh \
		GWINCLUDE=$(STAGEDIR)/usr/include/hyficommon ATHDIR=$(STAGEDIR)/usr/include HYFIDIR=$(STAGEDIR)/usr/include/hyfibr \
		HYFIMCDIR=$(STAGEDIR)/usr/include/hyfibr HYFICMNDIR=$(STAGEDIR)/usr/include/hyficommon HYFIBRLIBDIR=$(STAGEDIR)/usr/include/libhyfibr \
		QCASSDKDIR=$(STAGEDIR)/usr/include/qca-ssdk WPA2INCLUDE=$(STAGEDIR)/usr/include/wpa2 INSTALL_ROOT=$(TOP)/router/easymesh-sdk/qca-ezmesh/install \
		QCACFLAGS="$(TARGET_CFLAGS)" QCALDFLAGS="-Wl,--gc-sections $(TARGET_LDFLAGS)" KERNELVERSION=$(LINUX_KERNEL_VERSION) STAGING_DIR=$(STAGEDIR) \
		ARCH="$(ARCH)"
else
endif

install:
ifneq ($(PREBUILT_BIN),1)
	install -d $(INSTALLDIR)/easymesh-sdk/usr/lib
	install -D qca-ezmesh/install/sbin/ezmesh $(INSTALLDIR)/easymesh-sdk/usr/sbin/ezmesh
	install -D qca-ezmesh/install/sbin/ezmeshCtrl $(INSTALLDIR)/easymesh-sdk/usr/sbin/ezmeshCtrl
	install -D qca-ezmesh/install/sbin/ezmeshAgent $(INSTALLDIR)/easymesh-sdk/usr/sbin/ezmeshAgent
	cp -rf qca-ezmesh/install/lib/libmapServiceCmn.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libmapConfigCmn.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libieee1905layer.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libstadb.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libezmeshcore.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libtdService.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/liblinkMetrics.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libdispatchService.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libdbgService.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libecmManager.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libethManager.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libhyManager.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libwlanManager.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libwlanif.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libezmpluginManager.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libezmmcfwdtblwlan5g.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libezmmcfwdtblwlan2g.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libezmmcfwdtbleswitch.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libmcManager.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libmapServiceCtrl.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libmapServiceAgent.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libmapConfigCtrl.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf qca-ezmesh/install/lib/libmapConfigAgent.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
else
endif
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/ezmesh
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/ezmeshCtrl
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/ezmeshAgent
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapServiceCmn.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapConfigCmn.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libieee1905layer.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libstadb.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libezmeshcore.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libtdService.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/liblinkMetrics.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libdispatchService.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libdbgService.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libecmManager.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libethManager.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libhyManager.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libwlanManager.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libwlanif.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libezmpluginManager.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libezmmcfwdtblwlan5g.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libezmmcfwdtblwlan2g.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libezmmcfwdtbleswitch.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmcManager.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapServiceCtrl.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapServiceAgent.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapConfigCtrl.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapConfigAgent.so

clean:
