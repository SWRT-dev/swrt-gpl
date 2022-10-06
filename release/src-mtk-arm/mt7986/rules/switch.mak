switch: libnl-tiny-0.1
	$(MAKE) -C switch

switch-install:
	install -D switch/switch $(INSTALLDIR)/bin/switch
	$(STRIP) $(INSTALLDIR)/bin/switch

switch-clean:
	$(MAKE) -C switch clean

