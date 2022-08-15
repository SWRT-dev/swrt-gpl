INSTALLKMODDIR:=$(INSTALLDIR)/lib/modules/$(LINUX_KERNEL)

qca-edma:
ifneq ($(wildcard qca-edma/Makefile),)
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(PLATFORM_ROUTER_SRCBASE)/qca-edma SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/qca-edma modules
endif

qca-edma-install:
ifneq ($(wildcard qca-edma/Makefile),)
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(PLATFORM_ROUTER_SRCBASE)/qca-edma SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/qca-edma INSTALL_MOD_PATH=$(INSTALLDIR) modules_install
else
	install -D qca-edma/prebuild/essedma.ko $(INSTALLKMODDIR)
endif
	@find $(INSTALLKMODDIR) -name "modules.*" | xargs rm -f
	@find $(INSTALLKMODDIR) -name "*.ko" | xargs $(STRIPX)

qca-edma-clean:
	$(RM) $(PLATFORM_ROUTER_SRCBASE)/qca-edma/*.o $(PLATFORM_ROUTER_SRCBASE)/qca-edma/*.ko $(PLATFORM_ROUTER_SRCBASE)/qca-edma/*.mod.*

