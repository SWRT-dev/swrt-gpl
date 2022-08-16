
qca-hostap-10.4-configure:
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	cp -f $(PLATFORM_ROUTER_SRCBASE)/rules/hostapd-default.config $(PLATFORM_ROUTER_SRCBASE)/qca-hostap-10.4/hostapd/.config
	cp -f $(PLATFORM_ROUTER_SRCBASE)/rules/wpa_supplicant-default.config $(PLATFORM_ROUTER_SRCBASE)/qca-hostap-10.4/wpa_supplicant/.config
endif

qca-hostap-10.4: qca-wifi-10.4 libnl
	@$(SEP)
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	$(MAKE) qca-hostap-10.4-configure
	$(MAKE) -C qca-hostap-10.4/hostapd hostapd hostapd_cli CFLAGS="$(CFLAGS) -I$(STAGEDIR)/usr/include" LIBS="-L$(STAGEDIR)/usr/lib -lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl"
	$(MAKE) -C qca-hostap-10.4/wpa_supplicant wpa_supplicant wpa_cli CFLAGS="$(CFLAGS) -I$(STAGEDIR)/usr/include" LIBS="-L$(STAGEDIR)/usr/lib -lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl"
endif

qca-hostap-10.4-clean:
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	$(MAKE) -C qca-hostap-10.4/hostapd clean
	$(MAKE) -C qca-hostap-10.4/wpa_supplicant clean
endif

qca-hostap-10.4-install: qca-hostap-10.4
ifneq ($(wildcard qca-hostap-10.4/hostapd/Makefile),)
	install -D qca-hostap-10.4/hostapd/hostapd $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd
	install -D qca-hostap-10.4/hostapd/hostapd_cli $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd_cli
	install -D qca-hostap-10.4/wpa_supplicant/wpa_supplicant $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/wpa_supplicant
	install -D qca-hostap-10.4/wpa_supplicant/wpa_cli $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/wpa_cli
else
	install -D qca-hostap-10.4/prebuild/hostapd $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd
	install -D qca-hostap-10.4/prebuild/hostapd_cli $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd_cli
	install -D qca-hostap-10.4/prebuild/wpa_supplicant $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/wpa_supplicant
	install -D qca-hostap-10.4/prebuild/wpa_cli $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/wpa_cli
endif
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/hostapd_cli
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/wpa_supplicant
	$(STRIP) $(INSTALLDIR)/qca-hostap-10.4/usr/sbin/wpa_cli
