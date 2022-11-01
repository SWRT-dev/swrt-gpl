include ../common.mak
include $(SRCBASE)/.config
ifeq ($(RTCONFIG_RALINK),y)
ifeq ($(RTCONFIG_RALINK_MT7621),y)
PREBUILT_BIN=$(shell if [ ! -f "$(SRCBASE)/../sdk/mtk/mt7621/1905daemon/Makefile" ]; then echo 1; else echo 0; fi)
else ifeq ($(RTCONFIG_MT798X),y)
PREBUILT_BIN=$(shell if [ ! -f "$(SRCBASE)/../sdk/mtk/mt7986/1905daemon/Makefile" ]; then echo 1; else echo 0; fi)
endif
endif

all:
ifeq ($(RTCONFIG_RALINK),y)
ifneq ($(PREBUILT_BIN),1)
	rm -f 1905daemon datconf fwdd libmapd mapd mapfilter mtfwd wappd
ifeq ($(RTCONFIG_RALINK_MT7621),y)
	ln -sf ../../../sdk/mtk/mt7621/1905daemon 1905daemon
	ln -sf ../../../sdk/mtk/mt7621/datconf datconf
	ln -sf ../../../sdk/mtk/mt7621/fwdd fwdd
	ln -sf ../../../sdk/mtk/mt7621/libmapd libmapd
	ln -sf ../../../sdk/mtk/mt7621/mapd mapd
	ln -sf ../../../sdk/mtk/mt7621/mapfilter mapfilter
	ln -sf ../../../sdk/mtk/mt7621/mtfwd mtfwd
	ln -sf ../../../sdk/mtk/mt7621/wappd wappd
else ifeq ($(RTCONFIG_MT798X),y)
	ln -sf ../../../sdk/mtk/mt7986/1905daemon 1905daemon
	ln -sf ../../../sdk/mtk/mt7986/datconf datconf
	ln -sf ../../../sdk/mtk/mt7986/fwdd fwdd
	ln -sf ../../../sdk/mtk/mt7986/libmapd libmapd
	ln -sf ../../../sdk/mtk/mt7986/mapd mapd
	ln -sf ../../../sdk/mtk/mt7986/mapfilter mapfilter
	ln -sf ../../../sdk/mtk/mt7986/mtfwd mtfwd
	ln -sf ../../../sdk/mtk/mt7986/wappd wappd
endif
	$(MAKE) -C mapfilter all
	$(MAKE) -C mtfwd all
	cd datconf && cmake -DCMAKE_INSTALL_PREFIX=/usr	-DCMAKE_BUILD_TYPE=Release -DBUILD_LUA=OFF && $(MAKE) CFLAGS="$(CFLAGS)"
	$(MAKE) -C 1905daemon all
	$(MAKE) -C libmapd all
	$(MAKE) -C wappd all
	$(MAKE) -C mapd all
	$(MAKE) -C fwdd all
else
	mkdir -p 1905daemon datconf fwdd libmapd mapd mapfilter wappd
	mkdir -p datconf/kvcutil datconf/datconf 1905daemon/ethernet/
	mkdir -p wappd/config_and_icon_files
	cp prebuilt/$(BUILD_NAME)/kvcedit datconf/kvcutil/
	cp prebuilt/$(BUILD_NAME)/datconf datconf/datconf/
	cp -rf prebuilt/$(BUILD_NAME)/libkvcutil.so* datconf/kvcutil/
	cp prebuilt/$(BUILD_NAME)/p1905_managerd 1905daemon/
	cp prebuilt/$(BUILD_NAME)/1905ctrl 1905daemon/
	cp prebuilt/$(BUILD_NAME)/tp_ts_switch.sh 1905daemon/
	cp -rf prebuilt/$(BUILD_NAME)/libeth_1905ops.so 1905daemon/ethernet/
	cp prebuilt/$(BUILD_NAME)/wappctrl wappd/
	cp prebuilt/$(BUILD_NAME)/wapp wappd/
	cp prebuilt/$(BUILD_NAME)/wifi_config_save.lua wappd/
#	cp prebuilt/$(BUILD_NAME)/mbo_nr.sh wappd/src/
	cp -rf prebuilt/$(BUILD_NAME)/config_and_icon_files/* wappd/src/config_and_icon_files/
	cp prebuilt/$(BUILD_NAME)/libmapd_interface_client.so libmapd/
	cp prebuilt/$(BUILD_NAME)/mapd mapd/
	cp prebuilt/$(BUILD_NAME)/mapd_cli mapd/
	cp prebuilt/$(BUILD_NAME)/bs20 mapd/
	cp prebuilt/$(BUILD_NAME)/mapd_user_iface mapd/
#	cp prebuilt/$(BUILD_NAME)/config_agent.lua mapd/
#	cp prebuilt/$(BUILD_NAME)/map_config_agent.lua mapd/
	cp prebuilt/$(BUILD_NAME)/map_cert_script.sh mapd/
	cp prebuilt/$(BUILD_NAME)/fwdd fwdd/
	cp prebuilt/$(BUILD_NAME)/mapfilter.ko mapfilter/src/
	cp prebuilt/$(BUILD_NAME)/mtfwd.ko mtfwd/src/
endif
endif

install:
ifeq ($(RTCONFIG_RALINK),y)
	install -d $(INSTALLDIR)/easymesh-sdk/usr/lib
	install -d $(INSTALLDIR)/easymesh-sdk/rom/etc/wappd/
ifneq ($(PREBUILT_BIN),1)
	$(MAKE) -C mapfilter install
	$(MAKE) -C mtfwd install
else
	install -d $(INSTALLDIR)/lib/modules/4.4.198/extra
	cp -rf mapfilter/src/mapfilter.ko $(INSTALLDIR)/easymesh-sdk/lib/modules/4.4.198/extra
	cp -rf mtfwd/src/mtfwd.ko $(INSTALLDIR)/easymesh-sdk/lib/modules/4.4.198/extra
endif
	install -D 1905daemon/p1905_managerd $(INSTALLDIR)/easymesh-sdk/usr/sbin/p1905_managerd
	install -D 1905daemon/1905ctrl $(INSTALLDIR)/easymesh-sdk/usr/sbin/1905ctrl
	install -D 1905daemon/tp_ts_switch.sh $(INSTALLDIR)/easymesh-sdk/usr/sbin/tp_ts_switch.sh
	install -D datconf/kvcutil/kvcedit $(INSTALLDIR)/easymesh-sdk/usr/sbin/kvcedit
	install -D datconf/datconf/datconf $(INSTALLDIR)/easymesh-sdk/usr/bin/datconf
	cp -rf datconf/kvcutil/libkvcutil.so* $(INSTALLDIR)/easymesh-sdk/usr/lib/
	cp -rf 1905daemon/ethernet/libeth_1905ops.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	install -D wappd/wappctrl $(INSTALLDIR)/easymesh-sdk/usr/sbin/wappctrl
	install -D wappd/wapp $(INSTALLDIR)/easymesh-sdk/usr/sbin/wapp
	install -D wappd/wifi_config_save.lua $(INSTALLDIR)/easymesh-sdk/usr/sbin/wifi_config_save
#	install -D wappd/src/mbo_nr.sh $(INSTALLDIR)/easymesh-sdk/usr/sbin/mbo_nr.sh
	cp -rf wappd/config_and_icon_files/* $(INSTALLDIR)/easymesh-sdk/rom/etc/wappd/
	cp -rf libmapd/libmapd_interface_client.so $(INSTALLDIR)/easymesh-sdk/usr/lib/
	install -D mapd/mapd $(INSTALLDIR)/easymesh-sdk/usr/sbin/mapd
	install -D mapd/mapd_cli $(INSTALLDIR)/easymesh-sdk/usr/sbin/mapd_cli
	install -D mapd/bs20 $(INSTALLDIR)/easymesh-sdk/usr/sbin/bs20
	install -D mapd/mapd_user_iface $(INSTALLDIR)/easymesh-sdk/usr/sbin/mapd_user_iface
#	install -D mapd/config_agent.lua $(INSTALLDIR)/easymesh-sdk/usr/sbin/config_agent.lua
#	install -D mapd/map_config_agent.lua $(INSTALLDIR)/easymesh-sdk/usr/sbin/map_config_agent.lua
	install -D mapd/map_cert_script.sh $(INSTALLDIR)/easymesh-sdk/usr/sbin/map_cert_script.sh
	install -D fwdd/fwdd $(INSTALLDIR)/easymesh-sdk/usr/sbin/fwdd
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/p1905_managerd
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/1905ctrl
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/kvcedit
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/bin/datconf
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/wappctrl
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/wapp
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libeth_1905ops.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libkvcutil.so.1.0.0.0
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/lib/libmapd_interface_client.so
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/mapd
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/mapd_cli
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/bs20
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/mapd_user_iface
	$(STRIP) $(INSTALLDIR)/easymesh-sdk/usr/sbin/fwdd
endif
