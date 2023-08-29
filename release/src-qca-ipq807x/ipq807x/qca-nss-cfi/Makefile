INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)
PBBINS=$(wildcard prebuilt/*)
NSS_CFI_KMOD=$(filter %.ko,$(PBBINS))
NSS_CFI_EXEC=$(filter-out %.ko,$(PBBINS))

all:

install:
	@[ -d $(INSTALLKMODDIR) ] || install -d $(INSTALLKMODDIR)
	@for i in $(NSS_CFI_KMOD); do install -D $$i $(INSTALLKMODDIR) ; done
	@$(STRIPX) $(INSTALLKMODDIR)/*.ko

clean:

stage:
