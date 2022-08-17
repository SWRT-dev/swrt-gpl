QCASSDKSHELL_CONFIG_OPTS:= TOOL_PATH=$(STAGING_DIR)/bin SYS_PATH=$(LINUXDIR) TOOLPREFIX=arm-openwrt-linux-muslgnueabi-
QCASSDKSHELL_CONFIG_OPTS += KVER=$(LINUX_KERNEL) ARCH=$(ARCH) CFLAGS=-I$(STAGEDIR)/usr/include

qca-ssdk-shell:
ifneq ($(wildcard qca-ssdk-shell/Makefile),)
	$(MAKE) -C qca-ssdk-shell $(QCASSDKSHELL_CONFIG_OPTS)
endif

qca-ssdk-shell-install:
ifneq ($(wildcard qca-ssdk-shell/Makefile),)
	install -D qca-ssdk-shell/build/bin/ssdk_sh $(INSTALLDIR)/qca-ssdk-shell/usr/sbin/ssdk_sh
else
	install -D qca-ssdk-shell/prebuild/ssdk_sh $(INSTALLDIR)/qca-ssdk-shell/usr/sbin/ssdk_sh
endif
	$(STRIP) $(INSTALLDIR)/qca-ssdk-shell/usr/sbin/ssdk_sh


qca-ssdk-shell-clean:
	$(MAKE) -C qca-ssdk-shell clean

