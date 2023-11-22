CC ?= gcc
CFLAGS += -I. -D_GNU_SOURCE
CFLAGS += -ggdb3 -Wall -O0
CFLAGS += $(TARGET_CFLAGS) -Wall -I./
CFLAGS += -I$(STAGEDIR)/usr/include -I$(STAGEDIR)/usr/include/libnl3 -I$(STAGEDIR)/usr/include/openssl -I$(TOP)/shared -I$(SRCBASE)/include
LDFLAGS += -L$(STAGEDIR)/usr/lib -L$(TOP)/shared -L$(TOP)/nvram -lshared -lnvram

OBJS =  debug.o \
	json_helper.o \
	util.o \
	timer.o \
	utf8.o \
	dpputils.o \
	dpp_api.o \
	dpp_sm.o

LIBS += -lssl -lcrypto -leasy
LIBS += -lubox
DPPLIBS = $(LIBS) -ldpp

.PHONY: all clean

all: libdpp.so


EXECS = controller enrollee

test: libdpp.so $(EXECS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

libdpp.so: $(OBJS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,libdpp.so -o $@ $^ $(LIBS) $(LDFLAGS)

controller: test/controller.o
	$(CC) $(CFLAGS)  -I. -L. -o test/$@ $^ $(DPPLIBS)

enrollee: test/enrollee.o
	$(CC) $(CFLAGS)  -I. -L. -o test/$@ $^ $(DPPLIBS)

clean:
	rm -f *.o test/*.o *.so* test/controller test/enrollee

