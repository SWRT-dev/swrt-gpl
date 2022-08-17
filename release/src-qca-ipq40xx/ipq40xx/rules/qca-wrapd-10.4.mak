qca-wrapd-10.4:
	$(MAKE) -C qca-wrapd-10.4 COPTS="$(CFLAGS)" WPA_PATH=$(STAGEDIR)/usr/lib QSDK_WRAPD=$(STAGEDIR)/usr/include TOOLPREFIX=$(CROSS_COMPILE)

qca-wrapd-10.4-clean:
	$(MAKE) -C qca-wrapd-10.4 clean

qca-wrapd-10.4-install:
	install -D qca-wrapd-10.4/wrapd $(INSTALLDIR)/qca-wrapd-10.4/usr/sbin/wrapd
	$(STRIP) $(INSTALLDIR)/qca-wrapd-10.4/usr/sbin/wrapd
