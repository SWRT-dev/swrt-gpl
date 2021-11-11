# Convert asuswrt build environment variable to libnl-tiny
PKG_BUILD_DIR=.

WFLAGS=-Wall
CFLAGS+=-O2 -fPIC
INCLUDES=-Iinclude

LIBNAME=libnl-tiny.so

all: $(LIBNAME)

%.o: %.c
	$(CC) $(WFLAGS) -c -o $@ $(INCLUDES) $(CFLAGS) $<

LIBNL_OBJ=nl.o handlers.o msg.o attr.o cache.o cache_mngt.o object.o socket.o error.o
GENL_OBJ=genl.o genl_family.o genl_ctrl.o genl_mngt.o unl.o

$(LIBNAME): $(LIBNL_OBJ) $(GENL_OBJ)
	$(CC) -shared -o $@ $^

clean:
	rm -f $(LIBNL_OBJ) $(GENL_OBJ) $(LIBNAME) stage

stage:
	install -d $(STAGEDIR)/usr/lib/pkgconfig $(STAGEDIR)/usr/include/libnl-tiny
	cp -rf $(PKG_BUILD_DIR)/include/* $(STAGEDIR)/usr/include/libnl-tiny
	cp -rf $(PKG_BUILD_DIR)/libnl-tiny.so $(STAGEDIR)/usr/lib/
	cp -rf files/libnl-tiny.pc $(STAGEDIR)/usr/lib/pkgconfig
	@touch $@
