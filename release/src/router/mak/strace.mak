.PHONY: strace libunwind

libunwind-configure:
	cd libunwind && $(CONFIGURE) \
		--libdir=/usr/lib \
		--sysconfdir=/etc \
		--disable-documentation \
		--enable-minidebuginfo=no \
		--enable-fast-install=yes \
		--disable-tests \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS) -ffunction-sections -fdata-sections -Wl,--gc-sections -L$(TOP)/openssl -DNEED_PRINTF -D_GNU_SOURCE" \
		LDFLAGS="-ffunction-sections -fdata-sections -Wl,--gc-sections -L$(TOP)/openssl"
	sed -i 's/need_relink=yes/need_relink=no/g' $(TOP)/libunwind/config/ltmain.sh
	sed -i 's/need_relink=yes/need_relink=no/g' $(TOP)/libunwind/libtool

libunwind: libunwind-configure
	$(MAKE) -C libunwind

libunwind-clean: 
	[ ! -f libunwind/Makefile ] || $(MAKE) -C libunwind clean
	@rm -f libunwind/Makefile

libunwind-install: 
	$(MAKE) -C libunwind install DESTDIR=$(INSTALLDIR)/strace
	rm -rf $(INSTALLDIR)/strace/usr/man
	rm -rf $(INSTALLDIR)/strace/usr/include
	rm -f $(INSTALLDIR)/strace/usr/lib/*.a
	rm -f $(INSTALLDIR)/strace/usr/lib/*.la
	rm -rf $(INSTALLDIR)/strace/usr/lib/pkgconfig
	rm -rf $(INSTALLDIR)/strace/usr/local

ifeq ($(or $(RTCONFIG_LANTIQ),$(RTCONFIG_RALINK_MT7621)),y)
LIBUNWIND_LIB = -lunwind-mips
else ifeq ($(or $(RTCONFIG_SOC_IPQ40XX),$(HND_ROUTER_AX_675X)),y)
LIBUNWIND_LIB = -lunwind-arm
else ifeq ($(or $(RTCONFIG_QCA),$(RTCONFIG_MT798X)),y)
LIBUNWIND_LIB = -lunwind-aarch64
else ifeq ($(BRCM_CHIP),$(or 4908,4912))
LIBUNWIND_LIB = -lunwind-aarch64
else
LIBUNWIND_LIB = -lunwind-arm
endif

strace: libunwind strace-configure
	$(MAKE) -C $@

strace-configure:
	cd strace && $(CONFIGURE) \
		--disable-gcc-Werror \
		--libdir=/usr/lib \
		--sysconfdir=/etc \
		--enable-mpers=no \
		--with-libunwind \
		CC="$(CC)" \
		CFLAGS="$(CFLAGS) -fcommon -ffunction-sections -fdata-sections -Wl,--gc-sections -L$(TOP)/openssl -DNEED_PRINTF -I$(TOP)/libunwind/include" \
		LDFLAGS="-ffunction-sections -fdata-sections -Wl,--gc-sections -L$(TOP)/openssl -L$(TOP)/libunwind/src/.libs $(LIBUNWIND_LIB)"


strace-install: libunwind-install
	@install -D strace/strace $(INSTALLDIR)/strace/sbin/strace
	@$(STRIP) $(INSTALLDIR)/strace/sbin/strace

strace-clean:
	[ ! -f strace/Makefile ] || $(MAKE) -C strace clean
	@rm -f strace/Makefile
