
qca-hostap-configure:
ifneq ($(wildcard qca-hostap/hostapd/Makefile),)
	cp -f $(PLATFORM_ROUTER)/rules/hostapd-default.config $(PLATFORM_ROUTER)/qca-hostap/hostapd/.config
	cp -f $(PLATFORM_ROUTER)/rules/wpa_supplicant-default.config $(PLATFORM_ROUTER)/qca-hostap/wpa_supplicant/.config
	cd $(PLATFORM_ROUTER)/qca-hostap/ && \
	( \
	for i in qca-patches/*.patch; do \
		patch -p1 < $${i}; \
	done; \
	);
endif

qca-hostap: qca-wifi
	@$(SEP)
ifneq ($(wildcard qca-hostap/hostapd/Makefile),)
	$(MAKE) -C qca-hostap/hostapd hostapd hostapd_cli CFLAGS="-I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/qca-nss-macsec -fpie -Werror" LIBS="-lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie"
	$(MAKE) -C qca-hostap/wpa_supplicant wpa_supplicant wpa_cli CFLAGS="-I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/qca-nss-macsec -fpie -Werror" LIBS="-lnl-3 -lnl-genl-3 -lm -lssl -lcrypto -lpthread -ldl -pie"
endif

qca-hostap-clean:
ifneq ($(wildcard qca-hostap/hostapd/Makefile),)
	$(MAKE) -C qca-hostap/hostapd clean
	$(MAKE) -C qca-hostap/wpa_supplicant clean
endif

qca-hostap-install: qca-hostap
ifneq ($(wildcard qca-hostap/hostapd/Makefile),)
	install -D qca-hostap/hostapd/hostapd $(INSTALLDIR)/qca-hostap/usr/sbin/hostapd
	install -D qca-hostap/hostapd/hostapd_cli $(INSTALLDIR)/qca-hostap/usr/sbin/hostapd_cli
	install -D qca-hostap/wpa_supplicant/wpa_supplicant $(INSTALLDIR)/qca-hostap/usr/sbin/wpa_supplicant
	install -D qca-hostap/wpa_supplicant/wpa_cli $(INSTALLDIR)/qca-hostap/usr/sbin/wpa_cli
else
	install -D qca-hostap/prebuild/hostapd $(INSTALLDIR)/qca-hostap/usr/sbin/hostapd
	install -D qca-hostap/prebuild/hostapd_cli $(INSTALLDIR)/qca-hostap/usr/sbin/hostapd_cli
	install -D qca-hostap/prebuild/wpa_supplicant $(INSTALLDIR)/qca-hostap/usr/sbin/wpa_supplicant
	install -D qca-hostap/prebuild/wpa_cli $(INSTALLDIR)/qca-hostap/usr/sbin/wpa_cli
endif
	$(STRIP) $(INSTALLDIR)/qca-hostap/usr/sbin/hostapd
	$(STRIP) $(INSTALLDIR)/qca-hostap/usr/sbin/hostapd_cli
	$(STRIP) $(INSTALLDIR)/qca-hostap/usr/sbin/wpa_supplicant
	$(STRIP) $(INSTALLDIR)/qca-hostap/usr/sbin/wpa_cli
