include ../common.mak
include $(SRCBASE)/.config
include ../.config
CFLAGS += -Os -Wall $(EXTRACFLAGS) -fPIC
#CFLAGS += $(PRIVATE_EXTRACFLAGS)
CFLAGS += -I. -I$(SRCBASE)/include
CFLAGS += -DLINUX26 -DCONFIG_BCMWL5
LDFLAGS += -L$(TOP)/nvram_arm -lnvram
OBJS = libutils.o

all: libutils.so

install:
	install -d $(INSTALLDIR)/usr/lib
	@install -m 755 libutils.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libutils.so

clean:
	rm -f *.a
	rm -f *.o


%.o : %.c
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

libutils.so: $(OBJS)
	@echo "[LD] $@"
	@$(LD) $(LDFLAGS) -shared -o $@ $^

