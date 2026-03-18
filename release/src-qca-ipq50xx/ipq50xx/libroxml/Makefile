# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
	cd $(PKG_BUILD_DIR) && $(CONFIGURE) --enable-shared --enable-static --disable-roxml
	$(TARGET_MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	$(CP) $(PKG_BUILD_DIR)/src/roxml.h $(STAGEDIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/.libs/libroxml.so* $(STAGEDIR)/usr/lib
	touch stamp-h1

install:
	@[ -d $(INSTALLDIR)/usr/lib ] || install -d $(INSTALLDIR)/usr/lib
	@install -m 755 -D $(PKG_BUILD_DIR)/.libs/libroxml.so.0.0.0 $(INSTALLDIR)/usr/lib
	cd $(INSTALLDIR)/usr/lib && ln -sf libroxml.so.0.0.0 libroxml.so.0 && ln -sf libroxml.so.0.0.0 libroxml.so
	@$(STRIP) $(INSTALLDIR)/usr/lib/libroxml.so.0.0.0

clean:

stage:
	$(CP) $(PKG_BUILD_DIR)/src/roxml.h $(STAGEDIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/.libs/libroxml.so* $(STAGEDIR)/usr/lib

