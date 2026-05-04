.PHONY: ltrace
ltrace: libelf ltrace/Makefile 
	$(MAKE) -C $@

ltrace/Makefile: ltrace/configure
	$(MAKE) ltrace-configure

ltrace/configure:
	( cd ltrace && ./autogen.sh )

ltrace-configure:
	( cd ltrace && $(CONFIGURE) --bindir=/sbin --with-libelf=$(STAGEDIR)/usr --with-sysroot=$(TOOLS)/include ac_cv_lib_elf_elf_begin=yes \
		CFLAGS="-I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/local/include" \
		LDFLAGS="-L$(STAGEDIR)/lib -L$(STAGEDIR)/usr/lib" )

ltrace-install:
	@install -D ltrace/ltrace $(INSTALLDIR)/ltrace/sbin/ltrace
	@$(STRIP) $(INSTALLDIR)/ltrace/sbin/ltrace

ltrace-clean:
	[ ! -f lstrace/Makefile ] || $(MAKE) -C lstrace distclean
	@rm -f lstrace/Makefile

libelf:libzstd
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ && $(MAKE) $@-stage && touch $@/stamp-h1; \
	fi

libelf-stage:
	$(MAKE) -C libelf install DESTDIR=$(STAGEDIR) PREFIX=/usr

libelf-install:
	@install -D libelf/libelf.so $(INSTALLDIR)/libelf/usr/lib/libelf.so.1.0.195
	cd $(INSTALLDIR)/libelf/usr/lib/ && \
	ln -sf libelf.so.1.0.195 libelf.so.1 && \
	ln -sf libelf.so.1.0.195 libelf.so
	@$(STRIP) $(INSTALLDIR)/libelf/usr/lib/libelf.so.1.0.195

libzstd:
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) $(PARALLEL_BUILD) -C $@ lib DESTDIR=$(STAGEDIR) prefix=/usr LIBDIR=/usr/lib && $(MAKE) $@-stage && touch $@/stamp-h1; \
	fi

libzstd-stage:
	$(MAKE) -C libzstd/lib install DESTDIR=$(STAGEDIR) prefix=/usr LIBDIR=/usr/lib

libzstd-install:
	@install -D libzstd/lib/libzstd.so.1.5.6 $(INSTALLDIR)/libzstd/usr/lib/libzstd.so.1.5.6
	cd $(INSTALLDIR)/libzstd/usr/lib/ && \
	ln -sf libzstd.so.1.5.6 libzstd.so.1 && \
	ln -sf libzstd.so.1.5.6 libzstd.so
	@$(STRIP) $(INSTALLDIR)/libzstd/usr/lib/libzstd.so.1.5.6

