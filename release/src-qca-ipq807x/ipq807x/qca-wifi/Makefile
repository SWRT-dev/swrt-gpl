INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
PBBINS=$(filter-out prebuilt/ini%, $(wildcard prebuilt/*))
QCAWIFI_KMOD=$(filter %.ko,$(PBBINS))
QCAWIFI_LIBS=$(filter %.so,$(PBBINS))
QCAWIFI_XML=$(filter %.xml,$(PBBINS))
QCAWIFI_USR_SBIN=$(filter %/athdiag %/athtestcmd,$(PBBINS))
QCAWIFI_EXEC=$(filter-out $(QCAWIFI_KMOD) $(QCAWIFI_LIBS) $(QCAWIFI_XML) $(QCAWIFI_USR_SBIN),$(PBBINS))

all:

install:
	@mkdir -p $(foreach d,$(INSTALLKMODDIR) $(INSTALLDIR)/usr/lib $(INSTALLDIR)/usr/sbin $(INSTALLDIR)/sbin $(INSTALLDIR)/ini $(INSTALLDIR)/data/vendor $(INSTALLDIR)/lib/wifi,$(d))
	@install $(foreach f,$(QCAWIFI_KMOD),$(f)) $(INSTALLKMODDIR)
	@install $(foreach f,$(QCAWIFI_USR_SBIN),$(f)) $(INSTALLDIR)/usr/sbin
	@install $(foreach f,$(QCAWIFI_LIBS),$(f)) $(INSTALLDIR)/usr/lib
	@install $(foreach f,$(QCAWIFI_EXEC),$(f)) $(INSTALLDIR)/sbin
	@install $(foreach f,$(QCAWIFI_XML),$(f)) $(INSTALLDIR)/lib/wifi
	@cd prebuilt/ini && tar cvf - . | tar xvf - -C $(INSTALLDIR)/ini
	@ln -sf /tmp/wifi $(INSTALLDIR)/data/vendor/wifi
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko
	@$(STRIP) $(INSTALLDIR)/usr/lib/*
	@$(STRIP) $(INSTALLDIR)/usr/sbin/*
	@$(STRIP) $(foreach f,$(filter-out %/cfg80211tool,$(QCAWIFI_EXEC)),$(f))

clean:

stage:
