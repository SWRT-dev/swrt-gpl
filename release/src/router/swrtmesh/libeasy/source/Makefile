CC ?= gcc
#CFLAGS += -I/usr/include/libnl3
CFLAGS += $(TARGET_CFLAGS)
CFLAGS += -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3 -I$(STAGEDIR)/usr/include/openssl
LDFLAGS += -L$(STAGEDIR)/usr/lib

#CFLAGS += -O2 -Wall -g -fvisibility=hidden

LIBEASY_CFLAGS = -fvisibility=hidden -Werror

LIBS += -lnl-3 -lnl-route-3 -lnl-genl-3
VFLAGS=$(shell ./genversion.sh)

objs_lib = event.o base64.o utils.o if_utils.o debug.o

ver=$(shell cat ./VERSION)
maj=$(shell cat ./VERSION | cut -f1 -d.)

.PHONY: all tests clean install

all: libeasy.so.$(ver)

%.o: %.c
	$(CC) $(CFLAGS) $(LIBEASY_CFLAGS) $(VFLAGS) -fPIC -c -o $@ $<

libeasy.so.$(ver): $(objs_lib)
	$(CC) $(CFLAGS) $(LIBEASY_CFLAGS) $(EXTRA_CFLAGS) $(VFLAGS) $(LDFLAGS) \
		-shared -Wl,-soname,libeasy.so.$(maj) -o $@ $^ $(LIBS)
	-ln -sf $@ libeasy.so.$(maj)
	-ln -sf $@ libeasy.so

tests:
	$(MAKE) -C tests all

#INCDIR=/usr/include/easy
INCDIR=$(STAGEDIR)/usr/include/easy

-include Makefile.inc

install: install-headers install-libs


clean:
	rm -f *.o *so*
