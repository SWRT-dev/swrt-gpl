switch: libnl-tiny-0.1
	$(MAKE) -C switch

switch-install:
	install -D switch/switch $(INSTALLDIR)/switch/bin/switch
	$(STRIP) $(INSTALLDIR)/switch/bin/switch

switch-clean:
	$(MAKE) -C switch clean

