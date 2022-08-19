
shortcut-fe-stage:
	install -d $(STAGEDIR)/usr/include/shortcut-fe
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe/sfe.h $(STAGEDIR)/usr/include/shortcut-fe

shortcut-fe:
ifneq ($(wildcard shortcut-fe/shortcut-fe/Makefile),)
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="-I$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe -DSFE_SUPPORT_IPV6" SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe SFE_SUPPORT_IPV6=1 modules
	$(MAKE) shortcut-fe-stage
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="-I$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier -DSFE_SUPPORT_IPV6" SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier SFE_SUPPORT_IPV6=y CONFIG_FAST_CLASSIFIER=m modules
endif

shortcut-fe-install:
ifneq ($(wildcard shortcut-fe/shortcut-fe/Makefile),)
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="-I$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe -DSFE_SUPPORT_IPV6" SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe SFE_SUPPORT_IPV6=1 INSTALL_MOD_PATH=$(INSTALLDIR)/shortcut-fe/ modules_install
	$(MAKE) -C $(LINUXDIR) CROSS_COMPILE=$(patsubst %-gcc,%-,$(KERNELCC)) EXTRA_CFLAGS="-I$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier -DSFE_SUPPORT_IPV6" SUBDIRS=$(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier SFE_SUPPORT_IPV6=y CONFIG_FAST_CLASSIFIER=m INSTALL_MOD_PATH=$(INSTALLDIR)/shortcut-fe/ modules_install
else
	install -D shortcut-fe/prebuild/shortcut-fe.ko $(INSTALLDIR)/shortcut-fe/lib/modules/$(LINUX_KERNEL)/shortcut-fe.ko
	install -D shortcut-fe/prebuild/shortcut-fe-ipv6.ko $(INSTALLDIR)/shortcut-fe/lib/modules/$(LINUX_KERNEL)/shortcut-fe-ipv6.ko
	install -D shortcut-fe/prebuild/shortcut-fe-cm.ko $(INSTALLDIR)/shortcut-fe/lib/modules/$(LINUX_KERNEL)/shortcut-fe-cm.ko
	install -D shortcut-fe/prebuild/fast-classifier.ko $(INSTALLDIR)/shortcut-fe/lib/modules/$(LINUX_KERNEL)/fast-classifier.ko
endif
	@find $(INSTALLDIR)/shortcut-fe/lib/modules/$(LINUX_KERNEL)/ -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR)/shortcut-fe/lib/modules/$(LINUX_KERNEL)/ -name "*.ko" | xargs $(STRIPX)

shortcut-fe-clean:
	$(RM) $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe/*.o $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe/*.ko $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/shortcut-fe/*.mod.*
	$(RM) $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier/*.o $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier/*.ko $(PLATFORM_ROUTER_SRCBASE)/shortcut-fe/fast-classifier/*.mod.*

