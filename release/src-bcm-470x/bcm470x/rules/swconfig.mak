swconfig.bcm470x:
	$(MAKE) -C swconfig.bcm470x all

swconfig.bcm470x-clean:
	$(MAKE) -C swconfig.bcm470x clean

swconfig.bcm470x-install:
	install -D swconfig.bcm470x/swconfig $(INSTALLDIR)/swconfig.bcm470x/usr/sbin/swconfig
	install -D swconfig.bcm470x/libswitch.so $(INSTALLDIR)/swconfig.bcm470x/usr/lib/libswitch.so
	$(STRIP) $(INSTALLDIR)/swconfig.bcm470x/usr/sbin/swconfig
	$(STRIP) $(INSTALLDIR)/swconfig.bcm470x/usr/lib/libswitch.so
