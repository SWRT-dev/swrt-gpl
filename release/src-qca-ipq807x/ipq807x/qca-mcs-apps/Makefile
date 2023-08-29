PBBINS=$(wildcard prebuilt/*)

all:

install:
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@for i in $(PBBINS); do install -D $$i $(INSTALLDIR)/usr/sbin ; done
	@$(STRIP) $(INSTALLDIR)/usr/sbin/mcsd
	@$(STRIP) $(INSTALLDIR)/usr/sbin/mcsctl

clean:
