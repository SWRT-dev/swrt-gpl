
all:
ifneq ($(wildcard $(SDK_DIR_PLATFORM)/8021xd/Makefile),)
	[ -d 8021xd ] || ln -sf $(SDK_DIR_PLATFORM)/8021xd 8021xd
	$(MAKE) -C 8021xd all
else
	mkdir -p 8021xd
	cp prebuilt/$(BUILD_NAME)/8021xd 8021xd/8021xd
endif

install:
	install -D 8021xd/8021xd $(INSTALLDIR)/usr/sbin/8021xd
	$(STRIP) $(INSTALLDIR)/usr/sbin/8021xd

clean:

