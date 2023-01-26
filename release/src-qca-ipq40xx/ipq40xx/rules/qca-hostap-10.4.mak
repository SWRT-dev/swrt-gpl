
qca-hostap-10.4-configure:
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	cp -f $(PLATFORM_ROUTER_SRCBASE)/rules/hostapd-default.config $(PLATFORM_ROUTER_SRCBASE)/qca-hostap-10.4/hostapd/.config
	cp -f $(PLATFORM_ROUTER_SRCBASE)/rules/wpa_supplicant-default.config $(PLATFORM_ROUTER_SRCBASE)/qca-hostap-10.4/wpa_supplicant/.config
endif

qca-hostap-10.4-stage:
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	[ -d $(STAGEDIR)/usr/include ] || install -d $(STAGEDIR)/usr/include
	[ -d $(STAGEDIR)/usr/lib ] || install -d $(STAGEDIR)/usr/lib
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/common/wpa_ctrl.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/common/ieee802_11_defs.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/includes.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/build_config.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/eloop.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/os.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/common.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/wpa_debug.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/wpa_debug.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/wpabuf.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/linux_ioctl.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/linux_wext.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/netlink.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/priv_netlink.h $(STAGEDIR)/usr/include/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/common/wpa_ctrl.o $(STAGEDIR)/usr/lib/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/netlink.o $(STAGEDIR)/usr/lib/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/eloop.o $(STAGEDIR)/usr/lib/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/wpa_debug.o $(STAGEDIR)/usr/lib/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/common.o $(STAGEDIR)/usr/lib/
	cp -f $(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/os_unix.o $(STAGEDIR)/usr/lib/
endif

qca-hostap-10.4: qca-wifi-10.4 libnl
	@$(SEP)
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	$(MAKE) -C libnl stage
	$(MAKE) qca-hostap-10.4-configure
	$(MAKE) -C qca-hostap-10.4/hostapd clean
	$(MAKE) -C qca-hostap-10.4/hostapd hostapd hostapd_cli CFLAGS="$(CFLAGS) -I$(STAGEDIR)/usr/include -I$(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src -I$(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/ -I$(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/" LIBS="-L$(STAGEDIR)/usr/lib -lnl-3 -lnl-genl-3 -lm -lpthread -ldl"
	$(MAKE) qca-hostap-10.4-stage
	$(MAKE) -C qca-hostap-10.4/wpa_supplicant clean
	$(MAKE) -C qca-hostap-10.4/wpa_supplicant wpa_supplicant wpa_cli CFLAGS="$(CFLAGS) -I$(STAGEDIR)/usr/include -I$(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src -I$(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/utils/ -I$(SRCBASE)/$(PLATFORM_ROUTER)/qca-hostap-10.4/src/drivers/" LIBS="-L$(STAGEDIR)/usr/lib -lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl"

endif

qca-hostap-10.4-clean:
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	$(MAKE) -C qca-hostap-10.4/hostapd clean
	$(MAKE) -C qca-hostap-10.4/wpa_supplicant clean
endif

qca-hostap-10.4-install:
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	install -D qca-hostap-10.4/hostapd/hostapd $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd
	install -D qca-hostap-10.4/hostapd/hostapd_cli $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd_cli
	install -D qca-hostap-10.4/wpa_supplicant/wpa_supplicant $(INSTALLDIR)/qca-hostap-10.4/usr/bin/wpa_supplicant
	install -D qca-hostap-10.4/wpa_supplicant/wpa_cli $(INSTALLDIR)/qca-hostap-10.4/usr/bin/wpa_cli
else
	install -D qca-hostap-10.4/prebuild/$(BUILD_NAME)/hostapd $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd
	install -D qca-hostap-10.4/prebuild/$(BUILD_NAME)/hostapd_cli $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd_cli
	install -D qca-hostap-10.4/prebuild/$(BUILD_NAME)/wpa_supplicant $(INSTALLDIR)/qca-hostap-10.4/usr/bin/wpa_supplicant
	install -D qca-hostap-10.4/prebuild/$(BUILD_NAME)/wpa_cli $(INSTALLDIR)/qca-hostap-10.4/usr/bin/wpa_cli
endif
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd_cli
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/bin/wpa_supplicant
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/bin/wpa_cli
