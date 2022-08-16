qca-wifi-fw-10.4-stage:
	install -d $(STAGEDIR)/usr/src/qca-wifi
	cp -rf $(PLATFORM_ROUTER_SRCBASE)/qca-wifi-fw-10.4/src/* $(STAGEDIR)/usr/src/qca-wifi

qca-wifi-fw-10.4:
ifneq ($(wildcard qca-wifi-fw-10.4/src/include/dbglog.h),)
	$(MAKE) qca-wifi-fw-10.4-stage
endif

qca-wifi-fw-10.4-clean:

qca-wifi-fw-10.4-install:

