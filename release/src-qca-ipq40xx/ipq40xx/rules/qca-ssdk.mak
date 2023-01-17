QCASSDK_CONFIG_OPTS:= PTP_FEATURE=disable HNAT_FEATURE=enable TOOL_PATH=$(STAGING_DIR)/bin SYS_PATH=$(LINUXDIR) TOOLPREFIX=arm-openwrt-linux-muslgnueabi-
QCASSDK_CONFIG_OPTS += KVER=$(LINUX_KERNEL) ARCH=$(ARCH) TARGET_SUFFIX=muslgnueabi GCC_VERSION=$(TOOLCHAIN_TARGET_GCCVER) CFLAGS=-I$(STAGEDIR)/usr/include
QCASSDK_CONFIG_OPTS += MINI_SSDK=disable OS_VER=4_4

qca-ssdk-stage:
	install -d $(STAGEDIR)/usr/include/qca-ssdk
	install -d $(STAGEDIR)/usr/include/qca-ssdk/api
	install -d $(STAGEDIR)/usr/include/qca-ssdk/ref
	install -d $(STAGEDIR)/usr/include/qca-ssdk/fal
	install -d $(STAGEDIR)/usr/include/qca-ssdk/sal
	install -d $(STAGEDIR)/usr/include/qca-ssdk/init
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/api/sw_ioctl.h $(STAGEDIR)/usr/include/qca-ssdk/api
	if [ -f $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/ref/ref_vsi.h ]; then \
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/ref/ref_vsi.h $(STAGEDIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/ref/ref_fdb.h ]; then \
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/ref/ref_fdb.h $(STAGEDIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/ref/ref_port_ctrl.h ]; then \
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/ref/ref_port_ctrl.h $(STAGEDIR)/usr/include/qca-ssdk/ref/; \
	fi
	if [ -f $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/init/ssdk_init.h ]; then \
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/init/ssdk_init.h $(STAGEDIR)/usr/include/qca-ssdk/init/; \
	fi
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/fal $(STAGEDIR)/usr/include/qca-ssdk
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/common/*.h $(STAGEDIR)/usr/include/qca-ssdk
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/sal/os/linux/*.h $(STAGEDIR)/usr/include/qca-ssdk
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-ssdk/include/sal/os/*.h $(STAGEDIR)/usr/include/qca-ssdk

qca-ssdk:
	$(MAKE) -C qca-ssdk $(QCASSDK_CONFIG_OPTS)
	$(MAKE) qca-ssdk-stage

qca-ssdk-install:
	install -D qca-ssdk/build/bin/qca-ssdk.ko $(INSTALLDIR)/qca-ssdk/lib/modules/$(LINUX_KERNEL)/qca-ssdk.ko
	@find $(INSTALLDIR)/qca-ssdk/lib/modules/$(LINUX_KERNEL)/ -name "modules.*" | xargs rm -f
	@find $(INSTALLDIR)/qca-ssdk/lib/modules/$(LINUX_KERNEL)/ -name "*.ko" | xargs $(STRIPX)

qca-ssdk-clean:
	$(MAKE) -C qca-ssdk clean

