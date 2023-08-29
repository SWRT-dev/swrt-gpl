# Convert swrt build environment variable to qsdk
PKG_BUILD_DIR=$(shell pwd)/source
STAGING_DIR=$(STAGEDIR)

all:
	cd $(PKG_BUILD_DIR) && $(CONFIGURE) --enable-shared --enable-static --disable-roxml
	$(MAKE) -C $(PKG_BUILD_DIR)
	cp -rf $(PKG_BUILD_DIR)/src/roxml.h $(STAGEDIR)/usr/include
	cp -rf $(PKG_BUILD_DIR)/.libs/libroxml.so* $(STAGEDIR)/usr/lib

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/.libs/libroxml.so.0.0.0 $(INSTALLDIR)/usr/lib
	cd $(INSTALLDIR)/usr/lib && ln -sf libroxml.so.0.0.0 libroxml.so.0 && ln -sf libroxml.so.0.0.0 libroxml.so
	@$(STRIP) $(INSTALLDIR)/usr/lib/libroxml.so.0.0.0

clean:

stage:
	cp -rf $(PKG_BUILD_DIR)/src/roxml.h $(STAGEDIR)/usr/include
	cp -rf $(PKG_BUILD_DIR)/.libs/libroxml.so* $(STAGEDIR)/usr/lib

