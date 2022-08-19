
qca-edma:
ifneq ($(wildcard qca-edma/Makefile),)
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(PLATFORM_ROUTER_SRCBASE)/qca-edma SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/qca-edma modules
endif

qca-edma-install:
ifneq ($(wildcard qca-edma/Makefile),)
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS=-I$(PLATFORM_ROUTER_SRCBASE)/qca-edma SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/qca-edma INSTALL_MOD_PATH=$(INSTALLDIR)/qca-edma/ modules_install
else
	install -D qca-edma/prebuild/essedma.ko $(INSTALLDIR)/qca-edma/lib/modules/$(LINUX_KERNEL)/essedma.ko
endif
	@find $(INSTALLDIR)/qca-edma/lib/modules/$(LINUX_KERNEL)/ -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR)/qca-edma/lib/modules/$(LINUX_KERNEL)/ -name "*.ko" | xargs $(STRIPX)

qca-edma-clean:
	$(RM) $(PLATFORM_ROUTER_SRCBASE)/qca-edma/*.o $(PLATFORM_ROUTER_SRCBASE)/qca-edma/*.ko $(PLATFORM_ROUTER_SRCBASE)/qca-edma/*.mod.*

