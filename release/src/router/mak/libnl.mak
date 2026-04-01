ifneq ($(HND_ROUTER_AX),y)
libnl/configure:
	cd libnl && ./autogen.sh

libnl/Makefile: libnl/configure
	cd libnl && $(CONFIGURE) \
		--prefix=/usr --bindir=/usr/sbin --libdir=/usr/lib

libnl: libdaemon libnl/Makefile
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -j 8 -C $@ && $(MAKE) $@-stage && touch $@/stamp-h1 && \
			(	cd $(STAGEDIR)/usr/lib && \
				for f in libnl*-3.la ; do \
					sed 's,/usr/lib,$(STAGEDIR)/usr/lib,g' -i $$f ; \
				done ; \
			) ; \
	fi

libnl-install: libnl
	( cd $(STAGEDIR) && \
		install -d $(INSTALLDIR)/libnl/usr/lib && \
		rsync -avcH $(STAGEDIR)/usr/lib/libnl*-3.so* $(INSTALLDIR)/libnl/usr/lib && \
		$(STRIP) $(INSTALLDIR)/libnl/usr/lib/*.so \
	)

libnl-clean:
	[ ! -f libnl/Makefile ] || $(MAKE) -C libnl distclean
	@rm -f libnl/Makefile
endif

