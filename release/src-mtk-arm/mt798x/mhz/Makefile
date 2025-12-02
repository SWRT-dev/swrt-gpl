# Convert swrt build environment variable to sdk
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all: mhz
	touch stamp-h1

mhz: %: %.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -Wall -fomit-frame-pointer $(INCLUDE) -o $@ -c $<

install:
	install -D mhz $(INSTALLDIR)/sbin/mhz
	$(STRIP) $(INSTALLDIR)/sbin/mhz

clean:
	rm -f mhz *.o *~
