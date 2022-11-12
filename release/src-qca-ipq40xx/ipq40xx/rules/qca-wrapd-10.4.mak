qca-wrapd-10.4:
ifneq ($(wildcard qca-wrapd-10.4/main.c),)
	$(MAKE) -C qca-wrapd-10.4 COPTS="$(CFLAGS)" WPA_PATH=$(STAGEDIR)/usr/lib QSDK_WRAPD=$(STAGEDIR)/usr/include TOOLPREFIX=$(CROSS_COMPILE)
endif

qca-wrapd-10.4-clean:
ifneq ($(wildcard qca-wrapd-10.4/main.c),)
	$(MAKE) -C qca-wrapd-10.4 clean
endif

qca-wrapd-10.4-install:
ifneq ($(wildcard qca-wrapd-10.4/main.c),)
	install -D qca-wrapd-10.4/wrapd $(INSTALLDIR)/qca-wrapd-10.4/usr/sbin/wrapd
else
	install -D qca-wrapd-10.4/prebuild/wrapd $(INSTALLDIR)/qca-wrapd-10.4/usr/sbin/wrapd
endif
	$(STRIP) $(INSTALLDIR)/qca-wrapd-10.4/usr/sbin/wrapd
