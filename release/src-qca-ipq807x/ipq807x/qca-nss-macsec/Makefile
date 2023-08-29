INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
PBBINS=$(wildcard prebuilt/*)
NSS_MACSEC_KMOD=$(filter %.ko,$(PBBINS))
NSS_MACSEC_LIBS=$(filter %.so,$(PBBINS))
NSS_MACSEC_EXEC=$(filter-out $(NSS_MACSEC_KMOD) $(NSS_MACSEC_LIBS),$(PBBINS))

all:

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@for i in $(NSS_MACSEC_KMOD); do install -D $$i $(INSTALLKMODDIR) ; done
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@for i in $(NSS_MACSEC_LIBS); do install -D $$i $(INSTALLDIR)/usr/lib ; done
	@[ -d $(INSTALLDIR)/usr/sbin ] || install -d $(INSTALLDIR)/usr/sbin
	@for i in $(NSS_MACSEC_EXEC); do install -D $$i $(INSTALLDIR)/usr/sbin ; done
	@$(STRIP) $(INSTALLDIR)/usr/sbin/*

clean:

stage:
