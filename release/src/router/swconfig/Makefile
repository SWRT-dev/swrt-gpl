include ../common.mak
include $(SRCBASE)/.config

##ifndef CFLAGS
##CFLAGS = -O2 -g -I ../src
##endif
LIBS=-lnl -lnl-genl
CFLAGS += -D_GNU_SOURCE -O2 -g -I$(TOP)/libnl-tiny-0.1/include -I$(TOP)/libuci
LDFLAGS += -L$(TOP)/libnl-tiny-0.1 -lnl-tiny
LDFLAGS += -L$(TOP)/libuci -luci
LDFLAGS += -L$(TOP)/libubox -lubox
ifeq ($(RTCONFIG_RALINK_MT7622),y)
LDFLAGS += -ldl
endif

all: swconfig

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $^

libsw.a: swlib.o
	$(AR) rcu $@ swlib.o
	$(RANLIB) $@

swconfig: libsw.a cli.o uci.o
##	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -L./ -lsw
	$(CC) -o $@ $^ $(LDFLAGS) -L./ -lsw

clean:
	rm -f *.o *.a *.so swconfig

install:
	install -D swconfig $(INSTALLDIR)/sbin/swconfig
	$(STRIP) $(INSTALLDIR)/sbin/swconfig
	chmod 0500 $(INSTALLDIR)/sbin/swconfig
