
all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/bndstrg_plus/Makefile),)
	ln -sf $(SDK_DIR_PLATFORM)/bndstrg_plus bndstrg_plus
	$(MAKE) -C bndstrg_plus all
else
	mkdir -p bndstrg_plus
	cp prebuilt/$(BUILD_NAME)/bndstrg2 bndstrg_plus/bndstrg2
endif

install:
	install -D bndstrg_plus/bndstrg2 $(INSTALLDIR)/usr/sbin/bndstrg2
	$(STRIP) $(INSTALLDIR)/usr/sbin/bndstrg2

clean:

