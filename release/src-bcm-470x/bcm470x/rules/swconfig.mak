swconfig.bcm470x:
	$(MAKE) -C swconfig.bcm470x all CFLAGS="-O2 -I$(PLATFORM_ROUTER_SRCBASE)/swconfig.bcm470x -I$(TOP)/libnl-tiny-0.1/include -I$(TOP)/kernel_header/include -DCONFIG_LIBNL20 -D_GNU_SOURCE" LDFLAGS="-L$(TOP)/libnl-tiny-0.1 -lnl-tiny" LIBS="-lm -lnl-tiny"

swconfig.bcm470x-clean:
	$(MAKE) -C swconfig.bcm470x clean

swconfig.bcm470x-install:
	install -D swconfig.bcm470x/swconfig $(INSTALLDIR)/swconfig.bcm470x/usr/sbin/swconfig
	install -D swconfig.bcm470x/libswitch.so $(INSTALLDIR)/swconfig.bcm470x/usr/lib/libswitch.so
	$(STRIP) $(INSTALLDIR)/swconfig.bcm470x/usr/sbin/swconfig
	$(STRIP) $(INSTALLDIR)/swconfig.bcm470x/usr/lib/libswitch.so
